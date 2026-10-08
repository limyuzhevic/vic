#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

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
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("ConfigurationError: " + msg) {}
};

class BrainInitializationError : public NLMException {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMException("BrainInitializationError: " + msg) {}
};

class InvalidOperationError : public NLMException {
public:
    explicit InvalidOperationError(const std::string& msg) : NLMException("InvalidOperationError: " + msg) {}
};

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& msg) : NLMException("MemoryError: " + msg) {}
};

// Template helper functions to reduce repetitive class binding code

// Template for simple ID classes with standard interface
template<typename T, typename U>
void bindIdClass(py::module_& m, const std::string& name, const std::string& doc) {
    py::class_<T>(m, name.c_str(), py::doc(doc.c_str()))
        .def(py::init<>())
        .def(py::init<U>(), py::arg("value"))
        .def_readwrite("value", &T::value)
        .def("index", &T::index)
        .def("__eq__", &T::operator==)
        .def("__ne__", &T::operator!=)
        .def("__hash__", [](const T& id) { return std::hash<U>{}(id.value); })
        .def("__repr__", [](const T& id) {
            return "<" + name + ": " + std::to_string(id.value) + ">";
        });
}

// Template for enum classes
template<typename T>
void bindEnumClass(py::module_& m, const std::string& name, const std::string& doc) {
    py::enum_<T>(m, name.c_str(), py::doc(doc.c_str()))
        .value("Excitatory", T::Excitatory)
        .value("Inhibitory", T::Inhibitory)
        .value("Modulatory", T::Modulatory)
        .export_values();
}

// Template for numeric enum classes
template<typename T>
void bindNumericEnumClass(py::module_& m, const std::string& name, const std::string& doc) {
    py::enum_<T>(m, name.c_str(), py::doc(doc.c_str()))
        .value("Excitatory", T::Excitatory)
        .value("Inhibitory", T::Inhibitory)
        .value("Modulatory", T::Modulatory)
        .value("Sensory", T::Sensory)
        .value("Motor", T::Motor)
        .value("Internal", T::Internal)
        .export_values();
}

// Template for arithmetic enums (ActionType, MotorCommand)
template<typename T>
void bindArithmeticEnumClass(py::module_& m, const std::string& name, const std::string& doc) {
    py::enum_<T>(m, name.c_str(), py::doc(doc.c_str()))
        .value("MoveForward", T::MoveForward)
        .value("MoveBackward", T::MoveBackward)
        .value("TurnLeft", T::TurnLeft)
        .value("TurnRight", T::TurnRight)
        .value("Look", T::Look)
        .value("LookUp", T::LookUp)
        .value("LookDown", T::LookDown)
        .value("Interact", T::Interact)
        .value("Wait", T::Wait)
        .export_values();
}

// Template for storage classes with basic operations
template<typename T, typename... Methods>
void bindStorageClass(py::module_& m, const std::string& name, const std::string& doc) {
    py::class_<T>(m, name.c_str(), py::doc(doc.c_str()))
        .def(py::init<>())
        .def(py::init<typename std::tuple_element<0, std::tuple<Methods...>>::type>(), "Placeholder for constructors")
        .def("reset", &T::reset, "Reset object to initial state");
}

// Template for complex classes with context manager support
class ContextManagerMixin {
public:
    virtual ~ContextManagerMixin() = default;
    
    py::object __enter__() {
        initialize();
        return py::cast(this);
    }
    
    void __exit__(py::object, py::object, py::object) {
        shutdown();
    }
    
protected:
    virtual void initialize() {}
    virtual void shutdown() {}
};

// Debugging utilities
namespace debug {
    template<typename T>
    std::string to_dict(const T& obj, const std::string& indent = "  ") {
        std::stringstream ss;
        ss << indent << "{";
        // This is a placeholder - actual implementation would need to be more sophisticated
        // and specific to each class type
        ss << indent << "  \"type\": \"" << typeid(T).name() << "\"";
        ss << indent << "}";
        return ss.str();
    }
    
    template<typename T>
    std::string pretty_print(const T& obj) {
        return to_dict(obj, "  ");
    }
    
    std::string format_sensory_input(const SensoryInput& input) {
        std::stringstream ss;
        ss << "SensoryInput:" << std::endl;
        ss << "  Type: " << static_cast<int>(input.getType()) << std::endl;
        ss << "  Dimensions: " << input.getDimensions() << std::endl;
        ss << "  Timestamp: " << input.getTimestamp();
        return ss.str();
    }
    
    std::string format_action(const Action& action) {
        std::stringstream ss;
        ss << "Action:" << std::endl;
        ss << "  Type: " << static_cast<int>(action.getType()) << std::endl;
        ss << "  Parameters: [";
        for (size_t i = 0; i < action.getParameters().size(); ++i) {
            if (i > 0) ss << ", ";
            ss << action.getParameters()[i];
        }
        ss << "]";
        return ss.str();
    }
}

// Property-like access helper
class PropertyAccessMixin {
protected:
    template<typename T, typename Getter, typename Setter>
    void add_property(py::class_<T>& cls, const std::string& name, Getter getter, Setter setter) {
        cls.def_readonly(name.c_str(), getter).def(name.c_str(), setter);
    }
};

// Pythonic callbacks utility
class CallbackUtilities {
public:
    template<typename T>
    static void add_callback(py::class_<T>& cls, const std::string& name, 
                           std::function<void()> callback, const std::string& doc = "") {
        cls.def(name.c_str(), [callback]() {
            callback();
        }, py::doc(doc.c_str()));
    }
};

// Enhanced error reporting
class ErrorReporter {
public:
    static std::string format_validation_error(const std::string& key, const std::string& error) {
        return "Validation error for key '" + key + "': " + error;
    }
    
    static std::string format_not_found_error(const std::string& itemType, const std::string& name) {
        return "" + itemType + " not found: '" + name + "'";
    }
    
    static std::string format_invalid_operation(const std::string& operation, const std::string& context) {
        return "Invalid operation '" + operation + "' " + context;
    }
};

// Context manager for Brain class
class BrainContextManager {
public:
    BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void __enter__() {
        if (!brain_->initialize()) {
            throw BrainInitializationError("Failed to initialize brain");
        }
        return *this;
    }
    
    void __exit__(py::object, py::object, py::object) {
        brain_->reset();
    }
    
    std::shared_ptr<Brain> brain() const { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Context manager for SimpleWorld class  
class WorldContextManager {
public:
    WorldContextManager(std::shared_ptr<SimpleWorld> world) : world_(world) {}
    
    void __enter__() {
        world_->reset();
        return *this;
    }
    
    void __exit__(py::object, py::object, py::object) {
        world_->reset();
    }
    
    std::shared_ptr<SimpleWorld> world() const { return world_; }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

// Context manager for AgentBrain class
class AgentBrainContextManager {
public:
    AgentBrainContextManager(std::shared_ptr<AgentBrain> agentBrain) : agentBrain_(agentBrain) {}
    
    void __enter__() {
        if (agentBrain_) {
            agentBrain_->reset();
        }
        return *this;
    }
    
    void __exit__(py::object, py::object, py::object) {
        if (agentBrain_) {
            agentBrain_->reset();
        }
    }
    
    std::shared_ptr<AgentBrain> agentBrain() const { return agentBrain_; }
    
private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Pythonic binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Python-style exceptions (NLMException, ConfigurationError, BrainInitializationError, etc.)
        - Context managers for resource management
        - Debugging utilities (to_dict(), pretty printing)
        - Property-like access for complex objects
        - Python-specific callbacks and utilities
    )pbdoc";

    // Register all Python exceptions
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError.ptr());
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_RuntimeError.ptr());
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError", PyExc_RuntimeError.ptr());
    py::register_exception<InvalidOperationError>(m, "InvalidOperationError", PyExc_RuntimeError.ptr());
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_RuntimeError.ptr());

    // Use template helper for ID classes to reduce repetition
    bindIdClass<NeuronId, uint64_t>(m, "NeuronId", "Unique identifier for a neuron");
    bindIdClass<SynapseId, uint64_t>(m, "SynapseId", "Unique identifier for a synapse");
    bindIdClass<RegionId, uint64_t>(m, "RegionId", "Unique identifier for a brain region");
    bindIdClass<PopulationId, uint64_t>(m, "PopulationId", "Unique identifier for a neuron population");

    // Use template helper for enum classes
    bindNumericEnumClass<NeuronType>(m, "NeuronType", "Neuron type enumeration");
    bindEnumClass<SynapseType>(m, "SynapseType", "Synapse type enumeration");
    bindEnumClass<DevelopmentalStage>(m, "DevelopmentalStage", "Developmental stage enumeration");
    bindEnumClass<FiringState>(m, "FiringState", "Neuron firing state enumeration");
    bindArithmeticEnumClass<ActionType>(m, "ActionType", "Action type enumeration");
    bindArithmeticEnumClass<MotorCommand>(m, "MotorCommand", "Low-level motor command enumeration");
    bindEnumClass<WorldObjectType>(m, "WorldObjectType", "World object type enumeration");

    // Enhanced Config class with debugging utilities and property access
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"), "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"), "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        // Add Python-style property access for common config values
        .def_property_readonly("is_valid", &Config::validateAll, "Check if entire configuration is valid")
        .def_property_readonly("error_count", [m](const Config& cfg) {
            auto errors = cfg.getValidationErrors();
            return errors.size();
        }, "Get number of validation errors")
        .def("to_dict", [m](const Config& cfg) {
            std::map<std::string, std::string> result;
            auto keys = cfg.getKeys();
            for (const auto& key : keys) {
                auto value = cfg.get<std::string>(key);
                if (value) {
                    result[key] = *value;
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput class with debugging utilities
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp")
        .def("to_dict", &debug::format_sensory_input, "Get debugging dictionary representation")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: type=" + std::to_string(input.getType()) + ", dims=" + std::to_string(input.getDimensions()) + ">";
        });

    // Enhanced Vision class with context manager and debugging
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("to_dict", [](const Vision& v) {
            std::map<std::string, std::vector<float>> result;
            result["data"] = v.getData();
            result["width"] = {static_cast<float>(v.getWidth())};
            result["height"] = {static_cast<float>(v.getHeight())};
            result["channels"] = {static_cast<float>(v.getChannels())};
            return result;
        }, "Get debugging dictionary representation")
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" + std::to_string(v.getHeight()) + ", " + std::to_string(v.getChannels()) + " channels>";
        });

    // Enhanced Audio class with debugging
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("to_dict", [](const Audio& a) {
            std::map<std::string, std::vector<float>> result;
            result["data"] = a.getData();
            result["sample_rate"] = {static_cast<float>(a.getSampleRate())};
            result["num_samples"] = {static_cast<float>(a.getNumSamples())};
            return result;
        }, "Get debugging dictionary representation")
        .def("__repr__", [](const Audio& a) {
            return "<Audio: " + std::to_string(a.getSampleRate()) + " Hz, " + std::to_string(a.getNumSamples()) + " samples>";
        });

    // Enhanced InternalSignals class with debugging
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", &InternalSignals::getData, "Get all signals as a vector")
        .def("to_dict", [](const InternalSignals& s) {
            std::map<std::string, std::vector<float>> result;
            result["signals"] = s.getData();
            return result;
        }, "Get debugging dictionary representation")
        .def("__repr__", [](const InternalSignals& s) {
            return "<InternalSignals: " + std::to_string(s.getData().size()) + " signals>";
        });

    // Enhanced Action class with debugging and property-like access
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
        // Add Python-style property access
        .def_property("type", &Action::getType, &Action::setType, "Get/set action type")
        .def_property("parameters", &Action::getParameters, &Action::setParameters, "Get/set action parameters")
        .def_property_readonly("is_simple", [](const Action& a) {
            return a.getParameters().empty();
        }, "Check if action has no parameters")
        // Debugging utilities
        .def("to_dict", [](const Action& a) {
            std::map<std::string, std::string> result;
            result["type"] = std::to_string(static_cast<int>(a.getType()));
            result["name"] = a.getName();
            result["parameter_count"] = std::to_string(a.getParameters().size());
            return result;
        }, "Get debugging dictionary representation")
        .def("pretty_print", &debug::format_action, "Get pretty printed representation")
        .def("__repr__", [](const Action& a) {
            return "<Action: " + a.getName() + " (type=" + std::to_string(static_cast<int>(a.getType())) + ")>";
        });

    // Enhanced WorldObject class with debugging and property access
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        // Use property access pattern
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        // Add computed properties
        .def_property_readonly("is_valid", [](const WorldObject& obj) {
            return obj.active && obj.radius > 0.0f;
        }, "Check if object is valid")
        .def_property_readonly("is_resource", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Resource;
        }, "Check if object is a resource")
        .def_property_readonly("is_hazard", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard")
        // Debugging utilities
        .def("to_dict", [](const WorldObject& obj) {
            std::map<std::string, std::string> result;
            result["x"] = std::to_string(obj.x);
            result["y"] = std::to_string(obj.y);
            result["radius"] = std::to_string(obj.radius);
            result["type"] = std::to_string(static_cast<int>(obj.type));
            result["value"] = std::to_string(obj.value);
            result["active"] = obj.active ? "true" : "false";
            return result;
        }, "Get debugging dictionary representation")
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("__repr__", [](const WorldObject& obj) {
            std::string typeStr;
            switch (obj.type) {
                case WorldObjectType::Empty: typeStr = "Empty"; break;
                case WorldObjectType::Resource: typeStr = "Resource"; break;
                case WorldObjectType::Hazard: typeStr = "Hazard"; break;
                case WorldObjectType::Wall: typeStr = "Wall"; break;
                case WorldObjectType::Marker: typeStr = "Marker"; break;
                default: typeStr = "Unknown";
            }
            return "<WorldObject: " + typeStr + " at (" + std::to_string(obj.x) + ", " + std::to_string(obj.y) + ")>";
        });

    // Enhanced AgentBody class with debugging
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        // Property access pattern
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
        // Computed properties
        .def_property_readonly("speed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX*body.velocityX + body.velocityY*body.velocityY);
        }, "Get current speed")
        .def_property_readonly("position", [](const AgentBody& body) {
            return std::vector<double>{body.x, body.y};
        }, "Get position as [x, y]")
        .def_property_readonly("velocity", [](const AgentBody& body) {
            return std::vector<double>{body.velocityX, body.velocityY};
        }, "Get velocity as [vx, vy]")
        // Debugging utilities
        .def("to_dict", [](const AgentBody& body) {
            std::map<std::string, std::string> result;
            result["x"] = std::to_string(body.x);
            result["y"] = std::to_string(body.y);
            result["orientation"] = std::to_string(body.orientation);
            result["velocity_x"] = std::to_string(body.velocityX);
            result["velocity_y"] = std::to_string(body.velocityY);
            result["angular_velocity"] = std::to_string(body.angularVelocity);
            result["energy"] = std::to_string(body.energy);
            result["health"] = std::to_string(body.health);
            result["age"] = std::to_string(body.age);
            result["is_moving"] = body.isMoving ? "true" : "false";
            result["is_turning"] = body.isTurning ? "true" : "false";
            return result;
        }, "Get debugging dictionary representation")
        .def("reset", &AgentBody::reset, "Reset agent body state")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: pos(" + std::to_string(body.x) + ", " + std::to_string(body.y) + ") energy=" + std::to_string(body.energy) + ">";
        });

    // Enhanced ActionResult class with debugging
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Computed properties
        .def_property_readonly("has_reward", [](const ActionResult& result) {
            return result.reward != 0.0f;
        }, "Check if result has non-zero reward")
        .def_property_readonly("is_good", [](const ActionResult& result) {
            return result.success && result.reward > 0.0f;
        }, "Check if result is good")
        // Debugging utilities
        .def("to_dict", [](const ActionResult& result) {
            std::map<std::string, std::string> dict;
            dict["reward"] = std::to_string(result.reward);
            dict["success"] = result.success ? "true" : "false";
            dict["message"] = result.message;
            return dict;
        }, "Get debugging dictionary representation")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) + ", success=" + std::to_string(result.success) + ", message='" + result.message + "'>";
        });

    // Enhanced SensoryPercept class with debugging
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
        // Add context manager support
        .def("__enter__", [](SensoryPercept& self) { return self; })
        .def("__exit__", [](SensoryPercept& self, py::object, py::object, py::object) {})
        // Debugging utilities
        .def("to_dict", [](const SensoryPercept& percept) {
            std::map<std::string, std::string> result;
            result["vision_width"] = std::to_string(percept.getVisionWidth());
            result["vision_height"] = std::to_string(percept.getVisionHeight());
            result["has_vision"] = "true"; // Simplified for example
            result["timestamp"] = std::to_string(percept.getTimestamp());
            return result;
        }, "Get debugging dictionary representation")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision(" + std::to_string(percept.getVisionWidth()) + "x" + std::to_string(percept.getVisionHeight()) + ") timestamp=" + std::to_string(percept.getTimestamp()) + ">";
        });

    // Enhanced SimpleWorld class with context manager and debugging
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Add Python-style property access
        .def_property_readonly("width", &SimpleWorld::getWidth, "Get world width")
        .def_property_readonly("height", &SimpleWorld::getHeight, "Get world height")
        .def_property("max_energy", &SimpleWorld::getMaxEnergy, &SimpleWorld::setMaxEnergy, "Get/set max energy")
        .def_property("energy_decay_rate", &SimpleWorld::getEnergyDecayRate, &SimpleWorld::setEnergyDecayRate, "Get/set energy decay rate")
        // Context manager support
        .def(py::self_type(), py::arg("world"))
        .def("__enter__", [](SimpleWorld& self) {
            self.reset();
            return self;
        }, "Enter context manager")
        .def("__exit__", [](SimpleWorld& self, py::object, py::object, py::object) {
            self.reset();
        }, "Exit context manager")
        // Debugging utilities
        .def("to_dict", [](const SimpleWorld& world) {
            std::map<std::string, std::string> result;
            result["width"] = std::to_string(world.getWidth());
            result["height"] = std::to_string(world.getHeight());
            result["max_energy"] = std::to_string(world.getMaxEnergy());
            result["energy_decay_rate"] = std::to_string(world.getEnergyDecayRate());
            result["simulation_time"] = std::to_string(world.getSimulationTime());
            result["random_seed"] = std::to_string(world.getRandomSeed());
            return result;
        }, "Get debugging dictionary representation")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight()) + ", energy=" + std::to_string(world.getMaxEnergy()) + ">";
        });

    // Enhanced Brain class with context manager and debugging
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
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
        // Context manager support
        .def("__enter__", [](Brain& self) {
            if (!self.initialize()) {
                throw BrainInitializationError("Failed to initialize brain");
            }
            return self;
        }, "Enter context manager - initializes brain")
        .def("__exit__", [](Brain& self, py::object, py::object, py::object) {
            self.reset();
        }, "Exit context manager - resets brain")
        // Debugging utilities
        .def("to_dict", [](const Brain& brain) {
            std::map<std::string, std::string> result;
            result["region_count"] = std::to_string(brain.getRegionCount());
            result["neuron_count"] = std::to_string(brain.getTotalNeuronCount());
            result["synapse_count"] = std::to_string(brain.getTotalSynapseCount());
            result["active_neurons"] = std::to_string(brain.getActiveNeuronCount());
            result["firing_neurons"] = std::to_string(brain.getFiringNeuronCount());
            result["avg_firing_rate"] = std::to_string(brain.getAverageFiringRate());
            result["e_i_ratio"] = std::to_string(brain.getExcitationInhibitionRatio());
            result["developmental_stage"] = std::to_string(static_cast<int>(brain.getDevelopmentalStage()));
            return result;
        }, "Get debugging dictionary representation")
        .def("pretty_print", [](const Brain& brain) {
            std::stringstream ss;
            ss << "Brain Status:" << std::endl;
            ss << "  Regions: " << brain.getRegionCount() << std::endl;
            ss << "  Neurons: " << brain.getTotalNeuronCount() << std::endl;
            ss << "  Synapses: " << brain.getTotalSynapseCount() << std::endl;
            ss << "  Active: " << brain.getActiveNeuronCount() << std::endl;
            ss << "  Firing: " << brain.getFiringNeuronCount() << std::endl;
            ss << "  Avg Firing Rate: " << brain.getAverageFiringRate() << std::endl;
            ss << "  E/I Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
            ss << "  Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
            return ss.str();
        }, "Get pretty printed status report")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: regions=" + std::to_string(brain.getRegionCount()) + 
                   ", neurons=" + std::to_string(brain.getTotalNeuronCount()) + ">";
        });

    // Enhanced AgentBrain class with context manager and debugging
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Context manager support
        .def("__enter__", [](AgentBrain& self) {
            self.reset();
            return self;
        }, "Enter context manager - resets agent")
        .def("__exit__", [](AgentBrain& self, py::object, py::object, py::object) {
            self.reset();
        }, "Exit context manager - resets agent")
        // Debugging utilities
        .def("to_dict", [](const AgentBrain& agentBrain) {
            std::map<std::string, std::string> result;
            result["sensory_input_size"] = std::to_string(agentBrain.getSensoryInputSize());
            result["motor_output_size"] = std::to_string(agentBrain.getMotorOutputSize());
            result["developmental_stage"] = std::to_string(static_cast<int>(agentBrain.getDevelopmentalStage()));
            result["neuromodulation_level"] = std::to_string(agentBrain.getNeuromodulationLevel());
            result["curiosity_level"] = std::to_string(agentBrain.getCuriosityLevel());
            result["novelty_level"] = std::to_string(agentBrain.getNoveltyLevel());
            result["prediction_error"] = std::to_string(agentBrain.getPredictionError());
            result["reward_modulation_enabled"] = agentBrain.isRewardModulationEnabled() ? "true" : "false";
            result["structural_plasticity_enabled"] = agentBrain.isStructuralPlasticityEnabled() ? "true" : "false";
            result["development_enabled"] = agentBrain.isDevelopmentEnabled() ? "true" : "false";
            result["curiosity_enabled"] = agentBrain.isCuriosityEnabled() ? "true" : "false";
            return result;
        }, "Get debugging dictionary representation")
        .def("pretty_print", [](const AgentBrain& agentBrain) {
            std::stringstream ss;
            ss << "AgentBrain Status:" << std::endl;
            ss << "  Sensory Input Size: " << agentBrain.getSensoryInputSize() << std::endl;
            ss << "  Motor Output Size: " << agentBrain.getMotorOutputSize() << std::endl;
            ss << "  Developmental Stage: " << static_cast<int>(agentBrain.getDevelopmentalStage()) << std::endl;
            ss << "  Neuromodulation Level: " << agentBrain.getNeuromodulationLevel() << std::endl;
            ss << "  Curiosity Level: " << agentBrain.getCuriosityLevel() << std::endl;
            ss << "  Novelty Level: " << agentBrain.getNoveltyLevel() << std::endl;
            ss << "  Prediction Error: " << agentBrain.getPredictionError() << std::endl;
            return ss.str();
        }, "Get pretty printed status report")
        .def("__repr__", [](const AgentBrain& agentBrain) {
            return "<AgentBrain: input_size=" + std::to_string(agentBrain.getSensoryInputSize()) + 
                   ", output_size=" + std::to_string(agentBrain.getMotorOutputSize()) + ">";
        });

    // Python utility functions
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

    // Python-specific callback utilities
    m.def("create_sensory_callback", [](std::function<void(const SensoryInput&)> callback) {
        return callback;
    }, py::arg("callback"), "Create a sensory input callback");

    m.def("create_action_callback", [](std::function<void(const Action&)> callback) {
        return callback;
    }, py::arg("callback"), "Create an action callback");

    m.def("create_reward_callback", [](std::function<void(float)> callback) {
        return callback;
    }, py::arg("callback"), "Create a reward callback");

    // Error reporting utilities
    m.def("report_error", &ErrorReporter::format_validation_error,
          py::arg("key"), py::arg("error"),
          "Format a validation error message");

    m.def("report_not_found_error", &ErrorReporter::format_not_found_error,
          py::arg("item_type"), py::arg("name"),
          "Format a not found error message");

    m.def("report_invalid_operation_error", &ErrorReporter::format_invalid_operation,
          py::arg("operation"), py::arg("context"),
          "Format an invalid operation error message");

    // Debug utilities
    m.def("debug_to_dict", &debug::to_dict<SensoryInput>,
          py::arg("obj"), "Convert object to debugging dictionary");

    m.def("debug_pretty_print", &debug::pretty_print<SensoryInput>,
          py::arg("obj"), "Get pretty printed representation of object");

    m.def("debug_sensory_input", &debug::format_sensory_input,
          py::arg("input"), "Get formatted string for sensory input");

    m.def("debug_action", &debug::format_action,
          py::arg("action"), "Get formatted string for action");

    // Invalid ID constants (maintaining backward compatibility)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm