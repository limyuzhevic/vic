#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/eigen.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <numeric>

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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

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
        // getRegion is const/non-const overloaded; bind the mutable one.
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Default factories
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

    // Utility functions
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, 
                           int numSteps, double timestep = 0.1) -> py::dict {
        py::dict results;
        
        // Reset everything
        if (brain) brain->reset();
        if (world) world->reset();
        
        for (int step = 0; step < numSteps; ++step) {
            if (world) {
                world->update(timestep);
            }
            
            if (agent && world) {
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
            }
            
            if (brain) {
                brain->step(step);
            }
            
            if (agent && world) {
                auto motor_cmd = agent->decodeMotorCommand();
                world->applyMotorCommand(motor_cmd, world->getSimulationTime());
                
                // Apply reward modulation
                double reward = 0.0;
                if (auto percept = world->getSensoryPercept()) {
                    auto internal = percept->getInternal();
                    if (!internal.empty()) reward = internal[0];
                }
                agent->applyRewardModulation(reward, 0.0);
                agent->updateDevelopment(timestep);
            }
        }
        
        // Collect statistics
        results["steps_completed"] = numSteps;
        results["total_spikes"] = brain ? brain->getTotalSpikeCount() : 0;
        results["firing_neurons"] = brain ? brain->getFiringNeuronCount() : 0;
        results["average_firing_rate"] = brain ? brain->getAverageFiringRate() : 0.0;
        results["development_stage"] = brain ? static_cast<int>(brain->getDevelopmentalStage()) : 0;
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"), py::arg("timestep") = 0.1,
          "Run a complete simulation with brain, world, and agent");

    m.def("createExperiment", [](std::string name, std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                std::shared_ptr<AgentBrain> agent) -> py::dict {
        py::dict experiment;
        experiment["name"] = name;
        experiment["brain"] = brain;
        experiment["world"] = world;
        experiment["agent"] = agent;
        experiment["start_time"] = std::chrono::system_clock::now();
        experiment["parameters"] = py::dict();
        experiment["results"] = py::dict();
        return experiment;
    }, py::arg("name"), py::arg("brain"), py::arg("world"), py::arg("agent"),
          "Create an experiment for running and analyzing simulations");

    m.def("collectStatistics", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) -> py::dict {
        py::dict stats;
        
        if (brain) {
            stats["total_neurons"] = brain->getTotalNeuronCount();
            stats["total_synapses"] = brain->getTotalSynapseCount();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            stats["region_count"] = brain->getRegionCount();
            
            // Network metrics
            double excitation = 0.0, inhibition = 0.0;
            auto regions = brain->getRegions();
            for (const auto& region : regions) {
                for (const auto& syn : region->getSynapses()) {
                    if (syn->getType() == SynapseType::Excitatory) excitation += syn->getWeight();
                    else if (syn->getType() == SynapseType::Inhibitory) inhibition += syn->getWeight();
                }
            }
            stats["network_excitation"] = excitation;
            stats["network_inhibition"] = inhibition;
            stats["network_balance"] = (excitation + inhibition) > 0 ? excitation / (excitation + inhibition) : 0.5;
        }
        
        if (agent) {
            stats["curiosity_level"] = agent->getCuriosityLevel();
            stats["novelty_level"] = agent->getNoveltyLevel();
            stats["neuromodulation_level"] = agent->getNeuromodulationLevel();
            stats["prediction_error"] = agent->getPredictionError();
        }
        
        return stats;
    }, py::arg("brain"), py::arg("agent"), 
          "Collect comprehensive statistics from brain and agent");

    // Enhanced configuration utilities
    m.def("configureBrain", [](std::shared_ptr<Config> config, std::map<std::string, py::object> settings) -> std::shared_ptr<Config> {
        for (const auto& pair : settings) {
            std::string key = pair.first;
            py::object value = pair.second;
            
            if (value.is(py::int_())) {
                config->set(key, value.cast<int64_t>(), ConfigSource::User);
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<double>(), ConfigSource::User);
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>(), ConfigSource::User);
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>(), ConfigSource::User);
            }
        }
        return config;
    }, py::arg("config"), py::arg("settings"), 
          "Configure brain with a dictionary of settings");

    // Advanced world management
    m.def("createAndConfigureWorld", [](double width, double height, int visionWidth, int visionHeight, 
                                      double maxEnergy = 100.0, double energyDecayRate = 0.95) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->setMaxEnergy(maxEnergy);
        world->setEnergyDecayRate(energyDecayRate);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), 
          py::arg("maxEnergy") = 100.0, py::arg("energyDecayRate") = 0.95,
          "Create and configure a simple world");

    m.def("addResource", [](std::shared_ptr<SimpleWorld> world, double x, double y, double value = 1.0) {
        if (world) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            world->addObject(obj);
        }
    }, py::arg("world"), py::arg("x"), py::arg("y"), py::arg("value") = 1.0,
          "Add a resource object to the world");

    // Batch operations
    m.def("batchInjectCurrent", [](std::shared_ptr<Brain> brain, std::vector<std::pair<NeuronId, double>> injections, 
                                  SimulationStep currentStep) {
        for (const auto& pair : injections) {
            brain->injectCurrent(pair.first, pair.second);
        }
        brain->step(currentStep);
    }, py::arg("brain"), py::arg("injections"), py::arg("currentStep"),
          "Inject current to multiple neurons in batch");

    m.def("stepBatch", [](std::shared_ptr<Brain> brain, int numSteps, double timestep = 0.1) -> std::vector<py::dict> {
        std::vector<py::dict> results;
        
        for (int step = 0; step < numSteps; ++step) {
            py::dict step_result;
            step_result["step"] = step;
            step_result["spikes"] = brain->getTotalSpikeCount();
            step_result["firing_neurons"] = brain->getFiringNeuronCount();
            step_result["average_firing_rate"] = brain->getAverageFiringRate();
            
            brain->step(step);
            results.push_back(step_result);
        }
        
        return results;
    }, py::arg("brain"), py::arg("numSteps"), py::arg("timestep") = 0.1,
          "Run multiple simulation steps and collect statistics for each");

    // Debugging and visualization utilities
    m.def("getDebugInfo", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent) -> std::string {
        std::stringstream ss;
        ss << "=== NLM Debug Information ===\n";
        
        if (brain) {
            ss << "Brain Status:\n";
            ss << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
            ss << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
            ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
            ss << "  Total Spikes: " << brain->getTotalSpikeCount() << "\n";
            ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << "\n";
            ss << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
            ss << "  Regions: " << brain->getRegionCount() << "\n";
        }
        
        if (world) {
            ss << "World Status:\n";
            ss << "  Width: " << world->getWidth() << "\n";
            ss << "  Height: " << world->getHeight() << "\n";
            ss << "  Simulation Time: " << world->getSimulationTime() << "\n";
            ss << "  Max Energy: " << world->getMaxEnergy() << "\n";
            ss << "  Energy Decay Rate: " << world->getEnergyDecayRate() << "\n";
        }
        
        if (agent) {
            ss << "Agent Status:\n";
            ss << "  Curiosity: " << agent->getCuriosityLevel() << "\n";
            ss << "  Novelty: " << agent->getNoveltyLevel() << "\n";
            ss << "  Neuromodulation: " << agent->getNeuromodulationLevel() << "\n";
            ss << "  Prediction Error: " << agent->getPredictionError() << "\n";
        }
        
        ss << "=================================\n";
        return ss.str();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
          "Get detailed debug information as a string");

    m.def("profilePerformance", [](std::shared_ptr<Brain> brain, int steps = 100, double timestep = 0.1) -> py::dict {
        py::dict profile;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            if (brain) brain->step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        profile["total_steps"] = steps;
        profile["total_time_ms"] = duration.count();
        profile["time_per_step_ms"] = duration.count() / static_cast<double>(steps);
        profile["steps_per_second"] = steps / (duration.count() / 1000.0);
        profile["spikes_generated"] = brain ? brain->getTotalSpikeCount() : 0;
        profile["final_firing_rate"] = brain ? brain->getAverageFiringRate() : 0.0;
        
        return profile;
    }, py::arg("brain"), py::arg("steps") = 100, py::arg("timestep") = 0.1,
          "Profile brain performance over multiple steps");

    // Enhanced sensory and motor utilities
    m.def("getPerceptFeatures", [](std::shared_ptr<SimpleWorld> world) -> py::dict {
        py::dict features;
        if (!world) return features;
        
        auto percept = world->getSensoryPercept();
        if (!percept) return features;
        
        // Vision features
        auto vision = percept->getVision();
        if (vision) {
            features["vision_width"] = vision->getWidth();
            features["vision_height"] = vision->getHeight();
            features["vision_channels"] = vision->getChannels();
            auto data = vision->getData();
            features["vision_data"] = std::vector<float>(data.begin(), data.end());
        }
        
        // Internal signals
        auto internal = percept->getInternal();
        if (!internal.empty()) features["internal_signals"] = internal;
        
        // Proprioception
        auto proprioception = percept->getProprioception();
        if (!proprioception.empty()) features["proprioception"] = proprioception;
        
        // Touch
        auto touch = percept->getTouch();
        if (!touch.empty()) features["touch"] = touch;
        
        return features;
    }, py::arg("world"), "Extract feature vector from world percept");

    m.def("createActionFromDict", [](std::map<std::string, py::object> actionDict) -> std::shared_ptr<Action> {
        ActionType type = ActionType::Wait;
        std::vector<float> parameters;
        
        if (actionDict.find("type") != actionDict.end()) {
            std::string typeStr = actionDict["type"].cast<std::string>();
            if (typeStr == "MoveForward") type = ActionType::MoveForward;
            else if (typeStr == "MoveBackward") type = ActionType::MoveBackward;
            else if (typeStr == "TurnLeft") type = ActionType::TurnLeft;
            else if (typeStr == "TurnRight") type = ActionType::TurnRight;
            else if (typeStr == "Look") type = ActionType::Look;
            else if (typeStr == "Interact") type = ActionType::Interact;
            else if (typeStr == "Eat") type = ActionType::Eat;
            else if (typeStr == "Drink") type = ActionType::Drink;
            else if (typeStr == "Rest") type = ActionType::Rest;
        }
        
        if (actionDict.find("parameters") != actionDict.end()) {
            parameters = actionDict["parameters"].cast<std::vector<float>>();
        }
        
        return std::make_shared<Action>(type, parameters);
    }, py::arg("actionDict"), "Create an Action from a dictionary");

    m.def("enableAllFeatures", [](std::shared_ptr<AgentBrain> agent, bool enable = true) {
        if (agent) {
            agent->enableRewardModulation(enable);
            agent->enableStructuralPlasticity(enable);
            agent->enableDevelopment(enable);
            agent->enableCuriosity(enable);
        }
    }, py::arg("agent"), py::arg("enable") = true,
          "Enable or disable all learning features");

    // Enhanced initialization validation
    m.def("isInitialized", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                             std::shared_ptr<AgentBrain> agent) -> bool {
        bool valid = true;
        
        if (brain && !brain->getTotalNeuronCount()) valid = false;
        if (world && (world->getWidth() <= 0 || world->getHeight() <= 0)) valid = false;
        
        return valid;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
          "Check if all components are properly initialized");

    m.def("initializeAll", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                            std::shared_ptr<AgentBrain> agent) -> bool {
        bool success = true;
        
        if (brain && !brain->initialize()) success = false;
        if (world) world->reset();
        if (agent && world) agent->initialize(world);
        
        return success;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
          "Initialize all components and return success status");

    // Additional convenience and utility functions
    
    // Simple simulation runner for common use cases
    m.def("runSimpleSimulation", [](std::shared_ptr<Brain> brain, int steps, double timestep = 0.1) -> py::dict {
        py::dict results;
        
        if (!brain) {
            return results;
        }
        
        // Get initial stats
        results["initial_neurons"] = brain->getTotalNeuronCount();
        results["initial_spikes"] = brain->getTotalSpikeCount();
        results["initial_firing_rate"] = brain->getAverageFiringRate();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
            
            // Collect stats every 100 steps
            if (step % 100 == 0) {
                py::dict step_result;
                step_result["step"] = step;
                step_result["total_spikes"] = brain->getTotalSpikeCount();
                step_result["firing_neurons"] = brain->getFiringNeuronCount();
                step_result["average_firing_rate"] = brain->getAverageFiringRate();
                results["step_" + std::to_string(step)] = step_result;
            }
        }
        
        // Final stats
        results["final_neurons"] = brain->getTotalNeuronCount();
        results["final_spikes"] = brain->getTotalSpikeCount();
        results["final_firing_rate"] = brain->getAverageFiringRate();
        results["total_steps"] = steps;
        
        return results;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.1,
          "Run a simple simulation and return step-by-step results");

    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filename) -> bool {
        if (!brain) return false;
        return brain->save(filename);
    }, py::arg("brain"), py::arg("filename"),
          "Export brain state to file");

    m.def("importBrainState", [](const std::string& filename, std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) return brain;
        if (!brain->load(filename)) return brain;
        return brain;
    }, py::arg("filename"), py::arg("config"),
          "Import brain state from file and create new brain");

    m.def("calculateLearningMetrics", [](std::shared_ptr<Brain> brain, int windowSteps = 100) -> py::dict {
        py::dict metrics;
        
        if (!brain) {
            metrics["error"] = "Invalid brain pointer";
            return metrics;
        }
        
        // Simple learning metrics calculation
        metrics["total_neurons"] = brain->getTotalNeuronCount();
        metrics["total_synapses"]_CALL brain->getTotalSynapseCount();
        metrics["current_firing_rate"] = brain->getAverageFiringRate();
        metrics["learning_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return metrics;
    }, py::arg("brain"), py::arg("windowSteps") = 100,
          "Calculate learning metrics for the brain");

    m.def("createTutorialExample", [](std::string exampleType) -> py::dict {
        py::dict example;
        
        if (exampleType == "minimal") {
            example["code"] = R"(
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
";
        }
        else if (exampleType == "complete") {
            example["code"] = R"(
import pynlm

# Complete agent example
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)
agent.enableAllFeatures(True)

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
";
        }
        
        return example;
    }, py::arg("exampleType"),
          "Create tutorial example code as Python code string");

    // Type conversion utilities
    m.def("neuronIdToInt", [](NeuronId id) { return id.value; }, py::arg("id"),
          "Convert NeuronId to integer for indexing");

    m.def("intToNeuronId", [](uint64_t value) { return NeuronId(value); }, py::arg("value"),
          "Convert integer to NeuronId");

    // Performance monitoring
    m.def("monitorPerformance", [](std::shared_ptr<Brain> brain, int samples = 10) -> py::dict {
        py::dict perf;
        
        if (!brain) return perf;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < samples; ++i) {
            brain->step(i);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        perf["samples"] = samples;
        perf["total_time_us"] = duration.count();
        perf["time_per_step_us"] = duration.count() / static_cast<double>(samples);
        perf["steps_per_second"] = samples / (duration.count() / 1000000.0);
        
        return perf;
    }, py::arg("brain"), py::arg("samples") = 10,
          "Monitor and report brain performance metrics");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
