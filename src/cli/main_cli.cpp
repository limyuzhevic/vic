// NLM Master CLI Main Entry Point
// Entry point for the unified NLM command-line interface

#include "cli/NLMCLI.hpp"

int main(int argc, char* argv[]) {
    NLMCommandLineInterface cli;
    return cli.execute(argc, argv);
}
