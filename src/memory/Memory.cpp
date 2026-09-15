#include "Memory.hpp"
#include <algorithm>

namespace nlm {

// Working Memory Implementation - delegates to NeuralWorkingMemory
struct WorkingMemory::Impl {
    NeuralWorkingMemory neuralWorkingMemory;
    size_t capacity;
    
    Impl(size_t cap) : capacity(cap) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Delegate to neural working memory
    pImpl->neuralWorkingMemory.storeToNeuron(neuron, value);
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    return pImpl->neuralWorkingMemory.getNeuronActivation(neuron);
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return pImpl->neuralWorkingMemory.contains(neuron);
}

void WorkingMemory::clear() {
    pImpl->neuralWorkingMemory.clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->neuralWorkingMemory.getCapacity();
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->neuralWorkingMemory.getActiveTraces();
}

void WorkingMemory::decay(float decayRate) {
    // Neural working memory handles decay internally
}

void WorkingMemory::consolidate(float threshold) {
    // Neural working memory handles consolidation internally
}

void WorkingMemory::update(SimulationStep timestep) {
    // Neural working memory updates internally
}

// Episodic Memory Implementation - delegates to NeuralEpisodicMemory
struct EpisodicMemory::Impl {
    NeuralEpisodicMemory neuralEpisodicMemory;
    
    Impl() {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl), replayEnabled_(false) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::initialize(Brain* brain) {
    pImpl->neuralEpisodicMemory.initialize(brain);
}

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    pImpl->neuralEpisodicMemory.storeEpisode(episode);
}

const EpisodicMemoryItem* EpisodicMemory::getEpisode(size_t index) const {
    return pImpl->neuralEpisodicMemory.getEpisode(index);
}

size_t EpisodicMemory::getEpisodeCount() const {
    return pImpl->neuralEpisodicMemory.getEpisodeCount();
}

std::vector<const EpisodicMemoryItem*> EpisodicMemory::getRecentEpisodes(size_t count) const {
    return pImpl->neuralEpisodicMemory.getRecentEpisodes(count);
}

std::vector<const EpisodicMemoryItem*> EpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    size_t maxResults) const {
    return pImpl->neuralEpisodicMemory.retrieveSimilar(sensoryPattern, maxResults);
}

std::vector<const EpisodicMemoryItem*> EpisodicMemory::retrieveTemporal(
    SimulationStep startTime,
    SimulationStep endTime,
    size_t maxResults) const {
    return pImpl->neuralEpisodicMemory.retrieveTemporal(startTime, endTime, maxResults);
}

std::vector<const EpisodicMemoryItem*> EpisodicMemory::retrieveByLocation(
    float x, float y, float radius,
    size_t maxResults) const {
    return pImpl->neuralEpisodicMemory.retrieveByLocation(x, y, radius, maxResults);
}

void EpisodicMemory::clear() {
    pImpl->neuralEpisodicMemory.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    pImpl->neuralEpisodicMemory.consolidate(relevanceThreshold);
}

void EpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    pImpl->neuralEpisodicMemory.replayEpisode(episode);
}

void EpisodicMemory::updateRelevance(size_t episodeId, float relevanceDelta) {
    pImpl->neuralEpisodicMemory.updateRelevance(episodeId, relevanceDelta);
}

std::vector<const EpisodicMemoryItem*> EpisodicMemory::getEpisodesForReplay(size_t count) const {
    return pImpl->neuralEpisodicMemory.getEpisodesForReplay(count);
}

void EpisodicMemory::replaySequence(const std::vector<size_t>& episodeIds) {
    pImpl->neuralEpisodicMemory.replaySequence(episodeIds);
}

float EpisodicMemory::getAverageReward() const {
    return pImpl->neuralEpisodicMemory.getAverageReward();
}

void EpisodicMemory::enableReplay(bool enable) {
    replayEnabled_ = enable;
    pImpl->neuralEpisodicMemory.enableReplay(enable);
}

bool EpisodicMemory::isReplayEnabled() const {
    return replayEnabled_;
}

// Semantic Memory Implementation - delegates to Neural Semantic implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            return;
        }
    }
    pImpl->facts.emplace_back(key, value);
    
    // Also create distributed representation
    if (pImpl->brain) {
        std::vector<float> pattern;
        // In real implementation, this would create a distributed pattern
        // For now, we create a simple pattern based on key length and value hash
        pattern.push_back(static_cast<float>(key.length()) / 100.0f);
        pattern.push_back(std::hash<std::string>{}(value) % 1000 / 1000.0f);
        
        pImpl->brain->getConceptFormation()->learnConcept(pattern, key);
    }
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
    if (pImpl->brain) {
        pImpl->brain->getConceptFormation()->clearConcepts();
    }
}

void SemanticMemory::storeConcept(const std::vector<float>& pattern, const std::string& label) {
    if (pImpl->brain) {
        pImpl->brain->getConceptFormation()->learnConcept(pattern, label);
    }
}

std::string SemanticMemory::retrieveConcept(const std::vector<float>& pattern) const {
    if (pImpl->brain) {
        return pImpl->brain->getConceptFormation()->retrieveConcept(pattern);
    }
    return "";
}

float SemanticMemory::getConceptSimilarity(const std::vector<float>& patternA,
                                           const std::vector<float>& patternB) const {
    if (pImpl->brain) {
        return pImpl->brain->getConceptFormation()->getConceptSimilarity(patternA, patternB);
    }
    return 0.0f;
}

void SemanticMemory::updateConcept(const std::vector<float>& pattern, float feedback) {
    if (pImpl->brain) {
        pImpl->brain->getConceptFormation()->updateConcept(pattern, feedback);
    }
}

// Procedural Memory Implementation - delegates to new ProceduralMemory
struct ProceduralMemory::Impl {
    std::unique_ptr<ProceduralMemory> proceduralMemory;
    
    Impl() : proceduralMemory(std::make_unique<ProceduralMemory>()) {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::initialize(Brain* brain) {
    pImpl->proceduralMemory->setBrain(brain);
}

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    pImpl->proceduralMemory->learnSkill(name, pattern);
}

const Skill* ProceduralMemory::getSkill(const std::string& name) const {
    return pImpl->proceduralMemory->getSkill(name);
}

std::vector<const Skill*> ProceduralMemory::getAllSkills() const {
    return pImpl->proceduralMemory->getAllSkills();
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta, bool success) {
    pImpl->proceduralMemory->updateProficiency(name, delta);
    
    // Learn from outcome
    pImpl->proceduralMemory->learnFromOutcome(name, std::vector<float>(), success);
}

bool ProceduralMemory::executeSkill(const std::string& name, const std::vector<float>& context) {
    return pImpl->proceduralMemory->executeSkill(name, context);
}

void ProceduralMemory::clear() {
    pImpl->proceduralMemory->clear();
}

void ProceduralMemory::consolidate() {
    pImpl->proceduralMemory->consolidate();
}

std::vector<const Skill*> ProceduralMemory::getAvailableSkills(const std::vector<float>& context) const {
    return pImpl->proceduralMemory->getAvailableSkills(context);
}

void ProceduralMemory::setSkillContext(const std::string& skillName, const std::vector<NeuronId>& context) {
    pImpl->proceduralMemory->setSkillContext(skillName, context);
}

void ProceduralMemory::clearSkillContext(const std::string& skillName) {
    pImpl->proceduralMemory->clearSkillContext(skillName);
}

// Associative Memory Implementation - delegates to NeuralAssociativeMemory
struct AssociativeMemory::Impl {
    NeuralAssociativeMemory neuralAssociativeMemory;
    
    Impl() {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::initialize(Brain* brain) {
    pImpl->neuralAssociativeMemory.initialize(brain);
}

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    pImpl->neuralAssociativeMemory.associate(
        std::vector<float>{a.getId() % 1000 / 1000.0f}, 
        std::vector<float>{b.getId() % 1000 / 1000.0f}, 
        strength);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    // Convert to pattern, retrieve associations, convert back
    std::vector<float> pattern{neuron.getId() % 1000 / 1000.0f};
    
    std::vector<std::vector<float>> associatedPatterns = 
        pImpl->neuralAssociativeMemory.retrieve(pattern, 10);
    
    std::vector<NeuronId> result;
    for (const auto& pattern : associatedPatterns) {
        if (!pattern.empty()) {
            // Convert pattern back to NeuronId (simplified)
            NeuronId assoc(pattern[0] * 1000);
            result.push_back(assoc);
        }
    }
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    std::vector<float> patternA{a.getId() % 1000 / 1000.0f};
    std::vector<float> patternB{b.getId() % 1000 / 1000.0f};
    
    return pImpl->neuralAssociativeMemory.getAssociationStrength(patternA, patternB);
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    std::vector<float> patternA{a.getId() % 1000 / 1000.0f};
    std::vector<float> patternB{b.getId() % 1000 / 1000.0f};
    
    pImpl->neuralAssociativeMemory.updateAssociation(patternA, patternB, delta);
}

void AssociativeMemory::clear() {
    pImpl->neuralAssociativeMemory.clear();
}

std::vector<NeuronId> AssociativeMemory::spreadActivation(NeuronId startNeuron, size_t steps) const {
    std::vector<float> pattern{startNeuron.getId() % 1000 / 1000.0f};
    
    std::vector<std::vector<float>> activatedPatterns = 
        pImpl->neuralAssociativeMemory.spreadActivation(pattern, steps);
    
    std::vector<NeuronId> result;
    for (const auto& pattern : activatedPatterns) {
        if (!pattern.empty()) {
            NeuronId neuron(pattern[0] * 1000);
            result.push_back(neuron);
        }
    }
    return result;
}

} // namespace nlm