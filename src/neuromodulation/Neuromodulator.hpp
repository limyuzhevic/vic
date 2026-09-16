#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

    // Novelty: Surprise detection
    // PLACEHOLDER - Phase 2
    class Novelty : public Neuromodulator {
    public:
        const char* getName() const override { return "Novelty"; }
        float getLevel() const override { return 0.0f; }
        void setLevel(float level) override {}
        float getPlasticityFactor() const override { return 1.0f; }
        void update(TimestepDuration dt) override {}
    };

    // Acetylcholine: Attention and memory consolidation
    // PLACEHOLDER - Phase 2
    class Acetylcholine : public Neuromodulator {
    public:
        Acetylcholine();
        ~Acetylcholine() override;
        
        const char* getName() const override;
        float getLevel() const override;
        void setLevel(float level) override;
        float getPlasticityFactor() const override;
        void update(TimestepDuration dt) override;
        
        // Attention modulation
        void enhanceAttention(float strength);
        void suppressDistractions(float strength);
        
        // Memory consolidation
        void consolidateMemories(float strength);
        
    private:
        struct Impl;
        Impl* pImpl;
    };

    // Norepinephrine: Arousal and vigilance
    // PLACEHOLDER - Phase 2
    class Norepinephrine : public Neuromodulator {
    public:
        Norepinephrine();
        ~Norepinephrine() override;
        
        const char* getName() const override;
        float getLevel() const override;
        void setLevel(float level) override;
        float getPlasticityFactor() const override;
        void update(TimestepDuration dt) override;
        
        // Arousal effects
        void increaseArousal(float level);
        void decreaseArousal(float level);
        void modulateVigilance(float vigilance);
        
    private:
        struct Impl;
        Impl* pImpl;
    };

    // Serotonin: Mood, impulsivity, and social behavior
    // PLACEHOLDER - Phase 2
    class Serotonin : public Neuromodulator {
    public:
        Serotonin();
        ~Serotonin() override;
        
        const char* getName() const override;
        float getLevel() const override;
        void setLevel(float level) override;
        float getPlasticityFactor() const override;
        void update(TimestepDuration dt) override;
        
        // Mood effects
        void modulateMood(float mood);
        void adjustImpulsivity(float level);
        
        // Social behavior
        void enhanceSocialCohesion(float strength);
        void reduceSocialBehavior(float strength);
        
    private:
        struct Impl;
        Impl* pImpl;
    };

    // Complete neuromodulator registry for agent access
    const std::vector<const Neuromodulator*> getAllNeuromodulators();
    Neuromodulator* getNeuromodulator(const std::string& name);
    
    // System-wide neuromodulation
    class NeuromodulationSystem {
    public:
        NeuromodulationSystem();
        ~NeuromodulationSystem();
        
        void initialize(Brain* brain);
        void update(TimestepDuration dt);
        
        // Add neuromodulator to system
        void addNeuromodulator(std::unique_ptr<Neuromodulator> modulator);
        
        // Update neuromodulation based on brain state
        void updateFromBrain(const Brain& brain);
        
        // Get current neuromodulation levels
        float getTotalNeuromodulationLevel() const;
        float getModulationFor(const std::string& name) const;
        
        // Control neuromodulation
        void setNeuromodulationLevel(const std::string& name, float level);
        void adjustNeuromodulation(const std::string& name, float delta);
        
        // Reset all neuromodulators
        void reset();
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
