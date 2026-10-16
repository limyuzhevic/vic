#include "Hippocampus.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include <algorithm>
#include <random>
#include <cmath>
#include <unordered_map>

namespace nlm {

// Global random number generator for hippocampus
static std::mt19937 rng(std::random_device{}());
static std::uniform_real_distribution<float> uniformDist(0.0f, 1.0f);
static std::normal_distribution<float> normalDist(0.0f, 1.0f);

// Hippocampus implementation

Hippocampus::Hippocampus() {
    // Initialize default parameters
    environmentWidth_ = 100.0f;
    environmentHeight_ = 100.0f;
    
    placeCellLearningRate_ = 0.05f;
    gridCellLearningRate_ = 0.03f;
    memoryConsolidationRate_ = 0.02f;
    
    placeCellsEnabled_ = true;
    gridCellsEnabled_ = true;
    memoryEncodingEnabled_ = true;
    memoryReplayEnabled_ = true;
    
    brain_ = nullptr;
    world_ = nullptr;
    
    replayIndex_ = 0;
}

Hippocampus::~Hippocampus() = default;

void Hippocampus::initialize(const SimpleWorld& world, Brain* brain) {
    world_ = &world;
    brain_ = brain;
    
    // Set up place cells
    placeCells_.clear();
    placeCells_.reserve(100);  // Start with 100 place cells
    
    // Set up grid cells
    gridCells_.clear();
    gridCells_.reserve(20);  // Start with 20 grid cells
    
    // Initialize place cells with random positions
    float fieldRadius = environmentWidth_ * 0.15f;  // 15% of environment width
    float firingRateMean = 5.0f;  // Hz
    float firingRateStd = 2.0f;
    
    for (size_t i = 0; i < placeCells_.size(); ++i) {
        PlaceCell cell;
        cell.id = NeuronId(i + 1);  // Use NeuronId for neuron reference
        cell.x = uniformDist(rng) * environmentWidth_;
        cell.y = uniformDist(rng) * environmentHeight_;
        cell.fieldRadius = fieldRadius;
        cell.familiarity = uniformDist(rng);
        cell.firingRate = normalDist(rng) * firingRateStd + firingRateMean;
        cell.firingRate = std::max(0.0f, std::min(10.0f, cell.firingRate));
        
        // Initialize neural pattern
        cell.pattern.resize(256, 0.0f);  // 256-neuron pattern
        for (float& value : cell.pattern) {
            value = uniformDist(rng) * 0.1f;
        }
        
        placeCells_.push_back(cell);
    }
    
    // Initialize grid cells with different scales
    std::vector<float> scales = {1.0f, 2.0f, 4.0f, 8.0f, 16.0f};
    
    for (size_t i = 0; i < gridCells_.size(); ++i) {
        GridCell cell;
        cell.id = NeuronId(i + 101);  // Different NeuronId range
        cell.scale = scales[i % scales.size()];
        cell.orientation = uniformDist(rng) * 2.0f * M_PI;  // 0 to 2π
        cell.periodicity = environmentWidth_ / cell.scale;  // Distance between fields
        cell.currentFiring = 0.0f;
        cell.coherence = uniformDist(rng);
        
        // Initialize grid fields
        cell.fields.resize(4);  // 4 fields per grid cell
        for (auto& field : cell.fields) {
            field.centerX = uniformDist(rng) * environmentWidth_;
            field.centerY = uniformDist(rng) * environmentHeight_;
            field.orientation = uniformDist(rng) * 2.0f * M_PI;
            field.periodicity = cell.periodicity;
            field.strength = uniformDist(rng) * 0.5f + 0.5f;
        }
        
        gridCells_.push_back(cell);
    }
    
    // Log initialization
    NLM_LOG_INFO("Hippocampus initialized with " + std::to_string(placeCells_.size()) + " place cells and " + 
                std::to_string(gridCells_.size()) + " grid cells");
}

void Hippocampus::update(TimestepDuration dt) {
    if (!brain_ || !world_) return;
    
    // Update current spatial information
    currentSpatialInfo_ = getCurrentSpatialInfo();
    
    // Update place cells
    if (placeCellsEnabled_) {
        updatePlaceCells(dt);
    }
    
    // Update grid cells
    if (gridCellsEnabled_) {
        updateGridCells(dt);
    }
    
    // Update episodic memory
    if (memoryEncodingEnabled_ || memoryReplayEnabled_) {
        updateEpisodicMemory(dt);
    }
    
    // Handle memory replay
    if (memoryReplayEnabled_ && shouldReplayMemories()) {
        replayScheduledMemories();
    }
}

void Hippocampus::processSensoryInput(const SensoryPercept& percept) {
    // Update hippocampal representation based on sensory input
    currentSpatialInfo_.vision = percept.getVision();
    currentSpatialInfo_.touch = percept.getTouch();
    currentSpatialInfo_.internal = percept.getInternal();
    
    // Trigger place cell updating
    if (placeCellsEnabled_) {
        triggerPlaceCellUpdate();
    }
}

void Hippocampus::recordEpisode(const std::string& context) {
    if (!memoryEncodingEnabled_) return;
    
    encodeEpisodicMemory(context);
    scheduleMemoryReplay();
}

EpisodicMemory* Hippocampus::retrieveMemory(const std::string& context,
                                          float similarityThreshold) {
    // Find the most similar episode based on context and neural patterns
    EpisodicMemory* bestMatch = nullptr;
    float bestSimilarity = 0.0f;
    
    for (auto& memory : episodicMemories_) {
        float similarity = computeMemorySimilarity(memory, context);
        
        if (similarity > bestSimilarity && similarity > similarityThreshold) {
            bestSimilarity = similarity;
            bestMatch = &memory;
        }
    }
    
    return bestMatch;
}

void Hippocampus::replayEpisodes(size_t episodeCount) {
    if (!memoryReplayEnabled_) return;
    
    std::vector<const EpisodicMemory*> episodesToReplay;
    
    if (episodicMemories_.size() > episodeCount) {
        // Select episodes weighted by coherence
        std::vector<float> weights;
        for (const auto& memory : episodicMemories_) {
            weights.push_back(memory.coherence);
        }
        
        episodesToReplay = selectEpisodes(episodesToReplay, weights, episodeCount);
    } else {
        // Use all available episodes
        for (const auto& memory : episodicMemories_) {
            episodesToReplay.push_back(&memory);
        }
    }
    
    // Replay selected episodes
    for (const EpisodicMemory* episode : episodesToReplay) {
        replayEpisode(*episode);
    }
}

SpatialInfo Hippocampus::getCurrentSpatialInfo() const {
    SpatialInfo info;
    
    if (!world_) {
        return info;
    }
    
    const auto& agentBody = world_->getAgentBody();
    
    info.x = agentBody.x;
    info.y = agentBody.y;
    info.orientation = agentBody.orientation;
    info.velocityX = agentBody.velocityX;
    info.velocityY = agentBody.velocityY;
    info.angularVelocity = agentBody.angularVelocity;
    info.energy = agentBody.energy;
    
    // Add sensory context
    const auto& percept = world_->getSensoryPercept();
    info.vision = percept.getVision();
    info.touch = percept.getTouch();
    
    return info;
}

void Hippocampus::setEnvironmentSize(float width, float height) {
    environmentWidth_ = width;
    environmentHeight_ = height;
    
    // Update place cell field radii based on new environment size
    float fieldRadius = environmentWidth_ * 0.15f;
    for (auto& cell : placeCells_) {
        cell.fieldRadius = fieldRadius;
    }
    
    // Update grid cell periodicities
    for (auto& cell : gridCells_) {
        cell.periodicity = environmentWidth_ / cell.scale;
    }
}

void Hippocampus::configurePlaceCells(size_t placeCellCount,
                                    const std::vector<float>& firingRates) {
    placeCells_.clear();
    placeCells_.reserve(placeCellCount);
    
    float fieldRadius = environmentWidth_ * 0.15f;
    
    for (size_t i = 0; i < placeCellCount; ++i) {
        PlaceCell cell;
        cell.id = NeuronId(i + 1);
        cell.x = uniformDist(rng) * environmentWidth_;
        cell.y = uniformDist(rng) * environmentHeight_;
        cell.fieldRadius = fieldRadius;
        cell.familiarity = uniformDist(rng);
        cell.firingRate = firingRates[i % firingRates.size()];
        
        // Initialize neural pattern
        cell.pattern.resize(256, 0.0f);
        for (float& value : cell.pattern) {
            value = uniformDist(rng) * 0.1f;
        }
        
        placeCells_.push_back(cell);
    }
}

void Hippocampus::configureGridCells(size_t gridCellCount,
                                    const std::vector<float>& scales) {
    gridCells_.clear();
    gridCells_.reserve(gridCellCount);
    
    for (size_t i = 0; i < gridCellCount; ++i) {
        GridCell cell;
        cell.id = NeuronId(100 + i);  // Different NeuronId range
        cell.scale = scales[i % scales.size()];
        cell.orientation = uniformDist(rng) * 2.0f * M_PI;
        cell.periodicity = environmentWidth_ / cell.scale;
        cell.currentFiring = 0.0f;
        cell.coherence = uniformDist(rng);
        
        // Initialize grid fields
        cell.fields.resize(4);
        for (auto& field : cell.fields) {
            field.centerX = uniformDist(rng) * environmentWidth_;
            field.centerY = uniformDist(rng) * environmentHeight_;
            field.orientation = uniformDist(rng) * 2.0f * M_PI;
            field.periodicity = cell.periodicity;
            field.strength = uniformDist(rng) * 0.5f + 0.5f;
        }
        
        gridCells_.push_back(cell);
    }
}

void Hippocampus::updatePlaceCells(TimestepDuration dt) {
    // Update place cell firing based on current position
    for (auto& cell : placeCells_) {
        float oldFiring = cell.firingRate;
        float newFiring = computePlaceCellActivation(cell, currentSpatialInfo_);
        
        // Apply familiarity factor
        newFiring *= cell.familiarity;
        
        // Apply learning rule
        if (newFiring > oldFiring) {
            // Increase firing rate for good matches
            cell.firingRate = oldFiring + placeCellLearningRate_ * (newFiring - oldFiring);
            cell.familiarity = oldFiring * 0.9f + newFiring * 0.1f;
        } else {
            // Decay firing rate for poor matches
            cell.firingRate = std::max(0.0f, newFiring * 0.95f);
            cell.familiarity *= 0.98f;
        }
        
        // Update neural pattern
        updatePlaceCellPattern(cell);
    }
    
    updatePlaceCellConnections();
}

void Hippocampus::updateGridCells(TimestepDuration dt) {
    // Update grid cell firing
    for (auto& cell : gridCells_) {
        float oldFiring = cell.currentFiring;
        float newFiring = computeGridCellActivation(cell, currentSpatialInfo_);
        
        // Grid cells maintain stable firing patterns
        cell.currentFiring = newFiring;
        
        // Update coherence
        cell.coherence = oldFiring * 0.9f + newFiring * 0.1f;
    }
    
    updateGridCellConnections();
}

void Hippocampus::updateEpisodicMemory(TimestepDuration dt) {
    // Process short-term memory into episodic memories
    if (shouldConsolidateMemory()) {
        consolidateMemories();
    }
    
    // Update existing episodic memories
    for (auto& memory : episodicMemories_) {
        updateEpisodicMemoryConsolidation(memory, dt);
    }
}

void Hippocampus::updatePlaceCellConnections() {
    // Implement associative learning for place cells
    for (size_t i = 0; i < placeCells_.size(); ++i) {
        for (size_t j = i + 1; j < placeCells_.size(); ++j) {
            const PlaceCell& cell1 = placeCells_[i];
            const PlaceCell& cell2 = placeCells_[j];
            
            float coActivation = computeCoActivation(cell1, cell2);
            
            if (coActivation > 0.01f) {
                strengthenPlaceCellConnection(i, j, coActivation);
            }
        }
    }
}

void Hippocampus::updateGridCellConnections() {
    // Maintain stable grid cell connectivity
    for (auto& cell : gridCells_) {
        establishGridPeriodicity(cell);
        updateGridCellOrientation(cell);
        updateGridCellScale(cell);
    }
}

void Hippocampus::updatePlaceCellPattern(PlaceCell& cell) {
    // Update neural pattern based on firing rate
    float patternStrength = cell.firingRate / 10.0f;
    
    for (float& value : cell.pattern) {
        value = (1.0f - 0.05f) * value + 0.05f * patternStrength * (uniformDist(rng) * 0.5f + 0.5f);
    }
    
    // Store pattern in working memory if encoding is enabled
    if (memoryEncodingEnabled_) {
        encodePlaceCellMemory(cell);
    }
}

void Hippocampus::establishGridPeriodicity(GridCell& cell) {
    // Establish periodic firing pattern
    float phaseShift = uniformDist(rng) * 2.0f * M_PI;
    
    for (auto& field : cell.fields) {
        float distance = computeDistance(field.centerX, field.centerY, 
                                        currentSpatialInfo_.x, currentSpatialInfo_.y);
        float phase = 2.0f * M_PI * distance / cell.periodicity + phaseShift;
        
        float fieldActivation = std::cos(phase);
        fieldActivation = std::max(0.0f, fieldActivation);
        
        // Update field strength based on activation
        field.strength = field.strength * 0.9f + fieldActivation * 0.1f;
    }
}

void Hippocampus::updateGridCellOrientation(GridCell& cell) {
    // Update orientation based on movement experience
    float movementDirection = std::atan2(currentSpatialInfo_.velocityY, 
                                       currentSpatialInfo_.velocityX);
    
    // Adjust cell orientation toward movement direction
    float orientationDiff = std::fmod(movementDirection - cell.orientation + M_PI, 2.0f * M_PI) - M_PI;
    cell.orientation += gridCellLearningRate_ * orientationDiff;
}

void Hippocampus::updateGridCellScale(GridCell& cell) {
    // Update scale based on environment familiarity
    float familiarity = computeSpatialFamiliarity();
    
    // Adjust scale toward optimal value based on familiarity
    float targetScale = 2.0f + familiarity * 6.0f;  // Range 2.0 to 8.0
    cell.scale += gridCellLearningRate_ * (targetScale - cell.scale);
    cell.scale = std::max(1.0f, std::min(16.0f, cell.scale));
    
    // Update periodicity
    cell.periodicity = environmentWidth_ / cell.scale;
}

void Hippocampus::updatePlaceCellConnections() {
    // Implement place cell associative learning
    for (size_t i = 0; i < placeCells_.size(); ++i) {
        for (size_t j = i + 1; j < placeCells_.size(); ++j) {
            const PlaceCell& cell1 = placeCells_[i];
            const PlaceCell& cell2 = placeCells_[j];
            
            float coActivation = computeCoActivation(cell1, cell2);
            
            if (coActivation > 0.01f) {
                strengthenPlaceCellConnection(i, j, coActivation);
            }
        }
    }
}

void Hippocampus::strengthenPlaceCellConnection(size_t cellIdx1, size_t cellIdx2, float strength) {
    // Strengthen connections between place cells that co-fire
    PlaceCell& cell1 = placeCells_[cellIdx1];
    PlaceCell& cell2 = placeCells_[cellIdx2];
    
    // Update connection weights
    float oldWeight = cell1.connectionWeights[cellIdx2];
    float newWeight = oldWeight + placeCellLearningRate_ * (strength - oldWeight);
    
    cell1.connectionWeights[cellIdx2] = newWeight;
    cell2.connectionWeights[cellIdx1] = newWeight;
}

void Hippocampus::encodePlaceCellMemory(const PlaceCell& cell) {
    // Create episodic memory entry from place cell activity
    if (!brain_) return;
    
    EpisodicMemory memory;
    memory.startTime = getCurrentTime();
    memory.context = "PlaceCell:" + std::to_string(cell.id.index()) + ":" + 
                     std::to_string(cell.x) + ":" + std::to_string(cell.y);
    memory.coherence = cell.familiarity;
    memory.reward = 0.0f;  // Will be updated if reward is available
    
    // Collect neural activity
    auto neurons = brain_->getAllNeurons();
    auto* workingMemory = brain_->getWorkingMemory();
    
    if (workingMemory) {
        for (Neuron* neuron : neurons) {
            float activation = workingMemory->getNeuronActivation(neuron->getId());
            memory.neurons.push_back(neuron->getId());
            memory.activations.push_back(activation);
        }
    }
    
    // Add to episodic memories
    episodicMemories_.push_back(memory);
}

void Hippocampus::createEpisodicFromTrace(const WorkingMemoryTrace& trace) {
    // Convert working memory trace to episodic memory
    EpisodicMemory memory;
    memory.startTime = trace.timestamp;
    memory.context = "WorkingMemoryTrace:" + std::to_string(trace.neuronId.index());
    memory.coherence = trace.importance;
    memory.reward = trace.reward;
    
    // Use the trace as the episodic memory content
    memory.neurons.push_back(trace.neuronId);
    memory.activations.push_back(trace.activation);
    
    episodicMemories_.push_back(memory);
}

void Hippocampus::consolidateMemories() {
    // Transfer important short-term memories to long-term storage
    std::vector<WorkingMemoryTrace> importantTraces;
    
    auto* workingMemory = brain_->getWorkingMemory();
    if (!workingMemory) return;
    
    // Identify high-salience memories
    for (size_t i = 0; i < shortTermMemory_.size(); ++i) {
        const WorkingMemoryTrace& trace = shortTermMemory_[i];
        float importance = calculateMemoryImportance(trace);
        
        if (importance > 0.7f) {
            importantTraces.push_back(trace);
        }
    }
    
    // Convert to episodic memories
    for (const auto& trace : importantTraces) {
        createEpisodicFromTrace(trace);
    }
    
    // Clear processed traces
    shortTermMemory_.clear();
}

void Hippocampus::replayEpisode(const EpisodicMemory& episode) {
    // Reactivate neural patterns associated with the episode
    auto* workingMemory = brain_->getWorkingMemory();
    if (!workingMemory) return;
    
    for (size_t i = 0; i < episode.neurons.size(); ++i) {
        NeuronId neuronId = episode.neurons[i];
        float activation = episode.activations[i] * 0.8f;
        
        workingMemory->storeToNeuron(neuronId, activation);
    }
    
    // Apply neuromodulation
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        dopamine->setLevel(dopamine->getLevel() * 1.1f);
    }
}

void Hippocampus::updateEpisodicMemoryConsolidation(EpisodicMemory& memory, TimestepDuration dt) {
    // Update memory coherence and stability
    memory.coherence *= (1.0f - memoryConsolidationRate_ * dt);
    
    // Decay weak memories
    if (memory.coherence < 0.1f) {
        // Weak memory fades away
        memory.coherence = 0.0f;
    }
}

bool Hippocampus::shouldConsolidateMemory() const {
    // Check if it's time to consolidate short-term memories
    static Timestamp lastConsolidation = 0;
    Timestamp currentTime = getCurrentTime();
    
    return (currentTime - lastConsolidation) > 1000.0f;  // Consolidate every 1000ms
}

bool Hippocampus::shouldReplayMemories() const {
    // Check if it's time to replay memories
    static Timestamp lastReplay = 0;
    Timestamp currentTime = getCurrentTime();
    
    return (currentTime - lastReplay) > 500.0f;  // Replay every 500ms
}

void Hippocampus::scheduleMemoryReplay() {
    replaySchedule_.push_back(getCurrentTime() + 500);  // Schedule in 500ms
}

void Hippocampus::replayScheduledMemories() {
    Timestamp currentTime = getCurrentTime();
    
    // Process scheduled replays
    auto it = replaySchedule_.begin();
    while (it != replaySchedule_.end()) {
        if (*it <= currentTime) {
            // Execute replay
            replayEpisodes(3);  // Replay 3 episodes
            it = replaySchedule_.erase(it);
        } else {
            ++it;
        }
    }
}

void Hippocampus::triggerPlaceCellUpdate() {
    // Trigger update of place cells based on current position
    for (auto& cell : placeCells_) {
        cell.familiarity = std::min(1.0f, cell.familiarity + 0.01f);
    }
}

float Hippocampus::computePlaceCellActivation(const PlaceCell& cell,
                                            const SpatialInfo& info) const {
    float dx = info.x - cell.x;
    float dy = info.y - cell.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    float sigma = cell.fieldRadius;
    float activation = std::exp(-(distance * distance) / (2.0f * sigma * sigma));
    
    activation *= cell.familiarity;
    activation *= (1.0f + (std::rand() % 100 - 50) * 0.01f);
    
    return std::max(0.0f, std::min(1.0f, activation));
}

float Hippocampus::computeGridCellActivation(const GridCell& cell,
                                            const SpatialInfo& info) const {
    // Compute grid cell activation based on multiple firing fields
    float activation = 0.0f;
    
    for (const auto& field : cell.fields) {
        float distance = computeDistance(field.centerX, field.centerY,
                                        info.x, info.y);
        float phase = 2.0f * M_PI * distance / cell.periodicity;
        
        float fieldActivation = std::cos(phase);
        fieldActivation = std::max(0.0f, fieldActivation);
        
        activation += fieldActivation * field.strength;
    }
    
    return activation;
}

float Hippocampus::computeCoActivation(const PlaceCell& cell1,
                                     const PlaceCell& cell2) const {
    // Compute co-activation between two place cells
    float dx = cell1.x - cell2.x;
    float dy = cell1.y - cell2.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    float maxDistance = environmentWidth_ * 0.3f;  // 30% of environment width
    float coActivation = std::exp(-distance / maxDistance);
    
    return coActivation;
}

float Hippocampus::computeMemorySimilarity(const EpisodicMemory& memory,
                                         const std::string& context) const {
    // Compute similarity between stored memory and current context
    float similarity = 0.0f;
    
    // Compare context strings (simple approach)
    if (memory.context == context) {
        similarity += 0.5f;
    } else {
        // Simple string similarity (could be improved)
        size_t minLength = std::min(memory.context.size(), context.size());
        size_t matches = 0;
        
        for (size_t i = 0; i < minLength; ++i) {
            if (memory.context[i] == context[i]) {
                matches++;
            }
        }
        
        similarity += static_cast<float>(matches) / static_cast<float>(minLength) * 0.5f;
    }
    
    // Add coherence factor
    similarity += memory.coherence * 0.5f;
    
    return std::min(1.0f, similarity);
}

float Hippocampus::calculateMemoryImportance(const WorkingMemoryTrace& trace) const {
    // Calculate importance of a working memory trace
    float importance = trace.activation;
    
    // Consider novelty and reward
    importance *= (1.0f + trace.novelty * 0.5f);
    importance *= (1.0f + trace.reward * 0.2f);
    
    // Consider age (older traces may be less important)
    float age = getCurrentTime() - trace.timestamp;
    float ageFactor = std::exp(-age * 0.01f);
    importance *= ageFactor;
    
    return std::min(1.0f, importance);
}

float Hippocampus::getMemoryCoherence() const {
    if (episodicMemories_.empty()) {
        return 0.0f;
    }
    
    float totalCoherence = 0.0f;
    for (const auto& memory : episodicMemories_) {
        totalCoherence += memory.coherence;
    }
    
    return totalCoherence / episodicMemories_.size();
}

void Hippocampus::updateGridCellConnections() {
    // Update grid cell connectivity
    for (auto& cell : gridCells_) {
        establishGridPeriodicity(cell);
        updateGridCellOrientation(cell);
        updateGridCellScale(cell);
    }
}

void Hippocampus::updatePlaceCellConnections() {
    // Update place cell connectivity
    for (size_t i = 0; i < placeCells_.size(); ++i) {
        for (size_t j = i + 1; j < placeCells_.size(); ++j) {
            const PlaceCell& cell1 = placeCells_[i];
            const PlaceCell& cell2 = placeCells_[j];
            
            float coActivation = computeCoActivation(cell1, cell2);
            
            if (coActivation > 0.01f) {
                strengthenPlaceCellConnection(i, j, coActivation);
            }
        }
    }
}

void Hippocampus::establishGridPeriodicity(GridCell& cell) {
    // Establish periodic firing pattern
    float phaseShift = uniformDist(rng) * 2.0f * M_PI;
    
    for (auto& field : cell.fields) {
        float distance = computeDistance(field.centerX, field.centerY,
                                        currentSpatialInfo_.x, currentSpatialInfo_.y);
        float phase = 2.0f * M_PI * distance / cell.periodicity + phaseShift;
        
        float fieldActivation = std::cos(phase);
        fieldActivation = std::max(0.0f, fieldActivation);
        
        field.strength = field.strength * 0.9f + fieldActivation * 0.1f;
    }
}

void Hippocampus::updateGridCellOrientation(GridCell& cell) {
    // Update orientation based on movement
    float movementDirection = std::atan2(currentSpatialInfo_.velocityY,
                                       currentSpatialInfo_.velocityX);
    
    float orientationDiff = std::fmod(movementDirection - cell.orientation + M_PI,
                                    2.0f * M_PI) - M_PI;
    cell.orientation += gridCellLearningRate_ * orientationDiff;
}

void Hippocampus::updateGridCellScale(GridCell& cell) {
    // Update scale based on familiarity
    float familiarity = computeSpatialFamiliarity();
    
    float targetScale = 2.0f + familiarity * 6.0f;  // Range 2.0 to 8.0
    cell.scale += gridCellLearningRate_ * (targetScale - cell.scale);
    cell.scale = std::max(1.0f, std::min(16.0f, cell.scale));
    
    cell.periodicity = environmentWidth_ / cell.scale;
}

std::vector<const EpisodicMemory*> Hippocampus::selectEpisodes(
    const std::vector<const EpisodicMemory*>& episodes,
    const std::vector<float>& weights,
    size_t count) {
    
    std::vector<const EpisodicMemory*> selected;
    selected.reserve(count);
    
    if (episodes.empty()) {
        return selected;
    }
    
    // Weighted random selection
    std::vector<float> cumulativeWeights;
    float totalWeight = 0.0f;
    
    for (size_t i = 0; i < weights.size(); ++i) {
        totalWeight += weights[i];
        cumulativeWeights.push_back(totalWeight);
    }
    
    std::vector<size_t> indices(episodes.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    // Shuffle indices based on weights
    std::shuffle(indices.begin(), indices.end(), rng);
    
    // Select top N by weighted random
    size_t selectedCount = std::min(count, episodes.size());
    for (size_t i = 0; i < selectedCount; ++i) {
        selected.push_back(episodes[indices[i]]);
    }
    
    return selected;
}

float Hippocampus::computeSpatialFamiliarity() const {
    // Compute spatial familiarity based on exploration
    float familiarity = 0.0f;
    
    // Use current position to compute familiarity with environment
    float regionX = std::fmod(currentSpatialInfo_.x, environmentWidth_);
    float regionY = std::fmod(currentSpatialInfo_.y, environmentHeight_);
    
    // Higher familiarity in well-explored regions
    familiarity = 1.0f - std::exp(-(regionX + regionY) * 0.01f);
    
    return familiarity;
}

float Hippocampus::computeDistance(float x1, float y1, float x2, float y2) const {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return std::sqrt(dx * dx + dy * dy);
}

Timestamp Hippocampus::getCurrentTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return static_cast<Timestamp>(duration.count() / 1000000.0);
}

} // namespace nlm

// Memory pool integration with hippocampus
namespace nlm {
namespace memory {

void integrateHippocampusWithMemory(Hippocampus* hippocampus) {
    if (!hippocampus || !hippocampus->getBrain()) {
        return;
    }
    
    // Get brain reference
    Brain* brain = hippocampus->getBrain();
    
    // Get memory pools
    auto* neuronPool = getNeuronPool();
    auto* synapsePool = getSynapsePool();
    auto* memoryPool = getMemorySystemPool();
    
    if (!neuronPool || !synapsePool) {
        return;
    }
    
    // Convert existing neurons to pool allocation
    auto regions = brain->getRegions();
    for (auto& region : regions) {
        auto populations = region->getPopulations();
        for (auto& pop : populations) {
            auto neurons = pop->getNeurons();
            for (auto* neuron : neurons) {
                // Neuron is already allocated, nothing to do
            }
        }
    }
}

} // namespace memory
} // namespace nlm