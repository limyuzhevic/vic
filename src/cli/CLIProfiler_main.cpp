#include "cli/CLIProfiler.hpp"

int main(int argc, char** argv) {
    CLIProfiler profiler;
    if (profiler.parseCommandLine(argc, argv)) {
        return 0;
    } else {
        return 1;
    }
}