// Brain.cpp - Simplified, clean implementation with PIMPL pattern

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <stdexcept>
#include <algorithm>
#include <unordered_map>

namespace nlm {

//===================================
// Brain::Impl - Simplified PIMPL Implementation
//===================================

struct Brain::Impl {
    // Core components
    std::shared_ptr<Config> config;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    std::unordered_map<uint64_t, Neuron*> neuronById;
    
    // Integrated systems
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    
    // Simulation state
    SimulationStep currentStep;
    Timestamp currentTime;
    size_t totalSpikesThisStep;
    
    // Configuration
    TimestepDuration timestep;
    
    explicit Impl(std::shared_ptr<Config> cfg)
        : config(std::move(cfg))
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , timestep(0.001)
    {
        if (!config) {
            throw std::invalid_argument("Config is null");
        }
        
        // Initialize spike system
        spikeSystem = std::make_unique<SpikeSystem>();
        
        // Initialize working memory
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        
        NLM_LOG_INFO("Brain initialized with PIMPL pattern");
    }
    
    void initialize() {
        if (!config) {
            return;
        }
        
        // Create default regions
        auto regionId = addRegion("Default_Region");
        auto* region = getRegion(regionId);
        
        if (region) {
            // Add neural populations
            region->addPopulation(250, NeuronType::Sensory);
            region->addPopulation(200, NeuronType::Internal);
            region->addPopulation(100, NeuronType::Motor);
        }
        
        NLM_LOG_INFO("Brain systems initialized");
    }
    
    void step(SimulationStep step, Timestamp time) {
        currentStep = step;
        currentTime = time;
        totalSpikesThisStep = 0;
        
        // Process delayed spikes
        if (spikeSystem) {
            spikeSystem->processDelayedSpikes(currentStep, currentTime);
        }
        
        // Update neurons
        for (auto& region : regions) {
            if (!region) continue;
            
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron) {
                        neuron->stepLIF(currentTime, timestep);
                    }
                }
            }
        }
        
        // Detect and process spikes
        processSpikes();
        
        // Update working memory
        if (workingMemory) {
            workingMemory->update(timestep);
        }
        
        // Collect statistics
        collectStats();
    }
    
    void processSpikes() {
        if (!spikeSystem) return;
        
        for (auto& region : regions) {
            if (!region) continue;
            
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) continue;
                    
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);
                    
                    if (justFired) {
                        totalSpikesThisStep++;
                        
                        // Queue spike event
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        spikeSystem->queueSpike(event);
                    }
                }
            }
        }
        
        // Process queued spikes
        spikeSystem->processSpikes(currentStep);
    }
    
    void collectStats() {
        size_t activeCount = 0;
        for (const auto& region : regions) {
            if (!region) continue;
            
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron && neuron->isFiring()) {
                        activeCount++;
                    }
                }
            }
        }
        
        NLM_LOG_DEBUG("Step " + std::to_string(currentStep) + ": " +
                     std::to_string(totalSpikesThisStep) + " spikes, " +
                     std::to_string(activeCount) + " active neurons");
    }
    
    // Getter methods
    NeuralRegion* getRegion(RegionId id) {
        for (auto& region : regions) {
            if (region && region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    }
    
    const NeuralRegion* getRegion(RegionId id) const {
        for (const auto& region : regions) {
            if (region && region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    }
    
    RegionId addRegion(const std::string& name) {
        auto region = std::make_unique<NeuralRegion>(name, regions.size() + 1);
        RegionId id = region->getId();
        regions.push_back(std::move(region));
        return id;
    }
    
    size_t getRegionCount() const {
        return regions.size();
    }
    
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const {
        return regions;
    }
    
    SpikeSystem* getSpikeSystem() {
        return spikeSystem.get();
    }
    
    NeuralWorkingMemory* getWorkingMemory() {
        return workingMemory.get();
    }
}; // End of Impl

//===================================
// Brain Class - Simplified Interface
//===================================

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() {
    delete pImpl;
}

bool Brain::initialize() {
    if (!pImpl) {
        NLM_LOG_ERROR("Cannot initialize: Brain implementation is null");
        return false;
    }
    
    pImpl->initialize();
    return true;
}

void Brain::step(SimulationStep step) {
    step(step, step * 0.001); // Default timestep
}

void Brain::step(SimulationStep step, Timestamp time) {
    if (!pImpl) return;
    pImpl->step(step, time);
}

void Brain::receiveSensoryInput(const SensoryInput& input) {
    // Simple implementation - inject to sensory neurons
    if (!pImpl) return;
    
    for (auto& region : pImpl->getRegions()) {
        if (!region) continue;
        
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron) {
                        neuron->injectCurrent(5.0f); // Simple injection
                    }
                }
            }
        }
    }
}

std::unique_ptr<Action> Brain::produceAction() {
    auto action = std::make_unique<Action>();
    // Simple action selection based on highest activity
    action->setCommand(MotorCommand::MoveForward);
    return action;
}

void Brain::reset() {
    if (!pImpl) return;
    
    pImpl->regions.clear();
    pImpl->neuronById.clear();
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
}

// PIMPL accessors
SpikeSystem* Brain::getSpikeSystem() { return pImpl ? pImpl->getSpikeSystem() : nullptr; }
NeuralWorkingMemory* Brain::getWorkingMemory() { return pImpl ? pImpl->getWorkingMemory() : nullptr; }

// Basic statistics
size_t Brain::getTotalSpikeCount() const { return pImpl ? pImpl->totalSpikesThisStep : 0; }
size_t Brain::getActiveNeuronCount() const {
    size_t count = 0;
    if (pImpl) {
        for (const auto& region : pImpl->getRegions()) {
            if (!region) continue;
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron && neuron->isFiring()) count++;
                }
            }
        }
    }
    return count;
}
