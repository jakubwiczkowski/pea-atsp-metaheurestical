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
#include "algorithm/genetic/genetic.h"
#include "algorithm/tabu/tabu_search.h"

int main() {
    menu main_menu(18);

    solution last_solution;

    graph* current_graph = nullptr;
    int time_limit = 120;

    // SA
    double a = 0.99;

    // TABU
    int neighbour_index = 0;
    tabu_search::neighbour_t* picked_neighbour = &swap;
    int term_length = 30;
    int iter_without_improvement = 3;

    // GA
    unsigned int population_size = 200;
    double mutation_factor = 0.01;
    double crossover_factor = 0.8;
    const genetic::crossover_fn* picked_crossover = &genetic::pmx;
    const genetic::mutation_fn* picked_mutation = &genetic::insertion;

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
    main_menu.add_option(8, "[GA] Ustalenie wielkości populacji początkowej", [&population_size] {
        std::cout << "[>] Aktualna wielkość populacji początkowej: " << population_size << std::endl;
        std::cout << "[?] Podaj nową wartość: ";

        std::cin >> population_size;
    });
    main_menu.add_option(9, "[GA] Ustalenie współczynnika mutacji", [&mutation_factor] {
        std::cout << "[>] Aktualna wartość współczynnika mutacji: " << mutation_factor << std::endl;
        std::cout << "[?] Podaj nową wartość: ";

        std::cin >> mutation_factor;
    });
    main_menu.add_option(10, "[GA] Ustawienie współczynnika krzyżowania", [&crossover_factor] {
        std::cout << "[>] Aktualna wartość współczynnika krzyżowania: " << crossover_factor << std::endl;
        std::cout << "[?] Podaj nową wartość: ";

        std::cin >> crossover_factor;
    });
    main_menu.add_option(11, "[GA] Wybór metody krzyżowania", [&picked_crossover] {
        std::cout << "[>] Aktualna metoda krzyżowania: ";
        if (picked_crossover == &genetic::pmx) std::cout << "PMX" << std::endl;
        else if (picked_crossover == &genetic::ox) std::cout << "OX" << std::endl;
        std::cout << "[>] Dostepne metody:" << std::endl;
        std::cout << "    1. PMX" << std::endl;
        std::cout << "    2. OX" << std::endl;
        std::cout << "[?] Podaj nową metodę (1-2): ";

        int pick;
        std::cin >> pick;

        if (pick - 1 == 0) picked_crossover = &genetic::pmx;
        else if (pick - 1 == 1) picked_crossover = &genetic::ox;
    });
    main_menu.add_option(12, "[GA] Wybór metody mutacji", [&picked_mutation] {
        std::cout << "[>] Aktualna metoda mutacji: ";
        if (picked_mutation == &genetic::insertion) std::cout << "INSERTION" << std::endl;
        else if (picked_mutation == &genetic::inversion) std::cout << "INVERSION" << std::endl;
        std::cout << "[>] Dostepne metody:" << std::endl;
        std::cout << "    1. INSERTION" << std::endl;
        std::cout << "    2. INVERSION" << std::endl;
        std::cout << "[?] Podaj nową metodę (1-2): ";

        int pick;
        std::cin >> pick;

        if (pick - 1 == 0) picked_mutation = &genetic::insertion;
        else if (pick - 1 == 1) picked_mutation = &genetic::inversion;
    });
    main_menu.add_option(13, "[GA] Uruchomienie algorytmu", [&current_graph, &time_limit, &last_solution, &population_size,
        &mutation_factor, &crossover_factor, &picked_crossover, &picked_mutation] {
        if (current_graph == nullptr) {
            std::cout << "[!] Nie zostal wczytany zadny graf!" << std::endl;
            return;
        }

        genetic alg(*picked_crossover, *picked_mutation, population_size, mutation_factor, crossover_factor);

        last_solution = alg.solve(*current_graph, time_limit);
        print_solution(last_solution);
    });
    main_menu.add_option(14, "Zapis sciezki rozwiazania do pliku txt", [&last_solution]{
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
    main_menu.add_option(15, "Wczytanie sciezki rozwiazania z pliku txt i obliczenie kosztu", [&current_graph, &last_solution]{
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
    main_menu.add_option(16, "Debug", [&time_limit, &term_length, &iter_without_improvement,
        &picked_crossover, &picked_mutation, &population_size, &mutation_factor, &crossover_factor] {
        // std::ifstream ftv55_ifs("../data/ftv55.atsp");
        std::ifstream ftv170_ifs("../data/ftv170.atsp");
        // std::ifstream rgb358_ifs("../data/rbg358.atsp");

        regular_loader loader;
        // auto loaded_ftv55 = loader.load(ftv55_ifs);
        auto loaded_ftv170 = loader.load(ftv170_ifs);
        // auto loaded_rbg358 = loader.load(rgb358_ifs);

        // int best_ftv55 = 1608;
        int best_ftv170 = 2755;
        // int best_rbg358 = 1163;

        constexpr int concurrent = 10;

        std::thread runner_threads[concurrent];
        solution results[concurrent];

        for (int i = 0; i < concurrent; ++i) {
            runner_threads[i] = std::thread([i, time_limit, &loaded_ftv170, &results, &picked_crossover, &picked_mutation, &population_size, &mutation_factor, &crossover_factor] {
//                simulated_annealing sa([&a](double temp) {
//                    return a * temp;
//                }, swap_a);
                genetic ga(*picked_crossover, *picked_mutation, population_size, mutation_factor, crossover_factor);
                results[i] = ga.solve(*loaded_ftv170.second, time_limit);
            });
        }

        for (int i = 0; i < concurrent; ++i) {
            runner_threads[i].join();
        }

//        std::cout << "[>]    LIMIT " << time_limit << " A " << a << std::endl;
        solution best = results[0];
        std::cout << "[>]    LIMIT " << time_limit << std::endl;
        for (int i = 0; i < concurrent; ++i) {
            std::cout << results[i].weight << "," << std::abs(results[i].weight - best_ftv170) / static_cast<double>(best_ftv170) << "," << (static_cast<double>(results[i].found_after) / 1000.0) << std::endl;

            if (results[i].weight < best.weight) best = results[i];
        }

        std::cout << "[>]    DANE DO WYKRESU:" << std::endl;
        for (auto [time, value]: best.relative_error_values) {
            std::cout << (static_cast<double>(time) / 1000.0) << "," << std::abs(value - best_ftv170) / ((double) best_ftv170) << std::endl;
        }
        std::cout << "[>]   NAJKROTSZA SCIEZKA:" << std::endl;
        print_solution(best);
    });
    main_menu.add_option(17, "Wyjscie", [&main_menu]{
        main_menu.close();
    });

    main_menu.open();
    delete current_graph;

    return 0;
}
