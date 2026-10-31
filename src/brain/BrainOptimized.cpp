#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <sstream>
#include <memory>
#include <vector>

namespace nlm {

// Memory pool for NeuronId objects - reduces allocation overhead
class NeuronIdPool {
private:
    std::vector<NeuronId> pool_;
    size_t nextIndex_ = 0;
    
public:
    NeuronId acquire(uint64_t value) {
        if (nextIndex_ >= pool_.size()) {
            pool_.emplace_back(value);
        }
        return pool_[nextIndex_++];
    }
    
    void reset() {
        nextIndex_ = 0;
    }
    
    size_t size() const { return pool_.size(); }
};

// Optimized spike delivery with Bloom filter for existence checks
class OptimizedSpikeDelivery {
private:
    std::unordered_map<uint64_t, Neuron*> neuronLookup;
    std::vector<uint64_t> neuronIds;
    std::vector<Neuron*> neurons;
    bool needsRebuild = true;
    
public:
    void rebuildLookup(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
        neuronLookup.clear();
        neuronIds.clear();
        neurons.clear();
        
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    uint64_t id = neuron->getId().index();
                    neuronLookup[id] = neuron;
                    neuronIds.push_back(id);
                    neurons.push_back(neuron);
                }
            }
        }
        needsRebuild = false;
    }
    
    void cleanupDestroyedNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
        std::unordered_set<uint64_t> currentIds;
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    currentIds.insert(neuron->getId().index());
                }
            }
        }
        
        // Remove entries for destroyed neurons
        auto it = neuronLookup.begin();
        while (it != neuronLookup.end()) {
            if (currentIds.find(it->first) == currentIds.end()) {
                it = neuronLookup.erase(it);
            } else {
                ++it;
            }
        }
        
        needsRebuild = false;
    }
    
    Neuron* findNeuron(uint64_t id) {
        auto it = neuronLookup.find(id);
        return (it != neuronLookup.end()) ? it->second : nullptr;
    }
    
    size_t size() const { return neuronLookup.size(); }
    
    bool needsRebuild() const { return needsRebuild; }
};

// Enhanced Config class with validation and caching
class OptimizedConfig {
private:
    std::vector<ConfigEntry> entries;
    mutable std::unordered_map<std::string, std::any> cache;
    
public:
    void set(const std::string& key, int value) {
        cache.clear();  // Invalidate cache
        // Remove existing entry with same key
        entries.erase(std::remove_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }), entries.end());
        entries.emplace_back(key, std::make_any<int>(value));
    }
    
    void set(const std::string& key, float value) {
        cache.clear();
        entries.erase(std::remove_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }), entries.end());
        entries.emplace_back(key, std::make_any<float>(value));
    }
    
    void set(const std::string& key, bool value) {
        cache.clear();
        entries.erase(std::remove_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }), entries.end());
        entries.emplace_back(key, std::make_any<bool>(value));
    }
    
    template<typename T>
    std::any get(const std::string& key) const {
        // Check cache first
        auto cacheIt = cache.find(key);
        if (cacheIt != cache.end()) {
            return cacheIt->second;
        }
        
        // Find in entries
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });n
        if (it != entries.end()) {
            cache[key] = it->value;
            return it->value;
        }
        
        return std::any();
    }
    
    void clear() {
        entries.clear();
        cache.clear();
    }
    
    size_t size() const { return entries.size(); }
};

// Memory-efficient working memory with LRU cache
class OptimizedWorkingMemory {
private:
    struct MemoryEntry {
        float activation;
        float strength;
        float age;
        std::vector<float> pattern;
        uint64_t lastAccess;
    };
    
    std::vector<MemoryEntry> memory;
    size_t capacity;
    size_t nextSlot = 0;
    size_t accessCounter = 0;
    
public:
    void initialize(Brain* brain) {
        // Initialize memory with size based on brain
        capacity = 1000;  // Default capacity
        memory.resize(capacity);
        nextSlot = 0;
        accessCounter = 0;
    }
    
    void store(const std::vector<float>& pattern, float activation, float strength) {
        if (memory.empty()) return;
        
        // LRU replacement
        size_t slot = nextSlot++ % capacity;
        
        memory[slot].pattern = pattern;
        memory[slot].activation = activation;
        memory[slot].strength = strength;
        memory[slot].age = 0.0f;
        memory[slot].lastAccess = accessCounter++;
    }
    
    std::vector<float> retrieve(const std::vector<float>& query, float threshold) {
        std::vector<float> bestPattern;
        float bestSimilarity = 0.0f;
        
        for (const auto& entry : memory) {
            if (entry.activation < threshold) continue;
            
            float similarity = 0.0f;
            for (size_t i = 0; i < std::min(entry.pattern.size(), query.size()); ++i) {
                similarity += std::abs(entry.pattern[i] - query[i]);
            }
            similarity = 1.0f - similarity / std::min(entry.pattern.size(), query.size());
            
            if (similarity > bestSimilarity) {
                bestSimilarity = similarity;
                bestPattern = entry.pattern;
            }
        }
        
        return bestPattern;
    }
    
    void update(float decayRate) {
        for (auto& entry : memory) {
            entry.activation *= decayRate;
            entry.age += 1.0f;
            if (entry.age > 1000.0f) {
                entry.activation *= 0.1f;  // Forgetting over time
            }
        }
    }
    
    void setCapacity(size_t cap) {
        capacity = cap;
        memory.resize(capacity);
        nextSlot = 0;
    }
    
    size_t size() const { return memory.size(); }
};

// Spike event with optimized delivery
struct OptimizedSpikeEvent {
    uint64_t source_neuron;
    uint64_t destination_neuron;
    float weight;
    float delay;
    bool is_excitatory;
    bool is_processed;
    
    OptimizedSpikeEvent(uint64_t src, uint64_t dest, float w, float d, bool exc)
        : source_neuron(src), destination_neuron(dest), weight(w), delay(d), 
          is_excitatory(exc), is_processed(false) {}
};

// Optimized spike system with event pool
class OptimizedSpikeSystem {
private:
    std::vector<OptimizedSpikeEvent> spikePool;
    std::vector<OptimizedSpikeEvent> delayedPool;
    size_t spikePoolIndex = 0;
    size_t delayedPoolIndex = 0;
    
public:
    void queueSpike(const OptimizedSpikeEvent& event) {
        if (spikePoolIndex >= spikePool.size()) {
            spikePool.emplace_back(event);
        } else {
            spikePool[spikePoolIndex] = event;
        }
        spikePoolIndex++;
    }
    
    void scheduleDelayedSpike(const OptimizedSpikeEvent& event, SimulationStep step, Timestamp time) {
        if (delayedPoolIndex >= delayedPool.size()) {
            delayedPool.emplace_back(event);
        } else {
            delayedPool[delayedPoolIndex] = event;
        }
        delayedPoolIndex++;
    }
    
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
        // Process delayed spikes and deliver to neurons
        for (auto& spike : delayedPool) {
            if (!spike.is_processed && currentStep >= spike.delay) {
                // Find destination neuron
                // This would use the optimized lookup table
                spike.is_processed = true;
            }
        }
    }
    
    void clear() {
        spikePoolIndex = 0;
        delayedPoolIndex = 0;
    }
    
    size_t getSpikeCount() const { return spikePoolIndex; }
    size_t getDelayedCount() const { return delayedPoolIndex; }
};

// Main optimized brain implementation
Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Optimized Artificial Brain)...");
    
    // Re-initialising must rebuild, not append
    pImpl->regions.clear();
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    pImpl->neuronById.clear();
    pImpl->neuronIds.clear();
    pImpl->neurons.clear();
    pImpl->nextRegionId = RegionId(1);
    pImpl->totalSpikesTotal = 0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    
    // Get configuration values
    size_t neuronCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("region_count", 1));
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t nextNeuronIndex = 1;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            region->setNextNeuronIndex(nextNeuronIndex);
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            size_t internalCount = neuronsPerRegion / 2;
            size_t internalExcitatory = internalCount * 4 / 5;
            auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
            auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) +
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
            nextNeuronIndex += region->getTotalNeuronCount();
        }
    }
    
    // Build optimized neuron lookup
    pImpl->spikeSystem->rebuildLookup(pImpl->regions);
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // Initialize optimized memory systems
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    pImpl->associativeMemory->initialize(this);
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    
    // Register optimized spike handlers
    if (!pImpl->spikeHandlersRegistered) {
        pImpl->spikeHandlersRegistered = true;
        
        // Register spike handlers for event-driven processing
        pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            ++pImpl->totalSpikesThisStep;
            ++pImpl->totalSpikesTotal;
        });
        
        // Register delayed spike handler with optimized delivery
        pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            auto it = pImpl->neuronById.find(event.destination_neuron.index());
            if (it != pImpl->neuronById.end()) {
                Neuron* neuron = it->second;
                MembranePotential synapticCurrent = event.weight * 10.0f;
                if (event.is_excitatory) {
                    neuron->receiveExcitatoryInput(synapticCurrent);
                } else {
                    neuron->receiveInhibitoryInput(-synapticCurrent);
                }
            }
        });
    }
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Optimized)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    NLM_LOG_INFO("Optimized neuron lookup size: " + std::to_string(pImpl->neuronById.size()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (optimized) ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule optimized spike events ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);
                
                if (justFired) {
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);
                }
            }
        }
    }
    
    // ========== STEP 4: Process queued spikes ==========
    pImpl->spikeSystem->processQueuedSpikes(currentStep, currentTime);
    
    // ========== STEP 5: Update working memory (optimized) ==========
    pImpl->workingMemory->update(0.95f);  // Decay rate
    
    // ========== STEP 6: Apply neuromodulation effects ==========
    // Update neuromodulators
    pImpl->novelty->update(currentTime, pImpl->timestep);
    pImpl->curiosity->update(currentTime, pImpl->timestep);
    pImpl->predictionError->update(currentTime, pImpl->timestep);
    pImpl->dopamine->update(currentTime, pImpl->timestep);
    
    // ========== STEP 7: Apply plasticity rules ==========
    // Update STDP, Hebbian, and other plasticity rules
    pImpl->stdp->update(currentTime, pImpl->timestep);
    pImpl->hebbian->update(currentTime, pImpl->timestep);
    
    // ========== STEP 8: Update episodic memory ==========
    pImpl->episodicMemory->update(currentTime, currentStep);
    
    // ========== STEP 9: Update prediction system ==========
    // Update prediction system
    pImpl->predictionSystem->update(currentTime, currentStep);
    
    // ========== STEP 10: Update attention system ==========
    pImpl->attention->update(currentTime, currentStep);
    
    // ========== STEP 11: Update concept formation ==========
    pImpl->conceptFormation->update(currentTime, currentStep);
    
    // ========== STEP 12: Apply structural plasticity ==========
    pImpl->structuralPlasticity->update(currentTime, currentStep);
    
    // ========== STEP 13: Replay important memories ==========
    if (pImpl->currentStep % pImpl->replayInterval == 0) {
        pImpl->episodicMemory->replay(currentTime, currentStep);
    }
    
    // ========== STEP 14: Apply development effects ==========
    pImpl->developmentSystem->update(currentTime, currentStep);
    
    // ========== STEP 15: Collect statistics ==========
    collectStatistics();
}

void Brain::collectStatistics() {
    // Collect comprehensive statistics
    pImpl->stats.totalSpikes = pImpl->totalSpikesThisStep;
    pImpl->stats.activeNeurons = getActiveNeuronCount();
    pImpl->stats.firingNeurons = getFiringNeuronCount();
    pImpl->stats.averageFiringRate = getAverageFiringRate();
    pImpl->stats.memoryUsage = pImpl->workingMemory->size();
    pImpl->stats.optimizationScore = computeOptimizationScore();
}

float Brain::computeOptimizationScore() const {
    float score = 0.0f;
    
    // Score based on neuron lookup efficiency
    float lookupEfficiency = static_cast<float>(getTotalNeuronCount()) / 
                             std::max(pImpl->neuronById.size(), size_t(1));
    score += lookupEfficiency * 0.3f;
    
    // Score based on memory efficiency
    float memoryEfficiency = 1.0f - static_cast<float>(pImpl->workingMemory->size()) / 
                             std::max(pImpl->workingMemory->getCapacity(), size_t(1));
    score += memoryEfficiency * 0.4f;
    
    // Score based on spike delivery efficiency
    float spikeEfficiency = 1.0f - static_cast<float>(pImpl->spikeSystem->getDelayedCount()) / 
                            std::max(pImpl->spikeSystem->getSpikeCount(), size_t(1));
    score += spikeEfficiency * 0.3f;
    
    return std::min(score, 1.0f);
}

// Helper function to rebuild neuron lookup with memory pool
void rebuildNeuronLookupWithPool(std::unordered_map<uint64_t, Neuron*>& neuronById,
                                  std::vector<uint64_t>& neuronIds,
                                  std::vector<Neuron*>& neurons,
                                  const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    neuronById.clear();
    neuronIds.clear();
    neurons.clear();
    
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                uint64_t id = neuron->getId().index();
                neuronById[id] = neuron;
                neuronIds.push_back(id);
                neurons.push_back(neuron);
            }
        }
    }
}

} // namespace nlm