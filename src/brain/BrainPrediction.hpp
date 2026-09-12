// Brain prediction implementation - handles prediction system integration
#include "Brain.hpp"
#include "PredictionSystem.hpp"
#include <memory>

namespace nlm {

class BrainPrediction : public Brain {
public:
    BrainPrediction(std::shared_ptr<Config> config);
    ~BrainPrediction() override;
    
    // Prediction system integration
    bool initializePredictionSystems();
    void updatePredictionSystems(TimestepDuration dt);
    
    // Override brain methods
    bool initialize() override;
    void step(SimulationStep currentStep) override;
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    
    // Prediction system accessors
    PredictionSystem* getPredictionSystem();
    
private:
    std::unique_ptr<PredictionSystem> predictionSystem;
};

} // namespace nlm
