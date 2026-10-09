#include "cli/CLIConfigManager.hpp"

int main(int argc, char** argv) {
    CLIConfigManager configManager;
    if (configManager.parseCommandLine(argc, argv)) {
        return 0;
    } else {
        return 1;
    }
}