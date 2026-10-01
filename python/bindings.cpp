#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <utility>
#include <unordered_map>
#include <optional>

namespace py = pybind11;

// =============================================================================
// ENHANCED ERROR HANDLING AND VALIDATION
// =============================================================================

// Base exception class for NLM
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

// Validation-specific exception
class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& message) : NLMError(message) {}
};

// Configuration-specific exception
class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError(message) {}
};

// Resource management exception
class ResourceError : public NLMError {
public:
    explicit ResourceError(const std::string& message) : NLMError(message) {}
};

// =============================================================================
// TEMPLATE-BASED ID CLASSES (REDUCED DUPLICATION)
// =============================================================================

template<typename T, typename ValueType = uint64_t>
class IDClass {
private:
    ValueType value;
    
public:
    // Constructors
    IDClass() : value(0) {}
    explicit IDClass(ValueType val) : value(val) {}
    IDClass(const IDClass& other) = default;
    IDClass& operator=(const IDClass& other) = default;
    
    // Accessors
    ValueType getValue() const { return value; }
    void setValue(ValueType val) { value = val; }
    
    // Index calculation
    size_t getIndex() const { return static_cast<size_t>(value); }
    
    // Comparison operators
    bool operator==(const IDClass& other) const { return value == other.value; }
    bool operator!=(const IDClass& other) const { return !(*this == other); }
    bool operator<(const IDClass& other) const { return value < other.value; }
    bool operator<=(const IDClass& other) const { return value <= other.value; }
    bool operator>(const IDClass& other) const { return value > other.value; }
    bool operator>=(const IDClass& other) const { return value >= other.value; }
    
    // Conversion operator
    explicit operator ValueType() const { return value; }
    
    // Python integration methods
    std::string repr() const {
        std::stringstream ss;
        ss << "<" << typeid(T).name() << ": " << value << ">";
        return ss.str();
    }
    
    size_t hash() const {
        return std::hash<ValueType>{}(value);
    }
    
    // Factory methods
    static IDClass fromString(const std::string& str) {
        try {
            return IDClass(static_cast<ValueType>(std::stoull(str)));
        } catch (const std::exception& e) {
            throw ValidationError("Invalid ID string: " + str);
        }
    }
    
    std::string toString() const {
        return std::to_string(value);
    }
    
    bool isValid() const {
        return value != static_cast<ValueType>(-1);
    }
    
    IDClass invalid() const {
        return IDClass(static_cast<ValueType>(-1));
    }
    
    // Batch operations
    static std::vector<IDClass> range(size_t start, size_t end) {
        std::vector<IDClass> result;
        result.reserve(end - start);
        for (size_t i = start; i < end; ++i) {
            result.push_back(IDClass(static_cast<ValueType>(i)));
        }
        return result;
    }
    
    static std::vector<IDClass> batch(size_t count, ValueType start = 0) {
        std::vector<IDClass> result;
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(IDClass(static_cast<ValueType>(start + i)));
        }
        return result;
    }
};

// =============================================================================
// CONTEXT MANAGER FOR RESOURCE MANAGEMENT
// =============================================================================

class ContextManagerBase {
protected:
    bool is_active = false;
    
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    
public:
    ContextManagerBase() : is_active(false) {}
    virtual ~ContextManagerBase() { if (is_active) deactivate(); }
    
    void enter() {
        if (is_active) throw ResourceError("Already active");
        activate();
        is_active = true;
    }
    
    void exit(int /*ptype*/, const char* /*pname*/, const char* /*pval*/, const char* /*ptrace*/) {
        if (!is_active) throw ResourceError("Not active");
        deactivate();
        is_active = false;
    }
    
    bool is_active_context() const { return is_active; }
};

// Simulation session context manager
class SimulationSession : public ContextManagerBase {
private:
    std::shared_ptr<class Brain> brain;
    std::shared_ptr<class SimpleWorld> world;
    bool initialized = false;
    
    void activate() override {
        if (!brain || !world) {
            throw ResourceError("Brain or world not set");
        }
        brain->reset();
        world->reset();
        initialized = true;
    }
    
    void deactivate() override {
        brain.reset();
        world.reset();
        initialized = false;
    }
    
public:
    SimulationSession() = default;
    SimulationSession(std::shared_ptr<class Brain> b, std::shared_ptr<class SimpleWorld> w) 
        : brain(std::move(b)), world(std::move(w)) {}
    
    void setBrain(std::shared_ptr<class Brain> b) { brain = std::move(b); }
    void setWorld(std::shared_ptr<class SimpleWorld> w) { world = std::move(w); }
    std::shared_ptr<class Brain> getBrain() const { return brain; }
    std::shared_ptr<class SimpleWorld> getWorld() const { return world; }
    
    double step(double timestep = 1.0) {
        if (!is_active_context()) throw ResourceError("Session not active");
        if (!world) throw ResourceError("World not set");
        return world->update(timestep);
    }
    
    void shutdown() {
        exit(0, "", "", "");
    }
};

// =============================================================================
// BUILDER PATTERN FOR COMPLEX OBJECTS
// =============================================================================

class ActionBuilder {
private:
    class Action action;
    
public:
    ActionBuilder& setType(class ActionType type) {
        action.setType(type);
        return *this;
    }
    
    ActionBuilder& setParameters(const std::vector<float>& params) {
        action.setParameters(params);
        return *this;
    }
    
    ActionBuilder& addParameter(float param) {
        auto params = action.getParameters();
        params.push_back(param);
        action.setParameters(params);
        return *this;
    }
    
    Action build() {
        return action;
    }
    
    ActionBuilder reset() {
        action = Action();
        return *this;
    }
};

// =============================================================================
// CONFIGURATION VALIDATION HELPER
// =============================================================================

class ConfigValidator {
public:
    static void validate(const class Config& config) {
        if (config.has("simulationSteps") && config.get<uint64_t>("simulationSteps") == 0) {
            throw ValidationError("simulationSteps must be greater than 0");
        }
        if (config.has("maxNeurons") && config.get<uint64_t>("maxNeurons") == 0) {
            throw ValidationError("maxNeurons must be greater than 0");
        }
        if (config.has("maxSynapses") && config.get<uint64_t>("maxSynapses") == 0) {
            throw ValidationError("maxSynapses must be greater than 0");
        }
    }
    
    static py::dict extractSettings(const Config& config, const std::vector<std::string>& keys) {
        py::dict result;
        for (const auto& key : keys) {
            if (config.has(key)) {
                result[key.c_str()] = config.get<std::string>(key);
            }
        }
        return result;
    }
    
    static bool validateSimulationParameters(const Config& config) {
        try {
            validate(config);
            return true;
        } catch (const ValidationError&) {
            return false;
        }
    }
};

// =============================================================================
// SERIALIZATION HELPER
// =============================================================================

class Serializer {
public:
    template<typename T>
    static std::string toJson(const T& obj) {
        std::stringstream ss;
        ss << "{\"data\": \"" << obj.toString() << "\"}";
        return ss.str();
    }
    
    template<typename T>
    static T fromJson(const std::string& json) {
        size_t pos = json.find("\"data\": \"");
        if (pos != std::string::npos) {
            size_t start = pos + 9;
            size_t end = json.find("\"}", start);
            std::string data = json.substr(start, end - start);
            return T::fromString(data);
        }
        throw ValidationError("Invalid JSON format");
    }
    
    static std::string toPrettyJson(const py::dict& obj) {
        std::stringstream ss;
        ss << "{\n";
        bool first = true;
        for (auto item : obj) {
            if (!first) ss << ",\n";
            ss << "  \"" << item.first.cast<std::string>() << "\": ";
            if (py::isinstance<py::dict>(item.second)) {
                ss << toPrettyJson(item.second.cast<py::dict>());
            } else if (py::isinstance<py::list>(item.second)) {
                ss << "[";
                auto list = item.second.cast<py::list>();
                for (size_t i = 0; i < list.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << list[i].cast<std::string>();
                }
                ss << "]";
            } else {
                ss << "\"" << item.second.cast<std::string>() << "\"";
            }
            first = false;
        }
        ss << "\n}";
        return ss.str();
    }
};

// =============================================================================
// PYTHONIC UTILITY FUNCTIONS
// =============================================================================

namespace python_bindings {
    
    // Factory functions
    std::shared_ptr<class Config> createDefaultConfig() {
        return std::make_shared<class Config>();
    }
    
    std::shared_ptr<class Brain> createBrain(std::shared_ptr<class Config> config) {
        return std::make_shared<class Brain>(config);
    }
    
    std::shared_ptr<class SimpleWorld> createWorld() {
        return std::make_shared<class SimpleWorld>();
    }
    
    std::shared_ptr<class AgentBrain> createAgentBrain(std::shared_ptr<class Brain> brain) {
        return std::make_shared<class AgentBrain>(brain);
    }
    
    // Batch operations helper
    py::list batchProcessNeurons(
        std::shared_ptr<class Brain> brain,
        const std::vector<class NeuronId>& neuronIds,
        std::function<void(class NeuronId, double)> operation) {
        
        py::list results;
        results.reserve(neuronIds.size());
        
        for (const auto& id : neuronIds) {
            try {
                operation(id, brain->getCurrentValue(id));
                results.append(py::make_tuple(id, true, "Success"));
            } catch (const std::exception& e) {
                results.append(py::make_tuple(id, false, std::string(e.what())));
            }
        }
        return results;
    }
    
    // Type conversion helpers
    std::vector<float> pyArrayToVector(const py::array_t<float>& arr) {
        auto buffer = arr.request();
        float* ptr = static_cast<float*>(buffer.ptr);
        size_t size = buffer.size;
        return std::vector<float>(ptr, ptr + size);
    }
    
    py::array_t<float> vectorToPyArray(const std::vector<float>& vec) {
        return py::array_t<float>(vec);
    }
    
    // Statistical helpers
    std::map<std::string, double> calculateStatistics(const std::vector<float>& data) {
        std::map<std::string, double> stats;
        if (data.empty()) return stats;
        
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        double mean = sum / data.size();
        
        double sq_sum = 0.0;
        for (double val : data) {
            sq_sum += (val - mean) * (val - mean);
        }
        double variance = sq_sum / data.size();
        double stddev = std::sqrt(variance);
        
        double min_val = *std::min_element(data.begin(), data.end());
        double max_val = *std::max_element(data.begin(), data.end());
        
        stats["mean"] = mean;
        stats["variance"] = variance;
        stats["stddev"] = stddev;
        stats["min"] = min_val;
        stats["max"] = max_val;
        stats["count"] = static_cast<double>(data.size());
        
        return stats;
    }
    
    // Advanced filtering helpers
    std::vector<class NeuronId> filterNeurons(
        const std::shared_ptr<class Brain>& brain,
        std::function<bool(const class NeuronId&, double)> predicate) {
        
        std::vector<class NeuronId> result;
        auto all_ids = brain->getNeuronIds();
        
        for (const auto& id : all_ids) {
            double value = brain->getCurrentValue(id);
            if (predicate(id, value)) {
                result.push_back(id);
            }
        }
        
        return result;
    }
}

// =============================================================================
// ENABLE COPY SUPPORT FOR CLASSES
// =============================================================================

void enable_copying(py::class_<class Action>& cl) {
    cl.def(py::init<const Action&>()); // Copy constructor
    cl.def(py::pickle(
        [](const Action& a) {
            return py::make_tuple(a.getType(), a.getParameters());
        },
        [](const py::tuple& t) {
            class ActionType type = t[0].cast<class ActionType>();
            std::vector<float> params = t[1].cast<std::vector<float>>();
            Action action(type, params);
            return action;
        }
    ));
}

template<typename T, typename ValueType>
void enable_copying(py::class_<IDClass<T, ValueType>>& cl) {
    cl.def(py::init<const IDClass<T, ValueType>&>()); // Copy constructor
}

// =============================================================================
// MAIN BINDINGS MODULE
// =============================================================================

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        -------------------------------------------------------
        
        An enhanced Python binding for the NLM C++ neural simulation framework.
        
        This module provides comprehensive Pythonic interfaces with:
        - Template-based ID classes with reduced code duplication
        - Context managers for resource management
        - Fluent interfaces and builder patterns
        - Comprehensive error handling and validation
        - Type hints and extensive documentation
        - Advanced features for performance and flexibility
        
        Examples:
        --------
        
        Basic usage::
        
            from pynlm import Brain, Config, createDefaultConfig
            
            # Create configuration and brain
            config = createDefaultConfig()
            brain = Brain(config)
            
        Using context managers::
        
            from pynlm import createBrain, createWorld, SimulationSession
            
            with SimulationSession(createBrain(config), createWorld()) as session:
                session.getBrain().initialize()
                result = session.step(0.1)
        
        Using builder pattern::
        
            from pynlm import Action, ActionBuilder
            
            action = ActionBuilder() \
                .setType(ActionType.MoveForward) \
                .addParameter(0.5) \
                .addParameter(1.0) \
                .build()
        
        Batch operations::
        
            from pynlm import NeuronId, batch_process_neurons
            
            neurons = NeuronId.batch(100)
            results = batch_process_neurons(brain, neurons, lambda id, val: brain.injectCurrent(id, val * 2.0))
        
        Serialization::
        
            from pynlm import serialize, deserialize, NeuronId
            
            neuron_id = NeuronId(42)
            json_str = serialize(neuron_id)
            loaded_id = deserialize[NeuronId](json_str)
    )pbdoc";
    
    // Register custom exceptions with type hints
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_RuntimeError);
    py::register_exception<ResourceError>(m, "ResourceError", PyExc_RuntimeError);
    
    // ========================================================================
    // TEMPLATE-BASED ID CLASSES
    // =======================================================================
    
    // NeuronId with enhanced functionality
    py::class_<IDClass<NeuronId, uint64_t>>(m, "NeuronId", 
        R"pbdoc(Unique identifier for a neuron with enhanced functionality)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const IDClass<NeuronId, uint64_t>&>())  // Copy constructor
        .def_readwrite("value", &IDClass<NeuronId, uint64_t>::getValue)
        .def("index", &IDClass<NeuronId, uint64_t>::getIndex)
        .def("__eq__", &IDClass<NeuronId, uint64_t>::operator==)
        .def("__ne__", &IDClass<NeuronId, uint64_t>::operator!=)
        .def("__lt__", &IDClass<NeuronId, uint64_t>::operator<)
        .def("__le__", &IDClass<NeuronId, uint64_t>::operator<=)
        .def("__gt__", &IDClass<NeuronId, uint64_t>::operator>)
        .def("__ge__", &IDClass<NeuronId, uint64_t>::operator>=)
        .def("__hash__", [](const IDClass<NeuronId, uint64_t>& id) { return id.hash(); })
        .def("__repr__", [](const IDClass<NeuronId, uint64_t>& id) { return id.repr(); })
        .def("__str__", [](const IDClass<NeuronId, uint64_t>& id) { return id.toString(); })
        .def("from_string", &IDClass<NeuronId, uint64_t>::fromString, 
             py::arg("str"), "Create NeuronId from string representation")
        .def("is_valid", &IDClass<NeuronId, uint64_t>::isValid, "Check if ID is valid")
        .def("to_string", &IDClass<NeuronId, uint64_t>::toString, "Convert to string")
        .def("batch", &IDClass<NeuronId, uint64_t>::batch,
             py::arg("count"), py::arg("start") = 0,
             "Create a batch of NeuronId objects")
        .def("range", &IDClass<NeuronId, uint64_t>::range,
             py::arg("start"), py::arg("end"),
             "Create a range of NeuronId objects")
        .def("invalid", &IDClass<NeuronId, uint64_t>::invalid, "Get invalid NeuronId");
    
    // SynapseId with enhanced functionality
    py::class_<IDClass<SynapseId, uint64_t>>(m, "SynapseId",
        R"pbdoc(Unique identifier for a synapse with enhanced functionality)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const IDClass<SynapseId, uint64_t>&>())
        .def_readwrite("value", &IDClass<SynapseId, uint64_t>::getValue)
        .def("index", &IDClass<SynapseId, uint64_t>::getIndex)
        .def("__eq__", &IDClass<SynapseId, uint64_t>::operator==)
        .def("__ne__", &IDClass<SynapseId, uint64_t>::operator!=)
        .def("__hash__", [](const IDClass<SynapseId, uint64_t>& id) { return id.hash(); })
        .def("__repr__", [](const IDClass<SynapseId, uint64_t>& id) { return id.repr(); })
        .def("__str__", [](const IDClass<SynapseId, uint64_t>& id) { return id.toString(); })
        .def("from_string", &IDClass<SynapseId, uint64_t>::fromString,
             py::arg("str"), "Create SynapseId from string representation")
        .def("is_valid", &IDClass<SynapseId, uint64_t>::isValid, "Check if ID is valid")
        .def("batch", &IDClass<SynapseId, uint64_t>::batch,
             py::arg("count"), py::arg("start") = 0,
             "Create a batch of SynapseId objects");
    
    // RegionId with enhanced functionality
    py::class_<IDClass<RegionId, uint64_t>>(m, "RegionId",
        R"pbdoc(Unique identifier for a brain region with enhanced functionality)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const IDClass<RegionId, uint64_t>&>())
        .def_readwrite("value", &IDClass<RegionId, uint64_t>::getValue)
        .def("index", &IDClass<RegionId, uint64_t>::getIndex)
        .def("__eq__", &IDClass<RegionId, uint64_t>::operator==)
        .def("__ne__", &IDClass<RegionId, uint64_t>::operator!=)
        .def("__hash__", [](const IDClass<RegionId, uint64_t>& id) { return id.hash(); })
        .def("__repr__", [](const IDClass<RegionId, uint64_t>& id) { return id.repr(); })
        .def("__str__", [](const IDClass<RegionId, uint64_t>& id) { return id.toString(); })
        .def("from_string", &IDClass<RegionId, uint64_t>::fromString,
             py::arg("str"), "Create RegionId from string representation");
    
    // PopulationId with enhanced functionality
    py::class_<IDClass<PopulationId, uint64_t>>(m, "PopulationId",
        R"pbdoc(Unique identifier for a neuron population with enhanced functionality)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const IDClass<PopulationId, uint64_t>&>())
        .def_readwrite("value", &IDClass<PopulationId, uint64_t>::getValue)
        .def("index", &IDClass<PopulationId, uint64_t>::getIndex)
        .def("__eq__", &IDClass<PopulationId, uint64_t>::operator==)
        .def("__ne__", &IDClass<PopulationId, uint64_t>::operator!=)
        .def("__hash__", [](const IDClass<PopulationId, uint64_t>& id) { return id.hash(); })
        .def("__repr__", [](const IDClass<PopulationId, uint64_t>& id) { return id.repr(); })
        .def("__str__", [](const IDClass<PopulationId, uint64_t>& id) { return id.toString(); });
    
    // ========================================================================
    // ENUM CLASSES WITH ENHANCED DOCUMENTATION
    // =======================================================================
    
    py::enum_<NeuronType>(m, "NeuronType", 
        R"pbdoc(Neuron type enumeration with enhanced documentation)")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::enum_<SynapseType>(m, "SynapseType",
        R"pbdoc(Synapse type enumeration with enhanced documentation)")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage",
        R"pbdoc(Developmental stage enumeration with enhanced documentation)")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    py::enum_<FiringState>(m, "FiringState",
        R"pbdoc(Neuron firing state enumeration with enhanced documentation)")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();
    
    py::enum_<ActionType>(m, "ActionType",
        R"pbdoc(Action type enumeration with enhanced documentation)")
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
    
    py::enum_<MotorCommand>(m, "MotorCommand",
        R"pbdoc(Low-level motor command enumeration with enhanced documentation)")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();
    
    py::enum_<WorldObjectType>(m, "WorldObjectType",
        R"pbdoc(World object type enumeration with enhanced documentation)")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    // ========================================================================
    // ENHANCED CONFIG CLASS WITH VALIDATION AND TYPE HINTS
    // =======================================================================
    
    py::class_<Config>(m, "Config", 
        R"pbdoc(Advanced configuration class for NLM system with validation)")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", py::overload_cast<const std::string&>(&Config::has), py::arg("key"),
             "Check if a configuration key exists")
        .def("get", py::overload_cast<const std::string&>(&Config::get), py::arg("key"),
             "Get configuration value by key")
        .def("get", py::overload_cast<const std::string&, const std::string&>(&Config::get), 
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default fallback")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("validate", &ConfigValidator::validate, py::arg("config"),
             "Validate configuration structure")
        .def("validate_simulation", &ConfigValidator::validateSimulationParameters,
             py::arg("config"), "Validate simulation-specific parameters")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("to_dict", &ConfigValidator::extractSettings,
             py::arg("config"), py::arg("keys"),
             "Extract specific settings as a dictionary")
        .def("from_dict", [](Config& self, const py::dict& settings) {
            for (auto& item : settings) {
                std::string key = item.first.cast<std::string>();
                std::string value = item.second.cast<std::string>();
                self.set(key, value);
            }
        }, py::arg("settings"),
            "Load configuration from dictionary")
        .def("to_json", &Serializer::toPrettyJson, py::arg("config"),
             "Convert configuration to pretty JSON string")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
    
    // ========================================================================
    // ENHANCED SENSORY INPUT CLASSES
    // =======================================================================
    
    py::class_<SensoryInput>(m, "SensoryInput",
        R"pbdoc(Base class for sensory input with enhanced Python features)")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("getNormalizedData", [](const SensoryInput& self) {
            auto data = self.getData();
            float max_val = *std::max_element(data.begin(), data.end());
            if (max_val > 0) {
                for (auto& val : data) val /= max_val;
            }
            return data;
        }, "Get normalized data (0-1 range)");
    
    py::class_<Vision, SensoryInput>(m, "Vision",
        R"pbdoc(Enhanced Vision sensory input with Pythonic interface)")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data from numpy array or list")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of color channels")
        .def("getShape", [](const Vision& self) {
            return py::make_tuple(self.getWidth(), self.getHeight(), self.getChannels());
        }, "Get image shape as (width, height, channels) tuple")
        .def("getArray", [](Vision& self) {
            return python_bindings::vectorToPyArray(self.getData());
        }, "Get data as numpy array")
        .def("normalize", [](Vision& self) {
            auto data = self.getData();
            float max_val = *std::max_element(data.begin(), data.end());
            if (max_val > 0) {
                for (auto& val : data) val /= max_val;
            }
            self.setData(data);
        }, "Normalize pixel values to 0-1 range");
    
    py::class_<Audio, SensoryInput>(m, "Audio",
        R"pbdoc(Enhanced Audio sensory input with Pythonic interface)")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data from numpy array or list")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of samples")
        .def("getDuration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("getArray", [](Audio& self) {
            return python_bindings::vectorToPyArray(self.getData());
        }, "Get data as numpy array")
        .def("normalize", [](Audio& self) {
            auto data = self.getData();
            float max_val = *std::max_element(data.begin(), data.end());
            if (max_val > 0) {
                for (auto& val : data) val /= max_val;
            }
            self.setData(data);
        }, "Normalize audio amplitude to 0-1 range");
    
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals",
        R"pbdoc(Enhanced internal signals sensory input)")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("getSignals", &InternalSignals::getSignals,
             "Get all signals as vector")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getMean", [](const InternalSignals& self) {
            auto signals = self.getSignals();
            if (signals.empty()) return 0.0f;
            float sum = std::accumulate(signals.begin(), signals.end(), 0.0f);
            return sum / signals.size();
        }, "Get mean signal value")
        .def("getVariance", [](const InternalSignals& self) {
            auto signals = self.getSignals();
            if (signals.size() < 2) return 0.0f;
            float mean = self.getMean();
            float sum_sq = 0.0f;
            for (float val : signals) {
                sum_sq += (val - mean) * (val - mean);
            }
            return sum_sq / (signals.size() - 1);
        }, "Get signal variance")
        .def("getStatistics", &python_bindings::calculateStatistics,
             "Get statistical summary of signals");
    
    // ========================================================================
    // ENHANCED ACTION CLASS WITH COPY SUPPORT AND SERIALIZATION
    // =======================================================================
    
    py::class_<Action>(m, "Action",
        R"pbdoc(Enhanced Action representation for motor output with Python features)")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def(py::init<const Action&>())  // Copy constructor
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            return index < params.size() ? params[index] : 0.0f;
        }, py::arg("index"), "Get specific parameter by index")
        .def("addParameter", [](Action& self, float param) {
            auto params = self.getParameters();
            params.push_back(param);
            self.setParameters(params);
        }, py::arg("param"), "Add a parameter")
        .def("getName", &Action::getName, "Get action name")
        .def("clone", &Action::clone, "Create a deep copy of the action")
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["parameters"] = self.getParameters();
            return result;
        }, "Convert to dictionary")
        .def("from_dict", [](Action& self, const py::dict& data) {
            if (data.contains("type")) {
                self.setType(static_cast<ActionType>(data["type"].cast<int>()));
            }
            if (data.contains("parameters")) {
                self.setParameters(data["parameters"].cast<std::vector<float>>());
            }
        }, py::arg("data"), "Create action from dictionary")
        .def("serialize", &Serializer::toJson<Action>, py::arg("action"),
             "Serialize action to JSON string")
        .def("to_json", &Serializer::toJson<Action>, py::arg("action"),
             "Alias for serialize")
        .def("__repr__", [](const Action& a) {
            return "<Action: " + a.getName() + ", params=" + 
                   std::to_string(a.getParameters().size()) + ">";
        });
    
    // ========================================================================
    // ENHANCED WORLD OBJECT CLASS
    // =======================================================================
    
    py::class_<WorldObject>(m, "WorldObject",
        R"pbdoc(Enhanced World object representation)")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X position")
        .def_readwrite("y", &WorldObject::y, "Y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Whether object is active")
        .def("distance_to", [](const WorldObject& self, const WorldObject& other) {
            double dx = self.x - other.x;
            double dy = self.y - other.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("other"), "Calculate distance to another object")
        .def("is_near", [](const WorldObject& self, const WorldObject& other, float threshold) {
            return self.distance_to(other) < threshold;
        }, py::arg("other"), py::arg("threshold"), "Check if object is within threshold distance");
    
    // ========================================================================
    // ENHANCED AGENT BODY CLASS
    // =======================================================================
    
    py::class_<AgentBody>(m, "AgentBody",
        R"pbdoc(Enhanced agent body state)")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level")
        .def_readwrite("health", &AgentBody::health, "Health level")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("move", [](AgentBody& self, float dx, float dy) {
            self.x += dx;
            self.y += dy;
            self.isMoving = (dx != 0 || dy != 0);
        }, py::arg("dx"), py::arg("dy"), "Move agent by delta")
        .def("rotate", [](AgentBody& self, float angle) {
            self.orientation += angle;
            self.isTurning = (angle != 0);
        }, py::arg("angle"), "Rotate agent by angle radians")
        .def("reset", &AgentBody::reset, "Reset body to initial state")
        .def("speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        }, "Calculate speed");
    
    // ========================================================================
    // ENHANCED ACTION RESULT CLASS
    // =======================================================================
    
    py::class_<ActionResult>(m, "ActionResult",
        R"pbdoc(Enhanced Action result from world)")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Result message")
        .def("is_success", &ActionResult::success, "Check if action succeeded")
        .def("__bool__", &ActionResult::success, "Boolean conversion");
    
    // ========================================================================
    // ENHANCED SENSORY PERCEPT CLASS
    // =======================================================================
    
    py::class_<SensoryPercept>(m, "SensoryPercept",
        R"pbdoc(Enhanced Sensory percept data)")
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
             "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, 
             py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all signals as dictionary")
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp");
    
    // ========================================================================
    // ENHANCED SIMPLE WORLD CLASS WITH CONTEXT MANAGER SUPPORT
    // =======================================================================
    
    py::class_<SimpleWorld>(m, "SimpleWorld",
        R"pbdoc(Enhanced Simple 2D world for NLM simulation with context manager support)")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation")
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
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed");
    
    // ========================================================================
    // ENHANCED BRAIN CLASS WITH BATCH OPERATIONS
    // =======================================================================
    
    py::class_<Brain>(m, "Brain",
        R"pbdoc(Enhanced central neural simulation brain class with comprehensive API)")
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
        .def("batch_inject_current", 
             [](Brain& self, const std::vector<std::pair<NeuronId, double>>& injections) {
                 for (const auto& inj : injections) {
                     self.injectCurrent(inj.first, inj.second);
                 }
             }, py::arg("injections"),
             "Batch inject current into multiple neurons")
        .def("batch_process_neurons",
             &python_bindings::batchProcessNeurons,
             py::arg("brain"), py::arg("neuronIds"), py::arg("operation"),
             "Batch process neurons with a custom operation");
    
    // ========================================================================
    // ENHANCED AGENT BRAIN CLASS
    // =======================================================================
    
    py::class_<AgentBrain>(m, "AgentBrain",
        R"pbdoc(Enhanced Agent brain interface connecting NLM brain to world)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize with brain")
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
    
    // ========================================================================
    // FACTORY FUNCTIONS
    // =======================================================================
    
    m.def("createDefaultConfig", &python_bindings::createDefaultConfig,
         "Create a default configuration", py::return_value_policy::take_ownership);
    
    m.def("createBrain", &python_bindings::createBrain, py::arg("config"),
         "Create a new brain with configuration", py::return_value_policy::take_ownership);
    
    m.def("createSimpleWorld", &python_bindings::createWorld,
         "Create a new simple world", py::return_value_policy::take_ownership);
    
    m.def("createAgentBrain", &python_bindings::createAgentBrain, py::arg("brain"),
         "Create a new agent brain interface", py::return_value_policy::take_ownership);
    
    // ========================================================================
    // BATCH OPERATION HELPER FUNCTIONS
    // =======================================================================
    
    m.def("batch_process_neurons", &python_bindings::batchProcessNeurons,
         py::arg("brain"), py::arg("neuronIds"), py::arg("operation"),
         "Batch process neurons with a custom operation");
    
    // ========================================================================
    // SIMULATION SESSION WITH CONTEXT MANAGER
    // =======================================================================
    
    py::class_<SimulationSession>(m, "SimulationSession",
        R"pbdoc(Simulation session with context manager for resource management)")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>(),
             py::arg("brain"), py::arg("world"))
        .def("setBrain", &SimulationSession::setBrain, py::arg("brain"),
             "Set brain for session")
        .def("setWorld", &SimulationSession::setWorld, py::arg("world"),
             "Set world for session")
        .def("getBrain", &SimulationSession::getBrain,
             py::return_value_policy::reference_internal,
             "Get brain from session")
        .def("getWorld", &SimulationSession::getWorld,
             py::return_value_policy::reference_internal,
             "Get world from session")
        .def("step", &SimulationSession::step, py::arg("timestep") = 1.0,
             "Step simulation forward")
        .def("shutdown", &SimulationSession::shutdown,
             "Shutdown the session");
    
    // ========================================================================
    // ACTION BUILDER WITH FLUENT INTERFACE
    // =======================================================================
    
    py::class_<ActionBuilder>(m, "ActionBuilder",
        R"pbdoc(Builder pattern for Action objects with fluent interface)")
        .def(py::init<>())
        .def("setType", &ActionBuilder::setType, py::arg("type"),
             "Set action type")
        .def("setParameters", &ActionBuilder::setParameters, py::arg("params"),
             "Set action parameters")
        .def("addParameter", &ActionBuilder::addParameter, py::arg("param"),
             "Add a parameter")
        .def("build", &ActionBuilder::build, "Build the action")
        .def("reset", &ActionBuilder::reset, "Reset builder");
    
    // ========================================================================
    // ERROR CLASSES FOR PYTHON
    // =======================================================================
    
    py::class_<NLMError, std::runtime_error>(m, "NLMError")
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<ValidationError, NLMError>(m, "ValidationError")
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<ConfigurationError, NLMError>(m, "ConfigurationError")
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<ResourceError, NLMError>(m, "ResourceError")
        .def(py::init<const std::string&>(), py::arg("message"));
    
    // ========================================================================
    // CONTEXT MANAGER SUPPORT
    // =======================================================================
    
    py::class_<ContextManagerBase>(m, "ContextManagerBase")
        .def("enter", &ContextManagerBase::enter)
        .def("exit", [](ContextManagerBase& self, int ptype, const char* pname, const char* pval, const char* ptrace) {
            self.exit(ptype, pname, pval, ptrace);
        })
        .def("is_active", &ContextManagerBase::is_active_context);
    
    // ========================================================================
    // INVALID ID CONSTANTS
    // =======================================================================
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // ========================================================================
    // SERIALIZATION HELPER
    // =======================================================================
    
    m.def("serialize", &Serializer::toJson,
         py::arg("obj"), "Serialize object to JSON string");
    
    m.def("deserialize", &Serializer::fromJson,
         py::arg("json"), "Deserialize JSON string to object");
    
    // ========================================================================
    // PYTHONIC UTILITY FUNCTIONS
    // =======================================================================
    
    m.def("to_numpy", &python_bindings::vectorToPyArray,
         py::arg("vec"), "Convert vector to numpy array");
    
    m.def("from_numpy", &python_bindings::pyArrayToVector,
         py::arg("arr"), "Convert numpy array to vector");
    
    m.def("calculate_statistics", &python_bindings::calculateStatistics,
         py::arg("data"), "Calculate statistical summary of data");
    
    m.def("filter_neurons", &python_bindings::filterNeurons,
         py::arg("brain"), py::arg("predicate"), 
         "Filter neurons based on predicate function");
}

namespace nlm {}