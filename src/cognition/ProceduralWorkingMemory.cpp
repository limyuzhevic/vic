// ProceduralWorkingMemory Implementation

#include "SemanticWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

struct ProceduralWorkingMemory::Impl {
    Brain* brain;
    
    // Skill execution state
    bool skillExecuting;
    std::string currentSkill;
    float executionProgress;
    
    // Skill refinement rate
    float refinementRate;
    float practiceRate;
    
    Impl()
        : brain(nullptr)
        , skillExecuting(false)
        , executionProgress(0.0f)
        , refinementRate(0.01f)
        , practiceRate(0.02f) {}
};

ProceduralWorkingMemory::ProceduralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr) {
}

ProceduralWorkingMemory::~ProceduralMemory() = default;

void ProceduralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("ProceduralWorkingMemory initialized");
}

void ProceduralWorkingMemory::learnSkill(const std::string& skillName, const std::vector<NeuronId>& sequence, float initialProficiency) {
    if (sequence.empty() || !brain_) return;
    
    // Create or update skill neural sequence
    createSkillSequence(skillName, sequence);
    
    // Set initial proficiency
    skillProficiencies_[skillName] = initialProficiency;
    skillSequences_[skillName] = sequence;
    skillLastUsed_[skillName] = 0;
    skillUtility_[skillName] = initialProficiency * 0.8f;
    
    NLM_LOG_INFO("Learned skill: " + skillName + " (proficiency: " + std::to_string(initialProficiency) + ")");
}

std::vector<NeuronId> ProceduralWorkingMemory::retrieveSkill(const std::string& skillName) const {
    auto it = skillSequences_.find(skillName);
    if (it == skillSequences_.end()) {
        return {};
    }
    
    skillLastUsed_[skillName] = 0;
    return it->second;
}

bool ProceduralWorkingMemory::hasSkill(const std::string& skillName) const {
    return skillSequences_.find(skillName) != skillSequences_.end();
}

std::vector<std::string> ProceduralWorkingMemory::getAllSkills() const {
    std::vector<std::string> skills;
    skills.reserve(skillSequences_.size());
    
    for (const auto& pair : skillSequences_) {
        skills.push_back(pair.first);
    }
    
    return skills;
}

float ProceduralWorkingMemory::getSkillProficiency(const std::string& skillName) const {
    auto it = skillProficiencies_.find(skillName);
    if (it == skillProficiencies_.end()) {
        return 0.0f;
    }
    
    return it->second;
}

void ProceduralWorkingMemory::executeSkill(const std::string& skillName) {
    if (!brain_) return;
    
    auto it = skillSequences_.find(skillName);
    if (it == skillSequences_.end()) {
        NLM_LOG_WARNING("Cannot execute unknown skill: " + skillName);
        return;
    }
    
    const std::vector<NeuronId>& sequence = it->second;
    if (sequence.empty()) {
        return;
    }
    
    // Set execution state
    pImpl->skillExecuting = true;
    pImpl->currentSkill = skillName;
    pImpl->executionProgress = 0.0f;
    
    // Update skill last used
    skillLastUsed_[skillName] = 0;
    
    // Activate skill sequence
    for (size_t i = 0; i < sequence.size(); ++i) {
        NeuronId neuronId = sequence[i];
        float activation = 1.0f;
        
        // Scale activation based on position in sequence
        if (sequence.size() > 1) {
            activation = 1.0f - (static_cast<float>(i) / static_cast<float>(sequence.size() - 1)) * 0.5f;
        }
        
        // Inject current to activate neuron
        brain_->injectCurrent(neuronId, activation * 5.0f);
        
        // Store execution strength
        executionStrengths_[neuronId] = activation;
    }
    
    NLM_LOG_INFO("Executing skill: " + skillName);
}

void ProceduralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update skill execution
    updateSkillExecution();
    
    // Refine skills through practice
    refineSkills();
    
    // Maintain skill sequences
    maintainSkillSequences();
    
    // Update execution progress
    if (pImpl->skillExecuting) {
        pImpl->executionProgress += dt * 0.1f;
        if (pImpl->executionProgress >= 1.0f) {
            pImpl->skillExecuting = false;
            pImpl->currentSkill.clear();
        }
    }
}

void ProceduralWorkingMemory::clear() {
    skillSequences_.clear();
    skillProficiencies_.clear();
    skillLastUsed_.clear();
    skillUtility_.clear();
    
    pImpl->skillExecuting = false;
    pImpl->currentSkill.clear();
    pImpl->executionProgress = 0.0f;
    executionStrengths_.clear();
    
    NLM_LOG_INFO("ProceduralWorkingMemory cleared");
}

float ProceduralWorkingMemory::getSkillSimilarity(const std::string& skillA, const std::string& skillB) const {
    auto itA = skillSequences_.find(skillA);
    auto itB = skillSequences_.find(skillB);
    
    if (itA == skillSequences_.end() || itB == skillSequences_.end()) {
        return 0.0f;
    }
    
    const std::vector<NeuronId>& sequenceA = itA->second;
    const std::vector<NeuronId>& sequenceB = itB->second;
    
    return computeSequenceSimilarity(sequenceA, sequenceB);
}

std::vector<std::string> ProceduralWorkingMemory::getRelatedSkills(const std::string& skill, float similarityThreshold) const {
    std::vector<std::string> relatedSkills;
    
    auto it = skillSequences_.find(skill);
    if (it == skillSequences_.end()) {
        return relatedSkills;
    }
    
    const std::vector<NeuronId>& sequence = it->second;
    
    for (const auto& pair : skillSequences_) {
        if (pair.first == skill) continue;
        
        float similarity = computeSequenceSimilarity(sequence, pair.second);
        if (similarity >= similarityThreshold) {
            relatedSkills.push_back(pair.first);
        }
    }
    
    return relatedSkills;
}

void ProceduralWorkingMemory::practiceSkill(const std::string& skillName, float delta) {
    auto it = skillProficiencies_.find(skillName);
    if (it == skillProficiencies_.end()) {
        return;
    }
    
    // Improve proficiency
    it->second = std::min(1.0f, it->second + delta);
    
    // Update utility
    skillUtility_[skillName] = it->second * 0.9f + 0.1f;
    
    // Increase skill utility based on practice
    skillUtility_[skillName] = std::min(1.0f, skillUtility_[skillName] + delta * 0.5f);
    
    NLM_LOG_INFO("Practiced skill: " + skillName + " (proficiency: " + std::to_string(it->second) + ")");
}

float ProceduralWorkingMemory::getProceduralCoherence() const {
    if (skillSequences_.size() < 2) return 1.0f;
    
    float totalSimilarity = 0.0f;
    size_t similarityCount = 0;
    
    auto it = skillSequences_.begin();
    const std::vector<NeuronId>& firstSequence = it->second;
    
    for (++it; it != skillSequences_.end(); ++it) {
        float similarity = computeSequenceSimilarity(firstSequence, it->second);
        totalSimilarity += similarity;
        similarityCount++;
    }
    
    if (similarityCount == 0) return 1.0f;
    
    return totalSimilarity / similarityCount;
}

void ProceduralWorkingMemory::createSkillSequence(const std::string& skillName, const std::vector<NeuronId>& sequence) {
    // Create neural sequence for skill
    skillSequences_[skillName] = sequence;
    
    // Create connections between sequence neurons
    if (sequence.size() > 1) {
        for (size_t i = 0; i < sequence.size() - 1; ++i) {
            createSequenceConnection(sequence[i], sequence[i + 1], 0.8f);
        }
    }
}

void ProceduralWorkingMemory::refineSkill(const std::string& skillName, float proficiencyDelta) {
    auto it = skillProficiencies_.find(skillName);
    if (it == skillProficiencies_.end()) {
        return;
    }
    
    // Refine skill proficiency
    it->second = std::min(1.0f, it->second + proficiencyDelta);
    
    // Update skill utility based on refinement
    skillUtility_[skillName] = std::min(1.0f, skillUtility_[skillName] + proficiencyDelta * 0.3f);
}

void ProceduralWorkingMemory::maintainSkillSequences() {
    if (!brain_) return;
    
    // Maintain active skill sequences
    for (const auto& pair : skillSequences_) {
        const std::vector<NeuronId>& sequence = pair.second;
        
        // Inject maintenance current to keep sequences active
        for (NeuronId neuronId : sequence) {
            // Check if neuron exists and is active
            auto* region = findRegionContainingNeuron(neuronId);
            if (region) {
                brain_->injectCurrent(neuronId, 0.05f);
            }
        }
    }
}

void ProceduralWorkingMemory::updateSkillExecution() {
    if (!pImpl->skillExecuting) return;
    
    // Update execution progress
    pImpl->executionProgress += 0.1f;
    
    if (pImpl->executionProgress >= 1.0f) {
        // Complete skill execution
        pImpl->skillExecuting = false;
        pImpl->currentSkill.clear();
        
        // Update skill utility based on successful execution
        if (!pImpl->currentSkill.empty()) {
            skillUtility_[pImpl->currentSkill] = std::min(1.0f, skillUtility_[pImpl->currentSkill] + 0.1f);
        }
    }
}

void ProceduralWorkingMemory::refineSkills() {
    // Automatically refine skills based on usage and utility
    for (auto& pair : skillProficiencies_) {
        std::string skillName = pair.first;
        float proficiency = pair.second;
        float utility = skillUtility_[skillName];
        
        // Refine skill based on utility
        if (utility > 0.7f) {
            pair.second = std::min(1.0f, proficiency + pImpl->refinementRate);
        }
        
        // Update proficiency based on last use
        auto lastUseIt = skillLastUsed_.find(skillName);
        if (lastUseIt != skillLastUsed_.end() && lastUseIt->second > 50) {
            // Decay proficiency if not used recently
            pair.second = std::max(0.0f, proficiency - pImpl->refinementRate * 0.5f);
        }
    }
}

float ProceduralWorkingMemory::computeSequenceSimilarity(const std::vector<NeuronId>& sequenceA, const std::vector<NeuronId>& sequenceB) const {
    if (sequenceA.empty() || sequenceB.empty()) return 0.0f;
    
    // Compute overlap similarity
    std::unordered_set<NeuronId> neuronsA(sequenceA.begin(), sequenceA.end());
    
    size_t overlapCount = 0;
    for (NeuronId neuronId : sequenceB) {
        if (neuronsA.find(neuronId) != neuronsA.end()) {
            overlapCount++;
        }
    }
    
    // Normalize by sequence length
    float similarity = static_cast<float>(overlapCount) / std::max(sequenceA.size(), sequenceB.size());
    
    return similarity;
}

NeuralRegion* ProceduralWorkingMemory::findRegionContainingNeuron(NeuronId neuronId) {
    if (!brain_) return nullptr;
    
    for (auto& region : brain_->getRegions()) {
        auto neurons = region->getAllNeurons();
        if (neurons) {
            for (auto* neuron : *neurons) {
                if (neuron->getId() == neuronId) {
                    return region.get();
                }
            }
        }
    }
    
    return nullptr;
}

void ProceduralWorkingMemory::createSequenceConnection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists
    for (const auto& conn : skillConnections_) {
        if (std::get<0>(conn) == from && std::get<1>(conn) == to) return;
    }
    
    skillConnections_.emplace_back(from, to, strength);
}

// ProceduralWorkingMemory.cpp implementation completed
}
namespace nlm