#pragma once

#include "../core/Types/Types.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/AssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
class Config;
class RandomGenerator;
class SpikeSystem;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class STDP;
class Hebbian;
class StructuralPlasticity;
class SensoryInput;
class Action;
class NeuralRegion;
class RegionId;

class BrainIntegration {
public:
    // ===========================================
    // BRAIN CONSTRUCTION AND LIFECYCLE
    // ===========================================
    
    static void constructBrain(Brain* brain, std::shared_ptr<Config> config);
    static void destructBrain(Brain* brain);
    static void initializeBrain(Brain* brain);
    static void resetBrain(Brain* brain);
    
    // ===========================================
    // SENSORY INPUT AND MOTOR OUTPUT
    // ===========================================
    
    static void receiveSensoryInput(Brain* brain, const SensoryInput& input);
    static void injectCurrent(Brain* brain, NeuronId neuron, MembranePotential current);
    static void injectCurrentToNeurons(Brain* brain, NeuronType type, MembranePotential current);
    static void processSensoryInput(Brain* brain, const SensoryPercept& percept);
    
    // ===========================================
    // ACCESSORS AND STATISTICS
    // ===========================================
    
    static void logBrainStatus(Brain* brain);
    
    // ===========================================
    // NEURAL SYSTEM ACCESSORS
    // ===========================================
    
    static SpikeSystem* getSpikeSystem(Brain* brain);
    static STDP* getSTDP(Brain* brain);
    static Hebbian* getHebbian(Brain* brain);
    static StructuralPlasticity* getStructuralPlasticity(Brain* brain);
    
    // ===========================================
    // MEMORY SYSTEM ACCESSORS
    // ===========================================
    
    static NeuralWorkingMemory* getWorkingMemory(Brain* brain);
    static NeuralEpisodicMemory* getEpisodicMemory(Brain* brain);
    static NeuralAssociativeMemory* getAssociativeMemory(Brain* brain);
    
    // ===========================================
    // PREDICTION SYSTEM ACCESSOR
    // ===========================================
    
    static PredictionSystem* getPredictionSystem(Brain* brain);
    
    // ===========================================
    // COGNITION SYSTEM ACCESSORS
    // ===========================================
    
    static NeuralPlanner* getPlanner(Brain* brain);
    static ConceptFormation* getConceptFormation(Brain* brain);
    static AttentionalSelection* getAttention(Brain* brain);
    
    // ===========================================
    // DEVELOPMENT SYSTEM ACCESSORS
    // ===========================================
    
    static DevelopmentSystem* getDevelopmentSystem(Brain* brain);
    static DevelopmentalStage getDevelopmentalStage(Brain* brain);
    
    // ===========================================
    // NEUROMODULATION SYSTEM ACCESSORS
    // ===========================================
    
    static Dopamine* getDopamine(Brain* brain);
    static Curiosity* getCuriosity(Brain* brain);
    static Novelty* getNovelty(Brain* brain);
    static PredictionError* getPredictionError(Brain* brain);
    
    // ===========================================
    // CONFIGURATION AND UTILITIES
    // ===========================================
    
    static std::shared_ptr<const Config> getConfig(Brain* brain);
    static RandomGenerator* getRandomGenerator(Brain* brain);
    
    // ===========================================
    // ACTION PRODUCTION AND MOTOR DECODING
    // ===========================================
    
    static std::unique_ptr<Action> produceAction(Brain* brain);
    static void applyNeuromodulation(Brain* brain, const Neuromodulator& signal);
    static void updatePlasticity(Brain* brain);
    static void develop(Brain* brain);
    
    // ===========================================
    // STATE PERSISTENCE
    // ===========================================
    
    static bool saveBrainState(Brain* brain, const std::string& filepath);
    static bool loadBrainState(Brain* brain, const std::string& filepath);
    
    // ===========================================
    // NEURAL NETWORK STATISTICS
    // ===========================================
    
    static size_t getTotalNeuronCount(Brain* brain);
    static size_t getTotalSynapseCount(Brain* brain);
    static size_t getActiveNeuronCount(Brain* brain);
    static size_t getFiringNeuronCount(Brain* brain);
    static float getAverageFiringRate(Brain* brain);
    static float getExcitationInhibitionRatio(Brain* brain);
    static size_t getTotalSpikeCount(Brain* brain);
    static size_t getPendingSpikeEventCount(Brain* brain);
    
    // ===========================================
    // REGION MANAGEMENT
    // ===========================================
    
    static RegionId addRegion(Brain* brain, const std::string& name);
    static NeuralRegion* getRegion(Brain* brain, RegionId id);
    static size_t getRegionCount(Brain* brain);
    static std::vector<RegionId> getRegionIds(Brain* brain);
    static const std::vector<std::unique_ptr<NeuralRegion>>& getRegions(Brain* brain);
};

} // namespace nlm