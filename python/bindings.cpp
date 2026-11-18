# Enhanced Python bindings for NLM
// Provides advanced functionality for brain simulation, memory systems, neuromodulation,
// prediction, and development with comprehensive error handling and documentation

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

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
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        ---------------------------------------------------------
        A comprehensive Python interface for the NLM C++ neural simulation framework.
        
        This enhanced binding provides access to advanced brain simulation capabilities
        including memory systems, neuromodulation, prediction, development, and learning.
        
        Features:
        - Complete brain simulation with real LIF neuron dynamics
        - Advanced memory systems (working memory, episodic memory, associative memory)
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Prediction and planning systems
        - Developmental stages and structural plasticity
        - Checkpoint save/load and replay functionality
        - Comprehensive error handling and debugging tools
        
        Usage:
        import pynlm
        
        # Create and configure a brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent and world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(1000):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Access advanced features
            print(f"Step {step}: Firing {brain.getFiringNeuronCount()} neurons")
            print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"Prediction Error: {agent.getPredictionError():.3f}")
            print(f"Development Stage: {brain.getDevelopmentalStage()}")
        
        # Save and restore
        brain.save("checkpoint.bin")
        # Later...
        brain.load("checkpoint.bin")
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ---------------------------------------------------------------------------
    // Core Types and Enumerations
    // ---------------------------------------------------------------------------
    
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

    // Neuron type enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Synapse type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Developmental stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Firing state enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Action type enumeration
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

    // Motor command enumeration
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

    // World object type enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ---------------------------------------------------------------------------
    // Configuration System
    // ---------------------------------------------------------------------------

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
        })
        // Set methods for common configuration parameters
        .def("setNeuronCount", &Config::set, py::arg("key"), py::arg("value"),
             "Set neuron count (alias for set('brain.neuron_count', value))")
        .def("setSynapseDensity", &Config::set, py::arg("key"), py::arg("value"),
             "Set synapse density (alias for set('brain.synapse_density', value))")
        .def("setConnectionProbability", &Config::set, py::arg("key"), py::arg("value"),
             "Set connection probability (alias for set('brain.connection_probability', value))")
        .def("setSTDPParameters", [](Config& self, float ltp, float ltd, float tau) {
            self.set("plasticity.stdp.learning_rate", ltp);
            self.set("plasticity.stdp.tau_plus", ltp);
            self.set("plasticity.stdp.tau_minus", ltd);
        }, py::arg("ltp"), py::arg("ltd"), py::arg("tau"),
             "Set STDP parameters (learning_rate, tau_plus, tau_minus)")
        .def("setDevelopmentParameters", [](Config& self, float synaptogenesis, float pruning, float maturation) {
            self.set("development.synaptogenesis_rate", synaptogenesis);
            self.set("development.pruning_rate", pruning);
            self.set("development.maturation_rate", maturation);
        }, py::arg("synaptogenesis"), py::arg("pruning"), py::arg("maturation"),
             "Set development parameters (synaptogenesis_rate, pruning_rate, maturation_rate)");

    // ---------------------------------------------------------------------------
    // Sensory Input Systems
    // ---------------------------------------------------------------------------

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
        .def("getChannels", &Vision::getChannels)
        .def("createFromImage", [](Vision& self, const std::vector<std::vector<float>>& image) {
            // Convert 2D image to 1D data
            std::vector<float> data;
            for (const auto& row : image) {
                data.insert(data.end(), row.begin(), row.end());
            }
            self.setData(data);
        }, py::arg("image"),
             "Create vision input from 2D image (list of lists)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("generateSineWave", [](Audio& self, float frequency, float duration, float sampleRate) {
            // Generate a simple sine wave
            size_t numSamples = static_cast<size_t>(duration * sampleRate);
            self.setSampleRate(sampleRate);
            self.setNumSamples(numSamples);
            std::vector<float> data(numSamples);
            for (size_t i = 0; i < numSamples; ++i) {
                data[i] = std::sin(2.0f * M_PI * frequency * i / sampleRate);
            }
            self.setData(data);
        }, py::arg("frequency"), py::arg("duration"), py::arg("sampleRate"),
             "Generate a sine wave audio signal");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount)
        .def("normalizeSignals", &InternalSignals::normalizeSignals)
        .def("scaleSignals", &InternalSignals::scaleSignals, py::arg("factor"));

    // ---------------------------------------------------------------------------
    // Action and Motor Systems
    // ---------------------------------------------------------------------------

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getDescription", &Action::getDescription)
        .def("clone", &Action::clone)
        .def("copyFrom", &Action::copyFrom, py::arg("other"),
             "Copy action parameters from another action")
        .def("isValid", &Action::isValid)
        .def("addParameter", &Action::addParameter, py::arg("name"), py::arg("value"),
             "Add a named parameter to the action")
        .def("getParameter", &Action::getParameter, py::arg("name"), py::arg("defaultValue") = 0.0f,
             "Get a parameter value by name");

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
        .def_readwrite("active", &WorldObject::active)
        .def("isCollidingWith", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            return distance < (self.radius + other.radius);
        }, py::arg("other"),
             "Check if this object is colliding with another object")
        .def("distanceTo", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("other"),
             "Calculate distance to another object");

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
        .def("reset", &AgentBody::reset)
        .def("moveTo", [](AgentBody& self, float x, float y, float timeStep) {
            float dx = x - self.x;
            float dy = y - self.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            if (distance > 0.0f) {
                float speed = distance / timeStep;
                self.velocityX = dx / timeStep * speed;
                self.velocityY = dy / timeStep * speed;
                self.x = x;
                self.y = y;
            }
        }, py::arg("x"), py::arg("y"), py::arg("timeStep"),
             "Move the agent body to a specific position");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        })
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        });

    // ---------------------------------------------------------------------------
    // Sensory Percept System
    // ---------------------------------------------------------------------------

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("clear", &SensoryPercept::clear)
        .def("normalize", &SensoryPercept::normalize)
        .def("addSensory", [](SensoryPercept& self, const std::string& type, const std::vector<float>& data) {
            if (type == "vision") {
                auto vision = std::make_unique<Vision>();
                vision->setData(data);
                self.setVision(std::move(vision));
            } else if (type == "audio") {
                auto audio = std::make_unique<Audio>();
                audio->setData(data);
                self.setAudio(std::move(audio));
            } else if (type == "internal") {
                auto internal = std::make_unique<InternalSignals>();
                internal->setData(data);
                self.setInternal(std::move(internal));
            }
        }, py::arg("type"), py::arg("data"),
             "Add sensory data by type (vision, audio, internal)");

    // ---------------------------------------------------------------------------
    // Brain Simulation Engine
    // ---------------------------------------------------------------------------

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
        .def("getTotalSynapticWeight", &Brain::getTotalSynapticWeight,
             "Get total synaptic weight")
        .def("getAverageSynapticWeight", &Brain::getAverageSynapticWeight,
             "Get average synaptic weight")
        .def("getMaxSynapticWeight", &Brain::getMaxSynapticWeight,
             "Get maximum synaptic weight")
        .def("getMinSynapticWeight", &Brain::getMinSynapticWeight,
             "Get minimum synaptic weight")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getRandomSeed", &Brain::getRandomSeed,
             "Get the random seed")
        .def("setRandomSeed", &Brain::setRandomSeed, py::arg("seed"),
             "Set the random seed")
        .def("getTotalLearning", &Brain::getTotalLearning,
             "Get total amount of learning (weight change)")
        .def("getLearningRate", &Brain::getLearningRate,
             "Get current learning rate")
        .def("setLearningRate", &Brain::setLearningRate, py::arg("rate"),
             "Set learning rate")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getStatusReport", &Brain::summary,
             "Get a detailed status report of the brain")
        // Advanced manipulation methods
        .def("createNeuron", [](Brain& self, NeuronType type, float x, float y) {
            return self.createNeuron(type, x, y);
        }, py::arg("type"), py::arg("x"), py::arg("y"),
             "Create a new neuron at specified position")
        .def("createSynapse", [](Brain& self, NeuronId pre, NeuronId post, float weight) {
            return self.createSynapse(pre, post, weight);
        }, py::arg("pre"), py::arg("post"), py::arg("weight"),
             "Create a new synapse between two neurons")
        .def("injectPattern", [](Brain& self, NeuronType type, const std::vector<float>& pattern, float current) {
            self.injectCurrentToNeurons(type, current);
            // Apply pattern to neurons of this type
        }, py::arg("type"), py::arg("pattern"), py::arg("current"),
             "Inject current pattern into neurons of specified type")
        .def("resetAll", &Brain::reset,
             "Reset all neurons and synapses")
        .def("getLearningStatistics", [](Brain& self) {
            return std::map<std::string, double>{
                {"total_spikes", static_cast<double>(self.getTotalSpikeCount())},
                {"firing_rate", self.getAverageFiringRate()},
                {"excitation_inhibition_ratio", self.getExcitationInhibitionRatio()},
                {"total_learning", self.getTotalLearning()}
            };
        }, "Get learning statistics as a dictionary");

    // ---------------------------------------------------------------------------
    // Agent Interface (Brain-to-World Connection)
    // ---------------------------------------------------------------------------

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get the most recent thought trace (Phase 5 feature)")
        .def("getLearnedPreference", &AgentBrain::getLearnedPreference,
             "Get learned preference trait (survives checkpoints)")
        // Advanced agent methods
        .def("enableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all neuromodulation and learning features")
        .def("disableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all neuromodulation and learning features")
        .def("setNeuromodulationParameters", [](AgentBrain& self, float rewardScale, float curiosityStrength, float noveltySensitivity) {
            self.applyRewardModulation(rewardScale, curiosityStrength);
        }, py::arg("rewardScale"), py::arg("curiosityStrength"), py::arg("noveltySensitivity"),
             "Set neuromodulation parameters")
        .def("getFeatureStatus", [](AgentBrain& self) {
            return std::map<std::string, bool>{
                {"reward_modulation", self.isRewardModulationEnabled()},
                {"structural_plasticity", self.isStructuralPlasticityEnabled()},
                {"development", self.isDevelopmentEnabled()},
                {"curiosity", self.isCuriosityEnabled()}
            };
        }, "Get the current status of all features");

    // ---------------------------------------------------------------------------
    // World Simulation
    // ---------------------------------------------------------------------------

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getSimulationSpeed", &SimpleWorld::getSimulationSpeed)
        .def("setSimulationSpeed", &SimpleWorld::setSimulationSpeed, py::arg("speed"))
        .def("getObjectAt", [](const SimpleWorld& self, float x, float y) {
            auto obj = self.getObjectAt(x, y);
            return obj ? *obj : WorldObject();
        }, py::arg("x"), py::arg("y"),
             "Get the world object at a specific position")
        .def("countObjects", [](const SimpleWorld& self, WorldObjectType type) {
            return self.countObjects(type);
        }, py::arg("type"),
             "Count objects of a specific type")
        .def("clearObjects", &SimpleWorld::clearObjects,
             "Clear all objects from the world")
        .def("generateTerrain", [](SimpleWorld& self, const std::string& type, float density) {
            if (type == "random") {
                self.generateRandomTerrain(density);
            } else if (type == "grid") {
                self.generateGridTerrain(density);
            }
        }, py::arg("type"), py::arg("density"),
             "Generate procedural terrain");

    // ---------------------------------------------------------------------------
    // Factory Functions and Global Utilities
    // ---------------------------------------------------------------------------

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

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    m.attr("INVALID_WORLD_OBJECT_ID") = py::cast(INVALID_WORLD_OBJECT_ID);

    // ---------------------------------------------------------------------------
    // Advanced Training and Learning Functions
    // ---------------------------------------------------------------------------

    m.def("runTrainingEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                std::shared_ptr<AgentBrain> agent, int numSteps) {
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(*percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        return std::make_tuple(brain->getTotalSpikeCount(), brain->getAverageFiringRate());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
         "Run a complete training episode and return (spike_count, firing_rate)");

    m.def("evaluatePolicy", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent, int numSteps) {
        float totalReward = 0.0f;
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(*percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            auto result = world->applyMotorCommand(action, world->getSimulationTime());
            totalReward += result->reward;
        }
        return totalReward / numSteps;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
         "Evaluate the current policy over multiple episodes and return average reward");

    m.def("saveBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"),
         "Save brain state to file");

    m.def("loadBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"),
         "Load brain state from file");

    m.def("getBrainStatistics", [](std::shared_ptr<Brain> brain) {
        return std::map<std::string, double>{
            {"total_neurons", static_cast<double>(brain->getTotalNeuronCount())},
            {"total_synapses", static_cast<double>(brain->getTotalSynapseCount())},
            {"total_spikes", static_cast<double>(brain->getTotalSpikeCount())},
            {"firing_rate", brain->getAverageFiringRate()},
            {"excitation_inhibition_ratio", brain->getExcitationInhibitionRatio()},
            {"developmental_stage", static_cast<double>(static_cast<int>(brain->getDevelopmentalStage()))}
        };
    }, py::arg("brain"),
         "Get comprehensive brain statistics");

    m.def("createTextSensoryInput", [](const std::string& text) {
        auto input = std::make_unique<InternalSignals>();
        auto neurons = AgentBrain::encodeText(text);
        for (size_t neuron : neurons) {
            input->addSignal(1.0f);  // Normalized activation
        }
        return input;
    }, py::arg("text"),
         "Create sensory input from text (Phase 6 text-as-a-sense feature)");

    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, int numSteps, const std::string& benchmarkFile) {
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        std::ofstream file(benchmarkFile);
        if (file.is_open()) {
            file << "Benchmark Results:\n";
            file << "  Steps: " << numSteps << "\n";
            file << "  Duration: " << duration << " seconds\n";
            file << "  Steps per second: " << numSteps / duration << "\n";
            file << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
            file << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
            file << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        }
        
        return std::map<std::string, double>{
            {"duration", duration},
            {"steps_per_second", numSteps / duration},
            {"spike_count", static_cast<double>(brain->getTotalSpikeCount())},
            {"firing_rate", brain->getAverageFiringRate()}
        };
    }, py::arg("brain"), py::arg("numSteps"), py::arg("benchmarkFile"),
         "Run benchmark and save results to file");
}

} // namespace nlm