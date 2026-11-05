// Simple test to verify CLI compilation
#include "core/CLI.hpp"
#include <iostream>

int main() {
    std::cout << "Testing CLI implementation..." << std::endl;
    nlm::CLIParser parser("test");
    parser.addArgument("test-arg", "Test argument", "Test help", "default");
    parser.addFlag("test-flag", "Test flag");
    return 0;
}