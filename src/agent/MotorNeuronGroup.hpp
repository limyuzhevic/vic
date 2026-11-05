// MotorNeuronGroup.hpp
#pragma once

#include "NeuronGroup.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Motor neuron group implementation
class MotorNeuronGroup : public NeuronGroup {
public:
    MotorNeuronGroup(const std::string& name, int priority = 0);
    ~MotorNeuronGroup() override = default;
    
    std::string getType() const override { return "Motor"; }
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
    
    // Get motor-specific properties
    const std::string& getActionTypeName() const { return actionTypeName_; }
    
private:
    std::string name_;
    int priority_;
    std::vector<Neuron*> neurons_;
    float activity_;
    float firingRate_;
    std::string actionTypeName_;
};

} // namespace nlm
