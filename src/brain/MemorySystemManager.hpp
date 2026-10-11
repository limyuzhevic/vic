#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Forward declarations
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;

class MemorySystemManager {
public:
    MemorySystemManager();
    ~MemorySystemManager();
    
    // Working memory updates
    void updateWorkingMemory(TimestepDuration dt);
    void storeToWorkingMemory(NeuronId neuronId, float activation);
    void updateWorkingMemoryFromNeurons(const std::vector<Neuron*>& neurons);
    
    // Episodic memory storage
    void storeEpisodicMemory(SimulationStep step, Timestamp time, 
                            const std::vector<NeuronId>& activeNeurons,
                            const std::vector<float>& activations,
                            float reward);
    void replayEpisodicMemory(size_t count);
    void consolidateEpisodicMemory(float threshold);
    
    // Memory consolidation and replay
    void consolidateMemories(float threshold);
    void replayImportantMemories(size_t count, SimulationStep currentStep);
    
    // Memory competition and winner selection
    void processMemoryCompetition();
    NeuronId selectMemoryWinner();
    
    // Integration with memory systems
    void setWorkingMemory(NeuralWorkingMemory* workingMemory) { 
        this->workingMemory = workingMemory; 
    }
    void setEpisodicMemory(NeuralEpisodicMemory* episodicMemory) { 
        this->episodicMemory = episodicMemory; 
    }
    void setAssociativeMemory(NeuralAssociativeMemory* associativeMemory) { 
        this->associativeMemory = associativeMemory; 
    }
    
    void setPredictionSystem(PredictionSystem* predictionSystem) { 
        this->predictionSystem = predictionSystem; 
    }
    
    void setPlanner(NeuralPlanner* planner) { this->planner = planner; }
    void setConceptFormation(ConceptFormation* conceptFormation) { 
        this->conceptFormation = conceptFormation; 
    }
    void setAttention(AttentionalSelection* attention) { 
        this->attention = attention; 
    }
    
    // Statistics
    size_t getActiveWorkingMemoryTraces() const { 
        return workingMemory ? workingMemory->getActiveTraces() : 0; 
    }
    size_t getEpisodicMemoryCount() const { 
        return episodicMemory ? episodicMemory->getEpisodeCount() : 0; 
    }
    
private:
    // Memory system instances
    NeuralWorkingMemory* workingMemory;
    NeuralEpisodicMemory* episodicMemory;
    NeuralAssociativeMemory* associativeMemory;
    
    // Associated systems
    PredictionSystem* predictionSystem;
    NeuralPlanner* planner;
    ConceptFormation* conceptFormation;
    AttentionalSelection* attention;
    
    // Memory state
    std::vector<NeuronId> recentActiveNeurons;
    std::vector<float> recentActivations;
    SimulationStep lastEpisodeStep;
};

} // namespace nlm
