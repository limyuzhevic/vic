#pragma once

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <chrono>

namespace nlm {

/**
 * Advanced command-line tools for NLM brain management and analysis
 */

class NLMCommandLine {
public:
    NLMCommandLine();
    ~NLMCommandLine();
    
    bool parseArguments(int argc, char* argv[]);
    void runCommand();
    
private:
    enum class Command {
        HELP,
        CREATE_BRAIN,
        INITIALIZE_BRAIN,
        RUN_BRAIN,
        SAVE_BRAIN,
        LOAD_BRAIN,
        ANALYZE_BRAIN,
        BENCHMARK,
        CONFIG_EDITOR
    };
    
    enum class OutputFormat {
        PLAIN_TEXT,
        JSON,
        YAML
    };
    
    struct CommandOptions {
        Command command;
        std::string brainName;
        std::string configFile;
        std::string outputFile;
        uint64_t neuronCount;
        uint64_t regionCount;
        uint64_t steps;
        double timestep;
        bool verbose;
        bool quiet;
        OutputFormat outputFormat;
        std::string checkpointPath;
        bool enableFeatures;
        std::string features;
    };
    
    Command parseCommand(const std::string& cmd);
    OutputFormat parseOutputFormat(const std::string& format);
    bool parseConfigFile(const std::string& filepath, std::shared_ptr<Config> config);
    bool executeCommand(const CommandOptions& options);
    void printHelp() const;
    void printBrainInfo(const Brain& brain) const;
    void analyzeBrainPerformance(const Brain& brain, uint64_t steps) const;
    bool benchmarkBrain(const std::string& brainName, const CommandOptions& options);
    bool createBrainWithFeatures(const std::string& name, const std::shared_ptr<Config>& config, const std::string& features);
    bool initializeBrainWithLogging(const std::shared_ptr<Brain>& brain, uint64_t steps);
    bool runBrainWithAnalysis(const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world, const CommandOptions& options);
    std::shared_ptr<Config> createAdvancedConfig(const CommandOptions& options);
    
    std::shared_ptr<Config> defaultConfig;
    std::shared_ptr<Brain> currentBrain;
    std::shared_ptr<SimpleWorld> currentWorld;
    std::shared_ptr<AgentBrain> currentAgent;
};

} // namespace nlm