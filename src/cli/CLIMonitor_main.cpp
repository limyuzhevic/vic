#include "cli/CLIMonitor.hpp"

int main(int argc, char** argv) {
    CLIMonitor monitor;
    if (monitor.parseCommandLine(argc, argv)) {
        return 0;
    } else {
        return 1;
    }
}