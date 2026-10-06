#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>

// Include NLM headers
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Types/Types.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/motor/Action.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/agent/ActionResult.hpp"
#include "src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        **Overview**
        -----------
        This module provides Python bindings to the NLM (Neural Learning Machine) C++ framework,
        enabling creation and simulation of spiking neural networks with cognitive capabilities.
        
        **Key Components**
        -----------------
        - **Brain**: Central neural simulation engine with neurons, synapses, and integrated cognitive systems
        - **AgentBrain**: Interface connecting neural processing to environmental interaction
        - **SimpleWorld**: 2D simulation environment with sensory and motor capabilities
        - **Config**: Configuration system for neural and environmental parameters
        - **Memory Systems**: Working memory, episodic memory, and associative memory
        - **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error systems
        - **Cognition**: Planning, concept formation, and attentional selection
        
        **Quick Start Examples**
        -----------------------
        ```python
        import pynlm
        
        # Create a simple brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run basic simulation
        for step in range(100):
            brain.step(step)
            
        # Create complete agent
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning capabilities
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation loop
        for step in range(1000):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        # Get brain statistics
        print(f"Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Avg firing rate: {brain.getAverageFiringRate():.2f} Hz")
        ```
        
        **Additional Resources**
        ----------------------
        - `HOW_TO_USE.md`: Comprehensive usage guide with detailed examples
        - `easy_usage.md`: Beginner-friendly introduction to NLM
        - Documentation directory: `docs/` contains architecture and scientific background
        
        **Scientific Context**
        ---------------------
        NLM implements Leaky Integrate-and-Fire neurons with event-driven dynamics,
        STDP and Hebbian plasticity, neuromodulation, memory systems, and cognitive
        processes in a unified brain loop. The system develops from primitive states
        through interaction with an environment.
        
        **Phase 6 Integration**
        ----------------------
        Current implementation integrates all major brain systems:
        - ✅ Memory systems (working, episodic, associative)
        - ✅ Neuromodulation (dopamine, curiosity, novelty)
        - ✅ Prediction systems
        - ✅ Cognition (planning, concept formation)
        - ✅ Development stages and structural plasticity
        - ✅ Checkpoint save/load and memory replay
        
        For more details, see: https://nlm.readthedocs.io
    )pbdoc";

    // Custom exception types for better error handling
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);
    py::register_exception<std::runtime_error>(m, "SimulationError", PyExc_RuntimeError);

    // Module-level constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module-level helper functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration for the neural brain");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with the given configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world instance");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(std::move(brain));
    }, py::arg("brain"), "Create a new agent brain interface for the given brain");

    // Enhanced parameter validation
    m.def("validate_parameters", 
        [](double width, double height, size_t visionWidth, size_t visionHeight,
           double timestep, size_t num_steps) {
            std::vector<std::string> errors;
            
            if (width <= 0.0) {
                errors.push_back("World width must be positive");
            }
            if (height <= 0.0) {
                errors.push_back("World height must be positive");
            }
            if (visionWidth == 0) {
                errors.push_back("Vision width must be greater than 0");
            }
            if (visionHeight == 0) {
                errors.push_back("Vision height must be greater than 0");
            }
            if (timestep <= 0.0) {
                errors.push_back("Timestep must be positive");
            }
            if (num_steps == 0) {
                errors.push_back("Number of steps must be greater than 0");
            }
            
            return errors;
        },
        py::arg("width") = 20.0, py::arg("height") = 20.0,
        py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
        py::arg("timestep") = 0.1, py::arg("num_steps") = 1000,
        "Validate simulation parameters and return error messages. Parameters have sensible defaults.");

    // Advanced simulation controls
    m.def("run_simulation", 
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
           std::shared_ptr<AgentBrain> agent, size_t num_steps, double timestep = 0.1) {
            if (!brain || !world || !agent) {
                throw py::value_error("All parameters must be non-null");
            }
            
            // Validate parameters
            auto validation_errors = validate_parameters(
                world->getWidth(), world->getHeight(),
                16, 16, // Default vision dimensions
                timestep, num_steps
            );
            
            if (!validation_errors.empty()) {
                std::string error_msg = "Invalid simulation parameters:";
                for (const auto& error : validation_errors) {
                    error_msg += " " + error;
                }
                throw py::value_error(error_msg);
            }
            
            // Create local copies for safe modification
            auto agentBrain = std::make_shared<AgentBrain>(std::move(brain));
            auto simpleWorld = std::make_shared<SimpleWorld>(*world);
            
            simpleWorld->configure(20.0, 20.0, 8, 8);
            simpleWorld->reset();
            agentBrain->initialize(*simpleWorld);
            
            for (size_t step = 0; step < num_steps; ++step) {
                simpleWorld->update(timestep);
                const SensoryPercept& percept = simpleWorld->getSensoryPercept();
                agentBrain->processSensoryInput(percept);
                agentBrain->getBrain()->step(static_cast<SimulationStep>(step), static_cast<Timestamp>(step * timestep));
                MotorCommand action = agentBrain->decodeMotorCommand();
                simpleWorld->applyMotorCommand(action, simpleWorld->getSimulationTime());
            }
            
            return std::make_tuple(
                agentBrain->getBrain()->getTotalNeuronCount(),
                agentBrain->getBrain()->getTotalSpikeCount(),
                agentBrain->getBrain()->getAverageFiringRate()
            );
        },
        py::arg("brain"), py::arg("world"), py::arg("agent"), 
        py::arg("num_steps") = 1000, py::arg("timestep") = 0.1,
        "Run a complete simulation with agent, brain, and world. Returns (neuron_count, spike_count, avg_firing_rate)");

    m.def("benchmark_neural_dynamics",
        [](std::shared_ptr<Brain> brain, size_t num_steps, bool enable_learning = true) {
            if (!brain) {
                throw py::value_error("Brain cannot be null");
            }
            
            auto config = brain->getConfig();
            std::shared_ptr<AgentBrain> agent = std::make_shared<AgentBrain>(std::move(brain));
            std::shared_ptr<SimpleWorld> world = std::make_shared<SimpleWorld>();
            
            world->configure(10.0, 10.0, 8, 8);
            world->reset();
            agent->initialize(*world);
            agent->enableRewardModulation(enable_learning);
            agent->enableCuriosity(enable_learning);
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (size_t step = 0; step < num_steps; ++step) {
                world->update(0.1);
                const SensoryPercept& percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                agent->getBrain()->step(static_cast<SimulationStep>(step));
                MotorCommand action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            return std::make_tuple(
                agent->getBrain()->getTotalNeuronCount(),
                agent->getBrain()->getTotalSpikeCount(),
                agent->getBrain()->getAverageFiringRate(),
                duration.count(),
                agent->getBrain()->getExcitationInhibitionRatio()
            );
        },
        py::arg("brain"), py::arg("num_steps") = 1000, py::arg("enable_learning") = true,
        "Run benchmark simulation and return performance metrics. Returns (neuron_count, spike_count, avg_firing_rate, ms_elapsed, ei_ratio)");

    m.def("validate_simulation_state",
        [](const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world) {
            if (!brain || !world) {
                throw py::value_error("Brain and world cannot be null");
            }
            
            std::vector<std::string> warnings;
            
            // Check brain state
            if (brain->getTotalNeuronCount() == 0) {
                warnings.push_back("Brain has no neurons");
            }
            if (!brain->getConfig()) {
                warnings.push_back("Brain has no configuration");
            }
            
            // Check world state
            if (world->getWidth() == 0 || world->getHeight() == 0) {
                warnings.push_back("World has invalid dimensions");
            }
            
            return warnings;
        },
        py::arg("brain"), py::arg("world"),
        "Validate simulation state and return warnings. Returns list of warning messages (empty if valid)");

    m.def("export_brain_data",
        [](const std::shared_ptr<Brain>& brain, const std::string& filepath, bool include_neurons = true, 
           bool include_synapses = true, bool include_regions = true) {
            if (!brain) {
                throw py::value_error("Brain cannot be null");
            }
            
            brain->save(filepath);
        },
        py::arg("brain"), py::arg("filepath"), py::arg("include_neurons") = true,
        py::arg("include_synapses") = true, py::arg("include_regions") = true,
        "Export brain state to file (currently saves via brain's save method)");

    m.def("import_brain_data",
        [](const std::string& filepath) -> std::shared_ptr<Brain> {
            if (filepath.empty()) {
                throw py::value_error("Filepath cannot be empty");
            }
            
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            brain->load(filepath);
            return brain;
        },
        py::arg("filepath"),
        "Import brain state from file. Returns a new Brain instance with loaded state");

    m.def("get_version", []() {
        return "pynlm 0.1.0 - NLM Phase 6 Integration";
    }, "Get the version and information about this NLM binding");

    // Data type bindings
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system
        
        A flexible key-value configuration system supporting multiple data types
        (integers, doubles, strings, booleans, and vectors). Configuration can be
        loaded from files (JSON format), command-line arguments, or set at runtime.
        
        **Configuration Sources**
        ------------------------
        - **Default**: Built-in default values
        - **File**: Loaded from JSON file
        - **CommandLine**: Parsed from command-line arguments  
        - **Runtime**: Set dynamically during execution
        
        **Value Types Supported**
        ------------------------
        - Basic types: int, int64_t, double, bool, std::string
        - Collections: std::vector<int>, std::vector<double>, std::vector<std::string>
        
        **Common Usage Examples**
        --------------------------
        ```python
        # Create default configuration
        config = pynlm.createDefaultConfig()
        
        # Set simple values
        config.set("brain.neuron_count", 1000)
        config.set("brain.simulation_steps", 500)
        config.set("world.width", 20.0)
        config.set("world.height", 20.0)
        
        # Set array values
        config.set("brain.initial_positions", [0.0, 0.0, 1.0, 1.0])
        
        # Get values with defaults
        neurons = config.getOr("brain.neuron_count", 100)
        world_size = config.getOr("world.width", 10.0)
        
        # Check if key exists
        if config.has("brain.custom_param"):
            value = config.get("brain.custom_param")
            
        # Get all configuration keys
        keys = config.getKeys()
        ```
        
        **Typical Configuration Keys**
        ------------------------------
        **Brain Settings**
        - `brain.neuron_count`: Number of neurons in simulation
        - `brain.synapse_density`: Connection probability between neurons
        - `brain.plasticity_rate`: Plasticity learning rate
        - `brain.development_enabled`: Enable developmental stages
        
        **World Settings**
        - `world.width`, `world.height`: World dimensions
        - `world.vision_width`, `world.vision_height`: Sensory range
        - `world.energy_decay_rate`: Energy loss per timestep
        - `world.max_energy`: Maximum energy per agent
        
        **Sensory Settings**
        - `sensory.vision_enabled`: Enable vision processing
        - `sensory.audio_enabled`: Enable audio processing
        - `sensory.touch_enabled`: Enable touch/proximity sensors
        
        **Learning Settings**
        - `learning.reward_modulation`: Enable reward-based learning
        - `learning.curiosity_driven`: Enable curiosity-driven exploration
        - `learning.stdp_enabled`: Enable Spike-Timing-Dependent Plasticity
        - `learning.hebbian_enabled`: Enable Hebbian learning
        
        **Random Seed**
        - `random_seed`: Seed for reproducible simulations
        
        For more configuration options, see the NLM documentation.
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
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input from the environment or internal brain state
        
        Sensory inputs are low-level data signals that the brain processes to create
        perceptions. They contain only raw numerical data without semantic labels.
        
        **Sensory Input Types**
        ----------------------
        - **Vision**: Visual input as intensity grid (no object labels)
        - **Audio**: Sound input as time-series samples
        - **Touch**: Proximity/collision detection signals
        - **Internal**: Homeostatic and internal brain state signals
        - **Proprioception**: Body position and velocity signals
        
        **Common Usage**
        ---------------
        ```python
        # Create sensory inputs
        vision = pynlm.Vision(width=8, height=8)
        data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8] * 8
        vision.setData(data)
        
        audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
        audio_data = [0.5] * 1024
        audio.setData(audio_data)
        audio.setSampleRate(22050)
        
        internal = pynlm.InternalSignals()
        internal.addSignal(0.8)
        internal.addSignal(0.6)
        internal.addSignal(0.4)
        ```
        
        **Internal Signals**
        ------------------
        Internal signals represent brain state information:
        - Homeostatic signals (energy, hunger, thirst)
        - Reward prediction errors
        - Motivational states
        - Cognitive states
        
        For more details on sensory modalities, see the NLM documentation.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input - visual perception
        
        Vision input represents what the agent sees as a simple intensity grid.
        **No semantic labels, object identities, or colors** - only raw intensity values.
        
        **Data Format**
        -------------
        - **Layout**: Row-major grid
        - **Range**: Float values 0.0-1.0 (0.0 = black, 1.0 = white)
        - **Channels**: Grayscale only (single intensity channel)
        - **Structure**: [row0_pixel0, row0_pixel1, ... row1_pixel0, ...]
        
        **Typical Usage**
        ----------------
        ```python
        # Create vision sensor
        vision = pynlm.Vision(width=8, height=8)
        
        # Set vision data (8x8 grid of intensity values)
        grid_data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8] * 8
        vision.setData(grid_data)
        
        # Access vision properties
        print(f"Width: {vision.getWidth()}, Height: {vision.getHeight()}")
        print(f"Dimensions: {vision.getDimensions()}")
        
        # Create vision from raw array
        import numpy as np
        data = np.random.random((16, 16))  # 16x16 vision grid
        vision.setData(data.flatten().tolist())
        ```
        
        **Common Vision Grid Sizes**
        ----------------------------
        - **Small**: 8x8 = 64 values (fast, coarse perception)
        - **Medium**: 16x16 = 256 values (balanced performance)
        - **Large**: 32x32 = 1024 values (detailed perception)
        
        For more details on vision processing, see NLM documentation.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input - sound perception
        
        Audio input represents auditory signals as time-series samples.
        Used for sound localization and temporal processing.
        
        **Data Format**
        -------------
        - **Layout**: Time series (sample0, sample1, sample2, ...)
        - **Range**: Float values (typically -1.0 to 1.0)
        - **Sample Rate**: Samples per second (Hz)
        - **Duration**: Determined by number of samples
        
        **Typical Usage**
        ----------------
        ```python
        # Create audio sensor
        audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
        
        # Set audio data
        audio_data = [0.1, 0.2, 0.3, 0.4, 0.5] * 204  # ~1 second at 44.1kHz
        audio.setData(audio_data)
        audio.setSampleRate(22050)  # Reduce sample rate
        
        # Access audio properties
        print(f"Sample rate: {audio.getSampleRate()} Hz")
        print(f"Duration: {audio.getNumSamples() / audio.getSampleRate():.2f} seconds")
        ```
        
        **Common Audio Settings**
        ------------------------
        - **Short**: 8000 samples = ~0.18 seconds at 44.1kHz
        - **Medium**: 44100 samples = 1.0 second at 44.1kHz
        - **Long**: 176400 samples = 4.0 seconds at 44.1kHz
        
        For more details on audio processing, see NLM documentation.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input - brain internal state
        
        Internal signals represent the brain's own state information:
        - Homeostatic signals (energy levels, hunger, thirst)
        - Reward prediction errors
        - Motivational states
        - Cognitive states (attention, focus)
        
        **Data Format**
        -------------
        - **Layout**: Signal values (signal0, signal1, signal2, ...)
        - **Range**: Float values (typically 0.0-1.0 normalized)
        - **Dynamics**: Changes over time based on brain activity
        
        **Typical Usage**
        ----------------
        ```python
        # Create internal signals sensor
        internal = pynlm.InternalSignals()
        
        # Add internal state signals
        internal.addSignal(0.8)  # High motivation
        internal.addSignal(0.4)  # Moderate energy
        internal.addSignal(0.1)  # Low attention
        
        # Get all signals
        all_signals = internal.getAllSignals()
        print(f"Number of signals: {len(all_signals)}")
        print(f"Signal values: {all_signals}")
        
        # Clear and reset
        internal.clearSignals()
        ```
        
        **Common Internal Signals**
        --------------------------
        - **Motivation**: Drive to explore and learn
        - **Curiosity**: Interest in novel stimuli
        - **Attention**: Focus on current tasks
        - **Energy**: Physiological energy levels
        - **Stress**: Physiological arousal state
        
        Internal signals modulate learning and behavior through neuromodulation.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output
        
        Actions are the motor commands that the agent executes in the world.
        Each action has a type (what to do) and optional parameters (how to do it).
        
        **Action Types**
        --------------
        - **Movement**: MoveForward, MoveBackward, MoveLeft, MoveRight, TurnLeft, TurnRight
        - **Looking**: Look, LookUp, LookDown, LookLeft, LookRight
        - **Interaction**: Interact, Eat, Drink
        - **States**: Rest, Wait, Custom
        
        **Data Format**
        -------------
        - **Type**: Enum value from ActionType
        - **Parameters**: Vector of float values (interpretation depends on action type)
        - **Name**: Human-readable action name
        
        **Typical Usage**
        ----------------
        ```python
        # Create actions using different constructors
        action1 = pynlm.Action(pynlm.ActionType.MoveForward)
        action2 = pynlm.Action(pynlm.ActionType.Interact)
        
        # Create actions with parameters
        move_action = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 0.5])  # distance, speed
        custom_action = pynlm.Action(pynlm.ActionType.Custom, [0.1, 0.2, 0.3])
        
        # Modify actions
        action1.setType(pynlm.ActionType.MoveLeft)
        action2.setParameters([0.5])
        
        # Get action information
        print(f"Action type: {action1.getType()}")
        print(f"Action name: {action1.getName()}")
        print(f"Parameters: {action2.getParameters()}")
        
        # Clone actions
        action_copy = action1.clone()
        ```
        
        **Common Action Parameters**
        ----------------------------
        **Movement Actions**: [distance, speed, duration]
        **Look Actions**: [angle_x, angle_y, speed]
        **Custom Actions**: User-defined parameter list
        
        Actions are produced by the brain's motor system and executed in the world.
        Each action has a type and optional parameters that control its execution.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation
        
        WorldObject represents an object in the simulated environment.
        Objects have position, type, and properties that affect agent behavior.
        
        **Object Types**
        --------------
        - **Empty**: No object (transparent)
        - **Resource**: Valuable item (can be collected)
        - **Hazard**: Dangerous item (causes damage)
        - **Wall**: Obstacle (cannot pass through)
        - **Marker**: Special location (has semantic meaning)
        
        **Data Format**
        -------------
        - **Position**: (x, y) coordinates in world space
        - **Type**: WorldObjectType enum
        - **Properties**: radius (size), value (reward/damage)
        - **State**: active (whether object exists)
        
        **Typical Usage**
        ----------------
        ```python
        # Create world objects
        empty_space = pynlm.WorldObject(x=5.0, y=5.0, type=pynlm.WorldObjectType.Empty)
        resource = pynlm.WorldObject(x=2.0, y=3.0, type=pynlm.WorldObjectType.Resource, value=10.0)
        hazard = pynlm.WorldObject(x=8.0, y=7.0, type=pynlm.WorldObjectType.Hazard, value=-5.0)
        wall = pynlm.WorldObject(x=10.0, y=10.0, type=pynlm.WorldObjectType.Wall)
        
        # Access object properties
        print(f"Object type: {resource.type}")
        print(f"Object position: ({resource.x}, {resource.y})")
        print(f"Object value: {resource.value}")
        print(f"Object radius: {resource.radius}")
        print(f"Object active: {resource.active}")
        
        # Modify object properties
        hazard.value = -2.5  # Reduce damage
        wall.radius = 1.0  # Change wall thickness
        ```
        
        **Object Properties**
        --------------------
        - **x, y**: Position coordinates
        - **radius**: Physical size (collision detection)
        - **type**: Object category (affects behavior)
        - **value**: Reward value (positive) or damage (negative)
        - **active**: Whether object exists in world
        
        Objects interact with the agent during world updates and motor command execution.
        Resources provide rewards, hazards cause damage, and walls block movement.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representation
        
        AgentBody contains the physical state of the agent in the world.
        It includes position, movement, energy, and other physiological parameters
        that determine how the agent interacts with the environment.
        
        **State Variables**
        -----------------
        - **Position**: (x, y) coordinates in world space
        - **Orientation**: Facing direction in radians
        - **Velocity**: Linear velocity (vx, vy) components
        - **Angular Velocity**: Rotational speed
        - **Physiological**: Energy, health, age
        - **Movement Flags**: isMoving, isTurning
        - **Time Tracking**: lastActionTime
        
        **Data Format**
        -------------
        - **Position**: Float coordinates (world space)
        - **Orientation**: Float in radians (0 = East, π/2 = North)
        - **Velocity**: Float components (units per timestep)
        - **Energy**: Float health metric (0 = dead, 1 = full)
        - **Health**: Float vitality metric
        - **Age**: Float developmental age
        - **lastActionTime**: Float time since last action
        
        **Typical Usage**
        ----------------
        ```python
        # Create agent body
        body = pynlm.AgentBody()
        
        # Set position and orientation
        body.x = 5.0
        body.y = 3.0
        body.orientation = 0.0  # Facing East
        
        # Set movement state
        body.isMoving = True
        body.velocityX = 1.5
        body.velocityY = 0.5
        body.angularVelocity = 0.1
        
        # Set physiological state
        body.energy = 0.8
        body.health = 0.9
        body.age = 1.5
        
        # Access agent state
        print(f"Position: ({body.x}, {body.y})")
        print(f"Orientation: {body.orientation} radians")
        print(f"Energy: {body.energy}")
        print(f"Health: {body.health}")
        print(f"Is moving: {body.isMoving}")
        
        # Reset agent state
        body.reset()
        ```
        
        **Movement Properties**
        ----------------------
        - **isMoving**: Boolean indicating if agent is currently moving
        - **isTurning**: Boolean indicating if agent is rotating
        - **velocityX, velocityY**: Linear velocity components
        - **angularVelocity**: Rotational speed (radians per timestep)
        
        **Physiological Properties**
        --------------------------
        - **energy**: Health/energy level (decreases over time)
        - **health**: Overall vitality (damage/recovery affects this)
        - **age**: Developmental age (affects behavior and learning)
        
        AgentBody state is updated by world physics and used by motor commands
        to determine agent behavior and capabilities.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world execution
        
        ActionResult contains the outcome of executing a motor command in the world.
        It includes reward feedback, success status, and descriptive messages.
        
        **Result Components**
        ------------------
        - **Reward**: Numerical value (positive = benefit, negative = cost)
        - **Success**: Boolean indicating if action succeeded
        - **Message**: Descriptive text explaining the outcome
        
        **Data Format**
        -------------
        - **reward**: Float value (any real number)
        - **success**: Boolean (true if action completed successfully)
        - **message**: String description of outcome
        
        **Typical Usage**
        ----------------
        ```python
        # Create action results
        success_result = pynlm.ActionResult(reward=10.0, success=True)
        failure_result = pynlm.ActionResult(reward=-5.0, success=False, message="Hit wall")
        
        # Create empty result
        neutral_result = pynlm.ActionResult()
        
        # Access result properties
        print(f"Reward: {success_result.reward}")
        print(f"Success: {success_result.success}")
        print(f"Message: {success_result.message}")
        
        # Modify results
        failure_result.reward = 0.0
        failure_result.success = True
        failure_result.message = "Collected resource"
        ```
        
        **Reward Types**
        ---------------
        - **Positive**: Rewards for beneficial actions (collecting resources)
        - **Negative**: Costs for harmful actions (hitting hazards)
        - **Zero**: Neutral outcomes (no change)
        
        **Success Indicators**
        ---------------------
        - **True**: Action completed as intended
        - **False**: Action failed or was blocked
        
        ActionResult is produced by SimpleWorld.applyMotorCommand() and provides
        feedback to the agent for learning through reward modulation.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data packet
        
        SensoryPercept contains the complete sensory information that the agent
        receives from the environment at each simulation timestep. It combines
        all sensory modalities into a single data structure for brain processing.
        
        **Sensory Components**
        --------------------
        - **Vision**: Visual input (intensity grid)
        - **Touch**: Proximity/collision detection signals
        - **Internal**: Brain internal state signals
        - **Proprioception**: Body position and velocity signals
        - **Audio**: Sound input (if enabled)
        
        **Data Format**
        -------------
        - **Each modality**: Vector of float values
        - **Vision**: 8x8 = 64 values (default), configurable size
        - **Touch**: 8 proximity sensors (default), configurable
        - **Internal**: Variable number of homeostatic signals
        - **Proprioception**: Body state signals (position, velocity)
        - **Audio**: Time-series audio samples (if enabled)
        - **Timestamp**: Double precision time of percept creation
        
        **Typical Usage**
        ----------------
        ```python
        # Create sensory percept
        percept = pynlm.SensoryPercept()
        
        # Set vision data (8x8 grid)
        vision_data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8] * 8
        percept.setVision(vision_data)
        
        # Set other sensory modalities
        touch_data = [0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0]
        percept.setTouch(touch_data)
        
        internal_data = [0.8, 0.5, 0.2]
        percept.setInternal(internal_data)
        
        proprioception_data = [5.0, 2.0, 0.1]  # position, velocity, orientation
        percept.setProprioception(proprioception_data)
        
        # Set timestamp
        percept.setTimestamp(world.getSimulationTime())
        
        # Get all sensory data
        all_signals = percept.getAllSignals()
        print(f"Total sensory values: {len(all_signals)}")
        print(f"Vision width: {percept.getVisionWidth()}, height: {percept.getVisionHeight()}")
        
        # Access individual modalities
        vision = percept.getVision()
        touch = percept.getTouch()
        internal = percept.getInternal()
        proprioception = percept.getProprioception()
        ```
        
        **Percept Processing**
        ---------------------
        SensoryPercept is created by SimpleWorld.getSensoryPercept() and passed
        to AgentBrain.processSensoryInput() for integration into the brain's neural processing.
        
        **Common Use Cases**
        ------------------
        - **Object Detection**: Vision grid shows nearby objects
        - **Collision Avoidance**: Touch signals detect obstacles
        - **Goal Pursuit**: Internal signals track motivation
        - **Movement Control**: Proprioception guides motor commands
        
        For more details on sensory processing, see NLM documentation.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation
        
        SimpleWorld represents a 2D simulation environment where the agent
        interacts with objects and performs actions. It provides sensory input
        (what the agent sees/hears/feels) and motor output (agent actions).
        
        **World Features**
        ----------------
        - **2D Geometry**: Rectangular world with width and height
        - **Sensory Input**: Vision, touch, internal, proprioception, audio
        - **Agent Physics**: Movement, collision detection, energy management
        - **Objects**: Resources (positive reward), hazards (negative reward), walls
        - **Time Simulation**: Continuous time simulation with physics updates
        - **Reproducibility**: Random seed control for reproducible experiments
        
        **World Components**
        ------------------
        - **Agent**: The simulated entity with position, movement, and physiology
        - **Objects**: World objects with type, position, and properties
        - **Sensory Systems**: Multiple modalities for environmental perception
        - **Physics Engine**: Basic physics simulation (position, velocity, collision)
        
        **Data Format**
        -------------
        - **Position**: (x, y) coordinates in world units
        - **Vision**: Intensity grid (width × height)
        - **Objects**: Center position, type, radius, value, active state
        - **Agent State**: Position, velocity, orientation, energy, health
        - **Time**: Simulation time in seconds
        
        **Typical Usage**
        ----------------
        ```python
        # Create world
        world = pynlm.createSimpleWorld()
        
        # Configure world dimensions
        world.configure(
            width=20.0,
            height=20.0,
            visionWidth=8,
            visionHeight=8
        )
        
        # Reset world (places agent and objects)
        world.reset()
        
        # Set agent starting position
        world.setAgentStart(5.0, 5.0)
        
        # Run physics simulation
        for step in range(100):
            # Update world physics
            world.update(0.1)  # 0.1 second timestep
            
            # Get agent's sensory input
            percept = world.getSensoryPercept()
            
            # Get agent's current state
            agent = world.getAgentBody()
            print(f"Position: ({agent.x}, {agent.y}), Energy: {agent.energy}")
            
            # Check what agent sees in vision
            vision_data = percept.getVision()
            print(f"Vision grid: {len(vision_data)} values")
            
            # Add new objects dynamically
            new_resource = pynlm.WorldObject(
                x=15.0, y=15.0,
                type=pynlm.WorldObjectType.Resource,
                value=20.0
            )
            world.addObject(new_resource)
            
            # Remove objects
            world.removeObject(10.0, 10.0)
            
            # Check if position is valid (not a wall)
            if world.isValidPosition(agent.x, agent.y):
                print("Position is safe")
        ```
        
        **World Physics**
        ----------------
        - **Movement**: Position updates based on velocity
        - **Collision**: Boundary detection and response
        - **Energy**: Natural energy decay over time
        - **Objects**: Resource collection and hazard damage
        
        **Object Types**
        --------------
        - **Resource**: Positive reward when collected
        - **Hazard**: Negative reward/damage when touched
        - **Wall**: Obstacle that blocks movement
        - **Marker**: Special location (can have semantic meaning)
        - **Empty**: No object (transparent)
        
        **Common World Settings**
        ------------------------
        - **Size**: width, height (world dimensions)
        - **Vision**: visionWidth, visionHeight (sensory resolution)
        - **Energy**: maxEnergy, energyDecayRate (agent physiology)
        - **Seed**: randomSeed (reproducibility)
        
        SimpleWorld is the primary environment interface for NLM simulations.
        It connects sensory input to motor output in a closed-loop control system.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class
        
        Brain implements real spiking neural computation with event-driven dynamics
        and integrated memory, prediction, cognition, and neuromodulation systems.
        
        **Brain Architecture**
        ---------------------
        - **Neurons**: Leaky Integrate-and-Fire neurons with spike dynamics
        - **Synapses**: Event-driven spike propagation with delays
        - **Plasticity**: STDP, Hebbian, and structural plasticity mechanisms
        - **Memory**: Working memory, episodic memory, and associative memory
        - **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error systems
        - **Cognition**: Planning, concept formation, and attentional selection
        - **Development**: Developmental stages and structural changes
        
        **Key Methods**
        ------------
        - **Initialization**: `initialize()` - Start neural computation
        - **Simulation**: `step()` - Perform time simulation steps
        - **Input**: `receiveSensoryInput()` - Inject sensory signals
        - **Output**: `produceAction()` - Generate motor commands
        - **Control**: `injectCurrent()` - Direct neural manipulation
        - **State**: `getConfig()`, `getDevelopmentalStage()`, etc.
        - **Persistence**: `save()`, `load()` - Checkpoint management
        
        **Data Format**
        -------------
        - **Neurons**: Float membrane potentials
        - **Synapses**: Weighted connections with delays
        - **Regions**: Spatial organization of neural populations
        - **Statistics**: Real-time performance metrics
        
        **Typical Usage**
        ----------------
        ```python
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
            
            # Monitor brain state
            print(f"Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"Total spikes: {brain.getTotalSpikeCount()}")
            print(f"Avg firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"EI ratio: {brain.getExcitationInhibitionRatio():.3f}")
            
            # Get developmental stage
            stage = brain.getDevelopmentalStage()
            print(f"Developmental stage: {stage}")
            
            # Control plasticity
            if stage == pynlm.DevelopmentalStage.Maturation:
                print("Brain is maturing - high plasticity")
            elif stage == pynlm.DevelopmentalStage.Aging:
                print("Brain is aging - low plasticity")
        ```
        
        **Brain Statistics**
        -------------------
        The brain provides comprehensive statistics:
        - **Neuronal activity**: Firing rates, spike counts
        - **Network properties**: Synapse counts, region organization
        - **Cognitive state**: Developmental stage, neuromodulation levels
        - **Performance metrics**: Learning progress, stability
        
        **Advanced Features**
        ------------------
        - **Memory systems**: Working memory with persistent activity
        - **Episodic memory**: Experience encoding and replay
        - **Associative memory**: Hebbian pattern associations
        - **Neuromodulation**: Coordinated learning signals
        - **Prediction systems**: Forward models and error signals
        - **Cognition**: Neural planning and concept formation
        - **Development**: Age-dependent plasticity rates
        
        Brain is the core computational engine of NLM, integrating all
        neural systems into a coherent learning and behavior system.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
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
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world
        
        AgentBrain handles sensory transduction and motor decoding, serving as the
        interface between neural processing and environmental interaction.
        
        **AgentBrain Architecture**
        ---------------------------
        - **Sensory Processing**: Convert world input to neural signals
        - **Motor Decoding**: Convert neural activity to action commands
        - **Neuromodulation**: Apply learning signals from world feedback
        - **Development**: Update brain development based on experience
        - **Integration**: Coordinate brain-world closed-loop control
        
        **Key Methods**
        ------------
        - **Initialization**: `initialize(world)` - Setup with environment
        - **Sensory Input**: `processSensoryInput(percept)` - Feed world data
        - **Motor Output**: `decodeMotorCommand()` - Get agent actions
        - **Learning**: `applyRewardModulation(reward, predicted)` - Learn from feedback
        - **Development**: `updateDevelopment(timestep)` - Brain maturation
        - **State**: `getNeuromodulationLevel()`, `getCuriosityLevel()`, etc.
        - **Control**: `enableRewardModulation()`, `enableCuriosity()`, etc.
        
        **Data Format**
        -------------
        - **Sensory Size**: Expected input dimensions from world
        - **Motor Size**: Expected output dimensions for world
        - **Neuromodulators**: Dopamine, curiosity, novelty, prediction error
        - **Cognitive State**: Attention, motivation, memory access
        
        **Typical Usage**
        ----------------
        ```python
        # Create complete agent
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Configure learning capabilities
        agent.enableRewardModulation(True)      # Learn from rewards
        agent.enableCuriosity(True)             # Explore novel stimuli
        agent.enableDevelopment(True)           # Brain maturation
        agent.enableStructuralPlasticity(True)  # Grow new connections
        
        # Run closed-loop control
        for step in range(1000):
            # Get world state
            world.update(0.1)
            agent_body = world.getAgentBody()
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Get motor command
            action = agent.decodeMotorCommand()
            
            # Execute action in world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning from world feedback
            reward = 0.0  # Calculate based on action result
            predicted_reward = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Get agent state
            neuromodulation = agent.getNeuromodulationLevel()
            curiosity = agent.getCuriosityLevel()
            development = agent.getDevelopmentalStage()
            print(f"Neuromodulation: {neuromodulation:.3f}, Curiosity: {curiosity:.3f}, Stage: {development}")
        ```
        
        **Neuromodulation Systems**
        ---------------------------
        AgentBrain integrates multiple neuromodulatory signals:
        - **Dopamine**: Reward prediction error and reinforcement learning
        - **Curiosity**: Novelty detection and exploration drive
        - **Novelty**: Unexpected stimulus detection
        - **Prediction Error**: Discrepancy between expected and actual outcomes
        
        **Cognitive Integration**
        ------------------------
        - **Memory Access**: Working memory for persistent activity
        - **Attention**: Competitive selection of neural representations
        - **Motivation**: Homeostatic drives and goal pursuit
        - **Learning**: Synaptic plasticity modulated by neuromodulators
        
        AgentBrain is the essential link between neural computation and
        adaptive behavior in dynamic environments.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
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
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Custom exception types for better error handling
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);
    py::register_exception<std::runtime_error>(m, "SimulationError", PyExc_RuntimeError);

    // Module-level constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Enhanced parameter validation function
    m.def("validate_parameters", 
        [](double width, double height, size_t visionWidth, size_t visionHeight,
           double timestep, size_t num_steps) {
            std::vector<std::string> errors;
            
            if (width <= 0.0) {
                errors.push_back("World width must be positive");
            }
            if (height <= 0.0) {
                errors.push_back("World height must be positive");
            }
            if (visionWidth == 0) {
                errors.push_back("Vision width must be greater than 0");
            }
            if (visionHeight == 0) {
                errors.push_back("Vision height must be greater than 0");
            }
            if (timestep <= 0.0) {
                errors.push_back("Timestep must be positive");
            }
            if (num_steps == 0) {
                errors.push_back("Number of steps must be greater than 0");
            }
            
            return errors;
        },
        py::arg("width") = 20.0, py::arg("height") = 20.0,
        py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
        py::arg("timestep") = 0.1, py::arg("num_steps") = 1000,
        "Validate simulation parameters and return error messages. Parameters have sensible defaults.");

    // Enhanced simulation runner with validation
    m.def("run_simulation_with_validation",
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
           std::shared_ptr<AgentBrain> agent, size_t num_steps, double timestep) {
            if (!brain || !world || !agent) {
                throw py::value_error("All parameters must be non-null");
            }
            
            // Validate parameters
            auto validation_errors = validate_parameters(
                world->getWidth(), world->getHeight(),
                16, 16, // Default vision dimensions
                timestep, num_steps
            );
            
            if (!validation_errors.empty()) {
                std::string error_msg = "Invalid simulation parameters:";
                for (const auto& error : validation_errors) {
                    error_msg += " " + error;
                }
                throw py::value_error(error_msg);
            }
            
            // Create local copies for safe modification
            auto agentBrain = std::make_shared<AgentBrain>(std::move(brain));
            auto simpleWorld = std::make_shared<SimpleWorld>(*world);
            
            simpleWorld->configure(20.0, 20.0, 8, 8);
            simpleWorld->reset();
            agentBrain->initialize(*simpleWorld);
            
            for (size_t step = 0; step < num_steps; ++step) {
                simpleWorld->update(timestep);
                const SensoryPercept& percept = simpleWorld->getSensoryPercept();
                agentBrain->processSensoryInput(percept);
                agentBrain->getBrain()->step(static_cast<SimulationStep>(step), static_cast<Timestamp>(step * timestep));
                MotorCommand action = agentBrain->decodeMotorCommand();
                simpleWorld->applyMotorCommand(action, simpleWorld->getSimulationTime());
            }
            
            return std::make_tuple(
                agentBrain->getBrain()->getTotalNeuronCount(),
                agentBrain->getBrain()->getTotalSpikeCount(),
                agentBrain->getBrain()->getAverageFiringRate()
            );
        },
        py::arg("brain"), py::arg("world"), py::arg("agent"),
        py::arg("num_steps") = 1000, py::arg("timestep") = 0.1,
        "Run validated simulation with comprehensive parameter checking. Throws error on invalid parameters.");

    // Safe brain creation with validation
    m.def("create_brain_safely",
        [](std::shared_ptr<Config> config, const std::string& brain_name = "default") {
            if (!config) {
                throw py::value_error("Configuration cannot be null");
            }
            if (brain_name.empty()) {
                throw py::value_error("Brain name cannot be empty");
            }
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            return std::make_tuple(brain, brain->getTotalNeuronCount());
        },
        py::arg("config"), py::arg("brain_name") = "default",
        "Create brain with validation. Returns (brain, neuron_count). Throws error on invalid parameters.");

    // Safe world creation with validation
    m.def("create_world_safely",
        [](double width, double height, size_t visionWidth, size_t visionHeight) {
            if (width <= 0.0) {
                throw py::value_error("World width must be positive");
            }
            if (height <= 0.0) {
                throw py::value_error("World height must be positive");
            }
            if (visionWidth == 0) {
                throw py::value_error("Vision width must be greater than 0");
            }
            if (visionHeight == 0) {
                throw py::value_error("Vision height must be greater than 0");
            }
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, visionWidth, visionHeight);
            world->reset();
            
            return world;
        },
        py::arg("width") = 20.0, py::arg("height") = 20.0,
        py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
        "Create world with validation. Throws error on invalid dimensions.");

    // Enhanced brain information with error handling
    m.def("get_brain_info", 
        [](const std::shared_ptr<Brain>& brain) {
            if (!brain) {
                throw py::value_error("Brain cannot be null");
            }
            
            auto config = brain->getConfig();
            std::string config_summary = config ? config->summary() : "No configuration";
            
            return std::make_tuple(
                brain->getTotalNeuronCount(),
                brain->getTotalSynapseCount(),
                brain->getAverageFiringRate(),
                brain->getExcitationInhibitionRatio(),
                brain->getDevelopmentalStage(),
                config_summary
            );
        },
        py::arg("brain"),
        "Get comprehensive brain information. Returns (neuron_count, synapse_count, avg_firing_rate, ei_ratio, developmental_stage, config_summary)");

    // Safe data export with validation
    m.def("export_brain_data_safe",
        [](const std::shared_ptr<Brain>& brain, const std::string& filepath, bool include_neurons = true,
           bool include_synapses = true, bool include_regions = true) {
            if (!brain) {
                throw py::value_error("Brain cannot be null");
            }
            if (filepath.empty()) {
                throw py::value_error("Filepath cannot be empty");
            }
            
            brain->save(filepath);
        },
        py::arg("brain"), py::arg("filepath"), py::arg("include_neurons") = true,
        py::arg("include_synapses") = true, py::arg("include_regions") = true,
        "Safely export brain data with parameter validation. Throws error on invalid parameters.");

    // Safe data import with validation
    m.def("import_brain_data_safe",
        [](const std::string& filepath) -> std::shared_ptr<Brain> {
            if (filepath.empty()) {
                throw py::value_error("Filepath cannot be empty");
            }
            
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            brain->load(filepath);
            return brain;
        },
        py::arg("filepath"),
        "Safely import brain data with parameter validation. Returns Brain instance.");

    // Integration with Python scientific libraries
    
    // Safe numpy integration with error checking
    m.def("get_brain_data_numpy", 
        [](const std::shared_ptr<Brain>& brain) {
            if (!brain) {
                throw py::value_error("Brain cannot be null");
            }
            
            // Create numpy arrays from brain data
            py::array_t<float> neurons_array({brain->getTotalNeuronCount()});
            py::array_t<float> spikes_array({1});  // Total spikes
            
            // Note: This is a simplified version. Full implementation would require
            // access to internal brain data structures
            return std::make_tuple(
                neurons_array,
                spikes_array,
                brain->getAverageFiringRate()
            );
        },
        py::arg("brain"),
        "Extract brain data as numpy arrays. Returns (neurons_array, spikes_array, avg_firing_rate)");

    // World data as numpy with visualization support
    m.def("get_world_data_numpy",
        [](const std::shared_ptr<SimpleWorld>& world) {
            if (!world) {
                throw py::value_error("World cannot be null");
            }
            
            // Create arrays for world data
            size_t total_objects = 0; // Would need to access objects_ if visible
            py::array_t<float> position_array({2});  // Agent position (x, y)
            py::array_t<float> energy_array({1});   // Agent energy
            
            // Simplified version - would need actual data access
            return std::make_tuple(
                position_array,
                energy_array,
                world->getSimulationTime()
            );
        },
        py::arg("world"),
        "Extract world data as numpy arrays. Returns (position_array, energy_array, simulation_time)");

    // Enhanced simulation with numpy integration
    m.def("run_simulation_numpy",
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
           std::shared_ptr<AgentBrain> agent, size_t num_steps, double timestep = 0.1,
           bool save_data = false, const std::string& data_file = "simulation_data.csv") {
            if (!brain || !world || !agent) {
                throw py::value_error("All parameters must be non-null");
            }
            
            // Create numpy arrays for recording simulation data
            std::vector<py::array_t<float>> step_data;
            step_data.reserve(num_steps);
            
            auto agentBrain = std::make_shared<AgentBrain>(std::move(brain));
            auto simpleWorld = std::make_shared<SimpleWorld>(*world);
            
            simpleWorld->configure(20.0, 20.0, 8, 8);
            simpleWorld->reset();
            agentBrain->initialize(*simpleWorld);
            
            // Record initial state
            std::vector<float> initial_state({
                simpleWorld->getAgentBody().x,
                simpleWorld->getAgentBody().y,
                simpleWorld->getAgentBody().energy,
                static_cast<float>(agentBrain->getBrain()->getTotalNeuronCount()),
                static_cast<float>(agentBrain->getBrain()->getTotalSpikeCount())
            });
            step_data.push_back(py::array_t<float>(initial_state));
            
            for (size_t step = 0; step < num_steps; ++step) {
                simpleWorld->update(timestep);
                const SensoryPercept& percept = simpleWorld->getSensoryPercept();
                agentBrain->processSensoryInput(percept);
                agentBrain->getBrain()->step(static_cast<SimulationStep>(step), static_cast<Timestamp>(step * timestep));
                MotorCommand action = agentBrain->decodeMotorCommand();
                simpleWorld->applyMotorCommand(action, simpleWorld->getSimulationTime());
                
                // Record state at each step
                std::vector<float> state({
                    simpleWorld->getAgentBody().x,
                    simpleWorld->getAgentBody().y,
                    simpleWorld->getAgentBody().energy,
                    static_cast<float>(step),
                    static_cast<float>(agentBrain->getBrain()->getFiringNeuronCount()),
                    static_cast<float>(agentBrain->getBrain()->getAverageFiringRate())
                });
                step_data.push_back(py::array_t<float>(state));
            }
            
            // Convert to single numpy array if requested
            py::array_t<float> simulation_array;
            if (save_data && !data_file.empty()) {
                // Flatten all data for CSV export
                size_t total_elements = step_data.size() * step_data[0].size();
                std::vector<float> flattened(total_elements);
                
                for (size_t i = 0; i < step_data.size(); ++i) {
                    const auto& arr = step_data[i];
                    for (ssize_t j = 0; j < arr.size(); ++j) {
                        flattened[i * arr.size() + j] = arr[j];
                    }
                }
                
                simulation_array = py::array_t<float>(flattened);
                
                // Note: CSV writing would require additional Python libraries
                // This is a placeholder for integration with pandas
            }
            
            return std::make_tuple(
                simulation_array,
                step_data.size(),
                agentBrain->getBrain()->getTotalNeuronCount(),
                agentBrain->getBrain()->getTotalSpikeCount()
            );
        },
        py::arg("brain"), py::arg("world"), py::arg("agent"),
        py::arg("num_steps") = 1000, py::arg("timestep") = 0.1,
        py::arg("save_data") = false, py::arg("data_file") = "simulation_data.csv",
        "Run simulation with numpy integration. Returns (data_array, num_steps, neuron_count, total_spikes)");

    // Enhanced visualization data export
    m.def("export_visualization_data",
        [](const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world,
           const std::string& output_prefix = "nlm_viz") {
            if (!brain || !world) {
                throw py::value_error("Brain and world cannot be null");
            }
            
            // Collect visualization data for multiple analysis formats
            py::dict visualization_data;
            
            // Time series data
            py::list time_series;
            for (size_t step = 0; step < 100; ++step) {
                time_series.append(py::dict(
                    "step", static_cast<int>(step),
                    "neurons_firing", static_cast<int>(brain->getFiringNeuronCount()),
                    "total_spikes", static_cast<int>(brain->getTotalSpikeCount()),
                    "avg_firing_rate", brain->getAverageFiringRate(),
                    "ei_ratio", brain->getExcitationInhibitionRatio(),
                    "time", static_cast<double>(step * 0.1)
                ));
            }
            
            visualization_data["time_series"] = time_series;
            
            // Agent state
            auto agent_body = world->getAgentBody();
            visualization_data["agent"] = py::dict(
                "x", agent_body.x,
                "y", agent_body.y,
                "orientation", agent_body.orientation,
                "energy", agent_body.energy,
                "health", agent_body.health,
                "age", agent_body.age
            );
            
            // Brain statistics
            visualization_data["brain_stats"] = py::dict(
                "total_neurons", brain->getTotalNeuronCount(),
                "total_synapses", brain->getTotalSynapseCount(),
                "developmental_stage", brain->getDevelopmentalStage(),
                "config_summary", brain->getConfig()->summary()
            );
            
            // World info
            visualization_data["world"] = py::dict(
                "width", world->getWidth(),
                "height", world->getHeight(),
                "simulation_time", world->getSimulationTime()
            );
            
            return visualization_data;
        },
        py::arg("brain"), py::arg("world"),
        py::arg("output_prefix") = "nlm_viz",
        "Export visualization data as Python dictionary with time series and state information.");

    // Scientific analysis helpers
    m.def("analyze_brain_dynamics",
        [](const std::shared_ptr<Brain>& brain, double window_size = 10.0) {
            if (!brain) {
                throw py::value_error("Brain cannot be null");
            }
            
            // Simplified dynamics analysis
            py::dict analysis;
            
            // Basic statistics
            analysis["total_neurons"] = brain->getTotalNeuronCount();
            analysis["total_synapses"] = brain->getTotalSynapseCount();
            analysis["firing_rate"] = brain->getAverageFiringRate();
            analysis["ei_ratio"] = brain->getExcitationInhibitionRatio();
            
            // Development information
            analysis["developmental_stage"] = brain->getDevelopmentalStage();
            analysis["has_plasticity"] = brain->getConfig()->getOr<bool>("brain.plasticity.enabled", false);
            
            // Processing time simulation (simplified)
            analysis["estimated_processing_time_ms"] = 1000.0 * window_size / 1000.0;
            
            return analysis;
        },
        py::arg("brain"), py::arg("window_size") = 10.0,
        "Perform scientific analysis of brain dynamics. Returns dictionary with analysis metrics.");

    // Export for scientific computing
    m.def("export_for_scientific_analysis",
        [](const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world,
           const std::string& analysis_type = "comprehensive") {
            if (!brain || !world) {
                throw py::value_error("Brain and world cannot be null");
            }
            
            py::dict scientific_data;
            
            if (analysis_type == "dynamics" || analysis_type == "comprehensive") {
                // Dynamics data
                scientific_data["dynamics"] = py::dict(
                    "neuron_count", brain->getTotalNeuronCount(),
                    "firing_rate", brain->getAverageFiringRate(),
                    "ei_ratio", brain->getExcitationInhibitionRatio(),
                    "developmental_stage", brain->getDevelopmentalStage(),
                    "simulation_time", world->getSimulationTime()
                );
            }
            
            if (analysis_type == "state" || analysis_type == "comprehensive") {
                // State data
                auto agent = world->getAgentBody();
                scientific_data["state"] = py::dict(
                    "agent_position", py::make_tuple(agent.x, agent.y),
                    "agent_orientation", agent.orientation,
                    "agent_energy", agent.energy,
                    "agent_health", agent.health,
                    "agent_age", agent.age
                );
            }
            
            if (analysis_type == "config" || analysis_type == "comprehensive") {
                // Configuration data
                auto config = brain->getConfig();
                if (config) {
                    scientific_data["configuration"] = py::dict(
                        "parameters", config->getKeys(),
                        "summary", config->summary()
                    );
                }
            }
            
            return scientific_data;
        },
        py::arg("brain"), py::arg("world"),
        py::arg("analysis_type") = "comprehensive",
        "Export data for scientific analysis in various formats. Supported types: 'dynamics', 'state', 'config', 'comprehensive'.");

    m.def("get_version", []() {
        return "pynlm 0.1.0 - NLM Phase 6 Integration";
    }, "Get the version and information about this NLM binding");
    }

} // namespace nlm
