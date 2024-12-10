#include <fstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include "regular_loader.h"

std::pair<regular_file_structure, graph*> regular_loader::load(std::ifstream &file) {
    static char split_char = ':';

    regular_file_structure rfs;
    graph* loaded_graph;

    while (!file.eof()) {
        std::string current_line;
        getline(file, current_line);

        // wczytywanie danych o pliku
        if (current_line.find(split_char) != std::string::npos) {
            size_t pos = current_line.find(split_char);

            std::string token = current_line.substr(0, pos);
            std::string value = current_line.substr(pos + 2, current_line.length());

            std::string::iterator end_pos = std::remove(value.begin(), value.end(), ' ');
            value.erase(end_pos, value.end());

            if (token == "NAME") {
                rfs.name = value;
            } else if (token == "TYPE") {
                auto mapped = data_type_mappings.find(value);

                if (mapped == data_type_mappings.end()) {
                    std::cout << "[!] TYPE " << value << " nie jest wspierany!";
                    break;
                }

                rfs.type = mapped->second;
            } else if (token == "COMMENT") {
                rfs.comment = value;
            } else if (token == "DIMENSION") {
                rfs.dimension = std::stoi(value);
            } else if (token == "EDGE_WEIGHT_TYPE") {
                auto mapped = edge_weight_type_mappings.find(value);

                if (mapped == edge_weight_type_mappings.end()) {
                    std::cout << "[!] EDGE_WEIGHT_TYPE " << value << " nie jest wspierany!";
                    break;
                }

                rfs.edge_weight = mapped->second;
            } else if (token == "EDGE_WEIGHT_FORMAT") {
                auto mapped = edge_weight_format_mappings.find(value);

                if (mapped == edge_weight_format_mappings.end()) {
                    std::cout << "[!] EDGE_WEIGHT_FORMAT " << value << " nie jest wspierany!";
                    break;
                }

                rfs.edge_format = mapped->second;
            }
        } else {
            if (current_line == "EDGE_WEIGHT_SECTION") {
                rfs.edge_weight_section = new int*[rfs.dimension];
                for (int row = 0; row < rfs.dimension; ++row) {
                    rfs.edge_weight_section[row] = new int[rfs.dimension];
                    for (int column = 0; column < rfs.dimension; ++column) {
                        file >> rfs.edge_weight_section[row][column];
                    }
                }
            }

            if (current_line == "EOF") {
                break;
            }
        }
    }

    file.close();

    if (rfs.dimension == -1) return {rfs, nullptr};

    loaded_graph = new graph(rfs.dimension);

    for (int row = 0; row < rfs.dimension; ++row) {
        for (int column = 0; column < rfs.dimension; ++column) {
            loaded_graph->set_edge(row, column, rfs.edge_weight_section[row][column]);
        }
    }

    return {rfs, loaded_graph};
}
