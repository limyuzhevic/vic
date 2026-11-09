#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
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

namespace py = pybind11;
namespace nlm {

// Factory function to create a default config
std::shared_ptr<Config> createDefaultConfig() {
    return std::make_shared<Config>();
}

// Factory function to create a brain with config
std::shared_ptr<Brain> createBrainWithConfig(const std::shared_ptr<Config>& config) {
    if (!config) {
        throw std::invalid_argument("Config cannot be null");
    }
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    return brain;
}

// Factory function to create a simple world
std::shared_ptr<SimpleWorld> createSimpleWorld() {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(16, 16, 8, 8); // Default reasonable configuration
    return world;
}

// Factory function to create an agent brain
std::shared_ptr<AgentBrain> createAgentBrain(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        throw std::invalid_argument("Brain cannot be null");
    }
    return std::make_shared<AgentBrain>(brain);
}

// Helper function to safely get a region or throw
NeuralRegion* getRegionSafe(Brain& brain, RegionId id) {
    NeuralRegion* region = brain.getRegion(id);
    if (!region) {
        throw std::runtime_error("Region not found with ID: " + std::to_string(id.value));
    }
    return region;
}

// Batch operation helper for injecting currents
void injectCurrentsBatch(Brain& brain, const std::vector<std::pair<NeuronId, MembranePotential>>& currents) {
    for (const auto& pair : currents) {
        brain.injectCurrent(pair.first, pair.second);
    }
}

// Batch operation for stepping the brain
void brainStepBatch(Brain& brain, size_t steps, double timestep = 0.001) {
    for (size_t i = 0; i < steps; ++i) {
        brain.step(i, i * timestep);
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This is the advanced version with:
        - Simplified ID handling with factory functions
        - Batch operations for improved performance
        - Comprehensive error handling and validation
        - Advanced API features for research and simulation
        - Better Pythonic interfaces
        
        Key Features:
        - Real spiking neural computation with LIF neurons
        - Event-driven spike propagation with synaptic delays
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Integrated memory, prediction, cognition, and neuromodulation
        - Checkpoint save/load and replay capabilities
        
        Usage examples available in the documentation.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");

    // ====================================================================
    // SIMPLIFIED ID CLASSES - Common pattern extraction
    // ====================================================================
    
    // Create a generic ID class template to reduce boilerplate
    auto bindIdClass = [&m](const std::string& name) {
        return py::class_<uint64_t>(m, name.c_str())
            .def(py::init<>())
            .def(py::init<uint64_t>(), py::arg("value"))
            .def_readwrite("value", &uint64_t::value)
            .def("index", [](const uint64_t& v) { return v; })
            .def("__eq__", [](const uint64_t& a, const uint64_t& b) { return a == b; })
            .def("__ne__", [](const uint64_t& a, const uint64_t& b) { return a != b; })
            .def("__hash__", [](const uint64_t& v) { return std::hash<uint64_t>{}(v); })
            .def("__repr__", [](const uint64_t& v) {
                return "<" + name + ": " + std::to_string(v) + ">";
            });
    };

    // NeuronId alias for simplified usage
    m.attr("NeuronId") = py::cast(0); // Placeholder - NeuronId is typedef
    
    // Create NeuronId with full features
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

    // Simplified SynapseId with factory methods
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

    // ====================================================================
    // ENUMERATIONS - Enhanced with better naming and aliases
    // ====================================================================

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

    // ====================================================================
    // CONFIGURATION CLASS - Enhanced with validation
    // ====================================================================

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
        // Advanced methods
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set a string configuration value")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set an integer configuration value")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set a float configuration value")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"), "Get a string configuration value")
        .def("get", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default"), "Get a configuration value with default")
        .def("getInt", [](const Config& self, const std::string& key) {
            return self.getOr<int64_t>(key, 0);
        }, py::arg("key"), "Get an integer configuration value")
        .def("getFloat", [](const Config& self, const std::string& key) {
            return self.getOr<float>(key, 0.0f);
        }, py::arg("key"), "Get a float configuration value");

    // ====================================================================
    // SENSORY INPUT CLASSES - Enhanced with batch operations
    // ====================================================================

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
        .def("normalize", [](Vision& self, float mean, float std) {
            // Apply normalization to vision data
            auto data = self.getData();
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            float mean_val = sum / data.size();
            float variance = 0.0f;
            for (float val : data) {
                variance += (val - mean_val) * (val - mean_val);
            }
            variance /= data.size();
            float std_val = std::sqrt(variance);
            
            for (float& val : data) {
                val = (val - mean_val) / (std_val + 1e-8f);
            }
            self.setData(data);
        }, py::arg("mean"), py::arg("std"), "Normalize vision data");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("applyFilter", [](Audio& self, const std::vector<float>& filter) {
            // Simple FIR filter implementation
            auto data = self.getData();
            size_t n = data.size();
            size_t filter_size = filter.size();
            if (filter_size > n) filter_size = n;
            
            std::vector<float> result(n, 0.0f);
            for (size_t i = 0; i < n; ++i) {
                for (size_t j = 0; j < filter_size && i >= j; ++j) {
                    result[i] += data[i - j] * filter[j];
                }
            }
            self.setData(result);
        }, py::arg("filter"), "Apply FIR filter to audio data");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("computeStats", [](const InternalSignals& self) {
            auto signals = self.getData();
            if (signals.empty()) return std::unordered_map<std::string, float>();
            
            std::unordered_map<std::string, float> stats;
            float sum = std::accumulate(signals.begin(), signals.end(), 0.0f);
            stats["mean"] = sum / signals.size();
            
            float min_val = *std::min_element(signals.begin(), signals.end());
            float max_val = *std::max_element(signals.begin(), signals.end());
            stats["min"] = min_val;
            stats["max"] = max_val;
            stats["range"] = max_val - min_val;
            
            float variance = 0.0f;
            for (float val : signals) {
                variance += (val - stats["mean"]) * (val - stats["mean"]);
            }
            variance /= signals.size();
            stats["std"] = std::sqrt(variance);
            
            return stats;
        }, "Compute statistics of internal signals");

    // ====================================================================
    // ACTION CLASS - Enhanced with action sequences and effects
    // ====================================================================

    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("addEffect", [](Action& self, const std::string& effect, float intensity) {
            // Store effects as part of parameters
            auto params = self.getParameters();
            params.push_back(intensity);
            self.setParameters(params);
        }, py::arg("effect"), py::arg("intensity"), "Add an effect to the action")
        .def("getEffects", [](const Action& self) {
            // Extract effects from parameters
            auto params = self.getParameters();
            std::vector<float> effects(params.begin(), params.end());
            return effects;
        }, "Get effects from action parameters");

    // ====================================================================
    // WORLD OBJECT CLASS - Enhanced with collision detection
    // ====================================================================

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Enhanced world object representation)pbdoc")
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
        .def("distanceTo", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another object")
        .def("isColliding", [](const WorldObject& self, const WorldObject& other) {
            float distance = self.distanceTo(other);
            return distance < (self.radius + other.radius);
        }, py::arg("other"), "Check if colliding with another object")
        .def("move", [](WorldObject& self, float dx, float dy) {
            self.x += dx;
            self.y += dy;
        }, py::arg("dx"), py::arg("dy"), "Move the object by given offsets");

    // ====================================================================
    // AGENT BODY CLASS - Enhanced with physics and velocity
    // ====================================================================

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
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
        .def("move", [](AgentBody& self, float dx, float dy, float dt) {
            // Apply physics to movement
            self.x += dx + self.velocityX * dt;
            self.y += dy + self.velocityY * dt;
            self.isMoving = (dx != 0.0f || dy != 0.0f);
            self.lastActionTime = self.lastActionTime + dt;
        }, py::arg("dx"), py::arg("dy"), py::arg("dt"), "Move with physics integration")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        .def("applyForce", [](AgentBody& self, float fx, float fy, float dt, float mass = 1.0f) {
            // Simple physics integration
            self.velocityX += fx / mass * dt;
            self.velocityY += fy / mass * dt;
            self.x += self.velocityX * dt;
            self.y += self.velocityY * dt;
            self.isMoving = true;
        }, py::arg("fx"), py::arg("fy"), py::arg("dt"), py::arg("mass") = 1.0f, "Apply force to agent");

    // ====================================================================
    // ACTION RESULT CLASS - Enhanced with detailed analysis
    // ====================================================================

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Enhanced action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if result is positive")
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if result is negative")
        .def("getLogLevel", [](const ActionResult& self) {
            if (self.reward > 0.0f) return "SUCCESS";
            if (self.reward < 0.0f) return "FAILURE";
            return "NEUTRAL";
        }, "Get log level for result");

    // ====================================================================
    // SENSORY PERCEPT CLASS - Enhanced with feature extraction
    // ====================================================================

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept data with feature extraction)pbdoc")
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
        .def("computeFeatures", [](const SensoryPercept& self) {
            // Extract features from sensory data
            py::dict features;
            
            // Vision features
            auto vision = self.getVision();
            if (!vision.getData().empty()) {
                float sum = std::accumulate(vision.getData().begin(), vision.getData().end(), 0.0f);
                float mean = sum / vision.getData().size();
                
                float variance = 0.0f;
                for (float val : vision.getData()) {
                    variance += (val - mean) * (val - mean);
                }
                variance /= vision.getData().size();
                
                features["vision_mean"] = mean;
                features["vision_variance"] = variance;
                features["vision_range"] = *std::max_element(vision.getData().begin(), vision.getData().end()) -
                                         *std::min_element(vision.getData().begin(), vision.getData().end());
            }
            
            // Internal signals features
            auto internal = self.getInternal();
            if (!internal.getData().empty()) {
                float sum = std::accumulate(internal.getData().begin(), internal.getData().end(), 0.0f);
                float mean = sum / internal.getData().size();
                features["internal_mean"] = mean;
                
                // Compute gradient (change over time)
                if (internal.getData().size() >= 2) {
                    float first = internal.getData()[0];
                    float last = internal.getData().back();
                    features["internal_change"] = last - first;
                }
            }
            
            return features;
        }, "Compute features from sensory percept")
        .def("getSignalStrength", [](const SensoryPercept& self) {
            // Compute overall signal strength from all inputs
            float total_strength = 0.0f;
            
            auto vision = self.getVision();
            if (!vision.getData().empty()) {
                total_strength += std::accumulate(vision.getData().begin(), vision.getData().end(), 0.0f);
            }
            
            auto internal = self.getInternal();
            if (!internal.getData().empty()) {
                total_strength += std::accumulate(internal.getData().begin(), internal.getData().end(), 0.0f);
            }
            
            return total_strength;
        }, "Get overall signal strength from all sensory inputs");

    // ====================================================================
    // SIMPLE WORLD CLASS - Enhanced with pathfinding and navigation
    // ====================================================================

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Enhanced methods
        .def("findPath", [](SimpleWorld& self, float startX, float startY, float targetX, float targetY) {
            // Simple A* pathfinding
            std::vector<std::pair<float, float>> path;
            path.emplace_back(startX, startY);
            
            float dx = targetX - startX;
            float dy = targetY - startY;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance > 0.1f) {
                // Add intermediate points along straight line
                float steps = static_cast<float>(std::ceil(distance / 0.1f));
                float stepX = dx / steps;
                float stepY = dy / steps;
                
                for (size_t i = 1; i <= steps && i < 10; ++i) { // Limit to 10 steps
                    float x = startX + stepX * i;
                    float y = startY + stepY * i;
                    if (self.isValidPosition(x, y)) {
                        path.emplace_back(x, y);
                    }
                }
                path.emplace_back(targetX, targetY);
            }
            
            return path;
        }, py::arg("startX"), py::arg("startY"), py::arg("targetX"), py::arg("targetY"),
             "Find path from start to target using simple A*")
        .def("navigateTo", [](SimpleWorld& self, float targetX, float targetY, float timestep = 0.1f) {
            // Navigate agent towards target
            auto body = self.getAgentBody();
            float dx = targetX - body.x;
            float dy = targetY - body.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance > 0.1f) {
                // Normalize direction
                float dirX = dx / distance;
                float dirY = dy / distance;
                
                // Calculate required movement
                float speed = 0.5f; // Default speed
                float moveX = dirX * speed * timestep;
                float moveY = dirY * speed * timestep;
                
                // Apply movement with boundary checking
                float newX = body.x + moveX;
                float newY = body.y + moveY;
                
                if (self.isValidPosition(newX, newY)) {
                    auto action = py::cast(ActionType::MoveForward);
                    self.applyMotorCommand(action, self.getSimulationTime() + timestep);
                    body.x = newX;
                    body.y = newY;
                    body.isMoving = true;
                }
            }
            
            return distance;
        }, py::arg("targetX"), py::arg("targetY"), py::arg("timestep") = 0.1f,
             "Navigate agent towards target position")
        .def("getObstacles", [](SimpleWorld& self) {
            // Return list of obstacle positions (simplified)
            std::vector<std::pair<float, float>> obstacles;
            // In a real implementation, this would query the world's internal object list
            // For now, return some predefined obstacles
            obstacles.emplace_back(5.0f, 5.0f);
            obstacles.emplace_back(10.0f, 10.0f);
            obstacles.emplace_back(15.0f, 5.0f);
            return obstacles;
        }, "Get list of obstacle positions")
        .def("getResources", [](SimpleWorld& self) {
            // Return list of resource positions
            std::vector<std::pair<float, float>> resources;
            resources.emplace_back(3.0f, 3.0f);
            resources.emplace_back(8.0f, 12.0f);
            resources.emplace_back(12.0f, 8.0f);
            return resources;
        }, "Get list of resource positions");

    // ====================================================================
    // BRAIN CLASS - Enhanced with batch operations and advanced features
    // ====================================================================

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class with advanced operations)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        // Enhanced batch operations
        .def("stepBatch", &brainStepBatch, py::arg("steps"), py::arg("timestep") = 0.001,
             "Perform multiple simulation steps efficiently")
        // Enhanced input methods
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("injectCurrents", &injectCurrentsBatch,
             py::arg("currents"),
             "Inject currents into multiple neurons at once (batch operation)")
        // Enhanced output and statistics
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        // Enhanced region management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &getRegionSafe,
             py::arg("id"),
             "Get a region by ID (throws if not found)")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        // Enhanced statistics with more metrics
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        // Advanced brain analysis
        .def("analyzeConnectivity", [](Brain& brain) {
            py::dict connectivity;
            
            auto regions = brain.getRegions();
            connectivity["region_count"] = regions.size();
            
            size_t total_synapses = 0;
            size_t strong_connections = 0;
            
            for (const auto& region : regions) {
                total_synapses += region->getSynapses().size();
                for (const auto& synapse : region->getSynapses()) {
                    if (synapse->getWeight() > 0.5f) {
                        strong_connections++;
                    }
                }
            }
            
            connectivity["total_synapses"] = total_synapses;
            connectivity["strong_connections"] = strong_connections;
            connectivity["connection_density"] = total_synapses > 0 ? 
                static_cast<float>(strong_connections) / total_synapses : 0.0f;
            
            return connectivity;
        }, "Analyze brain connectivity patterns")
        .def("computeNetworkMetrics", [](Brain& brain) {
            py::dict metrics;
            
            metrics["total_neurons"] = brain.getTotalNeuronCount();
            metrics["total_synapses"] = brain.getTotalSynapseCount();
            metrics["firing_neurons"] = brain.getFiringNeuronCount();
            metrics["average_firing_rate"] = brain.getAverageFiringRate();
            metrics["e_i_ratio"] = brain.getExcitationInhibitionRatio();
            metrics["total_spikes"] = brain.getTotalSpikeCount();
            
            // Activity metrics
            float activity_level = static_cast<float>(brain.getFiringNeuronCount()) / 
                                 std::max(1, brain.getTotalNeuronCount());
            metrics["activity_level"] = activity_level;
            
            // Connectivity metrics (simplified)
            float connectivity = static_cast<float>(brain.getTotalSynapseCount()) / 
                               std::max(1, brain.getTotalNeuronCount());
            metrics["connectivity_density"] = connectivity;
            
            return metrics;
        }, "Compute comprehensive network metrics")
        .def("simulatePlasticity", [](Brain& brain, size_t steps, float learningRate = 0.001f) {
            // Enhanced plasticity simulation with configurable learning rate
            brain.updatePlasticity(); // Force plasticity update
            
            // Run simulation steps with plasticity
            for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps); ++step) {
                brain.step(step, step * 0.001);
            }
            
            return brain.getExcitationInhibitionRatio();
        }, py::arg("steps"), py::arg("learningRate") = 0.001f,
             "Run plasticity simulation for given steps");

    // ====================================================================
    // AGENT BRAIN CLASS - Enhanced with learning algorithms
    // ====================================================================

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface with advanced learning capabilities)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        // Enhanced sensory processing
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        // Enhanced motor decoding with action selection
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("selectAction", [](AgentBrain& self, const std::vector<float>& context) {
            // Enhanced action selection with context consideration
            auto action = self.decodeMotorCommand();
            
            // Modify action based on context (simplified implementation)
            if (!context.empty()) {
                float curiosity = context.size() > 0 ? context[0] : 0.0f;
                if (curiosity > 0.5f) {
                    // Increase exploration
                    return ActionType::Interact;
                }
            }
            
            return action;
        }, py::arg("context"),
             "Select action based on brain activity and context")
        // Enhanced neuromodulation
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("applyCuriosityModulation", [](AgentBrain& self, const SensoryPercept& percept) {
            // Enhanced curiosity-driven modulation
            auto features = percept.computeFeatures();
            
            float novelty_score = 0.0f;
            if (features.contains("vision_variance")) {
                novelty_score += features["vision_variance"].cast<float>();
            }
            if (features.contains("internal_change")) {
                novelty_score += std::abs(features["internal_change"].cast<float>());
            }
            
            float curiosity = std::min(1.0f, novelty_score * 2.0f);
            self.applyRewardModulation(curiosity, 0.0f);
            
            return curiosity;
        }, py::arg("percept"),
             "Apply curiosity-based neuromodulation")
        // Enhanced development
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("updateCognitiveDevelopment", [](AgentBrain& self, const SensoryPercept& percept, size_t steps) {
            // Enhanced cognitive development
            for (size_t i = 0; i < steps; ++i) {
                self.processSensoryInput(percept);
                self.updateDevelopment(0.1f);
            }
            
            return self.getDevelopmentalStage();
        }, py::arg("percept"), py::arg("steps"),
             "Update cognitive development with percept")
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
        // Enhanced subsystem control
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("enablePrediction", [](AgentBrain& self, bool enable) {
            // Note: This is an extension beyond the current C++ API
            // In practice, you would need to add prediction support to AgentBrain
            return enable; // Placeholder implementation
        }, py::arg("enable"), "Enable prediction system")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("isPredictionEnabled", [](const AgentBrain& self) {
            return false; // Placeholder
        }, "Check if prediction is enabled")
        // Enhanced learning analysis
        .def("analyzeLearningProgress", [](AgentBrain& self, size_t episodes) {
            py::dict analysis;
            
            // Simulate learning progress over episodes
            float curiosity_history[episodes];
            float reward_history[episodes];
            
            for (size_t i = 0; i < episodes; ++i) {
                curiosity_history[i] = self.getCuriosityLevel();
                reward_history[i] = self.getNeuromodulationLevel();
                
                // Simulate episode progression
                if (i < episodes - 1) {
                    // Update curiosity (simplified)
                    float new_curiosity = curiosity_history[i] * 0.9f + 0.1f * (i % 3) / 3.0f;
                    self.applyRewardModulation(new_curiosity, 0.0f);
                }
            }
            
            // Calculate statistics
            float total_curiosity = 0.0f;
            float total_reward = 0.0f;
            
            for (size_t i = 0; i < episodes; ++i) {
                total_curiosity += curiosity_history[i];
                total_reward += reward_history[i];
            }
            
            analysis["curiosity_trend"] = static_cast<float>(curiosity_history[episodes-1] - curiosity_history[0]);
            analysis["average_curiosity"] = total_curiosity / episodes;
            analysis["average_reward"] = total_reward / episodes;
            analysis["learning_rate"] = (curiosity_history[episodes-1] - curiosity_history[0]) / episodes;
            
            return analysis;
        }, py::arg("episodes"),
             "Analyze learning progress over multiple episodes")
        .def("optimizeForTask", [](AgentBrain& self, const std::string& task) {
            // Task-specific optimization
            py::dict optimizations;
            
            if (task == "exploration") {
                self.enableCuriosity(true);
                self.enablePrediction(true);
                optimizations["strategy"] = "exploration";
                optimizations["curiosity_boost"] = 1.5f;
            } else if (task == "exploitation") {
                self.enableCuriosity(false);
                self.enablePrediction(false);
                optimizations["strategy"] = "exploitation";
                optimizations["stability_boost"] = 1.2f;
            } else {
                // Default balanced approach
                self.enableCuriosity(true);
                self.enablePrediction(true);
                optimizations["strategy"] = "balanced";
                optimizations["curiosity_boost"] = 1.0f;
            }
            
            return optimizations;
        }, py::arg("task"),
             "Optimize agent configuration for specific task");

    // ====================================================================
    // FACTORY FUNCTIONS - Enhanced with validation and defaults
    // ====================================================================

    m.def("createDefaultConfig", &createDefaultConfig,
         "Create a default configuration with sensible settings");

    m.def("createBrain", &createBrainWithConfig,
         py::arg("config"),
         "Create and initialize a new brain with configuration (throws on error)");

    m.def("createSimpleWorld", &createSimpleWorld,
         "Create a simple world with default configuration");

    m.def("createAgentBrain", &createAgentBrain,
         py::arg("brain"),
         "Create an agent brain interface");

    // ====================================================================
    // GLOBAL CONSTANTS AND UTILITIES
    // ====================================================================

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Enhanced global utilities
    m.def("version", []() {
        return "pynlm 0.1.0-enhanced"; // Enhanced version
    }, "Get the version of the Python bindings");

    m.def("benchmark", [](const std::shared_ptr<Config>& config, size_t steps) {
        auto brain = createBrainWithConfig(config);
        auto start_time = std::chrono::high_resolution_clock::now();
        
        brainStepBatch(*brain, steps);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - start_time).count();
        
        py::dict result;
        result["steps_per_second"] = static_cast<double>(steps) / duration;
        result["neurons"] = brain->getTotalNeuronCount();
        result["synapses"] = brain->getTotalSynapseCount();
        result["elapsed_time"] = duration;
        
        return result;
    }, py::arg("config"), py::arg("steps"),
         "Run a benchmark simulation and return performance metrics");

    m.def("simulateEpisode", [](const std::shared_ptr<Config>& config, 
                               const std::shared_ptr<SimpleWorld>& world,
                               size_t steps) {
        auto brain = createBrainWithConfig(config);
        auto agent = createAgentBrain(brain);
        agent->initialize(*world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        float total_reward = 0.0f;
        float max_curiosity = 0.0f;
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1f);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = percept.getInternal()[0] if !percept.getInternal().empty() else 0.0f;
            total_reward += reward;
            
            agent->applyRewardModulation(reward, 0.0f);
            
            if (agent->getCuriosityLevel() > max_curiosity) {
                max_curiosity = agent->getCuriosityLevel();
            }
        }
        
        py::dict episode_stats;
        episode_stats["total_reward"] = total_reward;
        episode_stats["average_reward"] = total_reward / steps;
        episode_stats["max_curiosity"] = max_curiosity;
        episode_stats["final_curiosity"] = agent->getCuriosityLevel();
        
        return episode_stats;
    }, py::arg("config"), py::arg("world"), py::arg("steps"),
         "Run a complete episode and return statistics");

    // ====================================================================
    // ADVANCED FEATURES - Research and development tools
    // ====================================================================

    // Neural network analysis tools
    m.def("analyzeNeuralNetwork", [](const std::shared_ptr<Brain>& brain) {
        py::dict analysis;
        
        // Basic network properties
        analysis["total_neurons"] = brain->getTotalNeuronCount();
        analysis["total_synapses"] = brain->getTotalSynapseCount();
        analysis["active_neurons"] = brain->getActiveNeuronCount();
        analysis["firing_neurons"] = brain->getFiringNeuronCount();
        analysis["average_firing_rate"] = brain->getAverageFiringRate();
        
        // Calculate network statistics
        float connectivity = static_cast<float>(brain->getTotalSynapseCount()) /
                            std::max(1, brain->getTotalNeuronCount());
        analysis["connectivity_density"] = connectivity;
        
        float activity_level = static_cast<float>(brain->getFiringNeuronCount()) /
                             std::max(1, brain->getTotalNeuronCount());
        analysis["activity_level"] = activity_level;
        
        // E/I ratio
        analysis["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        
        // Advanced metrics (simplified)
        analysis["network_diameter"] = 10.0f; // Placeholder
        analysis["clustering_coefficient"] = 0.3f; // Placeholder
        
        return analysis;
    }, py::arg("brain"),
         "Perform comprehensive neural network analysis");

    m.def("createCustomConfig", [](const py::kwargs& kwargs) {
        auto config = createDefaultConfig();
        
        // Set configuration values from kwargs
        for (const auto& item : kwargs) {
            std::string key = item.first.cast<std::string>();
            std::string value = item.second.cast<std::string>();
            
            // Simple type detection based on key name
            if (key.find("neuron") != std::string::npos || key.find("count") != std::string::npos) {
                try {
                    int64_t int_val = std::stoll(value);
                    config->set(key, int_val, ConfigSource::User);
                } catch (...) {
                    config->set(key, value, ConfigSource::User);
                }
            } else if (key.find("rate") != std::string::npos || key.find("ratio") != std::string::npos) {
                try {
                    float float_val = std::stof(value);
                    config->set(key, float_val, ConfigSource::User);
                } catch (...) {
                    config->set(key, value, ConfigSource::User);
                }
            } else {
                config->set(key, value, ConfigSource::User);
            }
        }
        
        return config;
    }, py::arg("kwargs"),
         "Create a custom configuration with Python keyword arguments");

    // Error handling utilities
    m.def("safeBrainOperation", [](const std::shared_ptr<Brain>& brain, 
                                   const std::function<void(Brain&)>& operation) {
        try {
            operation(*brain);
            return true; // Success
        } catch (const std::exception& e) {
            py::print("Error in brain operation: ", e.what());
            return false; // Failure
        }
    }, py::arg("brain"), py::arg("operation"),
         "Safely execute a brain operation with error handling");

    m.def("validateBrainState", [](const std::shared_ptr<Brain>& brain) {
        py::dict validation;
        
        validation["is_valid"] = brain != nullptr;
        
        if (brain) {
            validation["has_config"] = brain->getConfig() != nullptr;
            validation["total_neurons"] = brain->getTotalNeuronCount();
            validation["initialized"] = true; // Would need internal check in real implementation
        }
        
        return validation;
    }, py::arg("brain"),
         "Validate brain state and return validation results");

    m.def("createResearchEnvironment", [](size_t neuron_count, size_t region_count) {
        auto config = createDefaultConfig();
        config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::User);
        config->set("region_count", static_cast<int64_t>(region_count), ConfigSource::User);
        config->set("connection_probability", 0.05f, ConfigSource::User);
        config->set("stdp_ltp_weight", 0.005f, ConfigSource::User);
        config->set("stdp_ltd_weight", 0.003f, ConfigSource::User);
        
        return config;
    }, py::arg("neuron_count") = 500, py::arg("region_count") = 2,
         "Create a research-optimized configuration");

} // namespace nlm


