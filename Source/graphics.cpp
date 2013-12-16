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

#include "attributedstr.h"
#include "font.h"

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
    
#pragma mark - Public Interface
    
    void Graphics::AddTo(StackFrame *frame)
    {
        Color::AddTo(frame);
        Context::AddTo(frame);
        Layer::AddTo(frame);
        Path::AddTo(frame);
        
        Font::AddTo(frame);
        AttributedString::AddTo(frame);
        
        Gradient::AddTo(frame);
    }
    
#pragma mark -
    
    void Graphics::AttachTo(Interpreter *interpreter)
    {
        Graphics::AddTo(interpreter->currentFrame());
        interpreter->prependWordHandler([interpreter](StackFrame *currentFrame, Word *word) {
            if(word->string()->hasPrefix(str("#"))) {
                auto color = make<Color>(word->string());
                currentFrame->push(color);
                return true;
            } else {
                return false;
            }
        });
    }
}

#endif /* GFX_Include_GraphicsStack */
