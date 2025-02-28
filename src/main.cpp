#include <fstream>
#include <iostream>
#include "Model.h"
#include "Printer.h"

int main(const int argc, char **argv) {

    if (argc < 2) {
        std::cout << "Error: The program requires at least one parameter!" << std::endl;
        return 1;
    }

    std::ifstream input_file;
    std::string temp;
    int cell_count = 0;

    // counting number of lines
    input_file.open(argv[1]);
    if (!input_file) {
        std::cout << "Error: Failed to open file for counting lines!" << std::endl;
        return 1;
    }
    while (std::getline(input_file, temp))
        cell_count++;
    input_file.close();

    // filling the cell array up
    input_file.open(argv[1]);
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

    // starting engine
    while (true) {
        if (!model.step())
            break;
    }

    auto printer = Printer("./out/");
    printer.print(model, "somefile");

    //
    // debug
    //
    for (int i = 0; i < model.height; i++) {
        for (int j = 0; j < model.width; j++)
            std::cout << model.field[i][j];
        std::cout << std::endl;
    }

    return 0;
}
