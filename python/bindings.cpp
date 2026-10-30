#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <variant>
#include <optional>
#include <sstream>
#include <typeinfo>

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

// Type aliases for cleaner Python API
typedef std::shared_ptr<Brain> BrainPtr;
typedef std::shared_ptr<Config> ConfigPtr;
typedef std::shared_ptr<AgentBrain> AgentBrainPtr;
typedef std::shared_ptr<SimpleWorld> SimpleWorldPtr;

// Forward declarations for wrapper classes
class BrainWrapper;
class ConfigWrapper;
class AgentBrainWrapper;
class SimpleWorldWrapper;

// Pythonic Brain wrapper with enhanced functionality
class BrainWrapper {
private:
    BrainPtr brain_;
    
    // Internal state caching for performance
    mutable std::optional<size_t> cached_neuron_count_;
    mutable std::optional<size_t> cached_synapse_count_;
    mutable std::optional<size_t> cached_firing_count_;
    mutable std::optional<double> cached_firing_rate_;
    mutable std::optional<size_t> cached_total_spikes_;
    mutable bool cache_valid_;
    
    // Performance optimization
    void invalidate_cache() const { cache_valid_ = false; }
    
public:
    BrainWrapper(BrainPtr brain) : brain_(std::move(brain)), cache_valid_(false) {
        if (!brain_) throw std::invalid_argument("Brain cannot be null");
    }
    
    // Pythonic property access with automatic caching
    size_t get_neuron_count() const {
        if (!cache_valid_ || !cached_neuron_count_) {
            cached_neuron_count_ = brain_->getTotalNeuronCount();
            cache_valid_ = true;
        }
        return *cached_neuron_count_;
    }
    
    size_t get_synapse_count() const {
        if (!cache_valid_ || !cached_synapse_count_) {
            cached_synapse_count_ = brain_->getTotalSynapseCount();
            cache_valid_ = true;
        }
        return *cached_synapse_count_;
    }
    
    size_t get_firing_count() const {
        if (!cache_valid_ || !cached_firing_count_) {
            cached_firing_count_ = brain_->getFiringNeuronCount();
            cache_valid_ = true;
        }
        return *cached_firing_count_;
    }
    
    double get_firing_rate() const {
        if (!cache_valid_ || !cached_firing_rate_) {
            cached_firing_rate_ = brain_->getAverageFiringRate();
            cache_valid_ = true;
        }
        return *cached_firing_rate_;
    }
    
    size_t get_total_spikes() const {
        if (!cache_valid_ || !cached_total_spikes_) {
            cached_total_spikes_ = brain_->getTotalSpikeCount();
            cache_valid_ = true;
        }
        return *cached_total_spikes_;
    }
    
    // Enhanced initialization with Pythonic interface
    void initialize(py::kwargs kwargs = py::kwargs()) {
        // Extract configuration from kwargs if provided
        if (kwargs.contains("config")) {
            auto config_arg = kwargs["config"];
            if (py::isinstance(config_arg, py::dict)) {
                // Create config from dictionary
                auto config = std::make_shared<Config>();
                for (auto item : config_arg) {
                    std::string key = item.first.cast<std::string>();
                    set_config_value(config, key, item.second);
                }
                brain_->setConfig(config);
            }
        }
        
        brain_->initialize();
        invalidate_cache();
    }
    
    // Enhanced step method with multiple parameter combinations
    void step(py::kwargs kwargs = py::kwargs()) {
        int64_t step = kwargs.contains("step") ? kwargs["step"].cast<int64_t>() : 0;
        double time = kwargs.contains("time") ? kwargs["time"].cast<double>() : step * 0.001;
        brain_->step(step, time);
        invalidate_cache();
    }
    
    // Action convenience methods
    std::vector<py::dict> get_top_neurons(int count = 10) const {
        std::vector<py::dict> neurons;
        auto region = brain_->getRegion(RegionId(1));
        if (region) {
            auto all_neurons = region->getAllNeurons();
            size_t actual_count = std::min(static_cast<size_t>(count), all_neurons.size());
            
            for (size_t i = 0; i < actual_count; ++i) {
                auto neuron = all_neurons[i];
                py::dict neuron_info;
                neuron_info["id"] = neuron->getId().value;
                neuron_info["type"] = static_cast<int>(neuron->getType());
                neuron_info["potential"] = neuron->getMembranePotential();
                neuron_info["firing_state"] = static_cast<int>(neuron->getFiringState());
                neuron_info["threshold"] = neuron->getThreshold();
                neuron_info["refractory_remaining"] = neuron->getRefractoryRemaining();
                neuron_info["firing_rate"] = neuron->getFiringRate();
                
                neurons.push_back(neuron_info);
            }
        }
        return neurons;
    }
    
    // Memory state with Pythonic access
    py::dict get_memory_state() const {
        py::dict state;
        
        // Working memory
        if (auto* wm = brain_->getWorkingMemory()) {
            py::dict wm_state;
            wm_state["active_neuron_count"] = wm->getActiveNeuronCount();
            wm_state["total_capacity"] = wm->getTotalCapacity();
            wm_state["activation_pattern"] = std::vector<float>(); // Would need to implement
            state["working_memory"] = wm_state;
        }
        
        // Episodic memory
        if (auto* em = brain_->getEpisodicMemory()) {
            py::dict em_state;
            em_state["episode_count"] = em->getEpisodeCount();
            em_state["recent_episodes"] = em->getRecentEpisodeCount();
            state["episodic_memory"] = em_state;
        }
        
        // Prediction system
        if (auto* ps = brain_->getPredictionSystem()) {
            py::dict ps_state;
            ps_state["prediction_error"] = ps->getPredictionError();
            ps_state["confidence"] = ps->getConfidence();
            ps_state["prediction_value"] = ps->getPrediction();
            state["prediction_system"] = ps_state;
        }
        
        // Neuromodulation
        py::dict neuromod_state;
        neuromod_state["dopamine_level"] = brain_->getDopamineLevel();
        neuromod_state["curiosity_level"] = brain_->getCuriosityLevel();
        neuromod_state["novelty_level"] = brain_->getNoveltyLevel();
        neuromod_state["prediction_error"] = brain_->getPredictionError();
        neuromod_state["reward_prediction_error"] = brain_->getRewardPredictionError();
        state["neuromodulation"] = neuromod_state;
        
        // Development
        py::dict dev_state;
        dev_state["stage"] = static_cast<int>(brain_->getDevelopmentalStage());
        dev_state["age"] = brain_->getDevelopmentAge();
        dev_state["plasticity_rate"] = brain_->getPlasticityRate();
        state["development"] = dev_state;
        
        return state;
    }
    
    // Pythonic serialization methods
    std::string to_json() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"brain_state\": {\n";
        oss << "    \"neuron_count\": " << get_neuron_count() << ",\n";
        oss << "    \"synapse_count\": " << get_synapse_count() << ",\n";
        oss << "    \"firing_rate\": " << get_firing_rate() << ",\n";
        oss << "    \"total_spikes\": " << get_total_spikes() << ",\n";
        oss << "    \"e_i_ratio\": " << brain_->getExcitationInhibitionRatio() << ",\n";
        oss << "    \"developmental_stage\": " << static_cast<int>(brain_->getDevelopmentalStage()) << "\n";
        oss << "  },\n";
        oss << "  \"memory_state\": " << get_memory_state().attr("__repr__")() << ",\n";
        oss << "  \"timestamp\": " << brain_->getCurrentTime() << ",\n";
        oss << "  \"current_step\": " << brain_->getCurrentStep() << "\n";
        oss << "}";
        return oss.str();
    }
    
    py::dict to_dict() const {
        py::dict result;
        
        // Basic brain state
        result["neuron_count"] = get_neuron_count();
        result["synapse_count"] = get_synapse_count();
        result["firing_rate"] = get_firing_rate();
        result["total_spikes"] = get_total_spikes();
        result["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
        result["developmental_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
        result["current_step"] = brain_->getCurrentStep();
        result["current_time"] = brain_->getCurrentTime();
        
        // Memory and system state
        result["memory_state"] = get_memory_state();
        
        // Subsystem status
        py::dict subsystems;
        subsystems["reward_modulation"] = brain_->isRewardModulationEnabled();
        subsystems["structural_plasticity"] = brain_->isStructuralPlasticityEnabled();
        subsystems["development"] = brain_->isDevelopmentEnabled();
        subsystems["curiosity"] = brain_->isCuriosityEnabled();
        result["subsystems"] = subsystems;
        
        return result;
    }
    
    // Comparison and equality
    bool equals(const BrainWrapper& other) const {
        return brain_->getTotalNeuronCount() == other.brain_->getTotalNeuronCount() &&
               brain_->getTotalSynapseCount() == other.brain_->getTotalSynapseCount();
    }
    
    // String representation
    std::string str() const {
        std::ostringstream oss;
        oss << "Brain(neuron_count=" << get_neuron_count()
            << ", synapse_count=" << get_synapse_count()
            << ", firing_rate=" << get_firing_rate() << " Hz)";
        return oss.str();
    }
    
    // Context manager support
    py::object __enter__() {
        initialize();
        return py::cast(this);
    }
    
    void __exit__(py::object, py::object, py::object) {
        reset();
    }
    
    // Resource management
    void reset() {
        brain_->reset();
        invalidate_cache();
    }
    
    bool save(const std::string& filepath) {
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        bool success = brain_->load(filepath);
        if (success) invalidate_cache();
        return success;
    }
    
    // Enhanced configuration methods
    void set_config(const py::dict& config) {
        if (!brain_->getConfig()) {
            auto new_config = std::make_shared<Config>();
            brain_->setConfig(new_config);
        }
        
        for (auto item : config) {
            std::string key = item.first.cast<std::string>();
            set_config_value(brain_->getConfig(), key, item.second);
        }
    }
    
    py::dict get_config() const {
        py::dict result;
        if (brain_->getConfig()) {
            for (const auto& key : brain_->getConfig()->getKeys()) {
                auto value = brain_->getConfig()->get(key);
                if (value.has_value()) {
                    result[key] = value.value();
                }
            }
        }
        return result;
    }
    
    // Statistics and diagnostics
    py::dict get_statistics() const {
        py::dict stats;
        
        // Basic statistics
        stats["neuron_count"] = get_neuron_count();
        stats["synapse_count"] = get_synapse_count();
        stats["firing_rate"] = get_firing_rate();
        stats["total_spikes"] = get_total_spikes();
        
        // Performance metrics
        stats["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
        stats["development_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
        stats["simulation_time"] = brain_->getCurrentTime();
        stats["simulation_step"] = brain_->getCurrentStep();
        
        // Memory metrics
        py::dict memory_stats;
        memory_stats["working_memory_active"] = brain_->getWorkingMemory() ? 
            brain_->getWorkingMemory()->getActiveNeuronCount() : 0;
        memory_stats["episodic_memories"] = brain_->getEpisodicMemory() ? 
            brain_->getEpisodicMemory()->getEpisodeCount() : 0;
        stats["memory"] = memory_stats;
        
        // Subsystem status
        py::dict subsystem_stats;
        auto agent_brain = std::dynamic_pointer_cast<AgentBrain>(brain_);
        if (agent_brain) {
            subsystem_stats["neuromodulation_level"] = agent_brain->getNeuromodulationLevel();
            subsystem_stats["curiosity_level"] = agent_brain->getCuriosityLevel();
            subsystem_stats["novelty_level"] = agent_brain->getNoveltyLevel();
            subsystem_stats["prediction_error"] = agent_brain->getPredictionError();
        }
        stats["subsystems"] = subsystem_stats;
        
        return stats;
    }
    
    // Simulation convenience methods
    void run_for_steps(int steps, double timestep = 0.001) {
        for (int i = 0; i < steps; ++i) {
            step(i, i * timestep);
        }
    }
    
    double run_until_time(double target_time, double timestep = 0.001) {
        double current_time = brain_->getCurrentTime();
        double elapsed = 0.0;
        
        while (current_time < target_time && elapsed < timestep * 10000) {
            int step = brain_->getCurrentStep() + 1;
            brain_->step(step, current_time + timestep);
            current_time += timestep;
            elapsed += timestep;
            invalidate_cache();
        }
        
        return brain_->getCurrentTime();
    }
    
    // Benchmarking
    py::dict benchmark(int steps, int warmup = 100) {
        // Warmup
        for (int i = 0; i < warmup; ++i) {
            step(i, i * 0.001);
        }
        
        // Benchmark
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = warmup; i < warmup + steps; ++i) {
            step(i, i * 0.001);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        double steps_per_second = steps / (duration.count() / 1000.0);
        
        py::dict result;
        result["steps"] = steps;
        result["warmup_steps"] = warmup;
        result["duration_ms"] = duration.count();
        result["steps_per_second"] = steps_per_second;
        result["average_step_time_ms"] = duration.count() / static_cast<double>(steps);
        
        return result;
    }
    
    // Comparison operators
    bool operator==(const BrainWrapper& other) const {
        return equals(other);
    }
    
    bool operator!=(const BrainWrapper& other) const {
        return !(*this == other);
    }
    
private:
    void set_config_value(std::shared_ptr<Config> config, const std::string& key, py::object value) {
        if (py::isinstance<py::int_>(value)) {
            config->set(key, value.cast<int64_t>(), ConfigSource::CommandLine);
        } else if (py::isinstance<py::float_>(value)) {
            config->set(key, value.cast<double>(), ConfigSource::CommandLine);
        } else if (py::isinstance<py::bool_>(value)) {
            config->set(key, value.cast<bool>(), ConfigSource::CommandLine);
        } else if (py::isinstance<py::str>(value)) {
            config->set(key, value.cast<std::string>(), ConfigSource::CommandLine);
        } else {
            throw py::value_error("Unsupported configuration value type for key: " + key);
        }
    }
};

// Enhanced Config wrapper
class ConfigWrapper {
private:
    ConfigPtr config_;
    
public:
    ConfigWrapper() : config_(std::make_shared<Config>()) {}
    
    explicit ConfigWrapper(const py::dict& values) : config_(std::make_shared<Config>()) {
        for (auto item : values) {
            std::string key = item.first.cast<std::string>();
            set_config_value(key, item.second);
        }
    }
    
    // Pythonic property access
    bool has(const std::string& key) const { return config_->has(key); }
    
    std::vector<std::string> get_keys() const { return config_->getKeys(); }
    
    py::dict get_all() const {
        py::dict result;
        for (const auto& key : get_keys()) {
            auto value = config_->get(key);
            if (value.has_value()) {
                result[key] = value.value();
            }
        }
        return result;
    }
    
    void set(const std::string& key, py::object value) {
        set_config_value(key, value);
    }
    
    void load_from_file(const std::string& filepath) {
        if (config_->loadFromFile(filepath)) {
            NLM_LOG_INFO("Loaded configuration from: " + filepath);
        } else {
            throw py::runtime_error("Failed to load configuration from file: " + filepath);
        }
    }
    
    void save_to_file(const std::string& filepath) const {
        if (!config_->saveToFile(filepath)) {
            throw py::runtime_error("Failed to save configuration to file: " + filepath);
        }
    }
    
    std::string get_summary() const { return config_->summary(); }
    
    std::string str() const {
        return "Config(keys=" + std::to_string(get_keys().size()) + ")";
    }
    
    // Enhanced export/import methods
    std::string to_json() const {
        return config_->exportToJSON();
    }
    
    void from_json(const std::string& jsonStr) {
        config_->importFromJSON(jsonStr);
    }
    
    std::string to_yaml() const {
        return config_->exportToYAML();
    }
    
    void from_yaml(const std::string& yamlStr) {
        config_->importFromYAML(yamlStr);
    }
    
    // Validation and error checking
    bool validate() const {
        return config_->validate();
    }
    
    py::list get_validation_errors() const {
        py::list errors;
        auto cpp_errors = config_->getValidationErrors();
        for (const auto& error : cpp_errors) {
            errors.append(error);
        }
        return errors;
    }
    
    // Configuration merging and comparison
    bool merge(const ConfigWrapper& other, bool overwrite = true) {
        return config_->merge(*other.config_, overwrite);
    }
    
    ConfigWrapper get_subset(const std::vector<std::string>& keys) const {
        return ConfigWrapper(config_->getSubset(keys));
    }
    
    // Configuration optimization
    void optimize_for_simulation() {
        config_->optimizeForSimulation();
    }
    
    void enable_debug_mode() {
        config_->enableDebugMode();
    }
    
    void disable_debug_mode() {
        config_->disableDebugMode();
    }
    
    // Serialization
    std::vector<uint8_t> serialize() const {
        return config_->serialize();
    }
    
    void deserialize(const std::vector<uint8_t>& data) {
        config_->deserialize(data);
    }
    
    // Comparison
    bool equals(const ConfigWrapper& other) const {
        return *config_ == *other.config_;
    }
    
    bool operator==(const ConfigWrapper& other) const {
        return equals(other);
    }
    
    bool operator!=(const ConfigWrapper& other) const {
        return !(*this == other);
    }
    
private:
    void set_config_value(const std::string& key, py::object value) {
        if (py::isinstance<py::int_>(value)) {
            config_->set(key, value.cast<int64_t>(), ConfigSource::CommandLine);
        } else if (py::isinstance<py::float_>(value)) {
            config_->set(key, value.cast<double>(), ConfigSource::CommandLine);
        } else if (py::isinstance<py::bool_>(value)) {
            config_->set(key, value.cast<bool>(), ConfigSource::CommandLine);
        } else if (py::isinstance<py::str>(value)) {
            config_->set(key, value.cast<std::string>(), ConfigSource::CommandLine);
        } else {
            throw py::value_error("Unsupported configuration value type for key: " + key);
        }
    }
};

// Enhanced AgentBrain wrapper
class AgentBrainWrapper {
private:
    std::shared_ptr<AgentBrain> agent_brain_;
    
public:
    AgentBrainWrapper(std::shared_ptr<Brain> brain) : agent_brain_(std::make_shared<AgentBrain>(brain)) {}
    
    void initialize(std::shared_ptr<SimpleWorldWrapper> world) {
        agent_brain_->initialize(world->get_world());
    }
    
    // Enhanced sensory processing
    void process_sensory_input(const py::dict& percept) {
        SensoryPercept pp;
        
        if (percept.contains("vision")) {
            auto vision_data = percept["vision"].cast<std::vector<float>>();
            size_t width = percept.contains("vision_width") ? 
                percept["vision_width"].cast<size_t>() : 0;
            size_t height = percept.contains("vision_height") ? 
                percept["vision_height"].cast<size_t>() : 0;
            
            if (width > 0 && height > 0) {
                Vision vision(width, height, vision_data);
                pp.setVision(vision);
            }
        }
        
        if (percept.contains("internal")) {
            auto internal_data = percept["internal"].cast<std::vector<float>>();
            InternalSignals internal;
            for (float val : internal_data) {
                internal.addSignal(val);
            }
            pp.setInternal(internal);
        }
        
        if (percept.contains("audio")) {
            auto audio_data = percept["audio"].cast<std::vector<float>>();
            Audio audio(audio_data.size() > 0 ? 44100 : 0, audio_data.size());
            audio.setData(audio_data);
            // Would need to add method to set audio on percept
        }
        
        agent_brain_->processSensoryInput(pp);
    }
    
    // Enhanced motor decoding
    py::dict decode_motor_command() const {
        Action action = agent_brain_->decodeMotorCommand();
        py::dict result;
        
        result["type"] = static_cast<int>(action.getType());
        result["parameters"] = action.getParameters();
        result["name"] = action.getName();
        result["value"] = action.getValue();
        
        return result;
    }
    
    // Neuromodulation with Pythonic interface
    void apply_reward_modulation(double reward, double predicted_reward = 0.0) {
        agent_brain_->applyRewardModulation(reward, predicted_reward);
    }
    
    void update_development(double timestep) {
        agent_brain_->updateDevelopment(timestep);
    }
    
    // Subsystem status and control
    py::dict get_subsystem_status() const {
        py::dict status;
        
        status["reward_modulation"] = agent_brain_->isRewardModulationEnabled();
        status["structural_plasticity"] = agent_brain_->isStructuralPlasticityEnabled();
        status["development"] = agent_brain_->isDevelopmentEnabled();
        status["curiosity"] = agent_brain_->isCuriosityEnabled();
        
        return status;
    }
    
    // Convenience methods
    double get_curiosity_level() const { return agent_brain_->getCuriosityLevel(); }
    double get_novelty_level() const { return agent_brain_->getNoveltyLevel(); }
    double get_prediction_error() const { return agent_brain_->getPredictionError(); }
    double get_neuromodulation_level() const { return agent_brain_->getNeuromodulationLevel(); }
    int get_developmental_stage() const { return static_cast<int>(agent_brain_->getDevelopmentalStage()); }
    
    // Enable/disable subsystems
    void enable_reward_modulation(bool enable) { agent_brain_->enableRewardModulation(enable); }
    void enable_structural_plasticity(bool enable) { agent_brain_->enableStructuralPlasticity(enable); }
    void enable_development(bool enable) { agent_brain_->enableDevelopment(enable); }
    void enable_curiosity(bool enable) { agent_brain_->enableCuriosity(enable); }
    
    // Reset functionality
    void reset() {
        agent_brain_->reset();
    }
    
    // Access underlying brain
    std::shared_ptr<Brain> get_brain() const {
        return agent_brain_->getBrain();
    }
    
    // Context manager
    py::object __enter__() {
        return py::cast(this);
    }
    
    void __exit__(py::object, py::object, py::object) {
        reset();
    }
};

// Enhanced SimpleWorld wrapper
class SimpleWorldWrapper {
private:
    std::shared_ptr<SimpleWorld> world_;
    
public:
    SimpleWorldWrapper() : world_(std::make_shared<SimpleWorld>()) {}
    
    void configure(size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    void reset() {
        world_->reset();
    }
    
    void update(double timestep) {
        world_->update(timestep);
    }
    
    void set_agent_start(double x, double y) {
        world_->setAgentStart(x, y);
    }
    
    // Enhanced sensory percept extraction
    py::dict get_sensory_percept() const {
        SensoryPercept percept = world_->getSensoryPercept();
        py::dict result;
        
        // Extract vision data
        Vision vision = percept.getVision();
        if (vision.getWidth() > 0 && vision.getHeight() > 0) {
            std::vector<float> visionData = vision.getData();
            result["vision"] = visionData;
            result["vision_width"] = vision.getWidth();
            result["vision_height"] = vision.getHeight();
            result["vision_channels"] = vision.getChannels();
        }
        
        // Extract internal signals
        InternalSignals internal = percept.getInternal();
        std::vector<float> internalData;
        // Would need to add method to get all signals from InternalSignals
        result["internal"] = internalData;
        
        // Extract audio
        Audio audio = percept.getAudio();
        if (audio.getNumSamples() > 0) {
            std::vector<float> audioData = audio.getData();
            result["audio"] = audioData;
            result["sample_rate"] = audio.getSampleRate();
        }
        
        return result;
    }
    
    // Enhanced agent body extraction
    py::dict get_agent_body() const {
        AgentBody body = world_->getAgentBody();
        py::dict result;
        
        result["x"] = body.x;
        result["y"] = body.y;
        result["orientation"] = body.orientation;
        result["velocity_x"] = body.velocityX;
        result["velocity_y"] = body.velocityY;
        result["angular_velocity"] = body.angularVelocity;
        result["energy"] = body.energy;
        result["health"] = body.health;
        result["age"] = body.age;
        result["is_moving"] = body.isMoving;
        result["is_turning"] = body.isTurning;
        result["last_action_time"] = body.lastActionTime;
        
        return result;
    }
    
    // Enhanced motor command application
    void apply_motor_command(const py::dict& action, double current_time) {
        Action motorAction;
        
        // Parse action from dictionary with enhanced support
        if (action.contains("type")) {
            int type = action["type"].cast<int>();
            motorAction.setType(static_cast<ActionType>(type));
        }
        
        if (action.contains("parameters")) {
            std::vector<float> params = action["parameters"].cast<std::vector<float>>();
            motorAction.setParameters(params);
        }
        
        if (action.contains("name")) {
            std::string name = action["name"].cast<std::string>();
            // Would need to add setName method to Action if available
        }
        
        world_->applyMotorCommand(motorAction, current_time);
    }
    
    // World management with enhanced API
    void add_object(double x, double y, int type, double value = 0.0, double radius = 0.5) {
        WorldObject obj(x, y, static_cast<WorldObjectType>(type), value, radius);
        world_->addObject(obj);
    }
    
    void remove_object(double x, double y) {
        world_->removeObject(x, y);
    }
    
    bool is_valid_position(double x, double y) const {
        return world_->isValidPosition(x, y);
    }
    
    // Properties and state
    size_t get_width() const { return world_->getWidth(); }
    size_t get_height() const { return world_->getHeight(); }
    size_t get_max_energy() const { return world_->getMaxEnergy(); }
    void set_max_energy(size_t e) { world_->setMaxEnergy(e); }
    double get_energy_decay_rate() const { return world_->getEnergyDecayRate(); }
    void set_energy_decay_rate(double r) { world_->setEnergyDecayRate(r); }
    double get_simulation_time() const { return world_->getSimulationTime(); }
    void set_random_seed(int64_t seed) { world_->setRandomSeed(seed); }
    int64_t get_random_seed() const { return world_->getRandomSeed(); }
    
    // Access to underlying world for advanced users
    std::shared_ptr<SimpleWorld> get_world() const {
        return world_;
    }
    
    // Context manager support
    py::object __enter__() {
        reset();
        return py::cast(this);
    }
    
    void __exit__(py::object, py::object, py::object) {
        // Cleanup if needed
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        
        A Pythonic interface to the NLM C++ neural simulation framework.
        
        Key Features:
        - Pythonic property access with automatic caching
        - Enhanced initialization with kwargs support
        - Context managers for resource management
        - Advanced serialization (JSON/YAML)
        - Error handling with Pythonic exceptions
        - Performance optimization helpers
        - Benchmark and profiling tools
        - Brain simulation convenience methods
        - Memory and subsystem monitoring
        - Configuration validation and optimization
        
        Usage Examples:
            import pynlm
            
            # Create brain with enhanced API
            config = pynlm.Config({
                'brain.neuron_count': 1000,
                'brain.synapse_density': 0.1
            })
            brain = pynlm.Brain(config)
            brain.initialize()
            
            # Use context manager
            with pynlm.SimulationContext(brain) as sim:
                for step in range(100):
                    sim.step()
                    print(f"Step {step}: {sim.get_statistics()}")
            
            # Enhanced simulation methods
            brain.run_for_steps(500)
            brain.run_until_time(10.0)
            
            # Benchmark
            stats = brain.benchmark(100)
            print(f"Performance: {stats['steps_per_second']:.2f} steps/sec")
    
    "pbdoc";

    // Register enhanced exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    py::register_exception<std::logic_error>(m, "RuntimeError");
    py::register_exception<std::domain_error>(m, "ValueError");

    // Core identifier types with Pythonic representations
    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        })
        .def("__int__", [](const NeuronId& id) { return id.value; });

    py::class_<SynapseId>(m, "SynapseId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
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
            return "RegionId(" + std::to_string(id.value) + ")";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enums with Pythonic naming
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
    py::class_<ConfigWrapper>(m, "Config")
        .def(py::init<>())
        .def(py::init<const py::dict&>(), py::arg("values"))
        .def("has", &ConfigWrapper::has, "Check if a configuration key exists")
        .def("getKeys", &ConfigWrapper::getKeys, "Get all configuration keys")
        .def("getAll", &ConfigWrapper::getAll, "Get all configuration values as dict")
        .def("set", &ConfigWrapper::set, "Set a configuration value",
             py::arg("key"), py::arg("value"))
        .def("loadFromFile", &ConfigWrapper::loadFromFile, "Load configuration from file",
             py::arg("filepath"))
        .def("saveToFile", &ConfigWrapper::saveToFile, "Save configuration to file",
             py::arg("filepath"))
        .def("getSummary", &ConfigWrapper::getSummary, "Get configuration summary")
        .def("to_json", &ConfigWrapper::to_json, "Export configuration as JSON",
             py::arg("indent") = 2)
        .def("from_json", &ConfigWrapper::from_json, "Import configuration from JSON",
             py::arg("json_str"))
        .def("to_yaml", &ConfigWrapper::to_yaml, "Export configuration as YAML")
        .def("from_yaml", &ConfigWrapper::from_yaml, "Import configuration from YAML",
             py::arg("yaml_str"))
        .def("validate", &ConfigWrapper::validate, "Validate configuration")
        .def("getValidationErrors", &ConfigWrapper::get_validation_errors, 
             "Get validation errors as list")
        .def("optimizeForSimulation", &ConfigWrapper::optimizeForSimulation,
             "Optimize configuration for simulation")
        .def("enableDebugMode", &ConfigWrapper::enableDebugMode,
             "Enable debug mode")
        .def("disableDebugMode", &ConfigWrapper::disableDebugMode,
             "Disable debug mode")
        .def("merge", &ConfigWrapper::merge, "Merge with another configuration",
             py::arg("other"), py::arg("overwrite") = true)
        .def("getSubset", &ConfigWrapper::get_subset, "Get subset of configuration",
             py::arg("keys"))
        .def("serialize", &ConfigWrapper::serialize, "Serialize configuration")
        .def("deserialize", &ConfigWrapper::deserialize, "Deserialize configuration",
             py::arg("data"))
        .def("__repr__", [](const ConfigWrapper& cfg) {
            return cfg.str();
        })
        .def("__eq__", &ConfigWrapper::operator==, "Check if two configs are equal")
        .def("__ne__", &ConfigWrapper::operator!=, "Check if two configs are different");

    // Enhanced SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", "Base class for sensory input")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, "Set the timestamp",
             py::arg("timestamp"));

    // Enhanced Vision class with Pythonic constructors
    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", &Vision::setData, "Set vision data", py::arg("data"))
        .def("getWidth", &Vision::getWidth, "Get vision width")
        .def("getHeight", &Vision::getHeight, "Get vision height")
        .def("getChannels", &Vision::getChannels, "Get number of channels");

    py::class_<Audio, SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", &Audio::setData, "Set audio data", py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, "Set sample rate", py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate, "Get sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, "Add a signal value", py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals");

    // Enhanced Action class with Pythonic methods
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, "Set action type", py::arg("type"))
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, "Set action parameters", py::arg("params"))
        .def("getName", &Action::getName, "Get action name")
        .def("getValue", &Action::getValue, "Get action value")
        .def("setValue", &Action::setValue, "Set action value", py::arg("value"))
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("to_dict", [](const Action& a) {
            py::dict result;
            result["type"] = static_cast<int>(a.getType());
            result["parameters"] = a.getParameters();
            result["name"] = a.getName();
            result["value"] = a.getValue();
            return result;
        }, "Convert action to Python dictionary")
        .def(py::init([](py::dict d) {
            Action action;
            if (d.contains("type")) {
                action.setType(static_cast<ActionType>(d["type"].cast<int>()));
            }
            if (d.contains("parameters")) {
                action.setParameters(d["parameters"].cast<std::vector<float>>());
            }
            if (d.contains("name")) {
                // Would need to add setName method if available
            }
            if (d.contains("value")) {
                action.setValue(d["value"].cast<float>());
            }
            return action;
        }), py::arg("d"), "Create action from dictionary");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            return std::hypot(obj.x - x, obj.y - y);
        }, "Calculate distance to position", py::arg("x"), py::arg("y"))
        .def("__repr__", [](const WorldObject& obj) {
            return "WorldObject(x=" + std::to_string(obj.x) + ", y=" + std::to_string(obj.y) +
                   ", type=" + std::to_string(static_cast<int>(obj.type)) + ")";
        });

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody")
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
        .def("reset", &AgentBody::reset, "Reset body state")
        .def("getPosition", [](const AgentBody& b) {
            return py::make_tuple(b.x, b.y);
        }, "Get position as tuple")
        .def("getVelocity", [](const AgentBody& b) {
            return py::make_tuple(b.velocityX, b.velocityY);
        }, "Get velocity as tuple")
        .def("distance_to", [](const AgentBody& body, float x, float y) {
            return std::hypot(body.x - x, body.y - y);
        }, "Calculate distance to position", py::arg("x"), py::arg("y"))
        .def("__repr__", [](const AgentBody& body) {
            return "AgentBody(x=" + std::to_string(body.x) + ", y=" + std::to_string(body.y) + ")";
        });

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, "Set vision data", py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, "Set touch data", py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, "Set internal signals", py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception")
        .def("setProprioception", &SensoryPercept::setProprioception, "Set proprioception", py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, "Set audio data", py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, "Set timestamp", py::arg("timestamp"))
        .def("to_dict", [](const SensoryPercept& sp) {
            py::dict result;
            Vision vision = sp.getVision();
            if (vision.getWidth() > 0) {
                std::vector<float> visionData = vision.getData();
                result["vision"] = visionData;
                result["vision_width"] = vision.getWidth();
                result["vision_height"] = vision.getHeight();
                result["vision_channels"] = vision.getChannels();
            }
            return result;
        }, "Convert sensory percept to Python dictionary");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorldWrapper>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorldWrapper::configure,
             "Configure world dimensions", py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorldWrapper::reset, "Reset world state")
        .def("update", &SimpleWorldWrapper::update, "Update world state", py::arg("timestep"))
        .def("setAgentStart", &SimpleWorldWrapper::setAgentStart, "Set agent start position",
             py::arg("x"), py::arg("y"))
        .def("applyMotorCommand", &SimpleWorldWrapper::apply_motor_command,
             "Apply motor command", py::arg("action"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorldWrapper::get_sensory_percept,
             "Get sensory percept", py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorldWrapper::get_agent_body,
             "Get agent body state", py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorldWrapper::add_object, "Add world object",
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0, py::arg("radius") = 0.5)
        .def("removeObject", &SimpleWorldWrapper::remove_object, "Remove object",
             py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorldWrapper::is_valid_position, "Check if position is valid",
             py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorldWrapper::get_width, "Get world width")
        .def("getHeight", &SimpleWorldWrapper::get_height, "Get world height")
        .def("getMaxEnergy", &SimpleWorldWrapper::get_max_energy, "Get max energy")
        .def("setMaxEnergy", &SimpleWorldWrapper::set_max_energy, "Set max energy", py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorldWrapper::get_energy_decay_rate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorldWrapper::set_energy_decay_rate, "Set energy decay rate", py::arg("r"))
        .def("getSimulationTime", &SimpleWorldWrapper::get_simulation_time, "Get simulation time")
        .def("setRandomSeed", &SimpleWorldWrapper::set_random_seed, "Set random seed", py::arg("seed"))
        .def("getRandomSeed", &SimpleWorldWrapper::get_random_seed, "Get random seed")
        .def("__enter__", &SimpleWorldWrapper::__enter__, "Context manager entry")
        .def("__exit__", &SimpleWorldWrapper::__exit__, "Context manager exit");

    // Enhanced Brain class
    py::class_<BrainWrapper>(m, "Brain")
        .def(py::init([](std::shared_ptr<Config> config) {
            auto brain = std::make_shared<Brain>(config);
            return std::make_unique<BrainWrapper>(brain);
        }), py::arg("config"), "Create brain with configuration")
        .def("initialize", py::overload_cast<>(&BrainWrapper::initialize),
             "Initialize brain with default settings")
        .def("initialize", py::overload_cast<const py::dict&>(&BrainWrapper::initialize),
             "Initialize brain with configuration dict", py::arg("config"))
        .def("step", py::overload_cast<int64_t>(&BrainWrapper::step),
             "Step brain with step number", py::arg("step"))
        .def("step", py::overload_cast<int64_t, double>(&BrainWrapper::step),
             "Step brain with step number and time", py::arg("step"), py::arg("time"))
        .def("reset", &BrainWrapper::reset, "Reset brain state")
        .def("receiveSensoryInput", &BrainWrapper::process_sensory_input,
             "Receive sensory input", py::arg("input"))
        .def("produceAction", &Brain::produce_action, "Produce action")
        .def("save", &BrainWrapper::save, "Save brain state", py::arg("filepath"))
        .def("load", &BrainWrapper::load, "Load brain state", py::arg("filepath"))
        .def("addRegion", &Brain::add_region, "Add a region", py::arg("name") = "")
        .def("getRegion", &Brain::get_region, "Get region by ID", py::arg("id"),
             py::return_value_policy::reference_internal)
        .def("getRegionCount", &Brain::get_region_count, "Get region count")
        .def("getRegionIds", &Brain::get_region_ids, "Get all region IDs")
        .def("getRegions", &Brain::get_regions, "Get all regions", py::return_value_policy::reference_internal)
        .def("getTotalNeuronCount", &Brain::get_total_neuron_count, "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::get_total_synapse_count, "Get total synapse count")
        .def("getActiveNeuronCount", &Brain::get_active_neuron_count, "Get active neuron count")
        .def("getFiringNeuronCount", &Brain::get_firing_neuron_count, "Get firing neuron count")
        .def("getAverageFiringRate", &Brain::get_average_firing_rate, "Get average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::get_excitation_inhibition_ratio, "Get E/I ratio")
        .def("getTotalSpikeCount", &Brain::get_total_spike_count, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::get_developmental_stage, "Get developmental stage")
        .def("setDevelopmentalStage", &Brain::set_developmental_stage, "Set developmental stage", py::arg("stage"))
        .def("getConfig", &Brain::get_config, "Get configuration", py::return_value_policy::reference_internal)
        .def("logStatus", &Brain::log_status, "Log brain status")
        .def("to_dict", &BrainWrapper::to_dict, "Convert brain to Python dictionary")
        .def("to_json", &BrainWrapper::to_json, "Convert brain to JSON string")
        .def("__repr__", [](const BrainWrapper& brain) {
            return brain.str();
        })
        .def("__eq__", &BrainWrapper::operator==, "Check if two brains are equal")
        .def("__ne__", &BrainWrapper::operator!=, "Check if two brains are different")
        .def("get_top_neurons", &BrainWrapper::get_top_neurons,
             "Get top neurons with metadata", py::arg("count") = 10)
        .def("get_memory_state", &BrainWrapper::get_memory_state,
             "Get detailed memory state", py::return_value_policy::reference_internal)
        .def("get_statistics", &BrainWrapper::get_statistics, "Get comprehensive brain statistics")
        .def("run_for_steps", &BrainWrapper::run_for_steps, "Run simulation for specified number of steps",
             py::arg("steps"), py::arg("timestep") = 0.001)
        .def("run_until_time", &BrainWrapper::run_until_time, "Run simulation until specific time",
             py::arg("target_time"), py::arg("timestep") = 0.001)
        .def("benchmark", &BrainWrapper::benchmark, "Benchmark brain performance",
             py::arg("steps"), py::arg("warmup") = 100)
        .def("__enter__", &BrainWrapper::__enter__, "Context manager entry")
        .def("__exit__", &BrainWrapper::__exit__, "Context manager exit");

    // Enhanced AgentBrain class
    py::class_<AgentBrainWrapper>(m, "AgentBrain")
        .def(py::init([](std::shared_ptr<Brain> brain) {
            auto agentBrain = std::make_shared<AgentBrain>(brain);
            return std::make_unique<AgentBrainWrapper>(agentBrain);
        }), py::arg("brain"), "Create agent brain interface")
        .def("initialize", &AgentBrainWrapper::initialize,
             "Initialize with world", py::arg("world"))
        .def("getSensoryInputSize", &AgentBrain::get_sensory_input_size,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::get_motor_output_size,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrainWrapper::process_sensory_input,
             "Process sensory input", py::arg("percept"))
        .def("decodeMotorCommand", &AgentBrainWrapper::decode_motor_command,
             "Decode motor command to action", py::return_value_policy::reference_internal)
        .def("applyRewardModulation", &AgentBrainWrapper::apply_reward_modulation,
             "Apply reward modulation", py::arg("reward"), py::arg("predicted_reward") = 0.0)
        .def("updateDevelopment", &AgentBrainWrapper::update_development,
             "Update development system", py::arg("timestep"))
        .def("getDevelopmentalStage", &AgentBrainWrapper::get_developmental_stage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrainWrapper::get_neuromodulation_level,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrainWrapper::get_curiosity_level,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrainWrapper::get_novelty_level,
             "Get novelty level")
        .def("getPredictionError", &AgentBrainWrapper::get_prediction_error,
             "Get prediction error")
        .def("reset", &AgentBrainWrapper::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::get_brain, "Get underlying brain", py::return_value_policy::reference_internal)
        .def("enableRewardModulation", &AgentBrainWrapper::enable_reward_modulation,
             "Enable/disable reward modulation", py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrainWrapper::enable_structural_plasticity,
             "Enable/disable structural plasticity", py::arg("enable"))
        .def("enableDevelopment", &AgentBrainWrapper::enable_development,
             "Enable/disable development", py::arg("enable"))
        .def("enableCuriosity", &AgentBrainWrapper::enable_curiosity,
             "Enable/disable curiosity", py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::is_reward_modulation_enabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::is_structural_plasticity_enabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::is_development_enabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::is_curiosity_enabled,
             "Check if curiosity is enabled")
        .def("getSubsystemStatus", &AgentBrainWrapper::get_subsystem_status,
             "Get subsystem status as dictionary")
        .def("__enter__", &AgentBrainWrapper::__enter__, "Context manager entry")
        .def("__exit__", &AgentBrainWrapper::__exit__, "Context manager exit");

    // Factory functions
    m.def("createDefaultConfig", []() {
        return std::make_unique<ConfigWrapper>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_unique<BrainWrapper>(std::make_shared<Brain>(config));
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() {
        return std::make_unique<SimpleWorldWrapper>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_unique<AgentBrainWrapper>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants and utilities
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Convenience aliases for backward compatibility
    m.def("Config", [](py::dict values) { return std::make_unique<ConfigWrapper>(values); });
    m.def("Brain", [](std::shared_ptr<Config> config) {
        return std::make_unique<BrainWrapper>(std::make_shared<Brain>(config));
    });
    m.def("SimpleWorld", []() { return std::make_unique<SimpleWorldWrapper>(); });
    m.def("AgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_unique<AgentBrainWrapper>(brain);
    });
    
    // Version and metadata
    m.attr("__version__") = "0.1.0-enhanced";
    m.attr("__description__") = "NLM Enhanced Python Bindings with Pythonic API Design";
    
    // Experimental features
    m.def("benchmark", [](std::unique_ptr<BrainWrapper>& brain, int steps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return py::dict({
            {"steps", steps},
            {"duration_ms", duration.count()},
            {"steps_per_second", steps / (duration.count() / 1000.0)}
        });
    }, "Benchmark brain performance", py::arg("brain"), py::arg("steps"));
    
    // Advanced configuration utilities
    m.def("validate_config", [](const py::dict& config_dict) {
        auto config = std::make_unique<ConfigWrapper>(config_dict);
        return config->validate();
    }, "Validate configuration dictionary");
    
    m.def("get_config_errors", [](const py::dict& config_dict) {
        auto config = std::make_unique<ConfigWrapper>(config_dict);
        return config->get_validation_errors();
    }, "Get configuration validation errors");
}

} // namespace nlm
