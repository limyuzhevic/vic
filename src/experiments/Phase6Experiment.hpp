#pragma once

/**
 * Phase6Experiment.hpp
 * Phase 6 experiment for testing the complete integrated brain
 */

#include "../experiments/Experiment.hpp"
#include "../experiments/Phase6IntegratedExperiment.hpp"
#include <memory>
#include <string>

namespace nlm {

/**
 * Phase 6 Integration Experiment
 * Tests the complete integrated brain system including all subsystems
 */
class Phase6Experiment : public Experiment {
public:
    Phase6Experiment();
    ~Phase6Experiment() override;
    
    /**
     * Initialize the experiment with brain components
     */
    bool initialize() override;
    
    /**
     * Run the Phase 6 integration experiment
     * Tests all brain systems working together
     */
    Phase6IntegrationResult run(const Phase6Config& config);
    
    /**
     * Run the experiment with generic ExperimentConfig
     * Converts ExperimentConfig to Phase6Config and runs
     */
    ExperimentResult run(const ExperimentConfig& config) override;
    
    /**
     * Run comprehensive integration tests
     * Tests each system and their interconnections
     */
    bool runTests();
    
    /**
     * Get experiment description
     */
    std::string getDescription() const override;
    
    /**
     * Get experiment name
     */
    std::string getName() const override;
    
    /**
     * Get experiment version
     */
    std::string getVersion() const override;
    
    /**
     * Get experiment status
     */
    ExperimentStatus getStatus() const override;
    
    /**
     * Set experiment status
     */
    void setStatus(const ExperimentStatus& status) override;
    
    /**
     * Get experiment metrics
     */
    const Metrics& getMetrics() const override;
    
    /**
     * Get experiment metrics
     */
    Metrics& getMetrics() override;

private:
    // Internal experiment runner for Phase 6 integration tests
    std::unique_ptr<Phase6IntegratedExperiment> experimentRunner_;
    
    // Current experiment status
    ExperimentStatus status_;
    
    // Experiment metrics
    Metrics metrics_;
};

} // namespace nlm
