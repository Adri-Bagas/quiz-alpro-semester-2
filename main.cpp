#include "src/ui/menu.hpp"
#include <iostream>
#include <vector>
#include <string>

constexpr const char* APP_NAME = "Sistem Manajemen Toko Buku Asoy Geboy";
constexpr const char* APP_VERSION = "0.0.1";

int main() {
    std::vector<std::string> app_options = {
        "Book Management", 
        "Transactions", 
        "Transactions History",
        "Reports", 
        "Exit"
    };

    // Block scoping ensures MainMenuUI destructor fires and endwin() is called 
    // before we try to print to standard console out.

    {
        MainMenuUI ui(app_options, APP_NAME, APP_VERSION);
        ui.run();
    }

    std::cout << "Application exited cleanly.\n";

    return 0;
}