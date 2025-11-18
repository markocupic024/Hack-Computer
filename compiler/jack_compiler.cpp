#include "jack_tokenizer.h"
#include "compilation_engine.h"
#include "vm_writer.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

void processFile(const fs::path &jackPath)
{
    fs::path vmPath = jackPath;
    vmPath.replace_extension(".vm");
    
    std::ifstream jackFile(jackPath);
    if (!jackFile)
    {
        std::cerr << "Error: Cannot open file " << jackPath << "\n";
        return;
    }
    
    JackTokenizer tokenizer(jackFile);
    VMWriter vmWriter(vmPath.string());
    CompilationEngine engine(tokenizer, vmWriter);
    
    engine.compileClass();
    vmWriter.close();
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: JackCompiler <input.jack|directory>\n";
        return EXIT_FAILURE;
    }
    
    fs::path inputPath(argv[1]);
    std::vector<fs::path> jackFiles;
    
    if (fs::is_directory(inputPath))
    {
        for (const auto &entry : fs::directory_iterator(inputPath))
        {
            if (entry.path().extension() == ".jack")
            {
                jackFiles.push_back(entry.path());
            }
        }
    }
    else
    {
        if (inputPath.extension() != ".jack")
        {
            std::cerr << "Error: Input file must have a .jack extension.\n";
            return EXIT_FAILURE;
        }
        jackFiles.push_back(inputPath);
    }
    
    for (const auto &file : jackFiles)
    {
        processFile(file);
    }
    
    return EXIT_SUCCESS;
}

