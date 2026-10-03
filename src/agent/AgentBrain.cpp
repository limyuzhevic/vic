#pragma once

#include "AgentBrain.hpp"
#include "AgentBrainMotor.hpp"
#include "AgentBrainSensory.hpp"
#include "AgentBrainNeuromodulation.hpp"
#include "AgentBrainConfig.hpp"
#include "AgentBrainError.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , config_(std::make_shared<AgentBrainConfig>())
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , errorRecoveryMode_(false)
    , motorComponent_(std::make_unique<AgentBrainMotor>())
    , sensoryComponent_(std::make_unique<AgentBrainSensory>())
    , neuromodulationComponent_(std::make_unique<AgentBrainNeuromodulation>())
{
    if (brain_) {
        try {
            auto motorResult = motorComponent_->distributeMotorNeurons(*brain_);
            if (!motorResult) {
                // Log motor error but continue (graceful degradation)
                NLM_LOG_WARN(std::string("Motor neuron distribution failed: ") + motorResult.error()->what());
                setErrorRecoveryMode(true);
            }
            
            auto sensoryResult = sensoryComponent_->distributeSensoryNeurons(*brain_);
            if (!sensoryResult) {
                // Log sensory error but continue (graceful degradation)
                NLM_LOG_WARN(std::string("Sensory neuron distribution failed: ") + sensoryResult.error()->what());
                setErrorRecoveryMode(true);
            }
        } catch (const std::bad_alloc&) {
            NLM_LOG_ERROR("Memory allocation failed in AgentBrain constructor");
            setErrorRecoveryMode(true);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in AgentBrain constructor: ") + e.what());
            setErrorRecoveryMode(true);
        }
    }
}

AgentBrain::~AgentBrain() = default;

AgentBrainResult<void> AgentBrain::initialize(const SimpleWorld& world) {
    try {
        if (!validateNotNull(&world, "world pointer")) {
            return AgentBrainInvalidInputError("AgentBrain",
                                              "world pointer cannot be null in initialize");
        }
        
        if (!validateBrainState("initialize")) {
            return AgentBrainResult<void>(std::make_shared<AgentBrainStateError>(
                "AgentBrain", "brain is in an invalid state for initialization"));
        }
        
        auto sensoryResult = sensoryComponent_->initialize(world);
        if (!sensoryResult) {
            NLM_LOG_ERROR(std::string("Sensory initialization failed: ") + sensoryResult.error()->what());
            return sensoryResult;
        }
        
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        NLM_LOG_INFO("AgentBrain initialized with " + 
                     std::to_string(sensoryComponent_->getVisionNeuronCount()) + " vision sensory neurons, " +
                     std::to_string(sensoryComponent_->getTouchNeuronCount()) + " touch sensory neurons, " +
                     std::to_string(sensoryComponent_->getInternalNeuronCount()) + " internal sensory neurons");
        
        return AgentBrainResult<void>(true);
    } catch (const std::bad_alloc&) {
        NLM_LOG_ERROR("Memory allocation failed in AgentBrain::initialize");
        return AgentBrainMemoryError("AgentBrain", "memory allocation failed in initialize");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AgentBrain::initialize: ") + e.what());
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR, "AgentBrain", std::string("exception in initialize: ") + e.what()));
    }
}

AgentBrainResult<size_t> AgentBrain::getSensoryInputSize() const {
    try {
        if (!validateBrainState("getSensoryInputSize")) {
            return AgentBrainResult<size_t>(std::make_shared<AgentBrainStateError>(
                "AgentBrain", "brain is in an invalid state for getSensoryInputSize"));
        }
        
        return sensoryComponent_->getSensoryInputSize();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AgentBrain::getSensoryInputSize: ") + e.what());
        return AgentBrainResult<size_t>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR, "AgentBrain", std::string("exception in getSensoryInputSize: ") + e.what()));
    }
}

AgentBrainResult<size_t> AgentBrain::getMotorOutputSize() const {
    try {
        if (!validateBrainState("getMotorOutputSize")) {
            return AgentBrainResult<size_t>(std::make_shared<AgentBrainStateError>(
                "AgentBrain", "brain is in an invalid state for getMotorOutputSize"));
        }
        
        return motorComponent_->getMotorOutputSize();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AgentBrain::getMotorOutputSize: ") + e.what());
        return AgentBrainResult<size_t>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR, "AgentBrain", std::string("exception in getMotorOutputSize: ") + e.what()));
    }
}

AgentBrainResult<void> AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    try {
        if (!validateNotNull(&percept, "percept pointer")) {
            return AgentBrainInvalidInputError("AgentBrain",
                                              "percept pointer cannot be null in processSensoryInput");
        }
        
        if (!validateBrainState("processSensoryInput")) {
            return AgentBrainResult<void>(std::make_shared<AgentBrainStateError>(
                "AgentBrain", "brain is in an invalid state for processSensoryInput"));
        }
        
        auto sensoryResult = sensoryComponent_->processInput(percept);
        if (!sensoryResult) {
            NLM_LOG_ERROR(std::string("Sensory processing failed: ") + sensoryResult.error()->what());
            return sensoryResult;
        }
        
        neuromodulationComponent_->updateCuriosity(*sensoryComponent_);
        neuromodulationComponent_->updateNovelty(*sensoryComponent_, previousVision_);
        
        return AgentBrainResult<void>(true);
    } catch (const std::bad_alloc&) {
        NLM_LOG_ERROR("Memory allocation failed in AgentBrain::processSensoryInput");
        return AgentBrainMemoryError("AgentBrain", "memory allocation failed in processSensoryInput");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AgentBrain::processSensoryInput: ") + e.what());
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR, "AgentBrain", std::string("exception in processSensoryInput: ") + e.what()));
    }
}

AgentBrainResult<MotorCommand> AgentBrain::decodeMotorCommand() {
    try {
        if (!validateBrainState("decodeMotorCommand")) {
            return AgentBrainResult<MotorCommand>(std::make_shared<AgentBrainStateError>(
                "AgentBrain", "brain is in an invalid state for decodeMotorCommand"));
        }
        
        MotorCommand decoded = MotorCommand::Wait;
        
        auto decodeResult = motorComponent_->decodeCommand(*brain_);
        if (!decodeResult) {
            NLM_LOG_WARN(std::string("Motor command decoding failed: ") + decodeResult.error()->what());
            // Graceful degradation: continue with default wait command
            decoded = MotorCommand::Wait;
        } else {
            decoded = decodeResult.value();
        }
        
        if (curiosityEnabled_ && neuromodulationComponent_->getCuriosityLevel() > 0.3f) {
            auto curiosityResult = motorComponent_->selectWithCuriosity(decoded, *brain_);
            if (!curiosityResult) {
                NLM_LOG_WARN(std::string("Curiosity-based command selection failed: ") + curiosityResult.error()->what());
            } else {
                decoded = curiosityResult.value();
            }
        }
        
        return AgentBrainResult<MotorCommand>(decoded);
    } catch (const std::bad_alloc&) {
        NLM_LOG_ERROR("Memory allocation failed in AgentBrain::decodeMotorCommand");
        return AgentBrainMemoryError<MotorCommand>("AgentBrain", "memory allocation failed in decodeMotorCommand");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AgentBrain::decodeMotorCommand: ") + e.what());
        return AgentBrainResult<MotorCommand>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR, "AgentBrain", std::string("exception in decodeMotorCommand: ") + e.what()));
    }
}

AgentBrainResult<void> AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    try {
        if (!validateBrainState("applyRewardModulation")) {
            return AgentBrainResult<void>(std::make_shared<AgentBrainStateError>(
                "AgentBrain", "brain is in an invalid state for applyRewardModulation"));
        }
        
        if (!validateRange(reward, -1.0f, 1.0f, "reward")) {
            return AgentBrainInvalidInputError("AgentBrain",
                                              "reward " + std::to_string(reward) + " is out of valid range [-1.0, 1.0]");
        }
        
        if (!validateRange(predictedReward, -1.0f, 1.0f, "predicted reward")) {
            return AgentBrainInvalidInputError("AgentBrain",
                                              "predicted reward " + std::to_string(predictedReward) + " is out of valid range [-1.0, 1.0]");
        }
        
        auto rewardResult = neuromodulationComponent_->applyReward(brain_, reward, predictedReward, plasticityModifier_);
        if (!rewardResult) {
            NLM_LOG_ERROR(std::string("Reward modulation failed: ") + rewardResult.error()->what());
            return rewardResult;
        }
        
        neuromodulationComponent_->updatePredictionError(reward, predictedReward);
        
        return AgentBrainResult<void>(true);
    } catch (const std::bad_alloc&) {
        NLM_LOG_ERROR("Memory allocation failed in AgentBrain::applyRewardModulation");
        return AgentBrainMemoryError("AgentBrain", "memory allocation failed in applyRewardModulation");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AgentBrain::applyRewardModulation: ") + e.what());
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR, "AgentBrain", std::string("exception in applyRewardModulation: ") + e.what()));
    }
}

AgentBrainResult<void> AgentBrain::updateDevelopment(double timestep) {
    try {
        if (!validateBrainState("updateDevelopment")) {
            return AgentBrainResult<void>(std::make_shared<AgentBrainStateError>(
                "AgentBrain", "brain is in an invalid state for updateDevelopment"));
        }
        
        if (!validateRange(timestep, 0.0, 3600.0, "timestep")) {
            return AgentBrainInvalidInputError("AgentBrain",
                                              "timestep " + std::to_string(timestep) + " is out of valid range [0.0, 3600.0]");
        }
        
        neuromodulationComponent_->updateDevelopment(brain_, timestep, developmentalAge_, plasticityModifier_);
        
        return AgentBrainResult<void>(true);
    } catch (const std::bad_alloc&) {
        NLM_LOG_ERROR("Memory allocation failed in AgentBrain::updateDevelopment");
        return AgentBrainMemoryError("AgentBrain", "memory allocation failed in updateDevelopment");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AgentBrain::updateDevelopment: ") + e.what());
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR, "AgentBrain", std::string("exception in updateDevelopment: ") + e.what()));
    }
}

AgentBrainResult<DevelopmentalStage> AgentBrain::getDevelopmentalStage() const {
    try {
        if (!validateBrainState("getDevelopmentalStage")) {
            return AgentBrainResult<DevelopmentalStage>(std::make_shared<AgentBrainStateError>(
                "AgentBrain", "brain is in an invalid state for getDevelopmentalStage"));
        }
        
        return neuromodulationComponent_->getDevelopmentalStage(brain_);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AgentBrain::getDevelopmentalStage: ") + e.what());
        return AgentBrainResult<DevelopmentalStage>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR, "AgentBrain", std::string("exception in getDevelopmentalStage: ") + e.what()));
    }
}

float AgentBrain::getNeuromodulationLevel() const {
    return neuromodulationComponent_->getDopamineLevel();
}

float AgentBrain::getCuriosityLevel() const {
    return neuromodulationComponent_->getCuriosityLevel();
}

float AgentBrain::getNoveltyLevel() const {
    return neuromodulationComponent_->getNoveltyLevel();
}

float AgentBrain::getPredictionError() const {
    return neuromodulationComponent_->getPredictionError();
}

AgentBrainResult<void> AgentBrain::reset() {
    try {
        if (!validateBrainState("reset")) {
            return AgentBrainResult<void>(std::make_shared<AgentBrainStateError>(
                "AgentBrain", "brain is in an invalid state for reset"));
        }
        
        neuromodulationComponent_->reset();
        motorComponent_->reset();
        sensoryComponent_->reset(previousVision_);
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        NLM_LOG_INFO("AgentBrain reset completed");
        
        return AgentBrainResult<void>(true);
    } catch (const std::bad_alloc&) {
        NLM_LOG_ERROR("Memory allocation failed in AgentBrain::reset");
        return AgentBrainMemoryError("AgentBrain", "memory allocation failed in reset");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AgentBrain::reset: ") + e.what());
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR, "AgentBrain", std::string("exception in reset: ") + e.what()));
    }
}

Brain* AgentBrain::getBrain() { return brain_.get(); }

void AgentBrain::enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
void AgentBrain::enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
void AgentBrain::enableDevelopment(bool enable) { developmentEnabled_ = enable; }
void AgentBrain::enableCuriosity(bool enable) { curiosityEnabled_ = enable; }

bool AgentBrain::isRewardModulationEnabled() const { return rewardModulationEnabled_; }
bool AgentBrain::isCuriosityEnabled() const { return curiosityEnabled_; }

AgentBrainResult<Brain*> AgentBrain::getBrain() {
    try {
        if (!validateBrainState("getBrain")) {
            return AgentBrainResult<Brain*>(std::make_shared<AgentBrainStateError>(
                "AgentBrain", "brain is in an invalid state for getBrain"));
        }
        
        return AgentBrainResult<Brain*>(brain_.get());
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AgentBrain::getBrain: ") + e.what());
        return AgentBrainResult<Brain*>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR, "AgentBrain", std::string("exception in getBrain: ") + e.what()));
    }
}

// Helper method implementations
bool AgentBrain::validateNotNull(const void* ptr, const std::string& context) const {
    if (!ptr) {
        NLM_LOG_WARN(std::string("Validation failed: ") + context + " is null");
        return false;
    }
    return true;
}

bool AgentBrain::validateIndex(size_t idx, size_t maxSize, const std::string& context) const {
    if (idx >= maxSize) {
        NLM_LOG_WARN(std::string("Validation failed: ") + context + 
                   " index " + std::to_string(idx) + " is out of bounds [0, " + std::to_string(maxSize) + "]");
        return false;
    }
    return true;
}

bool AgentBrain::validateRange(double value, double min, double max, const std::string& context) const {
    if (value < min || value > max) {
        NLM_LOG_WARN(std::string("Validation failed: ") + context + 
                   " value " + std::to_string(value) + " is out of range [" + std::to_string(min) + ", " + std::to_string(max) + "]");
        return false;
    }
    return true;
}

bool AgentBrain::validateRange(float value, float min, float max, const std::string& context) const {
    if (value < min || value > max) {
        NLM_LOG_WARN(std::string("Validation failed: ") + context + 
                   " value " + std::to_string(value) + " is out of range [" + std::to_string(min) + ", " + std::to_string(max) + "]");
        return false;
    }
    return true;
}

bool AgentBrain::validateBrainState(const std::string& operation) const {
    if (!brain_) {
        NLM_LOG_WARN(std::string("Validation failed: ") + operation + 
                   " cannot be performed with null brain");
        return false;
    }
    
    if (isErrorRecoveryMode()) {
        NLM_LOG_WARN(std::string("Validation failed: ") + operation + 
                   " cannot be performed in error recovery mode");
        return false;
    }
    
    return true;
}

// Other methods that need to be updated

float AgentBrain::getNeuromodulationLevel() const {
    return neuromodulationComponent_->getDopamineLevel();
}

float AgentBrain::getCuriosityLevel() const {
    return neuromodulationComponent_->getCuriosityLevel();
}

float AgentBrain::getNoveltyLevel() const {
    return neuromodulationComponent_->getNoveltyLevel();
}

float AgentBrain::getPredictionError() const {
    return neuromodulationComponent_->getPredictionError();
}

void AgentBrain::enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
void AgentBrain::enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
void AgentBrain::enableDevelopment(bool enable) { developmentEnabled_ = enable; }
void AgentBrain::enableCuriosity(bool enable) { curiosityEnabled_ = enable; }

bool AgentBrain::isRewardModulationEnabled() const { return rewardModulationEnabled_; }
bool AgentBrain::isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
bool AgentBrain::isDevelopmentEnabled() const { return developmentEnabled_; }
bool AgentBrain::isCuriosityEnabled() const { return curiosityEnabled_; }

} // namespace nlm