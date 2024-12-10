#include "menu.h"

#include <iostream>

menu::menu(index_t menu_size) {
    this->menu_size = menu_size;
    this->titles = std::make_unique<const char*[]>(menu_size);
    this->functions = std::make_unique<std::function<void()>[]>(menu_size);
}

void menu::add_option(index_t index, const char* title, std::function<void()> to_execute) const {
    this->titles[index] = title;
    this->functions[index] = std::move(to_execute);
}

void menu::print_menu() {
    for (index_t idx = 0; idx < this->menu_size; idx++) {
        std::cout << idx + 1 << ". " << this->titles[idx] << "\n";
    }
}

void menu::open() {
    this->is_open = true;

    while (this->is_open) {
        print_menu();

        index_t choice;
        std::cin >> choice;

        if (choice > this->menu_size || choice < 1)
            continue;

        this->functions[choice - 1]();
    }
}

void menu::close() {
    this->is_open = false;
}
