#pragma once

#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "ExperimentConstants.hpp"
#include <vector>
#include <string>

namespace nlm {

/**
 * Metrics collection and analysis
 * Handles collection and validation of experiment metrics
 */
class MetricsCollector {
public:
    MetricsCollector() = default;
    ~MetricsCollector() = default;
    
    /**
     * Collect simulation metrics
     */
    void collectSimulationMetrics(std::shared_ptr<Brain> brain,
                                  std::shared_ptr<AgentBrain> agent,
                                  float totalReward,
                                  size_t totalSteps,
                                  float totalFiringRate,
                                  Phase6IntegrationResult& result);
    
    /**
     * Validate collected metrics
     */
    bool validateMetrics(const Phase6IntegrationResult& result) const;
    
    /**
     * Get validation error messages
     */
    const std::vector<std::string>& getValidationErrors() const { return validationErrors_; }
    
    /**
     * Calculate performance statistics
     */
    void calculatePerformanceStats(const Phase6IntegrationResult& result,
                                   float& rewardPerStep,
                                   float& rewardStdDev,
                                   float& firingRatePerStep,
                                   float& totalSimulationTime) const;
    
    /**
     * Check if metrics are within acceptable ranges
     */
    bool isMetricsWithinRange(const Phase6IntegrationResult& result) const;
    
private:
    std::vector<std::string> validationErrors_;
};

} // namespace nlm
