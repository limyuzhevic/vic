// MemorySystemAdapter.cpp
#include "MemorySystemAdapter.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

MemorySystemAdapter::MemorySystemAdapter(
    std::unique_ptr<NeuralWorkingMemory> workingMemory,
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory,
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory,
    std::unique_ptr<PredictionSystem> predictionSystem,
    std::unique_ptr<NeuralPlanner> planner,
    std::unique_ptr<ConceptFormation> conceptFormation,
    std::unique_ptr<AttentionalSelection> attention,
    std::unique_ptr<DevelopmentSystem> developmentSystem,
    std::unique_ptr<Dopamine> dopamine,
    std::unique_ptr<Curiosity> curiosity,
    std::unique_ptr<Novelty> novelty,
    std::unique_ptr<PredictionError> predictionError)
    : workingMemory_(std::move(workingMemory))
    , episodicMemory_(std::move(episodicMemory))
    , associativeMemory_(std::move(associativeMemory))
    , predictionSystem_(std::move(predictionSystem))
    , planner_(std::move(planner))
    , conceptFormation_(std::move(conceptFormation))
    , attention_(std::move(attention))
    , developmentSystem_(std::move(developmentSystem))
    , dopamine_(std::move(dopamine))
    , curiosity_(std::move(curiosity))
    , novelty_(std::move(novelty))
    , predictionError_(std::move(predictionError))
    , developmentalStage_(DevelopmentalStage::Initial)
{
    NLM_LOG_INFO("MemorySystemAdapter initialized with unified interface");
}

} // namespace nlm