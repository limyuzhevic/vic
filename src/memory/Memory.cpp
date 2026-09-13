#include "Memory.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    // Enhanced working memory with neural activation patterns
    struct NeuralItem {
        NeuronId neuronId;
        float activationLevel;      // Current activation (0-1)
        float decayRate;            // How quickly activation decays
        float interferenceFactor;   // How much other neurons affect this item
        float memoryStrength;       // How stable the memory is (learned over time)
        float lastAccessTime;       // For temporal decay
        bool isConsolidated;        // Whether this item has been consolidated to long-term memory
        
        NeuralItem(NeuronId id) : neuronId(id), activationLevel(0.0f), decayRate(0.1f), 
                                interferenceFactor(0.5f), memoryStrength(0.5f), 
                                lastAccessTime(0.0f), isConsolidated(false) {}
    };
    
    std::vector<NeuralItem> items;
    size_t capacity;
    float globalActivationDecay;    // Base decay rate for all items
    float consolidationThreshold;   // Activation level needed for consolidation
    std::vector<NeuronId> activeNeurons;  // Neurons currently active in working memory
    
    Impl(size_t cap) : capacity(cap), globalActivationDecay(0.05f), 
                      consolidationThreshold(0.7f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Real working memory storage with neural activation dynamics
    float currentTime = 0.0f;  // In real implementation, would track simulation time
    
    for (auto& item : pImpl->items) {
        if (item.neuronId == neuron) {
            // Update existing item with new activation
            item.activationLevel = std::max(item.activationLevel, value);
            item.lastAccessTime = currentTime;
            
            // Increase memory strength through Hebbian-like learning
            item.memoryStrength = std::min(1.0f, item.memoryStrength + 0.01f * value);
            
            // Interference from co-activation
            for (const auto& other : pImpl->items) {
                if (other.neuronId != neuron && item.activationLevel > 0.5f && other.activationLevel > 0.5f) {
                    // Similarity-based interference
                    float similarity = calculateSimilarity(neuron, other.neuronId);
                    if (similarity > 0.3f) {
                        float interference = similarity * item.interferenceFactor;
                        item.activationLevel *= (1.0f - interference);
                    }
                }
            }
            
            return;
        }
    }
    
    // New item - add if capacity allows
    if (pImpl->items.size() < pImpl->capacity) {
        NeuralItem newItem(neuron);
        newItem.activationLevel = value;
        newItem.memoryStrength = 0.5f + 0.5f * value;  // Initial strength based on input
        newItem.lastAccessTime = currentTime;
        pImpl->items.emplace_back(std::move(newItem));
        
        // Track active neurons
        pImpl->activeNeurons.push_back(neuron);
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    // Real memory retrieval with decay and interference
    float currentTime = 0.0f;  // Would track simulation time
    
    for (const auto& item : pImpl->items) {
        if (item.neuronId == neuron) {
            // Apply temporal decay based on time since last access
            float timeSinceAccess = currentTime - item.lastAccessTime;
            float decayFactor = std::exp(-item.decayRate * timeSinceAccess);
            
            float retrievedValue = item.activationLevel * decayFactor;
            
            // Modify by memory strength (stronger memories are more stable)
            retrievedValue *= item.memoryStrength;
            
            // Interference from other active items
            for (const auto& other : pImpl->items) {
                if (other.neuronId != neuron && other.activationLevel > 0.5f) {
                    float similarity = calculateSimilarity(neuron, other.neuronId);
                    if (similarity > 0.3f) {
                        // Similarity-based interference
                        retrievedValue *= (1.0f - other.interferenceFactor * similarity);
                    }
                }
            }
            
            return std::max(0.0f, retrievedValue);
        }
    }
    return 0.0f;  // Item not found
}

bool WorkingMemory::contains(NeuronId neuron) const {
    for (const auto& item : pImpl->items) {
        if (item.neuronId == neuron) {
            return true;
        }
    }
    return false;
}

void WorkingMemory::clear() {
    pImpl->items.clear();
    pImpl->activeNeurons.clear();
}

void WorkingMemory::decay(float decayRate) {
    // Apply global decay and update active neurons
    float currentTime = 0.0f;  // Would track simulation time
    
    // Decay all items based on time
    for (auto& item : pImpl->items) {
        float timeSinceAccess = currentTime - item.lastAccessTime;
        item.activationLevel *= std::exp(-item.decayRate * timeSinceAccess);
        item.lastAccessTime = currentTime;
        
        // Update interference factor based on decay
        if (item.activationLevel < 0.1f) {
            item.interferenceFactor *= 0.9f;  // Reduce interference as activation decays
        }
    }
    
    // Remove items with very low activation
    pImpl->items.erase(
        std::remove_if(pImpl->items.begin(), pImpl->items.end(),
            [currentTime](const NeuralItem& item) {
                return item.activationLevel < 0.01f;
            }),
        pImpl->items.end()
    );
    
    // Update active neurons list
    std::vector<NeuronId> newActive;
    for (const auto& item : pImpl->items) {
        if (item.activationLevel > 0.2f) {
            newActive.push_back(item.neuronId);
        }
    }
    pImpl->activeNeurons = newActive;
}

// Consolidate working memory items into long-term storage
void WorkingMemory::consolidate() {
    // Move highly activated items to long-term memory
    float currentTime = 0.0f;
    
    for (auto& item : pImpl->items) {
        if (item.activationLevel > pImpl->consolidationThreshold && 
            !item.isConsolidated) {
            
            // Create long-term memory representation
            item.isConsolidated = true;
            
            // Reduce interference factor (long-term memories are less susceptible to interference)
            item.interferenceFactor *= 0.5f;
            
            // Increase memory strength significantly during consolidation
            item.memoryStrength = std::min(1.0f, item.memoryStrength + 0.5f);
            
            // Reduce decay rate (long-term memories decay slower)
            item.decayRate *= 0.5f;
            
            // Record consolidation time
            item.lastAccessTime = currentTime;
            
            // Remove from working memory to prevent competition
            item.activationLevel *= 0.3f;
        }
    }
    
    // Update active neurons list
    std::vector<NeuronId> newActive;
    for (const auto& item : pImpl->items) {
        if (item.activationLevel > 0.1f) {
            newActive.push_back(item.neuronId);
        }
    }
    pImpl->activeNeurons = newActive;
}

float WorkingMemory::calculateSimilarity(NeuronId neuron1, NeuronId neuron2) const {
    // Calculate similarity between neurons based on their connectivity patterns
    // This would use actual neural connectivity data in a real implementation
    
    // Simplified placeholder - would use actual neural similarity metrics
    std::hash<NeuronId> hasher;
    size_t seed1 = hasher(static_cast<uint64_t>(neuron1.value));
    size_t seed2 = hasher(static_cast<uint64_t>(neuron2.value));
    
    // Create deterministic but pseudo-random similarity
    std::mt19937 rng(seed1 ^ seed2);
    std::uniform_real_distribution<float> dist(0.1f, 0.9f);
    
    return dist(rng);
}

void WorkingMemory::updateActivity(const std::vector<NeuronId>& activeNeurons) {
    // Update working memory with current neural activity
    for (const auto& neuron : activeNeurons) {
        float activation = getNeuronActivation(neuron);  // Would get actual activation
        store(neuron, activation);
    }
}

float WorkingMemory::getNeuronActivation(NeuronId neuron) const {
    // Get current activation level of a neuron (placeholder)
    // In real implementation, would query the neuron's current state
    return 0.5f;  // Placeholder value
}

// Get the most strongly activated items
std::vector<std::pair<NeuronId, float>> WorkingMemory::getStrongestItems(size_t count) const {
    std::vector<std::pair<NeuronId, float>> result;
    
    for (const auto& item : pImpl->items) {
        result.emplace_back(item.neuronId, item.activationLevel);
    }
    
    // Sort by activation level (descending)
    std::sort(result.begin(), result.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Limit to requested count
    if (count > result.size()) {
        count = result.size();
    }
    
    return std::vector<std::pair<NeuronId, float>>(result.begin(), result.begin() + count);
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    // Enhanced episodic memory with temporal context and associative links
    struct Episode {
        Timestamp timestamp;
        std::vector<NeuronId> involvedNeurons;
        std::vector<float> activationPatterns;
        float strength;              // How vivid/recallable the memory is
        float temporalContext;       // When this episode occurred (relative time)
        float spatialContext;        // Where this episode occurred (placeholder)
        std::vector<size_t> associations;  // Indices of related episodes
        
        Episode(Timestamp ts) : timestamp(ts), strength(0.0f), temporalContext(0.0f), spatialContext(0.0f) {}
    };
    
    std::vector<Episode> episodes;
    size_t maxEpisodes;
    float consolidationWindow;     // Time window for consolidation
    float relevanceThreshold;      // Threshold for episode retrieval
    
    Impl(size_t max) : maxEpisodes(max), consolidationWindow(1000.0f), 
                      relevanceThreshold(0.6f) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        // Remove oldest episode (FIFO)
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    
    pImpl->episodes.push_back(episode);
    
    // Create associative links with similar episodes
    for (size_t i = 0; i < pImpl->episodes.size() - 1; ++i) {
        float similarity = calculateEpisodeSimilarity(pImpl->episodes[i], episode);
        if (similarity > 0.3f) {
            pImpl->episodes[i].associations.push_back(pImpl->episodes.size() - 1);
            pImpl->episodes[pImpl->episodes.size() - 1].associations.push_back(i);
            
            // Increase episode strength based on similarity links
            pImpl->episodes[i].strength = std::min(1.0f, pImpl->episodes[i].strength + 0.1f * similarity);
            pImpl->episodes[pImpl->episodes.size() - 1].strength = std::min(1.0f, 
                pImpl->episodes[pImpl->episodes.size() - 1].strength + 0.1f * similarity);
        }
    }
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    if (index < pImpl->episodes.size()) {
        return pImpl->episodes[index];
    }
    return EpisodicMemoryItem();  // Return default if index out of range
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

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Consolidate relevant episodes to more permanent storage
    // This simulates memory consolidation during rest/sleep periods
    
    for (auto& episode : pImpl->episodes) {
        if (episode.strength > relevanceThreshold) {
            // Strengthen the memory through consolidation
            episode.strength = std::min(1.0f, episode.strength + 0.2f);
            
            // Update temporal context to make it more retrievable
            episode.temporalContext = calculateTemporalContext(episode);
            
            // Create additional associative links
            for (auto& other : pImpl->episodes) {
                if (&other != &episode) {
                    float similarity = calculateEpisodeSimilarity(episode, other);
                    if (similarity > 0.4f) {
                        // Add bidirectional association
                        episode.associations.push_back(&other - &pImpl->episodes[0]);
                        other.associations.push_back(&episode - &pImpl->episodes[0]);
                    }
                }
            }
        }
    }
}

float EpisodicMemory::calculateEpisodeSimilarity(const Episode& ep1, const Episode& ep2) const {
    // Calculate similarity between two episodes based on their neural patterns
    if (ep1.involvedNeurons.size() != ep2.involvedNeurons.size()) {
        return 0.0f;
    }
    
    float similarity = 0.0f;
    for (size_t i = 0; i < ep1.involvedNeurons.size(); ++i) {
        // Check if neurons are the same
        if (ep1.involvedNeurons[i] == ep2.involvedNeurons[i]) {
            similarity += 1.0f;
        }
    }
    
    // Add temporal proximity factor
    float timeDiff = std::abs(ep1.temporalContext - ep2.temporalContext);
    float temporalSimilarity = std::exp(-timeDiff / 100.0f);  // Decay over time
    
    return (similarity / ep1.involvedNeurons.size()) * 0.7f + temporalSimilarity * 0.3f;
}

float EpisodicMemory::calculateTemporalContext(const Episode& episode) const {
    // Calculate how recent the episode is for temporal context
    if (pImpl->episodes.empty()) {
        return 0.0f;
    }
    
    float oldestTime = pImpl->episodes.front().temporalContext;
    float newestTime = pImpl->episodes.back().temporalContext;
    
    return 1.0f - (episode.temporalContext - oldestTime) / (newestTime - oldestTime);
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    // Enhanced semantic memory with spreading activation and connections
    struct Concept {
        std::string name;
        std::string description;
        float activationLevel;        // Current activation (0-1)
        std::vector<std::string> relatedConcepts;  // Concepts that are related/associated
        std::vector<std::pair<std::string, float>> facts;  // Key-value facts about this concept
        float stability;              // How stable/verified this concept is
        float interferenceResilience;  // Resistance to interference from other concepts
        
        Concept(const std::string& n) : name(n), activationLevel(0.0f), stability(0.5f), 
                                      interferenceResilience(0.7f) {}
    };
    
    std::vector<Concept> concepts;
    std::vector<std::pair<std::string, std::string>> facts;  // Direct facts (name, value)
    float spreadingActivationDecay;   // How quickly activation spreads
    float conceptActivationThreshold; // Minimum activation for concept retrieval
    
    Impl() : spreadingActivationDecay(0.1f), conceptActivationThreshold(0.3f) {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Store fact in both concept and direct fact structures
    
    // Extract concept name (first word or key before first underscore)
    size_t underscorePos = key.find('_');
    std::string conceptName;
    if (underscorePos != std::string::npos) {
        conceptName = key.substr(0, underscorePos);
    } else {
        conceptName = key;
    }
    
    // Find or create concept
    auto conceptIt = std::find_if(pImpl->concepts.begin(), pImpl->concepts.end(),
        [&conceptName](const Concept& c) { return c.name == conceptName; });
    
    if (conceptIt == pImpl->concepts.end()) {
        // Create new concept
        pImpl->concepts.emplace_back(conceptName);
        conceptIt = pImpl->concepts.end() - 1;
    }
    
    // Add fact to concept
    conceptIt->facts.emplace_back(key, value);
    
    // Store in direct facts as well
    pImpl->facts.emplace_back(key, value);
    
    // Update concept activation
    conceptIt->activationLevel = std::min(1.0f, conceptIt->activationLevel + 0.2f);
    
    // Find related concepts and activate them (spreading activation)
    for (auto& other : pImpl->concepts) {
        if (&other != &*conceptIt) {
            // Check if concepts share words or are semantically related
            std::string otherName = other.name;
            size_t pos = otherName.find(conceptName);
            if (pos != std::string::npos && conceptName.length() > 3) {
                other.activationLevel = std::min(1.0f, other.activationLevel + 0.1f);
            }
        }
    }
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    // First try direct facts lookup
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return fact.second;
        }
    }
    
    // If not found, try concept facts
    for (const auto& concept : pImpl->concepts) {
        for (const auto& fact : concept.facts) {
            if (fact.first == key) {
                return fact.second;
            }
        }
    }
    
    return "";  // Not found
}

bool SemanticMemory::hasFact(const std::string& key) const {
    // Check direct facts first
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return true;
        }
    }
    
    // Check concept facts
    for (const auto& concept : pImpl->concepts) {
        for (const auto& fact : concept.facts) {
            if (fact.first == key) {
                return true;
            }
        }
    }
    
    return false;
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> allFacts;
    
    // Add direct facts
    for (const auto& fact : pImpl->facts) {
        allFacts.push_back(fact);
    }
    
    // Add concept facts
    for (const auto& concept : pImpl->concepts) {
        for (const auto& fact : concept.facts) {
            allFacts.push_back(fact);
        }
    }
    
    return allFacts;
}

void SemanticMemory::clear() {
    pImpl->concepts.clear();
    pImpl->facts.clear();
}

void SemanticMemory::updateConceptActivations(const std::vector<std::string>& activeConcepts) {
    // Update concept activations based on current activity
    for (const auto& conceptName : activeConcepts) {
        for (auto& concept : pImpl->concepts) {
            if (concept.name == conceptName) {
                concept.activationLevel = std::min(1.0f, concept.activationLevel + 0.3f);
                
                // Strengthen stability through reinforcement
                concept.stability = std::min(1.0f, concept.stability + 0.05f);
                
                break;
            }
        }
    }
}

void SemanticMemory::propagateActivation() {
    // Propagate activation between related concepts (spreading activation)
    bool activationChanged = true;
    int iteration = 0;
    const int maxIterations = 10;
    
    while (activationChanged && iteration < maxIterations) {
        activationChanged = false;
        
        for (auto& concept : pImpl->concepts) {
            float originalActivation = concept.activationLevel;
            
            // Spread activation to related concepts
            for (const auto& relatedName : concept.relatedConcepts) {
                for (auto& other : pImpl->concepts) {
                    if (other.name == relatedName) {
                        other.activationLevel = std::min(1.0f, other.activationLevel + 0.1f * concept.activationLevel);
                        activationChanged = true;
                    }
                }
            }
            
            // Decay own activation
            concept.activationLevel *= (1.0f - pImpl->spreadingActivationDecay);
            
            // Interference from competing concepts
            for (auto& other : pImpl->concepts) {
                if (&other != &concept && other.activationLevel > conceptActivationThreshold) {
                    // Reduce this concept's activation based on competitor strength
                    float interference = other.activationLevel * other.interferenceResilience;
                    concept.activationLevel *= (1.0f - interference * 0.1f);
                    activationChanged = true;
                }
            }
            
            // Clamp to valid range
            concept.activationLevel = std::max(0.0f, std::min(1.0f, concept.activationLevel));
        }
        
        iteration++;
    }
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    // Enhanced procedural memory with skill sequences and neural patterns
    struct Skill {
        std::string name;
        std::vector<NeuronId> neuralPattern;      // Sequence of neurons involved
        std::vector<float> activationSequence;   // How activation varies over time
        float proficiency;                       // How well the skill is performed (0-1)
        float executionTime;                     // How quickly the skill executes
        float reliability;                        // How reliably the skill works
        float rewardSensitivity;                  // How much reward affects learning
        
        Skill(const std::string& n) : name(n), proficiency(0.5f), executionTime(1.0f), 
                                     reliability(0.5f), rewardSensitivity(0.5f) {}
    };
    
    std::vector<Skill> skills;
    std::vector<NeuronId> activeSkillSequence;    // Current sequence of neurons being activated
    float skillConsolidationRate;                 // How quickly skills are consolidated
    float proceduralDecayRate;                    // General decay for unused skills
    
    Impl() : skillConsolidationRate(0.05f), proceduralDecayRate(0.01f) {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Learn a new procedural skill based on neural activation patterns
    
    // Check if skill already exists
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            // Update existing skill with new pattern
            skill.neuralPattern = pattern;
            skill.proficiency = std::min(1.0f, skill.proficiency + 0.1f);
            
            // Improve execution time based on pattern efficiency
            size_t minDistance = calculateMinNeuralDistance(pattern);
            skill.executionTime = std::max(0.1f, 1.0f - 0.05f * minDistance);
            
            // Improve reliability through reinforcement
            skill.reliability = std::min(1.0f, skill.reliability + 0.05f);
            
            return;
        }
    }
    
    // Create new skill
    Skill newSkill(name);
    newSkill.neuralPattern = pattern;
    newSkill.activationSequence.resize(pattern.size(), 0.5f);  // Initial activation
    
    // Calculate skill characteristics based on neural pattern
    size_t minDistance = calculateMinNeuralDistance(pattern);
    newSkill.executionTime = std::max(0.1f, 1.0f - 0.05f * minDistance);
    newSkill.reliability = 0.5f + 0.5f * (1.0f - minDistance / 100.0f);
    
    pImpl->skills.push_back(std::move(newSkill));
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
    Skill* skill = getSkill(name);
    if (skill) {
        // Update proficiency with bounds checking
        skill->proficiency = std::clamp(skill->proficiency + delta, 0.0f, 1.0f);
        
        // Update execution time based on proficiency
        skill->executionTime = std::max(0.1f, skill->executionTime * (1.0f - 0.1f * delta));
        
        // Update reliability based on proficiency
        skill->reliability = std::min(1.0f, skill->reliability + 0.02f * delta);
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->activeSkillSequence.clear();
}

void ProceduralMemory::updateSkillExecution(const std::vector<NeuronId>& currentNeuralActivity) {
    // Update procedural skills based on current neural activity
    
    // Find matching skills for current neural pattern
    for (auto& skill : pImpl->skills) {
        if (isSkillActive(skill, currentNeuralActivity)) {
            // Skill is being executed - update its activation sequence
            skill.executionTime = std::max(0.1f, skill.executionTime * 0.95f);  // Speed up with practice
            skill.reliability = std::min(1.0f, skill.reliability + 0.01f);
            
            // Update neural pattern for efficiency
            skill.neuralPattern = currentNeuralActivity;
        }
    }
}

bool ProceduralMemory::isSkillActive(const Skill& skill, const std::vector<NeuronId>& neuralActivity) const {
    // Check if the skill is currently being executed
    if (neuralActivity.size() < skill.neuralPattern.size()) {
        return false;
    }
    
    // Simple pattern matching - in real implementation would use more sophisticated matching
    for (size_t i = 0; i < skill.neuralPattern.size(); ++i) {
        if (neuralActivity[i] != skill.neuralPattern[i]) {
            return false;
        }
    }
    
    return true;
}

size_t ProceduralMemory::calculateMinNeuralDistance(const std::vector<NeuronId>& pattern) const {
    // Calculate minimum distance between neurons in a pattern
    // Simplified implementation - would use actual neural connectivity in real system
    
    if (pattern.size() < 2) {
        return 0;
    }
    
    size_t minDist = static_cast<size_t>(pattern.size() * 10);  // Large default
    
    for (size_t i = 0; i < pattern.size(); ++i) {
        for (size_t j = i + 1; j < pattern.size(); ++j) {
            size_t dist = calculateNeuralDistance(pattern[i], pattern[j]);
            if (dist < minDist) {
                minDist = dist;
            }
        }
    }
    
    return minDist;
}

size_t ProceduralMemory::calculateNeuralDistance(NeuronId id1, NeuronId id2) const {
    // Calculate distance between two neurons
    // Simplified placeholder - would use actual neural connectivity metrics
    
    // Use hash-based pseudo-random distance for determinism
    std::hash<NeuronId> hasher;
    size_t seed1 = hasher(static_cast<uint64_t>(id1.value));
    size_t seed2 = hasher(static_cast<uint64_t>(id2.value));
    
    std::mt19937 rng(seed1 ^ seed2);
    std::uniform_int_distribution<size_t> dist(1, 100);
    
    return dist(rng);
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    // Enhanced associative memory with distributed representations
    struct Association {
        std::vector<NeuronId> conceptA;
        std::vector<NeuronId> conceptB;
        float strength;              // How strong the association is (0-1)
        float activationThreshold;   // Minimum activation to retrieve
        float decayRate;             // How quickly the association decays
        float interferenceFactor;    // How susceptible to interference
        float learningRate;          // Rate of learning/forgetting
        
        Association(const std::vector<NeuronId>& a, const std::vector<NeuronId>& b) 
            : conceptA(a), conceptB(b), strength(0.5f), activationThreshold(0.5f), 
              decayRate(0.01f), interferenceFactor(0.5f), learningRate(0.05f) {}
    };
    
    std::vector<Association> associations;
    std::vector<NeuronId> activeConceptA;    // Currently active concept A
    std::vector<NeuronId> activeConceptB;    // Currently active concept B
    float globalInterference;          // Overall interference level
    float memoryCapacity;              // Total memory capacity
    float consolidationThreshold;      // Threshold for consolidation
    
    Impl() : globalInterference(0.0f), memoryCapacity(1000.0f), 
             consolidationThreshold(0.7f) {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Create or update an association between two concepts/neurons
    
    // Check if association already exists
    for (auto& assoc : pImpl->associations) {
        bool matchesA = false, matchesB = false;
        
        // Simple match check (would use more sophisticated matching in real implementation)
        if (assoc.conceptA.size() == 1 && assoc.conceptA[0] == a) {
            matchesA = true;
        }
        if (assoc.conceptB.size() == 1 && assoc.conceptB[0] == b) {
            matchesB = true;
        }
        
        if (matchesA && matchesB) {
            // Update existing association
            assoc.strength = std::min(1.0f, assoc.strength + strength);
            return;
        }
    }
    
    // Create new association
    std::vector<NeuronId> conceptA = {a};
    std::vector<NeuronId> conceptB = {b};
    
    Association newAssoc(conceptA, conceptB);
    newAssoc.strength = std::min(1.0f, strength);
    
    pImpl->associations.push_back(std::move(newAssoc));
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    for (const auto& assoc : pImpl->associations) {
        if (contains(assoc.conceptA, neuron)) {
            result.insert(result.end(), assoc.conceptB.begin(), assoc.conceptB.end());
        } else if (contains(assoc.conceptB, neuron)) {
            result.insert(result.end(), assoc.conceptA.begin(), assoc.conceptA.end());
        }
    }
    
    // Remove duplicates
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->associations) {
        if ((contains(assoc.conceptA, a) && contains(assoc.conceptB, b)) ||
            (contains(assoc.conceptA, b) && contains(assoc.conceptB, a))) {
            return assoc.strength;
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((contains(assoc.conceptA, a) && contains(assoc.conceptB, b)) ||
            (contains(assoc.conceptA, b) && contains(assoc.conceptB, a))) {
            
            // Update strength with bounds and decay
            assoc.strength = std::clamp(assoc.strength + delta, 0.0f, 1.0f);
            
            // Apply decay based on time since last use
            assoc.decayRate = std::min(0.1f, assoc.decayRate + 0.001f);
            
            // Interference from other associations
            for (auto& other : pImpl->associations) {
                if (&other != &assoc) {
                    float interference = calculateInterference(a, b, other);
                    if (interference > 0.0f) {
                        assoc.strength *= (1.0f - interference * assoc.interferenceFactor);
                    }
                }
            }
            
            break;
        }
    }
}

void AssociativeMemory::decayAssociations() {
    // Decay all associations over time
    float currentTime = 0.0f;  // Would track simulation time
    
    for (auto& assoc : pImpl->associations) {
        // Temporal decay
        assoc.strength *= (1.0f - assoc.decayRate);
        
        // Remove very weak associations
        if (assoc.strength < 0.01f) {
            // Mark for removal
        }
    }
    
    // Remove weak associations
    pImpl->associations.erase(
        std::remove_if(pImpl->associations.begin(), pImpl->associations.end(),
            [currentTime](const Association& assoc) {
                return assoc.strength < 0.01f;
            }),
        pImpl->associations.end()
    );
}

void AssociativeMemory::updateMemoryLoad(const std::vector<NeuronId>& activeA, const std::vector<NeuronId>& activeB) {
    // Update memory load based on current activity
    pImpl->activeConceptA = activeA;
    pImpl->activeConceptB = activeB;
    
    // Calculate global interference level
    pImpl->globalInterference = 0.0f;
    
    for (const auto& assoc : pImpl->associations) {
        float interference = calculateInterferenceForActiveConcepts(assoc, activeA, activeB);
        pImpl->globalInterference += interference;
    }
}

bool AssociativeMemory::contains(const std::vector<NeuronId>& concept, NeuronId neuron) const {
    return std::find(concept.begin(), concept.end(), neuron) != concept.end();
}

float AssociativeMemory::calculateInterference(NeuronId a, NeuronId b, const Association& other) const {
    // Calculate interference between associations
    bool aInOther = contains(other.conceptA, a) || contains(other.conceptB, a);
    bool bInOther = contains(other.conceptB, b) || contains(other.conceptA, b);
    
    if (aInOther && bInOther) {
        // Both neurons involved in other association - high interference
        return 0.8f;
    } else if (aInOther || bInOther) {
        // One neuron shared - moderate interference
        return 0.4f;
    }
    
    return 0.0f;
}

float AssociativeMemory::calculateInterferenceForActiveConcepts(const Association& assoc, 
    const std::vector<NeuronId>& activeA, const std::vector<NeuronId>& activeB) const {
    // Calculate interference based on currently active concepts
    bool conceptAMatches = false, conceptBMatches = false;
    
    for (const auto& neuron : assoc.conceptA) {
        if (std::find(activeA.begin(), activeA.end(), neuron) != activeA.end()) {
            conceptAMatches = true;
        }
    }
    
    for (const auto& neuron : assoc.conceptB) {
        if (std::find(activeB.begin(), activeB.end(), neuron) != activeB.end()) {
            conceptBMatches = true;
        }
    }
    
    if (conceptAMatches && conceptBMatches) {
        return 0.7f;
    } else if (conceptAMatches || conceptBMatches) {
        return 0.3f;
    }
    
    return 0.0f;
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->activeConceptA.clear();
    pImpl->activeConceptB.clear();
}

} // namespace nlm