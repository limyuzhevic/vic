// Enhanced NLM Python Bindings Implementation

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

// Include the base NLM headers
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

// Enhanced Exception Classes
class PyNLMError : public std::runtime_error {
public:
    explicit PyNLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class PyValidationError : public PyNLMError {
public:
    explicit PyValidationError(const std::string& msg) : PyNLMError("Validation Error: " + msg) {}
};

class PyConfigurationError : public PyNLMError {
public:
    explicit PyConfigurationError(const std::string& msg) : PyNLMError("Configuration Error: " + msg) {}
};

// Utility namespace for Python bindings
namespace utils {

// Helper function to validate sensory input data
def validateSensoryData(const std::string& type, const std::vector<float>& data, 
                        size_t expectedWidth, size_t expectedHeight, size_t expectedChannels) {
    if (data.empty()) {
        throw PyValidationError(type + " data cannot be empty");
    }
    
    size_t expectedSize = expectedWidth * expectedHeight * expectedChannels;
    if (data.size() != expectedSize) {
        throw PyValidationError(type + " data size mismatch: expected " + 
                               std::to_string(expectedSize) + " elements, got " + 
                               std::to_string(data.size()));
    }
}

// Helper function to convert Python list to vector
def convertToVector(const py::list& list) {
    std::vector<float> result;
    result.reserve(list.size());
    for (auto it : list) {
        result.push_back(it.cast<float>());
    }
    return result;
}

// Helper function to clamp values
def clamp(float value, float min, float max) {
    return std::max(min, std::min(value, max));
}

} // namespace utils

// VisionHelper class for enhanced vision processing
class VisionHelper {
public:
    VisionHelper() = default;
    
    // Create vision from Python list of lists
    static py::object createVisionFromPyList(const py::list& image, size_t channels = 3) {
        size_t height = image.size();
        if (height == 0) {
            throw PyValidationError("Image cannot be empty");
        }
        size_t width = image[0].cast<py::list>().size();
        
        std::vector<float> flatData;
        flatData.reserve(width * height * channels);
        
        for (py::ssize_t row = 0; row < static_cast<py::ssize_t>(height); ++row) {
            py::list rowData = image[row].cast<py::list>();
            for (py::ssize_t col = 0; col < static_cast<py::ssize_t>(width); ++col) {
                py::list pixel = rowData[col].cast<py::list>();
                for (py::ssize_t c = 0; c < static_cast<py::ssize_t>(channels); ++c) {
                    float value = pixel[c].cast<float>();
                    value = utils::clamp(value, 0.0f, 1.0f);
                    flatData.push_back(value);
                }
            }
        }
        
        py::module_ vision_class = py::module::import("pynlm").attr("Vision");
        auto vision_obj = vision_class(width, height, channels);
        vision_obj.attr("setData")(flatData);
        return vision_obj;
    }
    
    // Create gradient vision
    static py::object createGradientVision(size_t width, size_t height, 
                                          float start = 0.0f, float end = 1.0f) {
        std::vector<float> gradient;
        gradient.reserve(width * height);
        
        float xStep = (end - start) / static_cast<float>(width - 1);
        
        for (size_t j = 0; j < height; ++j) {
            for (size_t i = 0; i < width; ++i) {
                gradient.push_back(start + static_cast<float>(i) * xStep);
            }
        }
        
        py::module_ vision_class = py::module::import("pynlm").attr("Vision");
        auto vision_obj = vision_class(width, height, 1); // Grayscale
        vision_obj.attr("setData")(gradient);
        return vision_obj;
    }
    
    // Convert vision to Python list
    static py::list visionToPyList(py::object vision) {
        py::list image;
        size_t width = vision.attr("getWidth"}().cast<size_t>();
        size_t height = vision.attr("getHeight"}().cast<size_t>();
        size_t channels = vision.attr("getChannels"}().cast<size_t>();
        
        std::vector<float> data = vision.attr("getData"}().cast<std::vector<float>>();
        
        for (size_t j = 0; j < height; ++j) {
            py::list row;
            for (size_t i = 0; i < width; ++i) {
                py::list pixel;
                for (size_t c = 0; c < channels; ++c) {
                    size_t index = (j * width + i) * channels + c;
                    pixel.append(data[index]);
                }
                row.append(pixel);
            }
            image.append(row);
        }
        
        return image;
    }
};

// AudioHelper class for enhanced audio processing
class AudioHelper {
public:
    AudioHelper() = default;
    
    // Create audio from Python list
    static py::object createAudioFromPyList(const py::list& audioData, size_t sampleRate = 44100) {
        py::module_ audio_class = py::module::import("pynlm").attr("Audio");
        auto audio_obj = audio_class(sampleRate, audioData.size());
        audio_obj.attr("setData")(utils::convertToVector(audioData));
        audio_obj.attr("setSampleRate")(sampleRate);
        return audio_obj;
    }
    
    // Convert audio to Python list
    static py::list audioToPyList(py::object audio) {
        size_t sampleRate = audio.attr("getSampleRate"}().cast<size_t>();
        size_t numSamples = audio.attr("getNumSamples"}().cast<size_t>();
        
        std::vector<float> data = audio.attr("getData"}().cast<std::vector<float>>();
        
        py::list result;
        result.append(sampleRate);
        for (size_t i = 0; i < numSamples; ++i) {
            result.append(data[i]);
        }
        
        return result;
    }
};

// SimulationUtils class for enhanced simulation management
class SimulationUtils {
public:
    SimulationUtils() = default;
    
    // Create optimized agent configuration
    static py::object createAgentConfig(const std::string& configName = "default_agent") {
        py::module_ config_class = py::module::import("pynlm").attr("Config");
        auto config_obj = config_class();
        
        // Set default values
        config_obj.attr("set")(std::string("agent.name"), configName);
        config_obj.attr("set")(std::string("agent.sensory.input_size"), 262);
        config_obj.attr("set")(std::string("agent.motor.output_size"), 6);
        config_obj.attr("set")(std::string("agent.max_energy"), 100.0);
        config_obj.attr("set")(std::string("agent.energy_decay_rate"), 0.1);
        config_obj.attr("set")(std::string("agent.development.enabled"), true);
        config_obj.attr("set")(std::string("agent.reward.modulation"), true);
        config_obj.attr("set")(std::string("agent.curiosity.enabled"), true);
        
        return config_obj;
    }
    
    // Create complete simulation setup
    static py::tuple createSimulationSetup(size_t worldWidth = 100.0f, 
                                          size_t worldHeight = 100.0f,
                                          size_t visionWidth = 16,
                                          size_t visionHeight = 16) {
        py::module_ world_class = py::module::import("pynlm").attr("SimpleWorld");
        py::module_ brain_class = py::module::import("pynlm").attr("Brain");
        py::module_ agent_brain_class = py::module::import("pynlm").attr("AgentBrain");
        
        // Create world
        auto world_obj = world_class();
        world_obj.attr("configure")(worldWidth, worldHeight, visionWidth, visionHeight);
        
        // Create brain
        auto brain_obj = brain_class();
        brain_obj.attr("initialize"})();
        
        // Create agent brain
        auto agent_brain_obj = agent_brain_class(brain_obj);
        agent_brain_obj.attr("initialize")(world_obj);
        
        // Create config
        auto config_obj = createAgentConfig();
        
        return py::make_tuple(world_obj, brain_obj, agent_brain_obj, config_obj);
    }
    
    // Safe save function
    static bool safeSave(py::object obj, const std::string& filepath) {
        try {
            if (obj.is_none()) {
                throw PyValidationError("Object cannot be None");
            }
            
            if (!obj.hasattr("save")) {
                throw PyConfigurationError("Object does not support save operation");
            }
            
            return obj.attr("save")(filepath).cast<bool>();
        } catch (const std::exception& e) {
            std::cerr << "Error saving to " << filepath << ": " << e.what() << std::endl;
            return false;
        }
    }
    
    // Safe load function
    static py::object safeLoad(const std::string& filepath, 
                             const std::string& objectType = "brain") {
        try {
            if (objectType == "brain") {
                py::module_ brain_class = py::module::import("pynlm").attr("Brain");
                py::object brain_obj = brain_class();
                bool success = brain_obj.attr("load")(filepath).cast<bool>();
                if (!success) {
                    throw PyConfigurationError("Failed to load brain from " + filepath);
                }
                return brain_obj;
            } else if (objectType == "config") {
                py::module_ config_class = py::module::import("pynlm").attr("Config");
                py::object config_obj = config_class();
                bool success = config_obj.attr("loadFromFile")(filepath).cast<bool>();
                if (!success) {
                    throw PyConfigurationError("Failed to load config from " + filepath);
                }
                return config_obj;
            } else {
                throw PyValidationError("Unknown object type: " + objectType);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error loading from " << filepath << ": " << e.what() << std::endl;
            return py::none();
        }
    }
};

// SimulationContext class for safe simulation management
class SimulationContext {
public:
    SimulationContext(py::object brain, size_t maxSteps = 1000) 
        : brain_(brain), maxSteps_(maxSteps), currentStep_(0) {
        if (brain_.is_none()) {
            throw PyValidationError("Brain object cannot be None");
        }
    }
    
    ~SimulationContext() {
        // Ensure brain is reset on destruction
        if (!brain_.is_none() && brain_.hasattr("reset")) {
            brain_.attr("reset")();
        }
    }
    
    // Enter context
    SimulationContext enter() {
        return *this;
    }
    
    // Exit context
    void exit(py::object type, py::object value, py::object traceback) {
        // Reset brain on exit
        if (!brain_.is_none() && brain_.hasattr("reset")) {
            brain_.attr("reset")();
        }
    }
    
    // Run simulation steps
    py::dict runSteps(size_t steps) {
        if (steps == 0) {
            throw PyValidationError("Number of steps cannot be zero");
        }
        
        py::dict results;
        std::vector<float> firingRates;
        
        for (size_t i = 0; i < steps && currentStep_ < maxSteps_; ++i, ++currentStep_) {
            brain_.attr("step")(currentStep_);
            
            // Collect statistics
            if (brain_.hasattr("getAverageFiringRate")) {
                firingRates.push_back(brain_.attr("getAverageFiringRate"}().cast<float>());
            }
            
            // Check for early termination
            if (brain_.hasattr("getActiveNeuronCount")) {
                size_t activeCount = brain_.attr("getActiveNeuronCount"}().cast<size_t>();
                if (activeCount == 0) {
                    // Brain went silent, stop simulation
                    results["early_stop"] = "Brain went silent";
                    break;
                }
            }
        }
        
        // Calculate statistics
        if (!firingRates.empty()) {
            float sum = std::accumulate(firingRates.begin(), firingRates.end(), 0.0f);
            float avg = sum / static_cast<float>(firingRates.size());
            results["average_firing_rate"] = avg;
            results["min_firing_rate"] = *std::min_element(firingRates.begin(), firingRates.end());
            results["max_firing_rate"] = *std::max_element(firingRates.begin(), firingRates.end());
        }
        
        results["steps_completed"] = currentStep_;
        results["completed"] = true;
        
        return results;
    }
    
    // Get current simulation time
    double getCurrentTime() {
        if (brain_.hasattr("getSimulationTime")) {
            return brain_.attr("getSimulationTime"}().cast<double>();
        }
        return 0.0;
    }
    
    // Get current step
    size_t getCurrentStep() const {
        return currentStep_;
    }
    
    // Get brain reference
    py::object getBrain() {
        return brain_;
    }

private:
    py::object brain_;
    size_t maxSteps_;
    size_t currentStep_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        This version includes comprehensive enhancements:
        - Advanced error handling and validation
        - Pythonic APIs with context managers
        - Utility classes for vision/audio processing
        - Safe save/load operations with error handling
        - Statistical analysis and monitoring tools
        - Backward compatibility with original APIs
    )pbdoc";

    // Register exceptions
    py::register_exception<PyNLMError>(m, "NLMError");
    py::register_exception<PyValidationError>(m, "ValidationError");
    py::register_exception<PyConfigurationError>(m, "ConfigurationError");

    // Register basic types (unchanged from original)
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

    // Enums (unchanged from original)
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

    // Enhanced Config class with better Python interface
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
        .def("get", &Config::get<int>, py::arg("key"),
             "Get configuration value as int (returns None if not found)")
        .def("getOr", &Config::getOr<int>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default fallback")
        .def("set", py::overload_cast<const std::string&, const ConfigValue&>(&Config::set), 
             py::arg("key"), py::arg("value"),
             "Set configuration value (value can be int, float, string, bool, or list)")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration string value")
        .def("set", py::overload_cast<const std::string&, int>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration integer value")
        .def("set", py::overload_cast<const std::string&, double>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration float value")
        .def("set", py::overload_cast<const std::string&, bool>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration boolean value")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput base class with validation
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class with Pythonic interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            // Enhanced validation and error handling
            if (data.empty()) {
                throw PyValidationError("Vision data cannot be empty");
            }
            size_t expectedSize = self.getWidth() * self.getHeight() * self.getChannels();
            if (data.size() != expectedSize) {
                throw PyValidationError("Vision data size mismatch: expected " + 
                                       std::to_string(expectedSize) + " elements, got " + 
                                       std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"), "Set image data with validation")
        .def("setData", [](Vision& self, const py::list& image) {
            // Convert Python list to flat vector with validation
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            size_t channels = self.getChannels();
            
            std::vector<float> flatData;
            flatData.reserve(width * height * channels);
            
            for (py::ssize_t row = 0; row < static_cast<py::ssize_t>(height); ++row) {
                py::list rowData = image[row].cast<py::list>();
                for (py::ssize_t col = 0; col < static_cast<py::ssize_t>(width); ++col) {
                    py::list pixel = rowData[col].cast<py::list>();
                    for (py::ssize_t c = 0; c < static_cast<py::ssize_t>(channels); ++c) {
                        float value = pixel[c].cast<float>();
                        // Clamp values to [0, 1] range
                        value = std::max(0.0f, std::min(value, 1.0f));
                        flatData.push_back(value);
                    }
                }
            }
            
            self.setData(flatData);
        }, py::arg("image"), "Set image from Python list of lists with validation")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("to_list", [](Vision& self) {
            return VisionHelper::visionToPyList(py::cast(self));
        }, "Convert vision data to Python list of lists");

    // Enhanced Audio class with Pythonic interface
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", &Audio::setData, py::arg("data"), "Set audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals");

    // Enhanced Action class with validation
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // Enhanced WorldObject class with validation
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

    // Enhanced AgentBody with validation
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

    // Enhanced ActionResult
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Enhanced SensoryPercept
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal data")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // Enhanced SimpleWorld with validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at position (if any)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set max energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Enhanced Brain class with better Python interface
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

    // Enhanced AgentBrain class with better Python interface
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

    // Factory functions (enhanced with better error handling)
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with default agent settings");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create a new brain with configuration and initialize it");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100.0f, 100.0f, 16, 16); // Default world size
        return world;
    }, "Create a new simple world with default configuration");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Enhanced utility functions
    m.def("saveSafely", &SimulationUtils::safeSave,
          py::arg("obj"), py::arg("filepath"),
          "Save object to file with error handling and logging");

    m.def("loadSafely", &SimulationUtils::safeLoad,
          py::arg("filepath"), py::arg("objectType") = "brain",
          "Load object from file with error handling and logging");

    m.def("createAgentConfig", &SimulationUtils::createAgentConfig,
          py::arg("configName") = "default_agent",
          "Create a default agent configuration with optimized settings");

    m.def("createSimulationSetup", &SimulationUtils::createSimulationSetup,
          py::arg("worldWidth") = 100.0f, py::arg("worldHeight") = 100.0f,
          py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
          "Create a complete simulation setup with world, brain, and agent brain");

    // Vision and Audio helper utilities
    m.def("createVisionFromPyList", &VisionHelper::createVisionFromPyList,
          py::arg("image"), py::arg("channels") = 3,
          "Create vision from Python list of lists (image format)");

    m.def("createGradientVision", &VisionHelper::createGradientVision,
          py::arg("width"), py::arg("height"), py::arg("start") = 0.0f, py::arg("end") = 1.0f,
          "Create gradient vision for testing");

    m.def("createUniformVision", &VisionHelper::createUniformVision,
          py::arg("width"), py::arg("height"), py::arg("value") = 0.5f,
          "Create uniform color vision for testing");

    m.def("createAudioFromPyList", &AudioHelper::createAudioFromPyList,
          py::arg("audioData"), py::arg("sampleRate") = 44100,
          "Create audio from Python list");

    m.def("createSineWave", &AudioHelper::createSineWave,
          py::arg("numSamples"), py::arg("sampleRate"), py::arg("frequency") = 440.0f, py::arg("amplitude") = 1.0f,
          "Create sine wave audio for testing");

    // Context manager for simulation
    py::class_<SimulationContext>(m, "SimulationContext", R"pbdoc(Context manager for brain simulation)pbdoc")
        .def(py::init<py::object, size_t>(), py::arg("brain"), py::arg("max_steps") = 1000)
        .def("__enter__", &SimulationContext::enter)
        .def("__exit__", &SimulationContext::exit)
        .def("runSteps", &SimulationContext::runSteps, py::arg("steps"),
             "Run simulation for specified number of steps")
        .def("getCurrentTime", &SimulationContext::getCurrentTime,
             "Get current simulation time")
        .def("getCurrentStep", &SimulationContext::getCurrentStep,
             "Get current simulation step")
        .def("getBrain", &SimulationContext::getBrain,
             "Get brain reference");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Utility constants
    m.attr("PI") = 3.14159265358979323846;
    m.attr("E") = 2.71828182845904523536;
    
    // Version information
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__description__") = R"pbdoc(
        Enhanced NLM Python bindings with comprehensive error handling, 
        Pythonic APIs, utility classes, safe operations, and statistical analysis.
        This version provides context managers, input validation, helper classes
        for vision/audio processing, and advanced simulation utilities.
    )pbdoc";

} // namespace nlm
