#include "AcetylCholine.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

struct AcetylCholine::Impl {
    float encodingRate;
    float consolidationStrength;
    float interferenceReduction;
    float workingMemoryLoad;
    float workingMemoryStability;
    
    // Memory traces
    std::vector<float> recentPattern;
    std::vector<std::pair<SimulationStep, std::vector<float>>> memoryTraces;
    
    // Focus mechanisms
    std::vector<std::pair<RegionId, float>> memoryFocusRegions;
    std::vector<std::pair<RegionId, float>> attentionalSpotlights;
    
    // Physiological parameters
    float encodingSensitivity;
    float consolidationThreshold;
    float interferenceThreshold;
    
    // Adaptive mechanisms
    float adaptationRate;
    float fatigueRate;
    
    // Decay and recovery
    float encodingDecayRate;
    float consolidationDecayRate;
    
    Impl() : encodingRate(0.0f)
        , consolidationStrength(1.0f)
        , interferenceReduction(1.0f)
        , workingMemoryLoad(0.0f)
        , workingMemoryStability(1.0f)
        , encodingSensitivity(0.8f)
        , consolidationThreshold(0.5f)
        , interferenceThreshold(0.3f)
        , adaptationRate(0.03f)
        , fatigueRate(0.02f)
        , encodingDecayRate(0.05f)
        , consolidationDecayRate(0.01f) {}
};

AcetylCholine::AcetylCholine() : pImpl(new Impl)
    , brain_(nullptr)
    , currentLevel_(0.0f)
    , memoryEncodingRate_(0.0f)
    , consolidationStrength_(1.0f)
    , interferenceReduction_(1.0f)
    , workingMemoryLoad_(0.0f)
    , workingMemoryStability_(1.0f)
{
}

AcetylCholine::~AcetylCholine() = default;

void AcetylCholine::initialize(Brain* brain) {
    brain_ = brain;
    recentPattern_.clear();
    memoryTraces_.clear();
    memoryFocusRegions_.clear();
    attentionalSpotlights_.clear();
    
    NLM_LOG_INFO("Acetylcholine (ACh) initialized - working memory and attention system ready");
}

float AcetylCholine::getLevel() const {
    return currentLevel_;
}

void AcetylCholine::setLevel(float level) {
    currentLevel_ = std::clamp(level, 0.0f, 1.0f);
}

float AcetylCholine::getWorkingMemoryEnhancement() const {
    // ACh enhances working memory proportionally to level with saturation
    return 1.0f + currentLevel_ * 1.5f;  // 1.0x to 2.5x enhancement
}

float AcetylCholine::getAttentionClarity() const {
    // ACh improves attention clarity and reduces noise
    return 1.0f + currentLevel_ * 0.7f;  // 1.0x to 1.7x clarity
}

void AcetylCholine::update(float workingMemoryLoad, float novelty, TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Update working memory load and stability
    workingMemoryLoad_ = std::min(1.0f, workingMemoryLoad_ + workingMemoryLoad * dtFloat * 0.5f);
    workingMemoryStability_ = 1.0f - workingMemoryLoad_ * 0.3f;  // More load = less stability
    
    // Enhance memory encoding with novelty
    float encodingBoost = novelty * pImpl->encodingSensitivity;
    if (encodingBoost > 0.0f) {
        memoryEncodingRate_ = std::min(1.0f, memoryEncodingRate_ + encodingBoost * dtFloat * 0.3f);
    }
    
    // Decay encoding rate
    memoryEncodingRate_ = std::max(0.0f, memoryEncodingRate_ - pImpl->encodingDecayRate * dtFloat);
    
    // Consolidation strength depends on current level and memory load
    consolidationStrength_ = (1.0f + currentLevel_ * 0.5f) * (1.0f - workingMemoryLoad_ * 0.2f);
    consolidationStrength_ = std::clamp(consolidationStrength_, 0.5f, 2.0f);
    
    // Interference reduction based on current level
    interferenceReduction_ = 1.0f + currentLevel_ * 0.8f;
    
    // Update current level
    setLevel(std::min(1.0f, currentLevel_ + pImpl->adaptationRate * dtFloat * (memoryEncodingRate_ + 0.1f)));
    
    // Update focus regions
    updateFocusRegions(dtFloat);
    
    // Update attentional gating
    updateAttentionGating(dtFloat);
}

void AcetylCholine::signalMemoryEncoding(const std::vector<float>& pattern) {
    recentPattern_ = pattern;
    memoryEncodingRate_ = std::min(1.0f, memoryEncodingRate_ + 0.5f);
    memoryTraces_.push_back({pImpl->currentStep, pattern});  // Note: Need currentStep access
    
    if (memoryTraces_.size() > 100) {
        memoryTraces_.erase(memoryTraces_.begin());
    }
    
    NLM_LOG_INFO("Acetylcholine: Memory encoding - pattern of size " + std::to_string(pattern.size()) + " encoded");
}

void AcetylCholine::signalMemoryConsolidation(class SimulationStep step) {
    if (step % 100 == 0 && !memoryTraces_.empty()) {
        // Consolidate some memory traces
        float totalTraceStrength = 0.0f;
        for (const auto& trace : memoryTraces_) {
            totalTraceStrength += 1.0f;  // Simple strength metric
        }
        
        float consolidationAmount = consolidationStrength_ * 0.1f * std::min(1.0f, static_cast<float>(memoryTraces_.size()) / 50.0f);
        
        // Clear old traces (simplified consolidation model)
        memoryTraces_.clear();
        
        NLM_LOG_INFO("Acetylcholine: Memory consolidation - consolidated " + std::to_string(consolidationAmount) + " traces");
    }
}

float AcetylCholine::getWorkingMemoryModulation(class NeuronId neuron) const {
    float modulation = 0.0f;
    
    // Check memory focus regions
    for (const auto& regionFocus : memoryFocusRegions_) {
        if (brain_) {
            NeuralRegion* regionPtr = brain_->getRegion(regionFocus.first);
            if (regionPtr) {
                for (const auto& pop : regionPtr->getPopulations()) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (n->getId() == neuron) {
                            modulation += regionFocus.second * getWorkingMemoryEnhancement();
                        }
                    }
                }
            }
        }
    }
    
    // Check attentional spotlights
    for (const auto& spotlight : attentionalSpotlights_) {
        if (brain_) {
            NeuralRegion* regionPtr = brain_->getRegion(spotlight.first);
            if (regionPtr) {
                for (const auto& pop : regionPtr->getPopulations()) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (n->getId() == neuron) {
                            modulation += spotlight.second * getAttentionClarity();
                        }
                    }
                }
            }
        }
    }
    
    return modulation;
}

float AcetylCholine::getAttentionGating(class NeuronId neuron) const {
    float gating = 0.0f;
    
    // Attention gating reduces interference between competing traces
    gating = interferenceReduction_ * (1.0f - workingMemoryLoad_ * 0.5f);
    
    return gating;
}

void AcetylCholine::setWorkingMemoryFocus(class RegionId region, float strength) {
    // Remove existing focus on this region
    for (auto it = memoryFocusRegions_.begin(); it != memoryFocusRegions_.end(); ++it) {
        if (it->first == region) {
            memoryFocusRegions_.erase(it);
            break;
        }
    }
    
    // Add new focus
    memoryFocusRegions_.push_back({region, strength});
    NLM_LOG_INFO("Acetylcholine: Working memory focus set on Region " + std::to_string(region.index()) + " (strength: " + std::to_string(strength) + ")");
}

void AcetylCholine::setAttentionalSpotlight(class RegionId region, float strength) {
    // Remove existing spotlight on this region
    for (auto it = attentionalSpotlights_.begin(); it != attentionalSpotlights_.end(); ++it) {
        if (it->first == region) {
            attentionalSpotlights_.erase(it);
            break;
        }
    }
    
    // Add new spotlight
    attentionalSpotlights_.push_back({region, strength});
    NLM_LOG_INFO("Acetylcholine: Attentional spotlight set on Region " + std::to_string(region.index()) + " (strength: " + std::to_string(strength) + ")");
}

void AcetylCholine::reset() {
    currentLevel_ = 0.0f;
    memoryEncodingRate_ = 0.0f;
    consolidationStrength_ = 1.0f;
    interferenceReduction_ = 1.0f;
    workingMemoryLoad_ = 0.0f;
    workingMemoryStability_ = 1.0f;
    recentPattern_.clear();
    memoryTraces_.clear();
    memoryFocusRegions_.clear();
    attentionalSpotlights_.clear();
    
    NLM_LOG_INFO("Acetylcholine reset complete");
}

void AcetylCholine::updateFocusRegions(float dt) {
    // Adjust focus region strengths based on current ACh level
    for (auto& regionFocus : memoryFocusRegions_) {
        float targetStrength = regionFocus.second * currentLevel_;
        float currentStrength = regionFocus.second;
        
        // Adapt focus strength toward target
        float adaptation = (targetStrength - currentStrength) * pImpl->adaptationRate * dt;
        regionFocus.second += adaptation;
        
        // Decay weak focus regions
        if (regionFocus.second < 0.02f) {
            regionFocus.second = 0.0f;
        }
    }
    
    for (auto& spotlight : attentionalSpotlights_) {
        float targetStrength = spotlight.second * currentLevel_;
        float currentStrength = spotlight.second;
        
        // Adapt spotlight strength toward target
        float adaptation = (targetStrength - currentStrength) * pImpl->adaptationRate * dt;
        spotlight.second += adaptation;
        
        // Decay weak spotlights
        if (spotlight.second < 0.02f) {
            spotlight.second = 0.0f;
        }
    }
    
    // Remove empty focus regions and spotlights
    memoryFocusRegions_.erase(
        std::remove_if(memoryFocusRegions_.begin(), memoryFocusRegions_.end(),
            [](const std::pair<RegionId, float>& rf) { return rf.second < 0.01f; }),
        memoryFocusRegions_.end()
    );
    
    attentionalSpotlights_.erase(
        std::remove_if(attentionalSpotlights_.begin(), attentionalSpotlights_.end(),
            [](const std::pair<RegionId, float>& sp) { return sp.second < 0.01f; }),
        attentionalSpotlights_.end()
    );
}

void AcetylCholine::updateAttentionGating(float dt) {
    // Update attentional gating based on working memory load
    // Higher load reduces gating (more interference)
    float targetGating = 1.0f + currentLevel_ * 0.8f;
    float currentGating = interferenceReduction_;
    
    float adaptation = (targetGating - currentGating) * pImpl->adaptationRate * dt;
    interferenceReduction_ += adaptation;
    
    // Fatigue effect
    interferenceReduction_ *= (1.0f - pImpl->fatigueRate * dt);
}

} // namespace nlm