#include "cli/CLICheckpointManager.hpp"

int main(int argc, char** argv) {
    CLICheckpointManager checkpointManager;
    if (checkpointManager.parseCommandLine(argc, argv)) {
        return 0;
    } else {
        return 1;
    }
}