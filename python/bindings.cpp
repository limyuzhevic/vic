#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>

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

// Exception classes for Python bindings
class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public std::runtime_error {
public:
    explicit ConfigurationError(const std::string& msg) : std::runtime_error(msg) {}
};

class SimulationError : public std::runtime_error {
public:
    explicit SimulationError(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper functions for Pythonic API
namespace py_helpers {
    // Convert vector to numpy-like array interface for better performance
    template<typename T>
    std::vector<T> ensure_vector(const py::object& obj) {
        if (py::isinstance<py::sequence>(obj)) {
            return obj.cast<std::vector<T>>();
        }
        throw ValidationError("Expected a sequence (list or tuple)");
    }
    
    // Validate data ranges
    template<typename T>
    void validate_range(const std::vector<T>& data, T min, T max, const std::string& context) {
        if (data.empty()) {
            throw ValidationError(context + " data cannot be empty");
        }
        if (min != std::numeric_limits<T>::lowest() && max != std::numeric_limits<T>::max()) {
            for (size_t i = 0; i < data.size(); ++i) {
                if (data[i] < min || data[i] > max) {
                    throw ValidationError(context + " data at index " + std::to_string(i) + 
                                        " (" + std::to_string(data[i]) + ") is outside valid range [" +
                                        std::to_string(min) + ", " + std::to_string(max) + "]");
                }
            }
        }
    }
    
    // Thread-safe string buffer for __repr__ methods
    thread_local static std::string repr_buffer;
    
    inline const std::string& get_repr_buffer() {
        return repr_buffer;
    }
    
    inline void set_repr_buffer(const std::string& str) {
        repr_buffer = str;
    }
}

// Enhanced ID classes with Pythonic features
py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
    Unique identifier for a neuron with Pythonic interface.
    
    NeuronId objects are hashable and can be compared.
    They provide intuitive string representation and arithmetic operations.
    
    Examples:
        >>> neuron_id = NeuronId(42)
        >>> print(neuron_id)
        <NeuronId: 42>
        >>> neuron_id.value
        42
        >>> hash(neuron_id)  # For use in dictionaries/sets
        42
    )pbdoc")
    .def(py::init<>())
    .def(py::init<uint64_t>(), py::arg("value"))
    .def_readwrite("value", &NeuronId::value, "The numeric value of the neuron ID")
    .def("index", &NeuronId::index, "Get the index (alias for value)")
    .def("__eq__", &NeuronId::operator==, py::is_operator(), "Check equality")
    .def("__ne__", &NeuronId::operator!=, py::is_operator(), "Check inequality")
    .def("__lt__", &NeuronId::operator<, py::is_operator(), "Check less than")
    .def("__gt__", &NeuronId::operator>, py::is_operator(), "Check greater than")
    .def("__le__", &NeuronId::operator<=, py::is_operator(), "Check less than or equal")
    .def("__ge__", &NeuronId::operator>=, py::is_operator(), "Check greater than or equal")
    .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
    .def("__repr__", [](const NeuronId& id) {
        py_helpers::set_repr_buffer("<NeuronId: " + std::to_string(id.value) + ">");
        return py_helpers::get_repr_buffer();
    })
    .def("__str__", [](const NeuronId& id) {
        return std::to_string(id.value);
    })
    .def("__int__", [](const NeuronId& id) { return id.value; })
    .def_static("is_valid", [](uint64_t value) {
        return value != UINT64_MAX;
    }, py::arg("value"), "Check if a value represents a valid NeuronId")
    .def("is_valid", &NeuronId::value, "Check if this ID is valid (not INVALID_NEURON_ID)");

py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
    Unique identifier for a synapse with Pythonic interface.
    
    SynapseId objects provide hashable identity and comparison operations.
    
    Examples:
        >>> syn_id = SynapseId(123)
        >>> print(repr(syn_id))
        <SynapseId: 123>
    )pbdoc")
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

py::class_<RegionId>(m, "RegionId", R"pbdoc(
    Unique identifier for a brain region with Pythonic interface.
    
    RegionId objects support comparison and hashing for use in dictionaries.
)pbdoc")
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

py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
    Unique identifier for a neuron population with Pythonic interface.
)pbdoc")
    .def(py::init<>())
    .def(py::init<uint64_t>(), py::arg("value"))
    .def_readwrite("value", &PopulationId::value)
    .def("index", &PopulationId::index)
    .def("__eq__", &PopulationId::operator==)
    .def("__ne__", &PopulationId::operator!=)
    .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
    .def("__repr__", [](const PopulationId& id) {
        return "<PopulationId: " + std::to_string(id.value) + ">";
    });

// Enhanced enumerations with Pythonic access
py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
    Enumeration of neuron types.
    
    These types categorize neurons based on their function in the network.
    
    Values:
        - Excitatory: Neurons that increase activity in other neurons
        - Inhibitory: Neurons that decrease activity in other neurons
        - Modulatory: Neurons that modify plasticity and learning
        - Sensory: Neurons that process sensory input
        - Motor: Neurons that control actions
        - Internal: Neurons involved in internal processing
    )pbdoc")
    .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron")
    .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron")
    .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron")
    .value("Sensory", NeuronType::Sensory, "Sensory neuron")
    .value("Motor", NeuronType::Motor, "Motor neuron")
    .value("Internal", NeuronType::Internal, "Internal neuron")
    .export_values();

py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
    Enumeration of synapse types.
    
    Different synapse types have different transmission characteristics.
)pbdoc")
    .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse")
    .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse")
    .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
    .value("Electrical", SynapseType::Electrical, "Electrical synapse")
    .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
    .export_values();

py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
    Developmental stage of the brain.
    
    The brain undergoes different developmental phases affecting plasticity
    and learning capabilities.
)pbdoc")
    .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
    .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period stage")
    .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
    .value("Adult", DevelopmentalStage::Adult, "Adult stage")
    .value("Aging", DevelopmentalStage::Aging, "Aging stage")
    .export_values();

py::enum_<FiringState>(m, "FiringState", R"pbdoc(
    Firing state of a neuron.
    
    Neurons can be in different firing states depending on their current activity.
)pbdoc")
    .value("Resting", FiringState::Resting, "Not currently firing")
    .value("Active", FiringState::Active, "Currently firing")
    .value("Refractory", FiringState::Refractory, "Refractory period after spike")
    .value("Inhibited", FiringState::Inhibited, "Temporarily inhibited")
    .export_values();

py::enum_<ActionType>(m, "ActionType", R"pbdoc(
    Enumeration of action types for motor output.
    
    These represent different possible actions an agent can take.
)pbdoc")
    .value("MoveForward", ActionType::MoveForward, "Move forward")
    .value("MoveBackward", ActionType::MoveBackward, "Move backward")
    .value("MoveLeft", ActionType::MoveLeft, "Move left")
    .value("MoveRight", ActionType::MoveRight, "Move right")
    .value("TurnLeft", ActionType::TurnLeft, "Turn left")
    .value("TurnRight", ActionType::TurnRight, "Turn right")
    .value("Look", ActionType::Look, "Look in current direction")
    .value("LookUp", ActionType::LookUp, "Look up")
    .value("LookDown", ActionType::LookDown, "Look down")
    .value("Interact", ActionType::Interact, "Interact with environment")
    .value("Eat", ActionType::Eat, "Eat/food action")
    .value("Drink", ActionType::Drink, "Drink/water action")
    .value("Rest", ActionType::Rest, "Rest/action")
    .value("Wait", ActionType::Wait, "Wait/idle action")
    .value("Custom", ActionType::Custom, "Custom action")
    .export_values();

py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
    Low-level motor command enumeration.
    
    These are the basic motor commands that can be executed by the agent.
)pbdoc")
    .value("MoveForward", MotorCommand::MoveForward, "Move forward")
    .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
    .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
    .value("TurnRight", MotorCommand::TurnRight, "Turn right")
    .value("LookLeft", MotorCommand::LookLeft, "Look left")
    .value("LookRight", MotorCommand::LookRight, "Look right")
    .value("Interact", MotorCommand::Interact, "Interact")
    .value("Wait", MotorCommand::Wait, "Wait/idle")
    .export_values();

py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
    Type of object in the world.
    
    Different object types have different properties and behaviors.
)pbdoc")
    .value("Empty", WorldObjectType::Empty, "Empty space")
    .value("Resource", WorldObjectType::Resource, "Resource for agent")
    .value("Hazard", WorldObjectType::Hazard, "Hazard/danger for agent")
    .value("Wall", WorldObjectType::Wall, "Physical wall")
    .value("Marker", WorldObjectType::Marker, "Visual marker")
    .export_values();

// Enhanced Config class with Pythonic interface
py::class_<Config>(m, "Config", R"pbdoc(
    Configuration system for NLM with Pythonic interface.
    
    The Config class provides key-value storage for brain parameters.
    It supports loading/saving from/to files and provides convenient
    access methods for common configuration patterns.
    
    Examples:
        >>> config = Config()
        >>> config.set("brain.neuron_count", 1000)
        >>> config.set("brain.synapse_density", 0.1)
        >>> neurons = config.get("brain.neuron_count", 1000)
        >>> config.has("brain.neuron_count")
        True
        
        >>> config.load_from_file("settings.json")
        >>> config.save_to_file("new_settings.json")
    
    Key Categories:
        - brain: Core brain parameters
        - plasticity: Learning rules and mechanisms  
        - neuromod: Neuromodulation parameters
        - development: Developmental processes
        - sensory: Sensory processing settings
        - motor: Motor system parameters
    )pbdoc")
    .def(py::init<>())
    .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
         "Load configuration from a JSON file")
    .def("load_from_args", [](Config& self, int argc, char** argv) {
        return self.loadFromArgs(argc, argv);
    }, py::arg("argc"), py::arg("argv"),
         "Load configuration from command line arguments")
    .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
         "Save configuration to a JSON file")
    .def("has", &Config::has, py::arg("key"),
         "Check if a configuration key exists")
    .def("get", py::overload_cast<const std::string&, const std::string&>(&Config::get),
         py::arg("key"), py::arg("default_value"),
         "Get a configuration value with default fallback")
    .def("get", py::overload_cast<const std::string&>(&Config::get),
         py::arg("key"),
         "Get a configuration value (throws if not found)")
    .def("set", py::overload_cast<const std::string&, int64_t>(&Config::set),
         py::arg("key"), py::arg("value"),
         "Set an integer configuration value")
    .def("set", py::overload_cast<const std::string&, double>(&Config::set),
         py::arg("key"), py::arg("value"),
         "Set a float/double configuration value")
    .def("set", py::overload_cast<const std::string&, bool>(&Config::set),
         py::arg("key"), py::arg("value"),
         "Set a boolean configuration value")
    .def("set", py::overload_cast<const std::string&, const std::string&>(&Config::set),
         py::arg("key"), py::arg("value"),
         "Set a string configuration value")
    .def("get_keys", &Config::getKeys, "Get all configuration keys")
    .def("clear", &Config::clear, "Clear all configuration entries")
    .def("summary", &Config::summary, "Get a summary string of the configuration")
    .def("__repr__", [](const Config& cfg) {
        return "<Config: " + cfg.summary() + ">";
    })
    .def("__getitem__", py::overload_cast<const std::string&>(&Config::get),
         py::arg("key"), "Access configuration using [] operator")
    .def("__setitem__", py::overload_cast<const std::string&, const std::string&>(&Config::set),
         py::arg("key"), py::arg("value"), "Set configuration using [] operator")
    .def("__contains__", &Config::has, py::arg("key"), "Check key existence using 'in' operator")
    // Convenience methods for common configuration patterns
    .def("brain", [](Config& self) {
        // Helper method to access brain-specific config
        py::dict brain_config;
        for (const auto& key : self.getKeys()) {
            if (key.find("brain.") == 0) {
                std::string brain_key = key.substr(6); // Remove "brain." prefix
                // Convert value type
                if (self.has(key)) {
                    brain_config[brain_key] = self.get(key);
                }
            }
        }
        return brain_config;
    }, "Get all brain-related configuration parameters")
    .def("get_brain", &Config::get, py::arg("brain_key"),
         "Get a specific brain configuration parameter (e.g., 'neuron_count')")
    .def("set_brain", [](Config& self, const std::string& key, auto value) {
        self.set("brain." + key, value);
    }, py::arg("key"), py::arg("value"),
         "Set a brain configuration parameter (e.g., set_brain('neuron_count', 1000))")
    .def("plasticity", [](Config& self) {
        py::dict plasticity_config;
        for (const auto& key : self.getKeys()) {
            if (key.find("plasticity.") == 0) {
                std::string plasticity_key = key.substr(10); // Remove "plasticity." prefix
                if (self.has(key)) {
                    plasticity_config[plasticity_key] = self.get(key);
                }
            }
        }
        return plasticity_config;
    }, "Get all plasticity-related configuration parameters")
    .def("neuromod", [](Config& self) {
        py::dict neuromod_config;
        for (const auto& key : self.getKeys()) {
            if (key.find("neuromod.") == 0) {
                std::string neuromod_key = key.substr(8); // Remove "neuromod." prefix
                if (self.has(key)) {
                    neuromod_config[neuromod_key] = self.get(key);
                }
            }
        }
        return neuromod_config;
    }, "Get all neuromodulation-related configuration parameters")
    .def("development", [](Config& self) {
        py::dict development_config;
        for (const auto& key : self.getKeys()) {
            if (key.find("development.") == 0) {
                std::string development_key = key.substr(11); // Remove "development." prefix
                if (self.has(key)) {
                    development_config[development_key] = self.get(key);
                }
            }
        }
        return development_config;
    }, "Get all development-related configuration parameters");

// Enhanced SensoryInput base class
py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
    Base class for sensory input with enhanced Python interface.
    
    This abstract class represents sensory data from the environment.
    Subclasses include Vision, Audio, and InternalSignals.
    
    The class provides methods for accessing sensory data and metadata.
)pbdoc")
    .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
    .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
    .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
    .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
    .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp")
    .def("__repr__", [](const SensoryInput& input) {
        return "<SensoryInput type=" + std::to_string(static_cast<int>(input.getType())) + ">";
    });

// Enhanced Vision class with Pythonic interface
py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
    Vision sensory input with Pythonic interface.
    
    Represents visual input from the environment as a 2D array.
    Provides convenient access to width, height, and pixel data.
    
    Examples:
        >>> vision = Vision(640, 480, 3)  # RGB image
        >>> vision.set_data([0.0] * 640 * 480 * 3)
        >>> print(f"Image size: {vision.width}x{vision.height}")
        >>> print(f"Channels: {vision.channels}")
        >>> print(f"Total pixels: {vision.get_dimensions()}")
        
        # Access individual pixels
        >>> for y in range(vision.height):
        ...     for x in range(vision.width):
        ...         pixel = vision.get_pixel(x, y)
    )pbdoc")
    .def(py::init<>())
    .def(py::init<size_t, size_t, size_t>(), py::arg("width") = 0, py::arg("height") = 0, py::arg("channels") = 3)
    .def("width", py::property([] (const Vision& v) { return v.getWidth(); }),
         "Get vision width")
    .def("height", py::property([] (const Vision& v) { return v.getHeight(); }),
         "Get vision height")
    .def("channels", py::property([] (const Vision& v) { return v.getChannels(); }),
         "Get vision channels")
    .def("get_pixel", [](const Vision& v, size_t x, size_t y, size_t c = 0) {
        size_t index = ((y * v.getWidth()) + x) * v.getChannels() + c;
        if (index < v.getData().size()) {
            return v.getData()[index];
        }
        throw ValidationError("Pixel coordinates out of bounds");
    }, py::arg("x"), py::arg("y"), py::arg("channel") = 0,
         "Get individual pixel value at (x, y, channel)")
    .def("set_pixel", [](Vision& v, size_t x, size_t y, size_t c, float value) {
        size_t index = ((y * v.getWidth()) + x) * v.getChannels() + c;
        if (index < v.getData().size()) {
            v.getData()[index] = value;
        } else {
            throw ValidationError("Pixel coordinates out of bounds");
        }
    }, py::arg("x"), py::arg("y"), py::arg("channel"), py::arg("value"),
         "Set individual pixel value at (x, y, channel)")
    .def("set_data", [](Vision& self, const std::vector<float>& data) {
        if (self.getWidth() * self.getHeight() * self.getChannels() != data.size()) {
            throw ValidationError("Data size doesn't match vision dimensions");
        }
        self.setData(data);
    }, py::arg("data"), "Set vision data with validation")
    .def("get_data", &Vision::getData, "Get vision data as a flat vector")
    .def("clear", [](Vision& v) {
        v.setData(std::vector<float>(v.getWidth() * v.getHeight() * v.getChannels(), 0.0f));
    }, "Clear vision data to zeros")
    .def("clone", &Vision::clone, "Create a copy of the vision input")
    .def("__len__", [](const Vision& v) {
        return v.getWidth() * v.getHeight() * v.getChannels();
    }, "Get total number of pixel values")
    .def("__repr__", [](const Vision& v) {
        return "<Vision width=" + std::to_string(v.getWidth()) + 
               " height=" + std::to_string(v.getHeight()) +
               " channels=" + std::to_string(v.getChannels()) + ">";
    });

// Enhanced Audio class
py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
    Audio sensory input with Pythonic interface.
    
    Represents audio input as a time series of samples.
    Provides convenient access to sample rate and duration.
    
    Examples:
        >>> audio = Audio(sample_rate=44100, num_samples=1024)
        >>> audio.set_data([0.0] * 1024)
        >>> print(f"Duration: {audio.duration:.2f} seconds")
        >>> print(f"Samples: {len(audio.get_data())}")
    )pbdoc")
    .def(py::init<>())
    .def(py::init<size_t, size_t>(), py::arg("sample_rate") = 0, py::arg("num_samples") = 0)
    .def("sample_rate", py::property([] (const Audio& a) { return a.getSampleRate(); }),
         "Get audio sample rate")
    .def("num_samples", py::property([] (const Audio& a) { return a.getNumSamples(); }),
         "Get number of audio samples")
    .def("duration", py::property([] (const Audio& a) {
        if (a.getSampleRate() > 0) {
            return static_cast<double>(a.getNumSamples()) / a.getSampleRate();
        }
        return 0.0;
    }), "Get audio duration in seconds")
    .def("set_data", [](Audio& self, const std::vector<float>& data) {
        if (data.size() != self.getNumSamples()) {
            throw ValidationError("Data size doesn't match audio sample count");
        }
        self.setData(data);
    }, py::arg("data"), "Set audio data with validation")
    .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"), "Set sample rate")
    .def("get_data", &Audio::getData, "Get audio data as a vector")
    .def("clear", [](Audio& a) {
        a.setData(std::vector<float>(a.getNumSamples(), 0.0f));
    }, "Clear audio data to zeros")
    .def("clone", &Audio::clone, "Create a copy of the audio input")
    .def("__len__", [](const Audio& a) {
        return a.getNumSamples();
    }, "Get number of audio samples")
    .def("__repr__", [](const Audio& a) {
        return "<Audio sample_rate=" + std::to_string(a.getSampleRate()) +
               " num_samples=" + std::to_string(a.getNumSamples()) + ">";
    });

// Enhanced InternalSignals class
py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
    Internal signals sensory input with Pythonic interface.
    
    Represents internal brain state signals such as rewards, prediction errors,
    neuromodulation levels, etc.
    
    Examples:
        >>> signals = InternalSignals()
        >>> signals.add_signal(1.0)  # Add a reward signal
        >>> signals.add_signal(0.5)  # Add a prediction error signal
        >>> print(f"Number of signals: {len(signals.get_data())}")
        >>> print(f"Sum of signals: {sum(signals.get_data())}")
    )pbdoc")
    .def(py::init<>())
    .def("add_signal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
    .def("get_signals", &InternalSignals::getData, "Get all signal values as a list")
    .def("set_signals", [](InternalSignals& self, const std::vector<float>& signals) {
        self.clearSignals();
        for (float value : signals) {
            self.addSignal(value);
        }
    }, py::arg("signals"), "Set all signal values")
    .def("clear", &InternalSignals::clearSignals, "Clear all signals")
    .def("get_sum", [](const InternalSignals& self) {
        const auto& data = self.getData();
        return std::accumulate(data.begin(), data.end(), 0.0f);
    }, "Get sum of all signals")
    .def("get_mean", [](const InternalSignals& self) {
        const auto& data = self.getData();
        if (data.empty()) return 0.0f;
        return std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
    }, "Get mean of all signals")
    .def("get_count", [](const InternalSignals& self) {
        return self.getData().size();
    }, "Get number of signals")
    .def("__len__", [](const InternalSignals& self) {
        return self.getData().size();
    }, "Get number of signals (using len())")
    .def("__iter__", [](const InternalSignals& self) {
        return py::make_iterator(self.getData().begin(), self.getData().end());
    }, "Iterate over signal values")
    .def("__repr__", [](const InternalSignals& self) {
        const auto& data = self.getData();
        std::string repr = "<InternalSignals count=" + std::to_string(data.size()) + " values=[";
        for (size_t i = 0; i < data.size(); ++i) {
            if (i > 0) repr += ", ";
            repr += std::to_string(data[i]);
        }
        repr += ">";
        return repr;
    });

// Enhanced Action class
py::class_<Action>(m, "Action", R"pbdoc(
    Action representation with Pythonic interface.
    
    Represents a motor action with type and parameters.
    Provides convenient access and manipulation methods.
    
    Examples:
        >>> action = Action(ActionType.MoveForward)
        >>> print(action.type)
        ActionType.MoveForward
        >>> print(action.get_name())
        'MoveForward'
        >>> action.set_parameters([1.0, 2.0])  # Set movement parameters
        >>> print(action.get_parameters())
        [1.0, 2.0]
        
        # Create action with parameters
        >>> move_action = Action(ActionType.MoveForward, [1.0, 0.0])
    )pbdoc")
    .def(py::init<>())
    .def(py::init<ActionType>(), py::arg("type"))
    .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
    .def("type", py::property(&Action::getType), "Get action type")
    .def("set_type", &Action::setType, py::arg("type"), "Set action type")
    .def("get_parameters", &Action::getParameters, "Get action parameters")
    .def("set_parameters", &Action::setParameters, py::arg("params"), "Set action parameters")
    .def("get_name", &Action::getName, "Get human-readable action name")
    .def("clone", &Action::clone, "Create a copy of the action")
    .def("with_parameters", [](const Action& self, const std::vector<float>& params) {
        Action new_action = self.clone();
        new_action.setParameters(params);
        return new_action;
    }, py::arg("parameters"), "Create a new action with different parameters")
    .def("__repr__", [](const Action& action) {
        return "<Action type=" + action.getName() + 
               " params=" + std::to_string(action.getParameters().size()) + ">";
    })
    .def("__eq__", [](const Action& a1, const Action& a2) {
        return a1.getType() == a2.getType() && 
               a1.getParameters() == a2.getParameters();
    }, "Check equality with another action")
    .def("is_same_type", [](const Action& self, ActionType type) {
        return self.getType() == type;
    }, py::arg("type"), "Check if action is of specific type");

// Enhanced WorldObject class
py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
    World object representation with Pythonic interface.
    
    Represents an object in the simulated world with position and properties.
    
    Examples:
        >>> obj = WorldObject(1.0, 2.0, WorldObjectType.Resource, value=1.0)
        >>> print(f"Position: ({obj.x}, {obj.y})")
        >>> print(f"Type: {obj.type}")
        >>> obj.value = 2.0  # Direct access to properties
        >>> obj.active = False
        
        # Create objects conveniently
        >>> wall = WorldObject.create_wall(5.0, 5.0, radius=0.5)
        >>> resource = WorldObject.create_resource(3.0, 4.0, value=10.0)
    )pbdoc")
    .def(py::init<>())
    .def(py::init<float, float, WorldObjectType, float, float>(),
         py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
         py::arg("radius") = 0.5f)
    .def_readwrite("x", &WorldObject::x, "X position")
    .def_readwrite("y", &WorldObject::y, "Y position")
    .def_readwrite("radius", &WorldObject::radius, "Object radius")
    .def_readwrite("type", &WorldObject::type, "Object type")
    .def_readwrite("value", &WorldObject::value, "Object value")
    .def_readwrite("active", &WorldObject::active, "Object active state")
    .def("position", py::property([] (const WorldObject& obj) {
        return std::make_pair(obj.x, obj.y);
    }, [](WorldObject& obj, const std::pair<float, float>& pos) {
        obj.x = pos.first;
        obj.y = pos.second;
    }), "Get/set position as (x, y) tuple")
    .def_static("create_empty", [](float x, float y, float radius = 0.5f) {
        return WorldObject(x, y, WorldObjectType::Empty, 0.0f, radius);
    }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, "Create an empty space object")
    .def_static("create_wall", [](float x, float y, float radius = 0.5f) {
        return WorldObject(x, y, WorldObjectType::Wall, 0.0f, radius);
    }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, "Create a wall object")
    .def_static("create_resource", [](float x, float y, float value = 1.0f, float radius = 0.5f) {
        return WorldObject(x, y, WorldObjectType::Resource, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, py::arg("radius") = 0.5f, "Create a resource object")
    .def_static("create_hazard", [](float x, float y, float value = 1.0f, float radius = 0.5f) {
        return WorldObject(x, y, WorldObjectType::Hazard, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, py::arg("radius") = 0.5f, "Create a hazard object")
    .def_static("create_marker", [](float x, float y, float value = 1.0f, float radius = 0.5f) {
        return WorldObject(x, y, WorldObjectType::Marker, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, py::arg("radius") = 0.5f, "Create a marker object")
    .def("distance_to", [](const WorldObject& obj, float x, float y) {
        float dx = obj.x - x;
        float dy = obj.y - y;
        return std::sqrt(dx*dx + dy*dy);
    }, py::arg("x"), py::arg("y"), "Calculate distance to a point")
    .def("is_near", [](const WorldObject& obj, float x, float y, float threshold) {
        return obj.distance_to(x, y) <= threshold;
    }, py::arg("x"), py::arg("y"), py::arg("threshold"), "Check if object is near a point")
    .def("clone", &WorldObject::clone, "Create a copy of the world object")
    .def("__repr__", [](const WorldObject& obj) {
        return "<WorldObject " + std::to_string(static_cast<int>(obj.type)) +
               " at (" + std::to_string(obj.x) + ", " + std::to_string(obj.y) + ")>";
    });

// Enhanced AgentBody class
py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
    Agent body state with Pythonic interface.
    
    Represents the physical state of an agent in the world.
    Provides convenient access to position, velocity, and state variables.
    
    Examples:
        >>> body = AgentBody()
        >>> body.x = 10.0
        >>> body.y = 5.0
        >>> body.orientation = 90.0  # degrees
        >>> print(f"Position: ({body.position[0]}, {body.position[1]})")
        >>> body.velocity_x = 1.0
        >>> body.velocity_y = 0.5
        >>> print(f"Velocity: {body.velocity_magnitude:.2f} units/s")
        
        # Movement helper
        >>> body.move_forward(1.0)  # Move 1 unit forward
        >>> body.turn(45.0)  # Turn 45 degrees
    )pbdoc")
    .def(py::init<>())
    .def_readwrite("x", &AgentBody::x, "X position")
    .def_readwrite("y", &AgentBody::y, "Y position")
    .def_readwrite("orientation", &AgentBody::orientation, "Orientation in degrees")
    .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
    .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
    .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
    .def_readwrite("energy", &AgentBody::energy, "Current energy level")
    .def_readwrite("health", &AgentBody::health, "Current health (0-100)")
    .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
    .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
    .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
    .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
    .def("position", py::property([] (const AgentBody& b) {
        return std::make_pair(b.x, b.y);
    }, [](AgentBody& b, const std::pair<float, float>& pos) {
        b.x = pos.first;
        b.y = pos.second;
    }), "Get/set position as (x, y) tuple")
    .def("velocity", py::property([] (const AgentBody& b) {
        return std::make_pair(b.velocityX, b.velocityY);
    }, [](AgentBody& b, const std::pair<float, float>& vel) {
        b.velocityX = vel.first;
        b.velocityY = vel.second;
    }), "Get/set velocity as (vx, vy) tuple")
    .def("velocity_magnitude", py::property([] (const AgentBody& b) {
        return std::sqrt(b.velocityX*b.velocityX + b.velocityY*b.velocityY);
    }), "Get velocity magnitude")
    .def("velocity_angle", py::property([] (const AgentBody& b) {
        return std::atan2(b.velocityY, b.velocityX) * 180.0 / M_PI;
    }), "Get velocity direction in degrees")
    .def("move_forward", [](AgentBody& b, float distance) {
        b.x += distance * std::cos(b.orientation * M_PI / 180.0);
        b.y += distance * std::sin(b.orientation * M_PI / 180.0);
    }, py::arg("distance"), "Move forward by distance units")
    .def("move_backward", [](AgentBody& b, float distance) {
        b.x -= distance * std::cos(b.orientation * M_PI / 180.0);
        b.y -= distance * std::sin(b.orientation * M_PI / 180.0);
    }, py::arg("distance"), "Move backward by distance units")
    .def("turn", [](AgentBody& b, float angle) {
        b.orientation = std::fmod(b.orientation + angle, 360.0f);
    }, py::arg("angle"), "Turn by angle degrees")
    .def("reset", &AgentBody::reset, "Reset body to initial state")
    .def("distance_to", [](const AgentBody& b, float x, float y) {
        float dx = b.x - x;
        float dy = b.y - y;
        return std::sqrt(dx*dx + dy*dy);
    }, py::arg("x"), py::arg("y"), "Calculate distance to a point")
    .def("is_near", [](const AgentBody& b, float x, float y, float threshold) {
        return b.distance_to(x, y) <= threshold;
    }, py::arg("x"), py::arg("y"), py::arg("threshold"), "Check if near a point");

// Enhanced ActionResult class
py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
    Action result from world interaction.
    
    Represents the outcome of an agent's action in the world,
    including reward, success status, and optional message.
    
    Examples:
        >>> result = ActionResult(reward=1.0, success=True, message="Found food")
        >>> print(f"Reward: {result.reward}")
        >>> print(f"Success: {result.success}")
        >>> print(f"Message: {result.message}")
        
        # Common result patterns
        >>> success_result = ActionResult(reward=1.0, success=True)
        >>> failure_result = ActionResult(reward=0.0, success=False, message="Hit wall")
        >>> negative_result = ActionResult(reward=-1.0, success=False)
    )pbdoc")
    .def(py::init<>())
    .def(py::init<float, bool, std::string>(),
         py::arg("reward"), py::arg("success"), py::arg("message") = "")
    .def_readwrite("reward", &ActionResult::reward, "Reward value (-1 to 1)")
    .def_readwrite("success", &ActionResult::success, "Whether action was successful")
    .def_readwrite("message", &ActionResult::message, "Optional message describing outcome")
    .def("is_positive", [](const ActionResult& r) { return r.reward > 0.1; },
         "Check if result represents a positive outcome")
    .def("is_negative", [](const ActionResult& r) { return r.reward < -0.1; },
         "Check if result represents a negative outcome")
    .def("is_neutral", [](const ActionResult& r) {
        return std::abs(r.reward) <= 0.1 && r.success;
    }, "Check if result is neutral (small reward, successful)")
    .def("clone", &ActionResult::clone, "Create a copy of the action result")
    .def("__repr__", [](const ActionResult& r) {
        return "<ActionResult reward=" + std::to_string(r.reward) +
               " success=" + std::to_string(r.success) + ">";
    });

// Enhanced SensoryPercept class
py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
    Sensory percept data with Pythonic interface.
    
    Contains all sensory information perceived by an agent at a given time.
    Provides convenient access to vision, touch, internal, and proprioception data.
    
    Examples:
        >>> percept = SensoryPercept()
        >>> vision = Vision(640, 480, 3)
        >>> vision.set_data([0.0] * 640 * 480 * 3)
        >>> percept.set_vision(vision)
        >>> 
        >>> # Access sensory data conveniently
        >>> if percept.has_vision():
        ...     vision = percept.get_vision()
        ...     print(f"Vision: {vision.width}x{vision.height}")
        >>> 
        >>> # Check what senses are active
        >>> print(f"Has vision: {percept.has_vision()}")
        >>> print(f"Has touch: {percept.has_touch()}")
        >>> print(f"Has internal: {percept.has_internal()}")
        >>> print(f"Has proprioception: {percept.has_proprioception()}")
        >>> 
        >>> # Process all signals
        >>> for i, signal in enumerate(percept.get_all_signals()):
        ...     print(f"Signal {i}: {signal}")
    )pbdoc")
    .def(py::init<>())
    .def("get_vision", &SensoryPercept::getVision,
         py::return_value_policy::reference_internal)
    .def("set_vision", &SensoryPercept::setVision, py::arg("vision"),
         "Set vision sensory data")
    .def("get_vision_width", &SensoryPercept::getVisionWidth,
         "Get vision width (convenience method)")
    .def("get_vision_height", &SensoryPercept::getVisionHeight,
         "Get vision height (convenience method)")
    .def("get_touch", &SensoryPercept::getTouch,
         py::return_value_policy::reference_internal)
    .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"),
         "Set touch sensory data")
    .def("get_internal", &SensoryPercept::getInternal,
         py::return_value_policy::reference_internal)
    .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"),
         "Set internal signals data")
    .def("get_proprioception", &SensoryPercept::getProprioception,
         py::return_value_policy::reference_internal)
    .def("set_proprioception", &SensoryPercept::setProprioception,
         py::arg("proprioception"), "Set proprioception data")
    .def("get_audio", &SensoryPercept::getAudio,
         py::return_value_policy::reference_internal)
    .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"),
         "Set audio sensory data")
    .def("get_all_signals", &SensoryPercept::getAllSignals,
         "Get all available signals as a list")
    .def("get_timestamp", &SensoryPercept::getTimestamp,
         "Get percept timestamp")
    .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
         "Set percept timestamp")
    .def("has_vision", [](const SensoryPercept& self) {
        return self.getVisionWidth() > 0 && self.getVisionHeight() > 0;
    }, "Check if vision data is available")
    .def("has_touch", [](const SensoryPercept& self) {
        const auto& touch = self.getTouch();
        return !touch.empty();
    }, "Check if touch data is available")
    .def("has_internal", [](const SensoryPercept& self) {
        const auto& internal = self.getInternal();
        return !internal.empty();
    }, "Check if internal signals are available")
    .def("has_audio", [](const SensoryPercept& self) {
        const auto& audio = self.getAudio();
        return !audio.empty();
    }, "Check if audio data is available")
    .def("clear", [](SensoryPercept& self) {
        // Clear all sensory data
        auto vision = self.getVision();
        vision.setData(std::vector<float>(vision.getWidth() * vision.getHeight() * vision.getChannels(), 0.0f));
        self.setTouch(std::vector<float>());
        self.setInternal(std::vector<float>());
        self.setAudio(std::vector<float>());
        self.setTimestamp(0.0);
    }, "Clear all sensory data")
    .def("clone", &SensoryPercept::clone, "Create a copy of the sensory percept")
    .def("__repr__", [](const SensoryPercept& p) {
        return "<SensoryPercept vision=" + std::to_string(p.getVisionWidth()) + "x" +
               std::to_string(p.getVisionHeight()) + ">";
    });

// Enhanced SimpleWorld class
py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
    Simple 2D world for NLM simulation with enhanced Python interface.
    
    Represents a 2D simulation world where agents interact.
    Provides methods for configuration, simulation control, and agent interaction.
    
    Examples:
        >>> world = SimpleWorld()
        >>> world.configure(width=20, height=20, vision_width=8, vision_height=8)
        >>> world.reset()
        >>> world.set_agent_start(10.0, 10.0)
        >>> 
        >>> # Run simulation
        >>> for step in range(100):
        ...     world.update(0.1)  # Update world physics
        ...     percept = world.get_sensory_percept()  # Get what agent sees
        ...     if world.get_agent_body().is_moving:
        ...         action = agent.decode_motor_command()  # Brain decides action
        ...         world.apply_motor_command(action, world.get_simulation_time())  # Execute action
        >>> 
        >>> # World management
        >>> world.add_object(WorldObject.create_resource(5, 5, value=10.0))
        >>> world.remove_object(5.0, 5.0)
        >>> print(f"World time: {world.get_simulation_time():.2f}")
    )pbdoc")
    .def(py::init<>())
    .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
         py::arg("vision_width") = 0, py::arg("vision_height") = 0,
         "Configure world dimensions and vision fields of view")
    .def("reset", &SimpleWorld::reset, "Reset world to initial state")
    .def("set_agent_start", &SimpleWorld::setAgentStart,
         py::arg("x"), py::arg("y"), "Set agent starting position")
    .def("update", &SimpleWorld::update, py::arg("timestep"),
         "Update world simulation by timestep duration")
    .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
         py::arg("cmd"), py::arg("current_time"),
         "Apply motor command to world at current time")
    .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
         py::return_value_policy::reference_internal,
         "Get current sensory percept from world")
    .def("get_agent_body", &SimpleWorld::getAgentBody,
         py::return_value_policy::reference_internal,
         "Get agent body state")
    .def("add_object", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
    .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
         "Remove object at position (x, y)")
    .def("is_valid_position", &SimpleWorld::isValidPosition,
         py::arg("x"), py::arg("y"), "Check if position is valid for agent")
    .def("get_width", &SimpleWorld::getWidth, "Get world width")
    .def("get_height", &SimpleWorld::getHeight, "Get world height")
    .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
    .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
         "Set maximum agent energy")
    .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
         "Get energy decay rate")
    .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate,
         py::arg("r"), "Set energy decay rate")
    .def("get_simulation_time", &SimpleWorld::getSimulationTime,
         "Get current simulation time")
    .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
         "Set random seed for reproducible simulations")
    .def("get_random_seed", &SimpleWorld::getRandomSeed,
         "Get random seed")
    .def("get_object_count", [](const SimpleWorld& w) {
        // Count active objects (simplified - would need access to internal object list)
        return 0;  // Placeholder for now
    }, "Get number of objects in world (placeholder)")
    .def("get_agent_position", [](SimpleWorld& w) {
        auto body = w.getAgentBody();
        return std::make_pair(body.x, body.y);
    }, "Get agent position as (x, y) tuple")
    .def("set_agent_position", [](SimpleWorld& w, float x, float y) {
        auto body = w.getAgentBody();
        body.x = x;
        body.y = y;
    }, py::arg("x"), py::arg("y"), "Set agent position")
    .def("get_agent_direction", [](SimpleWorld& w) {
        auto body = w.getAgentBody();
        return body.orientation;
    }, "Get agent orientation in degrees")
    .def("set_agent_direction", [](SimpleWorld& w, float angle) {
        auto body = w.getAgentBody();
        body.orientation = angle;
    }, py::arg("angle"), "Set agent orientation in degrees");

// Enhanced Brain class
py::class_<Brain>(m, "Brain", R"pbdoc(
    Central neural simulation brain with enhanced Python interface.
    
    The core neural network that processes information, learns, and produces actions.
    Provides methods for simulation, input processing, and state management.
    
    Examples:
        >>> config = Config()
        >>> config.set_brain("neuron_count", 1000)
        >>> brain = Brain(config)
        >>> brain.initialize()
        >>> 
        >>> # Run simulation
        >>> for step in range(100):
        ...     brain.step(step)  # One step of neural processing
        ...     if step % 10 == 0:
        ...         print(f"Step {step}: {brain.get_firing_neurons()} neurons firing")
        >>> 
        >>> # Process sensory input
        >>> brain.receive_sensory_input(percept)
        >>> action = brain.produce_action()  # Brain generates motor command
        >>> 
        >>> # State management
        >>> brain.save("checkpoint.bin")
        >>> # ... later ...
        >>> brain.load("checkpoint.bin")
    )pbdoc")
    .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
    .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
    .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
         py::arg("currentStep"), "Perform a simulation step")
    .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
         py::arg("currentStep"), py::arg("currentTime"),
         "Perform a simulation step with timestamp")
    .def("receive_sensory_input", &Brain::receiveSensoryInput, py::arg("input"),
         "Inject sensory input into the brain")
    .def("inject_current", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
         "Inject current into a specific neuron")
    .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
         py::arg("type"), py::arg("current"),
         "Inject current into all neurons of a specific type")
    .def("produce_action", &Brain::produceAction,
         "Produce motor action based on neural activity")
    .def("reset", &Brain::reset, "Reset brain state")
    .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
    .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
    .def("add_region", &Brain::addRegion, py::arg("name") = "",
         "Add a new neural region")
    .def("get_region", &Brain::getRegion, py::arg("id"),
         py::return_value_policy::reference_internal, "Get a region by ID")
    .def("get_region_count", &Brain::getRegionCount,
         "Get the number of regions")
    .def("get_region_ids", &Brain::getRegionIds,
         "Get all region IDs")
    .def("get_regions", &Brain::getRegions,
         py::return_value_policy::reference_internal, "Get all regions")
    .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
         "Get total neuron count across all regions")
    .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
         "Get total synapse count across all regions")
    .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
         "Get count of active neurons")
    .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
         "Get count of currently firing neurons")
    .def("get_average_firing_rate", &Brain::getAverageFiringRate,
         "Get average firing rate across all neurons")
    .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
         "Get excitation/inhibition balance ratio")
    .def("get_total_spike_count", &Brain::getTotalSpikeCount,
         "Get total spike count")
    .def("get_developmental_stage", &Brain::getDevelopmentalStage,
         "Get current developmental stage")
    .def("set_developmental_stage", &Brain::setDevelopmentalStage,
         py::arg("stage"), "Set developmental stage")
    .def("get_config", &Brain::getConfig,
         py::return_value_policy::reference_internal, "Get the configuration")
    .def("log_status", &Brain::logStatus, "Log brain status")
    // Pythonic property access
    .def("firing_neurons", [](const Brain& b) { return b.getFiringNeuronCount(); },
         "Get number of firing neurons (convenience property)")
    .def("active_neurons", [](const Brain& b) { return b.getActiveNeuronCount(); },
         "Get number of active neurons (convenience property)")
    .def("spike_count", [](const Brain& b) { return b.getTotalSpikeCount(); },
         "Get total spike count (convenience property)")
    .def("avg_firing_rate", [](const Brain& b) { return b.getAverageFiringRate(); },
         "Get average firing rate (convenience property)")
    .def("ei_ratio", [](const Brain& b) { return b.getExcitationInhibitionRatio(); },
         "Get E/I ratio (convenience property)")
    .def("region_count", [](const Brain& b) { return b.getRegionCount(); },
         "Get region count (convenience property)")
    .def("neuron_count", [](const Brain& b) { return b.getTotalNeuronCount(); },
         "Get total neuron count (convenience property)")
    .def("synapse_count", [](const Brain& b) { return b.getTotalSynapseCount(); },
         "Get total synapse count (convenience property)")
    // Context manager support for safe simulation
    .def("__enter__", [](Brain& self) { return &self; },
         "Context manager entry")
    .def("__exit__", [](Brain&, const py::object&) {
         // Cleanup on context exit
     }, "Context manager exit")
    .def("run_steps", [](Brain& self, SimulationStep start_step, SimulationStep num_steps) {
        for (SimulationStep i = start_step; i < start_step + num_steps; ++i) {
            self.step(i);
        }
    }, py::arg("start_step"), py::arg("num_steps"),
         "Run multiple simulation steps (convenience method)")
    .def("run_simulation", [](Brain& self, SimulationStep num_steps, double timestep = 0.1) {
        for (SimulationStep i = 0; i < num_steps; ++i) {
            self.step(i, i * timestep);
        }
    }, py::arg("num_steps"), py::arg("timestep") = 0.1,
         "Run complete simulation (convenience method)");

// Enhanced AgentBrain class
py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
    Agent brain interface connecting NLM brain to world with enhanced Python interface.
    
    Provides the interface between the neural brain and the external world,
    handling sensory processing, motor decoding, and neuromodulation.
    
    Examples:
        >>> brain = Brain(config)
        >>> brain.initialize()
        >>> agent = AgentBrain(brain)
        >>> world = SimpleWorld()
        >>> world.configure(20, 20, 8, 8)
        >>> world.reset()
        >>> agent.initialize(world)
        >>> 
        >>> # Enable learning subsystems
        >>> agent.enable_reward_modulation(True)
        >>> agent.enable_curiosity(True)
        >>> agent.enable_development(True)
        >>> 
        >>> # Run agent episode
        >>> for step in range(100):
        ...     world.update(0.1)
        ...     agent.process_sensory_input(world.get_sensory_percept())
        ...     brain.step(step)
        ...     action = agent.decode_motor_command()
        ...     world.apply_motor_command(action, world.get_simulation_time())
    )pbdoc")
    .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
    .def("initialize", &AgentBrain::initialize, py::arg("world"),
         "Initialize with world")
    .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
         "Get expected sensory input size")
    .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
         "Get expected motor output size")
    .def("process_sensory_input", &AgentBrain::processSensoryInput,
         py::arg("percept"), "Process sensory percept and inject into brain")
    .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
         "Decode brain motor activity into motor command")
    .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
         py::arg("reward"), py::arg("predictedReward"),
         "Apply reward-based neuromodulation")
    .def("update_development", &AgentBrain::updateDevelopment, py::arg("timestep"),
         "Update development system")
    .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
         "Get current developmental stage")
    .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
         "Get current neuromodulation level")
    .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
         "Get curiosity level")
    .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
         "Get novelty level")
    .def("get_prediction_error", &AgentBrain::getPredictionError,
         "Get prediction error")
    .def("reset", &AgentBrain::reset, "Reset agent for new episode")
    .def("get_brain", &AgentBrain::getBrain,
         py::return_value_policy::reference_internal, "Get the underlying brain")
    .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
         py::arg("enable"), "Enable/disable reward modulation")
    .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
         py::arg("enable"), "Enable/disable structural plasticity")
    .def("enable_development", &AgentBrain::enableDevelopment,
         py::arg("enable"), "Enable/disable development")
    .def("enable_curiosity", &AgentBrain::enableCuriosity,
         py::arg("enable"), "Enable/disable curiosity")
    .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
         "Check if reward modulation is enabled")
    .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
         "Check if structural plasticity is enabled")
    .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
         "Check if development is enabled")
    .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
         "Check if curiosity is enabled")
    // Pythonic convenience methods
    .def("get_stats", [](const AgentBrain& agent) {
        py::dict stats;
        stats["curiosity"] = agent.getCuriosityLevel();
        stats["novelty"] = agent.getNoveltyLevel();
        stats["prediction_error"] = agent.getPredictionError();
        stats["neuromodulation"] = agent.getNeuromodulationLevel();
        stats["development_stage"] = static_cast<int>(agent.getDevelopmentalStage());
        return stats;
    }, "Get comprehensive agent statistics")
    .def("run_episode", [](AgentBrain& agent, SimpleWorld& world, int num_steps) {
        for (int step = 0; step < num_steps; ++step) {
            world.update(0.1);
            agent.processSensoryInput(world.getSensoryPercept());
            auto& brain = agent.getBrain();
            brain.step(step);
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime());
        }
    }, py::arg("world"), py::arg("num_steps"),
         "Run complete agent episode (convenience method)")
    .def("__repr__", [](const AgentBrain& agent) {
        return "<AgentBrain curiosity=" + std::to_string(agent.getCuriosityLevel()) +
               " novelty=" + std::to_string(agent.getNoveltyLevel()) + ">";
    });

// Factory functions
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

// Convenience factory functions
m.def("quick_brain", [](int neuron_count = 1000) {
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", neuron_count);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}, py::arg("neuron_count") = 1000,
     "Quick creation of a brain with specified neuron count");

m.def("quick_agent", [](int neuron_count = 1000, int world_width = 20, int world_height = 20) {
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", neuron_count);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(world_width, world_height, 8, 8);
    world->reset();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    return std::make_tuple(brain, world, agent);
}, py::arg("neuron_count") = 1000, py::arg("world_width") = 20, py::arg("world_height") = 20,
     "Quick creation of agent with brain, world, and interface");

// Global constants
m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

// Module metadata
m.doc() = R"pbdoc(
    NLM (Neural Learning Machine) Enhanced Python Bindings
    ------------------------------------------------------
    
    Enhanced Python bindings for the NLM C++ neural simulation framework.
    
    Key Features:
    - Pythonic API with properties and convenience methods
    - Enhanced error handling and input validation
    - Context manager support for safe resource management
    - Comprehensive documentation and examples
    - Factory functions for quick setup
    - Batch processing methods for performance
    - Advanced type safety and validation
    
    The module provides classes for:
    - Brain: Central neural network
    - AgentBrain: Interface between brain and world
    - SimpleWorld: 2D simulation world
    - Config: Configuration system
    - SensoryInput: Sensory data (Vision, Audio, InternalSignals)
    - Action: Motor actions
    - WorldObject: Objects in the world
    - AgentBody: Agent physical state
    - ActionResult: Action outcomes
    - SensoryPercept: Combined sensory data
    - Various ID types for entities (NeuronId, SynapseId, etc.)
    
    See also:
    - easy_usage.md: Quick start guide
    - HOW_TO_USE.md: Comprehensive documentation
    - pynlm_example.py: Example scripts
    )pbdoc";

// Version information
m.attr("__version__") = "0.1.0";
m.attr("__author__") = "NLM Project";
m.attr("__license__") = "MIT";

// Exception registration
py::register_exception<ValidationError>(m, "ValidationError");
py::register_exception<ConfigurationError>(m, "ConfigurationError");
py::register_exception<SimulationError>(m, "SimulationError");

} // namespace nlm
