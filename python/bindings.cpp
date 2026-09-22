#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
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
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/Observation.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"

namespace py = pybind11;
namespace nlm {

// Helper function to create a simple structured representation of the brain
std::string brainToJson(const Brain& brain) {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"total_neurons\": " << brain.getTotalNeuronCount() << ",\n";
    oss << "  \"total_synapses\": " << brain.getTotalSynapseCount() << ",\n";
    oss << "  \"active_neurons\": " << brain.getActiveNeuronCount() << ",\n";
    oss << "  \"firing_neurons\": " << brain.getFiringNeuronCount() << ",\n";
    oss << "  \"total_spikes\": " << brain.getTotalSpikeCount() << ",\n";
    oss << "  \"avg_firing_rate\": " << brain.getAverageFiringRate() << ",\n";
    oss << "  \"e_i_ratio\": " << brain.getExcitationInhibitionRatio() << ",\n";
    oss << "  \"developmental_stage\": " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
    oss << "}\n";
    return oss.str();
}

// Create a default Config with optimized brain parameters for simulation
std::shared_ptr<Config> createOptimizedConfig(int neuronCount, int regionCount) {
    auto config = std::make_shared<Config>();
    
    // Set brain configuration
    config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Runtime,
                "Total number of neurons in the brain", "Brain", false);
    config->set("brain.region_count", static_cast<int64_t>(regionCount), ConfigSource::Runtime,
                "Number of brain regions", "Brain", false);
    config->set("brain.connection_probability", 0.15f, ConfigSource::Runtime,
                "Probability of connection between neurons", "Brain", false);
    config->set("brain.initial_weight_mean", 0.5f, ConfigSource::Runtime,
                "Mean initial synaptic weight", "Brain", false);
    config->set("brain.initial_weight_std", 0.1f, ConfigSource::Runtime,
                "Standard deviation of initial weights", "Brain", false);
    
    // Set plasticity configuration for learning
    config->set("plasticity.stdp.enable", true, ConfigSource::Runtime,
                "Enable Spike-Timing Dependent Plasticity", "Plasticity", false);
    config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime,
                "Enable Hebbian learning", "Plasticity", false);
    config->set("plasticity.structural.enable", true, ConfigSource::Runtime,
                "Enable structural plasticity", "Plasticity", false);
    config->set("plasticity.stdp.ltp_weight", 0.02f, ConfigSource::Runtime,
                "STDP potentiation weight", "Plasticity", true);
    config->set("plasticity.stdp.ltd_weight", 0.015f, ConfigSource::Runtime,
                "STDP depression weight", "Plasticity", true);
    config->set("plasticity.stdp.tau", 20.0f, ConfigSource::Runtime,
                "STDP time constant (ms)", "Plasticity", true);
    
    // Set neuromodulation for motivation
    config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Runtime,
                "Dopamine modulation scale", "Neuromodulation", true);
    config->set("neuromod.curiosity.enable", true, ConfigSource::Runtime,
                "Enable curiosity-driven exploration", "Neuromodulation", false);
    config->set("neuromod.novelty.enable", true, ConfigSource::Runtime,
                "Enable novelty detection", "Neuromodulation", false);
    
    return config;
}

// Create a simple learning config (smaller, faster learning)
std::shared_ptr<Config> createLearningConfig(int neuronCount, int regionCount) {
    auto config = std::make_shared<Config>();
    
    // Smaller network for faster learning
    config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Runtime,
                "Total number of neurons in the brain", "Brain", false);
    config->set("brain.region_count", static_cast<int64_t>(regionCount), ConfigSource::Runtime,
                "Number of brain regions", "Brain", false);
    config->set("brain.connection_probability", 0.1f, ConfigSource::Runtime,
                "Probability of connection between neurons", "Brain", false);
    
    // Enhanced plasticity for rapid learning
    config->set("plasticity.stdp.enable", true, ConfigSource::Runtime,
                "Enable Spike-Timing Dependent Plasticity", "Plasticity", false);
    config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime,
                "Enable Hebbian learning", "Plasticity", false);
    config->set("plasticity.structural.enable", true, ConfigSource::Runtime,
                "Enable structural plasticity", "Plasticity", false);
    config->set("plasticity.stdp.ltp_weight", 0.05f, ConfigSource::Runtime,
                "STDP potentiation weight (high for rapid learning)", "Plasticity", true);
    config->set("plasticity.stdp.ltd_weight", 0.04f, ConfigSource::Runtime,
                "STDP depression weight (high for rapid learning)", "Plasticity", true);
    config->set("plasticity.stdp.tau", 10.0f, ConfigSource::Runtime,
                "STDP time constant (shorter for faster learning)", "Plasticity", true);
    
    // Stronger neuromodulation for exploration
    config->set("neuromod.dopamine.scale", 2.0f, ConfigSource::Runtime,
                "Dopamine modulation scale (high for exploration)", "Neuromodulation", true);
    config->set("neuromod.curiosity.enable", true, ConfigSource::Runtime,
                "Enable curiosity-driven exploration", "Neuromodulation", false);
    config->set("neuromod.novelty.enable", true, ConfigSource::Runtime,
                "Enable novelty detection", "Neuromodulation", false);
    
    return config;
}

// Create a stable config (larger, slower changes)
std::shared_ptr<Config> createStableConfig(int neuronCount, int regionCount) {
    auto config = std::make_shared<Config>();
    
    // Larger network for stability
    config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Runtime,
                "Total number of neurons in the brain", "Brain", false);
    config->set("brain.region_count", static_cast<int64_t>(regionCount), ConfigSource::Runtime,
                "Number of brain regions", "Brain", false);
    config->set("brain.connection_probability", 0.05f, ConfigSource::Runtime,
                "Probability of connection between neurons (sparse)", "Brain", false);
    
    // Conservative plasticity
    config->set("plasticity.stdp.enable", true, ConfigSource::Runtime,
                "Enable Spike-Timing Dependent Plasticity", "Plasticity", false);
    config->set("plasticity.hebbian.enable", false, ConfigSource::Runtime,
                "Disable Hebbian learning (stable)", "Plasticity", false);
    config->set("plasticity.structural.enable", false, ConfigSource::Runtime,
                "Disable structural plasticity (stable)", "Plasticity", false);
    config->set("plasticity.stdp.ltp_weight", 0.005f, ConfigSource::Runtime,
                "STDP potentiation weight (conservative)", "Plasticity", true);
    config->set("plasticity.stdp.ltd_weight", 0.01f, ConfigSource::Runtime,
                "STDP depression weight (strong depression)", "Plasticity", true);
    
    // Stable neuromodulation
    config->set("neuromod.dopamine.scale", 0.5f, ConfigSource::Runtime,
                "Dopamine modulation scale (low for stability)", "Neuromodulation", true);
    config->set("neuromod.curiosity.enable", false, ConfigSource::Runtime,
                "Disable curiosity-driven exploration (stable)", "Neuromodulation", false);
    
    return config;
}

// Create a comprehensive world with different terrain types
std::shared_ptr<SimpleWorld> createRichWorld(int width, int height) {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width, height, 8, 8); // Vision size
    
    // Add resources at random positions
    for (int x = 2; x < width - 2; x += 3) {
        for (int y = 2; y < height - 2; y += 3) {
            if ((x + y) % 5 == 0) { // Every 5th position
                WorldObject resource;
                resource.x = static_cast<float>(x);
                resource.y = static_cast<float>(y);
                resource.type = WorldObjectType::Resource;
                resource.value = 1.0f + (rand() % 100) / 100.0f; // 1.0 to 2.0
                resource.radius = 0.3f + (rand() % 70) / 100.0f;  // 0.3 to 1.0
                world->addObject(resource);
            } else if ((x + y) % 7 == 0) { // Every 7th position
                WorldObject hazard;
                hazard.x = static_cast<float>(x);
                hazard.y = static_cast<float>(y);
                hazard.type = WorldObjectType::Hazard;
                hazard.value = -0.5f; // Negative value for hazard
                hazard.radius = 0.2f;
                world->addObject(hazard);
            }
        }
    }
    
    // Add walls around the perimeter
    for (int x = 0; x < width; ++x) {
        WorldObject wall;
        wall.x = static_cast<float>(x);
        wall.y = 0.0f;
        wall.type = WorldObjectType::Wall;
        wall.radius = 0.5f;
        world->addObject(wall);
        
        wall.y = static_cast<float>(height - 1);
        world->addObject(wall);
    }
    
    for (int y = 0; y < height; ++y) {
        WorldObject wall;
        wall.x = 0.0f;
        wall.y = static_cast<float>(y);
        wall.type = WorldObjectType::Wall;
        wall.radius = 0.5f;
        world->addObject(wall);
        
        wall.x = static_cast<float>(width - 1);
        world->addObject(wall);
    }
    
    // Place agent in center
    world->setAgentStart(static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f);
    
    return world;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------------
        
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This enhanced API provides:
        
        **Basic Brain Operations:**
        - Create and configure brains with different preset modes
        - Run simulations and collect metrics
        - Save and load brain states
        
        **Advanced Features:**
        - Structured configuration access (brain, plasticity, neuromodulation)
        - Rich world environments with resources and hazards
        - Agent interfaces for goal-directed behavior
        - Comprehensive monitoring and statistics
        - JSON serialization support
        
        **Presets:**
        - learning: Fast learning with high plasticity
        - stable: Stable learning with low plasticity  
        - balanced: Balanced learning with moderate plasticity
        - compact: Small network for testing
        - exploration: High exploration and curiosity
        
        **Key Classes:**
        - Brain: Central neural processing unit
        - Config: Configuration management with structured access
        - AgentBrain: Interface for goal-directed behavior
        - SimpleWorld: Rich environment with objects and hazards
        - SensoryPercept: Processed sensory information
        - Action: Motor commands for action execution
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Enumeration exports
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

    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
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
        .def("getKeysByCategory", &Config::getKeysByCategory, py::arg("category"),
             "Get configuration keys by category")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a detailed summary string of the configuration")
        .def("toMap", &Config::toMap,
             "Get configuration as key-value string map")
        .def("validate", &Config::validate,
             "Validate configuration and return list of errors")
        .def("isModified", &Config::isModified, py::arg("key"),
             "Check if a configuration value differs from default")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Identity classes
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

    // Sensory input classes
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

    // Action and object classes
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

    // Enhanced World class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Advanced 2D world with rich environment)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("applyAction", &SimpleWorld::applyAction,
             py::arg("agent"), py::arg("action"),
             "Apply action from agent to world")
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
        .def("observe", &SimpleWorld::observe, py::arg("agent"),
             "Get sensory percept from agent's perspective")
        .def("computeReward", &SimpleWorld::computeReward, py::arg("agent"),
             "Compute reward for agent's action");

    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Advanced neural simulation brain class)pbdoc")
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
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             "Get attention system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error signal")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("to_json", &brainToJson,
             "Convert brain state to JSON string")
        .def("getBrainConfig", &Brain::getBrainConfig,
             "Get brain configuration struct")
        .def("getPlasticityConfig", &Brain::getPlasticityConfig,
             "Get plasticity configuration struct")
        .def("getNeuromodulationConfig", &Brain::getNeuromodulationConfig,
             "Get neuromodulation configuration struct");

    // AgentBrain class
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

    // Factory functions for creating brains
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createOptimizedConfig", &createOptimizedConfig,
         py::arg("neuronCount"), py::arg("regionCount"),
         "Create an optimized configuration for simulation");

    m.def("createLearningConfig", &createLearningConfig,
         py::arg("neuronCount"), py::arg("regionCount"),
         "Create a learning-optimized configuration");

    m.def("createStableConfig", &createStableConfig,
         py::arg("neuronCount"), py::arg("regionCount"),
         "Create a stable configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createRichWorld", &createRichWorld,
         py::arg("width"), py::arg("height"),
         "Create a rich world with resources and hazards");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience functions for common tasks
    m.def("runQuickSimulation", [](int steps, int neurons) {
        auto config = createOptimizedConfig(neurons, 1);
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            for (int step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);
            }
        }
        return brain;
    }, py::arg("steps"), py::arg("neurons"),
        "Run a quick simulation and return brain");

    m.def("createTestWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        return world;
    }, "Create a simple test world");

    m.def("createLearningAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        return agent;
    }, py::arg("brain"), py::arg("world"),
        "Create an agent with all learning systems enabled");

    // Special enumerations for common configurations
    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source enumeration)pbdoc")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values();

    // Configuration category enumerations
    py::class_<py::typed<int, std::string>>((m, "Categories"), R"pbdoc(Configuration categories)")
        .def(py::init<>[])
        .def("brain", []() { return std::make_shared<std::string>("Brain"); })
        .def("plasticity", []() { return std::make_shared<std::string>("Plasticity"); })
        .def("neuromodulation", []() { return std::make_shared<std::string>("Neuromodulation"); })
        .def("general", []() { return std::make_shared<std::string>("General"); })
        .def("advanced", []() { return std::make_shared<std::string>("Advanced"); });

    // Utility constants
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_REGION_COUNT") = 1;
    m.attr("DEFAULT_SIMULATION_STEPS") = 1000;
    m.attr("MIN_NEURON_COUNT") = 10;
    m.attr("MAX_NEURON_COUNT") = 10000;

    // Version information
    m.attr("__version__") = "2.0.0-enhanced";

    // Advanced convenience methods for the Config class
    m.def("configureBrain", [](Config& config, int neuronCount, int regions) {
        config.set("brain.neuron_count", neuronCount, ConfigSource::Runtime,
                  "Total number of neurons", "Brain", false);
        config.set("brain.region_count", regions, ConfigSource::Runtime,
                  "Number of brain regions", "Brain", false);
        config.set("brain.connection_probability", 0.15f, ConfigSource::Runtime,
                  "Connection probability", "Brain", false);
        return config;
    }, py::arg("config"), py::arg("neuronCount"), py::arg("regions"),
        "Configure brain parameters");

    m.def("enableLearningMode", [](Config& config) {
        config.set("plasticity.stdp.enable", true, ConfigSource::Runtime,
                  "STDP plasticity", "Plasticity", false);
        config.set("plasticity.hebbian.enable", true, ConfigSource::Runtime,
                  "Hebbian plasticity", "Plasticity", false);
        config.set("plasticity.structural.enable", true, ConfigSource::Runtime,
                  "Structural plasticity", "Plasticity", false);
        config.set("neuromod.dopamine.scale", 1.5f, ConfigSource::Runtime,
                  "Dopamine scale", "Neuromodulation", true);
        config.set("neuromod.curiosity.enable", true, ConfigSource::Runtime,
                  "Curiosity exploration", "Neuromodulation", false);
        return config;
    }, py::arg("config"), "Enable full learning mode");

    m.def("enableExplorationMode", [](Config& config) {
        config.set("neuromod.curiosity.enable", true, ConfigSource::Runtime,
                  "Enable curiosity-driven exploration", "Neuromodulation", false);
        config.set("neuromod.novelty.enable", true, ConfigSource::Runtime,
                  "Enable novelty detection", "Neuromodulation", false);
        config.set("plasticity.stdp.ltp_weight", 0.03f, ConfigSource::Runtime,
                  "Higher LTP for exploration", "Plasticity", true);
        config.set("plasticity.stdp.ltd_weight", 0.02f, ConfigSource::Runtime,
                  "Higher LTD for exploration", "Plasticity", true);
        return config;
    }, py::arg("config"), "Enable exploration mode");
}

} // namespace nlm
