#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <cmath>

// Include all necessary NLM headers
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Types/Types.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/sensory/Vision.hpp"
#include "src/sensory/Audio.hpp"
#include "src/sensory/InternalSignals.hpp"
#include "src/agent/Action.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/agent/SensoryPercept.hpp"

// Memory system headers
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/memory/Memory.hpp"
#include "src/memory/NeuralAssociativeMemory.hpp"

// Neuromodulation system headers
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/neuromodulation/Reward.hpp"

// Plasticity system headers
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/Hebbian.hpp"
#include "src/plasticity/StructuralPlasticity.hpp"

// Development system headers
#include "src/development/DevelopmentSystem.hpp"

// Cognition system headers
#include "src/cognition/PredictionSystem.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "src/cognition/AttentionalSelection.hpp"

namespace py = pybind11;
namespace nlm {

// Custom Python exception classes
class NLMException : public py::builtin_exception {
public:
    using py::builtin_exception::builtin_exception;
};

class ConfigurationError : public NLMException {
public:
    using py::builtin_exception::builtin_exception;
};

class MemoryError : public NLMException {
public:
    using py::builtin_exception::builtin_exception;
};

class NeuromodulationError : public NLMException {
public:
    using py::builtin_exception::builtin_exception;
};

class DevelopmentError : public NLMException {
public:
    using py::builtin_exception::builtin_exception;
};

// NEW EXCEPTION CLASSES
class WorldError : public NLMException {
public:
    using py::builtin_exception::builtin_exception;
};

class AgentError : public NLMException {
public:
    using py::builtin_exception::builtin_exception;
};

class ActionError : public NLMException {
public:
    using py::builtin_exception::builtin_exception;
};

class SensoryError : public NLMException {
public:
    using py::builtin_exception::builtin_exception;
};

class NoveltyError : public NLMException {
public:
    using py::builtin_exception::builtin_exception;
};

// Register all exceptions
void registerExceptionClasses(py::module_ &m) {
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_RuntimeError);
    py::register_exception<NeuromodulationError>(m, "NeuromodulationError", PyExc_RuntimeError);
    py::register_exception<DevelopmentError>(m, "DevelopmentError", PyExc_RuntimeError);
    // Register new exceptions
    py::register_exception<WorldError>(m, "WorldError", PyExc_RuntimeError);
    py::register_exception<AgentError>(m, "AgentError", PyExc_RuntimeError);
    py::register_exception<ActionError>(m, "ActionError", PyExc_RuntimeError);
    py::register_exception<SensoryError>(m, "SensoryError", PyExc_RuntimeError);
    py::register_exception<NoveltyError>(m, "NoveltyError", PyExc_RuntimeError);
}

// Enhanced validation helpers with comprehensive error messages
namespace validation {

    // Validate NeuronId and return value
    uint64_t validateNeuronId(const NeuronId& id) {
        if (id.value == UINT64_MAX) {
            throw NLMException("Invalid NeuronId: value is UINT64_MAX");
        }
        return id.value;
    }

    // Validate configuration key with enhanced error messages
    void validateConfigKey(const std::string& key) {
        if (key.empty()) {
            throw ConfigurationError("Configuration key cannot be empty: validation failed");
        }
        if (key.length() > 100) {
            throw ConfigurationError("Configuration key too long (max 100 characters, got " + 
                                   std::to_string(key.length()) + ")");
        }
        // Check for invalid characters
        for (char c : key) {
            if (!(std::isalnum(c) || c == '.' || c == '_' || c == '-' || c == '/')) {
                throw ConfigurationError("Invalid character in configuration key '" + key + "': only alphanumeric, dot, underscore, hyphen, and slash allowed");
            }
        }
    }

    // Validate numeric bounds for float values
    void validateFloat(float value, float min, float max, const std::string& paramName) {
        if (std::isnan(value)) {
            throw ConfigurationError("Invalid " + paramName + ": NaN is not allowed");
        }
        if (std::isinf(value)) {
            throw ConfigurationError("Invalid " + paramName + ": infinity is not allowed");
        }
        if (value < min) {
            throw ConfigurationError("Invalid " + paramName + ": must be >= " + std::to_string(min) + ", got " + std::to_string(value));
        }
        if (value > max) {
            throw ConfigurationError("Invalid " + paramName + ": must be <= " + std::to_string(max) + ", got " + std::to_string(value));
        }
    }

    // Validate numeric bounds for integer values
    void validateInt(int64_t value, int64_t min, int64_t max, const std::string& paramName) {
        if (value < min) {
            throw ConfigurationError("Invalid " + paramName + ": must be >= " + std::to_string(min) + ", got " + std::to_string(value));
        }
        if (value > max) {
            throw ConfigurationError("Invalid " + paramName + ": must be <= " + std::to_string(max) + ", got " + std::to_string(value));
        }
    }

    // Safe vector access with comprehensive bounds checking
    template<typename T>
    T& safeAt(std::vector<T>& vec, size_t index) {
        if (index >= vec.size()) {
            throw MemoryError("Vector index out of bounds: index " + std::to_string(index) + ", size " + 
                            std::to_string(vec.size()) + ". Cannot safely access element");
        }
        return vec[index];
    }

    // Validate array access bounds
    void validateArrayAccess(size_t index, size_t size, const std::string& context) {
        if (index >= size) {
            throw MemoryError("Array access out of bounds in '" + context + "': index " + 
                            std::to_string(index) + ", size " + std::to_string(size));
        }
    }

    // Validate memory allocation
    void validateMemoryAllocation(size_t size, size_t maxSize) {
        if (size > maxSize) {
            throw MemoryError("Memory allocation failed: requested " + std::to_string(size) + 
                            " bytes, but only " + std::to_string(maxSize) + " bytes available");
        }
    }

    // Validate sensory data dimensions
    void validateSensoryDimensions(size_t width, size_t height, size_t channels, const std::string& type) {
        if (width == 0 || height == 0 || channels == 0) {
            throw SensoryError("Invalid sensory dimensions for " + type + ": width, height, and channels must be > 0. Got: " +
                              std::to_string(width) + "x" + std::to_string(height) + "x" + std::to_string(channels));
        }
        if (width > 10000 || height > 10000) {
            throw SensoryError("Sensory dimensions too large for " + type + ": max 10000x10000, got " +
                              std::to_string(width) + "x" + std::to_string(height));
        }
    }

    // Validate action parameters
    void validateActionParameters(const std::vector<float>& parameters, ActionType type) {
        if (parameters.empty()) {
            throw ActionError("Invalid action: parameters vector cannot be empty for action type " + 
                            std::to_string(static_cast<int>(type)));
        }
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (std::isnan(parameters[i]) || std::isinf(parameters[i])) {
                throw ActionError("Invalid action parameter at index " + std::to_string(i) + ": " +
                                (std::isnan(parameters[i]) ? "NaN" : "infinity") + " is not allowed");
            }
        }
    }

    // Validate world object parameters
    void validateWorldObject(float x, float y, float radius, WorldObjectType type) {
        if (std::isnan(x) || std::isnan(y) || std::isnan(radius)) {
            throw WorldError("Invalid world object coordinates or radius: NaN values not allowed. Got: x=" + 
                           std::to_string(x) + ", y=" + std::to_string(y) + ", radius=" + std::to_string(radius));
        }
        if (radius <= 0.0f) {
            throw WorldError("Invalid world object radius: must be > 0, got " + std::to_string(radius));
        }
        if (x < 0.0f || y < 0.0f) {
            throw WorldError("Invalid world object position: coordinates must be >= 0. Got: x=" + 
                           std::to_string(x) + ", y=" + std::to_string(y));
        }
    }

    // Validate neuromodulation levels
    void validateNeuromodulationLevel(float level, const std::string& type) {
        if (std::isnan(level) || std::isinf(level)) {
            throw NeuromodulationError("Invalid neuromodulation level for " + type + ": " +
                                      (std::isnan(level) ? "NaN" : "infinity") + " is not allowed");
        }
        if (level < 0.0f || level > 1.0f) {
            throw NeuromodulationError("Invalid neuromodulation level for " + type + ": must be in [0.0, 1.0], got " + 
                                      std::to_string(level));
        }
    }

    // Validate developmental stage values
    void validateDevelopmentalStage(const DevelopmentalStage& stage) {
        if (stage == DevelopmentalStage::Initial && !stageIsValid(stage)) {
            throw DevelopmentError("Invalid developmental stage: Initial stage not properly initialized");
        }
    }

    // Helper to check if developmental stage is valid
    bool stageIsValid(const DevelopmentalStage& stage) {
        switch (stage) {
            case DevelopmentalStage::Initial:
            case DevelopmentalStage::CriticalPeriod:
            case DevelopmentalStage::Maturation:
            case DevelopmentalStage::Adult:
            case DevelopmentalStage::Aging:
                return true;
            default:
                return false;
        }
    }

    // Validate novelty level
    void validateNoveltyLevel(float novelty) {
        if (std::isnan(novelty) || std::isinf(novelty)) {
            throw NoveltyError("Invalid novelty level: " + (std::isnan(novelty) ? "NaN" : "infinity") + " is not allowed");
        }
        if (novelty < 0.0f || novelty > 1.0f) {
            throw NoveltyError("Invalid novelty level: must be in [0.0, 1.0], got " + std::to_string(novelty));
        }
    }
}

// Context manager for Brain simulation cleanup
class BrainSimulationContext {
public:
    BrainSimulationContext(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void __enter__() {
        if (!brain_) {
            throw NLMException("Cannot enter context: brain pointer is null");
        }
        brain_->initialize();
    }
    
    void __exit__(int /*type*/, py::object /*value*/, py::object /*traceback*/) {
        if (brain_) {
            brain_->reset();
        }
    }
    
    std::shared_ptr<Brain> getBrain() { 
        if (!brain_) {
            throw NLMException("Cannot get brain: brain pointer is null");
        }
        return brain_; 
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Enhanced BrainBuilder with comprehensive error handling
class BrainBuilder {
public:
    BrainBuilder() {
        config_ = std::make_shared<Config>();
        if (!config_) {
            throw MemoryError("Failed to allocate configuration in BrainBuilder constructor");
        }
    }
    
    // Set basic configuration values with validation
    BrainBuilder& setWindowSize(int width, int height) {
        validation::validateInt(width, 1, 10000, "window width");
        validation::validateInt(height, 1, 10000, "window height");
        config_->set("window.width", width);
        config_->set("window.height", height);
        return *this;
    }
    
    BrainBuilder& setSimulationParameters(double timestep, size_t maxSteps) {
        validation::validateFloat(static_cast<float>(timestep), 0.001f, 1.0f, "timestep");
        validation::validateInt(static_cast<int64_t>(maxSteps), 1, 1000000, "maxSteps");
        config_->set("simulation.timestep", timestep);
        config_->set("simulation.max_steps", maxSteps);
        return *this;
    }
    
    BrainBuilder& enableNeuromodulation(bool enable) {
        config_->set("neuromodulation.enabled", enable);
        return *this;
    }
    
    BrainBuilder& enableStructuralPlasticity(bool enable) {
        config_->set("plasticity.structural.enabled", enable);
        return *this;
    }
    
    BrainBuilder& enableDevelopment(bool enable) {
        config_->set("development.enabled", enable);
        return *this;
    }
    
    BrainBuilder& enableCuriosity(bool enable) {
        config_->set("curiosity.enabled", enable);
        return *this;
    }
    
    // Load configuration from file with enhanced error handling
    BrainBuilder& loadFromFile(const std::string& filepath) {
        validation::validateConfigKey(filepath);
        if (filepath.empty()) {
            throw ConfigurationError("Configuration file path cannot be empty");
        }
        if (!config_->loadFromFile(filepath)) {
            throw ConfigurationError("Failed to load configuration from file: " + filepath + ". File may be corrupted or inaccessible.");
        }
        return *this;
    }
    
    // Load configuration from command line with enhanced error handling
    BrainBuilder& loadFromArgs(int argc, char** argv) {
        if (argc == 0) {
            throw ConfigurationError("Command line argument count cannot be zero");
        }
        if (!argv) {
            throw ConfigurationError("Command line argument vector is null");
        }
        if (!config_->loadFromArgs(argc, argv)) {
            throw ConfigurationError("Failed to load configuration from command line arguments. Check for valid configuration format.");
        }
        return *this;
    }
    
    // Build and return the Brain with error checking
    std::shared_ptr<Brain> build() {
        auto brain = std::make_shared<Brain>(config_);
        if (!brain) {
            throw MemoryError("Failed to allocate Brain instance");
        }
        return brain;
    }
    
    // Build and initialize the Brain with comprehensive error checking
    std::shared_ptr<Brain> buildAndInitialize() {
        auto brain = build();
        if (!brain->initialize()) {
            throw NLMException("Failed to initialize brain: initialization returned false. Check configuration validity.");
        }
        return brain;
    }
    
    // Get the configuration
    std::shared_ptr<Config> getConfig() { return config_; }
    
private:
    std::shared_ptr<Config> config_;
};

// Enhanced BrainPythonWrapper with comprehensive error handling
class BrainPythonWrapper {
public:
    explicit BrainPythonWrapper(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (!brain) {
            throw AgentError("Cannot create BrainPythonWrapper: brain pointer is null");
        }
    }
    
    // Fluent interface for step operations with validation
    BrainPythonWrapper& step(SimulationStep step) {
        if (!brain_) {
            throw AgentError("Cannot perform step: brain pointer is null");
        }
        brain_->step(step);
        return *this;
    }
    
    BrainPythonWrapper& stepWithTime(SimulationStep step, Timestamp time) {
        if (!brain_) {
            throw AgentError("Cannot perform stepWithTime: brain pointer is null");
        }
        brain_->step(step, time);
        return *this;
    }
    
    BrainPythonWrapper& injectCurrent(NeuronId neuron, MembranePotential current) {
        if (!brain_) {
            throw AgentError("Cannot inject current: brain pointer is null");
        }
        brain_->injectCurrent(neuron, current);
        return *this;
    }
    
    BrainPythonWrapper& injectCurrentToNeurons(NeuronType type, MembranePotential current) {
        if (!brain_) {
            throw AgentError("Cannot inject current to neurons: brain pointer is null");
        }
        brain_->injectCurrentToNeurons(type, current);
        return *this;
    }
    
    // Brain operations with error checking
    BrainPythonWrapper& receiveSensoryInput(const SensoryInput& input) {
        if (!brain_) {
            throw AgentError("Cannot receive sensory input: brain pointer is null");
        }
        brain_->receiveSensoryInput(input);
        return *this;
    }
    
    Action produceAction() {
        if (!brain_) {
            throw AgentError("Cannot produce action: brain pointer is null");
        }
        auto actionPtr = brain_->produceAction();
        if (!actionPtr) {
            throw ActionError("Brain produced null action: action production failed");
        }
        return *actionPtr;
    }
    
    BrainPythonWrapper& reset() {
        if (brain_) {
            brain_->reset();
        }
        return *this;
    }
    
    // Configuration operations with error checking
    BrainPythonWrapper& saveState(const std::string& filepath) {
        if (!brain_) {
            throw AgentError("Cannot save state: brain pointer is null");
        }
        if (!brain_->save(filepath)) {
            throw MemoryError("Failed to save brain state to file: " + filepath + ". Disk may be full or file may be inaccessible.");
        }
        return *this;
    }
    
    bool loadState(const std::string& filepath) {
        if (!brain_) {
            throw AgentError("Cannot load state: brain pointer is null");
        }
        return brain_->load(filepath);
    }
    
    // Development system operations
    BrainPythonWrapper& develop() {
        if (brain_) {
            brain_->develop();
        }
        return *this;
    }
    
    BrainPythonWrapper& updatePlasticity() {
        if (brain_) {
            brain_->updatePlasticity();
        }
        return *this;
    }
    
    // Statistical operations
    float getExcitationInhibitionRatio() const {
        if (!brain_) {
            throw AgentError("Cannot get excitation/inhibition ratio: brain pointer is null");
        }
        return brain_->getExcitationInhibitionRatio();
    }
    
    size_t getTotalSpikeCount() const {
        if (!brain_) {
            throw AgentError("Cannot get total spike count: brain pointer is null");
        }
        return brain_->getTotalSpikeCount();
    }
    
    size_t getPendingSpikeEventCount() const {
        if (!brain_) {
            throw AgentError("Cannot get pending spike event count: brain pointer is null");
        }
        return brain_->getPendingSpikeEventCount();
    }
    
    size_t getActiveNeuronCount() const {
        if (!brain_) {
            throw AgentError("Cannot get active neuron count: brain pointer is null");
        }
        return brain_->getActiveNeuronCount();
    }
    
    size_t getFiringNeuronCount() const {
        if (!brain_) {
            throw AgentError("Cannot get firing neuron count: brain pointer is null");
        }
        return brain_->getFiringNeuronCount();
    }
    
    float getAverageFiringRate() const {
        if (!brain_) {
            throw AgentError("Cannot get average firing rate: brain pointer is null");
        }
        return brain_->getAverageFiringRate();
    }
    
    // Development operations
    DevelopmentalStage getDevelopmentalStage() const {
        if (!brain_) {
            throw AgentError("Cannot get developmental stage: brain pointer is null");
        }
        return brain_->getDevelopmentalStage();
    }
    
    BrainPythonWrapper& setDevelopmentalStage(DevelopmentalStage stage) {
        if (!brain_) {
            throw AgentError("Cannot set developmental stage: brain pointer is null");
        }
        brain_->setDevelopmentalStage(stage);
        return *this;
    }
    
    // Context manager support
    BrainSimulationContext createContext() {
        if (!brain_) {
            throw AgentError("Cannot create context: brain pointer is null");
        }
        return BrainSimulationContext(brain_);
    }
    
    // Get underlying brain
    std::shared_ptr<Brain> getBrain() { 
        if (!brain_) {
            throw AgentError("Cannot get brain: brain pointer is null");
        }
        return brain_; 
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Enhanced ConfigPythonWrapper with validation
class ConfigPythonWrapper {
public:
    explicit ConfigPythonWrapper(std::shared_ptr<Config> config) : config_(config) {
        if (!config) {
            throw ConfigurationError("Cannot create ConfigPythonWrapper: config pointer is null");
        }
    }
    
    // Get a value with type conversion and validation
    template<typename T>
    py::object getValue(const std::string& key) const {
        validation::validateConfigKey(key);
        auto value = config_->get<T>(key);
        if (value) {
            return py::cast(*value);
        } else {
            return py::none();
        }
    }
    
    // Set a value with type conversion and validation
    template<typename T>
    void setValue(const std::string& key, const T& value) {
        validation::validateConfigKey(key);
        config_->set(key, value);
    }
    
    // Check if key exists
    bool hasKey(const std::string& key) const {
        validation::validateConfigKey(key);
        return config_->has(key);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    // Clear configuration
    void clear() {
        config_->clear();
    }
    
    // Get configuration summary
    std::string summary() const {
        return config_->summary();
    }
    
    // Load from file with validation
    bool loadFromFile(const std::string& filepath) {
        validation::validateConfigKey(filepath);
        if (filepath.empty()) {
            throw ConfigurationError("Configuration file path cannot be empty");
        }
        return config_->loadFromFile(filepath);
    }
    
    // Load from arguments with validation
    bool loadFromArgs(int argc, char** argv) {
        if (argc == 0) {
            throw ConfigurationError("Command line argument count cannot be zero");
        }
        if (!argv) {
            throw ConfigurationError("Command line argument vector is null");
        }
        return config_->loadFromArgs(argc, argv);
    }
    
    // Save to file with validation
    bool saveToFile(const std::string& filepath) const {
        validation::validateConfigKey(filepath);
        return config_->saveToFile(filepath);
    }
    
    // Get the underlying config
    std::shared_ptr<Config> getConfig() { return config_; }
    
private:
    std::shared_ptr<Config> config_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Enhanced error handling with comprehensive validation and descriptive error messages.
    )pbdoc";

    // Register exception classes
    registerExceptionClasses(m);

    // NeuronId class
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

    // SynapseId class
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

    // RegionId class
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

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enum classes
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

    // Config class
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

    // SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("clone", &SensoryInput::clone, "Clone the sensory input");

    // Vision class
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

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
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

    // WorldObject class
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

    // AgentBody class
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

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
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

    // SimpleWorld class
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

    // Memory system classes
    py::class_<NeuralWorkingMemory>(m, "WorkingMemory", R"pbdoc(Neural working memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f)
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"))
        .def("retrieve", &NeuralWorkingMemory::retrieve)
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"))
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"))
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces)
        .def("getCapacity", &NeuralWorkingMemory::getCapacity)
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"))
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate)
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"))
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons)
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"))
        .def("runCompetition", &NeuralWorkingMemory::runCompetition)
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"))
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity);

    py::class_<NeuralEpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Neural episodic memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"))
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"))
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar,
             py::arg("sensoryPattern"), py::arg("maxResults") = 5)
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10)
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5)
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction,
             py::arg("action"), py::arg("maxResults") = 5)
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"))
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance,
             py::arg("episodeId"), py::arg("relevanceDelta"))
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"))
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount)
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"))
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward)
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"))
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"))
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled)
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"))
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"));

    py::class_<NeuralAssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Neural associative memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"))
        .def("associate", &NeuralAssociativeMemory::associate,
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f)
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience, py::arg("episode"))
        .def("retrieve", &NeuralAssociativeMemory::retrieve,
             py::arg("queryPattern"), py::arg("maxResults") = 5)
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("patternA"), py::arg("patternB"))
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("delta"))
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation,
             py::arg("cuePattern"), py::arg("steps") = 2)
        .def("clear", &NeuralAssociativeMemory::clear)
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount);

    // Neuromodulation classes
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Neuromodulatory signal)pbdoc")
        .def("getName", &Neuromodulator::getName)
        .def("getLevel", &Neuromodulator::getLevel)
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor)
        .def("update", &Neuromodulator::update, py::arg("dt"));

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"));

    py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getCuriosityLevel", &Curiosity::getCuriosityLevel);

    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(Novelty neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getNoveltyLevel", &Novelty::getNoveltyLevel);

    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(Prediction error signal)pbdoc")
        .def(py::init<>())
        .def("getPredictionErrorSignal", &PredictionError::getPredictionErrorSignal);

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward signal for reinforcement)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue)
        .def("setValue", &Reward::setValue, py::arg("value"))
        .def("add", &Reward::add, py::arg("delta"))
        .def("reset", &Reward::reset)
        .def("getHistory", &Reward::getHistory)
        .def("clearHistory", &Reward::clearHistory);

    // Plasticity system classes
    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-timing dependent plasticity)pbdoc")
        .def(py::init<>())
        .def("getSTDPStrength", &STDP::getSTDPStrength);

    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian plasticity)pbdoc")
        .def(py::init<>())
        .def("getHebbianStrength", &Hebbian::getHebbianStrength);

    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity)pbdoc")
        .def(py::init<>())
        .def("getStructuralPlasticityStrength", &StructuralPlasticity::getStructuralPlasticityStrength);

    // Development system class
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Developmental system)pbdoc")
        .def(py::init<>())
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalStage)
        .def("setDevelopmentalStage", &DevelopmentSystem::setDevelopmentalStage, py::arg("stage"))
        .def("getMaturationProgress", &DevelopmentSystem::getMaturationProgress)
        .def("getCriticalPeriodEnd", &DevelopmentSystem::getCriticalPeriodEnd)
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod)
        .def("getAge", &DevelopmentSystem::getAge);

    // Cognition system classes
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system)pbdoc")
        .def(py::init<>())
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("getPredictionConfidence", &PredictionSystem::getPredictionConfidence)
        .def("resetPrediction", &PredictionSystem::resetPrediction);

    py::class_<NeuralPlanner>(m, "Planner", R"pbdoc(Neural planner)pbdoc")
        .def(py::init<>())
        .def("getActionPlan", &NeuralPlanner::getActionPlan)
        .def("getPlanConfidence", &NeuralPlanner::getPlanConfidence)
        .def("executePlan", &NeuralPlanner::executePlan)
        .def("cancelPlan", &NeuralPlanner::cancelPlan);

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system)pbdoc")
        .def(py::init<>())
        .def("getActiveConcepts", &ConceptFormation::getActiveConcepts)
        .def("getConceptStrength", &ConceptFormation::getConceptStrength)
        .def("getConceptSimilarity", &ConceptFormation::getConceptSimilarity, py::arg("conceptA"), py::arg("conceptB"))
        .def("formConcept", &ConceptFormation::formConcept, py::arg("pattern"), py::arg("context"))
        .def("updateConcept", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("updateType"));

    py::class_<AttentionalSelection>(m, "Attention", R"pbdoc(Attentional selection)pbdoc")
        .def(py::init<>())
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions)
        .def("getWinners", &AttentionalSelection::getWinners)
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"))
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"))
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"))
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"))
        .def("releaseAttention", &AttentionalSelection::releaseAttention)
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"))
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"));

    // Brain class - Core neural simulation class
    py::class_<Brain, BrainPythonWrapper>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &BrainPythonWrapper::initialize, "Initialize the brain with configuration")
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
             "Inject current into all neurons of a specific type");

        // Memory system accessors
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get the episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get the associative memory system")

        // Neuromodulation access
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get the dopamine neuromodulation system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get the curiosity neuromodulation system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get the novelty neuromodulation system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get the prediction error signal")

        // Plasticity system access
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system")

        // Spike system access
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system")

        // Configuration access
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")

        // Development system access
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get the development system")

        // Cognition access
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get the prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get the concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get the attentional selection system")

        // Pending events
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get the number of pending spike events")

        // Action production
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")

        // Reset operations
        .def("reset", &Brain::reset, "Reset brain state")

        // Save/Load operations
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")

        // Region management
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

        // Statistics
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
             "Get the number of pending spike events")

        // Development operations
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")

        // Logging
        .def("logStatus", &Brain::logStatus,
             "Log brain status")

        // Context manager support
        .def("createContext", &BrainPythonWrapper::createContext,
             "Create a context manager for simulation cleanup")

        // Pythonic property accessors
        .def("getWorkingMemory", &BrainPythonWrapper::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the working memory system")
        .def("getEpisodicMemory", &BrainPythonWrapper::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get the episodic memory system")
        .def("getAssociativeMemory", &BrainPythonWrapper::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get the associative memory system")
        .def("getDopamine", &BrainPythonWrapper::getDopamine,
             py::return_value_policy::reference_internal,
             "Get the dopamine neuromodulation system")
        .def("getCuriosity", &BrainPythonWrapper::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get the curiosity neuromodulation system")
        .def("getNovelty", &BrainPythonWrapper::getNovelty,
             py::return_value_policy::reference_internal,
             "Get the novelty neuromodulation system")
        .def("getPredictionErrorSignal", &BrainPythonWrapper::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get the prediction error signal")
        .def("getSTDP", &BrainPythonWrapper::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity system")
        .def("getHebbian", &BrainPythonWrapper::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", &BrainPythonWrapper::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system")
        .def("getSpikeSystem", &BrainPythonWrapper::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system")
        .def("getDevelopmentSystem", &BrainPythonWrapper::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get the development system")
        .def("getPredictionSystem", &BrainPythonWrapper::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get the prediction system")
        .def("getPlanner", &BrainPythonWrapper::getPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner")
        .def("getConceptFormation", &BrainPythonWrapper::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get the concept formation system")
        .def("getAttention", &BrainPythonWrapper::getAttention,
             py::return_value_policy::reference_internal,
             "Get the attentional selection system")

        // Fluent interface methods
        .def("step", &BrainPythonWrapper::step, py::arg("step"),
             "Perform a simulation step (fluent interface)")
        .def("stepWithTime", &BrainPythonWrapper::stepWithTime, py::arg("step"), py::arg("time"),
             "Perform a simulation step with timestamp (fluent interface)")
        .def("injectCurrent", &BrainPythonWrapper::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron (fluent interface)")
        .def("injectCurrentToNeurons", &BrainPythonWrapper::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type (fluent interface)")
        .def("reset", &BrainPythonWrapper::reset,
             "Reset brain state (fluent interface)")
        .def("develop", &BrainPythonWrapper::develop,
             "Update development system (fluent interface)")
        .def("updatePlasticity", &BrainPythonWrapper::updatePlasticity,
             "Update plasticity rules (fluent interface)")
        .def("saveState", &BrainPythonWrapper::saveState, py::arg("filepath"),
             "Save brain state (fluent interface)")
        .def("setDevelopmentalStage", &BrainPythonWrapper::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage (fluent interface)")

        // Get underlying brain
        .def("getBrain", &BrainPythonWrapper::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain");

    // AgentBrain class
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

    // Brain builder
    py::class_<BrainBuilder>(m, "BrainBuilder", R"pbdoc(Builder for creating Brain instances with convenient configuration)pbdoc")
        .def(py::init<>())
        .def("setWindowSize", &BrainBuilder::setWindowSize,
             py::arg("width"), py::arg("height"),
             "Set window size")
        .def("setSimulationParameters", &BrainBuilder::setSimulationParameters,
             py::arg("timestep"), py::arg("maxSteps"),
             "Set simulation parameters")
        .def("enableNeuromodulation", &BrainBuilder::enableNeuromodulation,
             py::arg("enable"),
             "Enable neuromodulation")
        .def("enableStructuralPlasticity", &BrainBuilder::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable structural plasticity")
        .def("enableDevelopment", &BrainBuilder::enableDevelopment,
             py::arg("enable"),
             "Enable development")
        .def("enableCuriosity", &BrainBuilder::enableCuriosity,
             py::arg("enable"),
             "Enable curiosity")
        .def("loadFromFile", &BrainBuilder::loadFromFile,
             py::arg("filepath"),
             "Load configuration from file")
        .def("loadFromArgs", &BrainBuilder::loadFromArgs,
             py::arg("argc"), py::arg("argv"),
             "Load configuration from command line")
        .def("build", &BrainBuilder::build,
             "Build Brain instance")
        .def("buildAndInitialize", &BrainBuilder::buildAndInitialize,
             "Build and initialize Brain instance")
        .def("getConfig", &BrainBuilder::getConfig,
             "Get the configuration");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw ConfigurationError("Cannot create brain: config pointer is null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createBrainBuilder", []() -> BrainBuilder {
        return BrainBuilder();
    }, "Create a new brain builder");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw AgentError("Cannot create agent brain: brain pointer is null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Invalid ID sentinels
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

} // namespace nlm
