// AgentBrain class binding implementation
// This file provides Python bindings for the AgentBrain class with advanced features

#include "bindings/pybind_common.h"
#include <string>

namespace py = pybind11;

void bindAgentBrainClass(py::module& m) {
    py::class_<nlm::AgentBrain, std::shared_ptr<nlm::AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"))
        .def("initialize", &nlm::AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &nlm::AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &nlm::AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &nlm::AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &nlm::AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &nlm::AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &nlm::AgentBrain::isCuriosityEnabled);

    // Advanced monitoring method
    m.def("getAgentStats", [](const std::shared_ptr<nlm::AgentBrain>& agent) {
        // Create a comprehensive statistics dictionary
        py::dict stats;
        stats["curiosity"] = agent->getCuriosityLevel();
        stats["novelty"] = agent->getNoveltyLevel();
        stats["neuromodulation"] = agent->getNeuromodulationLevel();
        stats["prediction_error"] = agent->getPredictionError();
        stats["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
        stats["reward_modulation_enabled"] = agent->isRewardModulationEnabled();
        stats["structural_plasticity_enabled"] = agent->isStructuralPlasticityEnabled();
        stats["development_enabled"] = agent->isDevelopmentEnabled();
        stats["curiosity_enabled"] = agent->isCuriosityEnabled();
        
        auto brain = agent->getBrain();
        if (brain) {
            stats["total_neurons"] = brain->getTotalNeuronCount();
            stats["total_synapses"] = brain->getTotalSynapseCount();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["total_spikes"] = brain->getTotalSpikeCount();
        }
        
        return stats;
    }, py::arg("agent"), "Get comprehensive agent statistics");
}
