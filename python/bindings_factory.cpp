// bindings_factory.cpp
#include "bindings_factory.h"
#include "bindings_utils.h"

#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"

namespace nlm {
namespace bindings {

void bindFactoryFunctions(py::module_& m) {
    // Factory function for creating default config
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration", py::return_value_policy::take_ownership);

    // Factory function for creating a brain with config
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration", py::return_value_policy::take_ownership);

    // Factory function for creating a simple world
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world", py::return_value_policy::take_ownership);

    // Factory function for creating an agent brain
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface", py::return_value_policy::take_ownership);

    // Simulation utilities
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                               int steps, double timestep) {
        if (!brain || !world) {
            throw NLMException("Brain and world must not be null");
        }
        if (steps < 0) {
            throw NLMException("Number of steps must be non-negative");
        }
        if (timestep <= 0) {
            throw NLMException("Time step must be positive");
        }
        
        brain->initialize();
        
        for (int i = 0; i < steps; ++i) {
            auto percept = world->getSensoryPercept();
            brain->receiveSensoryInput(percept);
            brain->step(i, i * timestep);
            auto action = brain->produceAction();
            world->applyMotorCommand(action, i * timestep);
        }
        
        return world->getAgentBody();
    }, py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.1,
         "Run a complete simulation with brain and world", py::return_value_policy::take_ownership);

    // Convenience function for creating a complete NLM system
    m.def("create_nlm_system", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100, nlm::ConfigSource::Default);
        config->set("region_count", 2, nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 32, 32);
        world->setAgentStart(50, 50);
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        
        return std::make_tuple(brain, world, agentBrain);
    }, "Create a complete NLM system with default configuration", py::return_value_policy::take_ownership);

    // Statistics and monitoring utilities
    m.def("get_system_stats", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw NLMException("Brain must not be null");
        }
        
        py::dict stats;
        stats["neuron_count"] = brain->getTotalNeuronCount();
        stats["synapse_count"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        stats["excitement_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["region_count"] = brain->getRegionCount();
        
        return stats;
    }, py::arg("brain"), "Get comprehensive statistics about the brain");

    // Error handling utilities
    m.def("safe_step", [](std::shared_ptr<Brain> brain, int step, double time) {
        try {
            brain->step(step, time);
            return true;
        } catch (const std::exception& e) {
            throw NLMException(std::string("Error during step " ) + std::to_string(step) + ": " + e.what());
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time"),
         "Safely perform a simulation step with error handling");

    // Batch processing utilities
    m.def("batch_process_sensory", [](std::shared_ptr<Brain> brain, 
                                   const std::vector<SensoryPercept>& percepts,
                                   double timestep) {
        if (!brain) {
            throw NLMException("Brain must not be null");
        }
        
        std::vector<Action> actions;
        for (size_t i = 0; i < percepts.size(); ++i) {
            brain->receiveSensoryInput(percepts[i]);
            brain->step(static_cast<int>(i), i * timestep);
            actions.push_back(brain->produceAction());
        }
        
        return actions;
    }, py::arg("brain"), py::arg("percepts"), py::arg("timestep") = 0.1,
         "Process multiple sensory inputs in batch", py::return_value_policy::take_ownership);
}

} // namespace bindings
} // namespace nlm