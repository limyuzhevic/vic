#include "agentbrain.h"
#include "error_handling.h"
#include <pybind11/stl.h>

namespace py = pybind11;
namespace nlm {

void include_agentbrain(py::module_& m) {
    // AgentBrain class with enhanced Pythonic features
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
    
    The AgentBrain class provides a high-level interface between the neural brain
    and the world simulation. It processes sensory percepts, decodes motor commands,
    and applies reward-based neuromodulation and developmental processes.
    
    AgentBrain manages the interaction between neural computation and embodied action,
    making it the key component for agent behavior in the NLM framework.
    
    Examples:
        >>> brain = createBrain(createDefaultConfig())
        >>> agent_brain = AgentBrain(brain)
        >>> percept = world.getSensoryPercept()
        >>> agent_brain.processSensoryInput(percept)
        >>> action = agent_brain.decodeMotorCommand()
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain with underlying neural brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world simulation for sensory processing and action execution")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get the expected sensory input size (number of input neurons)")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get the expected motor output size (number of output neurons)")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain. Raises RuntimeError if invalid.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command. Returns Action object.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation to adjust neural activity. Reward range [-1, 1].")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update developmental system based on neural activity and environmental interaction.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level (0-1)")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level (0-1)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level (0-1)")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error signal from the brain")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode (clear memory and neuromodulation)")
        .def("getBrain", &AgentBrain::getBrain,
             "Get the underlying brain object")
        // Neuromodulation controls
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             "Enable or disable reward-based neuromodulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             "Enable or disable structural plasticity (synaptic rewiring)")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
             "Enable or disable developmental processes")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             "Enable or disable curiosity-driven exploration")
        // Status queries
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development processes are enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        // Enhanced statistics and monitoring
        .def("getAgentBrainStatistics", [](const AgentBrain& self) {
            std::ostringstream oss;
            oss << "AgentBrain Statistics:" << std::endl;
            oss << "  Sensory Input Size: " << self.getSensoryInputSize() << std::endl;
            oss << "  Motor Output Size: " << self.getMotorOutputSize() << std::endl;
            oss << "  Neuromodulation Level: " << self.getNeuromodulationLevel() << std::endl;
            oss << "  Curiosity Level: " << self.getCuriosityLevel() << std::endl;
            oss << "  Novelty Level: " << self.getNoveltyLevel() << std::endl;
            oss << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage());
            return oss.str();
        }, "Get detailed statistics as formatted string")
        .def("getModulationStats", [](const AgentBrain& self) {
            std::vector<std::string> mods;
            if (self.isRewardModulationEnabled()) mods.push_back("RewardModulation");
            if (self.isStructuralPlasticityEnabled()) mods.push_back("StructuralPlasticity");
            if (self.isDevelopmentEnabled()) mods.push_back("Development");
            if (self.isCuriosityEnabled()) mods.push_back("Curiosity");
            return mods;
        }, "Get list of enabled modulation processes")
        .def("getNeuromodulationSummary", [](const AgentBrain& self) {
            std::ostringstream oss;
            double level = self.getNeuromodulationLevel();
            oss << "Neuromodulation: " << (level > 0.0 ? "Active (Level: " + std::to_string(level) + ")" : "Inactive");
            return oss.str();
        }, "Get neuromodulation status summary")
        .def("getDevelopmentStatus", [](const AgentBrain& self) {
            int stage = static_cast<int>(self.getDevelopmentalStage());
            std::string stageName;
            switch (self.getDevelopmentalStage()) {
                case DevelopmentalStage::Initial: stageName = "Initial"; break;
                case DevelopmentalStage::CriticalPeriod: stageName = "CriticalPeriod"; break;
                case DevelopmentalStage::Maturation: stageName = "Maturation"; break;
                case DevelopmentalStage::Adult: stageName = "Adult"; break;
                case DevelopmentalStage::Aging: stageName = "Aging"; break;
            }
            return "Development Stage: " + stageName + " (Level: " + std::to_string(stage) + ")";
        }, "Get development stage and level")
        .def("isReady", [](const AgentBrain& self) {
            return self.getSensoryInputSize() > 0 && self.getMotorOutputSize() > 0;
        }, "Check if agent brain is properly initialized")
        .def("getPerformanceIndicators", [](const AgentBrain& self) {
            std::vector<double> indicators;
            indicators.push_back(self.getNeuromodulationLevel());
            indicators.push_back(self.getCuriosityLevel());
            indicators.push_back(self.getNoveltyLevel());
            return indicators;
        }, "Get performance indicators as list")
        // Factory methods
        .def("createAgentBrain", [](const std::shared_ptr<Brain>& brain) {
            return AgentBrain(brain);
        }, "Factory method to create agent brain", py::arg("brain")),
        .def("createDefaultAgentBrain", []() {
            auto brain = std::make_shared<Brain>(std::make_shared<Config>());
            return AgentBrain(brain);
        }, "Create an agent brain with default configuration"),
        .def("createConfiguredAgentBrain", [](std::shared_ptr<Config> config) {
            auto brain = std::make_shared<Brain>(config);
            return AgentBrain(brain);
        }, "Create an agent brain with specified configuration", py::arg("config"));

    // AgentBrain factory functions
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, "Create a new agent brain interface", py::arg("brain"));

    m.def("createDefaultAgentBrain", []() -> std::shared_ptr<AgentBrain> {
        auto brain = std::make_shared<Brain>(std::make_shared<Config>());
        return std::make_shared<AgentBrain>(brain);
    }, "Create an agent brain with default configuration");

    // AgentBrain utility functions
    m.def("initializeAgentBrain", [](AgentBrain& agentBrain, SimpleWorld& world) {
        agentBrain.initialize(world);
    }, "Initialize an agent brain with world", py::arg("agentBrain"), py::arg("world"));

    m.def("processSensoryInput", [](AgentBrain& agentBrain, const SensoryPercept& percept) {
        agentBrain.processSensoryInput(percept);
    }, "Process sensory input for agent brain", py::arg("agentBrain"), py::arg("percept"));

    m.def("decodeAndApplyAction", [](AgentBrain& agentBrain, SimpleWorld& world) {
        auto action = agentBrain.decodeMotorCommand();
        world.applyMotorCommand(action, 1.0f);
        return action;
    }, "Decode motor command and apply to world", py::arg("agentBrain"), py::arg("world"));

    // AgentBrain validation and error handling
    m.def("validateAgentBrain", [](const AgentBrain& agentBrain) {
        if (!agentBrain.isReady()) {
            throw std::runtime_error("AgentBrain is not properly initialized");
        }
        return true;
    }, "Validate agent brain and raise RuntimeError if invalid", py::arg("agentBrain"));

    // AgentBrain simulation helpers
    m.def("runAgentEpisode", [](AgentBrain& agentBrain, SimpleWorld& world, int maxSteps = 1000) {
        agentBrain.initialize(world);
        for (int i = 0; i < maxSteps; ++i) {
            auto percept = world.getSensoryPercept();
            agentBrain.processSensoryInput(percept);
            auto action = agentBrain.decodeMotorCommand();
            world.applyMotorCommand(action, 1.0f);
            world.update(1.0f);
        }
        return agentBrain.getDevelopmentalStage();
    }, "Run a complete agent episode", py::arg("agentBrain"), py::arg("world"), py::arg("maxSteps") = 1000);

    // AgentBrain convenience functions
    m.def("getAgentBrainStatus", [](const AgentBrain& agentBrain) {
        std::ostringstream oss;
        oss << "AgentBrain Status:" << std::endl;
        oss << "  Ready: " << (agentBrain.isReady() ? "Yes" : "No") << std::endl;
        oss << "  Reward Modulation: " << (agentBrain.isRewardModulationEnabled() ? "Enabled" : "Disabled") << std::endl;
        oss << "  Structural Plasticity: " << (agentBrain.isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
        oss << "  Development: " << (agentBrain.isDevelopmentEnabled() ? "Enabled" : "Disabled") << std::endl;
        oss << "  Curiosity: " << (agentBrain.isCuriosityEnabled() ? "Enabled" : "Disabled");
        return oss.str();
    }, "Get a formatted status string for agent brain", py::arg("agentBrain"));

} // namespace nlm