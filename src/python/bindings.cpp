// NLM Python Bindings
// This file provides Python bindings for the NLM (Neural Learning Machine) C++ library
// Using pybind11 for seamless Python-C++ integration

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <string>
#include <memory>

// Include core NLM headers
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/environment/Observation.hpp"
#include "src/environment/Action.hpp"
#include "src/development/DevelopmentSystem.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/memory/WorkingMemory.hpp"
#include "src/plasticity/PlasticityRule.hpp"
#include "src/dynamics/SpikeSystem.hpp"

namespace py = pybind11;

// Pybind11 module definition
PYBIND11_MODULE(pynlm, m) {
    m.doc() = "Python bindings for NLM (Neural Learning Machine) - an experimental artificial brain simulator";
    
    // Version information
    m.attr("__version__") = "0.1.0";
    
    // ==============================================================================
    // Configuration System
    // ==============================================================================
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &nlm::Config::loadFromFile)
        .def("loadFromArgs", &nlm::Config::loadFromArgs)
        .def("saveToFile", &nlm::Config::saveToFile)
        .def("get", &nlm::Config::get<int>)
        .def("get", &nlm::Config::get<double>)
        .def("get", &nlm::Config::get<bool>)
        .def("get", &nlm::Config::get<std::string>)
        .def("getOr", &nlm::Config::getOr<int>)
        .def("getOr", &nlm::Config::getOr<double>)
        .def("getOr", &nlm::Config::getOr<bool>)
        .def("getOr", &nlm::Config::getOr<std::string>)
        .def("set", &nlm::Config::set)
        .def("has", &nlm::Config::has)
        .def("remove", &nlm::Config::remove)
        .def("getKeys", &nlm::Config::getKeys)
        .def("clear", &nlm::Config::clear)
        .def("summary", &nlm::Config::summary)
        .def_static("createDefaultConfig", []() {
            auto config = std::make_shared<nlm::Config>();
            // Set default values
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.1);
            config->set("plasticity.stdp.enable", true);
            return config;
        });
    
    // ==============================================================================
    // Neuron Types and State
    // ==============================================================================
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();
    
    py::enum_<nlm::FiringState>(m, "FiringState")
        .value("Resting", nlm::FiringState::Resting)
        .value("Active", nlm::FiringState::Active)
        .value("Refractory", nlm::FiringState::Refractory)
        .export_values();
    
    // NeuronState structure
    py::class_<nlm::NeuronState>(m, "NeuronState")
        .def(py::init<>())
        .def_readwrite("membranePotential", &nlm::NeuronState::membranePotential)
        .def_readwrite("threshold", &nlm::NeuronState::threshold)
        .def_readwrite("restingPotential", &nlm::NeuronState::restingPotential)
        .def_readwrite("resetPotential", &nlm::NeuronState::resetPotential)
        .def_readwrite("firingState", &nlm::NeuronState::firingState)
        .def_readwrite("refractoryRemaining", &nlm::NeuronState::refractoryRemaining)
        .def_readwrite("refractoryPeriod", &nlm::NeuronState::refractoryPeriod)
        .def_readwrite("lastSpikeTime", &nlm::NeuronState::lastSpikeTime)
        .def_readwrite("adaptationVariable", &nlm::NeuronState::adaptationVariable)
        .def_readwrite("firingRate", &nlm::NeuronState::firingRate)
        .def_readwrite("leakConductance", &nlm::NeuronState::leakConductance);
    
    // Neuron class
    py::class_<nlm::Neuron>(m, "Neuron")
        .def(py::init<nlm::NeuronId>())
        .def("setType", &nlm::Neuron::setType)
        .def("setMembranePotential", &nlm::Neuron::setMembranePotential)
        .def("addToMembranePotential", &nlm::Neuron::addToMembranePotential)
        .def("setThreshold", &nlm::Neuron::setThreshold)
        .def("setFiringState", &nlm::Neuron::setFiringState)
        .def("decrementRefractory", &nlm::Neuron::decrementRefractory)
        .def("setFiringRate", &nlm::Neuron::setFiringRate)
        .def("setLeakConductance", &nlm::Neuron::setLeakConductance)
        .def("setRefractoryPeriod", &nlm::Neuron::setRefractoryPeriod)
        .def("setRestingPotential", &nlm::Neuron::setRestingPotential)
        .def("setResetPotential", &nlm::Neuron::setResetPotential)
        .def("checkThreshold", &nlm::Neuron::checkThreshold)
        .def("getLastSpikeTime", &nlm::Neuron::getLastSpikeTime)
        .def("receiveExcitatoryInput", &nlm::Neuron::receiveExcitatoryInput)
        .def("receiveInhibitoryInput", &nlm::Neuron::receiveInhibitoryInput)
        .def("receiveModulatoryInput", &nlm::Neuron::receiveModulatoryInput)
        .def("injectCurrent", &nlm::Neuron::injectCurrent)
        .def("clearTotalCurrent", &nlm::Neuron::clearTotalCurrent)
        .def("recordSpike", &nlm::Neuron::recordSpike)
        .def("clearSpikeHistory", &nlm::Neuron::clearSpikeHistory)
        .def("addIncomingSynapse", &nlm::Neuron::addIncomingSynapse)
        .def("addOutgoingSynapse", &nlm::Neuron::addOutgoingSynapse)
        .def("getState", &nlm::Neuron::getState)
        .def("getPlasticityFlags", &nlm::Neuron::getPlasticityFlags)
        .def("enablePlasticity", &nlm::Neuron::enablePlasticity)
        .def("setRegionId", &nlm::Neuron::setRegionId)
        .def("setPopulationId", &nlm::Neuron::setPopulationId)
        .def("stepLIF", &nlm::Neuron::stepLIF)
        .def("step", &nlm::Neuron::step)
        .def("reset", &nlm::Neuron::reset)
        .def("initializeRandom", &nlm::Neuron::initializeRandom)
        .def("getId", &nlm::Neuron::getId)
        .def("getType", &nlm::Neuron::getType)
        .def("getMembranePotential", &nlm::Neuron::getMembranePotential)
        .def("getThreshold", &nlm::Neuron::getThreshold)
        .def("getFiringState", &nlm::Neuron::getFiringState)
        .def("getRefractoryPeriod", &nlm::Neuron::getRefractoryPeriod)
        .def("getRestingPotential", &nlm::Neuron::getRestingPotential)
        .def("getResetPotential", &nlm::Neuron::getResetPotential)
        .def("getLeakConductance", &nlm::Neuron::getLeakConductance)
        .def("isFiring", &nlm::Neuron::isFiring);
    
    // Synapse class
    py::class_<nlm::Synapse>(m, "Synapse")
        .def(py::init<>())
        .def("getSourceNeuron", &nlm::Synapse::getSourceNeuron)
        .def("getDestinationNeuron", &nlm::Synapse::getDestinationNeuron)
        .def("getWeight", &nlm::Synapse::getWeight)
        .def("setWeight", &nlm::Synapse::setWeight)
        .def("getDelay", &nlm::Synapse::getDelay)
        .def("getType", &nlm::Synapse::getType)
        .def("getEligibilityTrace", &nlm::Synapse::getEligibilityTrace)
        .def("enablePlasticity", &nlm::Synapse::enablePlasticity)
        .def("getPlasticityFlags", &nlm::Synapse::getPlasticityFlags)
        .def("recordPreSpike", &nlm::Synapse::recordPreSpike)
        .def("recordPostSpike", &nlm::Synapse::recordPostSpike)
        .def("step", &nlm::Synapse::step)
        .def("getPreSpikeHistory", &nlm::Synapse::getPreSpikeHistory)
        .def("getPostSpikeHistory", &nlm::Synapse::getPostSpikeHistory);
    
    // ==============================================================================
    // Spike System
    // ==============================================================================
    py::class_<nlm::SpikeEvent>(m, "SpikeEvent")
        .def(py::init<>())
        .def_readwrite("neuronId", &nlm::SpikeEvent::neuronId)
        .def_readwrite("timestamp", &nlm::SpikeEvent::timestamp)
        .def_readwrite("step", &nlm::SpikeEvent::step);
    
    py::class_<nlm::DelayedSpikeEvent>(m, "DelayedSpikeEvent")
        .def(py::init<>())
        .def_readwrite("sourceNeuron", &nlm::DelayedSpikeEvent::sourceNeuron)
        .def_readwrite("destinationNeuron", &nlm::DelayedSpikeEvent::destinationNeuron)
        .def_readwrite("synapseId", &nlm::DelayedSpikeEvent::synapseId)
        .def_readwrite("weight", &nlm::DelayedSpikeEvent::weight)
        .def_readwrite("isExcitatory", &nlm::DelayedSpikeEvent::isExcitatory)
        .def_readwrite("sourceTime", &nlm::DelayedSpikeEvent::sourceTime)
        .def_readwrite("deliveryTime", &nlm::DelayedSpikeEvent::deliveryTime)
        .def_readwrite("sourceStep", &nlm::DelayedSpikeEvent::sourceStep)
        .def_readwrite("deliveryStep", &nlm::DelayedSpikeEvent::deliveryStep);
    
    py::class_<nlm::SpikeSystem>(m, "SpikeSystem")
        .def(py::init<>())
        .def("registerHandler", &nlm::SpikeSystem::registerHandler)
        .def("registerDelayedHandler", &nlm::SpikeSystem::registerDelayedHandler)
        .def("queueSpike", &nlm::SpikeSystem::queueSpike)
        .def("queueDelayedSpike", &nlm::SpikeSystem::queueDelayedSpike)
        .def("processSpikes", &nlm::SpikeSystem::processSpikes)
        .def("processDelayedSpikes", &nlm::SpikeSystem::processDelayedSpikes)
        .def("getPendingSpikeCount", &nlm::SpikeSystem::getPendingSpikeCount)
        .def("getPendingDelayedCount", &nlm::SpikeSystem::getPendingDelayedCount)
        .def("reset", &nlm::SpikeSystem::reset);
    
    // ==============================================================================
    // Plasticity Systems
    // ==============================================================================
    py::class_<nlm::PlasticityFlags>(m, "PlasticityFlags")
        .def(py::init<>())
        .def_readwrite("stdp", &nlm::PlasticityFlags::stdp)
        .def_readwrite("hebbian", &nlm::PlasticityFlags::hebbian)
        .def_readwrite("rewardModulated", &nlm::PlasticityFlags::reward_modulated);
    
    // STDP class
    py::class_<nlm::STDP>(m, "STDP")
        .def(py::init<>())
        .def("configure", &nlm::STDP::configure)
        .def("update", &nlm::STDP::update)
        .def("setLTPWeight", &nlm::STDP::setLTPWeight)
        .def("setLTDWeight", &nlm::STDP::setLTDWeight);
    
    // Hebbian class
    py::class_<nlm::Hebbian>(m, "Hebbian")
        .def(py::init<>())
        .def("update", &nlm::Hebbian::update);
    
    // StructuralPlasticity class
    py::class_<nlm::StructuralPlasticity>(m, "StructuralPlasticity")
        .def(py::init<>())
        .def("update", &nlm::StructuralPlasticity::update)
        .def("setSynaptogenesisRate", &nlm::StructuralPlasticity::setSynaptogenesisRate)
        .def("setPruningRate", &nlm::StructuralPlasticity::setPruningRate);
    
    // ==============================================================================
    // Brain Class
    // ==============================================================================
    py::class_<nlm::Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>())
        .def("initialize", &nlm::Brain::initialize)
        .def("step", py::overload_cast<nlm::SimulationStep>(&nlm::Brain::step))
        .def("step", py::overload_cast<nlm::SimulationStep, nlm::Timestamp>(&nlm::Brain::step))
        .def("reset", &nlm::Brain::reset)
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput)
        .def("injectCurrent", &nlm::Brain::injectCurrent)
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons)
        .def("getSpikeSystem", &nlm::Brain::getSpikeSystem)
        .def("getSTDP", &nlm::Brain::getSTDP)
        .def("getHebbian", &nlm::Brain::getHebbian)
        .def("getStructuralPlasticity", &nlm::Brain::getStructuralPlasticity)
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount)
        .def("getPendingSpikeEventCount", &nlm::Brain::getPendingSpikeEventCount)
        .def("produceAction", &nlm::Brain::produceAction)
        .def("save", &nlm::Brain::save)
        .def("load", &nlm::Brain::load)
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount)
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate)
        .def("addRegion", &nlm::Brain::addRegion)
        .def("getRegion", &nlm::Brain::getRegion)
        .def("getRegionCount", &nlm::Brain::getRegionCount)
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
        .def("getPredictionErrorSignal", &nlm::Brain::getPredictionErrorSignal)
        .def("getRandomGenerator", &nlm::Brain::getRandomGenerator)
        .def("logStatus", &nlm::Brain::logStatus)
        .def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage)
        .def("setDevelopmentalStage", &nlm::Brain::setDevelopmentalStage);
    
    // ==============================================================================
    // Agent System
    // ==============================================================================
    py::class_<nlm::AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>())
        .def("initialize", &nlm::AgentBrain::initialize)
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation)
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment)
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage)
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment)
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity)
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError)
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel)
        .def("getSensoryPercept", &nlm::AgentBrain::getSensoryPercept);
    
    // ==============================================================================
    // World System
    // ==============================================================================
    py::class_<nlm::Observation>(m, "Observation")
        .def(py::init<>())
        .def("getVision", &nlm::Observation::getVision)
        .def("getAudio", &nlm::Observation::getAudio)
        .def("getInternal", &nlm::Observation::getInternal)
        .def("getTimestamp", &nlm::Observation::getTimestamp);
    
    py::class_<nlm::SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure)
        .def("reset", &nlm::SimpleWorld::reset)
        .def("update", &nlm::SimpleWorld::update)
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept)
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody)
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart)
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand)
        .def("addObject", &nlm::SimpleWorld::addObject)
        .def("removeObject", &nlm::SimpleWorld::removeObject)
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition)
        .def("getWidth", &nlm::SimpleWorld::getWidth)
        .def("getHeight", &nlm::SimpleWorld::getHeight)
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed)
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy);
    
    // ==============================================================================
    // Action and Motor System
    // ==============================================================================
    py::enum_<nlm::ActionType>(m, "ActionType")
        .value("Wait", nlm::ActionType::Wait)
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Interact", nlm::ActionType::Interact)
        .value("Eat", nlm::ActionType::Eat)
        .value("Sleep", nlm::ActionType::Sleep)
        .export_values();
    
    py::class_<nlm::Action>(m, "Action")
        .def(py::init<nlm::ActionType>())
        .def("getType", &nlm::Action::getType)
        .def("getValue", &nlm::Action::getValue)
        .def("getTimestamp", &nlm::Action::getTimestamp)
        .def("setType", &nlm::Action::setType)
        .def("setValue", &nlm::Action::setValue);
    
    // ==============================================================================
    // Neuromodulation Systems
    // ==============================================================================
    py::class_<nlm::Dopamine>(m, "Dopamine")
        .def(py::init<>())
        .def("update", &nlm::Dopamine::update)
        .def("getLevel", &nlm::Dopamine::getLevel)
        .def("getPlasticityFactor", &nlm::Dopamine::getPlasticityFactor);
    
    py::class_<nlm::Curiosity>(m, "Curiosity")
        .def(py::init<>())
        .def("update", &nlm::Curiosity::update)
        .def("getLevel", &nlm::Curiosity::getLevel)
        .nL(const std::string& key, const std::string& value) = nlm::ConfigSource::Runtime);

    // ==============================================================================
    // Utility Functions
    // ==============================================================================
    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) {
        return std::make_shared<nlm::Brain>(config);
    });
    
    m.def("createDefaultConfig", []() {
        return std::make_shared<nlm::Config>();
    });
    
    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) {
        return std::make_shared<nlm::AgentBrain>(brain);
    });
    
    m.def("createSimpleWorld", []() {
        return std::make_shared<nlm::SimpleWorld>();
    });
    
    // Convenience functions
    m.def("run_simulation", [](std::shared_ptr<nlm::Brain> brain, std::shared_ptr<nlm::SimpleWorld> world, std::shared_ptr<nlm::AgentBrain> agent, int num_steps) {
        for (int i = 0; i < num_steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    });
    
    // ==============================================================================
    // Example Functions
    // ==============================================================================
    m.def("minimal_example", []() {
        auto config = std::make_shared<nlm::Config>();
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        for (int i = 0; i < 10; ++i) {
            brain->step(i);
        }
        return brain->getTotalSpikeCount();
    });
    
    m.def("environment_interaction_example", []() {
        auto config = std::make_shared<nlm::Config>();
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(10, 10, 8, 8);
        world->reset();
        agent->initialize(world);
        
        for (int i = 0; i < 50; ++i) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(i);
            world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
        }
        return brain->getAverageFiringRate();
    });
    
    // Create Python output function for easy debugging
    py::register_ostream_decorator<nlm::Config>();
    py::register_ostream_decorator<nlm::Brain>();
    py::register_ostream_decorator<nlm::SimpleWorld>();
}