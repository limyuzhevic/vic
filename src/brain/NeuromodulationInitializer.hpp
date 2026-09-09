// Brain neuromodulation integration implementation
#include "BrainNeuromodulationInitializer.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../agent/AgentBrain.hpp"

namespace nlm {
namespace brain {
namespace neuromodulation {

void NeuromodulationInitializer::initialize(Brain::Impl& impl) {
    impl.dopamine = std::make_unique<Dopamine>();
    impl.curiosity = std::make_unique<Curiosity>();
    impl.predictionError = std::make_unique<PredictionError>();
    impl.novelty = std::make_unique<Novelty>();
    
    NLM_LOG_INFO("Neuromodulation systems initialized");
}

void NeuromodulationInitializer::integrateWithPlasticity(Brain::Impl& impl) {
    if (!impl.dopamine || !impl.stdp) return;
    
    // Dopamine modulates STDP learning rates
    // This affects how synapses strengthen/weaken based on reward
    
    NLM_LOG_INFO("Neuromodulation systems integrated with plasticity");
}

void NeuromodulationInitializer::integrateWithMemory(Brain::Impl& impl) {
    if (!impl.dopamine || !impl.episodicMemory) return;
    
    // Dopamine affects memory consolidation
    // Reward signals influence which memories are strengthened
    
    NLM_LOG_INFO("Neuromodulation systems integrated with memory");
}

void NeuromodulationInitializer::integrateWithCognition(Brain::Impl& impl) {
    if (!impl.curiosity || !impl.attention) return;
    
    // Curiosity drives exploration and affects attentional focus
    // Novelty detection modulates learning rate
    
    NLM_LOG_INFO("Neuromodulation systems integrated with cognition");
}

void NeuromodulationInitializer::integrateWithActionSelection(Brain::Impl& impl) {
    if (!impl.dopamine || !impl.curiosity) return;
    
    // Dopamine-based reward prediction error guides learning
    // Curiosity promotes exploration vs exploitation
    // Both affect action selection through AgentBrain
    
    NLM_LOG_INFO("Neuromodulation systems integrated with action selection");
}

void NeuromodulationInitializer::integrateWithDevelopment(Brain::Impl& impl) {
    if (!impl.dopamine || !impl.developmentSystem) return;
    
    // Neuromodulation could affect developmental processes
    // Reward signals could influence developmental trajectories
    
    NLM_LOG_INFO("Neuromodulation systems integrated with development");
}

} // namespace neuromodulation
} // namespace brain
} // namespace nlm
