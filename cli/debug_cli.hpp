// NLM CLI Debug - Advanced debugging commands header
// Handles neural activity visualization, spike trains, and synapse tracking

#ifndef NLM_CLI_DEBUG_HPP
#define NLM_CLI_DEBUG_HPP

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <algorithm>

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/ExperimentRunner.hpp"

namespace nlm {
namespace cli {

class DebugCLI {
public:
    DebugCLI();
    
    int run(int argc, char** argv);
    
private:
    void initializeLogger();
    
    // Debug commands
    int handleNeuralDebugCommand(int argc, char** argv);
    int handleSpikeDebugCommand(int argc, char** argv);
    int handleSynapseDebugCommand(int argc, char** argv);
    int handleMemoryDebugCommand(int argc, char** argv);
    int handleTraceDebugCommand(int argc, char** argv);
    int handleVisualizeCommand(int argc, char** argv);
    int handleErrorDebugCommand(int argc, char** argv);
    int handleBreakpointCommand(int argc, char** argv);
    int handleWatchCommand(int argc, char** argv);
    int handleInspectCommand(int argc, char** argv);
    int handleLogCommand(int argc, char** argv);
    
    // Debug data structures
    struct NeuralActivity {
        std::chrono::system_clock::time_point timestamp;
        std::vector<double> firingRates;
        std::vector<double> membranePotentials;
        std::vector<double> synapticInputs;
        double globalField;
        size_t stepNumber;
    };
    
    struct synapse_evolution_data {
        std::chrono::system_clock::time_point timestamp;
        size_t stepNumber;
    };
    
    struct memory_trace {
        std::chrono::system_clock::time_point timestamp;
        size_t stepNumber;
        size_t memoryUsage;
        size_t spikeCount;
    };
    
    struct trace_entry {
        std::chrono::system_clock::time_point timestamp;
        size_t stepNumber;
        double time;
        size_t spikeCount;
        size_t firingNeuronCount;
    };
    
    // Helper functions
    void debugNeuralActivity(std::shared_ptr<Brain> brain, std::vector<NeuralActivity>& history);
    void debugSynapseEvolution(std::shared_ptr<Brain> brain, std::vector<synapse_evolution_data>& history);
    void debugMemoryTrace(std::shared_ptr<Brain> brain, std::vector<memory_trace>& history);
    void traceEntry(std::shared_ptr<Brain> brain, std::vector<trace_entry>& trace, size_t step, double time);
    
    void generateNeuralActivityReport(const std::vector<NeuralActivity>& history, 
                                      const std::string& outputFile,
                                      const std::string& format);
    void generateSpikeTraceReport(const std::vector<std::vector<size_t>>& spikeTrains,
                                 const std::vector<double>& firingRates,
                                 const std::string& outputFile);
    void generateSynapseEvolutionReport(const std::vector<synapse_evolution_data>& history,
                                       const std::string& outputFile);
    void generateMemoryTraceReport(const std::vector<memory_trace>& history,
                                   const std::string& outputFile);
    void generateTraceReport(const std::vector<trace_entry>& trace, const std::string& outputFile);
    
    void generateVisualizationData(std::shared_ptr<Brain> brain,
                                   const std::string& outputFile,
                                   const std::string& plotType,
                                   size_t duration);
    
    void printUsage();
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_DEBUG_HPP
