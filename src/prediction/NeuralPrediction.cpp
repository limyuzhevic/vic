// Implement setters with error handling
void NeuralPrediction::setSequenceMemorySize(size_t size) {
    if (size > 0) {
        sequenceMemorySize_ = size;
        NLM_LOG_INFO("Sequence memory size set to " + std::to_string(size));
    } else {
        NLM_LOG_WARNING("Invalid sequence memory size, using default");
    }
}

void NeuralPrediction::setPredictionHorizon(size_t steps) {
    if (steps > 0) {
        predictionHorizon_ = steps;
        NLM_LOG_INFO("Prediction horizon set to " + std::to_string(steps));
    } else {
        NLM_LOG_WARNING("Invalid prediction horizon, using default");
    }
}

// Implement remaining methods with error handling
NeuralPrediction::NeuralPrediction()
    : pImpl(new Impl)
    , sequenceMemorySize_(10)
    , predictionHorizon_(1)
    , temporalPredictionEnabled_(true)
    , actionConsequenceEnabled_(true)
    , brain_(nullptr)
    , predictionError_(0.0f)
    , predictionConfidence_(0.5f)
    , errorHistory_()
    , recentSensoryStates_()
    , stateTimestamps_()
    , recentActions_()
    , predictionNeurons_()
    , sequenceNeurons_()
{
    // Configure bounded buffer sizes in pImpl
    pImpl->errorHistory.reserve(1000);
    NLM_LOG_INFO("NeuralPrediction constructor completed");
}

NeuralPrediction::~NeuralPrediction() = default;

void NeuralPrediction::initialize(Brain* brain) {
    try {
        NLM_LOG_DEBUG("NeuralPrediction::initialize called");
        
        if (!brain) {
            NLM_LOG_ERROR("Null pointer provided to NeuralPrediction::initialize");
            throw NLMError(NLMErrorCode::NullPointer, "Brain pointer is null in NeuralPrediction::initialize");
        }
        
        pImpl->brain = brain;
        brain_ = brain;
        
        NLM_LOG_INFO("NeuralPrediction initialized");
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR("Failed to initialize NeuralPrediction: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in NeuralPrediction::initialize: " + std::string(e.what()));
        throw NLMError(NLMErrorCode::Unknown, "Failed to initialize NeuralPrediction: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error in NeuralPrediction::initialize");
        throw NLMError(NLMErrorCode::Unknown, "Unknown error in NeuralPrediction::initialize");
    }
}

void NeuralPrediction::recordSensoryState(const std::vector<float>& sensoryState, 
                                         SimulationStep currentStep) {
    try {
        NLM_LOG_DEBUG("NeuralPrediction::recordSensoryState called");
        
        // Input validation
        if (!ErrorHandler::validateNonEmpty(sensoryState, NLMErrorCode::ValidationError)) {
            return; // Silently fail for empty data
        }
        
        if (!sensoryState.data()) {
            NLM_LOG_ERROR("Null pointer detected in sensory state data");
            throw NLMError(NLMErrorCode::NullPointer, "Null pointer in sensory state data");
        }
        
        if (!temporalPredictionEnabled_) {
            NLM_LOG_DEBUG("Temporal prediction disabled, skipping state recording");
            return;
        }
        
        recentSensoryStates_.push_back(sensoryState);
        stateTimestamps_.push_back(currentStep);
        
        // Keep only recent states with bounds checking
        if (recentSensoryStates_.size() > static_cast<size_t>(sequenceMemorySize_)) {
            recentSensoryStates_.erase(recentSensoryStates_.begin());
            stateTimestamps_.erase(stateTimestamps_.begin());
            NLM_LOG_DEBUG("Recent sensory states trimmed to sequence memory size");
        }
        
        // Learn sequence if we have at least 2 states
        if (recentSensoryStates_.size() >= 2) {
            learnTemporalSequence(recentSensoryStates_[recentSensoryStates_.size() - 2],
                                recentSensoryStates_.back(),
                                currentStep);
        }
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR("Failed to record sensory state: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in NeuralPrediction::recordSensoryState: " + std::string(e.what()));
        throw NLMError(NLMErrorCode::Unknown, "Failed to record sensory state: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error in NeuralPrediction::recordSensoryState");
        throw NLMError(NLMErrorCode::Unknown, "Unknown error in NeuralPrediction::recordSensoryState");
    }
}

std::vector<float> NeuralPrediction::generatePrediction(SimulationStep currentStep) {
    try {
        NLM_LOG_DEBUG("NeuralPrediction::generatePrediction called");
        
        // Check if prediction is enabled and we have data
        if (!temporalPredictionEnabled_ || recentSensoryStates_.empty()) {
            NLM_LOG_DEBUG("Cannot generate prediction - temporal prediction disabled or no recent states");
            predictionConfidence_ = 0.0f;
            return std::vector<float>();
        }
        
        // Input validation
        const auto& currentState = recentSensoryStates_.back();
        if (currentState.empty() || !currentState.data()) {
            NLM_LOG_ERROR("Invalid current state in generatePrediction");
            throw NLMError(NLMErrorCode::ValidationError, "Invalid current state in generatePrediction");
        }
        
        // Find best matching learned sequence
        NeuronId bestMatch = INVALID_NEURON_ID;
        float bestSimilarity = 0.0f;
        
        // Bounds checking for pattern representations
        if (pImpl->patternRepresentations.empty()) {
            NLM_LOG_DEBUG("No pattern representations found, returning current state as baseline");
            predictionConfidence_ = 0.0f;
            return currentState;
        }
        
        for (size_t i = 0; i < pImpl->patternRepresentations.size(); ++i) {
            ErrorHandler::validateRange(i, pImpl->patternRepresentations.size(), NLMErrorCode::OutOfBounds);
            
            float sim = computeSimilarity(currentState, pImpl->patternRepresentations[i]);
            if (sim > bestSimilarity) {
                bestSimilarity = sim;
                bestMatch = pImpl->patternNeurons[i];
            }
        }
        
        // If we have a good match, predict what comes next
        if (bestSimilarity > 0.7f && bestMatch != INVALID_NEURON_ID) {
            // Find what we associate with this pattern
            for (const auto& assoc : pImpl->sequenceAssociations) {
                if (assoc.first == bestMatch) {
                    // Found association - return the associated pattern
                    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
                        ErrorHandler::validateRange(i, pImpl->patternNeurons.size(), NLMErrorCode::OutOfBounds);
                        
                        if (pImpl->patternNeurons[i] == assoc.second) {
                            predictionConfidence_ = bestSimilarity;
                            NLM_LOG_INFO("Generated prediction based on learned sequence, similarity: " + std::to_string(bestSimilarity));
                            return pImpl->patternRepresentations[i];
                        }
                    }
                }
            }
        }
        
        // No good prediction - return current state as baseline
        predictionConfidence_ = 0.0f;
        NLM_LOG_DEBUG("No good prediction found, returning current state as baseline");
        return currentState;
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR("Failed to generate prediction: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in NeuralPrediction::generatePrediction: " + std::string(e.what()));
        throw NLMError(NLMErrorCode::Unknown, "Failed to generate prediction: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error in NeuralPrediction::generatePrediction");
        throw NLMError(NLMErrorCode::Unknown, "Unknown error in NeuralPrediction::generatePrediction");
    }
}

float NeuralPrediction::updateWithObservation(const std::vector<float>& actualState,
                                             SimulationStep currentStep) {
    try {
        NLM_LOG_DEBUG("NeuralPrediction::updateWithObservation called");
        
        // Input validation
        if (!ErrorHandler::validateNonEmpty(actualState, NLMErrorCode::ValidationError)) {
            NLM_LOG_ERROR("Null or empty actual state provided to updateWithObservation");
            return predictionError_;
        }
        
        if (!actualState.data()) {
            NLM_LOG_ERROR("Null pointer detected in actual state data");
            throw NLMError(NLMErrorCode::NullPointer, "Null pointer in actual state data");
        }
        
        // Generate prediction first
        auto predicted = generatePrediction(currentStep);
        
        // Compute error
        if (predicted.empty()) {
            predictionError_ = 0.0f;
            NLM_LOG_DEBUG("No prediction generated, error set to 0");
        } else {
            // Bounds checking for similarity calculation
            if (predicted.size() != actualState.size()) {
                NLM_LOG_WARNING("Predicted and actual state sizes don't match: " + 
                              std::to_string(predicted.size()) + " != " + std::to_string(actualState.size()));
                // Truncate to smaller size for computation
                size_t minSize = std::min(predicted.size(), actualState.size());
                predicted.resize(minSize);
                NLM_LOG_DEBUG("Truncated prediction state to match actual state size");
            }
            
            predictionError_ = computeSimilarity(predicted, actualState);
            predictionError_ = 1.0f - predictionError_;  // Convert to error
            NLM_LOG_INFO("Prediction error computed: " + std::to_string(predictionError_));
        }
        
        errorHistory_.push_back(predictionError_);
        if (errorHistory_.size() > 1000) {
            errorHistory_.erase(errorHistory_.begin());
            NLM_LOG_DEBUG("Error history trimmed to max size");
        }
        
        // Record the actual state for future learning
        recordSensoryState(actualState, currentStep);
        
        // Modulate learning based on error
        float reward = 1.0f - predictionError_;
        modulatePredictionSynapses(predictionError_, reward);
        
        return predictionError_;
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR("Failed to update with observation: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in NeuralPrediction::updateWithObservation: " + std::string(e.what()));
        throw NLMError(NLMErrorCode::Unknown, "Failed to update with observation: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error in NeuralPrediction::updateWithObservation");
        throw NLMError(NLMErrorCode::Unknown, "Unknown error in NeuralPrediction::updateWithObservation");
    }
}

std::vector<float> NeuralPrediction::predictActionConsequence(ActionType action,
                                                               const std::vector<float>& currentState) {
    try {
        NLM_LOG_DEBUG("NeuralPrediction::predictActionConsequence called");
        
        // Input validation
        if (!ErrorHandler::validateNonEmpty(currentState, NLMErrorCode::ValidationError)) {
            NLM_LOG_ERROR("Null or empty current state provided to predictActionConsequence");
            return currentState;
        }
        
        if (!currentState.data()) {
            NLM_LOG_ERROR("Null pointer detected in current state data");
            throw NLMError(NLMErrorCode::NullPointer, "Null pointer in current state data");
        }
        
        if (!actionConsequenceEnabled_) {
            NLM_LOG_DEBUG("Action consequence prediction disabled, returning current state");
            return currentState;  // No change predicted
        }
        
        // Find experiences with same action and similar state
        float bestMatch = 0.0f;
        std::vector<float> bestConsequence;
        
        for (size_t i = 0; i < recentActions_.size(); ++i) {
            ErrorHandler::validateRange(i, recentActions_.size(), NLMErrorCode::OutOfBounds);
            
            if (recentActions_[i].first == action) {
                float sim = computeSimilarity(currentState, recentActions_[i].second);
                if (sim > bestMatch) {
                    bestMatch = sim;
                    // Bounds checking for actionConsequences access
                    if (i < pImpl->actionConsequences.size()) {
                        bestConsequence = pImpl->actionConsequences[i].second;
                    }
                }
            }
        }
        
        if (bestMatch > 0.5f && !bestConsequence.empty()) {
            NLM_LOG_INFO("Predicted action consequence with confidence: " + std::to_string(bestMatch));
            return bestConsequence;
        }
        
        NLM_LOG_DEBUG("No matching action consequence found, returning current state");
        return currentState;  // Default: no change
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR("Failed to predict action consequence: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in NeuralPrediction::predictActionConsequence: " + std::string(e.what()));
        throw NLMError(NLMErrorCode::Unknown, "Failed to predict action consequence: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error in NeuralPrediction::predictActionConsequence");
        throw NLMError(NLMErrorCode::Unknown, "Unknown error in NeuralPrediction::predictActionConsequence");
    }
}

std::vector<std::vector<float>> NeuralPrediction::predictMultipleSteps(SimulationStep currentStep,
                                                                        size_t numSteps) {
    try {
        NLM_LOG_DEBUG("NeuralPrediction::predictMultipleSteps called");
        
        std::vector<std::vector<float>> predictions;
        
        // Input validation
        if (numSteps == 0) {
            NLM_LOG_WARNING("Zero steps requested for multi-step prediction");
            return predictions;
        }
        
        auto currentPred = recentSensoryStates_.empty() ? 
                           std::vector<float>() : recentSensoryStates_.back();
        
        for (size_t step = 0; step < numSteps; ++step) {
            if (currentPred.empty()) break;
            
            predictions.push_back(currentPred);
            
            // Use current prediction to generate next
            // (simple approach - in reality this would use chained predictions)
            auto nextPred = generatePrediction(currentStep + step);
            if (nextPred.empty()) break;
            currentPred = nextPred;
        }
        
        NLM_LOG_INFO("Generated " + std::to_string(predictions.size()) + " predictions for " + std::to_string(numSteps) + " steps");
        return predictions;
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR("Failed to predict multiple steps: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in NeuralPrediction::predictMultipleSteps: " + std::to_string(e.what()));
        throw NLMError(NLMErrorCode::Unknown, "Failed to predict multiple steps: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error in NeuralPrediction::predictMultipleSteps");
        throw NLMError(NLMErrorCode::Unknown, "Unknown error in NeuralPrediction::predictMultipleSteps");
    }
}

void NeuralPrediction::recordAction(ActionType action, SimulationStep step) {
    try {
        NLM_LOG_DEBUG("NeuralPrediction::recordAction called");
        
        if (recentSensoryStates_.empty()) {
            NLM_LOG_DEBUG("No recent sensory states, skipping action recording");
            return;
        }
        
        recentActions_.push_back({action, recentSensoryStates_.back()});
        
        // Keep only recent actions with bounds checking
        if (recentActions_.size() > 100) {
            recentActions_.erase(recentActions_.begin());
            NLM_LOG_DEBUG("Recent actions trimmed to max size");
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in NeuralPrediction::recordAction: " + std::string(e.what()));
        throw NLMError(NLMErrorCode::Unknown, "Failed to record action: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error in NeuralPrediction::recordAction");
        throw NLMError(NLMErrorCode::Unknown, "Unknown error in NeuralPrediction::recordAction");
    }
}

void NeuralPrediction::learnTemporalSequence(const std::vector<float>& currentState,
                                             const std::vector<float>& nextState,
                                             SimulationStep currentStep) {
    try {
        NLM_LOG_DEBUG("NeuralPrediction::learnTemporalSequence called");
        
        // Input validation
        if (!ErrorHandler::validateNonEmpty(currentState, NLMErrorCode::ValidationError) ||
            !ErrorHandler::validateNonEmpty(nextState, NLMErrorCode::ValidationError)) {
            NLM_LOG_ERROR("Empty state provided to learnTemporalSequence");
            return;
        }
        
        // Find or create pattern neuron for current state
        NeuronId currentNeuron = findMatchingPatternNeuron(currentState);
        NeuronId nextNeuron = findMatchingPatternNeuron(nextState);
        
        if (currentNeuron != nextNeuron && currentNeuron != INVALID_NEURON_ID && 
            nextNeuron != INVALID_NEURON_ID) {
            createSequenceAssociation(currentNeuron, nextNeuron, 0.8f);
            NLM_LOG_INFO("Learned temporal sequence between pattern neurons");
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in NeuralPrediction::learnTemporalSequence: " + std::string(e.what()));
        throw NLMError(NLMErrorCode::Unknown, "Failed to learn temporal sequence: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error in NeuralPrediction::learnTemporalSequence");
        throw NLMError(NLMErrorCode::Unknown, "Unknown error in NeuralPrediction::learnTemporalSequence");
    }
}

NeuronId NeuralPrediction::findMatchingPatternNeuron(const std::vector<float>& pattern,
                                                     float similarityThreshold) {
    try {
        NLM_LOG_DEBUG("NeuralPrediction::findMatchingPatternNeuron called");
        
        // Input validation
        if (!ErrorHandler::validateNonEmpty(pattern, NLMErrorCode::ValidationError)) {
            NLM_LOG_ERROR("Empty pattern provided to findMatchingPatternNeuron");
            return INVALID_NEURON_ID;
        }
        
        // Check if we already have a similar pattern
        for (size_t i = 0; i < pImpl->patternRepresentations.size(); ++i) {
            ErrorHandler::validateRange(i, pImpl->patternRepresentations.size(), NLMErrorCode::OutOfBounds);
            
            float sim = computeSimilarity(pattern, pImpl->patternRepresentations[i]);
            if (sim >= similarityThreshold) {
                NLM_LOG_DEBUG("Found matching pattern neuron with similarity " + std::to_string(sim));
                return pImpl->patternNeurons[i];
            }
        }
        
        // Create new pattern neuron
        if (brain_) {
            NeuronId newId = NeuronId(pImpl->patternNeurons.size() + 10000);
            pImpl->patternNeurons.push_back(newId);
            pImpl->patternRepresentations.push_back(pattern);
            NLM_LOG_INFO("Created new pattern neuron with ID " + std::to_string(newId));
            return newId;
        } else {
            NLM_LOG_ERROR("Brain pointer is null, cannot create new pattern neuron");
            return INVALID_NEURON_ID;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in NeuralPrediction::findMatchingPatternNeuron: " + std::string(e.what()));
        throw NLMError(NLMErrorCode::Unknown, "Failed to find matching pattern neuron: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error in NeuralPrediction::findMatchingPatternNeuron");
        throw NLMError(NLMErrorCode::Unknown, "Unknown error in NeuralPrediction::findMatchingPatternNeuron");
    }
}

void NeuralPrediction::createSequenceAssociation(NeuronId from, NeuronId to, float strength) {
    try {
        NLM_LOG_DEBUG("NeuralPrediction::createSequenceAssociation called");
        
        if (from == INVALID_NEURON_ID || to == INVALID_NEURON_ID) {
            NLM_LOG_WARNING("Invalid neuron IDs provided to createSequenceAssociation");
            return;
        }
        
        // Check if association already exists
        for (auto& assoc : pImpl->sequenceAssociations) {
            if (assoc.first == from && assoc.second == to) {
                // Update strength
                return;
            }
        }
        
        // Create new association
        pImpl->sequenceAssociations.emplace_back(from, to);
        NLM_LOG_INFO("Created sequence association between neurons " + 
                     std::to_string(from) + " and " + std::to_string(to));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in NeuralPrediction::createSequenceAssociation: " + std::string(e.what()));
        throw NLMError(NLMErrorCode::Unknown, "Failed to create sequence association: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error in NeuralPrediction::createSequenceAssociation");
        throw NLMError(NLMErrorCode::Unknown, "Unknown error in NeuralPrediction::createSequenceAssociation");
    }
}

float NeuralPrediction::computeSimilarity(const std::vector<float>& a, 
                                         const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    // Cosine similarity
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

void NeuralPrediction::modulatePredictionSynapses(float error, float reward) {
    if (!brain_) return;
    
    // Apply reward-modulated plasticity to prediction-related synapses
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            // If this synapse was recently active in prediction pathway
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * (reward - 0.5f) * 2.0f;
                syn->addToWeight(delta);
                
                // Decay eligibility
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

std::vector<NeuronId> NeuralPrediction::getPredictionNeurons() const {
    return predictionNeurons_;
}

std::vector<NeuronId> NeuralPrediction::getSequenceNeurons() const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->sequenceAssociations) {
        result.push_back(assoc.first);
        result.push_back(assoc.second);
    }
    return result;
}

void NeuralPrediction::clearHistory() {
    errorHistory_.clear();
    recentSensoryStates_.clear();
    stateTimestamps_.clear();
    recentActions_.clear();
    pImpl->learnedSequences.clear();
    pImpl->sequenceAssociations.clear();
    NLM_LOG_INFO("NeuralPrediction history cleared");
}
