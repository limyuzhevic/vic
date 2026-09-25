#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <memory>

namespace py = pybind11;

namespace nlm {

// Python wrapper for SpikeSystem with enhanced spike callback support
class PySpikeSystem {
public:
    PySpikeSystem(std::shared_ptr<SpikeSystem> spikeSystem)
        : spikeSystem_(std::move(spikeSystem)) {}
    
    // Register spike handler callback (supports lambda functions and Python callables)
    py::cpp_function register_spike_handler(py::function callback) {
        if (!callback) {
            throw py::value_error("Callback cannot be None");
        }
        
        // Create a std::function wrapper that retains the Python callback
        std::function<void(const DetailedSpikeEvent&)> cppCallback = 
            [callback = std::move(callback)](const DetailedSpikeEvent& event) {
                try {
                    // Call the Python callback with appropriate arguments
                    callback(py::arg("source") = event.source,
                            py::arg("timestamp") = event.timestamp,
                            py::arg("step") = event.step,
                            py::arg("regionId") = event.regionId,
                            py::arg("populationId") = event.populationId);
                } catch (const std::exception& e) {
                    // Log error but continue processing
                }
            };
        
        spikeSystem_->registerHandler(cppCallback);
        return cppCallback;
    }
    
    // Register delayed spike handler callback
    py::cpp_function register_delayed_spike_handler(py::function callback) {
        if (!callback) {
            throw py::value_error("Delayed callback cannot be None");
        }
        
        std::function<void(const DelayedSpikeEvent&)> cppCallback =
            [callback = std::move(callback)](const DelayedSpikeEvent& event) {
                try {
                    callback(py::arg("sourceNeuron") = event.source_neuron,
                            py::arg("destinationNeuron") = event.destination_neuron,
                            py::arg("synapseId") = event.synapse_id,
                            py::arg("weight") = event.weight,
                            py::arg("synapseType") = event.synapse_type,
                            py::arg("timestamp") = event.timestamp,
                            py::arg("deliveryTime") = event.delivery_time,
                            py::arg("step") = event.step,
                            py::arg("deliveryStep") = event.delivery_step,
                            py::arg("isExcitatory") = event.is_excitatory);
                } catch (const std::exception& e) {
                    // Log error but continue processing
                }
            };
        
        spikeSystem_->registerDelayedHandler(cppCallback);
        return cppCallback;
    }
    
    // Remove spike handler by index
    bool remove_spike_handler(size_t index) {
        return spikeSystem_->removeHandler(index);
    }
    
    // Remove delayed spike handler by index
    bool remove_delayed_spike_handler(size_t index) {
        return spikeSystem_->removeDelayedHandler(index);
    }
    
    // Get spike handler count
    size_t get_spike_handler_count() const {
        return spikeSystem_->getSpikeHandlerCount();
    }
    
    // Get delayed spike handler count
    size_t get_delayed_spike_handler_count() const {
        return spikeSystem_->getDelayedSpikeHandlerCount();
    }
    
    // Get callback statistics
    std::pair<size_t, size_t> get_callback_statistics() const {
        // Note: SpikeSystem doesn't expose statistics, so we return default values
        // This could be extended in the future
        return {0, 0};
    }
    
    // Delegate to underlying SpikeSystem
    void queueSpike(const SpikeEvent& event) {
        spikeSystem_->queueSpike(event);
    }
    
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
        spikeSystem_->queueSpike(neuron, timestamp, step);
    }
    
    void queueDelayedSpike(const DelayedSpikeEvent& event) {
        spikeSystem_->queueDelayedSpike(event);
    }
    
    void processSpikes(SimulationStep currentStep) {
        spikeSystem_->processSpikes(currentStep);
    }
    
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
        spikeSystem_->processDelayedSpikes(currentStep, currentTime);
    }
    
    const std::vector<DetailedSpikeEvent>& getSpikeHistory() const {
        return spikeSystem_->getSpikeHistory();
    }
    
    void clearHistory() {
        spikeSystem_->clearHistory();
    }
    
    size_t getSpikeCount() const {
        return spikeSystem_->getSpikeCount();
    }
    
    size_t getPendingSpikeCount() const {
        return spikeSystem_->getPendingSpikeCount();
    }
    
    size_t getPendingDelayedCount() const {
        return spikeSystem_->getPendingDelayedCount();
    }
    
    float getAverageSpikeRate() const {
        return spikeSystem_->getAverageSpikeRate();
    }
    
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const {
        return spikeSystem_->getMostActiveNeurons(count);
    }
    
    void reset() {
        spikeSystem_->reset();
    }
    
    // Get reference to underlying system
    std::shared_ptr<SpikeSystem> getSystem() const {
        return spikeSystem_;
    }

private:
    std::shared_ptr<SpikeSystem> spikeSystem_;
};

} // namespace nlm