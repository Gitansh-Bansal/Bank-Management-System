#include "../include/BankApp.h"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        BankApp* app = BankApp::getInstance("Nirma UniBank");
        app->run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
} 