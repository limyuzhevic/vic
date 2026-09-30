#include "SpikeSystem.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <chrono>

namespace nlm {

// Forward declarations for event types (defined elsewhere)
class SpikeEvent;
class DelayedSpikeEvent;
class DetailedSpikeEvent;

struct SpikeSystem::Impl {
    // Event queues
    std::queue<SpikeEvent> immediateSpikeQueue;
    std::queue<DelayedSpikeEvent> delayedSpikeQueue;
    
    // Handlers for events
    std::vector<SpikeHandler> spikeHandlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    
    // Statistics
    size_t totalSpikeEvents;
    size_t totalDelayedSpikeEvents;
    size_t currentStepSpikes;
    size_t peakSpikesPerStep;
    Timestamp lastStatsUpdate;
    
    // Most active neurons tracking
    std::unordered_map<NeuronId, size_t> neuronSpikeCounts;
    std::priority_queue<std::pair<size_t, NeuronId>> activeNeuronQueue;
    
    // Event delivery timing
    Timestamp currentTime;
    SimulationStep currentStep;
    
    // Delivery buffer for delayed events
    std::vector<DelayedSpikeEvent> delayedDeliveryBuffer;
    
    Impl() : totalSpikeEvents(0), totalDelayedSpikeEvents(0), 
             currentStepSpikes(0), peakSpikesPerStep(0), 
             lastStatsUpdate(0.0), currentTime(0.0), currentStep(0) {}
};

SpikeSystem::SpikeSystem(Brain* brain) : BrainSystemBase(brain) {
    pImpl = std::make_unique<Impl>();
}

SpikeSystem::~SpikeSystem() = default;

void SpikeSystem::doInitialize() {
    pImpl->immediateSpikeQueue = std::queue<SpikeEvent>();
    pImpl->delayedSpikeQueue = std::queue<DelayedSpikeEvent>();
    pImpl->spikeHandlers.clear();
    pImpl->delayedHandlers.clear();
    pImpl->totalSpikeEvents = 0;
    pImpl->totalDelayedSpikeEvents = 0;
    pImpl->currentStepSpikes = 0;
    pImpl->peakSpikesPerStep = 0;
    pImpl->neuronSpikeCounts.clear();
    pImpl->activeNeuronQueue = std::priority_queue<std::pair<size_t, NeuronId>>();
    
    if (getBrain()) {
        getLogger()->info("SpikeSystem initialized with brain reference");
    }
}

void SpikeSystem::doShutdown() {
    pImpl->immediateSpikeQueue = std::queue<SpikeEvent>();
    pImpl->delayedSpikeQueue = std::queue<DelayedSpikeEvent>();
    pImpl->spikeHandlers.clear();
    pImpl->delayedHandlers.clear();
    
    if (getBrain()) {
        getLogger()->info("SpikeSystem shutdown complete");
    }
}

void SpikeSystem::doStep(SimulationStep step, Timestamp time) {
    pImpl->currentStep = step;
    pImpl->currentTime = time;
    pImpl->currentStepSpikes = 0;
    
    // Process immediate spikes
    processSpikes(step);
    
    // Process delayed spikes
    processDelayedSpikes(step, time);
    
    // Clean up old neuron spike counts periodically
    if (step % 100 == 0) {
        cleanupOldSpikeCounts();
    }
}

bool SpikeSystem::doConfigure(const std::shared_ptr<Config>& config) {
    if (!config) {
        getLogger()->warning("SpikeSystem: No configuration provided");
        return false;
    }
    
    getLogger()->info("SpikeSystem configured with timestep parameters");
    return true;
}

void SpikeSystem::doSetBrainReference(Brain* brain) {
    if (brain) {
        getLogger()->info("SpikeSystem brain reference set");
    }
}

void SpikeSystem::queueSpike(const SpikeEvent& event) {
    pImpl->immediateSpikeQueue.push(event);
    pImpl->totalSpikeEvents++;
    
    // Update neuron spike count
    pImpl->neuronSpikeCounts[event.neuronId]++;
    pImpl->activeNeuronQueue.push(std::make_pair(pImpl->neuronSpikeCounts[event.neuronId], event.neuronId));
    
    // Update statistics
    pImpl->currentStepSpikes++;
    if (pImpl->currentStepSpikes > pImpl->peakSpikesPerStep) {
        pImpl->peakSpikesPerStep = pImpl->currentStepSpikes;
    }
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    pImpl->delayedSpikeQueue.push(event);
    pImpl->totalDelayedSpikeEvents++;
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    // Process all immediate spikes in the queue
    while (!pImpl->immediateSpikeQueue.empty()) {
        const SpikeEvent& event = pImpl->immediateSpikeQueue.front();
        
        // Create detailed event for handlers
        DetailedSpikeEvent detailedEvent(
            event.neuronId,
            event.time,
            event.step,
            event.isSomatic,  // Assuming this field exists
            event.isExcitatory,
            event.weight,
            event.delay,
            event.isSpontaneous,
            event.isBurst
        );
        
        // Notify all handlers
        for (auto& handler : pImpl->spikeHandlers) {
            try {
                handler(detailedEvent);
            } catch (const std::exception& e) {
                getLogger()->error("Spike handler exception: " + std::string(e.what()));
            }
        }
        
        pImpl->immediateSpikeQueue.pop();
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Deliver any delayed spikes whose time has come
    while (!pImpl->delayedSpikeQueue.empty()) {
        const DelayedSpikeEvent& event = pImpl->delayedSpikeQueue.front();
        
        if (event.deliveryTime <= currentTime) {
            // Deliver synaptic input to destination neuron
            deliverSynapticInput(event);
            
            // Notify delayed handlers
            for (auto& handler : pImpl->delayedHandlers) {
                try {
                    handler(event);
                } catch (const std::exception& e) {
                    getLogger()->error("Delayed spike handler exception: " + std::string(e.what()));
                }
            }
            
            pImpl->delayedSpikeQueue.pop();
        } else {
            // Queue is sorted by delivery time, so we can stop
            break;
        }
    }
}

void SpikeSystem::deliverSynapticInput(const DelayedSpikeEvent& event) {
    if (!getBrain()) {
        return;
    }
    
    // Find destination neuron in the brain
    for (auto& region : getBrain()->getRegions()) {
        for (auto& neuron : region->getAllNeurons()) {
            if (neuron->getId() == event.destination_neuron) {
                // Apply synaptic input based on event type
                MembranePotential synapticInput = event.weight * 10.0f;  // Scale factor
                
                if (event.is_excitatory) {
                    neuron->receiveExcitatoryInput(synapticInput);
                } else {
                    neuron->receiveInhibitoryInput(-synapticInput);
                }
                break;
            }
        }
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    pImpl->spikeHandlers.push_back(std::move(handler));
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    pImpl->delayedHandlers.push_back(std::move(handler));
}

void SpikeSystem::cleanupOldSpikeCounts() {
    // Reset spike counts periodically to prevent unbounded growth
    for (auto& pair : pImpl->neuronSpikeCounts) {
        if (pair.second > 1000) {  // Threshold for cleanup
            pair.second = 1;  // Reset to 1 to preserve current activity
        } else {
            pair.second = 0;
        }
    }
    
    // Clear the priority queue
    while (!pImpl->activeNeuronQueue.empty()) {
        pImpl->activeNeuronQueue.pop();
    }
    
    // Rebuild queue with updated counts
    for (const auto& pair : pImpl->neuronSpikeCounts) {
        if (pair.second > 0) {
            pImpl->activeNeuronQueue.push(pair);
        }
    }
}

void SpikeSystem::reset() {
    doShutdown();
    doInitialize();
    getLogger()->info("SpikeSystem reset complete");
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->totalSpikeEvents;
}

float SpikeSystem::getAverageSpikeRate() const {
    if (getBrain() && pImpl->currentTime > 0.0) {
        return static_cast<float>(pImpl->totalSpikeEvents) / pImpl->currentTime;
    }
    return 0.0f;
}

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    std::vector<NeuronId> activeNeurons;
    
    // Create a temporary copy of the queue to avoid modifying original
    std::priority_queue<std::pair<size_t, NeuronId>> tempQueue = pImpl->activeNeuronQueue;
    
    size_t processed = 0;
    while (!tempQueue.empty() && processed < count) {
        auto pair = tempQueue.top();
        tempQueue.pop();
        activeNeurons.push_back(pair.second);
        processed++;
    }
    
    return activeNeurons;
}

} // namespace nlm
