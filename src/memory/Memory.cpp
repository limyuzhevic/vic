#include "Memory.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <vector>
#include <random>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    std::vector<std::pair<NeuronId, float>> items;
    size_t capacity;
    std::unordered_map<NeuronId, float> activityDependencies;
    float decayRate;
    float interferenceFactor;
    
    Impl(size_t cap) : capacity(cap), decayRate(0.01f), interferenceFactor(0.1f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Enhanced working memory storage with competitive dynamics
    // and biological accuracy
    
    // Check if neuron already in working memory
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Update existing item with new value
            float oldValue = item.second;
            item.second = value;
            
            // Apply synaptic modulation based on update
            if (std::abs(value - oldValue) > 0.1f) {
                // Significant change triggers neural adaptation
                float modulationFactor = 1.0f + std::abs(value - oldValue) * 0.1f;
                pImpl->activityDependencies[neuron] *= modulationFactor;
            }
            
            // Apply interference: similar activities compete
            for (auto& otherItem : pImpl->items) {
                if (&otherItem != &item && neuron != otherItem.first) {
                    float similarity = 1.0f - std::abs(value - otherItem.second) / 2.0f;
                    if (similarity > 0.8f) {
                        // High interference: reduce other item's strength
                        otherItem.second *= (1.0f - pImpl->interferenceFactor * similarity);
                    }
                }
            }
            
            return;
        }
    }
    
    // Capacity management with metabolic constraints
    if (pImpl->items.size() >= pImpl->capacity) {
        // Apply competitive selection: remove weakest item
        size_t weakestIndex = 0;
        float weakestStrength = pImpl->items[0].second;
        
        for (size_t i = 1; i < pImpl->items.size(); ++i) {
            if (std::abs(pImpl->items[i].second) < std::abs(weakestStrength)) {
                weakestStrength = pImpl->items[i].second;
                weakestIndex = i;
            }
        }
        
        pImpl->items.erase(pImpl->items.begin() + weakestIndex);
    }
    
    // Biological storage with noise and adaptation
    float biologicalStorage = value * (1.0f + (std::rand() % 200 - 100) / 100.0f);
    
    // Apply decay during storage
    float decayedStorage = biologicalStorage * (1.0f - pImpl->decayRate);
    
    pImpl->items.emplace_back(neuron, decayedStorage);
    pImpl->activityDependencies[neuron] = 1.0f;
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            return item.second;
        }
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            return true;
        }
    }
    return false;
}

void WorkingMemory::clear() {
    pImpl->items.clear();
    pImpl->activityDependencies.clear();
}

void WorkingMemory::setDecayRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.0f, 0.1f);
}

void WorkingMemory::setInterferenceFactor(float factor) {
    pImpl->interferenceFactor = std::clamp(factor, 0.0f, 1.0f);
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

void WorkingMemory::setCapacity(size_t capacity) {
    pImpl->capacity = capacity;
    // Trim items if capacity reduced
    if (pImpl->items.size() > pImpl->capacity) {
        pImpl->items.resize(pImpl->capacity);
    }
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::decay(float decayRate) {
    setDecayRate(decayRate);
    for (auto& item : pImpl->items) {
        item.second *= (1.0f - decayRate);
    }
}

float WorkingMemory::getItemStrength(NeuronId neuron) const {
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            return item.second;
        }
    }
    return 0.0f;
}

std::vector<NeuronId> WorkingMemory::getAllActiveNeurons() const {
    std::vector<NeuronId> activeNeurons;
    for (const auto& item : pImpl->items) {
        if (std::abs(item.second) > 0.01f) {
            activeNeurons.push_back(item.first);
        }
    }
    return activeNeurons;
}

float WorkingMemory::getAverageActivation() const {
    if (pImpl->items.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (const auto& item : pImpl->items) {
        sum += std::abs(item.second);
    }
    return sum / static_cast<float>(pImpl->items.size());
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    float consolidationThreshold;
    float replayProbability;
    float noveltyWeight;
    
    Impl(size_t max) : maxEpisodes(max), consolidationThreshold(0.5f), 
                        replayProbability(0.01f), noveltyWeight(0.3f) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Enhanced episodic storage with biological accuracy
    
    // Apply consolidation before storage
    EpisodicMemoryItem consolidatedEpisode = consolidateEpisode(episode);
    
    // Check capacity and remove oldest episodes if necessary
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    
    // Store with temporal tagging
    consolidatedEpisode.timestamp = episode.timestamp;
    consolidatedEpisode.reward = episode.reward;
    pImpl->episodes.push_back(consolidatedEpisode);
    
    // Apply memory prioritization based on reward and novelty
    if (episode.reward > 0.0f) {
        // Reward modulates memory strength
        float rewardBoost = std::min(2.0f, 1.0f + episode.reward * 2.0f);
        for (auto& ep : pImpl->episodes) {
            if (&ep != &consolidatedEpisode) {
                ep.reward *= 0.95f;  // Other episodes decay slightly
            }
        }
    }
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    if (index < pImpl->episodes.size()) {
        return pImpl->episodes[index];
    }
    return EpisodicMemoryItem();
}

size_t EpisodicMemory::getEpisodeCount() const {
    return pImpl->episodes.size();
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<EpisodicMemoryItem> result;
    size_t start = count > pImpl->episodes.size() ? 0 : pImpl->episodes.size() - count;
    for (size_t i = start; i < pImpl->episodes.size(); ++i) {
        result.push_back(pImpl->episodes[i]);
    }
    return result;
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getHighRewardEpisodes(float threshold) const {
    std::vector<EpisodicMemoryItem> result;
    for (const auto& episode : pImpl->episodes) {
        if (episode.reward >= threshold) {
            result.push_back(episode);
        }
    }
    return result;
}

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
}

void EpisodicMemory::setConsolidationThreshold(float threshold) {
    pImpl->consolidationThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void EpisodicMemory::setReplayProbability(float probability) {
    pImpl->replayProbability = std::clamp(probability, 0.0f, 0.1f);
}

void EpisodicMemory::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

EpisodicMemoryItem EpisodicMemory::consolidateEpisode(const EpisodicMemoryItem& episode) {
    // Enhanced episodic consolidation with biological accuracy
    
    EpisodicMemoryItem result = episode;
    
    // Temporal compression: similar time steps merge
    if (!pImpl->episodes.empty()) {
        float timeSinceLast = std::abs(episode.timestamp - pImpl->episodes.back().timestamp);
        if (timeSinceLast < 10.0f) {  // Less than 10 steps
            // Compress: average activation patterns
            std::vector<NeuronId> compressedActivations;
            std::vector<float> compressedActivationsStrength;
            
            for (const auto& neuron : episode.activeNeurons) {
                compressedActivations.push_back(neuron);
            }
            for (float strength : episode.neuronActivations) {
                compressedActivationsStrength.push_back(strength);
            }
            
            if (!pImpl->episodes.empty()) {
                auto& lastEpisode = pImpl->episodes.back();
                for (size_t i = 0; i < episode.activeNeurons.size() && i < lastEpisode.activeNeurons.size(); ++i) {
                    if (episode.activeNeurons[i] == lastEpisode.activeNeurons[i]) {
                        float blendedStrength = (compressedActivationsStrength[i] + lastEpisode.neuronActivations[i]) * 0.5f;
                        compressedActivationsStrength[i] = blendedStrength;
                    }
                }
                
                result.activeNeurons = compressedActivations;
                result.neuronActivations = compressedActivationsStrength;
            }
        }
    }
    
    // Noise reduction and regularization
    for (size_t i = 0; i < result.neuronActivations.size(); ++i) {
        // Apply biological noise and decay
        float noise = std::max(0.0f, result.neuronActivations[i] * 0.1f);
        float decayed = result.neuronActivations[i] * (1.0f - 0.01f);
        result.neuronActivations[i] = decayed - noise;
        
        // Clamp to biological limits
        result.neuronActivations[i] = std::clamp(result.neuronActivations[i], 0.0f, 1.0f);
    }
    
    // Apply novelty detection for consolidation
    if (pImpl->noveltyWeight > 0.0f) {
        // Calculate novelty based on new activation patterns
        float novelty = 0.0f;
        if (!pImpl->episodes.empty()) {
            auto& lastEpisode = pImpl->episodes.back();
            if (episode.activeNeurons.size() == lastEpisode.activeNeurons.size()) {
                for (size_t i = 0; i < episode.activeNeurons.size(); ++i) {
                    if (episode.activeNeurons[i] == lastEpisode.activeNeurons[i]) {
                        float activationDiff = std::abs(episode.neuronActivations[i] - lastEpisode.neuronActivations[i]);
                        novelty += activationDiff * pImpl->noveltyWeight;
                    }
                }
            }
        }
        
        // Novel episodes get consolidation boost
        result.noveltyScore = novelty;
    }
    
    return result;
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    setConsolidationThreshold(relevanceThreshold);
    
    // Advanced consolidation process
    std::vector<EpisodicMemoryItem> consolidated;
    
    for (const auto& episode : pImpl->episodes) {
        // Calculate episode consolidation score
        float strength = 0.0f;
        
        // Reward contribution
        strength += episode.reward * 0.4f;
        
        // Activation strength contribution
        float activationStrength = 0.0f;
        for (float activation : episode.neuronActivations) {
            activationStrength += activation;
        }
        strength += activationStrength * 0.3f;
        
        // Novelty contribution
        strength += episode.noveltyScore * pImpl->noveltyWeight * 0.3f;
        
        // Time decay: older episodes decay
        float ageDecay = std::exp(-episode.timestamp / 1000.0f);
        strength *= ageDecay;
        
        // Apply consolidation threshold
        if (strength >= pImpl->consolidationThreshold) {
            // Enhance consolidated episode
            EpisodicMemoryItem enhanced = episode;
            enhanced.consolidationStrength = strength;
            consolidated.push_back(enhanced);
        }
    }
    
    // Replace old episodes with consolidated ones
    pImpl->episodes = consolidated;
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    std::unordered_map<std::string, std::string> factMap;
    float patternCompletionThreshold;
    float fuzzyMatchingStrength;
    
    Impl() : patternCompletionThreshold(0.7f), fuzzyMatchingStrength(0.2f) {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Enhanced semantic storage with biological accuracy
    
    // Check for existing fact
    auto it = pImpl->factMap.find(key);
    if (it != pImpl->factMap.end()) {
        // Update existing fact with gradual change
        float changeFactor = 0.1f;
        std::string currentValue = it->second;
        std::string newValue;
        
        // Gradual update preserving semantic similarity
        for (size_t i = 0; i < std::min(currentValue.size(), value.size()); ++i) {
            char oldChar = currentValue[i];
            char newChar = value[i];
            char blendedChar = static_cast<char>((static_cast<int>(oldChar) * (1.0f - changeFactor) + static_cast<int>(newChar) * changeFactor));
            newValue += blendedChar;
        }
        
        it->second = newValue;
        return;
    }
    
    // Apply pattern completion for incomplete keys
    if (key.find('?') != std::string::npos) {
        // Try to complete the pattern
        std::string completedKey = completePattern(key);
        if (!completedKey.empty() && pImpl->factMap.find(completedKey) != pImpl->factMap.end()) {
            // Use existing completed fact
            pImpl->factMap[key] = pImpl->factMap[completedKey];
            return;
        }
    }
    
    // Store new fact with biological encoding
    std::string encodedValue = encodeFact(value);
    pImpl->facts.emplace_back(key, encodedValue);
    pImpl->factMap[key] = encodedValue;
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    // Enhanced retrieval with pattern completion and fuzzy matching
    auto it = pImpl->factMap.find(key);
    if (it != pImpl->factMap.end()) {
        return decodeFact(it->second);
    }
    
    // Try pattern completion
    std::string completedKey = completePattern(key);
    if (!completedKey.empty()) {
        auto it2 = pImpl->factMap.find(completedKey);
        if (it2 != pImpl->factMap.end()) {
            return decodeFact(it2->second);
        }
    }
    
    // Try fuzzy matching for partial keys
    std::string fuzzyKey = fuzzyMatch(key);
    if (!fuzzyKey.empty()) {
        auto it3 = pImpl->factMap.find(fuzzyKey);
        if (it3 != pImpl->factMap.end()) {
            return decodeFact(it3->second);
        }
    }
    
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return pImpl->factMap.find(key) != pImpl->factMap.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    for (const auto& fact : pImpl->facts) {
        result.emplace_back(fact.first, decodeFact(fact.second));
    }
    return result;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
    pImpl->factMap.clear();
}

void SemanticMemory::setPatternCompletionThreshold(float threshold) {
    pImpl->patternCompletionThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void SemanticMemory::setFuzzyMatchingStrength(float strength) {
    pImpl->fuzzyMatchingStrength = std::clamp(strength, 0.0f, 1.0f);
}

std::string SemanticMemory::completePattern(const std::string& incompleteKey) const {
    // Pattern completion using stored facts
    if (incompleteKey.find('?') == std::string::npos) {
        return incompleteKey;
    }
    
    std::string pattern = incompleteKey;
    std::replace(pattern.begin(), pattern.end(), '?', '.');
    
    for (const auto& fact : pImpl->facts) {
        if (patternMatch(fact.first, pattern)) {
            return fact.first;
        }
    }
    
    return "";
}

std::string SemanticMemory::fuzzyMatch(const std::string& key) const {
    // Fuzzy matching for approximate key retrieval
    float bestMatchScore = 0.0f;
    std::string bestMatch;
    
    for (const auto& fact : pImpl->facts) {
        float score = calculatePatternSimilarity(key, fact.first);
        if (score > bestMatchScore && score >= pImpl->patternCompletionThreshold) {
            bestMatchScore = score;
            bestMatch = fact.first;
        }
    }
    
    return bestMatch;
}

bool SemanticMemory::patternMatch(const std::string& pattern, const std::string& target) const {
    if (pattern.size() != target.size()) {
        return false;
    }
    
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (pattern[i] != '.' && pattern[i] != target[i]) {
            return false;
        }
    }
    
    return true;
}

float SemanticMemory::calculatePatternSimilarity(const std::string& key1, const std::string& key2) const {
    if (key1.size() != key2.size()) {
        return 0.0f;
    }
    
    size_t matches = 0;
    for (size_t i = 0; i < key1.size(); ++i) {
        if (key1[i] == key2[i]) {
            matches++;
        }
    }
    
    return static_cast<float>(matches) / static_cast<float>(key1.size());
}

std::string SemanticMemory::encodeFact(const std::string& value) {
    // Biological encoding of semantic information
    std::string encoded = value;
    
    // Apply neural encoding principles
    for (char& c : encoded) {
        // Simple encoding with biological plausibility
        c = static_cast<char>((static_cast<int>(c) + rand() % 10 - 5) % 256);
    }
    
    return encoded;
}

std::string SemanticMemory::decodeFact(const std::string& encoded) {
    // Decoding with error tolerance
    std::string decoded = encoded;
    
    // Simple decoding
    for (char& c : decoded) {
        c = static_cast<char>((static_cast<int>(c) - rand() % 10 + 5) % 256);
    }
    
    return decoded;
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    float learningRate;
    float decayRate;
    float noveltyThreshold;
    
    Impl() : learningRate(0.05f), decayRate(0.01f), noveltyThreshold(0.3f) {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Enhanced procedural learning with biological accuracy
    
    // Check if skill already exists
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            // Update existing skill with new pattern
            skill.neuralPattern = pattern;
            skill.acquisitionRate = std::min(1.0f, skill.acquisitionRate + pImpl->learningRate);
            skill.lastPracticed = 0.0f;
            return;
        }
    }
    
    // Create new skill with biological initialization
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.acquisitionRate = pImpl->learningRate;
    skill.proficiency = 0.0f;
    skill.lastPracticed = 0.0f;
    skill.noveltyScore = 1.0f;  // New skills start with high novelty
    
    pImpl->skills.push_back(skill);
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            return &skill;
        }
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
            skill.lastPracticed = 0.0f;
            
            // Apply decay based on practice interval
            if (skill.proficiency < 0.5f) {
                skill.proficiency *= (1.0f - pImpl->decayRate);
            }
            return;
        }
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
}

void ProceduralMemory::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.1f);
}

void ProceduralMemory::setDecayRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.0f, 0.05f);
}

void ProceduralMemory::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float ProceduralMemory::getNoveltyScore(const std::string& name) const {
    for (const auto& skill : pImpl->skills) {
        if (skill.name == name) {
            return skill.noveltyScore;
        }
    }
    return 0.0f;
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    // Pairs of (a, b) associations with strength
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    float spreadingActivationRadius;
    float interferenceThreshold;
    
    Impl() : spreadingActivationRadius(50.0f), interferenceThreshold(0.1f) {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Enhanced associative memory with biological accuracy
    
    // Check for existing association
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            // Update existing association with gradual change
            float oldStrength = std::get<2>(assoc);
            float newStrength = oldStrength + (strength - oldStrength) * 0.1f;
            std::get<2>(assoc) = std::clamp(newStrength, 0.0f, 1.0f);
            return;
        }
    }
    
    // Apply spreading activation
    float effectiveStrength = strength * calculateSpreadingActivation(a, b);
    
    // Check for interference with existing associations
    for (auto& assoc : pImpl->associations) {
        NeuronId neuronA = std::get<0>(assoc);
        NeuronId neuronB = std::get<1>(assoc);
        
        if ((neuronA == a || neuronA == b) || (neuronB == a || neuronB == b)) {
            // Interfering association exists
            float interference = std::get<2>(assoc) * pImpl->interferenceThreshold;
            effectiveStrength *= (1.0f - interference);
        }
    }
    
    pImpl->associations.emplace_back(a, b, effectiveStrength);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->associations) {
        if (std::get<0>(assoc) == neuron) {
            result.push_back(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == neuron) {
            result.push_back(std::get<0>(assoc));
        }
    }
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            return std::get<2>(assoc);
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
}

void AssociativeMemory::setSpreadingActivationRadius(float radius) {
    pImpl->spreadingActivationRadius = std::clamp(radius, 1.0f, 100.0f);
}

void AssociativeMemory::setInterferenceThreshold(float threshold) {
    pImpl->interferenceThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float AssociativeMemory::calculateSpreadingActivation(NeuronId a, NeuronId b) const {
    // Calculate spreading activation for association strength
    float maxStrength = 1.0f;
    
    // Check for neural activity
    auto* neuronA = findNeuron(a);
    auto* neuronB = findNeuron(b);
    
    if (!neuronA || !neuronB) {
        return maxStrength;
    }
    
    // Calculate activation based on neural state
    float activationA = neuronA->getMembranePotential();
    float activationB = neuronB->getMembranePotential();
    
    float avgActivation = (activationA + activationB) / 2.0f;
    
    // Apply activation modulation
    float activationModulation = std::min(1.0f, avgActivation / 50.0f);
    
    return maxStrength * (1.0f + activationModulation);
}

Neuron* AssociativeMemory::findNeuron(NeuronId neuronId) {
    // Find neuron by ID - this would need access to brain's neuron collection
    // For now, return nullptr as this is a simplified implementation
    return nullptr;
}

} // namespace nlm