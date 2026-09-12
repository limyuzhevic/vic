// Brain cognition implementation - handles cognition system integration
#include "Brain.hpp"
#include "CognitionSystem.hpp"
#include <memory>

namespace nlm {

class BrainCognition : public Brain {
public:
    BrainCognition(std::shared_ptr<Config> config);
    ~BrainCognition() override;
    
    // Cognition system integration
    bool initializeCognitionSystems();
    void updateCognitionSystems(TimestepDuration dt);
    
    // Override brain methods
    bool initialize() override;
    void step(SimulationStep currentStep) override;
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    
    // Cognition system accessors
    CognitionSystem* getCognitionSystem();
    
private:
    std::unique_ptr<CognitionSystem> cognitionSystem;
};

} // namespace nlm
