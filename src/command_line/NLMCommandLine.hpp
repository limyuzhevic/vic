// NLM Command Line Interface
// Enhanced command-line tool for running NLM simulations

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "sensory/SensoryPercept.hpp"
#include "motor/Action.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "world/SimpleWorld.hpp"

namespace nlm {

// Command-line argument parser
class CommandLineParser {
public:
    struct Option {
        std::string longName;
        char shortName;
        std::string description;
        std::string defaultValue;
        bool required;
        std::string type;
    };
    
    struct Argument {
        std::string key;
        std::string value;
        std::string source;
    };
    
    CommandLineParser();
    
    void parseOptions();
    
    void printHelp() const;
    void printVersion() const;
    
    bool hasOption(const std::string& option) const;
    std::string getOptionValue(const std::string& option, const std::string& defaultValue = "") const;
    std::string getOptionSource(const std::string& option) const;
    
    bool validateOption(const std::string& option, const std::string& value) const;
    
    std::vector<Argument> getAllArguments() const;
    
private:
    std::vector<Option> options;
    std::vector<Argument> parsedArgs;
    
    void parseLongOption(const std::string& arg, const std::vector<std::string>& args, size_t& i);
    void parseShortOption(const std::string& arg, const std::vector<std::string>& args, size_t& i);
    void parseKeyValueOption(const std::string& arg);
    std::vector<std::string> getCommandLineArgs();
    std::string toLower(const std::string& str) const;
};

// Standalone NLM command-line tool
class NLMCommandLine {
public:
    static int run(int argc, char** argv);
    
private:
    static int validateConfiguration(CommandLineParser& parser);
    static int runBenchmark(CommandLineParser& parser);
    static int runDemo(CommandLineParser& parser);
    static int runPhase6Demo(CommandLineParser& parser);
    
    static void configureEnvironment(std::shared_ptr<Config> config);
    static void configureBrain(std::shared_ptr<Config> config);
    static void runDemoEpisodes(AgentBrain& agentBrain, SimpleWorld& world,
                               int episodes, int stepsPerEpisode);
    
    static void printBenchmarkResults(const BenchmarkResult& result);
};

// Benchmark result structure
struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
};

// Simple world for command-line interface
class SimpleWorldForCLI {
    static void configure(SimpleWorld& world, float width, float height,
                         size_t visionWidth, size_t visionHeight) {
        world.configure(width, height, visionWidth, visionHeight);
    }
};

// Demo configuration helper
void configureDemoEnvironment(std::shared_ptr<Config> config);
void configureDemoBrain(std::shared_ptr<Config> config);
void runDemoWithCLI(std::shared_ptr<Config> config, const std::vector<std(input)& args);
