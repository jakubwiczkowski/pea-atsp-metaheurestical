

#include <iostream>
#include <iomanip>
#include <random>
#include <algorithm>
#include "graph.h"

graph::graph(uint16_t vertices) {
    this->vertices = vertices;

    this->graph_matrix = new int*[vertices];

    for (int row = 0; row < vertices; ++row) {
        this->graph_matrix[row] = new int[vertices];
    }

}

graph::graph(const graph &to_copy) {
    this->vertices = to_copy.get_vertices();

    this->graph_matrix = new int*[this->vertices];
    for (int row = 0; row < this->vertices; ++row) {
        this->graph_matrix[row] = new int[this->vertices];

        for (int column = 0; column < this->vertices; ++column) {
            this->graph_matrix[row][column] = to_copy.get_weight(row, column);
        }
    }
}

graph::~graph() {
    for (vertex_t row = 0; row < get_vertices(); ++row) {
        delete[] this->graph_matrix[row];
    }
    delete[] this->graph_matrix;
}

void graph::set_edge(vertex_t u, vertex_t v, int weight) {
    this->graph_matrix[u][v] = weight;
}

vertex_t graph::get_vertices() const {
    return this->vertices;
}

std::vector<vertex_t> graph::get_vertices_list() const {
    std::vector<vertex_t> vertices(this->get_vertices());

    for (vertex_t i = 0; i < this->get_vertices(); i++) {
        vertices[i] = i;
    }

    return vertices;
}

int graph::get_weight(vertex_t u, vertex_t v) const {
    return this->graph_matrix[u][v];
}

void graph::display() {
    std::cout << "           ";
    for (int col = 0; col < this->get_vertices(); ++col) {
        std::cout << std::setw(10) << col << " ";
    }
    std::cout << std::endl;
    for (int i = 0; i < this->get_vertices(); i++) {
        std::cout << std::setw(10) << i << " ";
        for (int j = 0; j < this->get_vertices(); j++) {
            std::cout << std::setw(10) << this->graph_matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

graph graph::generate_random_full(uint16_t vertices) {
    static std::random_device r;
    static std::default_random_engine e1(r());
    static std::uniform_int_distribution<int> uniform_dist_weight(1, 500);

    auto generated = graph(vertices);

    for (vertex_t u = 0; u < vertices; u++) {
        for (vertex_t v = 0; v < vertices; v++) {
            if (u == v) {
                generated.set_edge(u, v, -1);
                continue;
            }

            generated.set_edge(u, v, uniform_dist_weight(e1));
        }
    }

    return generated;
}

int graph::degree() const {
    return this->vertices;
}

std::vector<std::vector<vertex_t>> graph::generate_adjacency_list() {
    std::vector<std::vector<vertex_t>> adjacency_list;

    for (vertex_t u = 0; u < this->get_vertices(); ++u) {
        std::vector<vertex_t> adjacent;

        auto comparator = [&u, this](const vertex_t &a, const vertex_t &b) -> bool {
            return this->get_weight(u, a) < this->get_weight(u, b);
        };

        for (vertex_t v = 0; v < this->get_vertices(); ++v) {
            if (this->get_weight(u, v) != -1) adjacent.emplace_back(v);
        }

        std::sort(adjacent.begin(), adjacent.end(), comparator);

        adjacency_list.emplace_back(adjacent);
    }

    return adjacency_list;
}

int graph::get_path_weight(std::vector<vertex_t> &path) const {
    int weight = 0;

    for (size_t i = 0; i < path.size() - 1; ++i) {
        weight += this->get_weight(path[i], path[i + 1]);
    }

    return weight;
}

int graph::get_non_closed_path_weight(std::vector<vertex_t>& path) const {
    int weight = 0;

    for (size_t i = 0; i < path.size() - 1; ++i) {
        weight += this->get_weight(path[i], path[i + 1]);
    }

    weight += this->get_weight(path.back(), path.front());

    return weight;
}
