// Phase 6 experiment definition
// This file contains the Phase 6 experiment class definition
// Compatible with the existing Phase6IntegratedExperiment.hpp structure

#pragma once

#include "Phase6Config.hpp"
#include "Phase6IntegratedExperiment.hpp"
#include <string>

namespace nlm {

// Phase 6 experiment class for advanced power users
// This is a simplified version that builds on Phase6IntegratedExperiment
class Phase6Experiment {
public:
    Phase6Experiment() = default;
    Phase6Experiment(const Phase6Config& config);
    ~Phase6Experiment();
    
    // Run the complete Phase 6 integration experiment
    Phase6IntegrationResult run();
    
    // Export results in various formats
    bool exportResults(const std::string& filename, const std::string& format = "json");
    
    // Set experiment configuration
    void setConfiguration(const Phase6Config& config);
    
    // Get experiment configuration
    const Phase6Config& getConfiguration() const;
    
    // Verify integration status
    bool verifyIntegration() const;
    
private:
    Phase6Config config;
    Phase6IntegrationResult result;
    
    // Export helper methods
    bool exportToJSON(const std::string& filename);
    bool exportToCSV(const std::string& filename);
    bool exportToHDF5(const std::string& filename);
};

} // namespace nlm
