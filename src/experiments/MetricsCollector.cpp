#include "MetricsCollector.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

void MetricsCollector::collectSimulationMetrics(std::shared_ptr<Brain> brain,
                                                std::shared_ptr<AgentBrain> agent,
                                                float totalReward,
                                                size_t totalSteps,
                                                float totalFiringRate,
                                                Phase6IntegrationResult& result) {
    validationErrors_.clear();
    
    if (!brain || !agent) {
        validationErrors_.push_back("Invalid brain or agent for metric collection");
        return;
    }
    
    try {
        // Calculate average metrics
        result.totalReward = (totalSteps > 0) ? (totalReward / totalSteps) : 0.0f;
        result.avgFiringRate = (totalSteps > 0) ? (totalFiringRate / totalSteps) : 0.0f;
        result.memoryEpisodesStored = brain->getEpisodicMemory() ? 
            static_cast<float>(brain->getEpisodicMemory()->getEpisodeCount()) : 0.0f;
        result.noveltyLevel = agent->getNoveltyLevel();
        result.curiosityLevel = agent->getCuriosityLevel();
        result.dopamineLevel = agent->getNeuromodulationLevel();
        
        // Calculate synaptic weight average (placeholder - should be implemented based on Brain API)
        result.avgSynapticWeight = 0.0f;
        
        NLM_LOG_INFO("Metrics collected - Reward: " + std::to_string(result.totalReward) +
                    " | Firing Rate: " + std::to_string(result.avgFiringRate) +
                    " | Episodes: " + std::to_string(result.memoryEpisodesStored));
        
    } catch (const std::exception& e) {
        validationErrors_.push_back("Failed to collect metrics: " + std::string(e.what()));
        NLM_LOG_ERROR("Metrics collection failed: " + std::string(e.what()));
    }
}

bool MetricsCollector::validateMetrics(const Phase6IntegrationResult& result) const {
    validationErrors_.clear();
    
    // Validate reward
    if (result.totalReward < ExperimentConstants::MIN_VALID_REWARD ||
        result.totalReward > ExperimentConstants::MAX_VALID_REWARD) {
        validationErrors_.push_back("Total reward out of valid range: " + std::to_string(result.totalReward));
    }
    
    // Validate firing rate
    if (result.avgFiringRate < ExperimentConstants::MIN_VALID_FIRING_RATE ||
        result.avgFiringRate > ExperimentConstants::MAX_VALID_FIRING_RATE) {
        validationErrors_.push_back("Average firing rate out of valid range: " + std::to_string(result.avgFiringRate));
    }
    
    // Validate novelty level
    if (result.noveltyLevel < ExperimentConstants::MIN_VALID_NOVELTY ||
        result.noveltyLevel > ExperimentConstants::MAX_VALID_NOVELTY) {
        validationErrors_.push_back("Novelty level out of valid range: " + std::to_string(result.noveltyLevel));
    }
    
    // Validate curiosity level
    if (result.curiosityLevel < ExperimentConstants::MIN_VALID_CURIOSTY ||
        result.curiosityLevel > ExperimentConstants::MAX_VALID_CURIOSTY) {
        validationErrors_.push_back("Curiosity level out of valid range: " + std::to_string(result.curiosityLevel));
    }
    
    // Validate dopamine level
    if (result.dopamineLevel < ExperimentConstants::MIN_VALID_DOPAMINE ||
        result.dopamineLevel > ExperimentConstants::MAX_VALID_DOPAMINE) {
        validationErrors_.push_back("Dopamine level out of valid range: " + std::to_string(result.dopamineLevel));
    }
    
    // Validate memory episodes
    if (result.memoryEpisodesStored < 0.0f) {
        validationErrors_.push_back("Memory episodes stored cannot be negative");
    }
    
    return validationErrors_.empty();
}

void MetricsCollector::calculatePerformanceStats(const Phase6IntegrationResult& result,
                                                 float& rewardPerStep,
                                                 float& rewardStdDev,
                                                 float& firingRatePerStep,
                                                 float& totalSimulationTime) const {
    // For now, provide basic statistics
    // In a real implementation, these would be calculated from step-by-step data
    rewardPerStep = result.totalReward;
    rewardStdDev = 0.0f; // Would need step-by-step reward data
    firingRatePerStep = result.avgFiringRate;
    totalSimulationTime = result.totalWallClockTime;
    
    NLM_LOG_INFO("Performance stats calculated - Reward/Step: " + std::to_string(rewardPerStep) +
                " | FiringRate/Step: " + std::to_string(firingRatePerStep) +
                " | TotalTime: " + std::to_string(totalSimulationTime) + "s");
}

bool MetricsCollector::isMetricsWithinRange(const Phase6IntegrationResult& result) const {
    // Check if metrics are within reasonable ranges for healthy simulation
    bool isValid = true;
    
    // Check for NaN or infinite values
    if (std::isnan(result.totalReward) || std::isinf(result.totalReward)) {
        validationErrors_.push_back("Total reward is NaN or infinite");
        isValid = false;
    }
    
    if (std::isnan(result.avgFiringRate) || std::isinf(result.avgFiringRate)) {
        validationErrors_.push_back("Average firing rate is NaN or infinite");
        isValid = false;
    }
    
    if (std::isnan(result.noveltyLevel) || std::isinf(result.noveltyLevel)) {
        validationErrors_.push_back("Novelty level is NaN or infinite");
        isValid = false;
    }
    
    // Check for implausibly high values
    if (result.totalReward > ExperimentConstants::MAX_REASONABLE_REWARD * 10) {
        validationErrors_.push_back("Total reward is unreasonably high");
        isValid = false;
    }
    
    if (result.avgFiringRate > ExperimentConstants::MAX_REASONABLE_FIRING_RATE * 10) {
        validationErrors_.push_back("Average firing rate is unreasonably high");
        isValid = false;
    }
    
    return isValid;
}

} // namespace nlm
