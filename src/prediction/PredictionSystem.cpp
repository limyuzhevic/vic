PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Just return a copy of current state
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
    }
}

// New methods for integration with Brain loop
void PredictionSystem::updateObservation(const std::vector<float>& observation) {
    // Store observation for prediction
    lastObservation_ = observation;
    
    // Generate prediction from observation
    predictFromObservation(observation);
}

void PredictionSystem::updatePrediction(const std::vector<float>& neuralState) {
    // Use neural state to update prediction
    // This enables predictive coding where predictions are driven by neural activity
    
    // Predict next sensory state based on neural state
    // Use simple Hebbian-like learning to strengthen associations
    if (lastObservation_.size() == neuralState.size() && !lastObservation_.empty()) {
        for (size_t i = 0; i < lastObservation_.size(); ++i) {
            // Prediction error drives learning
            float predictionError = neuralState[i] - lastObservation_[i];
            
            // Update prediction weights based on error
            predictionWeight_[i] = predictionWeight_[i] * 0.95f + 
                                neuralState[i] * 0.05f + predictionError * 0.1f;
        }
        
        // Recalculate prediction error
        pImpl->predictionError = 0.0f;
        for (size_t i = 0; i < lastObservation_.size(); ++i) {
            float diff = predictionWeight_[i] - lastObservation_[i];
            pImpl->predictionError += diff * diff;
        }
        pImpl->predictionError /= lastObservation_.size();
        
        // Update confidence based on prediction error
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

// Get last observation
const std::vector<float>& PredictionSystem::getLastObservation() const {
    return lastObservation_;
}

// Get prediction weight vector  
const std::vector<float>& PredictionSystem::getPredictionWeights() const {
    return predictionWeight_;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
    // For now, update observation and predict
    const auto& obsData = observation.getData();
    if (!obsData.empty()) {
        std::vector<float> obsVec(obsData.size());
        for (size_t i = 0; i < obsData.size(); ++i) {
            obsVec[i] = static_cast<float>(obsData[i]);
        }
        updateObservation(obsVec);
    }
}
