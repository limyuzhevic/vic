// NLM Command Line Interface
// Provides comprehensive control over neural simulation with advanced options

#ifndef NLM_CLI_HPP
#define NLM_CLI_HPP

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <iostream>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

class CommandLineParser {
public:
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::vector<std::string> flags;
        std::function<void()> action;
        bool requiresBrain = false;
    };

    CommandLineParser();
    ~CommandLineParser();

    // Parse command line arguments
    bool parse(int argc, char** argv);

    // Get parsed options
    const std::map<std::string, std::string>& getOptions() const { return options; }
    const std::vector<std::string>& getArgs() const { return positionalArgs; }

    // Command registration
    void registerCommand(const std::string& name, const std::string& description,
                         const std::string& usage, const std::vector<std::string>& flags,
                         std::function<void()> action, bool requiresBrain = false);

    // Help and version
    void printHelp() const;
    void printVersion() const;

    // Brain operations
    bool createBrain(const std::map<std::string, std::string>& config);
    bool runBrain(const std::map<std::string, std::string>& config, 
                 const std::vector<std::string>& commands);
    bool saveBrain(const std::string& filename);
    bool loadBrain(const std::string& filename);
    bool resetBrain();

    // Get current brain state
    std::shared_ptr<Brain> getBrain() const { return currentBrain; }

private:
    std::map<std::string, std::string> options;
    std::vector<std::string> positionalArgs;
    std::vector<Command> commands;
    std::shared_ptr<Brain> currentBrain;
    bool helpRequested;
    bool versionRequested;

    // Command execution
    bool executeCommand(const std::string& name, const std::vector<std::string>& args);
    void setupDefaultCommands();

    // Configuration parsing
    void parseConfigFile(const std::string& filename);
    void applyConfigToBrain(const std::map<std::string, std::string>& config);

    // Argument parsing helpers
    void parseShortOptions(const std::string& arg);
    void parseLongOptions(const std::string& arg);
    bool isOption(const std::string& arg) const;
    std::string extractOptionValue(const std::string& arg) const;
};

// Advanced CLI commands for power users
class AdvancedCLI {
public:
    AdvancedCLI(std::shared_ptr<Brain> brain, CommandLineParser& parser);
    ~AdvancedCLI();

    // Performance monitoring
    void benchmark(int steps = 1000);
    void profile(float duration = 10.0f);
    void analyzeNetworkTopology();

    // Advanced plasticity control
    void enableAllPlasticity(bool enable = true);
    void setPlasticityStrength(float strength);
    void resetPlasticity();

    // Memory management
    void consolidateMemory();
    void clearMemory();

    // Development features
    void enableDebugMode(bool enable = true);
    void setLogLevel(const std::string& level);
    void generateBrainReport();

    // Export/import functionality
    void exportNeurons(const std::string& filename);
    void importNeurons(const std::string& filename);
    void exportConnections(const std::string& filename);
    void importConnections(const std::string& filename);

    // Simulation control
    void setSimulationSpeed(float speed);
    void pauseSimulation(bool pause);
    void setCustomTimestep(float dt);

private:
    std::shared_ptr<Brain> brain_;
    CommandLineParser& parser_;
    bool debugMode_;
    float simulationSpeed_;
    float customTimestep_;
};

// Configuration file parser
class ConfigParser {
public:
    struct ConfigValue {
        std::string key;
        std::string value;
        std::string type;
        bool required;
    };

    static std::map<std::string, std::string> parseINIFile(const std::string& filename);
    static std::map<std::string, std::string> parseJSONFile(const std::string& filename);
    static void validateConfig(const std::map<std::string, std::string>& config);
    static void printConfigHelp();
};

} // namespace nlm

#endif // NLM_CLI_HPP
