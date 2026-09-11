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

    // Add enhanced error handling to key functions for better user experience
    // Error handling for Config class
    
    // Wrap key functions to provide better error messages
    auto originalGet = &Config::get<int>;
    auto wrappedGet = [](const Config& self, const std::string& key) -> std::optional<int> {
        try {
            return originalGet(self, key);
        } catch (const std::exception& e) {
            throw py::value_error("Error getting integer value for key '" + key + "': " + e.what());
        }
    };
    
    // Add validation to key methods
    auto originalSet = &Config::set;
    auto wrappedSet = [](Config& self, const std::string& key, const ConfigValue& value) {
        try {
            originalSet(self, key, value, ConfigSource::Runtime);
        } catch (const std::exception& e) {
            throw py::value_error("Error setting value for key '" + key + "': " + e.what());
        }
    };
    
    // Wrap getOr for better error handling
    auto originalGetOr = &Config::getOr<int>;
    auto wrappedGetOr = [](const Config& self, const std::string& key, int defaultValue) -> int {
        try {
            return originalGetOr(self, key, defaultValue);
        } catch (const std::exception& e) {
            throw py::value_error("Error getting value with default for key '" + key + "': " + e.what());
        }
    };
    
    // Wrap has for validation
    auto originalHas = &Config::has;
    auto wrappedHas = [](const Config& self, const std::string& key) -> bool {
        return originalHas(self, key);
    };
    
    // Register enhanced config methods
    py::class_<Config> enhancedConfig = py::class_<Config>(m, "ConfigEnhanced", R"pbdoc(Enhanced Configuration class with improved error handling)pbdoc");
    enhancedConfig
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file with enhanced error handling")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file with enhanced error handling")
        .def("has", wrappedHas, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](const Config& self, const std::string& key) {
            // Try all types automatically
            if (auto val = self.get<int>(key)) return py::cast(*val);
            if (auto val = self.get<double>(key)) return py::cast(*val);
            if (auto val = self.get<bool>(key)) return py::cast(*val);
            if (auto val = self.get<std::string>(key)) return py::cast(*val);
            return py::none();
        }, py::arg("key"), "Get configuration value with automatic type detection")
        .def("getOr", wrappedGetOr, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default, with enhanced error handling")
        .def("set", wrappedSet, py::arg("key"), py::arg("value"),
             "Set configuration value with enhanced error handling")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration with error information")
        .def("__repr__", [](const Config& cfg) {
            return "<ConfigEnhanced: " + cfg.summary() + ">";
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

    // ========== FACTORY FUNCTIONS (MAIN ENTRY POINTS) ==========
    
    // Simplified factory for common use cases
    m.def("createBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create a new brain with default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with specific configuration");
    
    // World factory with presets
    m.def("createWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        return world;
    }, "Create a new world with default settings");
    
    m.def("createWorld", [](size_t width, size_t height, 
                          size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(static_cast<float>(width), static_cast<float>(height), 
                        visionWidth, visionHeight);
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20,
       py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
       "Create a new world with custom dimensions");
    
    // Agent factory with convenience methods
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface");
    
    // ========== ADDITIONAL FACTORY FUNCTIONS ==========
    
    // Quick setup for simple demonstrations
    m.def("createDemoSetup", []() {
        py::dict result;
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        
        // Basic setup
        world->configure(20.0f, 20.0f, 16, 16);
        agent->initialize(*world);
        
        // Enable core subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Store initialized components
        result["brain"] = brain;
        result["agent"] = agent;
        result["world"] = world;
        
        // Run quick test
        for (int step = 0; step < 50; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return result;
    }, "Create a demo setup with brain, agent, and world, and run initial steps");

    // Complete simulation runner with statistics
    m.def("runSimulation", [](int numSteps = 1000, bool verbose = false) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        
        world->configure(20.0f, 20.0f, 16, 16);
        world->setAgentStart(10.0f, 10.0f);
        
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        py::dict history;
        std::vector<py::dict> steps;
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            auto result = world->applyMotorCommand(action, world->getSimulationTime());
            
            // Store step data
            py::dict stepData;
            stepData["step"] = step;
            stepData["time"] = world->getSimulationTime();
            stepData["reward"] = result.reward;
            stepData["success"] = result.success;
            stepData["firing_neurons"] = static_cast<size_t>(brain->getFiringNeuronCount());
            stepData["total_spikes"] = static_cast<size_t>(brain->getTotalSpikeCount());
            stepData["curiosity"] = agent->getCuriosityLevel();
            stepData["novelty"] = agent->getNoveltyLevel();
            stepData["action"] = static_cast<int>(action);
            
            steps.push_back(stepData);
            
            if (verbose && step % 100 == 0) {
                py::print(f"Step {step}: {stepData["firing_neurons"]} firing, "
                         f"{agent->getCuriosityLevel():.3f} curiosity");
            }
        }
        
        history["steps"] = steps;
        
        // Final statistics
        py::dict finalStats;
        finalStats["total_steps"] = numSteps;
        finalStats["final_spikes"] = static_cast<size_t>(brain->getTotalSpikeCount());
        finalStats["final_curiosity"] = agent->getCuriosityLevel();
        finalStats["final_novelty"] = agent->getNoveltyLevel();
        finalStats["final_stage"] = static_cast<int>(agent->getDevelopmentalStage());
        
        // Calculate summary statistics
        if (!steps.empty()) {
            size_t totalFiring = 0;
            float totalCuriosity = 0.0f;
            for (const auto& step : steps) {
                totalFiring += py::cast<size_t>(step["firing_neurons"]);
                totalCuriosity += py::cast<float>(step["curiosity"]);
            }
            
            finalStats["avg_firing"] = static_cast<double>(totalFiring) / steps.size();
            finalStats["avg_curiosity"] = totalCuriosity / steps.size();
        }
        
        history["stats"] = finalStats;
        return history;
    }, py::arg("numSteps") = 1000, py::arg("verbose") = false,
       "Run a complete simulation and return detailed statistics and history");

    // Create world with objects
    m.def("createWorldWithObjects", [](const std::vector<py::dict>& objectConfigs) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        
        for (const auto& config : objectConfigs) {
            WorldObject obj;
            obj.x = py::cast<float>(config["x"]);
            obj.y = py::cast<float>(config["y"]);
            obj.radius = py::cast<float>(config["radius"]);
            obj.type = py::cast<WorldObjectType>(config["type"]);
            obj.value = py::cast<float>(config["value"]);
            obj.active = true;
            
            world->addObject(obj);
        }
        
        return world;
    }, py::arg("objectConfigs"), "Create a world with custom objects");

    // Get default configurations as Python dictionaries
    m.def("getConfigTemplates", []() {
        py::dict templates;
        
        // Small brain for quick testing
        py::dict smallBrain;
        smallBrain["neuron_count"] = 500;
        smallBrain["region_count"] = 1;
        smallBrain["connection_probability"] = 0.05f;
        templates["small"] = smallBrain;
        
        // Medium brain for balanced simulation
        py::dict mediumBrain;
        mediumBrain["neuron_count"] = 2000;
        mediumBrain["region_count"] = 2;
        mediumBrain["connection_probability"] = 0.1f;
        templates["medium"] = mediumBrain;
        
        // Large brain for complex simulations
        py::dict largeBrain;
        largeBrain["neuron_count"] = 10000;
        largeBrain["region_count"] = 4;
        largeBrain["connection_probability"] = 0.15f;
        templates["large"] = largeBrain;
        
        return templates;
    }, "Get predefined configuration templates");

    // Utility: create sensory input for testing
    m.def("createTestSensory", [](const std::string& type, const py::dict& data) {
        if (type == "vision") {
            std::vector<float> visionData;
            if (data.contains("width") && data.contains("height")) {
                size_t width = py::cast<size_t>(data["width"]);
                size_t height = py::cast<size_t>(data["height"]);
                visionData.resize(width * height, 0.0f);
                
                if (data.contains("value")) {
                    float value = py::cast<float>(data["value"]);
                    for (size_t i = 0; i < visionData.size(); ++i) {
                        visionData[i] = value;
                    }
                }
            }
            return SensoryInputPtr(std::make_shared<Vision>(visionData));
        } else if (type == "internal") {
            std::vector<float> internalData(4, 0.0f);
            if (data.contains("energy")) internalData[0] = py::cast<float>(data["energy"]);
            if (data.contains("health")) internalData[1] = py::cast<float>(data["health"]);
            if (data.contains("moving")) internalData[2] = py::cast<float>(data["moving"]);
            if (data.contains("turning")) internalData[3] = py::cast<float>(data["turning"]);
            
            return SensoryInputPtr(std::make_shared<InternalSignals>(internalData));
        }
        return SensoryInputPtr(std::make_shared<SensoryInput>());
    }, py::arg("type"), py::arg("data"), "Create test sensory input for testing");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
