#ifndef PEA_ATSP_GRAPH_H
#define PEA_ATSP_GRAPH_H


#include <cstdint>
#include <vector>
#include <set>
#include "../type/types.h"

/**
 * Klasa reprezentująca graf w postaci
 * macierzy sąsiedztwa
 */
class graph {
private:
    /**
     * Ilość wierzchołków w grafie
     */
    uint16_t vertices;

    /**
     * Macierz sąsiedztwa
     */
    int** graph_matrix;
public:
    explicit graph(uint16_t vertices);
    graph(const graph& to_copy);

    ~graph();

    /**
     * Funkcja odpowiedzialna za ustawienie podanej wagi
     * na krawędzi (u, v)
     *
     * @param u początek krawędzi
     * @param v koniec krawędzi
     * @param weight waga krawędzi
     */
    void set_edge(vertex_t u, vertex_t v, int weight);

    /**
     * Zwraca ilość wierzchołków w grafie
     *
     * @return ilość wierzchołków w grafie
     */
    [[nodiscard]] uint16_t get_vertices() const;
    /**
     * Zwraca wagę danej krawędzi
     *
     * @param u początek krawędzi
     * @param v koniec krawędzi
     * @return waga krawędzi
     */
    [[nodiscard]] int get_weight(vertex_t u, vertex_t v) const;

    std::vector<std::vector<vertex_t>> generate_adjacency_list();

    int get_path_weight(std::vector<vertex_t>& path);

    /**
     * Zwraca rząd macierzy
     *
     * @return rząd macierzy
     */
    [[nodiscard]] int degree() const;

    /**
     * Wyświetla graf w postaci macierzowej
     */
    void display();

    /**
     * Generuje losowy graf pełny o
     * danej liczbie wierzchołków
     *
     * @param vertices ilość wierzchołków w grafie
     * @return graf pełny
     */
    static graph generate_random_full(uint16_t vertices);
};


#endif //PEA_ATSP_GRAPH_H
