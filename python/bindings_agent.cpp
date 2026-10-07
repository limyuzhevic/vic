// Agent system bindings

#include "bindings.h"

namespace nlm {

void bind_agent_module(py::module_& m) {
    // AgentBody class (already defined in world.cpp, but included here for completeness)
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state and physics information.
        
        Represents the physical body of an agent in the NLM world:
        - Position and orientation in 2D space
        - Velocity and angular velocity for movement
        - Energy and health status
        - Age and movement state
        - Last action timing for behavior
        
        Body properties:
        - Position: (x, y) coordinates in world space
        - Orientation: Rotation angle in radians
        - Velocity: Linear velocity vector (vx, vy)
        - Angular velocity: Rotational speed
        - Energy: Current energy level (0-100%)
        - Health: Current health level (0-100%)
        - Age: Developmental age in simulation time
        - Movement state: Whether agent is moving or turning
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate")
        .def_readwrite("y", &AgentBody::y, "Y coordinate")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation angle (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity (rad/s)")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level (0-100%)")
        .def_readwrite("health", &AgentBody::health, "Current health level (0-100%)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation time")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Timestamp of last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        This class provides the interface between the neural brain and the external world:
        - Processes sensory input from the environment
        - Decodes neural activity into motor commands
        - Applies reward modulation based on outcomes
        - Manages development updates
        - Provides access to brain internal state
        
        Key functionality:
        - Sensory processing and neural injection
        - Motor command decoding
        - Reward-based learning modulation
        - Developmental stage management
        - Neuromodulation level control
        - Curiosity and novelty monitoring
        - Prediction error tracking
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world environment")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data combining all sensory modalities.
        
        This class represents the complete sensory input from the environment:
        - Vision: Visual scene data
        - Touch: Physical contact and proximity
        - Internal: Internal brain state signals
        - Audio: Sound information
        - Proprioception: Body position and movement
        
        Sensory percepts provide:
        - Access to all sensory modalities
        - Individual modality getters/setters
        - Timestamp tracking
        - Complete sensory snapshot for processing
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             py::return_value_policy::reference_internal,
             "Get vision sensory input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision sensory input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get vision image width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get vision image height")
        .def("getTouch", &SensoryPercept::getTouch,
             py::return_value_policy::reference_internal,
             "Get touch sensory input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch sensory input")
        .def("getInternal", &SensoryPercept::getInternal,
             py::return_value_policy::reference_internal,
             "Get internal signals sensory input")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals sensory input")
        .def("getProprioception", &SensoryPercept::getProprioception,
             py::return_value_policy::reference_internal,
             "Get proprioception sensory input")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception sensory input")
        .def("getAudio", &SensoryPercept::getAudio,
             py::return_value_policy::reference_internal,
             "Get audio sensory input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio sensory input")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as combined vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp");
}

} // namespace nlm