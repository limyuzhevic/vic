#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python API
        =========================================

        **NLM (熙然, meaning "serene flow")** is an experimental artificial developmental brain system
        that implements a complete spiking neural network with learning, memory, and development capabilities.

        This module provides Python bindings to the NLM C++ framework, enabling:
        - Event-driven spiking neural network simulation
        - Multiple learning mechanisms (STDP, Hebbian, structural plasticity)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Working memory, episodic memory, and associative memory systems
        - Prediction and planning systems
        - Development through multiple life stages
        - Direct interaction with simulated environments

        # Key Components

        ## Brain
        The central neural processor that implements real spiking neuron dynamics
        using the Leaky Integrate-and-Fire (LIF) model. The brain coordinates all
        neural subsystems including memory, prediction, cognition, and neuromodulation.

        ## Config
        Configuration system for all NLM parameters using hierarchical JSON-like keys.
        Control neuron properties, connectivity, learning rates, and all system parameters.

        ## AgentBrain
        Interface layer that connects the brain to the external world, handling:
        - Sensory transduction and input processing
        - Motor command decoding from neural activity
        - Reward-based neuromodulation
        - Developmental updates and learning

        ## SimpleWorld
        2D simulated environment with:
        - Dynamic object rendering and collision detection
        - Vision, touch, and proprioception sensors
        - Energy decay and resource management
        - Action execution and feedback

        ## Mathematical Models

        ### Leaky Integrate-and-Fire (LIF) Neuron
        ```math
        \tau \frac{dV}{dt} = -(V - V_{rest}) + I_{syn}
        \text{if } V \geq V_{thresh}: \\text{fire spike and reset } V = V_{reset}
        ```

        ### Spike-Timing-Dependent Plasticity (STDP)
        Temporal learning rule that strengthens synapses for causal spike pairs:
        - **LTP (Long-Term Potentiation)**: Pre-before-post spikes → weight increase
        - **LTD (Long-Term Depression)**: Post-before-pre spikes → weight decrease

        ### Reward-Modulated STDP (R-STDP)
        Integrates reward prediction error to modulate learning strength:
        ```math
        \Delta w = \, (1 - R_{pred}) \\times \\Delta w_{standard}
        ```

        # Quick Start Examples

        ## Basic Brain Simulation
        ```python
        import pynlm

        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        # Run simulation
        for step in range(1000):
            brain.step(step)

        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        ```

        ## Complete Agent Simulation
        ```python
        import pynlm

        def run_agent_simulation(config, num_steps=1000):
            # Create systems
            brain = pynlm.createBrain(config)
            brain.initialize()
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            agent.initialize(world)

            # Enable learning features
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)

            # Run episode
            for step in range(num_steps):
                world.update(0.1)  # Time step
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

            return brain, world, agent
        ```

        ## Parameter Tuning for Different Scenarios

        ### Exploration Agent (High Curiosity)
        ```python
        config = pynlm.createDefaultConfig()
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.exploration_strength", 1.5)
        ```

        ### Stable Learning (Low Learning Rate)
        ```python
        config = pynlm.createDefaultConfig()
        config.set("plasticity.stdp.learning_rate", 0.0001)
        config.set("plasticity.hebbian.learning_rate", 0.0001)
        ```

        ### Rapid Development (Critical Period)
        ```python
        config = pynlm.createDefaultConfig()
        config.set("development.stage", "CriticalPeriod")
        config.set("development.plasticity_multiplier", 3.0)
        ```

        # Detailed Parameter Documentation

        ## Config Class

        Configuration system for controlling all NLM parameters through key-value pairs.
        Supports multiple value types (int, float, bool, string, arrays).

        ### Configuration Categories

        **Brain Parameters**:
        - `brain.neuron_count`: Total number of neurons
        - `brain.synapse_density`: Probability of synaptic connections
        - `brain.v_thresh`: Neuron threshold potential (mV)
        - `brain.v_rest`: Resting potential (mV)
        - `brain.tau_mem`: Membrane time constant (ms)

        **Plasticity Parameters**:
        - `plasticity.stdp.enable`: Enable STDP learning
        - `plasticity.stdp.learning_rate`: STDP learning strength
        - `plasticity.hebbian.enable`: Enable Hebbian learning
        - `plasticity.hebbian.learning_rate`: Hebbian learning strength

        **Neuromodulation Parameters**:
        - `neuromod.dopamine.scale`: Reward modulation strength
        - `neuromod.curiosity.enable`: Enable curiosity-driven exploration
        - `neuromod.novelty.enable`: Enable novelty detection

        ## Brain Class

        The central neural processor with complete spiking neural dynamics.

        ### Mathematical Models

        **LIF Neuron Dynamics**:
        - Membrane potential evolves according to:
          ```math
          V(t+dt) = V(t) + dt * (-(V(t)-V_rest) + I_syn(t))/tau_mem
          ```
        - When V(t) >= V_thresh: neuron fires, current time is recorded
        - After firing, V is reset to V_reset

        **Excitatory-Inhibitory Balance**:
        - Excitation-to-inhibition ratio maintained for stability
        - Prevents runaway excitation in neural populations

        ### Learning Mechanisms

        **STDP (Spike-Timing-Dependent Plasticity)**:
        - Temporal credit assignment based on spike timing
        - Pre-synaptic before post-synaptic: LTP (weight increase)
        - Post-synaptic before pre-synaptic: LTD (weight decrease)
        - Time window typically 20-50ms

        **Hebbian Learning**:
        - Co-activation based learning
        - Correlates neural activity patterns
        - Strength increases with simultaneous firing

        ### Key Methods

        **Simulation Control**:
        - `step(step)`: Advance simulation by one time step
        - `reset()`: Reset brain to initial state
        - `save(filename)`: Checkpoint brain state
        - `load(filename)`: Load saved brain state

        **Sensory Processing**:
        - `receiveSensoryInput(input)`: Inject external input into sensory neurons
        - `injectCurrent(neuron, current)`: Direct current injection

        **Action Production**:
        - `produceAction()`: Decode neural activity into motor commands
        - `applyNeuromodulation(signal)`: Apply neuromodulatory signals

        **Statistics and Monitoring**:
        - `getTotalSpikeCount()`: Total spikes fired
        - `getFiringNeuronCount()`: Currently firing neurons
        - `getAverageFiringRate()`: Population firing rate (Hz)
        - `getExcitationInhibitionRatio()`: E/I balance

        ## AgentBrain Class

        Interface connecting the brain to environmental interaction.

        ### Sensory Processing Pipeline
        1. **World Perception**: Visual, tactile, and proprioceptive input
        2. **Signal Transduction**: Convert sensory data to neural current
        3. **Brain Injection**: Inject processed signals into appropriate neurons

        ### Motor Decoding Pipeline
        1. **Neural Activity Analysis**: Analyze motor neuron populations
        2. **Command Selection**: Map neural patterns to action types
        3. **Parameter Extraction**: Extract action parameters (position, velocity)

        ### Neuromodulation Systems

        **Dopamine (Reward)**:
        - Reinforces successful actions
        - Modulates plasticity strength
        - Affects decision-making confidence

        **Curiosity**: - Drives exploration of novel states
        - Increases learning in uncertain environments
        - Reduces stimulus response tracking

        **Novelty Detection**: - Detects sensory changes
        - Triggers adaptation processes
        - Promotes generalization

        ## SimpleWorld Class

        2D simulated environment with rich sensory capabilities.

        ### Vision System
        - Pixel-based visual input with configurable resolution
        - FOV (Field of View) based on agent orientation
        - Dynamic lighting and occlusion

        ### Physical Simulation
        - Position-based object interactions
        - Energy conservation and decay
        - Collision detection and response

        ### Action Execution
        - Discrete motor commands (move, turn, interact, etc.)
        - Parameter control (speed, direction, duration)
        - Success/failure feedback with rewards

        # Advanced Usage Patterns

        ## Memory Integration
        ```python
        # Access working memory
        working_memory = brain.getWorkingMemory()
        # Store/retrieve patterns
        working_memory.store_pattern(neural_pattern)
        retrieved = working_memory.retrieve_pattern(pattern_id)
        ```

        ## Development Control
        ```python
        # Manually control development stages
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
        # Monitor development metrics
        stage = brain.getDevelopmentalStage()
        age = brain.getBrain().getDevelopmentSystem().getAge()
        ```

        ## Checkpoint and Replay
        ```python
        # Save brain state
        brain.save("experiment_checkpoint.pkl")

        # Load later (continue from saved state)
        brain.load("experiment_checkpoint.pkl")
        ```

        # Cross-References to C++ Implementation

        For detailed C++ implementation details, refer to:
        - **Brain.hpp**: Core neural processing class
        - **Neuron.hpp**: LIF neuron implementation
        - **STDP.hpp**: Spike-timing dependent plasticity
        - **Config.hpp**: Configuration management
        - **SimpleWorld.hpp**: Environment simulation

        # Error Handling and Troubleshooting

        ## Common Issues and Solutions

        **Problem**: Brain not initializing properly
        **Solution**:
        ```python
        # Check configuration
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        if not brain.initialize():
            print("Initialization failed")
        ```

        **Problem**: Segmentation fault during simulation
        **Solution**:
        ```python
        # Always call reset before starting new simulation
        brain.reset()
        world.reset()
        agent.reset()
        ```

        **Problem**: No neuronal activity
        **Solution**:
        ```python
        # Ensure sensory input is connected
        agent.processSensoryInput(world.getSensoryPercept())
        
        # Enable neuromodulation for learning
        agent.enableRewardModulation(True)
        ```

        **Problem**: Simulation too slow
        **Solution**:
        ```python
        # Optimize configuration for performance
        config.set("brain.neuron_count", 500)  # Start smaller
        config.set("performance.optimization", "fast")
        ```

        # Performance Considerations

        ## Scaling Guidelines

        - **Small Brains (<1000 neurons)**: Use full feature set for development
        - **Medium Brains (1000-5000 neurons)**: Enable neuromodulation
        - **Large Brains (>5000 neurons)**: Focus on performance-critical features

        ## Parameter Tuning Table

        | Scenario | Neuron Count | STDP | Hebbian | Neuromodulation | Development |
        |----------|--------------|------|---------|-----------------|-------------|
        | Research | 100-500 | Enable | Enable | Curiosity only | CriticalPeriod |
        | Development | 500-2000 | Enable | Enable | All enabled | Full lifecycle |
        | Production | 1000-10000 | Enable | Disable | Reward only | Adult |

        # Acknowledgments

        This module implements Phase 6 integration of the NLM research project:
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and planning systems
        - Full developmental lifecycle
        - Checkpoint persistence

        For more information, see:
        - [ARCHITECTURE.md](docs/ARCHITECTURE.md)
        - [SCIENCE.md](docs/SCIENCE.md)
        - [EXPERIMENTS.md](docs/EXPERIMENTS.md)
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

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

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration system for NLM neural simulation
        
        The Config class provides a flexible key-value configuration system
        using JSON-like hierarchical keys. This enables fine-grained control
        over all aspects of NLM operation including:
        
        - Brain parameters (neuron count, connectivity, neuron dynamics)
        - Plasticity rules (STDP, Hebbian, structural)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Development system (growth, maturation, aging)
        - Performance settings (memory pools, event queues)
        - Environment parameters (world dimensions, physics)
        
        Configuration values can be:
        - Integers and floats for numerical parameters
        - Booleans for enabling/disabling features
        - Strings for names and labels
        - Vectors for multi-dimensional parameters
        
        See the module docstring for detailed parameter categories
        and tuning guidelines for different scenarios.
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
        Abstract base class for all sensory input types
        
        SensoryInput represents raw sensory data from the environment before
        processing by the brain. This class provides the interface for all
        specific sensory modalities (Vision, Audio, InternalSignals).
        
        ## Sensory Categories
        
        1. **Vision**: Pixel-based visual information
           - Resolution: Configurable width x height
           - Channels: RGB (3) or grayscale (1)
           - Format: Floating-point values [0.0, 1.0]
        
        2. **Audio**: Sound signal data
           - Sample rate: Configurable sampling frequency
           - Duration: Number of samples
           - Format: Floating-point waveform
        
        3. **Internal Signals**: Physiological states
           - Heart rate, respiration, metabolic indicators
           - Provides internal body state information
        
        ## Common Methods
        All sensory types provide:
        - `getType()`: Returns the sensory modality type
        - `getData()`: Returns raw data as vector<float>
        - `getDimensions()`: Returns sensory dimensionality
        - `getTimestamp()`: Returns data acquisition time
        - `setTimestamp(time)`: Sets the acquisition timestamp
        
        Cross-reference: See src/sensory/SensoryInput.hpp for C++ implementation
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input - pixel-based visual representation
        
        Vision provides visual information from the environment as a grid of
        pixels, simulating a camera sensor. This is the primary modality for
        spatial awareness and object recognition.
        
        ## Vision Properties
        - **Resolution**: Width × Height pixels
        - **Channels**: 1 (grayscale) or 3 (RGB)
        - **Range**: Floating-point values normalized to [0.0, 1.0]
        - **Field of View**: Determined by agent orientation and distance
        
        ## Vision Data Access
        ```python
        vision = percept.getVision()  # Get Vision object
        width = vision.getWidth()
        height = vision.getHeight()
        channels = vision.getChannels()
        
        # Access pixel data (row-major order)
        data = vision.getData()  # vector<float>
        pixel_value = data[y * width + x]  # For grayscale
        r, g, b = data[(y * width + x) * 3 : (y * width + x) * 3 + 3]  # For RGB
        ```
        
        ## Common Use Cases
        1. **Object Detection**: Scan for specific colors or patterns
        2. **Spatial Navigation**: Analyze distances and positions
        3. **Scene Understanding**: Detect edges, textures, and objects
        4. **Depth Perception**: Calculate relative distances
        
        Cross-reference: See src/sensory/Vision.hpp for C++ implementation
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
        Audio sensory input - sound signal processing
        
        Audio provides auditory information from the environment as a time-series
        sound waveform. This modality enables agents to process environmental
        sounds, music, and acoustic cues for behavior guidance.
        
        ## Audio Properties
        - **Sample Rate**: Samples per second (e.g., 44.1 kHz, 48 kHz)
        - **Duration**: Total number of samples captured
        - **Format**: Floating-point amplitude values normalized to [-1.0, 1.0]
        - **Channels**: Mono (1) or stereo (2) typically
        
        ## Audio Data Access
        ```python
        audio = percept.getAudio()  # Get Audio object
        sample_rate = audio.getSampleRate()
        num_samples = audio.getNumSamples()
        
        # Access waveform data
        data = audio.getData()  # vector<float>
        amplitude = data[sample_index]  # Value between -1.0 and 1.0
        ```
        
        ## Common Use Cases
        1. **Sound Source Localization**: Determine direction of sounds
        2. **Speech Recognition**: Extract language and commands
        3. **Environmental Awareness**: Detect threats or opportunities
        4. **Emotional Cues**: Identify emotional content in sounds
        
        Cross-reference: See src/sensory/Audio.hpp for C++ implementation
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
        Internal signals sensory input - physiological states
        
        InternalSignals provides information about the agent's internal
        physiological state, such as heart rate, respiration, metabolic levels,
        and other homeostatic indicators. This modality enables interoception
        and self-monitoring capabilities.
        
        ## Internal Signal Types
        - **Heart Rate**: Beats per minute or interval between beats
        - **Respiration**: Breathing rate and depth
        - **Metabolic**: Energy levels, glucose concentration
        - **Temperature**: Body temperature regulation
        - **Hormones**: Chemical messengers affecting behavior
        
        ## Internal Data Access
        ```python
        internal = percept.getInternal()  # Get vector<float>
        heart_rate = internal[0]        # Index 0: heart rate
        respiration = internal[1]       # Index 1: respiration
        metabolism = internal[2]        # Index 2: metabolic level
        
        # Add more signals
        signals.addSignal(value)         # Add new signal measurement
        signals.clearSignals()           # Reset all signals
        ```
        
        ## Common Use Cases
        1. **Stress Detection**: Monitor physiological stress indicators
        2. **Energy Management**: Track energy levels for activity planning
        3. **Self-Regulation**: Adjust behavior based on internal state
        4. **Health Monitoring**: Detect anomalies in physiological patterns
        
        Cross-reference: See src/sensory/InternalSignals.hpp for C++ implementation
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output and behavior execution
        
        Action represents a discrete motor command or behavioral output that
        an agent produces based on its neural processing. Actions are the bridge
        between neural activity and environmental interaction.
        
        ## Action Components
        - **Type**: The fundamental action category (MoveForward, TurnLeft, etc.)
        - **Parameters**: Continuous values controlling action execution
        - **Name**: Human-readable description of the action
        - **Validation**: Success/failure status and associated rewards
        
        ## Action Types (Enum)
        - **Movement**: MoveForward, MoveBackward, MoveLeft, MoveRight
        - **Rotation**: TurnLeft, TurnRight
        - **Perception**: Look, LookUp, LookDown
        - **Interaction**: Interact, Eat, Drink, Rest, Wait
        - **Special**: Custom (for user-defined actions)
        
        ## Action Lifecycle
        1. **Selection**: Brain produces motor command via decodeMotorCommand()
        2. **Execution**: World applies motor command
        3. **Feedback**: World returns ActionResult with reward and status
        4. **Learning**: Agent applies reward modulation based on outcome
        
        ## Action Usage
        ```python
        # Create action
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        
        # Add parameters (e.g., speed, duration)
        params = [1.0, 0.5]  # speed=1.0, duration=0.5
        action.setParameters(params)
        
        # Get action properties
        action_type = action.getType()
        parameters = action.getParameters()
        action_name = action.getName()
        
        # Clone action for history or replay
        cloned = action.clone()
        ```
        
        Cross-reference: See src/motor/Action.hpp for C++ implementation
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
        World object representation with position, type, and properties
        
        WorldObject represents entities in the simulated environment that the
        agent can interact with. Objects have physical properties and semantic
        types that determine how they behave and what value they provide.
        
        ## Object Properties
        - **Position**: (x, y) coordinates in 2D space
        - **Type**: Semantic category determining behavior
          - Empty: No interaction possible
          - Resource: Positive value, can be collected
          - Hazard: Negative value, causes damage
          - Wall: Obstacle, blocks movement
          - Marker: Landmark, provides location reference
        - **Value**: Quantitative measure of object importance
          - Positive for resources (energy, food, water)
          - Negative for hazards (poison, traps)
          - Zero for neutral objects (walls, markers)
        - **Radius**: Physical size for collision detection
        - **Active**: State flag for object lifecycle management
        
        ## Object Types
        - **Empty**: Placeholder, no interaction
        - **Resource**: Collectible value (1.0 = full resource unit)
        - **Hazard**: Damage source (-1.0 = full damage)
        - **Wall**: Physical barrier, impenetrable
        - **Marker**: Reference point, no intrinsic value
        
        ## Interaction Patterns
        1. **Resource Collection**: Agent approaches, ActionType.Eat
           - Reward = object.value
           - Success if within radius and object.active
           - Object becomes inactive after collection
        
        2. **Hazard Avoidance**: Agent avoids or moves away
           - Reward = -object.value
           - Success depends on collision avoidance
           - May trigger defensive behaviors
        
        3. **Navigation**: Object serves as waypoint or landmark
           - Used for path planning and spatial memory
           - Provides reference for coordinate system
        
        ## World Management
        ```python
        # Create objects
        resource = pynlm.WorldObject(5.0, 3.0, pynlm.WorldObjectType.Resource, value=2.0)
        hazard = pynlm.WorldObject(8.0, 7.0, pynlm.WorldObjectType.Hazard, value=-1.5, radius=0.8)
        wall = pynlm.WorldObject(0.0, 0.0, pynlm.WorldObjectType.Wall, radius=1.0)
        
        # Add to world
        world.addObject(resource)
        world.addObject(hazard)
        world.addObject(wall)
        
        # Query objects
        object = world.getObjectAt(x, y)
        if object.type != pynlm.WorldObjectType.Empty:
            process_object(object)
        ```
        
        Cross-reference: WorldObject definition in src/world/SimpleWorld.hpp
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
        Agent body state and physical properties
        
        AgentBody represents the physical embodiment of an agent in the simulated
        environment. It contains position, orientation, velocity, and physiological
        properties that determine how the agent interacts with the world.
        
        ## Body Properties
        - **Position**: (x, y) coordinates in world space
        - **Orientation**: Angle in radians (0 = East, π/2 = North)
        - **Velocity**: Linear velocity components (vx, vy)
        - **Angular Velocity**: Rotational speed around center
        - **Energy**: Current energy reserves (0-100 scale)
        - **Health**: Health status (0 = dead, 100 = perfect health)
        - **Age**: Developmental age in simulation steps
        - **Movement Flags**: Boolean states for current actions
        - **Turn Flags**: Boolean states for rotational actions
        - **Last Action Time**: Timestamp of last successful action
        
        ## Physical State
        The body follows Newtonian mechanics:
        - **Motion**: Position changes based on velocity
        - **Rotation**: Orientation changes based on angular velocity
        - **Energy Decay**: Natural energy loss over time
        - **Collision Response**: Velocity changes based on impacts
        
        ## State Management
        ```python
        # Access body state
        body = world.getAgentBody()
        x = body.x
        y = body.y
        orientation = body.orientation
        velocity_x = body.velocityX
        velocity_y = body.velocityY
        
        # Check movement state
        is_moving = body.isMoving
        is_turning = body.isTurning
        
        # Energy management
        current_energy = body.energy
        max_energy = world.getMaxEnergy()
        energy_percentage = current_energy / max_energy
        
        # Health status
        health_status = body.health
        is_alive = health_status > 0
        ```
        
        ## Body Lifecycle
        1. **Initialization**: Set starting position and orientation
        2. **Motion**: Update based on motor commands
        3. **Interaction**: Experience collisions and environmental effects
        4. **Consumption**: Gain/lose energy based on actions
        5. **Development**: Age and change physiological properties
        
        Cross-reference: AgentBody definition in src/agent/AgentBody.hpp
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
        Action result from world execution with reward and feedback
        
        ActionResult represents the outcome of executing a motor command
        in the world environment. It provides essential feedback for learning
        and adaptation, including success status, reward value, and descriptive
        information about the action outcome.
        
        ## Result Components
        - **Reward**: Numerical value indicating the quality of outcome
          - Positive: Beneficial outcome (resource acquisition)
          - Negative: Detrimental outcome (hazard exposure)
          - Zero: Neutral outcome (no change in value)
        - **Success**: Boolean indicating if action executed successfully
          - True: Action completed as intended
          - False: Action failed or was constrained
        - **Message**: Human-readable description of outcome
          - Explains why action succeeded/failed
          - Contains additional context information
        
        ## Reward System
        The reward value integrates:
        - **Immediate Value**: Direct value of obtained resource or damage taken
        - **Effort Cost**: Energy expenditure required for the action
        - **Risk Assessment**: Probability of success vs. failure
        - **Learning Signal**: Magnitude of prediction error for neuromodulation
        
        ## Usage in Learning Loop
        ```python
        # Execute action in world
        result = world.applyMotorCommand(action, current_time)
        
        # Extract feedback
        reward = result.reward
        success = result.success
        message = result.message
        
        # Apply reward modulation (if enabled)
        if agent.isRewardModulationEnabled():
            agent.applyRewardModulation(reward, predicted_reward)
        ```
        
        ## Common Scenarios
        1. **Resource Collection**: High positive reward, success=True
        2. **Hazard Encounter**: Negative reward, success=False
        3. **Action Constraints**: Zero reward, success=False
        4. **Boundary Conditions**: Variable reward, success depends on position
        
        Cross-reference: Action execution interface in SimpleWorld.hpp
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Integrated sensory percept combining all modalities
        
        SensoryPercept represents the complete sensory input that an agent
        receives at a given moment in time. It integrates information from
        all sensory modalities (vision, audio, internal signals, proprioception)
        into a unified perceptual experience.
        
        ## Sensory Components
        - **Vision**: Pixel-based visual scene
        - **Touch**: Contact and proximity detection
        - **Internal**: Physiological state information
        - **Proprioception**: Body position and movement feedback
        - **Audio**: Sound signal processing
        
        ## Percept Structure
        The percept provides:
        - **Modality-Specific Data**: Raw sensor readings
        - **Synchronization**: All data synchronized to same timestamp
        - **Reference Frame**: World coordinates for spatial data
        - **Consistency**: Multiple modalities cross-validated
        
        ## Access Patterns
        ```python
        # Get complete sensory input
        percept = world.getSensoryPercept()
        
        # Vision access
        vision = percept.getVision()
        if vision is not None:
            width = vision.getWidth()
            height = vision.getHeight()
            data = vision.getData()
        
        # Touch access
        touch = percept.getTouch()
        if touch is not None:
            touch_data = touch.getData()
        
        # Internal signals
        internal = percept.getInternal()
        if internal is not None:
            heart_rate = internal[0]
            metabolism = internal[1]
        
        # Proprioception
        proprioception = percept.getProprioception()
        if proprioception is not None:
            x = proprioception.getData()[0]
            y = proprioception.getData()[1]
        
        # Audio
        audio = percept.getAudio()
        if audio is not None:
            sample_rate = audio.getSampleRate()
            waveform = audio.getData()
        
        # Timestamp access
        timestamp = percept.getTimestamp()
        ```
        
        ## Percept Lifecycle
        1. **Generation**: World creates percept at each simulation step
        2. **Transmission**: Agent receives percept via processSensoryInput()
        3. **Processing**: Brain extracts features and creates neural representation
        4. **Integration**: Multiple modalities combined for coherent perception
        
        Cross-reference: SensoryPercept definition in src/agent/SensoryPercept.hpp
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
        2D simulated environment for NLM agent interaction
        
        SimpleWorld provides a virtual physical environment where NLM agents
        interact with objects, learn from experiences, and develop capabilities.
        The world implements:
        
        ## Sensorimotor Loop
        - **Perception**: Visual, tactile, and proprioceptive sensors
        - **Action**: Discrete motor commands and continuous parameters
        - **Feedback**: Reward signals and environmental consequences
        
        ## Physical Model
        - 2D Euclidean space with discrete time steps
        - Newtonian mechanics with energy conservation
        - Collision detection and response
        - Dynamic object rendering and occlusion
        
        ## Sensory Systems
        1. **Vision**: Pixel-based camera with configurable FOV and resolution
           - Provides visual scene to agent via percept
           - Includes lighting, shadows, and object boundaries
        2. **Touch**: Position-based proximity and collision sensing
           - Detects object contact and surface properties
           - Provides haptic feedback for interaction
        3. **Proprioception**: Agent body state and orientation
           - Tracks position, velocity, and angular momentum
           - Enables coordination and balance
        
        ## Action System
        - **Discrete Commands**: Move, turn, look, interact, eat, drink, rest
        - **Continuous Parameters**: Speed, direction, duration, target position
        - **Success/Failure**: Binary outcomes with associated rewards
        
        ## Cross-References
        See src/world/SimpleWorld.hpp for C++ implementation and
        src/world/Observation.hpp for percept structure.
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
        Central neural processor with complete spiking neural dynamics
        
        The Brain class implements a complete artificial developmental brain
        based on the Leaky Integrate-and-Fire (LIF) neuron model. This is the
        core computational engine that:
        
        - Simulates event-driven spiking neural networks
        - Implements multiple learning mechanisms (STDP, Hebbian, structural)
        - Coordinates neuromodulation systems (dopamine, curiosity, novelty)
        - Manages memory systems (working, episodic, associative)
        - Performs prediction and planning computations
        - Controls developmental stages and plasticity
        
        The brain operates in discrete simulation steps where:
        1. Neurons integrate incoming currents
        2. Spikes are generated when threshold is reached
        3. Synapses update based on spike timing
        4. Neuromodulators influence learning and behavior
        5. Actions are produced based on motor neuron activity
        
        Key mathematical model: Leaky Integrate-and-Fire dynamics
        τ·dV/dt = -(V - V_rest) + I_syn
        When V ≥ V_thresh: fire spike, reset V = V_reset
        
        Cross-reference: See src/brain/Brain.hpp for C++ implementation
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
        Agent brain interface connecting NLM brain to world environment
        
        AgentBrain serves as the crucial interface layer between the abstract
        neural brain and the concrete world environment. It handles:
        
        ## Sensory Processing Pipeline
        1. **Percept Reception**: Convert world state (vision, touch, audio) into
           standardized sensory percepts
        2. **Signal Transduction**: Transform sensory data into neural currents
           injected into appropriate brain regions
        3. **Neural Integration**: Synchronize brain processing with world updates
        
        ## Motor Decoding Pipeline
        1. **Activity Analysis**: Analyze motor neuron population activity
        2. **Command Selection**: Map neural patterns to discrete action types
        3. **Parameter Extraction**: Extract continuous parameters (position, velocity)
        4. **Action Execution**: Send commands to world simulation
        
        ## Neuromodulation Integration
        - **Dopamine**: Mediates reward prediction error and reinforcement learning
        - **Curiosity**: Drives exploration of novel states and uncertainty
        - **Novelty**: Detects sensory changes and promotes adaptation
        - **Prediction Error**: Monitors prediction accuracy and triggers learning
        
        ## Development Coordination
        Manages brain developmental stages and age-dependent plasticity changes,
        ensuring appropriate learning rates and connectivity patterns throughout
        the agent's lifespan.
        
        Cross-reference: See src/agent/AgentBrain.hpp for C++ implementation details
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

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration for NLM simulation
        
        Returns a Config object with all parameters set to their default values.
        The defaults are optimized for general-purpose simulation and can be
        modified using the Config.set() method.
        
        Default Configuration Overview:
        - Neuron count: 1000
        - Synapse density: 0.1 (10% connectivity)
        - Learning rates: 0.001 for STDP and Hebbian
        - Neuromodulation: Enabled for all systems
        - Development: Full lifecycle enabled
        - Performance: Optimized for stability and learning
        
        Typical Usage:
        ```python
        # Get default configuration
        config = pynlm.createDefaultConfig()
        
        # Modify specific parameters
        config.set("brain.neuron_count", 2000)
        config.set("plasticity.stdp.learning_rate", 0.0005)
        
        # Create brain with modified configuration
        brain = pynlm.createBrain(config)
        ```
        
        Cross-reference: Config class for detailed parameter documentation
    )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a new Brain instance with the given configuration
        
        This is the main entry point for creating neural network simulations.
        The Brain object implements the complete artificial developmental brain
        system with spiking neurons, learning mechanisms, and memory systems.
        
        Parameters:
        - config: Configuration object specifying all brain parameters
                  (use createDefaultConfig() for defaults)
        
        Brain Configuration Categories:
        - **brain.neuron_count**: Total number of neurons (default: 1000)
        - **brain.synapse_density**: Connection probability (default: 0.1)
        - **brain.v_thresh**: Neuron threshold potential (default: -50.0 mV)
        - **brain.v_rest**: Resting potential (default: -70.0 mV)
        - **plasticity.stdp.enable**: Enable STDP learning (default: true)
        - **plasticity.hebbian.enable**: Enable Hebbian learning (default: true)
        - **neuromod.dopamine.scale**: Reward modulation strength (default: 1.0)
        
        After creating, always call brain.initialize() before simulation:
        ```python
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Initialize before use
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
        ```
        
        Cross-reference: Brain class for method documentation
        and Config class for parameter details
    )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a new SimpleWorld simulation environment
        
        Creates a 2D simulated world where NLM agents can interact with objects,
        learn from experiences, and develop capabilities. The world provides
        sensory inputs (vision, touch, proprioception) and motor outputs
        (actions) for agent interaction.
        
        World Features:
        - **Spatial Layout**: 2D grid with configurable dimensions
        - **Sensory Modalities**: Vision, touch, and proprioception sensors
        - **Object System**: Dynamic objects with types and values
        - **Physics**: Collision detection and response
        - **Energy Management**: Resource collection and decay
        - **Time Control**: Discrete simulation steps with timestamps
        
        Typical Configuration:
        ```python
        # Create and configure world
        world = pynlm.createSimpleWorld()
        world.configure(
            width=20,
            height=20,
            visionWidth=8,
            visionHeight=8
        )
        
        # Set agent starting position
        world.setAgentStart(10.0, 10.0)
        
        # Reset to initial state
        world.reset()
        ```
        
        World Dimensions Considerations:
        - Larger worlds = more realistic but computationally expensive
        - Vision resolution affects agent perception capability
        - Width/height should accommodate agent's movement patterns
        
        Cross-reference: SimpleWorld class for detailed method documentation
    )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(
        Create a new AgentBrain interface connecting brain to world
        
        AgentBrain serves as the bridge between the neural brain and the
        external world environment. It handles sensory transduction (converting
        world state to neural input) and motor decoding (converting neural
        activity to environmental actions).
        
        Parameters:
        - brain: The Brain instance to control
        
        AgentBrain Capabilities:
        - **Sensory Processing**: Vision, touch, and proprioception transduction
        - **Motor Decoding**: Mapping neural activity to action commands
        - **Neuromodulation**: Reward-based learning and motivation
        - **Development**: Age-dependent plasticity and learning
        
        Typical Usage:
        ```python
        # Create the complete agent system
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        ```
        
        AgentBrain Workflow:
        1. **Initialization**: Call agent.initialize(world)
        2. **Sensory Processing**: Process percepts each step
        3. **Neural Processing**: Run brain simulation
        4. **Motor Output**: Decode actions from brain activity
        5. **World Interaction**: Apply actions to environment
        6. **Learning**: Apply neuromodulation based on outcomes
        
        Cross-reference: AgentBrain class for detailed method documentation
    )pbdoc");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
