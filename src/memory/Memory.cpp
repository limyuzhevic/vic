#include "Memory.hpp"
#include <algorithm>

namespace nlm {

// Working Memory Implementation
WorkingMemory::WorkingMemory()
    : items_()
    , capacity_(100)
{
}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Find existing item
    for (auto& item : items_) {
        if (item.first == neuron) {
            item.second = value;
            return;
        }
    }
    
    // Add new item if capacity allows
    if (items_.size() < capacity_) {
        items_.emplace_back(neuron, value);
    } else {
        // Replace weakest item (lowest activation)
        auto weakest = std::min_element(items_.begin(), items_.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        *weakest = std::make_pair(neuron, value);
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    for (const auto& item : items_) {
        if (item.first == neuron) {
            return item.second;
        }
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return std::find_if(items_.begin(), items_.end(),
        [neuron](const auto& item) { return item.first == neuron; }) != items_.end();
}

void WorkingMemory::clear() {
    items_.clear();
}

void WorkingMemory::decay(float decayRate) {
    for (auto& item : items_) {
        item.second *= (1.0f - decayRate);
    }
}

// Episodic Memory Implementation
EpisodicMemory::EpisodicMemory()
    : episodes_()
    , maxEpisodes_(1000)
{
}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Add relevance based on factors
    EpisodicMemoryItem stored = episode;
    
    // Calculate relevance score
    float relevance = 0.5f;  // Base relevance
    if (!episode.metadata.empty()) {
        relevance += 0.3f;
    }
    if (episode.neurons.size() > 10) {
        relevance += 0.2f;
    }
    relevance = std::min(1.0f, relevance);
    
    stored.relevance = relevance;
    
    // Add to episodes
    episodes_.push_back(stored);
    
    // Remove old episodes if over capacity
    while (episodes_.size() > maxEpisodes_) {
        episodes_.erase(episodes_.begin());
    }
    
    // Age all episodes
    for (auto& ep : episodes_) {
        // Simple aging based on age
    }
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    if (index < episodes_.size()) {
        return episodes_[index];
    }
    return EpisodicMemoryItem();
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<EpisodicMemoryItem> result;
    size_t start = count > episodes_.size() ? 0 : episodes_.size() - count;
    
    for (size_t i = start; i < episodes_.size(); ++i) {
        result.push_back(episodes_[i]);
    }
    
    return result;
}

void EpisodicMemory::clear() {
    episodes_.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Remove episodes below relevance threshold
    auto it = std::remove_if(episodes_.begin(), episodes_.end(),
        [relevanceThreshold](const EpisodicMemoryItem& ep) {
            return ep.relevance < relevanceThreshold;
        });
    
    if (it != episodes_.end()) {
        episodes_.erase(it, episodes_.end());
    }
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getEpisodesForReplay(size_t count) const {
    // Select episodes based on relevance and recency
    std::vector<std::pair<float, size_t>> scored;  // (score, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        const auto& ep = episodes_[i];
        
        // Score combines relevance and recency (newer episodes have higher priority)
        float recencyScore = 1.0f / (i + 1);  // Earlier in vector = more recent
        float score = ep.relevance * 0.7f + recencyScore * 0.3f;
        
        scored.emplace_back(score, i);
    }
    
    // Sort by score
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top episodes
    std::vector<EpisodicMemoryItem> result;
    for (size_t i = 0; i < std::min(count, scored.size()); ++i) {
        result.push_back(episodes_[scored[i].second]);
    }
    
    return result;
}

void EpisodicMemory::replayEpisode(const EpisodicMemoryItem& episode) {
    // Replay episode by setting neuron activations based on stored values
    // This is a placeholder for actual neural integration
    // In a real implementation, this would:
    // 1. Find neurons that were active during this episode
    // 2. Inject appropriate currents to reactivate them
    // 3. Update synaptic weights through replay
    
    // For now, just store for later use
    // Episodes are automatically stored when needed
    
    // Simple replay: the episode is already stored in the list
    // In a real system, this would reactivate neural patterns
    
    // Update any episode references
    for (auto& ep : episodes_) {
        if (ep.timestamp == episode.timestamp && ep.metadata == episode.metadata) {
            // Mark this episode for replay
            ep.relevance = std::min(1.0f, ep.relevance * 1.2f);  // Boost relevance
            break;
        }
    }
}

// Semantic Memory Implementation
SemanticMemory::SemanticMemory()
    : facts_()
{
}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    for (auto& fact : facts_) {
        if (fact.first == key) {
            fact.second = value;
            return;
        }
    }
    facts_.emplace_back(key, value);
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    for (const auto& fact : facts_) {
        if (fact.first == key) {
            return fact.second;
        }
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return std::find_if(facts_.begin(), facts_.end(),
        [key](const auto& fact) { return fact.first == key; }) != facts_.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    return facts_;
}

void SemanticMemory::clear() {
    facts_.clear();
}

// Procedural Memory Implementation
ProceduralMemory::ProceduralMemory()
    : skills_()
{
}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    for (auto& skill : skills_) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            return;
        }
    }
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skills_.push_back(skill);
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    for (auto& skill : skills_) {
        if (skill.name == name) {
            return &skill;
        }
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (auto& skill : skills_) {
        result.push_back(&skill);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    for (auto& skill : skills_) {
        if (skill.name == name) {
            skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void ProceduralMemory::clear() {
    skills_.clear();
}

// Associative Memory Implementation
AssociativeMemory::AssociativeMemory()
    : associations_()
{
}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = strength;
            return;
        }
    }
    associations_.emplace_back(a, b, strength);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    for (const auto& assoc : associations_) {
        if (std::get<0>(assoc) == neuron) {
            result.push_back(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == neuron) {
            result.push_back(std::get<0>(assoc));
        }
    }
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : associations_) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            return std::get<2>(assoc);
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void AssociativeMemory::clear() {
    associations_.clear();
}

void SemanticMemory::clear() {
    facts_.clear();
}

// Procedural Memory Implementation
ProceduralMemory::ProceduralMemory()
    : skills_()
{
}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    for (auto& skill : skills_) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            return;
        }
    }
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skills_.push_back(skill);
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    for (auto& skill : skills_) {
        if (skill.name == name) {
            return &skill;
        }
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (auto& skill : skills_) {
        result.push_back(&skill);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    for (auto& skill : skills_) {
        if (skill.name == name) {
            skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void ProceduralMemory::clear() {
    skills_.clear();
}

// Associative Memory Implementation
AssociativeMemory::AssociativeMemory()
    : associations_()
{
}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = strength;
            return;
        }
    }
    associations_.emplace_back(a, b, strength);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    for (const auto& assoc : associations_) {
        if (std::get<0>(assoc) == neuron) {
            result.push_back(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == neuron) {
            result.push_back(std::get<0>(assoc));
        }
    }
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : associations_) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            return std::get<2>(assoc);
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void AssociativeMemory::clear() {
    associations_.clear();
}

} // namespace nlm
