#ifndef COGNITION_SYSTEM_H
#define COGNITION_SYSTEM_H

#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "brain/Brain.hpp"

namespace nlm {

class CognitionSystem {
public:
    virtual ~CognitionSystem() = default;
    
    virtual void initialize(Brain* brain) = 0;
    virtual void update(TimestepDuration timestep) = 0;
    
    virtual NeuralPlanner* getPlanner() = 0;
    virtual ConceptFormation* getConceptFormation() = 0;
    virtual AttentionalSelection* getAttention() = 0;
    
    virtual void applyAttentionToWorkingMemory() = 0;
    virtual void formConceptsFromActivity() = 0;
    virtual void planActions() = 0;
};

} // namespace nlm

#endif // COGNITION_SYSTEM_H