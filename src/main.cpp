#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

#include "Model.h"
#include "Printer.h"
#include "../lib/ArgParser.h"

int main(const int argc, char **argv) {

    std::vector<std::string> args;
    for (int i = 0; i < argc; i++)
        args.push_back(argv[i]);

    auto arg_parser = ArgumentParser::ArgParser("AbelianSandpile");
    arg_parser.AddStringArgument('i', "input", "-i  --input=<filename>  Файл с изначальными точками (required)");
    arg_parser.AddStringArgument('o', "output", "-o --output=<path>     Путь к папке, в которую будут выведены изображения").Default(".");
    arg_parser.AddIntArgument('m', "max-iter", "-m  --max-iter=<number> Максимально количество итераций").Default(-1);
    arg_parser.AddIntArgument('f', "freq", "-f  --freq=<number>     Частота вывода изображений").Default(0);
    if (!arg_parser.Parse(args)) {
        std::cout << "Loh, Pidr!" << std::endl;
        return 1;
    }

    std::string input = arg_parser.GetStringValue("input");
    std::string output = arg_parser.GetStringValue("output");
    int max_iter = arg_parser.GetIntValue("max-iter");
    int freq = arg_parser.GetIntValue("freq");

    std::ifstream input_file;
    std::string temp;
    int cell_count = 0;

    // counting number of lines
    input_file.open(input);
    if (!input_file) {
        std::cout << "Error: Failed to open file for counting lines!" << std::endl;
        return 1;
    }
    while (std::getline(input_file, temp))
        cell_count++;
    input_file.close();

    // filling the cell array up
    input_file.open(input);
    if (!input_file) {
        std::cout << "Error: Failed to open file for reading cell values!" << std::endl;
        return 1;
    }
    const auto cells = new Cell[cell_count];
    for (int i = 0; i < cell_count; i++) {
        cells[i] = Cell();
        input_file >> cells[i].x;
        input_file >> cells[i].y;
        input_file >> cells[i].count;
    }
    input_file.close();

    auto model = Model(cells, cell_count);
    auto printer = Printer(output);
    if (output != ".")
        std::filesystem::create_directory(arg_parser.GetStringValue("output"));

    // starting engine
    char* next_filename = new char[20];
    for (int i = 0; i+1 != max_iter; i++) {
        sprintf(next_filename, "iteration%d", i+1);
        if (freq != 0 && i % freq == 0)
            printer.print(model, next_filename);
        if (!model.step())
            break;
    }

    // //
    // // debug
    // //
    // std::cout << "-- Debug -- " << std::endl;
    // for (int i = 0; i < model.height; i++) {
    //     for (int j = 0; j < model.width; j++) {
    //         std::cout << model.field[i][j] << ' ';
    //     }
    //     std::cout << std::endl;
    // }

    printer.print(model, "iteration_final");

    return 0;
}
