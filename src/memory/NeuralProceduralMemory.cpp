#include "NeuralProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <set>

namespace nlm {

struct NeuralProceduralMemory::Impl {
    Brain* brain;
    
    // Maintenance connections for procedures
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceConnections;
    
    // Motor pathway weights
    std::vector<std::pair<NeuronId, float>> motorPathwayWeights;
    
    // Procedural consolidation state
    std::vector<float> consolidationState;
    
    Impl() : brain(nullptr), learningRate(0.1f), proficiencyDecayRate(0.001f), 
             predictionErrorWeight(0.5f) {}
    
    float learningRate;
    float proficiencyDecayRate;
    float predictionErrorWeight;
};

NeuralProceduralMemory::NeuralProceduralMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , proceduralEnabled_(true)
    , learningRate_(0.1f)
    , proficiencyDecayRate_(0.001f)
    , predictionErrorWeight_(0.5f)
{
}

NeuralProceduralMemory::~NeuralProceduralMemory() = default;

void NeuralProceduralMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralProceduralMemory initialized");
}

void NeuralProceduralMemory::learnProcedure(const std::string& name,
                                            const std::vector<float>& pattern,
                                            const std::vector<NeuronId>& motorNeurons) {
    if (pattern.empty() || motorNeurons.empty() || !brain_) return;
    
    // Check if procedure already exists
    if (procedurePrototypes_.find(name) != procedurePrototypes_.end()) {
        // Update existing procedure
        updateProcedure(name, 0.0f, 0.0f); // No outcome for update
    } else {
        // Create new procedure
        procedurePrototypes_[name] = pattern;
        procedureMotorNeurons_[name] = motorNeurons;
        procedureProficiency_[name] = 0.0f; // Start with low proficiency
        procedureHistory_.emplace(name, std::vector<float>());
        procedurePerformance_.emplace(name, 0.0f);
        procedureContexts_.emplace(name, std::vector<float>());
        procedurePredictions_.emplace(name, std::vector<float>());
        
        // Create maintenance connections for the procedure
        for (size_t i = 0; i < motorNeurons.size(); ++i) {
            NeuronId motorNeuron = motorNeurons[i];
            // Create maintenance connection from sensory to motor neurons
            pImpl->maintenanceConnections.emplace_back(motorNeuron, motorNeuron);
            
            // Set initial pathway weight
            pImpl->motorPathwayWeights.emplace_back(motorNeuron, 0.5f);
        }
        
        // Inject pattern into neural network
        std::vector<float> patternCopy = pattern;
        for (float& val : patternCopy) {
            val *= 5.0f;  // Scale for neural injection
        }
        
        // This would normally inject into procedure-related neurons
        // For now, just log the procedure learning
    }
}

std::vector<float> NeuralProceduralMemory::executeProcedure(const std::string& name,
                                                            float proficiency) {
    auto itPattern = procedurePrototypes_.find(name);
    if (itPattern == procedurePrototypes_.end()) {
        return std::vector<float>();
    }
    
    // Get procedure pattern
    std::vector<float> pattern = itPattern->second;
    
    // Scale pattern by proficiency
    for (float& val : pattern) {
        val *= proficiency;
    }
    
    // Update history
    auto itHistory = procedureHistory_.find(name);
    if (itHistory != procedureHistory_.end()) {
        itHistory->second.push_back(proficiency);
        // Keep only recent history
        if (itHistory->second.size() > 100) {
            itHistory->second.erase(itHistory->second.begin());
        }
    }
    
    // Execute procedure by injecting into motor neurons
    auto itMotorNeurons = procedureMotorNeurons_.find(name);
    if (itMotorNeurons != procedureMotorNeurons_.end()) {
        for (NeuronId motorNeuron : itMotorNeurons->second) {
            // This would normally inject current into motor neurons
            // For now, just log the execution
        }
    }
    
    return pattern;
}

std::vector<float> NeuralProceduralMemory::retrieveProcedure(const std::string& name) const {
    auto it = procedurePrototypes_.find(name);
    if (it != procedurePrototypes_.end()) {
        return it->second;
    }
    return std::vector<float>();
}

std::vector<NeuronId> NeuralProceduralMemory::getProcedureMotorNeurons(const std::string& name) const {
    auto it = procedureMotorNeurons_.find(name);
    if (it != procedureMotorNeurons_.end()) {
        return it->second;
    }
    return std::vector<NeuronId>();
}

std::vector<std::string> NeuralProceduralMemory::getAllProcedures() const {
    std::vector<std::string> names;
    names.reserve(procedurePrototypes_.size());
    for (const auto& entry : procedurePrototypes_) {
        names.push_back(entry.first);
    }
    return names;
}

void NeuralProceduralMemory::updateProficiency(const std::string& name,
                                               float delta, float maxProficiency) {
    auto it = procedureProficiency_.find(name);
    if (it != procedureProficiency_.end()) {
        it->second = std::clamp(it->second + delta, 0.0f, maxProficiency);
        
        // Decay proficiency gradually
        if (it->second > 0.0f) {
            it->second *= (1.0f - proficiencyDecayRate_);
        }
    }
}

float NeuralProceduralMemory::getProcedureProficiency(const std::string& name) const {
    auto it = procedureProficiency_.find(name);
    if (it != procedureProficiency_.end()) {
        return it->second;
    }
    return 0.0f;
}

bool NeuralProceduralMemory::hasProcedure(const std::string& name) const {
    return procedurePrototypes_.find(name) != procedurePrototypes_.end();
}

void NeuralProceduralMemory::updateProcedure(const std::string& name, float outcome, float predictionError) {
    if (!hasProcedure(name)) return;
    
    // Update procedure based on outcome
    auto itPerf = procedurePerformance_.find(name);
    if (itPerf != procedurePerformance_.end()) {
        // Combine outcome with prediction error
        float adjustedOutcome = outcome + predictionError * predictionErrorWeight_;
        itPerf->second = itPerf->second * 0.9f + adjustedOutcome * 0.1f;
    }
    
    // Update proficiency based on performance
    float perfDelta = (outcome > 0.5f) ? 0.1f : -0.05f;
    updateProficiency(name, perfDelta);
    
    // Update prediction for next execution
    std::vector<float>& prediction = procedurePredictions_[name];
    prediction.push_back(outcome);
    if (prediction.size() > 10) {
        prediction.erase(prediction.begin());
    }
}

std::vector<float> NeuralProceduralMemory::getProcedureExecutionHistory(const std::string& name) const {
    auto it = procedureHistory_.find(name);
    if (it != procedureHistory_.end()) {
        return it->second;
    }
    return std::vector<float>();
}

float NeuralProceduralMemory::getProcedurePerformance(const std::string& name) const {
    auto it = procedurePerformance_.find(name);
    if (it != procedurePerformance_.end()) {
        return it->second;
    }
    return 0.0f;
}

void NeuralProceduralMemory::createProcedureFromExperience(const std::vector<float>& sensoryPattern,
                                                            const std::vector<float>& motorPattern,
                                                            const std::vector<NeuronId>& motorNeurons) {
    if (sensoryPattern.empty() || motorPattern.empty() || motorNeurons.empty()) return;
    
    // Create a new procedure name based on patterns
    std::string name = "procedure_" + std::to_string(procedurePrototypes_.size());
    
    // Learn the procedure
    learnProcedure(name, motorPattern, motorNeurons);
    
    // Store context for later retrieval
    procedureContexts_[name] = sensoryPattern;
}

std::vector<float> NeuralProceduralMemory::executeProcedureWithAdaptation(
    const std::string& name, const std::vector<float>& context) {
    std::vector<float> result = executeProcedure(name);
    
    if (!result.empty()) {
        // Adapt execution based on context
        for (size_t i = 0; i < result.size() && i < context.size(); ++i) {
            result[i] *= (1.0f + context[i]);
        }
        
        // Update context
        auto itContexts = procedureContexts_.find(name);
        if (itContexts != procedureContexts_.end()) {
            // Store recent context
            std::vector<float> newContext = context;
            if (newContext.size() > 10) {
                newContext.resize(10);
            }
            itContexts->second = newContext;
        }
    }
    
    return result;
}

std::vector<std::string> NeuralProceduralMemory::getProceduresForContext(
    const std::vector<float>& context) const {
    
    std::vector<std::string> procedures;
    
    for (const auto& entry : procedurePrototypes_) {
        const std::string& name = entry.first;
        
        // Check if procedure has context
        auto itContexts = procedureContexts_.find(name);
        if (itContexts != procedureContexts_.end() && !itContexts->second.empty()) {
            // Compute context similarity
            const std::vector<float>& storedContext = itContexts->second;
            if (storedContext.size() == context.size()) {
                float similarity = 0.0f;
                for (size_t i = 0; i < context.size(); ++i) {
                    similarity += std::abs(context[i] - storedContext[i]);
                }
                similarity /= context.size();
                
                // If context is similar enough, include procedure
                if (similarity < 0.5f) {
                    procedures.push_back(name);
                }
            }
        }
    }
    
    return procedures;
}

void NeuralProceduralMemory::clear() {
    procedurePrototypes_.clear();
    procedureMotorNeurons_.clear();
    procedureProficiency_.clear();
    procedureHistory_.clear();
    procedurePerformance_.clear();
    procedureContexts_.clear();
    procedurePredictions_.clear();
    pImpl->maintenanceConnections.clear();
    pImpl->motorPathwayWeights.clear();
    pImpl->consolidationState.clear();
}

} // namespace nlm
