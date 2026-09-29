#include "ResultsValidator.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>

namespace nlm {

bool ResultsValidator::validateResults(const Phase6IntegrationResult& result) {
    validationErrors_.clear();
    isValid_ = true;
    
    try {
        // Validate basic result structure
        if (std::isnan(result.totalReward) || std::isinf(result.totalReward)) {
            validationErrors_.push_back("Total reward is NaN or infinite");
            isValid_ = false;
        }
        
        if (std::isnan(result.avgFiringRate) || std::isinf(result.avgFiringRate)) {
            validationErrors_.push_back("Average firing rate is NaN or infinite");
            isValid_ = false;
        }
        
        if (std::isnan(result.memoryEpisodesStored) || std::isinf(result.memoryEpisodesStored)) {
            validationErrors_.push_back("Memory episodes stored is NaN or infinite");
            isValid_ = false;
        }
        
        // Validate timestamp consistency
        if (result.endTime <= 0) {
            validationErrors_.push_back("End time is invalid");
            isValid_ = false;
        }
        
        if (result.startTime >= result.endTime) {
            validationErrors_.push_back("Start time is not before end time");
            isValid_ = false;
        }
        
        if (result.totalWallClockTime < 0.0) {
            validationErrors_.push_back("Wall clock time cannot be negative");
            isValid_ = false;
        }
        
        // Validate against thresholds
        if (!validateAgainstThresholds(result)) {
            isValid_ = false;
        }
        
        // Validate consistency
        if (!validateResultConsistency(result)) {
            isValid_ = false;
        }
        
        // Log validation results
        if (isValid_) {
            NLM_LOG_INFO("Results validation passed");
        } else {
            NLM_LOG_ERROR("Results validation failed with " + std::to_string(validationErrors_.size()) + " errors");
            for (const auto& error : validationErrors_) {
                NLM_LOG_ERROR("  - " + error);
            }
        }
        
    } catch (const std::exception& e) {
        validationErrors_.push_back("Validation exception: " + std::string(e.what()));
        NLM_LOG_ERROR("Results validation exception: " + std::string(e.what()));
        isValid_ = false;
    }
    
    return isValid_;
}

bool ResultsValidator::validateAgainstThresholds(const Phase6IntegrationResult& result) const {
    bool isValid = true;
    
    // Check reward thresholds
    if (result.totalReward < ExperimentConstants::MIN_VALID_REWARD ||
        result.totalReward > ExperimentConstants::MAX_VALID_REWARD) {
        validationErrors_.push_back("Total reward out of valid range: " + std::to_string(result.totalReward));
        isValid = false;
    }
    
    // Check firing rate thresholds
    if (result.avgFiringRate < ExperimentConstants::MIN_VALID_FIRING_RATE ||
        result.avgFiringRate > ExperimentConstants::MAX_VALID_FIRING_RATE) {
        validationErrors_.push_back("Average firing rate out of valid range: " + std::to_string(result.avgFiringRate));
        isValid = false;
    }
    
    // Check novelty thresholds
    if (result.noveltyLevel < ExperimentConstants::MIN_VALID_NOVELTY ||
        result.noveltyLevel > ExperimentConstants::MAX_VALID_NOVELTY) {
        validationErrors_.push_back("Novelty level out of valid range: " + std::to_string(result.noveltyLevel));
        isValid = false;
    }
    
    // Check curiosity thresholds
    if (result.curiosityLevel < ExperimentConstants::MIN_VALID_CURIOSTY ||
        result.curiosityLevel > ExperimentConstants::MAX_VALID_CURIOSTY) {
        validationErrors_.push_back("Curiosity level out of valid range: " + std::to_string(result.curiosityLevel));
        isValid = false;
    }
    
    // Check dopamine thresholds
    if (result.dopamineLevel < ExperimentConstants::MIN_VALID_DOPAMINE ||
        result.dopamineLevel > ExperimentConstants::MAX_VALID_DOPAMINE) {
        validationErrors_.push_back("Dopamine level out of valid range: " + std::to_string(result.dopamineLevel));
        isValid = false;
    }
    
    // Check memory episodes
    if (result.memoryEpisodesStored < 0.0f) {
        validationErrors_.push_back("Memory episodes stored cannot be negative");
        isValid = false;
    }
    
    return isValid;
}

bool ResultsValidator::validateResultConsistency(const Phase6IntegrationResult& result) const {
    bool isValid = true;
    
    // Check for implausibly high values
    if (result.totalReward > ExperimentConstants::MAX_REASONABLE_REWARD * 10) {
        validationErrors_.push_back("Total reward is unreasonably high: " + std::to_string(result.totalReward));
        isValid = false;
    }
    
    if (result.avgFiringRate > ExperimentConstants::MAX_REASONABLE_FIRING_RATE * 10) {
        validationErrors_.push_back("Average firing rate is unreasonably high: " + std::to_string(result.avgFiringRate));
        isValid = false;
    }
    
    // Check for negative values where not expected
    if (result.avgFiringRate < 0.0f) {
        validationErrors_.push_back("Average firing rate cannot be negative: " + std::to_string(result.avgFiringRate));
        isValid = false;
    }
    
    if (result.memoryEpisodesStored < 0.0f) {
        validationErrors_.push_back("Memory episodes stored cannot be negative: " + std::to_string(result.memoryEpisodesStored));
        isValid = false;
    }
    
    // Check for NaN or infinite values
    if (std::isnan(result.totalReward) || std::isinf(result.totalReward)) {
        validationErrors_.push_back("Total reward is NaN or infinite");
        isValid = false;
    }
    
    if (std::isnan(result.avgFiringRate) || std::isinf(result.avgFiringRate)) {
        validationErrors_.push_back("Average firing rate is NaN or infinite");
        isValid = false;
    }
    
    // Check integration flags consistency
    if (result.checkpointingWorks && result.checkpointPath.empty()) {
        validationErrors_.push_back("Checkpointing works but path is empty");
        isValid = false;
    }
    
    return isValid;
}

float ResultsValidator::calculateQualityScore(const Phase6IntegrationResult& result) const {
    float score = 0.0f;
    
    // Reward quality (40% weight)
    float rewardScore = 1.0f;
    if (result.totalReward < ExperimentConstants::MIN_VALID_REWARD) {
        rewardScore = std::max(0.0f, result.totalReward / ExperimentConstants::MIN_VALID_REWARD);
    } else if (result.totalReward > ExperimentConstants::MAX_VALID_REWARD) {
        rewardScore = std::max(0.0f, 1.0f - (result.totalReward - ExperimentConstants::MAX_VALID_REWARD) / ExperimentConstants::MAX_VALID_REWARD);
    }
    score += rewardScore * 0.4f;
    
    // Firing rate quality (20% weight)
    float firingScore = 1.0f;
    if (result.avgFiringRate < ExperimentConstants::MIN_VALID_FIRING_RATE) {
        firingScore = result.avgFiringRate / ExperimentConstants::MIN_VALID_FIRING_RATE;
    } else if (result.avgFiringRate > ExperimentConstants::MAX_VALID_FIRING_RATE) {
        firingScore = 1.0f - (result.avgFiringRate - ExperimentConstants::MAX_VALID_FIRING_RATE) / ExperimentConstants::MAX_VALID_FIRING_RATE;
    }
    firingScore = std::max(0.0f, firingScore);
    score += firingScore * 0.2f;
    
    // Memory quality (20% weight)
    float memoryScore = 1.0f;
    if (result.memoryEpisodesStored > 0.0f) {
        memoryScore = std::min(1.0f, result.memoryEpisodesStored / 100.0f); // Normalize to 100 episodes
    }
    score += memoryScore * 0.2f;
    
    // Neuromodulation quality (20% weight)
    float neuromodScore = 0.0f;
    if (result.noveltyLevel > 0.0f) neuromodScore += 0.33f;
    if (result.curiosityLevel > 0.0f) neuromodScore += 0.33f;
    if (result.dopamineLevel > 0.0f) neuromodScore += 0.34f;
    score += neuromodScore * 0.2f;
    
    // Integration quality (10% weight)
    float integrationScore = 0.0f;
    if (result.memoryWorkingMemoryIntegrated) integrationScore += 0.25f;
    if (result.memoryEpisodicMemoryIntegrated) integrationScore += 0.25f;
    if (result.neuromodulationIntegrated) integrationScore += 0.25f;
    if (result.predictionIntegrated) integrationScore += 0.25f;
    if (result.developmentIntegrated) integrationScore += 0.25f;
    score += integrationScore * 0.1f;
    
    return score * 100.0f; // Return as percentage
}

std::string ResultsValidator::getValidationSummary() const {
    std::stringstream ss;
    
    ss << "Results Validation Summary:" << std::endl;
    ss << "  Valid: " << (isValid_ ? "Yes" : "No") << std::endl;
    ss << "  Errors: " << validationErrors_.size() << std::endl;
    
    if (!validationErrors_.empty()) {
        ss << "  Issues:" << std::endl;
        for (const auto& error : validationErrors_) {
            ss << "    - " << error << std::endl;
        }
    }
    
    if (isValid_) {
        ss << "  All checks passed!";
    }
    
    return ss.str();
}

} // namespace nlm
