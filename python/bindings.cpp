// Improved Python bindings for NLM (Neural Learning Machine)
// Provides a clean, Pythonic API for working with the neural simulation framework

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
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, 
        SensoryInput, Action, and related components.
        
        The API is designed to be intuitive and Pythonic while providing
        access to all the power and flexibility of the underlying C++ implementation.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // =================================================================
    // Core Type Definitions
    // =================================================================
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

    // =================================================================
    // Enumerations
    // =================================================================
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

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("NONE", ActionType::NONE)
        .value("MOVE", ActionType::MOVE)
        .value("GRASP", ActionType::GRASP)
        .value("RELEASE", ActionType::RELEASE)
        .value("ROTATE", ActionType::ROTATE)
        .value("PUSH", ActionType::PUSH)
        .value("PULL", ActionType::PULL)
        .value("EXPLORE", ActionType::EXPLORE)
        .value("INTERACT", ActionType::INTERACT)
        .export_values();

    // =================================================================
    // Core Neural Components
    // =================================================================
    py::class_<Neuron, std::shared_ptr<Neuron>>(m, "Neuron", R"pbdoc(Spiking neural element with LIF dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId)
        .def("setType", &Neuron::setType, py::arg("type"))
        .def("getType", &Neuron::getType)
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"))
        .def("getMembranePotential", &Neuron::getMembranePotential)
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"))
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"))
        .def("getThreshold", &Neuron::getThreshold)
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"))
        .def("getFiringState", &Neuron::getFiringState)
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"))
        .def("getFiringRate", &Neuron::getFiringRate)
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"))
        .def("getLeakConductance", &Neuron::getLeakConductance)
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"))
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod)
        .def("decrementRefractory", &Neuron::decrementRefractory)
        .def("setRegion", &Neuron::setRegion, py::arg("regionId"))
        .def("getRegion", &Neuron::getRegion)
        .def("setPopulation", &Neuron::setPopulation, py::arg("populationId"))
        .def("getPopulation", &Neuron::getPopulation)
        .def("addSynapse", &Neuron::addSynapse, py::arg("synapse"))
        .def("getSynapses", &Neuron::getSynapses)
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"))
        .def("getMembranePotential", &Neuron::getMembranePotential)
        .def("getCurrentState", &Neuron::getCurrentState)
        .def("getSpikeHistory", &Neuron::getSpikeHistory)
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime)
        .def("isActive", &Neuron::isActive)
        .def("isFiring", &Neuron::isFiring)
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"))
        .def("recordSpike", &Neuron::recordSpike, py::arg("time"))
        .def("toString", &Neuron::toString);

    py::class_<Synapse, std::shared_ptr<Synapse>>(m, "Synapse", R"pbdoc(Connection between neurons with plasticity)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId)
        .def("getSourceNeuron", &Synapse::getSourceNeuron)
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron)
        .def("getWeight", &Synapse::getWeight)
        .def("setWeight", &Synapse::setWeight, py::arg("weight"))
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"))
        .def("getDelay", &Synapse::getDelay)
        .def("setDelay", &Synapse::setDelay, py::arg("delay"))
        .def("getType", &Synapse::getType)
        .def("setType", &Synapse::setType, py::arg("type"))
        .def("isExcitatory", &Synapse::isExcitatory)
        .def("isInhibitory", &Synapse::isInhibitory)
        .def("enablePlasticity", &Synapse::enablePlasticity, 
              py::arg("hebbian"), py::arg("stdp"), py::arg("structural"))
        .def("hasHebbianPlasticity", &Synapse::hasHebbianPlasticity)
        .def("hasSTDPPlasticity", &Synapse::hasSTDPPlasticity)
        .def("hasStructuralPlasticity", &Synapse::hasStructuralPlasticity)
        .def("getSourceNeuronType", &Synapse::getSourceNeuronType)
        .def("getDestinationNeuronType", &Synapse::getDestinationNeuronType)
        .def("getTypeCompatibilityScore", &Synapse::getTypeCompatibilityScore)
        .def("getEfficacy", &Synapse::getEfficacy)
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"))
        .def("getLastPreSpikeTime", &Synapse::getLastPreSpikeTime)
        .def("getLastPostSpikeTime", &Synapse::getLastPostSpikeTime)
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"))
        .def("toString", &Synapse::toString);

    py::class_<NeuralRegion, std::shared_ptr<NeuralRegion>>(m, "NeuralRegion", R"pbdoc(Collection of neurons and their connections)pbdoc")
        .def("getId", &NeuralRegion::getId)
        .def("setName", &NeuralRegion::setName, py::arg("name"))
        .def("getName", &NeuralRegion::getName)
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type"))
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"))
        .def("getPopulationCount", &NeuralRegion::getPopulationCount)
        .def("getPopulations", &NeuralRegion::getPopulations)
        .def("addSynapse", &NeuralRegion::addSynapse, 
              py::arg("sourceNeuron"), py::arg("destinationNeuron"), 
              py::arg("weight") = 1.0f, py::arg("delay") = 1)
        .def("getSynapses", &NeuralRegion::getSynapses)
        .def("getSynapseCount", &NeuralRegion::getSynapseCount)
        .def("getAllNeurons", &NeuralRegion::getAllNeurons)
        .def("getNeuron", &NeuralRegion::getNeuron, py::arg("neuronId"))
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount)
        .def("getTypeDistribution", &NeuralRegion::getTypeDistribution)
        .def("initializeRandomConnectivity", 
              &NeuralRegion::initializeRandomConnectivity,
              py::arg("randomGenerator"), py::arg("connectionProbability") = 0.1f,
              py::arg("excitatoryWeight") = 1.0f, py::arg("inhibitoryWeight") = -1.0f)
        .def("getExcitatoryInhibitionRatio", &NeuralRegion::getExcitationInhibitionRatio)
        .def("getPopulationByType", &NeuralRegion::getPopulationByType, py::arg("type"))
        .def("toString", &NeuralRegion::toString);

    // =================================================================
    // Configuration System
    // =================================================================
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Neural simulation configuration manager)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from file (JSON or YAML format)")
        .def("loadFromArgs", &Config::loadFromArgs, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save current configuration to file")
        .def("set", py::overload_cast<const std::string&, int>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("set", py::overload_cast<const std::string&, double>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set floating-point configuration value")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set string configuration value")
        .def("set", py::overload_cast<const std::string&, bool>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set boolean configuration value")
        .def("get", py::overload_cast<const std::string&, int>(&Config::get),
             py::arg("key"),
             "Get integer configuration value")
        .def("get", py::overload_cast<const std::string&, double>(&Config::get),
             py::arg("key"),
             "Get floating-point configuration value")
        .def("get", py::overload_cast<const std::string&, const std::string&>(&Config::get),
             py::arg("key"),
             "Get string configuration value")
        .def("get", py::overload_cast<const std::string&, bool>(&Config::get),
             py::arg("key"),
             "Get boolean configuration value")
        .def("getOr", py::overload_cast<const std::string&, int, int>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value with default fallback")
        .def("getOr", py::overload_cast<const std::string&, double, double>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value with default fallback")
        .def("has", &Config::has, py::arg("key"),
             "Check if configuration key exists")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("clear", &Config::clear,
             "Clear all configuration values")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("getSummary", &Config::summary,
             "Get configuration summary as string")
        .def("isValidKey", &Config::isValidKey, py::arg("key"),
             "Validate configuration key format")
        .def("toDict", &Config::summary,
             "Convert configuration to dictionary (alias for getSummary)")
        .def("_to_dict_", &Config::summary,  // Python dict protocol
             "Python dict protocol for configuration");

    // =================================================================
    // Sensory and Motor Systems
    // =================================================================
    py::class_<SensoryInput, std::shared_ptr<SensoryInput>>(m, "SensoryInput", 
        R"pbdoc(Base class for sensory inputs to the brain)pbdoc")
        .def(py::init<>())
        .def("getType", &SensoryInput::getType)
        .def("getTimestamp", &SensoryInput::getTimestamp)
        .def("getAllSignals", &SensoryInput::getAllSignals)
        .def("addSignal", &SensoryInput::addSignal, py::arg("name"), py::arg("value"))
        .def("getSignal", &SensoryInput::getSignal, py::arg("name"))
        .def("hasSignal", &SensoryInput::hasSignal, py::arg("name"))
        .def("clone", &SensoryInput::clone,
             "Create a copy of this sensory input");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Visual sensory input from environment)pbdoc")
        .def(py::init<>())
        .def("configure", &Vision::configure, py::arg("width"), py::arg("height"),
             py::arg("resolution") = 1.0f)
        .def("update", &Vision::update, py::arg("worldObjects"))
        .def("getVisionWidth", &Vision::getVisionWidth)
        .def("getVisionHeight", &Vision::getVisionHeight)
        .def("getVisionData", &Vision::getVisionData,
             py::return_value_policy::reference_internal)
        .def("getActiveObjects", &Vision::getActiveObjects)
        .def("getObjectsInRange", &Vision::getObjectsInRange,
             py::arg("x"), py::arg("y"), py::arg("radius"));

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def("addSound", &Audio::addSound, py::arg("frequency"), py::arg("amplitude"),
             py::arg("duration"), py::arg("source"))
        .def("getSounds", &Audio::getSounds)
        .def("getSoundCount", &Audio::getSoundCount)
        .def("removeSound", &Audio::removeSound, py::arg("index"))
        .def("clearSounds", &Audio::clearSounds);

    py::class_<Touch, SensoryInput>(m, "Touch", R"pbdoc(Touch sensory input)pbdoc")
        .def(py::init<>())
        .def("addTouchPoint", &Touch::addTouchPoint, py::arg("x"), py::arg("y"),
             py::arg("intensity"), py::arg("type"))
        .def("getTouchPoints", &Touch::getTouchPoints)
        .def("getTouchPointCount", &Touch::getTouchPointCount)
        .def("getTotalIntensity", &Touch::getTotalIntensity)
        .def("getPointsInRadius", &Touch::getPointsInRadius,
             py::arg("centerX"), py::arg("centerY"), py::arg("radius"));

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal state signals)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action, std::shared_ptr<Action>>(m, "Action", R"pbdoc(Motor output action)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getDuration", &Action::getDuration)
        .def("getTargetPosition", &Action::getTargetPosition)
        .def("isCompleted", &Action::isCompleted)
        .def("update", &Action::update, py::arg("timestep"))
        .def("clone", &Action::clone,
             "Create a copy of this action");

    // =================================================================
    // Agent and World Components
    // =================================================================
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Game world object)pbdoc")
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
        .def("distanceTo", &WorldObject::distanceTo, py::arg("other"));

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent physical state and properties)pbdoc")
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
        .def("update", &AgentBody::update, py::arg("worldWidth"), py::arg("worldHeight"),
             py::arg("deltaTime"), py::arg("wrapWorld") = true)
        .def("canMoveTo", &AgentBody::canMoveTo, py::arg("x"), py::arg("y"),
             py::arg("worldWidth"), py::arg("worldHeight"), py::arg("wrapWorld") = true)
        .def("moveTo", &AgentBody::moveTo, py::arg("x"), py::arg("y"),
             py::arg("worldWidth"), py::arg("worldHeight"), py::arg("wrapWorld") = true)
        .def("rotateTo", &AgentBody::rotateTo, py::arg("targetX"), py::arg("targetY"),
             py::arg("worldWidth"), py::arg("worldHeight"), py::arg("wrapWorld") = true);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory perception data for agent)pbdoc")
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
        .def("getSignal", &SensoryPercept::getSignal, py::arg("name"))
        .def("getSignalNames", &SensoryPercept::getSignalNames)
        .def("addSignal", &SensoryPercept::addSignal, py::arg("name"), py::arg("value"));

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
        .def("getWorldObjects", &SimpleWorld::getWorldObjects)
        .def("getWorldObject", &SimpleWorld::getWorldObject, py::arg("x"), py::arg("y"));

    // =================================================================
    // Agent Brain Interface
    // =================================================================
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent interface to neural brain)pbdoc")
        .def(py::init<>())
        .def("setBrain", &AgentBrain::setBrain, py::arg("brain"))
        .def("getBrain", &AgentBrain::getBrain)
        .def("setWorld", &AgentBrain::setWorld, py::arg("world"))
        .def("getWorld", &AgentBrain::getWorld)
        .def("getSensoryPercept", &AgentBrain::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getActionResult", &AgentBrain::getActionResult,
             py::return_value_policy::reference_internal)
        .def("getAction", &AgentBrain::getAction)
        .def("executeAction", &AgentBrain::executeAction, py::arg("action"))
        .def("step", &AgentBrain::step, py::arg("timestep"),
             "Execute one simulation step with the agent")
        .def("receiveSensoryInput", &AgentBrain::receiveSensoryInput, py::arg("input"),
             "Process sensory input from environment")
        .def("getMotorCommand", &AgentBrain::getMotorCommand,
             py::return_value_policy::reference_internal,
             "Get the motor command to execute")
        .def("getSensoryBuffer", &AgentBrain::getSensoryBuffer,
             py::return_value_policy::reference_internal)
        .def("getMotorBuffer", &AgentBrain::getMotorBuffer,
             py::return_value_policy::reference_internal)
        .def("setReward", &AgentBrain::setReward, py::arg("reward"),
             "Set reward signal from environment")
        .def("getScore", &AgentBrain::getScore,
             "Get cumulative score/reward")
        .def("reset", &AgentBrain::reset,
             "Reset agent state")
        .def("getInternalState", &AgentBrain::getInternalState,
             py::return_value_policy::reference_internal,
             "Get internal agent state for debugging/analysis")
        .def("setInternalState", &AgentBrain::setInternalState,
             py::arg("state"), "Set internal agent state (for testing/debugging)")
        .def("getActionHistory", &AgentBrain::getActionHistory,
             "Get action execution history")
        .def("getPerceptionHistory", &AgentBrain::getPerceptionHistory,
             "Get sensory perception history");

    // =================================================================
    // Main Brain Class
    // =================================================================
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
             "Get excitation-inhibition balance")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count across entire brain")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get number of pending spike events")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system reference")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getNeuralPlanner", &Brain::getNeuralPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner system")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttentionalSelection", &Brain::getAttentionalSelection,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty detection system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("getPredictionError", &Brain::getPredictionError,
             py::return_value_policy::reference_internal,
             "Get prediction error system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulation system")
        .def("getCheckpointManager", &Brain::getCheckpointManager,
             py::return_value_policy::reference_internal,
             "Get checkpoint manager system")
        .def("getDevelopmentStage", &Brain::getDevelopmentStage,
             "Get current developmental stage")
        .def("getRandomSeed", &Brain::getRandomSeed,
             "Get current random seed")
        .def("getSimulationTimeStep", &Brain::getSimulationTimeStep,
             "Get current simulation timestep")
        .def("toString", &Brain::toString,
             "Get brain state as string")
        .def("toDict", &Brain::toString,
             "Convert brain state to dictionary (alias for toString)")
        .def("_to_dict_", &Brain::toString,
             "Python dict protocol for brain");

    // =================================================================
    // Constants and Type Aliases for Python Users
    // =================================================================
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Pythonic convenience functions
    m.def("createBrainFromConfig", [](const std::string& configFile) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(configFile);
        return std::make_shared<Brain>(config);
    }, py::arg("configFile"),
        "Create a brain from configuration file");

    m.def("createBrainFromDict", [](const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        py::list keys = configDict.keys();
        for (auto& key : keys) {
            std::string keyStr = py::cast<std::string>(key);
            py::object value = configDict[key];
            
            if (py::isinstance<py::int_(long)>(value)) {
                config->set(keyStr, py::cast<int64_t>(value));
            } else if (py::isinstance<py::float>(value)) {
                config->set(keyStr, py::cast<double>(value));
            } else if (py::isinstance<py::str>(value)) {
                config->set(keyStr, py::cast<std::string>(value));
            } else if (py::isinstance<py::bool>(value)) {
                config->set(keyStr, py::cast<bool>(value));
            } else if (py::isinstance<py::list>(value)) {
                py::list listValue = py::cast<py::list>(value);
                std::vector<double> doubleVec;
                for (auto& item : listValue) {
                    doubleVec.push_back(py::cast<double>(item));
                }
                config->set(keyStr, doubleVec);
            }
        }
        return std::make_shared<Brain>(config);
    }, py::arg("configDict"),
        "Create a brain from Python dictionary");

    m.def("run_simulation_step", [](std::shared_ptr<Brain> brain, double timestep) {
        brain->step(0, timestep);
    }, py::arg("brain"), py::arg("timestep"),
        "Run a single simulation step");

    m.def("get_brain_summary", [](std::shared_ptr<Brain> brain) {
        return brain->toString();
    }, py::arg("brain"),
        "Get brain summary as string");

    // =================================================================
    // Version and Metadata
    // =================================================================
    m.attr("__version__") = "2.1.0";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__license__") = "MIT";
    m.attr("__description__") = "Python bindings for the NLM (Neural Learning Machine) neural simulation framework";
}
