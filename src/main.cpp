#include <iostream>
#include <fstream>
#include "menu/menu.h"
#include "graph/loader/regular_loader.h"
#include "algorithm/greedy/nearest_neighbour.h"

int main() {
    menu main_menu(10);

    graph* current_graph = nullptr;

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

//        auto a = current_graph->generate_adjacency_list();
//        for (size_t i = 0; i < a.size(); ++i) {
//            std::cout << i << " - ";
//            for (size_t j = 0; j < a[i].size(); ++j) {
//                std::cout << a[i][j] << " ";
//            }
//            std::cout << std::endl;
//        }
    });
    main_menu.add_option(1, "Wprowadzenie kryterium stopu", []{

    });
    main_menu.add_option(2, "Obliczenie rozwiazania metoda zachlanna", [&current_graph]{
        if (current_graph == nullptr)
            return;

        static nearest_neighbour nn;
        solution sol = nn.solve(*current_graph, -1);
        print_solution(sol);
    });
    main_menu.add_option(3, "[TS] Wybor sasiedztwa", [] {

    });
    main_menu.add_option(4, "[TS] Uruchomienie algorytmu", []{

    });
    main_menu.add_option(5, "[SW] Ustawienie wspolczynnika zmiany temperatury", []{

    });
    main_menu.add_option(6, "[SW] Uruchomienie algorytmu", []{

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
