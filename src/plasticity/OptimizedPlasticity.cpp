#include "OptimizedPlasticity.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Optimized Hebbian implementation
void OptimizedHebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, minWeight, maxWeight);
    synapse->setWeight(newWeight);
}

// Optimized STDP implementation  
void OptimizedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, minWeight, maxWeight);
    synapse->setWeight(newWeight);
}

// Optimized Structural Plasticity implementation
void OptimizedStructuralPlasticity::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Rebuild indices if needed
    if (indexEnabled && !isIndexValid(brain)) {
        rebuildIndices(brain);
    }
    
    // Optimized synaptogenesis
    synaptogenesisOptimized(brain, rng);
    
    // Optimized pruning
    pruningOptimized(brain, rng);
}

} // namespace nlm
