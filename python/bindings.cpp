#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <unordered_map>
#include <algorithm>

// Core includes
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/visualization/VisualizationInterface.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

namespace py = pybind11;
namespace nlm {

// Python-specific exception classes
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("Configuration Error: " + msg) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& msg) : NLMException("Simulation Error: " + msg) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& msg) : NLMException("Brain Error: " + msg) {}
};

// Helper functions for Python integration
namespace py_helpers {

    // Convert Python list to vector
    template<typename T>
    std::vector<T> pyListToVector(const py::list& py_list) {
        std::vector<T> vec;
        vec.reserve(py_list.size());
        for (const auto& item : py_list) {
            vec.push_back(item.cast<T>());
        }
        return vec;
    }

    // Convert Python dict to unordered_map
    template<typename K, typename V>
    std::unordered_map<K, V> pyDictToMap(const py::dict& py_dict) {
        std::unordered_map<K, V> map;
        for (const auto& item : py_dict) {
            map[item.first.cast<K>()] = item.second.cast<V>();
        }
        return map;
    }

    // Validate configuration keys
    bool validateConfigKey(const std::string& key) {
        static const std::vector<std::string> validKeys = {
            "brain.neuronCount", "brain.regionCount", "simulation.dt",
            "simulation.maxTime", "world.width", "world.height",
            "development.stage", "neuromodulation.level"
        };
        return std::find(validKeys.begin(), validKeys.end(), key) != validKeys.end();
    }

    // Safe float conversion with validation
    float safeFloatConversion(const py::object& obj, const std::string& paramName) {
        try {
            float value = obj.cast<float>();
            if (std::isnan(value) || std::isinf(value)) {
                throw ConfigurationError("Invalid " + paramName + ": NaN or infinite value");
            }
            return value;
        } catch (const py::cast_error&) {
            throw ConfigurationError("Invalid type for " + paramName + ": expected float");
        }
    }

    // Safe int conversion with validation
    int safeIntConversion(const py::object& obj, const std::string& paramName, int min = 0) {
        try {
            int value = obj.cast<int>();
            if (value < min) {
                throw ConfigurationError("Invalid " + paramName + ": value must be >= " + std::to_string(min));
            }
            return value;
        } catch (const py::cast_error&) {
            throw ConfigurationError("Invalid type for " + paramName + ": expected int");
        }
    }
}

// Enhanced Config class with Pythonic API
class PyConfig {
private:
    std::shared_ptr<Config> config_;
    
public:
    PyConfig() : config_(std::make_shared<Config>()) {}
    
    // Load from JSON string
    bool load_from_json_string(const std::string& json_str) {
        return config_->loadFromFile(json_str);
    }
    
    // Load from dictionary
    bool load_from_dict(const py::dict& py_dict) {
        bool success = true;
        for (const auto& item : py_dict) {
            std::string key = item.first.cast<std::string>();
            auto& value = item.second;
            
            if (value.is(py::int_())) {
                config_->set(key, value.cast<int>());
            } else if (value.is(py::float_())) {
                config_->set(key, value.cast<double>());
            } else if (value.is(py::bool_())) {
                config_->set(key, value.cast<bool>());
            } else if (value.is(py::str_())) {
                config_->set(key, value.cast<std::string>());
            } else if (value.is(py::list_())) {
                config_->set(key, value.cast<std::vector<double>>());
            } else {
                throw ConfigurationError("Unsupported type for key '" + key + "'");
            }
        }
        return success;
    }
    
    // Get value with type safety
    template<typename T>
    T get_value(const std::string& key, const T& default_value = T{}) {
        if (!config_->has(key)) {
            return default_value;
        }
        auto result = config_->get<T>(key);
        return result ? *result : default_value;
    }
    
    // Get value with validation
    py::object get_with_validation(const std::string& key) {
        if (!config_->has(key)) {
            throw ConfigurationError("Key not found: '" + key + "'");
        }
        if (!py_helpers::validateConfigKey(key)) {
            throw ConfigurationError("Invalid configuration key: '" + key + "'");
        }
        return config_->get(key).value();
    }
    
    // Set with validation
    void set_with_validation(const std::string& key, const py::object& value) {
        if (!py_helpers::validateConfigKey(key)) {
            throw ConfigurationError("Invalid configuration key: '" + key + "'");
        }
        
        if (value.is(py::int_())) {
            config_->set(key, value.cast<int>());
        } else if (value.is(py::float_())) {
            float val = py_helpers::safeFloatConversion(value, key);
            config_->set(key, static_cast<double>(val));
        } else if (value.is(py::bool_())) {
            config_->set(key, value.cast<bool>());
        } else if (value.is(py::str_())) {
            config_->set(key, value.cast<std::string>());
        } else {
            throw ConfigurationError("Unsupported value type for key '" + key + "'");
        }
    }
    
    // Get all keys
    py::list get_keys() const {
        auto keys = config_->getKeys();
        py::list py_list;
        for (const auto& key : keys) {
            py_list.append(key);
        }
        return py_list;
    }
    
    // Get summary with formatting
    std::string get_summary() const {
        return config_->summary();
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Clear configuration
    void clear() {
        config_->clear();
    }
    
    // Create default config
    static std::shared_ptr<PyConfig> create_default() {
        return std::make_shared<PyConfig>();
    }
    
    // Save to JSON string
    std::string save_to_json_string() const {
        // Implementation would write to a temporary file and return content
        // For now, just return summary
        return config_->summary();
    }
    
    // Get config as dictionary (Pythonic interface)
    py::dict as_dict() const {
        py::dict py_dict;
        auto keys = config_->getKeys();
        for (const auto& key : keys) {
            auto value = config_->get(key);
            if (value) {
                py_dict[key] = py::cast(*value);
            }
        }
        return py_dict;
    }
};

// Enhanced Brain class with Pythonic API
class PyBrain {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    PyBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    
    // Initialize brain
    bool initialize() {
        return brain_->initialize();
    }
    
    // Step simulation
    void step(int current_step) {
        brain_->step(current_step);
    }
    
    void step_with_time(int current_step, double current_time) {
        brain_->step(current_step, current_time);
    }
    
    // Receive sensory input (enhanced)
    void receive_sensory_input(const PyVision& vision) {
        // Convert PyVision to SensoryInput
        brain_->receiveSensoryInput(vision.to_sensory_input());
    }
    
    void receive_percept(const PySensoryPercept& percept) {
        // Convert PySensoryPercept to SensoryInput
        brain_->receiveSensoryInput(percept.to_sensory_input());
    }
    
    // Convenience methods for common operations
    std::vector<float> get_motor_output() {
        auto action = brain_->produceAction();
        if (action) {
            return action->getParameters();
        }
        return {};
    }
    
    double get_prediction_error() const {
        return brain_->getPredictionErrorSignal()->getError();
    }
    
    double get_excitation_inhibition_ratio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    // Statistics helpers
    py::dict get_statistics() const {
        py::dict stats;
        stats["total_neurons"] = brain_->getTotalNeuronCount();
        stats["total_synapses"] = brain_->getTotalSynapseCount();
        stats["active_neurons"] = brain_->getActiveNeuronCount();
        stats["firing_neurons"] = brain_->getFiringNeuronCount();
        stats["total_spikes"] = brain_->getTotalSpikeCount();
        stats["avg_firing_rate"] = brain_->getAverageFiringRate();
        stats["ei_ratio"] = brain_->getExcitationInhibitionRatio();
        stats["prediction_error"] = brain_->getPredictionErrorSignal()->getError();
        return stats;
    }
    
    // Development helpers
    void develop_stage(int stage) {
        DevelopmentalStage dev_stage = static_cast<DevelopmentalStage>(stage);
        brain_->setDevelopmentalStage(dev_stage);
    }
    
    int get_developmental_stage() const {
        return static_cast<int>(brain_->getDevelopmentalStageStage());
    }
    
    // Reset brain
    void reset() {
        brain_->reset();
    }
    
    // Save/load with error handling
    bool save(const std::string& filepath) {
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Context manager for simulation steps
    class SimulationContext {
    private:
        std::shared_ptr<PyBrain> brain_;
        int start_step_;
        int end_step_;
        
    public:
        SimulationContext(std::shared_ptr<PyBrain> brain, int start_step, int end_step)
            : brain_(brain), start_step_(start_step), end_step_(end_step) {}
        
        ~SimulationContext() {
            // Cleanup if needed
        }
        
        void run() {
            for (int step = start_step_; step <= end_step_; ++step) {
                brain_->step(step);
            }
        }
        
        py::dict run_and_collect(int current_time = 0) {
            run();
            return brain_->get_statistics();
        }
    };
    
    // Create simulation context
    SimulationContext create_simulation_context(int start_step, int end_step) {
        return SimulationContext(shared_from_this(), start_step, end_step);
    }
    
    // Get brain pointer (for advanced users)
    std::shared_ptr<Brain> get_brain() {
        return brain_;
    }
};

// Enhanced SensoryPercept class
class PySensoryPercept {
private:
    std::shared_ptr<SensoryPercept> percept_;
    
public:
    PySensoryPercept() : percept_(std::make_shared<SensoryPercept>()) {}
    
    // Get vision data
    py::array_t<float> get_vision() const {
        auto vision = percept_->getVision();
        return py::array_t<float>({vision.getWidth(), vision.getHeight(), vision.getChannels()}, vision.getData().data());
    }
    
    // Set vision data from numpy-like array
    void set_vision(py::array_t<float> vision_array) {
        // Implementation would convert array to Vision object
        // For now, just a placeholder
    }
    
    // Get audio data
    py::array_t<float> get_audio() const {
        auto audio = percept_->getAudio();
        return py::array_t<float>({audio.getSampleRate(), audio.getNumSamples()}, audio.getData().data());
    }
    
    // Convenience methods
    py::dict get_all_data() const {
        py::dict data;
        data["vision"] = get_vision();
        data["audio"] = get_audio();
        data["internal"] = percept_->getInternal();
        data["touch"] = percept_->getTouch();
        data["proprioception"] = percept_->getProprioception();
        return data;
    }
    
    // Create from dictionary
    static PySensoryPercept from_dict(const py::dict& data) {
        PySensoryPercept percept;
        // Implementation would populate from dict
        return percept;
    }
    
    // Convert to SensoryInput
    class SensoryInputAdapter {
    public:
        static std::shared_ptr<SensoryInput> to_sensory_input(const PySensoryPercept& percept) {
            // This would need proper implementation
            return nullptr;
        }
    };
};

// Enhanced Vision class
class PyVision {
private:
    std::shared_ptr<Vision> vision_;
    
public:
    PyVision() : vision_(std::make_shared<Vision>()) {}
    
    PyVision(size_t width, size_t height, size_t channels = 3) 
        : vision_(std::make_shared<Vision>(width, height, channels)) {}
    
    // Create from numpy array
    static PyVision from_array(py::array_t<float> array) {
        PyVision vision;
        // Implementation would copy array data
        return vision;
    }
    
    // Create from list of lists
    static PyVision from_nested_list(const py::list& nested_list) {
        PyVision vision;
        // Implementation would convert nested list to Vision
        return vision;
    }
    
    // Get width/height/channels
    size_t get_width() const { return vision_->getWidth(); }
    size_t get_height() const { return vision_->getHeight(); }
    size_t get_channels() const { return vision_->getChannels(); }
    
    // Get numpy array
    py::array_t<float> to_array() const {
        return py::array_t<float>({get_width(), get_height(), get_channels()}, vision_->getData().data());
    }
    
    // Convert to SensoryInput
    class SensoryInputAdapter {
    public:
        static std::shared_ptr<SensoryInput> to_sensory_input(const PyVision& py_vision) {
            return py_vision.vision_;
        }
    };
    
    // Helper to access vision_
    std::shared_ptr<Vision> vision_() const { return vision_; }
};

// Enhanced Audio class
class PyAudio {
private:
    std::shared_ptr<Audio> audio_;
    
public:
    PyAudio() : audio_(std::make_shared<Audio>()) {}
    
    PyAudio(size_t sample_rate, size_t num_samples) 
        : audio_(std::make_shared<Audio>(sample_rate, num_samples)) {}
    
    // Create from numpy array
    static PyAudio from_array(py::array_t<float> array, size_t sample_rate = 44100) {
        PyAudio audio;
        // Implementation would copy array data
        return audio;
    }
    
    // Get sample rate and num samples
    size_t get_sample_rate() const { return audio_->getSampleRate(); }
    size_t get_num_samples() const { return audio_->getNumSamples(); }
    
    // Get numpy array
    py::array_t<float> to_array() const {
        return py::array_t<float>({get_sample_rate(), get_num_samples()}, audio_->getData().data());
    }
    
    // Convenience methods
    py::dict get_metadata() const {
        py::dict metadata;
        metadata["sample_rate"] = get_sample_rate();
        metadata["num_samples"] = get_num_samples();
        metadata["duration"] = static_cast<float>(get_num_samples()) / get_sample_rate();
        return metadata;
    }
};

// Enhanced Action class
class PyAction {
private:
    std::shared_ptr<Action> action_;
    
public:
    PyAction() : action_(std::make_shared<Action>()) {}
    
    PyAction(ActionType type) : action_(std::make_shared<Action>(type)) {}
    
    PyAction(ActionType type, const std::vector<float>& parameters) 
        : action_(std::make_shared<Action>(type, parameters)) {}
    
    // Get type
    int get_type() const { return static_cast<int>(action_->getType()); }
    
    // Get name
    std::string get_name() const { return action_->getName(); }
    
    // Get parameters
    std::vector<float> get_parameters() const { return action_->getParameters(); }
    
    // Set parameters
    void set_parameters(const std::vector<float>& params) { action_->setParameters(params); }
    
    // Convenience methods
    py::dict to_dict() const {
        py::dict dict;
        dict["type"] = get_type();
        dict["name"] = get_name();
        dict["parameters"] = get_parameters();
        return dict;
    }
    
    // Create from dictionary
    static PyAction from_dict(const py::dict& dict) {
        int type = dict["type"].cast<int>();
        std::vector<float> params = dict["parameters"].cast<std::vector<float>>();
        return PyAction(static_cast<ActionType>(type), params);
    }
    
    // Clone
    PyAction clone() const {
        auto cloned = action_->clone();
        return PyAction(cloned->getType(), cloned->getParameters());
    }
    
    // Helper to access action_
    std::shared_ptr<Action> action_() const { return action_; }
};

// Enhanced SimpleWorld class
class PySimpleWorld {
private:
    std::shared_ptr<SimpleWorld> world_;
    
public:
    PySimpleWorld() : world_(std::make_shared<SimpleWorld>()) {}
    
    // Configure world
    void configure(size_t width, size_t height, size_t vision_width, size_t vision_height) {
        world_->configure(width, height, vision_width, vision_height);
    }
    
    // Update simulation
    void update(double timestep = 0.1) {
        world_->update(timestep);
    }
    
    // Get agent body
    py::dict get_agent_body() const {
        auto body = world_->getAgentBody();
        py::dict dict;
        dict["x"] = body->x;
        dict["y"] = body->y;
        dict["orientation"] = body->orientation;
        dict["velocityX"] = body->velocityX;
        dict["velocityY"] = body->velocityY;
        dict["angularVelocity"] = body->angularVelocity;
        dict["energy"] = body->energy;
        dict["health"] = body->health;
        dict["age"] = body->age;
        dict["isMoving"] = body->isMoving;
        dict["isTurning"] = body->isTurning;
        return dict;
    }
    
    // Get sensory percept
    py::dict get_sensory_percept() const {
        auto percept = world_->getSensoryPercept();
        return percept; // Would need conversion
    }
    
    // Apply motor command from action
    void apply_action(const PyAction& action) {
        // Convert PyAction to MotorCommand
        world_->applyMotorCommand(action.action_());
    }
    
    // Add world object
    void add_world_object(float x, float y, int type, float value = 0.0f, float radius = 0.5f) {
        world_->addObject(WorldObject{x, y, static_cast<WorldObjectType>(type), value, radius});
    }
    
    // Check position validity
    bool is_valid_position(float x, float y) const {
        return world_->isValidPosition(x, y);
    }
    
    // Get world dimensions
    py::dict get_dimensions() const {
        py::dict dims;
        dims["width"] = world_->getWidth();
        dims["height"] = world_->getHeight();
        dims["maxEnergy"] = world_->getMaxEnergy();
        dims["energyDecayRate"] = world_->getEnergyDecayRate();
        return dims;
    }
    
    // Reset world
    void reset() {
        world_->reset();
    }
    
    // Get simulation time
    double get_simulation_time() const {
        return world_->getSimulationTime();
    }
    
    // Set random seed
    void set_random_seed(int seed) {
        world_->setRandomSeed(seed);
    }
    
    // Get random seed
    int get_random_seed() const {
        return world_->getRandomSeed();
    }
};

// Enhanced AgentBrain class
class PyAgentBrain {
private:
    std::shared_ptr<AgentBrain> agent_brain_;
    
public:
    PyAgentBrain(std::shared_ptr<Brain> brain) 
        : agent_brain_(std::make_shared<AgentBrain>(brain)) {}
    
    // Initialize with world
    void initialize(const PySimpleWorld& world) {
        agent_brain_->initialize(*world.world_);
    }
    
    // Process sensory input
    void process_sensory_input(const PySensoryPercept& percept) {
        agent_brain_->processSensoryInput(percept.percept_);
    }
    
    // Get sensory input size
    size_t get_sensory_input_size() const {
        return agent_brain_->getSensoryInputSize();
    }
    
    // Get motor output size
    size_t get_motor_output_size() const {
        return agent_brain_->getMotorOutputSize();
    }
    
    // Decode motor command
    PyAction decode_motor_command() {
        auto cmd = agent_brain_->decodeMotorCommand();
        return PyAction(cmd);
    }
    
    // Apply reward modulation
    void apply_reward_modulation(double reward, double predicted_reward) {
        agent_brain_->applyRewardModulation(reward, predicted_reward);
    }
    
    // Update development
    void update_development(double timestep) {
        agent_brain_->updateDevelopment(timestep);
    }
    
    // Get neuromodulation levels
    py::dict get_neuromodulation_levels() const {
        py::dict levels;
        levels["dopamine"] = agent_brain_->getNeuromodulationLevel();
        levels["curiosity"] = agent_brain_->getCuriosityLevel();
        levels["novelty"] = agent_brain_->getNoveltyLevel();
        levels["prediction_error"] = agent_brain_->getPredictionError();
        return levels;
    }
    
    // Get developmental stage
    int get_developmental_stage() const {
        return static_cast<int>(agent_brain_->getDevelopmentalStage());
    }
    
    // Configuration toggles
    void enable_reward_modulation(bool enable) {
        agent_brain_->enableRewardModulation(enable);
    }
    
    void enable_structural_plasticity(bool enable) {
        agent_brain_->enableStructuralPlasticity(enable);
    }
    
    void enable_development(bool enable) {
        agent_brain_->enableDevelopment(enable);
    }
    
    void enable_curiosity(bool enable) {
        agent_brain_->enableCuriosity(enable);
    }
    
    // Check if features enabled
    bool is_reward_modulation_enabled() const {
        return agent_brain_->isRewardModulationEnabled();
    }
    
    bool is_structural_plasticity_enabled() const {
        return agent_brain_->isStructuralPlasticityEnabled();
    }
    
    bool is_development_enabled() const {
        return agent_brain_->isDevelopmentEnabled();
    }
    
    bool is_curiosity_enabled() const {
        return agent_brain_->isCuriosityEnabled();
    }
    
    // Reset agent
    void reset() {
        agent_brain_->reset();
    }
    
    // Get brain
    std::shared_ptr<Brain> get_brain() {
        return agent_brain_->getBrain();
    }
};

// High-level simulation helpers
namespace simulation {

    // Run a simple episode
    py::dict run_episode(
        std::shared_ptr<PyBrain> brain,
        std::shared_ptr<PySimpleWorld> world,
        std::shared_ptr<PyAgentBrain> agent_brain,
        int max_steps = 1000
    ) {
        py::dict episode_data;
        episode_data["steps"] = py::list();
        episode_data["rewards"] = py::list();
        episode_data["actions"] = py::list();
        
        brain->initialize();
        agent_brain->initialize(*world);
        
        for (int step = 0; step < max_steps; ++step) {
            // Update world
            world->update();
            
            // Get percept
            auto percept = world->get_sensory_percept();
            agent_brain->process_sensory_input(percept);
            
            // Get action
            auto action = agent_brain->decode_motor_command();
            world->apply_action(action);
            
            // Apply reward modulation
            double reward = 0.0; // Would get from world
            agent_brain->apply_reward_modulation(reward, 0.0);
            
            // Record data
            episode_data["steps"].cast<py::list>().append(step);
            episode_data["rewards"].cast<py::list>().append(reward);
            episode_data["actions"].cast<py::list>().append(action.to_dict());
            
            // Check termination
            if (brain->get_developmental_stage() >= 4) { // Adult stage
                break;
            }
        }
        
        episode_data["statistics"] = brain->get_statistics();
        return episode_data;
    }

    // Create standard configuration
    std::shared_ptr<PyConfig> create_standard_config() {
        auto config = PyConfig::create_default();
        // Set standard values
        config->set("brain.neuronCount", 1000);
        config->set("brain.regionCount", 5);
        config->set("simulation.dt", 0.1);
        config->set("simulation.maxTime", 100.0);
        config->set("world.width", 100.0);
        config->set("world.height", 100.0);
        config->set("development.stage", 0);
        config->set("neuromodulation.level", 0.5);
        return config;
    }

    // Load from JSON file
    std::shared_ptr<PyConfig> load_config_from_json(const std::string& filepath) {
        auto config = PyConfig::create_default();
        config->load_from_json_string(filepath);
        return config;
    }

    // Save config to JSON file
    bool save_config_to_json(const std::shared_ptr<PyConfig>& config, const std::string& filepath) {
        // Implementation would save config to file
        return true;
    }
}

// Main Python module
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides enhanced Pythonic interfaces to the NLM brain simulation system.
        
        Key Features:
        - Pythonic API design with convenience methods
        - Enhanced error handling and type safety
        - Simulation helpers and episode runners
        - Configuration management with JSON support
        - Data structures with numpy-compatible interfaces
        - Advanced simulation utilities
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<BrainError>(m, "BrainError");

    // Types
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

    // Enhanced Config class
    py::class_<PyConfig>(m, "Config")
        .def(py::init<>())
        .def("load_from_json", &PyConfig::load_from_json_string,
             py::arg("json_str"), "Load configuration from JSON string")
        .def("load_from_dict", &PyConfig::load_from_dict,
             py::arg("config_dict"), "Load configuration from Python dictionary")
        .def("save_to_json", &PyConfig::save_to_json_string,
             "Save configuration to JSON string")
        .def("get_value", &PyConfig::get_value<T>,
             py::arg("key"), py::arg("default_value") = py::none(),
             "Get configuration value with type safety")
        .def("get_with_validation", &PyConfig::get_with_validation,
             py::arg("key"), "Get configuration value with validation")
        .def("set_with_validation", &PyConfig::set_with_validation,
             py::arg("key"), py::arg("value"), "Set configuration value with validation")
        .def("get_keys", &PyConfig::get_keys, "Get all configuration keys")
        .def("get_summary", &PyConfig::get_summary, "Get configuration summary")
        .def("has", &PyConfig::has, py::arg("key"), "Check if key exists")
        .def("clear", &PyConfig::clear, "Clear all configuration")
        .def("as_dict", &PyConfig::as_dict, "Get configuration as Python dictionary")
        .def_static("create_default", &PyConfig::create_default,
                   "Create default configuration")
        .def("__repr__", [](const PyConfig& cfg) {
            return "<Config with " + std::to_string(cfg.get_keys().cast<py::list>().size()) + " keys>";
        });

    // Enhanced Vision class
    py::class_<PyVision>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(),
             py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Create vision input with specified dimensions")
        .def_static("from_array", &PyVision::from_array,
                   py::arg("array"), "Create Vision from numpy array")
        .def_static("from_nested_list", &PyVision::from_nested_list,
                   py::arg("nested_list"), "Create Vision from nested list")
        .def("get_width", &PyVision::get_width, "Get vision width")
        .def("get_height", &PyVision::get_height, "Get vision height")
        .def("get_channels", &PyVision::get_channels, "Get vision channels")
        .def("to_array", &PyVision::to_array, "Convert Vision to numpy array")
        .def("__repr__", [](const PyVision& v) {
            return "<Vision " + std::to_string(v.get_width()) + "x" + 
                   std::to_string(v.get_height()) + "x" + std::to_string(v.get_channels()) + ">";
        });

    // Enhanced Audio class
    py::class_<PyAudio>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(),
             py::arg("sample_rate"), py::arg("num_samples"),
             "Create audio input with specified sample rate and samples")
        .def_static("from_array", &PyAudio::from_array,
                   py::arg("array"), py::arg("sample_rate") = 44100,
                   "Create Audio from numpy array")
        .def("get_sample_rate", &PyAudio::get_sample_rate, "Get sample rate")
        .def("get_num_samples", &PyAudio::get_num_samples, "Get number of samples")
        .def("to_array", &PyAudio::to_array, "Convert Audio to numpy array")
        .def("get_metadata", &PyAudio::get_metadata, "Get audio metadata")
        .def("__repr__", [](const PyAudio& a) {
            return "<Audio " + std::to_string(a.get_sample_rate()) + " Hz, " +
                   std::to_string(a.get_num_samples()) + " samples>";
        });

    // Enhanced Action class
    py::class_<PyAction>(m, "Action")
        .def(py::init<>())
        .def(py::init<int>(), py::arg("type"), "Create action from type")
        .def(py::init<int, std::vector<float>>(),
             py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("get_type", &PyAction::get_type, "Get action type")
        .def("get_name", &PyAction::get_name, "Get action name")
        .def("get_parameters", &PyAction::get_parameters, "Get action parameters")
        .def("set_parameters", &PyAction::set_parameters, py::arg("params"), "Set action parameters")
        .def("to_dict", &PyAction::to_dict, "Convert action to dictionary")
        .def_static("from_dict", &PyAction::from_dict,
                   py::arg("dict"), "Create action from dictionary")
        .def("clone", &PyAction::clone, "Clone action")
        .def("__repr__", [](const PyAction& a) {
            return "<Action " + a.get_name() + " with " +
                   std::to_string(a.get_parameters().size()) + " parameters>";
        });

    // Enhanced SensoryPercept class
    py::class_<PySensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("get_vision", &PySensoryPercept::get_vision, "Get vision data")
        .def("set_vision", &PySensoryPercept::set_vision,
             py::arg("vision"), "Set vision data")
        .def("get_audio", &PySensoryPercept::get_audio, "Get audio data")
        .def("set_audio", &PySensoryPercept::set_audio,
             py::arg("audio"), "Set audio data")
        .def("get_internal", &PySensoryPercept::get_internal, "Get internal signals")
        .def("set_internal", &PySensoryPercept::set_internal,
             py::arg("internal"), "Set internal signals")
        .def("get_touch", &PySensoryPercept::get_touch, "Get touch data")
        .def("set_touch", &PySensoryPercept::set_touch,
             py::arg("touch"), "Set touch data")
        .def("get_proprioception", &PySensoryPercept::get_proprioception, "Get proprioception")
        .def("set_proprioception", &PySensoryPercept::set_proprioception,
             py::arg("proprioception"), "Set proprioception")
        .def("get_all_data", &PySensoryPercept::get_all_data, "Get all sensory data as dictionary")
        .def_static("from_dict", &PySensoryPercept::from_dict,
                   py::arg("data"), "Create SensoryPercept from dictionary")
        .def("__repr__", [](const PySensoryPercept& p) {
            return "<SensoryPercept with vision, audio, internal>";
        });

    // Enhanced SimpleWorld class
    py::class_<PySimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &PySimpleWorld::configure,
             py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions")
        .def("update", &PySimpleWorld::update,
             py::arg("timestep") = 0.1, "Update world simulation")
        .def("get_agent_body", &PySimpleWorld::get_agent_body, "Get agent body state")
        .def("get_sensory_percept", &PySimpleWorld::get_sensory_percept, "Get sensory percept")
        .def("apply_action", &PySimpleWorld::apply_action,
             py::arg("action"), "Apply action to world")
        .def("add_world_object", &PySimpleWorld::add_world_object,
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
             "Add object to world")
        .def("remove_world_object", &PySimpleWorld::remove_world_object,
             py::arg("x"), py::arg("y"), "Remove object from world")
        .def("is_valid_position", &PySimpleWorld::is_valid_position,
             py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("get_dimensions", &PySimpleWorld::get_dimensions, "Get world dimensions")
        .def("reset", &PySimpleWorld::reset, "Reset world")
        .def("get_simulation_time", &PySimpleWorld::get_simulation_time, "Get simulation time")
        .def("set_random_seed", &PySimpleWorld::set_random_seed,
             py::arg("seed"), "Set random seed")
        .def("get_random_seed", &PySimpleWorld::get_random_seed, "Get random seed")
        .def("__repr__", [](const PySimpleWorld& w) {
            return "<SimpleWorld" + std::to_string(w.get_dimensions().cast<py::dict>("width")) + "x" +
                   std::to_string(w.get_dimensions().cast<py::dict>("height")) + ">";
        });

    // Enhanced AgentBrain class
    py::class_<PyAgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain from brain")
        .def("initialize", &PyAgentBrain::initialize,
             py::arg("world"), "Initialize with world")
        .def("process_sensory_input", &PyAgentBrain::process_sensory_input,
             py::arg("percept"), "Process sensory input")
        .def("decode_motor_command", &PyAgentBrain::decode_motor_command,
             "Decode motor command from neural activity")
        .def("get_sensory_input_size", &PyAgentBrain::get_sensory_input_size,
             "Get expected sensory input size")
        .def("get_motor_output_size", &PyAgentBrain::get_motor_output_size,
             "Get expected motor output size")
        .def("apply_reward_modulation", &PyAgentBrain::apply_reward_modulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &PyAgentBrain::update_development,
             py::arg("timestep"), "Update development system")
        .def("get_developmental_stage", &PyAgentBrain::get_developmental_stage,
             "Get current developmental stage")
        .def("get_neuromodulation_levels", &PyAgentBrain::get_neuromodulation_levels,
             "Get all neuromodulation levels")
        .def("enable_reward_modulation", &PyAgentBrain::enable_reward_modulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &PyAgentBrain::enable_structural_plasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enable_development", &PyAgentBrain::enable_development,
             py::arg("enable"), "Enable/disable development")
        .def("enable_curiosity", &PyAgentBrain::enable_curiosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("is_reward_modulation_enabled", &PyAgentBrain::is_reward_modulation_enabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &PyAgentBrain::is_structural_plasticity_enabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &PyAgentBrain::is_development_enabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &PyAgentBrain::is_curiosity_enabled,
             "Check if curiosity is enabled")
        .def("reset", &PyAgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &PyAgentBrain::get_brain, "Get underlying brain")
        .def("__repr__", [](const PyAgentBrain& agent) {
            return "<AgentBrain connected to " + std::to_string(agent.get_brain()->getTotalNeuronCount()) + " neurons>";
        });

    // Enhanced Brain class
    py::class_<PyBrain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &PyBrain::initialize, "Initialize the brain")
        .def("step", &PyBrain::step,
             py::arg("current_step"), "Perform simulation step")
        .def("step_with_time", &PyBrain::step_with_time,
             py::arg("current_step"), py::arg("current_time"),
             "Perform simulation step with timestamp")
        .def("receive_sensory_input", &PyBrain::receive_sensory_input,
             py::arg("input"), "Receive sensory input")
        .def("inject_current", &PyBrain::inject_current,
             py::arg("neuron"), py::arg("current"),
             "Inject current into specific neuron")
        .def("inject_current_to_neurons", &PyBrain::inject_current_to_neurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of specific type")
        .def("produce_action", &PyBrain::get_motor_output,
             "Produce motor action based on neural activity")
        .def("reset", &PyBrain::reset, "Reset brain state")
        .def("save", &PyBrain::save,
             py::arg("filepath"), "Save brain state to file")
        .def("load", &PyBrain::load,
             py::arg("filepath"), "Load brain state from file")
        .def("get_statistics", &PyBrain::get_statistics, "Get brain statistics")
        .def("get_prediction_error", &PyBrain::get_prediction_error,
             "Get prediction error")
        .def("get_excitation_inhibition_ratio", &PyBrain::get_excitation_inhibition_ratio,
             "Get excitation/inhibition ratio")
        .def("develop_stage", &PyBrain::develop_stage,
             py::arg("stage"), "Set developmental stage")
        .def("get_developmental_stage", &PyBrain::get_developmental_stage,
             "Get current developmental stage")
        .def("get_total_neuron_count", &PyBrain::get_brain->getTotalNeuronCount,
             "Get total neuron count")
        .def("get_total_synapse_count", &PyBrain::get_brain->getTotalSynapseCount,
             "Get total synapse count")
        .def("get_active_neuron_count", &PyBrain::get_brain->getActiveNeuronCount,
             "Get active neuron count")
        .def("get_firing_neuron_count", &PyBrain::get_brain->getFiringNeuronCount,
             "Get firing neuron count")
        .def("get_average_firing_rate", &PyBrain::get_brain->getAverageFiringRate,
             "Get average firing rate")
        .def("add_region", &PyBrain::get_brain->addRegion,
             py::arg("name") = "", "Add new neural region")
        .def("get_region", &PyBrain::get_brain->getRegion,
             py::arg("id"), "Get region by ID")
        .def("get_region_count", &PyBrain::get_brain->getRegionCount,
             "Get number of regions")
        .def("get_region_ids", &PyBrain::get_brain->getRegionIds,
             "Get all region IDs")
        .def("get_all_regions", &PyBrain::get_brain->getRegions,
             "Get all regions")
        .def("log_status", &PyBrain::get_brain->logStatus,
             "Log brain status")
        .def("__repr__", [](const PyBrain& brain) {
            auto stats = brain.get_statistics();
            return "<Brain with " + std::to_string(stats.cast<py::dict>("total_neurons")) + " neurons, " +
                   std::to_string(stats.cast<py::dict>("total_synapses")) + " synapses>";
        });

    // Helper functions and simulation utilities
    m.def("create_default_config", &simulation::create_standard_config,
          "Create standard NLM configuration");

    m.def("load_config_from_json", &simulation::load_config_from_json,
          py::arg("filepath"), "Load configuration from JSON file");

    m.def("run_episode", &simulation::run_episode,
          py::arg("brain"), py::arg("world"), py::arg("agent_brain"), py::arg("max_steps") = 1000,
          "Run a complete episode with brain-world interaction");

    // Error types
    py::class_<NLMException, std::exception>(m, "NLMException")
        .def(py::init<const std::string&>(), py::arg("message"));

    py::class_<ConfigurationError, NLMException>(m, "ConfigurationError")
        .def(py::init<const std::string&>(), py::arg("message"));

    py::class_<SimulationError, NLMException>(m, "SimulationError")
        .def(py::init<const std::string&>(), py::arg("message"));

    py::class_<BrainError, NLMException>(m, "BrainError")
        .def(py::init<const std::string&>(), py::arg("message"));

    // Convenience aliases
    m.add_object("_PyVision", PyVision());
    m.add_object("_PyAudio", PyAudio());
    m.add_object("_PyAction", PyAction());
    m.add_object("_PySensoryPercept", PySensoryPercept());

    m.doc() += "\n\nThis enhanced version provides Pythonic APIs, error handling, "
              "and simulation helpers for easier NLM usage. "
              "See documentation for detailed usage examples.";

} // namespace nlm
