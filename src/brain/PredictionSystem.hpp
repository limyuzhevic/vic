// Prediction system component - handles prediction, error computation, and action selection
#include "Brain.hpp"
#include <memory>

namespace nlm {

class PredictionSystem {
public:
    PredictionSystem(Brain& brain);
    ~PredictionSystem();
    
    // Initialize prediction system
    bool initialize();
    
    // Update prediction system
    void update(TimestepDuration dt);
    
    // Make prediction based on current state
    std::unique_ptr<class Prediction> predict();
    
    // Update with prediction error
    void updateWithError(const class PredictionError& error);
    
private:
    Brain& brain;
    std::unique_ptr<class Prediction> currentPrediction;
    std::unique_ptr<class PredictionError> predictionError;
};

} // namespace nlm
