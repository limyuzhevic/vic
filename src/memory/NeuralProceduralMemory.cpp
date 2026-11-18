#include "NeuralProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

NeuralProceduralMemory::NeuralProceduralMemory()
    : brain_(nullptr)
{
}

NeuralProceduralMemory::~NeuralProceduralMemory() = default;

void NeuralProceduralMemory::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("Procedural memory initialized");
}

void NeuralProceduralMemory::storeProcedure(const std::string& actionName, const MotorCommand& command, const std::vector<float>& context) {
    // Check if action already exists
    auto it = nameToIndex_.find(actionName);
    if (it != nameToIndex_.end()) {
        // Update existing action
        size_t index = it->second;
        procedures_[index].command = command;
        procedures_[index].context = context;
        procedures_[index].lastUpdated = 0;
        procedures_[index].successCount = 0;
        procedures_[index].failureCount = 0;
    } else {
        // Add new action
        Procedure newProcedure;
        newProcedure.name = actionName;
        newProcedure.command = command;
        newProcedure.context = context;
        newProcedure.effectiveness = 0.5f;
        newProcedure.successCount = 0;
        newProcedure.failureCount = 0;
        newProcedure.lastUpdated = 0;
        
        procedures_.push_back(newProcedure);
        nameToIndex_[actionName] = procedures_.size() - 1;
        
        NLM_LOG_INFO("Stored new procedural memory: " + actionName);
    }
}

MotorCommand NeuralProceduralMemory::retrieveProcedure(const std::string& actionName) const {
    auto it = nameToIndex_.find(actionName);
    if (it != nameToIndex_.end()) {
        return procedures_[it->second].command;
    }
    return MotorCommand(); // Empty command
}

std::vector<std::string> NeuralProceduralMemory::getActionNames() const {
    std::vector<std::string> names;
    names.reserve(procedures_.size());
    for (const auto& procedure : procedures_) {
        names.push_back(procedure.name);
    }
    return names;
}

std::vector<float> NeuralProceduralMemory::getActionContext(const std::string& actionName) const {
    auto it = nameToIndex_.find(actionName);
    if (it != nameToIndex_.end()) {
        return procedures_[it->second].context;
    }
    return std::vector<float>();
}

void NeuralProceduralMemory::updateProcedures(const std::string& actionName, bool success, float reward) {
    auto it = nameToIndex_.find(actionName);
    if (it != nameToIndex_.end()) {
        size_t index = it->second;
        if (success) {
            procedures_[index].successCount++;
            procedures_[index].effectiveness = std::min(1.0f, procedures_[index].effectiveness + 0.05f);
        } else {
            procedures_[index].failureCount++;
            procedures_[index].effectiveness = std::max(0.0f, procedures_[index].effectiveness - 0.02f);
        }
        procedures_[index].lastUpdated = 0;
    }
}

float NeuralProceduralMemory::getActionEffectiveness(const std::string& actionName) const {
    auto it = nameToIndex_.find(actionName);
    if (it != nameToIndex_.end()) {
        const Procedure& procedure = procedures_[it->second];
        float totalAttempts = procedure.successCount + procedure.failureCount;
        if (totalAttempts > 0) {
            return procedure.successCount / totalAttempts;
        }
    }
    return 0.0f;
}

void NeuralProceduralMemory::clear() {
    procedures_.clear();
    nameToIndex_.clear();
    NLM_LOG_INFO("Procedural memory cleared");
}

size_t NeuralProceduralMemory::getProcedureCount() const {
    return procedures_.size();
}

void NeuralProceduralMemory::decayProcedures(float decayRate) {
    if (decayRate <= 0.0f) return;
    
    for (auto& procedure : procedures_) {
        procedure.effectiveness *= (1.0f - decayRate);
        if (procedure.effectiveness < 0.05f) {
            // Remove ineffective procedures
            nameToIndex_.erase(procedure.name);
        }
    }
    
    // Rebuild procedures vector after removal
    std::vector<Procedure> newProcedures;
    for (const auto& procedure : procedures_) {
        if (procedure.effectiveness >= 0.05f) {
            newProcedures.push_back(procedure);
        }
    }
    procedures_ = std::move(newProcedures);
    
    // Update nameToIndex map
    nameToIndex_.clear();
    for (size_t i = 0; i < procedures_.size(); ++i) {
        nameToIndex_[procedures_[i].name] = i;
    }
}

std::string NeuralProceduralMemory::selectBestAction(const std::vector<float>& context) const {
    std::string bestAction = "";
    float bestScore = -1.0f;
    
    for (const auto& procedure : procedures_) {
        float score = 0.0f;
        
        // Check context similarity
        float contextSimilarity = 0.0f;
        if (!procedure.context.empty() && !context.empty()) {
            if (procedure.context.size() == context.size()) {
                float dotProduct = 0.0f;
                for (size_t i = 0; i < procedure.context.size(); ++i) {
                    dotProduct += procedure.context[i] * context[i];
                }
                contextSimilarity = dotProduct / (procedure.context.size() * context.size());
            }
        } else {
            contextSimilarity = 1.0f; // Use if no context specified
        }
        
        // Combine effectiveness and context similarity
        score = procedure.effectiveness * 0.7f + contextSimilarity * 0.3f;
        
        if (score > bestScore) {
            bestScore = score;
            bestAction = procedure.name;
        }
    }
    
    return bestAction;
}

const std::vector<std::pair<std::string, std::pair<MotorCommand, std::vector<float>>>>& NeuralProceduralMemory::getAllProcedures() const {
    std::vector<std::pair<std::string, std::pair<MotorCommand, std::vector<float>>>> result;
    result.reserve(procedures_.size());
    
    for (const auto& procedure : procedures_) {
        result.emplace_back(procedure.name, 
                           std::make_pair(procedure.command, procedure.context));
    }
    
    return result;
}

} // namespace nlm