#pragma once

#include "Brain.hpp"
#include "Config.hpp"
#include <string>
#include <chrono>

namespace nlm {

class AdvancedCommands {
public:
    AdvancedCommands(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    
    void printHelp();
    void processCommand(const std::string& command);
    
    void inspectRegion(int regionId);
    void searchNeurons(const std::string& pattern);
    void exportBrainState(const std::string& filename);
    void importBrainState(const std::string& filename);
    void modifyConfig(const std::string& key, const std::string& value);
    void analyzeNeuron(int neuronId);
    void runBenchmark(int iterations);
    void generateVisualization();
    
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    
    std::string neuronTypeToString(NeuronType type);
};

// Advanced commands main function
void advancedCommandsMain(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);

} // namespace nlm