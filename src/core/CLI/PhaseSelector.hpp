// Phase selector for unified interface
// Provides unified interface for selecting and executing different phases

#pragma once

#include <string>

namespace nlm {

class CLIResult;

class PhaseSelector {
public:
    enum Phase {
        Phase1,
        Phase2,
        Phase3,
        Phase4,
        Phase5,
        Phase6,
        Benchmark,
        All
    };
    
    PhaseSelector();
    
    // Parse phase string to enum
    Phase parsePhase(const std::string& phaseStr);
    
    // Get current phase
    Phase getCurrentPhase() const;
    
    // Set current phase
    void setCurrentPhase(Phase phase);
    
    // Convert phase enum to string
    std::string phaseToString(Phase phase);
    
    // Execute a specific phase with CLI results
    void executePhase(Phase phase, const CLIResult& cliResult);
    
private:
    Phase currentPhase;
};

} // namespace nlm
