#include "SystemFactory.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>

namespace nlm {

std::unique_ptr<MemorySystem> SystemFactory::createMemorySystem(const std::string& type) {
    if (type == "working_memory") {
        return std::make_unique<NeuralWorkingMemory>();
    } else if (type == "episodic_memory") {
        return std::make_unique<NeuralEpisodicMemory>();
    } else if (type == "associative_memory") {
        return std::make_unique<NeuralAssociativeMemory>();
    }
    return nullptr;
}

std::unique_ptr<PredictionSystemBase> SystemFactory::createPredictionSystem(const std::string& type) {
    if (type == "prediction_system") {
        return std::make_unique<PredictionSystem>();
    }
    return nullptr;
}

std::unique_ptr<CognitiveSystem> SystemFactory::createCognitiveSystem(const std::string& type) {
    if (type == "neural_planner") {
        return std::make_unique<NeuralPlanner>();
    } else if (type == "concept_formation") {
        return std::make_unique<ConceptFormation>();
    } else if (type == "attentional_selection") {
        return std::make_unique<AttentionalSelection>();
    }
    return nullptr;
}

std::unique_ptr<NeuromodulatorBase> SystemFactory::createNeuromodulator(const std::string& type) {
    if (type == "dopamine") {
        return std::make_unique<Dopamine>();
    } else if (type == "curiosity") {
        return std::make_unique<Curiosity>();
    } else if (type == "novelty") {
        return std::make_unique<Novelty>();
    } else if (type == "prediction_error") {
        return std::make_unique<PredictionError>();
    }
    return nullptr;
}

std::unique_ptr<PlasticityRule> SystemFactory::createPlasticityRule(const std::string& type) {
    if (type == "stdp") {
        return std::make_unique<STDP>();
    } else if (type == "hebbian") {
        return std::make_unique<Hebbian>();
    } else if (type == "structural_plasticity") {
        return std::make_unique<StructuralPlasticity>();
    }
    return nullptr;
}

} // namespace nlm