#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <deque>
#include <algorithm>

namespace nlm {

// NeuralProceduralMemory: Stores skills, habits, and behavioral sequences
// NOT a symbolic procedure repository - procedures are encoded as neural trajectories
//
// Key mechanisms:
// - Sequences of neural activations: skills as patterns of activation over time
// - Reinforcement learning: procedures strengthened by successful outcomes
// - Chunking: complex procedures broken into learned subroutines
// - Motor patterns: sequences of actions stored as neural trajectories
// - Habit formation: repeated patterns become automatic (low cognitive load)

class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Learn a new skill/procedure
    // Encodes the sequence of actions as a neural trajectory
    void learnProcedure(const std::string& procedureName,
                       const std::vector<std::vector<float>>& actionSequence,
                       float initialStrength = 1.0f);

    // Retrieve procedure by name
    std::vector<std::vector<float>> retrieveProcedure(const std::string& procedureName) const;

    // Check if procedure exists
    bool hasProcedure(const std::string& procedureName) const;

    // Get all procedure names
    std::vector<std::string> getProcedureNames() const;

    // Execute procedure - returns sequence of motor commands
    std::vector<ActionType> executeProcedure(const std::string& procedureName,
                                           float variation = 0.0f) const;

    // Learn from experience - refine procedure based on outcome
    void learnFromExperience(const std::string& procedureName,
                           const std::vector<ActionType>& actions,
                           float reward,
                           float learningRate = 0.01f);

    // Store memory trace for later recall
    void storeMemoryTrace(const std::vector<ActionType>& actions,
                         float reward,
                         const std::vector<float>& sensoryContext);

    // Retrieve similar procedures based on current state
    std::vector<std::string> retrieveSimilarProcedures(const std::vector<float>& state,
                                                      size_t maxResults = 5) const;

    // Chunk procedures - combine related sub-procedures
    void chunkProcedures(const std::vector<std::string>& procedureNames,
                        const std::string& chunkName);

    // Decompose procedure into sub-procedures
    std::vector<std::string> decomposeProcedure(const std::string& procedureName) const;

    // Get procedure statistics
    struct ProcedureStats {
        std::string name;
        size_t executionCount;
        float successRate;
        float averageReward;
        float strength;
        SimulationStep lastExecuted;
        SimulationStep creationTime;
        size_t chunkSize;
        bool isChunkned;
    };

    ProcedureStats getProcedureStats(const std::string& procedureName) const;

    // Update procedure weights
    void updateProcedureWeights(float learningRate = 0.01f);

    // Get all memory traces for a procedure
    std::vector<std::tuple<std::vector<ActionType>, float, std::vector<float>>> getMemoryTraces(const std::string& procedureName) const;

    // Execute procedural memory with variation
    std::vector<ActionType> executeWithVariation(const std::string& procedureName,
                                               float variationRange,
                                               float explorationRate = 0.1f) const;

    // Apply reinforcement learning to procedures
    void applyReinforcement(const std::string& procedureName,
                         float reward,
                         float predictionError,
                         float learningRate = 0.01f);

    // Check if procedure is ready to be executed automatically
    bool isProcedureAutomated(const std::string& procedureName) const;

    // Set procedure automation level
    void setProcedureAutomation(const std::string& procedureName, float automationLevel);

    // Get procedure activation level
    float getProcedureActivation(const std::string& procedureName) const;

    // Mark procedure as used (for activation)
    void markProcedureUsed(const std::string& procedureName);

    // Get active procedures
    std::vector<std::pair<std::string, float>> getActiveProcedures() const;

    // Get procedural memory statistics
    size_t getProcedureCount() const { return procedures_.size(); }
    size_t getTraceCount() const { return memoryTraces_.size(); }
    size_t getChunkCount() const { return chunks_.size(); }

    // Clear old memory traces
    void consolidate(float ageThreshold = 1000.0f);

    // Clear all procedures
    void clear();

    // Set storage capacity
    void setCapacity(size_t cap) { capacity_ = cap; }
    size_t getCapacity() const { return capacity_; }

    // Enable/disable procedural memory
    void enable(bool enable) { enabled_ = enable; }
    bool isEnabled() const { return enabled_; }

private:
    // Encode sequence of actions as neural pattern
    std::vector<float> encodeActionSequence(const std::vector<ActionType>& actions) const;

    // Decode neural pattern to sequence of actions
    std::vector<ActionType> decodeActionPattern(const std::vector<float>& pattern) const;

    // Get similar neural patterns
    std::vector<std::pair<std::string, float>> getSimilarPatterns(const std::vector<float>& pattern,
                                                                 size_t maxResults = 5) const;

    // Update neural network for procedure
    void updateNeuralNetwork(const std::string& procedureName,
                           const std::vector<float>& pattern,
                           float targetStrength,
                           float learningRate);

    // Create procedure from neural pattern
    void createProcedureFromPattern(const std::vector<float>& pattern,
                                   const std::string& suggestedName);

    // Propagate procedure activation to related procedures
    void propagateProcedureActivation(const std::string& procedureName, float strength);

    // Update procedure hierarchy
    void updateProcedureHierarchy();

    // Get all procedures that contain a specific action
    std::vector<std::string> getProceduresContainingAction(ActionType action) const;

    // Calculate procedure similarity based on neural patterns
    float calculateProcedureSimilarity(const std::string& procA,
                                     const std::string& procB) const;

    // Decay less used procedures
    void decayUnusedProcedures(float decayRate);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t capacity_;
    bool enabled_;

    // Procedural memory
    std::unordered_map<std::string, NeuronId> procedureNeurons_;
    std::unordered_map<std::string, std::vector<std::vector<float>>> procedurePatterns_;
    std::unordered_map<std::string, float> procedureStrength_;
    std::unordered_map<std::string, size_t> procedureExecutionCount_;
    std::unordered_map<std::string, float> procedureSuccessRate_;
    std::unordered_map<std::string, float> procedureAvgReward_;
    std::unordered_map<std::string, SimulationStep> procedureLastExecuted_;
    std::unordered_map<std::string, SimulationStep> procedureCreationTime_;
    std::unordered_map<std::string, float> procedureActivation_;
    std::unordered_map<std::string, float> procedureAutomationLevel_;

    // Memory traces (experience-based learning)
    std::deque<std::tuple<std::vector<ActionType>, float, std::vector<float>, SimulationStep>> memoryTraces_;

    // Procedural chunks (grouped procedures)
    std::unordered_map<std::string, std::vector<std::string>> chunks_;
    std::unordered_map<std::string, std::vector<size_t>> chunkProceduralIndices_;

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

    // Neural network for action sequence prediction
    NeuralNetwork* actionPredictor_;

    // Simulation time
    SimulationStep simulationTime_;
};

} // namespace nlm
