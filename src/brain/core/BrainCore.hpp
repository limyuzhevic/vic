#pragma once

// brain/core/BrainCore.hpp
// Core brain utilities and shared functionality
// This header provides the foundation for the modular brain architecture

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class NeuralRegion;

// Inter-regional connection structure (moved from Brain.hpp)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Core brain state management
class BrainCore {
public:
    // Core brain operations
    static bool validateConfiguration(const std::shared_ptr<const Config>& config);
    static std::string generateRegionName(size_t index);
    static void updateStatistics(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                size_t& totalSpikes,
                                size_t& totalSynapses);
    static float calculateExcitatoryInhibitionRatio(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    static size_t countActiveNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    // Memory system utilities
    static void initializeMemorySystems(std::unique_ptr<NeuralWorkingMemory>& workingMemory,
                                         std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
                                         std::unique_ptr<NeuralAssociativeMemory>& associativeMemory,
                                         const std::shared_ptr<const Config>& config,
                                         const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    // Prediction system utilities
    static void initializePredictionSystem(std::unique_ptr<PredictionSystem>& predictionSystem,
                                           const std::shared_ptr<const Config>& config);
    
    // Cognition system utilities
    static void initializeCognitionSystems(std::unique_ptr<NeuralPlanner>& planner,
                                           std::unique_ptr<ConceptFormation>& conceptFormation,
                                           std::unique_ptr<AttentionalSelection>& attention,
                                           const std::shared_ptr<const Config>& config,
                                           const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    // Neuromodulation system utilities
    static void initializeNeuromodulationSystems(std::unique_ptr<Dopamine>& dopamine,
                                                 std::unique_ptr<Curiosity>& curiosity,
                                                 std::unique_ptr<PredictionError>& predictionError,
                                                 std::unique_ptr<Novelty>& novelty,
                                                 const std::shared_ptr<const Config>& config);
    
    // Development system utilities
    static void initializeDevelopmentSystem(std::unique_ptr<DevelopmentSystem>& developmentSystem,
                                           const std::shared_ptr<const Config>& config);
    
    // Plasticity system utilities
    static void initializePlasticitySystems(std::unique_ptr<SpikeSystem>& spikeSystem,
                                           std::unique_ptr<STDP>& stdp,
                                           std::unique_ptr<Hebbian>& hebbian,
                                           std::unique_ptr<StructuralPlasticity>& structuralPlasticity,
                                           const std::shared_ptr<const Config>& config);
    
    // Checkpoint system utilities
    static void configureCheckpointSystem(std::unique_ptr<CheckpointManager>& checkpointManager,
                                         const std::shared_ptr<const Config>& config);
    
    // Sensory integration utilities
    static void distributeSensoryNeurons(std::vector<Neuron*>& sensoryNeurons,
                                        std::vector<Neuron*>& motorNeurons,
                                        const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    // Statistics utilities
    static std::string formatStatistics(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                       const std::shared_ptr<const Config>& config,
                                       size_t totalSpikes,
                                       size_t totalSynapses,
                                       const SpikeSystem* spikeSystem,
                                       const Dopamine* dopamine,
                                       DevelopmentalStage developmentalStage);
};

} // namespace nlm
