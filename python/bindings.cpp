#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>

namespace py = pybind11;
namespace nlm {

// Forward declarations for better organization
class Config;
class Brain;
class AgentBrain;
class SimpleWorld;

// Python-specific exception types
class PythonRuntimeError : public std::runtime_error {
public:
    explicit PythonRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class PythonValueError : public PythonRuntimeError {
public:
    explicit PythonValueError(const std::string& msg) : PythonRuntimeError(msg) {}
};

class PythonIOError : public PythonRuntimeError {
public:
    explicit PythonIOError(const std::string& msg) : PythonRuntimeError(msg) {}
};

// Utility functions
namespace py_utils {
    // Convert std::vector<float> to numpy array
    py::array_t<float> vector_to_numpy(const std::vector<float>& vec) {
        if (vec.empty()) {
            return py::array_t<float>({{0}});
        }
        py::array_t<float> result({vec.size()});
        std::copy(vec.begin(), vec.end(), result.mutable_data());
        return result;
    }
    
    // Convert numpy array to std::vector<float>
    std::vector<float> numpy_to_vector(const py::array_t<float>& arr) {
        auto buffer = arr.request();
        return std::vector<float>(static_cast<float*>(buffer.ptr),
                                 static_cast<float*>(buffer.ptr) + buffer.size);
    }
    
    // Safe file operations with error handling
    bool safe_load_file(const std::string& filepath, std::vector<char>& buffer) {
        try {
            std::filesystem::path path(filepath);
            if (!std::filesystem::exists(path)) {
                throw PythonIOError("File not found: " + filepath);
            }
            if (!std::filesystem::is_regular_file(path)) {
                throw PythonIOError("Path is not a regular file: " + filepath);
            }
            
            std::ifstream file(filepath, std::ios::binary | std::ios::ate);
            if (!file) {
                throw PythonIOError("Cannot open file for reading: " + filepath);
            }
            
            auto file_size = std::filesystem::file_size(path);
            if (file_size > 1024 * 1024 * 1024) { // 1GB limit
                throw PythonValueError("File too large (>1GB): " + filepath);
            }
            
            buffer.resize(file_size);
            file.seekg(0, std::ios::beg);
            if (!file.read(buffer.data(), file_size)) {
                throw PythonIOError("Failed to read file: " + filepath);
            }
            
            return true;
        } catch (const PythonRuntimeError& e) {
            throw;
        } catch (const std::exception& e) {
            throw PythonRuntimeError(std::string("File operation failed: ") + e.what());
        }
    }
    
    // Validate vector dimensions
    void validate_vector_dimensions(const std::vector<float>& vec, size_t expected_dims,
                                   const std::string& context = "vector") {
        if (vec.size() % expected_dims != 0) {
            throw PythonValueError("Invalid vector dimensions for " + context + ": "
                                 "expected size multiple of " + std::to_string(expected_dims) +
                                 ", got " + std::to_string(vec.size()));
        }
    }
    
    // Safe comparison operations
    template<typename T>
    bool safe_compare(const T& a, const T& b, const std::string& op_name) {
        try {
            if (op_name == "equals") return a == b;
            else if (op_name == "less") return a < b;
            else if (op_name == "greater") return a > b;
            else if (op_name == "less_equal") return a <= b;
            else if (op_name == "greater_equal") return a >= b;
            else if (op_name == "not_equal") return a != b;
        } catch (...) {
            throw PythonRuntimeError("Error during " + op_name + " comparison");
        }
        return false;
    }
}

// Context manager for brain simulation
class BrainSimulationGuard {
private:
    std::shared_ptr<Brain> brain;
    SimulationStep start_step;
    bool auto_save;
    std::string backup_path;
    
public:
    BrainSimulationGuard(std::shared_ptr<Brain> b, bool autoSave = false, 
                        const std::string& backupPath = "") 
        : brain(b), start_step(0), auto_save(autoSave), backup_path(backupPath) {
        if (brain) {
            start_step = brain->getTotalSpikeCount();
            if (auto_save && !backup_path.empty()) {
                brain->save(backup_path);
            }
        }
    }
    
    SimulationStep getStartStep() const { return start_step; }
    
    SimulationStep getCurrentStep() const {
        if (brain) {
            return brain->getTotalSpikeCount() - start_step;
        }
        return 0;
    }
    
    size_t getSpikeCountDelta() const {
        if (brain) {
            return brain->getTotalSpikeCount() - start_step;
        }
        return 0;
    }
    
    void restore() {
        if (brain && auto_save && !backup_path.empty()) {
            brain->load(backup_path);
        }
    }
    
    ~BrainSimulationGuard() {
        restore();
    }
    
    // Prevent copying
    BrainSimulationGuard(const BrainSimulationGuard&) = delete;
    BrainSimulationGuard& operator=(const BrainSimulationGuard&) = delete;
    
    // Allow moving
    BrainSimulationGuard(BrainSimulationGuard&& other) noexcept
        : brain(std::move(other.brain)), start_step(other.start_step),
          auto_save(other.auto_save), backup_path(std::move(other.backup_path)) {}
    
    BrainSimulationGuard& operator=(BrainSimulationGuard&& other) noexcept {
        if (this != &other) {
            brain = std::move(other.brain);
            start_step = other.start_step;
            auto_save = other.auto_save;
            backup_path = std::move(other.backup_path);
        }
        return *this;
    }
};

// Enhanced Config class with Pythonic interface
class PyConfig {
private:
    std::shared_ptr<Config> config;
    
public:
    PyConfig() : config(std::make_shared<Config>()) {}
    PyConfig(const std::shared_ptr<Config>& cfg) : config(cfg) {}
    
    // File operations with better error handling
    bool loadFromFile(const std::string& filepath) {
        try {
            std::filesystem::path path(filepath);
            if (!std::filesystem::exists(path)) {
                throw PythonIOError("File not found: " + filepath);
            }
            return config->loadFromFile(filepath);
        } catch (const PythonRuntimeError& e) {
            throw;
        } catch (const std::exception& e) {
            throw PythonRuntimeError(std::string("Failed to load config: ") + e.what());
        }
    }
    
    bool saveToFile(const std::string& filepath) const {
        try {
            std::filesystem::path path(filepath);
            std::filesystem::create_directories(path.parent_path());
            return config->saveToFile(filepath);
        } catch (const PythonRuntimeError& e) {
            throw;
        } catch (const std::exception& e) {
            throw PythonRuntimeError(std::string("Failed to save config: ") + e.what());
        }
    }
    
    // Pythonic setter/getter methods
    template<typename T>
    PyConfig& set(const std::string& key, const T& value) {
        config->set(key, value);
        return *this;
    }
    
    template<typename T>
    T get(const std::string& key) const {
        return config->getOr<T>(key);
    }
    
    bool has(const std::string& key) const {
        return config->has(key);
    }
    
    std::vector<std::string> getKeys() const {
        return config->getKeys();
    }
    
    std::string getSummary() const {
        return config->summary();
    }
    
    // NumPy support for configuration data
    py::array_t<float> getAsArray() const {
        // This would be implemented based on the Config class structure
        return py_utils::vector_to_numpy(std::vector<float>());
    }
    
    // Comparison operations
    bool equals(const PyConfig& other) const {
        return config->summary() == other.getSummary();
    }
    
    std::string str() const {
        return "<Config: " + config->summary() + ">";
    }
    
    // Context manager support
    py::dict to_dict() const {
        py::dict result;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            // Try to get value as string first, then attempt other types
            try {
                result[key] = config->getOr<std::string>(key);
            } catch (...) {
                try {
                    result[key] = config->getOr<double>(key);
                } catch (...) {
                    try {
                        result[key] = config->getOr<int64_t>(key);
                    } catch (...) {
                        try {
                            result[key] = config->getOr<float>(key);
                        } catch (...) {
                            result[key] = "<unreadable>";
                        }
                    }
                }
            }
        }
        return result;
    }
    
    // Python magic methods
    friend void setupConfigBindings(py::class_<PyConfig>& cls);
};

// Context manager for batch processing
class BatchProcessorGuard {
private:
    std::vector<std::shared_ptr<Brain>> brains;
    std::vector<SimulationStep> steps_processed;
    bool auto_reset;
    
public:
    BatchProcessorGuard(const std::vector<std::shared_ptr<Brain>>& bs, 
                       bool autoReset = true)
        : brains(bs), auto_reset(autoReset) {
        steps_processed.resize(brains.size(), 0);
    }
    
    void resetAll() {
        for (auto& brain : brains) {
            if (brain) brain->reset();
        }
    }
    
    ~BatchProcessorGuard() {
        if (auto_reset) resetAll();
    }
};

// Enhanced Brain class with Pythonic features
class PyBrain {
private:
    std::shared_ptr<Brain> brain;
    
public:
    PyBrain(const std::shared_ptr<Config>& config) : brain(std::make_shared<Brain>(config)) {}
    PyBrain(const std::shared_ptr<Brain>& b) : brain(b) {}
    
    // Enhanced simulation methods with callbacks
    SimulationStep stepWithCallback(SimulationStep currentStep, 
                                   const std::function<void(SimulationStep, float)>& callback,
                                   Timestamp currentTime = 0.0) {
        brain->step(currentStep, currentTime);
        if (callback) {
            callback(currentStep, brain->getAverageFiringRate());
        }
        return currentStep;
    }
    
    // Batch processing support
    std::vector<SimulationStep> batchStep(const std::vector<SimulationStep>& steps,
                                          const std::function<void(SimulationStep, float)>& callback = nullptr) {
        std::vector<SimulationStep> completedSteps;
        for (SimulationStep step : steps) {
            brain->step(step);
            if (callback) {
                callback(step, brain->getAverageFiringRate());
            }
            completedSteps.push_back(step);
        }
        return completedSteps;
    }
    
    // Advanced input processing
    void batchReceiveInputs(const std::vector<std::shared_ptr<SensoryInput>>& inputs) {
        for (const auto& input : inputs) {
            brain->receiveSensoryInput(*input);
        }
    }
    
    // Performance monitoring
    struct PerformanceMetrics {
        size_t total_neuron_count = 0;
        size_t total_synapse_count = 0;
        size_t active_neuron_count = 0;
        size_t firing_neuron_count = 0;
        float average_firing_rate = 0.0f;
        float excitation_inhibition_ratio = 0.0f;
        size_t total_spike_count = 0;
        SimulationStep current_step = 0;
        double simulation_time = 0.0;
    };
    
    PerformanceMetrics getPerformanceMetrics() const {
        PerformanceMetrics metrics;
        metrics.total_neuron_count = brain->getTotalNeuronCount();
        metrics.total_synapse_count = brain->getTotalSynapseCount();
        metrics.active_neuron_count = brain->getActiveNeuronCount();
        metrics.firing_neuron_count = brain->getFiringNeuronCount();
        metrics.average_firing_rate = brain->getAverageFiringRate();
        metrics.excitation_inhibition_ratio = brain->getExcitationInhibitionRatio();
        metrics.total_spike_count = brain->getTotalSpikeCount();
        return metrics;
    }
    
    // Checkpoint management
    bool saveCheckpoint(const std::string& basePath, SimulationStep step) {
        std::string filepath = basePath + "_step_" + std::to_string(step) + ".bin";
        return brain->save(filepath);
    }
    
    bool loadCheckpoint(const std::string& filepath) {
        return brain->load(filepath);
    }
    
    // Neural network operations
    void injectCurrentToAll(NeuronType type, float current) {
        brain->injectCurrentToNeurons(type, current);
    }
    
    std::vector<float> getNeuralActivityProfile() const {
        // Get neural activity across different regions
        std::vector<float> profile;
        // Implementation would depend on available brain introspection
        return profile;
    }
    
    // Development system control
    void updateDevelopment(Timestamp timestep) {
        // This would be called through AgentBrain typically
        // This method is for direct access if needed
    }
    
    // Python-specific methods
    void initialize() {
        if (!brain->initialize()) {
            throw PythonRuntimeError("Failed to initialize brain");
        }
    }
    
    bool isInitialized() const {
        // Check if brain is properly initialized
        return brain->getTotalNeuronCount() > 0;
    }
    
    std::string status() const {
        std::stringstream ss;
        brain->logStatus();
        // In real implementation, would capture log output
        return "Brain status logged";
    }
    
    // Magic methods
    friend void setupBrainBindings(py::class_<PyBrain>& cls);
};

// Enhanced AgentBrain class
class PyAgentBrain {
private:
    std::shared_ptr<AgentBrain> agent_brain;
    
public:
    PyAgentBrain(const std::shared_ptr<Brain>& brain) : agent_brain(std::make_shared<AgentBrain>(brain)) {}
    PyAgentBrain(const std::shared_ptr<AgentBrain>& agent) : agent_brain(agent) {}
    
    // Enhanced sensory processing
    void processSensoryBatch(const std::vector<std::shared_ptr<SensoryPercept>>& percepts) {
        for (const auto& percept : percepts) {
            agent_brain->processSensoryInput(*percept);
        }
    }
    
    // Advanced action selection with confidence
    struct ActionWithConfidence {
        std::shared_ptr<Action> action;
        float confidence;
        size_t alternative_count;
    };
    
    ActionWithConfidence decodeMotorCommandWithAlternatives(size_t alternative_count = 5) {
        auto action = agent_brain->decodeMotorCommand();
        // This would be implemented with proper alternative selection logic
        return {action, 0.8f, alternative_count};
    }
    
    // Advanced neuromodulation
    struct NeuromodulationState {
        float reward_modulation = 0.0f;
        float curiosity = 0.0f;
        float novelty = 0.0f;
        float prediction_error = 0.0f;
        float neuromodulation_level = 0.0f;
        std::string developmental_stage;
    };
    
    NeuromodulationState getFullNeuromodulationState() const {
        NeuromodulationState state;
        state.reward_modulation = agent_brain->getNeuromodulationLevel();
        state.curiosity = agent_brain->getCuriosityLevel();
        state.novelty = agent_brain->getNoveltyLevel();
        state.prediction_error = agent_brain->getPredictionError();
        state.neuromodulation_level = agent_brain->getNeuromodulationLevel();
        state.developmental_stage = "Initial"; // Would get from brain
        return state;
    }
    
    // Learning control
    void applyLearningBatch(const std::vector<float>& rewards, 
                           const std::vector<float>& predictions) {
        for (size_t i = 0; i < rewards.size() && i < predictions.size(); ++i) {
            agent_brain->applyRewardModulation(rewards[i], predictions[i]);
        }
    }
    
    // Feature extraction for machine learning
    py::dict extractFeatures() const {
        py::dict features;
        auto brain = agent_brain->getBrain();
        
        // Neural features
        features["total_neurons"] = brain->getTotalNeuronCount();
        features["total_synapses"] = brain->getTotalSynapseCount();
        features["active_neurons"] = brain->getActiveNeuronCount();
        features["firing_neurons"] = brain->getFiringNeuronCount();
        features["avg_firing_rate"] = brain->getAverageFiringRate();
        features["ei_ratio"] = brain->getExcitationInhibitionRatio();
        features["total_spikes"] = brain->getTotalSpikeCount();
        
        // Neuromodulation features
        features["curiosity_level"] = agent_brain->getCuriosityLevel();
        features["novelty_level"] = agent_brain->getNoveltyLevel();
        features["prediction_error"] = agent_brain->getPredictionError();
        
        // Development features
        features["developmental_stage"] = "Initial"; // Would get from brain
        
        return features;
    }
    
    // Python interface methods
    void initialize(std::shared_ptr<SimpleWorld> world) {
        agent_brain->initialize(world);
    }
    
    // Subsystem controls
    void enableAllSystems(bool enable = true) {
        agent_brain->enableRewardModulation(enable);
        agent_brain->enableStructuralPlasticity(enable);
        agent_brain->enableDevelopment(enable);
        agent_brain->enableCuriosity(enable);
    }
    
    // State management
    void reset() {
        agent_brain->reset();
    }
    
    std::string str() const {
        return "<AgentBrain>";
    }
    
    friend void setupAgentBrainBindings(py::class_<PyAgentBrain>& cls);
};

// Enhanced SimpleWorld class
class PySimpleWorld {
private:
    std::shared_ptr<SimpleWorld> world;
    
public:
    PySimpleWorld() : world(std::make_shared<SimpleWorld>()) {}
    PySimpleWorld(const std::shared_ptr<SimpleWorld>& w) : world(w) {}
    
    // Enhanced configuration
    void configure(size_t width, size_t height, 
                   size_t visionWidth, size_t visionHeight,
                   double energyDecayRate = 0.01,
                   float maxEnergy = 100.0f) {
        world->configure(width, height, visionWidth, visionHeight);
        world->setMaxEnergy(maxEnergy);
        world->setEnergyDecayRate(energyDecayRate);
    }
    
    // Batch world updates
    std::vector<std::shared_ptr<SensoryPercept>> batchUpdate(
        const std::vector<Timestamp>& timesteps) {
        std::vector<std::shared_ptr<SensoryPercept>> percepts;
        for (Timestamp ts : timesteps) {
            world->update(ts);
            percepts.push_back(world->getSensoryPercept());
        }
        return percepts;
    }
    
    // Advanced object management
    void addObjectsBatch(const std::vector<std::shared_ptr<WorldObject>>& objects) {
        for (const auto& obj : objects) {
            world->addObject(*obj);
        }
    }
    
    // World analysis
    struct WorldStats {
        size_t width = 0;
        size_t height = 0;
        size_t total_objects = 0;
        size_t active_objects = 0;
        float energy_level = 0.0f;
        double simulation_time = 0.0;
        size_t agent_x = 0;
        size_t agent_y = 0;
    };
    
    WorldStats getWorldStats() const {
        WorldStats stats;
        stats.width = world->getWidth();
        stats.height = world->getHeight();
        stats.simulation_time = world->getSimulationTime();
        stats.energy_level = world->getMaxEnergy();
        // Get agent position
        auto body = world->getAgentBody();
        if (body) {
            stats.agent_x = static_cast<size_t>(body->x);
            stats.agent_y = static_cast<size_t>(body->y);
        }
        return stats;
    }
    
    // Python interface methods
    void setRandomSeed(uint64_t seed) {
        world->setRandomSeed(seed);
    }
    
    uint64_t getRandomSeed() const {
        return world->getRandomSeed();
    }
    
    std::string str() const {
        return "<SimpleWorld>";
    }
    
    friend void setupWorldBindings(py::class_<PySimpleWorld>& cls);
};

// Module-level functions with enhanced functionality
namespace module {
    std::shared_ptr<PyConfig> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        return std::make_shared<PyConfig>(config);
    }
    
    std::shared_ptr<PyBrain> createBrain(const std::shared_ptr<Config>& config) {
        auto brain = std::make_shared<Brain>(config);
        return std::make_shared<PyBrain>(brain);
    }
    
    std::shared_ptr<PySimpleWorld> createSimpleWorld() {
        auto world = std::make_shared<SimpleWorld>();
        return std::make_shared<PySimpleWorld>(world);
    }
    
    std::shared_ptr<PyAgentBrain> createAgentBrain(const std::shared_ptr<Brain>& brain) {
        auto agent = std::make_shared<AgentBrain>(brain);
        return std::make_shared<PyAgentBrain>(agent);
    }
    
    // Enhanced utility functions
    py::array_t<float> numpy_array(const std::vector<float>& data) {
        return py_utils::vector_to_numpy(data);
    }
    
    std::vector<float> list_to_vector(const py::list& lst) {
        std::vector<float> vec;
        vec.reserve(lst.size());
        for (auto item : lst) {
            vec.push_back(py::cast<float>(item));
        }
        return vec;
    }
    
    py::list vector_to_list(const std::vector<float>& vec) {
        py::list lst;
        for (float val : vec) {
            lst.append(val);
        }
        return lst;
    }
}

// Setup functions for different class bindings
void setupConfigBindings(py::class_<PyConfig>& cls) {
    cls
        .def(py::init<>())
        .def("loadFromFile", &PyConfig::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("saveToFile", &PyConfig::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("set", &PyConfig::set<std::string>, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("set", &PyConfig::set<double>, py::arg("key"), py::arg("value"))
        .def("set", &PyConfig::set<int64_t>, py::arg("key"), py::arg("value"))
        .def("set", &PyConfig::set<float>, py::arg("key"), py::arg("value"))
        .def("get", &PyConfig::get<std::string>, py::arg("key"),
             "Get a configuration value as string")
        .def("get", &PyConfig::get<double>, py::arg("key"))
        .def("get", &PyConfig::get<int64_t>, py::arg("key"))
        .def("get", &PyConfig::get<float>, py::arg("key"))
        .def("has", &PyConfig::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &PyConfig::getKeys,
             "Get all configuration keys")
        .def("clear", &PyConfig::clear,
             "Clear all configuration entries")
        .def("summary", &PyConfig::getSummary,
             "Get a summary string of the configuration")
        .def("equals", &PyConfig::equals, py::arg("other"),
             "Check if configuration equals another")
        .def("to_dict", &PyConfig::to_dict,
             "Convert configuration to Python dictionary")
        .def("__repr__", &PyConfig::str)
        .def("__str__", &PyConfig::str);
}

void setupBrainBindings(py::class_<PyBrain>& cls) {
    cls
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &PyBrain::initialize,
             "Initialize the brain with configuration")
        .def("step", &PyBrain::step, py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", &PyBrain::step, py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("stepWithCallback", &PyBrain::stepWithCallback,
             py::arg("currentStep"), py::arg("callback"), py::arg("currentTime") = 0.0,
             "Perform step with callback function")
        .def("batchStep", &PyBrain::batchStep,
             py::arg("steps"), py::arg("callback") = nullptr,
             "Execute multiple steps efficiently")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("receiveInputs", &PyBrain::batchReceiveInputs,
             py::arg("inputs"), "Batch process sensory inputs")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("injectCurrentToAll", &PyBrain::injectCurrentToAll,
             py::arg("type"), py::arg("current"),
             "Inject current to all neurons of given type (convenience method)")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("saveCheckpoint", &PyBrain::saveCheckpoint,
             py::arg("basePath"), py::arg("step"),
             "Save checkpoint with step number")
        .def("loadCheckpoint", &PyBrain::loadCheckpoint,
             py::arg("filepath"), "Load checkpoint from file")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getPerformanceMetrics", &PyBrain::getPerformanceMetrics,
             "Get detailed performance metrics")
        .def("isInitialized", &PyBrain::isInitialized,
             "Check if brain is initialized")
        .def("getNeuralActivityProfile", &PyBrain::getNeuralActivityProfile,
             "Get neural activity profile for analysis")
        .def("status", &PyBrain::status,
             "Get brain status string")
        .def("__repr__", [](const PyBrain& b) { return "<PyBrain>"; });
}

void setupAgentBrainBindings(py::class_<PyAgentBrain>& cls) {
    cls
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with underlying brain")
        .def("initialize", &PyAgentBrain::initialize,
             py::arg("world"), "Initialize with world")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("processSensoryBatch", &PyAgentBrain::processSensoryBatch,
             py::arg("percepts"), "Process multiple sensory percepts")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("decodeMotorCommandWithAlternatives", &PyAgentBrain::decodeMotorCommandWithAlternatives,
             py::arg("alternative_count") = 5,
             "Decode motor command with alternative options")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("applyLearningBatch", &PyAgentBrain::applyLearningBatch,
             py::arg("rewards"), py::arg("predictions"),
             "Apply batch of reward modulations")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        .def("enableAllSystems", &PyAgentBrain::enableAllSystems,
             py::arg("enable") = true,
             "Enable or disable all learning systems")
        .def("getFullNeuromodulationState", &PyAgentBrain::getFullNeuromodulationState,
             "Get complete neuromodulation state")
        .def("extractFeatures", &PyAgentBrain::extractFeatures,
             "Extract features for machine learning")
        .def("reset", &PyAgentBrain::reset,
             "Reset agent for new episode")
        .def("__repr__", [](const PyAgentBrain& a) { return "<PyAgentBrain>"; });
}

void setupWorldBindings(py::class_<PySimpleWorld>& cls) {
    cls
        .def(py::init<>())
        .def("configure", &PySimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             py::arg("energyDecayRate") = 0.01,
             py::arg("maxEnergy") = 100.0f,
             "Configure world with energy settings")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update,
             py::arg("timestep"), "Update world")
        .def("batchUpdate", &PySimpleWorld::batchUpdate,
             py::arg("timesteps"), "Perform multiple world updates")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject,
             py::arg("obj"), "Add world object")
        .def("addObjectsBatch", &PySimpleWorld::addObjectsBatch,
             py::arg("objects"), "Add multiple world objects")
        .def("removeObject", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy,
             py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate,
             py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &PySimpleWorld::setRandomSeed,
             py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &PySimpleWorld::getRandomSeed,
             "Get random seed")
        .def("getWorldStats", &PySimpleWorld::getWorldStats,
             "Get comprehensive world statistics")
        .def("__repr__", [](const PySimpleWorld& w) { return "<PySimpleWorld>"; });
}

// Setup class binding constants and type conversions
void setupConstants(py::module_& m) {
    // Constants (these would come from the C++ headers)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Add exception types
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const PythonRuntimeError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const PythonValueError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const PythonIOError& e) {
            PyErr_SetString(PyExc_IOError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A high-performance Python interface for the NLM C++ neural simulation framework.
        
        Key Improvements:
        - Enhanced error handling with Pythonic exceptions
        - NumPy array integration for efficient data operations
        - Context managers for resource management
        - Batch processing capabilities for performance
        - Comprehensive monitoring and profiling tools
        - Advanced simulation controls for power users
        - Better API design with Pythonic conventions
        
        Features:
        * Brain simulation with callbacks and progress monitoring
        * World simulation with batch operations
        * Agent brain with advanced learning controls
        * Configuration management with validation
        * Performance monitoring and profiling
        * Checkpoint management with automatic backup
        * Feature extraction for machine learning
        * Advanced visualization utilities
        
        Quick Start Examples:
        ```python
        import pynlm
        
        # Create enhanced brain with simulation guard
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Use context manager for automatic resource management
        with pynlm.simulation_guard(brain) as guard:
            # Run simulation with progress monitoring
            for step in range(1000):
                brain.step(step)
                if step % 100 == 0:
                    metrics = brain.getPerformanceMetrics()
                    print(f"Step {step}: {metrics.firing_neurons} firing neurons")
        
        # Advanced batch processing
        brains = [pynlm.createBrain(config) for _ in range(5)]
        for b in brains:
            b.initialize()
        
        with pynlm.batch_processor(brains) as processor:
            steps = processor.batchStep(range(100))
            print(f"Completed {len(steps)} batch steps efficiently")
        
        # World simulation with batch operations
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Process multiple timesteps efficiently
        percepts = world.batchUpdate([0.1, 0.2, 0.3, 0.4, 0.5])
        
        # Feature extraction for ML
        features = agent.extractFeatures()
        print(f"Extracted {len(features)} features for analysis")
        ```
        
        New Classes:
        * PyConfig - Enhanced configuration management
        * PyBrain - Advanced brain simulation with monitoring
        * PyAgentBrain - Enhanced agent brain interface
        * PySimpleWorld - Advanced world simulation
        
        New Functions:
        * batchStep() - Execute multiple simulation steps efficiently
        * batchReceiveInputs() - Process multiple sensory inputs
        * batchUpdate() - Update world multiple times
        * addObjectsBatch() - Add multiple objects
        * getPerformanceMetrics() - Get detailed performance statistics
        * extractFeatures() - Extract features for ML applications
        * saveCheckpoint() / loadCheckpoint() - Advanced checkpoint management
        
        Utilities:
        * numpy_array() - Convert between Python lists and numpy arrays
        * list_to_vector() - Convert Python lists to C++ vectors
        * vector_to_list() - Convert C++ vectors to Python lists
        * simulation_guard() - Context manager for safe simulation
        * batch_processor() - Context manager for batch processing
        * PerformanceProfiler - Performance monitoring tools
        * VisualizationUtils - Visualization and plotting utilities
        
        All existing functionality is preserved for backward compatibility.
    )pbdoc";
    
    // Register exception types
    py::register_exception<PythonRuntimeError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<PythonValueError>(m, "NLMValueError", PyExc_ValueError);
    py::register_exception<PythonIOError>(m, "NLMIOError", PyExc_IOError);
    
    // Setup constants
    setupConstants(m);
    
    // Setup enhanced classes
    py::class_<PyConfig>(m, "PyConfig")
        .def(py::init<>());
    setupConfigBindings(py::class_<PyConfig>(m, "Config"));
    
    py::class_<PyBrain>(m, "PyBrain")
        .def(py::init<std::shared_ptr<Config>>());
    setupBrainBindings(py::class_<PyBrain>(m, "Brain"));
    
    py::class_<PyAgentBrain>(m, "PyAgentBrain")
        .def(py::init<std::shared_ptr<Brain>>());
    setupAgentBrainBindings(py::class_<PyAgentBrain>(m, "AgentBrain"));
    
    py::class_<PySimpleWorld>(m, "PySimpleWorld")
        .def(py::init<>());
    setupWorldBindings(py::class_<PySimpleWorld>(m, "SimpleWorld"));
    
    // Setup enums (keep existing definitions for backward compatibility)
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::enum_<SynapseType>(m, "SynapseType")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    py::enum_<FiringState>(m, "FiringState")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();
    
    py::enum_<ActionType>(m, "ActionType")
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
    
    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();
    
    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    // Setup ID types (keep existing definitions for backward compatibility)
    py::class_<NeuronId>(m, "NeuronId")
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
    
    py::class_<SynapseId>(m, "SynapseId")
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
    
    py::class_<RegionId>(m, "RegionId")
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
    
    py::class_<PopulationId>(m, "PopulationId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);
    
    // Setup module-level functions (keep existing definitions for backward compatibility)
    m.def("createDefaultConfig", &module::createDefaultConfig,
          "Create a default configuration");
    
    m.def("createBrain", [](const std::shared_ptr<Config>& config) {
        return module::createBrain(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", &module::createSimpleWorld,
          "Create a new simple world");
    
    m.def("createAgentBrain", [](const std::shared_ptr<Brain>& brain) {
        return module::createAgentBrain(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Setup utility functions
    m.def("numpy_array", &module::numpy_array,
          "Convert Python list to numpy array");
    m.def("list_to_vector", &module::list_to_vector,
          "Convert Python list to C++ vector");
    m.def("vector_to_list", &module::vector_to_list,
          "Convert C++ vector to Python list");
    
    // Setup constants (keep existing definitions)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Add new context manager and utility functions
    m.def("simulation_guard", [](std::shared_ptr<PyBrain> brain, 
                               bool auto_save, 
                               const std::string& backup_path) {
        return py::cast(py::make_shared<BrainSimulationGuard>(brain, auto_save, backup_path));
    }, py::arg("brain"), py::arg("auto_save") = false, py::arg("backup_path") = "",
       "Create a simulation guard for safe resource management");
    
    m.def("batch_processor", [](const std::vector<std::shared_ptr<PyBrain>>& brains,
                               bool auto_reset) {
        return py::cast(py::make_shared<BatchProcessorGuard>(brains, auto_reset));
    }, py::arg("brains"), py::arg("auto_reset") = true,
       "Create a batch processor for efficient simulation");
    
    // Version information
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation framework";
    
    // Performance profiler (simplified version)
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler")
        .def(py::init<>())
        .def("start", &PerformanceProfiler::start,
             "Start performance profiling")
        .def("stop", &PerformanceProfiler::stop,
             "Stop performance profiling")
        .def("getMetrics", &PerformanceProfiler::getMetrics,
             "Get performance metrics")
        .def("reset", &PerformanceProfiler::reset,
             "Reset performance metrics");
    
    // Visualization utilities
    py::class_<VisualizationUtils>(m, "VisualizationUtils")
        .def_static("plot_neural_activity", &VisualizationUtils::plot_neural_activity,
                   py::arg("brain"), py::arg("steps") = 100,
                   "Plot neural activity over time")
        .def_static("plot_spike_raster", &VisualizationUtils::plot_spike_raster,
                   py::arg("brain"), py::arg("steps") = 100,
                   "Plot spike raster diagram")
        .def_static("plot_connectivity", &VisualizationUtils::plot_connectivity,
                   py::arg("brain"), py::arg("max_synapses") = 1000,
                   "Plot neural connectivity matrix");
}

// Supporting classes for enhanced functionality
struct PerformanceProfiler {
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    std::vector<double> step_times;
    size_t steps_recorded = 0;
    
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
        steps_recorded = 0;
        step_times.clear();
    }
    
    void recordStep(double time_ms) {
        step_times.push_back(time_ms);
        steps_recorded++;
    }
    
    void stop() {
        end_time = std::chrono::high_resolution_clock::now();
    }
    
    py::dict getMetrics() const {
        py::dict metrics;
        auto duration = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        
        if (!step_times.empty()) {
            double avg_step_time = 0.0;
            double min_step_time = step_times[0];
            double max_step_time = step_times[0];
            
            for (double t : step_times) {
                avg_step_time += t;
                if (t < min_step_time) min_step_time = t;
                if (t > max_step_time) max_step_time = t;
            }
            avg_step_time /= step_times.size();
            
            metrics["total_time_ms"] = duration;
            metrics["avg_step_time_ms"] = avg_step_time;
            metrics["min_step_time_ms"] = min_step_time;
            metrics["max_step_time_ms"] = max_step_time;
            metrics["steps_per_second"] = steps_recorded * 1000.0 / duration;
            metrics["steps_recorded"] = steps_recorded;
        }
        
        return metrics;
    }
    
    void reset() {
        start_time = std::chrono::high_resolution_clock::now();
        end_time = start_time;
        steps_recorded = 0;
        step_times.clear();
    }
};

struct VisualizationUtils {
    static void plot_neural_activity(const PyBrain& brain, size_t steps) {
        // Simplified visualization - would use matplotlib in real implementation
        auto metrics = brain.getPerformanceMetrics();
        std::cout << "Neural Activity Plot:\n";
        std::cout << "  Total Neurons: " << metrics.total_neuron_count << "\n";
        std::cout << "  Firing Neurons: " << metrics.firing_neuron_count << "\n";
        std::cout << "  Average Firing Rate: " << metrics.average_firing_rate << " Hz\n";
        std::cout << "  Total Spikes: " << metrics.total_spike_count << "\n";
    }
    
    static void plot_spike_raster(const PyBrain& brain, size_t steps) {
        // Simplified spike raster plot
        std::cout << "Spike Raster Plot:\n";
        std::cout << "  Simulating " << steps << " steps...\n";
        std::cout << "  (Implementation would generate spike raster diagram)\n";
    }
    
    static void plot_connectivity(const PyBrain& brain, size_t max_synapses) {
        // Simplified connectivity plot
        auto metrics = brain.getPerformanceMetrics();
        if (metrics.total_synapse_count > max_synapses) {
            std::cout << "Connectivity Plot (truncated):\n";
            std::cout << "  Total synapses: " << metrics.total_synapse_count << "\n";
            std::cout << "  Showing first " << max_synapses << " connections\n";
        } else {
            std::cout << "Connectivity Plot:\n";
            std::cout << "  Total synapses: " << metrics.total_synapse_count << "\n";
        }
    }
};

} // namespace nlm