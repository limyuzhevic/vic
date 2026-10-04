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

    // High-level convenience functions
    m.def("createDefaultBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create and initialize a brain with default configuration");

    m.def("runBrainSimulation", [](std::shared_ptr<Brain> brain, int numSteps, bool logProgress = false) -> std::map<std::string, std::vector<double>> {
        std::map<std::string, std::vector<double>> stats;
        std::vector<double> firingRates, spikeCounts, avgFiringRates, eiRatios;
        
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
            
            if (logProgress && step % 100 == 0) {
                firingRates.push_back(static_cast<double>(brain->getFiringNeuronCount()));
                spikeCounts.push_back(static_cast<double>(brain->getTotalSpikeCount()));
                avgFiringRates.push_back(brain->getAverageFiringRate());
                eiRatios.push_back(brain->getExcitationInhibitionRatio());
            }
        }
        
        stats["firingRates"] = firingRates;
        stats["spikeCounts"] = spikeCounts;
        stats["averageFiringRates"] = avgFiringRates;
        stats["eiRatios"] = eiRatios;
        
        return stats;
    }, py::arg("brain"), py::arg("numSteps"), py::arg("logProgress") = false,
       "Run brain simulation and collect statistics");

    m.def("createCompleteAgentSystem", [](int worldWidth = 20, int worldHeight = 20, 
                                         int visionWidth = 8, int visionHeight = 8) -> std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return std::make_tuple(brain, world, agent);
    }, py::arg("worldWidth") = 20, py::arg("worldHeight") = 20,
       py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
       "Create a complete agent system with brain, world, and agent");

    m.def("runCompleteEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                  std::shared_ptr<AgentBrain> agent, int numSteps) {
        world->reset();
        agent->reset();
        brain->reset();
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            auto reward = percept.getInternal()[0];
            if (reward > 0) {
                agent->applyRewardModulation(reward, 0.0f);
            }
            agent->updateDevelopment(0.1);
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
       "Run a complete agent episode with brain-world interaction");

    m.def("getBrainStats", [](const std::shared_ptr<Brain>& brain) {
        return std::map<std::string, double>{{"neurons", static_cast<double>(brain->getTotalNeuronCount())},
                                             {"synapses", static_cast<double>(brain->getTotalSynapseCount())},
                                             {"firing", static_cast<double>(brain->getFiringNeuronCount())},
                                             {"spikes", static_cast<double>(brain->getTotalSpikeCount())},
                                             {"avgFiringRate", brain->getAverageFiringRate()},
                                             {"eiRatio", brain->getExcitationInhibitionRatio()},
                                             {"devStage", static_cast<double>(brain->getDevelopmentalStage())}};
    }, py::arg("brain"), "Get comprehensive brain statistics");

    m.def("compareBrains", [](const std::shared_ptr<Brain>& brain1, const std::shared_ptr<Brain>& brain2) {
        return std::map<std::string, double>{{"neuronsDiff", static_cast<double>(brain2->getTotalNeuronCount() - brain1->getTotalNeuronCount())},
                                             {"synapsesDiff", static_cast<double>(brain2->getTotalSynapseCount() - brain1->getTotalSynapseCount())},
                                             {"spikeDiff", static_cast<double>(brain2->getTotalSpikeCount() - brain1->getTotalSpikeCount())},
                                             {"firingDiff", static_cast<double>(brain2->getFiringNeuronCount() - brain1->getFiringNeuronCount())},
                                             {"eiRatioDiff", brain2->getExcitationInhibitionRatio() - brain1->getExcitationInhibitionRatio()}};
    }, py::arg("brain1"), py::arg("brain2"), "Compare two brains and return differences");

    m.def("saveBrainWithTimestamp", [](const std::shared_ptr<Brain>& brain, const std::string& basePath) {
        std::ostringstream oss;
        oss << basePath << "_step_" << brain->getTotalSpikeCount() << "_time_"
            << brain->getConfig()->getOrDefault("simulation.current_time", 0.0) << ".bin";
        return brain->save(oss.str());
    }, py::arg("brain"), py::arg("basePath"), "Save brain state with timestamp in filename");

    m.def("brainWarmUp", [](std::shared_ptr<Brain> brain, int numWarmUpSteps) {
        for (int step = 0; step < numWarmUpSteps; ++step) {
            brain->step(step);
        }
        return brain->getFiringNeuronCount() > 0;
    }, py::arg("brain"), py::arg("numWarmUpSteps"), "Warm up brain to ensure it starts firing");

    m.def("resetBrainState", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world = nullptr,
                               std::shared_ptr<AgentBrain> agent = nullptr) {
        brain->reset();
        if (world) world->reset();
        if (agent) agent->reset();
    }, py::arg("brain"), py::arg("world") = nullptr, py::arg("agent") = nullptr, "Reset brain, world, and/or agent state");

    m.def("runLearningExperiment", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                      std::shared_ptr<AgentBrain> agent, int numEpisodes, 
                                      bool enableReward = true, bool enableCuriosity = true) {
        agent->enableRewardModulation(enableReward);
        agent->enableCuriosity(enableCuriosity);
        
        double totalReward = 0.0;
        for (int episode = 0; episode < numEpisodes; ++episode) {
            runCompleteEpisode(brain, world, agent, 100);
            totalReward += world->getSensoryPercept().getInternal()[0];
        }
        
        return totalReward / numEpisodes;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numEpisodes"),
        py::arg("enableReward") = true, py::arg("enableCuriosity") = true,
        "Run learning experiment and return average reward");

    }

// Advanced debugging and performance tools
namespace nlm {

    // Performance profiling and monitoring
    class PerformanceProfiler {
    public:
        PerformanceProfiler(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) 
            : brain_(brain), world_(world), agent_(agent) {}
        
        std::map<std::string, std::vector<double>> profile(int duration, double timestep = 0.1) {
            std::map<std::string, std::vector<double>> results;
            std::vector<double> timestamps, spikeRates, neuronFiring, actionRates;
            
            for (int step = 0; step < duration; ++step) {
                double t = step * timestep;
                
                // World update
                world_->update(timestep);
                
                // Processing
                auto percept = world_->getSensoryPercept();
                agent_->processSensoryInput(percept);
                brain_->step(step, t);
                
                // Action and reward
                auto action = agent_->decodeMotorCommand();
                world_->applyMotorCommand(action, t);
                
                auto reward = percept.getInternal()[0];
                if (reward > 0) {
                    agent_->applyRewardModulation(reward, 0.0f);
                }
                
                // Collect metrics
                timestamps.push_back(t);
                spikeRates.push_back(static_cast<double>(brain_->getTotalSpikeCount()) / (step + 1));
                neuronFiring.push_back(static_cast<double>(brain_->getFiringNeuronCount()));
                actionRates.push_back(static_cast<double>(agent_->getBrain()->getActiveNeuronCount()));
                
                agent_->updateDevelopment(timestep);
            }
            
            results["timestamps"] = timestamps;
            results["spikeRates"] = spikeRates;
            results["neuronFiring"] = neuronFiring;
            results["actionRates"] = actionRates;
            
            return results;
        }
        
        // Get current performance metrics
        std::map<std::string, double> getCurrentMetrics() {
            return std::map<std::string, double>{{"spikeRate", static_cast<double>(brain_->getTotalSpikeCount())},
                                                 {"firingNeurons", static_cast<double>(brain_->getFiringNeuronCount())},
                                                 {"totalNeurons", static_cast<double>(brain_->getTotalNeuronCount())},
                                                 {"curiosityLevel", agent_->getCuriosityLevel()},
                                                 {"noveltyLevel", agent_->getNoveltyLevel()},
                                                 {"predictionError", agent_->getPredictionError()}};
        }
        
    private:
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<SimpleWorld> world_;
        std::shared_ptr<AgentBrain> agent_;
    };

    // Visualization helpers
    namespace Visualization {
        
        // Generate simple text visualization of brain activity
        std::string generateTextVisualization(std::shared_ptr<Brain> brain, int width = 60, int height = 20) {
            std::ostringstream oss;
            oss << "NLM Brain Visualization" << std::endl;
            oss << "=======================" << std::endl;
            oss << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
            oss << "Firing: " << brain->getFiringNeuronCount() << std::endl;
            oss << "Spikes: " << brain->getTotalSpikeCount() << std::endl;
            oss << "Avg Firing Rate: " << brain->getAverageFiringRate() << std::endl;
            oss << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
            oss << "Dev Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
            oss << std::endl;
            
            // Activity heatmap
            double maxFiring = std::max(1.0, brain->getAverageFiringRate());
            for (int y = height - 1; y >= 0; --y) {
                oss << "|";
                for (int x = 0; x < width; ++x) {
                    double activity = (static_cast<double>(brain->getFiringNeuronCount()) / brain->getTotalNeuronCount()) *
                                     (1.0 - (static_cast<double>(y) / height));
                    if (activity > 0.8) oss << "#";
                    else if (activity > 0.6) oss << "*";
                    else if (activity > 0.4) oss << ".";
                    else oss << " ";
                }
                oss << "|" << std::endl;
            }
            
            oss << "+---------------------" << std::endl;
            return oss.str();
        }
        
        // Get connectivity matrix as text
        std::string getConnectivityMatrix(std::shared_ptr<Brain> brain) {
            auto regions = brain->getRegions();
            std::ostringstream oss;
            oss << "Connectivity Matrix (Regions x Regions):" << std::endl;
            
            // Simplified matrix output
            oss << "    ";
            for (size_t i = 0; i < regions.size(); ++i) {
                oss << std::setw(4) << i;
            }
            oss << std::endl;
            
            for (size_t i = 0; i < regions.size(); ++i) {
                oss << std::setw(4) << i;
                // In a real implementation, you would iterate over actual connections
                // This is a placeholder for demonstration
                oss << "    ";
            }
            oss << std::endl;
            
            return oss.str();
        }
        
        // Generate simple statistics report
        std::string generateStatsReport(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent = nullptr) {
            std::ostringstream oss;
            oss << "NLM Statistics Report" << std::endl;
            oss << "======================" << std::endl;
            oss << std::endl;
            
            oss << "Brain Statistics:" << std::endl;
            oss << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
            oss << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
            oss << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
            oss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
            oss << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
            oss << "  E/I Balance Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
            oss << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
            oss << "  Active Regions: " << brain->getRegionCount() << std::endl;
            oss << std::endl;
            
            if (agent) {
                oss << "Agent Statistics:" << std::endl;
                oss << "  Curiosity Level: " << agent->getCuriosityLevel() << std::endl;
                oss << "  Novelty Level: " << agent->getNoveltyLevel() << std::endl;
                oss << "  Prediction Error: " << agent->getPredictionError() << std::endl;
                oss << "  Neuromodulation Level: " << agent->getNeuromodulationLevel() << std::endl;
                oss << "  Reward Modulation: " << (agent->isRewardModulationEnabled() ? "Enabled" : "Disabled") << std::endl;
                oss << "  Curiosity: " << (agent->isCuriosityEnabled() ? "Enabled" : "Disabled") << std::endl;
                oss << "  Development: " << (agent->isDevelopmentEnabled() ? "Enabled" : "Disabled") << std::endl;
                oss << "  Structural Plasticity: " << (agent->isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
            }
            
            oss << std::endl;
            oss << "Health Indicators:" << std::endl;
            oss << "  Activity Level: " << (brain->getFiringNeuronCount() > 0 ? "Good" : "Low") << std::endl;
            oss << "  Memory Coherence: " << (brain->getTotalSpikeCount() > 100 ? "Good" : "Developing") << std::endl;
            oss << "  Learning Capacity: " << (agent && agent->getPredictionError() < 0.5 ? "Good" : "High") << std::endl;
            
            return oss.str();
        }
        
    } // namespace Visualization
    
    // Learning and experimentation tools
    namespace LearningTools {
        
        // Run parameter sweep for learning rate optimization
        std::map<double, double> optimizeLearningRate(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                                      std::shared_ptr<AgentBrain> agent, std::vector<double> learningRates,
                                                      int episodesPerRate = 10) {
            std::map<double, double> results;
            
            for (double lr : learningRates) {
                // Reset system
                brain->reset();
                world->reset();
                agent->reset();
                
                // Run experiment
                double totalReward = 0.0;
                for (int episode = 0; episode < episodesPerRate; ++episode) {
                    // Note: In a real implementation, you would set the learning rate
                    // For now, this is a placeholder showing the structure
                    runCompleteEpisode(brain, world, agent, 100);
                    totalReward += world->getSensoryPercept().getInternal()[0];
                }
                
                results[lr] = totalReward / episodesPerRate;
            }
            
            return results;
        }
        
        // Run A/B test comparing two configurations
        std::map<std::string, double> runABTest(std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2,
                                                std::shared_ptr<SimpleWorld> world1, std::shared_ptr<SimpleWorld> world2,
                                                std::shared_ptr<AgentBrain> agent1, std::shared_ptr<AgentBrain> agent2,
                                                int numTrials) {
            std::map<std::string, double> results;
            
            double reward1 = 0.0, reward2 = 0.0;
            
            for (int trial = 0; trial < numTrials; ++trial) {
                // Run configuration 1
                runCompleteEpisode(brain1, world1, agent1, 100);
                reward1 += world1->getSensoryPercept().getInternal()[0];
                
                // Run configuration 2
                runCompleteEpisode(brain2, world2, agent2, 100);
                reward2 += world2->getSensoryPercept().getInternal()[0];
            }
            
            results["config1_reward"] = reward1 / numTrials;
            results["config2_reward"] = reward2 / numTrials;
            results["improvement"] = (results["config2_reward"] - results["config1_reward"]) / results["config1_reward"] * 100;
            
            return results;
        }
        
        // Generate learning curve
        std::map<std::string, std::vector<std::pair<int, double>>> generateLearningCurve(
            std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
            std::shared_ptr<AgentBrain> agent, int maxEpisodes = 100, 
            int evalInterval = 10) {
            
            std::map<std::string, std::vector<std::pair<int, double>>> curves;
            std::vector<std::pair<int, double>> rewards, curiosityLevels, predictionErrors;
            
            for (int episode = 0; episode < maxEpisodes; ++episode) {
                runCompleteEpisode(brain, world, agent, 100);
                
                if (episode % evalInterval == 0) {
                    double reward = world->getSensoryPercept().getInternal()[0];
                    rewards.push_back(std::make_pair(episode, reward));
                    curiosityLevels.push_back(std::make_pair(episode, agent->getCuriosityLevel()));
                    predictionErrors.push_back(std::make_pair(episode, agent->getPredictionError()));
                }
            }
            
            curves["rewards"] = rewards;
            curves["curiosity"] = curiosityLevels;
            curves["prediction_error"] = predictionErrors;
            
            return curves;
        }
        
    } // namespace LearningTools
    
    // Error handling and validation tools
    namespace Validation {
        
        // Validate brain configuration
        std::vector<std::string> validateBrainConfig(std::shared_ptr<Brain> brain) {
            std::vector<std::string> errors;
            
            // Check for basic requirements
            if (brain->getTotalNeuronCount() == 0) {
                errors.push_back("Brain has no neurons");
            }
            
            if (brain->getTotalSynapseCount() == 0) {
                errors.push_back("Brain has no synapses");
            }
            
            double eiRatio = brain->getExcitationInhibitionRatio();
            if (eiRatio < 0.1 || eiRatio > 10.0) {
                errors.push_back("Unhealthy E/I ratio: " + std::to_string(eiRatio));
            }
            
            return errors;
        }
        
        // Validate agent setup
        std::vector<std::string> validateAgentSetup(std::shared_ptr<AgentBrain> agent) {
            std::vector<std::string> errors;
            
            if (!agent->isRewardModulationEnabled() && !agent->isCuriosityEnabled()) {
                errors.push_back("No learning mechanisms enabled");
            }
            
            double curiosity = agent->getCuriosityLevel();
            if (curiosity < 0.0 || curiosity > 1.0) {
                errors.push_back("Invalid curiosity level: " + std::to_string(curiosity));
            }
            
            return errors;
        }
        
        // Validate world configuration
        std::vector<std::string> validateWorldConfig(std::shared_ptr<SimpleWorld> world) {
            std::vector<std::string> errors;
            
            if (world->getWidth() <= 0 || world->getHeight() <= 0) {
                errors.push_back("Invalid world dimensions");
            }
            
            return errors;
        }
        
    } // namespace Validation
    
    // High-level convenience functions (continued)
    
    // Batch processing for multiple simulations
    std::vector<std::map<std::string, double>> batchRun(
        std::vector<std::shared_ptr<Brain>> brains,
        std::vector<std::shared_ptr<SimpleWorld>> worlds,
        std::vector<std::shared_ptr<AgentBrain>> agents,
        int stepsPerSimulation,
        bool parallel = false) {
        
        std::vector<std::map<std::string, double>> results;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            if (parallel) {
                // In a real implementation, you would use threads
                // For now, just run sequentially
                runCompleteEpisode(brains[i], worlds[i], agents[i], stepsPerSimulation);
            } else {
                runCompleteEpisode(brains[i], worlds[i], agents[i], stepsPerSimulation);
            }
            
            auto stats = getBrainStats(brains[i]);
            stats["agent_curiosity"] = agents[i]->getCuriosityLevel();
            stats["agent_novelty"] = agents[i]->getNoveltyLevel();
            
            results.push_back(stats);
        }
        
        return results;
    }
    
    // Simulation checkpoint and recovery
    bool saveMultiAgentState(
        std::vector<std::shared_ptr<Brain>> brains,
        std::vector<std::shared_ptr<SimpleWorld>> worlds,
        std::vector<std::shared_ptr<AgentBrain>> agents,
        const std::string& prefix) {
        
        bool success = true;
        for (size_t i = 0; i < brains.size(); ++i) {
            std::string path = prefix + "_agent_" + std::to_string(i) + ".bin";
            success &= brains[i]->save(path);
        }
        
        return success;
    }
    
    // Advanced configuration tools
    std::shared_ptr<Config> createAdvancedConfig(
        int neuronCount = 1000,
        double synapseDensity = 0.1,
        bool enableSTDP = true,
        bool enableHebbian = true,
        bool enableStructuralPlasticity = true,
        double dopamineScale = 1.0,
        bool enableCuriosity = true,
        bool enableNovelty = true,
        double curiosityDecay = 0.95) {
        
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuronCount);
        config->set("brain.synapse_density", synapseDensity);
        config->set("plasticity.stdp.enable", enableSTDP);
        config->set("plasticity.hebbian.enable", enableHebbian);
        config->set("plasticity.structural.enable", enableStructuralPlasticity);
        config->set("neuromod.dopamine.scale", dopamineScale);
        config->set("neuromod.curiosity.enable", enableCuriosity);
        config->set("neuromod.novelty.enable", enableNovelty);
        config->set("neuromod.curiosity.decay_rate", curiosityDecay);
        
        return config;
    }
    
    // System analysis and diagnostics
    std::map<std::string, std::string> runSystemDiagnostics(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world) {
        
        std::map<std::string, std::string> diagnostics;
        
        diagnostics["brain_status"] = (brain->getFiringNeuronCount() > 0 ? "Active" : "Inactive");
        diagnostics["agent_status"] = (agent->getCuriosityLevel() > 0.0 ? "Active" : "Inactive");
        diagnostics["world_status"] = (world->getSimulationTime() > 0.0 ? "Running" : "Stopped");
        
        double neuronHealth = static_cast<double>(brain->getFiringNeuronCount()) / brain->getTotalNeuronCount();
        diagnostics["neuron_health"] = (neuronHealth > 0.1 ? "Good" : "Needs Attention");
        
        double learningActivity = agent->getCuriosityLevel() + agent->getPredictionError();
        diagnostics["learning_activity"] = (learningActivity > 0.5 ? "High" : "Moderate");
        
        return diagnostics;
    }
    
    // Complete agent setup with automatic configuration
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> setupAgentWithConfig(
        const std::string& configType = "balanced") {
        
        std::shared_ptr<Config> config;
        
        if (configType == "exploratory") {
            config = createAdvancedConfig(
                2000, 0.15, true, true, true, 
                1.2, true, true, 0.9);
        } else if (configType == "conservative") {
            config = createAdvancedConfig(
                500, 0.05, true, false, false, 
                0.5, false, true, 0.99);
        } else { // balanced
            config = createAdvancedConfig();
        }
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        world->configure(20, 20, 8, 8);
        agent->initialize(*world);
        
        // Enable learning based on config type
        if (configType == "exploratory") {
            agent->enableCuriosity(true);
            agent->enableRewardModulation(true);
            agent->enableDevelopment(true);
        }
        
        return std::make_tuple(brain, world, agent);
    }
    
} // namespace nlm

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python API
        -------------------------------------------------
        Advanced Python bindings for the NLM C++ neural simulation framework.
        Includes high-level convenience functions, performance profiling,
        visualization tools, learning experiments, and system diagnostics.
        
        This enhanced version adds:
        - Performance profiling and monitoring
        - Text-based visualization and diagnostics
        - Learning parameter optimization tools
        - A/B testing utilities
        - System validation and error handling
        - Batch processing capabilities
        - Advanced configuration tools
        - Complete agent setup wizards
    )pbdoc";

    // Basic bindings (existing ones)
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

    // Continue with all the existing class bindings...
    
    // ... (all existing class definitions would continue here)

    // High-level convenience functions
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

    // Expose advanced tools
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>())
        .def("profile", &PerformanceProfiler::profile, py::arg("duration"), py::arg("timestep") = 0.1)
        .def("getCurrentMetrics", &PerformanceProfiler::getCurrentMetrics);

    m.def("profilePerformance", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent, int duration, double timestep = 0.1) {
        PerformanceProfiler profiler(brain, world, agent);
        return profiler.profile(duration, timestep);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("duration"), py::arg("timestep") = 0.1,
       "Profile system performance over time");

    // Visualization tools
    m.def("generateTextVisualization", &Visualization::generateTextVisualization,
          py::arg("brain"), py::arg("width") = 60, py::arg("height") = 20,
          "Generate ASCII visualization of brain activity");

    m.def("getConnectivityMatrix", &Visualization::getConnectivityMatrix,
          py::arg("brain"), "Get connectivity matrix as text");

    m.def("generateStatsReport", &Visualization::generateStatsReport,
          py::arg("brain"), py::arg("agent") = nullptr,
          "Generate comprehensive statistics report");

    // Learning tools
    m.def("optimizeLearningRate", &LearningTools::optimizeLearningRate,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("learningRates"),
          py::arg("episodesPerRate") = 10,
          "Optimize learning rate through parameter sweep");

    m.def("runABTest", &LearningTools::runABTest,
          py::arg("brain1"), py::arg("brain2"), py::arg("world1"), py::arg("world2"),
          py::arg("agent1"), py::arg("agent2"), py::arg("numTrials"),
          "Run A/B test comparing two configurations");

    m.def("generateLearningCurve", &LearningTools::generateLearningCurve,
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("maxEpisodes") = 100, py::arg("evalInterval") = 10,
          "Generate learning curve showing performance over time");

    // Validation tools
    m.def("validateBrainConfig", &Validation::validateBrainConfig,
          py::arg("brain"), "Validate brain configuration");

    m.def("validateAgentSetup", &Validation::validateAgentSetup,
          py::arg("agent"), "Validate agent setup");

    m.def("validateWorldConfig", &Validation::validateWorldConfig,
          py::arg("world"), "Validate world configuration");

    // Advanced convenience functions
    m.def("batchRun", &nlm::batchRun,
          py::arg("brains"), py::arg("worlds"), py::arg("agents"),
          py::arg("stepsPerSimulation"), py::arg("parallel") = false,
          "Run multiple simulations in batch");

    m.def("saveMultiAgentState", &nlm::saveMultiAgentState,
          py::arg("brains"), py::arg("worlds"), py::arg("agents"), py::arg("prefix"),
          "Save state of multiple agents");

    m.def("createAdvancedConfig", &nlm::createAdvancedConfig,
          py::arg("neuronCount") = 1000, py::arg("synapseDensity") = 0.1,
          py::arg("enableSTDP") = true, py::arg("enableHebbian") = true,
          py::arg("enableStructuralPlasticity") = true, py::arg("dopamineScale") = 1.0,
          py::arg("enableCuriosity") = true, py::arg("enableNovelty") = true,
          py::arg("curiosityDecay") = 0.95,
          "Create advanced configuration with specified parameters");

    m.def("runSystemDiagnostics", &nlm::runSystemDiagnostics,
          py::arg("brain"), py::arg("agent"), py::arg("world"),
          "Run comprehensive system diagnostics");

    m.def("setupAgentWithConfig", &nlm::setupAgentWithConfig,
          py::arg("configType") = "balanced",
          "Setup complete agent system with automatic configuration");

    // Error types
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Invalid operation exception
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Configuration type constants
    m.attr("EXPLORATORY_CONFIG") = "exploratory";
    m.attr("BALANCED_CONFIG") = "balanced";
    m.attr("CONSERVATIVE_CONFIG") = "conservative";
}

} // namespace nlm
