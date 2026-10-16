// NLM Python Bindings
// pybind11 implementation for the Neural Learning Machine

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/NeuralRegion.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "environment/Environment.hpp"
#include "environment/Action.hpp"
#include "environment/Observation.hpp"
#include "sensory/SensoryInput.hpp"
#include "sensory/Vision.hpp"
#include "motor/MotorSystem.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <string>
#include <memory>
#include <vector>
#include <map>

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "Python bindings for NLM (Neural Learning Machine) - an experimental artificial developmental brain";
    
    // Version
    m.attr("__version__") = "0.1.0";
    
    // Enums
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();
    
    py::enum_<nlm::ActionType>(m, "ActionType")
        .value("Wait", nlm::ActionType::Wait)
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Interact", nlm::ActionType::Interact)
        .value("LookLeft", nlm::ActionType::LookLeft)
        .value("LookRight", nlm::ActionType::LookRight)
        .value("Eat", nlm::ActionType::Eat)
        .value("PickUp", nlm::ActionType::PickUp)
        .value("Drop", nlm::ActionType::Drop)
        .value("Sleep", nlm::ActionType::Sleep)
        .value("Wake", nlm::ActionType::Wake)
        .export_values();
    
    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker)
        .export_values();
    
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();
    
    // SimpleWorldObject
    py::class_<nlm::WorldObject>(m, "WorldObject")
        .def(py::init<float, float, nlm::WorldObjectType, float>())
        .def_readwrite("x", &nlm::WorldObject::x)
        .def_readwrite("y", &nlm::WorldObject::y)
        .def_readwrite("type", &nlm::WorldObject::type)
        .def_readwrite("value", &nlm::WorldObject::value)
        .def("__repr__", [](const nlm::WorldObject& obj) {
            return "WorldObject(x=" + std::to_string(obj.x) + ", y=" + std::to_string(obj.y) +
                   ", type=" + std::to_string(static_cast<int>(obj.type)) + ", value=" + std::to_string(obj.value) + ")";
        });
    
    // Action
    py::class_<nlm::Action>(m, "Action")
        .def(py::init<nlm::ActionType>())
        .def(py::init<nlm::ActionType, float, float>())
        .def_readwrite("type", &nlm::Action::type)
        .def_readwrite("power", &nlm::Action::power)
        .def_readwrite("duration", &nlm::Action::duration)
        .def("__repr__", [](const nlm::Action& action) {
            return "Action(type=" + std::to_string(static_cast<int>(action.type)) + ", power=" + std::to_string(action.power) + ")";
        });
    
    // SensoryPercept
    py::class_<nlm::SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("getVision", &nlm::SensoryPercept::getVision)
        .def("getTouch", &nlm::SensoryPercept::getTouch)
        .def("getInternal", &nlm::SensoryPercept::getInternal)
        .def("getProprioception", &nlm::SensoryPercept::getProprioception)
        .def("setVision", &nlm::SensoryPercept::setVision)
        .def("setTouch", &nlm::SensoryPercept::setTouch)
        .def("setInternal", &nlm::SensoryPercept::setInternal)
        .def("setProprioception", &nlm::SensoryPercept::setProprioception)
        .def("__repr__", [](const nlm::SensoryPercept& percept) {
            return "SensoryPercept(vision_size=" + std::to_string(percept.getVision().size()) + ", touch_size=" + std::to_string(percept.getTouch().size()) + ")";
        });
    
    // Vision
    py::class_<nlm::Vision>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t>())
        .def("getWidth", &nlm::Vision::getWidth)
        .def("getHeight", &nlm::Vision::getHeight)
        .def("getData", &nlm::Vision::getData)
        .def("setData", &nlm::Vision::setData)
        .def("at", &nlm::Vision::at)
        .def("size", &nlm::Vision::size)
        .def("__getitem__", &nlm::Vision::at)
        .def("__setitem__", [](nlm::Vision& v, size_t idx, float val) {
            auto& data = v.getData();
            if (idx < data.size()) data[idx] = val;
        });
    
    // VisionWrapper for easier access
    py::class_<nlm::VisionWrapper>(m, "VisionWrapper")
        .def(py::init<>())
        .def("resize", &nlm::VisionWrapper::resize)
        .def("getData", &nlm::VisionWrapper::getData)
        .def("setData", &nlm::VisionWrapper::setData)
        .def("at", &nlm::VisionWrapper::at)
        .def("size", &nlm::VisionWrapper::size)
        .def("__getitem__", &nlm::VisionWrapper::at)
        .def("__setitem__", [](nlm::VisionWrapper& v, size_t idx, float val) {
            auto& data = v.getData();
            if (idx < data.size()) data[idx] = val;
        });
    
    // Configuration
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("set", py::overload_cast<const std::string&, int64_t, nlm::ConfigSource>(&nlm::Config::set))
        .def("set", py::overload_cast<const std::string&, float, nlm::ConfigSource>(&nlm::Config::set))
        .def("set", py::overload_cast<const std::string&, double, nlm::ConfigSource>(&nlm::Config::set))
        .def("set", py::overload_cast<const std::string&, const std::string&, nlm::ConfigSource>(&nlm::Config::set))
        .def("getOr", py::overload_cast<const std::string&, int64_t, nlm::ConfigSource>(&nlm::Config::getOr))
        .def("getOr", py::overload_cast<const std::string&, float, nlm::ConfigSource>(&nlm::Config::getOr))
        .def("getOr", py::overload_cast<const std::string&, double, nlm::ConfigSource>(&nlm::Config::getOr))
        .def("getOr", py::overload_cast<const std::string&, const std::string&, nlm::ConfigSource>(&nlm::Config::getOr))
        .def("has", &nlm::Config::has)
        .def("getKeys", &nlm::Config::getKeys)
        .def("loadFromFile", &nlm::Config::loadFromFile)
        .def("saveToFile", &nlm::Config::saveToFile)
        .def("loadFromArgs", &nlm::Config::loadFromArgs)
        .def("clear", &nlm::Config::clear)
        .def_static("createDefaultConfig", &nlm::Config::createDefaultConfig);
    
    // ConfigSource enum
    py::enum_<nlm::ConfigSource>(m, "ConfigSource")
        .value("Default", nlm::ConfigSource::Default)
        .value("File", nlm::ConfigSource::File)
        .value("CommandLine", nlm::ConfigSource::CommandLine)
        .export_values();
    
    // Neuron
    py::class_<nlm::Neuron>(m, "Neuron")
        .def_readonly("id", &nlm::Neuron::getId)
        .def_readonly("type", &nlm::Neuron::getType)
        .def("getMembranePotential", &nlm::Neuron::getMembranePotential)
        .def("setMembranePotential", &nlm::Neuron::setMembranePotential)
        .def("getRestingPotential", &nlm::Neuron::getRestingPotential)
        .def("getThreshold", &nlm::Neuron::getThreshold)
        .def("getResetPotential", &nlm::Neuron::getResetPotential)
        .def("getFiringState", &nlm::Neuron::getFiringState)
        .def("getRefractoryRemaining", &nlm::Neuron::getRefractoryRemaining)
        .def("getRefractoryPeriod", &nlm::Neuron::getRefractoryPeriod)
        .def("getLastSpikeTime", &nlm::Neuron::getLastSpikeTime)
        .def("injectCurrent", &nlm::Neuron::injectCurrent)
        .def("receiveExcitatoryInput", &nlm::Neuron::receiveExcitatoryInput)
        .def("receiveInhibitoryInput", &nlm::Neuron::receiveInhibitoryInput)
        .def("stepLIF", &nlm::Neuron::stepLIF)
        .def("isFiring", &nlm::Neuron::isFiring)
        .def("getState", &nlm::Neuron::getState)
        .def("setState", &nlm::Neuron::setState)
        .def("__repr__", [](const nlm::Neuron& neuron) {
            return "Neuron(id=" + std::to_string(neuron.getId()) + ", type=" + std::to_string(static_cast<int>(neuron.getType())) + ")";
        });
    
    // Synapse
    py::class_<nlm::Synapse>(m, "Synapse")
        .def(py::init<nlm::SynapseId, nlm::NeuronId, nlm::NeuronId>())
        .def_readonly("id", &nlm::Synapse::getId)
        .def_readonly("source_neuron", &nlm::Synapse::getSourceNeuron)
        .def_readonly("destination_neuron", &nlm::Synapse::getDestinationNeuron)
        .def("getWeight", &nlm::Synapse::getWeight)
        .def("setWeight", &nlm::Synapse::setWeight)
        .def("getType", &nlm::Synapse::getType)
        .def("setType", &nlm::Synapse::setType)
        .def("getDelay", &nlm::Synapse::getDelay)
        .def("setDelay", &nlm::Synapse::setDelay)
        .def("isExcitatory", &nlm::Synapse::isExcitatory)
        .def("isInhibitory", &nlm::Synapse::isInhibitory)
        .def("addToWeight", &nlm::Synapse::addToWeight)
        .def("enablePlasticity", &nlm::Synapse::enablePlasticity)
        .def("disablePlasticity", &nlm::Synapse::disablePlasticity)
        .def("getPlasticityFlags", &nlm::Synapse::getPlasticityFlags)
        .def("getEligibilityTrace", &nlm::Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", &nlm::Synapse::setEligibilityTrace)
        .def("decayEligibilityTrace", &nlm::Synapse::decayEligibilityTrace)
        .def("recordPreSpike", &nlm::Synapse::recordPreSpike)
        .def("recordPostSpike", &nlm::Synapse::recordPostSpike)
        .def("getPreSpikeHistory", &nlm::Synapse::getPreSpikeHistory)
        .def("getPostSpikeHistory", &nlm::Synapse::getPostSpikeHistory)
        .def("step", &nlm::Synapse::step)
        .def("__repr__", [](const nlm::Synapse& synapse) {
            return "Synapse(id=" + std::to_string(synapse.getId().index()) + ", w=" + std::to_string(synapse.getWeight()) + ")";
        });
    
    // NeuralPopulation
    py::class_<nlm::NeuralPopulation>(m, "NeuralPopulation")
        .def_readonly("id", &nlm::NeuralPopulation::getId)
        .def_readonly("type", &nlm::NeuralPopulation::getNeuronType)
        .def("getNeuronCount", &nlm::NeuralPopulation::getNeuronCount)
        .def("getNeurons", &nlm::NeuralPopulation::getNeurons)
        .def("getNeuron", py::overload_cast<size_t>(&nlm::NeuralPopulation::getNeuron))
        .def("addNeuron", &nlm::NeuralPopulation::addNeuron)
        .def("removeNeuron", &nlm::NeuralPopulation::removeNeuron)
        .def("clearNeurons", &nlm::NeuralPopulation::clearNeurons)
        .def("initializeRandomConnectivity", &nlm::NeuralPopulation::initializeRandomConnectivity)
        .def("getAverageFiringRate", &nlm::NeuralPopulation::getAverageFiringRate)
        .def("getActiveNeuronCount", &nlm::NeuralPopulation::getActiveNeuronCount)
        .def("__repr__", [](const nlm::NeuralPopulation& pop) {
            return "NeuralPopulation(id=" + std::to_string(pop.getId().index()) + ", neurons=" + std::to_string(pop.getNeuronCount()) + ")";
        });
    
    // NeuralRegion
    py::class_<nlm::NeuralRegion>(m, "NeuralRegion")
        .def(py::init<nlm::RegionId, std::string>())
        .def_readonly("id", &nlm::NeuralRegion::getId)
        .def_readonly("name", &nlm::NeuralRegion::getName)
        .def("getNeuronCount", &nlm::NeuralRegion::getNeuronCount)
        .def("getSynapseCount", &nlm::NeuralRegion::getSynapseCount)
        .def("getPopulationCount", &nlm::NeuralRegion::getPopulationCount)
        .def("addPopulation", &nlm::NeuralRegion::addPopulation)
        .def("getPopulation", &nlm::NeuralRegion::getPopulation)
        .def("removePopulation", &nlm::NeuralRegion::removePopulation)
        .def("getPopulations", &nlm::NeuralRegion::getPopulations)
        .def("addSynapse", &nlm::NeuralRegion::addSynapse)
        .def("getSynapse", py::overload_cast<size_t>(&nlm::NeuralRegion::getSynapse))
        .def("removeSynapse", &nlm::NeuralRegion::removeSynapse)
        .def("getSynapses", &nlm::NeuralRegion::getSynapses)
        .def("getAllNeurons", &nlm::NeuralRegion::getAllNeurons)
        .def("getNeuronsTo", &nlm::NeuralRegion::getNeuronsTo)
        .def("getNeuronsFrom", &nlm::NeuralRegion::getNeuronsFrom)
        .def("getSynapsesTo", &nlm::NeuralRegion::getSynapsesTo)
        .def("getSynapsesFrom", &nlm::NeuralRegion::getSynapsesFrom)
        .def("getSynapses", py::overload_cast<>&nlm::NeuralRegion::getSynapses)
        .def("initializeRandomConnectivity", &nlm::NeuralRegion::initializeRandomConnectivity)
        .def("getAverageSynapticWeight", &nlm::NeuralRegion::getAverageSynapticWeight)
        .def("getActiveNeuronCount", &nlm::NeuralRegion::getActiveNeuronCount)
        .def("reset", &nlm::NeuralRegion::reset)
        .def("logStatus", &nlm::NeuralRegion::logStatus)
        .def("__repr__", [](const nlm::NeuralRegion& region) {
            return "NeuralRegion(id=" + std::to_string(region.getId().index()) + ", name=" + region.getName() + ")";
        });
    
    // RegionId
    py::class_<nlm::RegionId>(m, "RegionId")
        .def(py::init<size_t>())
        .def("index", &nlm::RegionId::index)
        .def("__eq__", &nlm::RegionId::operator==)
        .def("__hash__", [](const nlm::RegionId& id) { return std::hash<size_t>()(id.index()); });
    
    // NeuronId
    py::class_<nlm::NeuronId>(m, "NeuronId")
        .def(py::init<size_t>())
        .def("index", &nlm::NeuronId::index)
        .def("__eq__", &nlm::NeuronId::operator==)
        .def("__hash__", [](const nlm::NeuronId& id) { return std::hash<size_t>()(id.index()); });
    
    // SynapseId
    py::class_<nlm::SynapseId>(m, "SynapseId")
        .def(py::init<size_t>())
        .def("index", &nlm::SynapseId::index)
        .def("__eq__", &nlm::SynapseId::operator==)
        .def("__hash__", [](const nlm::SynapseId& id) { return std::hash<size_t>()(id.index()); });
    
    // Configuration system utilities
    m.def("createDefaultConfig", &nlm::Config::createDefaultConfig);
    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) {
        return std::make_shared<nlm::Brain>(config);
    });
    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) {
        return std::make_shared<nlm::AgentBrain>(brain);
    });
    m.def("createSimpleWorld", []() {
        return std::make_shared<nlm::SimpleWorld>();
    });
    
    // Brain class
    py::class_<nlm::Brain, std::shared_ptr<nlm::Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>())
        .def("initialize", &nlm::Brain::initialize)
        .def("step", py::overload_cast<nlm::SimulationStep>(&nlm::Brain::step))
        .def("step", py::overload_cast<nlm::SimulationStep, nlm::Timestamp>(&nlm::Brain::step))
        .def("reset", &nlm::Brain::reset)
        .def("logStatus", &nlm::Brain::logStatus)
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount)
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate)
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount)
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio)
        .def("getRegionCount", &nlm::Brain::getRegionCount)
        .def("getRegionIds", &nlm::Brain::getRegionIds)
        .def("getRegion", &nlm::Brain::getRegion)
        .def("addRegion", &nlm::Brain::addRegion)
        .def("removeRegion", &nlm::Brain::removeRegion)
        .def("getWorkingMemory", &nlm::Brain::getWorkingMemory)
        .def("getEpisodicMemory", &nlm::Brain::getEpisodicMemory)
        .def("getAssociativeMemory", &nlm::Brain::getAssociativeMemory)
        .def("getPredictionSystem", &nlm::Brain::getPredictionSystem)
        .def("getPlanner", &nlm::Brain::getPlanner)
        .def("getConceptFormation", &nlm::Brain::getConceptFormation)
        .def("getAttention", &nlm::Brain::getAttention)
        .def("getDevelopmentSystem", &nlm::Brain::getDevelopmentSystem)
        .def("getDopamine", &nlm::Brain::getDopamine)
        .def("getCuriosity", &nlm::Brain::getCuriosity)
        .def("getNovelty", &nlm::Brain::getNovelty)
        .def("getPredictionErrorSignal", &nlm::Brain::getPredictionErrorSignal)
        .def("getRandomGenerator", &nlm::Brain::getRandomGenerator)
        .def("getSpikeSystem", &nlm::Brain::getSpikeSystem)
        .def("getSTDP", &nlm::Brain::getSTDP)
        .def("getHebbian", &nlm::Brain::getHebbian)
        .def("getStructuralPlasticity", &nlm::Brain::getStructuralPlasticity)
        .def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage)
        .def("setDevelopmentalStage", &nlm::Brain::setDevelopmentalStage)
        .def("injectCurrent", &nlm::Brain::injectCurrent)
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons)
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput)
        .def("produceAction", &nlm::Brain::produceAction)
        .def("applyNeuromodulation", &nlm::Brain::applyNeuromodulation)
        .def("updatePlasticity", &nlm::Brain::updatePlasticity)
        .def("develop", &nlm::Brain::develop)
        .def("save", &nlm::Brain::save)
        .def("load", &nlm::Brain::load)
        .def("enableRewardModulation", &nlm::Brain::enableRewardModulation)
        .def("enableStructuralPlasticity", &nlm::Brain::enableStructuralPlasticity)
        .def("enableDevelopment", &nlm::Brain::enableDevelopment)
        .def("enableCuriosity", &nlm::Brain::enableCuriosity)
        .def("getCuriosityLevel", &nlm::Brain::getCuriosityLevel)
        .def("getNoveltyLevel", &nlm::Brain::getNoveltyLevel)
        .def("getPredictionError", &nlm::Brain::getPredictionError)
        .def("getNeuromodulationLevel", &nlm::Brain::getNeuromodulationLevel);
    
    // AgentBrain class
    py::class_<nlm::AgentBrain, std::shared_ptr<nlm::AgentBrain>>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>())
        .def("initialize", &nlm::AgentBrain::initialize)
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand)
        .def("decodeFromMotorNeurons", &nlm::AgentBrain::decodeFromMotorNeurons)
        .def("selectWithCuriosity", &nlm::AgentBrain::selectWithCuriosity)
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation)
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment)
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError)
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize)
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment)
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity)
        .def("reset", &nlm::AgentBrain::reset)
        .def("setNoveltyThreshold", &nlm::AgentBrain::setNoveltyThreshold)
        .def("getNoveltyThreshold", &nlm::AgentBrain::getNoveltyThreshold)
        .def("setCuriosityDecay", &nlm::AgentBrain::setCuriosityDecay)
        .def("getCuriosityDecay", &nlm::AgentBrain::getCuriosityDecay);
    
    // SimpleWorld class
    py::class_<nlm::SimpleWorld, std::shared_ptr<nlm::SimpleWorld>>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", [](nlm::SimpleWorld& world, size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
            world.configure(width, height, visionWidth, visionHeight);
        })
        .def("configure", [](nlm::SimpleWorld& world, size_t width, size_t height) {
            world.configure(width, height);
        })
        .def("getWidth", &nlm::SimpleWorld::getWidth)
        .def("getHeight", &nlm::SimpleWorld::getHeight)
        .def("getVisionWidth", &nlm::SimpleWorld::getVisionWidth)
        .def("getVisionHeight", &nlm::SimpleWorld::getVisionHeight)
        .def("reset", &nlm::SimpleWorld::reset)
        .def("update", &nlm::SimpleWorld::update)
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept)
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart)
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand)
        .def("getAgentPosition", &nlm::SimpleWorld::getAgentPosition)
        .def("getAgentRotation", &nlm::SimpleWorld::getAgentRotation)
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed)
        .def("addObject", &nlm::SimpleWorld::addObject)
        .def("removeObject", &nlm::SimpleWorld::removeObject)
        .def("getObjectAt", &nlm::SimpleWorld::getObjectAt)
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition)
        .def("getObjects", &nlm::SimpleWorld::getObjects)
        .def("clearObjects", &nlm::SimpleWorld::clearObjects)
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy)
        .def("getEnergy", &nlm::SimpleWorld::getEnergy)
        .def("setEnergy", &nlm::SimpleWorld::setEnergy)
        .def("getResources", &nlm::SimpleWorld::getResources)
        .def("getHazards", &nlm::SimpleWorld::getHazards)
        .def("getWalls", &nlm::SimpleWorld::getWalls)
        .def("getMarkers", &nlm::SimpleWorld::getMarkers)
        .def("logStatus", &nlm::SimpleWorld::logStatus);
    
    // Neural working memory
    py::class_<nlm::NeuralWorkingMemory>(m, "NeuralWorkingMemory")
        .def("initialize", &nlm::NeuralWorkingMemory::initialize)
        .def("storeToNeuron", &nlm::NeuralWorkingMemory::storeToNeuron)
        .def("update", &nlm::NeuralWorkingMemory::update)
        .def("getActiveTraces", &nlm::NeuralWorkingMemory::getActiveTraces)
        .def("getMemoryNeurons", &nlm::NeuralWorkingMemory::getMemoryNeurons)
        .def("clear", &nlm::NeuralWorkingMemory::clear)
        .def("setCapacity", &nlm::NeuralWorkingMemory::setCapacity)
        .def("getCapacity", &nlm::NeuralWorkingMemory::getCapacity);
    
    // Neural episodic memory
    py::class_<nlm::NeuralEpisodicMemory>(m, "NeuralEpisodicMemory")
        .def("initialize", &nlm::NeuralEpisodicMemory::initialize)
        .def("storeEpisode", &nlm::NeuralEpisodicMemory::storeEpisode)
        .def("getEpisodeCount", &nlm::NeuralEpisodicMemory::getEpisodeCount)
        .def("getEpisodes", &nlm::NeuralEpisodicMemory::getEpisodes)
        .def("getEpisodesForReplay", &nlm::NeuralEpisodicMemory::getEpisodesForReplay)
        .def("replayEpisode", &nlm::NeuralEpisodicMemory::replayEpisode)
        .def("consolidate", &nlm::NeuralEpisodicMemory::consolidate)
        .def("clear", &nlm::NeuralEpisodicMemory::clear)
        .def("setMaxEpisodes", &nlm::NeuralEpisodicMemory::setMaxEpisodes)
        .def("getMaxEpisodes", &nlm::NeuralEpisodicMemory::getMaxEpisodes);
    
    // Neural associative memory
    py::class_<nlm::NeuralAssociativeMemory>(m, "NeuralAssociativeMemory")
        .def("initialize", &nlm::NeuralAssociativeMemory::initialize)
        .def("storePattern", &nlm::NeuralAssociativeMemory::storePattern)
        .def("recallPattern", &nlm::NeuralAssociativeMemory::recallPattern)
        .def("getPatternCount", &nlm::NeuralAssociativeMemory::getPatternCount)
        .def("clear", &nlm::NeuralAssociativeMemory::clear);
    
    // Spike system
    py::class_<nlm::SpikeSystem>(m, "SpikeSystem")
        .def("registerHandler", &nlm::SpikeSystem::registerHandler)
        .def("registerDelayedHandler", &nlm::SpikeSystem::registerDelayedHandler)
        .def("queueSpike", &nlm::SpikeSystem::queueSpike)
        .def("queueDelayedSpike", &nlm::SpikeSystem::queueDelayedSpike)
        .def("processSpikes", &nlm::SpikeSystem::processSpikes)
        .def("processDelayedSpikes", &nlm::SpikeSystem::processDelayedSpikes)
        .def("getPendingSpikeCount", &nlm::SpikeSystem::getPendingSpikeCount)
        .def("getPendingDelayedCount", &nlm::SpikeSystem::getPendingDelayedCount)
        .def("reset", &nlm::SpikeSystem::reset)
        .def("getSpikeHistory", &nlm::SpikeSystem::getSpikeHistory);
    
    // Neural prediction system
    py::class_<nlm::PredictionSystem>(m, "PredictionSystem")
        .def("initialize", &nlm::PredictionSystem::initialize)
        .def("update", &nlm::PredictionSystem::update)
        .def("getPredictionError", &nlm::PredictionSystem::getPredictionError)
        .def("getPredictionConfidence", &nlm::PredictionSystem::getPredictionConfidence)
        .def("clear", &nlm::PredictionSystem::clear);
    
    // Neural planner
    py::class_<nlm::NeuralPlanner>(m, "NeuralPlanner")
        .def("initialize", &nlm::NeuralPlanner::initialize)
        .def("update", &nlm::NeuralPlanner::update)
        .def("getActionPlan", &nlm::NeuralPlanner::getActionPlan)
        .def("getPlanningScore", &nlm::NeuralPlanner::getPlanningScore)
        .def("clear", &nlm::NeuralPlanner::clear)
        .def("setPlanningDepth", &nlm::NeuralPlanner::setPlanningDepth)
        .def("getPlanningDepth", &nlm::NeuralPlanner::getPlanningDepth);
    
    // Concept formation
    py::class_<nlm::ConceptFormation>(m, "ConceptFormation")
        .def("initialize", &nlm::ConceptFormation::initialize)
        .def("update", &nlm::ConceptFormation::update)
        .def("getConcepts", &nlm::ConceptFormation::getConcepts)
        .def("getConceptStrength", &nlm::ConceptFormation::getConceptStrength)
        .def("clear", &nlm::ConceptFormation::clear);
    
    // Attention system
    py::class_<nlm::AttentionalSelection>(m, "AttentionalSelection")
        .def("initialize", &nlm::AttentionalSelection::initialize)
        .def("update", &nlm::AttentionalSelection::update)
        .def("processCompetition", &nlm::AttentionalSelection::processCompetition)
        .def("getFocusNeuron", &nlm::AttentionalSelection::getFocusNeuron)
        .def("getAttentionLevel", &nlm::AttentionalSelection::getAttentionLevel)
        .def("clear", &nlm::AttentionalSelection::clear)
        .def("setInhibitionStrength", &nlm::AttentionalSelection::setInhibitionStrength)
        .def("setExcitationStrength", &nlm::AttentionalSelection::setExcitationStrength);
    
    // Development system
    py::class_<nlm::DevelopmentSystem>(m, "DevelopmentSystem")
        .def("initialize", &nlm::DevelopmentSystem::initialize)
        .def("update", &nlm::DevelopmentSystem::update)
        .def("getDevelopmentalStage", &nlm::DevelopmentSystem::getDevelopmentalStage)
        .def("getPlasticityRate", &nlm::DevelopmentSystem::getPlasticityRate)
        .def("getMaturationProgress", &nlm::DevelopmentSystem::getMaturationProgress)
        .def("clear", &nlm::DevelopmentSystem::clear);
    
    // Neuromodulators
    py::class_<nlm::Dopamine>(m, "Dopamine")
        .def("initialize", &nlm::Dopamine::initialize)
        .def("update", &nlm::Dopamine::update)
        .def("getLevel", &nlm::Dopamine::getLevel)
        .def("getPlasticityFactor", &nlm::Dopamine::getPlasticityFactor)
        .def("clear", &nlm::Dopamine::clear);
    
    py::class_<nlm::Curiosity>(m, "Curiosity")
        .def("initialize", &nlm::Curiosity::initialize)
        .def("update", &nlm::Curiosity::update)
        .def("getLevel", &nlm::Curiosity::getLevel)
        .def("clear", &nlm::Curiosity::clear);
    
    py::class_<nlm::Novelty>(m, "Novelty")
        .def("initialize", &nlm::Novelty::initialize)
        .def("update", &nlm::Novelty::update)
        .def("getLevel", &nlm::Novelty::getLevel)
        .def("clear", &nlm::Novelty::clear);
    
    py::class_<nlm::PredictionError>(m, "PredictionError")
        .def("initialize", &nlm::PredictionError::initialize)
        .def("update", &nlm::PredictionError::update)
        .def("getLevel", &nlm::PredictionError::getLevel)
        .def("clear", &nlm::PredictionError::clear);
    
    // Plasticity systems
    py::class_<nlm::STDP>(m, "STDP")
        .def(py::init<>())
        .def("configure", &nlm::STDP::configure)
        .def("update", &nlm::STDP::update)
        .def("getLTPWeight", &nlm::STDP::getLTPWeight)
        .def("setLTPWeight", &nlm::STDP::setLTPWeight)
        .def("getLTDWeight", &nlm::STDP::getLTDWeight)
        .def("setLTDWeight", &nlm::STDP::setLTDWeight)
        .def("getTimeConstant", &nlm::STDP::getTimeConstant)
        .def("setTimeConstant", &nlm::STDP::setTimeConstant)
        .def("getName", &nlm::STDP::getName)
        .def("clear", &nlm::STDP::clear);
    
    py::class_<nlm::Hebbian>(m, "Hebbian")
        .def(py::init<>())
        .def("update", &nlm::Hebbian::update)
        .def("getLearningRate", &nlm::Hebbian::getLearningRate)
        .def("setLearningRate", &nlm::Hebbian::setLearningRate)
        .def("getName", &nlm::Hebbian::getName)
        .def("clear", &nlm::Hebbian::clear);
    
    py::class_<nlm::StructuralPlasticity>(m, "StructuralPlasticity")
        .def(py::init<>())
        .def("update", &nlm::StructuralPlasticity::update)
        .def("setSynaptogenesisRate", &nlm::StructuralPlasticity::setSynaptogenesisRate)
        .def("getSynaptogenesisRate", &nlm::StructuralPlasticity::getSynaptogenesisRate)
        .def("setPruningRate", &nlm::StructuralPlasticity::setPruningRate)
        .def("getPruningRate", &nlm::StructuralPlasticity::getPruningRate)
        .def("getName", &nlm::StructuralPlasticity::getName)
        .def("clear", &nlm::StructuralPlasticity::clear);
    
    // Developmental stages
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();
    
    // Configuration source
    py::enum_<nlm::ConfigSource>(m, "ConfigSource")
        .value("Default", nlm::ConfigSource::Default)
        .value("File", nlm::ConfigSource::File)
        .value("CommandLine", nlm::ConfigSource::CommandLine)
        .export_values();
    
    // Random generator
    py::class_<nlm::RandomGenerator>(m, "RandomGenerator")
        .def(py::init<uint64_t>())
        .def("uniformReal", &nlm::RandomGenerator::uniformReal)
        .def("uniformInt", &nlm::RandomGenerator::uniformInt)
        .def("normalReal", &nlm::RandomGenerator::normalReal)
        .def("gaussian", &nlm::RandomGenerator::gaussian)
        .def("exponential", &nlm::RandomGenerator::exponential)
        .def("bernoulli", &nlm::RandomGenerator::bernoulli)
        .def("seed", &nlm::RandomGenerator::seed)
        .def("getSeed", &nlm::RandomGenerator::getSeed);
    
    // Spike events
    py::class_<nlm::SpikeEvent>(m, "SpikeEvent")
        .def(py::init<nlm::NeuronId, nlm::Timestamp, nlm::SimulationStep>())
        .def_readonly("neuron_id", &nlm::SpikeEvent::neuronId)
        .def_readonly("timestamp", &nlm::SpikeEvent::timestamp)
        .def_readonly("step", &nlm::SpikeEvent::step);
    
    py::class_<nlm::DelayedSpikeEvent>(m, "DelayedSpikeEvent")
        .def(py::init<nlm::NeuronId, nlm::NeuronId, nlm::SynapseId, float, nlm::SynapseType, 
                                   nlm::Timestamp, nlm::Timestamp, nlm::SimulationStep, nlm::SimulationStep>())
        .def_readonly("source_neuron", &nlm::DelayedSpikeEvent::sourceNeuron)
        .def_readonly("destination_neuron", &nlm::DelayedSpikeEvent::destinationNeuron)
        .def_readonly("synapse_id", &nlm::DelayedSpikeEvent::synapseId)
        .def_readonly("weight", &nlm::DelayedSpikeEvent::weight)
        .def_readonly("type", &nlm::DelayedSpikeEvent::type)
        .def_readonly("timestamp", &nlm::DelayedSpikeEvent::timestamp)
        .def_readonly("delivery_time", &nlm::DelayedSpikeEvent::deliveryTime)
        .def_readonly("step", &nlm::DelayedSpikeEvent::step)
        .def_readonly("delivery_step", &nlm::DelayedSpikeEvent::deliveryStep)
        .def_readonly("is_excitatory", &nlm::DelayedSpikeEvent::is_excitatory);
    
    // Type aliases
    m.def("SimulationStep", &nlm::SimulationStep);
    m.def("Timestamp", &nlm::Timestamp);
    m.def("MembranePotential", &nlm::MembranePotential);
    m.def("Delay", &nlm::Delay);
    
    // Helper functions
    m.def("createDefaultConfig", &nlm::Config::createDefaultConfig);
    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) {
        return std::make_shared<nlm::Brain>(config);
    });
    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) {
        return std::make_shared<nlm::AgentBrain>(brain);
    });
    m.def("createSimpleWorld", []() {
        return std::make_shared<nlm::SimpleWorld>();
    });
    
    // Example usage patterns
    m.def("run_simple_simulation", [](int steps) {
        auto config = nlm::Config::createDefaultConfig();
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        return brain->getTotalSpikeCount();
    });
    
    m.def("run_agent_simulation", [](int steps, int worldWidth, int worldHeight) {
        auto config = nlm::Config::createDefaultConfig();
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        auto world = nlm::createSimpleWorld();
        world->configure(worldWidth, worldHeight);
        world->reset();
        
        auto agent = nlm::createAgentBrain(brain);
        agent->initialize(world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        for (int i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return brain->getTotalSpikeCount();
    });
    
    // Version info
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine) - an experimental artificial developmental brain";
    
    // Enable easy printing
    py::add_ostream_redirect(m, "cout");
}