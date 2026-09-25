// NLM Configuration Examples
// Demonstrates command line interface capabilities

#include "src/core/Config/Config.hpp"
#include <iostream>

int main(int argc, char** argv) {
    nlm::Config config;
    
    // Load from command line arguments
    config.loadFromArgs(argc, argv);
    
    // Also load from a configuration file
    if (config.has("config_file")) {
        std::string configFile = config.getOr<std::string>("config_file", "");
        if (!configFile.empty()) {
            config.loadFromFile(configFile);
        }
    }
    
    // Display configuration
    std::cout << config.summary() << std::endl;
    
    return 0;
}
