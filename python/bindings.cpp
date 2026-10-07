#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
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
namespace nlm {

// Context manager for Brain
class BrainWrapper {
public:
    BrainWrapper(std::shared_ptr<Config> config) : brain(std::make_shared<Brain>(config)) {}
    
    void initialize() {
        brain->initialize();
    }
    
    void step(int currentStep) {
        brain->step(currentStep);
    }
    
    void stepWithTime(int currentStep, double currentTime) {
        brain->step(currentStep, currentTime);
    }
    
    void reset() {
        brain->reset();
    }
    
    void save(const std::string& filepath) {
        brain->save(filepath);
    }
    
    void load(const std::string& filepath) {
        brain->load(filepath);
    }
    
    int getTotalNeuronCount() const {
        return brain->getTotalNeuronCount();
    }
    
    int getTotalSynapseCount() const {
        return brain->getTotalSynapseCount();
    }
    
    int getFiringNeuronCount() const {
        return brain->getFiringNeuronCount();
    }
    
    double getAverageFiringRate() const {
        return brain->getAverageFiringRate();
    }
    
    int getTotalSpikeCount() const {
        return brain->getTotalSpikeCount();
    }
    
    // Convenience methods for common operations
    void runForSteps(int numSteps, bool withTime = false, double timestep = 0.1) {
        for (int i = 0; i < numSteps; ++i) {
            if (withTime) {
                brain->step(i, i * timestep);
            } else {
                brain->step(i);
            }
        }
    }
    
    // Statistical analysis
    double getStats(const std::string& metric = "firing_rate") const {
        if (metric == "firing_rate") {
            return getAverageFiringRate();
        } else if (metric == "activity_level") {
            return getFiringNeuronCount() / (double)getTotalNeuronCount();
        } else if (metric == "spike_rate") {
            return getTotalSpikeCount();
        }
        return 0.0;
    }
    
    // Development helpers
    void enableDevelopment(bool enabled = true) {
        auto config = brain->getConfig();
        config->set("development.enabled", enabled);
    }
    
    std::string getDevelopmentStatus() const {
        auto stage = brain->getDevelopmentalStage();
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "Critical Period";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    // Get reference to underlying brain for advanced usage
    std::shared_ptr<Brain> getBrain() {
        return brain;
    }
    
private:
    std::shared_ptr<Brain> brain;
};

// Context manager for AgentBrain
class AgentBrainWrapper {
public:
    AgentBrainWrapper(std::shared_ptr<Brain> brain) : agent(std::make_shared<AgentBrain>(brain)) {}
    
    void initialize(std::shared_ptr<SimpleWorld> world) {
        agent->initialize(world);
    }
    
    void processSensoryInput(const SensoryPercept& percept) {
        agent->processSensoryInput( percept);
    }
    
    Action decodeMotorCommand() {
        return agent->decodeMotorCommand();
    }
    
    void applyRewardModulation(double reward, double predictedReward = 0.0) {
        agent->applyRewardModulation(reward, predictedReward);
    }
    
    void updateDevelopment(double timestep) {
        agent->updateDevelopment(timestep);
    }
    
    void reset() {
        agent->reset();
    }
    
    double getCuriosityLevel() const {
        return agent->getCuriosityLevel();
    }
    
    double getNoveltyLevel() const {
        return agent->getNoveltyLevel();
    }
    
    // Convenience methods for agent operation
    void runEpisode(std::shared_ptr<SimpleWorld> world, int maxSteps, double timestep = 0.1) {
        world->reset();
        agent->initialize(world);
        
        for (int step = 0; step < maxSteps; ++step) {
            world->update(step * timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            auto brain = agent->getBrain();
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, step * timestep);
            
            // Optional: Add reward modulation
            double reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
            agent->applyRewardModulation(reward, 0.0);
            
            agent->updateDevelopment(timestep);
        }
    }
    
    // Get reference to underlying agent for advanced usage
    std::shared_ptr<AgentBrain> getAgent() {
        return agent;
    }
    
private:
    std::shared_ptr<AgentBrain> agent;
};

// Enhanced Config class with better Python integration
class ConfigWrapper {
public:
    ConfigWrapper() : config(std::make_shared<Config>()) {}
    
    bool loadFromFile(const std::string& filepath) {
        return config->loadFromFile(filepath);
    }
    
    bool saveToFile(const std::string& filepath) const {
        return config->saveToFile(filepath);
    }
    
    void set(const std::string& key, const py::object& value) {
        if (value.is_none()) {
            throw std::invalid_argument("Cannot set config value to None");
        }
        
        // Convert Python object to appropriate ConfigValue
        if (py::isinstance<py::int_>(value)) {
            config->set(key, value.cast<int>());
        } else if (py::isinstance<py::float_>(value)) {
            config->set(key, value.cast<double>());
        } else if (py::isinstance<py::bool_>(value)) {
            config->set(key, value.cast<bool>());
        } else if (py::isinstance<py::str>(value)) {
            config->set(key, value.cast<std::string>());
        } else if (py::isinstance<py::list>(value)) {
            // Handle lists - convert to appropriate array type
            auto list = value.cast<std::vector<py::object>>();
            if (!list.empty()) {
                // Try to determine type from first element
                if (py::isinstance<py::int_>(list[0])) {
                    std::vector<int> intList;
                    for (const auto& item : list) {
                        intList.push_back(item.cast<int>());
                    }
                    config->set(key, intList);
                } else if (py::isinstance<py::float_>(list[0])) {
                    std::vector<double> doubleList;
                    for (const auto& item : list) {
                        doubleList.push_back(item.cast<double>());
                    }
                    config->set(key, doubleList);
                } else {
                    std::vector<std::string> stringList;
                    for (const auto& item : list) {
                        stringList.push_back(item.cast<std::string>());
                    }
                    config->set(key, stringList);
                }
            }
        } else {
            throw std::invalid_argument("Unsupported Python type for config value");
        }
    }
    
    py::object get(const std::string& key) const {
        if (!config->has(key)) {
            return py::none();
        }
        
        auto val = config->get<py::object>(key);
        return val.has_value() ? val.value() : py::none();
    }
    
    template<typename T>
    T getWithDefault(const std::string& key, T defaultValue) const {
        auto val = config->get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
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
    
    void remove(const std::string& key) {
        config->remove(key);
    }
    
    void clear() {
        config->clear();
    }
    
    // Brain configuration helpers
    void configureBrain(int neuronCount, float connectionProbability = 0.1f) {
        config->set("brain.neuron_count", neuronCount);
        config->set("brain.connection_probability", connectionProbability);
        config->set("brain.initial_weight_mean", 0.5f);
        config->set("brain.initial_weight_std", 0.1f);
    }
    
    void configureForTraining(int neuronCount, int maxSteps = 10000) {
        configureBrain(neuronCount);
        config->set("training.enabled", true);
        config->set("training.max_steps", maxSteps);
        config->set("training.learning_rate", 0.001f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
    }
    
    // Get reference to underlying config for advanced usage
    std::shared_ptr<Config> getConfig() {
        return config;
    }
    
private:
    std::shared_ptr<Config> config;
};

// Enhanced SimpleWorld with better Python API
class SimpleWorldWrapper {
public:
    SimpleWorldWrapper() : world(std::make_shared<SimpleWorld>()) {}
    
    void configure(int width, int height, int visionWidth = 8, int visionHeight = 8) {
        world->configure(width, height, visionWidth, visionHeight);
    }
    
    void reset() {
        world->reset();
    }
    
    void setAgentStart(double x, double y) {
        world->setAgentStart(x, y);
    }
    
    void update(double timestep) {
        world->update(timestep);
    }
    
    SensoryPercept getSensoryPercept() const {
        return world->getSensoryPercept();
    }
    
    AgentBody getAgentBody() const {
        return world->getAgentBody();
    }
    
    void applyMotorCommand(const Action& cmd, double currentTime) {
        world->applyMotorCommand(cmd, currentTime);
    }
    
    int getWidth() const {
        return world->getWidth();
    }
    
    int getHeight() const {
        return world->getHeight();
    }
    
    double getSimulationTime() const {
        return world->getSimulationTime();
    }
    
    // World object management
    void addObject(const WorldObject& obj) {
        world->addObject(obj);
    }
    
    void removeObject(int x, int y) {
        world->removeObject(x, y);
    }
    
    bool isValidPosition(int x, int y) const {
        return world->isValidPosition(x, y);
    }
    
    // Convenience methods for common world setup
    void createEnvironment() {
        // Create simple environment with resources
        world->reset();
        
        // Add some resources
        WorldObject food(5.0f, 5.0f, WorldObjectType::Resource, 1.0f, 0.5f);
        world->addObject(food);
        
        // Add some hazards
        WorldObject hazard(8.0f, 8.0f, WorldObjectType::Hazard, 10.0f, 0.3f);
        world->addObject(hazard);
    }
    
    // Get reference to underlying world for advanced usage
    std::shared_ptr<SimpleWorld> getWorld() {
        return world;
    }
    
private:
    std::shared_ptr<SimpleWorld> world;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python API
        -----------------------------------------------
        
        A user-friendly Python interface for the NLM neural simulation framework.
        
        Key features:
        - Context managers for automatic resource management
        - Convenience methods for common operations
        - Better error handling and validation
        - Enhanced documentation and examples
        - Statistical analysis tools
        - Development stage helpers
        - Episode running utilities
        
        The enhanced API makes it easier to:
        * Create and train neural networks
        * Run complete simulation episodes
        * Analyze brain activity
        * Save and load brain states
        * Configure complex environments
    )pbdoc";
    
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Neuron types
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
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // Enhanced Config class
    py::class_<ConfigWrapper>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &ConfigWrapper::loadFromFile,
             py::arg("filepath"), "Load configuration from a file (supports JSON, YAML, and CFG formats)")
        .def("save_to_file", &ConfigWrapper::saveToFile,
             py::arg("filepath"), "Save configuration to a file")
        .def("set", &ConfigWrapper::set,
             py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("get", &ConfigWrapper::get,
             py::arg("key"), "Get a configuration value (returns None if not set)")
        .def("get_with_default", &ConfigWrapper::template getWithDefault<int>,
             py::arg("key"), py::arg("default_value"), "Get a configuration value with default")
        .def("has", &ConfigWrapper::has,
             py::arg("key"), "Check if a configuration key exists")
        .def("get_keys", &ConfigWrapper::getKeys,
             "Get all configuration keys")
        .def("get_summary", &ConfigWrapper::getSummary,
             "Get a summary string of the configuration")
        .def("remove", &ConfigWrapper::remove,
             py::arg("key"), "Remove a configuration key")
        .def("clear", &ConfigWrapper::clear,
             "Clear all configuration entries")
        .def("configure_brain", &ConfigWrapper::configureBrain,
             py::arg("neuron_count"), py::arg("connection_probability") = 0.1f,
             "Configure brain with typical parameters")
        .def("configure_for_training", &ConfigWrapper::configureForTraining,
             py::arg("neuron_count"), py::arg("max_steps") = 10000,
             "Configure for training with neural network parameters");
    
    // Enhanced Brain class with context manager
    py::class_<BrainWrapper>(m, "Brain", R"pbdoc(Enhanced brain class with context manager support)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &BrainWrapper::initialize,
             "Initialize the brain with configuration")
        .def("step", &BrainWrapper::step,
             py::arg("current_step"), "Perform a simulation step")
        .def("step_with_time", &BrainWrapper::stepWithTime,
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("reset", &BrainWrapper::reset,
             "Reset brain state")
        .def("save", &BrainWrapper::save,
             py::arg("filepath"), "Save brain state to file")
        .def("load", &BrainWrapper::load,
             py::arg("filepath"), "Load brain state from file")
        .def("run_for_steps", &BrainWrapper::runForSteps,
             py::arg("num_steps"), py::arg("with_time") = false, py::arg("timestep") = 0.1,
             "Run brain for specified number of steps")
        .def("get_total_neuron_count", &BrainWrapper::getTotalNeuronCount,
             "Get total neuron count")
        .def("get_total_synapse_count", &BrainWrapper::getTotalSynapseCount,
             "Get total synapse count")
        .def("get_firing_neuron_count", &BrainWrapper::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &BrainWrapper::getAverageFiringRate,
             "Get average firing rate")
        .def("get_total_spike_count", &BrainWrapper::getTotalSpikeCount,
             "Get total spike count")
        .def("get_stats", &BrainWrapper::getStats,
             py::arg("metric") = "firing_rate", "Get statistics for specified metric")
        .def("enable_development", &BrainWrapper::enableDevelopment,
             py::arg("enabled") = true, "Enable/disable development system")
        .def("get_development_status", &BrainWrapper::getDevelopmentStatus,
             "Get current developmental stage as string")
        .def("get_brain", &BrainWrapper::getBrain,
             "Get underlying Brain object for advanced usage");
    
    // Enhanced AgentBrain class
    py::class_<AgentBrainWrapper>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrainWrapper::initialize,
             py::arg("world"), "Initialize with world")
        .def("process_sensory_input", &AgentBrainWrapper::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrainWrapper::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrainWrapper::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward") = 0.0,
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrainWrapper::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("reset", &AgentBrainWrapper::reset,
             "Reset agent for new episode")
        .def("get_curiosity_level", &AgentBrainWrapper::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrainWrapper::getNoveltyLevel,
             "Get novelty level")
        .def("run_episode", &AgentBrainWrapper::runEpisode,
             py::arg("world"), py::arg("max_steps"), py::arg("timestep") = 0.1,
             "Run complete episode with brain and world interaction")
        .def("get_agent", &AgentBrainWrapper::getAgent,
             "Get underlying AgentBrain object for advanced usage");
    
    // Enhanced SimpleWorld class
    py::class_<SimpleWorldWrapper>(m, "SimpleWorld", R"pbdoc(Enhanced simple world simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorldWrapper::configure,
             py::arg("width"), py::arg("height"), py::arg("vision_width") = 8, py::arg("vision_height") = 8,
             "Configure world dimensions")
        .def("reset", &SimpleWorldWrapper::reset,
             "Reset world to initial state")
        .def("set_agent_start", &SimpleWorldWrapper::setAgentStart,
             py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorldWrapper::update,
             py::arg("timestep"), "Update world")
        .def("get_sensory_percept", &SimpleWorldWrapper::getSensoryPercept,
             "Get sensory percept from world")
        .def("get_agent_body", &SimpleWorldWrapper::getAgentBody,
             "Get agent body state")
        .def("apply_motor_command", &SimpleWorldWrapper::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"), "Apply motor command to world")
        .def("create_environment", &SimpleWorldWrapper::createEnvironment,
             "Create simple environment with resources and hazards")
        .def("get_world", &SimpleWorldWrapper::getWorld,
             "Get underlying SimpleWorld object for advanced usage");
    
    // Factory functions
    m.def("create_default_config", []() {
        auto config = std::make_shared<Config>();
        config->configureBrain(1000);
        return config;
    }, "Create a default configuration with typical brain parameters");
    
    m.def("create_brain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<BrainWrapper>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a brain");
    
    m.def("create_simple_world", []() {
        auto world = std::make_shared<SimpleWorldWrapper>();
        world->configure(20, 20);
        world->createEnvironment();
        world->setAgentStart(10.0, 10.0);
        return world;
    }, "Create and initialize a simple world with environment");
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) {
        auto agent = std::make_shared<AgentBrainWrapper>(brain);
        return agent;
    }, py::arg("brain"), "Create agent brain interface");
    
    // Utility functions
    m.def("run_complete_training_episode", [](int neuronCount, int maxSteps, double timestep = 0.1) {
        auto config = std::make_shared<Config>();
        config->configureForTraining(neuronCount, maxSteps);
        
        auto brain = std::make_shared<BrainWrapper>(config);
        auto world = std::make_shared<SimpleWorldWrapper>();
        world->configure(15, 15);
        world->createEnvironment();
        world->setAgentStart(7.0, 7.0);
        
        auto agent = std::make_shared<AgentBrainWrapper>(brain->getBrain());
        agent->runEpisode(world->getWorld(), maxSteps, timestep);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("neuron_count") = 1000, py::arg("max_steps") = 10000, py::arg("timestep") = 0.1,
       "Run complete training episode and return brain, world, and agent");
    
    // Error types
    py::register_exception<std::runtime_error>(m, "NLMError");
    
    // Version info
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "Enhanced Python API for NLM neural simulation framework";
}

} // namespace nlm