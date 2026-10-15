#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/RandomGenerator.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "agent/AgentBody.hpp"
#include "world/SimpleWorld.hpp"
#include "environment/Environment.hpp"
#include "environment/Observation.hpp"
#include "environment/Action.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/MotorSystem.hpp"
#include "motor/Action.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/SemanticMemory.hpp"
#include "memory/ProceduralMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "prediction/PredictionSystem.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "development/DevelopmentSystem.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "dynamics/NeuralDynamics.hpp"
#include "visualization/VisualizationInterface.hpp"
#include "experiments/Experiment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Metrics.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace py = pybind11;
using namespace nlm;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "NLM (熙然) - Neural Learning Machine Python Bindings";
    m.attr("__version__") = "0.1.0";
    
    // Basic types
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Interneuron", NeuronType::Interneuron)
        .export_values();
    
    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .export_values();
    
    py::enum_<PlasticityFlags>(m, "PlasticityFlags")
        .value("Hebbian", PlasticityFlags::Hebbian)
        .value("STDP", PlasticityFlags::STDP)
        .value("Structural", PlasticityFlags::Structural)
        .export_values();
    
    // Core classes
    py::class_<Config, std::shared_ptr<Config>>(m, "Config")
        .def(py::init<>())
        .def("set", &Config::set)
        .def("get", &Config::get)
        .def("getOr", &Config::getOr)
        .def("loadFromFile", &Config::loadFromFile)
        .def("loadFromArgs", &Config::loadFromArgs);
    
    py::class_<Logger, std::shared_ptr<Logger>>(m, "Logger")
        .def("addLogger", &Logger::addLogger)
        .def("setLevel", &Logger::setLevel)
        .def("getLevel", &Logger::getLevel);
    
    py::class_<Logger::getInstance()>(m, "GlobalLogger")
        .def("setLevel", &Logger::getInstance().setLevel)
        .def("getLevel", &Logger::getInstance().getLevel);
    
    py::class_<SimulationClock, std::shared_ptr<SimulationClock>>(m, "SimulationClock")
        .def(py::init<double>())
        .def("step", &SimulationClock::step)
        .def("getTime", &SimulationClock::getTime)
        .def("getStep", &SimulationClock::getStep);
    
    py::class_<RandomGenerator, std::shared_ptr<RandomGenerator>>(m, "RandomGenerator")
        .def("uniformReal", &RandomGenerator::uniformReal)
        .def("uniformInt", &RandomGenerator::uniformInt)
        .def("normal", &RandomGenerator::normal);
    
    // ID types
    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<uint64_t>())
        .def("get", &NeuronId::get)
        .def("toString", &NeuronId::toString);
    
    py::class_<RegionId>(m, "RegionId")
        .def(py::init<uint64_t>())
        .def("get", &RegionId::get)
        .def("toString", &RegionId::toString);
    
    py::class_<SimulationStep>(m, "SimulationStep")
        .def(py::init<uint64_t>())
        .def("get", &SimulationStep::get);
    
    // State and value types
    py::class_<NeuronState>(m, "NeuronState")
        .def_readonly("membranePotential", &NeuronState::membranePotential)
        .def_readonly("restingPotential", &NeuronState::restingPotential)
        .def_readonly("spikeCount", &NeuronState::spikeCount)
        .def_readonly("isSpiking", &NeuronState::isSpiking);
    
    // Brain components
    py::class_<Neuron, std::shared_ptr<Neuron>>(m, "Neuron")
        .def(py::init<>())
        .def("injectCurrent", &Neuron::injectCurrent)
        .def("getMembranePotential", &Neuron::getMembranePotential)
        .def("getThreshold", &Neuron::getThreshold)
        .def("getNeuronType", &Neuron::getNeuronType)
        .def("getState", &Neuron::getState)
        .def("step", &Neuron::step)
        .def("spike", &Neuron::spike);
    
    py::class_<Synapse, std::shared_ptr<Synapse>>(m, "Synapse")
        .def(py::init<>())
        .def("getWeight", &Synapse::getWeight)
        .def("addToWeight", &Synapse::addToWeight)
        .def("getPreNeuron", &Synapse::getPreNeuron)
        .def("getPostNeuron", &Synapse::getPostNeuron)
        .def("enablePlasticity", &Synapse::enablePlasticity)
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace)
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace);
    
    py::class_<NeuralRegion, std::shared_ptr<NeuralRegion>>(m, "NeuralRegion")
        .def("getRegionId", &NeuralRegion::getRegionId)
        .def("getPopulations", &NeuralRegion::getPopulations)
        .def("getAllNeurons", &NeuralRegion::getAllNeurons)
        .def("getSynapses", &NeuralRegion::getSynapses)
        .def("getNeuronCount", &NeuralRegion::getNeuronCount)
        .def("getSynapseCount", &NeuralRegion::getSynapseCount);
    
    py::class_<NeuralPopulation, std::shared_ptr<NeuralPopulation>>(m, "NeuralPopulation")
        .def("getNeuronType", &NeuralPopulation::getNeuronType)
        .def("getNeurons", &NeuralPopulation::getNeurons)
        .def("getNeuronCount", &NeuralPopulation::getNeuronCount);
    
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>())
        .def("initialize", &Brain::initialize)
        .def("step", &Brain::step)
        .def("step", &Brain::step)
        .def("receiveSensoryInput", &Brain::receiveSensoryInput)
        .def("injectCurrent", &Brain::injectCurrent)
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons)
        .def("getSpikeSystem", &Brain::getSpikeSystem)
        .def("getSTDP", &Brain::getSTDP)
        .def("getHebbian", &Brain::getHebbian)
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity)
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount)
        .def("produceAction", &Brain::produceAction)
        .def("applyNeuromodulation", &Brain::applyNeuromodulation)
        .def("updatePlasticity", &Brain::updatePlasticity)
        .def("develop", &Brain::develop)
        .def("reset", &Brain::reset)
        .def("save", &Brain::save)
        .def("load", &Brain::load)
        .def("addRegion", &Brain::addRegion)
        .def("getRegion", &Brain::getRegion)
        .def("getRegionCount", &Brain::getRegionCount)
        .def("getRegionIds", &Brain::getRegionIds)
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate)
        .def("getWorkingMemory", &Brain::getWorkingMemory)
        .def("getEpisodicMemory", &Brain::getEpisodicMemory)
        .def("getAssociativeMemory", &Brain::getAssociativeMemory)
        .def("getPredictionSystem", &Brain::getPredictionSystem)
        .def("getPlanner", &Brain::getPlanner)
        .def("getConceptFormation", &Brain::getConceptFormation)
        .def("getAttention", &Brain::getAttention)
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem)
        .def("getDopamine", &Brain::getDopamine)
        .def("getCuriosity", &Brain::getCuriosity)
        .def("getNovelty", &Brain::getNovelty)
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal)
        .def("getConfig", &Brain::getConfig)
        .def("getRandomGenerator", &Brain::getRandomGenerator)
        .def("logStatus", &Brain::logStatus);
    
    // Agent components
    py::class_<SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("getInternal", &SensoryPercept::getInternal)
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setVision", &SensoryPercept::setVision)
        .def("setTouch", &SensoryPercept::setTouch)
        .def("setInternal", &SensoryPercept::setInternal)
        .def("setProprioception", &SensoryPercept::setProprioception)
        .def("setAudio", &SensoryPercept::setAudio);
    
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("initialize", &AgentBrain::initialize)
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation)
        .def("updateDevelopment", &AgentBrain::updateDevelopment)
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("reset", &AgentBrain::reset)
        .def("getBrain", &AgentBrain::getBrain)
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &AgentBrain::enableDevelopment)
        .def("enableCuriosity", &AgentBrain::enableCuriosity)
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);
    
    // World components
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld")
        .def(py::init<size_t, size_t>())
        .def("getVisionWidth", &SimpleWorld::getVisionWidth)
        .def("getVisionHeight", &SimpleWorld::getVisionHeight)
        .def("getCell", &SimpleWorld::getCell)
        .def("setCell", &SimpleWorld::setCell)
        .def("getAgent", &SimpleWorld::getAgent)
        .def("setAgent", &SimpleWorld::setAgent)
        .def("step", &SimpleWorld::step)
        .def("getStep", &SimpleWorld::getStep);
    
    py::class_<AgentBody>(m, "AgentBody")
        .def(py::init<>())
        .def("reset", &AgentBody::reset)
        .def_readonly("x", &AgentBody::x)
        .def_readonly("y", &AgentBody::y)
        .def_readonly("orientation", &AgentBody::orientation)
        .def_readonly("energy", &AgentBody::energy)
        .def_readonly("health", &AgentBody::health);
    
    // Environment components
    py::class_<Environment, std::shared_ptr<Environment>>(m, "Environment")
        .def(py::init<>())
        .def("reset", &Environment::reset)
        .def("step", &Environment::step)
        .def("getObservation", &Environment::getObservation)
        .def("getAction", &Environment::getAction)
        .def("applyAction", &Environment::applyAction)
        .def("getReward", &Environment::getReward)
        .def("isDone", &Environment::isDone);
    
    py::class_<Observation, std::shared_ptr<Observation>>(m, "Observation")
        .def(py::init<>())
        .def("getSensoryInput", &Observation::getSensoryInput)
        .def("getActionResult", &Observation::getActionResult)
        .def("getReward", &Observation::getReward)
        .def("isSuccess", &Observation::isSuccess);
    
    // Sensory components
    py::class_<Vision, std::shared_ptr<Vision>>(m, "Vision")
        .def(py::init<>())
        .def("getData", &Vision::getData)
        .def("setData", &Vision::setData)
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("get", &Vision::get)
        .def("set", &Vision::set);
    
    py::class_<Audio, std::shared_ptr<Audio>>(m, "Audio")
        .def(py::init<>())
        .def("getData", &Audio::getData)
        .def("setData", &Audio::setData);
    
    py::class_<InternalSignals, std::shared_ptr<InternalSignals>>(m, "InternalSignals")
        .def(py::init<>())
        .def("getData", &InternalSignals::getData)
        .def("setData", &InternalSignals::setData);
    
    py::class_<SensoryInput, std::shared_ptr<SensoryInput>>(m, "SensoryInput")
        .def(py::init<>())
        .def("addSensoryData", &SensoryInput::addSensoryData)
        .def("getVision", &SensoryInput::getVision)
        .def("getAudio", &SensoryInput::getAudio)
        .def("getInternal", &SensoryInput::getInternal)
        .def("getProprioception", &SensoryInput::getProprioception);
    
    // Motor components
    py::class_<MotorSystem, std::shared_ptr<MotorSystem>>(m, "MotorSystem")
        .def(py::init<>())
        .def("decodeMotorCommand", &MotorSystem::decodeMotorCommand)
        .def("getMotorOutput", &MotorSystem::getMotorOutput)
        .def("setMotorOutput", &MotorSystem::setMotorOutput);
    
    py::class_<Action, std::shared_ptr<Action>>(m, "Action")
        .def(py::init<>())
        .def("getType", &Action::getType)
        .def("getParameters", &Action::getParameters)
        .def("setType", &Action::setType)
        .def("setParameters", &Action::setParameters);
    
    // Neuromodulation components
    py::class_<Neuromodulator, std::shared_ptr<Neuromodulator>>(m, "Neuromodulator")
        .def(py::init<>())
        .def("getDopamine", &Neuromodulator::getDopamine)
        .def("getCuriosity", &Neuromodulator::getCuriosity)
        .def("getNovelty", &Neuromodulator::getNovelty)
        .def("getPredictionError", &Neuromodulator::getPredictionError)
        .def("update", &Neuromodulator::update);
    
    py::class_<Dopamine, std::shared_ptr<Dopamine>>(m, "Dopamine")
        .def(py::init<>())
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", &Dopamine::setLevel)
        .def("getModulation", &Dopamine::getModulation);
    
    py::class_<Curiosity, std::shared_ptr<Curiosity>>(m, "Curiosity")
        .def(py::init<>())
        .def("getLevel", &Curiosity::getLevel)
        .def("setLevel", &Curiosity::setLevel)
        .def("getDrive", &Curiosity::getDrive);
    
    py::class_<Novelty, std::shared_ptr<Novelty>>(m, "Novelty")
        .def(py::init<>())
        .def("getLevel", &Novelty::getLevel)
        .def("setLevel", &Novelty::setLevel)
        .def("getDetection", &Novelty::getDetection);
    
    py::class_<PredictionError, std::shared_ptr<PredictionError>>(m, "PredictionError")
        .def(py::init<>())
        .def("getValue", &PredictionError::getValue)
        .def("setValue", &PredictionError::setValue)
        .def("getErrorSignal", &PredictionError::getErrorSignal);
    
    // Memory components
    py::class_<WorkingMemory, std::shared_ptr<WorkingMemory>>(m, "WorkingMemory")
        .def(py::init<>())
        .def("store", &WorkingMemory::store)
        .def("recall", &WorkingMemory::recall)
        .def("getActiveItems", &WorkingMemory::getActiveItems)
        .def("clear", &WorkingMemory::clear);
    
    py::class_<EpisodicMemory, std::shared_ptr<EpisodicMemory>>(m, "EpisodicMemory")
        .def(py::init<>())
        .def("storeExperience", &EpisodicMemory::storeExperience)
        .def("recallExperience", &EpisodicMemory::recallExperience)
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount)
        .def("replay", &EpisodicMemory::replay);
    
    py::class_<SemanticMemory, std::shared_ptr<SemanticMemory>>(m, "SemanticMemory")
        .def(py::init<>())
        .def("storeConcept", &SemanticMemory::storeConcept)
        .def("recallConcept", &SemanticMemory::recallConcept)
        .def("getConceptCount", &SemanticMemory::getConceptCount);
    
    py::class_<ProceduralMemory, std::shared_ptr<ProceduralMemory>>(m, "ProceduralMemory")
        .def(py::init<>())
        .def("storeAction", &ProceduralMemory::storeAction)
        .def("recallAction", &ProceduralMemory::recallAction)
        .def("getActionCount", &ProceduralMemory::getActionCount);
    
    py::class_<AssociativeMemory, std::shared_ptr<AssociativeMemory>>(m, "AssociativeMemory")
        .def(py::init<>())
        .def("storePattern", &AssociativeMemory::storePattern)
        .def("recallPattern", &AssociativeMemory::recallPattern)
        .def("getPatternCount", &AssociativeMemory::getPatternCount);
    
    // Cognition components
    py::class_<NeuralPlanner, std::shared_ptr<NeuralPlanner>>(m, "NeuralPlanner")
        .def(py::init<>())
        .def("planAction", &NeuralPlanner::planAction)
        .def("evaluateAction", &NeuralPlanner::evaluateAction)
        .def("getPlan", &NeuralPlanner::getPlan);
    
    py::class_<ConceptFormation, std::shared_ptr<ConceptFormation>>(m, "ConceptFormation")
        .def(py::init<>())
        .def("extractConcept", &ConceptFormation::extractConcept)
        .def("getConcepts", &ConceptFormation::getConcepts)
        .def("getConceptSimilarity", &ConceptFormation::getConceptSimilarity);
    
    py::class_<AttentionalSelection, std::shared_ptr<AttentionalSelection>>(m, "AttentionalSelection")
        .def(py::init<>())
        .def("selectFocus", &AttentionalSelection::selectFocus)
        .def("getFocus", &AttentionalSelection::getFocus)
        .def("updatePriority", &AttentionalSelection::updatePriority);
    
    // Prediction components
    py::class_<PredictionSystem, std::shared_ptr<PredictonSystem>>(m, "PredictionSystem")
        .def(py::init<>())
        .def("predict", &PredictionSystem::predict)
        .def("computeError", &PredictionSystem::computeError)
        .def("update", &PredictionSystem::update)
        .def("getPrediction", &PredictionSystem::getPrediction)
        .def("getError", &PredictionSystem::getError);
    
    py::class_<NeuralPrediction, std::shared_ptr<NeuralPrediction>>(m, "NeuralPrediction")
        .def(py::init<>())
        .def("getPrediction", &NeuralPrediction::getPrediction)
        .def("setPrediction", &NeuralPrediction::setPrediction)
        .def("getConfidence", &NeuralPrediction::getConfidence)
        .def("setConfidence", &NeuralPrediction::setConfidence);
    
    // Development components
    py::class_<DevelopmentSystem, std::shared_ptr<DevelopmentSystem>>(m, "DevelopmentSystem")
        .def(py::init<>())
        .def("update", &DevelopmentSystem::update)
        .def("getStage", &DevelopmentSystem::getStage)
        .def("getAge", &DevelopmentSystem::getAge)
        .def("setStage", &DevelopmentSystem::setStage);
    
    // Plasticity components
    py::class_<PlasticityRule, std::shared_ptr<PlasticityRule>>(m, "PlasticityRule")
        .def(py::init<>())
        .def("apply", &PlasticityRule::apply)
        .def("getLTPWeight", &PlasticityRule::getLTPWeight)
        .def("getLTDWeight", &PlasticityRule::getLTDWeight)
        .def("setLTPWeight", &PlasticityRule::setLTPWeight)
        .def("setLTDWeight", &PlasticityRule::setLTDWeight);
    
    py::class_<STDP, std::shared_ptr<STDP>>(m, "STDP")
        .def(py::init<>())
        .def("applySTDP", &STDP::applySTDP)
        .def("getLTPWeight", &STDP::getLTPWeight)
        .def("getLTDWeight", &STDP::getLTDWeight)
        .def("setLTPWeight", &STDP::setLTPWeight)
        .def("setLTDWeight", &STDP::setLTDWeight)
        .def("setTau", &STDP::setTau);
    
    py::class_<Hebbian, std::shared_ptr<Hebbian>>(m, "Hebbian")
        .def(py::init<>())
        .def("applyHebbian", &Hebbian::applyHebbian)
        .def("getLearningRate", &Hebbian::getLearningRate)
        .def("setLearningRate", &Hebbian::setLearningRate);
    
    py::class_<StructuralPlasticity, std::shared_ptr<StructuralPlasticity>>(m, "StructuralPlasticity")
        .def(py::init<>())
        .def("applyStructuralPlasticity", &StructuralPlasticity::applyStructuralPlasticity)
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate)
        .def("getPruningRate", &StructuralPlasticity::getPruningRate)
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate)
        .def("setPruningRate", &StructuralPlasticity::setPruningRate);
    
    // Dynamics components
    py::class_<SpikeSystem, std::shared_ptr<SpikeSystem>>(m, "SpikeSystem")
        .def(py::init<>())
        .def("processSpikes", &SpikeSystem::processSpikes)
        .def("getTotalSpikeCount", &SpikeSystem::getTotalSpikeCount)
        .def("getActiveNeuronCount", &SpikeSystem::getActiveNeuronCount)
        .def("getMostActiveNeurons", &SpikeSystem::getMostActiveNeurons)
        .def("reset", &SpikeSystem::reset);
    
    py::class_<NeuralDynamics, std::shared_ptr<NeuralDynamics>>(m, "NeuralDynamics")
        .def(py::init<>())
        .def("update", &NeuralDynamics::update)
        .def("getFiringRate", &NeuralDynamics::getFiringRate)
        .def("getExcitatoryInhibitionRatio", &NeuralDynamics::getExcitatoryInhibitionRatio);
    
    // Visualization components
    py::class_<VisualizationInterface, std::shared_ptr<VisualizationInterface>>(m, "VisualizationInterface")
        .def(py::init<>())
        .def("plotNeuralActivity", &VisualizationInterface::plotNeuralActivity)
        .def("plotLearningCurve", &VisualizationInterface::plotLearningCurve)
        .def("savePlot", &VisualizationInterface::savePlot)
        .def("showPlot", &VisualizationInterface::showPlot);
    
    // Experiment components
    py::class_<Experiment, std::shared_ptr<Experiment>>(m, "Experiment")
        .def(py::init<>())
        .def("run", &Experiment::run)
        .def("getResults", &Experiment::getResults)
        .def("reset", &Experiment::reset);
    
    py::class_<ExperimentRunner, std::shared_ptr<ExperimentRunner>>(m, "ExperimentRunner")
        .def(py::init<>())
        .def("runExperiment", &ExperimentRunner::runExperiment)
        .def("addExperiment", &ExperimentRunner::addExperiment)
        .def("getResults", &ExperimentRunner::getResults)
        .def("clear", &ExperimentRunner::clear);
    
    py::class_<Phase3Experiment, std::shared_ptr<Phase3Experiment>>(m, "Phase3Experiment")
        .def(py::init<>())
        .def("run", &Phase3Experiment::run)
        .def("getResults", &Phase3Experiment::getResults);
    
    py::class_<Phase4Experiment, std::shared_ptr<Phase4Experiment>>(m, "Phase4Experiment")
        .def(py::init<>())
        .def("run", &Phase4Experiment::run)
        .def("getResults", &Phase4Experiment::getResults);
    
    py::class_<Phase6IntegratedExperiment, std::shared_ptr<Phase6IntegratedExperiment>>(m, "Phase6IntegratedExperiment")
        .def(py::init<>())
        .def("verifyIntegration", &Phase6IntegratedExperiment::verifyIntegration)
        .def("testMemoryIntegration", &Phase6IntegratedExperiment::testMemoryIntegration)
        .def("testNeuromodulationIntegration", &Phase6IntegratedExperiment::testNeuromodulationIntegration)
        .def("testCheckpointing", &Phase6IntegratedExperiment::testCheckpointing)
        .def("testReplay", &Phase6IntegratedExperiment::testReplay)
        .def("run", &Phase6IntegratedExperiment::run)
        .def("getResults", &Phase6IntegratedExperiment::getResults);
    
    // Convenience functions
    m.def("create_brain", [](size_t neuronCount, double timestep, uint64_t seed) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("simulation_timestep", timestep);
        config->set("random_seed", static_cast<int64_t>(seed));
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            return brain;
        } else {
            throw std::runtime_error("Failed to initialize brain");
        }
    }, "Create a new brain with specified parameters", 
        py::arg("neuronCount") = 500,
        py::arg("timestep") = 0.001,
        py::arg("seed") = 42);
    
    m.def("create_agent", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, "Create an agent brain with the specified brain", py::arg("brain"));
    
    m.def("create_simple_world", [](size_t width, size_t height) {
        return std::make_shared<SimpleWorld>(width, height);
    }, "Create a simple world with specified dimensions",
        py::arg("width") = 20,
        py::arg("height") = 20);
    
    // Utility functions
    m.def("version", []() {
        return "NLM 0.1.0";
    });
    
    m.def("get_build_info", []() {
        return std::map<std::string, std::string>{
            {"compiler", "C++20"},
            {"python_bindings", "pybind11"},
            {"version", "0.1.0"},
            {"status", "Production Ready"}
        };
    });
}