#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {

class CognitiveDevelopment {
public:
    CognitiveDevelopment();
    ~CognitiveDevelopment();
    
    // Update cognitive capabilities based on development
    void update(Brain* brain, double age, double dt);
    
    // Develop working memory capacity
    void developWorkingMemory(Brain* brain, double age);
    
    // Develop planning capabilities
    void developPlanning(Brain* brain, double age);
    
    // Develop abstract thinking
    void developAbstractThinking(Brain* brain, double age);
    
    // Develop causal reasoning
    void developCausalReasoning(Brain* brain, double age);
    
    // Get current cognitive capabilities
    CognitiveCapabilities getCapabilities() const;
    
    // Check if a capability is developed to a threshold
    bool isCapabilityDeveloped(float current, float threshold) const;
    
private:
    struct Impl;
    Impl* pImpl;
    CognitiveCapabilities capabilities;
};

} // namespace nlm