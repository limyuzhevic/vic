#pragma once

#include "AgentBrainComponents.hpp"
#include "AgentBrainConfig.hpp"
#include "AgentBrainError.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

AgentBrainSensory::AgentBrainSensory() : noveltyDecay_(0.99f), noveltyLevel_(0.0f), config_(std::make_shared<AgentBrainConfig>()) {}

AgentBrainResult<void> AgentBrainSensory::distributeSensoryNeurons(const Brain& brain) {
    if (!validateNotNull(&brain, "brain pointer")) {
        return AgentBrainInvalidInputError("AgentBrainSensory",
                                          "brain pointer cannot be null in distributeSensoryNeurons");
    }
    
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    if (!brain.getRegions()) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainNeuralError>(
            "AgentBrainSensory", "brain has no regions in distributeSensoryNeurons"));
    }
    
    try {
        for (const auto& region : brain.getRegions()) {
            if (!validateNotNull(region, "region pointer")) {
                continue; // Graceful degradation: skip invalid region
            }
            
            if (!region->getPopulations()) continue;
            
            for (auto& pop : region->getPopulations()) {
                if (!validateNotNull(pop, "population pointer")) continue;
                
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Sensory) {
                    const auto& neurons = pop->getNeurons();
                    if (!validateNotNull(&neurons, "neurons vector")) continue;
                    
                    for (Neuron* n : neurons) {
                        if (!validateNotNull(n, "neuron pointer")) continue;
                        
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
        return AgentBrainResult<void>(true);
    } catch (const std::bad_alloc&) {
        return AgentBrainMemoryError("AgentBrainSensory", 
                                   "memory allocation failed in distributeSensoryNeurons");
    } catch (const std::exception& e) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainNeuralError>(
            "AgentBrainSensory", std::string("exception in distributeSensoryNeurons: ") + e.what()));
    }
}

AgentBrainResult<void> AgentBrainSensory::initialize(const SimpleWorld& world) {
    try {
        if (!validateNotNull(&world, "world pointer")) {
            return AgentBrainInvalidInputError("AgentBrainSensory",
                                              "world pointer cannot be null in initialize");
        }
        
        if (!validateNotNull(config_.get(), "config pointer")) {
            return AgentBrainInvalidInputError("AgentBrainSensory",
                                              "config pointer cannot be null in initialize");
        }
        
        // Vision processing dimensions (derived from world)
        // Note: world.getVisionWidth() and world.getVisionHeight() would be called here
        
        // Initialize novelty decay from config
        try {
            noveltyDecay_ = config_->getParameter<float>(AgentBrainConfig::ParameterID::NOVELTY_DECAY_RATE);
        } catch (const std::exception& e) {
            return AgentBrainResult<void>(std::make_shared<AgentBrainConfigurationError>(
                "AgentBrainSensory", std::string("failed to get novelty decay from config: ") + e.what()));
        }
        
        return AgentBrainResult<void>(true);
    } catch (const std::bad_alloc&) {
        return AgentBrainMemoryError("AgentBrainSensory", 
                                   "memory allocation failed in initialize");
    } catch (const std::exception& e) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR,
            "AgentBrainSensory", std::string("exception in initialize: ") + e.what()));
    }
}

AgentBrainResult<void> AgentBrainSensory::processInput(const SensoryPercept& percept) {
    try {
        if (!validateNotNull(&percept, "percept pointer")) {
            return AgentBrainInvalidInputError("AgentBrainSensory",
                                              "percept pointer cannot be null in processInput");
        }
        
        if (!validateNotNull(config_.get(), "config pointer")) {
            return AgentBrainInvalidInputError("AgentBrainSensory",
                                              "config pointer cannot be null in processInput");
        }
        
        if (sensoryVision_.empty() && sensoryTouch_.empty() && 
            sensoryInternal_.empty() && sensoryProprioception_.empty()) {
            return AgentBrainResult<void>(true); // Early return if no neurons to process
        }
        
        // Vision input (256 values -> sensoryVision_ neurons) with config scaling
        const auto& vision = percept.getVision();
        float visionScale = config_->getParameter<float>(AgentBrainConfig::ParameterID::SENSORY_VISION_SCALE);
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (!validateNotNull(sensoryVision_[i], "vision neuron pointer")) continue;
            
            // Inject current proportional to vision intensity (scaled by config)
            float current = vision[i] * visionScale;
            try {
                sensoryVision_[i]->injectCurrent(current);
            } catch (const std::exception& e) {
                // Graceful degradation: skip neuron if injection fails
                continue;
            }
        }
        
        // Touch input (8 values -> sensoryTouch_ neurons) with config scaling
        const auto& touch = percept.getTouch();
        float touchScale = config_->getParameter<float>(AgentBrainConfig::ParameterID::SENSORY_TOUCH_SCALE);
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (!validateNotNull(sensoryTouch_[i], "touch neuron pointer")) continue;
            
            float current = touch[i] * touchScale;  // Collision signal
            try {
                sensoryTouch_[i]->injectCurrent(current);
            } catch (const std::exception& e) {
                continue;
            }
        }
        
        // Internal signals (4 values -> sensoryInternal_ neurons) with config scaling
        const auto& intern = percept.getInternal();
        float internalScale = config_->getParameter<float>(AgentBrainConfig::ParameterID::SENSORY_INTERNAL_SCALE);
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (!validateNotNull(sensoryInternal_[i], "internal neuron pointer")) continue;
            
            float current = (intern[i] * 2.0f - 1.0f) * internalScale;  // Center and scale
            try {
                sensoryInternal_[i]->injectCurrent(current);
            } catch (const std::exception& e) {
                continue;
            }
        }
        
        // Proprioception (6 values -> sensoryProprioception_ neurons) with config scaling
        const auto& proprio = percept.getProprioception();
        float proprioScale = config_->getParameter<float>(AgentBrainConfig::ParameterID::SENSORY_PROPRIOCEPTION_SCALE);
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (!validateNotNull(sensoryProprioception_[i], "proprioception neuron pointer")) continue;
            
            float current = (proprio[i] * 2.0f - 1.0f) * proprioScale;  // Center and scale
            try {
                sensoryProprioception_[i]->injectCurrent(current);
            } catch (const std::exception& e) {
                continue;
            }
        }
        
        return AgentBrainResult<void>(true);
    } catch (const std::bad_alloc&) {
        return AgentBrainMemoryError("AgentBrainSensory", 
                                   "memory allocation failed in processInput");
    } catch (const std::exception& e) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR,
            "AgentBrainSensory", std::string("exception in processInput: ") + e.what()));
    }
}

AgentBrainResult<void> AgentBrainSensory::updateNovelty(const std::vector<float>& currentVision, std::vector<float>& previousVision) {
    try {
        if (currentVision.empty()) {
            return AgentBrainResult<void>(true); // Early return if no data
        }
        
        if (!validateNotNull(&previousVision, "previousVision pointer")) {
            return AgentBrainInvalidInputError("AgentBrainSensory",
                                              "previousVision pointer cannot be null in updateNovelty");
        }
        
        float totalDiff = 0.0f;
        for (size_t i = 0; i < currentVision.size() && i < previousVision.size(); ++i) {
            float diff = std::abs(currentVision[i] - previousVision[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(currentVision.size(), 1);
        
        // Decay with config decay rate
        noveltyLevel_ *= noveltyDecay_;
        
        // Apply novelty sensitivity
        float noveltySensitivity = config_->getParameter<float>(AgentBrainConfig::ParameterID::NOVELTY_SENSITIVITY);
        noveltyLevel_ *= noveltySensitivity;
        
        // Store for next time
        previousVision = currentVision;
        
        return AgentBrainResult<void>(true);
    } catch (const std::bad_alloc&) {
        return AgentBrainMemoryError("AgentBrainSensory", 
                                   "memory allocation failed in updateNovelty");
    } catch (const std::exception& e) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR,
            "AgentBrainSensory", std::string("exception in updateNovelty: ") + e.what()));
    }
}

AgentBrainResult<void> AgentBrainSensory::updateCuriosity(float novelty, float predictionError, float& curiosityLevel) {
    try {
        if (!validateNotNull(&curiosityLevel, "curiosityLevel pointer")) {
            return AgentBrainInvalidInputError("AgentBrainSensory",
                                              "curiosityLevel pointer cannot be null in updateCuriosity");
        }
        
        float curiositySensitivity = config_->getParameter<float>(AgentBrainConfig::ParameterID::CURIOSITY_SENSITIVITY);
        float predictionErrorWeight = config_->getParameter<float>(AgentBrainConfig::ParameterID::PREDICTION_ERROR_WEIGHT);
        
        curiosityLevel = novelty * curiositySensitivity + std::abs(predictionError) * predictionErrorWeight;
        curiosityLevel = std::clamp(curiosityLevel, 0.0f, 1.0f);
        
        return AgentBrainResult<void>(true);
    } catch (const std::bad_alloc&) {
        return AgentBrainMemoryError("AgentBrainSensory", 
                                   "memory allocation failed in updateCuriosity");
    } catch (const std::exception& e) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR,
            "AgentBrainSensory", std::string("exception in updateCuriosity: ") + e.what()));
    }
}

void AgentBrainSensory::reset(std::vector<float>& previousVision) {
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    std::fill(previousVision.begin(), previousVision.end(), 0.0f);
    noveltyLevel_ = 0.0f;
}

AgentBrainResult<size_t> AgentBrainSensory::getSensoryInputSize() const {
    try {
        if (!validateNotNull(config_.get(), "config pointer")) {
            return AgentBrainResult<size_t>(size_t(0)); // Default return
        }
        
        try {
            // For now, return a default value based on sensory neurons
            // In a real implementation, this would match the brain's expected input size
            return AgentBrainResult<size_t>(size_t(256 + 8 + 4 + 6)); // vision + touch + internal + proprioception
        } catch (const std::exception& e) {
            return AgentBrainResult<size_t>(std::make_shared<AgentBrainConfigurationError>(
                "AgentBrainSensory", std::string("error in getSensoryInputSize: ") + e.what()));
        }
    } catch (const std::exception& e) {
        return AgentBrainResult<size_t>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR,
            "AgentBrainSensory", std::string("error in getSensoryInputSize: ") + e.what()));
    }
}

AgentBrainResult<void> AgentBrainSensory::reset(std::vector<float>& previousVision) {
    try {
        if (!validateNotNull(&previousVision, "previousVision pointer")) {
            return AgentBrainInvalidInputError("AgentBrainSensory",
                                              "previousVision pointer cannot be null in reset");
        }
        
        sensoryVision_.clear();
        sensoryTouch_.clear();
        sensoryInternal_.clear();
        sensoryProprioception_.clear();
        std::fill(previousVision.begin(), previousVision.end(), 0.0f);
        noveltyLevel_ = 0.0f;
        
        return AgentBrainResult<void>(true);
    } catch (const std::exception& e) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR,
            "AgentBrainSensory", std::string("exception in reset: ") + e.what()));
    }
}

} // namespace nlm