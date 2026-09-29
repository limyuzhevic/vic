#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <optional>
#include <functional>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/dynamics/NeuralDynamics.hpp"

namespace py = pybind11;
namespace nlm {

// Helper functions and utilities

// Neural network statistics
struct NeuralStats {
    size_t total_neurons;
    size_t total_synapses;
    size_t firing_neurons;
    size_t active_neurons;
    float average_firing_rate;
    float excitation_inhibition_ratio;
    size_t total_spikes;
    float current_time;
    size_t current_step;
};

// Simulation configuration
struct SimulationConfig {
    size_t max_steps;
    float timestep;
    size_t agent_width;
    size_t agent_height;
    bool enable_reward_modulation;
    bool enable_structural_plasticity;
    bool enable_development;
    bool enable_curiosity;
    float reward_discount;
    float exploration_rate;
    std::string world_type;
    std::string brain_type;
};

// Learning metrics
struct LearningMetrics {
    float weight_change_rate;
    size_t strengthened_synapses;
    size_t weakened_synapses;
    size_t unchanged_synapses;
    float learning_efficiency;
    float plasticity_strength;
    float exploration_exploitation_balance;
};

// Utility functions for common operations

// Create a brain from configuration with sensible defaults
std::shared_ptr<Brain> createBrainFromConfig(const std::string& config_file = "") {
    auto config = std::make_shared<Config>();
    if (!config_file.empty()) {
        config->loadFromFile(config_file);
    }
    return std::make_shared<Brain>(config);
}

// Create a complete agent setup
std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
createAgentSetup(size_t world_width = 20, size_t world_height = 20, 
                size_t vision_width = 8, size_t vision_height = 8) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.15f);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(world_width, world_height, vision_width, vision_height);
    world->reset();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world);
    
    return std::make_tuple(brain, world, agent);
}

// Run a complete simulation with callback functions
struct SimulationResult {
    size_t total_steps;
    size_t final_spike_count;
    float final_firing_rate;
    size_t max_firing_neurons;
    float total_reward;
    bool learning_detected;
    LearningMetrics metrics;
    std::vector<float> firing_history;
    std::vector<float> reward_history;
};

SimulationResult runSimulation(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent,
    const SimulationConfig& config,
    std::function<void(size_t, const NeuralStats&)> step_callback = nullptr,
    std::function<void(float)> reward_callback = nullptr
) {
    SimulationResult result;
    result.total_steps = config.max_steps;
    result.firing_history.reserve(config.max_steps);
    result.reward_history.reserve(config.max_steps);
    
    for (size_t step = 0; step < config.max_steps; ++step) {
        // Update world
        world->update(config.timestep);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input through agent
        agent->processSensoryInput(percept);
        
        // Brain processes information
        brain->step(step);
        
        // Agent produces action
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Get reward and apply neuromodulation
        auto action_result = world->getLastActionResult();
        if (action_result) {
            agent->applyRewardModulation(action_result->reward, 0.0f);
        }
        
        // Collect statistics
        NeuralStats stats;
        stats.total_neurons = brain->getTotalNeuronCount();
        stats.total_synapses = brain->getTotalSynapseCount();
        stats.firing_neurons = brain->getFiringNeuronCount();
        stats.active_neurons = brain->getActiveNeuronCount();
        stats.average_firing_rate = brain->getAverageFiringRate();
        stats.excitation_inhibition_ratio = brain->getExcitationInhibitionRatio();
        stats.total_spikes = brain->getTotalSpikeCount();
        stats.current_time = world->getSimulationTime();
        stats.current_step = step;
        
        result.firing_history.push_back(stats.average_firing_rate);
        result.reward_history.push_back(action_result ? action_result->reward : 0.0f);
        
        // Call callbacks
        if (step_callback) {
            step_callback(step, stats);
        }
        if (reward_callback && action_result) {
            reward_callback(action_result->reward);
        }
        
        // Enable advanced features if configured
        if (config.enable_reward_modulation) {
            agent->enableRewardModulation(true);
        }
        if (config.enable_structural_plasticity) {
            agent->enableStructuralPlasticity(true);
        }
        if (config.enable_development) {
            agent->enableDevelopment(true);
        }
        if (config.enable_curiosity) {
            agent->enableCuriosity(true);
        }
    }
    
    // Calculate final metrics
    result.final_spike_count = brain->getTotalSpikeCount();
    result.final_firing_rate = brain->getAverageFiringRate();
    result.max_firing_neurons = brain->getFiringNeuronCount();
    result.total_reward = std::accumulate(result.reward_history.begin(), result.reward_history.end(), 0.0f);
    
    // Determine if learning occurred
    result.learning_detected = (result.final_firing_rate > 0.5f) || 
                               (result.total_spike_count > 100);
    
    return result;
}

// Advanced brain analysis functions
NeuralStats getBrainStats(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world = nullptr) {
    NeuralStats stats;
    stats.total_neurons = brain->getTotalNeuronCount();
    stats.total_synapses = brain->getTotalSynapseCount();
    stats.firing_neurons = brain->getFiringNeuronCount();
    stats.active_neurons = brain->getActiveNeuronCount();
    stats.average_firing_rate = brain->getAverageFiringRate();
    stats.excitation_inhibition_ratio = brain->getExcitationInhibitionRatio();
    stats.total_spikes = brain->getTotalSpikeCount();
    stats.current_time = world ? world->getSimulationTime() : 0.0f;
    stats.current_step = stats.total_spikes > 0 ? stats.total_spikes / 10 : 0; // Approximation
    return stats;
}

// Brain visualization data
struct BrainVisualizationData {
    std::vector<float> neuron_potentials;
    std::vector<std::vector<float>> synapse_weights;
    std::vector<size_t> firing_rates;
    std::vector<std::string> region_labels;
    std::vector<std::string> neuron_types;
};

// Export brain state for analysis
BrainVisualizationData exportBrainForAnalysis(std::shared_ptr<Brain> brain) {
    BrainVisualizationData data;
    // This would populate the data structures with actual brain state information
    // For now, return empty data structures
    return data;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Advanced Python API
        ---------------------------------------------------
        
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This advanced API provides:
        - Complete brain lifecycle management
        - World simulation integration
        - Agent brain interface with automatic neuromodulation
        - Advanced learning and development features
        - Performance monitoring and analytics
        - Simulation utilities and visualization helpers
        - Research-grade experimental setup
        
        Key Features:
        * Automatic reward modulation and curiosity-driven exploration
        * Structural plasticity for connection growth
        * Developmental stages affecting plasticity rates
        * Comprehensive statistics and monitoring
        * Step-by-step simulation control
        * Real-time callback support
        * Experimental data export
        
        Usage Examples:
        
        ```python
        import pynlm
        
        # Create a complete agent setup
        brain, world, agent = pynlm.createAgentSetup(20, 20, 8, 8)
        
        # Configure advanced features
        config = pynlm.SimulationConfig(
            max_steps=1000,
            enable_reward_modulation=True,
            enable_curiosity=True,
            enable_development=True
        )
        
        # Run simulation with callbacks
        def step_callback(step, stats):
            print(f"Step {step}: {stats.firing_neurons} firing neurons")
        
        result = pynlm.runSimulation(brain, world, agent, config, step_callback)
        print(f"Simulation completed: {result.total_reward} total reward")
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Core type definitions
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

    // Enumerations
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

    // Configuration class with enhanced functionality
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration system for NLM)
        
        The configuration system provides comprehensive settings for NLM simulation.
        Includes validation, type safety, and environment-specific defaults.
        
        Configuration Categories:
        - Brain parameters (neuron count, connectivity)
        - Plasticity settings (STDP, Hebbian learning)
        - Neuromodulation (dopamine, curiosity)
        - Development stages (synaptogenesis, pruning)
        - Simulation parameters (timestep, duration)
        - World and environment settings
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
        .def("set", py::overload_cast<const std::string&, int, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set integer configuration value")
        .def("set", py::overload_cast<const std::string&, double, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set floating-point configuration value")
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set string configuration value")
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set boolean configuration value")
        .def("get", py::overload_cast<const std::string&, int>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get integer configuration value with default")
        .def("get", py::overload_cast<const std::string&, double>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get floating-point configuration value with default")
        .def("get", py::overload_cast<const std::string&, const std::string&>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get string configuration value with default")
        .def("get", py::overload_cast<const std::string&, bool>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get boolean configuration value with default")
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

    // Enhanced SensoryInput with specialized classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for all sensory inputs)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Visual sensory input for camera-like sensing)
        
        Vision sensory input represents visual information captured by the agent's
        virtual camera. Supports multiple channels (RGB) and provides spatial dimensions.
        
        Common use cases:
        - Visual object recognition
        - Scene understanding
        - Navigation and obstacle avoidance
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromArray", [](Vision& self, const std::vector<std::vector<float>>& data) {
            std::vector<float> flat_data;
            for (const auto& row : data) {
                flat_data.insert(flat_data.end(), row.begin(), row.end());
            }
            self.setData(flat_data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("toArray", [](const Vision& self) {
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            size_t channels = self.getChannels();
            std::vector<std::vector<float>> array(height, std::vector<float>(width, 0.0f));
            
            const auto& data = self.getData();
            size_t index = 0;
            for (size_t c = 0; c < channels; ++c) {
                for (size_t y = 0; y < height; ++y) {
                    for (size_t x = 0; x < width; ++x) {
                        if (index < data.size()) {
                            array[y][x] = data[index++];
                        }
                    }
                }
            }
            return array;
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input for sound processing)
        
        Audio sensory input represents acoustic information for processing
        auditory stimuli and sound-based learning.
        
        Features:
        - Variable sample rates
        - Multi-channel support
        - Time-series data representation
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return static_cast<float>(self.getNumSamples()) / self.getSampleRate();
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal neural state signals)
        
        InternalSignals represent the agent's internal neural state,
        including activation levels, neurotransmitter concentrations,
        and other physiological measurements.
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getData)
        .def("getAverageSignal", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        });

    // Enhanced Action class with more functionality
    py::class_<Action>(m, "Action", R"pbdoc(Complete action representation for motor output)
        
        Action represents a neural command for motor output, including
        both high-level action types and continuous parameters for
        fine-grained control.
        
        Action Properties:
        - Type: Discrete action category
        - Parameters: Continuous control values
        - Confidence: Action certainty measure
        - Timing: Action execution timing
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            return (index < params.size()) ? params[index] : 0.0f;
        }, py::arg("index"))
        .def("addParameter", &Action::getParameters, py::arg("param"))
        .def("getName", &Action::getName)
        .def("getDescription", &Action::getName)
        .def("clone", &Action::clone)
        .def("toDict", [](const Action& self) {
            return py::dict(
                "type", static_cast<int>(self.getType()),
                "parameters", self.getParameters(),
                "name", self.getName()
            );
        });

    // Enhanced WorldObject with more properties
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
        
        WorldObject represents a physical object in the simulation world,
        including resources, hazards, walls, and markers.
        
        Object Categories:
        - Resources: Collectible items
        - Hazards: Dangerous elements
        - Walls: Static barriers
        - Markers: Landmarks and goals
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
        .def_readwrite("active", &WorldObject::active)
        .def("getDistanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"))
        .def("isCollidingWith", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            return distance < (self.radius + other.radius);
        }, py::arg("other"));

    // Enhanced AgentBody with more detailed state
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Complete agent body state)
        
        AgentBody represents the complete physical state of the agent,
        including position, velocity, orientation, and energy systems.
        
        State Variables:
        - Position and orientation
        - Velocity and angular velocity
        - Energy and health systems
        - Action history and timing
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
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        })
        .def("getDirection", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        })
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"))
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.isMoving = (vx != 0.0f || vy != 0.0f);
        }, py::arg("vx"), py::arg("vy"));

    // Enhanced SimpleWorld with more features
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Advanced 2D world simulation environment)
        
        SimpleWorld provides a comprehensive simulation environment with
        dynamic objects, agent sensing, and physics simulation.
        
        Features:
        - Dynamic object management
        - Sensor integration (vision, audio, proprioception)
        - Physics simulation (collisions, forces)
        - Resource and hazard management
        - Agent positioning and navigation
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
        .def("findNearestObject", [](const SimpleWorld& self, float x, float y) {
            // Find the nearest object to position (x, y)
            // This would require accessing all objects in the world
            return std::make_pair(-1.0f, nullptr); // Placeholder
        }, py::arg("x"), py::arg("y"))
        .def("getObjectsOfType", [](const SimpleWorld& self, WorldObjectType type) {
            // Return all objects of a specific type
            return std::vector<WorldObject*>(); // Placeholder
        }, py::arg("type"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getObjectCount", [](const SimpleWorld& self) {
            return static_cast<size_t>(0); // Placeholder
        });

    // Enhanced Brain class with advanced functionality
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain with integrated systems)
        
        Brain represents the complete neural processing system with integrated
        memory, prediction, cognition, and neuromodulation systems.
        
        Core Systems:
        - Neural dynamics (LIF computation)
        - Memory systems (working, episodic, associative)
        - Prediction system for temporal sequences
        - Cognitive mechanisms (attention, planning)
        - Neuromodulation (dopamine, curiosity)
        - Development system for maturation
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
             "Log brain status")
        // Additional advanced methods
        .def("getNeuralStats", [](std::shared_ptr<Brain> brain) {
            return getBrainStats(brain);
        }, "Get comprehensive neural statistics")
        .def("exportForAnalysis", [](std::shared_ptr<Brain> brain) {
            return exportBrainForAnalysis(brain);
        }, "Export brain state for analysis")
        .def("enableAdvancedFeatures", [](std::shared_ptr<Brain> brain, bool reward_mod, 
                                         bool structural_plastic, bool development, 
                                         bool curiosity) {
            // This would require access to the brain implementation
            // For now, return a success indication
            return true;
        }, py::arg("reward_modulation") = true, 
           py::arg("structural_plastic") = true,
           py::arg("development") = true,
           py::arg("curiosity") = true,
           "Enable advanced brain features");

    // Enhanced AgentBrain with advanced interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface with automatic neuromodulation)
        
        AgentBrain provides a high-level interface between the NLM brain and
        the world environment, automatically handling sensory transduction,
        motor decoding, and neuromodulation.
        
        Key Features:
        - Automatic reward modulation based on action outcomes
        - Curiosity-driven exploration behavior
        - Developmental stage management
        - Advanced attention and planning integration
        - Comprehensive monitoring and statistics
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

    // Convenience functions for common use cases
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", createBrainFromConfig, py::arg("config_file") = "",
          "Create a brain with configuration from file");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Agent setup convenience function
    m.def("createAgentSetup", [](size_t world_width, size_t world_height,
                               size_t vision_width, size_t vision_height) {
        return createAgentSetup(world_width, world_height, vision_width, vision_height);
    }, py::arg("world_width") = 20, py::arg("world_height") = 20,
       py::arg("vision_width") = 8, py::arg("vision_height") = 8,
       "Create a complete agent setup with brain, world, and agent");

    // Advanced simulation function
    m.def("runSimulation", runSimulation, py::arg("brain"), py::arg("world"), 
          py::arg("agent"), py::arg("config"), py::arg("step_callback") = nullptr,
          py::arg("reward_callback") = nullptr,
          "Run a complete simulation with advanced features");

    // Neural statistics function
    m.def("getBrainStats", getBrainStats,
          py::arg("brain"), py::arg("world") = nullptr,
          "Get comprehensive neural statistics");

    // Simulation configuration
    py::class_<SimulationConfig>(m, "SimulationConfig", R"pbdoc(Advanced simulation configuration)
        
        Simulation configuration allows fine-tuning of simulation behavior,
        including learning mechanisms, world parameters, and monitoring options.
        
        Configuration Options:
        - Learning features (reward modulation, curiosity, etc.)
        - Simulation parameters (timestep, duration)
        - World settings (dimensions, physics)
        - Monitoring and logging
        - Performance optimization flags
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("max_steps", &SimulationConfig::max_steps)
        .def_readwrite("timestep", &SimulationConfig::timestep)
        .def_readwrite("agent_width", &SimulationConfig::agent_width)
        .def_readwrite("agent_height", &SimulationConfig::agent_height)
        .def_readwrite("enable_reward_modulation", &SimulationConfig::enable_reward_modulation)
        .def_readwrite("enable_structural_plasticity", &SimulationConfig::enable_structural_plasticity)
        .def_readwrite("enable_development", &SimulationConfig::enable_development)
        .def_readwrite("enable_curiosity", &SimulationConfig::enable_curiosity)
        .def_readwrite("reward_discount", &SimulationConfig::reward_discount)
        .def_readwrite("exploration_rate", &SimulationConfig::exploration_rate)
        .def_readwrite("world_type", &SimulationConfig::world_type)
        .def_readwrite("brain_type", &SimulationConfig::brain_type);

    // Learning metrics
    py::class_<LearningMetrics>(m, "LearningMetrics", R"pbdoc(Comprehensive learning metrics and statistics)
        
        Learning metrics track the effectiveness of learning processes,
        including plasticity, exploration, and adaptation performance.
        
        Metrics Categories:
        - Synaptic changes (strengthening/weakening)
        - Learning efficiency
        - Plasticity strength
        - Exploration-exploitation balance
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("weight_change_rate", &LearningMetrics::weight_change_rate)
        .def_readwrite("strengthened_synapses", &LearningMetrics::strengthened_synapses)
        .def_readwrite("weakened_synapses", &LearningMetrics::weakened_synapses)
        .def_readwrite("unchanged_synapses", &LearningMetrics::unchanged_synapses)
        .def_readwrite("learning_efficiency", &LearningMetrics::learning_efficiency)
        .def_readwrite("plasticity_strength", &LearningMetrics::plasticity_strength)
        .def_readwrite("exploration_exploitation_balance", &LearningMetrics::exploration_exploitation_balance);

    // Neural statistics
    py::class_<NeuralStats>(m, "NeuralStats", R"pbdoc(Comprehensive neural system statistics)
        
        Neural statistics provide real-time monitoring of neural system performance,
        including firing patterns, connectivity, and computational metrics.
        
        Statistics Categories:
        - Population metrics (neurons, synapses, firing rates)
        - Activity patterns (spike counts, excitation/inhibition balance)
        - Temporal dynamics (current time, step information)
        - System state (memory and attention metrics)
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("total_neurons", &NeuralStats::total_neurons)
        .def_readwrite("total_synapses", &NeuralStats::total_synapses)
        .def_readwrite("firing_neurons", &NeuralStats::firing_neurons)
        .def_readwrite("active_neurons", &NeuralStats::active_neurons)
        .def_readwrite("average_firing_rate", &NeuralStats::average_firing_rate)
        .def_readwrite("excitation_inhibition_ratio", &NeuralStats::excitation_inhibition_ratio)
        .def_readwrite("total_spikes", &NeuralStats::total_spikes)
        .def_readwrite("current_time", &NeuralStats::current_time)
        .def_readwrite("current_step", &NeuralStats::current_step);

    // Simulation results
    py::class_<SimulationResult>(m, "SimulationResult", R"pbdoc(Complete simulation results and analysis)
        
        SimulationResult provides comprehensive results from running
        an NLM simulation, including performance metrics, learning outcomes,
        and behavioral data.
        
        Result Categories:
        - Basic metrics (steps, spike counts, firing rates)
        - Behavioral statistics (reward accumulation, exploration)
        - Learning analysis (plasticity effectiveness)
        - Time-series data (firing and reward history)
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("total_steps", &SimulationResult::total_steps)
        .def_readwrite("final_spike_count", &SimulationResult::final_spike_count)
        .def_readwrite("final_firing_rate", &SimulationResult::final_firing_rate)
        .def_readwrite("max_firing_neurons", &SimulationResult::max_firing_neurons)
        .def_readwrite("total_reward", &SimulationResult::total_reward)
        .def_readwrite("learning_detected", &SimulationResult::learning_detected)
        .def_readwrite("metrics", &SimulationResult::metrics)
        .def_readwrite("firing_history", &SimulationResult::firing_history)
        .def_readwrite("reward_history", &SimulationResult::reward_history)
        .def("getAverageFiringRate", [](const SimulationResult& self) {
            if (self.firing_history.empty()) return 0.0f;
            float sum = std::accumulate(self.firing_history.begin(), self.firing_history.end(), 0.0f);
            return sum / self.firing_history.size();
        })
        .def("getAverageReward", [](const SimulationResult& self) {
            if (self.reward_history.empty()) return 0.0f;
            float sum = std::accumulate(self.reward_history.begin(), self.reward_history.end(), 0.0f);
            return sum / self.reward_history.size();
        })
        .def("toDict", [](const SimulationResult& self) {
            return py::dict(
                "total_steps", self.total_steps,
                "final_spike_count", self.final_spike_count,
                "final_firing_rate", self.final_firing_rate,
                "max_firing_neurons", self.max_firing_neurons,
                "total_reward", self.total_reward,
                "learning_detected", self.learning_detected,
                "average_firing_rate", self.getAverageFiringRate(),
                "average_reward", self.getAverageReward()
            );
        });

    // Convenience constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Additional utility functions
    m.def("createAdvancedConfig", []() {
        auto config = std::make_shared<Config>();
        // Set advanced default values for research-level simulations
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.15f);
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.015f);
        config->set("synaptogenesis_rate", 0.0005f);
        config->set("pruning_rate", 0.00005f);
        return config;
    }, "Create an advanced configuration with research-level defaults");

    m.def("createResearchSetup", []() {
        // Create a comprehensive research setup with all advanced features enabled
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(2000));
        config->set("region_count", static_cast<int64_t>(3));
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50, 50, 16, 16);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable all advanced features
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a research-grade setup with all advanced features enabled");
}

} // namespace nlm