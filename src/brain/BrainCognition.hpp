#pragma once

#include "../core/Types/Types.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class Neuron;

class BrainCognition {
public:
    // ===========================================
    // NEURAL PLANNER COGNITION
    // ===========================================
    
    static void initializeNeuralPlanner(NeuralPlanner* planner, Brain* brain);
    static void updateNeuralPlanner(NeuralPlanner* planner, size_t planningDepth);
    static void executePlan(NeuralPlanner* planner, const std::vector<Neuron*>& neurons);
    
    // ===========================================
    // CONCEPT FORMATION COGNITION
    // ===========================================
    
    static void initializeConceptFormation(ConceptFormation* conceptFormation, Brain* brain);
    static void updateConceptFormation(ConceptFormation* conceptFormation, const std::vector<Neuron*>& neurons);
    static void retrieveConcepts(ConceptFormation* conceptFormation, std::vector<ConceptId>& concepts);
    
    // ===========================================
    // ATTENTION COGNITION
    // ===========================================
    
    static void initializeAttentionalSelection(AttentionalSelection* attention, Brain* brain);
    static void updateAttentionalSelection(AttentionalSelection* attention, const std::vector<Neuron*>& neurons);
    static void applyAttentionToNeurons(AttentionalSelection* attention, std::vector<Neuron*>& neurons);
    
    // ===========================================
    // COGNITION STATISTICS AND ACCESSORS
    // ===========================================
    
    static void logCognitionStatus(NeuralPlanner* planner,
                                   ConceptFormation* conceptFormation,
                                   AttentionalSelection* attention);
};

} // namespace nlm