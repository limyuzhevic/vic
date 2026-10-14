# Advanced NLM Command Line Interface

// Advanced command system for NLM (熙然) neural network simulation
// Provides enhanced user control, debugging, visualization, and performance monitoring

#ifndef NLM_ADVANCED_COMMANDS_HPP
#define NLM_ADVANCED_COMMANDS_HPP

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <chrono>
#include <thread>
#include <map>
#include <iomanip>
#include <sstream>
#include <fstream>

namespace nlm {

// Command types
enum class CommandType {
    CreateSimulation,
    Benchmark,
    Profile,
    Visualize,
    Compare,
    DebugMode,
    Step,
    Breakpoint,
    Inspect,
    Patch,
    Help,
    Version,
    Status,
    Config,
    Save,
    Load,
    Reset,
    Exit
};

// Command structure
struct Command {
    CommandType type;
    std::vector<std::string> args;
    std::string description;
    std::function<void(const Command&, std::shared_ptr<Brain>, std::shared_ptr<Config>)> handler;
};

// Advanced command system
class AdvancedCommandSystem {
public:
    AdvancedCommandSystem(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    
    // Command registration
    void registerCommand(CommandType type, const std::string& description, 
                       std::function<void(const Command&, std::shared_ptr<Brain>, std::shared_ptr<Config>)> handler);
    
    // Command execution
    bool executeCommand(const std::string& input, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    
    // Command parsing
    Command parseCommand(const std::string& input);
    
    // Help and documentation
    void showHelp() const;
    void showCommandHelp(CommandType type) const;
    
    // Command completion
    std::vector<std::string> completeCommand(const std::string& partial) const;
    
    // Performance benchmarking
    struct BenchmarkResult {
        double executionTime;
        size_t neuronCount;
        size_t synapseCount;
        float learningRate;
        float accuracy;
    };
    
    BenchmarkResult runBenchmark(const std::string& benchmarkType = "default");
    
    // Performance profiling
    struct ProfileData {
        double totalTime;
        size_t stepCount;
        size_t spikeCount;
        float avgFiringRate;
        std::map<std::string, double> componentTimes;
    };
    
    ProfileData startProfile();
    void stopProfile();
    void printProfileReport() const;
    
    // Interactive debugging
    class Debugger {
    public:
        enum class BreakpointType { Step, Spike, Learning, Memory };
        
        struct Breakpoint {
            BreakpointType type;
            size_t targetValue;
            bool triggered;
            std::string description;
        };
        
        void setBreakpoint(BreakpointType type, size_t target);
        void clearBreakpoint(BreakpointType type);
        bool checkBreakpoints(const std::shared_ptr<Brain>& brain);
        void stepThrough(const std::shared_ptr<Brain>& brain);
        void printDebugInfo(const std::shared_ptr<Brain>& brain) const;
        
    private:
        std::vector<Breakpoint> breakpoints;
        bool debugMode;
        size_t stepCount;
    };
    
    // Configuration management
    struct SimulationTemplate {
        std::string name;
        std::string description;
        std::map<std::string, std::string> parameters;
    };
    
    void saveTemplate(const std::string& name, const std::string& description);
    bool loadTemplate(const std::string& name);
    void listTemplates() const;
    
    // Visualization interface
    class VisualizationInterface {
    public:
        void showNetworkGraph() const;
        void showSpikeRaster() const;
        void showWeightMatrix() const;
        void showActivityHeatmap() const;
        void showMemoryUsage() const;
        void showPerformanceMetrics() const;
        
    private:
        void updateDisplay() const;
        void renderNetwork() const;
        void renderSpikes() const;
        void renderWeights() const;
    } visualization;
    
    // Comparison tools
    struct ComparisonResult {
        std::string scenario1;
        std::string scenario2;
        double performanceDifference;
        float accuracyDifference;
        std::map<std::string, double> metricDifferences;
    };
    
    ComparisonResult compareSimulations(const std::string& scenario1, const std::string& scenario2);
    
    // State management
    void saveState(const std::string& filepath);
    bool loadState(const std::string& filepath);
    void printState() const;
    
    // Configuration commands
    void processConfigCommand(const std::string& key, const std::string& value);
    void printConfigSummary() const;
    
    // State commands
    void resetBrain();
    void printBrainStatus() const;

private:
    std::unordered_map<CommandType, Command> commands;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    Debugger debugger;
    ProfileData currentProfile;
    bool profileActive;
    std::chrono::high_resolution_clock::time_point profileStartTime;
    
    // Command helper methods
    void parseArguments(const std::string& input, std::string& command, std::vector<std::string>& args);
    bool validateCommand(const Command& cmd) const;
    void executeCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    
    // Configuration templates
    std::vector<SimulationTemplate> templates;
    
    // Performance monitoring
    std::chrono::high_resolution_clock::time_point lastStepTime;
    double totalStepTime;
    size_t stepCount;
    
    // Command definitions
    void defineCommands();
    Command createCommand(CommandType type, const std::string& description);
};

} // namespace nlm

#endif // NLM_ADVANCED_COMMANDS_HPP
