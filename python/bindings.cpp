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
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId: Strong type for neuron identification with proper memory management
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId: Strong type for synapse identification with proper memory management
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) {
            return std::hash<uint64_t>{}(id.value);
        })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId: Strong type for brain region identification with proper memory management
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) {
            return std::hash<uint64_t>{}(id.value);
        })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId: Strong type for neuron population identification with proper memory management
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            // Validate input path
            if (filepath.empty()) {
                throw std::invalid_argument("File path cannot be empty");
            }
            if (filepath.length() > 4096) {
                throw std::invalid_argument("File path too long (max 4096 characters)");
            }
            // Check for null bytes
            if (filepath.find('\0') != std::string::npos) {
                throw std::invalid_argument("File path contains null bytes");
            }
            return self.loadFromFile(filepath);
        }, py::arg("filepath"),
             "Load configuration from a JSON file with input validation")
        .def("loadFromArgs", [](Config& self, int argc, const std::vector<std::string>& argv) {
            // Validate command line arguments
            if (argc < 0) {
                throw std::invalid_argument("argc cannot be negative");
            }
            if (argv.size() > static_cast<size_t>(argc)) {
                throw std::invalid_argument("argv size cannot exceed argc");
            }
            return self.loadFromArgs(argc, const_cast<char**>(&argv[0]));
        }, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments with input validation")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            // Validate output path
            if (filepath.empty()) {
                throw std::invalid_argument("File path cannot be empty");
            }
            if (filepath.length() > 4096) {
                throw std::invalid_argument("File path too long (max 4096 characters)");
            }
            if (filepath.find('\0') != std::string::npos) {
                throw std::invalid_argument("File path contains null bytes");
            }
            return self.saveToFile(filepath);
        }, py::arg("filepath"),
             "Save configuration to a JSON file with input validation")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists (case-sensitive)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            // Validate key and value
            if (key.empty()) {
                throw std::invalid_argument("Key cannot be empty");
            }
            if (key.length() > 256) {
                throw std::invalid_argument("Key too long (max 256 characters)");
            }
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration key with validation")
        .def("get", [](const Config& self, const std::string& key) {
            // Validate key
            if (key.empty()) {
                throw std::invalid_argument("Key cannot be empty");
            }
            try {
                return self.get(key);
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Failed to get key \") + key + "\": " + e.what());
            }
        }, py::arg("key"),
             "Get configuration value (returns empty string if key not found)")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput: Base class for all sensory input with improved error handling
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def(py::init<>())
        .def("getType", [](const SensoryInput& self) {
            try {
                return std::string(self.getType());
            } catch (const std::exception& e) {
                throw std::runtime_error("Failed to get sensory input type: " + std::string(e.what()));
            }
        }, "Get the type of sensory input")
        .def("getData", [](const SensoryInput& self) {
            try {
                return self.getData();
            } catch (const std::exception& e) {
                throw std::runtime_error("Failed to get sensory input data: " + std::string(e.what()));
            }
        }, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("clone", [](const SensoryInput& self) {
            try {
                return self.clone();
            } catch (const std::exception& e) {
                throw std::runtime_error("Failed to clone sensory input: " + std::string(e.what()));
            }
        }, "Clone this sensory input object")
        .def("__repr__", [](const SensoryInput& input) {
            try {
                return "<SensoryInput type='" + std::string(input.getType()) + 
                       "' dimensions=" + std::to_string(input.getDimensions()) + 
                       " timestamp=" + std::to_string(input.getTimestamp()) + ">";
            } catch (...) {
                return "<SensoryInput invalid>";
            }
        });

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create Vision with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            // Validate input data
            if (data.size() > 1000000) { // Limit to 1MB of data
                throw std::invalid_argument("Data size too large (max 1MB)");
            }
            self.setData(data);
        }, py::arg("data"),
             "Set image data with validation")
        .def("setData", [](Vision& self, float* data, size_t size) {
            // Validate input data
            if (!data && size > 0) {
                throw std::invalid_argument("Null data pointer with non-zero size");
            }
            if (size > 1000000) { // Limit to 1MB of data
                throw std::invalid_argument("Data size too large (max 1MB)");
            }
            self.setData(data, size);
        }, py::arg("data"), py::arg("size"),
             "Set image data from raw pointer with validation")
        .def("getWidth", &Vision::getWidth,
             "Get image width")
        .def("getHeight", &Vision::getHeight,
             "Get image height")
        .def("getChannels", &Vision::getChannels,
             "Get number of color channels")
        .def("__repr__", [](const Vision& vision) {
            try {
                return "<Vision width=" + std::to_string(vision.getWidth()) +
                       " height=" + std::to_string(vision.getHeight()) +
                       " channels=" + std::to_string(vision.getChannels()) + ">";
            } catch (...) {
                return "<Vision invalid>";
            }
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create Audio with specified sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            // Validate input data
            if (data.size() > 10000000) { // Limit to 10MB of data (audio)
                throw std::invalid_argument("Audio data size too large (max 10MB)");
            }
            self.setData(data);
        }, py::arg("data"),
             "Set audio data with validation")
        .def("setSampleRate", [](Audio& self, size_t sampleRate) {
            // Validate sample rate
            if (sampleRate == 0 || sampleRate > 192000) {
                throw std::invalid_argument("Invalid sample rate (must be 1-192kHz)");
            }
            self.setSampleRate(sampleRate);
        }, py::arg("sampleRate"),
             "Set sample rate with validation")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get number of audio samples")
        .def("__repr__", [](const Audio& audio) {
            try {
                return "<Audio sampleRate=" + std::to_string(audio.getSampleRate()) +
                       " numSamples=" + std::to_string(audio.getNumSamples()) + ">";
            } catch (...) {
                return "<Audio invalid>";
            }
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", [](InternalSignals& self, float value) {
            // Validate signal value (should be reasonable for internal signals)
            if (std::isnan(value) || std::isinf(value)) {
                throw std::invalid_argument("Signal value cannot be NaN or infinity");
            }
            self.addSignal(value);
        }, py::arg("value"),
             "Add internal signal with validation")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("getSignalCount", &InternalSignals::getDimensions,
             "Get number of signals")
        .def("getAllSignals", [](const InternalSignals& self) {
            try {
                return self.getData();
            } catch (const std::exception& e) {
                throw std::runtime_error("Failed to get internal signals: " + std::string(e.what()));
            }
        }, "Get all internal signals as vector")
        .def("__repr__", [](const InternalSignals& signals) {
            try {
                return "<InternalSignals count=" + std::to_string(signals.getDimensions()) + ">";
            } catch (...) {
                return "<InternalSignals invalid>";
            }
        });

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

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
        .def_readwrite("active", &WorldObject::active);

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
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, size_t width, size_t height, 
                         size_t visionWidth, size_t visionHeight) {
            // Validate configuration parameters
            if (width == 0 || width > 10000) {
                throw std::invalid_argument("World width must be between 1 and 10000");
            }
            if (height == 0 || height > 10000) {
                throw std::invalid_argument("World height must be between 1 and 10000");
            }
            if (visionWidth == 0 || visionWidth > width) {
                throw std::invalid_argument("Vision width must be between 1 and world width");
            }
            if (visionHeight == 0 || visionHeight > height) {
                throw std::invalid_argument("Vision height must be between 1 and world height");
            }
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world with validated parameters")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            // Validate agent start position
            if (std::isnan(x) || std::isinf(x)) {
                throw std::invalid_argument("Agent start X must be a finite number");
            }
            if (std::isnan(y) || std::isinf(y)) {
                throw std::invalid_argument("Agent start Y must be a finite number");
            }
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"),
             "Set agent start position with validation")
        .def("update", [](SimpleWorld& self, float timestep) {
            // Validate timestep
            if (timestep <= 0.0f) {
                throw std::invalid_argument("Timestep must be positive");
            }
            if (timestep > 1.0f) {
                throw std::invalid_argument("Timestep too large (max 1.0 seconds)");
            }
            self.update(timestep);
        }, py::arg("timestep"),
             "Update world with validated timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
              py::arg("cmd"), py::arg("currentTime"),
              "Apply motor command (cmd and currentTime validated internally)")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
              py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
              py::return_value_policy::reference_internal)
        .def("addObject", [](SimpleWorld& self, const WorldObject& obj) {
            // Validate world object
            if (std::isnan(obj.x) || std::isinf(obj.x)) {
                throw std::invalid_argument("Object X coordinate must be finite");
            }
            if (std::isnan(obj.y) || std::isinf(obj.y)) {
                throw std::invalid_argument("Object Y coordinate must be finite");
            }
            if (obj.radius <= 0.0f) {
                throw std::invalid_argument("Object radius must be positive");
            }
            if (obj.radius > 100.0f) {
                throw std::invalid_argument("Object radius too large (max 100.0)");
            }
            self.addObject(obj);
        }, py::arg("obj"),
             "Add world object with validation")
        .def("removeObject", [](SimpleWorld& self, float x, float y) {
            // Validate coordinates
            if (std::isnan(x) || std::isinf(x)) {
                throw std::invalid_argument("X coordinate must be finite");
            }
            if (std::isnan(y) || std::isinf(y)) {
                throw std::invalid_argument("Y coordinate must be finite");
            }
            self.removeObject(x, y);
        }, py::arg("x"), py::arg("y"),
             "Remove object at position with validation")
        .def("isValidPosition", [](SimpleWorld& self, float x, float y) {
            // Validate coordinates
            if (std::isnan(x) || std::isinf(x)) {
                throw std::invalid_argument("X coordinate must be finite");
            }
            if (std::isnan(y) || std::isinf(y)) {
                throw std::invalid_argument("Y coordinate must be finite");
            }
            return self.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"),
             "Check if position is valid with validation")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", [](SimpleWorld& self, float e) {
            // Validate energy value
            if (e < 0.0f) {
                throw std::invalid_argument("Max energy cannot be negative");
            }
            if (e > 10000.0f) {
                throw std::invalid_argument("Max energy too large (max 10000.0)");
            }
            self.setMaxEnergy(e);
        }, py::arg("e"),
             "Set maximum energy with validation")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", [](SimpleWorld& self, float r) {
            // Validate decay rate
            if (r < 0.0f) {
                throw std::invalid_argument("Energy decay rate cannot be negative");
            }
            if (r > 1.0f) {
                throw std::invalid_argument("Energy decay rate too large (max 1.0)");
            }
            self.setEnergyDecayRate(r);
        }, py::arg("r"),
             "Set energy decay rate with validation")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", [](SimpleWorld& self, uint64_t seed) {
            // Validate seed
            if (seed == 0) {
                throw std::invalid_argument("Random seed cannot be zero");
            }
            self.setRandomSeed(seed);
        }, py::arg("seed"),
             "Set random seed with validation")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step (without timestamp)")
        .def("step_with_time", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
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

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("getBrain", [](const AgentBrain& self) {
            // Return a shared_ptr to the brain for proper memory management
            return self.brain_; // Access internal shared_ptr
        }, py::return_value_policy::copy,
             "Get a shared_ptr to the underlying brain for proper memory management")
        .def("getBrainRaw", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get raw brain pointer (use with caution - caller manages lifetime)")
        .def("getBrainShared", [](const AgentBrain& self) {
            // Alternative way to get shared_ptr
            return self.brain_;
        }, py::return_value_policy::copy,
             "Get brain as shared_ptr using alternative method")
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
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience methods for common patterns

    // Complete agent simulation with full integration
    m.def("runAgentSimulation", [](std::shared_ptr<Brain> brain, 
                                   std::shared_ptr<SimpleWorld> world,
                                   int numSteps, bool enableLearning = true) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
        }
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1f);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() 
                          << " firing neurons, " << agent->getCuriosityLevel() 
                          << " curiosity, " << agent->getNoveltyLevel() << " novelty" << std::endl;
            }
        }
        
        return std::make_pair(world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("numSteps"), py::arg("enableLearning"),
         "Run a complete agent simulation with brain, world, and agent integration");

    // Quick setup for simple experiments
    m.def("setupSimpleExperiment", [](size_t neuronCount = 1000, size_t worldSize = 20) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int>(neuronCount));
        config->set("brain.synapse_density", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldSize, worldSize, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("neuronCount") = 1000, py::arg("worldSize") = 20,
         "Set up a simple experiment with brain, world, and agent");

    // Statistical analysis helpers
    m.def("analyzeBrainStats", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, double> stats;
        stats["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        stats["active_neurons"] = static_cast<double>(brain->getActiveNeuronCount());
        stats["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        stats["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        stats["e_to_i_ratio"] = brain->getExcitationInhibitionRatio();
        return stats;
    }, py::arg("brain"),
         "Analyze brain statistics and return as dictionary");

    // Sensor data aggregation helper
    m.def("aggregateSensoryData", [](const std::shared_ptr<SimpleWorld>& world, 
                                   int numSteps = 10) {
        std::vector<std::vector<float>> visionData;
        std::vector<float> internalData;
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1f);
            
            auto percept = world->getSensoryPercept();
            
            // Vision
            auto vision = percept.getVision();
            std::vector<float> visionVector;
            visionVector.reserve(vision.getWidth() * vision.getHeight() * vision.getChannels());
            // Note: In practice, you'd extract the actual data here
            
            // Internal signals
            auto internal = percept.getInternal();
            internalData.insert(internalData.end(), internal.begin(), internal.end());
            
            if (step % 2 == 0) {
                visionData.push_back(visionVector);
            }
        }
        
        return std::make_tuple(visionData, internalData);
    }, py::arg("world"), py::arg("numSteps"),
         "Aggregate sensory data over multiple time steps");

    // Configuration presets
    m.def("createConfigForDevelopment", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.15f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("development.enable", true);
        return config;
    }, "Create configuration optimized for developmental learning");

    m.def("createConfigForAdult", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 5000);
        config->set("brain.synapse_density", 0.2f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 2.0f);
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", true);
        config->set("development.enable", false);
        return config;
    }, "Create configuration optimized for adult brain");

    // Utility functions
    m.def("printBrainInfo", [](const std::shared_ptr<Brain>& brain) {
        std::cout << "Brain Information:" << std::endl;
        std::cout << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "  E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    }, py::arg("brain"),
         "Print detailed brain information to console");

    m.def("isReadyForSimulation", [](const std::shared_ptr<Brain>& brain, 
                                    const std::shared_ptr<SimpleWorld>& world) {
        if (!brain) {
            std::cerr << "Warning: Brain is null" << std::endl;
            return false;
        }
        if (!world) {
            std::cerr << "Warning: World is null" << std::endl;
            return false;
        }
        return true;
    }, py::arg("brain"), py::arg("world"),
         "Check if brain and world are ready for simulation");

    // Backward compatibility wrapper
    m.def("getBrainFromAgent", [](const std::shared_ptr<AgentBrain>& agent) {
        // This is a convenience wrapper for getBrain() to maintain API consistency
        return agent->getBrain();
    }, py::arg("agent"),
         "Get brain from agent (backward compatibility)");

} // namespace nlm
