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

    // Enhanced convenience methods for AgentBrain
    .def("getActionName", [](const AgentBrain& agent) {
        MotorCommand cmd = agent.decodeMotorCommand();
        switch(cmd) {
            case MotorCommand::MoveForward: return std::string("MoveForward");
            case MotorCommand::MoveBackward: return std::string("MoveBackward");
            case MotorCommand::TurnLeft: return std::string("TurnLeft");
            case MotorCommand::TurnRight: return std::string("TurnRight");
            case MotorCommand::LookLeft: return std::string("LookLeft");
            case MotorCommand::LookRight: return std::string("LookRight");
            case MotorCommand::Interact: return std::string("Interact");
            case MotorCommand::Wait: return std::string("Wait");
            default: return std::string("Unknown");
        }
    }, "Get action name from current motor command")
    .def("getStateSummary", [](const AgentBrain& agent) {
        auto brain = agent.getBrain();
        if (!brain) return std::string("No brain available");
        
        std::stringstream ss;
        ss << "Neurons: " << brain->getTotalNeuronCount()
           << ", Firing: " << brain->getFiringNeuronCount()
           << ", Total Spikes: " << brain->getTotalSpikeCount()
           << ", Stage: " << static_cast<int>(brain->getDevelopmentalStage())
           << ", Curiosity: " << agent.getCuriosityLevel()
           << ", Neuromodulation: " << agent.getNeuromodulationLevel();
        return ss.str();
    }, "Get brain state summary")
    .def("run_simulation_episode", [](AgentBrain& agent, std::shared_ptr<SimpleWorld> world, 
                                      size_t num_steps, bool auto_save = true) {
        if (!world) throw std::runtime_error("World is null");
        
        for (size_t step = 0; step < num_steps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            agent.processSensoryInput(percept);
            
            // Step brain
            auto brain = agent.getBrain();
            if (brain) brain->step(step);
            
            // Get and apply action
            auto action = agent.decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Update development
            agent.updateDevelopment(0.1);
            
            // Apply reward
            auto reward = agent.getNeuromodulationLevel();
            agent.applyRewardModulation(reward, 0.0);
        }
        
        if (auto_save && world->getAgentBody()) {
            auto brain = agent.getBrain();
            if (brain) {
                std::string filename = "agent_checkpoint_" + 
                    std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".bin";
                brain->save(filename);
            }
        }
    }, py::arg("world"), py::arg("num_steps"), py::arg("auto_save") = true,
         "Run a complete simulation episode with world interaction")
    .def("create_training_session", [](std::shared_ptr<Config> config, 
                                       std::shared_ptr<SimpleWorld> world,
                                       size_t num_episodes) {
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        
        agent->initialize(*world);
        brain->initialize();
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("config"), py::arg("world"), py::arg("num_episodes"),
         "Create a complete training session with brain, agent, and world")
    .def("run_quick_benchmark", [](size_t timesteps) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto start = std::chrono::steady_clock::now();
        for (size_t i = 0; i < timesteps; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::steady_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::map<std::string, double> stats;
        stats["timesteps"] = static_cast<double>(timesteps);
        stats["duration_ms"] = static_cast<double>(duration.count());
        stats["timesteps_per_second"] = (timesteps * 1000.0) / duration.count();
        stats["neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        
        return stats;
    }, py::arg("timesteps") = 1000,
         "Run a quick benchmark and return performance statistics");

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

    // High-level convenience functions
    m.def("run_quick_test", [](size_t timesteps) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (size_t i = 0; i < timesteps; ++i) {
            brain->step(i);
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("timesteps") = 1000,
         "Run a quick test and return total spike count");

    m.def("benchmark_performance", [](size_t timesteps) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto start = std::chrono::steady_clock::now();
        for (size_t i = 0; i < timesteps; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::steady_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        double fps = (timesteps * 1000.0) / duration.count();
        double neurons_per_second = (brain->getTotalNeuronCount() * fps) / 1000.0;
        
        std::map<std::string, double> results;
        results["total_time_ms"] = static_cast<double>(duration.count());
        results["fps"] = fps;
        results["timesteps_per_second"] = fps;
        results["neurons_per_second"] = neurons_per_second;
        results["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        
        return results;
    }, py::arg("timesteps") = 5000,
         "Run a performance benchmark and return timing statistics");

// Experiment management and metrics
    py::class_<Experiment>(m, "Experiment", R"pbdoc(Experiment data recording system)pbdoc")
        .def(py::init<>())
        .def(py::init<std::string>(), py::arg("name"))
        .def("getName", &Experiment::getName)
        .def("setName", &Experiment::setName, py::arg("name"))
        .def("getSeed", &Experiment::getSeed)
        .def("setSeed", &Experiment::setSeed, py::arg("seed"))
        .def("setConfigValue", &Experiment::setConfigValue, py::arg("key"), py::arg("value"))
        .def("getConfigValue", &Experiment::getConfigValue, py::arg("key"))
        .def("recordMetric", [](Experiment& exp, const std::string& name, float value) {
            exp.recordMetric(name, value);
        }, py::arg("name"), py::arg("value"))
        .def("recordMetric", [](Experiment& exp, const std::string& name, double value) {
            exp.recordMetric(name, value);
        }, py::arg("name"), py::arg("value"))
        .def("recordMetric", [](Experiment& exp, const std::string& name, int value) {
            exp.recordMetric(name, value);
        }, py::arg("name"), py::arg("value"))
        .def("getMetrics", &Experiment::getMetrics)
        .def("getMetric", &Experiment::getMetric, py::arg("name"))
        .def("hasMetric", &Experiment::hasMetric, py::arg("name"))
        .def("recordStep", &Experiment::recordStep,
             py::arg("step"), py::arg("neuronCount"), py::arg("synapseCount"),
             py::arg("firingCount"), py::arg("avgFiringRate"))
        .def("getStepData", &Experiment::getStepData)
        .def("getStartStep", &Experiment::getStartStep)
        .def("setStartStep", &Experiment::setStartStep, py::arg("step"))
        .def("getEndStep", &Experiment::getEndStep)
        .def("setEndStep", &Experiment::setEndStep, py::arg("step"))
        .def("addNote", &Experiment::addNote, py::arg("note"))
        .def("getNotes", &Experiment::getNotes)
        .def("toDict", [](const Experiment& exp) {
            py::dict result;
            result["name"] = exp.getName();
            result["seed"] = exp.getSeed();
            
            py::list metrics_list;
            for (const auto& metric : exp.getMetrics()) {
                py::dict metric_dict;
                metric_dict["name"] = metric.first;
                metric_dict["value"] = metric.second;
                metrics_list.append(metric_dict);
            }
            result["metrics"] = metrics_list;
            
            py::list steps_list;
            for (const auto& step : exp.getStepData()) {
                py::dict step_dict;
                step_dict["step"] = step.step;
                step_dict["neuronCount"] = step.neuronCount;
                step_dict["synapseCount"] = step.synapseCount;
                step_dict["firingCount"] = step.firingCount;
                step_dict["avgFiringRate"] = step.avgFiringRate;
                steps_list.append(step_dict);
            }
            result["steps"] = steps_list;
            
            py::list notes_list;
            for (const auto& note : exp.getNotes()) {
                notes_list.append(note);
            }
            result["notes"] = notes_list;
            
            return result;
        }, "Export experiment data to Python dictionary");

    // Phase 6 experiment integration
    py::class_<Phase6IntegrationResult>(m, "Phase6IntegrationResult", R"pbdoc(Phase 6 integration test results)pbdoc")
        .def_readonly("memoryWorkingMemoryIntegrated", &Phase6IntegrationResult::memoryWorkingMemoryIntegrated)
        .def_readonly("memoryEpisodicMemoryIntegrated", &Phase6IntegrationResult::memoryEpisodicMemoryIntegrated)
        .def_readonly("neuromodulationIntegrated", &Phase6IntegrationResult::neuromodulationIntegrated)
        .def_readonly("predictionIntegrated", &Phase6IntegrationResult::predictionIntegrated)
        .def_readonly("developmentIntegrated", &Phase6IntegrationResult::developmentIntegrated)
        .def_readonly("checkpointingWorks", &Phase6IntegrationResult::checkpointingWorks)
        .def_readonly("replayWorks", &Phase6IntegrationResult::replayWorks)
        .def_readonly("totalReward", &Phase6IntegrationResult::totalReward)
        .def_readonly("avgFiringRate", &Phase6IntegrationResult::avgFiringRate)
        .def_readonly("avgSynapticWeight", &Phase6IntegrationResult::avgSynapticWeight)
        .def_readonly("memoryEpisodesStored", &Phase6IntegrationResult::memoryEpisodesStored)
        .def_readonly("noveltyLevel", &Phase6IntegrationResult::noveltyLevel)
        .def_readonly("curiosityLevel", &Phase6IntegrationResult::curiosityLevel)
        .def_readonly("dopamineLevel", &Phase6IntegrationResult::dopamineLevel)
        .def_readonly("totalWallClockTime", &Phase6IntegrationResult::totalWallClockTime)
        .def("getSummary", [](const Phase6IntegrationResult& result) {
            std::stringstream ss;
            ss << "=== PHASE 6 INTEGRATION RESULTS ===" << std::endl;
            ss << "Memory Systems:" << std::endl;
            ss << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            ss << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            ss << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            ss << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            ss << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            ss << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
            ss << "Replay: " << (result.replayWorks ? "FUNCTIONAL" : "FAILED") << std::endl;
            ss << std::endl;
            ss << "Performance:" << std::endl;
            ss << "  Total Reward: " << result.totalReward << std::endl;
            ss << "  Average Firing Rate: " << result.avgFiringRate << std::endl;
            ss << "  Average Synaptic Weight: " << result.avgSynapticWeight << std::endl;
            ss << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
            ss << "  Novelty Level: " << result.noveltyLevel << std::endl;
            ss << "  Curiosity Level: " << result.curiosityLevel << std::endl;
            ss << "  Dopamine Level: " << result.dopamineLevel << std::endl;
            ss << "  Total Time: " << result.totalWallClockTime << "s" << std::endl;
            return ss.str();
        }, "Get formatted result summary");

    py::class_<Phase6Config>(m, "Phase6Config", R"pbdoc(Phase 6 experiment configuration)pbdoc")
        .def(py::init<>())
        .def_readwrite("maxSteps", &Phase6Config::maxSteps)
        .def_readwrite("neuronCount", &Phase6Config::neuronCount)
        .def_readwrite("regionCount", &Phase6Config::regionCount)
        .def_readwrite("connectionProbability", &Phase6Config::connectionProbability)
        .def_readwrite("enableCheckpointing", &Phase6Config::enableCheckpointing)
        .def_readwrite("enableReplay", &Phase6Config::enableReplay)
        .def_readwrite("enableDevelopment", &Phase6Config::enableDevelopment)
        .def_readwrite("checkpointPath", &Phase6Config::checkpointPath)
        .def("toDict", [](const Phase6Config& config) {
            py::dict result;
            result["maxSteps"] = config.maxSteps;
            result["neuronCount"] = config.neuronCount;
            result["regionCount"] = config.regionCount;
            result["connectionProbability"] = config.connectionProbability;
            result["enableCheckpointing"] = config.enableCheckpointing;
            result["enableReplay"] = config.enableReplay;
            result["enableDevelopment"] = config.enableDevelopment;
            result["checkpointPath"] = config.checkpointPath;
            return result;
        }, "Export configuration to Python dictionary")
        .def("fromDict", [](py::dict settings) {
            Phase6Config config;
            if (settings.contains("maxSteps")) {
                config.maxSteps = py::cast<size_t>(settings["maxSteps"]);
            }
            if (settings.contains("neuronCount")) {
                config.neuronCount = py::cast<size_t>(settings["neuronCount"]);
            }
            if (settings.contains("regionCount")) {
                config.regionCount = py::cast<size_t>(settings["regionCount"]);
            }
            if (settings.contains("connectionProbability")) {
                config.connectionProbability = py::cast<float>(settings["connectionProbability"]);
            }
            if (settings.contains("enableCheckpointing")) {
                config.enableCheckpointing = py::cast<bool>(settings["enableCheckpointing"]);
            }
            if (settings.contains("enableReplay")) {
                config.enableReplay = py::cast<bool>(settings["enableReplay"]);
            }
            if (settings.contains("enableDevelopment")) {
                config.enableDevelopment = py::cast<bool>(settings["enableDevelopment"]);
            }
            if (settings.contains("checkpointPath")) {
                config.checkpointPath = py::cast<std::string>(settings["checkpointPath"]);
            }
            return config;
        }, py::arg("settings"), "Create configuration from Python dictionary");

    // High-level phase 6 experiment functionality
    m.def("run_phase6_experiment", [](const Phase6Config& config) {
        Phase6IntegratedExperiment experiment;
        return experiment.run(config);
    }, py::arg("config"), "Run Phase 6 integration experiment");

    m.def("verify_phase6_integration", []() {
        Phase6IntegratedExperiment experiment;
        return experiment.verifyIntegration();
    }, "Verify Phase 6 integration");

    m.def("test_phase6_systems", []() {
        Phase6IntegratedExperiment experiment;
        bool memory_ok = experiment.testMemoryIntegration();
        bool neuromodulation_ok = experiment.testNeuromodulationIntegration();
        bool checkpoint_ok = experiment.testCheckpointing();
        bool replay_ok = experiment.testReplay();
        
        return std::make_tuple(memory_ok, neuromodulation_ok, checkpoint_ok, replay_ok);
    }, "Test individual Phase 6 systems");

    // Visualization and plotting tools
    py::class_<VisualizationTools>(m, "VisualizationTools", R"pbdoc(Visualization and plotting tools for NLM)pbdoc")
        .def(py::init<>())
        .def("plot_neuron_activity", &VisualizationTools::plotNeuronActivity,
             py::arg("brain"), py::arg("output_file"),
             "Plot neuron firing activity over time")
        .def("plot_spike_raster", &VisualizationTools::plotSpikeRaster,
             py::arg("spikes"), py::arg("output_file"),
             "Create spike raster plot")
        .def("plot_connectivity_matrix", &VisualizationTools::plotConnectivityMatrix,
             py::arg("brain"), py::arg("output_file"),
             "Plot brain connectivity matrix")
        .def("plot_weight_distribution", &VisualizationTools::plotWeightDistribution,
             py::arg("brain"), py::arg("output_file"),
             "Plot synaptic weight distribution")
        .def("plot_learning_curve", &VisualizationTools::plotLearningCurve,
             py::arg("rewards"), py::arg("output_file"),
             "Plot learning curve from reward data")
        .def("plot_development_curve", &VisualizationTools::plotDevelopmentCurve,
             py::arg("brain"), py::arg("output_file"),
             "Plot brain development over time")
        .def("generate_brain_report", &VisualizationTools::generateBrainReport,
             py::arg("brain"), py::arg("output_file"),
             "Generate comprehensive brain analysis report")
        .def("save_screenshot", &VisualizationTools::saveScreenshot,
             py::arg("window_name"), py::arg("output_file"),
             "Save visualization screenshot")
        .def("export_data", &VisualizationTools::exportData,
             py::arg("brain"), py::arg("filename"),
             "Export brain data to CSV file");

    // Advanced configuration presets
    py::class_<ConfigPresets>(m, "ConfigPresets", R"pbdoc(Preset brain configurations for different use cases)pbdoc")
        .def(py::init<>())
        .def("get_default_config", &ConfigPresets::getDefaultConfig,
             "Get default configuration for basic testing")
        .def("get_minimal_config", &ConfigPresets::getMinimalConfig,
             "Get minimal configuration for fastest startup")
        .def("get_advanced_config", &ConfigPresets::getAdvancedConfig,
             "Get advanced configuration for detailed learning")
        .def("get_agent_config", &ConfigPresets::getAgentConfig,
             "Get agent-optimized configuration for world interaction")
        .def("get_development_config", &ConfigPresets::getDevelopmentConfig,
             "Get development-focused configuration with extended plasticity")
        .def("get_research_config", &ConfigPresets::getResearchConfig,
             "Get research-oriented configuration with extensive logging")
        .def("get_performance_config", &ConfigPresets::getPerformanceConfig,
             "Get performance-optimized configuration for benchmarking")
        .def("get_custom_config", [](const py::dict& settings) {
            return ConfigPresets::createCustomConfig(settings);
        }, py::arg("settings"), "Create custom configuration from settings dictionary");

    // Performance monitoring and profiling tools
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring and profiling for NLM simulations)pbdoc")
        .def(py::init<>())
        .def("start_monitoring", &PerformanceMonitor::startMonitoring,
             "Start performance monitoring")
        .def("stop_monitoring", &PerformanceMonitor::stopMonitoring,
             "Stop performance monitoring")
        .def("is_monitoring", &PerformanceMonitor::isMonitoring,
             "Check if monitoring is active")
        .def("record_timestep", &PerformanceMonitor::recordTimestep,
             py::arg("timestep"), py::arg("neuron_count"), py::arg("firing_count"),
             "Record performance data for a timestep")
        .def("get_performance_stats", &PerformanceMonitor::getPerformanceStats,
             "Get performance statistics");

    // High-level experiment configuration builder
    py::class_<ExperimentBuilder>(m, "ExperimentBuilder", R"pbdoc(High-level experiment configuration builder)pbdoc")
        .def(py::init<>())
        .def("set_brain_parameters", &ExperimentBuilder::setBrainParameters,
             py::arg("neuron_count"), py::arg("region_count"),
             "Set brain neuron and region parameters")
        .def("set_plasticity_parameters", &ExperimentBuilder::setPlasticityParameters,
             py::arg("stdp_enabled"), py::arg("hebbian_enabled"),
             "Set plasticity rule parameters")
        .def("set_neuromodulation_parameters", &ExperimentBuilder::setNeuromodulationParameters,
             py::arg("dopamine_enabled"), py::arg("curiosity_enabled"),
             "Set neuromodulation parameters")
        .def("set_development_parameters", &ExperimentBuilder::setDevelopmentParameters,
             py::arg("enable_development"), py::arg("plasticity_modifier"),
             "Set development parameters")
        .def("enable_checkpointing", &ExperimentBuilder::enableCheckpointing,
             py::arg("checkpoint_path"), "Enable checkpoint saving")
        .def("enable_replay", &ExperimentBuilder::enableReplay,
             "Enable memory replay system")
        .def("build_config", &ExperimentBuilder::buildConfig,
             "Build complete configuration from set parameters")
        .def("reset", &ExperimentBuilder::reset, "Reset builder to default state");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
