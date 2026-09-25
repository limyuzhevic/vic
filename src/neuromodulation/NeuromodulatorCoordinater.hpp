#pragma once

#include "Neuromodulator.hpp"
#include "Dopamine.hpp"
#include "Acetylcholine.hpp"
#include "Norepinephrine.hpp"
#include "Serotonin.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"
#include "Reward.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <vector>
#include <map>

namespace nlm {

// Neuromodulator Coordination System
// Coordinates and balances interactions between different neuromodulators
class NeuromodulatorCoordinater {
public:
    NeuromodulatorCoordinater(std::shared_ptr<Config> config);
    ~NeuromodulatorCoordinater();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Update all neuromodulators and their interactions
    void update(TimestepDuration dt);
    
    // Get neuromodulator balance levels
    float getBalanceLevel() const { return balanceLevel_; }
    float getCoherenceLevel() const { return coherenceLevel_; }
    
    // Get state-dependent modulation
    float getArousalState() const { return arousalState_; }
    float getAttentionState() const { return attentionState_; }
    float getMoodState() const { return moodState_; }
    
    // Neuromodulator interactions
    void updateSerotoninDynamics();  // 5-HT modulates mood and anxiety
    void updateNorepinephrineDynamics();  // NE modulates arousal and vigilance
    void updateAcetylcholineDynamics();  // ACh modulates attention and memory
    void updateDopamineDynamics();  // DA modulates reward and learning
    void updateCrossModulation();  // Interactions between neuromodulators
    
    // Balance system
    void maintainBalance();  // Prevent any single neuromodulator from dominating
    void adaptToEnvironmentalChanges();  // Adapt balance to environment
    
    // Get neuromodulator signals for integration
    float getIntegratedArousal() const;
    float getIntegratedAttention() const;
    float getIntegratedLearning() const;
    float getIntegratedBehavior() const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Neuromodulator Balance System
// Maintains homeostasis among neuromodulators
class NeuromodulatorBalanceSystem {
public:
    NeuromodulatorBalanceSystem();
    ~NeuromodulatorBalanceSystem();
    
    // Update balance based on neuromodulator levels
    void updateBalance(float dopamine, float acetylcholine, float norepinephrine, 
                      float serotonin, float curiosity, float novelty);
    
    // Get balancing weights for each neuromodulator
    float getDopamineWeight() const { return weights_.at("dopamine"); }
    float getAcetylcholineWeight() const { return weights_.at("acetylcholine"); }
    float getNorepinephrineWeight() const { return weights_.at("norepinephrine"); }
    float getSerotoninWeight() const { return weights_.at("serotonin"); }
    float getCuriosityWeight() const { return weights_.at("curiosity"); }
    float getNoveltyWeight() const { return weights_.at("novelty"); }
    
    // Reset balance
    void reset();
    
private:
    std::map<std::string, float> weights_;
    float balanceLevel_;
    float coherenceLevel_;
    TimestepDuration balanceDecayRate_;
};

// State-Dependent Neuromodulation
// Adapts neuromodulation based on internal and external state
class StateDependentModulation {
public:
    StateDependentModulation();
    ~StateDependentModulation();
    
    // Update state-dependent modulation
    void updateState(float arousal, float attention, float mood, float stress);
    
    // Get state-dependent modulation factors
    float getArousalModulation() const { return arousalModulation_; }
    float getAttentionModulation() const { return attentionModulation_; }
    float getMoodModulation() const { return moodModulation_; }
    float getStressModulation() const { return stressModulation_; }
    
    // Get behavioral output
    float getBehavioralOutput() const;
    
    // Reset
    void reset();
    
private:
    float arousalModulation_;
    float attentionModulation_;
    float moodModulation_;
    float stressModulation_;
    float behavioralOutput_;
    
    // State history
    std::vector<float> arousalHistory_;
    std::vector<float> attentionHistory_;
    std::vector<float> moodHistory_;
    std::vector<float> stressHistory_;
};

} // namespace nlm