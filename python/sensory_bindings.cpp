// Python bindings for NLM sensory and motor systems
// This file contains bindings for sensory processing and motor control

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/motor/MotorSystem.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"

namespace py = pybind11;
namespace nlm {

void bindSensoryInput(py::module& m);
void bindVision(py::module& m);
void bindAudio(py::module& m);
void bindInternalSignals(py::module& m);
void bindAction(py::module& m);
void bindMotorSystem(py::module& m);
void bindEnvironment(py::module& m);
void bindObservation(py::module& m);

void include_sensory_bindings(py::module& m) {
    bindSensoryInput(m);
    bindVision(m);
    bindAudio(m);
    bindInternalSignals(m);
}

void include_motor_bindings(py::module& m) {
    bindAction(m);
    bindMotorSystem(m);
}

void include_environment_bindings(py::module& m) {
    bindEnvironment(m);
    bindObservation(m);
}

void bindSensoryInput(py::module& m) {
    // SensoryInput class - Base class for sensory input
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input in NLM system.
        
        The SensoryInput class provides the foundation for all sensory modalities
        in the NLM system:
        - Vision: Visual information from camera sensors
        - Audio: Sound information from microphones
        - Internal: Internal body state and signals
        - Touch: Contact and proximity information
        - Proprioception: Body position and orientation
        
        SensoryInput enables the brain to perceive and interact with the world
        by providing processed information about the environment.
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create vision input
            >>> vision = pynlm.Vision(width=8, height=8)
            >>> vision.setData([0.1, 0.9, 0.2, 0.8, 0.3, 0.7, 0.4, 0.6])
            >>> 
            >>> # Create internal signals
            >>> internal = pynlm.InternalSignals()
            >>> internal.addSignal(100.0)  # Energy
            >>> internal.addSignal(0.8)   # Health
            >>> 
            >>> # Get sensory input type
            >>> print(f"Vision type: {vision.getType()}")
            >>> print(f"Vision dimensions: {vision.getDimensions()}")
            >>> print(f"Internal signal count: {internal.getSignals().size()}")
        
        Methods:
            getType(): Get sensory input type
            getData(): Get raw data as vector
            getDimensions(): Get dimensionality
            getTimestamp(): Get timestamp
            setTimestamp(timestamp): Set timestamp
        
        Attributes:
            type: Sensory input type
            dimensions: Number of dimensions
            timestamp: Time when data was captured
            
        See Also:
            Vision, Audio, InternalSignals, SimpleWorld, AgentBrain
    )pbdoc")
        .def("getType", &SensoryInput::getType,
             "Get sensory input type")
        .def("getData", &SensoryInput::getData,
             "Get raw data as vector")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: type=" + std::to_string(static_cast<int>(input.getType())) +
                   ", dims=" + std::to_string(input.getDimensions()) + ">";
        }, "String representation of sensory input");
}

void bindVision(py::module& m) {
    // Vision class - Visual sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input for NLM system.
        
        The Vision class implements visual sensory processing based on
        camera and imaging systems:
        
        - 2D visual field: Horizontal and vertical resolution
        - Multi-channel: RGB, grayscale, or feature-based
        - Real-world mapping: World coordinates to visual coordinates
        - Dynamic updates: Real-time visual processing
        - Compression: Efficient storage of visual information
        
        Vision enables the brain to:
        - Process visual scenes
        - Detect objects and features
        - Navigate and interact with environment
        - Learn visual patterns
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create vision sensor
            >>> vision = pynlm.Vision(width=8, height=8, channels=3)
            >>> 
            >>> # Set visual data
            >>> data = []
            >>> for i in range(64):
            ...     data.append(i % 256 / 255.0)
            >>> vision.setData(data)
            >>> 
            >>> # Get vision information
            >>> print(f"Vision size: {vision.getWidth()}x{vision.getHeight()}")
            >>> print(f"Channels: {vision.getChannels()}")
            >>> 
            >>> # Process visual input
            >>> brain.receiveSensoryInput(vision)
        
        Methods:
            __init__(): Create empty vision sensor
            __init__(width, height, channels): Create vision sensor
            setData(data): Set visual data
            getData(): Get visual data
            getWidth(): Get visual width
            getHeight(): Get visual height
            getChannels(): Get number of channels
            getDimensions(): Get dimensions (width * height * channels)
            clear(): Clear visual data
        
        Attributes:
            width: Visual field width
            height: Visual field height
            channels: Number of color/feature channels
            
        See Also:
            SensoryInput, Audio, InternalSignals, SimpleWorld, AgentBrain
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(),
             py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create vision sensor")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
         "Set visual data")
        .def("getData", &Vision::getData,
             "Get visual data")
        .def("getWidth", &Vision::getWidth,
             "Get visual width")
        .def("getHeight", &Vision::getHeight,
             "Get visual height")
        .def("getChannels", &Vision::getChannels,
             "Get number of channels")
        .def("getDimensions", [](const Vision& self) {
            return self.getWidth() * self.getHeight() * self.getChannels();
        }, "Get dimensions (width * height * channels)")
        .def("clear", &Vision::clear,
             "Clear visual data")
        .def("__repr__", [](const Vision& vision) {
            return "<Vision: " + std::to_string(vision.getWidth()) + "x" +
                   std::to_string(vision.getHeight()) + "x" +
                   std::to_string(vision.getChannels()) + ">";
        }, "String representation of vision sensor");
}

void bindAudio(py::module& m) {
    // Audio class - Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input for NLM system.
        
        The Audio class implements audio sensory processing based on
        microphone and sound processing systems:
        
        - Sampling rate: Audio sampling frequency
        - Sample count: Number of audio samples
        - Frequency spectrum: Audio frequency content
        - Amplitude envelope: Audio amplitude over time
        - Sound localization: Spatial audio information
        
        Audio enables the brain to:
        - Process sound information
        - Detect direction and distance
        - Recognize speech and music
        - Navigate using sound cues
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create audio sensor
            >>> audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
            >>> 
            >>> # Generate test audio (sine wave)
            >>> sample_rate = audio.getSampleRate()
            >>> num_samples = audio.getNumSamples()
            >>> data = []
            >>> for i in range(num_samples):
            ...     data.append(0.5 * math.sin(2 * math.pi * 440 * i / sample_rate))
            >>> audio.setData(data)
            >>> 
            >>> # Get audio information
            >>> print(f"Sample rate: {audio.getSampleRate()}")
            >>> print(f"Duration: {audio.getNumSamples() / audio.getSampleRate():.3f}s")
        
        Methods:
            __init__(): Create empty audio sensor
            __init__(sample_rate, num_samples): Create audio sensor
            setData(data): Set audio data
            getData(): Get audio data
            setSampleRate(sample_rate): Set sample rate
            getSampleRate(): Get sample rate
            getNumSamples(): Get number of samples
            getDuration(): Get duration in seconds
            clear(): Clear audio data
        
        Attributes:
            sample_rate: Audio sampling rate (Hz)
            num_samples: Number of audio samples
            duration: Audio duration (seconds)
            
        See Also:
            SensoryInput, Vision, InternalSignals, SimpleWorld, AgentBrain
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(),
             py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio sensor")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
         "Set audio data")
        .def("getData", &Audio::getData,
             "Get audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get sample rate")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get number of samples")
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get duration in seconds")
        .def("clear", &Audio::clear,
             "Clear audio data")
        .def("__repr__", [](const Audio& audio) {
            return "<Audio: " + std::to_string(audio.getSampleRate()) + "Hz, " +
                   std::to_string(audio.getNumSamples()) + " samples, " +
                   std::to_string(audio.getDuration()) + "s>";
        }, "String representation of audio sensor");
}

void bindInternalSignals(py::module& m) {
    // InternalSignals class - Internal body state signals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal body state signals for NLM system.
        
        The InternalSignals class implements internal sensory processing based on
        body state and physiological signals:
        
        - Energy level: Current energy reserves
        - Health status: Current health condition
        - Temperature: Body temperature
        - Metabolism: Metabolic rate
        - Hormonal levels: Hormone concentrations
        - Neural activity: Brain and nervous system activity
        
        Internal signals enable the brain to:
        - Monitor body state
        - Regulate physiological processes
        - Make homeostatic adjustments
        - Coordinate with other systems
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create internal signals
            >>> internal = pynlm.InternalSignals()
            >>> 
            >>> # Add physiological signals
            >>> internal.addSignal(100.0)  # Energy (0-100)
            >>> internal.addSignal(0.8)   # Health (0-1)
            >>> internal.addSignal(37.0)  # Temperature (Celsius)
            >>> internal.addSignal(1.0)   # Metabolism (1.0 = normal)
            >>> 
            >>> # Get signal information
            >>> signals = internal.getSignals()
            >>> print(f"Number of signals: {len(signals)}")
            >>> print(f"Signal values: {signals}")
            >>> 
            >>> # Update signals
            >>> internal.updateSignal(0, 95.0)  # Update energy
            >>> 
            >>> # Clear signals
            >>> internal.clearSignals()
        
        Methods:
            __init__(): Create empty internal signals
            addSignal(value): Add signal value
            removeSignal(index): Remove signal by index
            getSignal(index): Get signal value by index
            updateSignal(index, value): Update signal value
            getSignals(): Get all signal values
            clearSignals(): Clear all signals
            getSignalCount(): Get number of signals
            getSignalTypes(): Get signal types
            setSignalTypes(types): Set signal types
        
        Attributes:
            signal_count: Number of signals
            signal_types: List of signal types
            
        See Also:
            SensoryInput, Vision, Audio, SimpleWorld, AgentBrain
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add signal value")
        .def("removeSignal", &InternalSignals::removeSignal, py::arg("index"),
             "Remove signal by index")
        .def("getSignal", &InternalSignals::getSignal, py::arg("index"),
             "Get signal value by index")
        .def("updateSignal", &InternalSignals::updateSignal,
             py::arg("index"), py::arg("value"),
             "Update signal value")
        .def("getSignals", &InternalSignals::getSignals,
             "Get all signal values")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals")
        .def("getSignalCount", &InternalSignals::getSignalCount,
             "Get number of signals")
        .def("getSignalTypes", &InternalSignals::getSignalTypes,
             "Get signal types")
        .def("setSignalTypes", &InternalSignals::setSignalTypes, py::arg("types"),
             "Set signal types")
        .def("__repr__", [](const InternalSignals& signals) {
            return "<InternalSignals: signals=" + std::to_string(signals.getSignalCount()) + ">";
        }, "String representation of internal signals");
}

void bindAction(py::module& m) {
    // Action class - Motor command
    py::class_<Action>(m, "Action", R"pbdoc(
        Motor command for NLM agents.
        
        The Action class represents a motor command that an agent can execute
        in the NLM world. Actions are generated by the brain and decoded by
        the agent, then applied to the world.
        
        Actions include movement, turning, looking, and interaction:
        - Movement: MoveForward, MoveBackward, MoveLeft, MoveRight
        - Rotation: TurnLeft, TurnRight
        - Perception: Look, LookUp, LookDown
        - Interaction: Interact, Eat, Drink
        - State: Rest, Wait, Custom
        
        Each action has:
        - Type: The action type (enumeration)
        - Parameters: Optional parameters (e.g., power, duration)
        - Name: Human-readable name
        
        Example:
            >>> import pynlm
            >>> # Create simple movement actions
            >>> move_forward = pynlm.Action(pynlm.ActionType.MoveForward)
            >>> move_backward = pynlm.Action(pynlm.ActionType.MoveBackward)
            >>> turn_left = pynlm.Action(pynlm.ActionType.TurnLeft)
            >>> turn_right = pynlm.Action(pynlm.ActionType.TurnRight)
            >>> 
            >>> # Create actions with parameters
            >>> look_action = pynlm.Action(
            ...     pynlm.ActionType.Look,
            ...     [30.0]  # Look angle
            ... )
            >>> 
            >>> # Get action information
            >>> print(f"Action type: {move_forward.getType()}")
            >>> print(f"Action name: {move_forward.getName()}")
            >>> print(f"Action parameters: {move_forward.getParameters()}")
        
        Methods:
            __init__(type): Create action with type
            __init__(type, parameters): Create action with type and parameters
            getType(): Get action type
            setType(type): Set action type
            getParameters(): Get action parameters
            setParameters(parameters): Set action parameters
            getName(): Get action name
            clone(): Create copy of action
            isValid(): Check if action is valid
            toDict(): Convert to dictionary
            fromDict(dictionary): Create from dictionary
        
        Properties:
            type: Get/set action type
            parameters: Get/set action parameters
            name: Get action name (read-only)
            
        See Also:
            ActionType, MotorCommand, AgentBrain, SimpleWorld
            pynlm.ActionType, pynlm.MotorCommand
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with type")
        .def(py::init<ActionType, std::vector<float>>(),
             py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        
        // Type
        .def("getType", &Action::getType,
             "Get action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type")
        
        // Parameters
        .def("getParameters", &Action::getParameters,
             "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        
        // Utilities
        .def("getName", &Action::getName,
             "Get action name")
        .def("clone", &Action::clone,
             "Create copy of action")
        .def("isValid", &Action::isValid,
             "Check if action is valid")
        
        // Conversion methods
        .def("toDict", &Action::toDict,
             "Convert to dictionary")
        .def_static("fromDict", &Action::fromDict, py::arg("dict"),
             "Create action from dictionary")
        
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + 
                   " (type=" + std::to_string(static_cast<int>(action.getType())) + ")>";
        }, "String representation of action")
        
        // Convenience properties
        .def_property("type", [](const Action& self) {
            return self.getType();
        }, [](Action& self, ActionType type) {
            self.setType(type);
        }, "Action type")
        
        .def_property("parameters", [](const Action& self) {
            return self.getParameters();
        }, [](Action& self, const std::vector<float>& params) {
            self.setParameters(params);
        }, "Action parameters");
}

void bindMotorSystem(py::module& m) {
    // MotorSystem class - Motor output system
    py::class_<MotorSystem>(m, "MotorSystem", R"pbdoc(
        Motor output system for NLM brain.
        
        The MotorSystem class implements motor output generation based on
        neural activity and motor command decoding:
        
        - Motor neuron pools: Groups of motor neurons
        - Motor command generation: Converting neural activity to actions
        - Muscle activation: Controlling muscle contractions
        - Biomechanics: Physical movement generation
        - Control theory: Closed-loop control systems
        
        Motor system enables the brain to:
        - Control movement
        - Generate actions
        - Interact with environment
        - Execute learned behaviors
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create motor system
            >>> motor = pynlm.MotorSystem()
            >>> 
            >>> # Configure motor parameters
            >>> motor.setMaxTorque(100.0)
            >>> motor.setControlGain(0.5)
            >>> motor.setDeadband(0.1)
            >>> 
            >>> # Generate motor command
            >>> # Simulate neural activity
            >>> motor_neuron_activity = [0.8, 0.3, 0.1, 0.9]
            >>> motor.decodeMotorActivity(motor_neuron_activity)
            >>> 
            >>> # Get motor output
            >>> torque = motor.getMotorOutput()
            >>> speed = motor.getMotorSpeed()
        
        Methods:
            __init__(): Create empty motor system
            decodeMotorActivity(activity): Decode motor activity to command
            getMotorOutput(): Get motor output
            getMotorSpeed(): Get motor speed
            getMotorTorque(): Get motor torque
            getMotorPosition(): Get motor position
            setMaxTorque(torque): Set maximum torque
            getMaxTorque(): Get maximum torque
            setControlGain(gain): Set control gain
            getControlGain(): Get control gain
            setDeadband(deadband): Set deadband
            getDeadband(): Get deadband
            reset(): Reset motor system
            getStrength(): Get modulation strength
            setStrength(strength): Set modulation strength
        
        Attributes:
            motor_output: Current motor output
            motor_speed: Current motor speed
            motor_torque: Current motor torque
            motor_position: Current motor position
            
        See Also:
            Action, MotorCommand, AgentBrain, SimpleWorld
    )pbdoc")
        .def(py::init<>())
        
        // Motor command decoding
        .def("decodeMotorActivity", &MotorSystem::decodeMotorActivity,
             py::arg("activity"),
             "Decode motor activity to command")
        
        .def("getMotorOutput", &MotorSystem::getMotorOutput,
             "Get motor output")
        
        .def("getMotorSpeed", &MotorSystem::getMotorSpeed,
             "Get motor speed")
        
        .def("getMotorTorque", &MotorSystem::getMotorTorque,
             "Get motor torque")
        
        .def("getMotorPosition", &MotorSystem::getMotorPosition,
             "Get motor position")
        
        // Configuration
        .def("setMaxTorque", &MotorSystem::setMaxTorque, py::arg("torque"),
             "Set maximum torque")
        
        .def("getMaxTorque", &MotorSystem::getMaxTorque,
             "Get maximum torque")
        
        .def("setControlGain", &MotorSystem::setControlGain, py::arg("gain"),
             "Set control gain")
        
        .def("getControlGain", &MotorSystem::getControlGain,
             "Get control gain")
        
        .def("setDeadband", &MotorSystem::setDeadband, py::arg("deadband"),
             "Set deadband")
        
        .def("getDeadband", &MotorSystem::getDeadband,
             "Get deadband")
        
        .def("reset", &MotorSystem::reset,
             "Reset motor system")
        
        .def("__repr__", [](const MotorSystem& motor) {
            return "<MotorSystem: torque=" + std::to_string(motor.getMotorTorque()) +
                   ", speed=" + std::to_string(motor.getMotorSpeed()) + ">";
        }, "String representation of motor system");
}

void bindEnvironment(py::module& m) {
    // Environment class - World-environment interface
    py::class_<Environment>(m, "Environment", R"pbdoc(
        World-environment interface for NLM agents.
        
        The Environment class provides the interface between NLM agents and the
        world, implementing:
        - State transition: Apply actions and get new state
        - Reward calculation: Compute rewards based on actions
        - Termination: Determine when episode ends
        - Information: Provide observations to agents
        
        The environment is the learning context for agents, providing
        feedback about the consequences of their actions.
        
        Example:
            >>> import pynlm
            >>> # Create simple grid world
            >>> env = pynlm.SimpleEnvironment(width=10, height=10)
            >>> env.setAgentStart(5, 5)
            >>> 
            >>> # Reset environment
            >>> state = env.reset()
            >>> 
            >>> # Run episode
            >>> for step in range(100):
            ...     action = agent.decodeMotorCommand()
            ...     next_state, reward, done, info = env.step(action)
            ...     agent.processSensoryInput(next_state)
            ...     
            ...     if done:
            ...         print("Episode finished!")
            ...         break
            ...     
            >>> print(f"Total reward: {sum(rewards)}")
        
        Methods:
            reset(): Reset environment to initial state
            step(action): Apply action and get next state
            render(): Render current state
            close(): Close environment
            getObservationSpace(): Get observation space
            getActionSpace(): Get action space
            getRewardRange(): Get reward range
            isTerminal(state): Check if state is terminal
        
        See Also:
            SimpleWorld, AgentBrain, Action, Observation
    )pbdoc")
        .def(py::init<>())
        .def("reset", &Environment::reset,
             "Reset environment to initial state")
        .def("step", &Environment::step,
             py::arg("action"),
             "Apply action and get next state")
        .def("render", &Environment::render,
             "Render current state")
        .def("close", &Environment::close,
             "Close environment")
        .def("getObservationSpace", &Environment::getObservationSpace,
             "Get observation space")
        .def("getActionSpace", &Environment::getActionSpace,
             "Get action space")
        .def("getRewardRange", &Environment::getRewardRange,
             "Get reward range")
        .def("isTerminal", &Environment::isTerminal, py::arg("state"),
             "Check if state is terminal");
}

void bindObservation(py::module& m) {
    // Observation class - Result of applying action
    py::class_<Observation>(m, "Observation", R"pbdoc(
        Result of applying action to the world.
        
        The Observation class represents the result of applying an action
        to the world, including:
        - Reward: Immediate reward or penalty
        - Success: Whether action succeeded
        - Message: Descriptive message about the result
        - WorldState: Snapshot of world state after action
        
        Observations are used by agents to learn from their actions
        and update their internal models of the world.
        
        Example:
            >>> import pynlm
            >>> # Create a simple observation
            >>> observation = pynlm.Observation()
            >>> observation.reward = 5.0
            >>> observation.success = True
            >>> observation.message = "Collected resource"
            >>> 
            >>> # Get observation information
            >>> print(f"Reward: {observation.reward}")
            >>> print(f"Success: {observation.success}")
            >>> print(f"Message: {observation.message}")
            >>> 
            >>> # Use in reinforcement learning
            >>> if observation.success:
            ...     print(f"Agent learned from action!")
            ...     if observation.reward > 0:
            ...         print(f"Gained {observation.reward} reward points")
        
        Attributes:
            reward: Reward or penalty value
            success: Whether action succeeded
            message: Descriptive message
            
        Methods:
            __init__(): Create empty observation
            __init__(reward, success, message): Create observation with values
            getReward(): Get reward value
            setReward(reward): Set reward value
            isSuccess(): Check if action succeeded
            setSuccess(success): Set success flag
            getMessage(): Get message
            setMessage(message): Set message
            toDict(): Convert to dictionary
            fromDict(dictionary): Create from dictionary
        
        Properties:
            reward: Get/set reward value
            success: Get/set success flag
            message: Get/set message
            
        See Also:
            Action, SensoryPercept, AgentBrain, SimpleWorld
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, const std::string&>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create observation with values")
        
        // Reward
        .def_readwrite("reward", &Observation::reward, "Reward or penalty value")
        
        // Success
        .def("isSuccess", &Observation::isSuccess,
             "Check if action succeeded")
        .def_readwrite("success", &Observation::success, "Success flag")
        
        // Message
        .def_readwrite("message", &Observation::message, "Descriptive message")
        
        // Conversion methods
        .def("toDict", &Observation::toDict,
             "Convert to dictionary")
        .def_static("fromDict", &Observation::fromDict, py::arg("dict"),
             "Create observation from dictionary")
        
        // Convenience properties
        .def_property("reward", [](const Observation& self) {
            return self.reward;
        }, [](Observation& self, float reward) {
            self.reward = reward;
        }, "Reward value")
        
        .def_property("success", [](const Observation& self) {
            return self.success;
        }, [](Observation& self, bool success) {
            self.success = success;
        }, "Success flag")
        
        .def_property("message", [](const Observation& self) {
            return self.message;
        }, [](Observation& self, const std::string& message) {
            self.message = message;
        }, "Message")
        
        .def("__repr__", [](const Observation& obs) {
            return "<Observation: reward=" + std::to_string(obs.reward) +
                   ", success=" + std::to_string(obs.success) +
                   ", message='" + obs.message + "'>";
        }, "String representation of observation");
}
