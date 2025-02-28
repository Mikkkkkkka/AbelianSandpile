//
// Created by NeSai on 27/02/2025.
//

#ifndef PRINTER_H
#define PRINTER_H
#include <string>

#include "Model.h"


class Printer {

    std::string output_dir;

public:

    Printer(const std::string& output_dir);

    void print(const Model &model, const std::string& filename) const;
};



#endif //PRINTER_H
