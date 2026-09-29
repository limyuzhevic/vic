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

    // ========== ERROR HANDLING AND VALIDATION ==========
    
    m.def("validateBrainConfig", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        
        bool valid = true;
        std::string errorMsg;
        
        if (!config->has("neuron_count") || config->getOr<size_t>("neuron_count", 0) == 0) {
            errorMsg += "neuron_count must be set and > 0; ";
            valid = false;
        }
        
        if (!config->has("region_count") || config->getOr<size_t>("region_count", 0) == 0) {
            errorMsg += "region_count must be set and > 0; ";
            valid = false;
        }
        
        if (config->has("connection_probability")) {
            float prob = config->getOr<float>("connection_probability", 0.0f);
            if (prob < 0.0f || prob > 1.0f) {
                errorMsg += "connection_probability must be between 0.0 and 1.0; ";
                valid = false;
            }
        }
        
        if (!valid) {
            throw std::runtime_error("Invalid brain configuration: " + errorMsg);
        }
        
        return valid;
    }, py::arg("config"), "Validate brain configuration and throw exception if invalid");
    
    m.def("createBrainSafe", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        
        if (!validateBrainConfig(config)) {
            throw std::runtime_error("Brain configuration validation failed");
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        return brain;
    }, py::arg("config"), "Create and initialize brain with safety checks");
    
    m.def("createConfigSafe", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        
        return config;
    }, py::arg("config"), "Validate and return config for safe usage");
    
    // ========== PERFORMANCE OPTIMIZATION OPTIONS ==========
    
    m.def("optimizeBrainForPerformance", [](std::shared_ptr<Brain> brain, const std::string& optimizationType) {
        if (!brain) {
            throw std::invalid_argument("Brain cannot be null");
        }
        
        auto config = brain->getConfig();
        
        if (optimizationType == "speed") {
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", std::max(static_cast<size_t>(500), brain->getTotalNeuronCount() / 2));
            config->set("region_count", 1);
            config->set("connection_probability", 0.05f);
        } else if (optimizationType == "memory") {
            config->set("simulation_timestep", 0.01);
            config->set("neuron_count", std::min(static_cast<size_t>(2000), brain->getTotalNeuronCount() * 2));
            config->set("region_count", 2);
            config->set("connection_probability", 0.15f);
        } else if (optimizationType == "accuracy") {
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", std::min(static_cast<size_t>(5000), brain->getTotalNeuronCount() * 5));
            config->set("region_count", 3);
            config->set("connection_probability", 0.2f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
        }
        
        return true;
    }, py::arg("brain"), py::arg("optimizationType"), 
       "Optimize brain configuration for specific performance characteristics");
    
    // ========== CONVENIENCE FUNCTIONS ==========
    
    m.def("createQuickTestBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(200));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return brain;
    }, "Create a small brain for quick testing and debugging");
    
    m.def("createResearchBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(5000));
        config->set("region_count", static_cast<int64_t>(3));
        config->set("connection_probability", 0.05f);
        config->set("simulation_timestep", 0.001);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return brain;
    }, "Create a comprehensive research-grade brain with all systems enabled");
    
    m.def("createAgentWithDefaults", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw std::invalid_argument("Brain cannot be null");
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Create a default world for the agent
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 4, 4);
        world->reset();
        
        agent->initialize(world);
        
        // Enable all subsystems by default
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return agent;
    }, py::arg("brain"), "Create agent with default settings and world");
    
    m.def("runSimulationWithStats", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                       std::shared_ptr<AgentBrain> agent, size_t steps,
                                       bool printProgress = true) {
        if (!brain || !world || !agent) {
            throw std::invalid_argument("Brain, world, and agent must be provided");
        }
        
        std::vector<size_t> spikeCounts;
        std::vector<size_t> firingCounts;
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (printProgress && step % 100 == 0) {
                spikeCounts.push_back(brain->getTotalSpikeCount());
                firingCounts.push_back(brain->getFiringNeuronCount());
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() 
                         << " firing neurons, " << brain->getTotalSpikeCount() << " total spikes" << std::endl;
            }
        }
        
        // Return final statistics
        return std::make_tuple(brain->getTotalSpikeCount(), brain->getFiringNeuronCount(),
                              brain->getAverageFiringRate(), brain->getExcitationInhibitionRatio());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("printProgress") = true,
       "Run simulation with detailed statistics collection");
    
    m.def("createCompleteSystem", [](const std::string& useCase) -> std::tuple<std::shared_ptr<Brain>, 
                                                                            std::shared_ptr<SimpleWorld>, 
                                                                            std::shared_ptr<AgentBrain>> {
        auto config = std::make_shared<Config>();
        auto world = std::make_shared<SimpleWorld>();
        
        if (useCase == "robotics_training") {
            // Robot training setup
            config->set("neuron_count", static_cast<int64_t>(3000));
            config->set("region_count", static_cast<int64_t>(3));
            config->set("connection_probability", 0.1f);
            world->configure(30, 30, 12, 12);
        } else if (useCase == "cognitive_testing") {
            // Cognitive testing setup
            config->set("neuron_count", static_cast<int64_t>(2000));
            config->set("region_count", static_cast<int64_t>(2));
            config->set("connection_probability", 0.15f);
            world->configure(15, 15, 6, 6);
            config->set("neuromod.curiosity.enable", true);
        } else if (useCase == "memory_research") {
            // Memory research setup
            config->set("neuron_count", static_cast<int64_t>(4000));
            config->set("region_count", static_cast<int64_t>(4));
            config->set("connection_probability", 0.08f);
            world->configure(20, 20, 8, 8);
            config->set("working_memory.capacity", 1000);
            config->set("episodic_memory.max_episodes", 500);
        } else {
            // Default balanced setup
            config->set("neuron_count", static_cast<int64_t>(2000));
            config->set("region_count", static_cast<int64_t>(2));
            config->set("connection_probability", 0.1f);
            world->configure(10, 10, 4, 4);
        }
        
        config->set("simulation_timestep", 0.001);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("useCase"), 
       "Create a complete brain-world-agent system for specific use cases");

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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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

    // ========== ADVANCED BRAIN OPERATIONS ==========
    
    m.def("createBrainWithPresetConfig", [](const std::string& presetName) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (presetName == "small") {
            config->set("neuron_count", static_cast<int64_t>(500));
            config->set("region_count", static_cast<int64_t>(2));
            config->set("connection_probability", 0.15f);
        } else if (presetName == "medium") {
            config->set("neuron_count", static_cast<int64_t>(2000));
            config->set("region_count", static_cast<int64_t>(3));
            config->set("connection_probability", 0.1f);
        } else if (presetName == "large") {
            config->set("neuron_count", static_cast<int64_t>(5000));
            config->set("region_count", static_cast<int64_t>(5));
            config->set("connection_probability", 0.05f);
        } else {
            config->set("neuron_count", static_cast<int64_t>(1000));
            config->set("region_count", static_cast<int64_t>(1));
            config->set("connection_probability", 0.1f);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("presetName"), "Create brain with predefined configuration for quick setup");
    
    m.def("createConfigWithDefaults", [](const std::string& presetName) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (presetName == "fast_simulation") {
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", static_cast<int64_t>(500));
            config->set("connection_probability", 0.05f);
        } else if (presetName == "learning_focus") {
            config->set("simulation_timestep", 0.01);
            config->set("neuron_count", static_cast<int64_t>(2000));
            config->set("connection_probability", 0.15f);
            config->set("stdp_ltp_weight", 0.02f);
            config->set("stdp_ltd_weight", 0.015f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
        } else if (presetName == "memory_focus") {
            config->set("simulation_timestep", 0.005);
            config->set("neuron_count", static_cast<int64_t>(3000));
            config->set("connection_probability", 0.1f);
            config->set("working_memory.capacity", 500);
            config->set("episodic_memory.max_episodes", 500);
        } else if (presetName == "curiosity_driven") {
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", static_cast<int64_t>(1500));
            config->set("connection_probability", 0.1f);
            config->set("neuromod.dopamine.scale", 1.0f);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
        } else {
            // Default balanced configuration
            config->set("simulation_timestep", 0.001);
            config->set("neuron_count", static_cast<int64_t>(1000));
            config->set("connection_probability", 0.1f);
        }
        return config;
    }, py::arg("presetName"), "Create configuration with predefined settings for specific use cases");
    
    // ========== BRAIN UTILITY FUNCTIONS ==========
    
    m.def("runBrainEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                              std::shared_ptr<AgentBrain> agent, size_t steps,
                              bool enableLearning = true) -> size_t {
        if (!brain || !world || !agent) {
            throw std::invalid_argument("Brain, world, and agent must be provided");
        }
        
        agent->enableRewardModulation(enableLearning);
        agent->enableStructuralPlasticity(enableLearning);
        agent->enableDevelopment(enableLearning);
        agent->enableCuriosity(enableLearning);
        
        size_t stepsExecuted = 0;
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            stepsExecuted++;
        }
        
        return stepsExecuted;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("enableLearning") = true,
       "Run a complete brain episode with world interaction");
    
    m.def("createBrainExperimentSetup", [](const std::string& experimentType, 
                                         size_t neuronCount = 1000) -> std::tuple<std::shared_ptr<Brain>, 
                                                                                 std::shared_ptr<SimpleWorld>, 
                                                                                 std::shared_ptr<AgentBrain>> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        
        if (experimentType == "spatial_navigation") {
            config->set("region_count", static_cast<int64_t>(3));
            config->set("connection_probability", 0.15f);
            config->set("neuromod.curiosity.enable", true);
        } else if (experimentType == "pattern_learning") {
            config->set("region_count", static_cast<int64_t>(2));
            config->set("connection_probability", 0.2f);
            config->set("plasticity.stdp.enable", true);
        } else if (experimentType == "social_learning") {
            config->set("region_count", static_cast<int64_t>(4));
            config->set("connection_probability", 0.1f);
            config->set("plasticity.hebbian.enable", true);
        } else {
            config->set("region_count", static_cast<int64_t>(1));
            config->set("connection_probability", 0.1f);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("experimentType"), py::arg("neuronCount") = 1000,
       "Create a complete brain-agent-world setup for specific experiment types");
    
    m.def("exportBrainForResearch", [](std::shared_ptr<Brain> brain, const std::string& filepath,
                                      const std::string& format = "binary") {
        if (!brain) {
            throw std::invalid_argument("Brain cannot be null");
        }
        
        brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), py::arg("format") = "binary",
       "Export brain state for research and analysis");

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

} // namespace nlm
