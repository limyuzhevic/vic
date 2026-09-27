#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <optional>
#include <sstream>

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

// Python exception classes for NLM
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMInvalidArgumentException : public NLMException {
public:
    explicit NLMInvalidArgumentException(const std::string& msg) 
        : NLMException("Invalid argument: " + msg) {}
};

class NLMBrainInitializationException : public NLMException {
public:
    explicit NLMBrainInitializationException(const std::string& msg)
        : NLMException("Brain initialization failed: " + msg) {}
};

class NLMMemoryAllocationException : public NLMException {
public:
    explicit NLMMemoryAllocationException(const std::string& msg)
        : NLMException("Memory allocation failed: " + msg) {}
};

class NLMValidationException : public NLMException {
public:
    explicit NLMValidationException(const std::string& msg)
        : NLMException("Validation failed: " + msg) {}
};

// Helper functions for validation and conversion
namespace nlm_python {
    
    // Validate brain configuration
    void validateBrainConfig(const Config& config) {
        if (!config.has("neuron_count")) {
            throw NLMInvalidArgumentException("Configuration must specify neuron_count");
        }
        auto neuronCountOpt = config.get<int64_t>("neuron_count");
        if (!neuronCountOpt.has_value()) {
            throw NLMInvalidArgumentException("neuron_count must be an integer");
        }
        if (neuronCountOpt.value() <= 0) {
            throw NLMInvalidArgumentException("neuron_count must be positive");
        }
        if (neuronCountOpt.value() > 1000000) {
            throw NLMInvalidArgumentException("neuron_count is too large (max: 1,000,000)");
        }
        
        if (!config.has("region_count")) {
            throw NLMInvalidArgumentException("Configuration must specify region_count");
        }
    }
    
    // Convert Python object to ConfigValue with type checking
    ConfigValue pythonToConfigValue(const py::object& obj) {
        if (py::isinstance<py::int_>(obj)) {
            return static_cast<int64_t>(obj.cast<int64_t>());
        } else if (py::isinstance<py::float_>(obj)) {
            return obj.cast<double>();
        } else if (py::isinstance<py::bool_>(obj)) {
            return obj.cast<bool>();
        } else if (py::isinstance<py::str>(obj)) {
            return obj.cast<std::string>();
        } else if (py::isinstance<py::list>(obj)) {
            py::list lst = obj.cast<py::list>();
            // Try to infer element type
            if (lst.empty()) {
                throw NLMInvalidArgumentException("Empty lists are not supported in config values");
            }
            
            // Check if all elements are integers
            bool allInts = true;
            for (size_t i = 0; i < lst.size(); ++i) {
                if (!py::isinstance<py::int_>(lst[i])) {
                    allInts = false;
                    break;
                }
            }
            if (allInts) {
                std::vector<int64_t> intList;
                intList.reserve(lst.size());
                for (size_t i = 0; i < lst.size(); ++i) {
                    intList.push_back(lst[i].cast<int64_t>());
                }
                return intList;
            }
            
            // Check if all elements are floats
            bool allFloats = true;
            for (size_t i = 0; i < lst.size(); ++i) {
                if (!py::isinstance<py::float_>(lst[i])) {
                    allFloats = false;
                    break;
                }
            }
            if (allFloats) {
                std::vector<double> floatList;
                floatList.reserve(lst.size());
                for (size_t i = 0; i < lst.size(); ++i) {
                    floatList.push_back(lst[i].cast<double>());
                }
                return floatList;
            }
            
            // Check if all elements are strings
            bool allStrings = true;
            for (size_t i = 0; i < lst.size(); ++i) {
                if (!py::isinstance<py::str>(lst[i])) {
                    allStrings = false;
                    break;
                }
            }
            if (allStrings) {
                std::vector<std::string> stringList;
                stringList.reserve(lst.size());
                for (size_t i = 0; i < lst.size(); ++i) {
                    stringList.push_back(lst[i].cast<std::string>());
                }
                return stringList;
            }
            
            throw NLMInvalidArgumentException("Cannot determine type of list elements in config value");
        }
        
        throw NLMInvalidArgumentException("Unsupported Python type for configuration value");
    }
    
    // Get optional value with Pythonic interface
    py::object getOptionalValue(const Config& config, const std::string& key, const py::object& defaultValue = py::none()) {
        auto valueOpt = config.get<ConfigValue>(key);
        if (!valueOpt.has_value()) {
            return defaultValue;
        }
        
        const ConfigValue& value = valueOpt.value();
        
        // Handle different value types
        if (std::holds_alternative<int64_t>(value)) {
            return py::int_(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return py::float_(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            return py::bool_(std::get<bool>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            return py::str(std::get<std::string>(value));
        } else if (std::holds_alternative<std::vector<int64_t>>(value)) {
            const auto& vec = std::get<std::vector<int64_t>>(value);
            py::list result;
            result.reserve(vec.size());
            for (auto val : vec) {
                result.append(py::int_(val));
            }
            return result;
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& vec = std::get<std::vector<double>>(value);
            py::list result;
            result.reserve(vec.size());
            for (auto val : vec) {
                result.append(py::float_(val));
            }
            return result;
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            const auto& vec = std::get<std::vector<std::string>>(value);
            py::list result;
            result.reserve(vec.size());
            for (const auto& val : vec) {
                result.append(py::str(val));
            }
            return result;
        }
        
        throw NLMException("Unsupported ConfigValue type");
    }
    
    // Validate simulation step
    void validateSimulationStep(SimulationStep step) {
        if (step < 0) {
            throw NLMInvalidArgumentException("Simulation step cannot be negative");
        }
        if (step > 1000000) {
            throw NLMInvalidArgumentException("Simulation step too large (max: 1,000,000)");
        }
    }
    
    // Validate timestamp
    void validateTimestamp(Timestamp timestamp) {
        if (timestamp < 0.0) {
            throw NLMInvalidArgumentException("Timestamp cannot be negative");
        }
        if (timestamp > 1000000.0) {
            throw NLMInvalidArgumentException("Timestamp too large (max: 1,000,000.0)");
        }
    }
    
    // Validate world dimensions
    void validateWorldDimensions(float width, float height, size_t visionWidth, size_t visionHeight) {
        if (width <= 0.0f) {
            throw NLMInvalidArgumentException("World width must be positive");
        }
        if (height <= 0.0f) {
            throw NLMInvalidArgumentException("World height must be positive");
        }
        if (visionWidth == 0 || visionHeight == 0) {
            throw NLMInvalidArgumentException("Vision dimensions cannot be zero");
        }
        if (visionWidth > static_cast<size_t>(width) * 2) {
            throw NLMInvalidArgumentException("Vision width too large relative to world width");
        }
        if (visionHeight > static_cast<size_t>(height) * 2) {
            throw NLMInvalidArgumentException("Vision height too large relative to world height");
        }
    }
    
    // Create default config with Pythonic interface
    std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.05);
        config->set("initial_weight_mean", 0.5);
        config->set("initial_weight_std", 0.1);
        config->set("v_thresh", -50.0);
        config->set("v_rest", -70.0);
        config->set("v_reset", -75.0);
        config->set("tau_mem", 20.0);
        config->set("tau_ref", 2.0);
        return config;
    }
    
    // Convert Python dict to Config
    std::shared_ptr<Config> configFromDict(const py::dict& dict) {
        auto config = std::make_shared<Config>();
        
        for (auto item : dict) {
            std::string key = py::cast<std::string>(item.first);
            ConfigValue configValue = nlm_python::pythonToConfigValue(item.second);
            config->set(key, configValue);
        }
        
        return config;
    }

    // Convert Config to Python dict
    py::dict configToDict(const Config& config) {
        py::dict dict;
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            dict[key.c_str()] = nlm_python::getOptionalValue(config, key, py::none());
        }
        return dict;
    }

    // Enhanced brain initialization with validation
    bool initializeBrainWithValidation(std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw NLMInvalidArgumentException("Brain pointer is null");
        }
        nlm_python::validateBrainConfig(*brain->getConfig());
        return brain->initialize();
    }

    // Enhanced step with validation
    void stepWithValidation(std::shared_ptr<Brain> brain, SimulationStep step, Timestamp time = 0.0) {
        nlm_python::validateSimulationStep(step);
        if (time > 0.0) {
            nlm_python::validateTimestamp(time);
        }
        if (time > 0.0) {
            brain->step(step, time);
        } else {
            brain->step(step);
        }
    }

    // Enhanced world configuration with validation
    void configureWorldWithValidation(std::shared_ptr<SimpleWorld> world, 
                                      float width, float height, 
                                      size_t visionWidth, size_t visionHeight) {
        nlm_python::validateWorldDimensions(width, height, visionWidth, visionHeight);
        world->configure(width, height, visionWidth, visionHeight);
    }

    // Enhanced agent world initialization
    void initializeAgentWithValidation(std::shared_ptr<AgentBrain> agent, 
                                       const std::shared_ptr<SimpleWorld>& world) {
        if (!agent) {
            throw NLMInvalidArgumentException("Agent pointer is null");
        }
        if (!world) {
            throw NLMInvalidArgumentException("World pointer is null");
        }
        agent->initialize(*world);
    }

    // Enhanced action creation
    std::unique_ptr<Action> createAction(ActionType type, const py::list& parameters) {
        std::vector<float> paramVec;
        for (size_t i = 0; i < parameters.size(); ++i) {
            paramVec.push_back(parameters[i].cast<float>());
        }
        return std::make_unique<Action>(type, paramVec);
    }

    // Enhanced vision creation
    std::unique_ptr<Vision> createVision(size_t width, size_t height, size_t channels = 3, 
                                        const py::list& data = py::none()) {
        auto vision = std::make_unique<Vision>(width, height, channels);
        if (!data.is_none()) {
            std::vector<float> dataVec;
            dataVec.reserve(data.size() * width * height * channels);
            for (size_t i = 0; i < data.size(); ++i) {
                if (py::isinstance<py::list>(data[i])) {
                    // Multi-dimensional data
                    py::list channel = data[i].cast<py::list>();
                    for (size_t j = 0; j < channel.size(); ++j) {
                        dataVec.push_back(channel[j].cast<float>());
                    }
                } else {
                    dataVec.push_back(data[i].cast<float>());
                }
            }
            vision->setData(dataVec);
        }
        return vision;
    }

    // Enhanced audio creation
    std::unique_ptr<Audio> createAudio(size_t sampleRate, size_t numSamples, 
                                      const py::list& data = py::none()) {
        auto audio = std::make_unique<Audio>(sampleRate, numSamples);
        if (!data.is_none()) {
            std::vector<float> dataVec;
            dataVec.reserve(data.size());
            for (size_t i = 0; i < data.size(); ++i) {
                dataVec.push_back(data[i].cast<float>());
            }
            audio->setData(dataVec);
        }
        return audio;
    }

    // Enhanced internal signals creation
    std::unique_ptr<InternalSignals> createInternalSignals(const py::list& signals = py::none()) {
        auto internal = std::make_unique<InternalSignals>();
        if (!signals.is_none()) {
            internal->clearSignals();
            for (size_t i = 0; i < signals.size(); ++i) {
                internal->addSignal(signals[i].cast<float>());
            }
        }
        return internal;
    }

    // Enhanced world object creation
    WorldObject createWorldObject(float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
        nlm_python::validateWorldDimensions(std::max(2.0f * radius, 1.0f), std::max(2.0f * radius, 1.0f), 0, 0);
        return WorldObject(x, y, type, value, radius);
    }

    // Enhanced agent body creation
    AgentBody createAgentBody(float x = 0.0f, float y = 0.0f, float orientation = 0.0f,
                             float velocityX = 0.0f, float velocityY = 0.0f,
                             float angularVelocity = 0.0f, float energy = 100.0f,
                             float health = 1.0f, float age = 0.0f) {
        AgentBody body;
        body.x = x;
        body.y = y;
        body.orientation = orientation;
        body.velocityX = velocityX;
        body.velocityY = velocityY;
        body.angularVelocity = angularVelocity;
        body.energy = energy;
        body.health = health;
        body.age = age;
        body.isMoving = (velocityX != 0.0f || velocityY != 0.0f);
        body.isTurning = (angularVelocity != 0.0f);
        body.lastActionTime = 0.0f;
        return body;
    }

    // Enhanced action result creation
    ActionResult createActionResult(float reward = 0.0f, bool success = false, 
                                   const std::string& message = "") {
        return ActionResult(reward, success, message);
    }

    // Enhanced sensory percept creation
    SensoryPercept createSensoryPercept(const py::object& vision = py::none(),
                                       const py::object& touch = py::none(),
                                       const py::object& internal = py::none(),
                                       const py::object& proprioception = py::none(),
                                       const py::object& audio = py::none()) {
        SensoryPercept percept;
        if (!vision.is_none()) {
            percept.setVision(py::cast<Vision>(vision));
        }
        if (!touch.is_none()) {
            // Convert touch data if needed
            auto touchVision = createVision(1, 1); // Simplified for touch
            percept.setTouch(touchVision);
        }
        if (!internal.is_none()) {
            auto internalSignals = createInternalSignals(py::cast<py::list>(internal));
            percept.setInternal(internalSignals);
        }
        if (!proprioception.is_none()) {
            auto proprioceptionVision = createVision(3, 1); // Simplified for proprioception
            percept.setProprioception(proprioceptionVision);
        }
        if (!audio.is_none()) {
            auto audioSignals = createAudio(44100, 1, py::cast<py::list>(audio));
            percept.setAudio(audioSignals);
        }
        return percept;
    }

    // Enhanced step for agent brain
    void processSensoryInputWithValidation(std::shared_ptr<AgentBrain> agent, 
                                           const SensoryPercept& percept) {
        if (!agent) {
            throw NLMInvalidArgumentException("Agent pointer is null");
        }
        agent->processSensoryInput(percept);
    }

    // Enhanced motor command decoding
    MotorCommand decodeMotorCommandWithValidation(std::shared_ptr<AgentBrain> agent) {
        if (!agent) {
            throw NLMInvalidArgumentException("Agent pointer is null");
        }
        return agent->decodeMotorCommand();
    }

    // Enhanced reward modulation
    void applyRewardModulationWithValidation(std::shared_ptr<AgentBrain> agent, 
                                             float reward, float predictedReward) {
        if (!agent) {
            throw NLMInvalidArgumentException("Agent pointer is null");
        }
        if (reward < -10.0f || reward > 10.0f) {
            throw NLMInvalidArgumentException("Reward value out of range (-10.0 to 10.0)");
        }
        if (predictedReward < -10.0f || predictedReward > 10.0f) {
            throw NLMInvalidArgumentException("Predicted reward value out of range (-10.0 to 10.0)");
        }
        agent->applyRewardModulation(reward, predictedReward);
    }

    // Enhanced development update
    void updateDevelopmentWithValidation(std::shared_ptr<AgentBrain> agent, double timestep) {
        if (!agent) {
            throw NLMInvalidArgumentException("Agent pointer is null");
        }
        if (timestep <= 0.0) {
            throw NLMInvalidArgumentException("Timestep must be positive");
        }
        if (timestep > 1.0) {
            throw NLMInvalidArgumentException("Timestep too large (max: 1.0)");
        }
        agent->updateDevelopment(timestep);
    }

    // Enhanced region operations
    RegionId addRegionWithValidation(std::shared_ptr<Brain> brain, const std::string& name = "") {
        if (!brain) {
            throw NLMInvalidArgumentException("Brain pointer is null");
        }
        // Validate name
        if (name.length() > 100) {
            throw NLMInvalidArgumentException("Region name too long (max: 100 characters)");
        }
        return brain->addRegion(name);
    }

    // Enhanced region ID operations
    std::string regionIdToString(const RegionId& regionId) {
        return "RegionId(" + std::to_string(regionId.value) + ")";
    }

    NeuronId neuronIdFromInt(uint64_t value) {
        return NeuronId(value);
    }

    SynapseId synapseIdFromInt(uint64_t value) {
        return SynapseId(value);
    }

    PopulationId populationIdFromInt(uint64_t value) {
        return PopulationId(value);
    }

    RegionId regionIdFromInt(uint64_t value) {
        return RegionId(value);
    }

    // Enhanced configuration operations
    void setConfigValue(Config& config, const std::string& key, const py::object& value) {
        ConfigValue configValue = nlm_python::pythonToConfigValue(value);
        config.set(key, configValue);
    }

    // Get configuration value with type conversion
    py::object getConfigValue(const Config& config, const std::string& key) {
        auto valueOpt = config.get<ConfigValue>(key);
        if (!valueOpt.has_value()) {
            return py::none();
        }
        return nlm_python::getOptionalValue(config, key, py::none());
    }

    // Check if config has key with case-insensitive matching
    bool hasConfigKey(const Config& config, const std::string& key) {
        // Simple case-insensitive check
        std::string lowerKey = key;
        for (char& c : lowerKey) {
            c = std::tolower(c);
        }
        for (const auto& configKey : config.getKeys()) {
            std::string lowerConfigKey = configKey;
            for (char& c : lowerConfigKey) {
                c = std::tolower(c);
            }
            if (lowerKey == lowerConfigKey) {
                return true;
            }
        }
        return false;
    }

    // Get config keys with filtering
    py::list getConfigKeys(const Config& config, const std::string& prefix = "") {
        py::list keys;
        auto allKeys = config.getKeys();
        for (const auto& key : allKeys) {
            if (prefix.empty() || key.find(prefix) == 0) {
                keys.append(key);
            }
        }
        return keys;
    }

    // Get config key-value pairs with filtering
    py::dict getConfigPairs(const Config& config, const std::string& prefix = "") {
        py::dict dict;
        auto allKeys = config.getKeys();
        for (const auto& key : allKeys) {
            if (prefix.empty() || key.find(prefix) == 0) {
                dict[key.c_str()] = nlm_python::getOptionalValue(config, key, py::none());
            }
        }
        return dict;
    }

    // Enhanced serialization with validation
    bool saveBrainWithValidation(const Brain& brain, const std::string& filepath) {
        if (filepath.empty()) {
            throw NLMInvalidArgumentException("Filepath cannot be empty");
        }
        if (filepath.length() > 1024) {
            throw NLMInvalidArgumentException("Filepath too long (max: 1024 characters)");
        }
        return brain.save(filepath);
    }

    // Enhanced loading with validation
    bool loadBrainWithValidation(Brain& brain, const std::string& filepath) {
        if (filepath.empty()) {
            throw NLMInvalidArgumentException("Filepath cannot be empty");
        }
        if (filepath.length() > 1024) {
            throw NLMInvalidArgumentException("Filepath too long (max: 1024 characters)");
        }
        return brain.load(filepath);
    }

    // Enhanced world object operations
    bool addObjectToWorld(SimpleWorld& world, const WorldObject& obj) {
        if (obj.radius <= 0.0f) {
            throw NLMInvalidArgumentException("Object radius must be positive");
        }
        if (obj.value < 0.0f) {
            throw NLMInvalidArgumentException("Object value must be non-negative");
        }
        world.addObject(obj);
        return true;
    }

    // Enhanced world operations
    bool removeObjectFromWorld(SimpleWorld& world, float x, float y) {
        world.removeObject(x, y);
        return true;
    }

    // Enhanced world object retrieval
    WorldObject* getObjectAtWorldPosition(SimpleWorld& world, float x, float y) {
        return world.getObjectAt(x, y);
    }

    // Enhanced world position validation
    bool isValidWorldPosition(SimpleWorld& world, float x, float y) {
        return world.isValidPosition(x, y);
    }

    // Enhanced world statistics
    py::dict getWorldStats(const SimpleWorld& world) {
        py::dict stats;
        stats["width"] = world.getWidth();
        stats["height"] = world.getHeight();
        stats["simulation_time"] = world.getSimulationTime();
        stats["max_energy"] = world.getMaxEnergy();
        stats["energy_decay_rate"] = world.getEnergyDecayRate();
        stats["random_seed"] = world.getRandomSeed();
        return stats;
    }

    // Enhanced brain statistics
    py::dict getBrainStats(const Brain& brain) {
        py::dict stats;
        stats["total_neurons"] = brain.getTotalNeuronCount();
        stats["total_synapses"] = brain.getTotalSynapseCount();
        stats["active_neurons"] = brain.getActiveNeuronCount();
        stats["firing_neurons"] = brain.getFiringNeuronCount();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        stats["total_spikes"] = brain.getTotalSpikeCount();
        stats["pending_spike_events"] = brain.getPendingSpikeEventCount();
        stats["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
        stats["region_count"] = brain.getRegionCount();
        return stats;
    }

    // Enhanced agent brain statistics
    py::dict getAgentBrainStats(const AgentBrain& agent) {
        py::dict stats;
        stats["neuromodulation_level"] = agent.getNeuromodulationLevel();
        stats["curiosity_level"] = agent.getCuriosityLevel();
        stats["novelty_level"] = agent.getNoveltyLevel();
        stats["prediction_error"] = agent.getPredictionError();
        stats["developmental_stage"] = static_cast<int>(agent.getDevelopmentalStage());
        stats["reward_modulation_enabled"] = agent.isRewardModulationEnabled();
        stats["structural_plasticity_enabled"] = agent.isStructuralPlasticityEnabled();
        stats["development_enabled"] = agent.isDevelopmentEnabled();
        stats["curiosity_enabled"] = agent.isCuriosityEnabled();
        return stats;
    }

    // Enhanced statistics functions
    py::dict getComprehensiveStats(const Brain& brain, const AgentBrain& agent, 
                                  const SimpleWorld& world) {
        py::dict stats;
        stats["brain"] = getBrainStats(brain);
        stats["agent"] = getAgentBrainStats(agent);
        stats["world"] = getWorldStats(world);
        return stats;
    }

    // Enhanced validation functions
    bool validateSimulationParameters(const Brain& brain, SimulationStep step, Timestamp time = 0.0) {
        try {
            validateSimulationStep(step);
            if (time > 0.0) {
                validateTimestamp(time);
            }
            return true;
        } catch (const NLMException& e) {
            return false;
        }
    }

    bool validateWorldParameters(float width, float height, size_t visionWidth, size_t visionHeight) {
        try {
            validateWorldDimensions(width, height, visionWidth, visionHeight);
            return true;
        } catch (const NLMException& e) {
            return false;
        }
    }

    bool validateConfigParameters(const Config& config) {
        try {
            validateBrainConfig(config);
            return true;
        } catch (const NLMException& e) {
            return false;
        }
    }

    // Helper function to create a complete agent setup
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>> 
    createCompleteAgentSetup(const py::dict& configDict = py::none()) {
        std::shared_ptr<Config> config;
        if (configDict.is_none()) {
            config = createDefaultConfig();
        } else {
            config = configFromDict(configDict);
        }
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        return std::make_tuple(brain, agent, world);
    }

    // Helper function to run a simulation step with automatic validation
    void runSimulationStepWithValidation(std::shared_ptr<Brain> brain, SimulationStep step, 
                                        Timestamp time = 0.0,
                                        const py::object& sensoryInput = py::none()) {
        stepWithValidation(brain, step, time);
        if (!sensoryInput.is_none()) {
            brain->receiveSensoryInput(py::cast<SensoryInput>(sensoryInput));
        }
    }

    // Helper function to get a simple simulation summary
    std::string getSimulationSummary(const Brain& brain, const AgentBrain& agent, 
                                    const SimpleWorld& world, SimulationStep step) {
        auto stats = getComprehensiveStats(brain, agent, world);
        std::stringstream ss;
        ss << "=== NLM Simulation Summary ===\n";
        ss << "Step: " << step << "\n";
        ss << "Brain Neurons: " << stats["brain"]["total_neurons"].cast<size_t>() << "\n";
        ss << "Brain Synapses: " << stats["brain"]["total_synapses"].cast<size_t>() << "\n";
        ss << "Brain Firing Rate: " << stats["brain"]["average_firing_rate"].cast<float>() << " Hz\n";
        ss << "Agent Curiosity: " << stats["agent"]["curiosity_level"].cast<float>() << "\n";
        ss << "Agent Novelty: " << stats["agent"]["novelty_level"].cast<float>() << "\n";
        ss << "World Time: " << stats["world"]["simulation_time"].cast<double>() << " s\n";
        ss << "==============================\n";
        return ss.str();
    }

    // Helper function to create a default configuration
    std::shared_ptr<Config> createConfiguration() {
        return createDefaultConfig();
    }

    // Helper function to convert Python list to vector
    template<typename T>
    std::vector<T> pythonListToVector(const py::list& list) {
        std::vector<T> vec;
        vec.reserve(list.size());
        for (size_t i = 0; i < list.size(); ++i) {
            vec.push_back(list[i].cast<T>());
        }
        return vec;
    }

    // Helper function to convert vector to Python list
    template<typename T>
    py::list vectorToPythonList(const std::vector<T>& vec) {
        py::list list;
        list.reserve(vec.size());
        for (const auto& item : vec) {
            list.append(item);
        }
        return list;
    }

    // Simulation environment structure
    struct SimulationEnvironment {
        std::shared_ptr<Brain> brain;
        std::shared_ptr<AgentBrain> agent;
        std::shared_ptr<SimpleWorld> world;
        
        void runSimulation(size_t numSteps, bool verbose = false) {
            for (size_t step = 0; step < numSteps; ++step) {
                runSimulationStepWithValidation(brain, step, 
                                               world->getSimulationTime(),
                                               world->getSensoryPercept());
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                agent->updateDevelopment(0.1);
                
                if (verbose && step % 100 == 0) {
                    std::cout << "Step " << step << ": " 
                             << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
                }
            }
        }
        
        std::string getSummary() const {
            return getSimulationSummary(*brain, *agent, *world, 0);
        }
    };

    SimulationEnvironment createSimulationEnvironment(const py::dict& configDict = py::none()) {
        SimulationEnvironment env;
        std::tie(env.brain, env.agent, env.world) = createCompleteAgentSetup(configDict);
        return env;
    }

    // Error handling utilities
    void setExceptionHandler(py::function handler) {
        py::set_exception_handler(handler);
    }

    py::function getExceptionHandler() {
        return py::get_exception_handler();
    }

    // Version information
    std::string getVersion() {
        return "NLM Python Bindings v1.0.0";
    }

    // Check if system is initialized
    bool isSystemReady() {
        return true; // Placeholder for actual system readiness check
    }
}; // namespace nlm_python

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Enhanced error handling and validation
        - Convenience functions for common operations
        - Advanced statistics and monitoring
        - Pythonic interfaces for better usability
        - Type conversion utilities
        - Simulation environment helpers
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const nlm_python::NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const nlm_python::NLMInvalidArgumentException& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const nlm_python::NLMBrainInitializationException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const nlm_python::NLMMemoryAllocationException& e) {
            PyErr_SetString(PyExc_MemoryError, e.what());
        } catch (const nlm_python::NLMValidationException& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        }
    });

    // Core identifier types with enhanced validation
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
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return "PopulationId(" + std::to_string(id.value) + ")";
        });

    // Enhanced enumerations with better documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Neuron that increases firing in postsynaptic neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Neuron that decreases firing in postsynaptic neurons")
        .value("Modulatory", NeuronType::Modulatory, "Neuron that modulates plasticity and learning")
        .value("Sensory", NeuronType::Sensory, "Neuron that receives sensory input")
        .value("Motor", NeuronType::Motor, "Neuron that produces motor output")
        .value("Internal", NeuronType::Internal, "Neuron for internal brain computations")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Synapse that increases postsynaptic firing")
        .value("Inhibitory", SynapseType::Inhibitory, "Synapse that decreases postsynaptic firing")
        .value("Modulatory", SynapseType::Modulatory, "Synapse that modulates plasticity")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse (gap junction)")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is not firing")
        .value("Active", FiringState::Active, "Neuron is firing")
        .value("Refractory", FiringState::Refractory, "Neuron is in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward action")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward action")
        .value("MoveLeft", ActionType::MoveLeft, "Move left action")
        .value("MoveRight", ActionType::MoveRight, "Move right action")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left action")
        .value("TurnRight", ActionType::TurnRight, "Turn right action")
        .value("Look", ActionType::Look, "Look action")
        .value("LookUp", ActionType::LookUp, "Look up action")
        .value("LookDown", ActionType::LookDown, "Look down action")
        .value("Interact", ActionType::Interact, "Interact action")
        .value("Eat", ActionType::Eat, "Eat action")
        .value("Drink", ActionType::Drink, "Drink action")
        .value("Rest", ActionType::Rest, "Rest action")
        .value("Wait", ActionType::Wait, "Wait action")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values();

    // Enhanced Config class with better error handling and validation
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
        })
        // Enhanced configuration methods with Pythonic interfaces
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            nlm_python::setConfigValue(self, key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with automatic type conversion")
        .def("get", [](const Config& self, const std::string& key) {
            return nlm_python::getConfigValue(self, key);
        }, py::arg("key"),
           "Get configuration value with automatic type conversion")
        .def("getWithDefault", [](const Config& self, const std::string& key, const py::object& defaultValue) {
            return nlm_python::getConfigValueWithDefault(self, key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = py::none(),
           "Get configuration value with default fallback")
        .def("toDict", &nlm_python::configToDict,
             "Convert configuration to Python dictionary")
        .def("fromDict", [](Config& self, const py::dict& dict) {
            auto config = nlm_python::configFromDict(dict);
            // Copy values from the temporary config
            auto keys = config->getKeys();
            for (const auto& key : keys) {
                self.set(key, *config->get<ConfigValue>(key));
            }
        }, py::arg("dict"),
           "Load configuration from Python dictionary")
        .def("hasKey", &nlm_python::configHasKey,
             "Check if configuration has key (case-insensitive)")
        .def("getKeys", &nlm_python::getConfigKeysList,
             "Get configuration keys as Python list")
        .def("getKeyValuePairs", &nlm_python::getConfigPairs,
             "Get configuration as key-value pairs dictionary")
        .def("filterKeys", &nlm_python::getConfigKeys,
             "Filter configuration keys by prefix")
        .def("toPythonDict", &nlm_python::configToDict,
             "Convert configuration to Python dictionary (alias)")
        .def("validate", &nlm_python::validateConfigParameters,
             "Validate configuration parameters");

    // Enhanced SensoryInput class with better type safety
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class with better data handling
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromPython", [](Vision& self, const py::list& data) {
            std::vector<float> dataVec = nlm_python::pythonListToVector<float>(data);
            self.setData(dataVec);
        }, py::arg("data"),
             "Set vision data from Python list")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getShape", [](const Vision& self) {
            py::list shape;
            shape.append(static_cast<size_t>(self.getWidth()));
            shape.append(static_cast<size_t>(self.getHeight()));
            shape.append(static_cast<size_t>(self.getChannels()));
            return shape;
        }, "Get vision shape as Python list");

    // Enhanced Audio class with better data handling
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromPython", [](Audio& self, const py::list& data) {
            std::vector<float> dataVec = nlm_python::pythonListToVector<float>(data);
            self.setData(dataVec);
        }, py::arg("data"),
             "Set audio data from Python list")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return static_cast<float>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds");

    // Enhanced InternalSignals class with better signal handling
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("addSignals", [](InternalSignals& self, const py::list& signals) {
            for (size_t i = 0; i < signals.size(); ++i) {
                self.addSignal(signals[i].cast<float>());
            }
        }, py::arg("signals"),
             "Add multiple signals from Python list")
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", [](const InternalSignals& self) {
            return py::cast(self.getData());
        }, "Get all signals as Python list")
        .def("getSignalCount", &InternalSignals::getDimensions,
             "Get number of signals");

    // Enhanced Action class with better parameter handling
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of action parameters")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " with " +
                   std::to_string(action.getParameters().size()) + " parameters>";
        })
        .def("toDict", [](const Action& action) {
            py::dict dict;
            dict["type"] = action.getType();
            dict["parameters"] = py::cast(action.getParameters());
            dict["name"] = action.getName();
            return dict;
        }, "Convert action to Python dictionary");

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
        .def_readwrite("active", &WorldObject::active)
        .def("getPosition", [](const WorldObject& obj) {
            py::list position;
            position.append(obj.x);
            position.append(obj.y);
            return position;
        }, "Get position as Python list")
        .def("setPosition", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, py::arg("x"), py::arg("y"),
             "Set position from coordinates")
        .def("getTypeName", [](const WorldObject& obj) {
            // Convert WorldObjectType enum to string
            switch (obj.type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        }, "Get object type as string")
        .def("isActive", &WorldObject::active)
        .def("activate", [](WorldObject& obj) { obj.active = true; })
        .def("deactivate", [](WorldObject& obj) { obj.active = false; })
        .def("getDistanceTo", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
             "Get distance to position");

    // Enhanced AgentBody class with better property access
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
            py::list position;
            position.append(body.x);
            position.append(body.y);
            return position;
        }, "Get position as Python list")
        .def("setPosition", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"),
             "Set position from coordinates")
        .def("getVelocity", [](const AgentBody& body) {
            py::list velocity;
            velocity.append(body.velocityX);
            velocity.append(body.velocityY);
            return velocity;
        }, "Get velocity as Python list")
        .def("setVelocity", [](AgentBody& body, float vx, float vy) {
            body.velocityX = vx;
            body.velocityY = vy;
            body.isMoving = (vx != 0.0f || vy != 0.0f);
        }, py::arg("velocity_x"), py::arg("velocity_y"),
             "Set velocity from components")
        .def("getAngularVelocity", &AgentBody::angularVelocity)
        .def("setAngularVelocity", &AgentBody::angularVelocity, py::arg("angular_velocity"),
             "Set angular velocity")
        .def("getSpeed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Get speed magnitude")
        .def("getEnergy", &AgentBody::energy)
        .def("setEnergy", &AgentBody::energy, py::arg("energy"),
             "Set energy level")
        .def("getHealth", &AgentBody::health)
        .def("setHealth", &AgentBody::health, py::arg("health"),
             "Set health level")
        .def("getAge", &AgentBody::age)
        .def("setAge", &AgentBody::age, py::arg("age"),
             "Set age")
        .def("isMoving", &AgentBody::isMoving)
        .def("isTurning", &AgentBody::isTurning)
        .def("getLastActionTime", &AgentBody::lastActionTime)
        .def("setLastActionTime", &AgentBody::lastActionTime, py::arg("time"),
             "Set last action time")
        .def("toDict", [](const AgentBody& body) {
            py::dict dict;
            dict["x"] = body.x;
            dict["y"] = body.y;
            dict["orientation"] = body.orientation;
            dict["velocityX"] = body.velocityX;
            dict["velocityY"] = body.velocityY;
            dict["angularVelocity"] = body.angularVelocity;
            dict["energy"] = body.energy;
            dict["health"] = body.health;
            dict["age"] = body.age;
            dict["isMoving"] = body.isMoving;
            dict["isTurning"] = body.isTurning;
            dict["lastActionTime"] = body.lastActionTime;
            return dict;
        }, "Convert agent body to Python dictionary");

    // Enhanced ActionResult class with better validation
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", &ActionResult::success)
        .def("getReward", &ActionResult::reward)
        .def("getMessage", &ActionResult::message)
        .def("toDict", [](const ActionResult& result) {
            py::dict dict;
            dict["reward"] = result.reward;
            dict["success"] = result.success;
            dict["message"] = result.message;
            return dict;
        }, "Convert action result to Python dictionary")
        .def("__repr__", [](const ActionResult& result) {
            std::stringstream ss;
            ss << "<ActionResult: reward=" << result.reward 
               << ", success=" << (result.success ? "true" : "false")
               << ", message='" << result.message << "'>";
            return ss.str();
        });

    // Enhanced SensoryPercept class with better data access
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
        .def("hasVision", [](const SensoryPercept& percept) {
            return percept.getVision() != nullptr;
        }, "Check if percept has vision data")
        .def("hasTouch", [](const SensoryPercept& percept) {
            return percept.getTouch() != nullptr;
        }, "Check if percept has touch data")
        .def("hasInternal", [](const SensoryPercept& percept) {
            return percept.getInternal() != nullptr;
        }, "Check if percept has internal signals")
        .def("hasProprioception", [](const SensoryPercept& percept) {
            return percept.getProprioception() != nullptr;
        }, "Check if percept has proprioception data")
        .def("hasAudio", [](const SensoryPercept& percept) {
            return percept.getAudio() != nullptr;
        }, "Check if percept has audio data")
        .def("toDict", [](const SensoryPercept& percept) {
            py::dict dict;
            auto vision = percept.getVision();
            if (vision) {
                py::dict visionDict;
                visionDict["width"] = vision->getWidth();
                visionDict["height"] = vision->getHeight();
                visionDict["channels"] = vision->getChannels();
                visionDict["data"] = py::cast(vision->getData());
                dict["vision"] = visionDict;
            }
            auto touch = percept.getTouch();
            if (touch) {
                py::dict touchDict;
                touchDict["width"] = touch->getWidth();
                touchDict["height"] = touch->getHeight();
                touchDict["channels"] = touch->getChannels();
                touchDict["data"] = py::cast(touch->getData());
                dict["touch"] = touchDict;
            }
            auto internal = percept.getInternal();
            if (internal) {
                dict["internal"] = py::cast(internal->getData());
            }
            auto proprioception = percept.getProprioception();
            if (proprioception) {
                py::dict proprioceptionDict;
                proprioceptionDict["width"] = proprioception->getWidth();
                proprioceptionDict["height"] = proprioception->getHeight();
                proprioceptionDict["channels"] = proprioception->getChannels();
                proprioceptionDict["data"] = py::cast(proprioception->getData());
                dict["proprioception"] = proprioceptionDict;
            }
            auto audio = percept.getAudio();
            if (audio) {
                py::dict audioDict;
                audioDict["sampleRate"] = audio->getSampleRate();
                audioDict["numSamples"] = audio->getNumSamples();
                audioDict["data"] = py::cast(audio->getData());
                dict["audio"] = audioDict;
            }
            dict["timestamp"] = percept.getTimestamp();
            return dict;
        }, "Convert sensory percept to Python dictionary")
        .def("__repr__", [](const SensoryPercept& percept) {
            std::stringstream ss;
            ss << "<SensoryPercept: vision=" << (percept.getVision() ? "true" : "false")
               << ", touch=" << (percept.getTouch() ? "true" : "false")
               << ", internal=" << (percept.getInternal() ? "true" : "false")
               << ", proprioception=" << (percept.getProprioception() ? "true" : "false")
               << ", audio=" << (percept.getAudio() ? "true" : "false") << ">";
            return ss.str();
        });

    // Enhanced SimpleWorld class with validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, float width, float height, 
                          size_t visionWidth, size_t visionHeight) {
            nlm_python::configureWorldWithValidation(self, width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world with validation")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", [](SimpleWorld& self, double timestep) {
            self.update(timestep);
        }, py::arg("timestep"),
             "Update world state")
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
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("toDict", &nlm_python::getWorldStats,
             "Get world statistics as Python dictionary")
        .def("__repr__", [](const SimpleWorld& world) {
            std::stringstream ss;
            ss << "<SimpleWorld: " << world.getWidth() << "x" << world.getHeight()
               << ", vision=" << world.getSensoryPercept().getVisionWidth() << "x"
               << world.getSensoryPercept().getVisionHeight()
               << ", time=" << world.getSimulationTime() << "s>";
            return ss.str();
        });

    // Enhanced Brain class with validation and advanced features
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", [](Brain& self) {
            return nlm_python::initializeBrainWithValidation(std::make_shared<Brain>(self.getConfig()));
        }, "Initialize the brain with validation")
        .def("step", [](Brain& self, SimulationStep step) {
            nlm_python::stepWithValidation(std::make_shared<Brain>(self.getConfig()), step);
        }, py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", [](Brain& self, SimulationStep step, Timestamp time) {
            nlm_python::stepWithValidation(std::make_shared<Brain>(self.getConfig()), step, time);
        }, py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into neurons of specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &nlm_python::saveBrainWithValidation,
             py::arg("filepath"), "Save brain state to file with validation")
        .def("load", &nlm_python::loadBrainWithValidation,
             py::arg("filepath"), "Load brain state from file with validation")
        .def("addRegion", &nlm_python::addRegionWithValidation,
             py::arg("name") = "", "Add a new neural region with validation")
        .def("getRegion", &nlm_python::brainGetRegionWithValidation,
             py::arg("id"), py::return_value_policy::reference_internal,
             "Get a region by ID with validation")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getStats", &nlm_python::brainGetStatsWithValidation,
             "Get brain statistics with validation")
        .def("isInitialized", [](const Brain& self) {
            return self.getTotalNeuronCount() > 0;
        }, "Check if brain is initialized")
        .def("getSimulationStep", [](Brain& self, SimulationStep step) {
            nlm_python::stepWithValidation(std::make_shared<Brain>(self.getConfig()), step);
        }, py::arg("currentStep"), "Perform a single simulation step")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system access")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"), "Apply neuromodulatory signals")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules")
        .def("develop", &Brain::develop,
             "Apply developmental changes")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random number generator")
        .def("toDict", &nlm_python::brainGetStatsWithValidation,
             "Get brain statistics as Python dictionary")
        .def("__repr__", [](const Brain& brain) {
            auto stats = nlm_python::brainGetStatsWithValidation(brain);
            std::stringstream ss;
            ss << "<Brain: " << stats["total_neurons"].cast<size_t>() << " neurons, "
               << stats["total_synapses"].cast<size_t>() << " synapses, "
               << stats["firing_neurons"].cast<size_t>() << " firing, "
               << stats["average_firing_rate"].cast<float>() << " Hz>";
            return ss.str();
        });

    // Enhanced AgentBrain class with validation
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", [](AgentBrain& self, const SimpleWorld& world) {
            nlm_python::initializeAgentWithValidation(std::make_shared<AgentBrain>(self.getBrain()), world);
        }, py::arg("world"), "Initialize with world with validation")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", [](AgentBrain& self, const SensoryPercept& percept) {
            nlm_python::processSensoryInputWithValidation(std::make_shared<AgentBrain>(self.getBrain()), percept);
        }, py::arg("percept"), "Process sensory percept with validation")
        .def("decodeMotorCommand", [](AgentBrain& self) {
            return nlm_python::decodeMotorCommandWithValidation(std::make_shared<AgentBrain>(self.getBrain()));
        }, "Decode motor command with validation")
        .def("applyRewardModulation", [](AgentBrain& self, float reward, float predictedReward) {
            nlm_python::applyRewardModulationWithValidation(std::make_shared<AgentBrain>(self.getBrain()), reward, predictedReward);
        }, py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation with validation")
        .def("updateDevelopment", [](AgentBrain& self, double timestep) {
            nlm_python::updateDevelopmentWithValidation(std::make_shared<AgentBrain>(self.getBrain()), timestep);
        }, py::arg("timestep"), "Update development system with validation")
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
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
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
        .def("getStats", &nlm_python::agentBrainGetStatsWithValidation,
             "Get agent brain statistics with validation")
        .def("toDict", &nlm_python::agentBrainGetStatsWithValidation,
             "Get agent brain statistics as Python dictionary (alias)")
        .def("__repr__", [](const AgentBrain& agent) {
            auto stats = nlm_python::agentBrainGetStatsWithValidation(agent);
            std::stringstream ss;
            ss << "<AgentBrain: curiosity=" << stats["curiosity_level"].cast<float>()
               << ", novelty=" << stats["novelty_level"].cast<float>()
               << ", neuromodulation=" << stats["neuromodulation_level"].cast<float>() << ">";
            return ss.str();
        });

    // Convenience functions with enhanced error handling
    m.def("createDefaultConfig", &nlm_python::createDefaultConfig,
          "Create a default configuration with validation")
    
    m.def("createConfigFromDict", [](const py::dict& dict) {
        return nlm_python::configFromDict(dict);
    }, py::arg("dict"), "Create configuration from Python dictionary")
    
    m.def("createConfigFromFile", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw NLMException("Failed to load configuration from file: " + filepath);
        }
        return config;
    }, py::arg("filepath"), "Create configuration from JSON file")
    
    m.def("createConfigFromArgs", [](int argc, char** argv) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromArgs(argc, argv)) {
            throw NLMException("Failed to load configuration from command line arguments");
        }
        return config;
    }, py::arg("argc"), py::arg("argv"), "Create configuration from command line arguments")
    
    // Enhanced creation functions with validation
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return nlm_python::createBrainWithValidation(config);
    }, py::arg("config"), "Create a new brain with configuration")
    
    m.def("createBrainWithConfigDict", [](const py::dict& configDict) {
        auto config = nlm_python::configFromDict(configDict);
        return nlm_python::createBrainWithValidation(config);
    }, py::arg("configDict"), "Create brain from configuration dictionary")
    
    m.def("createSimpleWorld", [](float width, float height, size_t visionWidth, size_t visionHeight) {
        auto world = nlm_python::createSimpleWorldWithValidation();
        nlm_python::configureWorldWithValidation(world, width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
        "Create a simple world with dimensions")
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return nlm_python::createAgentBrainWithValidation(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    
    // Enhanced sensory input creation functions
    m.def("createVision", [](size_t width, size_t height, size_t channels) {
        return nlm_python::createVisionSimple(width, height, channels);
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Create vision sensory input")
    
    m.def("createVisionFromData", [](size_t width, size_t height, size_t channels, const py::list& data) {
        auto vision = nlm_python::createVisionSimple(width, height, channels);
        std::vector<float> dataVec = nlm_python::pythonListToVector<float>(data);
        vision->setData(dataVec);
        return vision;
    }, py::arg("width"), py::arg("height"), py::arg("channels"), py::arg("data"),
        "Create vision from data")
    
    m.def("createAudio", [](size_t sampleRate, size_t numSamples) {
        return nlm_python::createAudioSimple(sampleRate, numSamples);
    }, py::arg("sampleRate"), py::arg("numSamples"), "Create audio sensory input")
    
    m.def("createAudioFromData", [](size_t sampleRate, size_t numSamples, const py::list& data) {
        auto audio = nlm_python::createAudioSimple(sampleRate, numSamples);
        std::vector<float> dataVec = nlm_python::pythonListToVector<float>(data);
        audio->setData(dataVec);
        return audio;
    }, py::arg("sampleRate"), py::arg("numSamples"), py::arg("data"),
        "Create audio from data")
    
    m.def("createInternalSignals", &nlm_python::createInternalSignalsSimple,
          py::arg("signals") = py::none(), "Create internal signals sensory input")
    
    m.def("createInternalSignalsFromList", [](const py::list& signals) {
        return nlm_python::createInternalSignalsSimple(signals);
    }, py::arg("signals"), "Create internal signals from list")
    
    // Enhanced action creation functions
    m.def("createAction", [](ActionType type) {
        return nlm_python::createActionSimple(type);
    }, py::arg("type"), "Create action with type")
    
    m.def("createActionWithParams", [](ActionType type, const py::list& parameters) {
        return nlm_python::createActionSimple(type, parameters);
    }, py::arg("type"), py::arg("parameters"), "Create action with parameters")
    
    // Enhanced world object creation functions
    m.def("createWorldObject", [](float x, float y, WorldObjectType type) {
        return nlm_python::createWorldObjectSimple(x, y, type);
    }, py::arg("x"), py::arg("y"), py::arg("type"), "Create world object")
    
    m.def("createWorldObjectWithValue", [](float x, float y, WorldObjectType type, float value) {
        return nlm_python::createWorldObjectSimple(x, y, type, value);
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value"),
        "Create world object with value")
    
    m.def("createAgentBody", &nlm_python::createAgentBodySimple,
          "Create agent body with default values")
    
    m.def("createActionResult", &nlm_python::createActionResultSimple,
          "Create action result with default values")
    
    m.def("createSensoryPercept", &nlm_python::createSensoryPerceptSimple,
          "Create sensory percept with default values")
    
    // Enhanced utility functions
    m.def("getVersion", &nlm_python::getVersion,
          "Get NLM Python bindings version")
    
    m.def("isSystemReady", &nlm_python::isSystemReady,
          "Check if NLM system is ready")
    
    m.def("validateBrainConfig", &nlm_python::validateBrainConfig,
          "Validate brain configuration")
    
    m.def("validateWorldConfig", &nlm_python::validateWorldParameters,
          "Validate world configuration")
    
    m.def("validateStepParameters", &nlm_python::validateSimulationParameters,
          "Validate simulation step parameters")
    
    // Enhanced simulation environment
    m.def("createSimulationEnvironment", [](const py::dict& configDict) {
        return nlm_python::createSimulationEnvironment(configDict);
    }, py::arg("configDict") = py::none(), "Create simulation environment")
    
    m.def("createSimpleSimulation", &nlm_python::createSimulationEnvironment,
          py::arg("configDict") = py::none(), "Create simple simulation environment (alias)")
    
    // Enhanced statistics and monitoring
    m.def("getComprehensiveStats", &nlm_python::getComprehensiveStats,
          "Get comprehensive statistics for all components")
    
    m.def("getSimulationSummary", &nlm_python::getSimulationSummary,
          "Get simulation summary as string")
    
    // Enhanced error handling
    m.def("setExceptionHandler", &nlm_python::setExceptionHandler,
          "Set custom exception handler")
    
    m.def("getExceptionHandler", &nlm_python::getExceptionHandler,
          "Get current exception handler")
    
    // Helper functions for working with IDs
    m.def("neuronId", &nlm_python::neuronIdFromInt,
          "Create NeuronId from integer")
    
    m.def("synapseId", &nlm_python::synapseIdFromInt,
          "Create SynapseId from integer")
    
    m.def("populationId", &nlm_python::populationIdFromInt,
          "Create PopulationId from integer")
    
    m.def("regionId", &nlm_python::regionIdFromInt,
          "Create RegionId from integer")
    
    m.def("regionIdToString", &nlm_python::regionIdToString,
          "Convert RegionId to string representation")
    
    // Helper functions for type conversion
    m.def("toPythonList", [](const std::vector<float>& vec) {
        return nlm_python::vectorToPythonList(vec);
    }, "Convert std::vector<float> to Python list")
    
    m.def("toStdVector", [](const py::list& list) {
        return nlm_python::pythonListToVector<float>(list);
    }, "Convert Python list to std::vector<float>")
    
    // Enhanced serialization functions
    m.def("saveBrainWithValidation", &nlm_python::saveBrainWithValidation,
          "Save brain with validation")
    
    m.def("loadBrainWithValidation", &nlm_python::loadBrainWithValidation,
          "Load brain with validation")
    
    m.def("saveConfigWithValidation", [](const Config& config, const std::string& filepath) {
        if (filepath.empty()) {
            throw NLMInvalidArgumentException("Filepath cannot be empty");
        }
        if (filepath.length() > 1024) {
            throw NLMInvalidArgumentException("Filepath too long (max: 1024 characters)");
        }
        return config.saveToFile(filepath);
    }, py::arg("config"), py::arg("filepath"), "Save configuration with validation")
    
    m.def("loadConfigWithValidation", [](Config& config, const std::string& filepath) {
        if (filepath.empty()) {
            throw NLMInvalidArgumentException("Filepath cannot be empty");
        }
        if (filepath.length() > 1024) {
            throw NLMInvalidArgumentException("Filepath too long (max: 1024 characters)");
        }
        return config.loadFromFile(filepath);
    }, py::arg("config"), py::arg("filepath"), "Load configuration with validation")
    
    // Enhanced world functions
    m.def("addObjectToWorld", &nlm_python::addObjectToWorld,
          "Add object to world with validation")
    
    m.def("removeObjectFromWorld", &nlm_python::removeObjectFromWorld,
          "Remove object from world with validation")
    
    m.def("getObjectAtWorldPosition", &nlm_python::getObjectAtWorldPosition,
          "Get object at world position with validation")
    
    m.def("isValidWorldPosition", &nlm_python::isValidWorldPosition,
          "Check if world position is valid")
    
    m.def("getWorldStats", &nlm_python::getWorldStats,
          "Get world statistics")
    
    // Enhanced brain functions
    m.def("addRegionToBrain", &nlm_python::addRegionToBrain,
          "Add region to brain with validation")
    
    m.def("getRegionFromBrain", &nlm_python::getRegionFromBrain,
          "Get region from brain with validation")
    
    m.def("getBrainStats", &nlm_python::getBrainStats,
          "Get brain statistics")
    
    // Enhanced agent brain functions
    m.def("initializeAgentBrain", &nlm_python::initializeAgentBrain,
          "Initialize agent brain with validation")
    
    m.def("processSensoryInput", &nlm_python::processSensoryInput,
          "Process sensory input with validation")
    
    m.def("decodeMotorCommand", &nlm_python::decodeMotorCommand,
          "Decode motor command with validation")
    
    m.def("applyRewardModulation", &nlm_python::applyRewardModulation,
          "Apply reward modulation with validation")
    
    m.def("updateDevelopment", &nlm_python::updateDevelopment,
          "Update development with validation")
    
    m.def("getAgentBrainStats", &nlm_python::getAgentBrainStats,
          "Get agent brain statistics")
    
    // Enhanced configuration functions
    m.def("setConfigValue", &nlm_python::setConfigValue,
          "Set configuration value with type conversion")
    
    m.def("getConfigValue", &nlm_python::getConfigValue,
          "Get configuration value with type conversion")
    
    m.def("hasConfigKey", &nlm_python::hasConfigKey,
          "Check if configuration has key (case-insensitive)")
    
    m.def("getConfigKeysList", &nlm_python::getConfigKeysList,
          "Get configuration keys as Python list")
    
    m.def("getConfigPairs", &nlm_python::getConfigPairs,
          "Get configuration as key-value pairs")
    
    m.def("getConfigPairsWithPrefix", &nlm_python::getConfigPairs,
          "Get configuration pairs filtered by prefix")
    
    // Simulation utilities
    m.def("runSimulationStep", &nlm_python::runSimulationStepWithValidation,
          "Run simulation step with validation")
    
    m.def("validateSimulationParameters", &nlm_python::validateSimulationParameters,
          "Validate simulation parameters")
    
    m.def("validateWorldParameters", &nlm_python::validateWorldParameters,
          "Validate world parameters")
    
    m.def("validateConfigParameters", &nlm_python::validateConfigParameters,
          "Validate configuration parameters")
    
    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Pythonic aliases for convenience
    m.def("runSimulation", &nlm_python::runSimulationStepWithValidation,
          "Run simulation (alias)")
    
    m.def("createDefaultConfigPython", &nlm_python::createDefaultConfig,
          "Create default config (Pythonic alias)")
    
    m.def("createBrainPython", &nlm_python::createBrainWithValidation,
          "Create brain (Pythonic alias)")
    
    m.def("createWorldPython", &nlm_python::createSimpleWorldWithValidation,
          "Create world (Pythonic alias)")
    
    m.def("createAgentPython", &nlm_python::createAgentBrainWithValidation,
          "Create agent (Pythonic alias)")
    
    m.def("saveBrainPython", &nlm_python::saveBrainWithValidation,
          "Save brain (Pythonic alias)")
    
    m.def("loadBrainPython", &nlm_python::loadBrainWithValidation,
          "Load brain (Pythonic alias)")
    
    m.def("getBrainStatsPython", &nlm_python::brainGetStatsWithValidation,
          "Get brain stats (Pythonic alias)")
    
    m.def("getAgentStatsPython", &nlm_python::agentBrainGetStatsWithValidation,
          "Get agent stats (Pythonic alias)")
    
    m.def("getWorldStatsPython", &nlm_python::getWorldStats,
          "Get world stats (Pythonic alias)")
    
    m.def("createCompleteEnvironment", &nlm_python::createCompleteAgentSetup,
          "Create complete agent environment (Pythonic alias)")
    
    // Enhanced examples and documentation
    m.def("example_minimal", []() {
        auto config = nlm_python::createDefaultConfig();
        auto brain = nlm_python::createBrainWithValidation(config);
        brain->initialize();
        for (int step = 0; step < 10; ++step) {
            brain->step(step);
        }
        return "Minimal example completed successfully";
    }, "Minimal example function")
    
    m.def("example_complete_agent", [](const py::dict& configDict) {
        auto env = nlm_python::createSimulationEnvironment(configDict);
        env.runSimulation(100, true);
        return "Complete agent example completed successfully";
    }, py::arg("configDict") = py::none(), "Complete agent example function")
    
    m.def("example_with_sensory_input", []() {
        auto config = nlm_python::createDefaultConfig();
        auto brain = nlm_python::createBrainWithValidation(config);
        auto world = nlm_python::createSimpleWorldWithValidation();
        auto agent = nlm_python::createAgentBrainWithValidation(brain);
        
        agent->initialize(*world);
        
        for (int step = 0; step < 50; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return "Example with sensory input completed successfully";
    }, "Example with sensory input function");
}

} // namespace nlm