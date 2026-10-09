#include "cli/CLIDebugger.hpp"

int main(int argc, char** argv) {
    CLIDebugger debugger;
    if (debugger.parseCommandLine(argc, argv)) {
        return 0;
    } else {
        return 1;
    }
}