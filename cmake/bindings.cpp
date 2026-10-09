// NLM Python Bindings (Pybind11 Module)
// Provides Python bindings for the NLM (Neuromodular Learning Machine) framework

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>

// NLM core includes
#include "NLM/Types/Types.h"
#include "NLM/Core/Config/Config.h"
#include "NLM/Core/Random/Random.h"
#include "NLM/Core/Logger/Logger.h"
#include "NLM/Core/SimulationClock/SimulationClock.h"

// Neural computation includes
#include "NLM/Brain/Neuron.h"
#include "NLM/Brain/Synapse.h"
#include "NLM/Brain/NeuralPopulation.h"
#include "NLM/Brain/NeuralRegion.h"
#include "NLM/Brain/Brain.h"

// Neuromodulation includes
#include "NLM/Neuromodulation/Neuromodulator.h"
#include "NLM/Neuromodulation/Reward.h"
#include "NLM/Neuromodulation/PredictionError.h"
#include "NLM/Neuromodulation/Novelty.h"
#include "NLM/Neuromodulation/Curiosity.h"

// Memory includes
#include "NLM/Memory/WorkingMemory.h"
#include "NLM/Memory/EpisodicMemory.h"
#include "NLM/Memory/SemanticMemory.h"
#include "NLM/Memory/ProceduralMemory.h"
#include "NLM/Memory/AssociativeMemory.h"
#include "NLM/Memory/NeuralWorkingMemory.h"
#include "NLM/Memory/NeuralEpisodicMemory.h"

// Cognition includes
#include "NLM/Cognition/ConceptFormation.h"
#include "NLM/Cognition/NeuralPlanner.h"

// Sensory includes
#include "NLM/Sensory/SensoryInput.h"
#include "NLM/Sensory/Vision.h"
#include "NLM/Sensory/Audio.h"
#include "NLM/Sensory/InternalSignals.h"

// Prediction includes
#include "NLM/Prediction/PredictionSystem.h"
#include "NLM/Prediction/NeuralPrediction.h"

// Motor includes
#include "NLM/Motor/MotorSystem.h"
#include "NLM/Motor/Action.h"

// Environment includes
#include "NLM/Environment/Environment.h"
#include "NLM/Environment/Observation.h"
#include "NLM/Environment/Action.h"

// Experiments includes
#include "NLM/Experiments/Experiment.h"
#include "NLM/Experiments/ExperimentRunner.h"
#include "NLM/Experiments/Metrics.h"

// Visualization includes
#include "NLM/Visualization/VisualizationInterface.h"

// Agent includes
#include "NLM/Agent/AgentBrain.h"
#include "NLM/Agent/SensoryPercept.h"

// World includes
#include "NLM/World/SimpleWorld.h"

// Agent includes
#include "NLM/Core/Config/Config.h"

// Forward declarations
namespace py = pybind11;

// Type definitions for Python
using ConfigPtr = std::shared_ptr<NLM::Config>;
using RandomPtr = std::shared_ptr<NLM::Random>;
using LoggerPtr = std::shared_ptr<NLM::Logger>;
using SimulationClockPtr = std::shared_ptr<NLM::SimulationClock>;

using NeuronPtr = std::shared_ptr<NLM::Neuron>;
using SynapsePtr = std::shared_ptr<NLM::Synapse>;
using NeuralPopulationPtr = std::shared_ptr<NLM::NeuralPopulation>;
using NeuralRegionPtr = std::shared_ptr<NLM::NeuralRegion>;
using BrainPtr = std::shared_ptr<NLM::Brain>;

using NeuromodulatorPtr = std::shared_ptr<NLM::Neuromodulator>;
using RewardPtr = std::shared_ptr<NLM::Reward>;
using PredictionErrorPtr = std::shared_ptr<NLM::PredictionError>;
using NoveltyPtr = std::shared_ptr<NLM::Novelty>;
using CuriosityPtr = std::shared_ptr<NLM::Curiosity>;

using WorkingMemoryPtr = std::shared_ptr<NLM::WorkingMemory>;
using EpisodicMemoryPtr = std::shared_ptr<NLM::EpisodicMemory>;
using SemanticMemoryPtr = std::shared_ptr<NLM::SemanticMemory>;
using ProceduralMemoryPtr = std::shared_ptr<NLM::ProceduralMemory>;
using AssociativeMemoryPtr = std::shared_ptr<NLM::AssociativeMemory>;
using NeuralWorkingMemoryPtr = std::shared_ptr<NLM::NeuralWorkingMemory>;
using NeuralEpisodicMemoryPtr = std::shared_ptr<NLM::NeuralEpisodicMemory>;

using ConceptFormationPtr = std::shared_ptr<NLM::ConceptFormation>;
using NeuralPlannerPtr = std::shared_ptr<NLM::NeuralPlanner>;

using SensoryInputPtr = std::shared_ptr<NLM::SensoryInput>;
using VisionPtr = std::shared_ptr<NLM::Vision>;
using AudioPtr = std::shared_ptr<NLM::Audio>;
using InternalSignalsPtr = std::shared_ptr<NLM::InternalSignals>;

using PredictionSystemPtr = std::shared_ptr<NLM::PredictionSystem>;
using NeuralPredictionPtr = std::shared_ptr<NLM::NeuralPrediction>;

using MotorSystemPtr = std::shared_ptr<NLM::MotorSystem>;
using ActionPtr = std::shared_ptr<NLM::Action>;

using EnvironmentPtr = std::shared_ptr<NLM::Environment>;
using ObservationPtr = std::shared_ptr<NLM::Observation>;
using ActionPtr = std::shared_ptr<NLM::Action>;

using ExperimentPtr = std::shared_ptr<NLM::Experiment>;
using ExperimentRunnerPtr = std::shared_ptr<NLM::ExperimentRunner>;
using MetricsPtr = std::shared_ptr<NLM::Metrics>;

using VisualizationInterfacePtr = std::shared_ptr<NLM::VisualizationInterface>;

using AgentBrainPtr = std::shared_ptr<NLM::AgentBrain>;
using SensoryPerceptPtr = std::shared_ptr<NLM::SensoryPercept>;

using SimpleWorldPtr = std::shared_ptr<NLM::SimpleWorld>;

// Python module definition
PYBIND11_MODULE(pynlm, m) {
    m.doc() = "NLM (Neuromodular Learning Machine) - Python bindings";
    m.attr("__version__") = "0.1.0";

    // Type definitions
    py::class_<NLM::Types>(m, "Types")
        .def(py::init<>());

    // Configuration system
    py::class_<ConfigPtr>(m, "ConfigPtr")
        .def("has", &ConfigPtr::has)
        .def("get", &ConfigPtr::get)
        .def("set", &ConfigPtr::set)
        .def("getKeys", &ConfigPtr::getKeys)
        .def("loadFromFile", &ConfigPtr::loadFromFile)
        .def("saveToFile", &ConfigPtr::saveToFile);

    // Random number generator
    py::class_<RandomPtr>(m, "RandomPtr")
        .def("setSeed", &RandomPtr::setSeed)
        .def("nextGaussian", &RandomPtr::nextGaussian)
        .def("nextUniform", &RandomPtr::nextUniform);

    // Logger
    py::class_<LoggerPtr>(m, "LoggerPtr")
        .def("log", &LoggerPtr::log)
        .def("setLevel", &LoggerPtr::setLevel)
        .def("getLevel", &LoggerPtr::getLevel);

    // Simulation clock
    py::class_<SimulationClockPtr>(m, "SimulationClockPtr")
        .def("getTime", &SimulationClockPtr::getTime)
        .def("step", &SimulationClockPtr::step)
        .def("setTimeScale", &SimulationClockPtr::setTimeScale)
        .def("getTimeScale", &SimulationClockPtr::getTimeScale);

    // Neural components
    py::class_<NeuronPtr>(m, "NeuronPtr")
        .def("getPotential", &NeuronPtr::getPotential)
        .def("getState", &NeuronPtr::getState)
        .def("fire", &NeuronPtr::fire)
        .def("getLastSpikeTime", &NeuronPtr::getLastSpikeTime);

    py::class_<SynapsePtr>(m, "SynapsePtr")
        .def("getWeight", &SynapsePtr::getWeight)
        .def("getDelay", &SynapsePtr::getDelay)
        .def("update", &SynapsePtr::update);

    py::class_<NeuralPopulationPtr>(m, "NeuralPopulationPtr")
        .def("getNeuronCount", &NeuralPopulationPtr::getNeuronCount)
        .def("getFiringRate", &NeuralPopulationPtr::getFiringRate)
        .def("addNeuron", &NeuralPopulationPtr::addNeuron);

    py::class_<NeuralRegionPtr>(m, "NeuralRegionPtr")
        .def("getRegionType", &NeuralRegionPtr::getRegionType)
        .def("getNeuronCount", &NeuralRegionPtr::getNeuronCount)
        .def("addPopulation", &NeuralRegionPtr::addPopulation);

    py::class_<BrainPtr>(m, "BrainPtr")
        .def("initialize", &BrainPtr::initialize)
        .def("step", &BrainPtr::step)
        .def("reset", &BrainPtr::reset)
        .def("getTotalNeuronCount", &BrainPtr::getTotalNeuronCount)
        .def("getTotalSynapseCount", &BrainPtr::getTotalSynapseCount)
        .def("getFiringNeuronCount", &BrainPtr::getFiringNeuronCount)
        .def("getAverageFiringRate", &BrainPtr::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &BrainPtr::getExcitationInhibitionRatio)
        .def("save", &BrainPtr::save)
        .def("load", &BrainPtr::load)
        .def("addRegion", &BrainPtr::addRegion)
        .def("getRegion", &BrainPtr::getRegion)
        .def("enableRewardModulation", &BrainPtr::enableRewardModulation)
        .def("enableStructuralPlasticity", &BrainPtr::enableStructuralPlasticity)
        .def("enableDevelopment", &BrainPtr::enableDevelopment)
        .def("enableCuriosity", &BrainPtr::enableCuriosity)
        .def("getDevelopmentalStage", &BrainPtr::getDevelopmentalStage);

    // Neuromodulation
    py::class_<NeuromodulatorPtr>(m, "NeuromodulatorPtr")
        .def("getDopamineLevel", &NeuromodulatorPtr::getDopamineLevel)
        .def("getCuriosityLevel", &NeuromodulatorPtr::getCuriosityLevel)
        .def("getNoveltyLevel", &NeuromodulatorPtr::getNoveltyLevel)
        .def("update", &NeuromodulatorPtr::update);

    // Memory systems
    py::class_<WorkingMemoryPtr>(m, "WorkingMemoryPtr")
        .def("store", &WorkingMemoryPtr::store)
        .def("retrieve", &WorkingMemoryPtr::retrieve)
        .def("getFiringNeurons", &WorkingMemoryPtr::getFiringNeurons)
        .def("clear", &WorkingMemoryPtr::clear)
        .def("getAttention", &WorkingMemoryPtr::getAttention);

    py::class_<EpisodicMemoryPtr>(m, "EpisodicMemoryPtr")
        .def("storeExperience", &EpisodicMemoryPtr::storeExperience)
        .def("recallExperience", &EpisodicMemoryPtr::recallExperience)
        .def("getMemoryStrength", &EpisodicMemoryPtr::getMemoryStrength)
        .def("getRecentMemories", &EpisodicMemoryPtr::getRecentMemories);

    py::class_<SemanticMemoryPtr>(m, "SemanticMemoryPtr")
        .def("storeConcept", &SemanticMemoryPtr::storeConcept)
        .def("retrieveConcept", &SemanticMemoryPtr::retrieveConcept)
        .def("getConceptFrequency", &SemanticMemoryPtr::getConceptFrequency);

    py::class_<ProceduralMemoryPtr>(m, "ProceduralMemoryPtr")
        .def("storeAction", &ProceduralMemoryPtr::storeAction)
        .def("retrieveAction", &ProceduralMemoryPtr::retrieveAction)
        .def("getActionStrength", &ProceduralMemoryPtr::getActionStrength);

    py::class_<AssociativeMemoryPtr>(m, "AssociativeMemoryPtr")
        .def("storeAssociation", &AssociativeMemoryPtr::storeAssociation)
        .def("retrieveAssociation", &AssociativeMemoryPtr::retrieveAssociation)
        .def("getAssociationStrength", &AssociativeMemoryPtr::getAssociationStrength);

    py::class_<NeuralWorkingMemoryPtr>(m, "NeuralWorkingMemoryPtr")
        .def("update", &NeuralWorkingMemoryPtr::update)
        .def("getActiveNeurons", &NeuralWorkingMemoryPtr::getActiveNeurons)
        .def("getAttentionWeights", &NeuralWorkingMemoryPtr::getAttentionWeights);

    py::class_<NeuralEpisodicMemoryPtr>(m, "NeuralEpisodicMemoryPtr")
        .def("encode", &NeuralEpisodicMemoryPtr::encode)
        .def("replay", &NeuralEpisodicMemoryPtr::replay)
        .def("consolidate", &NeuralEpisodicMemoryPtr::consolidate)
        .def("getConsolidationProgress", &NeuralEpisodicMemoryPtr::getConsolidationProgress);

    // Cognition
    py::class_<ConceptFormationPtr>(m, "ConceptFormationPtr")
        .def("processInput", &ConceptFormationPtr::processInput)
        .def("getConceptCount", &ConceptFormationPtr::getConceptCount)
        .def("getConceptStrength", &ConceptFormationPtr::getConceptStrength);

    py::class_<NeuralPlannerPtr>(m, "NeuralPlannerPtr")
        .def("plan", &NeuralPlannerPtr::plan)
        .def("getPlanDuration", &NeuralPlannerPtr::getPlanDuration)
        .def("getPlanComplexity", &NeuralPlannerPtr::getPlanComplexity);

    // Sensory systems
    py::class_<SensoryInputPtr>(m, "SensoryInputPtr")
        .def("getVisionData", &SensoryInputPtr::getVisionData)
        .def("getAudioData", &SensoryInputPtr::getAudioData)
        .def("getInternalData", &SensoryInputPtr::getInternalData)
        .def("update", &SensoryInputPtr::update);

    py::class_<VisionPtr>(m, "VisionPtr")
        .def("getImageData", &VisionPtr::getImageData)
        .def("getFieldOfView", &VisionPtr::getFieldOfView)
        .def("getResolution", &VisionPtr::getResolution);

    py::class_<AudioPtr>(m, "AudioPtr")
        .def("getSoundData", &AudioPtr::getSoundData)
        .def("getSoundFrequency", &AudioPtr::getSoundFrequency)
        .def("getSoundAmplitude", &AudioPtr::getSoundAmplitude);

    py::class_<InternalSignalsPtr>(m, "InternalSignalsPtr")
        .def("getHomeostatValue", &InternalSignalsPtr::getHomeostatValue)
        .def("getArousalLevel", &InternalSignalsPtr::getArousalLevel)
        .def("getAttentionLevel", &InternalSignalsPtr::getAttentionLevel)
        .def("update", &InternalSignalsPtr::update);

    // Prediction system
    py::class_<PredictionSystemPtr>(m, "PredictionSystemPtr")
        .def("predict", &PredictionSystemPtr::predict)
        .def("getPredictionConfidence", &PredictionSystemPtr::getPredictionConfidence)
        .def("getErrorSignal", &PredictionSystemPtr::getErrorSignal)
        .def("update", &PredictionSystemPtr::update);

    py::class_<NeuralPredictionPtr>(m, "NeuralPredictionPtr")
        .def("getPredictionStrength", &NeuralPredictionPtr::getPredictionStrength)
        .def("getPredictionDuration", &NeuralPredictionPtr::getPredictionDuration)
        .def("getPredictionTarget", &NeuralPredictionPtr::getPredictionTarget);

    // Motor system
    py::class_<MotorSystemPtr>(m, "MotorSystemPtr")
        .def("getAction", &MotorSystemPtr::getAction)
        .def("getMotorOutput", &MotorSystemPtr::getMotorOutput)
        .def("getActionStrength", &MotorSystemPtr::getActionStrength)
        .def("update", &MotorSystemPtr::update);

    // Environment
    py::class_<EnvironmentPtr>(m, "EnvironmentPtr")
        .def("reset", &EnvironmentPtr::reset)
        .def("update", &EnvironmentPtr::update)
        .def("getSensoryPercept", &EnvironmentPtr::getSensoryPercept)
        .def("getObservation", &EnvironmentPtr::getObservation)
        .def("getAction", &EnvironmentPtr::getAction)
        .def("applyAction", &EnvironmentPtr::applyAction)
        .def("setRandomSeed", &EnvironmentPtr::setRandomSeed)
        .def("configure", &EnvironmentPtr::configure);

    py::class_<ObservationPtr>(m, "ObservationPtr")
        .def("getPosition", &ObservationPtr::getPosition)
        .def("getVelocity", &ObservationPtr::getVelocity)
        .def("getEnergy", &ObservationPtr::getEnergy)
        .def("getAttributes", &ObservationPtr::getAttributes);

    // Agent components
    py::class_<AgentBrainPtr>(m, "AgentBrainPtr")
        .def("initialize", &AgentBrainPtr::initialize)
        .def("processSensoryInput", &AgentBrainPtr::processSensoryInput)
        .def("decodeMotorCommand", &AgentBrainPtr::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrainPtr::applyRewardModulation)
        .def("updateDevelopment", &AgentBrainPtr::updateDevelopment)
        .def("getCuriosityLevel", &AgentBrainPtr::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrainPtr::getNoveltyLevel)
        .def("getPredictionError", &AgentBrainPtr::getPredictionError)
        .def("getNeuromodulationLevel", &AgentBrainPtr::getNeuromodulationLevel)
        .def("enableRewardModulation", &AgentBrainPtr::enableRewardModulation)
        .def("enableStructuralPlasticity", &AgentBrainPtr::enableStructuralPlasticity)
        .def("enableDevelopment", &AgentBrainPtr::enableDevelopment)
        .def("enableCuriosity", &AgentBrainPtr::enableCuriosity);

    py::class_<SensoryPerceptPtr>(m, "SensoryPerceptPtr")
        .def("getVisionData", &SensoryPerceptPtr::getVisionData)
        .def("getAudioData", &SensoryPerceptPtr::getAudioData)
        .def("getInternalData", &SensoryPerceptPtr::getInternalData)
        .def("getPosition", &SensoryPerceptPtr::getPosition)
        .def("getVelocity", &SensoryPerceptPtr::getVelocity)
        .def("getEnergy", &SensoryPerceptPtr::getEnergy);

    // World
    py::class_<SimpleWorldPtr>(m, "SimpleWorldPtr")
        .def("reset", &SimpleWorldPtr::reset)
        .def("update", &SimpleWorldPtr::update)
        .def("setAgentStart", &SimpleWorldPtr::setAgentStart)
        .def("getSensoryPercept", &SimpleWorldPtr::getSensoryPercept)
        .def("getAgentBody", &SimpleWorldPtr::getAgentBody)
        .def("applyMotorCommand", &SimpleWorldPtr::applyMotorCommand)
        .def("getSimulationTime", &SimpleWorldPtr::getSimulationTime)
        .def("getWidth", &SimpleWorldPtr::getWidth)
        .def("getHeight", &SimpleWorldPtr::getHeight)
        .def("getMaxEnergy", &SimpleWorldPtr::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorldPtr::setMaxEnergy)
        .def("configure", &SimpleWorldPtr::configure)
        .def("setRandomSeed", &SimpleWorldPtr::setRandomSeed);

    // Factory functions
    m.def("createDefaultConfig", &createDefaultConfig, "Create default configuration");
    m.def("createBrain", &createBrain, "Create a new brain instance");
    m.def("createAgentBrain", &createAgentBrain, "Create agent brain interface");
    m.def("createSimpleWorld", &createSimpleWorld, "Create simple world environment");
    m.def("createExperiment", &createExperiment, "Create experiment");

    // Utility functions
    m.def("getVersion", []() { return "0.1.0"; });
    m.def("getBuildInfo", []() {
        return "NLM Python bindings (built with pybind11)";
    });
}
