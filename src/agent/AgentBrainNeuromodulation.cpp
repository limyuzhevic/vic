#pragma once

#include "AgentBrainComponents.hpp"
#include "AgentBrainConfig.hpp"
#include "AgentBrainError.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

AgentBrainNeuromodulation::AgentBrainNeuromodulation() 
    : rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , config_(std::make_shared<AgentBrainConfig>()) {}

AgentBrainNeuromodulation::AgentBrainNeuromodulation() 
    : rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , config_(std::make_shared<AgentBrainConfig>()) {}

AgentBrainResult<void> AgentBrainNeuromodulation::applyReward(const std::shared_ptr<Brain>& brain, float reward, float predictedReward, float plasticityModifier) {
    try {
        if (!validateNotNull(brain.get(), "brain pointer")) {
            return AgentBrainInvalidInputError("AgentBrainNeuromodulation",
                                              "brain pointer cannot be null in applyReward");
        }
        
        if (!rewardModulationEnabled_) {
            return AgentBrainResult<void>(true); // Early return if disabled
        }
        
        if (!validateRange(reward, -1.0f, 1.0f, "reward value")) {
            return AgentBrainInvalidInputError("AgentBrainNeuromodulation",
                                              "reward value " + std::to_string(reward) + " is out of valid range [-1.0, 1.0]");
        }
        
        if (!validateRange(predictedReward, -1.0f, 1.0f, "predicted reward value")) {
            return AgentBrainInvalidInputError("AgentBrainNeuromodulation",
                                              "predicted reward value " + std::to_string(predictedReward) + " is out of valid range [-1.0, 1.0]");
        }
        
        if (!validateRange(plasticityModifier, 0.0f, 2.0f, "plasticity modifier")) {
            return AgentBrainInvalidInputError("AgentBrainNeuromodulation",
                                              "plasticity modifier " + std::to_string(plasticityModifier) + " is out of valid range [0.0, 2.0]");
        }
        
        // Compute prediction error
        predictionError_ = reward - predictedReward;
        
        // Update expected reward (exponential moving average)
        expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
        
        // Dopamine-like signal (based on prediction error) with config sensitivity
        float dopamineSensitivity = config_->getParameter<float>(AgentBrainConfig::ParameterID::DOPAMINE_SENSITIVITY);
        dopamineLevel_ = predictionError_ * dopamineSensitivity;
        
        // Clamp to config limits
        float clampMin = config_->getParameter<float>(AgentBrainConfig::ParameterID::DOPAMINE_CLAMP_MIN);
        float clampMax = config_->getParameter<float>(AgentBrainConfig::ParameterID::DOPAMINE_CLAMP_MAX);
        dopamineLevel_ = std::clamp(dopamineLevel_, clampMin, clampMax);
        
        // Apply to all synapses with eligibility traces
        if (brain->getRegions()) {
            for (const auto& region : brain->getRegions()) {
                if (!validateNotNull(region, "region pointer")) {
                    continue; // Graceful degradation: skip invalid region
                }
                
                if (!region->getSynapses()) continue;
                
                for (auto* syn : region->getSynapses()) {
                    if (!validateNotNull(syn, "synapse pointer")) continue;
                    
                    float eligibility = syn->getEligibilityTrace();
                    
                    if (std::abs(eligibility) > 0.001f) {
                        // Apply reward-modulated weight change
                        float delta = eligibility * dopamineLevel_ * plasticityModifier;
                        try {
                            syn->addToWeight(delta);
                            
                            // Decay eligibility trace (use config plasticity)
                            float decayRate = 0.1f * plasticityModifier;
                            syn->decayEligibilityTrace(decayRate);
                        } catch (const std::exception& e) {
                            // Graceful degradation: skip synapse if operations fail
                            continue;
                        }
                    }
                }
            }
        }
        
        // Modulate plasticity based on dopamine (use config parameters)
        // Positive dopamine increases plasticity, negative decreases
        float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
        plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
        
        // Apply to STDP
        auto* stdp = brain->getSTDP();
        if (stdp) {
            try {
                // Use config base values multiplied by plasticity factor
                float baseLTP = 0.01f * plasticityFactor;
                float baseLTD = 0.012f * plasticityFactor;
                stdp->setLTPWeight(baseLTP);
                stdp->setLTDWeight(baseLTD);
            } catch (const std::exception& e) {
                // Graceful degradation: skip STDP if operations fail
                continue;
            }
        }
        
        return AgentBrainResult<void>(true);
    } catch (const std::bad_alloc&) {
        return AgentBrainMemoryError("AgentBrainNeuromodulation", 
                                   "memory allocation failed in applyReward");
    } catch (const std::exception& e) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR,
            "AgentBrainNeuromodulation", std::string("exception in applyReward: ") + e.what()));
    }
}

AgentBrainResult<void> AgentBrainNeuromodulation::updateDevelopment(const std::shared_ptr<Brain>& brain, double timestep, double& developmentalAge, float& plasticityModifier) {
    try {
        if (!validateNotNull(brain.get(), "brain pointer")) {
            return AgentBrainInvalidInputError("AgentBrainNeuromodulation",
                                              "brain pointer cannot be null in updateDevelopment");
        }
        
        if (!validateRange(timestep, 0.0, 3600.0, "timestep")) {
            return AgentBrainInvalidInputError("AgentBrainNeuromodulation",
                                              "timestep " + std::to_string(timestep) + " is out of valid range [0.0, 3600.0]");
        }
        
        if (!validateNotNull(&developmentalAge, "developmentalAge pointer")) {
            return AgentBrainInvalidInputError("AgentBrainNeuromodulation",
                                              "developmentalAge pointer cannot be null in updateDevelopment");
        }
        
        if (!validateNotNull(&plasticityModifier, "plasticityModifier pointer")) {
            return AgentBrainInvalidInputError("AgentBrainNeuromodulation",
                                              "plasticityModifier pointer cannot be null in updateDevelopment");
        }
        
        if (!developmentEnabled_) {
            return AgentBrainResult<void>(true); // Early return if disabled
        }
        
        developmentalAge += timestep;
        
        // Simple developmental stages based on age (use config stage thresholds)
        double stageInitial = config_->getParameter<double>(AgentBrainConfig::ParameterID::DEVELOPMENT_STAGE_INITIAL);
        double stageCritical = config_->getParameter<double>(AgentBrainConfig::ParameterID::DEVELOPMENT_STAGE_CRITICAL);
        double stageMaturation = config_->getParameter<double>(AgentBrainConfig::ParameterID::DEVELOPMENT_STAGE_MATURATION);
        
        if (developmentalAge < stageInitial) {  // ~1 minute
            plasticityModifier = config_->getParameter<float>(AgentBrainConfig::ParameterID::PLASTICITY_MODIFIER_INITIAL);
            brain->setDevelopmentalStage(DevelopmentalStage::Initial);
        } else if (developmentalAge < stageCritical) {  // ~5 minutes
            plasticityModifier = config_->getParameter<float>(AgentBrainConfig::ParameterID::PLASTICITY_MODIFIER_CRITICAL);
            brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else if (developmentalAge < stageMaturation) {  // ~15 minutes
            plasticityModifier = config_->getParameter<float>(AgentBrainConfig::ParameterID::PLASTICITY_MODIFIER_MATURATION);
            brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
        } else {
            plasticityModifier = config_->getParameter<float>(AgentBrainConfig::ParameterID::PLASTICITY_MODIFIER_ADULT);
            brain->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
        
        // Structural plasticity changes with development (use config rates)
        if (structuralPlasticityEnabled_) {
            auto* sp = brain->getStructuralPlasticity();
            if (sp) {
                // Higher synaptogenesis in early development (use config base rates)
                float synRateBase = config_->getParameter<float>(AgentBrainConfig::ParameterID::SYNAPTOGENESIS_RATE_BASE);
                float pruneRateBase = config_->getParameter<float>(AgentBrainConfig::ParameterID::PRUNING_RATE_BASE);
                
                float synRate = synRateBase * plasticityModifier;
                float pruneRate = pruneRateBase * (2.0f - plasticityModifier);
                sp->setSynaptogenesisRate(synRate);
                sp->setPruningRate(pruneRate);
            }
        }
        
        return AgentBrainResult<void>(true);
    } catch (const std::bad_alloc&) {
        return AgentBrainMemoryError("AgentBrainNeuromodulation", 
                                   "memory allocation failed in updateDevelopment");
    } catch (const std::exception& e) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR,
            "AgentBrainNeuromodulation", std::string("exception in updateDevelopment: ") + e.what()));
    }
}

AgentBrainResult<DevelopmentalStage> AgentBrainNeuromodulation::getDevelopmentalStage(const std::shared_ptr<Brain>& brain) {
    if (!validateNotNull(brain.get(), "brain pointer")) {
        return AgentBrainResult<DevelopmentalStage>(std::make_shared<AgentBrainInvalidInputError>(
            "AgentBrainNeuromodulation", "brain pointer cannot be null in getDevelopmentalStage"));
    }
    
    return AgentBrainResult<DevelopmentalStage>(brain->getDevelopmentalStage());
}

AgentBrainResult<void> AgentBrainNeuromodulation::reset() {
    try {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        
        return AgentBrainResult<void>(true);
    } catch (const std::exception& e) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR,
            "AgentBrainNeuromodulation", std::string("exception in reset: ") + e.what()));
    }
}

AgentBrainResult<void> AgentBrainNeuromodulation::updateCuriosity(float curiosityLevel) {
    try {
        if (!validateRange(curiosityLevel, 0.0f, 1.0f, "curiosity level")) {
            return AgentBrainInvalidInputError("AgentBrainNeuromodulation",
                                              "curiosity level " + std::to_string(curiosityLevel) + " is out of valid range [0.0, 1.0]");
        }
        
        curiosityLevel_ = curiosityLevel;
        return AgentBrainResult<void>(true);
    } catch (const std::exception& e) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR,
            "AgentBrainNeuromodulation", std::string("exception in updateCuriosity: ") + e.what()));
    }
}

AgentBrainResult<void> AgentBrainNeuromodulation::updateNovelty(float noveltyLevel) {
    try {
        if (!validateRange(noveltyLevel, 0.0f, 1.0f, "novelty level")) {
            return AgentBrainInvalidInputError("AgentBrainNeuromodulation",
                                              "novelty level " + std::to_string(noveltyLevel) + " is out of valid range [0.0, 1.0]");
        }
        
        noveltyLevel_ = noveltyLevel;
        return AgentBrainResult<void>(true);
    } catch (const std::exception& e) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR,
            "AgentBrainNeuromodulation", std::string("exception in updateNovelty: ") + e.what()));
    }
}

AgentBrainResult<void> AgentBrainNeuromodulation::updatePredictionError(float reward, float predictedReward) {
    try {
        if (!validateRange(reward, -1.0f, 1.0f, "reward")) {
            return AgentBrainInvalidInputError("AgentBrainNeuromodulation",
                                              "reward " + std::to_string(reward) + " is out of valid range [-1.0, 1.0]");
        }
        
        if (!validateRange(predictedReward, -1.0f, 1.0f, "predicted reward")) {
            return AgentBrainInvalidInputError("AgentBrainNeuromodulation",
                                              "predicted reward " + std::to_string(predictedReward) + " is out of valid range [-1.0, 1.0]");
        }
        
        predictionError_ = reward - predictedReward;
        return AgentBrainResult<void>(true);
    } catch (const std::exception& e) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR,
            "AgentBrainNeuromodulation", std::string("exception in updatePredictionError: ") + e.what()));
    }
}

} // namespace nlm