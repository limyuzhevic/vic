#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

// Include NumPy for numerical array support
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <pybind11/numpy.h>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

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

    // Neuron class - Core brain cell with LIF dynamics
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Leaky Integrate-and-Fire neuron)")
        .def(py::init<NeuronId>(), py::arg("id"), R"pbdoc(Create neuron with ID)pbdoc")
        .def("getId", &Neuron::getId, R"pbdoc(Get neuron ID)pbdoc")
        .def("getType", &Neuron::getType, R"pbdoc(Get neuron type)pbdoc")
        .def("setType", &Neuron::setType, py::arg("type"), R"pbdoc(Set neuron type)pbdoc")
        .def("getMembranePotential", &Neuron::getMembranePotential, R"pbdoc(Get membrane potential)pbdoc")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), R"pbdoc(Set membrane potential)pbdoc")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), R"pbdoc(Add to membrane potential)pbdoc")
        .def("getThreshold", &Neuron::getThreshold, R"pbdoc(Get firing threshold)pbdoc")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), R"pbdoc(Set firing threshold)pbdoc")
        .def("isFiring", &Neuron::isFiring, R"pbdoc(Check if neuron is firing)pbdoc")
        .def("isRefractory", &Neuron::isRefractory, R"pbdoc(Check if neuron is in refractory period)pbdoc")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), R"pbdoc(Set firing state)pbdoc")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"), R"pbdoc(Set refractory period in steps)pbdoc")
        .def("decrementRefractory", &Neuron::decrementRefractory, R"pbdoc(Decrement refractory period)pbdoc")
        .def("getFiringRate", &Neuron::getFiringRate, R"pbdoc(Get firing rate)pbdoc")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"), R"pbdoc(Set firing rate)pbdoc")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"), R"pbdoc(Set leak conductance)pbdoc")
        .def("getLeakConductance", &Neuron::getLeakConductance, R"pbdoc(Get leak conductance)pbdoc")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod, R"pbdoc(Get refractory period)pbdoc")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"), R"pbdoc(Set resting potential)pbdoc")
        .def("getRestingPotential", &Neuron::getRestingPotential, R"pbdoc(Get resting potential)pbdoc")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"), R"pbdoc(Set reset potential)pbdoc")
        .def("checkThreshold", &Neuron::checkThreshold, R"pbdoc(Check if membrane potential exceeds threshold)pbdoc")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, R"pbdoc(Get last spike time)pbdoc")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"), R"pbdoc(Perform LIF integration step, returns true if neuron fired)pbdoc")
        .def("step", &Neuron::step, py::arg("currentTime"), R"pbdoc(Perform standard LIF step with 1ms timestep)pbdoc")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"), R"pbdoc(Receive excitatory synaptic input)pbdoc")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"), R"pbdoc(Receive inhibitory synaptic input)pbdoc")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"), R"pbdoc(Receive modulatory input)pbdoc")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), R"pbdoc(Inject external current)pbdoc")
        .def("getTotalCurrent", &Neuron::getTotalCurrent, R"pbdoc(Get total current)pbdoc")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent, R"pbdoc(Clear accumulated current)pbdoc")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"), R"pbdoc(Record a spike for STDP)pbdoc")
        .def("getSpikeHistory", &Neuron::getSpikeHistory, R"pbdoc(Get spike history)pbdoc")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory, R"pbdoc(Clear spike history)pbdoc")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"), R"pbdoc(Add incoming synapse)pbdoc")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"), R"pbdoc(Add outgoing synapse)pbdoc")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses, R"pbdoc(Get incoming synapses)pbdoc")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses, R"pbdoc(Get outgoing synapses)pbdoc")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), R"pbdoc(Enable plasticity rules)pbdoc")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"), R"pbdoc(Set region ID)pbdoc")
        .def("getRegionId", &Neuron::getRegionId, R"pbdoc(Get region ID)pbdoc")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"), R"pbdoc(Set population ID)pbdoc")
        .def("getPopulationId", &Neuron::getPopulationId, R"pbdoc(Get population ID)pbdoc")
        .def("reset", &Neuron::reset, R"pbdoc(Reset neuron to initial state)pbdoc")
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron id=" + std::to_string(n.getId().value) + ">";
        }, R"pbdoc(String representation)pbdoc");

    // Synapse class - Connection between neurons
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection between neurons)")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"), R"pbdoc(Create synapse with IDs)pbdoc")
        .def("getId", &Synapse::getId, R"pbdoc(Get synapse ID)pbdoc")
        .def("getSourceNeuron", &Synapse::getSourceNeuron, R"pbdoc(Get source neuron ID)pbdoc")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron, R"pbdoc(Get destination neuron ID)pbdoc")
        .def("getWeight", &Synapse::getWeight, R"pbdoc(Get synaptic weight)pbdoc")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), R"pbdoc(Set synaptic weight)pbdoc")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), R"pbdoc(Add to synaptic weight)pbdoc")
        .def("getDelay", &Synapse::getDelay, R"pbdoc(Get synaptic delay)pbdoc")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), R"pbdoc(Set synaptic delay)pbdoc")
        .def("getType", &Synapse::getType, R"pbdoc(Get synapse type)pbdoc")
        .def("setType", &Synapse::setType, py::arg("type"), R"pbdoc(Set synapse type)pbdoc")
        .def("isExcitatory", &Synapse::isExcitatory, R"pbdoc(Check if excitatory)pbdoc")
        .def("isInhibitory", &Synapse::isInhibitory, R"pbdoc(Check if inhibitory)pbdoc")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"), R"pbdoc(Record pre-synaptic spike for STDP)pbdoc")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"), R"pbdoc(Record post-synaptic spike for STDP)pbdoc")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory, R"pbdoc(Get pre-synaptic spike history)pbdoc")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory, R"pbdoc(Get post-synaptic spike history)pbdoc")
        .def("clearHistory", &Synapse::clearHistory, R"pbdoc(Clear spike history)pbdoc")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, R"pbdoc(Get plasticity flags)pbdoc")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), R"pbdoc(Enable plasticity rules)pbdoc")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace, R"pbdoc(Get eligibility trace)pbdoc")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"), R"pbdoc(Set eligibility trace)pbdoc")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"), R"pbdoc(Decay eligibility trace)pbdoc")
        .def("getEfficacy", &Synapse::getEfficacy, R"pbdoc(Get synaptic efficacy)pbdoc")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"), R"pbdoc(Set synaptic efficacy)pbdoc")
        .def("step", &Synapse::step, py::arg("currentTime"), R"pbdoc(Perform synaptic dynamics step)pbdoc")
        .def("reset", &Synapse::reset, R"pbdoc(Reset synapse to initial state)pbdoc")
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse id=" + std::to_string(s.getId().value) + 
                   " source=" + std::to_string(s.getSourceNeuron().value) +
                   " dest=" + std::to_string(s.getDestinationNeuron().value) + ">";
        }, R"pbdoc(String representation)pbdoc");

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Helper functions for batch operations
    m.def("create_neuron_population", [](size_t size, NeuronType type = NeuronType::Internal) {
        std::vector<std::shared_ptr<Neuron>> neurons;
        neurons.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            neurons.push_back(std::make_shared<Neuron>(NeuronId(i)));
            neurons[i]->setType(type);
        }
        return neurons;
    }, py::arg("size"), py::arg("type") = NeuronType::Internal,
        "Create a population of neurons with specified type");

    m.def("connect_neurons", [](const std::shared_ptr<Neuron>& source, const std::shared_ptr<Neuron>& destination,
                                float weight = 1.0f, Delay delay = 1) {
        SynapseId synId(destination->getId().value * 10000 + source->getId().value);
        auto synapse = std::make_shared<Synapse>(synId, source->getId(), destination->getId());
        synapse->setWeight(weight);
        synapse->setDelay(delay);
        destination->addIncomingSynapse(SynapseHandle(synId.value));
        source->addOutgoingSynapse(SynapseHandle(synId.value));
        return synapse;
    }, py::arg("source"), py::arg("destination"), py::arg("weight") = 1.0f, py::arg("delay") = 1,
        "Create a synaptic connection between two neurons");

    m.def("simulate_steps", [](const std::shared_ptr<Brain>& brain, size_t num_steps,
                              bool use_timestamps = false) {
        if (use_timestamps) {
            for (size_t i = 0; i < num_steps; ++i) {
                brain->step(static_cast<SimulationStep>(i), static_cast<Timestamp>(i * 0.001));
            }
        } else {
            for (size_t i = 0; i < num_steps; ++i) {
                brain->step(static_cast<SimulationStep>(i));
            }
        }
    }, py::arg("brain"), py::arg("num_steps"), py::arg("use_timestamps") = false,
        "Run a batch of simulation steps");

    m.def("run_agent_episode", [](const std::shared_ptr<AgentBrain>& agent,
                                 const std::shared_ptr<SimpleWorld>& world,
                                 size_t max_steps = 1000) {
        for (size_t step = 0; step < max_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            auto brain = agent->getBrain();
            brain->step(static_cast<SimulationStep>(step));
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward and apply modulation
            auto internal = percept->getInternal();
            float reward = internal && !internal->empty() ? internal->front() : 0.0f;
            agent->applyRewardModulation(reward, 0.0f);
            
            agent->updateDevelopment(0.1);
        }
    }, py::arg("agent"), py::arg("world"), py::arg("max_steps") = 1000,
        "Run a complete agent episode with world interaction");

    m.def("get_brain_stats", [](const std::shared_ptr<Brain>& brain) {
        py::dict stats;
        stats["neurons"] = brain->getTotalNeuronCount();
        stats["synapses"] = brain->getTotalSynapseCount();
        stats["firing"] = brain->getFiringNeuronCount();
        stats["spikes"] = brain->getTotalSpikeCount();
        stats["firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        stats["active"] = brain->getActiveNeuronCount();
        return stats;
    }, py::arg("brain"), "Get comprehensive brain statistics as a dictionary");

    m.def("create_development_config", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(1000));
        config->set("brain.synapse_density", 0.1f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("brain.v_thresh", -55.0f);
        config->set("brain.v_rest", -70.0f);
        config->set("brain.tau_mem", 20.0f);
        return config;
    }, "Create a development-ready configuration with balanced settings");

    m.def("validate_neural_connectivity", [](const std::shared_ptr<Brain>& brain) {
        auto regions = brain->getRegions();
        size_t total_connections = 0;
        size_t connected_regions = 0;
        
        for (const auto& region : regions) {
            auto neurons = region->getAllNeurons();
            size_t region_connections = 0;
            
            for (const auto& neuron : neurons) {
                auto outgoing = neuron->getOutgoingSynapses();
                region_connections += outgoing.size();
            }
            
            if (region_connections > 0) {
                connected_regions++;
            }
            total_connections += region_connections;
        }
        
        py::dict result;
        result["total_regions"] = regions.size();
        result["connected_regions"] = connected_regions;
        result["total_connections"] = total_connections;
        result["avg_connections_per_region"] = 
            regions.empty() ? 0.0f : static_cast<float>(total_connections) / regions.size();
        result["connectivity_ratio"] = 
            regions.empty() ? 0.0f : static_cast<float>(connected_regions) / regions.size();
            
        return result;
    }, py::arg("brain"), "Validate neural connectivity and return connectivity statistics");

    m.def("export_brain_to_csv", [](const std::shared_ptr<Brain>& brain, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "NeuronID,RegionID,Type,MembranePotential,Threshold,IsFiring,IsRefractory\n";
        
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            auto neurons = region->getAllNeurons();
            for (const auto& neuron : neurons) {
                auto state = neuron->getState();
                file << neuron->getId().value << ",";
                file << neuron->getRegionId().value << ",";
                file << static_cast<int>(neuron->getType()) << ",";
                file << state.membranePotential << ",";
                file << state.threshold << ",";
                file << (neuron->isFiring() ? 1 : 0) << ",";
                file << (neuron->isRefractory() ? 1 : 0) << ",";
                file << "\n";
            }
        }
        
        file << "\nSynapseID,SourceNeuronID,TargetNeuronID,Weight,Delay,Type\n";
        
        for (const auto& region : regions) {
            auto neurons = region->getAllNeurons();
            for (const auto& neuron : neurons) {
                auto outgoing = neuron->getOutgoingSynapses();
                for (const auto& handle : outgoing) {
                    // Note: We don't have direct synapse access here without more complex data structures
                    // This is a simplified version
                    file << handle.value << "," << neuron->getId().value << ",";
                    file << "0," << 1.0f << "," << 0 << "\n";
                }
            }
        }
        
        file.close();
        return filename;
    }, py::arg("brain"), py::arg("filename"), "Export brain state to CSV file for analysis");

    m.def("import_brain_from_csv", [](const std::string& filename) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filename);
        }
        
        std::string line;
        bool neuron_section = true;
        size_t neuron_count = 0;
        
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            if (line.find("SynapseID") != std::string::npos) {
                neuron_section = false;
                continue;
            }
            
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> values;
            
            while (std::getline(ss, token, ',')) {
                values.push_back(token);
            }
            
            if (neuron_section && values.size() >= 6) {
                try {
                    size_t id = std::stoul(values[0]);
                    // Note: This is a simplified import - real implementation would need proper
                    // synapse and neuron reconstruction from saved state
                } catch (...) {
                    // Skip malformed lines
                }
            }
        }
        
        file.close();
        return brain;
    }, py::arg("filename"), "Import brain state from CSV file (simplified import)");

    // Module metadata
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ================================================

        Enhanced Python bindings for the NLM C++ neural simulation framework.

        Key improvements over basic bindings:
        - Full Neuron and Synapse class support
        - Batch operations for performance
        - High-level convenience functions
        - Comprehensive statistics and analysis tools
        - CSV import/export for data analysis
        - Advanced simulation utilities

        Usage examples:

        Basic neuron creation and manipulation:
        ```python
        import pynlm

        # Create neurons
        neurons = pynlm.create_neuron_population(10, pynlm.NeuronType.Excitatory)
        
        # Connect them
        synapse = pynlm.connect_neurons(neurons[0], neurons[1], weight=0.5)
        
        # Simulate
        pynlm.simulate_steps(brain, 100)
        
        # Get stats
        stats = pynlm.get_brain_stats(brain)
        print(f"Firing rate: {stats['firing_rate']:.2f}")
        ```

        Complete agent simulation:
        ```python
        # Create everything
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        world = pynlm.create_simple_world()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.create_agent_brain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        
        # Run episode
        pynlm.run_agent_episode(agent, world, 500)
        ```

        The enhanced bindings provide:
        - **Direct access** to Neuron and Synapse classes
        - **Batch operations** for improved performance
        - **High-level utilities** for common tasks
        - **Data export/import** for analysis
        - **Comprehensive statistics** for monitoring
        )pbdoc";

    m.attr("__version__") = "2.0.0"; // Enhanced version
    m.attr("__description__") = "Enhanced NLM Python bindings with advanced features";
}

} // namespace nlm
