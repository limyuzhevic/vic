// NLM Main Entry Point with CLI
// Refactored from monolithic main.cpp into proper CLI system

#include "CLI.hpp"
#include <iostream>

int main(int argc, char** argv) {
    nlm::CLI cli;
    bool success = cli.run(argc, argv);
    return success ? 0 : 1;
}
