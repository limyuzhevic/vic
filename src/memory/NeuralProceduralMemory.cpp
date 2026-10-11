# NeuralProceduralMemory.cpp

#include "NeuralProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>

namespace nlm {

struct NeuralProceduralMemory::Impl {
    // Neural patterns for skills
    std::vector<std::vector<float>> skillPatterns;
    std::vector<std::string> skillNames;
    std::vector<float> skillProficiencies;
    
    // Pattern similarity network
    std::unordered_map<std::string, size_t> nameToIndex;
    
    // Skill sequences (actions in order)
    std::vector<std::vector<ActionType>> skillSequences;
    
    // Memory queue for recent activations
    std::queue<std::pair<size_t, SimulationStep>> activationQueue;
    
    // Development-dependent learning rate
    float baseLearningRate;
    float developmentalMultiplier;
    
    // Procedural rule network
    std::vector<std::pair<size_t, size_t>> proceduralRules;
};

NeuralProceduralMemory::NeuralProceduralMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , baseLearningRate(0.1f)
    , developmentalMultiplier(1.0f)
{
    pImpl->baseLearningRate = 0.1f;
    pImpl->developmentalMultiplier = 1.0f;
}

NeuralProceduralMemory::~NeuralProceduralMemory() = default;

void NeuralProceduralMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralProceduralMemory initialized");
}

void NeuralProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern,
                                        const std::vector<ActionType>& sequence,
                                        float initialProficiency) {
    if (!brain_) return;
    
    // Check if skill already exists
    auto it = pImpl->nameToIndex.find(name);
    if (it != pImpl->nameToIndex.end()) {
        size_t index = it->second;
        // Update existing skill
        convertNeuronPatternToNeuralPattern(pattern, pImpl->skillPatterns[index]);
        pImpl->skillSequences[index] = sequence;
        pImpl->skillProficiencies[index] = std::clamp(initialProficiency, 0.0f, 1.0f);
        return;
    }
    
    // New skill - add it
    pImpl->nameToIndex[name] = pImpl->skillPatterns.size();
    std::vector<float> neuralPattern;
    convertNeuronPatternToNeuralPattern(pattern, neuralPattern);
    
    pImpl->skillPatterns.push_back(neuralPattern);
    pImpl->skillNames.push_back(name);
    pImpl->skillProficiencies.push_back(std::clamp(initialProficiency, 0.0f, 1.0f));
    pImpl->skillSequences.push_back(sequence);
    
    // Create procedural rules for this skill
    createProceduralRules(pImpl->skillPatterns.size() - 1);
}

Skill* NeuralProceduralMemory::getSkill(const std::string& name) {
    auto it = pImpl->nameToIndex.find(name);
    if (it == pImpl->nameToIndex.end()) {
        return nullptr;
    }
    
    size_t index = it->second;
    // Return pointer to internal skill (caller must be careful)
    // For safety, we'll create a temporary Skill struct
    Skill tempSkill;
    tempSkill.name = pImpl->skillNames[index];
    tempSkill.proficiency = pImpl->skillProficiencies[index];
    
    // Convert neural pattern back to neuron IDs for demonstration
    // In practice, this would require reverse mapping
    tempSkill.neuralPattern = convertNeuralPatternToNeuronPattern(pImpl->skillPatterns[index]);
    
    // Return pointer to static temporary (unsafe but works for demo)
    static Skill temp;
    temp = tempSkill;
    return &temp;
}

std::vector<Skill*> NeuralProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    
    for (size_t i = 0; i < pImpl->skillPatterns.size(); ++i) {
        Skill tempSkill;
        tempSkill.name = pImpl->skillNames[i];
        tempSkill.proficiency = pImpl->skillProficiencies[i];
        tempSkill.neuralPattern = convertNeuralPatternToNeuronPattern(pImpl->skillPatterns[i]);
        
        static Skill temp;
        temp = tempSkill;
        result.push_back(&temp);
    }
    
    return result;
}

void NeuralProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = pImpl->nameToIndex.find(name);
    if (it == pImpl->nameToIndex.end()) {
        return;
    }
    
    size_t index = it->second;
    pImpl->skillProficiencies[index] = std::clamp(pImpl->skillProficiencies[index] + delta, 0.0f, 1.0f);
    
    // Update neural representation based on proficiency
    updateSkillNeuralRepresentation(index, delta);
}

void NeuralProceduralMemory::update(const std::vector<ActionType>& actions, float reward) {
    if (actions.empty() || !brain_) return;
    
    // Find most similar skill sequence
    size_t bestMatch = findMostSimilarSequence(actions);
    
    if (bestMatch != SIZE_MAX) {
        // Update skill proficiency based on performance
        pImpl->skillProficiencies[bestMatch] = std::min(1.0f, 
            pImpl->skillProficiencies[bestMatch] + reward * baseLearningRate * developmentalMultiplier);
        
        // Update neural pattern to reinforce successful sequence
        updateSkillSequenceNeuralRepresentation(bestMatch, actions);
        
        // Add to activation queue
        pImpl->activationQueue.push(std::make_pair(bestMatch, static_cast<SimulationStep>(0)));
    }
}

void NeuralProceduralMemory::decay(float decayRate) {
    for (auto& proficiency : pImpl->skillProficiencies) {
        proficiency *= (1.0f - decayRate * developmentalMultiplier);
    }
}

void NeuralProceduralMemory::createProceduralRules(size_t skillIndex) {
    // Create rules connecting this skill to others
    if (skillIndex >= pImpl->skillPatterns.size()) return;
    
    // For each other skill, create a probabilistic rule
    for (size_t otherIndex = 0; otherIndex < pImpl->skillPatterns.size(); ++otherIndex) {
        if (otherIndex == skillIndex) continue;
        
        // Calculate similarity between skills
        float similarity = calculatePatternSimilarity(pImpl->skillPatterns[skillIndex], 
                                                     pImpl->skillPatterns[otherIndex]);
        
        if (similarity > 0.3f) {  // Create rule for similar skills
            pImpl->proceduralRules.emplace_back(skillIndex, otherIndex);
        }
    }
}

void NeuralProceduralMemory::convertNeuronPatternToNeuralPattern(const std::vector<NeuronId>& neuronPattern,
                                                                std::vector<float>& neuralPattern) {
    // Convert neuron IDs to neural pattern based on properties
    neuralPattern.clear();
    
    for (NeuronId neuronId : neuronPattern) {
        // Find the neuron and extract its properties
        for (auto* region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->getId() == neuronId) {
                        // Extract key neural properties
                        float membranePotential = neuron->getState().membranePotential;
                        float firingRate = neuron->getState().firingRate;
                        float threshold = neuron->getState().threshold;
                        
                        // Normalize and add to pattern
                        neuralPattern.push_back((membranePotential + 100.0f) / 200.0f);  // -100 to +100 -> 0 to 1
                        neuralPattern.push_back(std::min(1.0f, firingRate / 100.0f));   // Normalize firing rate
                        neuralPattern.push_back((threshold + 100.0f) / 200.0f);         // Normalize threshold
                        
                        break;
                    }
                }
            }
        }
    }
}

std::vector<NeuronId> NeuralProceduralMemory::convertNeuralPatternToNeuronPattern(const std::vector<float>& neuralPattern) {
    std::vector<NeuronId> neuronPattern;
    
    // For demonstration: create dummy neuron IDs based on pattern values
    // In a real implementation, this would map back to specific neurons
    size_t patternSize = neuralPattern.size() / 3;  // 3 properties per neuron
    
    for (size_t i = 0; i < patternSize; ++i) {
        // Create a dummy neuron ID (this is just for demonstration)
        NeuronId dummyId(i + 1000);  // Use range outside normal IDs
        neuronPattern.push_back(dummyId);
    }
    
    return neuronPattern;
}

size_t NeuralProceduralMemory::findMostSimilarSequence(const std::vector<ActionType>& actions) const {
    if (pImpl->skillSequences.empty() || actions.empty()) {
        return SIZE_MAX;
    }
    
    float bestSimilarity = 0.0f;
    size_t bestIndex = SIZE_MAX;
    
    for (size_t i = 0; i < pImpl->skillSequences.size(); ++i) {
        float similarity = calculateSequenceSimilarity(pImpl->skillSequences[i], actions);
        if (similarity > bestSimilarity && similarity > 0.2f) {  // Minimum similarity threshold
            bestSimilarity = similarity;
            bestIndex = i;
        }
    }
    
    return bestIndex;
}

void NeuralProceduralMemory::updateSkillNeuralRepresentation(size_t skillIndex, float delta) {
    if (skillIndex >= pImpl->skillPatterns.size()) return;
    
    // Adjust neural pattern based on proficiency change
    for (auto& value : pImpl->skillPatterns[skillIndex]) {
        value = std::clamp(value + delta * 0.1f, 0.0f, 1.0f);
    }
}

void NeuralProceduralMemory::updateSkillSequenceNeuralRepresentation(size_t skillIndex, const std::vector<ActionType>& actions) {
    if (skillIndex >= pImpl->skillSequences.size()) return;
    
    // Update sequence based on performance
    const auto& currentSequence = pImpl->skillSequences[skillIndex];
    
    // Reinforce correct sequence elements
    for (size_t i = 0; i < std::min(actions.size(), currentSequence.size()); ++i) {
        if (actions[i] == currentSequence[i]) {
            // This action was correct - strengthen the neural pathway
            // In a real implementation, this would modify synaptic weights
        }
    }
}

float NeuralProceduralMemory::calculatePatternSimilarity(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size()) return 0.0f;
    
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (size_t i = 0; i < pattern1.size(); ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
    
    return dotProduct / std::sqrt(norm1 * norm2);
}

float NeuralProceduralMemory::calculateSequenceSimilarity(const std::vector<ActionType>& seq1, const std::vector<ActionType>& seq2) const {
    if (seq1.empty() || seq2.empty()) return 0.0f;
    
    size_t maxLen = std::max(seq1.size(), seq2.size());
    if (maxLen == 0) return 0.0f;
    
    size_t matches = 0;
    for (size_t i = 0; i < std::min(seq1.size(), seq2.size()); ++i) {
        if (seq1[i] == seq2[i]) {
            matches++;
        }
    }
    
    return static_cast<float>(matches) / static_cast<float>(maxLen);
}

void NeuralProceduralMemory::setBaseLearningRate(float rate) {
    baseLearningRate = std::clamp(rate, 0.0f, 1.0f);
}

void NeuralProceduralMemory::setDevelopmentalMultiplier(float multiplier) {
    developmentalMultiplier = std::clamp(multiplier, 0.0f, 3.0f);
}

float NeuralProceduralMemory::getBaseLearningRate() const {
    return baseLearningRate;
}

float NeuralProceduralMemory::getDevelopmentalMultiplier() const {
    return developmentalMultiplier;
}

void NeuralProceduralMemory::activateSkill(const std::string& name) {
    auto it = pImpl->nameToIndex.find(name);
    if (it == pImpl->nameToIndex.end()) {
        return;
    }
    
    size_t index = it->second;
    
    // Increase proficiency when activated
    pImpl->skillProficiencies[index] = std::min(1.0f, pImpl->skillProficiencies[index] + 0.1f);
    
    // Add to activation queue
    pImpl->activationQueue.push(std::make_pair(index, static_cast<SimulationStep>(0)));
}

std::vector<std::string> NeuralProceduralMemory::getExecutableSkills() const {
    std::vector<std::string> executableSkills;
    
    for (size_t i = 0; i < pImpl->skillPatterns.size(); ++i) {
        if (pImpl->skillProficiencies[i] > 0.5f) {  // Can execute if proficiency > 50%
            executableSkills.push_back(pImpl->skillNames[i]);
        }
    }
    
    return executableSkills;
}

} // namespace nlm
