// Step 10: Update concept formation
// This module updates concept formation with current neural activity patterns
// Extracts patterns and forms abstract concepts from sensory experience

#pragma once

#include <memory>
#include <vector>
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step10 {

// Helper function to process current neural patterns for concept formation
void ProcessNeuralPatternsForConcepts(Brain::Impl* impl,
                                      Timestamp currentTime);

// Helper function to update concept prototypes
void UpdateConceptPrototypes(Brain::Impl* impl,
                             Timestamp currentTime);

// Update concept formation system
void UpdateConceptFormation(Brain::Impl* impl,
                           SimulationStep currentStep,
                           Timestamp currentTime);

} // namespace Step10

} // namespace nlm