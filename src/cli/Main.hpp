#pragma once

#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/brain/Brain.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/brain/NeuralRegion.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

class SimpleNLMCLI {
public:
    SimpleNLMCLI(int argc, char** argv);
    bool run();
    
private:
    // Helper functions
    void printBanner();
    void printUsage();
    void printVersion();
    bool parseArguments();
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    
    // Test functions
    bool runTests(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    bool runBasicTest(std::shared_ptr<Brain> brain);
    bool runPlasticityTest(std::shared_ptr<Brain> brain);
    bool runStdpTest(std::shared_ptr<Brain> brain);
    bool runAllTests(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    
    // Utility functions
    Neuron* getNeuronAtIndex(std::shared_ptr<Brain> brain, size_t index);
    
    int argc;
    char** argv;
    std::string mode;
    std::string configFile;
    std::unordered_map<std::string, std::string> configArgs;
};

} // namespace nlm
