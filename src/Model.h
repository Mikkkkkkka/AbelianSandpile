//
// Created by NeSai on 27/02/2025.
//

#ifndef MODEL_H
#define MODEL_H

#include <cstdint>

struct Cell {
    uint16_t x;
    uint16_t y;
    uint64_t count;
};

class Model {

    void resize_field(); // updates field size and increments `width` and `height`
    void insert_top_line();
    void insert_left_line();
    void insert_right_line();
    void insert_bottom_line();

public:
    uint64_t width;
    uint64_t height;
    uint64_t** field;

    Model(const Cell* cells, const uint64_t& cell_count);

    ~Model();

    bool step(); // true if the sandpile was unstable
};



#endif //MODEL_H
