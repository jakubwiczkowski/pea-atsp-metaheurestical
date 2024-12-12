#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include "menu/menu.h"
#include "graph/loader/regular_loader.h"
#include "algorithm/greedy/nearest_neighbour.h"
#include "algorithm/annealing/simulated_annealing.h"

int main() {
    menu main_menu(10);

    graph* current_graph = nullptr;
    int time_limit = 120;
    double a = 0.99;

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
    main_menu.add_option(3, "[TS] Wybor sasiedztwa", [] {

    });
    main_menu.add_option(4, "[TS] Uruchomienie algorytmu", []{

    });
    main_menu.add_option(5, "[SW] Ustawienie wspolczynnika zmiany temperatury", [&a]{
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
    main_menu.add_option(6, "[SW] Uruchomienie algorytmu", [&current_graph, &time_limit, &a]{
        static auto swap = [](const std::vector<vertex_t>& current) {
            static std::random_device r;
            static std::default_random_engine e1(r());
            std::uniform_int_distribution<size_t> random_vertex(0, current.size() - 1);

            std::vector<vertex_t> neighbour(current);

            size_t first_index = random_vertex(e1);
            size_t second_index = random_vertex(e1);

            vertex_t first = neighbour[first_index];
            vertex_t second = neighbour[second_index];

            // muszą być dwa różne
            while (first == second) {
                second_index = random_vertex(e1);
                second = neighbour[second_index];
            }

            std::iter_swap(neighbour.begin() + first_index, neighbour.begin() + second_index);

            return neighbour;
        };

        static auto insertion = [](const std::vector<vertex_t>& current) {
            static const size_t insertion_size = 4;

            static std::random_device r;
            static std::default_random_engine e1(r());
            std::uniform_int_distribution<size_t> random_vertex(0, current.size() - 1 - insertion_size);

            std::vector<vertex_t> neighbour(current);

            size_t index = random_vertex(e1);

            std::reverse(neighbour.begin() + index, neighbour.begin() + index + insertion_size);

            return neighbour;
        };

        simulated_annealing sa([&a](double temp) {
            return a * temp;
        }, insertion);

        if (current_graph == nullptr) return;

        solution sol = sa.solve(*current_graph, time_limit);
        print_solution(sol);
    });
    main_menu.add_option(7, "Zapis sciezki rozwiazania do pliku txt", []{

    });
    main_menu.add_option(8, "Wczytanie sciezki rozwiazania z pliku txt i obliczenie kosztu", []{

    });
    main_menu.add_option(9, "Wyjscie", [&main_menu]{
        main_menu.close();
    });

    main_menu.open();
    delete current_graph;

    return 0;
}
