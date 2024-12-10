

#ifndef PEA_ATSP_OPT_REGULAR_LOADER_H
#define PEA_ATSP_OPT_REGULAR_LOADER_H


#include <memory>
#include "loader.h"

enum data_type {
    TSP, ATSP, SOP, HCP, CVRP, TOUR
};

static std::unordered_map<std::string, data_type> const data_type_mappings = {
        {"TSP", data_type::TSP},
        {"ATSP", data_type::ATSP},
        {"SOP", data_type::SOP},
        {"HCP", data_type::HCP},
        {"CVRP", data_type::CVRP},
        {"TOUR", data_type::TOUR}
};

enum edge_weight_type {
    EXPLICIT
};

static std::unordered_map<std::string, edge_weight_type> const edge_weight_type_mappings = {
        {"EXPLICIT", edge_weight_type::EXPLICIT}
};

enum edge_weight_format {
    FULL_MATRIX
};

static std::unordered_map<std::string, edge_weight_format> const edge_weight_format_mappings = {
        {"FULL_MATRIX", edge_weight_format::FULL_MATRIX}
};

struct regular_file_structure {
    std::string name;
    data_type type;
    std::string comment;
    int dimension = -1;
    edge_weight_type edge_weight;
    edge_weight_format edge_format;
    int** edge_weight_section;
};

class regular_loader : public loader<regular_file_structure> {
public:
    std::pair<regular_file_structure, graph*> load(std::ifstream& file) override;
};


#endif //PEA_ATSP_OPT_REGULAR_LOADER_H
