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

    solution last_solution;

    graph* current_graph = nullptr;
    int time_limit = 120;
    double a = 0.99;

    int neighbour_index = 0;
    tabu_search::neighbour_t* picked_neighbour = &swap;
    int term_length = 30;
    int iter_without_improvement = 3;

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
    main_menu.add_option(3, "[TS] Wybor sasiedztwa", [&picked_neighbour, &neighbour_index] {
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
        else if (pick - 1 == 2) picked_neighbour = &inversion2;
    });
    main_menu.add_option(4, "[TS] Wybor parametrow", [&term_length, &iter_without_improvement] {
        std::cout << "[>] Aktualna dlugosc kadencji: " << term_length << std::endl;
        std::cout << "[>] Aktualna liczba maksymalna iteracji bez poprawy: " << iter_without_improvement << std::endl;
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
        std::cout << "[>] Aktualny wspolczynnik \"a\": " << a << std::endl;
        std::cout << "[?] Podaj nowy wspolczynnik: ";

        double temp;

        std::cin >> temp;

        if (temp <= 0 || temp >= 1) {
            std::cout << "[!] Wspolczynnik \"a\" musi sie zawierac w zakresie 0 < a < 1" << std::endl;
            return;
        }

        a = temp;
    });
    main_menu.add_option(7, "[SW] Uruchomienie algorytmu", [&current_graph, &time_limit, &a, &last_solution]{
        if (current_graph == nullptr) {
            std::cout << "[!] Nie zostal wczytany zadny graf!" << std::endl;
            return;
        }

        simulated_annealing sa([&a](double temp) {
            return a * temp;
        }, swap_a);


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
    main_menu.add_option(10, "Debug", [&time_limit, &a, &picked_neighbour, &term_length, &iter_without_improvement] {
        std::ifstream ftv55_ifs("../data/ftv55.atsp");
        std::ifstream ftv170_ifs("../data/ftv170.atsp");
        std::ifstream rgb358_ifs("../data/rbg358.atsp");

        regular_loader loader;
        auto loaded_ftv55 = loader.load(ftv55_ifs);
        auto loaded_ftv170 = loader.load(ftv170_ifs);
        auto loaded_rbg358 = loader.load(rgb358_ifs);

        int best_ftv55 = 1608;
        int best_ftv170 = 2755;
        int best_rbg358 = 1163;

        const int concurrent = 10;

        std::thread runner_threads[concurrent];
        solution results[concurrent];

        for (int i = 0; i < concurrent; ++i) {
            runner_threads[i] = std::thread([i, a, time_limit, &loaded_ftv55, &results, &picked_neighbour, &term_length, &iter_without_improvement] {
//                simulated_annealing sa([&a](double temp) {
//                    return a * temp;
//                }, swap_a);
                tabu_search ts(*picked_neighbour, term_length, iter_without_improvement);
                results[i] = ts.solve(*loaded_ftv55.second, time_limit);
            });
        }

        for (int i = 0; i < concurrent; ++i) {
            runner_threads[i].join();
        }

//        std::cout << "[>]    LIMIT " << time_limit << " A " << a << std::endl;
        solution best = results[0];
        std::cout << "[>]    LIMIT " << time_limit << " KAD = " << term_length << " ITER = " << iter_without_improvement << std::endl;
        for (int i = 0; i < concurrent; ++i) {
            std::cout << results[i].weight << "," << std::abs(results[i].weight - best_ftv55) / ((double) best_ftv55) << "," << ((double) results[i].found_after / 1000.0) << std::endl;

            if (results[i].weight < best.weight) best = results[i];
//            print_solution(results[i]);
//            std::cout << "[>] Blad wzgledny: " << std::abs(results[i].weight - best_ftv55) / ((double) best_ftv55) << std::endl;
        }

        std::cout << "[>]    DANE DO WYKRESU:" << std::endl;
        for (auto [time, value]: best.relative_error_values) {
            std::cout << ((double) time / 1000.0) << "," << std::abs(value - best_ftv55) / ((double) best_ftv55) << std::endl;
        }
        std::cout << "[>]   NAJKROTSZA SCIEZKA:" << std::endl;
        print_solution(best);

        //        if (current_graph == nullptr) {
//            std::cout << "[!] Nie zostal wczytany zadny graf!" << std::endl;
//            return;
//        }

//        const int offset_x = 70;
//        const int offset_z = 1000;
//        const int concurrent = 10;
//
//        std::thread threads[concurrent][concurrent];
//        int result[concurrent][concurrent];
//
//        for (int i = offset_x; i < offset_x + concurrent; ++i) {
//            for (int j = offset_z; j < offset_z + concurrent; ++j) {
//
//                threads[i - offset_x][j - offset_z] = std::thread([&current_graph, &picked_neighbour, i, j, &result]() {
//                    tabu_search ts(*picked_neighbour, i, j);
//                    solution sol = ts.solve(*current_graph, 120);
//                    result[i - offset_x][j - offset_z] = sol.weight;
//                });
//            }
//        }
//        for (int i = offset_x; i < offset_x + concurrent; ++i) {
//            for (int j = offset_z; j < offset_z + concurrent; ++j) {
//                threads[i - offset_x][j - offset_z].join();
//            }
//        }
//
//        for (int i = offset_x; i < offset_x + concurrent; ++i) {
//            for (int j = offset_z; j < offset_z + concurrent; ++j) {
//                std::cout << i << "," << j << " = " << result[i - offset_x][j - offset_z] << std::endl;
//            }
//        }
    });
    main_menu.add_option(11, "Wyjscie", [&main_menu]{
        main_menu.close();
    });

    main_menu.open();
    delete current_graph;

    return 0;
}
