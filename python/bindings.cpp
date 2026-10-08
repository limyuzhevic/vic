#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <optional>

// Include all NLM headers
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

// Forward declarations for type hints
class Config;
class Brain;
class AgentBrain;
class SimpleWorld;
class SensoryInput;
class Vision;
class Audio;
class InternalSignals;
class Action;
class WorldObject;
class AgentBody;
class ActionResult;
class SensoryPercept;

// =============================================================================
// Type Aliases and Convenience Definitions
// =============================================================================

using SimulationStep = nlm::SimulationStep;
using Timestamp = nlm::Timestamp;
using SynapticWeight = nlm::SynapticWeight;
using NeuronId = nlm::NeuronId;
using SynapseId = nlm::SynapseId;
using RegionId = nlm::RegionId;
using PopulationId = nlm::PopulationId;

PYBIND11_MAKE_OPAQUE(std::vector<float>);
PYBIND11_MAKE_OPAQUE(std::vector<WorldObject>);
PYBIND11_MAKE_OPAQUE(std::vector<NeuronId>);
PYBIND11_MAKE_OPAQUE(std::vector<SynapseId>);
PYBIND11_MAKE_OPAQUE(std::vector<RegionId>);
PYBIND11_MAKE_OPAQUE(std::vector<Action>);
PYBIND11_MAKE_OPAQUE(std::vector<std::string>);

// =============================================================================
// Helper Functions and Utilities
// =============================================================================

namespace py_utils {
    // Convert simulation step to time for convenience
    Timestamp stepToTime(SimulationStep step, double timestep = 0.1) {
        return static_cast<Timestamp>(step * timestep);
    }

    // Format brain statistics for easier consumption
    std::string formatBrainStats(const Brain& brain) {
        std::stringstream ss;
        ss << "Brain Statistics:";
        ss << "\n  Total Neurons: " << brain.getTotalNeuronCount();
        ss << "\n  Total Synapses: " << brain.getTotalSynapseCount();
        ss << "\n  Firing Neurons: " << brain.getFiringNeuronCount();
        ss << "\n  Average Firing Rate: " << brain.getAverageFiringRate();
        ss << "\n  Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage());
        ss << "\n  Total Spikes: " << brain.getTotalSpikeCount();
        ss << "\n  E/I Ratio: " << brain.getExcitationInhibitionRatio();
        return ss.str();
    }

    // Validate configuration parameters
    void validateConfig(const Config& config) {
        // Basic validation - users can override for specific needs
        if (config.has("brain.neuron_count") && config.get<int64_t>("brain.neuron_count") <= 0) {
            throw std::invalid_argument("Neuron count must be positive");
        }
        if (config.has("brain.region_count") && config.get<int64_t>("brain.region_count") <= 0) {
            throw std::invalid_argument("Region count must be positive");
        }
    }

    // Safe float comparison for serialization
    bool floatEquals(float a, float b, float epsilon = 1e-5) {
        return std::abs(a - b) < epsilon;
    }
}

// =============================================================================
// Enhanced Pythonic Data Structures
// =============================================================================

// Dataclass for representing a simple brain snapshot
struct BrainSnapshot {
    SimulationStep step;
    float avg_firing_rate;
    int firing_neuron_count;
    int total_spike_count;
    float excitation_inhibition_ratio;
    std::string developmental_stage_string;

    std::string to_string() const {
        std::stringstream ss;
        ss << "BrainSnapshot(step=" << step << ", ";
        ss << "avg_firing_rate=" << avg_firing_rate << ", ";
        ss << "firing_neuron_count=" << firing_neuron_count << ", ";
        ss << "total_spike_count=" << total_spike_count << ", ";
        ss << "e_i_ratio=" << excitation_inhibition_ratio << ", ";
        ss << "developmental_stage=" << developmental_stage_string << ")";
        return ss.str();
    }
};

// Context manager for controlled brain operations
class BrainContext {
public:
    BrainContext(std::shared_ptr<Brain> brain, double timestep = 0.1)
        : brain_(brain), timestep_(timestep), active_(false) {}

    void __enter__() {
        active_ = true;
        return;
    }

    BrainSnapshot __exit__(py::object exc_type, py::object exc_val, py::object exc_tb) {
        active_ = false;
        if (exc_type) {
            // An exception occurred, return error info
            BrainSnapshot snapshot = getSnapshot();
            return snapshot;
        }
        return getSnapshot();
    }

    BrainSnapshot getSnapshot() const {
        BrainSnapshot snapshot;
        snapshot.step = current_step_;
        snapshot.avg_firing_rate = brain_->getAverageFiringRate();
        snapshot.firing_neuron_count = brain_->getFiringNeuronCount();
        snapshot.total_spike_count = brain_->getTotalSpikeCount();
        snapshot.excitation_inhibition_ratio = brain_->getExcitationInhibitionRatio();
        snapshot.developmental_stage_string = developmentalStageToString(brain_->getDevelopmentalStage());
        return snapshot;
    }

    void step(SimulationStep step) {
        if (!active_) {
            throw std::runtime_error("BrainContext must be used with 'with' statement");
        }
        current_step_ = step;
        brain_->step(step);
    }

    void processSensoryInput(const SensoryPercept& percept) {
        if (!active_) {
            throw std::runtime_error("BrainContext must be used with 'with' statement");
        }
        brain_->receiveSensoryInput(percept);
    }

    Action produceAction() {
        if (!active_) {
            throw std::runtime_error("BrainContext must be used with 'with' statement");
        }
        return brain_->produceAction();
    }

    SimulationStep getCurrentStep() const { return current_step_; }
    bool isActive() const { return active_; }

private:
    std::shared_ptr<Brain> brain_;
    double timestep_;
    bool active_;
    SimulationStep current_step_ = 0;

    static std::string developmentalStageToString(nlm::DevelopmentalStage stage) {
        switch (stage) {
            case nlm::DevelopmentalStage::Initial: return "Initial";
            case nlm::DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case nlm::DevelopmentalStage::Maturation: return "Maturation";
            case nlm::DevelopmentalStage::Adult: return "Adult";
            case nlm::DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
};

// =============================================================================
// Enhanced Configuration Class
// =============================================================================

void configureBrain(Config& config, int neuronCount, int regionCount, 
                    double connectionProbability = 0.1,
                    double stdWeightMean = 0.5, double stdWeightStd = 0.1,
                    double vThreshold = -50.0, double vRest = -70.0,
                    double vReset = -75.0, double tauMem = 20.0,
                    double tauRef = 2.0) {
    config.set("brain.neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Runtime);
    config.set("brain.region_count", static_cast<int64_t>(regionCount), nlm::ConfigSource::Runtime);
    config.set("brain.connection_probability", connectionProbability, nlm::ConfigSource::Runtime);
    config.set("brain.initial_weight_mean", stdWeightMean, nlm::ConfigSource::Runtime);
    config.set("brain.initial_weight_std", stdWeightStd, nlm::ConfigSource::Runtime);
    config.set("brain.v_thresh", vThreshold, nlm::ConfigSource::Runtime);
    config.set("brain.v_rest", vRest, nlm::ConfigSource::Runtime);
    config.set("brain.v_reset", vReset, nlm::ConfigSource::Runtime);
    config.set("brain.tau_mem", tauMem, nlm::ConfigSource::Runtime);
    config.set("brain.tau_ref", tauRef, nlm::ConfigSource::Runtime);
    
    // Validate configuration
    py_utils::validateConfig(config);
}

// =============================================================================
// Enhanced Sensory Input Classes
// =============================================================================

// Vision helper methods
Vision createVision(size_t width, size_t height, size_t channels = 3) {
    return Vision(width, height, channels);
}

Vision createVisionFromData(size_t width, size_t height, const std::vector<float>& data) {
    Vision vision(width, height);
    vision.setData(data);
    return vision;
}

// Audio helper methods  
Audio createAudio(size_t sampleRate, size_t numSamples) {
    return Audio(sampleRate, numSamples);
}

Audio createAudioFromData(size_t sampleRate, const std::vector<float>& data) {
    Audio audio(sampleRate, data.size());
    audio.setData(data);
    audio.setSampleRate(sampleRate);
    return audio;
}

// =============================================================================
// Enhanced World and Agent Helper Functions
// =============================================================================

class WorldSimulator {
public:
    WorldSimulator(std::shared_ptr<SimpleWorld> world) : world_(world) {}

    void runEpisode(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                   SimulationStep numSteps, double timestep = 0.1) {
        world_->reset();
        agent->initialize(*world_);
        brain->initialize();
        
        for (SimulationStep step = 0; step < numSteps; ++step) {
            world_->update(timestep);
            
            auto percept = world_->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world_->applyMotorCommand(action, world_->getSimulationTime());
        }
    }

    WorldObject addResource(float x, float y, float value = 1.0f) {
        return world_->addObject(WorldObject(x, y, WorldObjectType::Resource, value));
    }

    WorldObject addHazard(float x, float y, float radius = 0.5f) {
        return world_->addObject(WorldObject(x, y, WorldObjectType::Hazard, 0.0f, radius));
    }

    WorldObject addWall(float x, float y, float radius = 0.5f) {
        return world_->addObject(WorldObject(x, y, WorldObjectType::Wall, 0.0f, radius));
    }

    std::vector<WorldObject> getAllObjects() const {
        // Note: This is a simplified version - real implementation would need additional methods
        return std::vector<WorldObject>();
    }

private:
    std::shared_ptr<SimpleWorld> world_;
};

// =============================================================================
// Advanced Factory Functions
// =============================================================================

std::shared_ptr<Brain> createBrainWithConfig(const Config& baseConfig) {
    auto config = std::make_shared<Config>(baseConfig);
    py_utils::validateConfig(*config);
    return std::make_shared<Brain>(config);
}

std::shared_ptr<Brain> createPreconfiguredBrain(int complexity = 1) {
    auto config = std::make_shared<Config>();
    
    switch (complexity) {
        case 1: // Simple
            configureBrain(*config, 100, 2, 0.05, 0.5, 0.1, -50.0, -70.0, -75.0, 20.0, 2.0);
            break;
        case 2: // Medium
            configureBrain(*config, 500, 5, 0.1, 0.5, 0.15, -55.0, -70.0, -75.0, 25.0, 3.0);
            break;
        case 3: // Complex
            configureBrain(*config, 1000, 10, 0.15, 0.5, 0.2, -60.0, -70.0, -75.0, 30.0, 4.0);
            break;
        default: // Default
            configureBrain(*config);
    }
    
    return std::make_shared<Brain>(config);
}

std::shared_ptr<AgentBrain> createLearningAgent(std::shared_ptr<Brain> brain, 
                                               bool enableRewardModulation = true,
                                               bool enableStructuralPlasticity = true,
                                               bool enableDevelopment = true,
                                               bool enableCuriosity = true) {
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->enableRewardModulation(enableRewardModulation);
    agent->enableStructuralPlasticity(enableStructuralPlasticity);
    agent->enableDevelopment(enableDevelopment);
    agent->enableCuriosity(enableCuriosity);
    return agent;
}

// =============================================================================
// Main Python Module Definition
// =============================================================================

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides access to the complete NLM brain simulation system,
        including neural networks, memory, learning, and environmental interaction.
        
        Key Features:
        - Complete brain simulation with spiking neurons
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and cognitive systems
        - Developmental stages and plasticity
        - World simulation and interaction
        - Checkpoint/restore capabilities
        
        The API follows Python best practices with:
        - Type hints and comprehensive documentation
        - Context managers for controlled operations
        - Factory functions for common use cases
        - Property-based access to data
        - Pythonic error handling
    )pbdoc";

    // Register exception translations
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "KeyError");
    py::register_exception<std::logic_error>(m, "RuntimeError");

    // =============================================================================
    // Core Type Definitions
    // =============================================================================
    
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, R"pbdoc(Get the 1-based index of this neuron ID)pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
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

    // =============================================================================
    // Enumeration Types
    // =============================================================================
    
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

    // =============================================================================
    // Core Configuration Class
    // =============================================================================
    
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        })
        // Dictionary-like interface
        .def("__getitem__", [](Config& self, const std::string& key) {
            auto value = self.get(key);
            if (!value.has_value()) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return value;
        })
        .def("__setitem__", [](Config& self, const std::string& key, const std::any& value) {
            // This is a simplified version - real implementation would need more sophisticated handling
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int64_t>) {
                    self.set(key, arg, nlm::ConfigSource::Runtime);
                } else if constexpr (std::is_same_v<T, double>) {
                    self.set(key, arg, nlm::ConfigSource::Runtime);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    self.set(key, arg, nlm::ConfigSource::Runtime);
                } else if constexpr (std::is_same_v<T, bool>) {
                    self.set(key, arg, nlm::ConfigSource::Runtime);
                } else {
                    throw std::runtime_error("Unsupported type for configuration");
                }
            }, value);
        })
        .def("__contains__", &Config::has)
        .def("getOr", [](Config& self, const std::string& key, const std::any& defaultValue) {
            auto value = self.get(key);
            if (value.has_value()) {
                return value.value();
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value"));

    // =============================================================================
    // Enhanced Sensory Input Classes
    // =============================================================================
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("to_dict", [](const Vision& self) {
            return py::dict("width"_a = self.getWidth(),
                           "height"_a = self.getHeight(),
                           "channels"_a = self.getChannels());
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount);

    // =============================================================================
    // Enhanced Action and World Object Classes
    // =============================================================================
    
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& self) {
            return py::dict("type"_a = self.getType(),
                           "parameters"_a = self.getParameters());
        });

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
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
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Calculate distance to given position)pbdoc")
        .def("isInteractive", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource || self.type == WorldObjectType::Hazard;
        }, R"pbdoc(Check if this object can be interacted with)pbdoc")
        .def("to_dict", [](const WorldObject& self) {
            return py::dict("x"_a = self.x,
                           "y"_a = self.y,
                           "type"_a = static_cast<int>(self.type),
                           "value"_a = self.value,
                           "radius"_a = self.radius,
                           "active"_a = self.active);
        });

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
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
        .def("reset", &AgentBody::reset)
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Calculate distance to given position)pbdoc")
        .def("speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, R"pbdoc(Calculate current speed)pbdoc")
        .def("to_dict", [](const AgentBody& self) {
            return py::dict("x"_a = self.x,
                           "y"_a = self.y,
                           "orientation"_a = self.orientation,
                           "velocityX"_a = self.velocityX,
                           "velocityY"_a = self.velocityY,
                           "energy"_a = self.energy,
                           "health"_a = self.health,
                           "age"_a = self.age);
        });

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("to_dict", [](const ActionResult& self) {
            return py::dict("reward"_a = self.reward,
                           "success"_a = self.success,
                           "message"_a = self.message);
        });

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("merge", [](SensoryPercept& self, const SensoryPercept& other) {
            if (other.getVision().getWidth() > 0) self.setVision(other.getVision());
            if (other.getAudio().getSampleRate() > 0) self.setAudio(other.getAudio());
            auto touch = other.getTouch();
            if (!touch.empty()) self.setTouch(touch);
            auto internal = other.getInternal();
            if (!internal.empty()) self.setInternal(internal);
            auto proprio = other.getProprioception();
            if (!proprio.empty()) self.setProprioception(proprio);
        }, "Merge another percept into this one")
        .def("to_dict", [](const SensoryPercept& self) {
            return py::dict(
                "vision"_a = py::dict("width"_a = self.getVisionWidth(),
                                     "height"_a = self.getVisionHeight()),
                "audio"_a = py::dict("sampleRate"_a = self.getAudio().getSampleRate()),
                "touch"_a = self.getTouch(),
                "internal"_a = self.getInternal(),
                "proprioception"_a = self.getProprioception(),
                "timestamp"_a = self.getTimestamp()
            );
        });

    // =============================================================================
    // Enhanced SimpleWorld Class
    // =============================================================================
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getSimulationSpeed", &SimpleWorld::getSimulationSpeed,
             "Get simulation speed")
        .def("setSimulationSpeed", &SimpleWorld::setSimulationSpeed, py::arg("speed"),
             "Set simulation speed")
        .def("to_dict", [](const SimpleWorld& self) {
            return py::dict(
                "width"_a = self.getWidth(),
                "height"_a = self.getHeight(),
                "visionWidth"_a = self.getVisionWidth(),
                "visionHeight"_a = self.getVisionHeight(),
                "simulationTime"_a = self.getSimulationTime(),
                "maxEnergy"_a = self.getMaxEnergy()
            );
        });

    // =============================================================================
    // Enhanced Brain Class
    // =============================================================================
    
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration")
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
        .def("formatStats", py_utils::formatBrainStats,
             "Get formatted brain statistics string")
        .def("validate", py_utils::validateConfig, py::arg("config"),
             "Validate configuration parameters");

    // =============================================================================
    // Enhanced AgentBrain Class
    // =============================================================================
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize agent brain interface")
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
             py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("getLearningStatus", [](const AgentBrain& self) {
            return py::dict(
                "reward_modulation"_a = self.isRewardModulationEnabled(),
                "structural_plasticity"_a = self.isStructuralPlasticityEnabled(),
                "development"_a = self.isDevelopmentEnabled(),
                "curiosity"_a = self.isCuriosityEnabled(),
                "neuromodulation_level"_a = self.getNeuromodulationLevel(),
                "curiosity_level"_a = self.getCuriosityLevel(),
                "novelty_level"_a = self.getNoveltyLevel(),
                "prediction_error"_a = self.getPredictionError()
            );
        }, "Get comprehensive learning status dictionary")
        .def("getLearningSummary", [](const AgentBrain& self) {
            std::stringstream ss;
            ss << "Learning Status:";
            ss << "\n  Reward Modulation: " << (self.isRewardModulationEnabled() ? "Enabled" : "Disabled");
            ss << "\n  Structural Plasticity: " << (self.isStructuralPlasticityEnabled() ? "Enabled" : "Disabled");
            ss << "\n  Development: " << (self.isDevelopmentEnabled() ? "Enabled" : "Disabled");
            ss << "\n  Curiosity: " << (self.isCuriosityEnabled() ? "Enabled" : "Disabled");
            ss << "\n  Neuromodulation Level: " << self.getNeuromodulationLevel();
            return ss.str();
        }, "Get human-readable learning status summary");

    // =============================================================================
    // Context Manager Support
    // =============================================================================
    
    py::class_<BrainContext>(m, "BrainContext", R"pbdoc(Context manager for controlled brain operations)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, double>(),
             py::arg("brain"), py::arg("timestep") = 0.1,
             "Create a brain context manager")
        .def("__enter__", &BrainContext::__enter__,
             "Enter the context manager")
        .def("__exit__", [](BrainContext& self, py::object exc_type, py::object exc_val, py::object exc_tb) {
            return self.__exit__(exc_type, exc_val, exc_tb);
        })
        .def("step", &BrainContext::step, py::arg("step"),
             "Execute a simulation step")
        .def("processSensoryInput", &BrainContext::processSensoryInput, py::arg("percept"),
             "Process sensory input in the context")
        .def("produceAction", &BrainContext::produceAction,
             "Produce action in the context")
        .def("getSnapshot", &BrainContext::getSnapshot,
             "Get current brain snapshot")
        .def("getCurrentStep", &BrainContext::getCurrentStep,
             "Get current simulation step")
        .def("isActive", &BrainContext::isActive,
             "Check if context is active");

    // =============================================================================
    // Convenience Factory Functions
    // =============================================================================
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        py_utils::validateConfig(*config);
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")

    m.def("createBrain", [](int neuronCount, int regionCount) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        configureBrain(*config, neuronCount, regionCount);
        return std::make_shared<Brain>(config);
    }, py::arg("neuron_count"), py::arg("region_count"),
       "Create a brain with specified neuron and region counts")

    m.def("createBrain", [](const std::string& complexity) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        int complexity_level = 1;
        if (complexity == "simple") complexity_level = 1;
        else if (complexity == "medium") complexity_level = 2;
        else if (complexity == "complex") complexity_level = 3;
        return createPreconfiguredBrain(complexity_level);
    }, py::arg("complexity"),
       "Create a preconfigured brain based on complexity level (simple/medium/complex)")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")

    // =============================================================================
    // Helper and Utility Functions
    // =============================================================================
    
    m.def("configureBrain", &configureBrain,
          py::arg("config"), py::arg("neuronCount"), py::arg("regionCount"),
          py::arg("connectionProbability") = 0.1,
          py::arg("stdWeightMean") = 0.5, py::arg("stdWeightStd") = 0.1,
          py::arg("vThreshold") = -50.0, py::arg("vRest") = -70.0,
          py::arg("vReset") = -75.0, py::arg("tauMem") = 20.0,
          py::arg("tauRef") = 2.0,
          "Helper function to configure a brain with common parameters")

    m.def("createVision", py_utils::createVision,
          py::arg("width"), py::arg("height"), py::arg("channels") = 3,
          "Create a vision sensory input")

    m.def("createAudio", py_utils::createAudio,
          py::arg("sampleRate"), py::arg("numSamples"),
          "Create an audio sensory input")

    m.def("createLearningAgent", &createLearningAgent,
          py::arg("brain"), py::arg("enableRewardModulation") = true,
          py::arg("enableStructuralPlasticity") = true,
          py::arg("enableDevelopment") = true,
          py::arg("enableCuriosity") = true,
          "Create an agent brain with learning subsystems enabled")

    // =============================================================================
    // Constants and Module Info
    // =============================================================================
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    m.attr("__version__") = "0.1.0";

    // Module documentation
    m.def("get_version", []() { return "0.1.0"; }, "Get the NLM Python bindings version");

    m.def("print_info", []() {
        std::cout << "NLM Python Bindings v0.1.0\n";
        std::cout << "For documentation, visit: https://nlm.readthedocs.io\n";
        std::cout << "Examples available in easy_usage.md\n";
    }, "Print NLM module information");

    // =============================================================================
    // Advanced Usage Patterns
    // =============================================================================

    // Create a world simulator helper
    py::class_<WorldSimulator>(m, "WorldSimulator", R"pbdoc(Helper class for world simulation)pbdoc")
        .def(py::init<std::shared_ptr<SimpleWorld>>(), py::arg("world"),
             "Initialize world simulator")
        .def("runEpisode", &WorldSimulator::runEpisode,
             py::arg("brain"), py::arg("agent"), py::arg("numSteps"), py::arg("timestep") = 0.1,
             "Run a complete episode with brain and agent")
        .def("addResource", &WorldSimulator::addResource,
             py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
             "Add a resource object to the world")
        .def("addHazard", &WorldSimulator::addHazard,
             py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f,
             "Add a hazard object to the world")
        .def("addWall", &WorldSimulator::addWall,
             py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f,
             "Add a wall object to the world");

    // Example usage function
    m.def("example_usage", []() {
        py::print("Example usage patterns:\n");
        py::print("1. Simple brain simulation:\n");
        py::print("   import pynlm\n");
        py::print("   config = pynlm.createDefaultConfig()\n");
        py::print("   brain = pynlm.createBrain(config)\n");
        py::print("   brain.initialize()\n");
        py::print("   for i in range(100): brain.step(i)\n\n");
        py::print("2. Agent in world simulation:\n");
        py::print("   import pynlm\n");
        py::print("   brain = pynlm.createBrain(1000, 5)\n");
        py::print("   world = pynlm.createSimpleWorld()\n");
        py::print("   agent = pynlm.createAgentBrain(brain)\n");
        py::print("   world.configure(20, 20, 8, 8)\n");
        py::print("   world.reset()\n");
        py::print("   agent.initialize(world)\n\n");
        py::print("3. Advanced learning agent:\n");
        py::print("   brain = pynlm.createBrain('complex')\n");
        py::print("   agent = pynlm.createLearningAgent(brain)\n");
        py::print("   with brain.create_context() as ctx:\n");
        py::print("       ctx.processSensoryInput(percept)\n");
        py::print("       ctx.step(step)\n");
        py::print("       action = ctx.produceAction()\n");
    }, "Show example usage patterns");
}
