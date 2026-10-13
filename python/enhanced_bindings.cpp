#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
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
#include "../src/development/DevConsole.hpp"

namespace py = pybind11;
namespace nlm {

// Python-style configuration manager with enhanced features
class PythonConfig {
public:
    PythonConfig() : config_(std::make_shared<Config>()) {}
    PythonConfig(const std::string& filepath) : config_(std::make_shared<Config>()) {
        load_from_file(filepath);
    }
    
    // Configuration loading
    bool load_from_file(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }
    
    bool load_from_dict(const py::dict& settings) {
        for (auto item : settings) {
            std::string key = py::cast<std::string>(item.first);
            
            py::object value_obj = item.second;
            if (value_obj.is(py::int_())) {
                config_->set(key, value_obj.cast<int64_t>(), ConfigSource::Runtime);
            } else if (value_obj.is(py::float_())) {
                config_->set(key, value_obj.cast<double>(), ConfigSource::Runtime);
            } else if (value_obj.is(py::bool_())) {
                config_->set(key, value_obj.cast<bool>(), ConfigSource::Runtime);
            } else if (value_obj.is(py::str())) {
                config_->set(key, value_obj.cast<std::string>(), ConfigSource::Runtime);
            } else if (value_obj.is(py::list())) {
                auto list = value_obj.cast<std::vector<double>>();
                config_->set(key, ConfigValue(list), ConfigSource::Runtime);
            } else {
                config_->set(key, ConfigValue(std::string("unknown_type")), ConfigSource::Runtime);
            }
        }
        return true;
    }
    
    // Get values with Python-friendly access
    int get_int(const std::string& key, int default_value = 0) const {
        auto val = config_->get<int64_t>(key);
        return val.has_value() ? val.value() : default_value;
    }
    
    float get_float(const std::string& key, float default_value = 0.0f) const {
        auto val = config_->get<double>(key);
        return val.has_value() ? static_cast<float>(val.value()) : default_value;
    }
    
    bool get_bool(const std::string& key, bool default_value = false) const {
        auto val = config_->get<bool>(key);
        return val.has_value() ? val.value() : default_value;
    }
    
    std::string get_string(const std::string& key, const std::string& default_value = "") const {
        auto val = config_->get<std::string>(key);
        return val.has_value() ? val.value() : default_value;
    }
    
    std::vector<float> get_vector(const std::string& key) const {
        auto val = config_->get<std::vector<double>>(key);
        if (val.has_value()) {
            std::vector<float> result(val.value().size());
            std::transform(val.value().begin(), val.value().end(), result.begin(),
                [](double v) { return static_cast<float>(v); });
            return result;
        }
        return {};
    }
    
    // Set values
    void set_int(const std::string& key, int value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set_float(const std::string& key, float value) {
        config_->set(key, static_cast<double>(value), ConfigSource::Runtime);
    }
    
    void set_bool(const std::string& key, bool value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set_string(const std::string& key, const std::string& value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    // Configuration statistics
    py::dict get_stats() const {
        auto stats = config_->getStats();
        py::dict result;
        result["total_entries"] = stats.totalEntries;
        result["file_entries"] = stats.fileEntries;
        result["command_line_entries"] = stats.commandLineEntries;
        result["runtime_entries"] = stats.runtimeEntries;
        result["duplicate_keys"] = stats.duplicateKeys;
        return result;
    }
    
    // Configuration validation
    std::vector<std::string> validate() const {
        return config_->validate();
    }
    
    // Get configuration help
    std::string get_help(const std::string& key = "") const {
        return config_->getHelp(key);
    }
    
    // Configuration summary
    std::string summary() const {
        return config_->summary();
    }
    
    // Internal access
    std::shared_ptr<Config> get_internal() const { return config_; }
    
private:
    std::shared_ptr<Config> config_;
};

// Enhanced brain with Python-friendly APIs
class PythonBrain {
public:
    PythonBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    
    // Initialization
    bool initialize() {
        return brain_->initialize();
    }
    
    bool reset() {
        brain_->reset();
        brain_->initialize();
        return true;
    }
    
    // Brain state queries
    size_t get_total_neuron_count() const { return brain_->getTotalNeuronCount(); }
    size_t get_total_synapse_count() const { return brain_->getTotalSynapseCount(); }
    size_t get_active_neuron_count() const { return brain_->getActiveNeuronCount(); }
    size_t get_firing_neuron_count() const { return brain_->getFiringNeuronCount(); }
    float get_average_firing_rate() const { return brain_->getAverageFiringRate(); }
    float get_excitation_inhibition_ratio() const { return brain_->getExcitationInhibitionRatio(); }
    size_t get_total_spike_count() const { return brain_->getTotalSpikeCount(); }
    double get_developmental_stage() const { return static_cast<double>(brain_->getDevelopmentalStage()); }
    
    // Development control
    void set_developmental_stage(double stage) {
        brain_->setDevelopmentalStage(static_cast<DevelopmentalStage>(static_cast<int>(stage)));
    }
    
    const char* get_developmental_stage_name() const {
        return brain_->getDevelopmentalStageName();
    }
    
    // Sensory input
    void receive_sensory_input(std::shared_ptr<SensoryInput> input) {
        brain_->receiveSensoryInput(input);
    }
    
    // Action production
    std::shared_ptr<Action> produce_action() {
        return brain_->produceAction();
    }
    
    // Simulation control
    void step(size_t step_number, double time = 0.0) {
        brain_->step(step_number, time);
    }
    
    // Simulation utilities
    py::dict get_brain_summary() const {
        py::dict result;
        result["neuron_count"] = get_total_neuron_count();
        result["synapse_count"] = get_total_synapse_count();
        result["firing_neuron_count"] = get_firing_neuron_count();
        result["average_firing_rate"] = get_average_firing_rate();
        result["excitation_inhibition_ratio"] = get_excitation_inhibition_ratio();
        result["total_spikes"] = get_total_spike_count();
        result["developmental_stage"] = get_developmental_stage();
        result["developmental_stage_name"] = get_developmental_stage_name();
        return result;
    }
    
    // Internal access
    std::shared_ptr<Brain> get_internal() const { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Enhanced agent with Python-friendly APIs
class PythonAgent {
public:
    PythonAgent(std::shared_ptr<Brain> brain) : agent_brain_(std::make_shared<AgentBrain>(brain)) {}
    
    // Initialization
    bool initialize(std::shared_ptr<SimpleWorld> world) {
        agent_brain_->initialize(world);
        return true;
    }
    
    // Processing
    void process_sensory_input(std::shared_ptr<SensoryPercept> percept) {
        agent_brain_->processSensoryInput(percept);
    }
    
    std::shared_ptr<Action> decode_motor_command() {
        return agent_brain_->decodeMotorCommand();
    }
    
    // Neuromodulation
    void apply_reward_modulation(float reward, float predicted_reward = 0.0f) {
        agent_brain_->applyRewardModulation(reward, predicted_reward);
    }
    
    void update_development(double timestep) {
        agent_brain_->updateDevelopment(timestep);
    }
    
    // Feature control
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
    
    // Status queries
    double get_developmental_stage() const {
        return static_cast<double>(agent_brain_->getDevelopmentalStage());
    }
    
    float get_neuromodulation_level() const {
        return agent_brain_->getNeuromodulationLevel();
    }
    
    float get_curiosity_level() const {
        return agent_brain_->getCuriosityLevel();
    }
    
    float get_novelty_level() const {
        return agent_brain_->getNoveltyLevel();
    }
    
    float get_prediction_error() const {
        return agent_brain_->getPredictionError();
    }
    
    py::dict get_agent_summary() const {
        py::dict result;
        result["developmental_stage"] = get_developmental_stage();
        result["neuromodulation_level"] = get_neuromodulation_level();
        result["curiosity_level"] = get_curiosity_level();
        result["novelty_level"] = get_novelty_level();
        result["prediction_error"] = get_prediction_error();
        return result;
    }
    
    // Internal access
    std::shared_ptr<AgentBrain> get_internal() const { return agent_brain_; }
    
private:
    std::shared_ptr<AgentBrain> agent_brain_;
};

// Enhanced simple world with Python-friendly APIs
class PythonSimpleWorld {
public:
    PythonSimpleWorld() : world_(std::make_shared<SimpleWorld>()) {}
    
    // Configuration
    void configure(int width, int height, int vision_width, int vision_height) {
        world_->configure(width, height, vision_width, vision_height);
    }
    
    void configure(int width, int height) {
        world_->configure(width, height, 8, 8);
    }
    
    // Setup
    void reset() {
        world_->reset();
    }
    
    void set_agent_start(float x, float y) {
        world_->setAgentStart(x, y);
    }
    
    // Simulation
    void update(float timestep) {
        world_->update(timestep);
    }
    
    // Action processing
    std::shared_ptr<ActionResult> apply_motor_command(std::shared_ptr<Action> action, float current_time) {
        return world_->applyMotorCommand(action, current_time);
    }
    
    // Sensory perception
    std::shared_ptr<SensoryPercept> get_sensory_percept() const {
        return world_->getSensoryPercept();
    }
    
    std::shared_ptr<AgentBody> get_agent_body() const {
        return world_->getAgentBody();
    }
    
    // World objects
    void add_world_object(std::shared_ptr<WorldObject> obj) {
        world_->addObject(obj);
    }
    
    void remove_world_object(float x, float y) {
        world_->removeObject(x, y);
    }
    
    bool is_valid_position(float x, float y) const {
        return world_->isValidPosition(x, y);
    }
    
    // World properties
    int get_width() const { return world_->getWidth(); }
    int get_height() const { return world_->getHeight(); }
    int get_max_energy() const { return world_->getMaxEnergy(); }
    void set_max_energy(int e) { world_->setMaxEnergy(e); }
    float get_energy_decay_rate() const { return world_->getEnergyDecayRate(); }
    void set_energy_decay_rate(float r) { world_->setEnergyDecayRate(r); }
    double get_simulation_time() const { return world_->getSimulationTime(); }
    
    // Vision utilities
    std::vector<std::vector<float>> get_vision_as_array() const {
        auto percept = get_sensory_percept();
        if (!percept) return {};
        
        auto vision = percept->getVision();
        if (!vision) return {};
        
        int width = vision->getWidth();
        int height = vision->getHeight();
        int channels = vision->getChannels();
        
        std::vector<std::vector<float>> result(height, std::vector<float>(width));
        
        auto data = vision->getData();
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int index = (y * width + x) * channels;
                if (index + 2 < data.size()) {
                    result[y][x] = data[index + 2]; // Use red channel as intensity
                }
            }
        }
        
        return result;
    }
    
    // Simulation utilities
    py::dict get_world_summary() const {
        py::dict result;
        result["width"] = get_width();
        result["height"] = get_height();
        result["vision_width"] = world_->getVisionWidth();
        result["vision_height"] = world_->getVisionHeight();
        result["max_energy"] = get_max_energy();
        result["energy_decay_rate"] = get_energy_decay_rate();
        result["simulation_time"] = get_simulation_time();
        return result;
    }
    
    // Internal access
    std::shared_ptr<SimpleWorld> get_internal() const { return world_; }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

// Development console with Python-friendly API
class PythonDevConsole {
public:
    PythonDevConsole() : console_(std::make_shared<DevConsole>()) {}
    
    // Initialize console
    void initialize(std::shared_ptr<Config> config,
                    std::shared_ptr<Brain> brain,
                    std::shared_ptr<DevelopmentSystem> developmentSystem) {
        console_->initialize(config, brain, developmentSystem);
    }
    
    // Process input (for non-interactive mode)
    bool process_input(const std::string& input) {
        return console_->processInput(input);
    }
    
    // Get command history
    std::vector<DevConsoleEntry> get_history() const {
        return console_->getHistory();
    }
    
    // Clear history
    void clear_history() {
        console_->clearHistory();
    }
    
    // Check if console is active
    bool is_active() const {
        return console_->isActive();
    }
    
    // Stop console
    void stop() {
        console_->stop();
    }
    
    // Internal access
    std::shared_ptr<DevConsole> get_internal() const { return console_; }
    
private:
    std::shared_ptr<DevConsole> console_;
};

// Simulation utilities
namespace simulation {
    
    // Run a simple simulation loop
    std::vector<py::dict> run_simulation(
        std::shared_ptr<PythonBrain> brain,
        std::shared_ptr<PythonSimpleWorld> world,
        std::shared_ptr<PythonAgent> agent,
        size_t num_steps,
        float timestep = 0.1f,
        bool verbose = false
    ) {
        std::vector<py::dict> results;
        
        for (size_t step = 0; step < num_steps; ++step) {
            // Update world
            world->update(timestep);
            
            // Get sensory input
            auto percept = world->get_sensory_percept();
            
            // Process sensory input
            agent->process_sensory_input(percept);
            
            // Brain processes
            brain->step(step, step * timestep);
            
            // Get action
            auto action = agent->decode_motor_command();
            
            // Apply action
            if (action) {
                world->apply_motor_command(action, world->get_simulation_time());
            }
            
            // Collect results
            py::dict step_result;
            step_result["step"] = step;
            step_result["time"] = world->get_simulation_time();
            step_result["neuron_count"] = brain->get_total_neuron_count();
            step_result["synapse_count"] = brain->get_total_synapse_count();
            step_result["firing_neuron_count"] = brain->get_firing_neuron_count();
            step_result["average_firing_rate"] = brain->get_average_firing_rate();
            step_result["action_type"] = static_cast<int>(action ? action->getType() : ActionType::Wait);
            
            results.push_back(step_result);
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << "/" << num_steps 
                         << " | Neurons: " << brain->get_total_neuron_count()
                         << " | Firing: " << brain->get_firing_neuron_count()
                         << " | Avg rate: " << brain->get_average_firing_rate()
                         << std::endl;
            }
        }
        
        return results;
    }
    
    // Run a learning simulation
    std::vector<py::dict> run_learning_simulation(
        std::shared_ptr<PythonBrain> brain,
        std::shared_ptr<PythonSimpleWorld> world,
        std::shared_ptr<PythonAgent> agent,
        size_t num_steps,
        float reward_rate = 0.01f,
        float timestep = 0.1f,
        bool verbose = false
    ) {
        std::vector<py::dict> results;
        float cumulative_reward = 0.0f;
        
        for (size_t step = 0; step < num_steps; ++step) {
            // Update world
            world->update(timestep);
            
            // Get sensory input
            auto percept = world->get_sensory_percept();
            
            // Process sensory input
            agent->process_sensory_input(percept);
            
            // Brain processes
            brain->step(step, step * timestep);
            
            // Get action
            auto action = agent->decode_motor_command();
            
            // Apply action and get reward
            auto action_result = world->apply_motor_command(action, world->get_simulation_time());
            
            if (action_result) {
                cumulative_reward += action_result->reward;
                
                // Apply reward modulation
                agent->apply_reward_modulation(action_result->reward, action_result->predicted_reward);
            }
            
            // Update development
            agent->update_development(timestep);
            
            // Collect results
            py::dict step_result;
            step_result["step"] = step;
            step_result["time"] = world->get_simulation_time();
            step_result["neuron_count"] = brain->get_total_neuron_count();
            step_result["synapse_count"] = brain->get_total_synapse_count();
            step_result["firing_neuron_count"] = brain->get_firing_neuron_count();
            step_result["average_firing_rate"] = brain->get_average_firing_rate();
            step_result["action_type"] = static_cast<int>(action ? action->getType() : ActionType::Wait);
            step_result["reward"] = action_result ? action_result->reward : 0.0f;
            step_result["cumulative_reward"] = cumulative_reward;
            step_result["development_stage"] = agent->get_developmental_stage();
            
            results.push_back(step_result);
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << "/" << num_steps 
                         << " | Reward: " << (action_result ? action_result->reward : 0.0f)
                         << " | Cumulative: " << cumulative_reward
                         << " | Stage: " << agent->get_developmental_stage()
                         << std::endl;
            }
        }
        
        return results;
    }
    
    // Run exploration simulation
    std::vector<py::dict> run_exploration_simulation(
        std::shared_ptr<PythonBrain> brain,
        std::shared_ptr<PythonSimpleWorld> world,
        std::shared_ptr<PythonAgent> agent,
        size_t num_steps,
        float timestep = 0.1f,
        bool enable_curiosity = true,
        bool verbose = false
    ) {
        std::vector<py::dict> results;
        float total_curiosity = 0.0f;
        
        if (enable_curiosity) {
            agent->enable_curiosity(true);
        }
        
        for (size_t step = 0; step < num_steps; ++step) {
            // Update world
            world->update(timestep);
            
            // Get sensory input
            auto percept = world->get_sensory_percept();
            
            // Process sensory input
            agent->process_sensory_input(percept);
            
            // Brain processes
            brain->step(step, step * timestep);
            
            // Get action
            auto action = agent->decode_motor_command();
            
            // Apply action and get reward
            auto action_result = world->apply_motor_command(action, world->get_simulation_time());
            
            if (action_result) {
                // Apply curiosity modulation if enabled
                if (enable_curiosity) {
                    agent->apply_reward_modulation(action_result->reward * 0.1f, 0.0f); // Small curiosity reward
                }
            }
            
            // Update development
            agent->update_development(timestep);
            
            // Collect results
            py::dict step_result;
            step_result["step"] = step;
            step_result["time"] = world->get_simulation_time();
            step_result["neuron_count"] = brain->get_total_neuron_count();
            step_result["synapse_count"] = brain->get_total_synapse_count();
            step_result["firing_neuron_count"] = brain->get_firing_neuron_count();
            step_result["average_firing_rate"] = brain->get_average_firing_rate();
            step_result["action_type"] = static_cast<int>(action ? action->getType() : ActionType::Wait);
            step_result["reward"] = action_result ? action_result->reward : 0.0f;
            step_result["curiosity_level"] = agent->get_curiosity_level();
            step_result["novelty_level"] = agent->get_novelty_level();
            step_result["prediction_error"] = agent->get_prediction_error();
            
            results.push_back(step_result);
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << "/" << num_steps 
                         << " | Curiosity: " << agent->get_curiosity_level()
                         << " | Novelty: " << agent->get_novelty_level()
                         << " | Prediction Error: " << agent->get_prediction_error()
                         << std::endl;
            }
        }
        
        return results;
    }
    
} // namespace simulation

} // namespace nlm

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings for Neural Learning Machine
        ---------------------------------------------------------
        
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        Provides advanced Pythonic APIs for brain simulation, configuration management,
        development console, and simulation utilities.
        
        Key Features:
        - PythonConfig: Enhanced configuration management with Python-friendly access
        - PythonBrain: Enhanced brain API with development controls and monitoring
        - PythonAgent: Enhanced agent API with feature controls and status queries
        - PythonSimpleWorld: Enhanced world API with simulation utilities
        - PythonDevConsole: Enhanced development console with Python integration
        - Simulation utilities for running complex simulations
        
        Usage Example:
            import pynlm_enhanced as nlm
            
            # Create brain with configuration
            config = nlm.PythonConfig({
                'neuron_count': 500,
                'connection_probability': 0.15,
                'simulation_timestep': 0.001
            })
            
            brain = nlm.PythonBrain(config)
            brain.initialize()
            
            # Create agent and world
            agent = nlm.PythonAgent(brain)
            world = nlm.PythonSimpleWorld()
            world.configure(20, 20)
            agent.initialize(world)
            
            # Run simulation
            results = nlm.simulation.run_learning_simulation(
                brain, world, agent, 1000, verbose=True
            )
    )pbdoc";
    
    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Register exception for configuration errors
    py::register_exception<std::invalid_argument>(m, "ValueError");
    
    // Register enhanced configuration
    py::class_<nlm::PythonConfig>(m, "PythonConfig", R"pbdoc(Python-friendly configuration manager)pbdoc")
        .def(py::init<>())
        .def(py::init<std::string>(), py::arg("filepath"))
        .def("load_from_file", &nlm::PythonConfig::load_from_file, py::arg("filepath"),
             "Load configuration from file")
        .def("load_from_dict", &nlm::PythonConfig::load_from_dict, py::arg("settings"),
             "Load configuration from dictionary")
        .def("get_int", &nlm::PythonConfig::get_int,
             py::arg("key"), py::arg("default_value") = 0,
             "Get integer value with default")
        .def("get_float", &nlm::PythonConfig::get_float,
             py::arg("key"), py::arg("default_value") = 0.0f,
             "Get float value with default")
        .def("get_bool", &nlm::PythonConfig::get_bool,
             py::arg("key"), py::arg("default_value") = false,
             "Get boolean value with default")
        .def("get_string", &nlm::PythonConfig::get_string,
             py::arg("key"), py::arg("default_value") = "",
             "Get string value with default")
        .def("get_vector", &nlm::PythonConfig::get_vector,
             py::arg("key"),
             "Get vector value")
        .def("set_int", &nlm::PythonConfig::set_int,
             py::arg("key"), py::arg("value"),
             "Set integer value")
        .def("set_float", &nlm::PythonConfig::set_float,
             py::arg("key"), py::arg("value"),
             "Set float value")
        .def("set_bool", &nlm::PythonConfig::set_bool,
             py::arg("key"), py::arg("value"),
             "Set boolean value")
        .def("set_string", &nlm::PythonConfig::set_string,
             py::arg("key"), py::arg("value"),
             "Set string value")
        .def("get_stats", &nlm::PythonConfig::get_stats,
             "Get configuration statistics")
        .def("validate", &nlm::PythonConfig::validate,
             "Validate configuration and return errors")
        .def("get_help", &nlm::PythonConfig::get_help,
             py::arg("key") = "",
             "Get help for configuration key")
        .def("summary", &nlm::PythonConfig::summary,
             "Get configuration summary")
        .def("__repr__", [](const nlm::PythonConfig& cfg) {
            return "<PythonConfig with " + std::to_string(cfg.get_internal()->getKeys().size()) + " entries>";
        });
    
    // Register enhanced brain
    py::class_<nlm::PythonBrain>(m, "PythonBrain", R"pbdoc(Python-friendly brain interface)pbdoc")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &nlm::PythonBrain::initialize,
             "Initialize the brain")
        .def("reset", &nlm::PythonBrain::reset,
             "Reset brain to initial state")
        .def("get_total_neuron_count", &nlm::PythonBrain::get_total_neuron_count,
             "Get total neuron count")
        .def("get_total_synapse_count", &nlm::PythonBrain::get_total_synapse_count,
             "Get total synapse count")
        .def("get_active_neuron_count", &nlm::PythonBrain::get_active_neuron_count,
             "Get active neuron count")
        .def("get_firing_neuron_count", &nlm::PythonBrain::get_firing_neuron_count,
             "Get firing neuron count")
        .def("get_average_firing_rate", &nlm::PythonBrain::get_average_firing_rate,
             "Get average firing rate")
        .def("get_excitation_inhibition_ratio", &nlm::PythonBrain::get_excitation_inhibition_ratio,
             "Get excitation/inhibition ratio")
        .def("get_total_spike_count", &nlm::PythonBrain::get_total_spike_count,
             "Get total spike count")
        .def("get_developmental_stage", &nlm::PythonBrain::get_developmental_stage,
             "Get developmental stage")
        .def("set_developmental_stage", &nlm::PythonBrain::set_developmental_stage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_developmental_stage_name", &nlm::PythonBrain::get_developmental_stage_name,
             "Get developmental stage name")
        .def("receive_sensory_input", &nlm::PythonBrain::receive_sensory_input,
             py::arg("input"),
             "Inject sensory input")
        .def("produce_action", &nlm::PythonBrain::produce_action,
             "Produce motor action")
        .def("step", &nlm::PythonBrain::step,
             py::arg("step_number"), py::arg("time") = 0.0,
             "Perform simulation step")
        .def("get_brain_summary", &nlm::PythonBrain::get_brain_summary,
             "Get brain summary as dictionary")
        .def("__repr__", [](const nlm::PythonBrain& brain) {
            return "<PythonBrain with " + std::to_string(brain.get_internal()->getTotalNeuronCount()) + " neurons>";
        });
    
    // Register enhanced agent
    py::class_<nlm::PythonAgent>(m, "PythonAgent", R"pbdoc(Python-friendly agent interface)pbdoc")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"),
             "Create agent with brain")
        .def("initialize", &nlm::PythonAgent::initialize,
             py::arg("world"),
             "Initialize with world")
        .def("process_sensory_input", &nlm::PythonAgent::process_sensory_input,
             py::arg("percept"),
             "Process sensory percept")
        .def("decode_motor_command", &nlm::PythonAgent::decode_motor_command,
             "Decode motor command from brain")
        .def("apply_reward_modulation", &nlm::PythonAgent::apply_reward_modulation,
             py::arg("reward"), py::arg("predicted_reward") = 0.0f,
             "Apply reward-based neuromodulation")
        .def("update_development", &nlm::PythonAgent::update_development,
             py::arg("timestep"),
             "Update development system")
        .def("enable_reward_modulation", &nlm::PythonAgent::enable_reward_modulation,
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &nlm::PythonAgent::enable_structural_plasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enable_development", &nlm::PythonAgent::enable_development,
             py::arg("enable"),
             "Enable/disable development")
        .def("enable_curiosity", &nlm::PythonAgent::enable_curiosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        .def("get_developmental_stage", &nlm::PythonAgent::get_developmental_stage,
             "Get developmental stage")
        .def("get_neuromodulation_level", &nlm::PythonAgent::get_neuromodulation_level,
             "Get neuromodulation level")
        .def("get_curiosity_level", &nlm::PythonAgent::get_curiosity_level,
             "Get curiosity level")
        .def("get_novelty_level", &nlm::PythonAgent::get_novelty_level,
             "Get novelty level")
        .def("get_prediction_error", &nlm::PythonAgent::get_prediction_error,
             "Get prediction error")
        .def("get_agent_summary", &nlm::PythonAgent::get_agent_summary,
             "Get agent summary as dictionary")
        .def("__repr__", [](const nlm::PythonAgent& agent) {
            return "<PythonAgent with brain>";
        });
    
    // Register enhanced simple world
    py::class_<nlm::PythonSimpleWorld>(m, "PythonSimpleWorld", R"pbdoc(Python-friendly simple world)pbdoc")
        .def(py::init<>())
        .def("configure", &nlm::PythonSimpleWorld::configure,
             py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions and vision")
        .def("configure", &nlm::PythonSimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             "Configure world dimensions with default vision")
        .def("reset", &nlm::PythonSimpleWorld::reset,
             "Reset world to initial state")
        .def("set_agent_start", &nlm::PythonSimpleWorld::set_agent_start,
             py::arg("x"), py::arg("y"),
             "Set agent start position")
        .def("update", &nlm::PythonSimpleWorld::update,
             py::arg("timestep"),
             "Update world by timestep")
        .def("apply_motor_command", &nlm::PythonSimpleWorld::apply_motor_command,
             py::arg("action"), py::arg("current_time"),
             "Apply motor command and get result")
        .def("get_sensory_percept", &nlm::PythonSimpleWorld::get_sensory_percept,
             "Get sensory percept from world")
        .def("get_agent_body", &nlm::PythonSimpleWorld::get_agent_body,
             "Get agent body state")
        .def("add_world_object", &nlm::PythonSimpleWorld::add_world_object,
             py::arg("obj"),
             "Add world object")
        .def("remove_world_object", &nlm::PythonSimpleWorld::remove_world_object,
             py::arg("x"), py::arg("y"),
             "Remove world object")
        .def("is_valid_position", &nlm::PythonSimpleWorld::is_valid_position,
             py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("get_width", &nlm::PythonSimpleWorld::get_width,
             "Get world width")
        .def("get_height", &nlm::PythonSimpleWorld::get_height,
             "Get world height")
        .def("get_vision_width", &nlm::PythonSimpleWorld::get_vision_width,
             "Get vision width")
        .def("get_vision_height", &nlm::PythonSimpleWorld::get_vision_height,
             "Get vision height")
        .def("get_max_energy", &nlm::PythonSimpleWorld::get_max_energy,
             "Get max energy")
        .def("set_max_energy", &nlm::PythonSimpleWorld::set_max_energy,
             py::arg("e"),
             "Set max energy")
        .def("get_energy_decay_rate", &nlm::PythonSimpleWorld::get_energy_decay_rate,
             "Get energy decay rate")
        .def("set_energy_decay_rate", &nlm::PythonSimpleWorld::set_energy_decay_rate,
             py::arg("r"),
             "Set energy decay rate")
        .def("get_simulation_time", &nlm::PythonSimpleWorld::get_simulation_time,
             "Get simulation time")
        .def("get_vision_as_array", &nlm::PythonSimpleWorld::get_vision_as_array,
             "Get vision as 2D array")
        .def("get_world_summary", &nlm::PythonSimpleWorld::get_world_summary,
             "Get world summary as dictionary")
        .def("__repr__", [](const nlm::PythonSimpleWorld& world) {
            return "<PythonSimpleWorld " + std::to_string(world.get_internal()->getWidth()) + 
                   "x" + std::to_string(world.get_internal()->getHeight()) + ">";
        });
    
    // Register enhanced development console
    py::class_<nlm::PythonDevConsole>(m, "PythonDevConsole", R"pbdoc(Python-friendly development console)pbdoc")
        .def(py::init<>())
        .def("initialize", &nlm::PythonDevConsole::initialize,
             py::arg("config"), py::arg("brain"), py::arg("development_system"),
             "Initialize console with brain and config")
        .def("process_input", &nlm::PythonDevConsole::process_input,
             py::arg("input"),
             "Process console input and return success")
        .def("get_history", &nlm::PythonDevConsole::get_history,
             "Get command history")
        .def("clear_history", &nlm::PythonDevConsole::clear_history,
             "Clear command history")
        .def("is_active", &nlm::PythonDevConsole::is_active,
             "Check if console is active")
        .def("stop", &nlm::PythonDevConsole::stop,
             "Stop the development console")
        .def("__repr__", [](const nlm::PythonDevConsole& console) {
            return "<PythonDevConsole>";
        });
    
    // Register simulation utilities
    py::module m_simulation = m.def_submodule("simulation", R"pbdoc(Simulation utilities)pbdoc");
    
    m_simulation.def("run_simulation", &nlm::simulation::run_simulation,
                     py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
                     py::arg("timestep") = 0.1f, py::arg("verbose") = false,
                     "Run a simple simulation loop");
    
    m_simulation.def("run_learning_simulation", &nlm::simulation::run_learning_simulation,
                     py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
                     py::arg("reward_rate") = 0.01f, py::arg("timestep") = 0.1f, py::arg("verbose") = false,
                     "Run a learning simulation with reward modulation");
    
    m_simulation.def("run_exploration_simulation", &nlm::simulation::run_exploration_simulation,
                     py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
                     py::arg("timestep") = 0.1f, py::arg("enable_curiosity") = true, py::arg("verbose") = false,
                     "Run an exploration simulation with curiosity");
    
    // Register convenience functions
    m.def("create_default_config", []() {
        auto config = std::make_shared<nlm::Config>();
        return std::make_shared<nlm::PythonConfig>(
            nlm::PythonConfig(config->getKeys().empty() ? "" : "configs/default.cfg")
        );
    }, "Create default configuration");
    
    m.def("create_brain", [](std::shared_ptr<nlm::Config> config) {
        auto brain = std::make_shared<nlm::PythonBrain>(config);
        return brain;
    }, py::arg("config"), "Create brain with configuration");
    
    m.def("create_simple_world", []() {
        return std::make_shared<nlm::PythonSimpleWorld>();
    }, "Create simple world");
    
    m.def("create_agent", [](std::shared_ptr<nlm::Brain> brain) {
        auto agent = std::make_shared<nlm::PythonAgent>(brain);
        return agent;
    }, py::arg("brain"), "Create agent with brain");
    
    m.def("create_dev_console", []() {
        return std::make_shared<nlm::PythonDevConsole>();
    }, "Create development console");
    
    // Add documentation
    m.attr("__version__") = "1.0.0-enhanced";
    m.attr("__description__") = "Enhanced Python bindings for NLM Neural Learning Machine";
    
    // Add examples to module documentation
    m.doc() += R"pbdoc(


    Examples:
    ---------
    >>> import pynlm_enhanced as nlm
    >>>
    >>> # Create a simple brain
    >>> brain = nlm.create_brain()
    >>> brain.initialize()
    >>> 
    >>> # Create world and agent
    >>> world = nlm.create_simple_world()
    >>> world.configure(10, 10)
    >>> agent = nlm.create_agent(brain)
    >>> agent.initialize(world)
    >>> 
    >>> # Run simulation
    >>> results = nlm.simulation.run_simulation(brain, world, agent, 100)
    >>> for step in results:
    ...     print(f"Step {step['step']}: {step['firing_neuron_count']} firing neurons")
    >>> 
    >>> # Use development console
    >>> console = nlm.create_dev_console()
    >>> console.initialize(config, brain, development_system)
    >>> # console.run()  # Run interactively
    >>> 
    >>> # Configure with dictionary
    >>> config = nlm.PythonConfig({'neuron_count': 500, 'connection_probability': 0.15})
    >>> brain = nlm.PythonBrain(config)
    >>> brain.initialize()
    "pbdoc";
} // namespace nlm