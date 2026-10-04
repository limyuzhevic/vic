#include "Memory.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/STDP.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>

namespace nlm {

// Procedural Memory Neural Implementation
struct ProceduralMemory::Impl {
    Brain* brain;
    
    // Skill representations as sequential neural patterns
    std::vector<std::vector<float>> skillSequences;
    std::vector<float> skillProficiency;
    std::vector<float> skillCompleteness;
    
    // Neural routines as connected neuron chains
    std::vector<std::vector<NeuronId>> skillNeurons;
    std::vector<std::vector<std::pair<NeuronId, NeuronId>>> skillConnections;
    
    // Forward models for skill execution
    std::vector<std::vector<float>> forwardModels;
    std::vector<float> predictionErrors;
    
    // Integration with other systems
    NeuralPlanner* neuralPlanner;
    NeuralPrediction* predictionSystem;
    Neuromodulator* neuromodulator;
    
    // Skill execution state
    std::unordered_map<std::string, size_t> activeSkills;
    std::vector<size_t> executionStates;
    
    // Skill generalization parameters
    float skillGeneralizationRadius;
    float skillTransferStrength;
    
    // Map skill names to indices for quick lookup
    std::unordered_map<std::string, size_t> skillMap;
    
    Impl() : brain(nullptr), neuralPlanner(nullptr), 
             predictionSystem(nullptr), neuromodulator(nullptr),
             skillGeneralizationRadius(0.5f), skillTransferStrength(0.3f) {
        skillSequences.reserve(50);
        skillProficiency.reserve(50);
        skillCompleteness.reserve(50);
        forwardModels.reserve(50);
        predictionErrors.reserve(50);
        executionStates.reserve(50);
    }
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {
    // Initialize neural skill system
    if (pImpl) {
        pImpl->skillSequences.reserve(50);
        pImpl->skillProficiency.reserve(50);
        pImpl->skillCompleteness.reserve(50);
        pImpl->forwardModels.reserve(50);
        pImpl->predictionErrors.reserve(50);
        pImpl->executionStates.reserve(50);
    }
}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    if (!pImpl || pattern.empty()) return;
    
    // Create skill sequence from pattern
    std::vector<float> sequence(pattern.size());
    for (size_t i = 0; i < pattern.size(); ++i) {
        sequence[i] = static_cast<float>(pattern[i].value) / 1000.0f;  // Normalize
    }
    
    size_t skillIndex = getSkillIndex(name);
    
    if (skillIndex == 0) {
        // Create new skill
        createSkill(name, sequence, pattern);
    } else {
        // Update existing skill
        updateSkill(skillIndex, sequence, pattern);
    }
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    if (!pImpl) return nullptr;
    
    size_t skillIndex = getSkillIndex(name);
    if (skillIndex == 0) return nullptr;
    
    skillIndex--; // Convert to 0-based
    
    if (skillIndex >= pImpl->skills.size()) return nullptr;
    
    return &pImpl->skills[skillIndex];
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> skills;
    if (!pImpl) return skills;
    
    for (auto& skill : pImpl->skills) {
        skills.push_back(&skill);
    }
    
    return skills;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    if (!pImpl) return;
    
    size_t skillIndex = getSkillIndex(name);
    if (skillIndex == 0) return;
    
    skillIndex--; // Convert to 0-based
    
    if (skillIndex < pImpl->skillProficiency.size()) {
        pImpl->skillProficiency[skillIndex] = std::clamp(
            pImpl->skillProficiency[skillIndex] + delta, 0.0f, 1.0f);
    }
}

void ProceduralMemory::clear() {
    if (!pImpl) return;
    
    pImpl->skillSequences.clear();
    pImpl->skillProficiency.clear();
    pImpl->skillCompleteness.clear();
    pImpl->skillNeurons.clear();
    pImpl->skillConnections.clear();
    pImpl->forwardModels.clear();
    pImpl->predictionErrors.clear(); 
    pImpl->activeSkills.clear();
    pImpl->executionStates.clear();
    pImpl->skillMap.clear();
}

void ProceduralMemory::createSkill(size_t skillIndex,
                                   const std::vector<float>& sequence,
                                   const std::vector<NeuronId>& neurons) {
    if (!pImpl || skillIndex >= pImpl->skills.size()) return;
    
    // Create skill entry
    Skill& skill = pImpl->skills[skillIndex];
    
    // Gradually update skill
    for (size_t i = 0; i < sequence.size() && i < skill.neuralPattern.size(); ++i) {
        skill.neuralPattern[i] = skill.neuralPattern[i] * 0.9f + neurons[i] * 0.1f;
    }
    
    skill.proficiency = std::min(1.0f, skill.proficiency + 0.1f);
    
    // Update forward model
    auto& forwardModel = pImpl->forwardModels[skillIndex];
    for (size_t i = 0; i < sequence.size() && i < forwardModel.size(); ++i) {
        forwardModel[i] = sequence[i];
    }
    
    // Create neural connections for the skill routine
    createSkillConnections(skillIndex, neurons);
}

void ProceduralMemory::createSkillConnections(size_t skillIndex,
                                             const std::vector<NeuronId>& neurons) {
    if (!pImpl || neurons.empty() || skillIndex >= pImpl->skillConnections.size()) return;
    
    // Create sequential connections (Hebbian chaining)
    std::vector<std::pair<NeuronId, NeuronId>>& connections = pImpl->skillConnections[skillIndex];
    
    for (size_t i = 0; i < neurons.size() - 1; ++i) {
        // Create Hebbian connection between sequential neurons
        connections.emplace_back(neurons[i], neurons[i + 1]);
        
        if (pImpl->brain) {
            // In real implementation, this would create actual synaptic connections
            // For now, just track them
        }
    }
}

void ProceduralMemory::registerSkillWithPlanner(const std::string& skillName,
                                               size_t skillIndex) {
    if (!pImpl || !pImpl->neuralPlanner || skillIndex >= pImpl->skills.size()) return;
    
    const Skill& skill = pImpl->skills[skillIndex];
    
    // Register skill as available action with NeuralPlanner
    // In real implementation, this would integrate with NeuralPlanner's action system
    
    // Could use ActionType enum or create new action type for procedural skills
    // For simplicity, we track the skill name
}

} // namespace nlm
