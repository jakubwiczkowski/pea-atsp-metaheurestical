#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <map>
#include <thread>
#include "menu/menu.h"
#include "graph/loader/regular_loader.h"
#include "algorithm/greedy/nearest_neighbour.h"
#include "algorithm/annealing/simulated_annealing.h"
#include "algorithm/tabu/tabu_search.h"

int main() {
    menu main_menu(12);

    tabu_search::neighbour_t swap = [](std::vector<vertex_t>& current,
                   graph& graph,
                   tabu_search::eval_func meval,
                   std::map<std::vector<vertex_t>, int>& tabu_map,
                   int aspire_to) -> std::vector<vertex_t> {
        std::vector<vertex_t> without_cycle(current);
        without_cycle.pop_back();

        std::vector<vertex_t> best_neighbour(current);
        int best_cost = std::numeric_limits<int>::min();

        for (long i = 0; i < without_cycle.size(); ++i) {
            for (long j = 0; j < without_cycle.size(); ++j) {
                if (i == j) continue;

                std::vector<vertex_t> iteration(without_cycle);
                std::iter_swap(iteration.begin() + i, iteration.begin() + j);
                iteration.emplace_back(iteration[0]);

                if (graph.get_path_weight(iteration) < aspire_to) {
                    best_neighbour = iteration;
                    best_cost = meval(current, iteration, graph);
                } else {
                    int eval = meval(current, iteration, graph);

                    if (tabu_map.find(iteration) != tabu_map.end()) continue;
                    if (eval < best_cost) continue;

                    best_neighbour = iteration;
                    best_cost = eval;
                }
            }
        }

        return best_neighbour;
    };

    tabu_search::neighbour_t inversion = [](std::vector<vertex_t>& current,
                        graph& graph,
                        tabu_search::eval_func meval,
                        std::map<std::vector<vertex_t>, int>& tabu_map,
                        int aspire_to) -> std::vector<vertex_t> {
        static const size_t insertion_size = 4;

        std::vector<vertex_t> without_cycle(current);
        without_cycle.pop_back();

        std::vector<vertex_t> best_neighbour;
        int best_cost = std::numeric_limits<int>::min();

        for (int i = 0; i < without_cycle.size() - insertion_size; ++i) {
            std::vector<vertex_t> iteration(without_cycle);
            std::iter_swap(iteration.begin() + i, iteration.begin() + i + 3);
            std::iter_swap(iteration.begin() + i + 1, iteration.begin() + i + 2);
            iteration.emplace_back(iteration[0]);

            if (graph.get_path_weight(iteration) < aspire_to) {
                best_neighbour = iteration;
                best_cost = meval(current, iteration, graph);
            } else {
                int eval = meval(current, iteration, graph);

                if (tabu_map.find(iteration) != tabu_map.end()) continue;
                if (eval < best_cost) continue;

                best_neighbour = iteration;
                best_cost = eval;
            }
        }

        return best_neighbour;
    };

    tabu_search::neighbour_t insertion = [](std::vector<vertex_t>& current,
                        graph& graph,
                        tabu_search::eval_func meval,
                        std::map<std::vector<vertex_t>, int>& tabu_map,
                        int aspire_to) -> std::vector<vertex_t> {
        static const size_t insertion_size = 4;

        std::vector<vertex_t> without_cycle(current);
        without_cycle.pop_back();

        std::vector<vertex_t> best_neighbour;
        int best_cost = std::numeric_limits<int>::min();

        for (int i = 0; i < without_cycle.size() - insertion_size; ++i) {
            std::vector<vertex_t> iteration(without_cycle);
            std::rotate(iteration.begin() + i, iteration.begin() + i + insertion_size, iteration.begin() + i + insertion_size + 1);
            iteration.emplace_back(iteration[0]);

            if (graph.get_path_weight(iteration) < aspire_to) {
                best_neighbour = iteration;
                best_cost = meval(current, iteration, graph);
            } else {
                int eval = meval(current, iteration, graph);

                if (tabu_map.find(iteration) != tabu_map.end()) continue;
                if (eval < best_cost) continue;

                best_neighbour = iteration;
                best_cost = eval;
            }
        }

        return best_neighbour;
    };

    solution last_solution;

    graph* current_graph = nullptr;
    int time_limit = 120;
    double a = 0.99;

    int neighbour_index = 0;
    tabu_search::neighbour_t* picked_neighbour = &swap;
    int term_length = 14;
    int iter_without_improvement = 13;

    main_menu.add_option(0, "Wczytanie danych z pliku", [&current_graph]{
        std::cout << "[?] Podaj sciezke do pliku: ";

        std::string file_path;
        std::cin >> file_path;

        std::ifstream file(file_path);

        if (!file.is_open()) {
            std::cout << "[!] Ten plik nie istnieje" << std::endl;
            return;
        }

        regular_loader loader;
        auto loaded_pair = loader.load(file);

        if (loaded_pair.second == nullptr) {
            std::cout << "[!] Wczytanie pliku nie powiodlo sie!" << std::endl;
            return;
        }

        delete current_graph;
        current_graph = loaded_pair.second;

        std::cout << "[>] Graf wczytany pomyslnie!" << std::endl;

        current_graph->display();
    });
    main_menu.add_option(1, "Wprowadzenie kryterium stopu", [&time_limit]{
        std::cout << "[>] Aktualny limit czasowy: " << time_limit << "s" << std::endl;
        std::cout << "[?] Podaj nowy limit (w sekundach): ";
        std::cin >> time_limit;
    });
    main_menu.add_option(2, "Obliczenie rozwiazania metoda zachlanna", [&current_graph]{
        if (current_graph == nullptr) return;

        static nearest_neighbour nn;
        solution sol = nn.solve(*current_graph, -1);
        print_solution(sol);
    });
    main_menu.add_option(3, "[TS] Wybor sasiedztwa", [&picked_neighbour, &neighbour_index, &swap, &insertion, &inversion] {
        std::cout << "[>] Aktualny typ sasiedztwa: ";
        if (neighbour_index == 0) std::cout << "SWAP" << std::endl;
        else if (neighbour_index == 1) std::cout << "INSERTION" << std::endl;
        else if (neighbour_index == 2) std::cout << "INVERSION" << std::endl;
        std::cout << "[>] Dostepne typy:" << std::endl;
        std::cout << "    1. SWAP" << std::endl;
        std::cout << "    2. INSERTION" << std::endl;
        std::cout << "    3. INVERSION" << std::endl;
        std::cout << "[?] Podaj nowy typ (1-3): ";

        int pick;
        std::cin >> pick;

        if (pick - 1 == 0) picked_neighbour = &swap;
        else if (pick - 1 == 1) picked_neighbour = &insertion;
        else if (pick - 1== 2) picked_neighbour = &inversion;
    });
    main_menu.add_option(4, "[TS] Wybor parametrow", [&term_length, &iter_without_improvement] {
        std::cout << "[>] Aktualna dlugosc kadencji: " << term_length << std::endl;
        std::cout << "[>] Aktualna liczba maksymalna iteracji bez poprawy: " << iter_without_improvement << std::endl;
        std::cout << "    Proponowane wartosci dla ftv55 (14, 13), ftv170 (x, y), rbg358 (x, y)" << std::endl;
        std::cout << "[?] Podaj nowa dlugosc kadencji: ";
        std::cin >> term_length;
        std::cout << "[?] Podaj nowa liczbe maksymalna iteracji bez poprawy: ";
        std::cin >> iter_without_improvement;
    });
    main_menu.add_option(5, "[TS] Uruchomienie algorytmu", [&current_graph, &time_limit, &picked_neighbour, &term_length, &iter_without_improvement, &last_solution]{
        if (current_graph == nullptr) {
            std::cout << "[!] Nie zostal wczytany zadny graf!" << std::endl;
            return;
        }

        tabu_search ts(*picked_neighbour, term_length, iter_without_improvement);

        solution sol = ts.solve(*current_graph, time_limit);
        print_solution(sol);
        last_solution = sol;
    });
    main_menu.add_option(6, "[SW] Ustawienie wspolczynnika zmiany temperatury", [&a]{
        std::cout << "[>] Aktualny wspolczynnik a: " << a << std::endl;
        std::cout << "[?] Podaj nowy wspolczynnik: ";

        double temp;

        std::cin >> temp;

        if (temp <= 0 || temp >= 1) {
            std::cout << "[!] Wspolczynnik a musi sie zawierac w zakresie 0 < a < 1" << std::endl;
            return;
        }

        a = temp;
    });
    main_menu.add_option(7, "[SW] Uruchomienie algorytmu", [&current_graph, &time_limit, &a, &last_solution]{
        static auto swap = [](const std::vector<vertex_t>& current) {
            static std::random_device r;
            static std::default_random_engine e1(r());
            std::vector<vertex_t> neighbour(current);
            neighbour.pop_back();

            std::uniform_int_distribution<long> random_vertex(0, neighbour.size() - 1);

            long first_index = random_vertex(e1);
            long second_index = random_vertex(e1);

            vertex_t first = neighbour[first_index];
            vertex_t second = neighbour[second_index];

            // muszą być dwa różne
            while (first == second) {
                second_index = random_vertex(e1);
                second = neighbour[second_index];
            }

            std::iter_swap(neighbour.begin() + first_index, neighbour.begin() + second_index);
            neighbour.emplace_back(neighbour[0]);

            return neighbour;
        };

        if (current_graph == nullptr) {
            std::cout << "[!] Nie zostal wczytany zadny graf!" << std::endl;
            return;
        }

        simulated_annealing sa([&a](double temp) {
            return a * temp;
        }, swap);


        solution sol = sa.solve(*current_graph, time_limit);
        print_solution(sol);
        last_solution = sol;
    });
    main_menu.add_option(8, "Zapis sciezki rozwiazania do pliku txt", [&last_solution]{
        std::cout << "[?] Podaj nazwe pliku wyjsciowego: ";
        std::string file_path;
        std::cin >> file_path;

        std::ofstream file(file_path);

        if (!file.is_open()) {
            std::cout << "[!] Ten plik nie istnieje" << std::endl;
            return;
        }

        file << last_solution.vertices.size() << std::endl;

        for (vertex_t v: last_solution.vertices) {
            file << v << std::endl;
        }

        file.close();
    });
    main_menu.add_option(9, "Wczytanie sciezki rozwiazania z pliku txt i obliczenie kosztu", [&current_graph, &last_solution]{
        if (current_graph == nullptr) {
            std::cout << "[!] Nie zostal wczytany zadny graf!" << std::endl;
            return;
        }

        std::cout << "[?] Podaj nazwe pliku wejsciowego: ";
        std::string file_path;
        std::cin >> file_path;

        std::ifstream file(file_path);

        if (!file.is_open()) {
            std::cout << "[!] Ten plik nie istnieje" << std::endl;
            return;
        }

        int size;
        file >> size;

        std::vector<vertex_t> cycle;

        for (int i = 0; i < size; ++i) {
            int vertex;
            file >> vertex;
            cycle.emplace_back(vertex);
        }

        file.close();

        std::cout << "[>] Cykl: ";
        for (vertex_t v: cycle) {
            std::cout << v << " ";
        }
        std::cout << std::endl;

        last_solution.weight = -1;
        last_solution.vertices = cycle;

        std::cout << "[>] Koszt cyklu: " << current_graph->get_path_weight(last_solution.vertices) << std::endl;
    });
    main_menu.add_option(10, "Debug", [&current_graph, &picked_neighbour] {
        if (current_graph == nullptr) {
            std::cout << "[!] Nie zostal wczytany zadny graf!" << std::endl;
            return;
        }

        const int offset_x = 10;
        const int offset_z = 20;
        const int concurrent = 10;

        std::thread threads[concurrent][concurrent];
        int result[concurrent][concurrent];

        for (int i = offset_x; i < offset_x + concurrent; ++i) {
            for (int j = offset_z; j < offset_z + concurrent; ++j) {

                threads[i - offset_x][j - offset_z] = std::thread([&current_graph, &picked_neighbour, i, j, &result]() {
                    tabu_search ts(*picked_neighbour, i, j);
                    solution sol = ts.solve(*current_graph, 120);
                    result[i - offset_x][j - offset_z] = sol.weight;
                });
            }
        }
        for (int i = offset_x; i < offset_x + concurrent; ++i) {
            for (int j = offset_z; j < offset_z + concurrent; ++j) {
                threads[i - offset_x][j - offset_z].join();
            }
        }

        for (int i = offset_x; i < offset_x + concurrent; ++i) {
            for (int j = offset_z; j < offset_z + concurrent; ++j) {
                std::cout << i << "," << j << " = " << result[i - offset_x][j - offset_z] << std::endl;
            }
        }
    });
    main_menu.add_option(11, "Wyjscie", [&main_menu]{
        main_menu.close();
    });

    main_menu.open();
    delete current_graph;
    delete picked_neighbour;

    return 0;
}
