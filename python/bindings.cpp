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
    
    // Additional utility functions for Python
    m.def("configureBrainForAgent", [](std::shared_ptr<Brain> brain, 
                                      const std::vector<float>& vision,
                                      const std::vector<float>& touch,
                                      const std::vector<float>& internal,
                                      const std::vector<float>& proprioception) {
        
        // Configure brain with optimal settings for agent usage
        auto config = brain->getConfig();
        
        // Set based on input dimensions
        size_t visionSize = vision.empty() ? 256 : vision.size();
        size_t touchSize = touch.empty() ? 8 : touch.size();
        size_t internalSize = internal.empty() ? 4 : internal.size();
        size_t proprioceptionSize = proprioception.empty() ? 6 : proprioception.size();
        
        // Calculate neuron count based on total sensory input
        size_t totalInput = visionSize + touchSize + internalSize + proprioceptionSize;
        config->set("neuron_count", static_cast<int64_t>(std::min<size_t>(totalInput * 10, 5000)), ConfigSource::Default);
        
        // Update brain config
        brain->reset();
        brain->initialize();
        
        return true;
    }, py::arg("brain"), py::arg("vision"), py::arg("touch"), 
           py::arg("internal"), py::arg("proprioception"),
           "Configure brain for agent use with given sensory input sizes");

    m.def("createDefaultAgentConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Set sensible defaults for agent use
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Agent-specific settings
        config->set("enable_reward_modulation", true, ConfigSource::Default);
        config->set("enable_structural_plasticity", true, ConfigSource::Default);
        config->set("enable_development", true, ConfigSource::Default);
        config->set("enable_curiosity", true, ConfigSource::Default);
        
        return config;
    }, "Create a default configuration optimized for agent use");

    m.def("initializeAgentBrain", [](std::shared_ptr<AgentBrain> agent, 
                                    std::shared_ptr<SimpleWorld> world,
                                    size_t visionWidth, size_t visionHeight) {
        
        // Configure world with agent expectations
        world->configure(visionWidth, visionHeight, visionWidth, visionHeight);
        world->reset();
        
        // Initialize agent with world
        agent->initialize(*world);
        
        return true;
    }, py::arg("agent"), py::arg("world"), py::arg("visionWidth"), 
           py::arg("visionHeight"), "Initialize agent brain with configured world");

    m.def("runAgentEpisode", [](std::shared_ptr<Brain> brain,
                               std::shared_ptr<SimpleWorld> world,
                               std::shared_ptr<AgentBrain> agent,
                               size_t numSteps,
                               float maxTime) {
        
        // Initialize
        world->reset();
        agent->initialize(*world);
        
        float time = 0.0f;
        size_t steps = 0;
        
        while (steps < numSteps && time < maxTime) {
            // Update world
            world->update(0.1f);
            time += 0.1f;
            
            // Get sensory percept
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain thinking
            brain->step(steps, time);
            
            // Get action
            auto command = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(command, time);
            
            steps++;
        }
        
        return steps;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
           py::arg("numSteps"), py::arg("maxTime"), 
           "Run a complete agent episode with brain-world-agent integration");

    m.def("createSensoryPerceptFromVectors", [](const std::vector<float>& vision,
                                              const std::vector<float>& touch,
                                              const std::vector<float>& internal,
                                              const std::vector<float>& proprioception,
                                              const std::vector<float>& audio,
                                              size_t visionWidth, size_t visionHeight) {
        
        nlm::SensoryPercept percept(visionWidth, visionHeight,
                                   touch.size(), internal.size(),
                                   proprioception.size(), audio.size());
        
        percept.setVision(vision);
        percept.setTouch(touch);
        percept.setInternal(internal);
        percept.setProprioception(proprioception);
        percept.setAudio(audio);
        
        return percept;
    }, py::arg("vision"), py::arg("touch"), py::arg("internal"), 
           py::arg("proprioception"), py::arg("audio"),
           py::arg("visionWidth"), py::arg("visionHeight"),
           "Create a configurable SensoryPercept from vector data");

    m.def("getSensoryVectorInfo", [](const nlm::SensoryPercept& percept) {
        py::dict info;
        info["vision_width"] = percept.getVisionWidth();
        info["vision_height"] = percept.getVisionHeight();
        info["vision_area"] = percept.getVisionWidth() * percept.getVisionHeight();
        info["vision_size"] = percept.getVision().size();
        
        info["touch_size"] = percept.getTouch().size();
        info["internal_size"] = percept.getInternal().size();
        info["proprioception_size"] = percept.getProprioception().size();
        info["audio_size"] = percept.getAudio().size();
        
        info["total_input_size"] = percept.getTotalSensoryInputSize();
        info["is_valid"] = percept.validate();
        
        return info;
    }, py::arg("percept"), "Get information about a sensory percept configuration");

    m.def("createTestPerceptWithData", [](const std::string& testType) {
        nlm::SensoryPercept percept;
        
        if (testType == "simple") {
            // Simple uniform percept for testing
            std::vector<float> vision(256, 0.5f);
            std::vector<float> touch(8, 0.0f);
            std::vector<float> internal(4, 0.0f);
            std::vector<float> proprioception(6, 0.0f);
            std::vector<float> audio(0, 0.0f);
            
            percept.setVision(vision);
            percept.setTouch(touch);
            percept.setInternal(internal);
            percept.setProprioception(proprioception);
            percept.setAudio(audio);
            
        } else if (testType == "complex") {
            // Complex varied percept for testing
            std::vector<float> vision(256);
            std::vector<float> touch(8);
            std::vector<float> internal(4);
            std::vector<float> proprioception(6);
            std::vector<float> audio(4);
            
            for (size_t i = 0; i < 256; ++i) {
                vision[i] = static_cast<float>(i) / 255.0f;
            }
            
            for (size_t i = 0; i < 8; ++i) {
                touch[i] = static_cast<float>(i) / 7.0f;
            }
            
            std::fill(internal.begin(), internal.end(), 0.5f);
            std::fill(proprioception.begin(), proprioception.end(), 0.3f);
            std::fill(audio.begin(), audio.end(), 0.8f);
            
            percept.setVision(vision);
            percept.setTouch(touch);
            percept.setInternal(internal);
            percept.setProprioception(proprioception);
            percept.setAudio(audio);
        }
        
        return percept;
    }, py::arg("testType"), "Create a test sensory percept for testing (simple or complex)");

    m.def("createEnhancedAgent", [](std::shared_ptr<Brain> brain, 
                                   std::shared_ptr<SimpleWorld> world,
                                   bool enableLearning = true,
                                   bool enableExploration = true,
                                   bool enableDevelopment = true) {
        
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        
        // Configure world for this agent
        world->configure(20, 20, 10, 10);
        world->reset();
        
        // Initialize agent
        agent->initialize(*world);
        
        // Enable agent features based on parameters
        if (enableLearning) {
            agent->enableRewardModulation(true);
        }
        if (enableExploration) {
            agent->enableCuriosity(true);
        }
        if (enableDevelopment) {
            agent->enableDevelopment(true);
        }
        
        return agent;
    }, py::arg("brain"), py::arg("world"), py::arg("enableLearning") = true,
           py::arg("enableExploration") = true, py::arg("enableDevelopment") = true,
           "Create an enhanced agent with configurable features");

    m.def("runAgentLearningSimulation", [](std::shared_ptr<Brain> brain,
                                           std::shared_ptr<SimpleWorld> world,
                                           std::shared_ptr<AgentBrain> agent,
                                           size_t numEpisodes,
                                           size_t stepsPerEpisode) {
        
        float totalReward = 0.0f;
        size_t totalSteps = 0;
        
        for (size_t episode = 0; episode < numEpisodes; ++episode) {
            // Reset episode
            world->reset();
            agent->initialize(*world);
            agent->reset();
            
            float episodeReward = 0.0f;
            size_t episodeSteps = 0;
            
            while (episodeSteps < stepsPerEpisode) {
                // Update world
                world->update(0.1f);
                
                // Get sensory percept
                auto percept = world->getSensoryPercept();
                
                // Process sensory input
                agent->processSensoryInput(percept);
                
                // Brain thinking
                brain->step(totalSteps, totalSteps * 0.001f);
                
                // Get action
                auto command = agent->decodeMotorCommand();
                
                // Apply action and get result
                auto result = world->applyMotorCommand(command, totalSteps * 0.001f);
                episodeReward += result.reward;
                
                // Apply reward modulation
                agent->applyRewardModulation(result.reward, agent->getPredictionError());
                
                // Update development
                agent->updateDevelopment(0.1f);
                
                totalSteps++;
                episodeSteps++;
                totalReward += result.reward;
            }
            
            // End of episode
            agent->reset();
        }
        
        return std::make_pair(totalReward, totalSteps);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
           py::arg("numEpisodes"), py::arg("stepsPerEpisode"),
           "Run a complete agent learning simulation with multiple episodes");

    m.def("createOptimizedSensoryConfiguration", []() {
        py::dict config;
        
        // Vision configuration (grayscale, 8x8 for efficiency)
        config["vision_width"] = 8;
        config["vision_height"] = 8;
        config["vision_neurons"] = 64;
        
        // Touch configuration (binary proximity sensors)
        config["touch_sensors"] = 8;
        config["touch_neurons"] = 8;
        
        // Internal signals (homeostatic regulation)
        config["internal_signals"] = 4;
        config["internal_neurons"] = 4;
        
        // Proprioception (body state)
        config["proprioception_channels"] = 6;
        config["proprioception_neurons"] = 6;
        
        // Audio (if enabled)
        config["audio_channels"] = 2;
        config["audio_neurons"] = 4;
        
        // Total input size
        config["total_input_size"] = 64 + 8 + 4 + 6 + 4;
        
        return config;
    }, "Create an optimized sensory configuration dictionary");

    m.def("createBrainFromSensoryConfig", [](const py::dict& sensoryConfig) {
        auto config = std::make_shared<Config>();
        
        // Extract configuration
        size_t visionWidth = sensoryConfig["vision_width"].cast<size_t>();
        size_t visionHeight = sensoryConfig["vision_height"].cast<size_t>();
        size_t visionNeurons = sensoryConfig["vision_neurons"].cast<size_t>();
        
        size_t touchSensors = sensoryConfig["touch_sensors"].cast<size_t>();
        size_t touchNeurons = sensoryConfig["touch_neurons"].cast<size_t>();
        
        size_t internalSignals = sensoryConfig["internal_signals"].cast<size_t>();
        size_t internalNeurons = sensoryConfig["internal_neurons"].cast<size_t>();
        
        size_t proprioceptionChannels = sensoryConfig["proprioception_channels"].cast<size_t>();
        size_t proprioceptionNeurons = sensoryConfig["proprioception_neurons"].cast<size_t>();
        
        size_t audioChannels = sensoryConfig["audio_channels"].cast<size_t>();
        size_t audioNeurons = sensoryConfig["audio_neurons"].cast<size_t>();
        
        // Calculate total neuron count
        size_t totalNeurons = visionNeurons + touchNeurons + internalNeurons + 
                           proprioceptionNeurons + audioNeurons;
        
        // Set config based on sensory requirements
        config->set("neuron_count", static_cast<int64_t>(totalNeurons), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        return config;
    }, py::arg("sensoryConfig"), "Create a brain configuration from sensory configuration dictionary");

    m.def("createSensorFusionPercept", [](const std::vector<float>& visionData,
                                        const std::vector<float>& audioData,
                                        const std::vector<float>& touchData,
                                        size_t visionWidth = 16,
                                        size_t visionHeight = 16,
                                        size_t audioChannels = 2) {
        
        // Create percept with vision data
        nlm::SensoryPercept percept(visionWidth, visionHeight,
                                   touchData.size(), 4, 6, audioChannels);
        
        // Add vision data
        std::vector<float> vision = visionData;
        if (vision.size() < visionWidth * visionHeight) {
            vision.resize(visionWidth * visionHeight, 0.0f);
        }
        percept.setVision(vision);
        
        // Add audio data
        std::vector<float> audio = audioData;
        if (audio.size() < audioChannels * 2) {
            audio.resize(audioChannels * 2, 0.0f);
        }
        percept.setAudio(audio);
        
        // Add touch data
        std::vector<float> touch = touchData;
        if (touch.size() < 8) {
            touch.resize(8, 0.0f);
        }
        percept.setTouch(touch);
        
        // Add default internal and proprioception
        std::vector<float> internal(4, 0.0f);
        std::vector<float> proprioception(6, 0.0f);
        
        percept.setInternal(internal);
        percept.setProprioception(proprioception);
        
        return percept;
    }, py::arg("visionData"), py::arg("audioData"), py::arg("touchData"),
           py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
           py::arg("audioChannels") = 2,
           "Create a sensor fusion percept combining vision, audio, and touch data");

    m.def("printPerceptInfo", [](const nlm::SensoryPercept& percept) {
        std::cout << "Sensory Percept Configuration:" << std::endl;
        std::cout << "  Vision: " << percept.getVisionWidth() << "x" << percept.getVisionHeight() 
                  << " (" << percept.getVision().size() << " values)" << std::endl;
        std::cout << "  Touch: " << percept.getTouch().size() << " sensors" << std::endl;
        std::cout << "  Internal: " << percept.getInternal().size() << " signals" << std::endl;
        std::cout << "  Proprioception: " << percept.getProprioception().size() << " channels" << std::endl;
        std::cout << "  Audio: " << percept.getAudio().size() << " samples" << std::endl;
        std::cout << "  Total input size: " << percept.getTotalSensoryInputSize() << std::endl;
        std::cout << "  Is valid: " << (percept.validate() ? "Yes" : "No") << std::endl;
        
        return true;
    }, py::arg("percept"), "Print detailed information about a sensory percept");

    m.def("createSensoryPerceptFromArrays", [](py::array vision,
                                              py::array touch,
                                              py::array internal,
                                              py::array proprioception,
                                              py::array audio,
                                              size_t visionWidth = 16,
                                              size_t visionHeight = 16) {
        
        // Convert numpy arrays to vectors
        std::vector<float> vision_vec;
        std::vector<float> touch_vec;
        std::vector<float> internal_vec;
        std::vector<float> proprioception_vec;
        std::vector<float> audio_vec;
        
        // Process vision array
        auto vision_buf = vision.request();
        if (vision_buf.format == py::format_descriptor<float>::format()) {
            float* vision_data = static_cast<float*>(vision_buf.ptr);
            vision_vec.assign(vision_data, vision_data + vision_buf.size);
        }
        
        // Process touch array
        auto touch_buf = touch.request();
        if (touch_buf.format == py::format_descriptor<float>::format()) {
            float* touch_data = static_cast<float*>(touch_buf.ptr);
            touch_vec.assign(touch_data, touch_data + touch_buf.size);
        }
        
        // Process internal array
        auto internal_buf = internal.request();
        if (internal_buf.format == py::format_descriptor<float>::format()) {
            float* internal_data = static_cast<float*>(internal_buf.ptr);
            internal_vec.assign(internal_data, internal_data + internal_buf.size);
        }
        
        // Process proprioception array
        auto proprioception_buf = proprioception.request();
        if (proprioception_buf.format == py::format_descriptor<float>::format()) {
            float* proprioception_data = static_cast<float*>(proprioception_buf.ptr);
            proprioception_vec.assign(proprioception_data, proprioception_data + proprioception_buf.size);
        }
        
        // Process audio array
        auto audio_buf = audio.request();
        if (audio_buf.format == py::format_descriptor<float>::format()) {
            float* audio_data = static_cast<float*>(audio_buf.ptr);
            audio_vec.assign(audio_data, audio_data + audio_buf.size);
        }
        
        // Create percept with the data
        nlm::SensoryPercept percept(visionWidth, visionHeight,
                                   touch_vec.size(), internal_vec.size(),
                                   proprioception_vec.size(), audio_vec.size());
        
        percept.setVision(vision_vec);
        percept.setTouch(touch_vec);
        percept.setInternal(internal_vec);
        percept.setProprioception(proprioception_vec);
        percept.setAudio(audio_vec);
        
        return percept;
    }, py::arg("vision"), py::arg("touch"), py::arg("internal"),
           py::arg("proprioception"), py::arg("audio"),
           py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
           "Create a sensory percept from numpy arrays (NumPy support)");
}

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
