#pragma once

#include "../brain/Brain.hpp"
#include "AgentBrain.hpp"
#include <memory>
#include <string>

namespace nlm {

class AdvancedCLI {
public:
    AdvancedCLI(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent);
    
    void run(const std::string& command);
    void run(const std::string& command, const std::string& subcommand);
    void run(const std::string& command, const std::string& subcommand, const std::string& arg);
    
private:
    void inspectBrain();
    void manageMemory(const std::string& subcommand);
    void controlNeuromodulation(const std::string& subcommand);
    void manageCheckpoints(const std::string& subcommand);
    void analyzePerformance();
    void exploreConfiguration(const std::string& subcommand);
    void replayExperience();
    void manageDevelopment();
    
    void printHelp();
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
};

} // namespace nlm
