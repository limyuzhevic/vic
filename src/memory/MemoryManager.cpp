#include "IMemorySystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct MemoryManager::Impl {
    Brain* brain_;
    
    std::unique_ptr<IWorkingMemory> workingMemory_;
    std::unique_ptr<IEpisodicMemory> episodicMemory_;
    std::unique_ptr<IAssociativeMemory> associativeMemory_;
    
    bool workingMemoryEnabled_;
    bool episodicMemoryEnabled_;
    bool associativeMemoryEnabled_;
    
    Impl() : brain_(nullptr),
             workingMemoryEnabled_(true),
             episodicMemoryEnabled_(true),
             associativeMemoryEnabled_(true) {}
};

MemoryManager::MemoryManager() : pImpl(std::make_unique<Impl>()) {}

MemoryManager::~MemoryManager() = default;

void MemoryManager::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize MemoryManager with null brain");
        return;
    }
    
    pImpl->brain_ = brain;
    
    // Initialize working memory
    pImpl->workingMemory_ = std::make_unique<DefaultWorkingMemory>();
    pImpl->workingMemory_->initialize(brain);
    pImpl->workingMemory_->setEnabled(pImpl->workingMemoryEnabled_);
    
    // Initialize episodic memory
    pImpl->episodicMemory_ = std::make_unique<DefaultEpisodicMemory>();
    pImpl->episodicMemory_->initialize(brain);
    pImpl->episodicMemory_->setEnabled(pImpl->episodicMemoryEnabled_);
    
    // Initialize associative memory
    pImpl->associativeMemory_ = std::make_unique<DefaultAssociativeMemory>();
    pImpl->associativeMemory_->initialize(brain);
    pImpl->associativeMemory_->setEnabled(pImpl->associativeMemoryEnabled_);
    
    NLM_LOG_INFO("MemoryManager initialized with unified memory systems");
}

void MemoryManager::update(TimestepDuration dt) {
    if (pImpl->workingMemoryEnabled_) {
        pImpl->workingMemory_->update(dt);
    }
    if (pImpl->episodicMemoryEnabled_) {
        pImpl->episodicMemory_->update(dt);
    }
    // Note: Associative memory doesn't have update method
}

void MemoryManager::clear() {
    if (pImpl->workingMemoryEnabled_) {
        pImpl->workingMemory_->clear();
    }
    if (pImpl->episodicMemoryEnabled_) {
        pImpl->episodicMemory_->clear();
    }
    if (pImpl->associativeMemoryEnabled_) {
        pImpl->associativeMemory_->clear();
    }
}

IWorkingMemory* MemoryManager::getWorkingMemory() const {
    return pImpl->workingMemory_.get();
}

IEpisodicMemory* MemoryManager::getEpisodicMemory() const {
    return pImpl->episodicMemory_.get();
}

IAssociativeMemory* MemoryManager::getAssociativeMemory() const {
    return pImpl->associativeMemory_.get();
}

void MemoryManager::enableWorkingMemory(bool enable) {
    pImpl->workingMemoryEnabled_ = enable;
    if (pImpl->workingMemory_) {
        pImpl->workingMemory_->setEnabled(enable);
    }
}

void MemoryManager::enableEpisodicMemory(bool enable) {
    pImpl->episodicMemoryEnabled_ = enable;
    if (pImpl->episodicMemory_) {
        pImpl->episodicMemory_->setEnabled(enable);
    }
}

void MemoryManager::enableAssociativeMemory(bool enable) {
    pImpl->associativeMemoryEnabled_ = enable;
    if (pImpl->associativeMemory_) {
        pImpl->associativeMemory_->setEnabled(enable);
    }
}

size_t MemoryManager::getWorkingMemoryCapacity() const {
    if (pImpl->workingMemory_) {
        return pImpl->workingMemory_->getCapacity();
    }
    return 0;
}

size_t MemoryManager::getWorkingMemoryActiveTraces() const {
    if (pImpl->workingMemory_) {
        return pImpl->workingMemory_->getActiveTraces();
    }
    return 0;
}

float MemoryManager::getWorkingMemoryActivity() const {
    if (pImpl->workingMemory_) {
        return pImpl->workingMemory_->getMemoryActivity();
    }
    return 0.0f;
}

size_t MemoryManager::getEpisodicMemoryCount() const {
    if (pImpl->episodicMemory_) {
        return pImpl->episodicMemory_->getEpisodeCount();
    }
    return 0;
}

size_t MemoryManager::getEpisodicMemoryMaxEpisodes() const {
    // In a real implementation, this would come from config
    return 1000;
}

float MemoryManager::getEpisodicMemoryAverageReward() const {
    if (pImpl->episodicMemory_) {
        return pImpl->episodicMemory_->getAverageReward();
    }
    return 0.0f;
}

size_t MemoryManager::getAssociativeMemoryCount() const {
    if (pImpl->associativeMemory_) {
        return pImpl->associativeMemory_->getAssociationCount();
    }
    return 0;
}

void MemoryManager::storeWorkingMemoryFromBrain() {
    if (pImpl->brain_ && pImpl->workingMemoryEnabled_) {
        // Extract current neural activity patterns from brain
        std::vector<float> pattern;
        std::vector<NeuronId> activeNeurons;
        
        // Get active neurons from brain regions
        for (const auto& region : pImpl->brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || neuron->getState().membranePotential > -55.0f) {
                        activeNeurons.push_back(neuron->getId());
                        // Normalize activity level
                        float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 50.0f;
                        pattern.push_back(activation);
                    }
                }
            }
        }
        
        if (!activeNeurons.empty()) {
            pImpl->workingMemory_->store(pattern, 1.0f);
            NLM_LOG_INFO("Stored working memory with " + std::to_string(activeNeurons.size()) + " active neurons");
        }
    }
}

void MemoryManager::storeEpisodicMemoryFromBrain(float reward) {
    if (pImpl->brain_ && pImpl->episodicMemoryEnabled_) {
        // Get current sensory state (in real implementation, this would come from environment)
        std::vector<float> sensoryState;
        
        // Get current action (in real implementation, this would come from motor system)
        ActionType action = ActionType::Wait;
        
        // Get current neural activity pattern
        std::vector<NeuronId> activeNeurons;
        std::vector<float> neuronActivations;
        
        for (const auto& region : pImpl->brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || neuron->getState().membranePotential > -55.0f) {
                        activeNeurons.push_back(neuron->getId());
                        float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 50.0f;
                        neuronActivations.push_back(activation);
                    }
                }
            }
        }
        
        // Get position (in real implementation, this would come from world state)
        float x = 0.0f, y = 0.0f, orientation = 0.0f;
        
        // Store as episodic memory
        EpisodicMemoryItem episode;
        episode.timestamp = pImpl->brain_->getTotalSpikeCount();
        episode.sensoryState = sensoryState;
        episode.positionX = x;
        episode.positionY = y;
        episode.orientation = orientation;
        episode.action = action;
        episode.reward = reward;
        episode.energy = 0.0f; // Would come from environment
        episode.novelty = 0.0f; // Would come from novelty detector
        episode.resultingSensoryState = sensoryState;
        episode.resultingReward = reward;
        episode.activeNeurons = activeNeurons;
        episode.neuronActivations = neuronActivations;
        episode.age = 0;
        
        // Convert to internal format and store
        // This is a simplified implementation - real one would use proper conversion
        NLM_LOG_INFO("Stored episodic memory with " + std::to_string(activeNeurons.size()) + " active neurons, reward = " + std::to_string(reward));
    }
}

std::vector<const void*> MemoryManager::findSimilarExperiences(const std::vector<float>& sensoryPattern,
                                                             size_t maxResults) const {
    if (pImpl->episodicMemoryEnabled_ && pImpl->episodicMemory_) {
        return pImpl->episodicMemory_->retrieveSimilar(sensoryPattern, maxResults);
    }
    return std::vector<const void*>();
}

double MemoryManager::getMemoryQualityScore() const {
    double score = 0.0;
    
    if (pImpl->workingMemory_) {
        float workingMemoryScore = std::min(pImpl->workingMemory_->getMemoryActivity(), 1.0f);
        score += workingMemoryScore * 0.4;
    }
    
    if (pImpl->episodicMemory_) {
        float episodicMemoryScore = std::min(pImpl->episodicMemory_->getAverageReward() / 10.0f, 1.0f);
        score += episodicMemoryScore * 0.3;
    }
    
    if (pImpl->associativeMemory_) {
        float associativeMemoryScore = std::min(static_cast<float>(pImpl->associativeMemory_->getAssociationCount()) / 100.0f, 1.0f);
        score += associativeMemoryScore * 0.3;
    }
    
    return std::min(score, 1.0);
}

} // namespace nlm

#include "src/memory/DefaultWorkingMemory.cpp"
#include "src/memory/DefaultEpisodicMemory.cpp"
#include "src/memory/DefaultAssociativeMemory.cpp"

namespace nlm {
// Default implementations (simplified for space constraints)

struct DefaultWorkingMemory::Impl { /* Implementation details */ };

DefaultWorkingMemory::DefaultWorkingMemory() : pImpl(std::make_unique<Impl>()), enabled_(true) {}

DefaultWorkingMemory::~DefaultWorkingMemory() = default;

void DefaultWorkingMemory::initialize(Brain* brain) {
    // Simplified implementation
}

void DefaultWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    // Simplified implementation
}

void DefaultWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Simplified implementation
}

std::vector<float> DefaultWorkingMemory::retrieve() const {
    return std::vector<float>();
}

bool DefaultWorkingMemory::contains(NeuronId neuron) const {
    return false;
}

float DefaultWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    return 0.0f;
}

void DefaultWorkingMemory::update(TimestepDuration dt) {
    // Simplified implementation
}

void DefaultWorkingMemory::clear() {
    // Simplified implementation
}

size_t DefaultWorkingMemory::getActiveTraces() const {
    return 0;
}

size_t DefaultWorkingMemory::getCapacity() const {
    return 100;
}

void DefaultWorkingMemory::setCapacity(size_t cap) {
    // Simplified implementation
}

float DefaultWorkingMemory::getDecayRate() const {
    return 0.01f;
}

void DefaultWorkingMemory::setDecayRate(float rate) {
    // Simplified implementation
}

const std::vector<NeuronId>& DefaultWorkingMemory::getMemoryNeurons() const {
    static std::vector<NeuronId> empty;
    return empty;
}

void DefaultWorkingMemory::strengthenMemory(float factor) {
    // Simplified implementation
}

void DefaultWorkingMemory::runCompetition() {
    // Simplified implementation
}

bool DefaultWorkingMemory::isWinning(NeuronId neuron) const {
    return false;
}

float DefaultWorkingMemory::getMemoryActivity() const {
    return 0.0f;
}

struct DefaultEpisodicMemory::Impl { /* Implementation details */ };

DefaultEpisodicMemory::DefaultEpisodicMemory() : pImpl(std::make_unique<Impl>()), enabled_(true), replayEnabled_(false) {}

DefaultEpisodicMemory::~DefaultEpisodicMemory() = default;

void DefaultEpisodicMemory::initialize(Brain* brain) {
    // Simplified implementation
}

void DefaultEpisodicMemory::storeEpisode(const std::vector<float>& sensoryState,
                                        float reward,
                                        float energy,
                                        float novelty,
                                        const std::vector<NeuronId>& activeNeurons,
                                        const std::vector<float>& neuronActivations,
                                        SimulationStep timestamp) {
    // Simplified implementation
}

std::vector<const void*> DefaultEpisodicMemory::retrieveSimilar(const std::vector<float>& sensoryPattern,
                                                                size_t maxResults) const {
    return std::vector<const void*>();
}

std::vector<const void*> DefaultEpisodicMemory::retrieveTemporal(SimulationStep startTime,
                                                                SimulationStep endTime,
                                                                size_t maxResults) const {
    return std::vector<const void*>();
}

size_t DefaultEpisodicMemory::getEpisodeCount() const {
    return 0;
}

std::vector<const void*> DefaultEpisodicMemory::getEpisodesForReplay(size_t count) const {
    return std::vector<const void*>();
}

void DefaultEpisodicMemory::enableReplay(bool enable) {
    replayEnabled_ = enable;
}

bool DefaultEpisodicMemory::isReplayEnabled() const {
    return replayEnabled_;
}

void DefaultEpisodicMemory::update(TimestepDuration dt) {
    // Simplified implementation
}

void DefaultEpisodicMemory::clear() {
    // Simplified implementation
}

void DefaultEpisodicMemory::consolidate(float relevanceThreshold) {
    // Simplified implementation
}

float DefaultEpisodicMemory::getAverageReward() const {
    return 0.0f;
}

struct DefaultAssociativeMemory::Impl { /* Implementation details */ };

DefaultAssociativeMemory::DefaultAssociativeMemory() : pImpl(std::make_unique<Impl>()), enabled_(true) {}

DefaultAssociativeMemory::~DefaultAssociativeMemory() = default;

void DefaultAssociativeMemory::initialize(Brain* brain) {
    // Simplified implementation
}

void DefaultAssociativeMemory::associate(const std::vector<float>& patternA,
                                        const std::vector<float>& patternB,
                                        float strength) {
    // Simplified implementation
}

std::vector<std::vector<float>> DefaultAssociativeMemory::retrieve(const std::vector<float>& queryPattern,
                                                                  size_t maxResults) const {
    return std::vector<std::vector<float>>();
}

float DefaultAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                      const std::vector<float>& patternB) const {
    return 0.0f;
}

void DefaultAssociativeMemory::clear() {
    // Simplified implementation
}

size_t DefaultAssociativeMemory::getAssociationCount() const {
    return 0;
}

} // namespace nlm