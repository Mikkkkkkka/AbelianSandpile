//
// Created by NeSai on 27/02/2025.
//

#include "Model.h"

#include <chrono>

void Model::resize_field() {
    const auto new_field = new uint64_t*[height+2];
    for (int i = 0; i < height+2; i++) {
        new_field[i] = new uint64_t[width+2];
        for (int j = 0; j < width+2; j++)
            new_field[i][j] = 0;
    }

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            new_field[i+1][j+1] = field[i][j];

    for (int i = 0; i < height; i++)
        delete[] field[i];
    delete[] field;

    height += 2;
    width += 2;
    field = new_field;
}

void Model::insert_top_line() {
    const auto new_field = new uint64_t*[height+1];
    const auto new_top_line = new uint64_t[width];
    for (int i = 0; i < width; i++)
        new_top_line[i] = 0;

    new_field[0] = new_top_line;

    for (int i = 0; i < height; i++)
        new_field[i+1] = field[i];

    height++;
    delete[] field;
    field = new_field;
}

void Model::insert_left_line() {
    for (int i = 0; i < height; i++) {
        const auto new_ith_line = new uint64_t[width+1];
        new_ith_line[0] = 0;

        for (int j = 0; j < width; j++)
            new_ith_line[j+1] = field[i][j];

        delete[] field[i];
        field[i] = new_ith_line;
    }
    width++;
}

void Model::insert_right_line() {
    for (int i = 0; i < height; i++) {
        const auto new_ith_line = new uint64_t[width+1];
        new_ith_line[width] = 0;

        for (int j = 0; j < width; j++)
            new_ith_line[j] = field[i][j];

        delete[] field[i];
        field[i] = new_ith_line;
    }
    width++;
}

void Model::insert_bottom_line() {
    const auto new_field = new uint64_t*[height+1];
    const auto new_bottom_line = new uint64_t[width];
    for (int i = 0; i < width; i++)
        new_bottom_line[i] = 0;

    for (int i = 0; i < height; i++)
        new_field[i] = field[i];

    new_field[height] = new_bottom_line;

    height++;
    delete[] field;
    field = new_field;
}

Model::Model(const Cell* cells, const uint64_t& cell_count) {
    height = 1;
    width = 1;
    field = new uint64_t*[height];
    field[0] = new uint64_t[width];
    field[0][0] = 0;

    for (int i = 0; i < cell_count; i++) {
        const uint16_t x = cells[i].x;
        const uint16_t y = cells[i].y;
        while (x > width-1 || y > height-1) {
            if (x > width-1)
                insert_right_line();
            if (y > height-1)
                insert_bottom_line();
        }
        field[y][x] = cells[i].count;
    }
}

Model::~Model() {
    for (int i = 0; i < height; i++)
        delete[] field[i];
    delete[] field;
}

bool Model::step() {
    // 1. Поиск неустойчивой клетки
    int x = 0;
    int y = 0;
    bool found = false;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            if (field[y][x] < 4)
                continue;
            found = true;
            break;
        }
        if (found)
            break;
    }

    if (!found)
        return false;

    // 2. Добавление полос (при необходимости)
    if (y == 0) {
        insert_top_line();
        y++;
    }
    if (x == 0) {
        insert_left_line();
        x++;
    }
    if (x + 1 == width)
        insert_right_line();
    if (y + 1 == height)
        insert_bottom_line();

    // 3. Изменение количества песчинок в окружающих клетках.
    field[y][x] -= 4;
    field[y-1][x] += 1;
    field[y+1][x] += 1;
    field[y][x-1] += 1;
    field[y][x+1] += 1;

    return true;
}
