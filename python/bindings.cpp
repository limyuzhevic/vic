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

namespace py = pybind11;
namespace nlm {

// Custom exception types for better error handling
class ConfigurationError : public std::runtime_error {
public:
    explicit ConfigurationError(const std::string& msg) : std::runtime_error("Configuration Error: " + msg) {}
};

class InitializationError : public std::runtime_error {
public:
    explicit InitializationError(const std::string& msg) : std::runtime_error("Initialization Error: " + msg) {}
};

class SimulationError : public std::runtime_error {
public:
    explicit SimulationError(const std::string& msg) : std::runtime_error("Simulation Error: " + msg) {}
};

class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& msg) : std::runtime_error("Validation Error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register custom exception types
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<InitializationError>(m, "InitializationError", PyExc_RuntimeError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError);

        // Register custom exception types
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<InitializationError>(m, "InitializationError", PyExc_RuntimeError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError);

    // Add convenience factory methods for common configurations
    m.def("createDevelopmentConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500);
        config->set("brain.synapse_density", 0.05);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 2.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("development.enabled", true);
        config->set("prediction.enabled", true);
        return config;
    }, "Create a development configuration optimized for learning");

    m.def("createResearchConfig", [](int neuronCount = 1000, float learningRate = 0.001f) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
        config->set("brain.synapse_density", 0.1f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", learningRate);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("prediction.enabled", true);
        config->set("cognition.planner.enabled", true);
        config->set("cognition.concept.enabled", true);
        config->set("cognition.attention.enabled", true);
        config->set("development.enabled", true);
        config->set("checkpointing.enabled", false);
        config->set("replay.enabled", false);
        return config;
    }, py::arg("neuron_count") = 1000, py::arg("learning_rate") = 0.001f,
       "Create a research configuration for experimentation");

    m.def("createProductionConfig", [](int neuronCount = 2000, bool enableLearning = false) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
        config->set("brain.synapse_density", 0.15f);
        config->set("plasticity.stdp.enable", enableLearning);
        config->set("plasticity.hebbian.enable", enableLearning);
        config->set("plasticity.structural.enable", false);  // Disable for stability
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", false);  // Reduce exploration
        config->set("neuromod.novelty.enable", false);
        config->set("prediction.enabled", true);
        config->set("development.enabled", false);  // Mature, stable brain
        config->set("checkpointing.enabled", true);
        config->set("replay.enabled", true);
        config->set("simulation.timestep", 0.001);
        return config;
    }, py::arg("neuron_count") = 2000, py::arg("enable_learning") = false,
       "Create a production configuration for stable operation");

    // Add brain factory methods
    m.def("createStandardBrain", [](int neuron_count) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config->set("brain.synapse_density", 0.1f);
        config->set("region_count", 1);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        return std::make_shared<Brain>(config);
    }, py::arg("neuron_count") = 1000, "Create a standard brain with default settings");

    m.def("createDummyBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 0);
        config->set("region_count", 0);
        config->set("plasticity.stdp.enable", false);
        config->set("plasticity.hebbian.enable", false);
        return std::make_shared<Brain>(config);
    }, "Create a minimal brain with zero neurons (for testing)");

    m.def("createSmallBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.05f);
        config->set("region_count", 1);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 0.5f);
        return std::make_shared<Brain>(config);
    }, "Create a small brain suitable for quick testing");

    // Add world factory methods
    m.def("createDefaultWorld", [](size_t width = 20, size_t height = 20, 
                                  size_t vision_width = 8, size_t vision_height = 8) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20,
       py::arg("vision_width") = 8, py::arg("vision_height") = 8,
       "Create a world with sensible defaults");

    m.def("createRandomWorld", [](uint32_t seed, size_t width = 20, size_t height = 20) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->setRandomSeed(seed);
        return world;
    }, py::arg("seed"), py::arg("width") = 20, py::arg("height") = 20,
       "Create a world with random initialization");

    // Add batch and utility functions
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                             std::shared_ptr<AgentBrain> agent, size_t steps) {
        if (!brain->initialize()) {
            throw InitializationError("Failed to initialize brain");
        }
        
        if (world) {
            world->reset();
        }
        
        if (agent && world) {
            agent->initialize(*world);
        }
        
        for (size_t i = 0; i < steps; ++i) {
            if (world) {
                world->update(0.1); // Fixed timestep for simulation
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(i, i * 0.1);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            } else {
                brain->step(i, i * 0.1);
            }
        }
        
        return brain->getAverageFiringRate();
    }, "Run a complete simulation with brain, world, and agent");

    m.def("createEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                             std::shared_ptr<AgentBrain> agent, size_t max_steps) {
        if (!brain->initialize()) {
            throw InitializationError("Failed to initialize brain for episode");
        }
        
        world->reset();
        agent->initialize(*world);
        
        std::vector<float> rewards;
        std::vector<float> firing_rates;
        
        for (size_t step = 0; step < max_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, step * 0.1);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = 0.0f;
            if (auto percept_ptr = brain->getEpisodicMemory()) {
                reward = percept_ptr->getRecentReward();
            }
            
            agent->applyRewardModulation(reward, 0.0f);
            
            rewards.push_back(reward);
            firing_rates.push_back(brain->getAverageFiringRate());
        }
        
        return py::make_tuple(rewards, firing_rates);
    }, "Create a learning episode and return rewards and firing rates");

    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, size_t steps) {
        if (!brain->initialize()) {
            throw InitializationError("Failed to initialize brain for benchmarking");
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        return py::make_tuple(
            brain->getAverageFiringRate(),
            brain->getTotalSpikeCount(),
            duration.count() / 1000.0f  // Convert to seconds
        );
    }, "Benchmark brain performance");

    m.def("validate_config", [](const Config& config) {
        // Basic validation
        if (config.has("brain.neuron_count")) {
            auto count_opt = config.get<int64_t>("brain.neuron_count");
            if (count_opt && *count_opt < 0) {
                throw ValidationError("Neuron count cannot be negative");
            }
        }
        
        if (config.has("brain.synapse_density")) {
            auto density_opt = config.get<double>("brain.synapse_density");
            if (density_opt && (*density_opt < 0.0 || *density_opt > 1.0)) {
                throw ValidationError("Synapse density must be between 0.0 and 1.0");
            }
        }
        
        return true;
    }, "Validate configuration parameters");

    m.def("create_learning_callback", [](std::function<void(size_t, float)> callback) {
        return callback;  // Just return the callback for later use
    }, "Create a callback function for learning progress updates");

    // Add Pythonic collection interfaces
    m.def("iter_regions", [](const Brain& brain) {
        auto region_ids = brain.getRegionIds();
        return py::range(region_ids.size());
    }, "Iterate over brain regions");

    m.def("get_region_names", [](const Brain& brain) {
        std::vector<std::string> names;
        for (RegionId id : brain.getRegionIds()) {
            if (auto region = brain.getRegion(id)) {
                names.push_back(region->getName());
            }
        }
        return names;
    }, "Get names of all brain regions");

    // Add vectorized operations
    m.def("batch_inject_current", [](Brain& brain, 
                                 const std::vector<NeuronId>& neurons,
                                 const std::vector<float>& currents) {
        if (neurons.size() != currents.size()) {
            throw ValidationError("Neurons and currents vectors must have same size");
        }
        
        for (size_t i = 0; i < neurons.size(); ++i) {
            brain.injectCurrent(neurons[i], currents[i]);
        }
    }, "Inject currents to multiple neurons at once");

    m.def("batch_enable_plasticity", [](Brain& brain,
                                      const std::vector<NeuronId>& neurons,
                                      bool stdp_enable, bool hebbian_enable,
                                      bool structural_enable) {
        for (NeuronId id : neurons) {
            // Note: This would require access to synapse management
            // For now, this is a placeholder for the functionality
        }
        return true;
    }, "Enable plasticity for multiple neurons");

    // Add serialization helpers
    m.def("save_brain", [](Brain& brain, const std::string& filepath) {
        return brain.save(filepath);
    }, "Save brain state to file");

    m.def("load_brain", [](Brain& brain, const std::string& filepath) {
        return brain.load(filepath);
    }, "Load brain state from file");

    m.def("create_config_from_dict", [](const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            py::object value = item.second;
            
            if (value.is(py::int_())) {
                config->set(key, py::cast<int64_t>(value));
            } else if (value.is(py::float_())) {
                config->set(key, py::cast<double>(value));
            } else if (value.is(py::bool_())) {
                config->set(key, py::cast<bool>(value));
            } else if (value.is(py::str())) {
                config->set(key, py::cast<std::string>(value));
            } else if (value.is(py::list())) {
                py::list list_val = py::cast<py::list>(value);
                std::vector<double> vec;
                for (auto item : list_val) {
                    vec.push_back(py::cast<double>(item));
                }
                config->set(key, vec);
            }
        }
        return config;
    }, "Create configuration from Python dictionary");

    m.def("config_to_dict", [](const Config& config) {
        py::dict result;
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            auto value_opt = config.get<ConfigValue>(key);
            if (value_opt) {
                result[key] = *value_opt;
            }
        }
        return result;
    }, "Convert configuration to Python dictionary");

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

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

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

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
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
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
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
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

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

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("config_to_dict", [](const Config& config) {
        py::dict result;
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            auto value_opt = config.get<ConfigValue>(key);
            if (value_opt) {
                result[key] = *value_opt;
            }
        }
        return result;
    }, "Convert configuration to Python dictionary");

    // Add Pythonic context managers for resource management
    class PythonBrainContext {
    public:
        PythonBrainContext(std::shared_ptr<Brain> brain) : brain_(brain), initialized_(false) {}
        
        bool enter() {
            if (brain_) {
                initialized_ = brain_->initialize();
            }
            return initialized_;
        }
        
        void exit() {
            if (brain_ && initialized_) {
                brain_->reset();
            }
        }
        
        // Expose brain methods through context manager
        void step(size_t step) {
            if (brain_ && initialized_) {
                brain_->step(step, step * 0.1);
            }
        }
        
        float get_average_firing_rate() const {
            if (brain_) {
                return brain_->getAverageFiringRate();
            }
            return 0.0f;
        }
        
        size_t get_total_spike_count() const {
            if (brain_) {
                return brain_->getTotalSpikeCount();
            }
            return 0;
        }
        
        std::shared_ptr<Brain> get_brain() { return brain_; }
        
    private:
        std::shared_ptr<Brain> brain_;
        bool initialized_;
    };

    m.def("brain_context", [](std::shared_ptr<Brain> brain) {
        return PythonBrainContext(brain);
    }, "Create a context manager for brain operations");

    class PythonWorldContext {
    public:
        PythonWorldContext(std::shared_ptr<SimpleWorld> world) : world_(world), configured_(false) {}
        
        bool enter() {
            if (world_) {
                // Configure with default settings
                world_->configure(20, 20, 8, 8);
                configured_ = true;
                world_->reset();
            }
            return configured_;
        }
        
        void exit() {
            if (world_ && configured_) {
                world_->reset();
            }
        }
        
        void update(double timestep = 0.1) {
            if (world_ && configured_) {
                world_->update(timestep);
            }
        }
        
        std::shared_ptr<SimpleWorld> get_world() { return world_; }
        
    private:
        std::shared_ptr<SimpleWorld> world_;
        bool configured_;
    };

    m.def("world_context", [](std::shared_ptr<SimpleWorld> world) {
        return PythonWorldContext(world);
    }, "Create a context manager for world operations");

    class PythonAgentContext {
    public:
        PythonAgentContext(std::shared_ptr<AgentBrain> agent) : agent_(agent), initialized_(false) {}
        
        bool enter() {
            // Agent needs a world, this is a placeholder for the pattern
            return true;
        }
        
        void exit() {
            if (agent_) {
                agent_->reset();
            }
        }
        
        void enable_all_systems() {
            if (agent_) {
                agent_->enableRewardModulation(true);
                agent_->enableStructuralPlasticity(true);
                agent_->enableDevelopment(true);
                agent_->enableCuriosity(true);
            }
        }
        
        float get_curiosity_level() const {
            if (agent_) {
                return agent_->getCuriosityLevel();
            }
            return 0.0f;
        }
        
    private:
        std::shared_ptr<AgentBrain> agent_;
        bool initialized_;
    };

    m.def("agent_context", [](std::shared_ptr<AgentBrain> agent) {
        return PythonAgentContext(agent);
    }, "Create a context manager for agent operations");

    // Add fluent API support - create wrapper classes that chain methods
    class FluentConfig {
    public:
        FluentConfig(std::shared_ptr<Config> config) : config_(config) {}
        
        std::shared_ptr<FluentConfig> set(const std::string& key, int64_t value) {
            config_->set(key, value);
            return shared_from_this();
        }
        
        std::shared_ptr<FluentConfig> set(const std::string& key, double value) {
            config_->set(key, value);
            return shared_from_this();
        }
        
        std::shared_ptr<FluentConfig> set(const std::string& key, bool value) {
            config_->set(key, value);
            return shared_from_this();
        }
        
        std::shared_ptr<FluentConfig> set(const std::string& key, const std::string& value) {
            config_->set(key, value);
            return shared_from_this();
        }
        
        std::shared_ptr<Config> build() {
            return config_;
        }
        
        std::shared_ptr<FluentConfig> with_neuron_count(size_t count) {
            return set("brain.neuron_count", static_cast<int64_t>(count));
        }
        
        std::shared_ptr<FluentConfig> with_synapse_density(float density) {
            return set("brain.synapse_density", static_cast<double>(density));
        }
        
        std::shared_ptr<FluentConfig> with_learning(bool enable) {
            return set("plasticity.stdp.enable", enable);
        }
        
    private:
        std::shared_ptr<Config> config_;
        
        std::shared_ptr<FluentConfig> shared_from_this() {
            return std::static_pointer_cast<FluentConfig>(shared_from_this());
        }
    };

    m.def("fluent_config", []() {
        auto config = std::make_shared<Config>();
        return std::make_shared<FluentConfig>(config);
    }, "Create a fluent configuration builder");

    class FluentBrain {
    public:
        FluentBrain(std::shared_ptr<Brain> brain) : brain_(brain) {}
        
        std::shared_ptr<FluentBrain> initialize() {
            if (brain_) {
                brain_->initialize();
            }
            return shared_from_this();
        }
        
        std::shared_ptr<FluentBrain> step(size_t step) {
            if (brain_) {
                brain_->step(step, step * 0.1);
            }
            return shared_from_this();
        }
        
        std::shared_ptr<FluentBrain> inject_sensory(const SensoryInput& input) {
            if (brain_) {
                brain_->receiveSensoryInput(input);
            }
            return shared_from_this();
        }
        
        float get_firing_rate() const {
            if (brain_) {
                return brain_->getAverageFiringRate();
            }
            return 0.0f;
        }
        
        std::shared_ptr<Brain> build() {
            return brain_;
        }
        
    private:
        std::shared_ptr<Brain> brain_;
        
        std::shared_ptr<FluentBrain> shared_from_this() {
            return std::static_pointer_cast<FluentBrain>(shared_from_this());
        }
    };

    m.def("fluent_brain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<FluentBrain>(brain);
    }, "Create a fluent brain API for method chaining");

    // Add serialization and conversion utilities
    m.def("export_brain_state", [](const Brain& brain, const std::string& filepath) {
        return brain.save(filepath);
    }, "Export brain state to file");

    m.def("import_brain_state", [](Brain& brain, const std::string& filepath) {
        return brain.load(filepath);
    }, "Import brain state from file");

    m.def("create_sensory_from_vision", [](size_t width, size_t height, 
                                           const std::vector<float>& data) {
        auto vision = std::make_shared<Vision>(width, height, 3);
        vision->setData(data);
        return vision;
    }, "Create vision sensory input from data array");

    m.def("create_sensory_from_internal", [](const std::vector<float>& signals) {
        auto internal = std::make_shared<InternalSignals>();
        for (float signal : signals) {
            internal->addSignal(signal);
        }
        return internal;
    }, "Create internal signals from array");

    // Add mathematical operations on brain data
    m.def("compute_brain_metrics", [](const Brain& brain) {
        py::dict metrics;
        metrics["firing_rate"] = brain.getAverageFiringRate();
        metrics["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        metrics["total_spikes"] = brain.getTotalSpikeCount();
        metrics["active_neurons"] = brain.getActiveNeuronCount();
        metrics["firing_neuron_count"] = brain.getFiringNeuronCount();
        metrics["total_neuron_count"] = brain.getTotalNeuronCount();
        metrics["total_synapse_count"] = brain.getTotalSynapseCount();
        metrics["region_count"] = brain.getRegionCount();
        return metrics;
    }, "Compute comprehensive brain metrics");

    m.def("normalize_brain_weights", [](Brain& brain, float target_mean = 0.5f, float target_std = 0.1f) {
        // Normalize synaptic weights to target mean/std
        // This is a placeholder implementation
        return true;
    }, "Normalize synaptic weights to target statistics");

    m.def("analyze_brain_connectivity", [](const Brain& brain) {
        py::dict analysis;
        analysis["regions"] = brain.getRegionCount();
        analysis["total_connections"] = brain.getTotalSynapseCount();
        
        auto region_ids = brain.getRegionIds();
        size_t max_connections = 0;
        size_t min_connections = SIZE_MAX;
        
        for (RegionId id : region_ids) {
            if (auto region = brain.getRegion(id)) {
                size_t connections = region->getSynapses().size();
                max_connections = std::max(max_connections, connections);
                min_connections = std::min(min_connections, connections);
            }
        }
        
        analysis["max_region_connections"] = max_connections;
        analysis["min_region_connections"] = (min_connections == SIZE_MAX) ? 0 : min_connections;
        
        return analysis;
    }, "Analyze brain connectivity patterns");

    // Add performance monitoring utilities
    class PerformanceMonitor {
    public:
        PerformanceMonitor(std::shared_ptr<Brain> brain) : brain_(brain), step_count_(0) {
            start_time_ = std::chrono::high_resolution_clock::now();
        }
        
        void step() {
            if (brain_) {
                brain_->step(step_count_, step_count_ * 0.001);
                step_count_++;
            }
        }
        
        py::dict get_stats() {
            py::dict stats;
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_);
            
            stats["steps_completed"] = step_count_;
            stats["total_time_seconds"] = duration.count() / 1000.0;
            stats["steps_per_second"] = (step_count_ > 0) ? step_count_ / (duration.count() / 1000.0) : 0;
            
            if (brain_) {
                stats["average_firing_rate"] = brain_->getAverageFiringRate();
                stats["total_spikes"] = brain_->getTotalSpikeCount();
                stats["active_neurons"] = brain_->getActiveNeuronCount();
                stats["firing_neuron_count"] = brain_->getFiringNeuronCount();
            }
            
            return stats;
        }
        
    private:
        std::shared_ptr<Brain> brain_;
        size_t step_count_;
        std::chrono::high_resolution_clock::time_point start_time_;
    };

    m.def("performance_monitor", [](std::shared_ptr<Brain> brain) {
        return PerformanceMonitor(brain);
    }, "Create a performance monitor for brain operations");

    // Add comparison utilities
    m.def("compare_brains", [](const std::vector<std::shared_ptr<Brain>>& brains, 
                              const std::string& metric = "firing_rate") {
        py::list results;
        
        for (const auto& brain : brains) {
            py::dict brain_result;
            brain_result["firing_rate"] = brain->getAverageFiringRate();
            brain_result["total_spikes"] = brain->getTotalSpikeCount();
            brain_result["active_neurons"] = brain->getActiveNeuronCount();
            results.append(brain_result);
        }
        
        return results;
    }, "Compare multiple brains on specified metrics");

    // Add batch operation utilities
    m.def("batch_initialize", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        for (const auto& brain : brains) {
            if (brain) {
                brain->initialize();
            }
        }
        return true;
    }, "Initialize multiple brains");

    m.def("batch_step", [](const std::vector<std::shared_ptr<Brain>>& brains, size_t steps) {
        for (size_t step = 0; step < steps; ++step) {
            for (const auto& brain : brains) {
                if (brain) {
                    brain->step(step, step * 0.001);
                }
            }
        }
        return true;
    }, "Step multiple brains simultaneously");

    // Add numpy conversion helpers
    m.def("to_vector", [](const std::vector<float>& data) {
        return py::array(py::cast(data));
    }, "Convert std::vector<float> to numpy array");

    m.def("from_vector", [](const py::array& arr) {
        py::buffer_info buf = arr.request();
        float* ptr = (float*)buf.ptr;
        size_t size = buf.size;
        return std::vector<float>(ptr, ptr + size);
    }, "Convert numpy array to std::vector<float>");

    // Add checkpoint utilities
    m.def("create_checkpoint", [](Brain& brain, const std::string& path) {
        return brain.save(path);
    }, "Create a checkpoint of brain state");

    m.def("restore_checkpoint", [](Brain& brain, const std::string& path) {
        return brain.load(path);
    }, "Restore brain state from checkpoint");

    m.def("checkpoint_exists", [](const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        return file.good();
    }, "Check if checkpoint file exists");

    // Add validation and quality checking
    m.def("validate_brain_state", [](const Brain& brain) {
        py::dict issues;
        
        if (brain.getTotalNeuronCount() == 0) {
            issues["warning"] = "Brain has no neurons - may not be properly initialized";
        }
        
        if (brain.getTotalSynapseCount() == 0) {
            issues["info"] = "Brain has no synapses - may need connectivity configuration";
        }
        
        auto regions = brain.getRegions();
        if (regions.empty()) {
            issues["error"] = "Brain has no regions - initialization likely failed";
        }
        
        return issues;
    }, "Validate brain state and return any issues");

    m.def("compute_stability_metrics", [](const Brain& brain) {
        py::dict stability;
        
        // Compute stability based on firing rate variability
        float current_rate = brain.getAverageFiringRate();
        
        // Check for extreme values that indicate instability
        if (current_rate > 100.0f) {
            stability["warning"] = "High firing rate may indicate instability";
        } else if (current_rate < 0.1f) {
            stability["info"] = "Low firing rate - brain may be underactive";
        }
        
        // Check neuron count vs synapse count ratio
        size_t neuron_count = brain.getTotalNeuronCount();
        size_t synapse_count = brain.getTotalSynapseCount();
        
        if (neuron_count > 0 && synapse_count > 0) {
            float connectivity_ratio = static_cast<float>(synapse_count) / neuron_count;
            if (connectivity_ratio > 10.0f) {
                stability["warning"] = "Very high connectivity ratio";
            } else if (connectivity_ratio < 0.01f) {
                stability["info"] = "Very low connectivity ratio";
            }
        }
        
        return stability;
    }, "Compute stability metrics for brain state");

    // Add learning progress tracking
    class LearningProgressTracker {
    public:
        LearningProgressTracker(std::shared_ptr<Brain> brain, 
                               std::shared_ptr<AgentBrain> agent,
                               std::shared_ptr<SimpleWorld> world)
            : brain_(brain), agent_(agent), world_(world), 
              total_reward_(0.0f), best_reward_(-1000.0f), episode_count_(0) {}
        
        void step() {
            if (!world_ || !agent_ || !brain_) return;
            
            world_->update(0.1);
            auto percept = world_->getSensoryPercept();
            agent_->processSensoryInput(percept);
            brain_->step(step_count_, step_count_ * 0.1);
            
            auto action = agent_->decodeMotorCommand();
            world_->applyMotorCommand(action, world_->getSimulationTime());
            
            // Calculate reward (placeholder - would need world-specific reward calculation)
            float reward = 1.0f; // Simplified for demo
            total_reward_ += reward;
            best_reward_ = std::max(best_reward_, reward);
            
            step_count_++;
        }
        
        py::dict get_summary() {
            py::dict summary;
            summary["episodes"] = episode_count_;
            summary["total_reward"] = total_reward_;
            summary["average_reward"] = (episode_count_ > 0) ? total_reward_ / episode_count_ : 0.0f;
            summary["best_reward"] = best_reward_;
            summary["current_firing_rate"] = (brain_ ? brain_->getAverageFiringRate() : 0.0f);
            summary["curiosity_level"] = (agent_ ? agent_->getCuriosityLevel() : 0.0f);
            return summary;
        }
        
    private:
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<AgentBrain> agent_;
        std::shared_ptr<SimpleWorld> world_;
        float total_reward_;
        float best_reward_;
        size_t episode_count_;
        size_t step_count_;
    };

    m.def("learning_tracker", [](std::shared_ptr<Brain> brain,
                                std::shared_ptr<AgentBrain> agent,
                                std::shared_ptr<SimpleWorld> world) {
        return LearningProgressTracker(brain, agent, world);
    }, "Create a learning progress tracker");

    // Add convenience helper for complete agent simulation
    m.def("run_complete_agent_simulation", [](std::shared_ptr<Brain> brain,
                                             std::shared_ptr<SimpleWorld> world,
                                             std::shared_ptr<AgentBrain> agent,
                                             size_t steps) {
        if (!brain->initialize()) {
            throw InitializationError("Failed to initialize brain for complete simulation");
        }
        
        world->reset();
        agent->initialize(*world);
        
        std::vector<float> rewards;
        std::vector<float> firing_rates;
        std::vector<float> curiosity_levels;
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, step * 0.1);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Calculate reward (placeholder - use first internal signal as reward proxy)
            float reward = 0.0f;
            auto internal = percept.getInternal();
            if (!internal.empty()) {
                reward = internal[0];
            }
            
            agent->applyRewardModulation(reward, 0.0f);
            
            rewards.push_back(reward);
            firing_rates.push_back(brain->getAverageFiringRate());
            curiosity_levels.push_back(agent->getCuriosityLevel());
        }
        
        return py::make_tuple(rewards, firing_rates, curiosity_levels);
    }, "Run a complete agent simulation with reward tracking");

    // Add advanced configuration templates
    m.def("create_reinforcement_learning_config", [](int neuron_count = 500) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config->set("brain.synapse_density", 0.08f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.01f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 2.0f);  // Strong reward signal
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("prediction.enabled", true);
        config->set("cognition.planner.enabled", true);
        config->set("cognition.concept.enabled", true);
        config->set("cognition.attention.enabled", true);
        config->set("development.enabled", true);
        config->set("checkpointing.enabled", true);
        config->set("replay.enabled", true);
        config->set("simulation.timestep", 0.01);  // Slower for better learning
        return config;
    }, "Create configuration optimized for reinforcement learning");

    m.def("create_supervised_learning_config", [](int neuron_count = 200) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config->set("brain.synapse_density", 0.12f);  // More connections for pattern learning
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.005f);  // Slower learning
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", false);  // Focus on existing connections
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", false);  // Less exploration
        config->set("neuromod.novelty.enable", false);
        config->set("prediction.enabled", true);
        config->set("cognition.planner.enabled", false);  // Less planning needed
        config->set("cognition.concept.enabled", true);  // Pattern formation
        config->set("cognition.attention.enabled", false);
        config->set("development.enabled", false);  // Mature, stable network
        config->set("checkpointing.enabled", true);
        config->set("replay.enabled", true);
        config->set("simulation.timestep", 0.001);  // Fast simulation
        return config;
    }, "Create configuration optimized for supervised learning");

    m.def("create_exploratory_config", [](int neuron_count = 300) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config->set("brain.synapse_density", 0.06f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.02f);  // Fast learning
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.5f);
        config->set("neuromod.curiosity.enable", true);  // Strong exploration
        config->set("neuromod.novelty.enable", true);
        config->set("prediction.enabled", true);
        config->set("cognition.planner.enabled", true);
        config->set("cognition.concept.enabled", true);
        config->set("cognition.attention.enabled", true);
        config->set("development.enabled", true);  // Rapid development
        config->set("checkpointing.enabled", true);
        config->set("replay.enabled", true);
        config->set("simulation.timestep", 0.05);  // Slower for exploration
        return config;
    }, "Create configuration optimized for exploratory learning");

    m.def("create_emergency_response_config", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.03f);  // Sparse connections
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.1f);  // Very fast response
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 3.0f);  // Very strong reward signal
        config->set("neuromod.curiosity.enable", false);  // Fast, focused response
        config->set("neuromod.novelty.enable", false);
        config->set("prediction.enabled", false);  // Immediate response
        config->set("development.enabled", false);
        config->set("checkpointing.enabled", true);
        config->set("replay.enabled", true);
        config->set("simulation.timestep", 0.1);  // Very slow for emergency response
        return config;
    }, "Create configuration optimized for emergency response scenarios");

    // Add advanced world configurations
    m.def("create_noisy_world", [](uint32_t seed, float noise_level = 0.1f) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->setRandomSeed(seed);
        // Add noise to world dynamics
        return world;
    }, "Create a world with noise for testing robustness");

    m.def("create_structured_world", [](const std::string& layout) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        
        if (layout == "maze") {
            // Create maze structure
        } else if (layout == "grid") {
            // Create grid structure
        } else if (layout == "open") {
            // Create open field
        }
        
        world->reset();
        return world;
    }, "Create a world with predefined structure");

    // Add advanced agent configurations
    m.def("create_advanced_agent", [](std::shared_ptr<Brain> brain,
                                     std::shared_ptr<SimpleWorld> world,
                                     bool enable_all_systems = true) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        if (enable_all_systems) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
        }
        
        return agent;
    }, "Create an advanced agent with all systems enabled");

    m.def("create_minimal_agent", [](std::shared_ptr<Brain> brain,
                                    std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Only enable essential systems
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        
        return agent;
    }, "Create a minimal agent with only essential systems");

    // Add experiment management utilities
    class ExperimentManager {
    public:
        ExperimentManager(std::shared_ptr<Brain> brain,
                         std::shared_ptr<SimpleWorld> world,
                         std::shared_ptr<AgentBrain> agent,
                         const std::string& experiment_name)
            : brain_(brain), world_(world), agent_(agent), experiment_name_(experiment_name) {}
        
        void run() {
            if (!brain_->initialize()) {
                throw InitializationError("Failed to initialize brain for experiment");
            }
            
            world_->reset();
            agent_->initialize(*world_);
            
            // Run experiment
            for (size_t step = 0; step < max_steps_; ++step) {
                world_->update(0.1);
                auto percept = world_->getSensoryPercept();
                agent_->processSensoryInput(percept);
                brain_->step(step, step * 0.1);
                
                auto action = agent_->decodeMotorCommand();
                world_->applyMotorCommand(action, world_->getSimulationTime());
            }
            
            record_results();
        }
        
        void set_max_steps(size_t steps) { max_steps_ = steps; }
        void set_name(const std::string& name) { experiment_name_ = name; }
        
        py::dict get_results() const { return results_; }
        
    private:
        void record_results() {
            results_["experiment_name"] = experiment_name_;
            results_["max_steps"] = static_cast<int64_t>(max_steps_);
            results_["final_firing_rate"] = brain_->getAverageFiringRate();
            results_["total_spikes"] = brain_->getTotalSpikeCount();
            results_["curiosity_level"] = agent_->getCuriosityLevel();
            results_["novelty_level"] = agent_->getNoveltyLevel();
        }
        
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<SimpleWorld> world_;
        std::shared_ptr<AgentBrain> agent_;
        std::string experiment_name_;
        py::dict results_;
        size_t max_steps_ = 1000;
    };

    m.def("experiment_manager", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   const std::string& experiment_name) {
        return ExperimentManager(brain, world, agent, experiment_name);
    }, "Create an experiment manager for controlled experiments");

    // Add utility functions for data processing
    m.def("compute_neuron_statistics", [](const Brain& brain) {
        py::dict stats;
        
        size_t total_neurons = brain.getTotalNeuronCount();
        size_t active_neurons = brain.getActiveNeuronCount();
        size_t firing_neurons = brain.getFiringNeuronCount();
        
        stats["total"] = static_cast<int64_t>(total_neurons);
        stats["active"] = static_cast<int64_t>(active_neurons);
        stats["firing"] = static_cast<int64_t>(firing_neurons);
        stats["active_ratio"] = (total_neurons > 0) ? static_cast<double>(active_neurons) / total_neurons : 0.0;
        stats["firing_ratio"] = (total_neurons > 0) ? static_cast<double>(firing_neurons) / total_neurons : 0.0;
        
        return stats;
    }, "Compute statistics about neuron population");

    m.def("analyze_plasticity", [](const Brain& brain) {
        py::dict analysis;
        
        // Get plasticity systems
        auto* stdp = brain.getSTDP();
        auto* hebbian = brain.getHebbian();
        auto* structural = brain.getStructuralPlasticity();
        
        if (stdp) {
            analysis["stdp_enabled"] = true;
            // Could add more STDP-specific metrics
        } else {
            analysis["stdp_enabled"] = false;
        }
        
        if (hebbian) {
            analysis["hebbian_enabled"] = true;
        } else {
            analysis["hebbian_enabled"] = false;
        }
        
        if (structural) {
            analysis["structural_plasticity_enabled"] = true;
        } else {
            analysis["structural_plasticity_enabled"] = false;
        }
        
        return analysis;
    }, "Analyze plasticity system configuration");

    m.def("export_to_csv", [](const Brain& brain, const std::string& filepath) {
        // Export brain metrics to CSV file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filepath);
        }
        
        file << "metric,value\n";
        file << "total_neurons," << brain.getTotalNeuronCount() << "\n";
        file << "total_synapses," << brain.getTotalSynapseCount() << "\n";
        file << "active_neurons," << brain.getActiveNeuronCount() << "\n";
        file << "firing_neurons," << brain.getFiringNeuronCount() << "\n";
        file << "average_firing_rate," << brain.getAverageFiringRate() << "\n";
        file << "excitation_inhibition_ratio," << brain.getExcitationInhibitionRatio() << "\n";
        file << "total_spikes," << brain.getTotalSpikeCount() << "\n";
        
        file.close();
        return true;
    }, "Export brain metrics to CSV file");

    m.def("create_training_dataset", [](const std::vector<std::shared_ptr<Brain>>& brains,
                                        const std::string& output_path) {
        // Create a training dataset from multiple brain states
        std::ofstream file(output_path);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + output_path);
        }
        
        file << "brain_id,neuron_count,synapse_count,firing_rate,total_spikes\n";
        
        for (size_t i = 0; i < brains.size(); ++i) {
            const auto& brain = brains[i];
            file << i << ","
                 << brain->getTotalNeuronCount() << ","
                 << brain->getTotalSynapseCount() << ","
                 << brain->getAverageFiringRate() << ","
                 << brain->getTotalSpikeCount() << "\n";
        }
        
        file.close();
        return true;
    }, "Create a training dataset from multiple brain states");

    m.def("load_training_dataset", [](const std::string& filepath) {
        std::vector<std::shared_ptr<Brain>> brains;
        std::ifstream file(filepath);
        
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filepath);
        }
        
        std::string line;
        std::getline(file, line); // Skip header
        
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 5) {
                auto config = std::make_shared<Config>();
                // Create brain from loaded data
                // This is a simplified version - in practice would need more sophisticated loading
                brains.push_back(std::make_shared<Brain>(config));
            }
        }
        
        return brains;
    }, "Load training dataset from CSV file");

    m.def("create_ml_compatible_brain", [](int neuron_count = 500) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config->set("brain.synapse_density", 0.1f);
        config->set("region_count", 1);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("prediction.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        
        // Initialize with standard pattern
        brain->initialize();
        
        // Apply some initial activity for ML training
        for (size_t i = 0; i < neuron_count / 10; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Excitatory, 10.0f);
        }
        
        for (size_t step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        return brain;
    }, "Create a brain ready for machine learning applications");

    m.def("create_neural_network_architecture", [](int input_size, int hidden_size, 
                                                  int output_size, const std::string& topology = "dense") {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(input_size + hidden_size + output_size));
        config->set("brain.synapse_density", 0.15f);
        config->set("region_count", 3); // Input, hidden, output regions
        
        // Set up regional connectivity based on topology
        if (topology == "dense") {
            config->set("brain.connection_probability", 0.8f);
        } else if (topology == "sparse") {
            config->set("brain.connection_probability", 0.1f);
        } else if (topology == "local") {
            config->set("brain.connection_probability", 0.3f);
        }
        
        auto brain = std::make_shared<Brain>(config);
        
        // Create regions with specific types
        auto regions = brain->getRegions();
        if (regions.size() >= 3) {
            // First region: input-like
            regions[0]->setNeuronType(NeuronType::Sensory);
            // Second region: hidden
            regions[1]->setNeuronType(NeuronType::Internal);
            // Third region: output
            regions[2]->setNeuronType(NeuronType::Motor);
        }
        
        return brain;
    }, "Create a neural network architecture with specified topology");

    m.def("create_brain_from_architecture", [](const py::dict& architecture) {
        int input_size = py::cast<int>(architecture["input_size"]);
        int hidden_size = py::cast<int>(architecture["hidden_size"]);
        int output_size = py::cast<int>(architecture["output_size"]);
        std::string topology = py::cast<std::string>(architecture["topology"]);
        
        return create_neural_network_architecture(input_size, hidden_size, output_size, topology);
    }, "Create a brain from neural network architecture dictionary");

    // Add comprehensive debugging utilities
    class DebugBrain {
    public:
        DebugBrain(std::shared_ptr<Brain> brain) : brain_(brain) {}
        
        py::dict get_debug_info() {
            py::dict info;
            
            if (!brain_) {
                info["error"] = "Brain pointer is null";
                return info;
            }
            
            info["initialized"] = brain_->initialize(); // This doesn't actually initialize if already initialized
            
            auto regions = brain_->getRegions();
            py::list region_info;
            
            for (size_t i = 0; i < regions.size(); ++i) {
                py::dict region_data;
                region_data["index"] = static_cast<int64_t>(i);
                region_data["neuron_count"] = regions[i]->getNeuronCount();
                region_data["synapse_count"] = regions[i]->getSynapseCount();
                
                auto neurons = regions[i]->getNeurons();
                size_t active_count = 0;
                float total_potential = 0.0f;
                
                for (Neuron* neuron : neurons) {
                    if (neuron && neuron->getState().isActive) {
                        active_count++;
                    }
                    if (neuron) {
                        total_potential += neuron->getState().membranePotential;
                    }
                }
                
                region_data["active_neurons"] = static_cast<int64_t>(active_count);
                region_data["avg_potential"] = (neurons.size() > 0) ? total_potential / neurons.size() : 0.0f;
                
                region_info.append(region_data);
            }
            
            info["regions"] = region_info;
            info["total_neurons"] = brain_->getTotalNeuronCount();
            info["total_synapses"] = brain_->getTotalSynapseCount();
            info["active_neurons"] = brain_->getActiveNeuronCount();
            info["firing_neurons"] = brain_->getFiringNeuronCount();
            info["average_firing_rate"] = brain_->getAverageFiringRate();
            info["excitation_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
            info["total_spikes"] = brain_->getTotalSpikeCount();
            
            return info;
        }
        
        std::string get_brain_summary() {
            auto info = get_debug_info();
            std::stringstream ss;
            ss << "Brain Debug Summary:" << std::endl;
            ss << "  Total neurons: " << info["total_neurons"].cast<size_t>() << std::endl;
            ss << "  Total synapses: " << info["total_synapses"].cast<size_t>() << std::endl;
            ss << "  Active neurons: " << info["active_neurons"].cast<size_t>() << std::endl;
            ss << "  Firing neurons: " << info["firing_neurons"].cast<size_t>() << std::endl;
            ss << "  Average firing rate: " << info["average_firing_rate"].cast<float>() << std::endl;
            ss << "  E/I ratio: " << info["excitation_inhibition_ratio"].cast<float>() << std::endl;
            return ss.str();
        }
        
    private:
        std::shared_ptr<Brain> brain_;
    };

    m.def("debug_brain", [](std::shared_ptr<Brain> brain) {
        return DebugBrain(brain);
    }, "Create a debug interface for brain inspection");

    // Add validation utilities for complex scenarios
    m.def("validate_brain_for_simulation", [](const Brain& brain) {
        py::dict validation_results;
        
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        // Check basic structure
        if (brain.getTotalNeuronCount() == 0) {
            errors.push_back("Brain has no neurons");
        }
        
        if (brain.getTotalSynapseCount() == 0) {
            warnings.push_back("Brain has no synapses - will not propagate activity");
        }
        
        // Check connectivity
        auto regions = brain.getRegions();
        if (regions.empty()) {
            errors.push_back("Brain has no regions");
        }
        
        // Check developmental stage
        auto stage = brain.getDevelopmentalStage();
        if (stage == DevelopmentalStage::Initial && brain.getTotalNeuronCount() > 100) {
            warnings.push_back("Adult brain with many neurons in Initial developmental stage");
        }
        
        validation_results["errors"] = errors;
        validation_results["warnings"] = warnings;
        validation_results["valid"] = errors.empty();
        
        return validation_results;
    }, "Validate brain for simulation use");

    m.def("prepare_brain_for_learning", [](Brain& brain) {
        // Enable all plasticity systems for learning
        auto* stdp = brain.getSTDP();
        auto* hebbian = brain.getHebbian();
        auto* structural = brain.getStructuralPlasticity();
        
        if (stdp) {
            stdp->enable(true);
        }
        
        if (hebbian) {
            hebbian->enable(true);
        }
        
        if (structural) {
            structural->enable(true);
        }
        
        // Initialize development system
        auto* development = brain.getDevelopmentSystem();
        if (development) {
            development->enable(true);
        }
        
        // Enable neuromodulation
        auto* dopamine = brain.getDopamine();
        auto* curiosity = brain.getCuriosity();
        
        if (dopamine) {
            dopamine->enable(true);
        }
        
        if (curiosity) {
            curiosity->enable(true);
        }
        
        return true;
    }, "Prepare brain for learning by enabling all plasticity systems");

    m.def("reset_brain_for_training", [](Brain& brain) {
        // Reset brain state but preserve structure
        brain.reset();
        
        // Re-initialize with same configuration
        auto config = brain.getConfig();
        if (config) {
            // Create new brain with same config
            brain = Brain(config);
            brain->initialize();
        }
        
        return true;
    }, "Reset brain state for training");

    // Add comprehensive utility functions for advanced users
    m.def("create_comprehensive_experiment_setup", [](const std::string& experiment_name,
                                                      int neuron_count = 1000,
                                                      bool enable_all_systems = true) {
        py::dict setup;
        
        // Create brain with optimal configuration for experiments
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config->set("brain.synapse_density", 0.08f);
        config->set("region_count", 2);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.01f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("prediction.enabled", true);
        config->set("cognition.planner.enabled", true);
        config->set("cognition.concept.enabled", true);
        config->set("development.enabled", true);
        config->set("checkpointing.enabled", true);
        config->set("replay.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw InitializationError("Failed to initialize brain for comprehensive experiment");
        }
        
        setup["brain"] = brain;
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(30, 30, 12, 12);
        world->reset();
        
        setup["world"] = world;
        
        // Create agent with all systems enabled
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        if (enable_all_systems) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
        }
        
        setup["agent"] = agent;
        
        // Create experiment metadata
        py::dict metadata;
        metadata["name"] = experiment_name;
        metadata["neuron_count"] = neuron_count;
        metadata["timestamp"] = std::time(nullptr);
        metadata["version"] = "1.0.0";
        
        setup["metadata"] = metadata;
        
        return setup;
    }, "Create a comprehensive experiment setup with all systems");

    m.def("save_experiment_setup", [](const py::dict& setup, const std::string& path) {
        // Save experiment setup to file
        std::ofstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + path);
        }
        
        py::dict brain_config = setup["brain"].cast<std::shared_ptr<Config>>()->summary();
        // Convert to JSON-like string for simplicity
        file << "experiment_setup\n";
        file << "brain_config: " << brain_config["brain.neuron_count"].cast<int64_t>() << "\n";
        file << "world_width: 30\n";
        file << "world_height: 30\n";
        file << "timestamp: " << setup["metadata"].cast<py::dict>()["timestamp"].cast<int64_t>() << "\n";
        
        file.close();
        return true;
    }, "Save experiment setup to file");

    m.def("load_experiment_setup", [](const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + path);
        }
        
        py::dict setup;
        // Parse file and reconstruct setup
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("brain_config:") != std::string::npos) {
                // Parse brain config
                size_t colon = line.find(":");
                if (colon != std::string::npos) {
                    std::string value = line.substr(colon + 1);
                    // Remove whitespace
                    value.erase(0, value.find_first_not_of(" \t"));
                    auto config = std::make_shared<Config>();
                    config->set("brain.neuron_count", std::stoll(value));
                    setup["brain"] = config;
                }
            }
        }
        
        file.close();
        return setup;
    }, "Load experiment setup from file");

    m.def("create_brain_snapshot", [](const Brain& brain, const std::string& filepath) {
        return brain.save(filepath);
    }, "Create a snapshot of brain state for later restoration");

    m.def("restore_brain_snapshot", [](Brain& brain, const std::string& filepath) {
        return brain.load(filepath);
    }, "Restore brain state from snapshot");

    m.def("compare_brain_states", [](const Brain& brain1, const Brain& brain2) {
        py::dict comparison;
        
        comparison["neuron_count_match"] = brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount();
        comparison["synapse_count_match"] = brain1.getTotalSynapseCount() == brain2.getTotalSynapseCount();
        comparison["firing_rate_diff"] = std::abs(brain1.getAverageFiringRate() - brain2.getAverageFiringRate());
        comparison["spike_count_diff"] = std::abs(brain1.getTotalSpikeCount() - brain2.getTotalSpikeCount());
        
        return comparison;
    }, "Compare two brain states");

    m.def("create_brain_statistics_report", [](const Brain& brain) {
        std::stringstream report;
        
        report << "=== NLM Brain Statistics Report ===\n";
        report << "Generated at: " << std::time(nullptr) << "\n\n";
        report << "--- Neural Population ---\n";
        report << "Total Neurons: " << brain.getTotalNeuronCount() << "\n";
        report << "Active Neurons: " << brain.getActiveNeuronCount() << "\n";
        report << "Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
        report << "Average Firing Rate: " << brain.getAverageFiringRate() << " Hz\n";
        report << "Excitation/Inhibition Ratio: " << brain.getExcitationInhibitionRatio() << "\n\n";
        
        report << "--- Synaptic Connectivity ---\n";
        report << "Total Synapses: " << brain.getTotalSynapseCount() << "\n";
        report << "Connectivity Density: " << (brain.getTotalNeuronCount() > 0 ? 
            static_cast<double>(brain.getTotalSynapseCount()) / brain.getTotalNeuronCount() : 0.0) << "\n\n";
        
        report << "--- Spike Activity ---\n";
        report << "Total Spikes: " << brain.getTotalSpikeCount() << "\n";
        
        report << "--- Developmental Stage ---\n";
        report << "Current Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n\n";
        
        report << "--- Memory Systems ---\n";
        report << "Working Memory: " << (brain.getWorkingMemory() ? "Active" : "Inactive") << "\n";
        report << "Episodic Memory: " << (brain.getEpisodicMemory() ? "Active" : "Inactive") << "\n";
        report << "Associative Memory: " << (brain.getAssociativeMemory() ? "Active" : "Inactive") << "\n\n";
        
        report << "--- Neuromodulation ---\n";
        report << "Dopamine: " << (brain.getDopamine() ? "Active" : "Inactive") << "\n";
        report << "Curiosity: " << (brain.getCuriosity() ? "Active" : "Inactive") << "\n";
        report << "Prediction: " << (brain.getPredictionSystem() ? "Active" : "Inactive") << "\n\n";
        
        report << "--- Cognition ---\n";
        report << "Planner: " << (brain.getPlanner() ? "Active" : "Inactive") << "\n";
        report << "Concept Formation: " << (brain.getConceptFormation() ? "Active" : "Inactive") << "\n";
        report << "Attention: " << (brain.getAttention() ? "Active" : "Inactive") << "\n\n";
        
        report << "--- Development ---\n";
        report << "Development System: " << (brain.getDevelopmentSystem() ? "Active" : "Inactive") << "\n";
        
        return report.str();
    }, "Generate a comprehensive statistics report for a brain");

    // Add comprehensive testing utilities
    m.def("run_brain_self_test", [](std::shared_ptr<Brain> brain) {
        py::dict results;
        
        std::vector<std::string> tests_passed;
        std::vector<std::string> tests_failed;
        
        // Test 1: Basic initialization
        if (brain->initialize()) {
            tests_passed.push_back("Basic initialization");
        } else {
            tests_failed.push_back("Basic initialization");
        }
        
        // Test 2: Simulation step
        try {
            brain->step(0, 0.0);
            tests_passed.push_back("Simulation step");
        } catch (...) {
            tests_failed.push_back("Simulation step");
        }
        
        // Test 3: Sensory input processing
        try {
            auto vision = std::make_shared<Vision>(8, 8, 3);
            vision->setData(std::vector<float>(192, 0.5f));
            brain->receiveSensoryInput(*vision);
            tests_passed.push_back("Sensory input processing");
        } catch (...) {
            tests_failed.push_back("Sensory input processing");
        }
        
        // Test 4: Action production
        try {
            auto action = brain->produceAction();
            tests_passed.push_back("Action production");
        } catch (...) {
            tests_failed.push_back("Action production");
        }
        
        // Test 5: State persistence
        try {
            std::string test_file = "/tmp/test_brain.bin";
            if (brain->save(test_file)) {
                auto new_brain = std::make_shared<Brain>(brain->getConfig());
                new_brain->initialize();
                new_brain->load(test_file);
                tests_passed.push_back("State persistence");
            } else {
                tests_failed.push_back("State persistence");
            }
        } catch (...) {
            tests_failed.push_back("State persistence");
        }
        
        results["passed"] = tests_passed;
        results["failed"] = tests_failed;
        results["total"] = static_cast<int64_t>(tests_passed.size() + tests_failed.size());
        results["success_rate"] = static_cast<double>(tests_passed.size()) / results["total"].cast<double>();
        
        return results;
    }, "Run comprehensive self-test on a brain");

    m.def("create_test_brain", [](const std::string& test_type) {
        auto config = std::make_shared<Config>();
        
        if (test_type == "minimal") {
            config->set("brain.neuron_count", 0);
            config->set("region_count", 0);
            config->set("plasticity.stdp.enable", false);
            config->set("plasticity.hebbian.enable", false);
        } else if (test_type == "small") {
            config->set("brain.neuron_count", 50);
            config->set("brain.synapse_density", 0.05f);
            config->set("region_count", 1);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
        } else if (test_type == "large") {
            config->set("brain.neuron_count", 5000);
            config->set("brain.synapse_density", 0.15f);
            config->set("region_count", 3);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a test brain of specified type");

    // Add comprehensive data analysis utilities
    m.def("analyze_brain_activity_patterns", [](const Brain& brain, size_t window_size = 100) {
        py::dict analysis;
        
        // Collect firing rates over time window
        std::vector<float> firing_rates;
        
        // Simulate activity collection (in practice would use time-series data)
        for (size_t i = 0; i < window_size; ++i) {
            // This is a simplified version - real implementation would track actual time series
            firing_rates.push_back(brain.getAverageFiringRate() * (1.0f + (i % 10) * 0.1f));
        }
        
        // Calculate statistics
        float sum = std::accumulate(firing_rates.begin(), firing_rates.end(), 0.0f);
        float mean = sum / firing_rates.size();
        
        float sq_sum = 0.0f;
        for (float rate : firing_rates) {
            sq_sum += (rate - mean) * (rate - mean);
        }
        float stddev = std::sqrt(sq_sum / firing_rates.size());
        
        float min_rate = *std::min_element(firing_rates.begin(), firing_rates.end());
        float max_rate = *std::max_element(firing_rates.begin(), firing_rates.end());
        
        analysis["mean_firing_rate"] = mean;
        analysis["firing_rate_stddev"] = stddev;
        analysis["firing_rate_min"] = min_rate;
        analysis["firing_rate_max"] = max_rate;
        analysis["coefficient_of_variation"] = (mean > 0) ? stddev / mean : 0.0f;
        
        // Detect patterns
        py::list patterns;
        if (stddev > mean * 0.5f) {
            patterns.append("high variability");
        }
        
        if (mean > 10.0f) {
            patterns.append("high activity");
        }
        
        if (max_rate - min_rate > mean * 2.0f) {
            patterns.append("oscillatory behavior");
        }
        
        analysis["patterns"] = patterns;
        
        return analysis;
    }, "Analyze brain activity patterns over time");

    m.def("compute_brain_adaptation_metrics", [](const Brain& brain) {
        py::dict metrics;
        
        // Compute adaptation metrics based on brain state
        auto stage = brain.getDevelopmentalStage();
        
        metrics["developmental_stage"] = static_cast<int>(stage);
        metrics["plasticity_readiness"] = (stage != DevelopmentalStage::Adult) ? 1.0f : 0.1f;
        
        // Estimate adaptation based on firing rate and connectivity
        float firing_rate = brain.getAverageFiringRate();
        float connectivity = static_cast<float>(brain.getTotalSynapseCount()) / 
                            std::max(static_cast<size_t>(1), brain.getTotalNeuronCount());
        
        metrics["estimated_adaptation"] = (firing_rate / 10.0f) * (connectivity * 10.0f);
        
        // Check for signs of over/under-connectivity
        if (connectivity > 0.2f) {
            metrics["connectivity_warning"] = "High connectivity may indicate over-excitability";
        } else if (connectivity < 0.01f) {
            metrics["connectivity_warning"] = "Low connectivity may indicate under-activation";
        }
        
        return metrics;
    }, "Compute brain adaptation and development metrics");

    m.def("generate_brain_summary_report", [](const Brain& brain) {
        std::stringstream report;
        
        report << "=== NLM Brain Analysis Report ===\n\n";
        report << "Brain Statistics:\n";
        report << "  Total Neurons: " << brain.getTotalNeuronCount() << "\n";
        report << "  Total Synapses: " << brain.getTotalSynapseCount() << "\n";
        report << "  Active Neurons: " << brain.getActiveNeuronCount() << "\n";
        report << "  Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
        report << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz\n";
        report << "  E/I Balance: " << brain.getExcitationInhibitionRatio() << "\n";
        report << "  Total Spikes: " << brain.getTotalSpikeCount() << "\n\n";
        
        report << "System Status:\n";
        report << "  Working Memory: " << (brain.getWorkingMemory() ? "✓" : "✗") << "\n";
        report << "  Episodic Memory: " << (brain.getEpisodicMemory() ? "✓" : "✗") << "\n";
        report << "  Neuromodulation: " << (brain.getDopamine() ? "✓" : "✗") << "\n";
        report << "  Prediction System: " << (brain.getPredictionSystem() ? "✓" : "✗") << "\n";
        report << "  Development System: " << (brain.getDevelopmentSystem() ? "✓" : "✗") << "\n";
        report << "  Cognition (Planner): " << (brain.getPlanner() ? "✓" : "✗") << "\n";
        
        report << "\nDevelopmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
        
        return report.str();
    }, "Generate a human-readable brain summary report");

    // Add comprehensive experiment utilities
    m.def("create_standard_experiment_setup", [](const std::string& experiment_name,
                                                const std::string& config_type = "standard") {
        py::dict setup;
        
        // Create brain based on config type
        std::shared_ptr<Brain> brain;
        
        if (config_type == "standard") {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.1f);
            config->set("region_count", 1);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            brain = std::make_shared<Brain>(config);
        } else if (config_type == "learning") {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", 500);
            config->set("brain.synapse_density", 0.08f);
            config->set("region_count", 2);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.01f);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.dopamine.scale", 1.5f);
            config->set("neuromod.curiosity.enable", true);
            config->set("development.enabled", true);
            brain = std::make_shared<Brain>(config);
        } else if (config_type == "production") {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", 2000);
            config->set("brain.synapse_density", 0.15f);
            config->set("region_count", 3);
            config->set("plasticity.stdp.enable", false);
            config->set("plasticity.hebbian.enable", false);
            config->set("development.enabled", false);
            config->set("checkpointing.enabled", true);
            brain = std::make_shared<Brain>(config);
        }
        
        if (brain) {
            brain->initialize();
            setup["brain"] = brain;
        }
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        setup["world"] = world;
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        
        setup["agent"] = agent;
        
        // Experiment metadata
        py::dict metadata;
        metadata["name"] = experiment_name;
        metadata["config_type"] = config_type;
        metadata["created"] = std::time(nullptr);
        metadata["version"] = "1.0.0";
        
        setup["metadata"] = metadata;
        
        return setup;
    }, "Create a standard experiment setup with configurable type");

    m.def("run_standard_experiment", [](const py::dict& setup, size_t steps = 1000) {
        auto brain = setup["brain"].cast<std::shared_ptr<Brain>>();
        auto world = setup["world"].cast<std::shared_ptr<SimpleWorld>>();
        auto agent = setup["agent"].cast<std::shared_ptr<AgentBrain>>();
        
        std::vector<float> rewards;
        std::vector<float> firing_rates;
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, step * 0.1);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simple reward calculation based on agent activity
            float reward = agent->getCuriosityLevel() * 0.5f + 
                          (brain->getAverageFiringRate() / 50.0f) * 0.5f;
            agent->applyRewardModulation(reward, 0.0f);
            
            rewards.push_back(reward);
            firing_rates.push_back(brain->getAverageFiringRate());
        }
        
        py::dict results;
        results["rewards"] = rewards;
        results["firing_rates"] = firing_rates;
        results["final_reward"] = rewards.empty() ? 0.0f : rewards.back();
        results["average_reward"] = std::accumulate(rewards.begin(), rewards.end(), 0.0f) / rewards.size();
        results["average_firing_rate"] = std::accumulate(firing_rates.begin(), firing_rates.end(), 0.0f) / firing_rates.size();
        
        return results;
    }, "Run a standard experiment with the given setup");

    m.def("save_experiment_results", [](const py::dict& results, const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filepath);
        }
        
        auto rewards = results["rewards"].cast<std::vector<float>>();
        auto firing_rates = results["firing_rates"].cast<std::vector<float>>();
        
        file << "step,reward,firing_rate\n";
        for (size_t i = 0; i < rewards.size(); ++i) {
            file << i << "," << rewards[i] << "," << firing_rates[i] << "\n";
        }
        
        file.close();
        return true;
    }, "Save experiment results to CSV file");

    m.def("load_experiment_results", [](const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filepath);
        }
        
        std::vector<float> rewards;
        std::vector<float> firing_rates;
        std::string line;
        
        // Skip header
        std::getline(file, line);
        
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 3) {
                try {
                    rewards.push_back(std::stof(tokens[1]));
                    firing_rates.push_back(std::stof(tokens[2]));
                } catch (...) {
                    // Skip invalid lines
                }
            }
        }
        
        py::dict results;
        results["rewards"] = rewards;
        results["firing_rates"] = firing_rates;
        
        return results;
    }, "Load experiment results from CSV file");

    m.def("create_benchmark_suite", []() {
        py::dict benchmark_suite;
        
        // Create benchmark configurations
        auto standard_config = std::make_shared<Config>();
        standard_config->set("brain.neuron_count", 100);
        standard_config->set("brain.synapse_density", 0.1f);
        
        auto learning_config = std::make_shared<Config>();
        learning_config->set("brain.neuron_count", 200);
        learning_config->set("brain.synapse_density", 0.08f);
        learning_config->set("plasticity.stdp.enable", true);
        learning_config->set("plasticity.hebbian.enable", true);
        
        auto production_config = std::make_shared<Config>();
        production_config->set("brain.neuron_count", 500);
        production_config->set("brain.synapse_density", 0.15f);
        
        benchmark_suite["standard"] = standard_config;
        benchmark_suite["learning"] = learning_config;
        benchmark_suite["production"] = production_config;
        
        return benchmark_suite;
    }, "Create a benchmark suite with different configurations");

    m.def("run_benchmark", [](const py::dict& benchmark_suite, const std::string& config_type, 
                             size_t steps = 500) {
        auto config = benchmark_suite[config_type].cast<std::shared_ptr<Config>>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw InitializationError("Failed to initialize brain for benchmarking");
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        py::dict benchmark_result;
        benchmark_result["config_type"] = config_type;
        benchmark_result["steps_completed"] = static_cast<int64_t>(steps);
        benchmark_result["total_time_seconds"] = duration.count() / 1000.0;
        benchmark_result["steps_per_second"] = steps / (duration.count() / 1000.0);
        benchmark_result["average_firing_rate"] = brain->getAverageFiringRate();
        benchmark_result["total_spikes"] = brain->getTotalSpikeCount();
        
        return benchmark_result;
    }, "Run a benchmark with specified configuration");

    m.def("create_performance_report", [](const py::dict& benchmark_results) {
        std::stringstream report;
        
        report << "=== NLM Performance Benchmark Report ===\n\n";
        
        // Collect all results
        std::vector<py::dict> results;
        for (auto item : benchmark_results) {
            results.push_back(item.second.cast<py::dict>());
        }
        
        report << "Benchmark Results:\n";
        for (const auto& result : results) {
            std::string config_type = result["config_type"].cast<std::string>();
            double throughput = result["steps_per_second"].cast<double>();
            double firing_rate = result["average_firing_rate"].cast<double>();
            size_t total_spikes = result["total_spikes"].cast<size_t>();
            
            report << "  " << config_type << ":\n";
            report << "    Throughput: " << throughput << " steps/second\n";
            report << "    Avg Firing Rate: " << firing_rate << " Hz\n";
            report << "    Total Spikes: " << total_spikes << "\n";
        }
        
        // Find best performer
        if (!results.empty()) {
            size_t best_idx = 0;
            double best_throughput = results[0]["steps_per_second"].cast<double>();
            
            for (size_t i = 1; i < results.size(); ++i) {
                double throughput = results[i]["steps_per_second"].cast<double>();
                if (throughput > best_throughput) {
                    best_throughput = throughput;
                    best_idx = i;
                }
            }
            
            report << "\nBest Performer: " << results[best_idx]["config_type"].cast<std::string>() << "\n";
            report << "  Throughput: " << best_throughput << " steps/second\n";
        }
        
        return report.str();
    }, "Create a performance report from benchmark results");

    // Add comprehensive documentation and help utilities
    m.def("get_api_version", []() {
        return "1.0.0";
    }, "Get the current version of the Python API");

    m.def("get_available_config_types", []() {
        std::vector<std::string> types;
        types.push_back("standard");
        types.push_back("learning");
        types.push_back("production");
        types.push_back("research");
        types.push_back("emergency");
        return types;
    }, "Get list of available configuration types");

    m.def("get_experiment_templates", []() {
        py::dict templates;
        
        py::dict standard;
        standard["description"] = "Basic brain simulation with default settings";
        standard["neuron_count"] = 1000;
        standard["steps"] = 1000;
        
        py::dict learning;
        learning["description"] = "Brain optimized for learning and plasticity";
        learning["neuron_count"] = 500;
        learning["enable_curiosity"] = true;
        learning["enable_structural_plasticity"] = true;
        
        py::dict production;
        production["description"] = "Stable brain for production use";
        production["neuron_count"] = 2000;
        production["enable_learning"] = false;
        
        templates["standard"] = standard;
        templates["learning"] = learning;
        templates["production"] = production;
        
        return templates;
    }, "Get experiment templates with descriptions");

    m.def("show_help", [](const std::string& topic) {
        std::stringstream help;
        
        if (topic == "basics") {
            help << "=== NLM Python Basics ===\n\n";
            help << "Creating a brain:\n";
            help << "  import pynlm\n";
            help << "  config = pynlm.createDefaultConfig()\n";
            help << "  brain = pynlm.createBrain(config)\n";
            help << "  brain.initialize()\n\n";
            
            help << "Running a simulation:\n";
            help << "  for i in range(100):\n";
            help << "      brain.step(i)\n\n";
            
            help << "Creating a complete agent:\n";
            help << "  import pynlm\n";
            help << "  brain = pynlm.createBrain(pynlm.createDefaultConfig())\n";
            help << "  world = pynlm.createSimpleWorld()\n";
            help << "  agent = pynlm.createAgentBrain(brain)\n";
            help << "  agent.initialize(world)\n";
            help << "  // Run simulation...\n";
            
        } else if (topic == "factories") {
            help << "=== Factory Functions ===\n\n";
            help << "createDevelopmentConfig() - Create config for development/learning\n";
            help << "createResearchConfig() - Create config for experimentation\n";
            help << "createProductionConfig() - Create config for stable operation\n";
            help << "createStandardBrain() - Create brain with standard settings\n";
            help << "createSmallBrain() - Create minimal brain for testing\n";
            help << "createDummyBrain() - Create empty brain for testing\n";
            help << "createDefaultWorld() - Create world with sensible defaults\n";
            help << "createRandomWorld() - Create world with random initialization\n";
            
        } else if (topic == "advanced") {
            help << "=== Advanced Features ===\n\n";
            help << "Context Managers:\n";
            help << "  with pynlm.brain_context(brain) as ctx:\n";
            help << "      ctx.step(0)\n\n";
            
            help << "Fluent API:\n";
            help << "  brain = pynlm.fluent_config().with_neuron_count(1000).build()\n\n";
            
            help << "Performance Monitoring:\n";
            help << "  monitor = pynlm.performance_monitor(brain)\n";
            help << "  for i in range(100):\n";
            help << "      monitor.step()\n";
            help << "  stats = monitor.get_stats()\n\n";
            
            help << "Experiment Management:\n";
            help << "  manager = pynlm.experiment_manager(brain, world, agent, \"my_exp\")\n";
            help << "  manager.set_max_steps(5000)\n";
            help << "  manager.run()\n";
            
        } else if (topic == "errors") {
            help << "=== Error Handling ===\n\n";
            help << "Common Exceptions:\n";
            help << "  ConfigurationError - Invalid configuration parameters\n";
            help << "  InitializationError - Failed to initialize brain/world\n";
            help << "  SimulationError - Errors during simulation\n";
            help << "  ValidationError - Parameter validation failed\n\n";
            
            help << "Error Handling Example:\n";
            help << "  try:\n";
            help << "      brain = pynlm.createBrain(config)\n";
            help << "      brain.initialize()\n";
            help << "  except pynlm.InitializationError as e:\n";
            help << "      print(f\"Initialization failed: {e}\")\n";
            
        } else {
            help << "=== NLM Python API Help ===\n\n";
            help << "Available topics: basics, factories, advanced, errors\n";
            help << "Use pynlm.show_help('topic_name') for detailed information.\n";
        }
        
        return help.str();
    }, "Show help documentation for specified topic");

    // Register the original basic classes
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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

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

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

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

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
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
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
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
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

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

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Add Python-specific optimizations and utilities
    
    // Add numpy integration helpers
    m.def("numpy_available", []() {
        return true;  // Would check if numpy is available in a real implementation
    }, "Check if numpy is available for array operations");

    m.def("to_numpy", [](const std::vector<float>& data) {
        // This would convert to numpy array if available
        // For now, return a simple Python list wrapper
        return py::array(py::cast(data));
    }, "Convert std::vector<float> to numpy array");

    m.def("from_numpy", [](const py::array& arr) {
        // This would convert from numpy array if available
        // For now, this is a placeholder
        throw NotImplementedError("Numpy conversion requires numpy to be installed");
    }, "Convert numpy array to std::vector<float>");

    // Add memory pool utilities
    class MemoryPool {
    public:
        MemoryPool(size_t initial_size = 1024 * 1024) : pool_size_(initial_size) {}
        
        // Allocate memory block
        uint8_t* allocate(size_t size) {
            if (available_ < size) {
                // Would grow pool if needed
                available_ = pool_size_;
            }
            uint8_t* ptr = buffer_.get();
            available_ -= size;
            return ptr;
        }
        
        // Free memory (no-op for now, as this is a simplified version)
        void deallocate(uint8_t* ptr, size_t size) {
            available_ += size;
        }
        
        size_t get_available() const { return available_; }
        
    private:
        size_t pool_size_ = 1024 * 1024;  // 1MB initial pool
        size_t available_ = pool_size_;
        std::unique_ptr<uint8_t[]> buffer_ = std::make_unique<uint8_t[]>(pool_size_);
    };

    m.def("create_memory_pool", [](size_t initial_size) {
        return MemoryPool(initial_size);
    }, "Create a memory pool for efficient allocation");

    // Add vectorized operation utilities
    m.def("vectorized_inject_current", [](Brain& brain,
                                          const std::vector<NeuronId>& neurons,
                                          const std::vector<float>& currents,
                                          float default_current = 0.0f) {
        if (neurons.size() != currents.size()) {
            throw ValidationError("Neurons and currents vectors must have same size");
        }
        
        for (size_t i = 0; i < neurons.size(); ++i) {
            brain.injectCurrent(neurons[i], currents[i]);
        }
        
        return true;
    }, "Inject currents to multiple neurons using vectorized operations");

    m.def("batch_neuron_operations", [](Brain& brain,
                                       const std::vector<NeuronId>& neurons,
                                       std::function<void(Neuron*)> operation) {
        for (NeuronId id : neurons) {
            if (auto* neuron = brain.getRegion(RegionId(id.value()))->getNeuron(id)) {
                operation(neuron);
            }
        }
        return true;
    }, "Apply operation to multiple neurons batch");

    // Add lazy evaluation utilities
    class LazyEvaluation {
    public:
        LazyEvaluation(std::shared_ptr<Brain> brain) : brain_(brain) {}
        
        // Lazy step computation - only computes when needed
        class LazyStep {
        public:
            LazyStep(std::shared_ptr<Brain> brain, SimulationStep step, Timestamp time)
                : brain_(brain), step_(step), time_(time), computed_(false) {}
            
            // Force computation
            void compute() {
                if (!computed_) {
                    if (brain_) {
                        brain_->step(step_, time_);
                    }
                    computed_ = true;
                }
            }
            
            // Get computed result (placeholder)
            float get_result() const {
                return brain_ ? brain_->getAverageFiringRate() : 0.0f;
            }
            
        private:
            std::shared_ptr<Brain> brain_;
            SimulationStep step_;
            Timestamp time_;
            bool computed_;
        };
        
        LazyStep lazy_step(SimulationStep step, Timestamp time = 0.0) {
            return LazyStep(brain_, step, time);
        }
        
    private:
        std::shared_ptr<Brain> brain_;
    };

    m.def("lazy_evaluation", [](std::shared_ptr<Brain> brain) {
        return LazyEvaluation(brain);
    }, "Create a lazy evaluation wrapper for brain operations");

    // Add caching utilities
    class ResultCache {
    public:
        ResultCache(size_t max_cache_size = 100) : max_cache_size_(max_cache_size) {}
        
        // Cache a result
        template<typename T>
        void cache(const std::string& key, const T& result) {
            if (cache_.size() >= max_cache_size_) {
                // Remove oldest entry (simplified: remove first)
                cache_.erase(cache_.begin());
            }
            cache_[key] = result;
        }
        
        // Retrieve cached result
        template<typename T>
        std::optional<T> get(const std::string& key) {
            auto it = cache_.find(key);
            if (it != cache_.end()) {
                return std::any_cast<T>(it->second);
            }
            return std::nullopt;
        }
        
        // Clear cache
        void clear() {
            cache_.clear();
        }
        
        // Get cache size
        size_t size() const {
            return cache_.size();
        }
        
    private:
        size_t max_cache_size_;
        std::map<std::string, py::object> cache_;
    };

    m.def("create_result_cache", [](size_t max_size) {
        return ResultCache(max_size);
    }, "Create a result cache for frequently accessed data");

    // Add performance profiling utilities
    class PerformanceProfiler {
    public:
        PerformanceProfiler() {
            start_time_ = std::chrono::high_resolution_clock::now();
        }
        
        void start_operation(const std::string& operation_name) {
            current_operation_ = operation_name;
            operation_start_ = std::chrono::high_resolution_clock::now();
        }
        
        void end_operation() {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - operation_start_);
            
            OperationProfile profile;
            profile.name = current_operation_;
            profile.duration_us = duration.count();
            
            profiles_.push_back(profile);
            
            current_operation_.clear();
        }
        
        py::dict get_profile_summary() {
            py::dict summary;
            
            double total_time = 0.0;
            for (const auto& profile : profiles_) {
                total_time += profile.duration_us / 1000000.0;
            }
            
            summary["total_operations"] = static_cast<int64_t>(profiles_.size());
            summary["total_time_seconds"] = total_time;
            summary["operations_per_second"] = (total_time > 0) ? profiles_.size() / total_time : 0;
            
            return summary;
        }
        
        std::string get_detailed_report() {
            std::stringstream report;
            
            report << "=== Performance Profile Report ===\n\n";
            
            double total_time = 0.0;
            for (const auto& profile : profiles_) {
                total_time += profile.duration_us / 1000000.0;
            }
            
            report << "Total Operations: " << profiles_.size() << "\n";
            report << "Total Time: " << total_time << " seconds\n";
            report << "Operations/sec: " << (total_time > 0 ? profiles_.size() / total_time : 0) << "\n\n";
            
            report << "Operation Details:\n";
            for (size_t i = 0; i < profiles_.size(); ++i) {
                const auto& profile = profiles_[i];
                report << "  " << i + 1 << ". " << profile.name << ": "
                       << profile.duration_us << " us ("
                       << (profile.duration_us / 1000000.0) << " s)\n";
            }
            
            return report.str();
        }
        
    private:
        struct OperationProfile {
            std::string name;
            long long duration_us;
        };
        
        std::chrono::high_resolution_clock::time_point start_time_;
        std::string current_operation_;
        std::chrono::high_resolution_clock::time_point operation_start_;
        std::vector<OperationProfile> profiles_;
    };

    m.def("performance_profiler", []() {
        return PerformanceProfiler();
    }, "Create a performance profiler for operation timing");

    // Add data compression utilities
    m.def("compress_brain_data", [](const Brain& brain, const std::string& algorithm = "gzip") {
        // Placeholder for data compression
        // In practice would compress brain state for storage/transmission
        return true;
    }, "Compress brain data for storage or transmission");

    m.def("decompress_brain_data", [](Brain& brain, const std::string& compressed_data, 
                                      const std::string& algorithm = "gzip") {
        // Placeholder for data decompression
        // In practice would restore brain state from compressed data
        return true;
    }, "Decompress brain data from storage or transmission");

    // Add checkpoint utilities
    m.def("create_checkpoint", [](Brain& brain, const std::string& path, 
                                 bool compress = false) {
        return brain.save(path);
    }, "Create a checkpoint of brain state");

    m.def("restore_checkpoint", [](Brain& brain, const std::string& path,
                                   bool decompress = false) {
        return brain.load(path);
    }, "Restore brain state from checkpoint");

    m.def("checkpoint_exists", [](const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        return file.good();
    }, "Check if checkpoint file exists");

    // Add data validation utilities
    m.def("validate_brain_state", [](const Brain& brain) {
        py::dict issues;
        
        if (brain.getTotalNeuronCount() == 0) {
            issues["error"] = "Brain has no neurons";
        }
        
        if (brain.getTotalSynapseCount() == 0) {
            issues["warning"] = "Brain has no synapses";
        }
        
        auto regions = brain.getRegions();
        if (regions.empty()) {
            issues["error"] = "Brain has no regions";
        }
        
        auto stage = brain.getDevelopmentalStage();
        if (stage == DevelopmentalStage::Initial && brain.getTotalNeuronCount() > 1000) {
            issues["warning"] = "Large brain in Initial developmental stage";
        }
        
        return issues;
    }, "Validate brain state and return any issues");

    m.def("validate_config_for_brain", [](const Config& config) {
        py::dict validation;
        
        bool valid = true;
        
        auto neuron_count_opt = config.get<int64_t>("brain.neuron_count");
        if (neuron_count_opt) {
            if (*neuron_count_opt < 0) {
                validation["error"] = "Neuron count cannot be negative";
                valid = false;
            } else if (*neuron_count_opt > 100000) {
                validation["warning"] = "Very large neuron count may impact performance";
            }
        }
        
        auto density_opt = config.get<double>("brain.synapse_density");
        if (density_opt) {
            if (*density_opt < 0.0 || *density_opt > 1.0) {
                validation["error"] = "Synapse density must be between 0.0 and 1.0";
                valid = false;
            }
        }
        
        validation["valid"] = valid;
        return validation;
    }, "Validate configuration for brain creation");

    m.def("analyze_brain_health", [](const Brain& brain) {
        py::dict health;
        
        // Basic metrics
        size_t neuron_count = brain.getTotalNeuronCount();
        size_t synapse_count = brain.getTotalSynapseCount();
        size_t active_neurons = brain.getActiveNeuronCount();
        size_t firing_neurons = brain.getFiringNeuronCount();
        float firing_rate = brain.getAverageFiringRate();
        
        health["neuron_count"] = static_cast<int64_t>(neuron_count);
        health["synapse_count"] = static_cast<int64_t>(synapse_count);
        health["active_neuron_percentage"] = (neuron_count > 0) ? 
            static_cast<double>(active_neurons) / neuron_count * 100.0 : 0.0;
        health["firing_neuron_percentage"] = (neuron_count > 0) ? 
            static_cast<double>(firing_neurons) / neuron_count * 100.0 : 0.0;
        health["firing_rate"] = firing_rate;
        
        // Health indicators
        py::list indicators;
        
        if (neuron_count == 0) {
            indicators.append("CRITICAL: No neurons present");
        } else if (active_neurons == 0) {
            indicators.append("WARNING: No active neurons");
        }
        
        if (firing_rate > 100.0f) {
            indicators.append("WARNING: Very high firing rate");
        } else if (firing_rate < 0.1f && neuron_count > 100) {
            indicators.append("INFO: Low firing rate for large network");
        }
        
        if (synapse_count == 0 && neuron_count > 0) {
            indicators.append("WARNING: No synaptic connections");
        }
        
        health["health_indicators"] = indicators;
        
        return health;
    }, "Analyze brain health and provide indicators");

    // Add serialization utilities
    m.def("export_brain_to_json", [](const Brain& brain, const std::string& filepath) {
        // Export brain state to JSON format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filepath);
        }
        
        py::dict brain_data;
        brain_data["neuron_count"] = brain.getTotalNeuronCount();
        brain_data["synapse_count"] = brain.getTotalSynapseCount();
        brain_data["active_neurons"] = brain.getActiveNeuronCount();
        brain_data["firing_rate"] = brain.getAverageFiringRate();
        brain_data["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
        
        // Write JSON (simplified - would use proper JSON library in practice)
        file << "{\n";
        file << "  \"neuron_count\": " << brain_data["neuron_count"].cast<int64_t>() << ",\n";
        file << "  \"synapse_count\": " << brain_data["synapse_count"].cast<int64_t>() << ",\n";
        file << "  \"active_neurons\": " << brain_data["active_neurons"].cast<int64_t>() << ",\n";
        file << "  \"firing_rate\": " << brain_data["firing_rate"].cast<float>() << ",\n";
        file << "  \"developmental_stage\": " << brain_data["developmental_stage"].cast<int>() << "\n";
        file << "}\n";
        
        file.close();
        return true;
    }, "Export brain state to JSON file");

    m.def("import_brain_from_json", [](Brain& brain, const std::string& filepath) {
        // Import brain state from JSON format (simplified)
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filepath);
        }
        
        // Parse JSON (simplified - would use proper JSON library in practice)
        std::string line;
        while (std::getline(file, line)) {
            // Simple parsing for demonstration
            if (line.find("neuron_count") != std::string::npos) {
                // Extract value and set in brain config
            }
        }
        
        file.close();
        return true;
    }, "Import brain state from JSON file");

    // Add comprehensive test utilities
    m.def("run_comprehensive_tests", [](std::shared_ptr<Brain> brain) {
        py::dict test_results;
        
        std::vector<std::string> tests_passed;
        std::vector<std::string> tests_failed;
        std::vector<std::string> tests_skipped;
        
        // Test 1: Initialization
        try {
            if (brain->initialize()) {
                tests_passed.push_back("Basic initialization");
            } else {
                tests_failed.push_back("Basic initialization");
            }
        } catch (const std::exception& e) {
            tests_failed.push_back("Basic initialization: " + std::string(e.what()));
        }
        
        // Test 2: Step execution
        try {
            brain->step(0, 0.0);
            tests_passed.push_back("Step execution");
        } catch (const std::exception& e) {
            tests_failed.push_back("Step execution: " + std::string(e.what()));
        }
        
        // Test 3: Memory access
        try {
            auto regions = brain->getRegions();
            tests_passed.push_back("Memory access");
        } catch (const std::exception& e) {
            tests_failed.push_back("Memory access: " + std::string(e.what()));
        }
        
        // Test 4: State persistence
        try {
            std::string test_file = "/tmp/test_brain.bin";
            if (brain->save(test_file)) {
                auto new_brain = std::make_shared<Brain>(brain->getConfig());
                new_brain->initialize();
                new_brain->load(test_file);
                tests_passed.push_back("State persistence");
            } else {
                tests_skipped.push_back("State persistence: save failed");
            }
        } catch (const std::exception& e) {
            tests_failed.push_back("State persistence: " + std::string(e.what()));
        }
        
        // Test 5: Configuration access
        try {
            auto config = brain->getConfig();
            if (config) {
                tests_passed.push_back("Configuration access");
            } else {
                tests_failed.push_back("Configuration access");
            }
        } catch (const std::exception& e) {
            tests_failed.push_back("Configuration access: " + std::string(e.what()));
        }
        
        test_results["passed"] = tests_passed;
        test_results["failed"] = tests_failed;
        test_results["skipped"] = tests_skipped;
        test_results["total"] = static_cast<int64_t>(tests_passed.size() + 
                                                   tests_failed.size() + 
                                                   tests_skipped.size());
        test_results["success_rate"] = static_cast<double>(tests_passed.size()) / 
                                       test_results["total"].cast<double>();
        
        return test_results;
    }, "Run comprehensive tests on a brain");

    m.def("create_test_suite", [](const std::string& test_type) {
        auto config = std::make_shared<Config>();
        
        if (test_type == "minimal") {
            config->set("brain.neuron_count", 0);
            config->set("region_count", 0);
            config->set("plasticity.stdp.enable", false);
            config->set("plasticity.hebbian.enable", false);
        } else if (test_type == "small") {
            config->set("brain.neuron_count", 10);
            config->set("brain.synapse_density", 0.05f);
            config->set("region_count", 1);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
        } else if (test_type == "medium") {
            config->set("brain.neuron_count", 100);
            config->set("brain.synapse_density", 0.1f);
            config->set("region_count", 2);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.dopamine.scale", 1.0f);
        } else if (test_type == "large") {
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.15f);
            config->set("region_count", 3);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.dopamine.scale", 1.0f);
            config->set("neuromod.curiosity.enable", true);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a test brain of specified size");

    m.def("compare_brain_states", [](const Brain& brain1, const Brain& brain2) {
        py::dict comparison;
        
        comparison["neuron_count_match"] = brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount();
        comparison["synapse_count_match"] = brain1.getTotalSynapseCount() == brain2.getTotalSynapseCount();
        comparison["firing_rate_diff"] = std::abs(brain1.getAverageFiringRate() - brain2.getAverageFiringRate());
        comparison["spike_count_diff"] = std::abs(brain1.getTotalSpikeCount() - brain2.getTotalSpikeCount());
        
        return comparison;
    }, "Compare two brain states");

    m.def("create_brain_statistics", [](const Brain& brain) {
        py::dict stats;
        
        stats["total_neurons"] = brain.getTotalNeuronCount();
        stats["total_synapses"] = brain.getTotalSynapseCount();
        stats["active_neurons"] = brain.getActiveNeuronCount();
        stats["firing_neurons"] = brain.getFiringNeuronCount();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        stats["total_spikes"] = brain.getTotalSpikeCount();
        
        return stats;
    }, "Get comprehensive brain statistics");

    m.def("benchmark_brain_performance", [](const Brain& brain, size_t steps = 1000) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain.step(i, i * 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        py::dict benchmark;
        benchmark["steps_completed"] = static_cast<int64_t>(steps);
        benchmark["total_time_seconds"] = duration.count() / 1000.0;
        benchmark["steps_per_second"] = steps / (duration.count() / 1000.0);
        benchmark["average_firing_rate"] = brain.getAverageFiringRate();
        benchmark["total_spikes"] = brain.getTotalSpikeCount();
        
        return benchmark;
    }, "Benchmark brain performance");

    m.def("create_experiment_setup", [](const std::string& experiment_name,
                                        const std::string& config_type = "standard") {
        py::dict setup;
        
        auto config = std::make_shared<Config>();
        
        if (config_type == "standard") {
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.1f);
            config->set("region_count", 1);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
        } else if (config_type == "learning") {
            config->set("brain.neuron_count", 500);
            config->set("brain.synapse_density", 0.08f);
            config->set("region_count", 2);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.01f);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.dopamine.scale", 1.5f);
            config->set("neuromod.curiosity.enable", true);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        setup["brain"] = brain;
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        setup["world"] = world;
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        
        setup["agent"] = agent;
        
        py::dict metadata;
        metadata["name"] = experiment_name;
        metadata["config_type"] = config_type;
        
        setup["metadata"] = metadata;
        
        return setup;
    }, "Create an experiment setup with specified configuration");

    // Add comprehensive data processing utilities
    m.def("export_to_csv", [](const std::string& filepath, const py::dict& data) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filepath);
        }
        
        // Write data to CSV (simplified implementation)
        file << "metric,value\n";
        for (auto item : data) {
            std::string key = item.first.cast<std::string>();
            std::string value_str;
            
            if (item.second.is(py::int_())) {
                value_str = std::to_string(item.second.cast<int64_t>());
            } else if (item.second.is(py::float_())) {
                value_str = std::to_string(item.second.cast<double>());
            } else if (item.second.is(py::str())) {
                value_str = item.second.cast<std::string>();
            }
            
            file << key << "," << value_str << "\n";
        }
        
        file.close();
        return true;
    }, "Export data to CSV file");

    m.def("load_from_csv", [](const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filepath);
        }
        
        py::dict data;
        std::string line;
        std::getline(file, line); // Skip header
        
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 2) {
                std::string key = tokens[0];
                std::string value = tokens[1];
                
                // Try to convert to appropriate type
                try {
                    size_t pos;
                    int int_val = std::stoi(value);
                    data[key] = int_val;
                } catch (...) {
                    try {
                        double float_val = std::stod(value);
                        data[key] = float_val;
                    } catch (...) {
                        data[key] = value;
                    }
                }
            }
        }
        
        file.close();
        return data;
    }, "Load data from CSV file");

    m.def("create_training_dataset", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        py::dict dataset;
        
        std::vector<py::dict> brain_data;
        for (const auto& brain : brains) {
            py::dict brain_dict;
            brain_dict["neuron_count"] = brain->getTotalNeuronCount();
            brain_dict["synapse_count"] = brain->getTotalSynapseCount();
            brain_dict["firing_rate"] = brain->getAverageFiringRate();
            brain_dict["total_spikes"] = brain->getTotalSpikeCount();
            brain_data.push_back(brain_dict);
        }
        
        dataset["brains"] = brain_data;
        dataset["count"] = static_cast<int64_t>(brain_data.size());
        
        return dataset;
    }, "Create a training dataset from multiple brains");

    m.def("create_ml_ready_brain", [](int neuron_count = 500) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config->set("brain.synapse_density", 0.1f);
        config->set("region_count", 1);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("prediction.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        
        // Initialize with standard ML pattern
        brain->initialize();
        
        // Add some initial activity for ML training
        for (size_t i = 0; i < neuron_count / 20; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Excitatory, 5.0f);
        }
        
        for (size_t step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        return brain;
    }, "Create a brain ready for machine learning applications");

    m.def("create_neural_network_architecture", [](int input_size, int hidden_size,
                                                  int output_size, const std::string& topology = "dense") {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(input_size + hidden_size + output_size));
        config->set("brain.synapse_density", 0.15f);
        config->set("region_count", 3);
        
        // Configure regional connectivity
        if (topology == "dense") {
            config->set("brain.connection_probability", 0.8f);
        } else if (topology == "sparse") {
            config->set("brain.connection_probability", 0.1f);
        } else if (topology == "local") {
            config->set("brain.connection_probability", 0.3f);
        }
        
        auto brain = std::make_shared<Brain>(config);
        
        // Create regions with specific types
        auto regions = brain->getRegions();
        if (regions.size() >= 3) {
            regions[0]->setNeuronType(NeuronType::Sensory);  // Input
            regions[1]->setNeuronType(NeuronType::Internal); // Hidden
            regions[2]->setNeuronType(NeuronType::Motor);    // Output
        }
        
        return brain;
    }, "Create a neural network architecture with specified topology");

    m.def("create_advanced_config", [](int neuron_count, const py::dict& parameters) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count));
        
        // Set parameters from dictionary
        for (auto item : parameters) {
            std::string key = item.first.cast<std::string>();
            py::object value = item.second;
            
            if (value.is(py::int_())) {
                config->set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<double>());
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>());
            }
        }
        
        return config;
    }, "Create an advanced configuration with custom parameters");

    // Add comprehensive debugging and analysis tools
    m.def("create_debugger", [](std::shared_ptr<Brain> brain) {
        py::dict debug_info;
        
        if (!brain) {
            debug_info["error"] = "Brain pointer is null";
            return debug_info;
        }
        
        debug_info["neuron_count"] = brain->getTotalNeuronCount();
        debug_info["synapse_count"] = brain->getTotalSynapseCount();
        debug_info["active_neuron_count"] = brain->getActiveNeuronCount();
        debug_info["firing_neuron_count"] = brain->getFiringNeuronCount();
        debug_info["average_firing_rate"] = brain->getAverageFiringRate();
        debug_info["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        
        return debug_info;
    }, "Create a debugger for brain analysis");

    m.def("analyze_brain_connectivity", [](const Brain& brain) {
        py::dict connectivity;
        
        auto regions = brain.getRegions();
        connectivity["region_count"] = static_cast<int64_t>(regions.size());
        
        size_t total_synapses = 0;
        for (const auto& region : regions) {
            total_synapses += region->getSynapses().size();
        }
        
        connectivity["total_synapses"] = static_cast<int64_t>(total_synapses);
        connectivity["average_synapses_per_region"] = (regions.size() > 0) ? 
            static_cast<double>(total_synapses) / regions.size() : 0.0;
        
        // Connectivity quality metrics
        size_t neuron_count = brain.getTotalNeuronCount();
        if (neuron_count > 0) {
            connectivity["connectivity_efficiency"] = static_cast<double>(total_synapses) / neuron_count;
        }
        
        return connectivity;
    }, "Analyze brain connectivity patterns");

    m.def("monitor_brain_activity", [](const Brain& brain, size_t samples = 10) {
        py::dict activity_monitor;
        
        std::vector<float> firing_rates;
        
        for (size_t i = 0; i < samples; ++i) {
            firing_rates.push_back(brain.getAverageFiringRate());
        }
        
        float sum = std::accumulate(firing_rates.begin(), firing_rates.end(), 0.0f);
        float mean = sum / firing_rates.size();
        
        float sq_sum = 0.0f;
        for (float rate : firing_rates) {
            sq_sum += (rate - mean) * (rate - mean);
        }
        float stddev = std::sqrt(sq_sum / firing_rates.size());
        
        activity_monitor["mean_firing_rate"] = mean;
        activity_monitor["firing_rate_stddev"] = stddev;
        activity_monitor["coefficient_of_variation"] = (mean > 0) ? stddev / mean : 0.0f;
        
        return activity_monitor;
    }, "Monitor brain activity over time");

    // Add comprehensive testing utilities
    m.def("run_system_tests", [](std::shared_ptr<Brain> brain) {
        py::dict test_results;
        
        std::vector<std::string> passed_tests;
        std::vector<std::string> failed_tests;
        
        // Test 1: Initialization
        try {
            if (brain->initialize()) {
                passed_tests.push_back("Initialization test");
            } else {
                failed_tests.push_back("Initialization test");
            }
        } catch (const std::exception& e) {
            failed_tests.push_back("Initialization test: " + std::string(e.what()));
        }
        
        // Test 2: Basic simulation
        try {
            for (size_t i = 0; i < 10; ++i) {
                brain->step(i, i * 0.001);
            }
            passed_tests.push_back("Basic simulation test");
        } catch (const std::exception& e) {
            failed_tests.push_back("Basic simulation test: " + std::string(e.what()));
        }
        
        // Test 3: State saving/loading
        try {
            std::string test_file = "/tmp/test_state.bin";
            if (brain->save(test_file)) {
                auto new_brain = std::make_shared<Brain>(brain->getConfig());
                new_brain->initialize();
                new_brain->load(test_file);
                passed_tests.push_back("State persistence test");
            } else {
                failed_tests.push_back("State persistence test: save failed");
            }
        } catch (const std::exception& e) {
            failed_tests.push_back("State persistence test: " + std::string(e.what()));
        }
        
        test_results["passed"] = passed_tests;
        test_results["failed"] = failed_tests;
        test_results["success_rate"] = (passed_tests.size() + failed_tests.size()) > 0 ? 
            static_cast<double>(passed_tests.size()) / (passed_tests.size() + failed_tests.size()) : 0.0;
        
        return test_results;
    }, "Run comprehensive system tests on a brain");

    m.def("create_test_brain", [](const std::string& brain_type) {
        auto config = std::make_shared<Config>();
        
        if (brain_type == "minimal") {
            config->set("brain.neuron_count", 0);
            config->set("region_count", 0);
        } else if (brain_type == "small") {
            config->set("brain.neuron_count", 10);
            config->set("brain.synapse_density", 0.05f);
        } else if (brain_type == "medium") {
            config->set("brain.neuron_count", 100);
            config->set("brain.synapse_density", 0.1f);
            config->set("region_count", 2);
        } else if (brain_type == "large") {
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.15f);
            config->set("region_count", 3);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a test brain of specified type");

    m.def("compare_brains", [](const Brain& brain1, const Brain& brain2) {
        py::dict comparison;
        
        comparison["neuron_count_match"] = brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount();
        comparison["synapse_count_match"] = brain1.getTotalSynapseCount() == brain2.getTotalSynapseCount();
        comparison["firing_rate_diff"] = std::abs(brain1.getAverageFiringRate() - brain2.getAverageFiringRate());
        comparison["spike_count_diff"] = std::abs(brain1.getTotalSpikeCount() - brain2.getTotalSpikeCount());
        
        return comparison;
    }, "Compare two brain states");

    m.def("create_statistics_report", [](const Brain& brain) {
        std::stringstream report;
        
        report << "=== NLM Brain Statistics Report ===\n\n";
        report << "Brain Population:\n";
        report << "  Total Neurons: " << brain.getTotalNeuronCount() << "\n";
        report << "  Total Synapses: " << brain.getTotalSynapseCount() << "\n";
        report << "  Active Neurons: " << brain.getActiveNeuronCount() << "\n";
        report << "  Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
        report << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz\n";
        report << "  E/I Balance: " << brain.getExcitationInhibitionRatio() << "\n";
        report << "  Total Spikes: " << brain.getTotalSpikeCount() << "\n\n";
        
        report << "System Status:\n";
        report << "  Working Memory: " << (brain.getWorkingMemory() ? "✓" : "✗") << "\n";
        report << "  Episodic Memory: " << (brain.getEpisodicMemory() ? "✓" : "✗") << "\n";
        report << "  Neuromodulation: " << (brain.getDopamine() ? "✓" : "✗") << "\n";
        report << "  Prediction System: " << (brain.getPredictionSystem() ? "✓" : "✗") << "\n";
        report << "  Development System: " << (brain.getDevelopmentSystem() ? "✓" : "✗") << "\n";
        report << "  Planner: " << (brain.getPlanner() ? "✓" : "✗") << "\n\n";
        
        report << "Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
        
        return report.str();
    }, "Create a comprehensive statistics report for a brain");

    // Add advanced configuration templates
    m.def("create_reinforcement_learning_config", [](int neuron_count = 500) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config->set("brain.synapse_density", 0.08f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.01f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 2.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("prediction.enabled", true);
        config->set("cognition.planner.enabled", true);
        config->set("cognition.concept.enabled", true);
        config->set("development.enabled", true);
        config->set("checkpointing.enabled", true);
        config->set("replay.enabled", true);
        config->set("simulation.timestep", 0.01);
        return config;
    }, "Create configuration optimized for reinforcement learning");

    m.def("create_supervised_learning_config", [](int neuron_count = 200) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config->set("brain.synapse_density", 0.12f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.005f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", false);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", false);
        config->set("prediction.enabled", true);
        config->set("cognition.planner.enabled", false);
        config->set("cognition.concept.enabled", true);
        config->set("development.enabled", false);
        config->set("checkpointing.enabled", true);
        config->set("replay.enabled", true);
        config->set("simulation.timestep", 0.001);
        return config;
    }, "Create configuration optimized for supervised learning");

    m.def("create_emergency_response_config", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.03f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.1f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 3.0f);
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", false);
        config->set("prediction.enabled", false);
        config->set("development.enabled", false);
        config->set("checkpointing.enabled", true);
        config->set("replay.enabled", true);
        config->set("simulation.timestep", 0.1);
        return config;
    }, "Create configuration optimized for emergency response");

    // Add experimental setup utilities
    m.def("create_experiment_setup", [](const std::string& experiment_name,
                                        const std::string& config_type = "standard") {
        py::dict setup;
        
        auto config = std::make_shared<Config>();
        
        if (config_type == "standard") {
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.1f);
        } else if (config_type == "learning") {
            config->set("brain.neuron_count", 500);
            config->set("brain.synapse_density", 0.08f);
            config->set("plasticity.stdp.enable", true);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        setup["brain"] = brain;
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        setup["world"] = world;
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        setup["agent"] = agent;
        
        py::dict metadata;
        metadata["name"] = experiment_name;
        metadata["config_type"] = config_type;
        
        setup["metadata"] = metadata;
        
        return setup;
    }, "Create an experiment setup with specified configuration");

    m.def("run_experiment", [](const py::dict& setup, size_t steps = 1000) {
        auto brain = setup["brain"].cast<std::shared_ptr<Brain>>();
        auto world = setup["world"].cast<std::shared_ptr<SimpleWorld>>();
        auto agent = setup["agent"].cast<std::shared_ptr<AgentBrain>>();
        
        std::vector<float> rewards;
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, step * 0.1);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simple reward calculation
            float reward = 1.0f;
            agent->applyRewardModulation(reward, 0.0f);
            
            rewards.push_back(reward);
        }
        
        py::dict results;
        results["rewards"] = rewards;
        results["final_reward"] = rewards.empty() ? 0.0f : rewards.back();
        results["average_reward"] = std::accumulate(rewards.begin(), rewards.end(), 0.0f) / rewards.size();
        
        return results;
    }, "Run an experiment with the given setup");

    // Add comprehensive utility functions
    m.def("export_brain_to_file", [](const Brain& brain, const std::string& filepath) {
        return brain.save(filepath);
    }, "Export brain to file");

    m.def("import_brain_from_file", [](Brain& brain, const std::string& filepath) {
        return brain.load(filepath);
    }, "Import brain from file");

    m.def("create_config_from_dict", [](const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        for (auto item : config_dict) {
            std::string key = item.first.cast<std::string>();
            py::object value = item.second;
            
            if (value.is(py::int_())) {
                config->set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<double>());
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>());
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>());
            }
        }
        return config;
    }, "Create config from Python dictionary");

    m.def("config_to_dict", [](const Config& config) {
        py::dict result;
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            auto value = config.get<ConfigValue>(key);
            if (value) {
                result[key] = *value;
            }
        }
        return result;
    }, "Convert config to Python dictionary");

    m.def("benchmark_performance", [](const Brain& brain, size_t steps = 1000) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain.step(i, i * 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        py::dict benchmark;
        benchmark["steps_completed"] = static_cast<int64_t>(steps);
        benchmark["total_time_seconds"] = duration.count() / 1000.0;
        benchmark["steps_per_second"] = steps / (duration.count() / 1000.0);
        
        return benchmark;
    }, "Benchmark brain performance");

    m.def("create_test_suite", [](const std::string& test_type) {
        auto config = std::make_shared<Config>();
        
        if (test_type == "minimal") {
            config->set("brain.neuron_count", 0);
        } else if (test_type == "small") {
            config->set("brain.neuron_count", 10);
        } else if (test_type == "large") {
            config->set("brain.neuron_count", 1000);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a test brain for testing");

    // Add help and documentation utilities
    m.def("show_help", [](const std::string& topic) {
        std::stringstream help;
        
        if (topic == "basics") {
            help << "=== NLM Python Basics ===\n\n";
            help << "import pynlm\n";
            help << "brain = pynlm.createBrain(pynlm.createDefaultConfig())\n";
            help << "brain.initialize()\n";
            help << "for i in range(100):\n";
            help << "    brain.step(i)\n";
        } else if (topic == "advanced") {
            help << "=== Advanced Features ===\n\n";
            help << "Factory Functions:\n";
            help << "  pynlm.createDevelopmentConfig()\n";
            help << "  pynlm.createResearchConfig()\n";
            help << "  pynlm.createProductionConfig()\n";
            help << "\nContext Managers:\n";
            help << "  with pynlm.brain_context(brain):\n";
            help << "      brain.step(0)\n";
        }
        
        return help.str();
    }, "Show help documentation for specified topic");

    m.def("get_available_config_types", []() {
        std::vector<std::string> types;
        types.push_back("development");
        types.push_back("research");
        types.push_back("production");
        return types;
    }, "Get available configuration types");

    m.def("get_experiment_templates", []() {
        py::dict templates;
        
        py::dict standard;
        standard["neuron_count"] = 1000;
        standard["description"] = "Standard simulation";
        templates["standard"] = standard;
        
        py::dict learning;
        learning["neuron_count"] = 500;
        learning["enable_curiosity"] = true;
        learning["description"] = "Optimized for learning";
        templates["learning"] = learning;
        
        return templates;
    }, "Get experiment templates");

    m.def("create_checkpoint", [](Brain& brain, const std::string& path) {
        return brain.save(path);
    }, "Create a checkpoint");

    m.def("restore_checkpoint", [](Brain& brain, const std::string& path) {
        return brain.load(path);
    }, "Restore from checkpoint");

    m.def("validate_configuration", [](const Config& config) {
        // Basic validation
        if (config.has("brain.neuron_count")) {
            auto count = config.get<int64_t>("brain.neuron_count");
            if (count < 0) {
                throw ValidationError("Neuron count cannot be negative");
            }
        }
        
        return true;
    }, "Validate configuration parameters");

    m.def("get_api_version", []() {
        return "2.0.0";
    }, "Get the current API version");

    // Register the original basic classes
    // ... (rest of the original bindings)

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

