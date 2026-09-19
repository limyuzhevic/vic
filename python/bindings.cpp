#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <tuple>

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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    py::register_exception<std::logic_error>(m, "LogicError");
    
    // ============ QUICK START FUNCTIONS FOR BEGINNERS ============
    
    m.def("quickStartSimulation", []() {
        // One-line complete setup for beginners
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "One-line quick start for beginners - creates complete working simulation");
    
    m.def("quickStartLearning", []() {
        // Pre-configured for learning scenarios
        auto config = std::make_shared<Config>();
        
        // Set learning-friendly defaults
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Enable learning features
        config->set("enable_reward_modulation", true, ConfigSource::Default);
        config->set("enable_curiosity", true, ConfigSource::Default);
        config->set("enable_development", true, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(15, 15, 6, 6);
        world->setMaxEnergy(100.0f);
        world->setEnergyDecayRate(0.01f);
        world->reset();
        
        // Add some learning objects
        WorldObject resource(10.0f, 10.0f, WorldObjectType::Resource, 50.0f);
        world->addObject(resource);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable learning features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        return std::make_tuple(brain, world, agent);
    }, "Quick start optimized for learning scenarios");
    
    m.def("quickStartExploration", []() {
        // Pre-configured for exploration and discovery
        auto config = std::make_shared<Config>();
        
        config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        config->set("enable_structural_plasticity", true, ConfigSource::Default);
        config->set("enable_reward_modulation", true, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(25, 25, 8, 8);
        world->setMaxEnergy(200.0f);
        world->setEnergyDecayRate(0.005f);
        world->setRandomSeed(123);
        world->reset();
        
        // Add more complex environment
        WorldObject hazard(5.0f, 5.0f, WorldObjectType::Hazard, 10.0f);
        world->addObject(hazard);
        WorldObject goal(20.0f, 20.0f, WorldObjectType::Marker, 100.0f);
        world->addObject(goal);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        agent->enableStructuralPlasticity(true);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, world, agent);
    }, "Quick start optimized for exploration scenarios");
    
    // ============ BATCH OPERATIONS ============
    
    m.def("setConfigBatch", [](Config& self, const py::dict& values) {
        for (auto item : values) {
            std::string key = item.first.cast<std::string>();
            ConfigValue value;
            
            if (py::isinstance<int>(item.second)) {
                value = item.second.cast<int>();
            } else if (py::isinstance<float>(item.second)) {
                value = item.second.cast<double>();
            } else if (py::isinstance<bool>(item.second)) {
                value = item.second.cast<bool>();
            } else {
                value = item.second.cast<std::string>();
            }
            self.set(key, value);
        }
        return &self;  // Return reference for chaining
    }, py::arg("values"), "Set multiple config values at once from a dictionary",
        py::return_value_policy::reference);
    
    m.def("configureWorldForSimulation", [](SimpleWorld& self, size_t width, size_t height,
                                           size_t visionWidth, size_t visionHeight,
                                           float maxEnergy, float energyDecayRate,
                                           uint64_t seed) {
        self.configure(width, height, visionWidth, visionHeight);
        if (maxEnergy > 0.0f) self.setMaxEnergy(maxEnergy);
        if (energyDecayRate > 0.0f) self.setEnergyDecayRate(energyDecayRate);
        if (seed > 0) self.setRandomSeed(seed);
        return &self;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
        py::arg("maxEnergy") = 100.0f, py::arg("energyDecayRate") = 0.01f,
        py::arg("seed") = 0, "Configure world with multiple parameters at once",
        py::return_value_policy::reference);
    
    m.def("initializeAgentComplete", [](AgentBrain& self, const SimpleWorld& world) {
        self.initialize(world);
        return &self;
    }, py::arg("world"), "Initialize agent with world and return reference for chaining",
        py::return_value_policy::reference);
    
    // ============ PRESET CONFIGURATIONS ============
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default empty configuration");
    
    m.def("createDebugConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("debug", true);
        config->set("logging_level", std::string("debug"));
        config->set("max_neurons", static_cast<int64_t>(1000));
        config->set("simulation_timestep", 0.001);
        config->set("enable_validation", true);
        return config;
    }, "Create debug-friendly configuration with verbose logging");
    
    m.def("createProductionConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("debug", false);
        config->set("logging_level", std::string("info"));
        config->set("max_neurons", static_cast<int64_t>(100000));
        config->set("simulation_timestep", 0.001);
        config->set("enable_validation", false);
        config->set("checkpoint_interval", 10000);
        return config;
    }, "Create production-optimized configuration with performance focus");
    
    m.def("createLearningConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Enable all learning features
        config->set("enable_reward_modulation", true, ConfigSource::Default);
        config->set("enable_structural_plasticity", true, ConfigSource::Default);
        config->set("enable_curiosity", true, ConfigSource::Default);
        config->set("enable_development", true, ConfigSource::Default);
        config->set("enable_prediction", true, ConfigSource::Default);
        
        return config;
    }, "Create configuration optimized for learning and development");
    
    // ============ VALIDATION AND UTILITIES ============
    
    m.def("validateConfig", [](const Config& config) {
        std::vector<std::string> errors;
        
        // Basic validation checks
        if (config.has("max_neurons") && config.getOr<int64_t>("max_neurons", 0) <= 0) {
            errors.push_back("max_neurons must be positive");
        }
        if (config.has("max_synapses") && config.getOr<int64_t>("max_synapses", 0) <= 0) {
            errors.push_back("max_synapses must be positive");
        }
        if (config.has("simulation_timestep") && config.getOr<double>("simulation_timestep", 0.0) <= 0.0) {
            errors.push_back("simulation_timestep must be positive");
        }
        
        if (!errors.empty()) {
            throw py::value_error("Configuration validation failed: " + 
                                joinErrors(errors));
        }
        return true;
    }, py::arg("config"), "Validate configuration and raise error if invalid");
    
    m.def("configSummary", [](const Config& config) {
        return config.summary();
    }, py::arg("config"), "Get a human-readable summary of configuration");
    
    m.def("validateSimulationSetup", [](Brain& brain, SimpleWorld& world, AgentBrain& agent) {
        std::vector<std::string> errors;
        
        // Check brain initialization
        if (brain.getTotalNeuronCount() == 0) {
            errors.push_back("Brain has no neurons - initialize first");
        }
        
        // Check world setup
        if (world.getWidth() == 0 || world.getHeight() == 0) {
            errors.push_back("World has invalid dimensions");
        }
        
        // Check agent connection
        if (agent.getBrain() != &brain) {
            errors.push_back("Agent not properly connected to brain");
        }
        
        if (!errors.empty()) {
            throw py::value_error("Simulation setup validation failed: " + 
                                joinErrors(errors));
        }
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
        "Validate complete simulation setup and raise error if invalid");
    
    // ============ SIMULATION HELPERS ============
    
    m.def("runEpisode", [](Brain& brain, SimpleWorld& world, AgentBrain& agent, int steps) {
        world.reset();
        for (int i = 0; i < steps; ++i) {
            world.update(0.1);
            agent.processSensoryInput(world.getSensoryPercept());
            brain.step(i);
            auto action = agent.decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
        }
        
        // Return episode statistics
        py::dict stats;
        stats["spikes"] = brain.getTotalSpikeCount();
        stats["firing_rate"] = brain.getAverageFiringRate();
        stats["steps"] = steps;
        stats["reward_modulation"] = agent.getNeuromodulationLevel();
        stats["curiosity"] = agent.getCuriosityLevel();
        return stats;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
        "Run a complete simulation episode and return statistics");
    
    m.def("runEpisodeWithLogging", [](Brain& brain, SimpleWorld& world, AgentBrain& agent, int steps,
                                     bool verbose = false) {
        world.reset();
        for (int i = 0; i < steps; ++i) {
            world.update(0.1);
            agent.processSensoryInput(world.getSensoryPercept());
            brain.step(i);
            
            if (verbose && i % 10 == 0) {
                brain.logStatus();
            }
            
            auto action = agent.decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
        }
        
        py::dict stats;
        stats["spikes"] = brain.getTotalSpikeCount();
        stats["firing_rate"] = brain.getAverageFiringRate();
        stats["steps"] = steps;
        stats["reward_modulation"] = agent.getNeuromodulationLevel();
        stats["curiosity"] = agent.getCuriosityLevel();
        stats["neuromodulation_level"] = agent.getNeuromodulationLevel();
        stats["novelty_level"] = agent.getNoveltyLevel();
        stats["prediction_error"] = agent.getPredictionError();
        return stats;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
        py::arg("verbose") = false,
        "Run episode with optional verbose logging and comprehensive statistics");
    
    // ============ COMPLETE SETUP HELPERS ============
    
    m.def("setupCompleteAgent", [](Brain& brain, SimpleWorld& world) {
        world.configure(10, 10, 8, 8);
        world.reset();
        
        auto agent = std::make_shared<AgentBrain>(&brain);
        agent->initialize(world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("brain"), 
        "Setup complete agent simulation with world and agent");
    
    m.def("setupCompleteAgentWithConfig", [](Config& config) {
        auto brain = std::make_shared<Brain>(std::make_shared<Config>(config));
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(15, 15, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("config"), 
        "Setup complete agent simulation with custom configuration");
    
    // ============ ADVANCED HELPERS ============
    
    // ============ ADVANCED HELPERS ============
    
    m.def("getLearningStats", [](Brain& brain, AgentBrain& agent) {
        py::dict stats;
        
        stats["total_spikes"] = brain.getTotalSpikeCount();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["excitatory_inhibitory_ratio"] = brain.getExcitationInhibitionRatio();
        stats["active_neurons"] = brain.getActiveNeuronCount();
        stats["region_count"] = brain.getRegionCount();
        
        stats["dopamine_level"] = agent.getNeuromodulationLevel();
        stats["curiosity_level"] = agent.getCuriosityLevel();
        stats["novelty_level"] = agent.getNoveltyLevel();
        stats["prediction_error"] = agent.getPredictionError();
        stats["developmental_stage"] = static_cast<int>(agent.getDevelopmentalStage());
        
        // Calculate learning indicators
        float spikeRate = brain.getAverageFiringRate();
        stats["learning_active"] = (spikeRate > 0.1f);
        stats["novelty_significant"] = (agent.getNoveltyLevel() > 0.5f);
        stats["curiosity_high"] = (agent.getCuriosityLevel() > 0.5f);
        
        return stats;
    }, py::arg("brain"), py::arg("agent"), "Get comprehensive learning and performance statistics");
    
    m.def("enableAllLearningFeatures", [](AgentBrain& agent) {
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(true);
        agent.enableDevelopment(true);
        agent.enableCuriosity(true);
        return &agent;
    }, py::arg("agent"), "Enable all learning features on agent",
        py::return_value_policy::reference);
    
    m.def("configureWorldForLearning", [](SimpleWorld& world, size_t width, size_t height,
                                         size_t visionWidth, size_t visionHeight) {
        world.configure(width, height, visionWidth, visionHeight);
        
        // Add learning-friendly defaults
        world.setMaxEnergy(100.0f);
        world.setEnergyDecayRate(0.01f);
        
        return &world;
    }, py::arg("world"), py::arg("width"), py::arg("height"), 
        py::arg("visionWidth"), py::arg("visionHeight"),
        "Configure world with learning-friendly defaults",
        py::return_value_policy::reference);
    
    m.def("safeInitializeWithValidation", [](Brain& brain) {
        try {
            bool success = brain.initialize();
            if (!success) {
                throw py::value_error("Brain initialization failed");
            }
            
            // Validate brain state
            if (brain.getTotalNeuronCount() == 0) {
                throw py::value_error("Brain created but contains no neurons");
            }
            
            return true;
        } catch (const std::exception& e) {
            throw py::value_error(std::string("Safe initialization failed: ") + e.what());
        }
    }, py::arg("brain"), "Safely initialize brain with validation and error handling");
    
    m.def("safeWorldSetupWithValidation", [](SimpleWorld& world, size_t width, size_t height,
                                            size_t visionWidth, size_t visionHeight) {
        try {
            if (width == 0 || height == 0) {
                throw py::value_error("Invalid world dimensions");
            }
            if (visionWidth == 0 || visionHeight == 0) {
                throw py::value_error("Invalid vision dimensions");
            }
            
            world.configure(width, height, visionWidth, visionHeight);
            world.reset();
            
            return true;
        } catch (const std::exception& e) {
            throw py::value_error(std::string("World setup failed: ") + e.what());
        }
    }, py::arg("world"), py::arg("width"), py::arg("height"), 
        py::arg("visionWidth"), py::arg("visionHeight"),
        "Setup world with comprehensive validation");
    
    m.def("completeLearningSetup", [](Config& config) {
        // Create brain
        auto brain = std::make_shared<Brain>(std::make_shared<Config>(config));
        
        // Initialize with error handling
        if (!brain->initialize()) {
            throw py::value_error("Brain initialization failed");
        }
        
        // Create and configure world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(15, 15, 6, 6);
        world->setMaxEnergy(100.0f);
        world->setEnergyDecayRate(0.01f);
        world->reset();
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all learning features
        agent->enableAllLearningFeatures();
        
        // Validate complete setup
        if (!validateCompleteSetup(*brain, *world, *agent)) {
            throw py::value_error("Learning setup validation failed");
        }
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("config"), 
        "Complete learning setup with validation and error handling");
    
    // ============ MONITORING AND DEBUGGING ============
    
    m.def("getSystemStatus", [](Brain& brain, SimpleWorld& world, AgentBrain& agent) {
        py::dict status;
        
        // Brain status
        status["neurons"] = brain.getTotalNeuronCount();
        status["synapses"] = brain.getTotalSynapseCount();
        status["spikes"] = brain.getTotalSpikeCount();
        status["firing_rate"] = brain.getAverageFiringRate();
        status["pending_events"] = brain.getPendingSpikeEventCount();
        
        // World status
        status["world_width"] = world.getWidth();
        status["world_height"] = world.getHeight();
        status["simulation_time"] = world.getSimulationTime();
        status["agent_energy"] = world.getAgentBody().energy;
        
        // Agent status
        status["dopamine"] = agent.getNeuromodulationLevel();
        status["curiosity"] = agent.getCuriosityLevel();
        status["novelty"] = agent.getNoveltyLevel();
        status["prediction_error"] = agent.getPredictionError();
        status["development_stage"] = static_cast<int>(agent.getDevelopmentalStage());
        
        return status;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
        "Get comprehensive system status as a dictionary");
    
    m.def("getLearningStats", [](Brain& brain, AgentBrain& agent) {
        py::dict stats;
        
        stats["total_spikes"] = brain.getTotalSpikeCount();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["excitatory_inhibitory_ratio"] = brain.getExcitationInhibitionRatio();
        
        stats["dopamine_level"] = agent.getNeuromodulationLevel();
        stats["curiosity_level"] = agent.getCuriosityLevel();
        stats["novelty_level"] = agent.getNoveltyLevel();
        stats["prediction_error"] = agent.getPredictionError();
        stats["developmental_stage"] = static_cast<int>(agent.getDevelopmentalStage());
        
        return stats;
    }, py::arg("brain"), py::arg("agent"), "Get learning and performance statistics");
    
    // ============ CREATION HELPERS ============
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface");
    
    m.def("createSimulation", [](std::shared_ptr<Config> config = nullptr) {
        auto brainConfig = config ? config : std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(brainConfig);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("config") = nullptr, "Create a complete simulation with sensible defaults");

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

// Helper function to join error messages
inline std::string joinErrors(const std::vector<std::string>& errors) {
    std::string result;
    for (size_t i = 0; i < errors.size(); ++i) {
        if (i > 0) result += "; ";
        result += errors[i];
    }
    return result;
}

} // namespace nlm
