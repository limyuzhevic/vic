#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "../python/nlm/core/binding_helpers.hpp"

namespace py = pybind11;
namespace nlm {

void bindAgentTypes(py::module_& m) {
    m.doc() = R"pbdoc(
        Agent Types
        -----------
        Types and classes related to agent behavior and control.
    )pbdoc";
}

void bindAgentClasses(py::module_& m) {
    // Enhanced AgentBrain class with better error handling and advanced features
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        Provides interface for agents to interact with the brain simulation,
        processing sensory input and producing motor actions.
        
        The AgentBrain acts as a bridge between the neural brain and the world,
        handling sensory processing, motor decoding, and neuromodulation.
        
        Examples:
            >>> import pynlm as nlm
            >>> brain = nlm.Brain(config)
            >>> agent_brain = nlm.AgentBrain(brain)
            >>> agent_brain.initialize(world)
            >>> 
            >>> # Process sensory input
            >>> percept = world.getSensoryPercept()
            >>> agent_brain.processSensoryInput(percept)
            >>> 
            >>> # Get action
            >>> action = agent_brain.decodeMotorCommand()
            >>> print(f"Action: {action.getName()}")
            >>> 
            >>> # Get brain state information
            >>> print(f"Neuromodulation: {agent_brain.getNeuromodulationLevel()}")
            >>> print(f"Curiosity: {agent_brain.getCuriosityLevel()}")
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface with the specified brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent brain with the world simulation")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size (dimensions)")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size (dimensions)")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation with prediction error")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system with specified timestep")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level (affects learning rate)")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level (drives exploration)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level (sensitivity to new information)")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error from neuromodulation system")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain object")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity (rewiring)")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable developmental processes")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("getSensoryStats", [](const AgentBrain& self) {
            py::dict stats;
            stats["input_size"] = self.getSensoryInputSize();
            stats["output_size"] = self.getMotorOutputSize();
            return stats;
        }, "Get sensory and motor statistics as a dictionary");

    // Enhanced SensoryPercept class with better API
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data for agent processing.
        
        Contains all sensory inputs collected by the agent in a single time step,
        including vision, touch, internal signals, proprioception, and audio.
        
        The percept provides easy access to different sensory modalities
        and can be used for agent decision making.
        
        Examples:
            >>> import pynlm as nlm
            >>> percept = nlm.SensoryPercept()
            >>> 
            >>> # Create vision input
            >>> vision = nlm.Vision(10, 10, 3)
            >>> data = list(range(10 * 10 * 3))
            >>> vision.setData(data)
            >>> percept.setVision(vision)
            >>> 
            >>> # Access vision data
            >>> vision_width = percept.getVisionWidth()
            >>> vision_height = percept.getVisionHeight()
            >>> 
            >>> # Create internal signals
            >>> internal = nlm.InternalSignals()
            >>> internal.addSignal(0.5)
            >>> internal.addSignal(0.8)
            >>> percept.setInternal(internal)
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get the vision input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set the vision input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get the width of vision input")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get the height of vision input")
        .def("getVisionChannels", [](const SensoryPercept& self) {
            auto vision = self.getVision();
            return vision.getChannels();
        }, "Get the number of channels in the vision input")
        .def("getTouch", &SensoryPercept::getTouch, "Get the touch input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set the touch input")
        .def("getInternal", &SensoryPercept::getInternal, "Get the internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set the internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get the proprioception input")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set the proprioception input")
        .def("getAudio", &SensoryPercept::getAudio, "Get the audio input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set the audio input")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals as a dictionary")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get the percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set the percept timestamp")
        .def("getSignalSummary", [](const SensoryPercept& self) {
            py::dict summary;
            auto vision = self.getVision();
            summary["vision_width"] = vision.getWidth();
            summary["vision_height"] = vision.getHeight();
            summary["vision_channels"] = vision.getChannels();
            summary["has_touch"] = !self.getTouch().isEmpty();
            summary["internal_signals"] = self.getInternal().getSignalCount();
            summary["has_proprioception"] = !self.getProprioception().isEmpty();
            summary["has_audio"] = !self.getAudio().isEmpty();
            return summary;
        }, "Get a summary of all sensory inputs");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state in the world.
        
        Represents the physical state of the agent in the simulation environment,
        including position, orientation, velocity, and physiological state.
        
        The agent body contains both position information and physiological state
        that affects agent behavior and capabilities.
        
        Examples:
            >>> import pynlm as nlm
            >>> body = nlm.AgentBody()
            >>> body.x = 50.0
            >>> body.y = 30.0
            >>> body.orientation = 1.57  # radians
            >>> body.energy = 100.0
            >>> body.health = 95.0
            >>> 
            >>> # Check if agent is moving
            >>> if body.isMoving:
            ...     print("Agent is moving")
            >>> 
            >>> # Get velocity
            >>> velocity = math.sqrt(body.velocityX**2 + body.velocityY**2)
            >>> print(f"Speed: {velocity}")
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position of the agent")
        .def_readwrite("y", &AgentBody::y, "Y position of the agent")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation angle in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X component of velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y component of velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity in radians per second")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-100)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-100)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether the agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether the agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Timestamp of last action")
        .def("getPosition", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, double x, double y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getVelocity", [](const AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("setVelocity", [](AgentBody& self, double vx, double vy) {
            self.velocityX = vx;
            self.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"), "Set velocity")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get speed (magnitude of velocity)")
        .def("getDirection", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get direction (heading) in radians")
        .def("isHealthy", [](const AgentBody& self) {
            return self.health > 50.0;
        }, "Check if agent is healthy")
        .def("isCritical", [](const AgentBody& self) {
            return self.health < 20.0 || self.energy < 10.0;
        }, "Check if agent is in critical condition")
        .def("reset", &AgentBody::reset, "Reset the agent body state to defaults")
        .def("copyTo", [](const AgentBody& self, AgentBody& other) {
            other = self;
            return other;
        }, py::arg("other"), "Copy agent state to another AgentBody object");
}

void bindAgentFactories(py::module_& m) {
    // Agent factory functions
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface");
    
    m.def("createDefaultAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        // Enable all features by default
        agentBrain->enableRewardModulation(true);
        agentBrain->enableStructuralPlasticity(true);
        agentBrain->enableDevelopment(true);
        agentBrain->enableCuriosity(true);
        return agentBrain;
    }, py::arg("brain"), "Create an agent brain with all features enabled");
    
    m.def("createSensoryPercept", []() -> std::shared_ptr<SensoryPercept> {
        return std::make_shared<SensoryPercept>();
    }, "Create a new sensory percept");
    
    m.def("createAgentBody", []() -> std::shared_ptr<AgentBody> {
        return std::make_shared<AgentBody>();
    }, "Create a new agent body state");
}

void bindAgentAdvanced(py::module_& m) {
    // Advanced agent APIs
    m.def("batchProcessSensoryInput", [](std::shared_ptr<AgentBrain> agentBrain,
                                       const std::vector<std::shared_ptr<SensoryPercept>>& percepts) {
        if (!agentBrain) {
            throw BrainError("AgentBrain cannot be null");
        }
        for (const auto& percept : percepts) {
            agentBrain->processSensoryInput(*percept);
        }
    }, py::arg("agent_brain"), py::arg("percepts"),
        "Process multiple sensory inputs in batch");
    
    m.def("createAgentFromBrain", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        return agentBrain;
    }, py::arg("brain"), py::arg("world"),
        "Create and initialize an agent brain with a world");
    
    m.def("getAgentStats", [](const std::shared_ptr<AgentBrain>& agentBrain) {
        if (!agentBrain) {
            throw BrainError("AgentBrain cannot be null");
        }
        py::dict stats;
        stats["sensory_input_size"] = agentBrain->getSensoryInputSize();
        stats["motor_output_size"] = agentBrain->getMotorOutputSize();
        stats["reward_modulation_enabled"] = agentBrain->isRewardModulationEnabled();
        stats["structural_plasticity_enabled"] = agentBrain->isStructuralPlasticityEnabled();
        stats["development_enabled"] = agentBrain->isDevelopmentEnabled();
        stats["curiosity_enabled"] = agentBrain->isCuriosityEnabled();
        stats["neuromodulation_level"] = agentBrain->getNeuromodulationLevel();
        stats["curiosity_level"] = agentBrain->getCuriosityLevel();
        stats["novelty_level"] = agentBrain->getNoveltyLevel();
        stats["prediction_error"] = agentBrain->getPredictionError();
        return stats;
    }, py::arg("agent_brain"), "Get comprehensive agent statistics");
}

} // namespace nlm