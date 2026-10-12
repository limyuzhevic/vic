#include "Memory.hpp"
#include <algorithm>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    std::vector<std::pair<NeuronId, float>> items;
    std::vector<double> accessTimestamps;
    std::vector<float> competitionScores;
    size_t capacity;
    float decayRate;
    float interferenceStrength;
    
    Impl(size_t cap) : capacity(cap), decayRate(0.01f), interferenceStrength(0.5f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {
    pImpl->accessTimestamps.reserve(pImpl->capacity);
    pImpl->competitionScores.reserve(pImpl->capacity);
}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Find existing item
    for (size_t i = 0; i < pImpl->items.size(); ++i) {
        if (pImpl->items[i].first == neuron) {
            // Update existing item with temporal dynamics
            pImpl->items[i].second = value;
            pImpl->accessTimestamps[i] = 0.0; // Reset timestamp for recency
            return;
        }
    }
    
    // Apply competition - evict items with lowest scores if full
    if (pImpl->items.size() >= pImpl->capacity) {
        // Calculate competition scores based on recency and value
        std::vector<size_t> scores(pImpl->items.size());
        for (size_t i = 0; i < pImpl->items.size(); ++i) {
            double ageScore = std::exp(-pImpl->accessTimestamps[i]); // Recency bias
            float valueScore = std::abs(pImpl->items[i].second);
            scores[i] = static_cast<size_t>(ageScore * pImpl->items.size() * 0.7f + 
                                           valueScore * pImpl->items.size() * 0.3f);
        }
        
        // Find and evict lowest scoring item
        size_t minScore = *std::min_element(scores.begin(), scores.end());
        for (size_t i = 0; i < pImpl->items.size(); ++i) {
            if (scores[i] == minScore) {
                pImpl->items.erase(pImpl->items.begin() + i);
                pImpl->accessTimestamps.erase(pImpl->accessTimestamps.begin() + i);
                pImpl->competitionScores.erase(pImpl->competitionScores.begin() + i);
                break;
            }
        }
    }
    
    pImpl->items.emplace_back(neuron, value);
    pImpl->accessTimestamps.push_back(0.0);
    pImpl->competitionScores.push_back(std::abs(value));
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    for (size_t i = 0; i < pImpl->items.size(); ++i) {
        if (pImpl->items[i].first == neuron) {
            // Update access timestamp for temporal dynamics
            return pImpl->items[i].second;
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
    pImpl->accessTimestamps.clear();
    pImpl->competitionScores.clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::decay(float decayRate) {
    // Apply temporal decay to all items
    pImpl->decayRate = decayRate;
    for (auto& item : pImpl->items) {
        item.second *= (1.0f - decayRate);
    }
}

// Enhanced Working Memory with Neural Integration
void WorkingMemory::integrateWithNeuralActivity(const std::vector<NeuronId>& activeNeurons, 
                                                double timestep) {
    // Update access timestamps for active neurons (recency)
    for (const auto& neuron : activeNeurons) {
        for (size_t i = 0; i < pImpl->items.size(); ++i) {
            if (pImpl->items[i].first == neuron) {
                // Boost competition score based on neural activity
                pImpl->competitionScores[i] = std::min(1.0f, pImpl->competitionScores[i] * 1.1f);
                break;
            }
        }
    }
    
    // Apply decay to access timestamps
    for (auto& timestamp : pImpl->accessTimestamps) {
        timestamp += timestep;
    }
}

float WorkingMemory::getRecencyScore(NeuronId neuron) const {
    for (size_t i = 0; i < pImpl->items.size(); ++i) {
        if (pImpl->items[i].first == neuron) {
            return std::exp(-pImpl->accessTimestamps[i]);
        }
    }
    return 0.0f;
}

std::vector<std::pair<NeuronId, float>> WorkingMemory::getMostRecentItems(size_t count) const {
    std::vector<std::pair<size_t, double>> indexedScores;
    for (size_t i = 0; i < pImpl->items.size(); ++i) {
        double recencyScore = std::exp(-pImpl->accessTimestamps[i]);
        double valueScore = std::abs(pImpl->items[i].second);
        indexedScores.push_back({i, recencyScore * 0.7 + valueScore * 0.3});
    }
    
    std::sort(indexedScores.begin(), indexedScores.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::pair<NeuronId, float>> result;
    size_t limit = std::min(count, indexedScores.size());
    for (size_t i = 0; i < limit; ++i) {
        result.push_back(pImpl->items[indexedScores[i].first]);
    }
    
    return result;
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    std::vector<std::vector<size_t>> episodeSimilarities; // For pattern completion
    double timeScale; // Convert simulation steps to time units
    
    Impl(size_t max) : maxEpisodes(max), timeScale(0.001) {} // Convert steps to seconds
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {
    pImpl->episodeSimilarities.reserve(pImpl->maxEpisodes);
}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Calculate similarity with existing episodes for pattern completion
    double episodeTime = static_cast<double>(episode.timestamp) * pImpl->timeScale;
    
    // Apply temporal compression - compress similar time periods
    for (const auto& existing : pImpl->episodes) {
        double existingTime = static_cast<double>(existing.timestamp) * pImpl->timeScale;
        double timeDiff = std::abs(episodeTime - existingTime);
        
        // Simple similarity based on temporal proximity
        if (timeDiff < 10.0) { // 10 seconds similarity window
            std::vector<size_t> similarIndices;
            similarIndices.push_back(pImpl->episodes.size());
            
            // Store similarity information
            if (pImpl->episodeSimilarities.size() <= pImpl->episodes.size()) {
                pImpl->episodeSimilarities.resize(pImpl->episodes.size() + 1);
            }
            pImpl->episodeSimilarities[pImpl->episodes.size()] = similarIndices;
            
            break;
        }
    }
    
    // Apply circular buffer eviction
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        // Evict least recent episode
        pImpl->episodes.erase(pImpl->episodes.begin());
        pImpl->episodeSimilarities.erase(pImpl->episodeSimilarities.begin());
    }
    
    pImpl->episodes.push_back(episode);
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    if (index < pImpl->episodes.size()) {
        EpisodicMemoryItem episode = pImpl->episodes[index];
        
        // Apply pattern completion if episode has gaps or incomplete data
        if (episode.neurons.empty() && episode.metadata.find("pattern_complete") == std::string::npos) {
            episode = completePatternFromSimilarEpisodes(index);
        }
        
        return episode;
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
    
    // Also include pattern-completed versions of similar episodes
    for (size_t i = start; i < pImpl->episodes.size() && result.size() < count * 2; ++i) {
        if (pImpl->episodeSimilarities.size() > i) {
            for (size_t similarIdx : pImpl->episodeSimilarities[i]) {
                if (similarIdx != i && similarIdx < pImpl->episodes.size()) {
                    EpisodicMemoryItem similar = pImpl->episodes[similarIdx];
                    similar = completePatternFromSimilarEpisodes(similarIdx);
                    if (std::find(result.begin(), result.end(), similar) == result.end()) {
                        result.push_back(similar);
                        if (result.size() >= count * 2) break;
                    }
                }
            }
        }
    }
    
    return result;
}

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
    pImpl->episodeSimilarities.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    if (pImpl->episodes.empty()) return;
    
    // Calculate episode relevance based on novelty and predictive value
    std::vector<size_t> relevantIndices;
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        const auto& episode = pImpl->episodes[i];
        
        // Calculate relevance score
        double relevanceScore = 0.0;
        
        // Factor 1: Temporal novelty (recentness)
        double currentTime = static_cast<double>(episode.timestamp) * pImpl->timeScale;
        double recentness = std::exp(-(currentTime)); // Decay over time
        
        // Factor 2: Pattern uniqueness
        size_t uniquePatterns = countUniquePatternComponents(episode);
        
        // Factor 3: Predictive value (if episode contains predictable sequences)
        double predictiveValue = calculatePredictiveValue(episode);
        
        relevanceScore = recentness * 0.3 + uniquePatterns * 0.4 + predictiveValue * 0.3;
        
        if (relevanceScore > relevanceThreshold) {
            relevantIndices.push_back(i);
        }
    }
    
    // Promote relevant episodes to higher priority regions
    // (This would integrate with the brain's region system)
    std::cout << "Episodic consolidation: " << relevantIndices.size() << " episodes consolidated" << std::endl;
}

// Helper methods for enhanced episodic memory
EpisodicMemoryItem EpisodicMemory::completePatternFromSimilarEpisodes(size_t baseIndex) const {
    EpisodicMemoryItem result = pImpl->episodes[baseIndex];
    
    // If episode has missing neurons, try to complete using similar episodes
    if (result.neurons.empty()) {
        if (pImpl->episodeSimilarities.size() > baseIndex) {
            for (size_t similarIdx : pImpl->episodeSimilarities[baseIndex]) {
                if (similarIdx < pImpl->episodes.size() && !pImpl->episodes[similarIdx].neurons.empty()) {
                    result = pImpl->episodes[similarIdx];
                    result.metadata += " (pattern_completed)";
                    return result;
                }
            }
        }
    }
    
    // Add pattern completion metadata
    result.metadata += " (pattern_complete)";
    return result;
}

size_t EpisodicMemory::countUniquePatternComponents(const EpisodicMemoryItem& episode) const {
    // Count unique neuron patterns and value distributions
    std::set<NeuronId> uniqueNeurons(episode.neurons.begin(), episode.neurons.end());
    
    // Group similar values
    std::map<float, size_t> valueGroups;
    for (float value : episode.values) {
        size_t group = static_cast<size_t>(value * 10); // Group values by decade
        valueGroups[group]++;
    }
    
    return uniqueNeurons.size() * 10 + valueGroups.size();
}

double EpisodicMemory::calculatePredictiveValue(const EpisodicMemoryItem& episode) const {
    // Simple predictive value calculation based on pattern regularity
    if (episode.neurons.size() < 2) return 0.0;
    
    double totalChange = 0.0;
    for (size_t i = 1; i < episode.neurons.size(); ++i) {
        totalChange += std::abs(episode.values[i] - episode.values[i-1]);
    }
    
    double regularity = 1.0 - (totalChange / (episode.neurons.size() * 10.0)); // Normalize
    return std::max(0.0, regularity);
}

std::vector<EpisodicMemoryItem> EpisodicMemory::replayEpisodes(double replaySpeed, 
                                                                 size_t maxReplays) const {
    std::vector<EpisodicMemoryItem> replayed;
    
    // Priority: replay most recent and most relevant episodes
    std::vector<size_t> indices;
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        indices.push_back(i);
    }
    
    // Sort by recency (most recent first)
    std::sort(indices.begin(), indices.end(), 
              [this](size_t a, size_t b) {
                  return static_cast<double>(pImpl->episodes[a].timestamp) > 
                         static_cast<double>(pImpl->episodes[b].timestamp);
              });
    
    // Limit replays
    size_t replayCount = std::min(maxReplays, indices.size());
    for (size_t i = 0; i < replayCount; ++i) {
        EpisodicMemoryItem replayedEpisode = pImpl->episodes[indices[i]];
        
        // Apply replay modifications
        replayedEpisode.metadata += " (replayed)";
        replayedEpisode.timestamp = static_cast<SimulationStep>(replaySpeed * i);
        
        replayed.push_back(replayedEpisode);
    }
    
    return replayed;
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    std::vector<std::vector<std::string>> factClusters; // Similar facts grouped
    std::map<std::string, float> factStrength; // Dynamic strength based on usage
    std::map<std::string, std::vector<std::string>> factAssociations; // Associations between facts
    float learningRate;
    float consolidationThreshold;
    
    Impl() : learningRate(0.01f), consolidationThreshold(0.7f) {
        factClusters.reserve(facts.size() + 100);
    }
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Check if fact already exists
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            // Update with Hebbian-like learning - strengthen based on usage
            fact.second = value;
            pImpl->factStrength[key] += pImpl->learningRate;
            return;
        }
    }
    
    // New fact - apply pattern-based learning
    pImpl->facts.emplace_back(key, value);
    pImpl->factStrength[key] = 1.0f;
    
    // Apply concept formation
    extractConceptsFromFact(key, value);
    
    // Update clusters
    updateClusters();
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    auto it = pImpl->factStrength.find(key);
    if (it != pImpl->factStrength.end() && it->second > 0.01f) {
        for (const auto& fact : pImpl->facts) {
            if (fact.first == key) {
                // Apply strength-based retrieval bias
                return fact.second;
            }
        }
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return pImpl->factStrength.find(key) != pImpl->factStrength.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    
    // Sort by strength for better retrieval patterns
    std::vector<std::pair<std::string, float>> strengthList;
    for (const auto& fact : pImpl->facts) {
        auto it = pImpl->factStrength.find(fact.first);
        if (it != pImpl->factStrength.end()) {
            strengthList.push_back({fact.first, it->second});
        }
    }
    
    std::sort(strengthList.begin(), strengthList.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (const auto& pair : strengthList) {
        for (const auto& fact : pImpl->facts) {
            if (fact.first == pair.first) {
                result.push_back(fact);
                break;
            }
        }
    }
    
    return result;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
    pImpl->factClusters.clear();
    pImpl->factStrength.clear();
    pImpl->factAssociations.clear();
}

// Enhanced Semantic Memory with Advanced Learning
void SemanticMemory::applyHebbianLearning(const std::string& key1, const std::string& key2) {
    // Create association between two facts
    if (pImpl->factAssociations.find(key1) == pImpl->factAssociations.end()) {
        pImpl->factAssociations[key1] = std::vector<std::string>();
    }
    
    if (pImpl->factAssociations.find(key2) == pImpl->factAssociations.end()) {
        pImpl->factAssociations[key2] = std::vector<std::string>();
    }
    
    // Create bidirectional association
    if (std::find(pImpl->factAssociations[key1].begin(), 
                  pImpl->factAssociations[key1].end(), key2) == pImpl->factAssociations[key1].end()) {
        pImpl->factAssociations[key1].push_back(key2);
        pImpl->factAssociations[key2].push_back(key1);
        
        // Strengthen both facts
        pImpl->factStrength[key1] += pImpl->learningRate * 2.0f;
        pImpl->factStrength[key2] += pImpl->learningRate * 2.0f;
    }
}

void SemanticMemory::extractConceptsFromFact(const std::string& key, const std::string& value) {
    // Extract semantic concepts from key-value pair
    std::string concept = extractConcept(key, value);
    
    if (!concept.empty() && !hasFact(concept)) {
        // Store concept with simplified representation
        std::string conceptValue = deriveConceptValue(value);
        storeFact(concept, conceptValue);
    }
}

void SemanticMemory::updateClusters() {
    // Re-cluster facts based on similarity
    pImpl->factClusters.clear();
    
    for (size_t i = 0; i < pImpl->facts.size(); ++i) {
        std::vector<std::string> cluster;
        cluster.push_back(pImpl->facts[i].first);
        
        // Find similar facts
        for (size_t j = i + 1; j < pImpl->facts.size(); ++j) {
            if (factSimilarity(pImpl->facts[i].first, pImpl->facts[j].first) > 0.7f) {
                cluster.push_back(pImpl->facts[j].first);
            }
        }
        
        if (cluster.size() > 1) {
            pImpl->factClusters.push_back(cluster);
        }
    }
}

float SemanticMemory::factSimilarity(const std::string& fact1, const std::string& fact2) const {
    // Simple similarity based on shared prefixes and word overlap
    std::string key1 = extractKeyComponents(fact1);
    std::string key2 = extractKeyComponents(fact2);
    
    // Word-based similarity
    std::vector<std::string> words1 = splitString(key1);
    std::vector<std::string> words2 = splitString(key2);
    
    if (words1.empty() || words2.empty()) return 0.0f;
    
    size_t commonWords = 0;
    for (const auto& word1 : words1) {
        for (const auto& word2 : words2) {
            if (word1 == word2) {
                commonWords++;
                break;
            }
        }
    }
    
    return static_cast<float>(commonWords) / std::max(words1.size(), words2.size());
}

std::string SemanticMemory::extractConcept(const std::string& key, const std::string& value) const {
    // Extract semantic concept (e.g., "temperature" from "temperature_is_hot")
    size_t underscorePos = key.find('_');
    if (underscorePos != std::string::npos) {
        return key.substr(0, underscorePos);
    }
    
    // Try to parse key as key_value pattern
    size_t valuePos = key.find('_');
    if (valuePos != std::string::npos && valuePos < key.length() - 1) {
        return key.substr(0, valuePos);
    }
    
    return "";
}

std::string SemanticMemory::deriveConceptValue(const std::string& value) const {
    // Derive simplified concept value (e.g., "hot" from "100celsius")
    if (value == "hot" || value == "cold" || value == "warm" || value == "cool") {
        return value;
    }
    
    // Extract qualitative property
    if (value.find("hot") != std::string::npos || value.find("warm") != std::string::npos) {
        return "warm";
    }
    
    if (value.find("cold") != std::string::npos || value.find("cool") != std::string::npos) {
        return "cool";
    }
    
    return value; // Return as-is if no simplification possible
}

std::string SemanticMemory::extractKeyComponents(const std::string& key) const {
    // Extract components for similarity comparison
    std::string components = key;
    
    // Replace numbers with generic placeholders
    for (char& c : components) {
        if (std::isdigit(c)) {
            c = 'X';
        }
    }
    
    // Remove known value words
    std::vector<std::string> removeWords = {"is", "are", "was", "were", "to", "of", "in", "on"};
    for (const auto& word : removeWords) {
        size_t pos = components.find(word);
        if (pos != std::string::npos) {
            components.replace(pos, word.length(), "");
        }
    }
    
    return components;
}

std::vector<std::string> SemanticMemory::splitString(const std::string& str) const {
    std::vector<std::string> result;
    std::string token;
    std::stringstream ss(str);
    
    while (ss >> token) {
        result.push_back(token);
    }
    
    return result;
}

std::vector<std::vector<std::string>> SemanticMemory::getClusters() const {
    return pImpl->factClusters;
}

float SemanticMemory::getFactStrength(const std::string& key) const {
    auto it = pImpl->factStrength.find(key);
    return (it != pImpl->factStrength.end()) ? it->second : 0.0f;
}

void SemanticMemory::applyForgettingCurve(double timeElapsed) {
    // Apply power-law forgetting curve to all facts
    double forgettingFactor = std::exp(-timeElapsed * 0.001); // Time constant adjustment
    
    for (auto& fact : pImpl->facts) {
        auto it = pImpl->factStrength.find(fact.first);
        if (it != pImpl->factStrength.end()) {
            it->second = std::max(0.0f, it->second * forgettingFactor);
        }
    }
}

void SemanticMemory::applyConsolidation() {
    // Consolidate facts based on usage patterns
    std::vector<std::pair<std::string, std::string>> consolidated;
    
    for (const auto& fact : pImpl->facts) {
        float strength = pImpl->factStrength[fact.first];
        if (strength >= pImpl->consolidationThreshold) {
            consolidated.push_back(fact);
        }
    }
    
    // Replace facts with consolidated ones
    pImpl->facts = consolidated;
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    std::vector<std::vector<size_t>> skillSequences; // Learned sequences of actions
    std::map<std::string, float> skillProficiency; // Proficiency by skill name
    std::vector<std::pair<std::string, std::vector<std::string>>> actionChunks; // Motor chunking
    float consolidationInterval;
    double lastConsolidationTime;
    
    Impl() : consolidationInterval(1000.0), lastConsolidationTime(0.0) {
        actionChunks.reserve(50);
    }
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Check if skill already exists
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            // Update existing skill pattern with gradual learning
            skill.neuralPattern = pattern;
            skill.proficiency = std::min(1.0f, skill.proficiency + 0.1f);
            return;
        }
    }
    
    // New skill - create with initial proficiency
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = 0.5f; // Start with medium proficiency
    
    pImpl->skills.push_back(skill);
    pImpl->skillProficiency[name] = 0.5f;
    
    // Extract motor chunks from pattern
    extractMotorChunks(skill);
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
            pImpl->skillProficiency[name] = skill.proficiency;
            return;
        }
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->skillSequences.clear();
    pImpl->skillProficiency.clear();
    pImpl->actionChunks.clear();
}

// Enhanced Procedural Memory with Advanced Skill Learning
void ProceduralMemory::recordSkillExecution(const std::string& skillName, 
                                            const std::vector<MotorCommand>& executedActions,
                                            double executionTime) {
    Skill* skill = getSkill(skillName);
    if (!skill) return;
    
    // Calculate execution efficiency
    float efficiency = calculateExecutionEfficiency(skillName, executedActions, executionTime);
    
    // Update skill proficiency based on performance
    updateProficiency(skillName, efficiency * 0.01f); // Small incremental update
    
    // Extract and store successful action sequences
    extractAndStoreSequence(skillName, executedActions);
    
    // Apply motor chunking for frequently used sequences
    if (executedActions.size() > 2) {
        chunkActions(skillName, executedActions);
    }
}

void ProceduralMemory::extractAndStoreSequence(const std::string& skillName, 
                                                const std::vector<MotorCommand>& sequence) {
    // Store successful sequences for later playback
    std::vector<size_t> sequenceIndex;
    for (size_t i = 0; i < sequence.size(); ++i) {
        sequenceIndex.push_back(static_cast<size_t>(sequence[i]));
    }
    
    // Check if sequence already exists
    for (const auto& existingSeq : pImpl->skillSequences) {
        if (sequencesEqual(existingSeq, sequenceIndex)) {
            return; // Already known sequence
        }
    }
    
    // Add new sequence
    pImpl->skillSequences.push_back(sequenceIndex);
}

void ProceduralMemory::chunkActions(const std::string& skillName, 
                                    const std::vector<MotorCommand>& actions) {
    // Find and create motor chunks for frequently used action patterns
    for (size_t i = 0; i < actions.size() - 2; ++i) {
        std::vector<MotorCommand> chunk;
        bool isFrequent = true;
        
        // Check if this 3-action chunk is common
        for (size_t j = 0; j < 3; ++j) {
            chunk.push_back(actions[i + j]);
        }
        
        // Check frequency against stored chunks
        for (const auto& existingChunk : pImpl->actionChunks) {
            if (chunkActionSequencesEqual(chunk, existingChunk.second)) {
                isFrequent = false;
                break;
            }
        }
        
        if (isFrequent) {
            pImpl->actionChunks.push_back({skillName + "_chunk_" + std::to_string(i), chunk});
        }
    }
}

void ProceduralMemory::extractMotorChunks(const Skill& skill) {
    // Extract motor patterns from skill neural representation
    // This would integrate with the motor neuron groups in a real implementation
    
    // For now, create placeholder chunks based on skill pattern size
    size_t numChunks = std::max(1, skill.neuralPattern.size() / 10);
    
    for (size_t i = 0; i < numChunks; ++i) {
        std::string chunkName = skill.name + "_chunk_" + std::to_string(i);
        pImpl->actionChunks.push_back({chunkName, {}}); // Empty chunks for now
    }
}

float ProceduralMemory::calculateExecutionEfficiency(const std::string& skillName,
                                                     const std::vector<MotorCommand>& actions,
                                                     double executionTime) const {
    // Calculate efficiency based on action count, success rate, and time
    if (actions.empty()) return 0.0f;
    
    // Time efficiency (lower is better)
    float timeScore = std::exp(-executionTime * 0.01f); // Exponential decay
    
    // Action efficiency (fewer actions is better for simple skills)
    float actionScore = std::exp(-static_cast<float>(actions.size()) * 0.1f);
    
    // Success probability based on skill proficiency
    Skill* skill = const_cast<ProceduralMemory*>(this)->getSkill(skillName);
    float proficiency = skill ? skill->proficiency : 0.5f;
    float successScore = proficiency;
    
    // Combined efficiency
    return (timeScore * 0.4f + actionScore * 0.3f + successScore * 0.3f);
}

bool ProceduralMemory::sequencesEqual(const std::vector<size_t>& seq1, 
                                     const std::vector<size_t>& seq2) const {
    if (seq1.size() != seq2.size()) return false;
    for (size_t i = 0; i < seq1.size(); ++i) {
        if (seq1[i] != seq2[i]) return false;
    }
    return true;
}

bool ProceduralMemory::chunkActionSequencesEqual(const std::vector<MotorCommand>& chunk1,
                                                 const std::pair<std::string, std::vector<MotorCommand>>& chunk2) const {
    if (chunk1.size() != chunk2.second.size()) return false;
    for (size_t i = 0; i < chunk1.size(); ++i) {
        if (chunk1[i] != chunk2.second[i]) return false;
    }
    return true;
}

void ProceduralMemory::applyConsolidation(double currentTime) {
    double timeSinceConsolidation = currentTime - pImpl->lastConsolidationTime;
    
    if (timeSinceConsolidation > pImpl->consolidationInterval) {
        // Consolidate highly proficient skills
        std::vector<Skill*> proficientSkills;
        for (auto& skill : pImpl->skills) {
            if (skill.proficiency > 0.8f) {
                proficientSkills.push_back(&skill);
            }
        }
        
        // Apply synaptic strengthening to proficient skills
        for (auto* skill : proficientSkills) {
            // Increase proficiency more rapidly for well-learned skills
            skill->proficiency = std::min(1.0f, skill->proficiency + 0.05f);
            pImpl->skillProficiency[skill->name] = skill->proficiency;
        }
        
        // Remove poorly learned skills
        pImpl->skills.erase(std::remove_if(pImpl->skills.begin(), pImpl->skills.end(),
                                          [](const Skill& s) { return s.proficiency < 0.1f; }),
                           pImpl->skills.end());
        
        pImpl->lastConsolidationTime = currentTime;
        
        std::cout << "Procedural memory consolidation completed" << std::endl;
    }
}

std::vector<MotorCommand> ProceduralMemory::retrieveOptimalSequence(const std::string& skillName) const {
    // Retrieve the best sequence for a given skill
    Skill* skill = const_cast<ProceduralMemory*>(this)->getSkill(skillName);
    if (!skill) return {};
    
    // Find the most frequent/completed sequence for this skill
    std::vector<size_t> bestSequence;
    for (const auto& sequence : pImpl->skillSequences) {
        // Simple heuristic: choose longest sequence with high proficiency
        if (bestSequence.empty() || sequence.size() > bestSequence.size()) {
            bestSequence = sequence;
        }
    }
    
    // Convert to MotorCommand
    std::vector<MotorCommand> result;
    for (size_t idx : bestSequence) {
        if (idx < static_cast<size_t>(MotorCommand::Wait)) {
            result.push_back(static_cast<MotorCommand>(idx));
        }
    }
    
    return result;
}

float ProceduralMemory::getSkillProficiency(const std::string& skillName) const {
    auto it = pImpl->skillProficiency.find(skillName);
    return (it != pImpl->skillProficiency.end()) ? it->second : 0.0f;
}

std::vector<std::string> ProceduralMemory::getAllSkillNames() const {
    std::vector<std::string> names;
    for (const auto& skill : pImpl->skills) {
        names.push_back(skill.name);
    }
    return names;
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    // Main associative pairs with strength and timing
    std::vector<std::tuple<NeuronId, NeuronId, float, double>> associations;
    
    // Temporal associations (causal relationships)
    std::vector<std::tuple<NeuronId, NeuronId, double, double>> temporalAssociations;
    
    // Predictive associations (prediction error based)
    std::vector<std::tuple<NeuronId, NeuronId, float, float>> predictiveAssociations;
    
    // Association statistics
    std::map<NeuronId, std::vector<NeuronId>> forwardAssociations;
    std::map<NeuronId, std::vector<NeuronId>> backwardAssociations;
    
    // Hebbian learning parameters
    float hebbianLearningRate;
    float decayRate;
    double timeToLive; // How long associations last
    
    // Association rules
    struct AssociationRule {
        float hebbianWeight;
        float temporalThreshold;
        float predictionThreshold;
        bool isBidirectional;
    };
    
    AssociationRule associationRule;
    
    Impl() : hebbianLearningRate(0.01f), decayRate(0.001f), timeToLive(100.0) {
        associationRule = {0.5f, 10.0, 0.5f, true};
    }
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Check if association already exists
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            // Update existing association with Hebbian learning
            std::get<2>(assoc) = std::min(1.0f, std::get<2>(assoc) + strength * pImpl->hebbianLearningRate);
            std::get<3>(assoc) = 0.0; // Reset TTL
            return;
        }
    }
    
    // New association
    pImpl->associations.emplace_back(a, b, strength, 0.0);
    
    // Update forward/backward maps
    pImpl->forwardAssociations[a].push_back(b);
    pImpl->backwardAssociations[b].push_back(a);
    
    // Check for higher-order associations
    updateHigherOrderAssociations(a, b, strength);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    // Check main associations
    for (const auto& assoc : pImpl->associations) {
        if (std::get<0>(assoc) == neuron) {
            result.push_back(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == neuron) {
            result.push_back(std::get<0>(assoc));
        }
    }
    
    // Check temporal associations
    for (const auto& tempAssoc : pImpl->temporalAssociations) {
        if (std::get<0>(tempAssoc) == neuron) {
            result.push_back(std::get<1>(tempAssoc));
        } else if (std::get<1>(tempAssoc) == neuron) {
            result.push_back(std::get<0>(tempAssoc));
        }
    }
    
    // Check predictive associations
    for (const auto& predAssoc : pImpl->predictiveAssociations) {
        if (std::get<0>(predAssoc) == neuron) {
            result.push_back(std::get<1>(predAssoc));
        } else if (std::get<1>(predAssoc) == neuron) {
            result.push_back(std::get<0>(predAssoc));
        }
    }
    
    // Remove duplicates
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    float maxStrength = 0.0f;
    
    // Check main associations
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            maxStrength = std::max(maxStrength, std::get<2>(assoc));
        }
    }
    
    // Check temporal associations
    for (const auto& tempAssoc : pImpl->temporalAssociations) {
        if ((std::get<0>(tempAssoc) == a && std::get<1>(tempAssoc) == b) ||
            (std::get<0>(tempAssoc) == b && std::get<1>(tempAssoc) == a)) {
            maxStrength = std::max(maxStrength, static_cast<float>(std::get<2>(tempAssoc)));
        }
    }
    
    // Check predictive associations
    for (const auto& predAssoc : pImpl->predictiveAssociations) {
        if ((std::get<0>(predAssoc) == a && std::get<1>(predAssoc) == b) ||
            (std::get<0>(predAssoc) == b && std::get<1>(predAssoc) == a)) {
            maxStrength = std::max(maxStrength, std::get<2>(predAssoc));
        }
    }
    
    return maxStrength;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    // Update association with change
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            std::get<3>(assoc) = 0.0; // Reset TTL
            return;
        }
    }
    
    // Create new association if not found
    associate(a, b, delta);
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->temporalAssociations.clear();
    pImpl->predictiveAssociations.clear();
    pImpl->forwardAssociations.clear();
    pImpl->backwardAssociations.clear();
}

// Enhanced Associative Memory with Advanced Learning
void AssociativeMemory::recordCoActivation(NeuronId a, NeuronId b, double timeDifference, 
                                           float predictionError) {
    // Apply different learning rules based on timing and prediction error
    
    // Hebbian learning for simultaneous co-activation
    if (std::abs(timeDifference) < pImpl->associationRule.temporalThreshold) {
        float strength = pImpl->hebbianLearningRate * 
                         (1.0f - std::abs(timeDifference) / pImpl->associationRule.temporalThreshold);
        associate(a, b, strength);
    }
    
    // Temporal learning for causal relationships
    if (timeDifference > 0) {
        // a precedes b - create temporal association
        for (const auto& existing : pImpl->temporalAssociations) {
            if ((std::get<0>(existing) == a && std::get<1>(existing) == b) ||
                (std::get<0>(existing) == b && std::get<1>(existing) == a)) {
                // Update existing temporal association
                std::get<2>(existing) = std::min(std::get<2>(existing), timeDifference);
                std::get<3>(existing) = 0.0; // Reset TTL
                return;
            }
        }
        
        pImpl->temporalAssociations.emplace_back(a, b, timeDifference, 0.0);
    }
    
    // Predictive learning based on prediction error
    if (std::abs(predictionError) > pImpl->associationRule.predictionThreshold) {
        float strength = predictionError * 0.1f; // Scale prediction error to association strength
        associate(a, b, strength);
        
        // Create predictive association
        for (auto& predAssoc : pImpl->predictiveAssociations) {
            if ((std::get<0>(predAssoc) == a && std::get<1>(predAssoc) == b) ||
                (std::get<0>(predAssoc) == b && std::get<1>(predAssoc) == a)) {
                std::get<2>(predAssoc) = std::max(std::get<2>(predAssoc), std::abs(predictionError));
                std::get<3>(predAssoc) = std::max(std::get<3>(predAssoc), predictionError);
                return;
            }
        }
        
        pImpl->predictiveAssociations.emplace_back(a, b, std::abs(predictionError), predictionError);
    }
}

void AssociativeMemory::update(double timestep) {
    // Update associations - apply decay and TTL
    double currentTime = timestep;
    
    // Update temporal associations
    for (auto& tempAssoc : pImpl->temporalAssociations) {
        std::get<3>(tempAssoc) += timestep; // Age temporal associations
        
        // Apply TTL decay
        if (std::get<3>(tempAssoc) > pImpl->timeToLive) {
            // Remove old temporal associations
            for (auto it = pImpl->temporalAssociations.begin(); 
                 it != pImpl->temporalAssociations.end(); ) {
                if (std::get<3>(*it) > pImpl->timeToLive) {
                    // Remove from forward/backward maps
                    NeuronId a = std::get<0>(*it);
                    NeuronId b = std::get<1>(*it);
                    
                    auto& forwardList = pImpl->forwardAssociations[a];
                    forwardList.erase(std::remove(forwardList.begin(), forwardList.end(), b), 
                                     forwardList.end());
                    
                    auto& backwardList = pImpl->backwardAssociations[b];
                    backwardList.erase(std::remove(backwardList.begin(), backwardList.end(), a), 
                                      backwardList.end());
                    
                    it = pImpl->temporalAssociations.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
    
    // Update main associations - apply decay
    for (auto& assoc : pImpl->associations) {
        std::get<2>(assoc) *= (1.0f - pImpl->decayRate * timestep);
        
        // Apply TTL to main associations
        std::get<3>(assoc) += timestep;
        if (std::get<3>(assoc) > pImpl->timeToLive) {
            // Remove from forward/backward maps
            NeuronId a = std::get<0>(assoc);
            NeuronId b = std::get<1>(assoc);
            
            auto& forwardList = pImpl->forwardAssociations[a];
            forwardList.erase(std::remove(forwardList.begin(), forwardList.end(), b), 
                             forwardList.end());
            
            auto& backwardList = pImpl->backwardAssociations[b];
            backwardList.erase(std::remove(backwardList.begin(), backwardList.end(), a), 
                              backwardList.end());
            
            // Remove association
            for (auto it = pImpl->associations.begin(); it != pImpl->associations.end(); ++it) {
                if (it->time_tuple() == std::tie(a, b, std::get<2>(*it), std::get<3>(*it))) {
                    pImpl->associations.erase(it);
                    break;
                }
            }
        }
    }
    
    // Update predictive associations
    for (auto& predAssoc : pImpl->predictiveAssociations) {
        std::get<2>(predAssoc) *= (1.0f - pImpl->decayRate * timestep * 2.0f); // Faster decay
        std::get<3>(predAssoc) *= (1.0f - pImpl->decayRate * timestep);
    }
    
    updateHigherOrderAssociations(); // Update higher-order associations
}

void AssociativeMemory::updateHigherOrderAssociations() {
    // Update meta-associations and higher-order patterns
    // This creates associations between associations
    
    // For each pair of neurons with multiple associations, create meta-associations
    std::vector<std::pair<NeuronId, NeuronId>> metaPairs;
    
    for (const auto& assoc1 : pImpl->associations) {
        for (const auto& assoc2 : pImpl->associations) {
            if (std::get<0>(assoc1) != std::get<0>(assoc2) && 
                std::get<1>(assoc1) != std::get<1>(assoc2)) {
                
                // Check if these associations share neurons
                if (std::get<0>(assoc1) == std::get<0>(assoc2) ||
                    std::get<0>(assoc1) == std::get<1>(assoc2) ||
                    std::get<1>(assoc1) == std::get<0>(assoc2) ||
                    std::get<1>(assoc1) == std::get<1>(assoc2)) {
                    
                    // Create meta-association between the paired neurons
                    NeuronId a = std::get<0>(assoc1);
                    NeuronId b = std::get<1>(assoc1);
                    NeuronId c = std::get<0>(assoc2);
                    NeuronId d = std::get<1>(assoc2);
                    
                    if (a != c && a != d && b != c && b != d) {
                        float strength = (std::get<2>(assoc1) + std::get<2>(assoc2)) * 0.5f;
                        metaPairs.push_back({a, c});
                        metaPairs.push_back({b, d});
                    }
                }
            }
        }
    }
    
    // Add or update meta-associations
    for (const auto& pair : metaPairs) {
        NeuronId a = pair.first;
        NeuronId b = pair.second;
        
        // Check if meta-association already exists
        bool exists = false;
        for (const auto& assoc : pImpl->associations) {
            if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            associate(a, b, 0.1f); // Add meta-association with weak strength
        }
    }
}

void AssociativeMemory::updateHigherOrderAssociations(NeuronId a, NeuronId b, float strength) {
    // Create higher-order associations when new associations are formed
    
    // Find neurons associated with a and b
    std::vector<NeuronId> assocWithA = getAssociations(a);
    std::vector<NeuronId> assocWithB = getAssociations(b);
    
    // Create meta-associations between associated neurons
    for (NeuronId neuronA : assocWithA) {
        for (NeuronId neuronB : assocWithB) {
            if (neuronA != neuronB && neuronA != a && neuronA != b && 
                neuronB != a && neuronB != b) {
                
                // Create meta-association
                associate(neuronA, neuronB, strength * 0.3f);
            }
        }
    }
}

std::vector<std::tuple<NeuronId, NeuronId, float>> AssociativeMemory::getAllAssociations() const {
    std::vector<std::tuple<NeuronId, NeuronId, float>> result;
    
    for (const auto& assoc : pImpl->associations) {
        result.push_back(std::make_tuple(std::get<0>(assoc), std::get<1>(assoc), std::get<2>(assoc)));
    }
    
    return result;
}

std::vector<std::tuple<NeuronId, NeuronId, double>> AssociativeMemory::getTemporalAssociations() const {
    return pImpl->temporalAssociations;
}

float AssociativeMemory::getHebbianLearningRate() const {
    return pImpl->hebbianLearningRate;
}

void AssociativeMemory::setHebbianLearningRate(float rate) {
    pImpl->hebbianLearningRate = rate;
}

float AssociativeMemory::getDecayRate() const {
    return pImpl->decayRate;
}

void AssociativeMemory::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

std::vector<std::pair<NeuronId, NeuronId>> AssociativeMemory::findAssociationChains(size_t minChainLength) const {
    // Find chains of associations (A-B, B-C, C-D...)
    std::vector<std::pair<NeuronId, NeuronId>> chains;
    
    // Build adjacency list
    std::map<NeuronId, std::vector<NeuronId>> graph;
    for (const auto& assoc : pImpl->associations) {
        NeuronId a = std::get<0>(assoc);
        NeuronId b = std::get<1>(assoc);
        
        if (std::get<2>(assoc) > 0.5f) { // Only strong associations
            graph[a].push_back(b);
            if (std::get<2>(assoc) > 0.5f) { // Bidirectional if strong
                graph[b].push_back(a);
            }
        }
    }
    
    // Find chains using DFS
    std::map<NeuronId, bool> visited;
    for (const auto& pair : graph) {
        visited[pair.first] = false;
    }
    
    for (const auto& pair : graph) {
        if (!visited[pair.first]) {
            std::vector<NeuronId> currentChain = {pair.first};
            findChainDFS(pair.first, graph, visited, currentChain, chains, minChainLength);
        }
    }
    
    return chains;
}

void AssociativeMemory::findChainDFS(NeuronId current, 
                                     const std::map<NeuronId, std::vector<NeuronId>>& graph,
                                     std::map<NeuronId, bool>& visited,
                                     std::vector<NeuronId>& currentChain,
                                     std::vector<std::pair<NeuronId, NeuronId>>& chains,
                                     size_t minChainLength) const {
    visited[current] = true;
    
    auto it = graph.find(current);
    if (it != graph.end()) {
        for (NeuronId neighbor : it->second) {
            if (!visited[neighbor]) {
                currentChain.push_back(neighbor);
                
                if (currentChain.size() >= minChainLength) {
                    chains.push_back(std::make_pair(currentChain[0], currentChain.back()));
                }
                
                findChainDFS(neighbor, graph, visited, currentChain, chains, minChainLength);
                
                currentChain.pop_back();
            }
        }
    }
    
    visited[current] = false;
}

} // namespace nlm
