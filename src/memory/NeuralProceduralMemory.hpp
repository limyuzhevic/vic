#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

namespace nlm {

// NeuralProceduralMemory: Stores skills and habits using neural patterns
// NOT a simple skill list - uses neural encoding of motor patterns
//
// Key mechanisms:
// - Neural pattern encoding of procedures
// - Motor pathway conditioning
// - Procedural memory consolidation during rest
// - Pattern completion for automatic execution
// - Proficiency tracking through neural strengthening

class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Learn a new procedure
    // The procedure is encoded as neural activity pattern
    void learnProcedure(const std::string& name, 
                        const std::vector<float>& pattern,
                        const std::vector<NeuronId>& motorNeurons);

    // Execute a procedure
    // Triggers the neural pattern for the procedure
    std::vector<float> executeProcedure(const std::string& name, float proficiency = 1.0f);

    // Retrieve procedure by name
    std::vector<float> retrieveProcedure(const std::string& name) const;

    // Get motor neurons associated with procedure
    std::vector<NeuronId> getProcedureMotorNeurons(const std::string& name) const;

    // Get all procedures
    std::vector<std::string> getAllProcedures() const;

    // Update procedure proficiency
    void updateProficiency(const std::string& name, float delta, float maxProficiency = 1.0f);

    // Get procedure proficiency
    float getProcedureProficiency(const std::string& name) const;

    // Check if procedure exists
    bool hasProcedure(const std::string& name) const;

    // Update procedure based on execution outcome
    void updateProcedure(const std::string& name, float outcome, float predictionError);

    // Get procedure execution history
    std::vector<float> getProcedureExecutionHistory(const std::string& name) const;

    // Get average procedure performance
    float getProcedurePerformance(const std::string& name) const;

    // Create new procedure from sensorimotor experience
    void createProcedureFromExperience(const std::vector<float>& sensoryPattern,
                                       const std::vector<float>& motorPattern,
                                       const std::vector<NeuronId>& motorNeurons);

    // Execute procedure with adaptation
    std::vector<float> executeProcedureWithAdaptation(const std::string& name,
                                                     const std::vector<float>& context);

    // Get procedures for given context
    std::vector<std::string> getProceduresForContext(const std::vector<float>& context) const;

    // Clear all procedures
    void clear();

    // Get number of procedures
    size_t getProcedureCount() const { return procedurePrototypes_.size(); }

    // Enable/disable procedural memory
    void enableProceduralMemory(bool enable) { proceduralEnabled_ = enable; }
    bool isProceduralMemoryEnabled() const { return proceduralEnabled_; }

    // Set learning rate
    void setLearningRate(float rate) { learningRate_ = rate; }
    float getLearningRate() const { return learningRate_; }

    // Set proficiency decay rate
    void setProficiencyDecayRate(float rate) { proficiencyDecayRate_ = rate; }
    float getProficiencyDecayRate() const { return proficiencyDecayRate_; }

private:
    // Find or create procedure neuron pattern
    std::vector<float> getProcedurePattern(const std::string& name);

    // Compute procedure similarity using pattern matching
    float computeProcedureSimilarity(const std::vector<float>& pattern1,
                                     const std::vector<float>& pattern2) const;

    // Update procedure motor connections
    void updateProcedureMotorConnections(const std::string& name,
                                         const std::vector<NeuronId>& motorNeurons);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::unordered_map<std::string, std::vector<float>> procedurePrototypes_;
    std::unordered_map<std::string, std::vector<NeuronId>> procedureMotorNeurons_;
    std::unordered_map<std::string, float> procedureProficiency_;
    std::unordered_map<std::string, std::vector<float>> procedureHistory_;
    std::unordered_map<std::string, float> procedurePerformance_;
    std::unordered_map<std::string, std::vector<float>> procedureContexts_;
    std::unordered_map<std::string, std::vector<float>> procedurePredictions_;
    bool proceduralEnabled_;
    float learningRate_;
    float proficiencyDecayRate_;
    float predictionErrorWeight_;
};

} // namespace nlm
