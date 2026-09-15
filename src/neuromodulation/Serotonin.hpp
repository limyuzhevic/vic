#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Real implementation for Phase 2
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get serotonin level
    float getLevel() const;
    void setLevel(float level);
    
    // Get plasticity factor modulation
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Modulate attention system
    void modulateAttention(class AttentionalSelection* attention);
    
    // Modulate memory consolidation
    void modulateMemory(class NeuralEpisodicMemory* memory);
    
    // Social behavior modulation
    void modulateSocialBehavior(class SocialLearning* social);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Real implementation for Phase 2
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get norepinephrine level
    float getLevel() const;
    void setLevel(float level);
    
    // Get plasticity factor modulation
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Modulate attention and alertness
    void modulateAttention(class AttentionalSelection* attention);
    
    // Modulate sensory processing
    void modulateSensory(class SensoryInput* sensory);
    
    // Arousal-based action selection
    void modulateAction(class AgentBrain* agent);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Real implementation for Phase 2
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get acetylcholine level
    float getLevel() const;
    void setLevel(float level);
    
    // Get plasticity factor modulation
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Modulate working memory
    void modulateWorkingMemory(class NeuralWorkingMemory* memory);
    
    // Modulate attention mechanisms
    void modulateAttention(class AttentionalSelection* attention);
    
    // Enhance memory encoding
    void enhanceMemoryEncoding(class NeuralEpisodicMemory* memory);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm