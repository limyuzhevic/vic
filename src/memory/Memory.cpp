#include "Memory.hpp"
#include <algorithm>
#include <unordered_map>

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
    // TODO PHASE 2: Implement real storage with capacity limits
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            return;
        }
    }
    if (pImpl->items.size() < pImpl->capacity) {
        pImpl->items.emplace_back(neuron, value);
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
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    pImpl->episodes.push_back(episode);
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
    // TODO PHASE 2: Implement real consolidation
}

// Semantic Memory Implementation - Enhanced
struct SemanticMemory::Impl {
    // Facts as key-value pairs
    std::vector<std::pair<std::string, std::string>> facts;
    
    // Knowledge graph: entity relationships
    std::unordered_map<std::string, std::vector<std::pair<std::string, float>>> knowledgeGraph;
    
    // Concept hierarchy
    std::unordered_map<std::string, std::string> conceptParent;
    std::vector<std::string> conceptRoots;
    
    // Facts that have been verified (higher confidence)
    std::unordered_set<std::string> verifiedFacts;
    
    // Decay for fact age
    std::unordered_map<std::string, float> factAge;
    
    // Concept associations for inference
    std::unordered_map<std::string, std::vector<std::string>> conceptAssociations;
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Check if fact already exists
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            pImpl->factAge[key] = 0.0f;  // Reset age
            return;
        }
    }
    
    // Extract concepts from key and value
    extractConcepts(key, value);
    
    // Store the fact
    pImpl->facts.emplace_back(key, value);
    pImpl->factAge[key] = 0.0f;
    
    // Create inference from related concepts
    createConceptAssociations(key, value);
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return fact.second;
        }
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return pImpl->factAge.find(key) != pImpl->factAge.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    return pImpl->facts;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
    pImpl->knowledgeGraph.clear();
    pImpl->conceptParent.clear();
    pImpl->conceptRoots.clear();
    pImpl->verifiedFacts.clear();
    pImpl->factAge.clear();
    pImpl->conceptAssociations.clear();
}

// Enhanced SemanticMemory methods for advanced features
void SemanticMemory::extractConcepts(const std::string& key, const std::string& value) {
    // Simple concept extraction - in a real implementation,
    // this would use NLP techniques to identify entities, relationships, and concepts
    
    // Extract words from key and value
    std::vector<std::string> concepts;
    extractWords(key, concepts);
    extractWords(value, concepts);
    
    // Add to knowledge graph
    for (const auto& concept : concepts) {
        if (!concept.empty()) {
            pImpl->knowledgeGraph[concept].push_back(std::make_pair(key, 1.0f));
        }
    }
}

void SemanticMemory::extractWords(const std::string& text, std::vector<std::string>& concepts) {
    std::string word;
    for (char c : text) {
        if (std::isalnum(c) || c == '_') {
            word += std::tolower(c);
        } else if (!word.empty()) {
            concepts.push_back(word);
            word.clear();
        }
    }
    if (!word.empty()) {
        concepts.push_back(word);
    }
}

void SemanticMemory::createConceptAssociations(const std::string& key, const std::string& value) {
    // Create concept associations for inference
    // Find related concepts and establish associations
    
    std::string keyConcept = extractMainConcept(key);
    std::string valueConcept = extractMainConcept(value);
    
    if (!keyConcept.empty() && !valueConcept.empty() && keyConcept != valueConcept) {
        pImpl->conceptAssociations[keyConcept].push_back(valueConcept);
        pImpl->conceptAssociations[valueConcept].push_back(keyConcept);
    }
}

std::string SemanticMemory::extractMainConcept(const std::string& text) const {
    // Simple extraction of main concept (first significant word)
    std::string concept;
    for (char c : text) {
        if (std::isalnum(c) || c == '_') {
            concept += std::tolower(c);
        } else if (!concept.empty()) {
            break;  // Return first word
        }
    }
    return concept;
}

void SemanticMemory::verifyFact(const std::string& key) {
    // Mark fact as verified (higher confidence)
    if (hasFact(key)) {
        pImpl->verifiedFacts.insert(key);
    }
}

bool SemanticMemory::isFactVerified(const std::string& key) const {
    return pImpl->verifiedFacts.find(key) != pImpl->verifiedFacts.end();
}

void SemanticMemory::incrementFactAge(float delta) {
    // Age all facts
    for (auto& pair : pImpl->factAge) {
        pair.second += delta;
    }
}

std::vector<std::string> SemanticMemory::inferFromConcept(const std::string& concept) const {
    // Infer related facts from concept associations
    std::vector<std::string> inferred;
    
    auto it = pImpl->conceptAssociations.find(concept);
    if (it != pImpl->conceptAssociations.end()) {
        for (const auto& relatedConcept : it->second) {
            // Find facts related to this concept
            auto kgIt = pImpl->knowledgeGraph.find(relatedConcept);
            if (kgIt != pImpl->knowledgeGraph.end()) {
                for (const auto& fact : kgIt->second) {
                    if (!hasFact(fact.first)) {
                        inferred.push_back(fact.first + " -> " + fact.second);
                    }
                }
            }
        }
    }
    
    return inferred;
}

void SemanticMemory::decayKnowledge(float decayRate) {
    // Decay all knowledge based on age
    for (auto& pair : pImpl->factAge) {
        pair.second *= (1.0f - decayRate);
    }
    
    // Remove aged facts (older than 100 time units)
    for (auto it = pImpl->factAge.begin(); it != pImpl->factAge.end();) {
        if (it->second > 100.0f) {
            removeFact(it->first);
            it = pImpl->factAge.erase(it);
        } else {
            ++it;
        }
    }
}

void SemanticMemory::removeFact(const std::string& key) {
    // Remove fact and related knowledge
    for (auto it = pImpl->facts.begin(); it != pImpl->facts.end(); ++it) {
        if (it->first == key) {
            pImpl->facts.erase(it);
            break;
        }
    }
    
    // Remove from knowledge graph
    for (auto it = pImpl->knowledgeGraph.begin(); it != pImpl->knowledgeGraph.end(); ++it) {
        for (auto factIt = it->second.begin(); factIt != it->second.end(); ) {
            if (factIt->first == key) {
                factIt = it->second.erase(factIt);
            } else {
                ++factIt;
            }
        }
        if (it->second.empty()) {
            pImpl->knowledgeGraph.erase(it);
            break;
        }
    }
}

int SemanticMemory::getFactCount() const {
    return pImpl->facts.size();
}

int SemanticMemory::getVerifiedFactCount() const {
    return pImpl->verifiedFacts.size();
}

float SemanticMemory::getKnowledgeDensity() const {
    // Measure how interconnected the knowledge is
    if (pImpl->facts.empty()) return 0.0f;
    
    float totalConnections = 0.0f;
    for (const auto& pair : pImpl->conceptAssociations) {
        totalConnections += pair.second.size();
    }
    
    return totalConnections / pImpl->facts.size();
}

std::vector<std::string> SemanticMemory::getConcepts() const {
    std::vector<std::string> concepts;
    for (const auto& pair : pImpl->knowledgeGraph) {
        concepts.push_back(pair.first);
    }
    return concepts;
}

// Procedural Memory Implementation - Enhanced
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    
    // Skill execution patterns with parameters
    std::unordered_map<std::string, std::vector<std::vector<float>>> executionPatterns;
    
    // Skill prerequisites and dependencies
    std::unordered_map<std::string, std::vector<std::string>> skillPrerequisites;
    std::unordered_map<std::string, std::vector<std::string>> skillDependents;
    
    // Skill performance metrics
    std::unordered_map<std::string, std::vector<float>> performanceHistory;
    std::unordered_map<std::string, float> averagePerformance;
    
    // Skill sequences (complex procedures)
    std::vector<std::vector<std::string>> skillSequences;
    
    // Neuromodulation effects on skill learning
    std::unordered_map<std::string, float> neuromodulationFactors;
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Check if skill already exists
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            skill.proficiency = std::clamp(skill.proficiency + 0.2f, 0.0f, 1.0f);
            return;
        }
    }
    
    // Create new skill
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = 0.5f;  // Start with moderate proficiency
    
    pImpl->skills.push_back(skill);
    
    // Initialize performance tracking
    pImpl->performanceHistory[name] = std::vector<float>();
    pImpl->averagePerformance[name] = 0.5f;
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
            
            // Update performance history
            auto& history = pImpl->performanceHistory[name];
            history.push_back(skill.proficiency);
            if (history.size() > 100) {
                history.erase(history.begin());
            }
            
            // Update average performance
            float sum = 0.0f;
            for (float perf : history) {
                sum += perf;
            }
            pImpl->averagePerformance[name] = sum / history.size();
            
            return;
        }
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->executionPatterns.clear();
    pImpl->skillPrerequisites.clear();
    pImpl->skillDependents.clear();
    pImpl->performanceHistory.clear();
    pImpl->averagePerformance.clear();
    pImpl->skillSequences.clear();
    pImpl->neuromodulationFactors.clear();
}

// Enhanced ProceduralMemory methods for advanced features
void ProceduralMemory::storeExecutionPattern(const std::string& skillName,
                                            const std::vector<std::vector<float>>& pattern) {
    pImpl->executionPatterns[skillName] = pattern;
}

std::vector<std::vector<float>> ProceduralMemory::retrieveExecutionPattern(const std::string& skillName) const {
    auto it = pImpl->executionPatterns.find(skillName);
    if (it != pImpl->executionPatterns.end()) {
        return it->second;
    }
    return std::vector<std::vector<float>>();
}

void ProceduralMemory::addSkillPrerequisite(const std::string& skill, const std::string& prerequisite) {
    pImpl->skillPrerequisites[skill].push_back(prerequisite);
    pImpl->skillDependents[prerequisite].push_back(skill);
}

bool ProceduralMemory::canExecuteSkill(const std::string& skill) const {
    // Check if all prerequisites are met (basic check - in reality would need brain state)
    auto it = pImpl->skillPrerequisites.find(skill);
    if (it == pImpl->skillPrerequisites.end()) {
        return true;  // No prerequisites
    }
    
    // For now, all prerequisites are assumed met
    // In a real implementation, this would check brain state and skill proficiency
    return true;
}

std::vector<std::string> ProceduralMemory::getSkillDependencies(const std::string& skill) const {
    auto it = pImpl->skillDependents.find(skill);
    if (it != pImpl->skillDependents.end()) {
        return it->second;
    }
    return std::vector<std::string>();
}

std::vector<std::string> ProceduralMemory::getSkillPrerequisites(const std::string& skill) const {
    auto it = pImpl->skillPrerequisites.find(skill);
    if (it != pImpl->skillPrerequisites.end()) {
        return it->second;
    }
    return std::vector<std::string>();
}

float ProceduralMemory::getSkillProficiency(const std::string& skill) const {
    auto it = pImpl->averagePerformance.find(skill);
    if (it != pImpl->averagePerformance.end()) {
        return it->second;
    }
    
    // If no performance history, find in skills
    for (const auto& skillObj : pImpl->skills) {
        if (skillObj.name == skill) {
            return skillObj.proficiency;
        }
    }
    
    return 0.0f;
}

void ProceduralMemory::setNeuromodulationFactor(const std::string& skill, float factor) {
    pImpl->neuromodulationFactors[skill] = factor;
}

float ProceduralMemory::getNeuromodulationFactor(const std::string& skill) const {
    auto it = pImpl->neuromodulationFactors.find(skill);
    if (it != pImpl->neuromodulationFactors.end()) {
        return it->second;
    }
    return 1.0f;  // Default no modulation
}

float ProceduralMemory::getAverageSkillPerformance() const {
    if (pImpl->averagePerformance.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& pair : pImpl->averagePerformance) {
        sum += pair.second;
    }
    return sum / pImpl->averagePerformance.size();
}

void ProceduralMemory::createSkillSequence(const std::vector<std::string>& sequence) {
    // Create a complex sequence of skills
    pImpl->skillSequences.push_back(sequence);
}

bool ProceduralMemory::canExecuteSequence(const std::vector<std::string>& sequence) const {
    // Check if a skill sequence can be executed
    for (const auto& skillName : sequence) {
        if (!canExecuteSkill(skillName)) {
            return false;
        }
    }
    return true;
}

std::vector<std::vector<std::string>> ProceduralMemory::getSkillSequences() const {
    return pImpl->skillSequences;
}

int ProceduralMemory::getSkillCount() const {
    return pImpl->skills.size();
}

std::vector<std::string> ProceduralMemory::getAllSkillNames() const {
    std::vector<std::string> names;
    for (const auto& skill : pImpl->skills) {
        names.push_back(skill.name);
    }
    return names;
}

// Associative Memory Implementation - Enhanced
struct AssociativeMemory::Impl {
    // Strong associations between patterns
    std::vector<std::tuple<NeuronId, NeuronId, float>> strongAssociations;
    
    // Weak associations for spreading activation
    std::vector<std::tuple<NeuronId, NeuronId, float>> weakAssociations;
    
    // Activation levels for patterns
    std::unordered_map<NeuronId, float> patternActivations;
    
    // Temporal associations for sequence learning
    std::vector<std::vector<NeuronId>> sequences;
    
    // Chunk-based associations (grouping related patterns)
    std::vector<std::vector<NeuronId>> chunks;
    
    // Hebbian learning parameters
    float hebbianLearningRate;
    float consolidationThreshold;
    
    // Sparse coding for efficiency
    std::vector<NeuronId> activePatternBuffer;
    std::vector<float> activationBuffer;
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {
    pImpl->hebbianLearningRate = 0.01f;
    pImpl->consolidationThreshold = 0.3f;
}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Check if association already exists
    for (auto& assoc : pImpl->strongAssociations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = strength;
            return;
        }
    }
    
    // Store as strong association
    pImpl->strongAssociations.emplace_back(a, b, strength);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->strongAssociations) {
        if (std::get<0>(assoc) == neuron) {
            result.push_back(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == neuron) {
            result.push_back(std::get<0>(assoc));
        }
    }
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->strongAssociations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            return std::get<2>(assoc);
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->strongAssociations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void AssociativeMemory::clear() {
    pImpl->strongAssociations.clear();
    pImpl->weakAssociations.clear();
    pImpl->patternActivations.clear();
    pImpl->sequences.clear();
    pImpl->chunks.clear();
}

// Enhanced AssociativeMemory methods for advanced features
void AssociativeMemory::addWeakAssociation(NeuronId a, NeuronId b, float strength) {
    pImpl->weakAssociations.emplace_back(a, b, strength);
}

float AssociativeMemory::getTotalAssociationStrength(NeuronId neuron) const {
    float total = 0.0f;
    
    // Add strong associations
    for (const auto& assoc : pImpl->strongAssociations) {
        if (std::get<0>(assoc) == neuron) {
            total += std::get<2>(assoc);
        } else if (std::get<1>(assoc) == neuron) {
            total += std::get<2>(assoc);
        }
    }
    
    // Add weak associations
    for (const auto& assoc : pImpl->weakAssociations) {
        if (std::get<0>(assoc) == neuron) {
            total += std::get<2>(assoc) * 0.1f;  // Weaker contribution
        } else if (std::get<1>(assoc) == neuron) {
            total += std::get<2>(assoc) * 0.1f;
        }
    }
    
    return total;
}

std::vector<NeuronId> AssociativeMemory::spreadActivation(NeuronId sourceNeuron,
                                                          float threshold,
                                                          size_t maxSteps) const {
    std::vector<NeuronId> activated;
    std::set<NeuronId> visited;
    
    // Initialize with source neuron
    activated.push_back(sourceNeuron);
    visited.insert(sourceNeuron);
    
    // Use both strong and weak associations
    std::vector<std::tuple<NeuronId, NeuronId, float>> allAssociations = pImpl->strongAssociations;
    allAssociations.insert(allAssociations.end(), pImpl->weakAssociations.begin(), pImpl->weakAssociations.end());
    
    // Perform spreading activation
    for (size_t step = 0; step < maxSteps && !activated.empty(); ++step) {
        std::vector<NeuronId> nextToActivate;
        
        for (const auto& neuron : activated) {
            if (visited.find(neuron) != visited.end()) {
                continue;  // Already processed
            }
            
            // Find associations for this neuron
            for (const auto& assoc : allAssociations) {
                NeuronId target = INVALID_NEURON_ID;
                if (std::get<0>(assoc) == neuron && visited.find(std::get<1>(assoc)) == visited.end()) {
                    target = std::get<1>(assoc);
                } else if (std::get<1>(assoc) == neuron && visited.find(std::get<0>(assoc)) == visited.end()) {
                    target = std::get<0>(assoc);
                }
                
                if (target != INVALID_NEURON_ID) {
                    float strength = std::get<2>(assoc);
                    
                    // Scale by neuron activation level
                    auto actIt = pImpl->patternActivations.find(neuron);
                    if (actIt != pImpl->patternActivations.end()) {
                        strength *= actIt->second;
                    }
                    
                    // Check threshold
                    if (strength > threshold) {
                        nextToActivate.push_back(target);
                    }
                }
            }
        }
        
        // Add newly activated neurons
        for (const auto& neuron : nextToActivate) {
            if (visited.find(neuron) == visited.end()) {
                activated.push_back(neuron);
                visited.insert(neuron);
            }
        }
    }
    
    return activated;
}

void AssociativeMemory::recordSequence(const std::vector<NeuronId>& sequence) {
    // Record a sequence of neural activations
    if (sequence.size() >= 2) {
        pImpl->sequences.push_back(sequence);
        
        // Create chunk associations
        for (size_t i = 0; i < sequence.size() - 1; ++i) {
            associate(sequence[i], sequence[i + 1], 0.5f + (sequence.size() - i) * 0.02f);
        }
    }
}

std::vector<NeuronId> AssociativeMemory::predictNext(NeuronId currentNeuron,
                                                     size_t sequenceLength) const {
    std::vector<NeuronId> prediction;
    
    // Find all sequences starting with current neuron
    for (const auto& seq : pImpl->sequences) {
        if (!seq.empty() && seq[0] == currentNeuron) {
            // Predict next neuron(s)
            size_t predictionLength = std::min(sequenceLength, seq.size() - 1);
            for (size_t i = 1; i <= predictionLength && i < seq.size(); ++i) {
                prediction.push_back(seq[i]);
            }
            break;
        }
    }
    
    return prediction;
}

void AssociativeMemory::createChunk(const std::vector<NeuronId>& chunk) {
    // Create a chunk of related neurons
    pImpl->chunks.push_back(chunk);
    
    // Establish strong associations within chunk
    for (size_t i = 0; i < chunk.size(); ++i) {
        for (size_t j = i + 1; j < chunk.size(); ++j) {
            associate(chunk[i], chunk[j], 0.7f);
        }
    }
}

std::vector<std::vector<NeuronId>> AssociativeMemory::getChunks() const {
    return pImpl->chunks;
}

void AssociativeMemory::setHebbianLearningRate(float rate) {
    pImpl->hebbianLearningRate = rate;
}

void AssociativeMemory::setConsolidationThreshold(float threshold) {
    pImpl->consolidationThreshold = threshold;
}

float AssociativeMemory::getHebbianLearningRate() const {
    return pImpl->hebbianLearningRate;
}

float AssociativeMemory::getConsolidationThreshold() const {
    return pImpl->consolidationThreshold;
}

int AssociativeMemory::getAssociationCount() const {
    return pImpl->strongAssociations.size();
}

int AssociativeMemory::getSequenceCount() const {
    return pImpl->sequences.size();
}

int AssociativeMemory::getChunkCount() const {
    return pImpl->chunks.size();
}

float AssociativeMemory::getNetworkConnectivity() const {
    // Measure how well connected the network is
    if (pImpl->strongAssociations.empty()) return 0.0f;
    
    std::set<NeuronId> connectedNeurons;
    for (const auto& assoc : pImpl->strongAssociations) {
        connectedNeurons.insert(std::get<0>(assoc));
        connectedNeurons.insert(std::get<1>(assoc));
    }
    
    return static_cast<float>(connectedNeurons.size()) / 10000.0f;  // Normalized
}

} // namespace nlm
