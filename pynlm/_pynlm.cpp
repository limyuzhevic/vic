"""
Python bindings for NLM using pybind11

This module provides the C++ extension interface for Python.
"""

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/functional.h"
#include "pybind11/iostream.h"

#include "core/Types/Types.hpp"
#include "core/Error/Error.hpp"
#include "core/Factory/Factory.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "development/DevelopmentSystem.hpp"
#include "world/SimpleWorld.hpp"
#include "world/Observation.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "environment/Environment.hpp"
#include "environment/Observation.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_pynlm, m) {
    m.doc() = "NLM Python bindings for neural simulation";
    
    // Version information
    m.attr("__version__") = "2.0.0";
    
    // Type aliases
    m.attr("NeuronId") = py::type<::nlm::NeuronId>();
    m.attr("SynapseId") = py::type<::nlm::SynapseId>();
    m.attr("RegionId") = py::type<::nlm::RegionId>();
    m.attr("PopulationId") = py::type<::nlm::PopulationId>();
    m.attr("Timestamp") = py::type<::nlm::Timestamp>();
    m.attr("TimestepDuration") = py::type<::nlm::TimestepDuration>();
    m.attr("SimulationStep") = py::type<::nlm::SimulationStep>();
    
    // Enums
    auto neuronType = py::enum_<::nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", ::nlm::NeuronType::Excitatory)
        .value("Inhibitory", ::nlm::NeuronType::Inhibitory)
        .value("Sensory", ::nlm::NeuronType::Sensory)
        .value("Motor", ::nlm::NeuronType::Motor)
        .value("Modulatory", ::nlm::NeuronType::Modulatory)
        .value("Internal", ::nlm::NeuronType::Internal)
        .export_values();
    
    auto firingState = py::enum_<::nlm::FiringState>(m, "FiringState")
        .value("Resting", ::nlm::FiringState::Resting)
        .value("Active", ::nlm::FiringState::Active)
        .value("Refractory", ::nlm::FiringState::Refractory)
        .export_values();
    
    auto actionType = py::enum_<::nlm::ActionType>(m, "ActionType")
        .value("Wait", ::nlm::ActionType::Wait)
        .value("MoveForward", ::nlm::ActionType::MoveForward)
        .value("MoveBackward", ::nlm::ActionType::MoveBackward)
        .value("TurnLeft", ::nlm::ActionType::TurnLeft)
        .value("TurnRight", ::nlm::ActionType::TurnRight)
        .value("Interact", ::nlm::ActionType::Interact)
        .value("Eat", ::nlm::ActionType::Eat)
        .value("LookLeft", ::nlm::ActionType::LookLeft)
        .value("LookRight", ::nlm::ActionType::LookRight)
        .export_values();
    
    auto developmentalStage = py::enum_<::nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", ::nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", ::nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", ::nlm::DevelopmentalStage::Maturation)
        .value("Adult", ::nlm::DevelopmentalStage::Adult)
        .value("Aging", ::nlm::DevelopmentalStage::Aging)
        .export_values();
    
    auto synapseType = py::enum_<::nlm::SynapseType>(m, "SynapseType")
        .value("Excitatory", ::nlm::SynapseType::Excitatory)
        .value("Inhibitory", ::nlm::SynapseType::Inhibitory)
        .value("Modulatory", ::nlm::SynapseType::Modulatory)
        .export_values();
    
    auto worldObjectType = py::enum_<::nlm::WorldObjectType>(m, "WorldObjectType")
        .value("Empty", ::nlm::WorldObjectType::Empty)
        .value("Resource", ::nlm::WorldObjectType::Resource)
        .value("Hazard", ::nlm::WorldObjectType::Hazard)
        .value("Wall", ::nlm::WorldObjectType::Wall)
        .value("Marker", ::nlm::WorldObjectType::Marker)
        .export_values();
    
    // Log levels
    auto logLevel = py::enum_<::nlm::LogLevel>(m, "LogLevel")
        .value("Debug", ::nlm::LogLevel::Debug)
        .value("Info", ::nlm::LogLevel::Info)
        .value("Warning", ::nlm::LogLevel::Warning)
        .value("Error", ::nlm::LogLevel::Error)
        .value("Critical", ::nlm::LogLevel::Critical)
        .export_values();
    
    // Plasticity flags
    py::class_<::nlm::PlasticityFlags>(m, "PlasticityFlags")
        .def_readwrite("hebbian", &::nlm::PlasticityFlags::hebbian)
        .def_readwrite("stdp", &::nlm::PlasticityFlags::stdp)
        .def_readwrite("reward_modulated", &::nlm::PlasticityFlags::reward_modulated);
    
    // Neuron class
    py::class_<::nlm::Neuron, std::shared_ptr<::nlm::Neuron>>(m, "Neuron")
        .def(py::init<::nlm::NeuronId>())
        .def("getId", &::nlm::Neuron::getId)
        .def("getType", &::nlm::Neuron::getType)
        .def("setType", &::nlm::Neuron::setType)
        .def("getMembranePotential", &::nlm::Neuron::getMembranePotential)
        .def("setMembranePotential", &::nlm::Neuron::setMembranePotential)
        .def("addToMembranePotential", &::nlm::Neuron::addToMembranePotential)
        .def("getThreshold", &::nlm::Neuron::getThreshold)
        .def("setThreshold", &::nlm::Neuron::setThreshold)
        .def("isFiring", &::nlm::Neuron::isFiring)
        .def("isRefractory", &::nlm::Neuron::isRefractory)
        .def("getFiringRate", &::nlm::Neuron::getFiringRate)
        .def("setFiringRate", &::nlm::Neuron::setFiringRate)
        .def("setLeakConductance", &::nlm::Neuron::setLeakConductance)
        .def("getLeakConductance", &::nlm::Neuron::getLeakConductance)
        .def("getRefractoryPeriod", &::nlm::Neuron::getRefractoryPeriod)
        .def("setRefractoryPeriod", &::nlm::Neuron::setRefractoryPeriod)
        .def("setRestingPotential", &::nlm::Neuron::setRestingPotential)
        .def("getRestingPotential", &::nlm::Neuron::getRestingPotential)
        .def("setResetPotential", &::nlm::Neuron::setResetPotential)
        .def("checkThreshold", &::nlm::Neuron::checkThreshold)
        .def("getLastSpikeTime", &::nlm::Neuron::getLastSpikeTime)
        .def("stepLIF", &::nlm::Neuron::stepLIF)
        .def("step", py::overload_cast<::nlm::Timestamp>(&::nlm::Neuron::step))
        .def("receiveExcitatoryInput", &::nlm::Neuron::receiveExcitatoryInput)
        .def("receiveInhibitoryInput", &::nlm::Neuron::receiveInhibitoryInput)
        .def("receiveModulatoryInput", &::nlm::Neuron::receiveModulatoryInput)
        .def("injectCurrent", &::nlm::Neuron::injectCurrent)
        .def("recordSpike", &::nlm::Neuron::recordSpike)
        .def("getSpikeHistory", &::nlm::Neuron::getSpikeHistory)
        .def("clearSpikeHistory", &::nlm::Neuron::clearSpikeHistory)
        .def("addIncomingSynapse", &::nlm::Neuron::addIncomingSynapse)
        .def("addOutgoingSynapse", &::nlm::Neuron::addOutgoingSynapse)
        .def("getIncomingSynapses", &::nlm::Neuron::getIncomingSynapses)
        .def("getOutgoingSynapses", &::nlm::Neuron::getOutgoingSynapses)
        .def("getState", &::nlm::Neuron::getState)
        .def("getPlasticityFlags", &::nlm::Neuron::getPlasticityFlags)
        .def("enablePlasticity", &::nlm::Neuron::enablePlasticity)
        .def("setRegionId", &::nlm::Neuron::setRegionId)
        .def("getRegionId", &::nlm::Neuron::getRegionId)
        .def("setPopulationId", &::nlm::Neuron::setPopulationId)
        .def("getPopulationId", &::nlm::Neuron::getPopulationId)
        .def("reset", &::nlm::Neuron::reset)
        .def("initializeRandom", &::nlm::Neuron::initializeRandom)
        .def("isFiring", &::nlm::Neuron::isFiring)
        .def("getMembranePotential", &::nlm::Neuron::getMembranePotential);
    
    // Synapse class
    py::class_<::nlm::Synapse, std::shared_ptr<::nlm::Synapse>>(m, "Synapse")
        .def(py::init<::nlm::SynapseId, ::nlm::NeuronId, ::nlm::NeuronId>())
        .def("getId", &::nlm::Synapse::getId)
        .def("getSourceNeuron", &::nlm::Synapse::getSourceNeuron)
        .def("getDestinationNeuron", &::nlm::Synapse::getDestinationNeuron)
        .def("getWeight", &::nlm::Synapse::getWeight)
        .def("setWeight", &::nlm::Synapse::setWeight)
        .def("addToWeight", &::nlm::Synapse::addToWeight)
        .def("getDelay", &::nlm::Synapse::getDelay)
        .def("setDelay", &::nlm::Synapse::setDelay)
        .def("getType", &::nlm::Synapse::getType)
        .def("setType", &::nlm::Synapse::setType)
        .def("isExcitatory", &::nlm::Synapse::isExcitatory)
        .def("isInhibitory", &::nlm::Synapse::isInhibitory)
        .def("recordPreSpike", &::nlm::Synapse::recordPreSpike)
        .def("recordPostSpike", &::nlm::Synapse::recordPostSpike)
        .def("getPreSpikeHistory", &::nlm::Synapse::getPreSpikeHistory)
        .def("getPostSpikeHistory", &::nlm::Synapse::getPostSpikeHistory)
        .def("clearHistory", &::nlm::Synapse::clearHistory)
        .def("getPlasticityFlags", &::nlm::Synapse::getPlasticityFlags)
        .def("enablePlasticity", &::nlm::Synapse::enablePlasticity)
        .def("getEligibilityTrace", &::nlm::Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", &::nlm::Synapse::setEligibilityTrace)
        .def("decayEligibilityTrace", &::nlm::Synapse::decayEligibilityTrace)
        .def("getEfficacy", &::nlm::Synapse::getEfficacy)
        .def("setEfficacy", &::nlm::Synapse::setEfficacy)
        .def("step", &::nlm::Synapse::step)
        .def("reset", &::nlm::Synapse::reset)
        .def("initializeRandom", &::nlm::Synapse::initializeRandom)
        .def("getSourceNeuron", &::nlm::Synapse::getSourceNeuron)
        .def("getDestinationNeuron", &::nlm::Synapse::getDestinationNeuron);
    
    // NeuralRegion class
    py::class_<::nlm::NeuralRegion, std::shared_ptr<::nlm::NeuralRegion>>(m, "NeuralRegion")
        .def(py::init<::nlm::RegionId>())
        .def(py::init<::nlm::RegionId, const std::string&>())
        .def("getId", &::nlm::NeuralRegion::getId)
        .def("getName", &::nlm::NeuralRegion::getName)
        .def("setName", &::nlm::NeuralRegion::setName)
        .def("addPopulation", &::nlm::NeuralRegion::addPopulation)
        .def("getPopulation", &::nlm::NeuralRegion::getPopulation)
        .def("getPopulationCount", &::nlm::NeuralRegion::getPopulationCount)
        .def("getPopulations", &::nlm::NeuralRegion::getPopulations)
        .def("getAllPopulations", &::nlm::NeuralRegion::getAllPopulations)
        .def("addSynapse", &::nlm::NeuralRegion::addSynapse)
        .def("getSynapse", &::nlm::NeuralRegion::getSynapse)
        .def("getSynapseCount", &::nlm::NeuralRegion::getSynapseCount)
        .def("getSynapses", &::nlm::NeuralRegion::getSynapses)
        .def("getSynapsesFrom", &::nlm::NeuralRegion::getSynapsesFrom)
        .def("getSynapsesTo", &::nlm::NeuralRegion::getSynapsesTo)
        .def("getTotalNeuronCount", &::nlm::NeuralRegion::getTotalNeuronCount)
        .def("getActiveNeuronCount", &::nlm::NeuralRegion::getActiveNeuronCount)
        .def("getFiringNeuronCount", &::nlm::NeuralRegion::getFiringNeuronCount)
        .def("getAverageFiringRate", &::nlm::NeuralRegion::getAverageFiringRate)
        .def("getAverageSynapticWeight", &::nlm::NeuralRegion::getAverageSynapticWeight)
        .def("getSynapticDensity", &::nlm::NeuralRegion::getSynapticDensity)
        .def("step", &::nlm::NeuralRegion::step)
        .def("reset", &::nlm::NeuralRegion::reset)
        .def("initializeRandomConnectivity", &::nlm::NeuralRegion::initializeRandomConnectivity)
        .def("getAllNeurons", &::nlm::NeuralRegion::getAllNeurons)
        .def("getTotalNeuronCount", &::nlm::NeuralRegion::getTotalNeuronCount)
        .def("getSynapseCount", &::nlm::NeuralRegion::getSynapseCount);
    
    // Config class
    py::class_<::nlm::Config, std::shared_ptr<::nlm::Config>>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &::nlm::Config::loadFromFile)
        .def("loadFromArgs", &::nlm::Config::loadFromArgs)
        .def("saveToFile", &::nlm::Config::saveToFile)
        .def("get", &::nlm::Config::get<::nlm::ConfigValue>)
        .def("getOr", &::nlm::Config::getOr<::nlm::ConfigValue>)
        .def("set", py::overload_cast<const std::string&, const ::nlm::ConfigValue&, ::nlm::ConfigSource>(&::nlm::Config::set))
        .def("set", py::overload_cast<const std::string&, int, ::nlm::ConfigSource>(&::nlm::Config::set))
        .def("set", py::overload_cast<const std::string&, double, ::nlm::ConfigSource>(&::nlm::Config::set))
        .def("set", py::overload_cast<const std::string&, bool, ::nlm::ConfigSource>(&::nlm::Config::set))
        .def("set", py::overload_cast<const std::string&, const std::string&, ::nlm::ConfigSource>(&::nlm::Config::set))
        .def("has", &::nlm::Config::has)
        .def("remove", &::nlm::Config::remove)
        .def("getKeys", &::nlm::Config::getKeys)
        .def("clear", &::nlm::Config::clear)
        .def("summary", &::nlm::Config::summary);
    
    // Logger class
    py::class_<::nlm::Logger, std::shared_ptr<::nlm::Logger>>(m, "Logger")
        .def(py::init<>())
        .def("addLogger", &::nlm::Logger::addLogger)
        .def("clearLoggers", &::nlm::Logger::clearLoggers)
        .def("setLevel", &::nlm::Logger::setLevel)
        .def("log", py::overload_cast<::nlm::LogLevel, const std::string&, const std::string&, int, const std::string&>(&::nlm::Logger::log))
        .def("debug", &::nlm::Logger::debug)
        .def("info", &::nlm::Logger::info)
        .def("warning", &::nlm::Logger::warning)
        .def("error", &::nlm::Logger::error)
        .def("critical", &::nlm::Logger::critical)
        .def("isEnabled", &::nlm::Logger::isEnabled)
        .def("getGlobal", &::nlm::Logger::getGlobal, py::return_value_policy::reference)
        .def("setGlobal", &::nlm::Logger::setGlobal);
    
    // RandomGenerator class
    py::class_<::nlm::RandomGenerator, std::shared_ptr<::nlm::RandomGenerator>>(m, "RandomGenerator")
        .def(py::init<uint64_t>())
        .def("uniformReal", py::overload_cast<float, float>(&::nlm::RandomGenerator::uniformReal))
        .def("uniformInt", py::overload_cast<int, int>(&::nlm::RandomGenerator::uniformInt))
        .def("normal", py::overload_cast<float, float>(&::nlm::RandomGenerator::normal))
        .def("setSeed", &::nlm::RandomGenerator::setSeed)
        .def("getSeed", &::nlm::RandomGenerator::getSeed);
    
    // SimulationClock class
    py::class_<::nlm::SimulationClock, std::shared_ptr<::nlm::SimulationClock>>(m, "SimulationClock")
        .def(py::init<double>())
        .def("getTime", &::nlm::SimulationClock::getTime)
        .def("getTimestep", &::nlm::SimulationClock::getTimestep)
        .def("step", &::nlm::SimulationClock::step);
    
    // Helper functions
    m.def("createDefaultConfig", &::nlm::createDefaultConfig);
    
    // Factory registration for convenience
    m.def("registerFactory", [](const std::string& name, std::function<std::unique_ptr<::nlm::Neuron>(const ::nlm::NeuronId&, const std::map<std::string, std::string>&)> creator) {
        // This would register with the factory system
        return true;
    });
    
    // Error handling utilities
    m.def("getLastErrors", &::nlm::ErrorHandler::getLastErrors);
    m.def("clearLastErrors", &::nlm::ErrorHandler::clearLastErrors);
    m.def("hasErrors", &::nlm::ErrorHandler::hasErrors);
    
    // Version info
    m.def("getVersion", []() { return "2.0.0"; });
    m.def("getBuildInfo", []() { return "Phase 6: Final Integration"; });
}