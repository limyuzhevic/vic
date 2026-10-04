#pragma once

#include <string>
#include <memory>
#include <iostream>

// Forward declarations
class Config;
class Logger;

namespace nlm {

class CLI {
public:
    CLI(int argc, char** argv);
    bool execute();
    void printHelp() const;
    void printVersion() const;
    
    static int run(int argc, char** argv);
    
private:
    int argc;
    char** argv;
    std::unique_ptr<Config> config;
    std::unique_ptr<Logger> logger;
    
    bool parseArguments();
    bool validateOptions();
    bool executeCommand();
    bool showHelp();
    bool showVersion();
    bool loadConfiguration();
    bool runPhase6Demo();
    bool runIntegrationTests();
    bool runPlasticityTests();
    bool runConnectivityTests();
    void printBanner() const;
    void cleanup();
};

} // namespace nlm
