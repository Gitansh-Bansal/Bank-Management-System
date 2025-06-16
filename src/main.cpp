#include "../include/BankApp.h"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        BankApp* app = BankApp::getInstance("Apna UniBank");
        app->run();
        delete BankApp::getInstance("Apna UniBank");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
} 