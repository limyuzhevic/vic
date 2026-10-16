#pragma once

#include "BrainSystem.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"

namespace nlm {

// Wrapper for NeuralWorkingMemory to adapt it to BrainSystem
class WorkingMemorySystemWrapper : public MemorySystem {
public:
    explicit WorkingMemorySystemWrapper(NeuralWorkingMemory* memory) : memory_(memory) {}
    
    const char* getName() const override { return "WorkingMemory"; }
    uint32_t getSystemId() const override { return 1; }
    
    void initialize(Brain* brain) override {
        if (memory_) {
            memory_->initialize(brain);
        }
    }
    
    void update(TimestepDuration dt) override {
        if (memory_) {
            memory_->update(dt);
        }
    }
    
    void reset() override {
        if (memory_) {
            memory_->clear();
        }
    }
    
    void store(const std::vector<float>& pattern, float strength) override {
        // For neural working memory, we store to neurons
        // This is a simplified implementation
        for (size_t i = 0; i < pattern.size() && i < 100; ++i) {
            memory_->storeToNeuron(NeuronId(i), pattern[i]);
        }
    }
    
    std::vector<float> retrieve() const override {
        return memory_ ? memory_->retrieve() : std::vector<float>();
    }
    
    void clear() override {
        if (memory_) {
            memory_->clear();
        }
    }
    
    size_t getItemCount() const override {
        return memory_ ? memory_->getActiveTraces() : 0;
    }
    
    // Access to the underlying memory for backward compatibility
    NeuralWorkingMemory* getMemory() { return memory_.get(); }
    
private:
    std::unique_ptr<NeuralWorkingMemory> memory_;
};

// Wrapper for NeuralEpisodicMemory to adapt it to BrainSystem
class EpisodicMemorySystemWrapper : public MemorySystem {
public:
    explicit EpisodicMemorySystemWrapper(NeuralEpisodicMemory* memory) : memory_(memory) {}
    
    const char* getName() const override { return "EpisodicMemory"; }
    uint32_t getSystemId() const override { return 2; }
    
    void initialize(Brain* brain) override {
        if (memory_) {
            memory_->initialize(brain);
        }
    }
    
    void store(const std::vector<float>& data, float strength) override {
        // For episodic memory, store as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = 0; // Will be set by the actual implementation
        episode.sensoryState = data;
        episode.reward = strength;
        
        if (memory_) {
            memory_->storeEpisode(episode);
        }
    }
    
    std::vector<float> retrieve() const override {
        // This is a simplified implementation
        std::vector<float> result;
        if (memory_) {
            size_t count = memory_->getEpisodeCount();
            if (count > 0) {
                const auto* episode = memory_->getEpisode(0);
                if (episode) {
                    result = episode->sensoryState;
                }
            }
        }
        return result;
    }
    
    void clear() override {
        if (memory_) {
            memory_->clear();
        }
    }
    
    size_t getItemCount() const override {
        return memory_ ? memory_->getEpisodeCount() : 0;
    }
    
    NeuralEpisodicMemory* getMemory() { return memory_.get(); }
    
private:
    std::unique_ptr<NeuralEpisodicMemory> memory_;
};

// Wrapper for NeuralAssociativeMemory to adapt it to BrainSystem
class AssociativeMemorySystemWrapper : public MemorySystem {
public:
    explicit AssociativeMemorySystemWrapper(NeuralAssociativeMemory* memory) : memory_(memory) {}
    
    const char* getName() const override { return "AssociativeMemory"; }
    uint32_t getSystemId() const override { return 3; }
    
    void initialize(Brain* brain) override {
        if (memory_) {
            memory_->initialize(brain);
        }
    }
    
    void store(const std::vector<float>& patternA, float strength) override {
        // Store association to a pattern
        if (!patternA.empty() && memory_) {
            // Create associations to some dummy patterns
            // This is a simplified implementation
            for (size_t i = 1; i <= patternA.size() && i < 10; ++i) {
                std::vector<float> patternB(patternA.size(), 0.0f);
                patternB[i % patternB.size()] = 1.0f;
                memory_->associate(patternA, patternB, strength);
            }
        }
    }
    
    std::vector<float> retrieve() const override {
        // This is a simplified implementation
        std::vector<float> result;
        if (memory_) {
            result.resize(10, 0.0f);
            result[0] = 1.0f; // Simple retrieval
        }
        return result;
    }
    
    void clear() override {
        if (memory_) {
            memory_->clear();
        }
    }
    
    size_t getItemCount() const override {
        return memory_ ? memory_->getAssociationCount() : 0;
    }
    
    NeuralAssociativeMemory* getMemory() { return memory_.get(); }
    
private:
    std::unique_ptr<NeuralAssociativeMemory> memory_;
};

// Wrapper for PredictionSystem to adapt it to BrainSystem
class PredictionSystemWrapper : public PredictionSystemBase {
public:
    explicit PredictionSystemWrapper(PredictionSystem* prediction) : prediction_(prediction) {}
    
    const char* getName() const override { return "PredictionSystem"; }
    uint32_t getSystemId() const override { return 4; }
    
    void initialize(Brain* brain) override {
        // PredictionSystem doesn't have initialize method currently
    }
    
    std::unique_ptr<SensoryInput> predict(const SensoryInput& input) override {
        return prediction_ ? prediction_->predictNextState(input) : nullptr;
    }
    
    void updatePrediction(const SensoryInput& predicted, const SensoryInput& actual) override {
        if (prediction_) {
            prediction_->updatePredictions(predicted, actual);
        }
    }
    
    float getPredictionError() const override {
        return prediction_ ? prediction_->getPredictionError() : 0.0f;
    }
    
    float getConfidence() const override {
        return prediction_ ? prediction_->getConfidence() : 0.0f;
    }
    
    void update(TimestepDuration dt) override {
        // PredictionSystem updates are handled by the brain
    }
    
    PredictionSystem* getPrediction() { return prediction_.get(); }
    
private:
    std::unique_ptr<PredictionSystem> prediction_;
};

// Wrapper for DevelopmentSystem to adapt it to BrainSystem
class DevelopmentSystemWrapper : public DevelopmentSystemBase {
public:
    explicit DevelopmentSystemWrapper(DevelopmentSystem* devSystem) : devSystem_(devSystem) {}
    
    const char* getName() const override { return "DevelopmentSystem"; }
    uint32_t getSystemId() const override { return 5; }
    
    void initialize(Brain* brain) override {
        // DevelopmentSystem doesn't have initialize method currently
    }
    
    void updateDevelopment(Brain* brain, SimulationStep step) override {
        if (devSystem_) {
            devSystem_->update(brain, step);
        }
    }
    
    DevelopmentalStage getStage() const override {
        return devSystem_ ? devSystem_->getStage() : DevelopmentalStage::Initial;
    }
    
    void setStage(DevelopmentalStage stage) override {
        if (devSystem_) {
            devSystem_->setStage(stage);
        }
    }
    
    bool isCriticalPeriod() const override {
        return devSystem_ ? devSystem_->isCriticalPeriod() : false;
    }
    
    void update(TimestepDuration dt) override {
        // Development is handled periodically by the brain
    }
    
    DevelopmentSystem* getDevelopment() { return devSystem_.get(); }
    
private:
    std::unique_ptr<DevelopmentSystem> devSystem_;
};

// Factory function for creating system wrappers
inline std::unique_ptr<WorkingMemorySystemWrapper> createWorkingMemoryWrapper(NeuralWorkingMemory* memory) {
    return std::make_unique<WorkingMemorySystemWrapper>(memory);
}

inline std::unique_ptr<EpisodicMemorySystemWrapper> createEpisodicMemoryWrapper(NeuralEpisodicMemory* memory) {
    return std::make_unique<EpisodicMemorySystemWrapper>(memory);
}

inline std::unique_ptr<AssociativeMemorySystemWrapper> createAssociativeMemoryWrapper(NeuralAssociativeMemory* memory) {
    return std::make_unique<AssociativeMemorySystemWrapper>(memory);
}

inline std::unique_ptr<PredictionSystemWrapper> createPredictionSystemWrapper(PredictionSystem* prediction) {
    return std::make_unique<PredictionSystemWrapper>(prediction);
}

inline std::unique_ptr<DevelopmentSystemWrapper> createDevelopmentSystemWrapper(DevelopmentSystem* devSystem) {
    return std::make_unique<DevelopmentSystemWrapper>(devSystem);
}

} // namespace nlm
