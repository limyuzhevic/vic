#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/core/Types/Types.hpp"
#include <memory>

namespace py = pybind11;
namespace nlm {

void bindBrain(py::module_ &m) {
    // Brain class - enhanced Python interface with context managers
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        
        // Step methods with Pythonic interface
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        
        // Enhanced sensory input method
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        
        // Current injection methods
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        
        // Action production
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        
        // State management
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        
        // Region management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal,
             "Get all regions")
        
        // Statistics
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        
        // Developmental stage
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        
        // Configuration
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal,
             "Get the configuration")
        
        // Logging and diagnostics
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        
        // Context manager for brain lifecycle
        .def("__enter__", [](Brain& brain) {
            if (!brain.initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
            return &brain;
        })
        
        .def("__exit__", [](Brain& brain, py::args) {
            brain.reset();
        })
        
        // Pythonic helper methods
        .def("run_simulation", [](Brain& brain, int steps, float timestep = 0.001f) {
            for (int step = 0; step < steps; ++step) {
                brain.step(step, step * timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.001f,
             "Run a simulation for specified number of steps")
        
        .def("with_sensory", [](Brain& brain, const SensoryInput& input) {
            brain.receiveSensoryInput(input);
            return &brain;
        }, py::arg("input"),
             "Inject sensory input and return self for chaining")
        
        .def("get_stats", [](const Brain& brain) {
            return py::dict(
                "neurons", brain.getTotalNeuronCount(),
                "synapses", brain.getTotalSynapseCount(),
                "firing", brain.getFiringNeuronCount(),
                "spikes", brain.getTotalSpikeCount(),
                "rate", brain.getAverageFiringRate(),
                "e_i_ratio", brain.getExcitationInhibitionRatio(),
                "dev_stage", static_cast<int>(brain.getDevelopmentalStage())
            );
        }, "Get brain statistics as dictionary")
        
        .def("__repr__", [](const Brain& brain) {
            return "<Brain neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   " firing=" + std::to_string(brain.getFiringNeuronCount()) + ">";
        });

    // AgentBrain class - enhanced Python interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        
        // Core processing methods
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        
        // Neuromodulation
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        
        // Development
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        
        // Neuromodulation levels
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        
        // Subsystem enable/disable
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        
        // Access to underlying brain
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal,
             "Get the underlying brain")
        
        // Context manager for agent lifecycle
        .def("__enter__", [](AgentBrain& agent) {
            return &agent;
        })
        
        .def("__exit__", [](AgentBrain& agent, py::args) {
            // Clean up can be added here if needed
        })
        
        // Pythonic helper methods
        .def("run_episode", [](AgentBrain& agent, SimpleWorld& world, int steps = 100) {
            world.reset();
            agent.initialize(world);
            
            for (int step = 0; step < steps; ++step) {
                world.update(0.1f);
                agent.processSensoryInput(world.getSensoryPercept());
                world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime());
                agent.updateDevelopment(0.1f);
            }
        }, py::arg("world"), py::arg("steps") = 100,
             "Run a complete episode")
        
        .def("get_episode_stats", [](AgentBrain& agent) {
            auto brain = agent.getBrain();
            return py::dict(
                "neurons", brain.getTotalNeuronCount(),
                "firing", brain.getFiringNeuronCount(),
                "curiosity", agent.getCuriosityLevel(),
                "novelty", agent.getNoveltyLevel(),
                "prediction_error", agent.getPredictionError(),
                "dev_stage", static_cast<int>(agent.getDevelopmentalStage())
            );
        }, "Get episode statistics")
        
        .def("enable_all_subsystems", [](AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
            return &agent;
        }, "Enable all subsystems (fluent interface)")
        
        .def("__repr__", [](const AgentBrain& agent) {
            auto brain = agent.getBrain();
            return "<AgentBrain neurons=" + std::to_string(brain.getTotalNeuronCount()) + ">";
        });
}
