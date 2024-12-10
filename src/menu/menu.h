#ifndef MENU_H
#define MENU_H
#include <functional>
#include <memory>

/**
 * Klasa reprezentująca menu.
 * Pozwala na dodawanie opcji i ustalanie
 * co ma się dzieć po wybraniu danej opcji.
 */

typedef unsigned long index_t;

class menu {
    index_t menu_size;
    bool is_open = false;
    std::unique_ptr<const char*[]> titles;
    std::unique_ptr<std::function<void()>[]> functions;

    void print_menu();

public:
    /**
     * Konstruktor klasy menu
     *
     * @param menu_size wielkość menu ustalona z góry
     */
    explicit menu(index_t menu_size);

    /**
     * Dodaje opcje wyboru do menu.
     *
     * @param index indeks opcji w menu
     * @param title tytuł wyświetlany obok numeru opcji
     * @param to_execute funkcja która zostanie wykonana po wybraniu tej opcji
     */
    void add_option(index_t index, const char title[], std::function<void()> to_execute) const;

    /**
     * Funkcja otwierająca menu
     */
    void open();
    /**
     * Funkcja zamykająca menu
     */
    void close();
};



#endif //MENU_H
