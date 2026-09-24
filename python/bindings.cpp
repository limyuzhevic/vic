#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/sensory/Vision.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Event-driven spiking neural networks with LIF dynamics
        - Integrated memory, prediction, cognition, and neuromodulation systems
        - Developmental learning and exploration mechanisms
        - Advanced vision processing and feature extraction
        - Action parameter support for precise motor control
        - Checkpoint saving/loading for reproducible experiments
        
        Getting Started:
        ```python
        import pynlm
        
        # Create a default brain configuration
        config = pynlm.createDefaultConfig()
        
        # Create a new brain with the configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create a world and agent
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation loop
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"Simulation complete. Firing neurons: {brain.getFiringNeuronCount()}")
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");

    // Configuration management
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        
        Manages brain configuration parameters including:
        - Neuron count and region distribution
        - Connection probabilities and weights
        - Plasticity parameters (STDP, Hebbian)
        - Structural plasticity rates
        - Simulation parameters (timestep, intervals)
        - Development settings
        - Neuromodulation factors
        
        Configuration keys:
        - neuron_count: Total number of neurons (default: 1000)
        - region_count: Number of brain regions (default: 1)
        - connection_probability: Initial connection probability (default: 0.1)
        - stdp_ltp_weight: STDP long-term potentiation weight
        - stdp_ltd_weight: STDP long-term depression weight
        - stdp_tau: STDP time constant
        - synaptogenesis_rate: New synapse formation rate
        - pruning_rate: Synapse removal rate
        - simulation_timestep: Integration timestep (default: 0.001)
        - replay_interval: Memory replay interval (default: 100)
        - consolidation_interval: Memory consolidation interval (default: 1000)
        - random_seed: Random number generator seed
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get a string configuration value")
        .def("get", [](const Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int64_t>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get an integer configuration value")
        .def("get", [](const Config& self, const std::string& key, float defaultValue) {
            auto value = self.get<float>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get a float configuration value")
        .def("get", [](const Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get a boolean configuration value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Action class with parameter support
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        
        An Action represents a motor command with associated parameters.
        Actions can have different types (MoveForward, TurnLeft, etc.) and
        can include continuous parameters for precise control.
        
        Common action types and their parameters:
        - MoveForward: [speed] (0.0-1.0)
        - MoveBackward: [speed] (0.0-1.0)
        - MoveLeft: [speed] (0.0-1.0)
        - MoveRight: [speed] (0.0-1.0)
        - TurnLeft: [angular_speed] (0.0-6.28 radians/sec)
        - TurnRight: [angular_speed] (0.0-6.28 radians/sec)
        - Look: [direction] (0.0-6.28 radians)
        - LookUp: [angle] (-1.57 to 1.57 radians)
        - LookDown: [angle] (-1.57 to 1.57 radians)
        - Interact: [strength] (0.0-1.0)
        - Eat: [quantity] (0.0-1.0)
        - Drink: [quantity] (0.0-1.0)
        - Rest: [duration] (0.0-1.0)
        - Wait: [probability] (0.0-1.0)
        - Custom: [parameters...] (arbitrary parameters)
        
        Attributes:
        - type: The action type
        - parameters: List of action parameters
        
        Methods:
        - get_type(): Returns the action type
        - get_parameters(): Returns the parameter list
        - set_type(type): Sets the action type
        - set_parameters(params): Sets the parameter list
        - get_name(): Returns a string name for the action
        - clone(): Creates a copy of the action
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getParameter", [](const Action& self, size_t index) {
            if (index >= self.getParameters().size()) {
                throw py::index_error("Parameter index out of range");
            }
            return self.getParameters()[index];
        }, py::arg("index"), "Get a specific parameter by index")
        .def("addParameter", [](Action& self, float param) {
            self.getParameters().push_back(param);
        }, py::arg("param"), "Add a parameter to the action")
        .def("getName", &Action::getName, "Get the action name as a string")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("__repr__", [](const Action& action) {
            std::string params_str = "[";
            const auto& params = action.getParameters();
            for (size_t i = 0; i < params.size(); ++i) {
                if (i > 0) params_str += ", ";
                params_str += std::to_string(params[i]);
            }
            params_str += "]";
            return "<Action: " + std::to_string(static_cast<int>(action.getType())) + " " + params_str + ">";
        });

    py::class_<VisionProcessor>(m, "VisionProcessor", R"pbdoc(Vision processing and feature extraction)
        
        VisionProcessor implements advanced vision processing techniques including:
        - Gaussian blur for noise reduction
        - Edge detection (Sobel operator)
        - Corner detection (Harris detector)
        - Motion detection and tracking
        - Contrast enhancement
        - Feature extraction for neural input
        
        The processor can extract various features from visual input:
        - Raw pixel data (for basic processing)
        - Edge maps for contour detection
        - Corner maps for key point identification
        - Motion vectors for dynamic scenes
        - Enhanced contrast images
        
        Methods:
        - process(input): Process vision input and extract features
        - get_features(): Get the feature representation
        - get_edges(): Get edge detection results
        - get_corners(): Get corner detection results
        - get_motion(): Get motion detection results
        - get_contrast(): Get contrast enhancement results
        - reset(): Reset the processor state
    )pbdoc")
        .def(py::init<>())
        .def("process", &VisionProcessor::process, py::arg("input"), "Process vision input")
        .def("reset", &VisionProcessor::reset, "Reset processor state")
        .def("getFeatures", &VisionProcessor::getFeatures, "Get the feature representation")
        .def("getEdges", &VisionProcessor::getEdges, "Get edge detection results")
        .def("getCorners", &VisionProcessor::getCorners, "Get corner detection results")
        .def("getMotion", &VisionProcessor::getMotion, "Get motion detection results")
        .def("getContrast", &VisionProcessor::getContrast, "Get contrast enhancement results");

    py::class_<VisionEnhancement>(m, "VisionEnhancement", R"pbdoc(Vision enhancement and filtering)
        
        VisionEnhancement provides various image processing techniques:
        - Gaussian blur for smoothing and noise reduction
        - Edge detection using Sobel operator
        - Corner detection using Harris detector
        - Contrast enhancement algorithms
        - Motion detection for video analysis
        
        Each enhancement method takes an input image and produces an output
        with the specified enhancement applied.
        
        Parameters:
        - sigma: Gaussian blur parameter (standard deviation)
        - threshold: Detection threshold for edge/corner detection
        - factor: Enhancement factor for contrast adjustment
        
        Methods:
        - apply_gaussian_blur(input, output, sigma): Apply Gaussian blur
        - detect_edges(input, output, threshold): Detect edges
        - detect_corners(input, output, threshold): Detect corners
        - enhance_contrast(input, output, factor): Enhance contrast
        - detect_motion(current, previous, output, threshold): Detect motion
    )pbdoc")
        .def(py::init<>())
        .def("applyGaussianBlur", &VisionEnhancement::applyGaussianBlur,
             py::arg("input"), py::arg("output"), py::arg("sigma") = 1.0f,
             "Apply Gaussian blur to input image")
        .def("detectEdges", &VisionEnhancement::detectEdges,
             py::arg("input"), py::arg("output"), py::arg("threshold") = 0.5f,
             "Detect edges in input image")
        .def("detectCorners", &VisionEnhancement::detectCorners,
             py::arg("input"), py::arg("output"), py::arg("threshold") = 0.3f,
             "Detect corners in input image")
        .def("enhanceContrast", &VisionEnhancement::enhanceContrast,
             py::arg("input"), py::arg("output"), py::arg("factor") = 1.5f,
             "Enhance contrast in input image")
        .def("detectMotion", &VisionEnhancement::detectMotion,
             py::arg("current"), py::arg("previous"), py::arg("output"), py::arg("threshold") = 0.1f,
             "Detect motion between two frames");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        
        Vision represents visual input with the following properties:
        - Resolution (width, height, channels)
        - Pixel data (grayscale or RGB)
        - Timestamp for temporal tracking
        - Interface for accessing raw data and properties
        
        Vision can be initialized with different resolutions and channel counts:
        - 16x16 grayscale (256 values)
        - 32x32 grayscale (1024 values)
        - 640x480 RGB (921,600 values)
        
        Common use cases:
        - Simple vision: Vision(16, 16)  # Grayscale
        - High resolution: Vision(640, 480, 3)  # RGB
        - Object detection: Vision(32, 32)  # Medium resolution
        
        Methods:
        - set_data(data): Set pixel data
        - get_width(): Get image width
        - get_height(): Get image height
        - get_channels(): Get number of channels
        - get_data(): Get raw pixel data
        
        The Vision class can be used directly or processed through VisionProcessor
        for feature extraction and enhancement.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set pixel data")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of channels");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for all sensory input)
        
        SensoryInput is the base class for all sensory modalities in NLM.
        It provides a unified interface for accessing sensory data.
        
        Common sensory modalities:
        - Vision: Visual input from cameras or artificial eyes
        - Audio: Sound input for auditory processing
        - Internal: Internal state signals (homeostatic)
        - Proprioception: Body position and movement sensing
        
        All sensory inputs provide:
        - getType(): Return the sensory modality name
        - getData(): Return raw data as a vector
        - getDimensions(): Return the dimensionality of the data
        - getTimestamp(): Return the time when the data was collected
        - setTimestamp(timestamp): Set the timestamp
        - clone(): Create a copy of the input
        
        The SensoryInput class is designed to be extended for specific
        sensory modalities while maintaining a consistent interface.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        
        Audio represents sound input with the following properties:
        - Sample rate (samples per second)
        - Number of samples
        - Audio data (amplitude values)
        - Timestamp for temporal tracking
        
        Audio data is stored as floating-point values in the range [-1.0, 1.0].
        
        Common configurations:
        - Speech recognition: Audio(16000, 1600)  # 1 second at 16kHz
        - Music: Audio(44100, 44100)  # 1 second at 44.1kHz
        - Environment: Audio(8000, 80000)  # 10 seconds at 8kHz
        
        Methods:
        - set_sample_rate(rate): Set the sample rate
        - get_sample_rate(): Get the sample rate
        - get_num_samples(): Get the number of samples
        - set_data(data): Set audio data
        
        The Audio class can be used for audio-based learning and recognition.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set the sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get the sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal state signals)
        
        InternalSignals represents the brain's internal state signals, including:
        - Homeostatic signals (energy, hunger, thirst)
        - Emotional states (arousal, fear, pleasure)
        - Cognitive states (attention, working memory load)
        - Physiological signals (temperature, fatigue)
        
        Internal signals are used to modulate brain activity and behavior:
        - High arousal enhances learning and memory formation
        - Low energy reduces activity and promotes rest
        - High attention improves sensory processing
        - High stress affects decision making
        
        Internal signals can be added dynamically as needed:
        - add_signal(value): Add a new internal signal
        - clear_signals(): Clear all signals
        - get_data(): Get all signals as a vector
        
        Common internal signal types:
        - Energy level (0.0-1.0, 1.0 = full)
        - Health (0.0-1.0, 1.0 = healthy)
        - Attention (0.0-1.0, 1.0 = fully attentive)
        - Arousal (0.0-1.0, 1.0 = highly alert)
        - Stress (0.0-1.0, 1.0 = highly stressed)
        
        Methods:
        - addSignal(value): Add a new internal signal
        - clearSignals(): Clear all internal signals
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add an internal signal")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals");

    // AgentBrain with comprehensive development and exploration
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        
        AgentBrain is the interface between the NLM brain and the environment.
        It handles:
        - Sensory transduction: Converting world percepts to neural input
        - Motor decoding: Converting neural activity to motor commands
        - Neuromodulation: Applying reward signals to plasticity
        - Development: Updating developmental stage and capabilities
        - Attention: Selective processing through competition
        - Planning: Using predictions to select actions
        
        The AgentBrain provides a complete control interface for NLM agents:
        - Initialize with a world environment
        - Process sensory input from the world
        - Decode motor commands for action
        - Apply reward modulation based on outcomes
        - Update development based on experience
        
        Key features:
        - Reward modulation (learning from rewards)
        - Structural plasticity (creating new connections)
        - Development (maturing over time)
        - Curiosity (exploration motivation)
        - Novelty detection (detecting new experiences)
        
        Configuration methods:
        - enableRewardModulation(True/False): Turn reward learning on/off
        - enableStructuralPlasticity(True/False): Turn connection growth on/off
        - enableDevelopment(True/False): Turn developmental learning on/off
        - enableCuriosity(True/False): Turn exploration on/off
        
        State access methods:
        - getDevelopmentalStage(): Current developmental stage
        - getNeuromodulationLevel(): Current neuromodulation signal
        - getCuriosityLevel(): Current curiosity/exploration level
        - getNoveltyLevel(): Current novelty detection level
        - getPredictionError(): Current prediction error signal
        
        Core methods:
        - initialize(world): Set up the agent with a world
        - processSensoryInput(percept): Inject sensory input into the brain
        - decodeMotorCommand(): Select an action based on brain activity
        - applyRewardModulation(reward, predicted_reward): Learn from outcomes
        - updateDevelopment(timestep): Update developmental state
        - reset(): Reset for a new episode
        - getBrain(): Access the underlying brain
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
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
             py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with optimized settings");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world environment");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants and utility functions
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add error types for better error handling
    py::register_exception_translator([](std::runtime_error& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
    });

    py::register_exception_translator([](std::invalid_argument& e) {
        PyErr_SetString(PyExc_ValueError, e.what());
    });

    // Performance and debugging utilities
    m.def("getVersion", []() {
        return "NLM Python Bindings v0.1.0";
    }, "Get the version of the Python bindings");

    m.def("getSystemInfo", []() {
        return "NLM Brain Simulation Framework (Phase 6)";
    }, "Get system information");

    m.def("validateInput", [](const std::vector<float>& data, size_t expected_size) {
        if (data.size() != expected_size) {
            throw std::invalid_argument("Input data size mismatch. Expected: " + std::to_string(expected_size) + ", Got: " + std::to_string(data.size()));
        }
        return true;
    }, py::arg("data"), py::arg("expected_size"), "Validate input data size");

    m.def("clamp", [](float value, float min_val, float max_val) {
        return std::max(min_val, std::min(max_val, value));
    }, py::arg("value"), py::arg("min_val"), py::arg("max_val"), "Clamp a value to a range");

    m.def("normalize", [](float value, float min_val, float max_val) {
        if (max_val - min_val < 0.001f) {
            return 0.5f;
        }
        return (value - min_val) / (max_val - min_val);
    }, py::arg("value"), py::arg("min_val"), py::arg("max_val"), "Normalize a value to [0, 1] range");

    // Example and demonstration functions
    m.def("create_example_brain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return brain;
    }, "Create an example brain for testing and demonstration");

    m.def("create_example_world", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        // Add some resources and hazards
        WorldObject resource1;
        resource1.x = 10.0f; resource1.y = 10.0f;
        resource1.type = WorldObjectType::Resource;
        resource1.value = 1.0f;
        resource1.radius = 0.5f;
        world->addObject(resource1);
        
        return world;
    }, "Create an example world with resources for testing");

    m.def("create_example_agent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable learning systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create an example agent with learning enabled");

    // Performance monitoring
    m.def("getPerformanceStats", [](const Brain& brain) {
        return py::dict(
            "neuron_count", static_cast<size_t>(brain.getTotalNeuronCount()),
            "synapse_count", static_cast<size_t>(brain.getTotalSynapseCount()),
            "active_neurons", static_cast<size_t>(brain.getActiveNeuronCount()),
            "firing_neurons", static_cast<size_t>(brain.getFiringNeuronCount()),
            "total_spikes", static_cast<size_t>(brain.getTotalSpikeCount()),
            "avg_firing_rate", brain.getAverageFiringRate(),
            "e_to_i_ratio", brain.getExcitationInhibitionRatio()
        );
    }, py::arg("brain"), "Get performance statistics from a brain");

    // Simulation utilities
    m.def("runSimulationStep", [](AgentBrain& agent, SimpleWorld& world, size_t step) {
        world.update(0.1);
        auto percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        agent.getBrain()->step(static_cast<nlm::SimulationStep>(step));
        return agent.decodeMotorCommand();
    }, py::arg("agent"), py::arg("world"), py::arg("step"), "Run a single simulation step");

    // State saving and loading
    m.def("saveBrainState", [](const Brain& brain, const std::string& filepath) {
        return brain.save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");

    m.def("loadBrainState", [](Brain& brain, const std::string& filepath) {
        return brain.load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file");

    // Development and exploration utilities
    m.def("getDevelopmentalInfo", [](const AgentBrain& agent) {
        return py::dict(
            "stage", static_cast<int>(agent.getDevelopmentalStage()),
            "age", agent.getNeuromodulationLevel(),  # Using as proxy for age
            "plasticity", agent.getCuriosityLevel(),   # Using as proxy for plasticity
            "novelty", agent.getNoveltyLevel(),
            "curiosity", agent.getCuriosityLevel()
        );
    }, py::arg("agent"), "Get developmental information from an agent");

    m.def("exploreEnvironment", [](AgentBrain& agent, SimpleWorld& world, size_t steps) {
        agent.enableCuriosity(true);
        
        for (size_t i = 0; i < steps; ++i) {
            world.update(0.1);
            auto percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            agent.getBrain()->step(static_cast<nlm::SimulationStep>(i));
            auto action = agent.decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
        }
        
        return agent.getCuriosityLevel();
    }, py::arg("agent"), py::arg("world"), py::arg("steps"), "Run exploration simulation");

    // Educational and demonstration utilities
    m.def("createDemonstrationScene", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 4, 4);
        world->reset();
        
        // Create a simple maze-like environment
        std::vector<WorldObject> walls;
        for (int x = 0; x < 10; x++) {
            WorldObject wall;
            wall.x = x; wall.y = 0;
            wall.type = WorldObjectType::Wall;
            wall.radius = 0.3f;
            walls.push_back(wall);
            
            wall.x = x; wall.y = 9;
            walls.push_back(wall);
        }
        
        for (int y = 0; y < 10; y++) {
            WorldObject wall;
            wall.x = 0; wall.y = y;
            wall.type = WorldObjectType::Wall;
            wall.radius = 0.3f;
            walls.push_back(wall);
            
            wall.x = 9; wall.y = y;
            walls.push_back(wall);
        }
        
        for (auto& wall : walls) {
            world->addObject(wall);
        }
        
        // Add a goal
        WorldObject goal;
        goal.x = 8; goal.y = 8;
        goal.type = WorldObjectType::Marker;
        goal.value = 10.0f;
        goal.radius = 0.5f;
        world->addObject(goal);
        
        return world;
    }, "Create a demonstration scene with walls and goals");

    // Debug and visualization utilities
    m.def("printBrainStatus", [](const Brain& brain) {
        brain.logStatus();
    }, py::arg("brain"), "Print detailed brain status information");

    m.def("checkBrainHealth", [](const Brain& brain) {
        auto stats = py::dict(
            "neuron_count", static_cast<size_t>(brain.getTotalNeuronCount()),
            "synapse_count", static_cast<size_t>(brain.getTotalSynapseCount()),
            "active_ratio", static_cast<float>(brain.getActiveNeuronCount()) / std::max(1.0f, static_cast<float>(brain.getTotalNeuronCount())),
            "firing_ratio", static_cast<float>(brain.getFiringNeuronCount()) / std::max(1.0f, static_cast<float>(brain.getTotalNeuronCount())),
            "e_to_i_ratio", brain.getExcitationInhibitionRatio()
        );
        
        // Check for potential issues
        std::vector<std::string> warnings;
        
        if (brain.getTotalNeuronCount() == 0) {
            warnings.push_back("No neurons created");
        }
        
        if (brain.getTotalSynapseCount() == 0) {
            warnings.push_back("No synapses created");
        }
        
        if (brain.getActiveNeuronCount() == 0) {
            warnings.push_back("No active neurons");
        }
        
        stats["warnings"] = warnings;
        
        return stats;
    }, py::arg("brain"), "Check brain health and return statistics");

    // Learning and adaptation utilities
    m.def("trainAgent", [](AgentBrain& agent, SimpleWorld& world, size_t episodes) {
        float total_reward = 0.0f;
        float best_reward = -1000.0f;
        
        for (size_t episode = 0; episode < episodes; ++episode) {
            float episode_reward = 0.0f;
            
            for (size_t step = 0; step < 100; ++step) {
                world.update(0.1);
                auto percept = world.getSensoryPercept();
                agent.processSensoryInput(percept);
                agent.getBrain()->step(static_cast<nlm::SimulationStep>(step));
                auto action = agent.decodeMotorCommand();
                auto result = world.applyMotorCommand(action, world.getSimulationTime());
                
                agent.applyRewardModulation(result.reward, agent.getPredictionError());
                episode_reward += result.reward;
            }
            
            total_reward += episode_reward;
            best_reward = std::max(best_reward, episode_reward);
            
            agent.reset();
            world.reset();
        }
        
        return py::dict(
            "avg_reward", total_reward / episodes,
            "best_reward", best_reward,
            "episodes", episodes,
            "final_curiosity", agent.getCuriosityLevel()
        );
    }, py::arg("agent"), py::arg("world"), py::arg("episodes"), "Train an agent through multiple episodes");

    // Export version and metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "NLM Python Bindings for Brain Simulation";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__license__") = "MIT";
}

} // namespace nlm
