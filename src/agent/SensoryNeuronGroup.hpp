#pragma once

#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include <vector>
#include <memory>

namespace nlm {

class SensoryNeuronGroup {
public:
    // Group different types of sensory neurons
    enum class Type {
        Vision,
        Touch,
        Internal,
        Proprioception
    };
    
    SensoryNeuronGroup(Type type, std::vector<Neuron*>& neurons)
        : type_(type), neurons_(neurons) {
        if (neurons.empty()) {
            throw AgentBrainException::nullPointerError(
                "SensoryNeuronGroup initialization: empty neuron vector");
        }
    }
    
    Type getType() const { return type_; }
    size_t size() const { return neurons_.size(); }
    bool empty() const { return neurons_.empty(); }
    
    // Get reference to neuron at index (for processing)
    Neuron* operator[](size_t index) {
        if (index >= neurons_.size()) {
            throw std::out_of_range("SensoryNeuronGroup index out of range");
        }
        return neurons_[index];
    }
    
    const Neuron* operator[](size_t index) const {
        if (index >= neurons_.size()) {
            throw std::out_of_range("SensoryNeuronGroup index out of range");
        }
        return neurons_[index];
    }
    
    // Iterator support for range-based loops
    auto begin() { return neurons_.begin(); }
    auto end() { return neurons_.end(); }
    auto begin() const { return neurons_.begin(); }
    auto end() const { return neurons_.end(); }
    
    // Check if group has valid neurons
    bool isValid() const {
        for (Neuron* neuron : neurons_) {
            if (!neuron) {
                return false;
            }
        }
        return !neurons_.empty();
    }
    
    // Calculate average neuron activity
    float calculateActivity() const {
        if (empty()) return 0.0f;
        
        float total = 0.0f;
        for (const Neuron* neuron : neurons_) {
            if (neuron) {
                total += std::abs(neuron->getState().membranePotential - 
                                 neuron->getState().restingPotential);
            }
        }
        return total / neurons_.size();
    }
    
private:
    Type type_;
    std::vector<Neuron*>& neurons_;
};

} // namespace nlm
