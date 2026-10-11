#pragma once

#include <string>
#include <vector>
#include "AdvancedCLI.hpp"
#include "CommandOutputFormatter.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "brain/Brain.hpp"

namespace nlm {

class DebuggingCommands {
public:
    DebuggingCommands(std::shared_ptr<CommandOutputFormatter> formatter,
                      std::shared_ptr<Logger> logger);
    
    // Debugging commands
    bool debugSpikeEvents(int count);
    bool debugSpikeHistory(uint64_t neuronId);
    bool debugPlasticityHistory(uint64_t synapseId);
    bool debugMemoryTraces(int count);
    bool debugPlasticChange(uint64_t synapseId);
    bool debugNeuromodulation();
    bool debugSimulationClock();
    bool debugCheckpointStatus();
    bool debugMemoryConsistency();
    bool debugPlasticityConsistency();
    bool debugNeuralConsistency();
    
private:
    std::shared_ptr<CommandOutputFormatter> formatter;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<Brain> brain;
    
    // Debugging helpers
    void formatAndLogSpikeEvent(const std::string& event);
    void formatAndLogSpikeHistory(const std::vector<std::string>& history);
    void formatAndLogPlasticityHistory(const std::string& history);
    void formatAndLogMemoryTrace(const std::string& trace);
    void formatAndLogPlasticChange(const std::string& change);
    
    // Validation helpers
    bool validateConsistency(const std::string& system);
    std::vector<std::string> getRecentSpikeEvents(int count);
    std::vector<std::string> getSpikeHistory(uint64_t neuronId);
    std::vector<std::string> getPlasticityHistory(uint64_t synapseId);
    std::vector<std::string> getMemoryTraces(int count);
    std::vector<std::string> getRecentPlasticChanges(uint64_t synapseId);
    bool checkSimulationClockState();
    bool checkCheckpointState();
};
