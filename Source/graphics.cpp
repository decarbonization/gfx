//
//  graphics.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/20/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#if GFX_Include_GraphicsStack

#include "graphics.h"
#include "interpreter.h"
#include "stackframe.h"
#include "function.h"
#include "number.h"
#include "file.h"
#include "word.h"
#include "blob.h"

#include "color.h"
#include "path.h"
#include "context.h"
#include "image.h"
#include "layer.h"

#include "gradient.h"
#include "shadow.h"

#include "attributedstr.h"
#include "font.h"

#include "type.h"

namespace gfx {
    
#pragma mark - Utility Functions
    
    Array<Base> *VectorFromSize(Size size)
    {
        return autoreleased(new Array<Base>{
            make<Number>(size.width),
            make<Number>(size.height)
        });
    }
    
    Size VectorToSize(const Array<Base> *sizeVector)
    {
        gfx_assert_param(sizeVector);
        gfx_assert(sizeVector->count() == 2, str("wrong number of numbers in vector"));
        
        return Size{
            dynamic_cast_or_throw<Number *>(sizeVector->at(0))->value(),
            dynamic_cast_or_throw<Number *>(sizeVector->at(1))->value(),
        };
    }
    
    Array<Base> *VectorFromPoint(Point point)
    {
        return autoreleased(new Array<Base>{
            make<Number>(point.x),
            make<Number>(point.y)
        });
    }
    
    Point VectorToPoint(const Array<Base> *pointVector)
    {
        gfx_assert_param(pointVector);
        gfx_assert(pointVector->count() == 2, str("wrong number of numbers in vector"));
        
        return Point{
            dynamic_cast_or_throw<Number *>(pointVector->at(0))->value(),
            dynamic_cast_or_throw<Number *>(pointVector->at(1))->value(),
        };
    }
    
    Array<Base> *VectorFromRect(Rect rect)
    {
        return autoreleased(new Array<Base>{
            make<Number>(rect.origin.x),
            make<Number>(rect.origin.y),
            make<Number>(rect.size.width),
            make<Number>(rect.size.height),
        });
    }
    
    Rect VectorToRect(const Array<Base> *rectVector)
    {
        gfx_assert_param(rectVector);
        
        if(rectVector->count() == 4) {
            return Rect{
                Point{
                    dynamic_cast_or_throw<Number *>(rectVector->at(0))->value(),
                    dynamic_cast_or_throw<Number *>(rectVector->at(1))->value(),
                },
                Size{
                    dynamic_cast_or_throw<Number *>(rectVector->at(2))->value(),
                    dynamic_cast_or_throw<Number *>(rectVector->at(3))->value(),
                }
            };
        } else if(rectVector->count() == 2) {
            return Rect{
                Point{
                    0.0,
                    0.0,
                },
                Size{
                    dynamic_cast_or_throw<Number *>(rectVector->at(0))->value(),
                    dynamic_cast_or_throw<Number *>(rectVector->at(1))->value(),
                }
            };
        } else {
            throw Exception(str("wrong number of numbers in vector for rect"), nullptr);
            return Rect{};
        }
    }
    
#pragma mark - Misc Functions
    
    static void noise(StackFrame *frame)
    {
        /* num vec -- */
        auto rect = VectorToRect(frame->popType<Array<Base>>());
        auto factor = frame->popNumber();
        
        size_t width = rect.getWidth(), height = rect.getHeight();
        size_t numberOfBits = width * height;
        
        uint32_t *bitmapData = new uint32_t[numberOfBits]();
        
        Index max = factor->value() * 255.0;
        for (Index i = 0; i < numberOfBits; i++) {
            uint8_t *pixel = (uint8_t *)&bitmapData[i];
            
            Byte intensity = arc4random() % max;
            pixel[0] = intensity;
        }
        
        //This is (obviously) not portable at all.
        cf::AutoRef<CGColorSpaceRef> colorSpace{ CGColorSpaceCreateDeviceRGB() };
        cf::AutoRef<CGContextRef> context{ CGBitmapContextCreate(bitmapData, width, height, 8, width * sizeof(uint32_t), colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedLast) };
        cf::AutoRef<CGImageRef> image{ CGBitmapContextCreateImage(context) };
        CGContextDrawImage(Context::currentContext()->get(), rect, image);
        
        delete[] bitmapData;
    }
    
#pragma mark - Public Interface
    
    void Graphics::addTo(StackFrame *frame)
    {
        Color::addTo(frame);
        Context::addTo(frame);
        Layer::addTo(frame);
        Path::addTo(frame);
        Image::addTo(frame);
        
        Font::addTo(frame);
        AttributedString::addTo(frame);
        
        Shadow::addTo(frame);
        Gradient::addTo(frame);
        
        
        //Misc Functions
        frame->createFunctionBinding(str("noise"), &noise);
    }
    
#pragma mark -
    
    void Graphics::AttachTo(Interpreter *interpreter)
    {
        Graphics::addTo(interpreter->rootFrame());
        
        interpreter->prependWordHandler([interpreter](StackFrame *currentFrame, Word *word) {
            if(word->string()->hasPrefix(str("#"))) {
                auto color = make<Color>(word->string());
                currentFrame->push(color);
                return true;
            } else {
                return false;
            }
        });
        
        auto typeMap = interpreter->typeResolutionMap();
        auto baseType = Type::BaseType();
        
        typeMap->registerType(typeid(Color), make<Type>(baseType, str("<color>")));
        typeMap->registerType(typeid(Context), make<Type>(baseType, str("<context>")));
        typeMap->registerType(typeid(Layer), make<Type>(baseType, str("<layer>")));
        typeMap->registerType(typeid(Path), make<Type>(baseType, str("<path>")));
        typeMap->registerType(typeid(Image), make<Type>(baseType, str("<image>")));
        typeMap->registerType(typeid(Font), make<Type>(baseType, str("<font>")));
        typeMap->registerType(typeid(AttributedString), make<Type>(baseType, str("<text>")));
        typeMap->registerType(typeid(Shadow), make<Type>(baseType, str("<shadow>")));
        typeMap->registerType(typeid(Gradient), make<Type>(baseType, str("<gradient>")));
    }
}

#endif /* GFX_Include_GraphicsStack */
