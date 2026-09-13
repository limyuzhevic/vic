#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

class BrainStatistics {
public:
    // Forward declaration for friendship
    friend class Brain;
    
    BrainStatistics() = default;
    ~BrainStatistics() = default;
    
    // Statistics collection
    void update(class Brain* brain);
    void reset();
    
    // Neural statistics
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // Memory statistics
    size_t getWorkingMemoryActiveTraces() const;
    size_t getEpisodicMemoryEpisodeCount() const;
    
    // Neuromodulation statistics
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionErrorLevel() const;
    
    // Development statistics
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Statistics logging
    std::string getStatisticsString(class Brain* brain) const;
    void logStatistics(class Brain* brain) const;
    
private:
    // Statistics storage
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    size_t neuronsProcessed;
    size_t synapsesProcessed;
    
    // Reference to brain for statistics gathering
    class Brain* brainRef;
};

} // namespace nlm