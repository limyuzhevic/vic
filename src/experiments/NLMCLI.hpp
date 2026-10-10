// NLM Advanced Command-Line Interface
// Provides advanced commands for controlling the NLM brain
// Supports batch operations, configuration management, and system exploration

#pragma once

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace nlm {

class NLMCommandLineInterface;
class NLMBatchExecutor;
class ConfigFormatter;
class BrainExplorer;
class PhaseCLI;

// Factory functions for creating advanced CLI components
std::unique_ptr<NLMCommandLineInterface> createNLMCLI(std::shared_ptr<Config> config);
std::unique_ptr<NLMBatchExecutor> createNLMBatchExecutor(std::shared_ptr<Config> config);
std::unique_ptr<ConfigFormatter> createConfigFormatter();
std::unique_ptr<BrainExplorer> createBrainExplorer(std::shared_ptr<Brain> brain);
std::unique_ptr<PhaseCLI> createPhaseCLI(std::shared_ptr<Config> config);

// Command enumeration for advanced CLI
enum class AdvancedCommand {
    // Basic commands
    Help,
    Version,
    CreateBrain,
    LoadBrain,
    SaveBrain,
    
    // Configuration commands
    ExportConfig,
    ImportConfig,
    SetParameter,
    GetParameter,
    ListParameters,
    
    // Brain exploration commands
    ListRegions,
    InspectRegion,
    ListNeurons,
    InspectNeuron,
    ListSynapses,
    InspectSynapse,
    
    // Simulation commands
    RunTest,
    RunExperiment,
    Step,
    ResetBrain,
    InitializeBrain,
    
    // Analysis commands
    Statistics,
    Profile,
    
    // Batch operations
    LoadBatch,
    ExecuteBatch,
    
    // Configuration formatting
    FormatConfig,
    
    // Brain state inspection
    BrainReport,
    NetworkStats,
    MemoryReport,
    PredictionReport,
    NeuromodulationReport,
    
    // Phase-specific commands
    Phase3,
    Phase4,
    Phase5,
    Phase6
};

// Advanced command structure
struct AdvancedCommandInfo {
    std::string name;
    std::string description;
    std::string usage;
    std::vector<std::string> aliases;
    std::map<std::string, std::string> parameters;
    bool requiresBrain;
    bool requiresConfig;
};

// Command registry for advanced CLI
class CommandRegistry {
public:
    CommandRegistry();
    ~CommandRegistry();

    // Register command
    void registerCommand(AdvancedCommand cmd, const AdvancedCommandInfo& info);
    
    // Get command info
    AdvancedCommandInfo getCommandInfo(AdvancedCommand cmd) const;
    
    // Get all command names
    std::vector<std::string> getAllCommandNames() const;
    
    // Get command by name
    AdvancedCommand getCommandByName(const std::string& name) const;

private:
    std::map<AdvancedCommand, AdvancedCommandInfo> commandRegistry_;
};

// Global command registry instance
CommandRegistry& getCommandRegistry();

} // namespace nlm