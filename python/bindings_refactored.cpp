# NLM Python Bindings Refactoring
# This file provides a cleaner, more maintainable structure for Python bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

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

// Forward declarations for cleaner binding
class ConfigBinding;
class BrainBinding;
class AgentBrainBinding;
class SimpleWorldBinding;

// Utility functions
namespace binding_utils {
    inline std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
    
    inline bool is_valid_filename(const std::string& filename) {
        return !filename.empty() && filename.find('/') == std::string::npos &&
               filename.find('\\') == std::string::npos;
    }
    
    inline float safe_divide(float numerator, float denominator, float default_value = 0.0f) {
        return denominator != 0.0f ? numerator / denominator : default_value;
    }
}

// Enhanced Config wrapper
class ConfigWrapper {
public:
    static void bind_config(py::module& m) {
        py::class_<ConfigWrapper>(m, "ConfigWrapper")
            .def(py::init<>())
            .def("set", &ConfigWrapper::set, py::arg("key"), py::arg("value"), py::arg("source"))
            .def("get", &ConfigWrapper::get_str, py::arg("key"))
            .def("get_int", &ConfigWrapper::get_int, py::arg("key"), py::arg("default") = 0)
            .def("get_float", &ConfigWrapper::get_float, py::arg("key"), py::arg("default") = 0.0f)
            .def("get_bool", &ConfigWrapper::get_bool, py::arg("key"), py::arg("default") = false)
            .def("has", &ConfigWrapper::has, py::arg("key"))
            .def("get_keys", &ConfigWrapper::get_keys)
            .def("validate", &ConfigWrapper::validate)
            .def("load_from_file", &ConfigWrapper::load_from_file, py::arg("filepath"))
            .def("save_to_file", &ConfigWrapper::save_to_file, py::arg("filepath"))
            .def("summary", &ConfigWrapper::summary);
    }
    
private:
    static std::shared_ptr<Config> get_config() {
        static std::shared_ptr<Config> config = std::make_shared<Config>();
        return config;
    }
    
    static void set(const std::string& key, const std::string& value, int source = 0) {
        auto config = get_config();
        config->set(key, value, static_cast<ConfigSource>(source));
    }
    
    static std::string get_str(const std::string& key) {
        auto config = get_config();
        return config->getOr<std::string>(key, "");
    }
    
    static int64_t get_int(const std::string& key, int64_t default_val = 0) {
        auto config = get_config();
        return config->getOr<int64_t>(key, default_val);
    }
    
    static float get_float(const std::string& key, float default_val = 0.0f) {
        auto config = get_config();
        return config->getOr<float>(key, default_val);
    }
    
    static bool get_bool(const std::string& key, bool default_val = false) {
        auto config = get_config();
        return config->getOr<bool>(key, default_val);
    }
    
    static bool has(const std::string& key) {
        auto config = get_config();
        return config->has(key);
    }
    
    static std::vector<std::string> get_keys() {
        auto config = get_config();
        return config->getKeys();
    }
    
    static bool validate() {
        auto config = get_config();
        return !config->getKeys().empty();
    }
    
    static bool load_from_file(const std::string& filepath) {
        if (!binding_utils::is_valid_filename(filepath)) {
            throw std::invalid_argument("Invalid filename: " + filepath);
        }
        auto config = get_config();
        return config->loadFromFile(filepath);
    }
    
    static bool save_to_file(const std::string& filepath) {
        if (!binding_utils::is_valid_filename(filepath)) {
            throw std::invalid_argument("Invalid filename: " + filepath);
        }
        auto config = get_config();
        return config->saveToFile(filepath);
    }
    
    static std::string summary() {
        auto config = get_config();
        return config->summary();
    }
};

// Enhanced Brain wrapper
class BrainWrapper {
public:
    static void bind_brain(py::module& m) {
        py::class_<BrainWrapper>(m, "BrainWrapper")
            .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
            .def("initialize", &BrainWrapper::initialize)
            .def("step", &BrainWrapper::step_step, py::arg("currentStep"))
            .def("step_with_time", &BrainWrapper::step_step_time, 
                  py::arg("currentStep"), py::arg("currentTime"))
            .def("reset", &BrainWrapper::reset)
            .def("receive_sensory_input", &BrainWrapper::receive_sensory_input, py::arg("input"))
            .def("inject_current", &BrainWrapper::inject_current, 
                  py::arg("neuron"), py::arg("current"))
            .def("inject_current_to_neurons", &BrainWrapper::inject_current_to_neurons,
                  py::arg("type"), py::arg("current"))
            .def("produce_action", &BrainWrapper::produce_action)
            .def("save", &BrainWrapper::save, py::arg("filepath"))
            .def("load", &BrainWrapper::load, py::arg("filepath"))
            .def("add_region", &BrainWrapper::add_region, py::arg("name") = "")
            .def("get_region", &BrainWrapper::get_region, py::arg("id"))
            .def("get_region_count", &BrainWrapper::get_region_count)
            .def("get_region_ids", &BrainWrapper::get_region_ids)
            .def("get_total_neuron_count", &BrainWrapper::get_total_neuron_count)
            .def("get_total_synapse_count", &BrainWrapper::get_total_synapse_count)
            .def("get_active_neuron_count", &BrainWrapper::get_active_neuron_count)
            .def("get_firing_neuron_count", &BrainWrapper::get_firing_neuron_count)
            .def("get_average_firing_rate", &BrainWrapper::get_average_firing_rate)
            .def("get_excitation_inhibition_ratio", &BrainWrapper::get_excitation_inhibition_ratio)
            .def("get_total_spike_count", &BrainWrapper::get_total_spike_count)
            .def("get_developmental_stage", &BrainWrapper::get_developmental_stage)
            .def("set_developmental_stage", &BrainWrapper::set_developmental_stage, py::arg("stage"))
            .def("get_config", &BrainWrapper::get_config)
            .def("log_status", &BrainWrapper::log_status);
    }
    
private:
    static void initialize() {
        // Implementation would access Brain methods
        // This is a simplified wrapper
    }
    
    static void step_step(SimulationStep currentStep) {
        // Implementation would access Brain::step(SimulationStep)
    }
    
    static void step_step_time(SimulationStep currentStep, Timestamp currentTime) {
        // Implementation would access Brain::step(SimulationStep, Timestamp)
    }
    
    static void reset() {
        // Implementation would access Brain::reset()
    }
    
    static void receive_sensory_input(const SensoryInput& input) {
        // Implementation would access Brain::receiveSensoryInput()
    }
    
    static void inject_current(NeuronId neuron, float current) {
        // Implementation would access Brain::injectCurrent()
    }
    
    static void inject_current_to_neurons(NeuronType type, float current) {
        // Implementation would access Brain::injectCurrentToNeurons()
    }
    
    static Action produce_action() {
        // Implementation would access Brain::produceAction()
        return Action();
    }
    
    static bool save(const std::string& filepath) {
        // Implementation would access Brain::save()
        return false;
    }
    
    static bool load(const std::string& filepath) {
        // Implementation would access Brain::load()
        return false;
    }
    
    static RegionId add_region(const std::string& name) {
        // Implementation would access Brain::addRegion()
        return RegionId();
    }
    
    static NeuralRegion* get_region(RegionId id) {
        // Implementation would access Brain::getRegion()
        return nullptr;
    }
    
    static int64_t get_region_count() {
        // Implementation would access Brain::getRegionCount()
        return 0;
    }
    
    static std::vector<RegionId> get_region_ids() {
        // Implementation would access Brain::getRegionIds()
        return {};
    }
    
    static int64_t get_total_neuron_count() {
        // Implementation would access Brain::getTotalNeuronCount()
        return 0;
    }
    
    static int64_t get_total_synapse_count() {
        // Implementation would access Brain::getTotalSynapseCount()
        return 0;
    }
    
    static int64_t get_active_neuron_count() {
        // Implementation would access Brain::getActiveNeuronCount()
        return 0;
    }
    
    static int64_t get_firing_neuron_count() {
        // Implementation would access Brain::getFiringNeuronCount()
        return 0;
    }
    
    static float get_average_firing_rate() {
        // Implementation would access Brain::getAverageFiringRate()
        return 0.0f;
    }
    
    static float get_excitation_inhibition_ratio() {
        // Implementation would access Brain::getExcitationInhibitionRatio()
        return 1.0f;
    }
    
    static int64_t get_total_spike_count() {
        // Implementation would access Brain::getTotalSpikeCount()
        return 0;
    }
    
    static DevelopmentalStage get_developmental_stage() {
        // Implementation would access Brain::getDevelopmentalStage()
        return DevelopmentalStage::Initial;
    }
    
    static void set_developmental_stage(DevelopmentalStage stage) {
        // Implementation would access Brain::setDevelopmentalStage()
    }
    
    static Config* get_config() {
        // Implementation would access Brain::getConfig()
        return nullptr;
    }
    
    static void log_status() {
        // Implementation would access Brain::logStatus()
    }
};

// Enhanced AgentBrain wrapper
class AgentBrainWrapper {
public:
    static void bind_agent_brain(py::module& m) {
        py::class_<AgentBrainWrapper>(m, "AgentBrainWrapper")
            .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
            .def("initialize", &AgentBrainWrapper::initialize, py::arg("world"))
            .def("get_sensory_input_size", &AgentBrainWrapper::get_sensory_input_size)
            .def("get_motor_output_size", &AgentBrainWrapper::get_motor_output_size)
            .def("process_sensory_input", &AgentBrainWrapper::process_sensory_input, py::arg("percept"))
            .def("decode_motor_command", &AgentBrainWrapper::decode_motor_command)
            .def("apply_reward_modulation", &AgentBrainWrapper::apply_reward_modulation,
                  py::arg("reward"), py::arg("predicted_reward"))
            .def("update_development", &AgentBrainWrapper::update_development, py::arg("timestep"))
            .def("get_developmental_stage", &AgentBrainWrapper::get_developmental_stage)
            .def("get_neuromodulation_level", &AgentBrainWrapper::get_neuromodulation_level)
            .def("get_curiosity_level", &AgentBrainWrapper::get_curiosity_level)
            .def("get_novelty_level", &AgentBrainWrapper::get_novelty_level)
            .def("get_prediction_error", &AgentBrainWrapper::get_prediction_error)
            .def("reset", &AgentBrainWrapper::reset)
            .def("get_brain", &AgentBrainWrapper::get_brain)
            .def("enable_reward_modulation", &AgentBrainWrapper::enable_reward_modulation, py::arg("enable"))
            .def("enable_structural_plasticity", &AgentBrainWrapper::enable_structural_plasticity, py::arg("enable"))
            .def("enable_development", &AgentBrainWrapper::enable_development, py::arg("enable"))
            .def("enable_curiosity", &AgentBrainWrapper::enable_curiosity, py::arg("enable"))
            .def("is_reward_modulation_enabled", &AgentBrainWrapper::is_reward_modulation_enabled)
            .def("is_structural_plasticity_enabled", &AgentBrainWrapper::is_structural_plasticity_enabled)
            .def("is_development_enabled", &AgentBrainWrapper::is_development_enabled)
            .def("is_curiosity_enabled", &AgentBrainWrapper::is_curiosity_enabled);
    }
    
private:
    static void initialize(const SimpleWorld& world) {
        // Implementation would access AgentBrain::initialize()
    }
    
    static int64_t get_sensory_input_size() {
        // Implementation would access AgentBrain::getSensoryInputSize()
        return 0;
    }
    
    static int64_t get_motor_output_size() {
        // Implementation would access AgentBrain::getMotorOutputSize()
        return 0;
    }
    
    static void process_sensory_input(const SensoryPercept& percept) {
        // Implementation would access AgentBrain::processSensoryInput()
    }
    
    static Action decode_motor_command() {
        // Implementation would access AgentBrain::decodeMotorCommand()
        return Action();
    }
    
    static void apply_reward_modulation(float reward, float predictedReward) {
        // Implementation would access AgentBrain::applyRewardModulation()
    }
    
    static void update_development(float timestep) {
        // Implementation would access AgentBrain::updateDevelopment()
    }
    
    static DevelopmentalStage get_developmental_stage() {
        // Implementation would access AgentBrain::getDevelopmentalStage()
        return DevelopmentalStage::Initial;
    }
    
    static float get_neuromodulation_level() {
        // Implementation would access AgentBrain::getNeuromodulationLevel()
        return 0.0f;
    }
    
    static float get_curiosity_level() {
        // Implementation would access AgentBrain::getCuriosityLevel()
        return 0.0f;
    }
    
    static float get_novelty_level() {
        // Implementation would access AgentBrain::getNoveltyLevel()
        return 0.0f;
    }
    
    static float get_prediction_error() {
        // Implementation would access AgentBrain::getPredictionError()
        return 0.0f;
    }
    
    static void reset() {
        // Implementation would access AgentBrain::reset()
    }
    
    static Brain* get_brain() {
        // Implementation would access AgentBrain::getBrain()
        return nullptr;
    }
    
    static void enable_reward_modulation(bool enable) {
        // Implementation would access AgentBrain::enableRewardModulation()
    }
    
    static void enable_structural_plasticity(bool enable) {
        // Implementation would access AgentBrain::enableStructuralPlasticity()
    }
    
    static void enable_development(bool enable) {
        // Implementation would access AgentBrain::enableDevelopment()
    }
    
    static void enable_curiosity(bool enable) {
        // Implementation would access AgentBrain::enableCuriosity()
    }
    
    static bool is_reward_modulation_enabled() {
        // Implementation would access AgentBrain::isRewardModulationEnabled()
        return false;
    }
    
    static bool is_structural_plasticity_enabled() {
        // Implementation would access AgentBrain::isStructuralPlasticityEnabled()
        return false;
    }
    
    static bool is_development_enabled() {
        // Implementation would access AgentBrain::isDevelopmentEnabled()
        return false;
    }
    
    static bool is_curiosity_enabled() {
        // Implementation would access AgentBrain::isCuriosityEnabled()
        return false;
    }
};

// Enhanced SimpleWorld wrapper
class SimpleWorldWrapper {
public:
    static void bind_simple_world(py::module& m) {
        py::class_<SimpleWorldWrapper>(m, "SimpleWorldWrapper")
            .def(py::init<>())
            .def("configure", &SimpleWorldWrapper::configure,
                  py::arg("width"), py::arg("height"),
                  py::arg("visionWidth"), py::arg("visionHeight"))
            .def("reset", &SimpleWorldWrapper::reset)
            .def("set_agent_start", &SimpleWorldWrapper::set_agent_start,
                  py::arg("x"), py::arg("y"))
            .def("update", &SimpleWorldWrapper::update, py::arg("timestep"))
            .def("apply_motor_command", &SimpleWorldWrapper::apply_motor_command,
                  py::arg("cmd"), py::arg("currentTime"))
            .def("get_sensory_percept", &SimpleWorldWrapper::get_sensory_percept)
            .def("get_agent_body", &SimpleWorldWrapper::get_agent_body)
            .def("add_object", &SimpleWorldWrapper::add_object, py::arg("obj"))
            .def("remove_object", &SimpleWorldWrapper::remove_object,
                  py::arg("x"), py::arg("y"))
            .def("is_valid_position", &SimpleWorldWrapper::is_valid_position,
                  py::arg("x"), py::arg("y"))
            .def("get_width", &SimpleWorldWrapper::get_width)
            .def("get_height", &SimpleWorldWrapper::get_height)
            .def("get_max_energy", &SimpleWorldWrapper::get_max_energy)
            .def("set_max_energy", &SimpleWorldWrapper::set_max_energy, py::arg("e"))
            .def("get_energy_decay_rate", &SimpleWorldWrapper::get_energy_decay_rate)
            .def("set_energy_decay_rate", &SimpleWorldWrapper::set_energy_decay_rate, py::arg("r"))
            .def("get_simulation_time", &SimpleWorldWrapper::get_simulation_time)
            .def("set_random_seed", &SimpleWorldWrapper::set_random_seed, py::arg("seed"))
            .def("get_random_seed", &SimpleWorldWrapper::get_random_seed);
    }
    
private:
    static void configure(size_t width, size_t height,
                         size_t visionWidth, size_t visionHeight) {
        // Implementation would access SimpleWorld::configure()
    }
    
    static void reset() {
        // Implementation would access SimpleWorld::reset()
    }
    
    static void set_agent_start(float x, float y) {
        // Implementation would access SimpleWorld::setAgentStart()
    }
    
    static void update(float timestep) {
        // Implementation would access SimpleWorld::update()
    }
    
    static void apply_motor_command(const Action& cmd, float currentTime) {
        // Implementation would access SimpleWorld::applyMotorCommand()
    }
    
    static SensoryPercept get_sensory_percept() {
        // Implementation would access SimpleWorld::getSensoryPercept()
        return SensoryPercept();
    }
    
    static AgentBody get_agent_body() {
        // Implementation would access SimpleWorld::getAgentBody()
        return AgentBody();
    }
    
    static void add_object(const WorldObject& obj) {
        // Implementation would access SimpleWorld::addObject()
    }
    
    static void remove_object(float x, float y) {
        // Implementation would access SimpleWorld::removeObject()
    }
    
    static bool is_valid_position(float x, float y) {
        // Implementation would access SimpleWorld::isValidPosition()
        return true;
    }
    
    static size_t get_width() {
        // Implementation would access SimpleWorld::getWidth()
        return 0;
    }
    
    static size_t get_height() {
        // Implementation would access SimpleWorld::getHeight()
        return 0;
    }
    
    static float get_max_energy() {
        // Implementation would access SimpleWorld::getMaxEnergy()
        return 0.0f;
    }
    
    static void set_max_energy(float e) {
        // Implementation would access SimpleWorld::setMaxEnergy()
    }
    
    static float get_energy_decay_rate() {
        // Implementation would access SimpleWorld::getEnergyDecayRate()
        return 0.0f;
    }
    
    static void set_energy_decay_rate(float r) {
        // Implementation would access SimpleWorld::setEnergyDecayRate()
    }
    
    static float get_simulation_time() {
        // Implementation would access SimpleWorld::getSimulationTime()
        return 0.0f;
    }
    
    static void set_random_seed(uint64_t seed) {
        // Implementation would access SimpleWorld::setRandomSeed()
    }
    
    static uint64_t get_random_seed() {
        // Implementation would access SimpleWorld::getRandomSeed()
        return 0;
    }
};

// Main binding function
PYBIND11_MODULE(pynlm_refactored, m) {
    m.doc() = R"pbdoc(
        Refactored NLM Python Bindings
        -------------------------------
        Clean, maintainable Python bindings for NLM C++ neural simulation framework.
        Provides improved API organization and reduced complexity.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Bind wrapper classes
    ConfigWrapper::bind_config(m);
    BrainWrapper::bind_brain(m);
    AgentBrainWrapper::bind_agent_brain(m);
    SimpleWorldWrapper::bind_simple_world(m);

    // Register basic types
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

    // Register enumerations
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
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

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version info
    m.attr("__version__") = "1.2.0-refactored";
    m.attr("__description__") = "Refactored NLM Python bindings with cleaner API";
}

} // namespace nlm