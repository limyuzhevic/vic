// NLM Command Line Interface
// Advanced CLI for controlling NLM experiments and operations

#ifndef NLM_CLI_HPP
#define NLM_CLI_HPP

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

class CLI {
public:
    struct Options {
        std::string configFile = "configs/default.cfg";
        std::string mode = "auto"; // auto, basic, plasticity, stdp, benchmark, script
        std::string outputFile;
        std::string logLevel = "info";
        bool verbose = false;
        bool help = false;
        bool version = false;
        int seed = 42;
        int steps = 1000;
        int neurons = 500;
        float timestep = 0.001f;
        std::vector<std::string> scriptArgs;
        bool enableCheckpoint = false;
        bool enableReplay = false;
        bool enableDevelopment = false;
        std::string checkpointPath = "/tmp/nlm_checkpoint.bin";
        std::string scriptPath;
        bool listCommands = false;
        bool showAdvancedHelp = false;
    };

    struct Command {
        std::string name;
        std::string description;
        std::function<bool(const Options&)> callback;
        std::string usage;
    };

    CLI();
    ~CLI();

    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Run the selected mode
    bool run();
    
    // Print help information
    void printHelp() const;
    void printAdvancedHelp() const;
    
    // Get parsed options
    const Options& getOptions() const { return options; }
    
    // Register a command
    void registerCommand(const Command& cmd);

private:
    Options options;
    std::vector<Command> commands;
    
    // Built-in commands
    bool runAutoMode(const Options& opts);
    bool runBasicMode(const Options& opts);
    bool runPlasticityMode(const Options& opts);
    bool runStdpMode(const Options& opts);
    bool runBenchmarkMode(const Options& opts);
    bool runScriptMode(const Options& opts);
    bool runIntegrationMode(const Options& opts);
    bool runAnalysisMode(const Options& opts);
    
    void setupCommands();
};

} // namespace nlm

#endif // NLM_CLI_HPP