#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <memory>

namespace nlm {

class SensoryMotorInterface {
public:
    SensoryMotorInterface(std::shared_ptr<Config> config);
    ~SensoryMotorInterface();
    
    // Initialize I/O systems
    bool initialize(Brain* brain);
    
    // Process sensory input
    void receiveSensoryInput(const SensoryInput& input);
    
    // Inject current directly into neurons
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Generate motor output
    std::unique_ptr<Action> produceAction();
    
    // Reset I/O systems
    void reset();
    
    // Get component access
    const std::vector<Neuron*>& getSensoryNeurons() const { return sensoryNeurons; }
    const std::vector<Neuron*>& getMotorNeurons() const { return motorNeurons; }
    
    // I/O statistics
    size_t getSensoryNeuronCount() const { return sensoryNeurons.size(); }
    size_t getMotorNeuronCount() const { return motorNeurons.size(); }
    
    // Configuration methods
    void setInputRange(float min, float max) { inputMin = min; inputMax = max; }
    void setOutputRange(float min, float max) { outputMin = min; outputMax = max; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    float inputMin;
    float inputMax;
    float outputMin;
    float outputMax;
};

} // namespace nlm