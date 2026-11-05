#pragma once

#include <string>
#include <vector>
#include <memory>
#include "NeuronGroup.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Sensory neuron group implementation
class SensoryNeuronGroup : public NeuronGroup {
public:
    SensoryNeuronGroup(const std::string& name, int priority = 0);
    ~SensoryNeuronGroup() override = default;
    
    std::string getType() const override { return "Sensory"; }
    std::string getName() const override { return name_; }
    int getPriority() const override { return priority_; }
    
    void addNeuron(Neuron* neuron) override;
    const std::vector<Neuron*>& getNeurons() const override { return neurons_; }
    size_t getNeuronCount() const override { return neurons_.size(); }
    float getActivity() const override;
    float getFiringRate() const override;
    size_t getRefractoryCount() const override;
    void clear() override;
    void setActivity(float activity) override;
    void setFiringRate(float rate) override;
    
private:
    std::string name_;
    int priority_;
    std::vector<Neuron*> neurons_;
    float activity_;
    float firingRate_;
};

} // namespace nlm
