#include "cli/NLMCLI.hpp"

int main(int argc, char** argv) {
    NLMCLI cli;
    if (cli.parseCommandLine(argc, argv)) {
        return 0;
    } else {
        return 1;
    }
}