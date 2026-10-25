#include "MemorySystem.h"
#include "core/Logger/Logger.hpp"
#include "brain/NeuralRegion.hpp"
#include "neuromodulation/Neuromodulator.hpp"

namespace nlm {

class MemorySystemImpl : public MemorySystem {
private:
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
    
    size_t stepsSinceLastEpisode = 0;
    size_t replayInterval = 100;
    size_t consolidationInterval = 1000;
    
public:
    MemorySystemImpl() {
        NLM_LOG_INFO("MemorySystem initialized");
    }
    
    void initialize(Brain* brain) override {
        brain_ = brain;
        
        if (brain_) {
            workingMemory_ = std::make_unique<NeuralWorkingMemory>();
            episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
            associativeMemory_ = std::make_unique<NeuralAssociativeMemory>();
            
            if (workingMemory_) workingMemory_->initialize(brain_);
            if (episodicMemory_) episodicMemory_->initialize(brain_);
            if (associativeMemory_) associativeMemory_->initialize(brain_);
            
            NLM_LOG_INFO("Memory systems initialized successfully");
        } else {
            NLM_LOG_ERROR("MemorySystem: Cannot initialize null brain pointer");
        }
    }
    
    void updateWorkingMemory(TimestepDuration timestep) override {
        if (workingMemory_) {
            workingMemory_->update(timestep);
        }
    }
    
    void storeEpisodicMemory(const EpisodicMemoryItem& episode) override {
        if (episodicMemory_) {
            episodicMemory_->storeEpisode(episode);
        }
    }
    
    void updateEpisodicMemory(const BrainContext& context, SimulationStep currentStep,
                             const NeuromodulatorSignal& rewardSignal) override {
        stepsSinceLastEpisode++;
        
        if (stepsSinceLastEpisode >= replayInterval) {
            stepsSinceLastEpisode = 0;
            
            if (episodicMemory_) {
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = rewardSignal.level;
                
                // Capture current brain state (simplified)
                episodicMemory_->captureCurrentState(context, episode);
                
                episodicMemory_->storeEpisode(episode);
                NLM_LOG_DEBUG("Episodic memory updated with " + 
                             std::to_string(episode.activeNeurons.size()) + " active neurons");
            }
        }
    }
    
    void consolidateMemories(float consolidationThreshold) override {
        if (episodicMemory_) {
            episodicMemory_->consolidate(consolidationThreshold);
        }
    }
    
    void replayMemories(size_t maxEpisodes) override {
        if (episodicMemory_) {
            auto episodesToReplay = episodicMemory_->getEpisodesForReplay(maxEpisodes);
            for (const auto* episode : episodesToReplay) {
                episodicMemory_->replayEpisode(episode);
            }
        }
    }
    
    size_t getEpisodeCount() const override {
        return episodicMemory_ ? episodicMemory_->getEpisodeCount() : 0;
    }
    
    // Memory accessors
    NeuralWorkingMemory* getWorkingMemory() override {
        return workingMemory_.get();
    }
    
    NeuralEpisodicMemory* getEpisodicMemory() override {
        return episodicMemory_.get();
    }
    
    NeuralAssociativeMemory* getAssociativeMemory() override {
        return associativeMemory_.get();
    }
    
    // Utility methods
    size_t getStepsSinceLastEpisode() const {
        return stepsSinceLastEpisode;
    }
    
    void setReplayInterval(size_t interval) {
        replayInterval = interval;
    }
    
    void setConsolidationInterval(size_t interval) {
        consolidationInterval = interval;
    }
};

} // namespace nlm