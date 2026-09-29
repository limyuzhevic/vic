#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <unordered_map>
#include <variant>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "pynlm/exceptions.h"
#include "pynlm/type_converter.h"
#include "pynlm/pythonic_interfaces.h"

#include "../src/core/Exceptions/FileNotFoundException.hpp"
#include "../src/core/Exceptions/FilePermissionException.hpp"
#include "../src/core/Exceptions/FileFormatException.hpp"
#include "../src/core/Exceptions/FileWriteException.hpp"
#include "../src/core/Exceptions/InvalidDimensionsException.hpp"
#include "../src/core/Exceptions/InvalidRangeException.hpp"
#include "../src/core/Exceptions/InvalidFormatException.hpp"
#include "../src/core/Exceptions/NullOrEmptyException.hpp"
#include "../src/core/Exceptions/KeyNotFoundException.hpp"
#include "../src/core/Exceptions/TypeMismatchException.hpp"
#include "../src/core/Exceptions/NeuronNotFoundException.hpp"
#include "../src/core/Exceptions/InvalidCurrentException.hpp"
#include "../src/core/Exceptions/CurrentOutOfRangeException.hpp"
#include "../src/core/Exceptions/PositionOutOfBoundsException.hpp"
#include "../src/core/Exceptions/InvalidCommandException.hpp"
#include "../src/core/Exceptions/InvalidDataSizeException.hpp"
#include "../src/core/Exceptions/InvalidDataFormatException.hpp"
#include "../src/core/Exceptions/InvalidSampleRateException.hpp"
#include "../src/core/Exceptions/SampleRateMismatchException.hpp"
#include "../src/core/Exceptions/AudioLengthMismatchException.hpp"
#include "../src/core/Exceptions/InvalidActionTypeException.hpp"
#include "../src/core/Exceptions/InvalidParameterException.hpp"
#include "../src/core/Exceptions/ParameterOutOfRangeException.hpp"
#include "../src/core/Exceptions/SensorInputException.hpp"
#include "../src/core/Exceptions/MotorOutputException.hpp"
#include "../src/core/Exceptions/MemoryAllocationException.hpp"

namespace py = pybind11;
namespace nlm {

// Python exception translators with enhanced context
void translate_exception(const std::exception& e) {
    // Convert common C++ exceptions to Python exceptions
    try {
        throw;
    } catch (const std::runtime_error& e) {
        py::raise<RuntimeError>(std::string("Runtime error: ") + e.what());
    } catch (const std::invalid_argument& e) {
        py::raise<ValidationError>(std::string("Invalid argument: ") + e.what());
    } catch (const std::out_of_range& e) {
        py::raise<ValidationError>(std::string("Out of range: ") + e.what());
    } catch (const std::length_error& e) {
        py::raise<RuntimeError>(std::string("Length error: ") + e.what());
    } catch (const std::bad_alloc& e) {
        py::raise<RuntimeError>(std::string("Memory allocation error: ") + e.what());
    } catch (...) {
        py::raise<RuntimeError>(std::string("Unknown C++ exception"));
    }
}

// Enhanced exception translation with error codes
void translate_enhanced_exception(const std::exception& e) {
    try {
        throw;
    } catch (const NLMException& e) {
        // Extract error code from C++ exception
        const char* error_code = e.getErrorCode();
        py::raise<NLMException>(std::string("NLM Exception [") + error_code + "] " + e.what());
    } catch (const FileNotFoundException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<FileIOException>(std::string("File not found [") + error_code + "] " + e.what());
    } catch (const FilePermissionException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<FileIOException>(std::string("File permission error [") + error_code + "] " + e.what());
    } catch (const FileFormatException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<FileIOException>(std::string("File format error [") + error_code + "] " + e.what());
    } catch (const FileWriteException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<FileIOException>(std::string("File write error [") + error_code + "] " + e.what());
    } catch (const ValidationException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<ValidationError>(std::string("Validation Error [") + error_code + "] " + e.what());
    } catch (const InvalidDimensionsException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<ValidationError>(std::string("Invalid dimensions [") + error_code + "] " + e.what());
    } catch (const InvalidRangeException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<ValidationError>(std::string("Invalid range [") + error_code + "] " + e.what());
    } catch (const InvalidFormatException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<ValidationError>(std::string("Invalid format [") + error_code + "] " + e.what());
    } catch (const NullOrEmptyException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<ValidationError>(std::string("Null or empty [") + error_code + "] " + e.what());
    } catch (const KeyNotFoundException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<ConfigurationError>(std::string("Key not found [") + error_code + "] " + e.what());
    } catch (const TypeMismatchException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<ConfigurationError>(std::string("Type mismatch [") + error_code + "] " + e.what());
    } catch (const NeuronNotFoundException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<BrainError>(std::string("Neuron not found [") + error_code + "] " + e.what());
    } catch (const InvalidCurrentException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<BrainError>(std::string("Invalid current [") + error_code + "] " + e.what());
    } catch (const CurrentOutOfRangeException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<BrainError>(std::string("Current out of range [") + error_code + "] " + e.what());
    } catch (const PositionOutOfBoundsException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<WorldError>(std::string("Position out of bounds [") + error_code + "] " + e.what());
    } catch (const InvalidCommandException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<WorldError>(std::string("Invalid command [") + error_code + "] " + e.what());
    } catch (const InvalidDataSizeException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<SensoryError>(std::string("Invalid data size [") + error_code + "] " + e.what());
    } catch (const InvalidDataFormatException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<SensoryError>(std::string("Invalid data format [") + error_code + "] " + e.what());
    } catch (const InvalidSampleRateException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<AudioError>(std::string("Invalid sample rate [") + error_code + "] " + e.what());
    } catch (const SampleRateMismatchException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<AudioError>(std::string("Sample rate mismatch [") + error_code + "] " + e.what());
    } catch (const AudioLengthMismatchException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<AudioError>(std::string("Audio length mismatch [") + error_code + "] " + e.what());
    } catch (const InvalidActionTypeException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<ActionError>(std::string("Invalid action type [") + error_code + "] " + e.what());
    } catch (const InvalidParameterException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<ActionError>(std::string("Invalid parameter [") + error_code + "] " + e.what());
    } catch (const ParameterOutOfRangeException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<ActionError>(std::string("Parameter out of range [") + error_code + "] " + e.what());
    } catch (const SensorInputException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<AgentError>(std::string("Sensor input error [") + error_code + "] " + e.what());
    } catch (const MotorOutputException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<AgentError>(std::string("Motor output error [") + error_code + "] " + e.what());
    } catch (const MemoryAllocationException& e) {
        const char* error_code = e.getErrorCode();
        py::raise<RuntimeError>(std::string("Memory allocation error [") + error_code + "] " + e.what());
    } catch (...) {
        py::raise<RuntimeError>(std::string("Unknown enhanced C++ exception"));
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features comprehensive error handling and validation:
        - File I/O error handling (file not found, permissions, malformed JSON)
        - Input validation for sensory data, parameters, and boundaries
        - Exception safety with proper exception translators
        - Performance considerations with lazy validation
    )pbdoc";

    // Register custom Python exceptions
    py::register_exception<NLMException, py::exception<Exception>>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<FileIOException, NLMException>(m, "FileIOException");
    py::register_exception<ValidationError, NLMException>(m, "ValidationError");
    py::register_exception<ConfigurationError, NLMException>(m, "ConfigurationError");
    py::register_exception<BrainError, NLMException>(m, "BrainError");
    py::register_exception<WorldError, NLMException>(m, "WorldError");
    py::register_exception<SensoryError, NLMException>(m, "SensoryError");
    py::register_exception<AudioError, NLMException>(m, "AudioError");
    py::register_exception<ActionError, NLMException>(m, "ActionError");
    py::register_exception<AgentError, NLMException>(m, "AgentError");

    // Register enhanced exception translator
    py::register_exception_translator(&translate_enhanced_exception);

    // Type conversion utilities for Python integration
    m.def("to_numpy", &NumpyConverter::to_numpy,
          py::arg("data"), py::arg("dtype") = py::arg("float32"),
          "Convert C++ data to NumPy array with automatic type detection");

    m.def("from_numpy", &NumpyConverter::from_numpy,
          py::arg("array"), py::arg("target_shape") = py::none,
          "Convert NumPy array to C++ compatible format");

    m.def("to_string", &EnumConverter::to_string,
          py::arg("value"),
          "Convert enum value to human-readable string");

    m.def("from_string", &EnumConverter::from_string,
          py::arg("value"), py::arg("enum_type"),
          "Convert string to enum value");

    // Pythonic interface factory functions
    m.def("create_pythonic_config", &create_pythonic_config,
          py::arg("config"),
          "Create PythonicConfig wrapper with enhanced interfaces");

    m.def("create_pythonic_sensory", &create_pythonic_sensory,
          py::arg("sensory"),
          "Create PythonicSensoryInput wrapper with enhanced interfaces");

    m.def("create_pythonic_action", &create_pythonic_action,
          py::arg("action"),
          "Create PythonicAction wrapper with enhanced interfaces");

    m.def("create_pythonic_brain", &create_pythonic_brain,
          py::arg("brain"),
          "Create PythonicBrain wrapper with enhanced interfaces");

    // Utility functions for Python integration
    m.def("get_as_numpy", &get_as_numpy,
          py::arg("data"), py::arg("dtype") = py::arg("float32"),
          "Convert data to NumPy array for vision/audio processing");

    m.def("set_from_numpy", &set_from_numpy,
          py::arg("target"), py::arg("numpy_array"),
          "Set target data from NumPy array");

    // Configuration dictionary-based interface
    m.def("config_to_dict", &ConfigConverter::to_config_dict,
          py::arg("config_obj"),
          "Convert C++ Config object to Python dictionary");

    m.def("config_from_dict", &ConfigConverter::from_config_dict,
          py::arg("config_dict"), py::arg("config_obj"),
          "Update C++ Config object from Python dictionary");

    // Lazy validation toggle for performance
    m.def("enable_lazy_validation", [](py::object obj) {
        if (hasattr(obj, "enable_lazy_validation")) {
            obj.attr("enable_lazy_validation")();
        }
    }, py::arg("obj"),
          "Enable lazy validation for performance-critical operations");

    // Performance optimization flags
    py::module_::get().attr("enable_numpy_support") = true;
    py::module_::get().attr("enable_property_access") = true;
    py::module_::get().attr("enable_context_managers") = true;
    py::module_::get().attr("enable_enum_conversion") = true;

    m.attr("enable_lazy_validation") = py::cpp_function([](py::object obj) {
        if (hasattr(obj, "enable_lazy_validation")) {
            obj.attr("enable_lazy_validation")();
        }
    }, py::arg("obj"));

    m.attr("get_version") = []() {
        return "0.1.0-enhanced";
    };

    m.attr("has_numpy") = []() {
        try {
            py::module::import("numpy");
            return true;
        } catch (...) {
            return false;
        }
    };

    m.attr("has_property_access") = []() {
        return true;
    };

    m.attr("has_context_managers") = []() {
        return true;
    };


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
        });

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
        .def("getChannels", &Vision::getChannels);

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
        .def("clearSignals", &InternalSignals::clearSignals);

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
