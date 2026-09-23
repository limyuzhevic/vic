#pragma once

#include "../core/Types/Types.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class PredictionSystem;
class AttentionalSelection;
class ConceptFormation;
class Novelty;
class Curiosity;
class Dopamine;
class DevelopmentSystem;
class Neuron;
class Synapse;

class BrainSimulation {
public:
    // ===========================================
    // SIMULATION LOOP MANAGEMENT
    // ===========================================
    
    static void step(Brain* brain, SimulationStep currentStep);
    static void step(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // ===========================================
    // UTILITY FUNCTIONS FOR SIMULATION
    // ===========================================
    
    static void updateNeurons(Brain* brain, Timestamp currentTime, TimestepDuration dt);
    static void processSpikes(Brain* brain, SimulationStep currentStep, SpikeSystem* spikeSystem);
    static void updateWorkingMemory(NeuralWorkingMemory* workingMemory, TimestepDuration dt);
    static void updateNovelty(Novelty* novelty, Timestamp currentTime, TimestepDuration dt);
    static void updateCuriosity(Curiosity* curiosity, Timestamp currentTime, TimestepDuration dt);
    static void applyPlasticityRules(Brain* brain, Timestamp currentTime, TimestepDuration dt);
    static void updateEpisodicMemory(NeuralEpisodicMemory* episodicMemory, size_t currentStep, float reward);
    static void updatePredictionSystem(PredictionSystem* predictionSystem);
    static void updateAttentionSystem(AttentionalSelection* attention,
                                      NeuralWorkingMemory* workingMemory,
                                      Timestamp currentTime, TimestepDuration dt);
    static void updateConceptFormation(ConceptFormation* conceptFormation, const std::vector<Neuron*>& neurons);
    static void applyStructuralPlasticity(Brain* brain);
    static void replayMemories(NeuralEpisodicMemory* episodicMemory, size_t replayCount);
    static void consolidateMemories(NeuralEpisodicMemory* episodicMemory, float consolidationRate);
    static void applyDevelopmentEffects(Brain* brain);
    static void applyDopamineToNeurons(Dopamine* dopamine, const std::vector<Neuron*>& neurons);
};

} // namespace nlm