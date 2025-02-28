//
// Created by NeSai on 27/02/2025.
//

#include "Printer.h"

#include <fstream>
#include <iostream>
#include <ostream>

uint8_t* bigint_to_bigendian(uint32_t value) {
    auto slice = new uint8_t[4];
    for (int i = 0; i < 4; i++)
        slice[i] = value >> (8 * i) & 0xFF;
    return slice;
}

void rewrite_data(uint8_t* original, const uint8_t* rewritten, int position, int length) {
    for (int i = 0; i < length; i++)
        original[position+i] = rewritten[i];
}

uint8_t calculate_pixel_array_padding(const Model& model) {
    return 8 - (model.width % 8); // in pseudo-pixels
}

uint64_t calculate_pixel_array_length(const Model& model) {
    uint8_t padding_amount = calculate_pixel_array_padding(model);
    return (model.width + padding_amount) * model.height / 2;
}

uint8_t* calculate_pixel_array(const Model& model) {
    uint8_t pixel_array_padding = calculate_pixel_array_padding(model);
    if (model.width % 2 == 1)
        pixel_array_padding--;

    uint64_t pixel_array_length = calculate_pixel_array_length(model);

    uint8_t* pixel_array = new uint8_t[pixel_array_length];
    for (int i = 0; i < pixel_array_length; i++)
        pixel_array[i] = 0;

    int count = 0;
    for (int y = 0; y < model.height; y++) {
        for (int x = 0; x < model.width; x+=2) {
            if (model.field[y][x] > 3)
                pixel_array[count] = 4 << 4;
            else
                pixel_array[count] = model.field[y][x] << 4;

            if (x + 1 == model.width)
                break;

            if (model.field[y][x+1] > 3)
                pixel_array[count] += 4;
            else
                pixel_array[count] += model.field[y][x+1];

            count++;
        }
        // add padding somehow
        count += pixel_array_padding / 2 + 1;
    }

    return pixel_array;
}

Printer::Printer(const std::string &output_dir) {
    this->output_dir = output_dir;
}

void Printer::print(const Model& model, const std::string& filename) const {
    uint8_t file_header[] = {
        'B', 'M',               // [ BM ] Отметка формата BM
        0x00, 0x00, 0x00, 0x00, // *  0 * Размер файла (пока что 0)
        0x00, 0x00,             // [    ] Резерв1 (должен быть 0)
        0x00, 0x00,             // [    ] Резерв2 (должен быть 0)
        0x4a, 0x00, 0x00, 0x00  // [ 74 ] Индекс первого пикселя (делаем через 4ю версию + таблица цветов)
    }; // 14
    uint8_t bitmap_info_header[] {
        0x28, 0x00, 0x00, 0x00, // [ 40 ] Размер info структуры (здесь 4-я версия)
        0x00, 0x00, 0x00, 0x00, // *  0 * Ширина в пикселях
        0x00, 0x00, 0x00, 0x00, // *  0 * Высота в пикселях
        0x01, 0x00,             // [  1 ] Количество слоёв? (у нас не изменяется 1)
        0x04, 0x00,             // [  4 ] Количество битов на пиксель (4 потому что ограничение тз)
        0x00, 0x00, 0x00, 0x00, // [  0 ] Вид компрессии (у нас нет по этому 0)
        0x00, 0x00, 0x00, 0x00, // [  0 ] Raw размер массива пикселей (для компрессии, нам не надо)
        0x00, 0x00, 0x00, 0x00, // [  0 ] Высота пиксель/метр (опционально)
        0x00, 0x00, 0x00, 0x00, // [  0 ] Ширина пиксель/метр (опционально)
        0x05, 0x00, 0x00, 0x00, // [  5 ] Количество цветов в таблице
        0x00, 0x00, 0x00, 0x00, // [  0 ] Количество важных цветов в таблице (0 потому что все важны)
    }; // 40
    uint8_t color_palette[] {
        0xFF, 0xFF, 0xFF, 0x00, // rgb(255, 255, 255)   0 песчинок  Белый цвет
        0x00, 0xFF, 0x00, 0x00, // rgb(0, 255, 0)       1 песчинок  Зелёный цвет
        0xFF, 0x00, 0xFF, 0x00, // rgb(255, 0, 255)     2 песчинок  Фиолетовый цвет
        0x00, 0xFF, 0xFF, 0x00, // rgb(255, 255, 0)     3 песчинок  Жёлтый цвет
        0x00, 0x00, 0x00, 0x00, // rgb(0, 0, 0)         >3 песчинок Чёрный цвет
    }; // 20
    const uint64_t pixel_array_length = calculate_pixel_array_length(model);
    const uint8_t* pixel_array = calculate_pixel_array(model);

    const uint8_t* bigendian_filesize = bigint_to_bigendian(74 + pixel_array_length);
    const uint8_t* bigendian_width = bigint_to_bigendian(model.width);
    const uint8_t* bigendian_height = bigint_to_bigendian(model.height);
    rewrite_data(
        file_header,
        bigendian_filesize,
        2,
        4);
    rewrite_data(
        bitmap_info_header,
        bigendian_width,
        4,
        4);
    rewrite_data(
        bitmap_info_header,
        bigendian_height,
        8,
        4);
    delete[] bigendian_filesize;
    delete[] bigendian_width;
    delete[] bigendian_height;

    std::ofstream output_file;
    output_file.open(output_dir + '/' + filename + ".bmp", std::ios::binary);
    if (!output_file) {
        std::cout << "Error: cannot open file for output!" << std::endl;
        exit(1);
    }

    for (int i = 0; i < sizeof(file_header); i++)
        output_file << file_header[i];

    for (int i = 0; i < sizeof(bitmap_info_header); i++)
        output_file << bitmap_info_header[i];

    for (int i = 0; i < sizeof(color_palette); i++)
        output_file << color_palette[i];

    for (int i = 0; i < pixel_array_length; i++)
        output_file << pixel_array[i];

    delete[] pixel_array;
    output_file.close();
}
