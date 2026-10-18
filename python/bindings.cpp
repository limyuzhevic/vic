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

// Version information
const char* getVersion() {
    return "0.1.0";
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";
    
    // Add version information
    m.attr("__version__") = getVersion();
    
    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    
    // Error type mappings for better Python error handling
    py::implicitly_convertible<ConfigValue, std::string>();
    py::implicitly_convertible<std::string, ConfigValue>();

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

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience functions for easier Python usage
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, int numSteps) {
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        if (agent && world) {
            agent->initialize(*world);
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
        }
        
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"), 
        "Run a complete simulation with optional world and agent integration");

    m.def("create_and_run_brain", [](int neuronCount = 100, int numSteps = 100) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
        config->set("brain.region_count", static_cast<int64_t>(1));
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        
        return brain;
    }, py::arg("neuron_count") = 100, py::arg("num_steps") = 100,
        "Create and run a simple brain simulation, returning the brain object");

    m.def("get_brain_summary", [](const std::shared_ptr<Brain>& brain) {
        std::ostringstream oss;
        oss << "Brain Summary:\n";
        oss << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        oss << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
        oss << "  Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        oss << "  Average Firing Rate: " << brain->getAverageFiringRate() << "\n";
        oss << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        oss << "  Total Spike Count: " << brain->getTotalSpikeCount() << "\n";
        return oss.str();
    }, py::arg("brain"), "Get a human-readable summary of brain statistics");

    m.def("quick_test", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Quick test: Failed to initialize brain");
        }
        
        for (int step = 0; step < 10; ++step) {
            brain->step(step);
        }
        
        return true;
    }, "Run a quick test to verify bindings are working");

    // Add convenience constructor for World configuration
    m.def("configure_world", [](std::shared_ptr<SimpleWorld> world, int width, int height, 
                               int visionWidth, int visionHeight) {
        if (world) {
            world->configure(width, height, visionWidth, visionHeight);
        }
        return world;
    }, py::arg("world"), py::arg("width"), py::arg("height"), 
        py::arg("vision_width") = 8, py::arg("vision_height") = 8,
        "Configure a simple world with dimensions and vision settings");

    m.def("add_resource_to_world", [](std::shared_ptr<SimpleWorld> world, float x, float y, 
                                     float value = 1.0f) {
        if (world) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            world->addObject(obj);
        }
        return world;
    }, py::arg("world"), py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
        "Add a resource object to the world at specified coordinates");

    m.def("set_agent_start_position", [](std::shared_ptr<SimpleWorld> world, float x, float y) {
        if (world) {
            world->setAgentStart(x, y);
        }
        return world;
    }, py::arg("world"), py::arg("x"), py::arg("y"),
        "Set the agent's starting position in the world");

    // Add quick creation helpers for common configurations
    m.def("create_simple_brain", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(100));
        config->set("brain.region_count", static_cast<int64_t>(1));
        config->set("brain.connection_probability", 0.1);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a simple brain with default settings");

    m.def("create_minimal_world", [](int width = 10, int height = 10) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->reset();
        world->setAgentStart(width/2.0f, height/2.0f);
        return world;
    }, py::arg("width") = 10, py::arg("height") = 10,
        "Create a minimal world with default settings");

    m.def("create_complete_agent", [](int neuronCount = 100, int worldSize = 10) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldSize, worldSize, 8, 8);
        world->reset();
        world->setAgentStart(worldSize/2.0f, worldSize/2.0f);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable useful features by default
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("neuron_count") = 100, py::arg("world_size") = 10,
        "Create a complete agent setup with brain, world, and agent interface");

    // Add module exports for better Python import control
    m.def("__all__", []() {
        return py::list({
            "Brain",
            "Config", 
            "AgentBrain",
            "SimpleWorld",
            "Vision",
            "Audio",
            "InternalSignals",
            "Action",
            "WorldObject",
            "AgentBody",
            "ActionResult",
            "SensoryPercept",
            "NeuronId",
            "SynapseId",
            "RegionId",
            "PopulationId",
            "NeuronType",
            "SynapseType",
            "DevelopmentalStage",
            "FiringState",
            "ActionType",
            "MotorCommand",
            "WorldObjectType",
            "createDefaultConfig",
            "createBrain",
            "createSimpleWorld",
            "createAgentBrain",
            "run_simulation",
            "create_and_run_brain",
            "get_brain_summary",
            "quick_test",
            "configure_world",
            "add_resource_to_world",
            "set_agent_start_position",
            "create_simple_brain",
            "create_minimal_world",
            "create_complete_agent",
            "INVALID_NEURON_ID",
            "INVALID_SYNAPSE_ID",
            "INVALID_REGION_ID",
            "INVALID_POPULATION_ID",
            "__version__"
        });
    }, "Get list of all exported symbols for this module");

    m.attr("__all__") = m.attr("__all__")();

    // Add better error handling and type safety
    py::class_<std::runtime_error>(m, "RuntimeError")
        .def(py::init<const char*>());
    
    py::class_<std::invalid_argument>(m, "InvalidArgumentError")
        .def(py::init<const char*>());
    
    py::class_<std::out_of_range>(m, "OutOfRangeError")
        .def(py::init<const char*>());
    
    // Add function overloading for type conversion safety
    m.def("safe_get_int", [](const std::shared_ptr<Config>& config, const std::string& key, int defaultValue) {
        auto val = config->get<int>(key);
        return val.has_value() ? val.value() : defaultValue;
    }, py::arg("config"), py::arg("key"), py::arg("default_value"),
        "Safely get an integer value from config with default");

    m.def("safe_get_string", [](const std::shared_ptr<Config>& config, const std::string& key, const std::string& defaultValue) {
        auto val = config->get<std::string>(key);
        return val.has_value() ? val.value() : defaultValue;
    }, py::arg("config"), py::arg("key"), py::arg("default_value"),
        "Safely get a string value from config with default");

    m.def("safe_get_double", [](const std::shared_ptr<Config>& config, const std::string& key, double defaultValue) {
        auto val = config->get<double>(key);
        return val.has_value() ? val.value() : defaultValue;
    }, py::arg("config"), py::arg("key"), py::arg("default_value"),
        "Safely get a double value from config with default");

    m.def("safe_get_bool", [](const std::shared_ptr<Config>& config, const std::string& key, bool defaultValue) {
        auto val = config->get<bool>(key);
        return val.has_value() ? val.value() : defaultValue;
    }, py::arg("config"), py::arg("key"), py::arg("default_value"),
        "Safely get a boolean value from config with default");

    // Add validation functions
    m.def("validate_config", [](const std::shared_ptr<Config>& config, const std::vector<std::string>& requiredKeys) {
        std::vector<std::string> missingKeys;
        for (const auto& key : requiredKeys) {
            if (!config->has(key)) {
                missingKeys.push_back(key);
            }
        }
        
        if (!missingKeys.empty()) {
            std::string errorMsg = "Missing required configuration keys: ";
            for (size_t i = 0; i < missingKeys.size(); ++i) {
                if (i > 0) errorMsg += ", ";
                errorMsg += "'" + missingKeys[i] + "'";
            }
            throw std::invalid_argument(errorMsg);
        }
        
        return true;
    }, py::arg("config"), py::arg("required_keys"),
        "Validate that config contains all required keys");

    // Add brain statistics helper
    m.def("get_detailed_stats", [](const std::shared_ptr<Brain>& brain) {
        std::ostringstream oss;
        oss << "=== BRAIN STATISTICS ===\n";
        oss << "Neurons: " << brain->getTotalNeuronCount() << "\n";
        oss << "Synapses: " << brain->getTotalSynapseCount() << "\n";
        oss << "Active Neurons: " << brain->getActiveNeuronCount() << "\n";
        oss << "Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        oss << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        oss << "Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        oss << "Total Spike Count: " << brain->getTotalSpikeCount() << "\n";
        oss << "Pending Spike Events: " << brain->getPendingSpikeEventCount() << "\n";
        oss << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        return oss.str();
    }, py::arg("brain"), "Get detailed brain statistics as a formatted string");

    // Add memory usage monitoring
    m.def("get_memory_usage", []() {
        std::ostringstream oss;
        oss << "Memory Usage Information:\n";
        oss << "  Note: This is a placeholder for memory monitoring.\n";
        oss << "  In production, this would report actual memory consumption.\n";
        return oss.str();
    }, "Get memory usage information (placeholder)");

    // Add performance benchmarking helpers
    m.def("benchmark_brain_operations", [](std::shared_ptr<Brain> brain, int steps, int warmupSteps = 10) {
        // Warm up
        for (int i = 0; i < warmupSteps; ++i) {
            brain->step(i);
        }
        
        // Benchmark
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            brain->step(i + warmupSteps);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        double avgTimePerStep = static_cast<double>(duration.count()) / steps;
        
        std::ostringstream oss;
        oss << "Benchmark Results:\n";
        oss << "  Steps: " << steps << "\n";
        oss << "  Total time: " << duration.count() << " ms\n";
        oss << "  Average time per step: " << avgTimePerStep << " ms\n";
        oss << "  Steps per second: " << (steps * 1000.0 / duration.count()) << "\n";
        
        return oss.str();
    }, py::arg("brain"), py::arg("steps"), py::arg("warmup_steps") = 10,
        "Benchmark brain operations and return performance statistics");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
