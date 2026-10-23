#pragma once

#include <string>
#include <memory>

namespace nlm {

class CLI {
public:
    CLI();
    void run(int argc, char** argv);
    
private:
    void printBanner();
    void initializeLogger();
    void printHelp();
    void printVersion();
    void listAvailableExperiments();
    void runCommand(int argc, char** argv);
    void configCommand(int argc, char** argv);
    void experimentCommand(int argc, char** argv);
};

}
