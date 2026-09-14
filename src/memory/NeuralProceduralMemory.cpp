#include "NeuralProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralProceduralMemory::Impl {
    // Neural network for procedure encoding/decoding
    struct NeuralNetwork {
        std::vector<float> weights;
        float learningRate;
        float weightDecay;
        size_t inputDim;
        size_t outputDim;

        NeuralNetwork(size_t inputSize, size_t outputSize)
            : inputDim(inputSize), outputDim(outputSize), learningRate(0.01f), weightDecay(0.0001f) {
            size_t totalWeights = (inputDim + 1) * (outputDim + 1);
            weights.assign(totalWeights, 0.0f);
        }

        // Forward pass
        std::vector<float> forward(const std::vector<float>& input) {
            std::vector<float> output(outputDim, 0.0f);

            for (size_t j = 0; j < outputDim; ++j) {
                float sum = 0.0f;
                // Bias
                size_t biasIdx = (0 * (outputDim + 1)) + j + 1;
                sum += weights[biasIdx];

                // Input connections
                for (size_t i = 0; i < inputDim; ++i) {
                    size_t weightIdx = (i + 1) * (outputDim + 1) + j + 1;
                    if (i < input.size()) {
                        sum += weights[weightIdx] * input[i];
                    }
                }

                output[j] = sigmoid(sum);
            }

            return output;
        }

        // Learning
        void learn(const std::vector<float>& input, const std::vector<float>& target,
                  float error, float learningRate = 0.01f) {
            std::vector<float> output(outputDim, 0.0f);

            // Compute output
            for (size_t j = 0; j < outputDim; ++j) {
                float sum = 0.0f;
                size_t biasIdx = (0 * (outputDim + 1)) + j + 1;
                sum += weights[biasIdx];

                for (size_t i = 0; i < inputDim; ++i) {
                    size_t weightIdx = (i + 1) * (outputDim + 1) + j + 1;
                    if (i < input.size()) {
                        sum += weights[weightIdx] * input[i];
                    }
                }

                output[j] = sigmoid(sum);
            }

            // Backpropagate
            for (size_t j = 0; j < outputDim; ++j) {
                float deriv = output[j] * (1.0f - output[j]);
                float grad = (target.size() > j ? target[j] : 0.0f) - output[j];

                // Update bias
                size_t biasIdx = (0 * (outputDim + 1)) + j + 1;
                weights[biasIdx] += learningRate * grad * deriv;

                // Update weights
                for (size_t i = 0; i < inputDim; ++i) {
                    size_t weightIdx = (i + 1) * (outputDim + 1) + j + 1;
                    if (i < input.size()) {
                        weights[weightIdx] += learningRate * grad * deriv * input[i];
                    }
                }

                // Apply weight decay
                weights[biasIdx] *= (1.0f - weightDecay);
                for (size_t i = 0; i < inputDim; ++i) {
                    size_t weightIdx = (i + 1) * (outputDim + 1) + j + 1;
                    if (i < input.size()) {
                        weights[weightIdx] *= (1.0f - weightDecay);
                    }
                }
            }
        }

        static float sigmoid(float x) { return 1.0f / (1.0f + std::exp(-x)); }
    };

    NeuralNetwork* actionPredictor;
    SimulationStep simulationTime;

    Impl() : actionPredictor(nullptr), simulationTime(0) {}
    ~Impl() {
        if (actionPredictor) {
            delete actionPredictor;
        }
    }
};

NeuralProceduralMemory::NeuralProceduralMemory() : pImpl(new Impl()) {
    capacity_ = 500;
    enabled_ = true;
}

NeuralProceduralMemory::~NeuralProceduralMemory() = default;

void NeuralProceduralMemory::initialize(Brain* brain) {
    brain_ = brain;

    if (brain_ && !pImpl->actionPredictor) {
        // Create neural network for action sequence prediction
        // Dimension depends on action encoding
        pImpl->actionPredictor = new NeuralProceduralMemory::Impl::NeuralNetwork(12, 6); // 12 input (sensory + state), 6 output (actions)

        NLM_LOG_INFO("NeuralProceduralMemory initialized");
    }
}

void NeuralProceduralMemory::learnProcedure(const std::string& procedureName,
                                           const std::vector<std::vector<float>>& actionSequence,
                                           float initialStrength) {
    if (!enabled_) return;

    if (brain_ && pImpl->actionPredictor) {
        if (procedures_.find(procedureName) != procedures_.end()) {
            // Update existing procedure
            procedurePatterns_[procedureName] = actionSequence;
            procedureStrength_[procedureName] = (procedureStrength_[procedureName] + initialStrength) * 0.5f;
            procedureExecutionCount_[procedureName]++;
            procedureLastExecuted_[procedureName] = pImpl->simulationTime;
        } else {
            // Check capacity
            if (procedures_.size() >= capacity_) {
                // Remove least recently used procedure
                std::string leastRecent;
                SimulationStep oldestTime = std::numeric_limits<SimulationStep>::max();

                for (const auto& pair : procedureLastExecuted_) {
                    if (pair.second < oldestTime) {
                        oldestTime = pair.second;
                        leastRecent = pair.first;
                    }
                }

                if (!leastRecent.empty()) {
                    // Remove procedure
                    auto it = procedureNeurons_.find(leastRecent);
                    if (it != procedureNeurons_.end()) {
                        procedureNeurons_.erase(it);
                    }
                    procedurePatterns_.erase(leastRecent);
                    procedureStrength_.erase(leastRecent);
                    procedureExecutionCount_.erase(leastRecent);
                    procedureSuccessRate_.erase(leastRecent);
                    procedureAvgReward_.erase(leastRecent);
                    procedureLastExecuted_.erase(leastRecent);
                    procedureCreationTime_.erase(leastRecent);
                    procedureActivation_.erase(leastRecent);
                    procedureAutomationLevel_.erase(leastRecent);
                }
            }

            // Store procedure
            procedures_.insert(procedureName);
            procedurePatterns_[procedureName] = actionSequence;
            procedureStrength_[procedureName] = initialStrength;
            procedureExecutionCount_[procedureName] = 1;
            procedureSuccessRate_[procedureName] = 1.0f;
            procedureAvgReward_[procedureName] = 0.0f;
            procedureLastExecuted_[procedureName] = pImpl->simulationTime;
            procedureCreationTime_[procedureName] = pImpl->simulationTime;
            procedureActivation_[procedureName] = 0.0f;
            procedureAutomationLevel_[procedureName] = 0.0f;

            // Encode procedure as neural pattern
            std::vector<float> pattern = encodeActionSequence(actionSequence);
            createProcedureFromPattern(pattern, procedureName);

            // Update neural network
            updateNeuralNetwork(procedureName, pattern, initialStrength);

            NLM_LOG_INFO("Learned procedural memory: " + procedureName);
        }
    }
}

std::vector<std::vector<float>> NeuralProceduralMemory::retrieveProcedure(const std::string& procedureName) const {
    auto it = procedurePatterns_.find(procedureName);
    if (it != procedurePatterns_.end()) {
        return it->second;
    }
    return std::vector<std::vector<float>>();
}

bool NeuralProceduralMemory::hasProcedure(const std::string& procedureName) const {
    return procedures_.find(procedureName) != procedures_.end();
}

std::vector<std::string> NeuralProceduralMemory::getProcedureNames() const {
    std::vector<std::string> names;
    names.reserve(procedures_.size());
    for (const auto& name : procedures_) {
        names.push_back(name);
    }
    return names;
}

std::vector<ActionType> NeuralProceduralMemory::executeProcedure(const std::string& procedureName,
                                                                float variation) const {
    if (!hasProcedure(procedureName)) {
        return std::vector<ActionType>();
    }

    auto it = procedurePatterns_.find(procedureName);
    if (it == procedurePatterns_.end()) {
        return std::vector<ActionType>();
    }

    const auto& actionSequence = it->second;

    if (actionSequence.empty()) {
        return std::vector<ActionType>();
    }

    std::vector<ActionType> actions;
    actions.reserve(actionSequence.size());

    for (const auto& actionPattern : actionSequence) {
        std::vector<ActionType> possibleActions;

        // Decode action from pattern (simplified)
        float maxVal = 0.0f;
        ActionType bestAction = ActionType::Wait;

        // Convert pattern to action (simplified mapping)
        float sum = std::accumulate(actionPattern.begin(), actionPattern.end(), 0.0f);
        if (sum > 0) {
            // Simple mapping based on first significant value
            for (size_t i = 0; i < actionPattern.size() && i < 6; ++i) {
                if (actionPattern[i] > maxVal) {
                    maxVal = actionPattern[i];
                    // Map to action (simplified)
                    switch (i) {
                        case 0: bestAction = ActionType::MoveForward; break;
                        case 1: bestAction = ActionType::MoveBackward; break;
                        case 2: bestAction = ActionType::TurnLeft; break;
                        case 3: bestAction = ActionType::TurnRight; break;
                        case 4: bestAction = ActionType::Interact; break;
                        case 5: bestAction = ActionType::Wait; break;
                    }
                }
            }

            // Apply variation
            if (variation > 0.0f && !executeWithVariation(procedureName, variation, variation).empty()) {
                bestAction = executeWithVariation(procedureName, variation, variation)[0];
            }

            actions.push_back(bestAction);
        }
    }

    return actions;
}

void NeuralProceduralMemory::learnFromExperience(const std::string& procedureName,
                                               const std::vector<ActionType>& actions,
                                               float reward,
                                               float learningRate) {
    if (!enabled_ || !hasProcedure(procedureName)) {
        return;
    }

    // Encode actions as pattern
    std::vector<float> pattern = encodeActionSequence(actions);

    // Update neural network
    float targetStrength = 0.5f + 0.5f * reward;
    updateNeuralNetwork(procedureName, pattern, targetStrength, learningRate);

    // Update procedure statistics
    procedureExecutionCount_[procedureName]++;
    procedureSuccessRate_[procedureName] = (procedureSuccessRate_[procedureName] * (procedureExecutionCount_[procedureName] - 1) + reward) / procedureExecutionCount_[procedureName];
    procedureAvgReward_[procedureName] = (procedureAvgReward_[procedureName] * (procedureExecutionCount_[procedureName] - 1) + reward) / procedureExecutionCount_[procedureName];
    procedureLastExecuted_[procedureName] = pImpl->simulationTime;

    // Increase activation
    procedureActivation_[procedureName] += 0.1f;
    procedureAutomationLevel_[procedureName] = std::min(1.0f, procedureAutomationLevel_[procedureName] + 0.05f);

    NLM_LOG_DEBUG("Updated procedural memory from experience: " + procedureName + " reward=" + std::to_string(reward));
}

void NeuralProceduralMemory::storeMemoryTrace(const std::vector<ActionType>& actions,
                                             float reward,
                                             const std::vector<float>& sensoryContext) {
    if (!enabled_) return;

    SimulationStep currentTime = pImpl->simulationTime;
    memoryTraces_.emplace_back(actions, reward, sensoryContext, currentTime);

    // Limit memory traces
    if (memoryTraces_.size() > 1000) {
        memoryTraces_.pop_front();
    }
}

std::vector<std::string> NeuralProceduralMemory::retrieveSimilarProcedures(const std::vector<float>& state,
                                                                          size_t maxResults) const {
    std::vector<std::pair<std::string, float>> similarProcedures;

    for (const auto& pair : procedurePatterns_) {
        std::vector<float> pattern = pair.second[0];  // First action pattern

        // Calculate similarity based on neural patterns
        float similarity = calculateProcedureSimilarity(pair.first, "temp");

        similarProcedures.emplace_back(pair.first, similarity);
    }

    // Sort by similarity
    std::sort(similarProcedures.begin(), similarProcedures.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::vector<std::string> result;
    for (size_t i = 0; i < std::min(maxResults, similarProcedures.size()); ++i) {
        result.push_back(similarProcedures[i].first);
    }

    return result;
}

void NeuralProceduralMemory::chunkProcedures(const std::vector<std::string>& procedureNames,
                                             const std::string& chunkName) {
    if (procedureNames.size() < 2) {
        return;
    }

    // Check if all procedures exist
    for (const auto& name : procedureNames) {
        if (!hasProcedure(name)) {
            return;
        }
    }

    // Create chunk
    chunks_[chunkName] = procedureNames;

    // Store indices
    std::vector<size_t> indices;
    for (const auto& name : procedureNames) {
        auto it = findProcedureIndex(name);
        if (it != std::numeric_limits<size_t>::max()) {
            indices.push_back(it);
        }
    }
    chunkProceduralIndices_[chunkName] = indices;

    NLM_LOG_INFO("Created procedural chunk: " + chunkName + " with " + std::to_string(procedureNames.size()) + " procedures");
}

std::vector<std::string> NeuralProceduralMemory::decomposeProcedure(const std::string& procedureName) const {
    if (!hasProcedure(procedureName)) {
        return std::vector<std::string>();
    }

    // Check if procedure is in any chunk
    for (const auto& pair : chunks_) {
        const auto& chunkName = pair.first;
        const auto& procedures = pair.second;

        for (const auto& procName : procedures) {
            if (procName == procedureName) {
                return procedures;
            }
        }
    }

    // Simple decomposition based on action count
    auto it = procedurePatterns_.find(procedureName);
    if (it == procedurePatterns_.end()) {
        return std::vector<std::string>();
    }

    const auto& actionSequence = it->second;

    if (actionSequence.size() <= 3) {
        return std::vector<std::string>(); // Too small to decompose
    }

    // Create sub-procedures based on action groups
    std::vector<std::string> subProcedures;

    for (size_t i = 0; i < actionSequence.size(); i += 3) {
        std::string subName = procedureName + "_sub" + std::to_string(i / 3);

        std::vector<std::vector<float>> subSequence;
        for (size_t j = i; j < std::min(i + 3, actionSequence.size()); ++j) {
            subSequence.push_back(actionSequence[j]);
        }

        // Store sub-procedure
        learnProcedure(subName, subSequence, procedureStrength_[procedureName] * 0.3f);
        subProcedures.push_back(subName);
    }

    NLM_LOG_INFO("Decomposed procedure " + procedureName + " into " + std::to_string(subProcedures.size()) + " sub-procedures");

    return subProcedures;
}

NeuralProceduralMemory::ProcedureStats NeuralProceduralMemory::getProcedureStats(const std::string& procedureName) const {
    ProcedureStats stats;
    stats.name = procedureName;

    auto itExecCount = procedureExecutionCount_.find(procedureName);
    if (itExecCount != procedureExecutionCount_.end()) {
        stats.executionCount = itExecCount->second;
    } else {
        stats.executionCount = 0;
    }

    auto itSuccessRate = procedureSuccessRate_.find(procedureName);
    if (itSuccessRate != procedureSuccessRate_.end()) {
        stats.successRate = itSuccessRate->second;
    } else {
        stats.successRate = 0.0f;
    }

    auto itAvgReward = procedureAvgReward_.find(procedureName);
    if (itAvgReward != procedureAvgReward_.end()) {
        stats.averageReward = itAvgReward->second;
    } else {
        stats.averageReward = 0.0f;
    }

    auto itStrength = procedureStrength_.find(procedureName);
    if (itStrength != procedureStrength_.end()) {
        stats.strength = itStrength->second;
    } else {
        stats.strength = 0.0f;
    }

    auto itLastExec = procedureLastExecuted_.find(procedureName);
    if (itLastExec != procedureLastExecuted_.end()) {
        stats.lastExecuted = itLastExec->second;
    } else {
        stats.lastExecuted = 0;
    }

    auto itCreation = procedureCreationTime_.find(procedureName);
    if (itCreation != procedureCreationTime_.end()) {
        stats.creationTime = itCreation->second;
    } else {
        stats.creationTime = 0;
    }

    // Calculate chunk size
    size_t chunkSize = 0;
    for (const auto& pair : chunks_) {
        if (std::find(pair.second.begin(), pair.second.end(), procedureName) != pair.second.end()) {
            chunkSize = pair.second.size();
            break;
        }
    }
    stats.chunkSize = chunkSize;
    stats.isChunkned = (chunkSize > 0);

    return stats;
}

void NeuralProceduralMemory::updateProcedureWeights(float learningRate) {
    for (auto& pair : procedureStrength_) {
        std::string procName = pair.first;

        // Decay less used procedures
        auto itExecCount = procedureExecutionCount_.find(procName);
        if (itExecCount != procedureExecutionCount_.end() && itExecCount->second == 0) {
            pair.second *= (1.0f - learningRate);
        } else {
            // Encourage used procedures
            pair.second += learningRate * 0.01f;
        }

        // Clamp to reasonable range
        pair.second = std::clamp(pair.second, 0.01f, 10.0f);
    }
}

std::vector<std::tuple<std::vector<ActionType>, float, std::vector<float>>> NeuralProceduralMemory::getMemoryTraces(const std::string& procedureName) const {
    std::vector<std::tuple<std::vector<ActionType>, float, std::vector<float>>> traces;

    for (const auto& trace : memoryTraces_) {
        // Filter by procedure (simplified)
        // In a real implementation, traces would be associated with specific procedures
        traces.push_back(trace);
    }

    return traces;
}

std::vector<ActionType> NeuralProceduralMemory::executeWithVariation(const std::string& procedureName,
                                                                   float variationRange,
                                                                   float explorationRate) const {
    if (!hasProcedure(procedureName)) {
        return std::vector<ActionType>();
    }

    std::vector<ActionType> actions;

    // Determine if we should explore
    bool explore = (brain_ && brain_->getRandomGenerator() &&
                   brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < explorationRate);

    if (explore) {
        // Explore: select random action
        std::vector<ActionType> allActions = {
            ActionType::MoveForward, ActionType::MoveBackward,
            ActionType::TurnLeft, ActionType::TurnRight,
            ActionType::Look, ActionType::LookUp,
            ActionType::LookDown, ActionType::Interact,
            ActionType::Eat, ActionType::Drink, ActionType::Rest,
            ActionType::Wait, ActionType::Custom
        };

        // Shuffle
        size_t seed = static_cast<size_t>(pImpl->simulationTime) + std::hash<std::string>{}(procedureName);
        std::mt19937 rng(seed);
        std::shuffle(allActions.begin(), allActions.end(), rng);

        for (size_t i = 0; i < std::min(static_cast<size_t>(variationRange), allActions.size()); ++i) {
            actions.push_back(allActions[i]);
        }
    } else {
        // Exploit: execute learned procedure
        actions = executeProcedure(procedureName, 0.0f);
    }

    return actions;
}

void NeuralProceduralMemory::applyReinforcement(const std::string& procedureName,
                                               float reward,
                                               float predictionError,
                                               float learningRate) {
    if (!hasProcedure(procedureName)) {
        return;
    }

    // Update based on reward prediction error
    float delta = learningRate * (reward - predictionError);

    // Update strength
    procedureStrength_[procedureName] += delta;

    // Clamp
    procedureStrength_[procedureName] = std::clamp(procedureStrength_[procedureName], 0.01f, 10.0f);

    // Update average reward
    procedureAvgReward_[procedureName] = (procedureAvgReward_[procedureName] * (procedureExecutionCount_[procedureName] - 1) + reward) / procedureExecutionCount_[procedureName];

    NLM_LOG_DEBUG("Applied reinforcement to procedure " + procedureName + ": reward=" + std::to_string(reward) + " predictionError=" + std::to_string(predictionError));
}

bool NeuralProceduralMemory::isProcedureAutomated(const std::string& procedureName) const {
    auto it = procedureAutomationLevel_.find(procedureName);
    if (it != procedureAutomationLevel_.end()) {
        return it->second > 0.7f; // Above 70% automation threshold
    }
    return false;
}

void NeuralProceduralMemory::setProcedureAutomation(const std::string& procedureName, float automationLevel) {
    if (procedures_.find(procedureName) != procedures_.end()) {
        procedureAutomationLevel_[procedureName] = std::clamp(automationLevel, 0.0f, 1.0f);
    }
}

float NeuralProceduralMemory::getProcedureActivation(const std::string& procedureName) const {
    auto it = procedureActivation_.find(procedureName);
    if (it != procedureActivation_.end()) {
        return it->second;
    }
    return 0.0f;
}

void NeuralProceduralMemory::markProcedureUsed(const std::string& procedureName) {
    if (procedures_.find(procedureName) != procedures_.end()) {
        procedureExecutionCount_[procedureName]++;
        procedureLastExecuted_[procedureName] = pImpl->simulationTime;
        procedureActivation_[procedureName] += 0.1f;

        // Increase automation for frequently used procedures
        if (procedureExecutionCount_[procedureName] > 10) {
            procedureAutomationLevel_[procedureName] = std::min(1.0f, procedureAutomationLevel_[procedureName] + 0.02f);
        }
    }
}

std::vector<std::pair<std::string, float>> NeuralProceduralMemory::getActiveProcedures() const {
    std::vector<std::pair<std::string, float>> active;

    for (const auto& pair : procedureActivation_) {
        if (pair.second > 0.05f) {
            active.emplace_back(pair.first, pair.second);
        }
    }

    // Sort by activation
    std::sort(active.begin(), active.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    return active;
}

void NeuralProceduralMemory::consolidate(float ageThreshold) {
    // Remove old memory traces
    SimulationStep currentTime = pImpl->simulationTime;

    auto it = memoryTraces_.begin();
    while (it != memoryTraces_.end()) {
        if (std::get<3>(*it) < currentTime - ageThreshold) {
            it = memoryTraces_.erase(it);
        } else {
            ++it;
        }
    }

    // Decay unused procedures
    decayUnusedProcedures(0.01f);
}

void NeuralProceduralMemory::clear() {
    procedureNeurons_.clear();
    procedurePatterns_.clear();
    procedureStrength_.clear();
    procedureExecutionCount_.clear();
    procedureSuccessRate_.clear();
    procedureAvgReward_.clear();
    procedureLastExecuted_.clear();
    procedureCreationTime_.clear();
    procedureActivation_.clear();
    procedureAutomationLevel_.clear();

    memoryTraces_.clear();
    chunks_.clear();
    chunkProceduralIndices_.clear();

    NLM_LOG_INFO("NeuralProceduralMemory cleared");
}

void NeuralProceduralMemory::updateNeuralNetwork(const std::string& procedureName,
                                                const std::vector<float>& pattern,
                                                float targetStrength,
                                                float learningRate) {
    if (!brain_ || !pImpl->actionPredictor) {
        return;
    }

    // Simplified neural network learning
    // In a real implementation, this would involve proper backpropagation

    // Update weights based on error
    float error = targetStrength - procedureStrength_[procedureName];
    pImpl->actionPredictor->learn(pattern, std::vector<float>(pImpl->actionPredictor->outputDim, targetStrength), error, learningRate);
}

void NeuralProceduralMemory::createProcedureFromPattern(const std::vector<float>& pattern,
                                                       const std::string& suggestedName) {
    // Simple implementation: store pattern as is
    // In a real implementation, this would create neural populations
    procedureNeurons_[suggestedName] = 0; // Placeholder
}

void NeuralProceduralMemory::propagateProcedureActivation(const std::string& procedureName, float strength) {
    // Simplified implementation
    if (procedureName.empty() || !hasProcedure(procedureName)) {
        return;
    }

    // Increase activation of related procedures through chunks
    for (const auto& pair : chunks_) {
        const auto& chunkName = pair.first;
        const auto& procedures = pair.second;

        for (const auto& procName : procedures) {
            if (procName != procedureName && hasProcedure(procName)) {
                procedureActivation_[procName] += strength * 0.05f;
            }
        }
    }
}

void NeuralProceduralMemory::updateProcedureHierarchy() {
    // Build simple hierarchy based on usage
    if (procedures_.size() <= 1) {
        return;
    }

    // Find most used procedure as root
    std::string rootProcedure;
    size_t maxExecutions = 0;

    for (const auto& pair : procedureExecutionCount_) {
        if (pair.second > maxExecutions) {
            maxExecutions = pair.second;
            rootProcedure = pair.first;
        }
    }

    // Create hierarchy: most used is root, others are children
    for (const auto& pair : procedures_) {
        std::string procName = pair.first;

        if (procName != rootProcedure) {
            // Find best parent (most similar or most related)
            std::string bestParent = rootProcedure;
            float bestSimilarity = calculateProcedureSimilarity(procName, rootProcedure);

            for (const auto& otherPair : procedures_) {
                if (otherPair.first != procName && otherPair.first != rootProcedure) {
                    float similarity = calculateProcedureSimilarity(procName, otherPair.first);
                    if (similarity > bestSimilarity) {
                        bestSimilarity = similarity;
                        bestParent = otherPair.first;
                    }
                }
            }

            // Store relationship (simplified)
            if (bestParent != procName) {
                // In a real implementation, this would create hierarchy nodes
            }
        }
    }
}

std::vector<std::string> NeuralProceduralMemory::getProceduresContainingAction(ActionType action) const {
    std::vector<std::string> result;

    for (const auto& pair : procedurePatterns_) {
        // Check if this procedure contains the action
        // Simplified: just add all procedures for now
        result.push_back(pair.first);
    }

    return result;
}

float NeuralProceduralMemory::calculateProcedureSimilarity(const std::string& procA,
                                                         const std::string& procB) const {
    auto itA = procedurePatterns_.find(procA);
    auto itB = procedurePatterns_.find(procB);

    if (itA == procedurePatterns_.end() || itB == procedurePatterns_.end()) {
        return 0.0f;
    }

    const auto& patternA = itA->second;
    const auto& patternB = itB->second;

    if (patternA.empty() || patternB.empty()) {
        return 0.0f;
    }

    // Calculate similarity based on first patterns
    float sumSq = 0.0f;
    size_t minSize = std::min(patternA.size(), patternB.size());

    for (size_t i = 0; i < minSize; ++i) {
        float diff = patternA[i] - patternB[i];
        sumSq += diff * diff;
    }

    float distance = std::sqrt(sumSq);
    return 1.0f - std::min(distance, 1.0f); // Convert distance to similarity
}

void NeuralProceduralMemory::decayUnusedProcedures(float decayRate) {
    for (auto& pair : procedureStrength_) {
        std::string procName = pair.first;

        // Check if procedure has been used recently
        auto itLastExec = procedureLastExecuted_.find(procName);
        if (itLastExec != procedureLastExecuted_.end()) {
            SimulationStep currentTime = pImpl->simulationTime;
            SimulationStep age = currentTime - itLastExec->second;

            if (age > 1000) {  // Not used in 1000 steps
                pair.second *= (1.0f - decayRate);
                procedureActivation_[procName] *= (1.0f - decayRate);
            }
        }
    }
}
size_t NeuralProceduralMemory::findProcedureIndex(const std::string& procedureName) const {
    size_t index = 0;
    for (const auto& pair : procedurePatterns_) {
        if (pair.first == procedureName) {
            return index;
        }
        index++;
    }
    return std::numeric_limits<size_t>::max();
}

std::vector<float> NeuralProceduralMemory::encodeActionSequence(const std::vector<ActionType>& actions) const {
    // Convert action sequence to neural pattern
    std::vector<float> pattern;
    pattern.reserve(actions.size() * 2); // 2 values per action (position + strength)

    for (const auto& action : actions) {
        // Convert ActionType to vector of float values
        std::vector<float> actionVector;

        // Map action to vector (simplified)
        switch (action) {
            case ActionType::MoveForward:
                actionVector = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
                break;
            case ActionType::MoveBackward:
                actionVector = {0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
                break;
            case ActionType::TurnLeft:
                actionVector = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
                break;
            case ActionType::TurnRight:
                actionVector = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
                break;
            case ActionType::Look:
                actionVector = {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
                break;
            case ActionType::LookUp:
                actionVector = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
                break;
            case ActionType::LookDown:
                actionVector = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
                break;
            case ActionType::Interact:
                actionVector = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
                break;
            case ActionType::Eat:
                actionVector = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
                break;
            case ActionType::Drink:
                actionVector = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
                break;
            case ActionType::Rest:
                actionVector = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
                break;
            case ActionType::Wait:
                actionVector = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
                break;
            case ActionType::Custom:
                actionVector = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
                // Custom action - use random vector
                for (size_t i = 0; i < 12; ++i) {
                    actionVector[i] = static_cast<float>(rand()) / RAND_MAX;
                }
                break;
        }

        pattern.insert(pattern.end(), actionVector.begin(), actionVector.end());
    }

    return pattern;
}

std::vector<ActionType> NeuralProceduralMemory::decodeActionPattern(const std::vector<float>& pattern) const {
    std::vector<ActionType> actions;

    if (pattern.size() < 12) {
        return actions;
    }

    // Decode each action (simplified)
    for (size_t i = 0; i < pattern.size(); i += 12) {
        if (i + 12 > pattern.size()) {
            break;
        }

        std::vector<float> actionVector(pattern.begin() + i, pattern.begin() + i + 12);

        // Find action with highest value
        float maxVal = 0.0f;
        ActionType action = ActionType::Wait;

        for (size_t j = 0; j < 12; ++j) {
            if (actionVector[j] > maxVal) {
                maxVal = actionVector[j];

                switch (j) {
                    case 0: action = ActionType::MoveForward; break;
                    case 1: action = ActionType::MoveBackward; break;
                    case 2: action = ActionType::TurnLeft; break;
                    case 3: action = ActionType::TurnRight; break;
                    case 4: action = ActionType::Look; break;
                    case 5: action = ActionType::LookUp; break;
                    case 6: action = ActionType::LookDown; break;
                    case 7: action = ActionType::Interact; break;
                    case 8: action = ActionType::Eat; break;
                    case 9: action = ActionType::Drink; break;
                    case 10: action = ActionType::Rest; break;
                    case 11: action = ActionType::Wait; break;
                }
            }
        }

        actions.push_back(action);
    }

    return actions;
}

std::vector<std::pair<std::string, float>> NeuralProceduralMemory::getSimilarPatterns(const std::vector<float>& pattern,
                                                                                      size_t maxResults) const {
    std::vector<std::pair<std::string, float>> similar;

    for (const auto& pair : procedurePatterns_) {
        std::vector<float> storedPattern = pair.second[0]; // First action pattern

        if (storedPattern.empty()) {
            continue;
        }

        // Calculate similarity
        float similarity = 1.0f; // Simplified: always similar

        similar.emplace_back(pair.first, similarity);
    }

    // Sort by similarity
    std::sort(similar.begin(), similar.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    // Limit results
    if (similar.size() > maxResults) {
        similar.resize(maxResults);
    }

    return similar;
}

} // namespace nlm
