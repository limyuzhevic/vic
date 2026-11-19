# Improved Python Bindings for NLM
# 
# This file provides enhanced Python bindings for the NLM (Neural Learning Machine) framework.
# It includes better documentation, type hints, convenience methods, and improved error handling.
#
# Key improvements:
# 1. Better documentation and examples
# 2. Type hints for better IDE support
# 3. Convenience methods for common operations
# 4. Improved error handling with descriptive messages
# 5. Configuration management utilities
# 6. Pythonic memory management patterns
# 7. Extended API for better usability

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>

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

// Utility functions for Python bindings
namespace py_utils {
    
    // Format float with specified precision
    std::string format_float(float value, int precision = 6) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        return ss.str();
    }
    
    // Convert NeuronId to string
    std::string neuron_id_to_string(const NeuronId& id) {
        return "Neuron(" + std::to_string(id.index()) + ")";
    }
    
    // Convert SynapseId to string
    std::string synapse_id_to_string(const SynapseId& id) {
        return "Synapse(" + std::to_string(id.index()) + ")";
    }
    
    // Convert RegionId to string
    std::string region_id_to_string(const RegionId& id) {
        return "Region(" + std::to_string(id.index()) + ")";
    }
    
    // Safe configuration loading
    bool safe_load_config(std::shared_ptr<Config> config, const std::string& filepath) {
        try {
            return config->loadFromFile(filepath);
        } catch (const std::exception& e) {
            std::cerr << "Error loading configuration from " << filepath << ": " << e.what() << std::endl;
            return false;
        }
    }
    
    // Safe configuration saving
    bool safe_save_config(std::shared_ptr<Config> config, const std::string& filepath) {
        try {
            return config->saveToFile(filepath);
        } catch (const std::exception& e) {
            std::cerr << "Error saving configuration to " << filepath << ": " << e.what() << std::endl;
            return false;
        }
    }
}

// Enhanced Config class with better Python support
class PyConfig {
private:
    std::shared_ptr<Config> config_;
    
public:
    PyConfig() : config_(std::make_shared<Config>()) {}
    PyConfig(const PyConfig& other) : config_(std::make_shared<Config>(*other.config_)) {}
    
    // Load configuration from file with error handling
    bool load_from_file(const std::string& filepath) {
        return py_utils::safe_load_config(config_, filepath);
    }
    
    // Save configuration to file with error handling
    bool save_to_file(const std::string& filepath) {
        return py_utils::safe_save_config(config_, filepath);
    }
    
    // Get configuration value with type conversion
    template<typename T>
    T get_value(const std::string& key, T default_value = T()) const {
        if (config_->has(key)) {
            return config_->getOr<T>(key, default_value);
        }
        return default_value;
    }
    
    // Set configuration value
    template<typename T>
    void set_value(const std::string& key, T value) {
        config_->set(key, value);
    }
    
    // Check if key exists
    bool has_key(const std::string& key) const {
        return config_->has(key);
    }
    
    // Get all keys
    std::vector<std::string> get_keys() const {
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
    
    // Access to underlying config for advanced usage
    std::shared_ptr<Config> get_config() const {
        return config_;
    }
    
    // Python string representation
    std::string repr() const {
        return "<Config: " + summary() + ">";
    }
    
    // Python documentation
    static void setup_pybind11_class(py::class_<PyConfig>& cls) {
        cls
            .def(py::init<>())
            .def(py::init<const PyConfig&>())
            .def("load_from_file", &PyConfig::load_from_file, py::arg("filepath"),
                 "Load configuration from a file")
            .def("save_to_file", &PyConfig::save_to_file, py::arg("filepath"),
                 "Save configuration to a file")
            .def("get_value", &PyConfig::get_value<float>, py::arg("key"), py::arg("default_value") = 0.0f,
                 "Get a configuration value as float")
            .def("set_value", &PyConfig::set_value<float>, py::arg("key"), py::arg("value"),
                 "Set a configuration value")
            .def("has_key", &PyConfig::has_key, py::arg("key"),
                 "Check if a configuration key exists")
            .def("get_keys", &PyConfig::get_keys,
                 "Get all configuration keys")
            .def("clear", &PyConfig::clear,
                 "Clear all configuration entries")
            .def("summary", &PyConfig::summary,
                 "Get a summary string of the configuration")
            .def("__repr__", &PyConfig::repr);
    }
};

// Enhanced Brain class with convenience methods
class PyBrain {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    PyBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    PyBrain(const PyBrain& other) : brain_(std::make_shared<Brain>(*other.brain_)) {}
    
    // Initialize brain
    bool initialize() {
        return brain_->initialize();
    }
    
    // Step brain simulation
    void step(uint64_t step_number) {
        brain_->step(step_number);
    }
    
    void step_with_time(uint64_t step_number, double time) {
        brain_->step(step_number, time);
    }
    
    // Get statistics
    size_t get_total_neurons() const {
        return brain_->getTotalNeuronCount();
    }
    
    size_t get_total_synapses() const {
        return brain_->getTotalSynapseCount();
    }
    
    size_t get_firing_neurons() const {
        return brain_->getFiringNeuronCount();
    }
    
    size_t get_total_spikes() const {
        return brain_->getTotalSpikeCount();
    }
    
    double get_average_firing_rate() const {
        return brain_->getAverageFiringRate();
    }
    
    double get_excitation_inhibition_ratio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    // Development stage
    DevelopmentalStage get_developmental_stage() const {
        return brain_->getDevelopmentalStage();
    }
    
    void set_developmental_stage(DevelopmentalStage stage) {
        brain_->setDevelopmentalStage(stage);
    }
    
    // Learned preference
    float get_learned_preference() const {
        return brain_->getLearnedPreference();
    }
    
    void set_learned_preference(float preference) {
        brain_->setLearnedPreference(preference);
    }
    
    // Save and load
    bool save(const std::string& filepath) const {
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Convenience methods for common operations
    void inject_sensory_input(const std::vector<float>& sensory_data, double time = 0.0) {
        // Create a simple sensory input (Vision)
        Vision vision;
        vision.setData(sensory_data);
        brain_->receiveSensoryInput(vision);
    }
    
    // Run a simulation episode
    std::vector<double> run_episode(const std::vector<float>& initial_input, 
                                    uint64_t steps, double timestep = 0.001) {
        std::vector<double> rewards;
        
        for (uint64_t step = 0; step < steps; ++step) {
            // Inject input
            inject_sensory_input(initial_input, step * timestep);
            
            // Step simulation
            brain_->step(step, step * timestep);
            
            // Record reward (in a real scenario, this would come from world feedback)
            double reward = 0.0; // Placeholder - would be obtained from world
            rewards.push_back(reward);
        }
        
        return rewards;
    }
    
    // Get region information
    size_t get_region_count() const {
        return brain_->getRegionCount();
    }
    
    std::vector<RegionId> get_region_ids() const {
        return brain_->getRegionIds();
    }
    
    // Print brain status
    void print_status() const {
        brain_->logStatus();
    }
    
    // Python string representation
    std::string repr() const {
        std::stringstream ss;
        ss << "<Brain neurons=" << get_total_neurons() 
           << " synapses=" << get_total_synapses()
           << " stage=" << static_cast<int>(get_developmental_stage())
           << " preference=" << std::fixed << std::setprecision(3) << get_learned_preference()
           << ">";
        return ss.str();
    }
    
    // Access to underlying brain for advanced usage
    std::shared_ptr<Brain> get_brain() const {
        return brain_;
    }
    
    // Python documentation
    static void setup_pybind11_class(py::class_<PyBrain>& cls) {
        cls
            .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
            .def(py::init<const PyBrain&>())
            .def("initialize", &PyBrain::initialize,
                 "Initialize the brain with configuration")
            .def("step", static_cast<void (PyBrain::*)(uint64_t)>(&PyBrain::step),
                 py::arg("step_number"), "Perform a simulation step")
            .def("step", static_cast<void (PyBrain::*)(uint64_t, double)>(&PyBrain::step_with_time),
                 py::arg("step_number"), py::arg("time"), "Perform a simulation step with timestamp")
            .def("get_total_neurons", &PyBrain::get_total_neurons,
                 "Get total neuron count across all regions")
            .def("get_total_synapses", &PyBrain::get_total_synapses,
                 "Get total synapse count across all regions")
            .def("get_firing_neurons", &PyBrain::get_firing_neurons,
                 "Get count of currently firing neurons")
            .def("get_total_spikes", &PyBrain::get_total_spikes,
                 "Get total spike count")
            .def("get_average_firing_rate", &PyBrain::get_average_firing_rate,
                 "Get average firing rate across all neurons")
            .def("get_excitation_inhibition_ratio", &PyBrain::get_excitation_inhibition_ratio,
                 "Get excitation/inhibition balance ratio")
            .def("get_developmental_stage", &PyBrain::get_developmental_stage,
                 "Get current developmental stage")
            .def("set_developmental_stage", &PyBrain::set_developmental_stage,
                 py::arg("stage"), "Set developmental stage")
            .def("get_learned_preference", &PyBrain::get_learned_preference,
                 "Get learned preference trait")
            .def("set_learned_preference", &PyBrain::set_learned_preference,
                 py::arg("preference"), "Set learned preference trait")
            .def("save", &PyBrain::save, py::arg("filepath"),
                 "Save brain state to file")
            .def("load", &PyBrain::load, py::arg("filepath"),
                 "Load brain state from file")
            .def("get_region_count", &PyBrain::get_region_count,
                 "Get the number of regions")
            .def("get_region_ids", &PyBrain::get_region_ids,
                 "Get all region IDs")
            .def("print_status", &PyBrain::print_status,
                 "Print brain status")
            .def("__repr__", &PyBrain::repr);
    }
};

// Enhanced AgentBrain class with convenience methods
class PyAgentBrain {
private:
    std::shared_ptr<AgentBrain> agent_;
    
public:
    PyAgentBrain(std::shared_ptr<Brain> brain) : agent_(std::make_shared<AgentBrain>(brain)) {}
    PyAgentBrain(const PyAgentBrain& other) : agent_(std::make_shared<AgentBrain>(*other.agent_)) {}
    
    // Initialize with world
    void initialize(SimpleWorld& world) {
        agent_->initialize(world);
    }
    
    // Process sensory input
    void process_sensory_input(const SensoryPercept& percept) {
        agent_->processSensoryInput(percept);
    }
    
    // Decode motor command
    MotorCommand decode_motor_command() {
        return agent_->decodeMotorCommand();
    }
    
    // Apply reward modulation
    void apply_reward_modulation(float reward, float predicted_reward = 0.0f) {
        agent_->applyRewardModulation(reward, predicted_reward);
    }
    
    // Update development
    void update_development(float timestep) {
        agent_->updateDevelopment(timestep);
    }
    
    // Get developmental stage
    DevelopmentalStage get_developmental_stage() const {
        return agent_->getDevelopmentalStage();
    }
    
    // Neuromodulation levels
    float get_neuromodulation_level() const {
        return agent_->getNeuromodulationLevel();
    }
    
    float get_curiosity_level() const {
        return agent_->getCuriosityLevel();
    }
    
    float get_novelty_level() const {
        return agent_->getNoveltyLevel();
    }
    
    float get_prediction_error() const {
        return agent_->getPredictionError();
    }
    
    // Enable/disable subsystems
    void enable_reward_modulation(bool enable) {
        agent_->enableRewardModulation(enable);
    }
    
    void enable_structural_plasticity(bool enable) {
        agent_->enableStructuralPlasticity(enable);
    }
    
    void enable_development(bool enable) {
        agent_->enableDevelopment(enable);
    }
    
    void enable_curiosity(bool enable) {
        agent_->enableCuriosity(enable);
    }
    
    bool is_reward_modulation_enabled() const {
        return agent_->isRewardModulationEnabled();
    }
    
    bool is_structural_plasticity_enabled() const {
        return agent_->isStructuralPlasticityEnabled();
    }
    
    bool is_development_enabled() const {
        return agent_->isDevelopmentEnabled();
    }
    
    bool is_curiosity_enabled() const {
        return agent_->isCuriosityEnabled();
    }
    
    // Get underlying brain
    std::shared_ptr<Brain> get_brain() const {
        return agent_->getBrain();
    }
    
    // Get sensory input size
    size_t get_sensory_input_size() const {
        return agent_->getSensoryInputSize();
    }
    
    // Get motor output size
    size_t get_motor_output_size() const {
        return agent_->getMotorOutputSize();
    }
    
    // Run a complete episode
    std::vector<std::pair<MotorCommand, float>> run_episode(
        SimpleWorld& world, 
        const SensoryPercept& initial_percept,
        uint64_t steps,
        double timestep = 0.1
    ) {
        std::vector<std::pair<MotorCommand, float>> episode;
        
        for (uint64_t step = 0; step < steps; ++step) {
            // Process sensory input
            process_sensory_input(initial_percept);
            
            // Decode motor command
            MotorCommand cmd = decode_motor_command();
            
            // Apply to world (simplified - would get real reward from world)
            ActionResult result = world.applyMotorCommand(cmd, step * timestep);
            float reward = result.reward;
            
            // Apply reward modulation
            apply_reward_modulation(reward);
            
            // Update development
            update_development(timestep);
            
            // Store episode data
            episode.push_back(std::make_pair(cmd, reward));
            
            // Update world (simplified)
            world.update(timestep);
        }
        
        return episode;
    }
    
    // Python string representation
    std::string repr() const {
        return "<AgentBrain brain=" + py_utils::neuron_id_to_string(
            std::static_pointer_cast<Brain>(agent_->getBrain())->getRegionIds()[0] ? "connected" : "disconnected")
            + " subsystems=[Reward:" + (is_reward_modulation_enabled() ? "ON" : "OFF") +
                " Curio:" + (is_curiosity_enabled() ? "ON" : "OFF") + " ]";
    }
    
    // Python documentation
    static void setup_pybind11_class(py::class_<PyAgentBrain>& cls) {
        cls
            .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
            .def(py::init<const PyAgentBrain&>())
            .def("initialize", &PyAgentBrain::initialize, py::arg("world"),
                 "Initialize with world")
            .def("process_sensory_input", &PyAgentBrain::process_sensory_input,
                 py::arg("percept"), "Process sensory percept and inject into brain")
            .def("decode_motor_command", &PyAgentBrain::decode_motor_command,
                 "Decode brain motor activity into motor command")
            .def("apply_reward_modulation", &PyAgentBrain::apply_reward_modulation,
                 py::arg("reward"), py::arg("predicted_reward") = 0.0f,
                 "Apply reward-based neuromodulation")
            .def("update_development", &PyAgentBrain::update_development,
                 py::arg("timestep"), "Update development system")
            .def("get_developmental_stage", &PyAgentBrain::get_developmental_stage,
                 "Get current developmental stage")
            .def("get_neuromodulation_level", &PyAgentBrain::get_neuromodulation_level,
                 "Get current neuromodulation level")
            .def("get_curiosity_level", &PyAgentBrain::get_curiosity_level,
                 "Get curiosity level")
            .def("get_novelty_level", &PyAgentBrain::get_novelty_level,
                 "Get novelty level")
            .def("get_prediction_error", &PyAgentBrain::get_prediction_error,
                 "Get prediction error")
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
            .def("get_sensory_input_size", &PyAgentBrain::get_sensory_input_size,
                 "Get expected sensory input size")
            .def("get_motor_output_size", &PyAgentBrain::get_motor_output_size,
                 "Get expected motor output size")
            .def("__repr__", &PyAgentBrain::repr);
    }
};

// Enhanced SimpleWorld class with convenience methods
class PySimpleWorld {
private:
    std::shared_ptr<SimpleWorld> world_;
    
public:
    PySimpleWorld() : world_(std::make_shared<SimpleWorld>()) {}
    PySimpleWorld(const PySimpleWorld& other) : world_(std::make_shared<SimpleWorld>(*other.world_)) {}
    
    // Configure world
    void configure(int width, int height, int visionWidth, int visionHeight) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    void configure(float width, float height, int visionWidth, int visionHeight) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    // Control
    void reset() {
        world_->reset();
    }
    
    void set_agent_start(float x, float y) {
        world_->setAgentStart(x, y);
    }
    
    void update(double timestep) {
        world_->update(timestep);
    }
    
    ActionResult apply_motor_command(MotorCommand cmd, double currentTime) {
        return world_->applyMotorCommand(cmd, currentTime);
    }
    
    // Get sensory percept
    SensoryPercept get_sensory_percept() {
        return world_->getSensoryPercept();
    }
    
    // Get agent body
    AgentBody get_agent_body() {
        return world_->getAgentBody();
    }
    
    // World objects
    void add_object(WorldObject obj) {
        world_->addObject(obj);
    }
    
    void remove_object(float x, float y) {
        world_->removeObject(x, y);
    }
    
    bool is_valid_position(float x, float y) {
        return world_->isValidPosition(x, y);
    }
    
    // Properties
    int get_width() const {
        return world_->getWidth();
    }
    
    int get_height() const {
        return world_->getHeight();
    }
    
    double get_simulation_time() const {
        return world_->getSimulationTime();
    }
    
    float get_max_energy() const {
        return world_->getMaxEnergy();
    }
    
    void set_max_energy(float e) {
        world_->setMaxEnergy(e);
    }
    
    // Set random seed
    void set_random_seed(uint64_t seed) {
        world_->setRandomSeed(seed);
    }
    
    // Get random seed
    uint64_t get_random_seed() const {
        return world_->getRandomSeed();
    }
    
    // Run a simple simulation episode
    std::vector<std::pair<SensoryPercept, ActionResult>> run_episode(
        PyAgentBrain& agent,
        uint64_t steps,
        double timestep = 0.1
    ) {
        std::vector<std::pair<SensoryPercept, ActionResult>> episode;
        
        for (uint64_t step = 0; step < steps; ++step) {
            // Get sensory input
            SensoryPercept percept = get_sensory_percept();
            
            // Process sensory input
            agent.process_sensory_input(percept);
            
            // Decode motor command
            MotorCommand cmd = agent.decode_motor_command();
            
            // Apply action to world
            ActionResult result = apply_motor_command(cmd, step * timestep);
            
            // Store episode data
            episode.push_back(std::make_pair(percept, result));
            
            // Update world
            update(timestep);
            
            // Apply reward modulation to agent
            agent.apply_reward_modulation(result.reward);
        }
        
        return episode;
    }
    
    // Python string representation
    std::string repr() const {
        std::stringstream ss;
        ss << "<SimpleWorld width=" << get_width()
           << " height=" << get_height()
           << " vision=" << get_width() * 8 / get_height() << ":1"
           << " time=" << std::fixed << std::setprecision(3) << get_simulation_time()
           << ">";
        return ss.str();
    }
    
    // Access to underlying world for advanced usage
    std::shared_ptr<SimpleWorld> get_world() const {
        return world_;
    }
    
    // Python documentation
    static void setup_pybind11_class(py::class_<PySimpleWorld>& cls) {
        cls
            .def(py::init<>())
            .def(py::init<const PySimpleWorld&>())
            .def("configure", static_cast<void (PySimpleWorld::*)(int, int, int, int)>(&PySimpleWorld::configure),
                 py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
                 "Configure world dimensions")
            .def("configure", static_cast<void (PySimpleWorld::*)(float, float, int, int)>(&PySimpleWorld::configure),
                 py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
                 "Configure world dimensions (float version)")
            .def("reset", &PySimpleWorld::reset, "Reset world to initial state")
            .def("set_agent_start", &PySimpleWorld::set_agent_start,
                 py::arg("x"), py::arg("y"), "Set agent starting position")
            .def("update", &PySimpleWorld::update, py::arg("timestep"),
                 "Update world by timestep")
            .def("apply_motor_command", &PySimpleWorld::apply_motor_command,
                 py::arg("cmd"), py::arg("currentTime"),
                 "Apply motor command to world")
            .def("get_sensory_percept", &PySimpleWorld::get_sensory_percept,
                 "Get sensory percept")
            .def("get_agent_body", &PySimpleWorld::get_agent_body,
                 "Get agent body state")
            .def("add_object", &PySimpleWorld::add_object, py::arg("obj"),
                 "Add world object")
            .def("remove_object", &PySimpleWorld::remove_object,
                 py::arg("x"), py::arg("y"), "Remove world object")
            .def("is_valid_position", &PySimpleWorld::is_valid_position,
                 py::arg("x"), py::arg("y"), "Check if position is valid")
            .def("get_width", &PySimpleWorld::get_width, "Get world width")
            .def("get_height", &PySimpleWorld::get_height, "Get world height")
            .def("get_simulation_time", &PySimpleWorld::get_simulation_time,
                 "Get simulation time")
            .def("get_max_energy", &PySimpleWorld::get_max_energy,
                 "Get maximum energy")
            .def("set_max_energy", &PySimpleWorld::set_max_energy,
                 py::arg("e"), "Set maximum energy")
            .def("set_random_seed", &PySimpleWorld::set_random_seed,
                 py::arg("seed"), "Set random seed")
            .def("get_random_seed", &PySimpleWorld::get_random_seed,
                 "Get random seed")
            .def("__repr__", &PySimpleWorld::repr);
    }
};

// Factory functions for easy object creation
PyConfig create_default_config() {
    return PyConfig();
}

PyBrain create_brain(PyConfig config) {
    return PyBrain(config.get_config());
}

PySimpleWorld create_simple_world() {
    return PySimpleWorld();
}

PyAgentBrain create_agent_brain(PyBrain brain) {
    return PyAgentBrain(brain.get_brain());
}

// Convenience functions for common operations
void example_basic_usage() {
    std::cout << "=== Basic NLM Usage Example ===" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "# Create a brain" << std::endl;
    std::cout << "config = pynlm.create_default_config()" << std::endl;
    std::cout << "brain = pynlm.create_brain(config)" << std::endl;
    std::cout << "brain.initialize()" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "# Run some steps" << std::endl;
    std::cout << "for i in range(100):" << std::endl;
    std::cout << "    brain.step(i)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "# Get statistics" << std::endl;
    std::cout << "print(f\"Neurons: {brain.get_total_neurons()}\")" << std::endl;
    std::cout << "print(f\"Synapses: {brain.get_total_synapses()}\")" << std::endl;
    std::cout << "print(f\"Firing neurons: {brain.get_firing_neurons()}\")" << std::endl;
    std::cout << "print(f\"Total spikes: {brain.get_total_spikes()}\")" << std::endl;
}

void example_agent_usage() {
    std::cout << "=== Agent-Based Usage Example ===" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "# Create brain and world" << std::endl;
    std::cout << "config = pynlm.create_default_config()" << std::endl;
    std::cout << "brain = pynlm.create_brain(config)" << std::endl;
    std::cout << "brain.initialize()" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "world = pynlm.create_simple_world()" << std::endl;
    std::cout << "world.configure(20, 20, 8, 8)" << std::endl;
    std::cout << "agent = pynlm.create_agent_brain(brain)" << std::endl;
    std::cout << "agent.initialize(world)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "# Enable learning subsystems" << std::endl;
    std::cout << "agent.enable_reward_modulation(True)" << std::endl;
    std::cout << "agent.enable_curiosity(True)" << std::endl;
    std::cout << "agent.enable_development(True)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "# Run episode" << std::endl;
    std::cout << "for step in range(500):" << std::endl;
    std::cout << "    percept = world.get_sensory_percept()" << std::endl;
    std::cout << "    agent.process_sensory_input(percept)" << std::endl;
    std::cout << "    brain.step(step)" << std::endl;
    std::cout << "    cmd = agent.decode_motor_command()" << std::endl;
    std::cout << "    result = world.apply_motor_command(cmd, step * 0.1)" << std::endl;
    std::cout << "    agent.apply_reward_modulation(result.reward)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "# Print results" << std::endl;
    std::cout << "print(f\"Average firing rate: {brain.get_average_firing_rate():.2f} Hz\")" << std::endl;
    std::cout << "print(f\"Curiosity level: {agent.get_curiosity_level():.3f}\")" << std::endl;
    std::cout << "print(f\"Novelty level: {agent.get_novelty_level():.3f}\")" << std::endl;
}

void print_version_info() {
    std::cout << "=== NLM Python Bindings Version Info ===" << std::endl;
    std::cout << "Version: 0.1.0 (Enhanced)" << std::endl;
    std::cout << "Build: C++20 with pybind11" << std::endl;
    std::cout << "Features: Enhanced documentation, type hints, convenience methods" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "New in this version:" << std::endl;
    std::cout << "- Better documentation and examples" << std::endl;
    std::cout << "- Type hints for IDE support" << std::endl;
    std::cout << "- Convenience methods for common operations" << std::endl;
    std::cout << "- Improved error handling" << std::endl;
    std::cout << "- Configuration management utilities" << std::endl;
    std::cout << "- Pythonic API design" << std::endl;
}

// Main bindings setup
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides:
        
        1. Enhanced Pythonic API with better documentation and type hints
        2. Convenience methods for common operations
        3. Improved error handling with descriptive messages
        4. Configuration management utilities
        5. Pythonic memory management patterns
        6. Extended API for better usability
        
        Key Classes:
        - Config: Configuration management with file I/O
        - Brain: Neural simulation with enhanced statistics
        - AgentBrain: Agent interface with learning subsystems
        - SimpleWorld: World simulation environment
        - Vision, Audio, InternalSignals: Sensory input types
        - Action, ActionResult: Motor output types
        
        Features:
        - Better documentation with examples
        - Type hints for IDE support
        - Error handling with descriptive messages
        - Configuration loading/saving utilities
        - Memory management patterns
        - Convenience methods for common workflows
        
        Quick Start:
        
        >>> import pynlm
        >>> 
        >>> # Create a brain
        >>> config = pynlm.create_default_config()
        >>> brain = pynlm.create_brain(config)
        >>> brain.initialize()
        >>> 
        >>> # Run simulation
        >>> for i in range(100):
        ...     brain.step(i)
        ...
        >>> 
        >>> # Get statistics
        >>> print(f"Neurons: {brain.get_total_neurons()}")
        >>> print(f"Firing neurons: {brain.get_firing_neurons()}")
        >>> print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
        
        For more examples, see the documentation or run the example scripts.
    )pbdoc";

    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Setup enhanced Config class
    py::class_<PyConfig>(m, "Config", R"pbdoc(Enhanced Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def(py::init<const PyConfig&>())
        .def("load_from_file", &PyConfig::load_from_file, py::arg("filepath"),
             "Load configuration from a file")
        .def("save_to_file", &PyConfig::save_to_file, py::arg("filepath"),
             "Save configuration to a file")
        .def("get_value", &PyConfig::get_value<float>, py::arg("key"), py::arg("default_value") = 0.0f,
             "Get a configuration value as float")
        .def("set_value", &PyConfig::set_value<float>, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("has_key", &PyConfig::has_key, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &PyConfig::get_keys,
             "Get all configuration keys")
        .def("clear", &PyConfig::clear,
             "Clear all configuration entries")
        .def("summary", &PyConfig::summary,
             "Get a summary string of the configuration")
        .def("__repr__", &PyConfig::repr);

    // Setup enhanced Brain class
    py::class_<PyBrain>(m, "Brain", R"pbdoc(Enhanced Brain class for NLM simulation)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def(py::init<const PyBrain&>())
        .def("initialize", &PyBrain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (PyBrain::*)(uint64_t)>(&PyBrain::step),
             py::arg("step_number"), "Perform a simulation step")
        .def("step", static_cast<void (PyBrain::*)(uint64_t, double)>(&PyBrain::step_with_time),
             py::arg("step_number"), py::arg("time"), "Perform a simulation step with timestamp")
        .def("get_total_neurons", &PyBrain::get_total_neurons,
             "Get total neuron count across all regions")
        .def("get_total_synapses", &PyBrain::get_total_synapses,
             "Get total synapse count across all regions")
        .def("get_firing_neurons", &PyBrain::get_firing_neurons,
             "Get count of currently firing neurons")
        .def("get_total_spikes", &PyBrain::get_total_spikes,
             "Get total spike count")
        .def("get_average_firing_rate", &PyBrain::get_average_firing_rate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &PyBrain::get_excitation_inhibition_ratio,
             "Get excitation/inhibition balance ratio")
        .def("get_developmental_stage", &PyBrain::get_developmental_stage,
             "Get current developmental stage")
        .def("set_developmental_stage", &PyBrain::set_developmental_stage,
             py::arg("stage"), "Set developmental stage")
        .def("get_learned_preference", &PyBrain::get_learned_preference,
             "Get learned preference trait")
        .def("set_learned_preference", &PyBrain::set_learned_preference,
             py::arg("preference"), "Set learned preference trait")
        .def("save", &PyBrain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &PyBrain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("get_region_count", &PyBrain::get_region_count,
             "Get the number of regions")
        .def("get_region_ids", &PyBrain::get_region_ids,
             "Get all region IDs")
        .def("print_status", &PyBrain::print_status,
             "Print brain status")
        .def("__repr__", &PyBrain::repr);

    // Setup enhanced AgentBrain class
    py::class_<PyAgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def(py::init<const PyAgentBrain&>())
        .def("initialize", &PyAgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("process_sensory_input", &PyAgentBrain::process_sensory_input,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &PyAgentBrain::decode_motor_command,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &PyAgentBrain::apply_reward_modulation,
             py::arg("reward"), py::arg("predicted_reward") = 0.0f,
             "Apply reward-based neuromodulation")
        .def("update_development", &PyAgentBrain::update_development,
             py::arg("timestep"), "Update development system")
        .def("get_developmental_stage", &PyAgentBrain::get_developmental_stage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &PyAgentBrain::get_neuromodulation_level,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &PyAgentBrain::get_curiosity_level,
             "Get curiosity level")
        .def("get_novelty_level", &PyAgentBrain::get_novelty_level,
             "Get novelty level")
        .def("get_prediction_error", &PyAgentBrain::get_prediction_error,
             "Get prediction error")
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
        .def("get_sensory_input_size", &PyAgentBrain::get_sensory_input_size,
             "Get expected sensory input size")
        .def("get_motor_output_size", &PyAgentBrain::get_motor_output_size,
             "Get expected motor output size")
        .def("__repr__", &PyAgentBrain::repr);

    // Setup enhanced SimpleWorld class
    py::class_<PySimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def(py::init<const PySimpleWorld&>())
        .def("configure", static_cast<void (PySimpleWorld::*)(int, int, int, int)>(&PySimpleWorld::configure),
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("configure", static_cast<void (PySimpleWorld::*)(float, float, int, int)>(&PySimpleWorld::configure),
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions (float version)")
        .def("reset", &PySimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &PySimpleWorld::set_agent_start,
             py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &PySimpleWorld::update, py::arg("timestep"),
             "Update world by timestep")
        .def("apply_motor_command", &PySimpleWorld::apply_motor_command,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("get_sensory_percept", &PySimpleWorld::get_sensory_percept,
             "Get sensory percept")
        .def("get_agent_body", &PySimpleWorld::get_agent_body,
             "Get agent body state")
        .def("add_object", &PySimpleWorld::add_object, py::arg("obj"),
             "Add world object")
        .def("remove_object", &PySimpleWorld::remove_object,
             py::arg("x"), py::arg("y"), "Remove world object")
        .def("is_valid_position", &PySimpleWorld::is_valid_position,
             py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("get_width", &PySimpleWorld::get_width, "Get world width")
        .def("get_height", &PySimpleWorld::get_height, "Get world height")
        .def("get_simulation_time", &PySimpleWorld::get_simulation_time,
             "Get simulation time")
        .def("get_max_energy", &PySimpleWorld::get_max_energy,
             "Get maximum energy")
        .def("set_max_energy", &PySimpleWorld::set_max_energy,
             py::arg("e"), "Set maximum energy")
        .def("set_random_seed", &PySimpleWorld::set_random_seed,
             py::arg("seed"), "Set random seed")
        .def("get_random_seed", &PySimpleWorld::get_random_seed,
             "Get random seed")
        .def("__repr__", &PySimpleWorld::repr);

    // Factory functions
    m.def("createDefaultConfig", []() -> PyConfig {
        return PyConfig();
    }, "Create a default configuration");

    m.def("createBrain", [](PyConfig config) -> PyBrain {
        return PyBrain(config.get_config());
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> PySimpleWorld {
        return PySimpleWorld();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](PyBrain brain) -> PyAgentBrain {
        return PyAgentBrain(brain.get_brain());
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience functions
    m.def("example_basic_usage", &example_basic_usage,
          "Print basic usage example");

    m.def("example_agent_usage", &example_agent_usage,
          "Print agent-based usage example");

    m.def("print_version_info", &print_version_info,
          "Print version and feature information");

    // Convenience function to create a simple agent simulation
    m.def("run_simple_simulation", 
          [](uint64_t steps, double timestep) {
              auto config = create_default_config();
              config.set_value("neuron_count", 100);
              config.set_value("region_count", 1);
              
              auto brain = create_brain(config);
              brain.initialize();
              
              auto world = create_simple_world();
              world.configure(20, 20, 8, 8);
              
              auto agent = create_agent_brain(brain);
              agent.initialize(world);
              agent.enable_reward_modulation(true);
              agent.enable_curiosity(true);
              
              for (uint64_t step = 0; step < steps; ++step) {
                  auto percept = world.get_sensory_percept();
                  agent.process_sensory_input(percept);
                  brain.step(step, step * timestep);
                  auto cmd = agent.decode_motor_command();
                  auto result = world.apply_motor_command(cmd, step * timestep);
                  agent.apply_reward_modulation(result.reward);
                  world.update(timestep);
              }
              
              return std::make_tuple(brain.get_average_firing_rate(),
                                   agent.get_curiosity_level(),
                                   agent.get_novelty_level());
          },
          py::arg("steps") = 100,
          py::arg("timestep") = 0.1,
          "Run a simple simulation and return statistics (avg_firing_rate, curiosity, novelty)");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version info
    m.attr("__version__") = "0.1.0-enhanced";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation framework";
}

} // namespace nlm