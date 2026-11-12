// Brain cognition system integration - extracted from monolithic Brain.cpp
#include "Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Extract cognition system integration logic from Brain.cpp
// This includes neural planner, concept formation, and attentional selection integration

void Brain::updateCognitionSystems() {
    // Update neural planner
    if (pImpl->planner) {
        // Process current brain state for planning
        PlanningContext context;
        context.currentStep = pImpl->currentStep;
        context.currentTime = pImpl->currentTime;
        context.brainState = getBrainStateSnapshot();
        
        // Generate action plans based on current state
        auto plans = pImpl->planner->generatePlans(context);
        
        // Apply top-down influences from planning to attention
        if (!plans.empty() && pImpl->attention) {
            for (const auto& plan : plans) {
                // Apply planning bias to relevant neurons
                applyPlanningBias(plan, pImpl->attention.get());
            }
        }
    }
    
    // Update concept formation
    if (pImpl->conceptFormation) {
        // Process current neural activity for concept formation
        ConceptFormationContext context;
        context.timestamp = pImpl->currentTime;
        context.neuralActivity = getCurrentNeuralActivity();
        context.workingMemoryContent = getWorkingMemoryPattern();
        
        // Form or reinforce concepts based on current activity
        pImpl->conceptFormation->processContext(context);
        
        // Update attention based on formed concepts
        auto conceptNeurons = pImpl->conceptFormation->getConceptNeurons();
        if (!conceptNeurons.empty() && pImpl->attention) {
            pImpl->attention->processCompetition(conceptNeurons);
        }
    }
    
    // Update attentional selection
    if (pImpl->attention) {
        // Process competition and update attention focus
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to memory systems
        updateAttentionWithMemory();
        
        // Apply attention to planning systems
        updateAttentionWithPlanning();
    }
}

void Brain::applyPlanningBias(const class PlanningAction& plan, class AttentionalSelection* attention) {
    if (!attention) return;
    
    // Convert planning action to attention bias
    for (NeuronId neuronId : plan.requiredNeurons) {
        attention->applyTopDownBias(neuronId, plan.priority);
    }
}

void Brain::updateAttentionWithMemory() {
    if (!pImpl->attention || !pImpl->workingMemory) return;
    
    // Get working memory content
    std::vector<NeuronId> memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
    if (memoryNeurons.empty()) return;
    
    // Process memory-based competition for attention
    pImpl->attention->processCompetition(memoryNeurons);
}

void Brain::updateAttentionWithPlanning() {
    if (!pImpl->attention || !pImpl->planner) return;
    
    // Get current planning state
    auto currentPlans = pImpl->planner->getCurrentPlans();
    if (currentPlans.empty()) return;
    
    // Extract neurons relevant to current plans
    std::vector<NeuronId> plannedNeurons;
    for (const auto& plan : currentPlans) {
        plannedNeurons.insert(plannedNeurons.end(), 
                             plan.requiredNeurons.begin(), 
                             plan.requiredNeurons.end());
    }
    
    if (!plannedNeurons.empty()) {
        // Apply planning-based attention
        pImpl->attention->processCompetition(plannedNeurons);
    }
}

BrainStateSnapshot Brain::getBrainStateSnapshot() const {
    BrainStateSnapshot snapshot;
    snapshot.timestamp = pImpl->currentTime;
    snapshot.step = pImpl->currentStep;
    
    // Capture current neural activity
    for (const auto& region : pImpl->regions) {
        RegionSnapshot regionSnap;
        regionSnap.id = region->getId();
        regionSnap.name = region->getName();
        
        for (const auto* neuron : region->getAllNeurons()) {
            NeuronSnapshot neuronSnap;
            neuronSnap.id = neuron->getId();
            neuronSnap.type = neuron->getType();
            neuronSnap.membranePotential = neuron->getMembranePotential();
            neuronSnap.firingRate = neuron->getFiringRate();
            neuronSnap.isFiring = neuron->isFiring();
            
            regionSnap.neurons.push_back(neuronSnap);
        }
        
        snapshot.regions.push_back(regionSnap);
    }
    
    return snapshot;
}

NeuralActivityPattern Brain::getCurrentNeuralActivity() const {
    NeuralActivityPattern pattern;
    pattern.timestamp = pImpl->currentTime;
    
    for (const auto& region : pImpl->regions) {
        for (const auto* neuron : region->getAllNeurons()) {
            if (neuron->isFiring() || neuron->getFiringRate() > 0.1f) {
                NeuralActivityActivity activity;
                activity.neuronId = neuron->getId();
                activity.firingRate = neuron->getFiringRate();
                activity.membranePotential = neuron->getMembranePotential();
                
                pattern.activities.push_back(activity);
            }
        }
    }
    
    return pattern;
}

std::vector<float> Brain::getWorkingMemoryPattern() const {
    if (!pImpl->workingMemory) return std::vector<float>();
    
    return pImpl->workingMemory->retrieve();
}

} // namespace nlm