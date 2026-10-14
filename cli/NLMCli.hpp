#pragma once

#include <string>
#include <vector>
#include <memory>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

using namespace nlm;

class NLMCli {
public:
    NLMCli();
    void run();
    void stop();
    
private:
    void printWelcome();
    void printPrompt();
    void handleCommand(const std::string& command);
    std::vector<std::string> tokenize(const std::string& command);
    std::string toLower(const std::string& str);
    
    void showHelp(const std::string& topic = "");
    void showStatus();
    void initBrain(const std::string& configFile);
    void saveBrain(const std::string& filename);
    void loadBrain(const std::string& filename);
    void runStep(int steps);
    void runExperiment(int maxSteps);
    void showConfig();
    void setConfig(const std::vector<std::string>& tokens);
    void showMemories();
    void showNeuromodulation();
    void developBrain();
    
    bool running_;
    std::shared_ptr<Brain> brain_;
};
