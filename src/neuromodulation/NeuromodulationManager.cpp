#include "NeuromodulationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct NeuromodulationManager::Impl {
    Brain* brain;
    
    // Neuromodulator references for quick access
    Dopamine* dopamine;
    Curiosity* curiosity;
    Novelty* novelty;
    PredictionError* predictionError;
    Acetylcholine* acetylcholine;
    Norepinephrine* norepinephrine;
    Serotonin* serotonin;
    
    // Current levels
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionErrorLevel;
    float acetylcholineLevel;
    float norepinephrineLevel;
    float serotoninLevel;
    
    // Configuration
    float dopamineLearningRate;
    float curiosityScaling;
    float noveltyThreshold;
    float predictionErrorDecay;
    
    // Performance metrics
    std::vector<float> plasticityFactors;
    size_t totalEvents;
    
    // Statistics
    size_t totalObservationUpdates;
    size_t totalRewardSignals;
    size_t totalErrorSignals;
    
    Impl()
        : brain(nullptr)
        , dopamine(nullptr)
        , curiosity(nullptr)
        , novelty(nullptr)
        , predictionError(nullptr)
        , acetylcholine(nullptr)
        , norepinephrine(nullptr)
        , serotonin(nullptr)
        , dopamineLevel(0.0f)
        , curiosityLevel(0.0f)
        , noveltyLevel(0.0f)
        , predictionErrorLevel(0.0f)
        , acetylcholineLevel(0.0f)
        , norepinephrineLevel(0.0f)
        , serotoninLevel(0.0f)
        , dopamineLearningRate(0.01f)
        , curiosityScaling(0.5f)
        , noveltyThreshold(0.1f)
        , predictionErrorDecay(0.99f)
        , totalEvents(0)
        , totalObservationUpdates(0)
        , totalRewardSignals(0)
        , totalErrorSignals(0)
    {
        plasticityFactors.reserve(1000);
    }
};

NeuromodulationManager::NeuromodulationManager()
    : pImpl(std::make_unique<Impl>())
    , brain_(nullptr)
    , dopamineLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , predictionErrorLevel_(0.0f)
    , acetylcholineLevel_(0.0f)
    , norepinephrineLevel_(0.0f)
    , serotoninLevel_(0.0f)
    , dopamineLearningRate_(0.01f)
    , curiosityScaling_(0.5f)
    , noveltyThreshold_(0.1f)
    , predictionErrorDecay_(0.99f)
    , averagePlasticityFactor_(1.0f)
    , totalNeuromodulationEvents_(0)
{
    // Create neuromodulator instances
    dopamine_ = std::make_unique<Dopamine>();
    curiosity_ = std::make_unique<Curiosity>();
    novelty_ = std::make_unique<Novelty>();
    predictionError_ = std::make_unique<PredictionError>();
    acetylcholine_ = std::make_unique<Acetylcholine>();
    norepinephrine_ = std::make_unique<Norepinephrine>();
    serotonin_ = std::make_unique<Serotonin>();
    
    // Initialize neuromodulator names for lookup
    neuromodulatorNames_ = {"Dopamine", "Curiosity", "Novelty", "PredictionError", "Acetylcholine", "Norepinephrine", "Serotonin"};
    
    NLM_LOG_INFO("NeuromodulationManager created with " + std::to_string(neuromulatorNames_.size()) + " neuromodulators");
}

NeuromodulationManager::~NeuromodulationManager() {
    NLM_LOG_INFO("NeuromodulationManager destroyed");
}

NeuromodulationManager::NeuromodulationManager(NeuromodulationManager&& other) noexcept
    : pImpl(std::move(other.pImpl))
    , brain_(std::move(other.brain_))
    , dopamine_(std::move(other.dopamine_))
    , curiosity_(std::move(other.curiosity_))
    , novelty_(std::move(other.novelty_))
    , predictionError_(std::move(other.predictionError_))
    , acetylcholine_(std::move(other.acetylcholine_))
    , norepinephrine_(std::move(other.norepinephrine_))
    , serotonin_(std::move(other.serotonin_))
    , dopamineLevel_(std::move(other.dopamineLevel_))
    , curiosityLevel_(std::move(other.curiosityLevel_))
    , noveltyLevel_(std::move(other.noveltyLevel_))
    , predictionErrorLevel_(std::move(other.predictionErrorLevel_))
    , acetylcholineLevel_(std::move(other.acetylcholineLevel_))
    , norepinephrineLevel_(std::move(other.norepinephrineLevel_))
    , serotoninLevel_(std::move(other.serotoninLevel_))
    , dopamineLearningRate_(std::move(other.dopamineLearningRate_))
    , curiosityScaling_(std::move(other.curiosityScaling_))
    , noveltyThreshold_(std::move(other.noveltyThreshold_))
    , predictionErrorDecay_(std::move(other.predictionErrorDecay_))
    , averagePlasticityFactor_(std::move(other.averagePlasticityFactor_))
    , totalNeuromodulationEvents_(std::move(other.totalNeuromodulationEvents_))
    , observers_(std::move(other.observers_))
    , neuromodulatorNames_(std::move(other.neuromodulatorNames_))
{
}

NeuromodulationManager& NeuromodulationManager::operator=(NeuromodulationManager&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        brain_ = std::move(other.brain_);
        dopamine_ = std::move(other.dopamine_);
        curiosity_ = std::move(other.curiosity_);
        novelty_ = std::move(other.novelty_);
        predictionError_ = std::move(other.predictionError_);
        acetylcholine_ = std::move(other.acetylcholine_);
        norepinephrine_ = std::move(other.norepinephrine_);
        serotonin_ = std::move(other.serotonin_);
        dopamineLevel_ = std::move(other.dopamineLevel_);
        curiosityLevel_ = std::move(other.curiosityLevel_);
        noveltyLevel_ = std::move(other.noveltyLevel_);
        predictionErrorLevel_ = std::move(other.predictionErrorLevel_);
        acetylcholineLevel_ = std::move(other.acetylcholineLevel_);
        norepinephrineLevel_ = std::move(other.norepinephrineLevel_);
        serotoninLevel_ = std::move(other.serotoninLevel_);
        dopamineLearningRate_ = std::move(other.dopamineLearningRate_);
        curiosityScaling_ = std::move(other.curiosityScaling_);
        noveltyThreshold_ = std::move(other.noveltyThreshold_);
        predictionErrorDecay_ = std::move(other.predictionErrorDecay_);
        averagePlasticityFactor_ = std::move(other.averagePlasticityFactor_);
        totalNeuromodulationEvents_ = std::move(other.totalNeuromodulationEvents_);
        observers_ = std::move(other.observers_);
        neuromodulatorNames_ = std::move(other.neuromodulatorNames_);
    }
    return *this;
}

void NeuromodulationManager::initialize(Brain* brain) {
    brain_ = brain;
    
    // Initialize neuromodulator implementations
    if (dopamine_) {
        // In a real implementation, we would set up the dopamine implementation
        // For now, it's a placeholder
        NLM_LOG_DEBUG("Dopamine neuromodulator initialized");
    }
    
    if (curiosity_) {
        // In a real implementation, we would set up the curiosity implementation
        NLM_LOG_DEBUG("Curiosity neuromodulator initialized");
    }
    
    if (novelty_) {
        // In a real implementation, we would set up the novelty implementation
        NLM_LOG_DEBUG("Novelty neuromodulator initialized");
    }
    
    if (predictionError_) {
        // In a real implementation, we would set up the prediction error implementation
        NLM_LOG_DEBUG("PredictionError neuromodulator initialized");
    }
    
    NLM_LOG_INFO("NeuromodulationManager initialized for brain");
}

void NeuromodulationManager::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Get current neuromodulator levels from brain
    dopamineLevel_ = brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f;
    curiosityLevel_ = brain_->getCuriosity() ? brain_->getCuriosity()->getLevel() : 0.0f;
    noveltyLevel_ = brain_->getNovelty() ? brain_->getNovelty()->getLevel() : 0.0f;
    predictionErrorLevel_ = brain_->getPredictionErrorSignal() ? brain_->getPredictionErrorSignal()->getError() : 0.0f;
    
    // Apply configuration scaling
    float plasticityFactor = 1.0f;
    
    if (dopamine_) {
        // Apply dopamine learning rate
        float dopamineEffect = dopamineLevel_ * dopamineLearningRate_;
        dopamineEffect = std::min(dopamineEffect, 1.0f); // Cap at 1.0
        plasticityFactor *= (1.0f + dopamineEffect);
        NLM_LOG_DEBUG("Dopamine effect: " + std::to_string(dopamineEffect) + " on plasticity");
    }
    
    if (curiosity_) {
        // Apply curiosity scaling
        float curiosityEffect = curiosityLevel_ * curiosityScaling_;
        curiosityEffect = std::min(curiosityEffect, 1.0f); // Cap at 1.0
        plasticityFactor *= (1.0f + curiosityEffect * 0.5f); // Curioisty has moderate effect
        NLM_LOG_DEBUG("Curiosity effect: " + std::to_string(curiosityEffect) + " on exploration");
    }
    
    if (novelty_) {
        // Apply novelty threshold check
        if (noveltyLevel_ > noveltyThreshold_) {
            // Novelty detected
            float noveltyEffect = (noveltyLevel_ - noveltyThreshold_) / (1.0f - noveltyThreshold_);
            noveltyEffect = std::min(noveltyEffect, 1.0f);
            plasticityFactor *= (1.0f + noveltyEffect * 0.3f); // Novelty has smaller effect
            NLM_LOG_DEBUG("Novelty detected: " + std::to_string(noveltyLevel_) + ", effect: " + std::to_string(noveltyEffect));
        }
    }
    
    if (predictionError_) {
        // Apply prediction error decay
        predictionErrorLevel_ *= predictionErrorDecay_;
        float errorEffect = predictionErrorLevel_ * 0.5f; // Prediction error affects learning moderately
        plasticityFactor *= (1.0f + errorEffect);
        NLM_LOG_DEBUG("Prediction error effect: " + std::to_string(errorEffect) + " on plasticity");
    }
    
    // Cap plasticity factor
    plasticityFactor = std::min(plasticityFactor, 2.0f);
    
    // Update performance metrics
    pImpl->plasticityFactors.push_back(plasticityFactor);
    if (pImpl->plasticityFactors.size() > 100) {
        pImpl->plasticityFactors.erase(pImpl->plasticityFactors.begin());
    }
    
    // Calculate average plasticity factor
    if (!pImpl->plasticityFactors.empty()) {
        float sum = std::accumulate(pImpl->plasticityFactors.begin(), pImpl->plasticityFactors.end(), 0.0f);
        averagePlasticityFactor_ = sum / static_cast<float>(pImpl->plasticityFactors.size());
    }
    
    // Notify observers
    for (const auto& observer : observers_) {
        observer("Dopamine", dopamineLevel_);
        observer("Curiosity", curiosityLevel_);
        observer("Novelty", noveltyLevel_);
        observer("PredictionError", predictionErrorLevel_);
    }
    
    totalNeuromodulationEvents_++;
    pImpl->totalEvents++;
    
    NLM_LOG_DEBUG("Neuromodulation update complete, plasticity factor: " + std::to_string(plasticityFactor));
}

Neuromodulator* NeuromodulationManager::getNeuromodulator(const std::string& name) {
    // Return appropriate neuromodulator based on name
    if (name == "Dopamine" || name == "dopamine") {
        return dopamine_.get();
    } else if (name == "Curiosity" || name == "curiosity") {
        return curiosity_.get();
    } else if (name == "Novelty" || name == "novelty") {
        return novelty_.get();
    } else if (name == "PredictionError" || name == "prediction_error" || name == "predictionError") {
        return predictionError_.get();
    } else if (name == "Acetylcholine" || name == "acetylcholine") {
        return acetylcholine_.get();
    } else if (name == "Norepinephrine" || name == "norepinephrine") {
        return norepinephrine_.get();
    } else if (name == "Serotonin" || name == "serotonin") {
        return serotonin_.get();
    }
    
    return nullptr;
}

const Neuromodulator* NeuromodulationManager::getNeuromodulator(const std::string& name) const {
    // Same logic as non-const version
    if (name == "Dopamine" || name == "dopamine") {
        return dopamine_.get();
    } else if (name == "Curiosity" || name == "curiosity") {
        return curiosity_.get();
    } else if (name == "Novelty" || name == "novelty") {
        return novelty_.get();
    } else if (name == "PredictionError" || name == "prediction_error" || name == "predictionError") {
        return predictionError_.get();
    } else if (name == "Acetylcholine" || name == "acetylcholine") {
        return acetylcholine_.get();
    } else if (name == "Norepinephrine" || name == "norepinephrine") {
        return norepinephrine_.get();
    } else if (name == "Serotonin" || name == "serotonin") {
        return serotonin_.get();
    }
    
    return nullptr;
}

void NeuromodulationManager::detectNovelty(const std::vector<float>& currentState) {
    if (!novelty_) return;
    
    // In a real implementation, this would detect novelty by comparing current state
    // with recent states
    float noveltyScore = 0.0f;
    
    // Simple novelty detection (placeholder)
    if (!currentState.empty()) {
        float sum = std::accumulate(currentState.begin(), currentState.end(), 0.0f);
        float average = sum / static_cast<float>(currentState.size());
        float variance = 0.0f;
        for (float value : currentState) {
            variance += (value - average) * (value - average);
        }
        variance /= static_cast<float>(currentState.size());
        noveltyScore = std::sqrt(variance); // Variance as novelty measure
    }
    
    // Update novelty detection based on variance
    if (noveltyScore > noveltyThreshold_) {
        // Novelty detected - update novelty system
        NLM_LOG_DEBUG("Novelty detected: score=" + std::to_string(noveltyScore));
    }
    
    pImpl->totalObservationUpdates++;
}

void NeuromodulationManager::updateEnvironmentComplexity(float complexity) {
    // Update environment complexity for neuromodulators
    // This could affect how curiosity and other neuromodulators respond
    NLM_LOG_DEBUG("Environment complexity updated: " + std::to_string(complexity));
}

void NeuromodulationManager::resetMetrics() {
    averagePlasticityFactor_ = 1.0f;
    totalNeuromodulationEvents_ = 0;
    pImpl->plasticityFactors.clear();
    pImpl->totalEvents = 0;
    pImpl->totalObservationUpdates = 0;
    pImpl->totalRewardSignals = 0;
    pImpl->totalErrorSignals = 0;
    
    NLM_LOG_INFO("NeuromodulationManager metrics reset");
}

} // namespace nlm
