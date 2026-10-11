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

namespace py = pybind11;
namespace nlm {

// Custom Python exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("ConfigurationError: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("BrainError: " + msg) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& msg) : NLMError("WorldError: " + msg) {}
};

class SensoryInputError : public NLMError {
public:
    explicit SensoryInputError(const std::string& msg) : NLMError("SensoryInputError: " + msg) {}
};

class ActionError : public NLMError {
public:
    explicit ActionError(const std::string& msg) : NLMError("ActionError: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");
    py::register_exception<SensoryInputError>(m, "SensoryInputError");
    py::register_exception<ActionError>(m, "ActionError");

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
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
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
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return "PopulationId(" + std::to_string(id.value) + ")";
        });

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def(py::init<const char*>());

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def(py::init<const char*>());

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def(py::init<const char*>());

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def(py::init<const char*>());

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
        .export_values()
        .def(py::init<const char*>());

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .def(py::init<const char*>());

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def(py::init<const char*>());

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<Config>>(), py::arg("other"),
             R"pbdoc(Create a config from another config object)pbdoc")
        .def("__enter__", [](Config& self) { return &self; },
             R"pbdoc(Enter context manager. Returns self for chaining)pbdoc")
        .def("__exit__", [](Config&, const py::args&) {
             R"pbdoc(Exit context manager. Cleanup is handled automatically)pbdoc")
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
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"), "Get a string value")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<int>(key);
        }, py::arg("key"), "Get an integer value")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<double>(key);
        }, py::arg("key"), "Get a float/double value")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<bool>(key);
        }, py::arg("key"), "Get a boolean value")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<std::vector<int>>(key);
        }, py::arg("key"), "Get an integer vector")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<std::vector<double>>(key);
        }, py::arg("key"), "Get a float vector")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<std::vector<std::string>>(key);
        }, py::arg("key"), "Get a string vector")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get a string value with default")
        .def("getOr", [](const Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get an integer value with default")
        .def("getOr", [](const Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get a float value with default")
        .def("getOr", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get a boolean value with default")
        .def("set", [](Config& self, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a string configuration value")
        .def("set", [](Config& self, const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set an integer configuration value")
        .def("set", [](Config& self, const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a float configuration value")
        .def("set", [](Config& self, const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a boolean configuration value")
        .def("set", [](Config& self, const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a string vector configuration value")
        .def("set", [](Config& self, const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set an integer vector configuration value")
        .def("set", [](Config& self, const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a float vector configuration value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        .def_property("source", &Config::getKeys, nullptr,
                     R"pbdoc(Get all configuration keys)pbdoc")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys (Pythonic alias)")
        .def("set_value", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Convenience method to set string value")
        .def("get_value", [](const Config& self, const std::string& key, const std::string& defaultValue = "") {
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "",
           "Get string value with default (convenience method)")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("to_dict", [](const Config& self) {
            std::vector<std::string> keys = self.getKeys();
            py::dict result;
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (value) {
                    result[key.c_str()] = value.value();
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        .def("from_dict", [](Config& self, const py::dict& data) {
            for (auto item : data) {
                std::string key = item.first.cast<std::string>();
                if (item.second.get_type() == py::dtype::of<std::string>()) {
                    self.set(key, item.second.cast<std::string>());
                } else if (item.second.get_type() == py::dtype::of<int>()) {
                    self.set(key, item.second.cast<int>());
                } else if (item.second.get_type() == py::dtype::of<double>()) {
                    self.set(key, item.second.cast<double>());
                } else if (item.second.get_type() == py::dtype::of<bool>()) {
                    self.set(key, item.second.cast<bool>());
                }
            }
        }, py::arg("data"), "Load configuration from Python dictionary");

    // Add operator overloading for copy construction/assignment
    py::implicitly_convertible<Config, std::shared_ptr<Config>>();

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def(py::init<>())
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("clone", &SensoryInput::clone, "Create a copy of this sensory input")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=\"" + std::string(input.getType()) + "\">";
        })
        .def("__str__", [](const SensoryInput& input) {
            return std::string(input.getType());
        })
        .def("set_data", [](SensoryInput& self, const std::vector<float>& data) {
            // Note: This is a hack to allow setting data, as base class doesn't have setData
            // In practice, subclasses should use their specific setData methods
            throw SensoryInputError("Cannot set data on abstract base class. Use subclass instead.");
        }, py::arg("data"), "Set data (subclasses only)")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector (Pythonic alias)")
        .def("copy", [](const SensoryInput& self) -> std::unique_ptr<SensoryInput> {
            return self.clone();
        }, "Create a copy of this sensory input (Pythonic alias)")
        .def("ensure_type", [](const SensoryInput& self, const std::string& expectedType) {
            if (std::string(self.getType()) != expectedType) {
                throw SensoryInputError("Expected sensory input type \"" + expectedType + "\", got \"" + std::string(self.getType()) + "\"");
            }
        }, py::arg("expectedType"), "Ensure sensory input has expected type");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create a Vision input with given dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set image data")
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set image data (Pythonic alias)")
        .def("getWidth", &Vision::getWidth)
        .def("get_height", &Vision::getWidth, "Get image width (Pythonic alias)")
        .def("getHeight", &Vision::getHeight)
        .def("get_height", &Vision::getHeight, "Get image height (Pythonic alias)")
        .def("getChannels", &Vision::getChannels)
        .def("get_channels", &Vision::getChannels, "Get number of channels (Pythonic alias)")
        .def("get_data", &Vision::getData, "Get the raw data as a vector")
        .def("get_shape", [](const Vision& self) {
            py::tuple shape = py::make_tuple(self.getWidth(), self.getHeight(), self.getChannels());
            return shape;
        }, "Get image shape as a tuple (width, height, channels)")
        .def("reshape", [](Vision& self, size_t newWidth, size_t newHeight, size_t newChannels) {
            // This would require implementing reshape in Vision class, for now it's a placeholder
            throw VisionError("Reshape not implemented in Vision class.");
        }, py::arg("newWidth"), py::arg("newHeight"), py::arg("newChannels"),
           "Reshape the image data (if supported by implementation)")
        .def("__repr__", [](const Vision& vision) {
            return "<Vision width=" + std::to_string(vision.getWidth()) +
                   " height=" + std::to_string(vision.getHeight()) +
                   " channels=" + std::to_string(vision.getChannels()) + ">";
        })
        .def("__str__", [](const Vision& vision) {
            return "Vision(" + std::to_string(vision.getWidth()) + "x" + std::to_string(vision.getHeight()) +
                   "x" + std::to_string(vision.getChannels()) + ")";
        })
        .def("is_grayscale", [](const Vision& self) {
            return self.getChannels() == 1;
        }, "Check if image is grayscale")
        .def("is_rgb", [](const Vision& self) {
            return self.getChannels() == 3;
        }, "Check if image is RGB")
        .def("is_rgba", [](const Vision& self) {
            return self.getChannels() == 4;
        }, "Check if image is RGBA")
        .def("get_pixel_count", [](const Vision& self) {
            return self.getWidth() * self.getHeight();
        }, "Get total number of pixels")
        .def("get_data_as_array", [](const Vision& self) {
            return self.getData();
        }, "Get data as Python list (convenience method)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create an Audio input with given sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data (Pythonic alias)")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("get_sample_rate", &Audio::getSampleRate, "Get sample rate (Pythonic alias)")
        .def("getNumSamples", &Audio::getNumSamples)
        .def("get_num_samples", &Audio::getNumSamples, "Get number of samples (Pythonic alias)")
        .def("get_data", &Audio::getData, "Get the raw data as a vector")
        .def("get_duration", [](const Audio& self) {
            return static_cast<float>(self.getNumSamples()) / static_cast<float>(self.getSampleRate());
        }, "Get duration in seconds")
        .def("get_duration_seconds", [](const Audio& self) {
            return static_cast<float>(self.getNumSamples()) / static_cast<float>(self.getSampleRate());
        }, "Get duration in seconds (Pythonic alias)")
        .def("get_frequency_range", [](const Audio& self) {
            // This would require implementing frequency analysis, for now it's a placeholder
            return py::make_tuple(0.0f, static_cast<float>(self.getSampleRate()) / 2.0f);
        }, "Get frequency range (placeholder)")
        .def("__repr__", [](const Audio& audio) {
            return "<Audio sampleRate=" + std::to_string(audio.getSampleRate()) +
                   " numSamples=" + std::to_string(audio.getNumSamples()) + ">";
        })
        .def("__str__", [](const Audio& audio) {
            return "Audio(sampleRate=" + std::to_string(audio.getSampleRate()) +
                   ", samples=" + std::to_string(audio.getNumSamples()) + ")";
        })
        .def("is_monophonic", [](const Audio& self) {
            return true;  // Assuming mono for now
        }, "Check if audio is monophonic")
        .def("is_stereo", [](const Audio& self) {
            return false;  // Assuming mono for now
        }, "Check if audio is stereo")
        .def("get_data_as_array", [](const Audio& self) {
            return self.getData();
        }, "Get data as Python list (convenience method)");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value (Pythonic alias)")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals")
        .def("clear_signals", &InternalSignals::clearSignals,
             "Clear all signals (Pythonic alias)")
        .def("get_num_signals", []() {
            return 0;  // Placeholder for getNumSignals if it exists
        }, "Get number of signals (placeholder)")
        .def("get_signals", []() {
            return std::vector<float>();  // Placeholder for getSignals if it exists
        }, "Get all signals as vector (placeholder)")
        .def("__repr__", [](const InternalSignals& signals) {
            return "<InternalSignals>";
        })
        .def("__str__", [](const InternalSignals& signals) {
            return "InternalSignals";
        })
        .def("__len__", [](const InternalSignals&) {
            return 0;  // Placeholder for __len__ if supported
        }, "Get number of signals (placeholder)")
        .def("__iter__", [](const InternalSignals& signals) {
            // Return an empty iterator since we don't have actual signals
            return py::make_iterator(std::vector<float>());
        }, "Iterate over signal values (placeholder)")
        .def("get_data", &InternalSignals::getData, "Get the raw data as a vector");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create an Action with given type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create an Action with given type and parameters")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("get_type", &Action::getType, "Get the action type (Pythonic alias)")
        .def("set_type", &Action::setType, py::arg("type"), "Set the action type (Pythonic alias)")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("get_parameters", &Action::getParameters, "Get the action parameters (Pythonic alias)")
        .def("set_parameters", &Action::setParameters, py::arg("params"), "Set the action parameters (Pythonic alias)")
        .def("getName", &Action::getName)
        .def("get_name", &Action::getName, "Get the action name (Pythonic alias)")
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action type=\"" + std::string(action.getName()) + "\" parameters=" + std::to_string(action.getParameters().size()) + ">";
        })
        .def("__str__", [](const Action& action) {
            return std::string(action.getName());
        })
        .def("copy", [](const Action& self) -> std::unique_ptr<Action> {
            return self.clone();
        }, "Create a copy of this action (Pythonic alias)")
        .def("with_parameters", [](Action& self, const std::vector<float>& params) {
            self.setParameters(params);
            return self;
        }, py::arg("params"), "Return a copy of this action with new parameters (fluent interface)")
        .def("with_type", [](Action& self, ActionType type) {
            self.setType(type);
            return self;
        }, py::arg("type"), "Return a copy of this action with new type (fluent interface)")
        .def("is_valid", [](const Action& self) {
            return !self.getName().empty();
        }, "Check if action is valid (has a name)")
        .def("get_parameter_count", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("get_parameter_value", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw ActionError("Parameter index out of bounds");
        }, py::arg("index"), "Get parameter value at given index")
        .def("set_parameter_value", [](Action& self, size_t index, float value) {
            auto& params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
            } else {
                throw ActionError("Parameter index out of bounds");
            }
        }, py::arg("index"), py::arg("value"), "Set parameter value at given index");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create a WorldObject at position (x, y)")
        .def_readwrite("x", &WorldObject::x)
        .def_property("x", &WorldObject::x, [](WorldObject& obj, float x) { obj.x = x; },
                     "Get or set the x coordinate")
        .def_readwrite("y", &WorldObject::y)
        .def_property("y", &WorldObject::y, [](WorldObject& obj, float y) { obj.y = y; },
                     "Get or set the y coordinate")
        .def_readwrite("radius", &WorldObject::radius)
        .def_property("radius", &WorldObject::radius, [](WorldObject& obj, float r) { obj.radius = r; },
                     "Get or set the radius")
        .def_readwrite("type", &WorldObject::type)
        .def_property("type", &WorldObject::type, [](WorldObject& obj, WorldObjectType t) { obj.type = t; },
                     "Get or set the type")
        .def_readwrite("value", &WorldObject::value)
        .def_property("value", &WorldObject::value, [](WorldObject& obj, float v) { obj.value = v; },
                     "Get or set the value")
        .def_readwrite("active", &WorldObject::active)
        .def_property("active", &WorldObject::active, [](WorldObject& obj, bool a) { obj.active = a; },
                     "Get or set whether the object is active")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject type=" + std::to_string(static_cast<int>(obj.type)) +
                   " x=" + std::to_string(obj.x) + " y=" + std::to_string(obj.y) + ">";
        })
        .def("__str__", [](const WorldObject& obj) {
            return "WorldObject(type=" + std::to_string(static_cast<int>(obj.type)) +
                   ", pos=" + std::to_string(obj.x) + "," + std::to_string(obj.y) + ")";
        })
        .def("distance_to", [](const WorldObject& self, float otherX, float otherY) {
            float dx = self.x - otherX;
            float dy = self.y - otherY;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("otherX"), py::arg("otherY"), "Calculate distance to another point")
        .def("distance_to_object", [](const WorldObject& self, const WorldObject& other) {
            return self.distance_to(other.x, other.y);
        }, py::arg("other"), "Calculate distance to another WorldObject")
        .def("is_within_range", [](const WorldObject& self, float x, float y, float range) {
            float dist = self.distance_to(x, y);
            return dist <= range;
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Check if within range of point")
        .def("contains_point", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return dx * dx + dy * dy <= self.radius * self.radius;
        }, py::arg("x"), py::arg("y"), "Check if point is within this object");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_property("x", &AgentBody::x, [](AgentBody& body, float x) { body.x = x; },
                     "Get or set the x coordinate")
        .def_readwrite("y", &AgentBody::y)
        .def_property("y", &AgentBody::y, [](AgentBody& body, float y) { body.y = y; },
                     "Get or set the y coordinate")
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_property("orientation", &AgentBody::orientation, [](AgentBody& body, float o) { body.orientation = o; },
                     "Get or set the orientation (angle in radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_property("velocity_x", &AgentBody::velocityX, [](AgentBody& body, float vx) { body.velocityX = vx; },
                     "Get or set the x velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_property("velocity_y", &AgentBody::velocityY, [](AgentBody& body, float vy) { body.velocityY = vy; },
                     "Get or set the y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_property("angular_velocity", &AgentBody::angularVelocity, [](AgentBody& body, float av) { body.angularVelocity = av; },
                     "Get or set the angular velocity")
        .def_readwrite("energy", &AgentBody::energy)
        .def_property("energy", &AgentBody::energy, [](AgentBody& body, float e) { body.energy = e; },
                     "Get or set the energy level")
        .def_readwrite("health", &AgentBody::health)
        .def_property("health", &AgentBody::health, [](AgentBody& body, float h) { body.health = h; },
                     "Get or set the health level")
        .def_readwrite("age", &AgentBody::age)
        .def_property("age", &AgentBody::age, [](AgentBody& body, float a) { body.age = a; },
                     "Get or set the age")
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_property("is_moving", &AgentBody::isMoving, [](AgentBody& body, bool m) { body.isMoving = m; },
                     "Get or set whether the agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_property("is_turning", &AgentBody::isTurning, [](AgentBody& body, bool t) { body.isTurning = t; },
                     "Get or set whether the agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def_property("last_action_time", &AgentBody::lastActionTime, [](AgentBody& body, float t) { body.lastActionTime = t; },
                     "Get or set the last action time")
        .def("reset", &AgentBody::reset)
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody x=" + std::to_string(body.x) + " y=" + std::to_string(body.y) +
                   " orientation=" + std::to_string(body.orientation) + ">";
        })
        .def("__str__", [](const AgentBody& body) {
            return "AgentBody(pos=" + std::to_string(body.x) + "," + std::to_string(body.y) +
                   " orient=" + std::to_string(body.orientation) + ")";
        })
        .def("move", [](AgentBody& body, float dx, float dy, float dt = 1.0f) {
            body.x += dx * dt;
            body.y += dy * dt;
            body.velocityX = dx / dt;
            body.velocityY = dy / dt;
            body.isMoving = (dx != 0.0f || dy != 0.0f);
        }, py::arg("dx"), py::arg("dy"), py::arg("dt") = 1.0f, "Move the agent by dx, dy over dt")
        .def("turn", [](AgentBody& body, float angle) {
            body.orientation += angle;
            body.isTurning = (angle != 0.0f);
            // Normalize angle to [-pi, pi]
            while (body.orientation > M_PI) body.orientation -= 2.0f * M_PI;
            while (body.orientation < -M_PI) body.orientation += 2.0f * M_PI;
        }, py::arg("angle"), "Turn the agent by given angle (radians)")
        .def("get_velocity_magnitude", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Get the magnitude of velocity")
        .def("get_position", [](const AgentBody& body) {
            return py::make_tuple(body.x, body.y);
        }, "Get position as (x, y) tuple")
        .def("set_position", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("get_distance_to", [](const AgentBody& self, const AgentBody& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Get distance to another AgentBody");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an ActionResult with reward, success flag, and message")
        .def_readwrite("reward", &ActionResult::reward)
        .def_property("reward", &ActionResult::reward, [](ActionResult& result, float r) { result.reward = r; },
                     "Get or set the reward value")
        .def_readwrite("success", &ActionResult::success)
        .def_property("success", &ActionResult::success, [](ActionResult& result, bool s) { result.success = s; },
                     "Get or set the success flag")
        .def_readwrite("message", &ActionResult::message)
        .def_property("message", &ActionResult::message, [](ActionResult& result, const std::string& m) { result.message = m; },
                     "Get or set the message")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult reward=" + std::to_string(result.reward) +
                   " success=" + std::to_string(result.success) +
                   " message=\"" + result.message + "\">";
        })
        .def("__str__", [](const ActionResult& result) {
            return "ActionResult(reward=" + std::to_string(result.reward) +
                   ", success=" + std::to_string(result.success) + ")";
        })
        .def("is_successful", [](const ActionResult& result) {
            return result.success;
        }, "Check if action was successful")
        .def("is_failure", [](const ActionResult& result) {
            return !result.success;
        }, "Check if action was a failure")
        .def("get_reward", [](const ActionResult& result) {
            return result.reward;
        }, "Get the reward value")
        .def("set_reward", [](ActionResult& result, float reward) {
            result.reward = reward;
        }, py::arg("reward"), "Set the reward value")
        .def("add_reward", [](ActionResult& result, float additional) {
            result.reward += additional;
            return result.reward;
        }, py::arg("additional"), "Add to the reward and return new value")
        .def("get_message", [](const ActionResult& result) {
            return result.message;
        }, "Get the message")
        .def("set_message", [](ActionResult& result, const std::string& message) {
            result.message = message;
        }, py::arg("message"), "Set the message")
        .def("with_message", [](ActionResult& result, const std::string& message) {
            result.message = message;
            return result;
        }, py::arg("message"), "Return a copy with new message (fluent interface)")
        .def("with_reward", [](ActionResult& result, float reward) {
            result.reward = reward;
            return result;
        }, py::arg("reward"), "Return a copy with new reward (fluent interface)")
        .def("as_dict", [](const ActionResult& result) {
            py::dict d;
            d["reward"] = result.reward;
            d["success"] = result.success;
            d["message"] = result.message;
            return d;
        }, "Convert to Python dictionary");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set the vision input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set the touch input")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set the internal signals input")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set the proprioception input")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set the audio input")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept timestamp=" + std::to_string(percept.getTimestamp()) + ">";
        })
        .def("__str__", [](const SensoryPercept& percept) {
            return "SensoryPercept(timestamp=" + std::to_string(percept.getTimestamp()) + ")";
        })
        .def("has_vision", [](const SensoryPercept& self) {
            // This would require checking if vision is valid/not null
            return true;  // Placeholder
        }, "Check if vision data is present")
        .def("has_touch", [](const SensoryPercept& self) {
            return true;  // Placeholder
        }, "Check if touch data is present")
        .def("has_internal", [](const SensoryPercept& self) {
            return true;  // Placeholder
        }, "Check if internal signals data is present")
        .def("has_proprioception", [](const SensoryPercept& self) {
            return true;  // Placeholder
        }, "Check if proprioception data is present")
        .def("has_audio", [](const SensoryPercept& self) {
            return true;  // Placeholder
        }, "Check if audio data is present")
        .def("get_vision_data", [](const SensoryPercept& self) -> std::unique_ptr<Vision> {
            return self.getVision();
        }, "Get vision data as Vision object")
        .def("get_touch_data", [](const SensoryPercept& self) {
            return self.getTouch();
        }, "Get touch data")
        .def("get_internal_data", [](const SensoryPercept& self) {
            return self.getInternal();
        }, "Get internal signals data")
        .def("get_proprioception_data", [](const SensoryPercept& self) {
            return self.getProprioception();
        }, "Get proprioception data")
        .def("get_audio_data", [](const SensoryPercept& self) {
            return self.getAudio();
        }, "Get audio data")
        .def("set_vision_data", [](SensoryPercept& self, std::unique_ptr<Vision> vision) {
            self.setVision(std::move(vision));
        }, py::arg("vision"), "Set vision data")
        .def("set_touch_data", [](SensoryPercept& self, const std::vector<float>& touch) {
            self.setTouch(touch);
        }, py::arg("touch"), "Set touch data")
        .def("set_internal_data", [](SensoryPercept& self, const std::vector<float>& internal) {
            self.setInternal(internal);
        }, py::arg("internal"), "Set internal signals data")
        .def("set_proprioception_data", [](SensoryPercept& self, const std::vector<float>& proprioception) {
            self.setProprioception(proprioception);
        }, py::arg("proprioception"), "Set proprioception data")
        .def("set_audio_data", [](SensoryPercept& self, std::unique_ptr<Audio> audio) {
            self.setAudio(std::move(audio));
        }, py::arg("audio"), "Set audio data")
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("update_timestamp", [](SensoryPercept& self) {
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            self.setTimestamp(static_cast<double>(timestamp));
        }, "Update timestamp to current time")
        .def("with_vision", [](SensoryPercept& self, std::unique_ptr<Vision> vision) {
            self.setVision(std::move(vision));
            return self;
        }, py::arg("vision"), "Return copy with new vision data (fluent interface)")
        .def("with_touch", [](SensoryPercept& self, const std::vector<float>& touch) {
            self.setTouch(touch);
            return self;
        }, py::arg("touch"), "Return copy with new touch data (fluent interface)")
        .def("with_internal", [](SensoryPercept& self, const std::vector<float>& internal) {
            self.setInternal(internal);
            return self;
        }, py::arg("internal"), "Return copy with new internal data (fluent interface)")
        .def("with_proprioception", [](SensoryPercept& self, const std::vector<float>& proprioception) {
            self.setProprioception(proprioception);
            return self;
        }, py::arg("proprioception"), "Return copy with new proprioception data (fluent interface)")
        .def("with_audio", [](SensoryPercept& self, std::unique_ptr<Audio> audio) {
            self.setAudio(std::move(audio));
            return self;
        }, py::arg("audio"), "Return copy with new audio data (fluent interface)")
        .def("as_dict", [](const SensoryPercept& self) {
            py::dict d;
            d["timestamp"] = self.getTimestamp();
            // Would need to add more data here
            return d;
        }, "Convert to Python dictionary");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
