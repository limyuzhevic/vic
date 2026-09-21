#pragma once

namespace nlm {

// Safe neural pointer check utility
inline bool isValidNeuron(const Neuron* neuron) {
    return neuron != nullptr && neuron->getId().value != 0;
}

// Safe synaptic pointer check utility  
inline bool isValidSynapse(const Synapse* synapse) {
    return synapse != nullptr && 
           synapse->getSourceNeuron().value != 0 && 
           synapse->getDestinationNeuron().value != 0;
}

// Configuration validation utilities
inline bool isValidNeuronCount(size_t count) {
    return count > 0 && count <= 100000;  // Reasonable upper bound
}

inline bool isValidConnectionProbability(float prob) {
    return prob >= 0.0f && prob <= 1.0f;
}

// Vector iteration helper for null-safe traversal
template<typename Container, typename Func>
void safeForEach(const Container& container, Func func) {
    for (const auto& item : container) {
        if (item) {
            func(item);
        }
    }
}

// Neuron safety wrapper
class NeuronSafetyWrapper {
private:
    Neuron* neuron_;
    
public:
    explicit NeuronSafetyWrapper(Neuron* neuron) : neuron_(neuron) {}
    
    bool isValid() const { return isValidNeuron(neuron_); }
    
    Neuron* get() { return neuron_; }
    const Neuron* get() const { return neuron_; }
    
    explicit operator bool() const { return isValid(); }
    
    Neuron& operator*() { return *neuron_; }
    const Neuron& operator*() const { return *neuron_; }
    
    Neuron* operator->() { return neuron_; }
    const Neuron* operator->() const { return neuron_; }
};

// Synapse safety wrapper
class SynapseSafetyWrapper {
private:
    Synapse* synapse_;
    
public:
    explicit SynapseSafetyWrapper(Synapse* synapse) : synapse_(synapse) {}
    
    bool isValid() const { return isValidSynapse(synapse_); }
    
    Synapse* get() { return synapse_; }
    const Synapse* get() const { return synapse_; }
    
    explicit operator bool() const { return isValid(); }
    
    Synapse& operator*() { return *synapse_; }
    const Synapse& operator*() const { return *synapse_; }
    
    Synapse* operator->() { return synapse_; }
    const Synapse* operator->() const { return synapse_; }
};

} // namespace nlm
