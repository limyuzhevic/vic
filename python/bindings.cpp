#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

        .def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
            auto config = std::make_shared<Config>();
            // Set developmental parameters
            config->set("brain.development.enabled", true);
            config->set("brain.plasticity.developmental_maturation", true);
            config->set("brain.development.critical_period_duration", 300.0);
            config->set("brain.development.maturation_threshold", 900.0);
            return config;
        }, "Create a developmental configuration")
        
        .def("createLearningConfig", []() -> std::shared_ptr<Config> {
            auto config = std::make_shared<Config>();
            // Set learning parameters
            config->set("brain.plasticity.stdp.enable", true);
            config->set("brain.plasticity.hebbian.enable", true);
            config->set("brain.plasticity.stdp.learning_rate", 0.001f);
            config->set("brain.plasticity.hebbian.learning_rate", 0.0005f);
            return config;
        }, "Create a learning-focused configuration")
        
        .def("createSensoryConfig", [](size_t visionWidth, size_t visionHeight) -> std::shared_ptr<Config> {
            auto config = std::make_shared<Config>();
            config->set("sensory.vision.width", static_cast<int64_t>(visionWidth));
            config->set("sensory.vision.height", static_cast<int64_t>(visionHeight));
            config->set("sensory.touch.count", 8);
            config->set("sensory.internal.count", 4);
            config->set("sensory.proprioception.count", 6);
            return config;
        }, py::arg("visionWidth"), py::arg("visionHeight"),
           "Create a sensory configuration with specified vision dimensions")
        
        .def("createAdvancedConfig", []() -> std::shared_ptr<Config> {
            auto config = std::make_shared<Config>();
            // Set advanced parameters for research/experiment
            config->set("brain.plasticity.structural.enable", true);
            config->set("brain.neuromodulation.dopamine.scale", 1.0f);
            config->set("brain.neuromodulation.curiosity.enable", true);
            config->set("brain.neuromodulation.novelty.enable", true);
            config->set("brain.prediction.enabled", true);
            config->set("brain.cognition.planner.enabled", true);
            config->set("brain.memory.working.enabled", true);
            config->set("brain.memory.episodic.enabled", true);
            config->set("brain.memory.associative.enabled", true);
            return config;
        }, "Create an advanced research configuration with all subsystems enabled")
        
        // Brain configuration convenience methods
        .def("initializeBrain", [](Brain& brain) {
            return brain.initialize();
        }, "Initialize the brain")
        
        .def("runBrainLoop", [](Brain& brain, size_t numSteps, double timestep) {
            for (size_t i = 0; i < numSteps; ++i) {
                brain.step(static_cast<SimulationStep>(i), static_cast<Timestamp>(i * timestep));
            }
        }, py::arg("numSteps"), py::arg("timestep"),
           "Run brain simulation loop for specified steps with timestep")
        
        .def("createAgentWithWorld", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            return agent;
        }, py::arg("brain"), py::arg("world"),
           "Create an agent brain with world integration")
        
        // Status and statistics
        .def("getBrainStatus", [](const Brain& brain) {
            std::map<std::string, std::string> status;
            status["neurons"] = std::to_string(brain.getTotalNeuronCount());
            status["synapses"] = std::to_string(brain.getTotalSynapseCount());
            status["active_neurons"] = std::to_string(brain.getActiveNeuronCount());
            status["firing_neurons"] = std::to_string(brain.getFiringNeuronCount());
            status["total_spikes"] = std::to_string(brain.getTotalSpikeCount());
            status["avg_firing_rate"] = std::to_string(brain.getAverageFiringRate());
            status["e_i_ratio"] = std::to_string(brain.getExcitationInhibitionRatio());
            status["dev_stage"] = std::to_string(static_cast<int>(brain.getDevelopmentalStage()));
            return status;
        }, "Get comprehensive brain status as dictionary")
        
        .def("createSimulationSetup", [](std::shared_ptr<Config> config, std::shared_ptr<Brain> brain,
                                        std::shared_ptr<SimpleWorld> world) {
            std::map<std::string, std::shared_ptr<void>> setup;
            setup["config"] = config;
            setup["brain"] = brain;
            setup["world"] = world;
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            setup["agent"] = agent;
            
            return setup;
        }, py::arg("config"), py::arg("brain"), py::arg("world"),
           "Create complete simulation setup with all components")
        
        // Utility functions
        .def("createDefaultWorld", []() {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(20, 20, 8, 8);
            return world;
        }, "Create a default world configuration")
        
        .def("setupLearningEnvironment", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            
            // Enable learning features
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
            
            return std::make_tuple(brain, world, agent);
        }, py::arg("brain"), py::arg("world"),
           "Setup a learning environment with all features enabled")
        
        .def("createDevelopmentStage", []() {
            std::map<std::string, double> stages;
            stages["initial"] = 0.0;
            stages["critical_period"] = 60.0;
            stages["maturation"] = 300.0;
            stages["adult"] = 900.0;
            return stages;
        }, "Get developmental stage timing thresholds")
        
        // Statistics helpers
        .def("getNetworkStats", [](const Brain& brain) {
            std::map<std::string, double> stats;
            
            // Connectivity statistics
            size_t totalNeurons = brain.getTotalNeuronCount();
            size_t totalSynapses = brain.getTotalSynapseCount();
            if (totalNeurons > 0) {
                stats["connection_density"] = static_cast<double>(totalSynapses) / (totalNeurons * totalNeurons);
            }
            
            // Activity statistics
            stats["activity_level"] = static_cast<double>(brain.getFiringNeuronCount()) / std::max(1.0, static_cast<double>(totalNeurons));
            stats["spike_rate"] = brain.getAverageFiringRate();
            
            // Balance statistics
            stats["e_i_balance"] = brain.getExcitationInhibitionRatio();
            
            return stats;
        }, "Get comprehensive network statistics")
        
        // Error handling and validation
        .def("validateConfig", [](const Config& config) {
            std::vector<std::string> errors;
            
            // Check for required parameters
            if (!config.has("brain.neuron_count")) {
                errors.push_back("Missing required parameter: brain.neuron_count");
            }
            
            if (!config.has("brain.region_count")) {
                errors.push_back("Missing required parameter: brain.region_count");
            }
            
            // Validate parameter ranges
            auto neuronCount = config.getOr<int64_t>("brain.neuron_count", -1);
            if (neuronCount < 0) {
                errors.push_back("brain.neuron_count must be non-negative");
            }
            
            auto regionCount = config.getOr<int64_t>("brain.region_count", -1);
            if (regionCount < 0) {
                errors.push_back("brain.region_count must be non-negative");
            }
            
            return errors;
        }, "Validate configuration and return list of errors")
        
        .def("getConfigSummary", [](const Config& config) {
            std::string summary = config.summary();
            return summary;
        }, "Get configuration summary as string")
        
        // Advanced operations
        .def("addRegionWithConnections", [](Brain& brain, const std::string& name, size_t neuronCount,
                                            float connectionProbability) {
            RegionId regionId = brain.addRegion(name);
            auto* region = brain.getRegion(regionId);
            if (region) {
                // In a real implementation, you would add neurons and connections here
                // For now, this is a placeholder for advanced region creation
            }
            return regionId;
        }, py::arg("name"), py::arg("neuronCount"), py::arg("connectionProbability"),
           "Add a neural region with specified size and connectivity")
        
        .def("createBrainNetwork", [](std::shared_ptr<Config> config, size_t numRegions) {
            auto brain = std::make_shared<Brain>(config);
            
            if (brain->initialize()) {
                // Add multiple regions
                for (size_t i = 0; i < numRegions; ++i) {
                    brain->addRegion("Region_" + std::to_string(i));
                }
            }
            
            return brain;
        }, py::arg("config"), py::arg("numRegions"),
           "Create a multi-region brain network")
        
        .def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                             std::shared_ptr<AgentBrain> agent, size_t numSteps) {
            // Episode simulation
            for (size_t step = 0; step < numSteps; ++step) {
                // Update world
                world->update(0.1);
                
                // Get perception
                auto percept = world->getSensoryPercept();
                
                // Process in agent
                agent->processSensoryInput(percept);
                
                // Brain computation
                brain->step(static_cast<SimulationStep>(step));
                
                // Motor action
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Development update
                agent->updateDevelopment(0.1);
                
                // Reward modulation (simplified)
                float reward = 0.0f;
                if (world->getSimulationTime() % 10.0 == 0.0) {
                    reward = 1.0f; // Simple reward every 10 time units
                }
                agent->applyRewardModulation(reward, 0.0f);
            }
            
            return std::make_tuple(brain, world, agent);
        }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
           "Run a complete simulation episode")
        
        .def("createLearningSystem", []() {
            std::map<std::string, std::function<void()>> system;
            
            system["initialize"] = []() {
                // Placeholder for learning system initialization
            };
            
            system["train"] = []() {
                // Placeholder for training
            };
            
            system["evaluate"] = []() {
                // Placeholder for evaluation
            };
            
            return system;
        }, "Create a learning system API")
        
        .def("exportBrainState", [](const Brain& brain, const std::string& filename) {
            brain.save(filename);
        }, py::arg("filename"),
           "Export brain state to file")
        
        .def("importBrainState", [](Brain& brain, const std::string& filename) {
            return brain.load(filename);
        }, py::arg("filename"),
           "Import brain state from file")
        
        .def("createCheckpointSystem", []() {
            std::map<std::string, std::function<bool(const std::string&)>> checkpoint;
            
            checkpoint["save"] = [](const std::string& filename) {
                // Placeholder for checkpoint save
                return true;
            };
            
            checkpoint["load"] = [](const std::string& filename) {
                // Placeholder for checkpoint load
                return true;
            };
            
            return checkpoint;
        }, "Create a checkpoint system API")
        
        // Error types
        .def("raiseError", [](const std::string& message) {
            throw std::runtime_error(message);
        }, py::arg("message"),
           "Raise a runtime error with given message")
        
        .def("assertCondition", [](bool condition, const std::string& message) {
            if (!condition) {
                throw std::runtime_error(message);
            }
            return true;
        }, py::arg("condition"), py::arg("message"),
           "Assert condition and raise error if false")
        
        // Version and info
        .def("getVersion", []() {
            return "pynlm 0.1.0 - NLM Python Bindings";
        }, "Get library version")
        
        .def("getInfo", []() {
            return std::map<std::string, std::string>{
                {"name", "NLM - Neural Learning Machine"},
                {"description", "Python bindings for NLM neural simulation framework"},
                {"version", "0.1.0"},
                {"author", "NLM Authors"},
                {"features", "Spiking neural networks, plasticity, neuromodulation, learning"}
            };
        }, "Get library information")
        
        .def("help", []() {
            return std::string(
                "NLM Python Bindings Help:\n"
                "=======================\n"
                "Basic Classes:\n"
                "  - Config: Configuration management\n"
                "  - Brain: Neural simulation core\n"
                "  - AgentBrain: Agent-world interface\n"
                "  - SimpleWorld: Environment simulation\n"
                "  - Vision, Audio, InternalSignals: Sensory inputs\n"
                "  - Action: Motor outputs\n"
                "\nKey Methods:\n"
                "  - brain.initialize(): Initialize brain\n"
                "  - brain.step(step, time): Advance simulation\n"
                "  - agent.processSensoryInput(percept): Process world input\n"
                "  - agent.decodeMotorCommand(): Generate action\n"
                "\nFactory Methods:\n"
                "  - createDefaultConfig(): Create basic config\n"
                "  - createBrain(config): Create neural network\n"
                "  - createSimpleWorld(): Create environment\n"
                "  - createAgentBrain(brain): Create agent interface\n"
                "\nConfiguration Types:\n"
                "  - createDevelopmentConfig(): Developmental brain\n"
                "  - createLearningConfig(): Learning-focused\n"
                "  - createAdvancedConfig(): Full feature set\n"
                "\nExample Usage:\n"
                "  import pynlm\n"
                "  config = pynlm.createDefaultConfig()\n"
                "  brain = pynlm.createBrain(config)\n"
                "  brain.initialize()\n"
                "  for i in range(100):\n"
                "      brain.step(i)\n"
            );
        }, "Get comprehensive help information");

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

        .def("enableContextAwareness", &AgentBrain::enableContextAwareness,
             py::arg("enable"))
        .def("enableMetaLearning", &AgentBrain::enableMetaLearning,
             py::arg("enable"))
        .def("enableNeuralCompression", &AgentBrain::enableNeuralCompression,
             py::arg("enable"))
        .def("isContextAware", &AgentBrain::isContextAware)
        .def("isMetaLearningEnabled", &AgentBrain::isMetaLearningEnabled)
        .def("isNeuralCompressionEnabled", &AgentBrain::isNeuralCompressionEnabled);

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

    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set developmental parameters
        config->set("brain.development.enabled", true);
        config->set("brain.plasticity.developmental_maturation", true);
        config->set("brain.development.critical_period_duration", 300.0);
        config->set("brain.development.maturation_threshold", 900.0);
        return config;
    }, "Create a developmental configuration");

    m.def("createLearningConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set learning parameters
        config->set("brain.plasticity.stdp.enable", true);
        config->set("brain.plasticity.hebbian.enable", true);
        config->set("brain.plasticity.stdp.learning_rate", 0.001f);
        config->set("brain.plasticity.hebbian.learning_rate", 0.0005f);
        return config;
    }, "Create a learning-focused configuration");

    m.def("createSensoryConfig", [](size_t visionWidth, size_t visionHeight) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("sensory.vision.width", static_cast<int64_t>(visionWidth));
        config->set("sensory.vision.height", static_cast<int64_t>(visionHeight));
        config->set("sensory.touch.count", 8);
        config->set("sensory.internal.count", 4);
        config->set("sensory.proprioception.count", 6);
        return config;
    }, py::arg("visionWidth"), py::arg("visionHeight"),
       "Create a sensory configuration with specified vision dimensions");

    m.def("createAdvancedConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set advanced parameters for research/experiment
        config->set("brain.plasticity.structural.enable", true);
        config->set("brain.neuromodulation.dopamine.scale", 1.0f);
        config->set("brain.neuromodulation.curiosity.enable", true);
        config->set("brain.neuromodulation.novelty.enable", true);
        config->set("brain.prediction.enabled", true);
        config->set("brain.cognition.planner.enabled", true);
        config->set("brain.memory.working.enabled", true);
        config->set("brain.memory.episodic.enabled", true);
        config->set("brain.memory.associative.enabled", true);
        return config;
    }, "Create an advanced research configuration with all subsystems enabled");

    m.def("createDefaultWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        return world;
    }, "Create a default world configuration");

    m.def("setupLearningEnvironment", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable learning features
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("brain"), py::arg("world"),
       "Setup a learning environment with all features enabled");

    m.def("createDevelopmentStage", []() {
        std::map<std::string, double> stages;
        stages["initial"] = 0.0;
        stages["critical_period"] = 60.0;
        stages["maturation"] = 300.0;
        stages["adult"] = 900.0;
        return stages;
    }, "Get developmental stage timing thresholds");

    m.def("getNetworkStats", [](const Brain& brain) {
        std::map<std::string, double> stats;
        
        // Connectivity statistics
        size_t totalNeurons = brain.getTotalNeuronCount();
        size_t totalSynapses = brain.getTotalSynapseCount();
        if (totalNeurons > 0) {
            stats["connection_density"] = static_cast<double>(totalSynapses) / (totalNeurons * totalNeurons);
        }
        
        // Activity statistics
        stats["activity_level"] = static_cast<double>(brain.getFiringNeuronCount()) / std::max(1.0, static_cast<double>(totalNeurons));
        stats["spike_rate"] = brain.getAverageFiringRate();
        
        // Balance statistics
        stats["e_i_balance"] = brain.getExcitationInhibitionRatio();
        
        return stats;
    }, "Get comprehensive network statistics");

    m.def("validateConfig", [](const Config& config) {
        std::vector<std::string> errors;
        
        // Check for required parameters
        if (!config.has("brain.neuron_count")) {
            errors.push_back("Missing required parameter: brain.neuron_count");
        }
        
        if (!config.has("brain.region_count")) {
            errors.push_back("Missing required parameter: brain.region_count");
        }
        
        // Validate parameter ranges
        auto neuronCount = config.getOr<int64_t>("brain.neuron_count", -1);
        if (neuronCount < 0) {
            errors.push_back("brain.neuron_count must be non-negative");
        }
        
        auto regionCount = config.getOr<int64_t>("brain.region_count", -1);
        if (regionCount < 0) {
            errors.push_back("brain.region_count must be non-negative");
        }
        
        return errors;
    }, "Validate configuration and return list of errors");

    m.def("getConfigSummary", [](const Config& config) {
        std::string summary = config.summary();
        return summary;
    }, "Get configuration summary as string");

    m.def("addRegionWithConnections", [](Brain& brain, const std::string& name, size_t neuronCount,
                                        float connectionProbability) {
        RegionId regionId = brain.addRegion(name);
        auto* region = brain.getRegion(regionId);
        if (region) {
            // In a real implementation, you would add neurons and connections here
            // For now, this is a placeholder for advanced region creation
        }
        return regionId;
    }, py::arg("name"), py::arg("neuronCount"), py::arg("connectionProbability"),
       "Add a neural region with specified size and connectivity");

    m.def("createBrainNetwork", [](std::shared_ptr<Config> config, size_t numRegions) {
        auto brain = std::make_shared<Brain>(config);
        
        if (brain->initialize()) {
            // Add multiple regions
            for (size_t i = 0; i < numRegions; ++i) {
                brain->addRegion("Region_" + std::to_string(i));
            }
        }
        
        return brain;
    }, py::arg("config"), py::arg("numRegions"),
       "Create a multi-region brain network");

    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                         std::shared_ptr<AgentBrain> agent, size_t numSteps) {
        // Episode simulation
        for (size_t step = 0; step < numSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get perception
            auto percept = world->getSensoryPercept();
            
            // Process in agent
            agent->processSensoryInput(percept);
            
            // Brain computation
            brain->step(static_cast<SimulationStep>(step));
            
            // Motor action
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Development update
            agent->updateDevelopment(0.1);
            
            // Reward modulation (simplified)
            float reward = 0.0f;
            if (world->getSimulationTime() % 10.0 == 0.0) {
                reward = 1.0f; // Simple reward every 10 time units
            }
            agent->applyRewardModulation(reward, 0.0f);
        }
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
       "Run a complete simulation episode");

    m.def("createLearningSystem", []() {
        std::map<std::string, std::function<void()>> system;
        
        system["initialize"] = []() {
            // Placeholder for learning system initialization
        };
        
        system["train"] = []() {
            // Placeholder for training
        };
        
        system["evaluate"] = []() {
            // Placeholder for evaluation
        };
        
        return system;
    }, "Create a learning system API");

    m.def("exportBrainState", [](const Brain& brain, const std::string& filename) {
        brain.save(filename);
    }, py::arg("filename"),
       "Export brain state to file");

    m.def("importBrainState", [](Brain& brain, const std::string& filename) {
        return brain.load(filename);
    }, py::arg("filename"),
       "Import brain state from file");

    m.def("createCheckpointSystem", []() {
        std::map<std::string, std::function<bool(const std::string&)>> checkpoint;
        
        checkpoint["save"] = [](const std::string& filename) {
            // Placeholder for checkpoint save
            return true;
        };
        
        checkpoint["load"] = [](const std::string& filename) {
            // Placeholder for checkpoint load
            return true;
        };
        
        return checkpoint;
    }, "Create a checkpoint system API");

    m.def("raiseError", [](const std::string& message) {
        throw std::runtime_error(message);
    }, py::arg("message"),
       "Raise a runtime error with given message");

    m.def("assertCondition", [](bool condition, const std::string& message) {
        if (!condition) {
            throw std::runtime_error(message);
        }
        return true;
    }, py::arg("condition"), py::arg("message"),
       "Assert condition and raise error if false");

    m.def("getVersion", []() {
        return "pynlm 0.1.0 - NLM Python Bindings";
    }, "Get library version");

    m.def("getInfo", []() {
        return std::map<std::string, std::string>{
            {"name", "NLM - Neural Learning Machine"},
            {"description", "Python bindings for NLM neural simulation framework"},
            {"version", "0.1.0"},
            {"author", "NLM Authors"},
            {"features", "Spiking neural networks, plasticity, neuromodulation, learning"}
        };
    }, "Get library information");

    m.def("help", []() {
        return std::string(
            "NLM Python Bindings Help:\n"
            "=======================\n"
            "Basic Classes:\n"
            "  - Config: Configuration management\n"
            "  - Brain: Neural simulation core\n"
            "  - AgentBrain: Agent-world interface\n"
            "  - SimpleWorld: Environment simulation\n"
            "  - Vision, Audio, InternalSignals: Sensory inputs\n"
            "  - Action: Motor outputs\n"
            "\nKey Methods:\n"
            "  - brain.initialize(): Initialize brain\n"
            "  - brain.step(step, time): Advance simulation\n"
            "  - agent.processSensoryInput(percept): Process world input\n"
            "  - agent.decodeMotorCommand(): Generate action\n"
            "\nFactory Methods:\n"
            "  - createDefaultConfig(): Create basic config\n"
            "  - createBrain(config): Create neural network\n"
            "  - createSimpleWorld(): Create environment\n"
            "  - createAgentBrain(brain): Create agent interface\n"
            "\nConfiguration Types:\n"
            "  - createDevelopmentConfig(): Developmental brain\n"
            "  - createLearningConfig(): Learning-focused\n"
            "  - createAdvancedConfig(): Full feature set\n"
            "\nExample Usage:\n"
            "  import pynlm\n"
            "  config = pynlm.createDefaultConfig()\n"
            "  brain = pynlm.createBrain(config)\n"
            "  brain.initialize()\n"
            "  for i in range(100):\n"
            "      brain.step(i)\n"
        );
    }, "Get comprehensive help information");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
