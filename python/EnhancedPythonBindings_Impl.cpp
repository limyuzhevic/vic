// Implementation of Enhanced Python Bindings helper functions
#include "EnhancedPythonBindings_Fwd.hpp"
#include "EnhancedPythonBindings.hpp"
#include "core/ConfigFactory.hpp"
#include <pybind11/pybind11.h>

namespace nlm {

std::shared_ptr<PythonBrainWrapper> createEnhancedBrainWrapper(size_t neuronCount,
                                                               float connectionProbability,
                                                               float simulationTimestep,
                                                               int randomSeed,
                                                               const std::string& preset) {
    auto config = createConfig(neuronCount, connectionProbability, simulationTimestep, randomSeed);
    if (preset != "default") {
        applyPreset(config, preset);
    }
    
    auto brain = std::make_shared<Brain>(config);
    return std::make_shared<PythonBrainWrapper>(brain);
}

std::shared_ptr<PythonAgentWrapper> createEnhancedAgentWrapper(std::shared_ptr<Brain> brain) {
    auto agent = std::make_shared<AgentBrain>(brain);
    return std::make_shared<PythonAgentWrapper>(agent);
}

std::shared_ptr<PythonWorldWrapper> createEnhancedWorldWrapper() {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(Constants::DEFAULT_VISION_WIDTH, Constants::DEFAULT_VISION_HEIGHT);
    return std::make_shared<PythonWorldWrapper>(world);
}

std::shared_ptr<Brain> createBrain(size_t neuronCount,
                                 float connectionProbability,
                                 float simulationTimestep,
                                 int randomSeed) {
    auto config = createConfig(neuronCount, connectionProbability, simulationTimestep, randomSeed);
    return std::make_shared<Brain>(config);
}

std::shared_ptr<SimpleWorld> createWorld() {
    return std::make_shared<SimpleWorld>();
}

std::shared_ptr<AgentBrain> createAgent(std::shared_ptr<Brain> brain) {
    return std::make_shared<AgentBrain>(brain);
}

void applyPreset(std::shared_ptr<Config> config, const std::string& presetName) {
    nlm::applyPreset(config, presetName);
}

std::vector<std::string> getAvailablePresets() {
    return nlm::getAvailablePresets();
}

std::pair<bool, std::vector<std::string>> validateConfig(std::shared_ptr<Config> config) {
    return nlm::validateConfig(config);
}

} // namespace nlm