//
//  main.c
//  gfx
//
//  Created by Kevin MacWhinnie on 11/13/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include <readline/readline.h>
#include <gfx/gfx.h>

using namespace gfx;

#pragma mark - REPL

Index find_unbalanced_pairs(UniChar open, UniChar close, const String *string)
{
	Index total = 0;
	
    for (Index i = 0; i < string->length(); i++) {
		auto token = string->at(i);
		if(token == open)
			total++;
		else if(token == close)
			total--;
	}
    
	return total;
}

void resolve_unbalanced_pairs(UniChar open, UniChar close, String *buffer)
{
    Index numberOfExpectedClosingTokens = find_unbalanced_pairs(open, close, buffer);
    while (numberOfExpectedClosingTokens > 0) {
        char *partialLine = readline("... ");
        
        for (size_t index = 0, length = strlen(partialLine); index < length; index++) {
            if(partialLine[index] == open)
                numberOfExpectedClosingTokens++;
            else if(partialLine[index] == close)
                numberOfExpectedClosingTokens--;
        }
        
        buffer->append(make<String>(partialLine));
        free(partialLine);
    }
}

void show_help()
{
    std::cout
        << "gfx is a forth-like language built for creating simple images using commands."
        << std::endl << std::endl
        << "Information about gfx can be found on <https://github.com/decarbonization/gfx>"
    << std::endl;
}

void run_repl(Interpreter *interpreter, gfx::Size canvasSize)
{
    PaperTape::WriteLine(String::Builder()
                         << (str("gfx " GFX_Version " ready. canvas is "))
                         << (long)canvasSize.width << "x" << (long)canvasSize.height);
    
    rl_initialize();
    
    auto frame = make<StackFrame>(interpreter->rootFrame(), interpreter);
    String *buffer = make<String>();
    for (;;) {
        AutoreleasePool pool;
        
        char *partialLine = readline("gfx> ");
        if(!partialLine || strstr(partialLine, "quit") != NULL || strstr(partialLine, "exit") != NULL) {
            if(!partialLine)
                PaperTape::WriteLine(String::Empty);
            
            break;
        } else if(strcmp(partialLine, "help") == 0) {
            show_help();
            
            continue;
        }
        
        buffer->append(make<String>(partialLine));
        free(partialLine);
        
        resolve_unbalanced_pairs('{', '}', buffer);
        resolve_unbalanced_pairs('[', ']', buffer);
        resolve_unbalanced_pairs('(', ')', buffer);
        
        add_history(buffer->getCString());
        
        try {
            interpreter->eval(frame, Parser(buffer).parse());
            frame->iterate([](Base *value, Index index, bool *stop) {
                PaperTape::WriteLine(value->description());
            });
        } catch (Exception e) {
            String::Builder errorMessage;
            errorMessage << "!!! ";
            errorMessage << e.reason();
            PaperTape::WriteLine(errorMessage);
        }
        
        buffer->deleteRange(Range(0, buffer->length()));
    }
    
    PaperTape::WriteLine(str("goodbye"));
}

#pragma mark - Main

int main(int argc, const char * argv[])
{
    Session::init(argc, argv);
    
    AutoreleasePool pool;
    
    auto arguments = Session::shared()->parsedArguments();
    auto interpreter = make<Interpreter>();
    auto files = make<Array<const String>>();
    
    const String *canvasOutputFilePath = nullptr;
    gfx::Size canvasSize{500, 500};
    
    for (const Argument *argument : arguments) {
        switch (argument->type()) {
            case Argument::Type::Text: {
                files->append(argument->value());
                break;
            }
                
            case Argument::Type::Flag: {
                break;
            }
                
            case Argument::Type::Parameter: {
                const String *label = argument->label();
                if(label->isEqual(str("to-file"))) {
                    canvasOutputFilePath = argument->value();
                } else if(label->isEqual(str("of-size"))) {
                    Array<String> *sizeVector = SplitString(argument->value(), str("x"));
                    if(sizeVector->count() != 2) {
                        std::cerr << "*** Warning: malformed size given with \"--of-size\". Must be of format 100x100" << std::endl;
                        break;
                    }
                    
                    canvasSize.width = sizeVector->at(0)->doubleValue();
                    canvasSize.height = sizeVector->at(1)->doubleValue();
                }
                
                break;
            }
        }
    }
    
    Context::pushContext(Context::bitmapContextWith(canvasSize));
    
    for (const String *filePath : files) {
        File *file = nullptr;
        try {
            file = make<File>(filePath, File::Mode::Read);
        } catch (Exception e) {
            std::cerr << "!!! Could not read file '" << filePath->getCString() << "'." << std::endl;
        }
        
        try {
            if(file) {
                auto frame = make<StackFrame>(interpreter->rootFrame(), interpreter);
                interpreter->eval(frame, Parser(file->readString(file->length())).parse());
            }
        } catch (Exception e) {
            std::cerr << filePath << " !!! " << e.reason()->getCString() << std::endl;
        }
    }
    
    if(!Session::shared()->hasTextArguments())
        run_repl(interpreter, canvasSize);
    
    if(canvasOutputFilePath) {
        auto image = Context::currentContext()->makeImage();
        Blob *data = image->makeRepresentation(Image::RepresentationType::PNG);
        auto file = make<File>(canvasOutputFilePath, File::Mode::Write);
        file->write(data);
    }
    
    return 0;
}
