// pybind11_brain_module.cpp - Brain module bindings implementation

#include "pybind11_brain_module.h"
#include <pybind11/stl.h>
#include <pybind11/functional.h>

namespace nlm {

void bind_brain_module(py::module& m) {
    // Enhanced Brain class with comprehensive bindings
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
    
    The core neural simulation brain that implements:
    - Leaky Integrate-and-Fire (LIF) neuron dynamics
    - Event-driven spike propagation with delays
    - Multiple plasticity mechanisms (STDP, Hebbian, structural)
    - Neuromodulation systems (dopamine, curiosity, novelty)
    - Developmental processes and maturation
    - Working memory and episodic memory systems
    - Prediction and planning capabilities
    - Complex cognitive functions
    
    Brain features:
    - Real-time neural simulation
    - Experience-dependent learning
    - Adaptive behavior through plasticity
    - Internal state representation
    - Action selection and planning
    - Reward-based learning
    - Curiosity-driven exploration
    
    Brain capabilities:
    - Inject sensory input from environment
    - Process neural activity and generate actions
    - Maintain internal state and memory
    - Learn from experience and rewards
    - Adapt through developmental processes
    - Simulate realistic neural dynamics
    "pbdoc")
    .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
         "Initialize brain with configuration (config is required)")
    .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
    .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
         py::arg("currentStep"), "Perform a simulation step")
    .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
         py::arg("currentStep"), py::arg("currentTime"), 
         "Perform a simulation step with timestamp")
    .def("receive_sensory_input", &Brain::receiveSensoryInput,
         py::arg("input"), "Inject sensory input into the brain")
    .def("inject_current", &Brain::injectCurrent,
         py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
    .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
         py::arg("type"), py::arg("current"), 
         "Inject current into all neurons of a specific type")
    .def("produce_action", &Brain::produceAction, "Produce motor action based on neural activity")
    .def("reset", &Brain::reset, "Reset brain state")
    .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
    .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
    .def("add_region", &Brain::addRegion, py::arg("name") = "",
         "Add a new neural region")
    .def("get_region", &Brain::getRegion,
         py::return_value_policy::reference_internal, "Get a region by ID")
    .def("get_region_count", &Brain::getRegionCount, "Get the number of regions")
    .def("get_region_ids", &Brain::getRegionIds, "Get all region IDs")
    .def("get_regions", &Brain::getRegions,
         py::return_value_policy::reference_internal, "Get all regions")
    .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
         "Get total neuron count across all regions")
    .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
         "Get total synapse count across all regions")
    .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
         "Get count of active neurons")
    .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
         "Get count of currently firing neurons")
    .def("get_average_firing_rate", &Brain::getAverageFiringRate,
         "Get average firing rate across all neurons")
    .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
         "Get excitation/inhibition balance ratio")
    .def("get_total_spike_count", &Brain::getTotalSpikeCount,
         "Get total spike count")
    .def("get_developmental_stage", &Brain::getDevelopmentalStage,
         "Get current developmental stage")
    .def("set_developmental_stage", &Brain::setDevelopmentalStage,
         py::arg("stage"), "Set developmental stage")
    .def("get_config", &Brain::getConfig,
         py::return_value_policy::reference_internal, "Get the configuration")
    .def("log_status", &Brain::logStatus, "Log brain status")
    .def("get_spike_system", [](const Brain& self) {
        return self.getSpikeSystem();
    }, "Get spike system for neural dynamics analysis")
    .def("get_development_system", [](const Brain& self) {
        return self.getDevelopmentSystem();
    }, "Get development system for developmental processes")
    .def("get_plasticity_rules", [](const Brain& self) {
        return self.getPlasticityRules();
    }, "Get all plasticity rules")
    .def("get_neuromodulators", [](const Brain& self) {
        return self.getNeuromodulators();
    }, "Get all neuromodulators")
    .def("get_memory_systems", [](const Brain& self) {
        return self.getMemorySystems();
    }, "Get all memory systems")
    .def("get_cognition_systems", [](const Brain& self) {
        return self.getCognitionSystems();
    }, "Get all cognition systems")
    .def("get_prediction_systems", [](const Brain& self) {
        return self.getPredictionSystems();
    }, "Get all prediction systems")
    .def("get_motor_systems", [](const Brain& self) {
        return self.getMotorSystems();
    }, "Get all motor systems")
    .def("get_environment_systems", [](const Brain& self) {
        return self.getEnvironmentSystems();
    }, "Get all environment systems")
    .def("get_plasticity_by_type", [](const Brain& self, SynapseType type) {
        auto rules = self.getPlasticityRules();
        std::vector<PlasticityRule*> matching_rules;
        for (auto* rule : rules) {
            if (rule && rule->get_synapse_type() == type) {
                matching_rules.push_back(rule);
            }
        }
        return matching_rules;
    }, py::arg("type"), "Get plasticity rules for specific synapse type")
    .def("get_neuromodulators_by_type", [](const Brain& self, NeuromodType type) {
        auto neuromodulators = self.getNeuromodulators();
        std::vector<Neuromodulator*> matching_neuromodulators;
        for (auto* neuromod : neuromodulators) {
            if (neuromod && neuromod->get_type() == type) {
                matching_neuromodulators.push_back(neuromod);
            }
        }
        return matching_neuromodulators;
    }, py::arg("type"), "Get neuromodulators for specific type")
    .def("enable_all_subsystems", [](Brain& self) {
        // Enable all learning and adaptive subsystems
        auto plasticity_rules = self.getPlasticityRules();
        for (auto* rule : plasticity_rules) {
            if (rule) {
                rule->enable(true);
            }
        }
        
        auto neuromodulators = self.getNeuromodulators();
        for (auto* neuromod : neuromodulators) {
            if (neuromod) {
                neuromod->enable(true);
            }
        }
        
        // Enable development
        if (auto* dev_system = self.getDevelopmentSystem()) {
            dev_system->enable(true);
        }
        
        return true;
    }, "Enable all adaptive and learning subsystems")
    .def("disable_all_subsystems", [](Brain& self) {
        // Disable all learning and adaptive subsystems
        auto plasticity_rules = self.getPlasticityRules();
        for (auto* rule : plasticity_rules) {
            if (rule) {
                rule->enable(false);
            }
        }
        
        auto neuromodulators = self.getNeuromodulators();
        for (auto* neuromod : neuromodulators) {
            if (neuromod) {
                neuromod->enable(false);
            }
        }
        
        // Disable development
        if (auto* dev_system = self.getDevelopmentSystem()) {
            dev_system->enable(false);
        }
        
        return true;
    }, "Disable all adaptive and learning subsystems")
    .def("get_system_status", [](const Brain& self) {
        py::dict status;
        
        auto regions = self.getRegions();
        status["region_count"] = regions.size();
        status["total_neurons"] = self.getTotalNeuronCount();
        status["total_synapses"] = self.getTotalSynapseCount();
        status["active_neurons"] = self.getActiveNeuronCount();
        status["firing_neurons"] = self.getFiringNeuronCount();
        status["total_spikes"] = self.getTotalSpikeCount();
        status["average_firing_rate"] = self.getAverageFiringRate();
        status["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
        
        status["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
        
        auto plasticity_rules = self.getPlasticityRules();
        int enabled_plasticity = 0;
        for (auto* rule : plasticity_rules) {
            if (rule && rule->is_enabled()) {
                enabled_plasticity++;
            }
        }
        status["enabled_plasticity_rules"] = enabled_plasticity;
        status["total_plasticity_rules"] = plasticity_rules.size();
        
        auto neuromodulators = self.getNeuromodulators();
        int enabled_neuromodulators = 0;
        for (auto* neuromod : neuromodulators) {
            if (neuromod && neuromod->is_enabled()) {
                enabled_neuromodulators++;
            }
        }
        status["enabled_neuromodulators"] = enabled_neuromodulators;
        status["total_neuromodulators"] = neuromodulators.size();
        
        return status;
    }, "Get comprehensive brain system status")
    .def("compute_network_metrics", [](const Brain& self) {
        py::dict metrics;
        
        metrics["neuron_density"] = static_cast<float>(self.getTotalNeuronCount()) / 1000.0f; // Normalized
        metrics["synapse_density"] = static_cast<float>(self.getTotalSynapseCount()) / 
                                    (self.getTotalNeuronCount() * 10); // Normalized
        
        metrics["firing_rate"] = self.getAverageFiringRate();
        metrics["activity_level"] = static_cast<float>(self.getActiveNeuronCount()) /
                                    self.getTotalNeuronCount();
        
        metrics["plasticity_strength"] = 0.5f; // Placeholder - would compute from actual plasticity rules
        metrics["learning_rate"] = 0.01f; // Placeholder
        
        metrics["network_efficiency"] = metrics["firing_rate"] * metrics["activity_level"];
        
        return metrics;
    }, "Compute network-wide metrics and statistics")
    .def("export_brain_state", [](const Brain& self) {
        py::dict state;
        
        state["neuron_count"] = self.getTotalNeuronCount();
        state["synapse_count"] = self.getTotalSynapseCount();
        state["spike_count"] = self.getTotalSpikeCount();
        state["firing_neuron_count"] = self.getFiringNeuronCount();
        state["average_firing_rate"] = self.getAverageFiringRate();
        state["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
        state["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
        
        // Note: Region export would require additional API methods
        return state;
    }, "Export brain state as Python dictionary")
    .def("import_brain_state", [](Brain& self, const py::dict& state) {
        // Note: Brain state import would require additional API methods
        // This is a placeholder for future implementation
        return true;
    }, py::arg("state"), "Import brain state from Python dictionary (placeholder)");
}

} // namespace nlm
