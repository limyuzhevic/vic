#include "ComponentFactory.hpp"
#include "../Neuromodulation/Neuromodulator.hpp"
#include "../Neuromodulation/Dopamine.hpp"
#include "../Neuromodulation/Acetylcholine.hpp"
#include "../Neuromodulation/Norepinephrine.hpp"
#include "../Neuromodulation/Serotonin.hpp"
#include "../Neuromodulation/Curiosity.hpp"
#include "../Neuromodulation/PredictionError.hpp"
#include "../Neuromodulation/New.hpp"
#include "../Neuromodulation/Novelty.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/ParallelProcessing.hpp"
#include "../performance/EventQueue.hpp"
#include "../performance/SparseConnectivity.hpp"
#include "../performance/OptimizedBrain.hpp"
#include "../development/Synaptogenesis.hpp"
#include "../development/Maturation.hpp"
#include "../development/Pruning.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../environment/Environment.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../visualization/VisualizationInterface.hpp"
#include "../experments/ExperimentRunner.hpp"
#include "../experiments/Experiment.hpp"
#include "../experiments/Metrics.hpp"
#include "../experiments/Phase3Experiment.hpp"
#include "../experiments/Phase4Experiment.hpp"
#include "../experiments/Phase5Experiment.hpp"
#include "../experiments/Phase6IntegratedExperiment.hpp"
#include "../experiments/Phase6Demo.hpp"
#include "../experiments/Phase4Demo.hpp"
#include "../experiments/Phase3Demo.hpp"
#include "../benchmark.hpp"
#include "../main.hpp"

namespace nlm {

// Neuromodulator factory implementations
std::shared_ptr<Neuromodulator> ComponentFactory<Neuromodulator>::createNeuromodulator(
    const std::string& type,
    std::shared_ptr<Config> config,
    std::shared_ptr<Brain> brain,
    const std::unordered_map<std::string, std::string>& params) {
    
    // Normalize type string
    std::string normalizedType = type;
    for (auto& c : normalizedType) {
        c = std::tolower(c);
    }
    
    if (normalizedType == "dopamine") {
        auto neuromod = std::make_shared<Dopamine>();
        neuromod->setLevel(0.5f);
        neuromod->setDecayRate(0.01f);
        neuromod->initialize(brain.get());
        return neuromod;
    } else if (normalizedType == "acetylcholine") {
        auto neuromod = std::make_shared<Acetylcholine>();
        neuromod->setLevel(0.3f);
        neuromod->setDecayRate(0.015f);
        neuromod->initialize(brain.get());
        return neuromod;
    } else if (normalizedType == "norepinephrine") {
        auto neuromod = std::make_shared<Norepinephrine>();
        neuromod->setLevel(0.2f);
        neuromod->setDecayRate(0.012f);
        neuromod->initialize(brain.get());
        return neuromod;
    } else if (normalizedType == "serotonin") {
        auto neuromod = std::make_shared<Serotonin>();
        neuromod->setLevel(0.4f);
        neuromod->setDecayRate(0.01f);
        neuromod->initialize(brain.get());
        return neuromod;
    } else if (normalizedType == "curiosity") {
        auto neuromod = std::make_shared<Curiosity>();
        neuromod->setLevel(0.3f);
        neuromod->setDecayRate(0.008f);
        neuromod->initialize(brain.get());
        return neuromod;
    } else if (normalizedType == "predictionerror" || normalizedType == "prediction_error") {
        auto neuromod = std::make_shared<PredictionError>();
        neuromod->setLevel(0.2f);
        neuromod->setDecayRate(0.015f);
        neuromod->initialize(brain.get());
        return neuromod;
    } else if (normalizedType == "novelty") {
        auto neuromod = std::make_shared<Novelty>();
        neuromod->setLevel(0.3f);
        neuromod->setDecayRate(0.01f);
        neuromod->initialize(brain.get());
        return neuromod;
    } else if (normalizedType == "new" || normalizedType == "information") {
        auto neuromod = std::make_shared<New>();
        neuromod->setLevel(0.2f);
        neuromod->setDecayRate(0.012f);
        neuromod->initialize(brain.get());
        return neuromod;
    } else {
        NLM_LOG_WARNING("Unknown neuromodulator type: " + type);
        return nullptr;
    }
}

// Memory system factory implementations
std::shared_ptr<MemorySystem> ComponentFactory<MemorySystem>::createMemorySystem(
    const std::string& type,
    std::shared_ptr<Config> config,
    std::shared_ptr<Brain> brain,
    const std::unordered_map<std::string, std::string>& params) {
    
    // Normalize type string
    std::string normalizedType = type;
    for (auto& c : normalizedType) {
        c = std::tolower(c);
    }
    
    if (normalizedType == "working") {
        auto memory = std::make_shared<NeuralWorkingMemory>();
        memory->initialize(brain.get());
        
        size_t capacity = 1000;
        if (config && config->has("working_memory_capacity")) {
            if (auto opt = config->get<size_t>("working_memory_capacity")) {
                capacity = *opt;
            }
        }
        memory->setCapacity(capacity);
        
        return memory;
    } else if (normalizedType == "episodic") {
        auto memory = std::make_shared<NeuralEpisodicMemory>();
        memory->initialize(brain.get());
        
        size_t maxEpisodes = 1000;
        if (config && config->has("episodic_memory_max_episodes")) {
            if (auto opt = config->get<size_t>("episodic_memory_max_episodes")) {
                maxEpisodes = *opt;
            }
        }
        memory->setMaxEpisodes(maxEpisodes);
        
        return memory;
    } else if (normalizedType == "associative") {
        auto memory = std::make_shared<NeuralEpisodicMemory>(); // Using episodic for associative
        memory->initialize(brain.get());
        memory->setMaxEpisodes(500);
        
        // Set as associative mode
        if (config) {
            config->set("memory_type", "associative");
        }
        
        return memory;
    } else if (normalizedType == "semantic") {
        // TODO: Implement SemanticMemory
        NLM_LOG_WARNING("SemanticMemory not yet implemented");
        return nullptr;
    } else if (normalizedType == "procedural") {
        // TODO: Implement ProceduralMemory
        NLM_LOG_WARNING("ProceduralMemory not yet implemented");
        return nullptr;
    } else {
        NLM_LOG_WARNING("Unknown memory system type: " + type);
        return nullptr;
    }
}

// Cognitive function factory implementations
std::shared_ptr<CognitiveFunction> ComponentFactory<CognitiveFunction>::createCognitiveFunction(
    const std::string& type,
    std::shared_ptr<Config> config,
    std::shared_ptr<Brain> brain,
    const std::unordered_map<std::string, std::string>& params) {
    
    // Normalize type string
    std::string normalizedType = type;
    for (auto& c : normalizedType) {
        c = std::tolower(c);
    }
    
    if (normalizedType == "planner") {
        auto cognition = std::make_shared<NeuralPlanner>();
        cognition->initialize(brain.get());
        
        int planningDepth = 5;
        if (config && config->has("planning_depth")) {
            if (auto opt = config->get<int>("planning_depth")) {
                planningDepth = *opt;
            }
        }
        cognition->setPlanningDepth(planningDepth);
        
        return cognition;
    } else if (normalizedType == "conceptformation" || normalizedType == "concept_formation") {
        auto cognition = std::make_shared<ConceptFormation>();
        cognition->initialize(brain.get());
        
        return cognition;
    } else if (normalizedType == "attention" || normalizedType == "attentionalselection") {
        auto cognition = std::make_shared<AttentionalSelection>();
        cognition->initialize(brain.get());
        
        float inhibitionStrength = 0.5f;
        if (config && config->has("inhibition_strength")) {
            if (auto opt = config->get<float>("inhibition_strength")) {
                inhibitionStrength = *opt;
            }
        }
        cognition->setInhibitionStrength(inhibitionStrength);
        
        float excitationStrength = 1.5f;
        if (config && config->has("excitation_strength")) {
            if (auto opt = config->get<float>("excitation_strength")) {
                excitationStrength = *opt;
            }
        }
        cognition->setExcitationStrength(excitationStrength);
        
        return cognition;
    } else {
        NLM_LOG_WARNING("Unknown cognitive function type: " + type);
        return nullptr;
    }
}

// Generic component factory implementations
template<typename ComponentType>
std::shared_ptr<ComponentType> ComponentFactory<ComponentType>::createGenericComponent(
    const std::string& type,
    std::shared_ptr<Config> config,
    std::shared_ptr<Brain> brain,
    const std::unordered_map<std::string, std::string>& params) {
    
    NLM_LOG_WARNING("Generic component creation for type: " + type);
    return nullptr;
}

// Template instantiation
// Note: These will be used when actual component types are determined

} // namespace nlm
