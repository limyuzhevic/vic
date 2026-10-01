// Template-based ID class for NLM Python bindings

#ifndef NLM_BINDINGS_HPP
#define NLM_BINDINGS_HPP

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>
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

// Include NLM core headers
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
// INVALID ID CONSTANTS
// =============================================================================

extern const NeuronId INVALID_NEURON_ID;
extern const SynapseId INVALID_SYNAPSE_ID;
extern const RegionId INVALID_REGION_ID;
extern const PopulationId INVALID_POPULATION_ID;

// =============================================================================
// FUNCTION IMPLEMENTATIONS
// =============================================================================

inline const NeuronId INVALID_NEURON_ID = NeuronId();
inline const SynapseId INVALID_SYNAPSE_ID = SynapseId();
inline const RegionId INVALID_REGION_ID = RegionId();
inline const PopulationId INVALID_POPULATION_ID = PopulationId();

} // namespace nlm

#endif // NLM_BINDINGS_HPP
