#include "cli/CLI.hpp"

int main(int argc, char** argv) {
    nlm::CLI cli;
    
    if (!cli.parse(argc, argv)) {
        return 1;
    }
    
    if (!cli.run()) {
        return 1;
    }
    
    return 0;
}