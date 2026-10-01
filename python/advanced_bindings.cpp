#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

// Include advanced feature headers
#include "../src/advanced/AdvancedBrain.hpp"
#include "../src/advanced/AdvancedPlasticity.hpp"
#include "../src/advanced/AdvancedSimulation.hpp"
#include "../src/advanced/AdvancedAnalysis.hpp"
#include "../src/advanced/AdvancedConfig.hpp"
#include "../src/experiments/advanced_experiments.hpp"

// Include existing bindings for base functionality
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

PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = "Advanced features for NLM neural simulation framework";
    
    // ---------------------------------------------------------------------------
    // Advanced Brain bindings
    // ---------------------------------------------------------------------------
    py::class_<nlm::AdvancedBrain>(m, "AdvancedBrain")
        .def(py::init<std::shared_ptr<nlm::Config>>())
        .def("initialize", &nlm::AdvancedBrain::initialize)
        .def("step", py::overload_cast<size_t>(&nlm::AdvancedBrain::step))
        .def("step", py::overload_cast<size_t, double>(&nlm::AdvancedBrain::step))
        .def("getTotalNeuronCount", &nlm::AdvancedBrain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &nlm::AdvancedBrain::getTotalSynapseCount)
        .def("getActiveNeuronCount", &nlm::AdvancedBrain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &nlm::AdvancedBrain::getFiringNeuronCount)
        .def("getAverageFiringRate", &nlm::AdvancedBrain::getAverageFiringRate)
        .def("produceAction", &nlm::AdvancedBrain::produceAction)
        .def("injectCurrent", &nlm::AdvancedBrain::injectCurrent)
        .def("injectCurrentToNeurons", &nlm::AdvancedBrain::injectCurrentToNeurons)
        .def("getConfig", &nlm::AdvancedBrain::getConfig)
        .def("getAdvancedConfig", &nlm::AdvancedBrain::getAdvancedConfig)
        .def("updateAdvancedConfig", &nlm::AdvancedBrain::updateAdvancedConfig)
        .def("addCustomNeuron", &nlm::AdvancedBrain::addCustomNeuron)
        .def("updateNeuronProperties", &nlm::AdvancedBrain::updateNeuronProperties)
        .def("loadMultiRegionArchitecture", &nlm::AdvancedBrain::loadMultiRegionArchitecture)
        .def("addRegionWithArchitecture", &nlm::AdvancedBrain::addRegionWithArchitecture)
        .def("addPlasticityMechanism", &nlm::AdvancedBrain::addPlasticityMechanism)
        .def("configurePlasticityRule", &nlm::AdvancedBrain::configurePlasticityRule)
        .def("loadDevelopmentTrajectory", &nlm::AdvancedBrain::loadDevelopmentTrajectory)
        .def("setDevelopmentTrajectory", &nlm::AdvancedBrain::setDevelopmentTrajectory)
        .def("getCurrentDevelopmentTrajectory", &nlm::AdvancedBrain::getCurrentDevelopmentTrajectory)
        .def("addCustomNeuromodulator", &nlm::AdvancedBrain::addCustomNeuromodulator)
        .def("activateNeuromodulator", &nlm::AdvancedBrain::activateNeuromodulator)
        .def("deactivateNeuromodulator", &nlm::AdvancedBrain::deactivateNeuromodulator)
        .def("getNeuromodulatorLevel", &nlm::AdvancedBrain::getNeuromodulatorLevel)
        .def("setCustomDynamicsModel", &nlm::AdvancedBrain::setCustomDynamicsModel)
        .def("configureCustomSynapticConductance", &nlm::AdvancedBrain::configureCustomSynapticConductance)
        .def("setSimulationMode", &nlm::AdvancedBrain::setSimulationMode)
        .def("getSimulationMode", &nlm::AdvancedBrain::getSimulationMode)
        .def("enableParallelProcessing", &nlm::AdvancedBrain::enableParallelProcessing)
        .def("enableMemoryOptimization", &nlm::AdvancedBrain::enableMemoryOptimization)
        .def("enableAnalysisFeature", &nlm::AdvancedBrain::enableAnalysisFeature)
        .def("disableAnalysisFeature", &nlm::AdvancedBrain::disableAnalysisFeature)
        .def("isAnalysisFeatureEnabled", &nlm::AdvancedBrain::isAnalysisFeatureEnabled)
        .def("addLearningTrajectory", &nlm::AdvancedBrain::addLearningTrajectory)
        .def("getLearningTrajectory", &nlm::AdvancedBrain::getLearningTrajectory)
        .def("setActiveLearningTrajectory", &nlm::AdvancedBrain::setActiveLearningTrajectory)
        .def("getActiveLearningTrajectory", &nlm::AdvancedBrain::getActiveLearningTrajectory)
        .def("setNeuromodulationTimeConstant", &nlm::AdvancedBrain::setNeuromodulationTimeConstant)
        .def("setNeuromodulationStrength", &nlm::AdvancedBrain::setNeuromodulationStrength)
        .def("enableNeuromodulationLearning", &nlm::AdvancedBrain::enableNeuromodulationLearning)
        .def("addCustomLearningRule", &nlm::AdvancedBrain::addCustomLearningRule)
        .def("optimizeConnectivity", &nlm::AdvancedBrain::optimizeConnectivity)
        .def("compressSynapticWeights", &nlm::AdvancedBrain::compressSynapticWeights)
        .def("enableSparseRepresentation", &nlm::AdvancedBrain::enableSparseRepresentation)
        .def("exportConfiguration", &nlm::AdvancedBrain::exportConfiguration)
        .def("importConfiguration", &nlm::AdvancedBrain::importConfiguration)
        .def("calculateAdvancedStats", &nlm::AdvancedBrain::calculateAdvancedStats)
        .def("logAdvancedStatus", &nlm::AdvancedBrain::logAdvancedStatus);
    
    // ---------------------------------------------------------------------------
    // Advanced Plasticity bindings
    // ---------------------------------------------------------------------------
    py::enum_<nlm::AdvancedPlasticityConfig::PlasticityRuleType>(m, "PlasticityRuleType")
        .value("STDP", nlm::AdvancedPlasticityConfig::PlasticityRuleType::STDP)
        .value("Hebbian", nlm::AdvancedPlasticityConfig::PlasticityRuleType::Hebbian)
        .value("BCM", nlm::AdvancedPlasticityConfig::PlasticityRuleType::BCM)
        .value("Oja", nlm::AdvancedPlasticityConfig::PlasticityRuleType::Oja)
        .value("HTT", nlm::AdvancedPlasticityConfig::PlasticityRuleType::HTT)
        .value("RewardModulatedSTDP", nlm::AdvancedPlasticityConfig::PlasticityRuleType::RewardModulatedSTDP)
        .value("TripletSTDP", nlm::AdvancedPlasticityConfig::PlasticityRuleType::TripletSTDP)
        .value("EligibilityTrace", nlm::AdvancedPlasticityConfig::PlasticityRuleType::EligibilityTrace)
        .value("SynapticTagging", nlm::AdvancedPlasticityConfig::PlasticityRuleType::SynapticTagging)
        .value("Homeostatic", nlm::AdvancedPlasticityConfig::PlasticityRuleType::Homeostatic)
        .value("Custom", nlm::AdvancedPlasticityConfig::PlasticityRuleType::Custom);
    
    py::class_<nlm::AdvancedPlasticityConfig>(m, "AdvancedPlasticityConfig")
        .def(py::init<>())
        .def_readwrite("stdpLTPWeight", &nlm::AdvancedPlasticityConfig::stdpLTPWeight)
        .def_readwrite("stdpLTDWeight", &nlm::AdvancedPlasticityConfig::stdpLTDWeight)
        .def_readwrite("stdpTimeConstant", &nlm::AdvancedPlasticityConfig::stdpTimeConstant)
        .def_readwrite("stdpTraceDecay", &nlm::AdvancedPlasticityConfig::stdpTraceDecay)
        .def_readwrite("hebbianLearningRate", &nlm::AdvancedPlasticityConfig::hebbianLearningRate)
        .def_readwrite("hebbianThreshold", &nlm::AdvancedPlasticityConfig::hebbianThreshold)
        .def_readwrite("bcmLearningRate", &nlm::AdvancedPlasticityConfig::bcmLearningRate)
        .def_readwrite("bcmThreshold", &nlm::AdvancedPlasticityConfig::bcmThreshold)
        .def_readwrite("bcmSlidingThreshold", &nlm::AdvancedPlasticityConfig::bcmSlidingThreshold)
        .def_readwrite("ojaLearningRate", &nlm::AdvancedPlasticityConfig::ojaLearningRate)
        .def_readwrite("ojaRegularization", &nlm::AdvancedPlasticityConfig::ojaRegularization)
        .def_readwrite("httPreWeight", &nlm::AdvancedPlasticityConfig::httPreWeight)
        .def_readwrite("httPostWeight", &nlm::AdvancedPlasticityConfig::httPostWeight)
        .def_readwrite("homeostaticTarget", &nlm::AdvancedPlasticityConfig::homeostaticTarget)
        .def_readwrite("homeostaticLearningRate", &nlm::AdvancedPlasticityConfig::homeostaticLearningRate)
        .def_readwrite("homeostaticTimeConstant", &nlm::AdvancedPlasticityConfig::homeostaticTimeConstant)
        .def_readwrite("rewardModulationFactor", &nlm::AdvancedPlasticityConfig::rewardModulationFactor)
        .def_readwrite("predictionErrorModulation", &nlm::AdvancedPlasticityConfig::predictionErrorModulation)
        .def_readwrite("noveltyModulation", &nlm::AdvancedPlasticityConfig::noveltyModulation)
        .def_readwrite("minWeight", &nlm::AdvancedPlasticityConfig::minWeight)
        .def_readwrite("maxWeight", &nlm::AdvancedPlasticityConfig::maxWeight)
        .def_readwrite("weightChangeLimit", &nlm::AdvancedPlasticityConfig::weightChangeLimit);
    
    py::class_<nlm::AdvancedPlasticity>(m, "AdvancedPlasticity")
        .def(py::init<>())
        .def("configure", &nlm::AdvancedPlasticity::configure)
        .def("updateConfig", &nlm::AdvancedPlasticity::updateConfig)
        .def("getConfigValue", &nlm::AdvancedPlasticity::getConfigValue)
        .def("addPlasticityRule", &nlm::AdvancedPlasticity::addPlasticityRule)
        .def("removePlasticityRule", &nlm::AdvancedPlasticity::removePlasticityRule)
        .def("setActiveRule", &nlm::AdvancedPlasticity::setActiveRule)
        .def("getActiveRule", &nlm::AdvancedPlasticity::getActiveRule)
        .def("updateSynapse", &nlm::AdvancedPlasticity::updateSynapse)
        .def("updateTrace", &nlm::AdvancedPlasticity::updateTrace)
        .def("getTraceValue", &nlm::AdvancedPlasticity::getTraceValue)
        .def("tagSynapse", &nlm::AdvancedPlasticity::tagSynapse)
        .def("hasTag", &nlm::AdvancedPlasticity::hasTag)
        .def("getTagStrength", &nlm::AdvancedPlasticity::getTagStrength)
        .def("adjustHomeostasis", &nlm::AdvancedPlasticity::adjustHomeostasis)
        .def("applyPatternLearning", &nlm::AdvancedPlasticity::applyPatternLearning)
        .def("applyRewardModulation", &nlm::AdvancedPlasticity::applyRewardModulation)
        .def("applyPredictionError", &nlm::AdvancedPlasticity::applyPredictionError)
        .def("applyTripletSTDP", &nlm::AdvancedPlasticity::applyTripletSTDP)
        .def("applyRewardModulatedSTDP", &nlm::AdvancedPlasticity::applyRewardModulatedSTDP)
        .def("applyBCM", &nlm::AdvancedPlasticity::applyBCM)
        .def("applyOja", &nlm::AdvancedPlasticity::applyOja)
        .def("applyConstrainedHebbian", &nlm::AdvancedPlasticity::applyConstrainedHebbian)
        .def("calculateStats", &nlm::AdvancedPlasticity::calculateStats)
        .def("exportConfiguration", &nlm::AdvancedPlasticity::exportConfiguration)
        .def("importConfiguration", &nlm::AdvancedPlasticity::importConfiguration)
        .def("getRuleNames", &nlm::AdvancedPlasticity::getRuleNames)
        .def("getRuleType", &nlm::AdvancedPlasticity::getRuleType)
        .def("setCustomLearningRule", &nlm::AdvancedPlasticity::setCustomLearningRule)
        .def("getConfig", &nlm::AdvancedPlasticity::getConfig)
        .def("resetTraces", &nlm::AdvancedPlasticity::resetTraces)
        .def("enforceWeightLimits", &nlm::AdvancedPlasticity::enforceWeightLimits)
        .def("logStatus", &nlm::AdvancedPlasticity::logStatus);
    
    // ---------------------------------------------------------------------------
    // Advanced Simulation bindings
    // ---------------------------------------------------------------------------
    py::enum_<nlm::MultiAgentConfig::ExperimentControl::MemoryMode>(m, "MemoryMode")
        .value("Standard", nlm::MultiAgentConfig::ExperimentControl::MemoryMode::Standard)
        .value("Compressed", nlm::MultiAgentConfig::ExperimentControl::MemoryMode::Compressed)
        .value("Sparse", nlm::MultiAgentConfig::ExperimentControl::MemoryMode::Sparse)
        .value("Streaming", nlm::MultiAgentConfig::ExperimentControl::MemoryMode::Streaming)
        .value("OnDemand", nlm::MultiAgentConfig::ExperimentControl::MemoryMode::OnDemand);
    
    py::enum_<nlm::MultiAgentConfig::ExperimentControl::ParallelProcessingConfig::SchedulingPolicy>(m, "SchedulingPolicy")
        .value("Static", nlm::MultiAgentConfig::ExperimentControl::ParallelProcessingConfig::SchedulingPolicy::Static)
        .value("Dynamic", nlm::MultiAgentConfig::ExperimentControl::ParallelProcessingConfig::SchedulingPolicy::Dynamic)
        .value("Workstealing", nlm::MultiAgentConfig::ExperimentControl::ParallelProcessingConfig::SchedulingPolicy::Workstealing);
    
    py::class_<nlm::MultiAgentConfig::ExperimentControl>(m, "ExperimentControl")
        .def(py::init<>())
        .def_readwrite("enableStepControl", &nlm::MultiAgentConfig::ExperimentControl::enableStepControl)
        .def_readwrite("enableConditionControl", &nlm::MultiAgentConfig::ExperimentControl::enableConditionControl)
        .def_readwrite("enableRewardControl", &nlm::AdvancedPlasticityConfig::enableRewardControl)
        .def_readwrite("enableNoiseControl", &nlm::AdvancedPlasticityConfig::enableNoiseControl)
        .def_readwrite("noiseLevel", &nlm::AdvancedPlasticityConfig::noiseLevel)
        .def_readwrite("stepDuration", &nlm::AdvancedPlasticityConfig::stepDuration)
        .def_readwrite("simulationSpeed", &nlm::AdvancedPlasticityConfig::simulationSpeed);
    
    py::class_<nlm::MultiAgentConfig::ParallelProcessingConfig>(m, "ParallelProcessingConfig")
        .def(py::init<>())
        .def_readwrite("enableParallel", &nlm::MultiAgentConfig::ParallelProcessingConfig::enableParallel)
        .def_readwrite("numThreads", &nlm::MultiAgentConfig::ParallelProcessingConfig::numThreads)
        .def_readwrite("schedulingPolicy", &nlm::MultiAgentConfig::ParallelProcessingConfig::schedulingPolicy)
        .def_readwrite("loadBalancingThreshold", &nlm::MultiAgentConfig::ParallelProcessingConfig::loadBalancingThreshold)
        .def_readwrite("memoryAllocationStrategy", &nlm::MultiAgentConfig::ParallelProcessingConfig::memoryAllocationStrategy);
    
    py::class_<nlm::AgentState>(m, "AgentState")
        .def(py::init<>())
        .def_readwrite("id", &nlm::AgentState::id)
        .def_readwrite("name", &nlm::AgentState::name)
        .def_readwrite("type", &nlm::AgentState::type)
        .def_readwrite("health", &nlm::AgentState::health)
        .def_readwrite("energy", &nlm::AgentState::energy)
        .def_readwrite("experience", &nlm::AgentState::experience)
        .def_readwrite("fitness", &nlm::AgentState::fitness)
        .def_readwrite("attributes", &nlm::AgentState::attributes);
    
    py::class_<nlm::CoordinationEvent>(m, "CoordinationEvent")
        .def(py::init<>())
        .def_readwrite("type", &nlm::CoordinationEvent::type)
        .def_readwrite("timestamp", &nlm::CoordinationEvent::timestamp)
        .def_readwrite("sourceAgent", &nlm::CoordinationEvent::sourceAgent)
        .def_readwrite("targetAgent", &nlm::CoordinationEvent::targetAgent)
        .def_readwrite("message", &nlm::CoordinationEvent::message)
        .def_readwrite("data", &nlm::CoordinationEvent::data)
        .def_readwrite("priority", &nlm::CoordinationEvent::priority);
    
    py::class_<nlm::AdvancedSimulation>(m, "AdvancedSimulation")
        .def(py::init<>())
        .def("configure", &nlm::AdvancedSimulation::configure)
        .def("loadEnvironment", &nlm::AdvancedSimulation::loadEnvironment)
        .def("loadAgentTemplate", &nlm::AdvancedSimulation::loadAgentTemplate)
        .def("createAgent", &nlm::AdvancedSimulation::createAgent)
        .def("removeAgent", &nlm::AdvancedSimulation::removeAgent)
        .def("activateAgent", &nlm::AdvancedSimulation::activateAgent)
        .def("deactivateAgent", &nlm::AdvancedSimulation::deactivateAgent)
        .def("start", &nlm::AdvancedSimulation::start)
        .def("stop", &nlm::AdvancedSimulation::stop)
        .def("pause", &nlm::AdvancedSimulation::pause)
        .def("resume", &nlm::AdvancedSimulation::resume)
        .def("isRunning", &nlm::AdvancedSimulation::isRunning)
        .def("step", py::overload_cast<size_t, double>(&nlm::AdvancedSimulation::step))
        .def("step", py::overload_cast<const std::vector<nlm::CoordinationEvent>&, size_t, double>(&nlm::AdvancedSimulation::step))
        .def("sendMessage", &nlm::AdvancedSimulation::sendMessage)
        .def("broadcastMessage", &nlm::AdvancedSimulation::broadcastMessage)
        .def("triggerLearning", &nlm::AdvancedSimulation::triggerLearning)
        .def("adaptEnvironment", &nlm::AdvancedSimulation::adaptEnvironment)
        .def("setStepCondition", &nlm::AdvancedSimulation::setStepCondition)
        .def("setReward", &nlm::AdvancedSimulation::setReward)
        .def("addNoise", &nlm::AdvancedSimulation::addNoise)
        .def("setSimulationSpeed", &nlm::AdvancedSimulation::setSimulationSpeed)
        .def("setStepDuration", &nlm::AdvancedSimulation::setStepDuration)
        .def("enableParallelProcessing", &nlm::AdvancedSimulation::enableParallelProcessing)
        .def("setThreadCount", &nlm::AdvancedSimulation::setThreadCount)
        .def("getActiveThreadCount", &nlm::AdvancedSimulation::getActiveThreadCount)
        .def("setMemoryMode", &nlm::AdvancedSimulation::setMemoryMode)
        .def("enableCheckpointing", &nlm::AdvancedSimulation::enableCheckpointing)
        .def("enableLazyLoading", &nlm::AdvancedSimulation::enableLazyLoading)
        .def("getAgentState", &nlm::AdvancedSimulation::getAgentState)
        .def("getAllAgentStates", &nlm::AdvancedSimulation::getAllAgentStates)
        .def("getRecentEvents", &nlm::AdvancedSimulation::getRecentEvents)
        .def("getSimulationStats", &nlm::AdvancedSimulation::getSimulationStats)
        .def("getSystemPerformance", &nlm::AdvancedSimulation::getSystemPerformance)
        .def("detectEmergentBehaviors", &nlm::AdvancedSimulation::detectEmergentBehaviors)
        .def("calculateCollectiveIntelligence", &nlm::AdvancedSimulation::calculateCollectiveIntelligence)
        .def("analyzeSocialStructure", &nlm::AdvancedSimulation::analyzeSocialStructure)
        .def("exportState", &nlm::AdvancedSimulation::exportState)
        .def("importState", &nlm::AdvancedSimulation::importState)
        .def("exportConfiguration", &nlm::AdvancedSimulation::exportConfiguration)
        .def("importConfiguration", &nlm::AdvancedSimulation::importConfiguration)
        .def("setEnvironmentParameter", &nlm::AdvancedSimulation::setEnvironmentParameter)
        .def("getEnvironmentParameter", &nlm::AdvancedSimulation::getEnvironmentParameter)
        .def("updateEnvironment", &nlm::AdvancedSimulation::updateEnvironment)
        .def("integrateNeuralNetwork", &nlm::AdvancedSimulation::integrateNeuralNetwork)
        .def("syncNeuralNetworks", &nlm::AdvancedSimulation::syncNeuralNetworks)
        .def("optimizeConnectivity", &nlm::AdvancedSimulation::optimizeConnectivity)
        .def("compressAgentMemory", &nlm::AdvancedSimulation::compressAgentMemory)
        .def("enableMemoryPooling", &nlm::AdvancedSimulation::enableMemoryPooling)
        .def("enableDebugMode", &nlm::AdvancedSimulation::enableDebugMode)
        .def("isDebugModeEnabled", &nlm::AdvancedSimulation::isDebugModeEnabled)
        .def("logSimulationStatus", &nlm::AdvancedSimulation::logSimulationStatus)
        .def("logAgentStatus", &nlm::AdvancedSimulation::logAgentStatus)
        .def("recordExperimentData", &nlm::AdvancedSimulation::recordExperimentData)
        .def("getExperimentData", &nlm::AdvancedSimulation::getExperimentData)
        .def("getAllExperimentData", &nlm::AdvancedSimulation::getAllExperimentData)
        .def("addCoordinationMechanism", &nlm::AdvancedSimulation::addCoordinationMechanism)
        .def("enableCoordinationMechanism", &nlm::AdvancedSimulation::enableCoordinationMechanism)
        .def("isCoordinationMechanismEnabled", &nlm::AdvancedSimulation::isCoordinationMechanismEnabled)
        .def("executeCoordinationCycle", &nlm::AdvancedSimulation::executeCoordinationCycle)
        .def("processEvents", &nlm::AdvancedSimulation::processEvents)
        .def("cleanup", &nlm::AdvancedSimulation::cleanup);
    
    // ---------------------------------------------------------------------------
    // Advanced Analysis bindings
    // ---------------------------------------------------------------------------
    py::enum_<nlm::NeuralPatternType>(m, "NeuralPatternType")
        .value("SpikingPattern", nlm::NeuralPatternType::SpikingPattern)
        .value("RatePattern", nlm::NeuralPatternType::RatePattern)
        .value("PhasePattern", nlm::NeuralPatternType::PhasePattern)
        .value("SpatialPattern", nlm::NeuralPatternType::SpatialPattern)
        .value("FrequencyPattern", nlm::NeuralPatternType::FrequencyPattern)
        .value("CrossRegionPattern", nlm::NeuralPatternType::CrossRegionPattern)
        .value("CustomPattern", nlm::NeuralPatternType::CustomPattern);
    
    py::class_<nlm::PatternAnalysisResult>(m, "PatternAnalysisResult")
        .def(py::init<>())
        .def_readwrite("type", &nlm::PatternAnalysisResult::type)
        .def_readwrite("name", &nlm::PatternAnalysisResult::name)
        .def_readwrite("similarity", &nlm::PatternAnalysisResult::similarity)
        .def_readwrite("metrics", &nlm::PatternAnalysisResult::metrics)
        .def_readwrite("signature", &nlm::PatternAnalysisResult::signature)
        .def_readwrite("confidence", &nlm::PatternAnalysisResult::confidence);
    
    py::class_<nlm::LearningCurveResult>(m, "LearningCurveResult")
        .def(py::init<>())
        .def_readwrite("curve", &nlm::LearningCurveResult::curve)
        .def_readwrite("steps", &nlm::LearningCurveResult::steps)
        .def_readwrite("trials", &nlm::LearningCurveResult::trials)
        .def_readwrite("metric", &nlm::LearningCurveResult::metric)
        .def_readwrite("fitParameters", &nlm::LearningCurveResult::fitParameters)
        .def_readwrite("statistics", &nlm::LearningCurveResult::statistics)
        .def_readwrite("rSquared", &nlm::LearningCurveResult::rSquared)
        .def_readwrite("convergencePoint", &nlm::LearningCurveResult::convergencePoint);
    
    py::class_<nlm::StatisticalAnalysisResult>(m, "StatisticalAnalysisResult")
        .def(py::init<>())
        .def_readwrite("testName", &nlm::StatisticalAnalysisResult::testName)
        .def_readwrite("testStatistic", &nlm::StatisticalAnalysisResult::testStatistic)
        .def_readwrite("pValue", &nlm::StatisticalAnalysisResult::pValue)
        .def_readwrite("effectSize", &nlm::StatisticalAnalysisResult::effectSize)
        .def_readwrite("confidenceInterval", &nlm::StatisticalAnalysisResult::confidenceInterval)
        .def_readwrite("interpretation", &nlm::StatisticalAnalysisResult::interpretation);
    
    py::class_<nlm::AdvancedAnalysis>(m, "AdvancedAnalysis")
        .def(py::init<>())
        .def("configure", &nlm::AdvancedAnalysis::configure)
        .def("updateConfig", &nlm::AdvancedAnalysis::updateConfig)
        .def("analyzeNeuralPattern", &nlm::AdvancedAnalysis::analyzeNeuralPattern)
        .def("analyzeMultiplePatterns", &nlm::AdvancedAnalysis::analyzeMultiplePatterns)
        .def("detectPatternAnomalies", &nlm::AdvancedAnalysis::detectPatternAnomalies)
        .def("analyzeLearningCurve", &nlm::AdvancedAnalysis::analyzeLearningCurve)
        .def("analyzeMultipleLearningCurves", &nlm::AdvancedAnalysis::analyzeMultipleLearningCurves)
        .def("predictLearningProgress", &nlm::AdvancedAnalysis::predictLearningProgress)
        .def("performStatisticalTest", &nlm::AdvancedAnalysis::performStatisticalTest)
        .def("performRegressionAnalysis", &nlm::AdvancedAnalysis::performRegressionAnalysis)
        .def("performANOVA", &nlm::AdvancedAnalysis::performANOVA)
        .def("performMultipleStatisticalTests", &nlm::AdvancedAnalysis::performMultipleStatisticalTests)
        .def("analyzeNeuralDynamics", &nlm::AdvancedAnalysis::analyzeNeuralDynamics)
        .def("analyzeNetworkConnectivity", &nlm::AdvancedAnalysis::analyzeNetworkConnectivity)
        .def("analyzeRegionalActivity", &nlm::AdvancedAnalysis::analyzeRegionalActivity)
        .def("analyzePlasticityLearning", &nlm::AdvancedAnalysis::analyzePlasticityLearning)
        .def("analyzeDevelopmentLearning", &nlm::AdvancedAnalysis::analyzeDevelopmentLearning)
        .def("generateNeuralPatternPlot", &nlm::AdvancedAnalysis::generateNeuralPatternPlot)
        .def("generateLearningCurvePlot", &nlm::AdvancedAnalysis::generateLearningCurvePlot)
        .def("generateStatisticalPlot", &nlm::AdvancedAnalysis::generateStatisticalPlot)
        .def("generateMultiPanelPlot", &nlm::AdvancedAnalysis::generateMultiPanelPlot)
        .def("classifyNeurons", &nlm::AdvancedAnalysis::classifyNeurons)
        .def("predictLearningOutcome", &nlm::AdvancedAnalysis::predictLearningOutcome)
        .def("clusterNeurons", &nlm::AdvancedAnalysis::clusterNeurons)
        .def("reduceDimensionality", &nlm::AdvancedAnalysis::reduceDimensionality)
        .def("calculateNeuralEfficiency", &nlm::AdvancedAnalysis::calculateNeuralEfficiency)
        .def("calculateLearningRate", &nlm::AdvancedAnalysis::calculateLearningRate)
        .def("calculateStability", &nlm::AdvancedAnalysis::calculateStability)
        .def("exportResults", &nlm::AdvancedAnalysis::exportResults)
        .def("importResults", &nlm::AdvancedAnalysis::importResults)
        .def("exportConfiguration", &nlm::AdvancedAnalysis::exportConfiguration)
        .def("importConfiguration", &nlm::AdvancedAnalysis::importConfiguration)
        .def("getPatternAnalysisResult", &nlm::AdvancedAnalysis::getPatternAnalysisResult)
        .def("getLearningCurveResult", &nlm::AdvancedAnalysis::getLearningCurveResult)
        .def("getStatisticalResult", &nlm::AdvancedAnalysis::getStatisticalResult)
        .def("clearResults", py::overload_cast<>(&nlm::AdvancedAnalysis::clearResults))
        .def("clearResults", py::overload_cast<const std::string&>(&nlm::AdvancedAnalysis::clearResults))
        .def("enableMonitoring", &nlm::AdvancedAnalysis::enableMonitoring)
        .def("isMonitoringEnabled", &nlm::AdvancedAnalysis::isMonitoringEnabled)
        .def("logAnalysisStatus", &nlm::AdvancedAnalysis::logAnalysisStatus)
        .def("startStreamingAnalysis", &nlm::AdvancedAnalysis::startStreamingAnalysis)
        .def("stopStreamingAnalysis", &nlm::AdvancedAnalysis::stopStreamingAnalysis)
        .def("isStreaming", &nlm::AdvancedAnalysis::isStreaming)
        .def("getStreamingResults", &nlm::AdvancedAnalysis::getStreamingResults)
        .def("enableParallelAnalysis", &nlm::AdvancedAnalysis::enableParallelAnalysis)
        .def("isParallelAnalysisEnabled", &nlm::AdvancedAnalysis::isParallelAnalysisEnabled)
        .def("setAnalysisThreads", &nlm::AdvancedAnalysis::setAnalysisThreads);
    
    // ---------------------------------------------------------------------------
    // Advanced Config bindings
    // ---------------------------------------------------------------------------
    py::enum_<nlm::InheritanceConfig::InheritanceStrategy>(m, "InheritanceStrategy")
        .value("Override", nlm::InheritanceConfig::InheritanceStrategy::Override)
        .value("Merge", nlm::InheritanceConfig::InheritanceStrategy::Merge)
        .value("Extend", nlm::InheritanceConfig::InheritanceStrategy::Extend);
    
    py::enum_<nlm::OptimizationConfig::OptimizationMethod>(m, "OptimizationMethod")
        .value("GridSearch", nlm::OptimizationConfig::OptimizationMethod::GridSearch)
        .value("RandomSearch", nlm::OptimizationConfig::OptimizationMethod::RandomSearch)
        .value("BayesianOptimization", nlm::OptimizationConfig::OptimizationMethod::BayesianOptimization)
        .value("GeneticAlgorithm", nlm::OptimizationConfig::OptimizationMethod::GeneticAlgorithm)
        .value("GradientDescent", nlm::OptimizationConfig::OptimizationMethod::GradientDescent)
        .value("SimulatedAnnealing", nlm::OptimizationConfig::OptimizationMethod::SimulatedAnnealing);
    
    py::enum_<nlm::OptimizationConfig::ObjectiveFunction>(m, "ObjectiveFunction")
        .value("MaximizeAccuracy", nlm::OptimizationConfig::ObjectiveFunction::MaximizeAccuracy)
        .value("MinimizeError", nlm::OptimizationConfig::ObjectiveFunction::MinimizeError)
        .value("MaximizeStability", nlm::OptimizationConfig::ObjectiveFunction::MaximizeStability)
        .value("MinimizeResourceUsage", nlm::OptimizationConfig::ObjectiveFunction::MinimizeResourceUsage)
        .value("BalanceAll", nlm::OptimizationConfig::ObjectiveFunction::BalanceAll);
    
    py::class_<nlm::InheritanceConfig>(m, "InheritanceConfig")
        .def(py::init<>())
        .def_readwrite("baseConfig", &nlm::InheritanceConfig::baseConfig)
        .def_readwrite("inheritFrom", &nlm::InheritanceConfig::inheritFrom)
        .def_readwrite("inheritanceStrategy", &nlm::InheritanceConfig::inheritanceStrategy);
    
    py::class_<nlm::ConfigurationTemplate>(m, "ConfigurationTemplate")
        .def(py::init<>())
        .def_readwrite("name", &nlm::ConfigurationTemplate::name)
        .def_readwrite("description", &nlm::ConfigurationTemplate::description)
        .def_readwrite("defaultValues", &nlm::ConfigurationTemplate::defaultValues)
        .def_readwrite("templateType", &nlm::ConfigurationTemplate::templateType);
    
    py::class_<nlm::OptimizationConfig::ParameterConstraint>(m, "ParameterConstraint")
        .def(py::init<>())
        .def_readwrite("parameterName", &nlm::OptimizationConfig::ParameterConstraint::parameterName)
        .def_readwrite("minValue", &nlm::OptimizationConfig::ParameterConstraint::minValue)
        .def_readwrite("maxValue", &nlm::OptimizationConfig::ParameterConstraint::maxValue)
        .def_readwrite("constraintType", &nlm::OptimizationConfig::ParameterConstraint::constraintType);
    
    py::class_<nlm::OptimizationConfig>(m, "OptimizationConfig")
        .def(py::init<>())
        .def_readwrite("method", &nlm::OptimizationConfig::method)
        .def_readwrite("objective", &nlm::OptimizationConfig::objective)
        .def_readwrite("constraints", &nlm::OptimizationConfig::constraints)
        .def_readwrite("hyperparameters", &nlm::OptimizationConfig::hyperparameters)
        .def_readwrite("populationSize", &nlm::OptimizationConfig::populationSize)
        .def_readwrite("generations", &nlm::OptimizationConfig::generations)
        .def_readwrite("convergenceThreshold", &nlm::OptimizationConfig::convergenceThreshold)
        .def_readwrite("parallelOptimization", &nlm::OptimizationConfig::parallelOptimization);
    
    py::class_<nlm::VersionInfo>(m, "VersionInfo")
        .def(py::init<>())
        .def_readwrite("version", &nlm::VersionInfo::version)
        .def_readwrite("description", &nlm::VersionInfo::description)
        .def_readwrite("createdBy", &nlm::VersionInfo::createdBy)
        .def_readwrite("changes", &nlm::VersionInfo::changes)
        .def_readwrite("previousVersion", &nlm::VersionInfo::previousVersion);
    
    py::class_<nlm::ValidationRule>(m, "ValidationRule")
        .def(py::init<>())
        .def_readwrite("key", &nlm::ValidationRule::key)
        .def_readwrite("type", &nlm::ValidationRule::type)
        .def_readwrite("allowedValues", &nlm::ValidationRule::allowedValues)
        .def_readwrite("errorMessage", &nlm::ValidationRule::errorMessage);
    
    py::class_<nlm::AdvancedConfig>(m, "AdvancedConfig")
        .def(py::init<>())
        .def("loadBaseConfiguration", &nlm::AdvancedConfig::loadBaseConfiguration)
        .def("inheritConfiguration", &nlm::AdvancedConfig::inheritConfiguration)
        .def("resolveInheritance", &nlm::AdvancedConfig::resolveInheritance)
        .def("validateConfiguration", py::overload_cast<const nlm::Config&>(&nlm::AdvancedConfig::validateConfiguration))
        .def("validateConfiguration", py::overload_cast<const nlm::Config&, const std::vector<nlm::ValidationRule>&>(&nlm::AdvancedConfig::validateConfiguration))
        .def("validateAgainstSchema", &nlm::AdvancedConfig::validateAgainstSchema)
        .def("validateSchema", &nlm::AdvancedConfig::validateSchema)
        .def("createTemplate", &nlm::AdvancedConfig::createTemplate)
        .def("loadTemplate", &nlm::AdvancedConfig::loadTemplate)
        .def("applyTemplate", &nlm::AdvancedConfig::applyTemplate)
        .def("getTemplate", &nlm::AdvancedConfig::getTemplate)
        .def("getAvailableTemplates", &nlm::AdvancedConfig::getAvailableTemplates)
        .def("optimizeParameters", &nlm::AdvancedConfig::optimizeParameters)
        .def("gridSearch", &nlm::AdvancedConfig::gridSearch)
        .def("bayesianOptimization", &nlm::AdvancedConfig::bayesianOptimization)
        .def("createVersion", &nlm::AdvancedConfig::createVersion)
        .def("restoreVersion", &nlm::AdvancedConfig::restoreVersion)
        .def("getVersionHistory", &nlm::AdvancedConfig::getVersionHistory)
        .def("deleteVersion", &nlm::AdvancedConfig::deleteVersion)
        .def("getVersionInfo", &nlm::AdvancedConfig::getVersionInfo)
        .def("createSchema", &nlm::AdvancedConfig::createSchema)
        .def("updateSchema", &nlm::AdvancedConfig::updateSchema)
        .def("deleteSchema", &nlm::AdvancedConfig::deleteSchema)
        .def("getSchema", &nlm::AdvancedConfig::getSchema)
        .def("getAvailableSchemas", &nlm::AdvancedConfig::getAvailableSchemas)
        .def("compareConfigurations", &nlm::AdvancedConfig::compareConfigurations)
        .def("diffConfigurations", &nlm::AdvancedConfig::diffConfigurations)
        .def("calculateConfigurationDistance", &nlm::AdvancedConfig::calculateConfigurationDistance)
        .def("enableAdvancedFeatures", &nlm::AdvancedConfig::enableAdvancedFeatures)
        .def("isAdvancedFeatureEnabled", &nlm::AdvancedConfig::isAdvancedFeatureEnabled)
        .def("getEnabledFeatures", &nlm::AdvancedConfig::getEnabledFeatures)
        .def("exportConfiguration", py::overload_cast<const nlm::Config&, const std::string&>(&nlm::AdvancedConfig::exportConfiguration))
        .def("importConfiguration", py::overload_cast<nlm::Config&, const std::string&>(&nlm::AdvancedConfig::importConfiguration))
        .def("exportConfigurationWithSchema", &nlm::AdvancedConfig::exportConfigurationWithSchema)
        .def("importConfigurationWithSchema", &nlm::AdvancedConfig::importConfigurationWithSchema)
        .def("getConfiguration", &nlm::AdvancedConfig::getConfiguration)
        .def("updateConfiguration", &nlm::AdvancedConfig::updateConfiguration)
        .def("getValidationStatistics", &nlm::AdvancedConfig::getValidationStatistics)
        .def("getOptimizationStatistics", &nlm::AdvancedConfig::getOptimizationStatistics)
        .def("enableLogging", &nlm::AdvancedConfig::enableLogging)
        .def("isLoggingEnabled", &nlm::AdvancedConfig::isLoggingEnabled)
        .def("logConfiguration", &nlm::AdvancedConfig::logConfiguration);
    
    // ---------------------------------------------------------------------------
    // Advanced Experiment bindings
    // ---------------------------------------------------------------------------
    py::enum_<nlm::AdvancedExperimentConfig::ExperimentType>(m, "ExperimentType")
        .value("MultiAgent", nlm::AdvancedExperimentConfig::ExperimentType::MultiAgent)
        .value("Plasticity", nlm::AdvancedExperimentConfig::ExperimentType::Plasticity)
        .value("Analysis", nlm::AdvancedExperimentConfig::ExperimentType::Analysis);
    
    py::enum_<nlm::AdvancedExperimentConfig::Objective>(m, "Objective")
        .value("MaximizeCohesion", nlm::AdvancedExperimentConfig::Objective::MaximizeCohesion)
        .value("MinimizeError", nlm::AdvancedExperimentConfig::Objective::MinimizeError)
        .value("MaximizeAdaptability", nlm::AdvancedExperimentConfig::Objective::MaximizeAdaptability)
        .value("BalanceAll", nlm::AdvancedExperimentConfig::Objective::BalanceAll);
    
    py::class_<nlm::AdvancedExperimentConfig>(m, "AdvancedExperimentConfig")
        .def(py::init<>())
        .def_readwrite("name", &nlm::AdvancedExperimentConfig::name)
        .def_readwrite("description", &nlm::AdvancedExperimentConfig::description)
        .def_readwrite("type", &nlm::AdvancedExperimentConfig::type)
        .def_readwrite("objective", &nlm::AdvancedExperimentConfig::objective)
        .def_readwrite("duration", &nlm::AdvancedExperimentConfig::duration)
        .def_readwrite("maxAgents", &nlm::AdvancedExperimentConfig::maxAgents)
        .def_readwrite("randomSeed", &nlm::AdvancedExperimentConfig::randomSeed)
        .def_readwrite("useAdvancedFeatures", &nlm::AdvancedExperimentConfig::useAdvancedFeatures);
    
    py::class_<nlm::AdvancedExperimentResult::StepData>(m, "AdvancedExperimentResultStepData")
        .def(py::init<>())
        .def_readwrite("step", &nlm::AdvancedExperimentResult::StepData::step)
        .def_readwrite("neuronCount", &nlm::AdvancedExperimentResult::StepData::neuronCount)
        .def_readwrite("fitness", &nlm::AdvancedExperimentResult::StepData::fitness)
        .def_readwrite("progress", &nlm::AdvancedExperimentResult::StepData::progress);
    
    py::class_<nlm::AdvancedExperimentResult::AnalysisResult>(m, "AdvancedExperimentResultAnalysisResult")
        .def(py::init<>())
        .def_readwrite("name", &nlm::AdvancedExperimentResult::AnalysisResult::name)
        .def_readwrite("data", &nlm::AdvancedExperimentResult::AnalysisResult::data);
    
    py::class_<nlm::AdvancedExperimentResult>(m, "AdvancedExperimentResult")
        .def(py::init<>())
        .def_readwrite("experimentName", &nlm::AdvancedExperimentResult::experimentName)
        .def_readwrite("experimentType", &nlm::AdvancedExperimentResult::experimentType)
        .def_readwrite("success", &nlm::AdvancedExperimentResult::success)
        .def_readwrite("duration", &nlm::AdvancedExperimentResult::duration)
        .def_readwrite("finalMetric", &nlm::AdvancedExperimentResult::finalMetric)
        .def_readwrite("agentCount", &nlm::AdvancedExperimentResult::agentCount)
        .def_readwrite("stepData", &nlm::AdvancedExperimentResult::stepData)
        .def_readwrite("analysisResults", &nlm::AdvancedExperimentResult::analysisResults);
    
    py::class_<nlm::AdvancedExperiment>(m, "AdvancedExperiment")
        .def(py::init<>())
        .def("configure", &nlm::AdvancedExperiment::configure)
        .def("run", &nlm::AdvancedExperiment::run)
        .def("getResult", &nlm::AdvancedExperiment::getResult)
        .def("getAllResults", &nlm::AdvancedExperiment::getAllResults)
        .def("saveResults", &nlm::AdvancedExperiment::saveResults)
        .def("enableAdvancedFeatures", &nlm::AdvancedExperiment::enableAdvancedFeatures)
        .def("enableExperimentTracking", &nlm::AdvancedExperiment::enableExperimentTracking)
        .def("logExperimentStatus", &nlm::AdvancedExperiment::logExperimentStatus);
    
    // ---------------------------------------------------------------------------
    // Expose essential types from core library
    // ---------------------------------------------------------------------------
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Internal", nlm::NeuronType::Internal)
        .value("Motor", nlm::NeuronType::Motor);
    
    py::class_<nlm::Types>(m, "Types");
    
    // ---------------------------------------------------------------------------
    // Summary
    // ---------------------------------------------------------------------------
    m.def("get_version", []() {
        return "1.0.0";
    });
    
    m.def("get_description", []() {
        return "Advanced features for NLM neural simulation framework";
    });
    
    m.def("list_advanced_features", []() {
        return std::vector<std::string>{
            "Advanced Brain Features",
            "Advanced Plasticity Mechanisms", 
            "Multi-Agent Systems",
            "Advanced Experiment Control",
            "Parallel Processing",
            "Memory-Efficient Simulation",
            "Neural Pattern Analysis",
            "Learning Curve Analysis",
            "Statistical Analysis Tools",
            "Advanced Configuration",
            "Configuration Templates and Versioning",
            "Machine Learning Integration",
            "Advanced Visualization",
            "Performance Optimization"
        };
    });
}

// Include the main bindings for base functionality
#include "bindings.cpp"
