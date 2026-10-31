// StepManager implementation
#include "BrainComponents.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

StepManager::StepManager(std::shared_ptr<Brain> brain, const BrainSystemConfig& config)
    : brain_(brain), config_(config), totalSpikes_(0), firingNeurons_(0), averageFiringRate_(0.0f) {}

bool StepManager::initialize() {
    NLM_LOG_INFO("Initializing StepManager...");
    // Register event handlers
    brain_->getSpikeSystem()->registerHandler(
        [this](const DetailedSpikeEvent& event) {
            ++totalSpikes_;
            // Track firing neurons
            for (auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->getId() == event.neuron_id) {
                            if (neuron->isFiring()) {
                                ++firingNeurons_;
                            }
                            break;
                        }
                    }
                }
            }
        }
    );
    
    brain_->getSpikeSystem()->registerDelayedHandler(
        [this](const DelayedSpikeEvent& event) {
            // Find destination neuron and deliver input
            for (auto& region : brain_->getRegions()) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (neuron->getId() == event.destination_neuron) {
                        // Apply synaptic weight as current
                        MembranePotential synapticCurrent = event.weight * 10.0f;
                        if (event.is_excitatory) {
                            neuron->receiveExcitatoryInput(synapticCurrent);
                        } else {
                            neuron->receiveInhibitoryInput(-synapticCurrent);
                        }
                        return;
                    }
                }
            }
        }
    );
    
    NLM_LOG_INFO("StepManager initialized successfully");
    return true;
}

void StepManager::reset() {
    NLM_LOG_INFO("Resetting StepManager...");
    totalSpikes_ = 0;
    firingNeurons_ = 0;
    averageFiringRate_ = 0.0f;
    spikeHandlers_.clear();
    delayedHandlers_.clear();
    NLM_LOG_INFO("StepManager reset complete");
}

void StepManager::update() {
    // Update average firing rate
    if (config_.neuronCount > 0) {
        averageFiringRate_ = static_cast<float>(firingNeurons_) / config_.neuronCount;
    }
    firingNeurons_ = 0; // Reset for next step
}

void StepManager::logStatus() const {
    NLM_LOG_INFO("=== StepManager Status ===");
    NLM_LOG_INFO("Total spikes: " + std::to_string(totalSpikes_));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(firingNeurons_));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(averageFiringRate_));
    NLM_LOG_INFO("Spike handlers: " + std::to_string(spikeHandlers_.size()));
    NLM_LOG_INFO("Delayed handlers: " + std::to_string(delayedHandlers_.size()));
}

void StepManager::advanceStep(SimulationStep step, Timestamp time) {
    // Process delayed spike events
    brain_->getSpikeSystem()->processDelayedSpikes(step, time);
    
    // Process immediate spikes
    brain_->getSpikeSystem()->processSpikes(step);
}

void StepManager::processDelayedSpikes(SimulationStep step, Timestamp time) {
    brain_->getSpikeSystem()->processDelayedSpikes(step, time);
}

void StepManager::processSpikes(SimulationStep step) {
    brain_->getSpikeSystem()->processSpikes(step);
}

void StepManager::registerSpikeHandler(std::function<void(const DetailedSpikeEvent&)> handler) {
    spikeHandlers_.push_back(std::move(handler));
}

void StepManager::registerDelayedHandler(std::function<void(const DelayedSpikeEvent&)> handler) {
    delayedHandlers_.push_back(std::move(handler));
}

} // namespace nlm