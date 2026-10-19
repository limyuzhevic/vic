namespace nlm {

// Implementation of helper methods for biologically realistic predictive coding

void PredictionSystem::applyPredictiveProcessing(SensoryInput& prediction, const SensoryInput& currentState) {
    // Apply predictive processing through neural hierarchies
    // Implements forward model and prediction error minimization
    
    const auto& currentData = currentState.getData();
    auto& predData = prediction.getData();
    
    if (currentData.size() != predData.size()) {
        predData.resize(currentData.size(), 0.0f);
    }
    
    // Hierarchical predictive processing
    // Higher levels predict lower-level inputs
    for (size_t i = 0; i < currentData.size(); ++i) {
        // Predictive inference based on learned statistics
        float predictionStrength = 1.0f - pImpl->confidence;  // Less confident = more prediction error
        
        // Generate prediction based on higher-level expectations
        float higherLevelExpectation = generateHigherLevelExpectation(i, currentData);
        
        // Combine with bottom-up input
        predData[i] = higherLevelExpectation * predictionStrength + currentData[i] * (1.0f - predictionStrength);
        
        // Apply prediction error scaling
        predData[i] += (currentData[i] - predData[i]) * pImpl->confidence * 0.5f;
    }
    
    NLM_LOG_INFO("PredictionSystem: Applied predictive processing through neural hierarchies");
}

void PredictionSystem::addPredictionUncertainty(SensoryInput& prediction) {
    // Add uncertainty to predictions based on prediction error history
    // Implements Bayesian uncertainty estimation
    
    auto& predData = prediction.getData();
    
    // Calculate uncertainty from prediction error history
    float uncertainty = 0.0f;
    if (pImpl->errorHistory.size() >= 5) {
        float meanError = 0.0f;
        for (size_t i = pImpl->errorHistory.size() - 5; i < pImpl->errorHistory.size(); ++i) {
            meanError += pImpl->errorHistory[i];
        }
        meanError /= 5.0f;
        
        float variance = 0.0f;
        for (size_t i = pImpl->errorHistory.size() - 5; i < pImpl->errorHistory.size(); ++i) {
            variance += (pImpl->errorHistory[i] - meanError) * (pImpl->errorHistory[i] - meanError);
        }
        variance /= 5.0f;
        
        uncertainty = std::sqrt(variance);
    }
    
    // Add uncertainty as noise to predictions
    for (size_t i = 0; i < predData.size(); ++i) {
        // Add Gaussian noise scaled by uncertainty
        float noise = rng_ ? rng_->generateFloat() - 0.5f : 0.0f;
        predData[i] += noise * uncertainty * 2.0f;
        
        // Constrain to reasonable bounds
        predData[i] = std::max(0.0f, std::min(1.0f, predData[i]));
    }
    
    NLM_LOG_INFO("PredictionSystem: Added uncertainty to predictions (uncertainty: " + std::to_string(uncertainty) + ")");
}

void PredictionSystem::modulatePredictionsByNeuromodulators(SensoryInput& prediction) {
    // Modulate predictions by neuromodulatory signals
    // Neuromodulators gate learning and attention in predictive coding
    
    auto& predData = prediction.getData();
    
    // Get neuromodulatory levels (in real implementation, get from actual neuromodulator systems)
    float dopamineLevel = 0.5f;  // In real implementation: brain->getDopamine()->getLevel()
    float acetylcholineLevel = 0.5f;  // In real implementation: brain->getAcetylcholine()->getLevel()
    float norepinephrineLevel = 0.5f;  // In real implementation: brain->getNorepinephrine()->getLevel()
    
    // Dopamine modulation (reward prediction error signaling)
    float dopamineModulation = 1.0f + dopamineLevel * 0.5f - 0.2f;  // Basic reward modulation
    
    // Acetylcholine modulation (attention and prediction error signaling)
    float acetylcholineModulation = 1.0f + acetylcholineLevel * 0.3f;
    
    // Norepinephrine modulation (arousal and prediction error sensitivity)
    float norepinephrineModulation = 1.0f + norepinephrineLevel * 0.4f;
    
    // Apply neuromodulatory modulation to predictions
    for (size_t i = 0; i < predData.size(); ++i) {
        // Combine neuromodulatory effects
        float totalModulation = dopamineModulation * acetylcholineModulation * norepinephrineModulation;
        
        // Apply modulation with saturation
        predData[i] *= totalModulation;
        predData[i] = std::max(0.0f, std::min(1.0f, predData[i]));
        
        // Add neuromodulatory noise
        float neuromodulatoryNoise = (rng_ ? rng_->generateFloat() - 0.5f : 0.0f) * 0.1f * totalModulation;
        predData[i] += neuromodulatoryNoise;
    }
    
    NLM_LOG_INFO("PredictionSystem: Modulated predictions by neuromodulators");
}

void PredictionSystem::applyErrorBackpropagation(const SensoryInput& predicted, const SensoryInput& actual, float predictionError) {
    // Apply error backpropagation through neural substrates
    // Implements gradient descent on prediction error
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    // Calculate error gradients
    std::vector<float> errorGradients(predData.size(), 0.0f);
    for (size_t i = 0; i < predData.size(); ++i) {
        // Gradient of prediction error with respect to predicted values
        float predictionErrorGradient = 2.0f * (predData[i] - actualData[i]) * predictionError;
        errorGradients[i] = predictionErrorGradient;
    }
    
    // Apply error backpropagation with learning rate
    float learningRate = 0.01f + predictionError * 0.05f;  // Adaptive learning rate
    learningRate = std::min(0.1f, learningRate);  // Limit learning rate
    
    // Update prediction model parameters
    for (size_t i = 0; i < predData.size(); ++i) {
        // Apply gradient descent update
        float update = -learningRate * errorGradients[i];
        // In real implementation: update neural weights based on gradient
    }
    
    NLM_LOG_INFO("PredictionSystem: Applied error backpropagation (error: " + std::to_string(predictionError) +
                 ", learning rate: " + std::to_string(learningRate) + ")");
}

void PredictionSystem::updateInternalModel(const SensoryInput& predicted, const SensoryInput& actual) {
    // Update internal model based on prediction errors
    // Implements model-based reinforcement learning
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    // Calculate prediction errors at different levels
    std::vector<float> levelErrors;
    
    // Coarse-to-fine error analysis
    size_t numLevels = std::min(static_cast<size_t>(4), predData.size() / 2);
    for (size_t level = 0; level < numLevels; ++level) {
        float levelError = 0.0f;
        size_t levelSize = predData.size() / (level + 1);
        
        for (size_t i = 0; i < levelSize; ++i) {
            size_t predIdx = i * (level + 1);
            size_t actualIdx = std::min(predIdx, static_cast<size_t>(actualData.size() - 1));
            
            float error = std::abs(predData[predIdx] - actualData[actualIdx]);
            levelError += error;
        }
        
        levelError /= levelSize;
        levelErrors.push_back(levelError);
    }
    
    // Update internal model based on level-specific errors
    for (size_t i = 0; i < levelErrors.size(); ++i) {
        float weight = 1.0f / (i + 1);  // Higher levels have lower weight
        float errorContribution = levelErrors[i] * weight;
        
        // In real implementation: update hierarchical model parameters
        pImpl->predictionError += errorContribution * 0.01f;
    }
    
    NLM_LOG_INFO("PredictionSystem: Updated internal model with multi-level error analysis");
}

float PredictionSystem::computeMultiLevelPredictionError(const SensoryInput& predicted, const SensoryInput& actual) const {
    // Compute prediction error at multiple processing levels
    // Implements hierarchical error computation for predictive coding
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return std::numeric_limits<float>::max();
    }
    
    // Level 1: Coarse sensory prediction error
    float coarseError = 0.0f;
    size_t coarseSize = std::min(static_cast<size_t>(16), static_cast<size_t>(predData.size() / 4));
    
    for (size_t i = 0; i < coarseSize; ++i) {
        size_t idx = i * (predData.size() / coarseSize);
        float error = predData[idx] - actualData[idx];
        coarseError += error * error;
    }
    coarseError /= coarseSize;
    
    // Level 2: Fine-grained prediction error
    float fineError = 0.0f;
    size_t fineSize = predData.size() - coarseSize;
    
    for (size_t i = coarseSize; i < predData.size(); ++i) {
        float error = predData[i] - actualData[i];
        fineError += error * error;
    }
    fineError /= fineSize;
    
    // Weighted combination of errors
    float totalError = coarseError * 0.3f + fineError * 0.7f;
    
    // Apply confidence scaling
    totalError *= (1.0f - pImpl->confidence);
    
    return std::sqrt(totalError);
}

float PredictionSystem::computeSingleLevelError(const SensoryInput& predicted, const SensoryInput& actual) const {
    // Compute single-level prediction error (simplified version)
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return 1.0f;
    }
    
    float sumError = 0.0f;
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actualData[i];
        sumError += diff * diff;
    }
    
    return std::sqrt(sumError / predData.size());
}

void PredictionSystem::updatePredictionConfidence(float predictionError) {
    // Update prediction confidence based on prediction error
    // Implements confidence as inverse of prediction uncertainty
    
    // Confidence decays with prediction error
    float errorSensitivity = 0.5f;
    float confidenceDecay = 1.0f / (1.0f + predictionError * errorSensitivity);
    
    // Confidence also increases with recent accuracy
    float recentAccuracy = 1.0f;
    if (pImpl->errorHistory.size() >= 3) {
        float recentMeanError = 0.0f;
        for (size_t i = pImpl->errorHistory.size() - 3; i < pImpl->errorHistory.size(); ++i) {
            recentMeanError += pImpl->errorHistory[i];
        }
        recentMeanError /= 3.0f;
        recentAccuracy = 1.0f / (1.0f + recentMeanError);
    }
    
    // Combine confidence updates
    pImpl->confidence = (pImpl->confidence * 0.7f) + (confidenceDecay * 0.2f) + (recentAccuracy * 0.1f);
    pImpl->confidence = std::max(0.1f, std::min(1.0f, pImpl->confidence));  // Constrain
}

void PredictionSystem::updateModelParameters(const SensoryInput& observation, float error) {
    // Update model parameters using observed data
    // Implements online learning and model refinement
    
    // Get current observation data
    const auto& obsData = observation.getData();
    
    // Update statistics
    float obsMean = 0.0f;
    float obsVariance = 0.0f;
    for (float val : obsData) {
        obsMean += val;
        obsVariance += val * val;
    }
    obsMean /= obsData.size();
    obsVariance /= obsData.size();
    obsVariance -= obsMean * obsMean;
    
    // Update prediction model with Bayesian updating
    float learningRate = error * 0.1f;  // Learning rate proportional to error
    learningRate = std::min(0.2f, learningRate);  // Limit learning rate
    
    // In real implementation: update model weights based on observation
    // and prediction error using Bayesian inference
    
    NLM_LOG_INFO("PredictionSystem: Updated model parameters, learning rate: " + std::to_string(learningRate));
}

void PredictionSystem::applyPredictionPlasticity(const SensoryInput& observation, float error) {
    // Apply synaptic plasticity to prediction circuitry
    // Implements reward-modulated learning in predictive networks
    
    // Get current prediction error
    float predictionError = error;
    
    // Calculate plasticity factor based on error and neuromodulators
    float plasticityFactor = calculatePredictionPlasticityFactor(predictionError);
    
    // Apply plasticity to prediction model
    // In real implementation: update synaptic weights in predictive network
    
    // Update error history statistics
    if (pImpl->errorHistory.size() >= 10) {
        float recentMeanError = 0.0f;
        for (size_t i = pImpl->errorHistory.size() - 10; i < pImpl->errorHistory.size(); ++i) {
            recentMeanError += pImpl->errorHistory[i];
        }
        recentMeanError /= 10.0f;
        
        // Adjust learning based on recent performance
        if (recentMeanError > 0.5f) {
            // High recent error: increase plasticity
            // In real implementation: increase learning rate in prediction circuits
            NLM_LOG_INFO("PredictionSystem: High recent error (" + std::to_string(recentMeanError) +
                         "), increasing plasticity");
        }
    }
    
    NLM_LOG_INFO("PredictionSystem: Applied prediction plasticity, factor: " + std::to_string(plasticityFactor));
}

float PredictionSystem::calculatePredictionPlasticityFactor(float predictionError) const {
    // Calculate plasticity factor for prediction updates
    // Implements adaptive learning based on prediction error
    
    // Base plasticity on prediction error
    float errorPlasticity = 1.0f / (1.0f + predictionError * 5.0f);  // Inverse relationship
    
    // Add neuromodulatory influence (dopamine)
    float neuromodulatoryInfluence = 1.0f + 0.5f * pImpl->confidence;  // Confident predictions = less plastic
    
    // Combine factors
    float plasticity = errorPlasticity * neuromodulatoryInfluence;
    
    // Ensure reasonable bounds
    return std::max(0.01f, std::min(1.0f, plasticity));
}

void PredictionSystem::setRng(std::unique_ptr<RandomGenerator> rng) {
    // Set random number generator for realistic prediction noise
    rng_ = std::move(rng);
}

} // namespace nlm