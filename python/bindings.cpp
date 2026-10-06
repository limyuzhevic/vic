#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>

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

// Custom Exception Hierarchy
class NLMException : public std::exception {
public:
    enum class ErrorCode {
        SUCCESS = 0,
        INVALID_ARGUMENT = 1,
        BRAIN_NOT_INITIALIZED = 2,
        REGION_NOT_FOUND = 3,
        NEURON_NOT_FOUND = 4,
        SYNAPSE_NOT_FOUND = 5,
        INVALID_CONFIG = 6,
        FILE_ERROR = 7,
        MEMORY_ERROR = 8,
        SIMULATION_ERROR = 9,
        DEVELOPMENT_ERROR = 10,
        PLASTICITY_ERROR = 11,
        NEUROMODULATION_ERROR = 12,
        PREDICTION_ERROR = 13,
        VISUALIZATION_ERROR = 14,
        CHECKPOINT_ERROR = 15,
        UNKNOWN_ERROR = 16
    };

protected:
    ErrorCode code_;
    std::string message_;
    std::string file_;
    int line_;
    std::string stack_trace_;

public:
    NLMException(ErrorCode code, const std::string& message, 
                 const std::string& file = "", int line = 0)
        : code_(code), message_(message), file_(file), line_(line) {
        std::ostringstream oss;
        oss << "NLM Error [" << static_cast<int>(code_) << "]: " << message_;
        if (!file_.empty()) {
            oss << " (" << file_ << ":" << line_ << ")";
        }
        message_ = oss.str();
    }

    ErrorCode getCode() const { return code_; }
    const char* what() const noexcept override { return message_.c_str(); }
    const std::string& getMessage() const { return message_; }
    
    static std::string getErrorCodeName(ErrorCode code) {
        switch (code) {
            case ErrorCode::SUCCESS: return "SUCCESS";
            case ErrorCode::INVALID_ARGUMENT: return "INVALID_ARGUMENT";
            case ErrorCode::BRAIN_NOT_INITIALIZED: return "BRAIN_NOT_INITIALIZED";
            case ErrorCode::REGION_NOT_FOUND: return "REGION_NOT_FOUND";
            case ErrorCode::NEURON_NOT_FOUND: return "NEURON_NOT_FOUND";
            case ErrorCode::SYNAPSE_NOT_FOUND: return "SYNAPSE_NOT_FOUND";
            case ErrorCode::INVALID_CONFIG: return "INVALID_CONFIG";
            case ErrorCode::FILE_ERROR: return "FILE_ERROR";
            case ErrorCode::MEMORY_ERROR: return "MEMORY_ERROR";
            case ErrorCode::SIMULATION_ERROR: return "SIMULATION_ERROR";
            case ErrorCode::DEVELOPMENT_ERROR: return "DEVELOPMENT_ERROR";
            case ErrorCode::PLASTICITY_ERROR: return "PLASTICITY_ERROR";
            case ErrorCode::NEUROMODULATION_ERROR: return "NEUROMODULATION_ERROR";
            case ErrorCode::PREDICTION_ERROR: return "PREDICTION_ERROR";
            case ErrorCode::VISUALIZATION_ERROR: return "VISUALIZATION_ERROR";
            case ErrorCode::CHECKPOINT_ERROR: return "CHECKPOINT_ERROR";
            case ErrorCode::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
            default: return "UNKNOWN_ERROR";
        }
    }

    virtual ~NLMException() = default;
};

class InvalidArgumentException : public NLMException {
public:
    InvalidArgumentException(const std::string& message,
                           const std::string& file = "", int line = 0)
        : NLMException(ErrorCode::INVALID_ARGUMENT, message, file, line) {}
};

class BrainInitializationException : public NLMException {
public:
    BrainInitializationException(const std::string& message,
                                const std::string& file = "", int line = 0)
        : NLMException(ErrorCode::BRAIN_NOT_INITIALIZED, message, file, line) {}
};

class RegionNotFoundException : public NLMException {
public:
    RegionNotFoundException(const std::string& message,
                           const std::string& file = "", int line = 0)
        : NLMException(ErrorCode::REGION_NOT_FOUND, message, file, line) {}
};

class NeuronNotFoundException : public NLMException {
public:
    NeuronNotFoundException(const std::string& message,
                           const std::string& file = "", int line = 0)
        : NLMException(ErrorCode::NEURON_NOT_FOUND, message, file, line) {}
};

class SynapseNotFoundException : public NLMException {
public:
    SynapseNotFoundException(const std::string& message,
                            const std::string& file = "", int line = 0)
        : NLMException(ErrorCode::SYNAPSE_NOT_FOUND, message, file, line) {}
};

class ConfigException : public NLMException {
public:
    ConfigException(const std::string& message,
                   const std::string& file = "", int line = 0)
        : NLMException(ErrorCode::INVALID_CONFIG, message, file, line) {}
};

class VisualizationException : public NLMException {
public:
    VisualizationException(const std::string& message,
                          const std::string& file = "", int line = 0)
        : NLMException(ErrorCode::VISUALIZATION_ERROR, message, file, line) {}
};

class CheckpointException : public NLMException {
public:
    CheckpointException(const std::string& message,
                       const std::string& file = "", int line = 0)
        : NLMException(ErrorCode::CHECKPOINT_ERROR, message, file, line) {}
};

// Stack trace utility function
inline std::string getStackTrace() {
    std::ostringstream oss;
    oss << "Stack trace not available at runtime";
    return oss.str();
}

// Context Manager Base Class Template
class ContextManagerBase {
protected:
    bool is_open_ = false;
    std::string name_;

public:
    ContextManagerBase(const std::string& name) : name_(name) {}
    
    virtual ~ContextManagerBase() {
        if (is_open_) {
            try {
                __exit__(nullptr, nullptr, nullptr);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Exception during context manager cleanup: " 
                         << e.what() << std::endl;
            }
        }
    }
    
    void setOpen(bool open) { is_open_ = open; }
    
    virtual py::object __enter__() {
        if (!__enter_impl()) {
            throw BrainInitializationException("Failed to enter context");
        }
        is_open_ = true;
        return py::cast(this);
    }
    
    virtual bool __enter_impl() = 0;
    
    virtual py::object __exit__(py::object exc_type, py::object exc_val, py::object exc_tb) {
        if (is_open_) {
            __exit_impl(exc_type, exc_val, exc_tb);
            is_open_ = false;
        }
        return py::bool_(false); // Don't suppress exceptions
    }
    
    virtual void __exit_impl(py::object exc_type, py::object exc_val, py::object exc_tb) = 0;
    
    std::string getName() const { return name_; }
};

// Template function for registering pybind11 classes with common patterns
template<typename T>
void registerBasicClass(py::class_<T>& cls, const std::string& docstring = "") {
    if (!docstring.empty()) {
        cls.doc() = docstring;
    }
    // Add common Pythonic patterns
    cls.def("__repr__", [](const T& obj) {
        std::ostringstream oss;
        oss << "<" << typeid(T).name() << " object at " << &obj << ">";
        return oss.str();
    });
}

// Python property decorator helper
#define PROPERTY_GETTER(Name, Getter) .def_readonly_property(Name, Getter)
#define PROPERTY_SETTER(Name, Getter, Setter) .def_property(Name, Getter, Setter)

// Iterator support helper
template<typename Container>
void registerIteratorSupport(py::class_<Container>& cls) {
    cls.def("__iter__", [](const Container& c) {
        return py::make_iterator(c.begin(), c.end());
    }, py::keep_alive<0, 1>());
}

// Type validation helper
inline bool validateType(const std::type_info& actual, const std::type_info& expected) {
    return actual == expected;
}

// Conversion helper for Python-friendly method names
inline std::string toSnakeCase(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (isupper(c)) {
            if (!result.empty()) result += '_';
            result += tolower(c);
        } else {
            result += c;
        }
    }
    return result;
}

// Context manager for Brain
class BrainContextManager : public ContextManagerBase {
private:
    std::shared_ptr<Brain> brain_;

public:
    BrainContextManager(std::shared_ptr<Brain> brain)
        : ContextManagerBase("Brain"), brain_(brain) {}
    
    bool __enter_impl() override {
        if (!brain_) {
            return false;
        }
        brain_->logStatus();
        return true;
    }
    
    void __exit_impl(py::object exc_type, py::object exc_val, py::object exc_tb) override {
        try {
            brain_->reset();
        } catch (const std::exception& e) {
            // Log but don't throw during cleanup
            std::cerr << "Error during Brain cleanup: " << e.what() << std::endl;
        }
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
};

// Context manager for SimpleWorld
class WorldContextManager : public ContextManagerBase {
private:
    std::shared_ptr<SimpleWorld> world_;

public:
    WorldContextManager(std::shared_ptr<SimpleWorld> world)
        : ContextManagerBase("World"), world_(world) {}
    
    bool __enter_impl() override {
        if (!world_) {
            return false;
        }
        world_->reset();
        return true;
    }
    
    void __exit_impl(py::object exc_type, py::object exc_val, py::object exc_tb) override {
        // World cleanup is handled by its destructor
    }
    
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
};

// Function to create enhanced Brain class with context manager support
void registerBrainWithContextManager(py::module_& m) {
    py::class_<Brain, std::shared_ptr<Brain>, ContextManagerBase>(m, "Brain", 
        R"pbdoc(Central neural simulation brain class with context manager support)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", [](std::shared_ptr<Brain> self) {
            // Return a context manager object
            return std::make_shared<BrainContextManager>(self);
        }, py::arg("self"), "Enter brain context manager")
        .def("__exit__", [](std::shared_ptr<Brain> self) {
            // Context manager exit is handled by the context manager itself
            return py::bool_(false);
        }, py::arg("self"), "Exit brain context manager")
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
        .def("create_context_manager", [](std::shared_ptr<Brain> brain) {
            return std::make_shared<BrainContextManager>(brain);
        }, py::arg("brain"), "Create a context manager for the brain");
}

// Neural network visualization functions
namespace Visualization {
    std::string generateNetworkGraph(const std::shared_ptr<Brain>& brain) {
        std::ostringstream oss;
        oss << "Neural Network Visualization:" << std::endl;
        oss << "Total Regions: " << brain->getRegionCount() << std::endl;
        oss << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        oss << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        oss << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        oss << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        oss << "Average Firing Rate: " << std::fixed << std::setprecision(2) 
            << brain->getAverageFiringRate() << " Hz" << std::endl;
        oss << "Excitation/Inhibition Ratio: " << std::fixed << std::setprecision(3)
            << brain->getExcitationInhibitionRatio() << std::endl;
        oss << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage())
            << " (" << getDevelopmentStageName(brain->getDevelopmentalStage()) << ")" << std::endl;
        
        return oss.str();
    }
    
    void saveNetworkGraph(const std::shared_ptr<Brain>& brain, 
                         const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw VisualizationException("Cannot open file for writing: " + filepath);
        }
        
        file << generateNetworkGraph(brain);
        file.close();
    }
    
    std::string getDevelopmentStageName(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "Critical Period";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
}

// Checkpoint management
namespace Checkpoint {
    bool saveCheckpoint(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        try {
            brain->save(filepath);
            return true;
        } catch (const std::exception& e) {
            throw CheckpointException(std::string("Failed to save checkpoint: ") + e.what());
        }
    }
    
    bool loadCheckpoint(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        try {
            brain->load(filepath);
            return true;
        } catch (const std::exception& e) {
            throw CheckpointException(std::string("Failed to load checkpoint: ") + e.what());
        }
    }
    
    bool validateCheckpoint(const std::string& filepath) {
        // Basic checkpoint validation
        std::ifstream file(filepath);
        return file.good();
    }
    
    std::vector<std::string> getAvailableCheckpoints(const std::string& directory) {
        std::vector<std::string> checkpoints;
        // Implementation would list files in directory
        return checkpoints;
    }
}

// Configuration validation
namespace ConfigValidation {
    bool validateConfig(const Config& config) {
        // Basic validation logic
        auto neuron_count_opt = config.get<int>("brain.neuron_count");
        if (!neuron_count_opt.has_value() || neuron_count_opt.value() <= 0) {
            throw ConfigException("Invalid or missing brain.neuron_count in configuration");
        }
        
        auto dt_opt = config.get<double>("simulation.dt");
        if (!dt_opt.has_value() || dt_opt.value() <= 0.0) {
            throw ConfigException("Invalid or missing simulation.dt in configuration");
        }
        
        auto threshold_opt = config.get<double>("brain.threshold");
        if (!threshold_opt.has_value() || threshold_opt.value() < 0.0) {
            throw ConfigException("Invalid or missing brain.threshold in configuration");
        }
        return true;
    }
    
    std::string getValidationReport(const Config& config) {
        std::ostringstream oss;
        oss << "Configuration Validation Report:" << std::endl;
        oss << "-----------------------------" << std::endl;
        
        // Validate various configuration sections
        auto neuron_count_opt = config.get<int>("brain.neuron_count");
        if (neuron_count_opt.has_value()) {
            oss << "✓ brain.neuron_count: " << neuron_count_opt.value() << std::endl;
        } else {
            oss << "✗ brain.neuron_count: MISSING" << std::endl;
        }
        
        auto dt_opt = config.get<double>("simulation.dt");
        if (dt_opt.has_value()) {
            oss << "✓ simulation.dt: " << dt_opt.value() << " s" << std::endl;
        } else {
            oss << "✗ simulation.dt: MISSING" << std::endl;
        }
        
        auto threshold_opt = config.get<double>("brain.threshold");
        if (threshold_opt.has_value()) {
            oss << "✓ brain.threshold: " << threshold_opt.value() << std::endl;
        } else {
            oss << "✗ brain.threshold: MISSING" << std::endl;
        }
        
        oss << "-----------------------------" << std::endl;
        oss << "Validation " << (validateConfig(config) ? "PASSED" : "FAILED") << std::endl;
        
        return oss.str();
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        --------------------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        New Features:
        - Custom exception hierarchy with specific exception types
        - Context manager support for automatic resource cleanup
        - Pythonic API with property decorators and iterator support
        - Neural network visualization capabilities
        - Checkpoint management APIs
        - Configuration validation and help
        
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, 
        SensoryInput, and Action with enhanced Python integration.
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Register all exception types
    py::class_<NLMException, std::exception>(m, "NLMException")
        .def(py::init<const std::string&, int>(), py::arg("message"), py::arg("code") = 16)
        .def("getCode", &NLMException::getCode)
        .def("getMessage", &NLMException::getMessage)
        .def("getCodeName", &NLMException::getErrorCodeName);

    py::class_<InvalidArgumentException, NLMException>(m, "InvalidArgumentException");
    py::class_<BrainInitializationException, NLMException>(m, "BrainInitializationException");
    py::class_<RegionNotFoundException, NLMException>(m, "RegionNotFoundException");
    py::class_<NeuronNotFoundException, NLMException>(m, "NeuronNotFoundException");
    py::class_<SynapseNotFoundException, NLMException>(m, "SynapseNotFoundException");
    py::class_<ConfigException, NLMException>(m, "ConfigException");
    py::class_<VisualizationException, NLMException>(m, "VisualizationException");
    py::class_<CheckpointException, NLMException>(m, "CheckpointException");

    // Register basic types (same as before)
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

    // Register enhanced Config class with safe value getters
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with validation)pbdoc")
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
        .def("get", [](const Config& self, const std::string& key) -> std::optional<std::string> {
            return self.get<std::string>(key);
        }, py::arg("key"),
             "Get configuration value by key (as string, returns None if missing)")
        .def("getInt", [](const Config& self, const std::string& key) -> std::optional<int> {
            return self.get<int>(key);
        }, py::arg("key"),
             "Get configuration value by key (as int, returns None if missing)")
        .def("getFloat", [](const Config& self, const std::string& key) -> std::optional<double> {
            return self.get<double>(key);
        }, py::arg("key"),
             "Get configuration value by key (as float, returns None if missing)")
        .def("getBool", [](const Config& self, const std::string& key) -> std::optional<bool> {
            return self.get<bool>(key);
        }, py::arg("key"),
             "Get configuration value by key (as bool, returns None if missing)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", &ConfigValidation::validateConfig,
             "Validate configuration")
        .def("validation_report", &ConfigValidation::getValidationReport,
             "Get configuration validation report")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Register enhanced sensory input classes with Pythonic property access
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def_readonly("type", &SensoryInput::getType, "Sensory input type")
        .def_readonly("dimensions", &SensoryInput::getDimensions, "Input dimensions")
        .def_readonly("timestamp", &SensoryInput::getTimestamp, "Timestamp");

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
        .def_readonly("width", &Vision::getWidth, "Image width")
        .def_readonly("height", &Vision::getHeight, "Image height")
        .def_readonly("channels", &Vision::getChannels, "Number of color channels");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def_readonly("sampleRate", &Audio::getSampleRate, "Audio sample rate")
        .def_readonly("numSamples", &Audio::getNumSamples, "Number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getDimensions", &InternalSignals::getDimensions, "Get number of signals")
        .def_readonly("dimension", &InternalSignals::getDimensions, "Number of signals");

    // Register enhanced Action class with available methods
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
        .def("getParameterCount", [](const Action& action) {
            return action.getParameters().size();
        }, "Get number of parameters")
        .def("isEmpty", [](const Action& action) {
            return action.getParameters().empty();
        }, "Check if action has no parameters")
        .def("hasType", [](const Action& action, ActionType type) {
            return action.getType() == type;
        }, py::arg("type"), "Check if action has specific type")
        .def("getTypeName", [](const Action& action) {
            // This would need actionTypeToString() which is private
            return "ActionType"; // Placeholder
        }, "Get action type name");

    // Register enhanced WorldObject class
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
        .def("getDistance", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Get distance to position")
        .def("isActive", &WorldObject::active, "Check if object is active")
        .def("setPosition", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position")
        .def("getTypeName", [](const WorldObject& obj) {
            return WorldObjectTypeNames[static_cast<size_t>(obj.type)];
        }, "Get human-readable type name");

    // Register enhanced AgentBody class with Pythonic properties
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
        .def("getPosition", [](const AgentBody& body) {
            return std::make_pair(body.x, body.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getVelocity", [](const AgentBody& body) {
            return std::make_pair(body.velocityX, body.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("getSpeed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + 
                           body.velocityY * body.velocityY);
        }, "Get speed magnitude")
        .def("isMoving", &AgentBody::isMoving, "Check if agent is moving")
        .def("isTurning", &AgentBody::isTurning, "Check if agent is turning")
        .def("getEnergyLevel", [](const AgentBody& body) {
            return body.energy / 100.0f; // Normalize to 0-1
        }, "Get normalized energy level (0-1)");

    // Register enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", &ActionResult::success, "Check if action was successful")
        .def("getRewardDescription", [](const ActionResult& result) {
            std::ostringstream oss;
            oss << "Reward: " << result.reward << " (" 
                << (result.success ? "Success" : "Failure") << ")";
            if (!result.message.empty()) {
                oss << " - " << result.message;
            }
            return oss.str();
        }, "Get human-readable reward description")
        .def("getSuccessLevel", [](const ActionResult& result) {
            return result.success ? 1.0f : 0.0f;
        }, "Get success level (0-1)");

    // Register enhanced SensoryPercept class with iterator support
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
        .def("__iter__", [](const SensoryPercept& percept) {
            return py::make_iterator(percept.getAllSignals().begin(),
                                    percept.getAllSignals().end());
        }, py::keep_alive<0, 1>(),
             "Iterate over all sensory signals")
        .def("__len__", [](const SensoryPercept& percept) {
            return percept.getAllSignals().size();
        }, "Get number of sensory signals")
        .def("getVisionShape", [](const SensoryPercept& percept) {
            return std::make_tuple(percept.getVisionWidth(), 
                                  percept.getVisionHeight());
        }, "Get vision shape as (width, height) tuple")
        .def("getTouchSize", [](const SensoryPercept& percept) {
            return percept.getTouch().size();
        }, "Get touch signal count")
        .def("getInternalSize", [](const SensoryPercept& percept) {
            return percept.getInternal().size();
        }, "Get internal signal count")
        .def("getProprioceptionSize", [](const SensoryPercept& percept) {
            return percept.getProprioception().size();
        }, "Get proprioception signal count")
        .def("getAudioSize", [](const SensoryPercept& percept) {
            return percept.getAudio().size();
        }, "Get audio signal count");

    // Register enhanced SimpleWorld class with context manager
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>, ContextManagerBase>(m, "SimpleWorld",
        R"pbdoc(Simple 2D world for NLM simulation with context manager support)pbdoc")
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
        .def("create_context_manager", [](std::shared_ptr<SimpleWorld> world) {
            return std::make_shared<WorldContextManager>(world);
        }, py::arg("world"), "Create a context manager for the world");

    // Register enhanced Brain class with context manager support
    registerBrainWithContextManager(m);

    // Register enhanced AgentBrain class
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
        .def("setGoal", [](AgentBrain& agent, const std::string& goal) {
            agent.setGoal(goal);
        }, py::arg("goal"), "Set agent's goal")
        .def("getGoal", &AgentBrain::getGoal, "Get agent's goal")
        .def("addMemory", [](AgentBrain& agent, const std::string& memory) {
            agent.addMemory(memory);
        }, py::arg("memory"), "Add memory entry")
        .def("clearMemory", &AgentBrain::clearMemory, "Clear agent's memory");

    // Register convenience factory functions
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

    // Register visualization functions
    m.def("visualizeNetwork", &Visualization::generateNetworkGraph,
          py::arg("brain"), "Generate network visualization report");
    
    m.def("saveNetworkGraph", &Visualization::saveNetworkGraph,
          py::arg("brain"), py::arg("filepath"), "Save network visualization to file");

    // Register checkpoint management functions
    m.def("saveCheckpoint", &Checkpoint::saveCheckpoint,
          py::arg("brain"), py::arg("filepath"), "Save brain state as checkpoint");
    
    m.def("loadCheckpoint", &Checkpoint::loadCheckpoint,
          py::arg("brain"), py::arg("filepath"), "Load brain state from checkpoint");
    
    m.def("validateCheckpoint", &Checkpoint::validateCheckpoint,
          py::arg("filepath"), "Validate checkpoint file");
    
    m.def("getAvailableCheckpoints", &Checkpoint::getAvailableCheckpoints,
          py::arg("directory"), "Get list of available checkpoints");

    // Register configuration validation functions
    m.def("validateConfig", &ConfigValidation::validateConfig,
          py::arg("config"), "Validate configuration");
    
    m.def("getValidationReport", &ConfigValidation::getValidationReport,
          py::arg("config"), "Get configuration validation report");

    // Register global constants and utilities
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Register module-level utility functions
    m.def("getErrorCodeName", &NLMException::getErrorCodeName,
          py::arg("code"), "Get human-readable error code name");
    
    m.def("getStackTrace", &getStackTrace,
          "Get stack trace (utility for debugging)");

    // Register class name utility
    m.def("getClassName", [](auto& obj) {
        return std::string(typeid(obj).name());
    }, "Get class name of object");
}

} // namespace nlm