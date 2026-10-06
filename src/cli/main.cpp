// CLI Main Entry Point
// Provides the entry point for the new NLM command-line interface

#include "cli/CommandLineInterface.h"
#include <iostream>

int main(int argc, char** argv) {
    nlm::CommandLineInterface cli;
    return cli.run(argc, argv);
}
