// Enhanced neuromodulation system for NLM
// Provides advanced neuromodulation behaviors with sophisticated learning mechanisms

#pragma once

#include "neuromodulation/Neuromodulator.hpp"
#include "brain/Brain.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

// Advanced neuromodulation types
enum class NeuromodulationType {
    Dopamine,
    Serotonin,
    Acetylcholine,
    Norepinephrine,
    Adrenaline,
    Custom
};

// Neuromodulation states
struct NeuromodulationState {
    NeuromodulationType type;
    float level;              // Current neuromodulator level (0.0 to 1.0)
    float baseline;           // Baseline level
    float targetLevel;        // Target level
    float adaptationRate;     // Rate of adaptation to target
    float noiseLevel;         // Level of biological noise
    float history[10];         // Recent history for pattern detection
    size_t historyIndex;       // Current index in history array
    
    NeuromodulationState() : type(NeuromodulationType::Dopamine),
                           level(0.0f), baseline(0.0f), targetLevel(0.0f),
                           adaptationRate(0.1f), noiseLevel(0.01f),
                           historyIndex(0) {
        for (auto& h : history) h = 0.0f;
    }
};

// Advanced neuromodulation effect parameters
struct NeuromodulationEffect {
    NeuromodulationType type;
    float baseStrength;           // Base effect strength
    float inputGain;              // Sensitivity to input signals
    float adaptationTime;         // Time constant for adaptation
    float saturationLevel;        // Maximum effect level
    float inhibitionThreshold;    // Threshold for inhibitory effects
    std::function<bool(float)> activationCondition;  // Custom activation condition
    std::function<float(float)> effectFunction;      // Custom effect function
    
    NeuromodulationEffect() : type(NeuromodulationType::Dopamine),
                              baseStrength(1.0f), inputGain(1.0f),
                              adaptationTime(1.0f), saturationLevel(1.0f),
                              inhibitionThreshold(0.5f) {
        // Default activation condition
        activationCondition = [](float level) { return level > 0.1f; };
        
        // Default effect function (sigmoidal response)
        effectFunction = [](float level) {
            // Sigmoidal response with saturation
            return 2.0f / (1.0f + std::exp(-10.0f * (level - 0.5f))) - 1.0f;
        };
    }
};

// Advanced neuromodulator with learning capabilities
class AdvancedNeuromodulator {
public:
    AdvancedNeuromodulator(NeuromodulationType type, const NeuromodulationEffect& effect)
        : type_(type), effect_(effect), brain_(nullptr), rng_(std::make_unique<std::mt19937>(std::random_device{}())) {
        // Initialize state with biologically plausible values
        initializeState();
    }
    
    virtual ~AdvancedNeuromodulator() = default;
    
    // Update neuromodulator level based on current state
    void update(double timestep) {
        // Add biological noise
        std::uniform_real_distribution<float> noiseDist(-effect_.noiseLevel, effect_.noiseLevel);
        float noise = noiseDist(*rng_);
        
        // Adapt toward target level
        float adaptation = (effect_.adaptationTime > 0.0f) ? 
                           (effect_.targetLevel - state_.level) * effect_.adaptationTime * timestep : 0.0f;
        
        // Update level with adaptation and noise
        state_.level += adaptation + noise;
        
        // Clamp to reasonable range
        state_.level = std::clamp(state_.level, -1.0f, 1.0f);
        
        // Update history
        state_.historyIndex = (state_.historyIndex + 1) % 10;
        state_.history[state_.historyIndex] = state_.level;
        
        // Apply neuromodulation effects to brain
        applyEffects();
    }
    
    // Set brain reference for effect application
    void setBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
    }
    
    // Get current neuromodulator level
    float getLevel() const { return state_.level; }
    
    // Get neuromodulator type
    NeuromodulationType getType() const { return type_; }
    
    // Set target level
    void setTargetLevel(float target) {
        state_.targetLevel = std::clamp(target, -1.0f, 1.0f);
    }
    
    // Get target level
    float getTargetLevel() const { return state_.targetLevel; }
    
    // Set baseline level
    void setBaselineLevel(float baseline) {
        state_.baseline = baseline;
        state_.level = baseline;
        state_.targetLevel = baseline;
    }
    
    // Get baseline level
    float getBaselineLevel() const { return state_.baseline; }
    
    // Update based on prediction error
    virtual void updateFromPredictionError(float predictionError, double timestep) {
        // Advanced prediction error integration
        float errorSignal = predictionError * effect_.inputGain;
        
        // Update target level based on prediction error
        setTargetLevel(std::clamp(getTargetLevel() + errorSignal * 0.1f, -1.0f, 1.0f));
    }
    
    // Update based on reward prediction
    virtual void updateFromRewardPrediction(float rewardPrediction, double timestep) {
        // Reward prediction modulation
        float rewardSignal = rewardPrediction * 0.5f;  // Scaling factor
        
        // Modulate baseline based on reward prediction
        setBaselineLevel(std::clamp(getBaselineLevel() + rewardSignal * 0.05f, -1.0f, 1.0f));
    }
    
    // Update based on novelty detection
    virtual void updateFromNovelty(float novelty, double timestep) {
        // Novelty-driven exploration
        float noveltySignal = novelty * 0.3f;
        
        // Increase target level for exploration
        setTargetLevel(std::min(getTargetLevel() + noveltySignal * 0.2f, 1.0f));
    }
    
    // Learn from experience
    virtual void learnFromExperience(const std::vector<float>& experienceMetrics, double timestep) {
        // Advanced learning from experience patterns
        if (experienceMetrics.empty()) return;
        
        // Calculate pattern statistics
        float mean = std::accumulate(experienceMetrics.begin(), experienceMetrics.end(), 0.0f) / experienceMetrics.size();
        float variance = 0.0f;
        for (float val : experienceMetrics) {
            variance += (val - mean) * (val - mean);
        }
        variance /= experienceMetrics.size();
        
        // Adapt based on pattern
        float patternStrength = std::sqrt(variance);
        if (patternStrength > 0.1f) {
            setTargetLevel(getTargetLevel() + patternStrength * 0.1f * timestep);
        }
    }
    
    // Get plasticity factor
    float getPlasticityFactor() const {
        // Neuromodulator affects plasticity
        float effect = effect_.effectFunction(getLevel());
        return std::clamp(0.5f + effect * 0.5f, 0.1f, 2.0f);
    }
    
    // Get excitability factor
    float getExcitabilityFactor() const {
        // Neuromodulator affects neural excitability
        float effect = effect_.effectFunction(getLevel());
        return std::clamp(1.0f + effect * 0.5f, 0.5f, 2.0f);
    }
    
protected:
    virtual void applyEffects() {
        if (!brain_) return;
        
        float level = getLevel();
        float effect = effect_.effectFunction(level);
        
        // Apply effects based on neuromodulator type
        switch (type_) {
            case NeuromodulationType::Dopamine:
                applyDopamineEffects(level, effect);
                break;
            case NeuromodulationType::Serotonin:
                applySerotoninEffects(level, effect);
                break;
            case NeuromodulationType::Acetylcholine:
                applyAcetylcholineEffects(level, effect);
                break;
            case NeuromodulationType::Norepinephrine:
                applyNorepinephrineEffects(level, effect);
                break;
            case NeuromodulationType::Adrenaline:
                applyAdrenalineEffects(level, effect);
                break;
            default:
                applyCustomEffects(level, effect);
                break;
        }
    }
    
    virtual void applyDopamineEffects(float level, float effect) {
        // Dopamine affects both plasticity and excitability
        if (brain_) {
            // Update STDP parameters
            auto* stdp = brain_->getSTDP();
            if (stdp) {
                float plasticityFactor = getPlasticityFactor();
                stdp->setLTPWeight(0.01f * plasticityFactor);
                stdp->setLTDWeight(0.012f * plasticityFactor);
            }
            
            // Apply to neurons (excitability modulation)
            float excitabilityFactor = getExcitabilityFactor();
            if (excitabilityFactor > 1.0f) {
                // Apply additional current to neurons
                for (auto& region : brain_->getRegions()) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            float additionalCurrent = (excitabilityFactor - 1.0f) * 0.5f;
                            neuron->injectCurrent(additionalCurrent);
                        }
                    }
                }
            }
        }
    }
    
    virtual void applySerotoninEffects(float level, float effect) {
        // Serotonin affects anxiety and learning rate
        if (brain_) {
            // Modulate plasticity rate
            float plasticityFactor = getPlasticityFactor();
            
            // Update learning rates
            auto* hebbian = brain_->getHebbian();
            if (hebbian) {
                hebbian->setLearningRate(0.01f * plasticityFactor);
            }
            
            // Apply to cognitive systems
            auto* planner = brain_->getPlanner();
            if (planner) {
                // Serotonin affects decision making and exploration
                planner->setExplorationFactor(std::clamp(level, 0.0f, 1.0f));
            }
        }
    }
    
    virtual void applyAcetylcholineEffects(float level, float effect) {
        // Acetylcholine affects attention and memory
        if (brain_) {
            // Enhance attention mechanisms
            auto* attention = brain_->getAttention();
            if (attention) {
                attention->setInhibitionStrength(0.5f - level * 0.3f);  // Reduced inhibition
                attention->setExcitationStrength(0.5f + level * 0.3f);  // Enhanced excitation
            }
            
            // Boost memory encoding
            if (level > 0.0f) {
                auto* workingMemory = brain_->getWorkingMemory();
                if (workingMemory) {
                    workingMemory->enhanceEncoding(std::abs(level));
                }
            }
        }
    }
    
    virtual void applyNorepinephrineEffects(float level, float effect) {
        // Norepinephrine affects arousal and vigilance
        if (brain_) {
            // Increase arousal
            if (level > 0.0f) {
                // Apply to sensory systems
                for (auto& region : brain_->getRegions()) {
                    for (auto& pop : region->getPopulations()) {
                        if (pop->getNeuronType() == NeuronType::Sensory) {
                            for (auto* neuron : pop->getNeurons()) {
                                neuron->injectCurrent(level * 0.3f);
                            }
                        }
                    }
                }
            }
            
            // Enhance alertness in cognitive systems
            auto* planner = brain_->getPlanner();
            if (planner) {
                planner->setAlertnessLevel(std::clamp(level, 0.0f, 1.0f));
            }
        }
    }
    
    virtual void applyAdrenalineEffects(float level, float effect) {
        // Adrenaline affects stress response and rapid reactions
        if (brain_) {
            // Emergency response - prepare for quick action
            if (level > 0.5f) {
                // Prepare motor systems for rapid response
                for (auto& region : brain_->getRegions()) {
                    for (auto& pop : region->getPopulations()) {
                        if (pop->getNeuronType() == NeuronType::Motor) {
                            for (auto* neuron : pop->getNeurons()) {
                                neuron->setRefractoryPeriod(static_cast<uint32_t>(std::max(1.0f, 5.0f - level * 4.0f)));
                            }
                        }
                    }
                }
            }
            
            // Increase overall excitability
            float overallExcitability = 1.0f + level * 0.8f;
            for (auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent((overallExcitability - 1.0f) * 0.2f);
                    }
                }
            }
        }
    }
    
    virtual void applyCustomEffects(float level, float effect) {
        // Custom neuromodulation effects
        if (brain_ && effect_.effectFunction) {
            float customEffect = effect_.effectFunction(level);
            
            // Apply custom effect to all neurons
            for (auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(customEffect * 0.1f);
                    }
                }
            }
        }
    }
    
    void initializeState() {
        // Initialize with biologically plausible values
        std::uniform_real_distribution<float> levelDist(-0.5f, 0.5f);
        std::uniform_real_distribution<float> baselineDist(-0.2f, 0.2f);
        std::uniform_real_distribution<float> targetDist(-0.5f, 0.5f);
        
        state_.level = levelDist(*rng_);
        state_.baseline = baselineDist(*rng_);
        state_.targetLevel = targetDist(*rng_);
        
        // Set default adaptation rate based on neuromodulator type
        switch (type_) {
            case NeuromodulationType::Dopamine:
                state_.adaptationRate = 0.05f;  // Fast adaptation
                state_.noiseLevel = 0.02f;
                break;
            case NeuromodulationType::Serotonin:
                state_.adaptationRate = 0.02f;  // Slower adaptation
                state_.noiseLevel = 0.01f;
                break;
            case NeuromodulationType::Acetylcholine:
                state_.adaptationRate = 0.08f;  // Moderate adaptation
                state_.noiseLevel = 0.03f;
                break;
            case NeuromodulationType::Norepinephrine:
                state_.adaptationRate = 0.06f;  // Medium adaptation
                state_.noiseLevel = 0.02f;
                break;
            case NeuromodulationType::Adrenaline:
                state_.adaptationRate = 0.1f;   // Fast adaptation for emergency response
                state_.noiseLevel = 0.05f;
                break;
            default:
                state_.adaptationRate = 0.05f;
                state_.noiseLevel = 0.02f;
                break;
        }
    }
    
private:
    NeuromodulationType type_;
    NeuromodulationEffect effect_;
    NeuromodulationState state_;
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<std::mt19937> rng_;
};

// Advanced neuromodulation manager
class AdvancedNeuromodulationManager {
public:
    AdvancedNeuromodulationManager() = default;
    ~AdvancedNeuromodulationManager() = default;
    
    // Add an advanced neuromodulator
    void addNeuromodulator(const std::string& name, NeuromodulationType type, 
                          const NeuromodulationEffect& effect) {
        auto neuromodulator = std::make_unique<AdvancedNeuromodulator>(type, effect);
        neuromodulators_[name] = std::move(neuromulator);
    }
    
    // Get neuromodulator by name
    AdvancedNeuromodulator* getNeuromodulator(const std::string& name) {
        auto it = neuromodulators_.find(name);
        if (it != neuromodulators_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    // Get neuromodulator by type
    AdvancedNeuromodulator* getNeuromodulator(NeuromodulationType type) {
        for (auto& pair : neuromodulators_) {
            if (pair.second->getType() == type) {
                return pair.second.get();
            }
        }
        return nullptr;
    }
    
    // Set brain reference for all neuromodulators
    void setBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
        for (auto& pair : neuromodulators_) {
            pair.second->setBrain(brain_);
        }
    }
    
    // Update all neuromodulators
    void update(double timestep) {
        for (auto& pair : neuromodulators_) {
            pair.second->update(timestep);
        }
    }
    
    // Get composite neuromodulation level
    float getCompositeNeuromodulationLevel() const {
        if (neuromulators_.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (const auto& pair : neuromodulators_) {
            sum += pair.second->getLevel();
        }
        return sum / static_cast<float>(neuromulators_.size());
    }
    
    // Get neuromodulation statistics
    struct NeuromodulationStats {
        size_t totalNeuromodulators;
        float averageLevel;
        float minLevel;
        float maxLevel;
        float plasticityImpact;
        float excitabilityImpact;
    };
    
    NeuromodulationStats getStats() const {
        NeuromodulationStats stats;
        stats.totalNeuromodulators = neuromodulators_.size();
        stats.averageLevel = 0.0f;
        stats.minLevel = 1.0f;
        stats.maxLevel = -1.0f;
        stats.plasticityImpact = 0.0f;
        stats.excitabilityImpact = 0.0f;
        
        if (stats.totalNeuromodulators == 0) {
            return stats;
        }
        
        float totalLevel = 0.0f;
        float totalPlasticity = 0.0f;
        float totalExcitability = 0.0f;
        
        for (const auto& pair : neuromodulators_) {
            float level = pair.second->getLevel();
            totalLevel += level;
            totalPlasticity += pair.second->getPlasticityFactor();
            totalExcitability += pair.second->getExcitabilityFactor();
            
            if (level < stats.minLevel) stats.minLevel = level;
            if (level > stats.maxLevel) stats.maxLevel = level;
        }
        
        stats.averageLevel = totalLevel / stats.totalNeuromodulators;
        stats.plasticityImpact = totalPlasticity / stats.totalNeuromodulators;
        stats.excitabilityImpact = totalExcitability / stats.totalNeuromodulators;
        
        return stats;
    }
    
    // Get neuromodulator names
    std::vector<std::string> getNeuromodulatorNames() const {
        std::vector<std::string> names;
        for (const auto& pair : neuromodulators_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Create preset neuromodulation configuration
    static std::unordered_map<std::string, NeuromodulationEffect> getPresetEffects() {
        std::unordered_map<std::string, NeuromodulationEffect> presets;
        
        // Dopamine preset
        NeuromodulationEffect dopamineEffect;
        dopamineEffect.type = NeuromodulationType::Dopamine;
        dopamineEffect.baseStrength = 1.0f;
        dopamineEffect.inputGain = 0.8f;
        dopamineEffect.adaptationTime = 0.05f;
        dopamineEffect.saturationLevel = 0.8f;
        dopamineEffect.inhibitionThreshold = 0.3f;
        presets["dopamine"] = dopamineEffect;
        
        // Serotonin preset
        NeuromodulationEffect serotoninEffect;
        serotoninEffect.type = NeuromodulationType::Serotonin;
        serotoninEffect.baseStrength = 0.6f;
        serotoninEffect.inputGain = 0.5f;
        serotoninEffect.adaptationTime = 0.1f;
        serotoninEffect.saturationLevel = 0.7f;
        serotoninEffect.inhibitionThreshold = 0.4f;
        presets["serotonin"] = serotoninEffect;
        
        // Acetylcholine preset
        NeuromodulationEffect acetylcholineEffect;
        acetylcholineEffect.type = NeuromodulationType::Acetylcholine;
        acetylcholineEffect.baseStrength = 0.7f;
        acetylcholineEffect.inputGain = 0.6f;
        acetylcholineEffect.adaptationTime = 0.08f;
        acetylcholineEffect.saturationLevel = 0.75f;
        acetylcholineEffect.inhibitionThreshold = 0.35f;
        presets["acetylcholine"] = acetylcholineEffect;
        
        // Norepinephrine preset
        NeuromodulationEffect norepinephrineEffect;
        norepinephrineEffect.type = NeuromodulationType::Norepinephrine;
        norepinephrineEffect.baseStrength = 0.8f;
        norepinephrineEffect.inputGain = 0.7f;
        norepinephrineEffect.adaptationTime = 0.06f;
        norepinephrineEffect.saturationLevel = 0.85f;
        norepinephrineEffect.inhibitionThreshold = 0.25f;
        presets["norepinephrine"] = norepinephrineEffect;
        
        // Adrenaline preset
        NeuromodulationEffect adrenalineEffect;
        adrenalineEffect.type = NeuromodulationType::Adrenaline;
        adrenalineEffect.baseStrength = 1.2f;  // Stronger
        adrenalineEffect.inputGain = 1.0f;
        adrenalineEffect.adaptationTime = 0.03f;  // Very fast
        adrenalineEffect.saturationLevel = 0.9f;
        adrenalineEffect.inhibitionThreshold = 0.2f;  // Low threshold
        presets["adrenaline"] = adrenalineEffect;
        
        return presets;
    }
    
private:
    std::unordered_map<std::string, std::unique_ptr<AdvancedNeuromodulator>> neuromodulators_;
    std::shared_ptr<Brain> brain_;
};

// Enhanced neuromodulation utility functions
namespace NeuromodulationUtils {
    // Create sophisticated neuromodulation effect
    NeuromodulationEffect createSophisticatedEffect(NeuromodulationType type, 
                                                     float baseStrength = 1.0f,
                                                     float inputGain = 1.0f,
                                                     float adaptationTime = 0.1f,
                                                     float saturationLevel = 1.0f,
                                                     float inhibitionThreshold = 0.5f) {
        NeuromodulationEffect effect;
        effect.type = type;
        effect.baseStrength = baseStrength;
        effect.inputGain = inputGain;
        effect.adaptationTime = adaptationTime;
        effect.saturationLevel = saturationLevel;
        effect.inhibitionThreshold = inhibitionThreshold;
        
        // Custom effect functions based on neuromodulator type
        switch (type) {
            case NeuromodulationType::Dopamine:
                effect.effectFunction = [](float level) {
                    // Dopamine: inverted U-shaped response (optimal at moderate levels)
                    float normalized = std::abs(level);
                    return 1.0f - std::abs(2.0f * normalized - 1.0f);
                };
                break;
            case NeuromodulationType::Serotonin:
                effect.effectFunction = [](float level) {
                    // Serotonin: linear response
                    return level;
                };
                break;
            case NeuromodulationType::Acetylcholine:
                effect.effectFunction = [](float level) {
                    // Acetylcholine: thresholded response
                    return (level > 0.0f) ? level : 0.0f;
                };
                break;
            case NeuromodulationType::Norepinephrine:
                effect.effectFunction = [](float level) {
                    // Norepinephrine: exponential response
                    return std::tanh(level * 2.0f);
                };
                break;
            case NeuromodulationType::Adrenaline:
                effect.effectFunction = [](float level) {
                    // Adrenaline: super-linear response
                    return std::pow(std::tanh(level), 2.0f);
                };
                break;
            default:
                effect.effectFunction = [](float level) {
                    // Default: sigmoidal response
                    return 2.0f / (1.0f + std::exp(-10.0f * (level - 0.5f))) - 1.0f;
                };
                break;
        }
        
        return effect;
    }
    
    // Create integrated neuromodulation configuration
    std::unordered_map<std::string, NeuromodulationEffect> createIntegratedConfiguration() {
        auto presets = AdvancedNeuromodulationManager::getPresetEffects();
        
        // Create integrated configuration with balanced neuromodulators
        std::unordered_map<std::string, NeuromodulationEffect> integrated;
        
        // Add all neuromodulators with adjusted parameters for integration
        for (const auto& pair : presets) {
            NeuromodulationEffect effect = pair.second;
            
            // Adjust for integration
            if (pair.first == "dopamine") {
                effect.baseStrength = 0.9f;
                effect.inputGain = 0.8f;
                effect.adaptationTime = 0.07f;
            } else if (pair.first == "serotonin") {
                effect.baseStrength = 0.7f;
                effect.inputGain = 0.6f;
                effect.adaptationTime = 0.12f;
            } else if (pair.first == "acetylcholine") {
                effect.baseStrength = 0.8f;
                effect.inputGain = 0.7f;
                effect.adaptationTime = 0.09f;
            } else if (pair.first == "norepinephrine") {
                effect.baseStrength = 0.85f;
                effect.inputGain = 0.75f;
                effect.adaptationTime = 0.08f;
            } else if (pair.first == "adrenaline") {
                effect.baseStrength = 1.1f;
                effect.inputGain = 0.9f;
                effect.adaptationTime = 0.04f;
            }
            
            integrated[pair.first] = effect;
        }
        
        return integrated;
    }
    
    // Calculate neuromodulation synergy
    float calculateNeuromodulationSynergy(const std::unordered_map<std::string, NeuromodulationEffect>& effects) {
        if (effects.empty()) return 0.0f;
        
        float totalEffect = 0.0f;
        for (const auto& pair : effects) {
            totalEffect += pair.second.baseStrength;
        }
        
        // Calculate synergy (non-linear interaction)
        return 1.0f + std::sqrt(totalEffect / effects.size() - 1.0f) * 0.5f;
    }
    
    // Create neuromodulation schedule
    struct NeuromodulationSchedule {
        std::string name;
        std::unordered_map<double, std::string> timeBasedSchedule;  // time -> neuromodulator
        std::unordered_map<std::string, std::pair<double, double>> valueBasedSchedule;  // neuromodulator -> [min, max] value
        std::vector<std::string> priorityOrder;  // Order of importance
    };
    
    NeuromodulationSchedule createLearningSchedule() {
        NeuromodulationSchedule schedule;
        schedule.name = "learning_schedule";
        
        // Early learning phase: high curiosity, moderate dopamine
        schedule.timeBasedSchedule = {
            {0.0, "curiosity"},      // Start with curiosity
            {100.0, "dopamine"},     // Then dopamine
            {500.0, "serotonin"},    // Then serotonin
            {1000.0, "acetylcholine"} // Finally acetylcholine
        };
        
        schedule.priorityOrder = {"curiosity", "dopamine", "serotonin", "acetylcholine"};
        
        return schedule;
    }
    
    NeuromodulationSchedule createEmergencySchedule() {
        NeuromodulationSchedule schedule;
        schedule.name = "emergency_schedule";
        
        // Emergency response: adrenaline dominant
        schedule.timeBasedSchedule = {
            {0.0, "adrenaline"},    // Immediate adrenaline surge
            {10.0, "norepinephrine"}, // Followed by norepinephrine
            {60.0, "dopamine"}      // Then dopamine for recovery
        };
        
        schedule.priorityOrder = {"adrenaline", "norepinephrine", "dopamine"};
        
        return schedule;
    }
    
    NeuromodulationSchedule createRestorationSchedule() {
        NeuromodulationSchedule schedule;
        schedule.name = "restoration_schedule";
        
        // Restoration phase: gentle rebalancing
        schedule.timeBasedSchedule = {
            {0.0, "serotonin"},      // Start with serotonin (calm)
            {100.0, "acetylcholine"}, // Then acetylcholine (memory)
            {500.0, "dopamine"}      // Finally dopamine (motivation)
        };
        
        schedule.priorityOrder = {"serotonin", "acetylcholine", "dopamine"};
        
        return schedule;
    }
}

} // namespace nlm
