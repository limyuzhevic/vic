#include "brain.h"
#include "error_handling.h"
#include <pybind11/stl.h>

namespace py = pybind11;
namespace nlm {

void include_brain(py::module_& m) {
    // Brain class with enhanced Pythonic features
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
    
    The Brain class is the central neural simulation component of NLM.
    It manages neural populations, synaptic connections, and the overall neural dynamics.
    
    The brain processes sensory input, produces motor actions, and maintains internal state
    through complex neural computations and learning mechanisms.
    
    Examples:
        >>> config = createDefaultConfig()
        >>> brain = Brain(config)
        >>> brain.initialize()
        >>> brain.step(current_step)
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a brain with configuration. Raises RuntimeError if config is invalid.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Must be called before simulation.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with the given step number.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with the given step number and timestamp.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain for processing. Raises RuntimeError if invalid.")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron. Current should be in range [-1.0, 1.0].")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Returns Action object.")
        .def("reset", &Brain::reset,
             "Reset brain to initial state (for new episode)")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Raises RuntimeError if save fails.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Raises RuntimeError if load fails.")
        // Region management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region with optional name. Returns region ID.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             "Get a region by ID. Returns Region object. Raises RuntimeError if region not found.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of neural regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs as list")
        .def("getRegions", &Brain::getRegions,
             "Get all regions as list")
        // Statistics and monitoring
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of currently active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons (Hz)")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count since initialization")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             "Get the configuration object")
        .def("logStatus", &Brain::logStatus,
             "Log brain status to console (debug mode)")
        // Enhanced statistics and convenience methods
        .def("getBrainStatistics", [](const Brain& self) {
            std::ostringstream oss;
            oss << "Brain Statistics:" << std::endl;
            oss << "  Regions: " << self.getRegionCount() << std::endl;
            oss << "  Total Neurons: " << self.getTotalNeuronCount() << std::endl;
            oss << "  Total Synapses: " << self.getTotalSynapseCount() << std::endl;
            oss << "  Active Neurons: " << self.getActiveNeuronCount() << std::endl;
            oss << "  Firing Neurons: " << self.getFiringNeuronCount() << std::endl;
            oss << "  Average Firing Rate: " << self.getAverageFiringRate() << " Hz" << std::endl;
            oss << "  E/I Ratio: " << self.getExcitationInhibitionRatio() << std::endl;
            oss << "  Total Spikes: " << self.getTotalSpikeCount() << std::endl;
            oss << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage());
            return oss.str();
        }, "Get detailed statistics as formatted string")
        .def("getPopulationStats", [](const Brain& self) {
            // This would require accessing regions and their populations
            return std::vector<std::string>(); // Placeholder
        }, "Get population statistics (placeholder)")
        .def("getConnectivityStats", [](const Brain& self) {
            // This would require analyzing synaptic connections
            return std::make_tuple(self.getTotalSynapseCount(), 
                                 self.getTotalNeuronCount()); // Placeholder
        }, "Get connectivity statistics as (synapseCount, neuronCount)")
        .def("getActivityLevel", [](const Brain& self) {
            int active = self.getActiveNeuronCount();
            int total = self.getTotalNeuronCount();
            return total > 0 ? static_cast<double>(active) / total : 0.0;
        }, "Get activity level as fraction of active neurons")
        .def("getExcitatoryInhibitoryBalance", [](const Brain& self) {
            double ratio = self.getExcitationInhibitionRatio();
            if (ratio > 2.0) return "Excitation Dominated";
            if (ratio < 0.5) return "Inhibition Dominated";
            return "Balanced";
        }, "Get a qualitative description of E/I balance")
        .def("isStable", [](const Brain& self) {
            // Add stability criteria here
            double activity = self.getActivityLevel();
            double ratio = self.getExcitationInhibitionRatio();
            return activity < 0.8 && ratio > 0.2 && ratio < 2.0;
        }, "Check if the brain is in a stable state")
        // Factory methods and helpers
        .def("createBrainWithConfig", [](const std::shared_ptr<Config>& config) {
            return Brain(config);
        }, "Factory method to create brain with config", py::arg("config")),
        .def("createDefaultBrain", []() {
            auto config = std::make_shared<Config>();
            return Brain(config);
        }, "Create a brain with default configuration"),
        .def("clone", &Brain::clone,
             "Create a deep copy of this brain"),
        .def("copyFrom", [](Brain& self, const Brain& other) {
            self.reset();
            // Add brain state copy logic here
            return *this;
        }, "Copy brain state from another brain");

    // Brain factory functions
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, "Create a new brain with configuration", py::arg("config"));

    m.def("createDefaultBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration");

    m.def("createBrainWithName", [](std::string name) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config.set("brain.name", name);
        return std::make_shared<Brain>(config);
    }, "Create a brain with specified name", py::arg("name"));

    // Brain utility functions
    m.def("initializeBrain", [](Brain& brain) {
        brain.initialize();
    }, "Initialize a brain object", py::arg("brain"));

    m.def("stepBrain", [](Brain& brain, SimulationStep step) {
        brain.step(step);
    }, "Perform one brain step", py::arg("brain"), py::arg("step"));

    m.def("stepBrainWithTime", [](Brain& brain, SimulationStep step, Timestamp time) {
        brain.step(step, time);
    }, "Perform one brain step with timestamp", py::arg("brain"), py::arg("step"), py::arg("time"));

    // Brain validation and error handling
    m.def("validateBrain", [](const Brain& brain) {
        if (brain.getTotalNeuronCount() <= 0) {
            throw std::runtime_error("Brain has no neurons");
        }
        return true;
    }, "Validate brain and raise RuntimeError if invalid", py::arg("brain"));

    // Brain serialization helpers
    m.def("exportBrain", [](const Brain& brain, const std::string& filepath) {
        brain.save(filepath);
    }, "Export brain to file", py::arg("brain"), py::arg("filepath"));

    m.def("importBrain", [](Brain& brain, const std::string& filepath) {
        brain.load(filepath);
    }, "Import brain from file", py::arg("brain"), py::arg("filepath"));

    // Brain simulation helpers
    m.def("simulateBrain", [](Brain& brain, SimulationStep maxSteps = 1000) {
        for (SimulationStep i = 0; i < maxSteps; ++i) {
            brain.step(i);
        }
        return brain.getTotalSpikeCount();
    }, "Simulate brain for specified steps", py::arg("brain"), py::arg("maxSteps") = 1000);

    m.def("simulateBrainWithInput", [](Brain& brain, const std::vector<SensoryInput>& inputs, 
                                     SimulationStep maxSteps = 1000) {
        for (SimulationStep i = 0; i < maxSteps; ++i) {
            if (i < inputs.size()) {
                brain.receiveSensoryInput(inputs[i]);
            }
            brain.step(i);
        }
        return brain.getTotalSpikeCount();
    }, "Simulate brain with sensory inputs", py::arg("brain"), py::arg("inputs"), py::arg("maxSteps") = 1000);

} // namespace nlm