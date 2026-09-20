// PredictionError implementation - reward prediction error system
struct PredictionError::Impl {
    // Current prediction error value
    float currentError;
    
    // Prediction history
    std::vector<float> predictionHistory;
    std::vector<float> actualHistory;
    
    // Prediction error history
    std::vector<float> errorHistory;
    
    // Brain reference for integration
    class Brain* brain;
    
    // Prediction algorithm parameters
    float learningRate;
    float predictionHorizon;
    
    // Error magnitude and direction
    float errorMagnitude;
    float errorDirection;  // 1.0 = positive, -1.0 = negative
    
    // Credit assignment eligibility
    std::unordered_map<uint64_t, float> creditAssignment;
    
    Impl() : 
        currentError(0.0f),
        learningRate(0.1f),
        predictionHorizon(1.0f),
        errorMagnitude(0.0f),
        errorDirection(0.0f),
        brain(nullptr) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {
    NLM_LOG_INFO("Prediction Error system initialized");
}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(class Brain* brain) {
    pImpl->brain = brain;
    
    // Configure based on brain parameters
    if (brain) {
        auto config = brain->getConfig();
        if (config) {
            pImpl->learningRate = config->getOr<float>("prediction_learning_rate", 0.1f);
            pImpl->predictionHorizon = config->getOr<float>("prediction_horizon", 1.0f);
        }
    }
    
    NLM_LOG_INFO("Prediction Error system initialized with learning rate: " + std::to_string(pImpl->learningRate));
}

float PredictionError::getError() const {
    return pImpl->currentError;
}

void PredictionError::computeError(float predicted, float actual) {
    // Compute prediction error
    pImpl->currentError = actual - predicted;
    
    // Update history
    pImpl->predictionHistory.push_back(predicted);
    pImpl->actualHistory.push_back(actual);
    pImpl->errorHistory.push_back(pImpl->currentError);
    
    // Calculate error magnitude and direction
    pImpl->errorMagnitude = std::abs(pImpl->currentError);
    pImpl->errorDirection = (pImpl->currentError >= 0.0f) ? 1.0f : -1.0f;
    
    // Apply credit assignment for temporal difference
    float tdError = (actual - predicted) * pImpl->predictionHorizon;
    
    // Accumulate credit assignment for neurons
    if (pImpl->brain) {
        auto regions = pImpl->brain->getRegions();
        for (auto& region : regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron && neuron->isActive()) {
                    // Apply prediction error to neuron activation
                    float neuronCredit = tdError * pImpl->errorDirection;
                    pImpl->creditAssignment[neuron->getId().value] += neuronCredit;
                }
            }
        }
    }
    
    NLM_LOG_DEBUG("Prediction Error: predicted=" + std::to_string(predicted) +
                  ", actual=" + std::to_string(actual) +
                  ", error=" + std::to_string(pImpl->currentError) +
                  ", magnitude=" + std::to_string(pImpl->errorMagnitude));
}

void PredictionError::updatePrediction(float newPrediction) {
    // Update prediction with new value (learning)
    float oldPrediction = pImpl->predictionHistory.empty() ? 0.0f : 
                         pImpl->predictionHistory.back();
    
    // Update prediction using learning rate
    float newValue = oldPrediction + pImpl->learningRate * (newPrediction - oldPrediction);
    pImpl->predictionHistory.push_back(newValue);
    
    NLM_LOG_DEBUG("Prediction updated from " + std::to_string(oldPrediction) + 
                  " to " + std::to_string(newValue));
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->errorHistory;
}

void PredictionError::clearHistory() {
    pImpl->predictionHistory.clear();
    pImpl->actualHistory.clear();
    pImpl->errorHistory.clear();
    pImpl->creditAssignment.clear();
    
    NLM_LOG_DEBUG("Prediction Error history cleared");
}

float PredictionError::getMagnitude() const {
    return pImpl->errorMagnitude;
}

} // namespace nlm
