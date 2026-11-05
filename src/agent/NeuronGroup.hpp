// NeuronGroup.hpp
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../core/Types/Types.hpp"

namespace nlm {

// Abstract base class for neuron groups
class NeuronGroup {
public:
    virtual ~NeuronGroup() = default;
    
    // Get the group type
    virtual std::string getType() const = 0;
    
    // Get group name
    virtual std::string getName() const = 0;
    
    // Get group priority (higher number = higher priority)
    virtual int getPriority() const = 0;
    
    // Add a neuron to this group
    virtual void addNeuron(Neuron* neuron) = 0;
    
    // Get all neurons in this group
    virtual const std::vector<Neuron*>& getNeurons() const = 0;
    
    // Get number of neurons in this group
    virtual size_t getNeuronCount() const = 0;
    
    // Get activity level of the group
    virtual float getActivity() const = 0;
    
    // Get firing rate of the group
    virtual float getFiringRate() const = 0;
    
    // Get number of refractory neurons
    virtual size_t getRefractoryCount() const = 0;
    
    // Check if group is empty
    virtual bool isEmpty() const { return getNeuronCount() == 0; }
    
    // Clear all neurons from group
    virtual void clear() = 0;
    
    // Set group activity
    virtual void setActivity(float activity) = 0;
    
    // Set group firing rate
    virtual void setFiringRate(float rate) = 0;
};

} // namespace nlm
