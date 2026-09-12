// Neuromodulation system component - handles dopamine, curiosity, novelty, prediction error
#include "Brain.hpp"
#include <memory>

namespace nlm {

class NeuromodulationSystem {
public:
    NeuromodulationSystem(Brain& brain);
    ~NeuromodulationSystem();
    
    // Initialize neuromodulation systems
    bool initialize();
    
    // Update all neuromodulators
    void update(TimestepDuration dt);
    
    // Get current neuromodulator levels
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionErrorLevel() const;
    
    // Apply neuromodulator to brain
    void applyToBrain();
    
private:
    Brain& brain;
    std::unique_ptr<class Dopamine> dopamine;
    std::unique_ptr<class Curiosity> curiosity;
    std::unique_ptr<class Novelty> novelty;
    std::unique_ptr<class PredictionError> predictionError;
};

} // namespace nlm
