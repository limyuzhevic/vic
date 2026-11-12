// BrainCognition.hpp - Cognition system access
#pragma once

#include "../core/Types/Types.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

class BrainCognition {
public:
    BrainCognition(Brain* brain);
    ~BrainCognition();
    
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    float getLearnedPreference() const;
    void setLearnedPreference(float preference);
    
    void logStatus() const;
    
private:
    Brain* brain_;
};

} // namespace nlm
