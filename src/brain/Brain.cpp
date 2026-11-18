#include "Brain.hpp"
#include "SplitSystems.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <sstream>
#include <iomanip>

namespace nlm {

struct Brain::Impl {
    // Core simulation
    std::unique_ptr<SimulationCore> simulationCore;
    
    // System components
    std::unique_ptr<MemorySystem> memorySystem;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<CognitionSystem> cognitionSystem;
    std::unique_ptr<NeuromodulationSystem> neuromodulationSystem;
    std::unique_ptr<PlasticitySystem> plasticitySystem;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Neural structures
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    std::unordered_map<uint64_t, Neuron*> neuronById;
    bool spikeHandlersRegistered;
    
    // Phase 5 learned preference trait
    float learnedPreference = 0.0f;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg)
        : simulationCore(std::make_unique<SimulationCore>(cfg))
        , memorySystem(std::make_unique<MemorySystem>())
        , predictionSystem(std::make_unique<PredictionSystem>())
        , cognitionSystem(std::make_unique<CognitionSystem>())
        , neuromodulationSystem(std::make_unique<NeuromodulationSystem>())
        , plasticitySystem(std::make_unique<PlasticitySystem>())
        , developmentSystem(std::make_unique<DevelopmentSystem>())
        , regions(), interRegionConnections(), sensoryNeurons(), motorNeurons()
        , neuronById(), spikeHandlersRegistered(false)
        , learnedPreference(0.0f)
    {
        try {
            plasticitySystem->configurePlasticity(*cfg);
            simulationCore->configureFromConfig(*cfg);
            checkpointManager = std::make_unique<CheckpointManager>();
        } catch (const std::exception&) {
            throw;
        }
    }
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(nullptr) {
    try {
        pImpl = std::make_unique<Impl>(config);
    } catch (const std::bad_alloc&) {
        // Handle memory allocation failure
        pImpl = nullptr;
        throw;
    }
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(nullptr) {
    // Transfer ownership safely
    if (other.pImpl) {
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;  // Safe because pImpl is raw pointer
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}