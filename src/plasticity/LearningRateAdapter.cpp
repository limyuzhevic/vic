#include "LearningRateAdapter.hpp"
#include "../../core/Config/Config.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <sstream>

namespace nlm {

void LearningRateAdapter::adaptWithRMSprop(Impl::SynapseData& data, TimestepDuration dt) {
    if (!data.weightHistory.empty()) {
        float gradient = data.averageWeightChange;
        
        data.rms = 0.75f * data.rms + 0.25f * gradient * gradient;
        float rms = std::sqrt(data.rms + 1e-8f);
        
        float adaptiveRate = pImpl->baseLearningRate / (rms + 1e-8f);
        
        data.currentLearningRate = data.currentLearningRate + 
                                   pImpl->adaptationRate * (adaptiveRate - data.currentLearningRate);
    }
}

void LearningRateAdapter::adaptWithAdam(Impl::SynapseData& data, TimestepDuration dt) {
    if (!data.weightHistory.empty()) {
        float gradient = data.averageWeightChange;
        
        data.momentum = 0.9f * data.momentum + 0.1f * gradient;
        data.variance = 0.999f * data.variance + 0.001f * gradient * gradient;
        
        float biasCorrection = 1.0f - std::pow(0.9f, static_cast<float>(data.weightHistory.size()));
        float mHat = data.momentum / (1.0f - std::pow(0.9f, static_cast<float>(data.weightHistory.size())));
        float vHat = data.variance / (1.0f - std::pow(0.999f, static_cast<float>(data.weightHistory.size())));
        
        float epsilon = 1e-8f;
        float adaptiveRate = pImpl->baseLearningRate * mHat / (std::sqrt(vHat) + epsilon);
        
        adaptiveRate = std::clamp(adaptiveRate, pImpl->minLearningRate, pImpl->maxLearningRate);
        data.currentLearningRate = data.currentLearningRate + 
                                   pImpl->adaptationRate * (adaptiveRate - data.currentLearningRate);
    }
}

void LearningRateAdapter::adaptWithHomeostatic(Impl::SynapseData& data) {
    float currentRate = data.currentLearningRate;
    
    float weightChangeFactor = std::min(2.0f, 1.0f + data.averageWeightChange);
    
    float stabilityFactor = 1.0f - std::min(0.9f, data.stabilityMeasure / pImpl->stabilityThreshold);
    
    float performanceFactor = 1.0f + 0.5f * data.performanceMetric;
    
    float rateLimitFactor = 1.0f;
    if (currentRate > pImpl->maxLearningRate * 0.5f) {
        rateLimitFactor = std::max(pImpl->minLearningRate / currentRate, 0.1f);
    }
    
    float newRate = currentRate * 
                   weightChangeFactor * 
                   stabilityFactor * 
                   performanceFactor * 
                   rateLimitFactor;
    
    newRate = std::clamp(newRate, pImpl->minLearningRate, pImpl->maxLearningRate);
    data.currentLearningRate = data.currentLearningRate + 
                               pImpl->adaptationRate * (newRate - data.currentLearningRate);
}

bool LearningRateAdapter::initializeFromConfig(const Config& config) {
    bool success = true;
    
    auto enabledOpt = config.get<bool>("learning_rate_adaptation_enabled");
    if (enabledOpt.has_value()) {
        pImpl->enabled = enabledOpt.value();
    } else {
        pImpl->enabled = false;
        success = false;
    }
    
    auto algorithmOpt = config.get<std::string>("adaptation_algorithm");
    if (algorithmOpt.has_value()) {
        std::string algo = algorithmOpt.value();
        if (algo == "rmsprop") {
            pImpl->algorithm = AdaptationAlgorithm::RMSprop;
        } else if (algo == "adam") {
            pImpl->algorithm = AdaptationAlgorithm::Adam;
        } else if (algo == "homeostatic") {
            pImpl->algorithm = AdaptationAlgorithm::Homeostatic;
        } else if (algo == "fixed") {
            pImpl->algorithm = AdaptationAlgorithm::Fixed;
        } else {
            pImpl->algorithm = AdaptationAlgorithm::Homeostatic;
            success = false;
        }
    }
    
    auto adaptationRateOpt = config.get<double>("adaptation_rate");
    if (adaptationRateOpt.has_value()) {
        pImpl->adaptationRate = static_cast<float>(adaptationRateOpt.value());
    }
    
    auto stabilityThresholdOpt = config.get<double>("stability_threshold");
    if (stabilityThresholdOpt.has_value()) {
        pImpl->stabilityThreshold = static_cast<float>(stabilityThresholdOpt.value());
    }
    
    auto maxLearningRateOpt = config.get<double>("max_learning_rate");
    if (maxLearningRateOpt.has_value()) {
        pImpl->maxLearningRate = static_cast<float>(maxLearningRateOpt.value());
    }
    
    auto minLearningRateOpt = config.get<double>("min_learning_rate");
    if (minLearningRateOpt.has_value()) {
        pImpl->minLearningRate = static_cast<float>(minLearningRateOpt.value());
    }
    
    auto baseLearningRateOpt = config.get<double>("base_learning_rate");
    if (baseLearningRateOpt.has_value()) {
        pImpl->baseLearningRate = static_cast<float>(baseLearningRateOpt.value());
    }
    
    pImpl->adaptationRate = std::clamp(pImpl->adaptationRate, 0.001f, 1.0f);
    pImpl->stabilityThreshold = std::clamp(pImpl->stabilityThreshold, 0.0f, 1.0f);
    pImpl->maxLearningRate = std::clamp(pImpl->maxLearningRate, pImpl->minLearningRate + 0.001f, 1.0f);
    pImpl->minLearningRate = std::clamp(pImpl->minLearningRate, 0.0001f, pImpl->maxLearningRate);
    pImpl->baseLearningRate = std::clamp(pImpl->baseLearningRate, pImpl->minLearningRate, pImpl->maxLearningRate);
    
    return success;
}

bool LearningRateAdapter::saveToConfig(Config& config) const {
    bool success = true;
    
    config.set("learning_rate_adaptation_enabled", pImpl->enabled);
    
    const char* algoName = getAlgorithmName();
    std::string algoStr(algoName);
    config.set("adaptation_algorithm", algoStr);
    
    config.set("adaptation_rate", static_cast<double>(pImpl->adaptationRate));
    config.set("stability_threshold", static_cast<double>(pImpl->stabilityThreshold));
    config.set("max_learning_rate", static_cast<double>(pImpl->maxLearningRate));
    config.set("min_learning_rate", static_cast<double>(pImpl->minLearningRate));
    config.set("base_learning_rate", static_cast<double>(pImpl->baseLearningRate));
    
    return success;
}

} // namespace nlm
