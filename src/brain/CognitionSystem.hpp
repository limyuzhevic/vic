// Cognition system component - handles planning, concept formation, and attention
#include "Brain.hpp"
#include <memory>

namespace nlm {

class CognitionSystem {
public:
    CognitionSystem(Brain& brain);
    ~CognitionSystem();
    
    // Initialize cognition systems
    bool initialize();
    
    // Update cognition systems
    void update(TimestepDuration dt);
    
    // Plan actions
    std::unique_ptr<class Action> planAction();
    
    // Form new concepts
    void formConcept(const class Observation& observation);
    
    // Focus attention
    void focusAttention(const class Observation& observation);
    
private:
    Brain& brain;
    std::unique_ptr<class NeuralPlanner> planner;
    std::unique_ptr<class ConceptFormation> conceptFormation;
    std::unique_ptr<class AttentionalSelection> attention;
};

} // namespace nlm
