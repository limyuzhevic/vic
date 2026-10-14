#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <iostream>
#include <sstream>

namespace nlm {

class Brain;
class Config;
class CommandManager;

class AdvancedCLI {
public:
    AdvancedCLI();
    void run();
    
private:
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::function<void(const std::vector<std::string>&)> handler;
        std::string category;
        bool requiresBrain;
    };
    
    std::vector<Command> commands;
    std::map<std::string, std::string> aliases;
    std::shared_ptr<nlm::Brain> currentBrain;
    std::shared_ptr<nlm::Config> currentConfig;
    std::shared_ptr<CommandManager> commandManager;
    
    void setupCommands();
    void setupAliases();
    void printBanner();
    void showHelp();
    void processCommand(const std::string& input);
    std::vector<std::string> split(const std::string& input);
    void ensureBrainExists();
    void ensureConfigExists();
    
    // Brain Control Commands
    void cmdCreate(const std::vector<std::string>& args);
    void cmdLoad(const std::vector<std::string>& args);
    void cmdSave(const std::vector<std::string>& args);
    void cmdReset(const std::vector<std::string>& args);
    void cmdStep(const std::vector<std::string>& args);
    void cmdInject(const std::vector<std::string>& args);
    void cmdStatus(const std::vector<std::string>& args);
    
    // Configuration Commands
    void cmdConfig(const std::vector<std::string>& args);
    void cmdValidate(const std::vector<std::string>& args);
    void cmdValidateAll(const std::vector<std::string>& args);
    
    // Command Management Commands
    void cmdCommand(const std::vector<std::string>& args);
    void cmdProfile(const std::vector<std::string>& args);
    
    // Experiment Commands
    void cmdExperiment(const std::vector<std::string>& args);
    void cmdBenchmark(const std::vector<std::string>& args);
    
    // Utility Commands
    void cmdHelp(const std::vector<std::string>& args);
    void cmdAlias(const std::vector<std::string>& args);
    void cmdExit(const std::vector<std::string>& args);
};

} // namespace nlm
