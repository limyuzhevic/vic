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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
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

    // Add more comprehensive error handling and validation
    m.def("createBrainWithValidation", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw std::invalid_argument("Config cannot be null. Use createDefaultConfig() or createSimpleBrain() instead.");
        }
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a brain with validation. The brain will be automatically initialized.\n         \n         Args:\n             config: Configuration object\n             \n         Raises:\n             std::invalid_argument: If config is null\n         \n         Example:\n             >>> import pynlm\n             >>> config = pynlm.createDefaultConfig()\n             >>> brain = pynlm.createBrainWithValidation(config)\n             >>> print(f\"Created brain with {brain.getTotalNeuronCount()} neurons\")\n         ");

    m.def("createCompleteSetupWithValidation", []() {
        py::dict result;
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Configure with reasonable defaults
        brain->initialize();
        world->configure(20, 20, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        result["config"] = config;
        result["brain"] = brain;
        result["world"] = world;
        result["agent"] = agent;
        
        return result;
    }, "Create a complete simulation setup with validation and sensible defaults.\n         \n         Returns:\n             dict: Dictionary containing 'config', 'brain', 'world', and 'agent' keys\n         \n         Example:\n             >>> import pynlm\n             >>> setup = pynlm.createCompleteSetupWithValidation()\n             >>> brain = setup['brain']\n             >>> world = setup['world']\n             >>> agent = setup['agent']\n             >>> print(f\"Ready to run simulation with {brain.getTotalNeuronCount()} neurons\")\n         ");

    m.def("runCompleteSimulationWithValidation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, int steps) {
        // Enhanced validation
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        if (!world) throw std::invalid_argument("World cannot be null");
        if (!agent) throw std::invalid_argument("Agent cannot be null");
        
        if (steps <= 0) throw std::invalid_argument("Number of steps must be positive");
        if (steps > 1000000) throw std::invalid_argument("Number of steps too large (max 1,000,000)");
        
        // Initialize if not already initialized
        if (brain->getTotalNeuronCount() == 0) {
            brain->initialize();
        }
        if (world->getSimulationTime() != 0.0) {
            world->reset();
        }
        agent->initialize(*world);
        
        // Run simulation with better error handling
        for (int i = 0; i < steps; ++i) {
            try {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(i);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            } catch (const std::exception& e) {
                throw std::runtime_error("Error during simulation step " + std::to_string(i) + ": " + std::string(e.what()));
            }
        }
    }, "Run a complete simulation with validation and error checking.\n         \n         Args:\n             brain: The neural brain instance\n             world: The world simulation instance  \n             agent: The agent brain interface\n             steps: Number of simulation steps to run\n             \n         Raises:\n             std::invalid_argument: If any component is null or steps is invalid\n             std::runtime_error: If an error occurs during simulation\n         \n         Example:\n             >>> import pynlm\n             >>> setup = pynlm.createCompleteSetupWithValidation()\n             >>> pynlm.runCompleteSimulationWithValidation(\n             ...     setup['brain'], setup['world'], setup['agent'], 1000)\n             >>> print(f\"Simulation completed successfully!\")\n         ",
         py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"));

    m.def("createQuickTestBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a small test brain for quick testing and development.\n         \n         Returns:\n             Brain: A small brain with 100 neurons for fast testing\n         \n         Example:\n             >>> import pynlm\n             >>> brain = pynlm.createQuickTestBrain()\n             >>> for i in range(100):\n             ...     brain.step(i)\n             >>> print(f\"Test completed: {brain.getTotalSpikeCount()} spikes\")\n         ");

    m.def("createWorldForTesting", [](int size) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(size, size, size/2, size/2);
        world->reset();
        return world;
    }, "Create a world for testing with square dimensions.\n         \n         Args:\n             size: Width and height of the world, also determines vision range\n         \n         Returns:\n             SimpleWorld: Configured world for testing\n         \n         Example:\n             >>> import pynlm\n             >>> world = pynlm.createWorldForTesting(10)\n             >>> print(f\"Created {world.getWidth()}x{world.getHeight()} world\")\n         ",
         py::arg("size"));

    m.def("getBrainInfo", [](std::shared_ptr<Brain> brain) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        
        py::dict info;
        info["total_neurons"] = brain->getTotalNeuronCount();
        info["total_synapses"] = brain->getTotalSynapseCount();
        info["firing_neurons"] = brain->getFiringNeuronCount();
        info["active_neurons"] = brain->getActiveNeuronCount();
        info["total_spikes"] = brain->getTotalSpikeCount();
        info["avg_firing_rate"] = brain->getAverageFiringRate();
        info["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        info["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return info;
    }, "Get comprehensive information about a brain instance.\n         \n         Args:\n             brain: The brain instance to get information from\n             \n         Returns:\n             dict: Dictionary containing brain statistics and information\n         \n         Example:\n             >>> import pynlm\n             >>> brain = pynlm.createQuickTestBrain()\n             >>> info = pynlm.getBrainInfo(brain)\n             >>> for key, value in info.items():\n             ...     print(f\"{key}: {value}\")\n         ",
         py::arg("brain"));

    m.def("createPerformanceProfile", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, int steps, bool enableProfiling = true) {
        if (!enableProfiling) {
            return brain->getTotalSpikeCount();
        }
        
        // Enhanced performance monitoring
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (brain->getTotalNeuronCount() == 0) {
            brain->initialize();
        }
        if (world->getSimulationTime() != 0.0) {
            world->reset();
        }
        agent->initialize(*world);
        
        size_t totalSpikes = 0;
        double maxTimePerStep = 0.0;
        
        for (int i = 0; i < steps; ++i) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            totalSpikes += brain->getTotalSpikeCount();
            
            auto stepEnd = std::chrono::high_resolution_clock::now();
            auto stepDuration = std::chrono::duration<double>(stepEnd - stepStart).count();
            maxTimePerStep = std::max(maxTimePerStep, stepDuration);
            
            // Optional: Progress callback (can be implemented for long simulations)
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Log performance summary
        std::cout << "=== Performance Profile ===" << std::endl;
        std::cout << "Total simulation time: " << totalTime << "s" << std::endl;
        std::cout << "Steps per second: " << (steps / totalTime) << std::endl;
        std::cout << "Max time per step: " << maxTimePerStep << "s" << std::endl;
        std::cout << "Total spikes generated: " << totalSpikes << std::endl;
        std::cout << "Average spikes per second: " << (totalSpikes / totalTime) << std::endl;
        
        return totalSpikes;
    }, "Run a simulation with performance profiling and monitoring.\n         \n         Args:\n             brain: The neural brain instance\n             world: The world simulation instance  \n             agent: The agent brain interface\n             steps: Number of simulation steps to run\n             enableProfiling: Enable detailed performance logging\n             \n         Returns:\n             int: Total number of spikes generated during simulation\n         \n         Example:\n             >>> import pynlm\n             >>> setup = pynlm.createCompleteSetupWithValidation()\n             >>> spikes = pynlm.createPerformanceProfile(\n             ...     setup['brain'], setup['world'], setup['agent'], 5000)\n             >>> print(f\"Simulation generated {spikes} total spikes\")\n         ",
         py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("enableProfiling") = true);

    m.def("createCustomExperiment", [](std::string experimentName, std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, int steps) {
        // Custom experiment runner with metadata
        py::dict experimentData;
        
        experimentData["name"] = experimentName;
        experimentData["start_time"] = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        experimentData["steps"] = steps;
        
        // Run the experiment
        if (brain->getTotalNeuronCount() == 0) {
            brain->initialize();
        }
        if (world->getSimulationTime() != 0.0) {
            world->reset();
        }
        agent->initialize(*world);
        
        for (int i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        experimentData["end_time"] = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        experimentData["duration"] = experimentData["end_time"].cast<double>() - experimentData["start_time"].cast<double>();
        experimentData["final_spike_count"] = brain->getTotalSpikeCount();
        experimentData["final_firing_neurons"] = brain->getFiringNeuronCount();
        experimentData["final_developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return experimentData;
    }, "Run a custom experiment with metadata collection.\n         \n         Args:\n             experimentName: Name/identifier for the experiment\n             brain: The neural brain instance\n             world: The world simulation instance  \n             agent: The agent brain interface\n             steps: Number of simulation steps to run\n             \n         Returns:\n             dict: Dictionary containing experiment metadata and results\n         \n         Example:\n             >>> import pynlm\n             >>> setup = pynlm.createCompleteSetupWithValidation()\n             >>> results = pynlm.createCustomExperiment(\"Curiosity Test\", \n             ...     setup['brain'], setup['world'], setup['agent'], 3000)\n             >>> print(f\"Experiment {results['name']} completed in {results['duration']:.2f}s\")\n         ",
         py::arg("experimentName"), py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"));

    m.def("createConfigurationPreset", [](std::string presetName) {
        auto config = std::make_shared<Config>();
        
        if (presetName == "small") {
            // Small brain for quick testing
            config->set("brain.neuron_count", 100);
            config->set("brain.synapse_density", 0.05);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("development.enable", false);
        } else if (presetName == "medium") {
            // Medium-sized brain for standard experiments
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.1);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("development.enable", true);
            config->set("development.critical_period_start", 0.0);
        } else if (presetName == "large") {
            // Large brain for complex simulations
            config->set("brain.neuron_count", 5000);
            config->set("brain.synapse_density", 0.15);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.0005);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("development.enable", true);
            config->set("development.critical_period_start", 100.0);
            config->set("development.maturation_rate", 0.005);
            config->set("performance.memory_pool_size", 4096);
        } else {
            throw std::invalid_argument("Unknown preset name: " + presetName + ". Available: 'small', 'medium', 'large'");
        }
        
        return config;
    }, "Create a configuration preset with predefined parameters.\n         \n         Args:\n             presetName: Name of the preset ('small', 'medium', or 'large')\n             \n         Returns:\n             Config: Configuration object with preset parameters\n         \n         Example:\n             >>> import pynlm\n             >>> config = pynlm.createConfigurationPreset(\"medium\")\n             >>> brain = pynlm.createBrainWithValidation(config)\n             >>> print(f\"Created medium brain with {brain.getTotalNeuronCount()} neurons\")\n         ",
         py::arg("presetName"));

    m.def("batchRunExperiments", [](std::vector<std::string> experimentNames, std::vector<std::shared_ptr<Brain>> brains, std::vector<std::shared_ptr<SimpleWorld>> worlds, std::vector<std::shared_ptr<AgentBrain>> agents, int stepsPerExperiment) {
        if (experimentNames.size() != brains.size() || brains.size() != worlds.size() || worlds.size() != agents.size()) {
            throw std::invalid_argument("All experiment vectors must have the same length");
        }
        
        std::vector<py::dict> results;
        
        for (size_t i = 0; i < experimentNames.size(); ++i) {
            py::dict result;
            result["name"] = experimentNames[i];
            
            try {
                auto startTime = std::chrono::high_resolution_clock::now();
                
                if (brains[i]->getTotalNeuronCount() == 0) {
                    brains[i]->initialize();
                }
                if (worlds[i]->getSimulationTime() != 0.0) {
                    worlds[i]->reset();
                }
                agents[i]->initialize(*worlds[i]);
                
                for (int step = 0; step < stepsPerExperiment; ++step) {
                    worlds[i]->update(0.1);
                    auto percept = worlds[i]->getSensoryPercept();
                    agents[i]->processSensoryInput(percept);
                    brains[i]->step(step);
                    auto action = agents[i]->decodeMotorCommand();
                    worlds[i]->applyMotorCommand(action, worlds[i]->getSimulationTime());
                }
                
                auto endTime = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(endTime - startTime).count();
                
                result["success"] = true;
                result["duration"] = duration;
                result["final_spike_count"] = brains[i]->getTotalSpikeCount();
                result["final_firing_neurons"] = brains[i]->getFiringNeuronCount();
                result["error_message"] = "";
                
            } catch (const std::exception& e) {
                result["success"] = false;
                result["duration"] = 0.0;
                result["final_spike_count"] = 0;
                result["final_firing_neurons"] = 0;
                result["error_message"] = std::string(e.what());
            }
            
            results.push_back(result);
        }
        
        return results;
    }, "Run multiple experiments in batch with consistent error handling.\n         \n         Args:\n             experimentNames: List of experiment names/identifiers\n             brains: List of brain instances (one per experiment)\n             worlds: List of world instances (one per experiment)\n             agents: List of agent instances (one per experiment)\n             stepsPerExperiment: Number of steps for each experiment\n             \n         Returns:\n             list: List of result dictionaries for each experiment\n         \n         Example:\n             >>> import pynlm\n             >>> import pynlm\n             >>> brains = [pynlm.createQuickTestBrain() for _ in range(3)]\n             >>> worlds = [pynlm.createWorldForTesting(10) for _ in range(3)]\n             >>> agents = [pynlm.createAgent(brain, world) for brain, world in zip(brains, worlds)]\n             >>> experiments = [\"Test A\", \"Test B\", \"Test C\"]\n             >>> results = pynlm.batchRunExperiments(experiments, brains, worlds, agents, 1000)\n             >>> for result in results:\n             ...     print(f\"{result['name']}: {'SUCCESS' if result['success'] else 'FAILED'} in {result['duration']:.2f}s\")\n         ",
         py::arg("experimentNames"), py::arg("brains"), py::arg("worlds"), py::arg("agents"), py::arg("stepsPerExperiment"));

} // namespace nlm
