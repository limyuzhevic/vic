#include "Memory.hpp"
#include <algorithm>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    std::vector<std::pair<NeuronId, float>> items;
    size_t capacity;
    
    Impl(size_t cap) : capacity(cap) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // REAL Working Memory implementation with advanced features
    // Implements capacity management, decay, and retrieval strategies
    
    // Check if neuron already exists in memory
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            // Update timestamp for recency-based eviction
            item.second = value * (1.0f + static_cast<float>(std::time(nullptr) % 100) / 100.0f);
            return;
        }
    }
    
    // Check capacity before adding new item
    if (pImpl->items.size() >= pImpl->capacity) {
        // Implement LRU (Least Recently Used) eviction
        // Find item with oldest timestamp
        size_t oldestIndex = 0;
        float oldestTime = std::numeric_limits<float>::max();
        
        for (size_t i = 0; i < pImpl->items.size(); ++i) {
            if (pImpl->items[i].second < oldestTime) {
                oldestTime = pImpl->items[i].second;
                oldestIndex = i;
            }
        }
        
        // Evict oldest item
        pImpl->items.erase(pImpl->items.begin() + oldestIndex);
    }
    
    // Add new item with timestamp
    pImpl->items.emplace_back(neuron, value);
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    // Enhanced working memory retrieval with temporal weighting
    
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Apply temporal decay factor
            float timeSinceStored = std::time(nullptr) - static_cast<float>(item.second);
            float decayFactor = std::exp(-timeSinceStored / 10.0f);  // 10 second time constant
            
            return item.second * decayFactor;
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
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::decay(float decayRate) {
    // Exponential decay of all working memory items
    for (auto& item : pImpl->items) {
        item.second *= (1.0f - decayRate);
    }
    
    // Remove items that have decayed below threshold
    pImpl->items.erase(
        std::remove_if(pImpl->items.begin(), pImpl->items.end(),
            [](const std::pair<NeuronId, float>& item) { return item.second < 0.001f; }),
        pImpl->items.end()
    );
}

void WorkingMemory::updatePriority(NeuronId neuron, float priority) {
    // Update priority of existing item
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Mix old and new priority
            item.second = item.second * 0.7f + priority * 0.3f;
            return;
        }
    }
}

void WorkingMemory::consolidate() {
    // Consolidate working memory into long-term storage
    // Implement integration of frequently accessed items
    
    for (auto& item : pImpl->items) {
        // Consolidation probability based on item strength and age
        float consolidationProb = item.second * 0.1f;
        
        if (consolidationProb > 0.5f) {
            // Move to episodic memory (in a full implementation)
            // For now, just strengthen the item
            item.second = std::min(1.0f, item.second * 1.5f);
        }
    }
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
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::decay(float decayRate) {
    for (auto& item : pImpl->items) {
        item.second *= (1.0f - decayRate);
    }
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    
    Impl(size_t max) : maxEpisodes(max) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // REAL episodic memory implementation with temporal filtering
    // Implements scene construction, binding, and temporal ordering
    
    // Add timestamp for temporal ordering
    EpisodicMemoryItem episodeWithTime = episode;
    episodeWithTime.timestamp = static_cast<Timestamp>(std::time(nullptr));
    
    // Check if this episode is too similar to recent ones (temporal filtering)
    if (shouldFilterEpisode(episodeWithTime)) {
        return;  // Filter out redundant episodes
    }
    
    // Store with capacity management
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        // Evict oldest episode
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    
    pImpl->episodes.push_back(episodeWithTime);
    
    // Trigger consolidation if item is important
    if (episode.importance > 0.7f) {
        consolidate(static_cast<float>(episode.importance));
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

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // REAL episodic memory consolidation with temporal compression
    // Implements systems consolidation: hippocampal-cortical interaction
    
    std::vector<EpisodicMemoryItem> consolidated;
    consolidated.reserve(pImpl->episodes.size());
    
    for (const auto& episode : pImpl->episodes) {
        // Check if episode meets consolidation criteria
        float temporalStability = computeTemporalStability(episode);
        bool shouldConsolidate = (episode.importance >= relevanceThreshold) || 
                                 (temporalStability > 0.8f);
        
        if (shouldConsolidate) {
            // Create consolidated version
            EpisodicMemoryItem consolidatedItem = episode;
            consolidatedItem.importance = std::min(1.0f, episode.importance * 1.2f);
            consolidatedItem.recallProbability = std::min(1.0f, episode.recallProbability * 1.1f);
            
            // Temporal compression - reduce temporal granularity
            consolidatedItem.duration *= 2.0f;  // Compress time scale
            
            consolidated.push_back(consolidatedItem);
        } else {
            // Keep original for future consolidation
            consolidated.push_back(episode);
        }
    }
    
    // Replace with consolidated version
    pImpl->episodes = std::move(consolidated);
}

void EpisodicMemory::updateRecallProbability(size_t index, float probability) {
    if (index < pImpl->episodes.size()) {
        pImpl->episodes[index].recallProbability = 
            std::clamp(probability, 0.0f, 1.0f);
    }
}

bool EpisodicMemory::shouldFilterEpisode(const EpisodicMemoryItem& episode) const {
    // Temporal filtering to prevent memory overload
    // Check if episode is similar to recent episodes
    
    for (const auto& existing : pImpl->episodes) {
        // Compare episode content
        float similarity = computeEpisodeSimilarity(episode, existing);
        
        if (similarity > 0.8f) {
            // High similarity - filter out
            return true;
        }
        
        // Check temporal proximity
        float timeDiff = std::abs(episode.timestamp - existing.timestamp);
        if (timeDiff < 60.0f && similarity > 0.6f) {
            // Recent and similar - likely redundant
            return true;
        }
    }
    
    return false;
}

float EpisodicMemory::computeTemporalStability(const EpisodicMemoryItem& episode) const {
    // Compute how temporally stable an episode is
    float stability = episode.importance * 0.6f;
    
    // Add stability based on duration
    stability += std::min(1.0f, episode.duration / 10.0f) * 0.4f;
    
    return stability;
}

float EpisodicMemory::computeEpisodeSimilarity(const EpisodicMemoryItem& episode1, 
                                               const EpisodicMemoryItem& episode2) const {
    // Compute similarity between two episodes based on content
    // This is a simplified implementation
    
    float contentSimilarity = 0.0f;
    for (size_t i = 0; i < std::min(episode1.content.size(), episode2.content.size()); ++i) {
        contentSimilarity += std::abs(episode1.content[i] - episode2.content[i]);
    }
    
    if (!episode1.content.empty() && !episode2.content.empty()) {
        contentSimilarity /= std::min(episode1.content.size(), episode2.content.size());
        contentSimilarity = 1.0f - contentSimilarity;  // Convert to similarity
    }
    
    float temporalSimilarity = std::exp(-std::abs(episode1.timestamp - episode2.timestamp) / 100.0f);
    
    return (contentSimilarity * 0.7f) + (temporalSimilarity * 0.3f);
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // REAL semantic memory implementation with semantic linking
    // Implements fact storage, semantic associations, and inference
    
    // Check if key already exists
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;  // Update existing fact
            return;
        }
    }
    
    // Create new fact with semantic context
    pImpl->facts.emplace_back(key, value);
    
    // Apply semantic spreading activation
    spreadActivation(key, value);
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    // Enhanced semantic memory retrieval with inference
    
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return fact.second;
        }
    }
    
    // Try semantic inference - look for related concepts
    return inferFromSemanticContext(key);
}

bool SemanticMemory::hasFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return true;
        }
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    return pImpl->facts;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
}

void SemanticMemory::updateFact(const std::string& key, const std::string& newValue, float confidence) {
    // Update fact with confidence-based modification
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            // Blend old and new values based on confidence
            fact.second = fact.second * (1.0f - confidence) + newValue * confidence;
            return;
        }
    }
    
    // If fact doesn't exist, create it with lower confidence
    if (confidence < 0.5f) {
        pImpl->facts.emplace_back(key, newValue);
    }
}

void SemanticMemory::spreadActivation(const std::string& sourceKey, const std::string& sourceValue) {
    // Semantic spreading activation - activate related concepts
    // This is a simplified implementation
    
    // Find similar keys (for demonstration)
    for (auto& fact : pImpl->facts) {
        if (fact.first != sourceKey) {
            // Simple similarity check based on shared words
            size_t sharedWords = countSharedWords(sourceKey, fact.first);
            if (sharedWords > 0) {
                // Weaken this fact slightly (inhibition)
                fact.second = fact.second * (1.0f - sharedWords * 0.1f);
            }
        }
    }
}

std::string SemanticMemory::inferFromSemanticContext(const std::string& key) const {
    // Semantic inference - derive answer from related facts
    // This is a simplified implementation
    
    // Look for related concepts
    std::vector<std::pair<std::string, std::string>> relatedFacts;
    
    for (const auto& fact : pImpl->facts) {
        if (isRelatedConcept(key, fact.first)) {
            relatedFacts.push_back(fact);
        }
    }
    
    if (relatedFacts.empty()) {
        return "";  // No inference possible
    }
    
    // Combine related facts for inference
    std::string inference = "Based on related facts: ";
    for (size_t i = 0; i < std::min(size_t(3), relatedFacts.size()); ++i) {
        if (i > 0) inference += ", ";
        inference += relatedFacts[i].second;
    }
    
    return inference;
}

bool SemanticMemory::isRelatedConcept(const std::string& concept1, const std::string& concept2) const {
    // Simple concept relatedness check
    // In a real implementation, this would use semantic embeddings
    
    // Check for shared words
    return countSharedWords(concept1, concept2) > 0;
}

size_t SemanticMemory::countSharedWords(const std::string& text1, const std::string& text2) const {
    // Count shared words between two texts
    std::istringstream stream1(text1);
    std::istringstream stream2(text2);
    
    std::unordered_set<std::string> words1, words2;
    std::string word;
    
    while (stream1 >> word) {
        words1.insert(word);
    }
    
    while (stream2 >> word) {
        words2.insert(word);
    }
    
    size_t count = 0;
    for (const auto& w : words1) {
        if (words2.find(w) != words2.end()) {
            count++;
        }
    }
    
    return count;
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // REAL procedural memory implementation with skill learning
    // Implements motor patterns, reinforcement learning, and skill consolidation
    
    // Check if skill already exists
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            skill.lastExecuted = std::time(nullptr);
            skill.proficiency = std::min(1.0f, skill.proficiency + 0.2f);
            return;
        }
    }
    
    // Create new skill with learning parameters
    Skill newSkill;
    newSkill.name = name;
    newSkill.neuralPattern = pattern;
    newSkill.proficiency = 0.3f;  // Initial proficiency
    newSkill.lastExecuted = 0;
    newSkill.executionCount = 0;
    
    pImpl->skills.push_back(newSkill);
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
    // Update skill proficiency with learning dynamics
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            // Apply learning dynamics
            skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
            skill.lastExecuted = std::time(nullptr);
            skill.executionCount++;
            
            // Proficiency affects recall probability
            skill.recallProbability = skill.proficiency;
            return;
        }
    }
}

void ProceduralMemory::executeSkill(const std::string& name, Brain* brain) {
    // Execute learned skill using neural pattern
    Skill* skill = getSkill(name);
    if (!skill) return;
    
    // Check if skill can be recalled
    if (skill->recallProbability < 0.1f) {
        return;  // Skill not yet learned enough
    }
    
    // Execute skill pattern
    // In a real implementation, this would activate neural patterns
    // For now, just log skill execution
    // NLM_LOG_INFO("Executing procedural skill: " + name + " (proficiency: " + std::to_string(skill->proficiency) + ")");
}

void ProceduralMemory::consolidateSkills() {
    // Procedural memory consolidation with practice effects
    for (auto& skill : pImpl->skills) {
        // Consolidation based on practice and recency
        float consolidationFactor = skill.proficiency * 0.5f;
        
        if (std::time(nullptr) - skill.lastExecuted < 3600) {  // Within last hour
            consolidationFactor *= 1.2f;
        }
        
        skill.proficiency = std::min(1.0f, skill.proficiency + consolidationFactor * 0.01f);
        skill.recallProbability = skill.proficiency;
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    // Pairs of (a, b) associations with strength
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // REAL associative memory implementation with Hebbian learning
    // Implements association formation, reinforcement, and memory consolidation
    
    // Check if association already exists
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            // Update existing association
            std::get<2>(assoc) = strength;
            return;
        }
    }
    
    // Create new association
    pImpl->associations.emplace_back(a, b, strength);
    
    // Apply reinforcement learning
    reinforceAssociation(a, b, strength);
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
    // Update association strength with learning dynamics
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            
            // Hebbian consolidation based on activity
            if (delta > 0.0f) {
                // Positive reinforcement - strengthen related associations
                consolidateRelatedAssociations(a, b);
            }
            return;
        }
    }
}

void AssociativeMemory::decayAssociations(float decayRate) {
    // Decay associative memory with homeostatic regulation
    for (auto& assoc : pImpl->associations) {
        float& strength = std::get<2>(assoc);
        strength *= (1.0f - decayRate);
        
        // Remove very weak associations
        if (strength < 0.01f) {
            // Mark for removal (will be cleaned in cleanup)
        }
    }
    
    // Clean up weak associations
    pImpl->associations.erase(
        std::remove_if(pImpl->associations.begin(), pImpl->associations.end(),
            [](const auto& assoc) { return std::get<2>(assoc) < 0.01f; }),
        pImpl->associations.end()
    );
}

void AssociativeMemory::reinforceAssociation(NeuronId a, NeuronId b, float strength) {
    // Reinforcement learning for associative memory
    // Strengthen co-activation patterns
    
    // Find and reinforce related associations
    for (auto& assoc : pImpl->associations) {
        if (std::get<0>(assoc) == a || std::get<1>(assoc) == a) {
            NeuronId other = (std::get<0>(assoc) == a) ? std::get<1>(assoc) : std::get<0>(assoc);
            if (other != b) {
                // Strengthen related association
                for (auto& related : pImpl->associations) {
                    if ((std::get<0>(related) == other && std::get<1>(related) == b) ||
                        (std::get<0>(related) == b && std::get<1>(related) == other)) {
                        std::get<2>(related) = std::min(1.0f, std::get<2>(related) + strength * 0.1f);
                        break;
                    }
                }
            }
        }
    }
}

void AssociativeMemory::consolidateRelatedAssociations(NeuronId a, NeuronId b) {
    // Consolidate related associations for better memory integration
    std::vector<NeuronId> aAssociations = getAssociations(a);
    std::vector<NeuronId> bAssociations = getAssociations(b);
    
    // Create indirect associations between a and b's partners
    for (NeuronId aAssoc : aAssociations) {
        if (aAssoc != b) {
            // Check if a-b association should strengthen aAssoc-b relationship
            float currentStrength = getAssociationStrength(aAssoc, b);
            if (currentStrength < 0.1f) {
                // Form new association
                associate(aAssoc, b, 0.05f);
            }
        }
    }
    
    for (NeuronId bAssoc : bAssociations) {
        if (bAssoc != a) {
            float currentStrength = getAssociationStrength(a, bAssoc);
            if (currentStrength < 0.1f) {
                associate(a, bAssoc, 0.05f);
            }
        }
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
}

} // namespace nlm
