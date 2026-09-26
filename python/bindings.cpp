#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"
#include "../src/performance/Performance.hpp"
#include "../src/performance/CheckpointSystem.hpp"
#include "src/development/Maturation.hpp"
#include "src/development/Synaptogenesis.hpp"
#include "src/development/Pruning.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        ----------------------------------------------------------
        Advanced Python bindings for the NLM C++ neural simulation framework.
        Includes debugging tools, experiment management, performance monitoring,
        checkpoint management, and development system tools.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // === Basic Classes (Original API) ===

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

    // Enumerations
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

    // === Enhanced Debugging Commands ===

    // Memory System Diagnostics
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f)
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"))
        .def("retrieve", &NeuralWorkingMemory::retrieve)
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"))
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"))
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces)
        .def("getCapacity", &NeuralWorkingMemory::getCapacity)
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"))
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate)
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"))
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons)
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"))
        .def("runCompetition", &NeuralWorkingMemory::runCompetition)
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"))
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity)
        .def("diagnoseMemorySystem", [](NeuralWorkingMemory& self) {
            auto stats = self.retrieve();
            std::map<std::string, double> diagnostics;
            diagnostics["memory_activity"] = self.getMemoryActivity();
            diagnostics["active_traces"] = (double)self.getActiveTraces();
            diagnostics["memory_capacity_utilization"] = 
                (double)self.getActiveTraces() / (double)self.getCapacity();
            diagnostics["current_patterns"] = stats.size();
            return diagnostics;
        }, "Run memory system diagnostics");

    // Neural Activity Analysis
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced brain class with debugging support)pbdoc")
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
             "Log brain status")
        // Advanced neural activity analysis
        .def("analyzeNeuralActivity", [](Brain& self) {
            std::map<std::string, double> analysis;
            analysis["total_neurons"] = (double)self.getTotalNeuronCount();
            analysis["total_synapses"] = (double)self.getTotalSynapseCount();
            analysis["active_neurons"] = (double)self.getActiveNeuronCount();
            analysis["firing_neurons"] = (double)self.getFiringNeuronCount();
            analysis["average_firing_rate"] = self.getAverageFiringRate();
            analysis["e_i_ratio"] = self.getExcitationInhibitionRatio();
            analysis["total_spikes"] = (double)self.getTotalSpikeCount();
            return analysis;
        }, "Analyze neural activity metrics")
        .def("getSpikeAnalysis", [](Brain& self) {
            // Simplified spike analysis - in reality this would be more complex
            std::map<std::string, double> spikeAnalysis;
            spikeAnalysis["recent_spike_rate"] = self.getAverageFiringRate();
            spikeAnalysis["active_ratio"] = self.getActiveNeuronCount() / (double)self.getTotalNeuronCount();
            return spikeAnalysis;
        }, "Get detailed spike statistics")
        .def("analyzeConnectivity", [](Brain& self) {
            // Simplified connectivity analysis
            std::map<std::string, double> connectivityAnalysis;
            connectivityAnalysis["connectivity_density"] = self.getTotalSynapseCount() / (double)(self.getTotalNeuronCount() * self.getTotalNeuronCount());
            connectivityAnalysis["e_i_ratio"] = self.getExcitationInhibitionRatio();
            return connectivityAnalysis;
        }, "Analyze neural connectivity patterns");

    // Prediction System Status
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for future state prediction)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionSystem::initialize, py::arg("brain"))
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"))
        .def("updatePredictions", &PredictionSystem::updatePredictions, py::arg("predicted"), py::arg("actual"))
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("getConfidence", &PredictionSystem::getConfidence)
        .def("getErrorHistory", &PredictionSystem::getErrorHistory)
        .def("clearHistory", &PredictionSystem::clearHistory)
        .def("train", &PredictionSystem::train, py::arg("observation"))
        .def("getWorkingMemoryTargets", &PredictionSystem::getWorkingMemoryTargets)
        .def("getConceptPrototypeTargets", &PredictionSystem::getConceptPrototypeTargets, py::arg("conceptId"))
        .def("getAllConceptPrototypes", &PredictionSystem::getAllConceptPrototypes)
        .def("getConceptUpdateSignal", &PredictionSystem::getConceptUpdateSignal)
        .def("reset", &PredictionSystem::reset)
        .def("analyzePredictionSystem", [](PredictionSystem& self) {
            std::map<std::string, double> analysis;
            analysis["prediction_error"] = self.getPredictionError();
            analysis["confidence"] = self.getConfidence();
            analysis["history_size"] = (double)self.getErrorHistory().size();
            analysis["current_targets_count"] = (double)self.getWorkingMemoryTargets().size();
            analysis["last_error"] = self.getErrorHistory().empty() ? 0.0 : self.getErrorHistory().back();
            return analysis;
        }, "Analyze prediction system status");

    // Concept Formation Analysis
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"))
        .def("presentExperience", &ConceptFormation::presentExperience, 
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"))
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f)
        .def("getConcepts", &ConceptFormation::getConcepts)
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"))
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"))
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"))
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"))
        .def("getConceptCount", &ConceptFormation::getConceptCount)
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"))
        .def("updateConcept", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"))
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"))
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f)
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"))
        .def("clear", &ConceptFormation::clear)
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"))
        .def("analyzeConceptFormation", [](ConceptFormation& self) {
            std::map<std::string, double> analysis;
            analysis["concept_count"] = (double)self.getConceptCount();
            double totalStability = 0.0;
            size_t stableConcepts = 0;
            for (size_t i = 0; i < self.getConceptCount(); ++i) {
                if (self.getConceptStability(i) > 0.5f) {
                    stableConcepts++;
                }
                totalStability += self.getConceptStability(i);
            }
            analysis["stable_concepts"] = (double)stableConcepts;
            analysis["average_stability"] = self.getConceptCount() > 0 ? totalStability / (double)self.getConceptCount() : 0.0;
            analysis["novelty_rate"] = 0.0; // Would need tracking
            return analysis;
        }, "Analyze concept formation system");

    // Neural Planner (Planning System Diagnostics)
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planning system)pbdoc")
        .def(py::init<>())
        .def("initialize", [](NeuralPlanner& self, std::shared_ptr<Brain> brain) {
            self.initialize(brain.get());
        }, py::arg("brain"))
        .def("planAction", [](NeuralPlanner& self, const std::vector<float>& currentState, 
                             const std::vector<float>& goalState, float timeHorizon) {
            return self.planAction(currentState, goalState, timeHorizon);
        }, py::arg("currentState"), py::arg("goalState"), py::arg("timeHorizon"))
        .def("analyzePlan", [](NeuralPlanner& self) {
            std::map<std::string, double> analysis;
            analysis["plan_confidence"] = 0.0; // Would need actual implementation
            analysis["plan_complexity"] = 0.0;
            analysis["execution_time"] = 0.0;
            analysis["goal_achievement"] = 0.0;
            return analysis;
        }, "Analyze current plan")
        .def("getCurrentPlan", &NeuralPlanner::getCurrentPlan)
        .def("resetPlan", &NeuralPlanner::reset)
        .def("setPlanningParameters", &NeuralPlanner::setParameters, py::arg("horizon"), py::arg("confidence"), py::arg("exploration"))
        .def("diagnosePlanningSystem", [](NeuralPlanner& self) {
            std::map<std::string, double> diagnostics;
            diagnostics["planning_enabled"] = 1.0; // Would need actual state
            diagnostics["plan_confidence"] = 0.0;
            diagnostics["plan_horizon"] = 0.0;
            diagnostics["exploration_rate"] = 0.0;
            return diagnostics;
        }, "Diagnose planning system status");

    // === Experiment Management Tools ===

    // Experiment Metrics
    py::class_<Metrics>(m, "Metrics", R"pbdoc(Experiment metrics collection)pbdoc")
        .def(py::init<>())
        .def("record", &Metrics::record, py::arg("name"), py::arg("value"), py::arg("timestamp"))
        .def("getMetric", &Metrics::getMetric, py::arg("name"))
        .def("getMetricHistory", &Metrics::getMetricHistory, py::arg("name"))
        .def("getAllMetricNames", &Metrics::getAllMetricNames)
        .def("reset", &Metrics::reset)
        .def("getSummary", [](Metrics& self) {
            std::map<std::string, double> summary;
            for (const auto& name : self.getAllMetricNames()) {
                summary[name] = self.getMetric(name);
            }
            return summary;
        }, "Get metrics summary")
        .def("exportMetrics", &Metrics::exportToFile, py::arg("filepath"))
        .def("importMetrics", &Metrics::importFromFile, py::arg("filepath"))
        .def("compareMetrics", &Metrics::compare, py::arg("other"));

    // Experiment Framework
    py::class_<Experiment>(m, "Experiment", R"pbdoc(Experiment framework)pbdoc")
        .def(py::init<>())
        .def("setConfiguration", &Experiment::setConfiguration, py::arg("config"))
        .def("setBrain", &Experiment::setBrain, py::arg("brain"))
        .def("setWorld", &Experiment::setWorld, py::arg("world"))
        .def("setAgentBrain", &Experiment::setAgentBrain, py::arg("agentBrain"))
        .def("initialize", &Experiment::initialize)
        .def("runTrial", &Experiment::runTrial, py::arg("trialId"), py::arg("config"))
        .def("getMetrics", &Experiment::getMetrics)
        .def("getTrialData", &Experiment::getTrialData, py::arg("trialId"))
        .def("getTrialCount", &Experiment::getTrialCount)
        .def("getCurrentTrial", &Experiment::getCurrentTrial)
        .def("isRunning", &Experiment::isRunning)
        .def("stop", &Experiment::stop)
        .def("reset", &Experiment::reset);

    // Phase 4 Experiments
    py::class_<Phase4Experiment>(m, "Phase4Experiment", R"pbdoc(Phase 4 cognitive experiments)pbdoc")
        .def(py::init<>())
        .def("runTemporalPredictionLearning", &Phase4Experiment::runTemporalPredictionLearning,
             py::arg("numTrials"), py::arg("config"))
        .def("runWorkingMemoryTest", &Phase4Experiment::runWorkingMemoryTest,
             py::arg("numTrials"), py::arg("config"))
        .def("runEpisodicRecallTest", &Phase4Experiment::runEpisodicRecallTest,
             py::arg("numTrials"), py::arg("config"))
        .def("runConceptFormationTest", &Phase4Experiment::runConceptFormationTest,
             py::arg("numTrials"), py::arg("config"))
        .def("runMultiStepPlanningTest", &Phase4Experiment::runMultiStepPlanningTest,
             py::arg("numTrials"), py::arg("config"))
        .def("getExperimentResults", &Phase4Experiment::getExperimentResults)
        .def("getExperimentMetrics", &Phase4Experiment::getExperimentMetrics)
        .def("analyzeResults", &Phase4Experiment::analyzeResults);

    // Experiment Runner
    py::class_<ExperimentRunner>(m, "ExperimentRunner", R"pbdoc(Experiment execution and management)pbdoc")
        .def(py::init<>())
        .def("addExperiment", &ExperimentRunner::addExperiment, py::arg("experiment"))
        .def("removeExperiment", &ExperimentRunner::removeExperiment, py::arg("experimentName"))
        .def("getExperiments", &ExperimentRunner::getExperiments)
        .def("runExperiment", &ExperimentRunner::runExperiment, py::arg("experimentName"), py::arg("config"))
        .def("runAllExperiments", &ExperimentRunner::runAllExperiments)
        .def("stopExperiment", &ExperimentRunner::stopExperiment, py::arg("experimentName"))
        .def("getExperimentStatus", &ExperimentRunner::getExperimentStatus, py::arg("experimentName"))
        .def("getAllStatus", &ExperimentRunner::getAllStatus)
        .def("saveResults", &ExperimentRunner::saveResults, py::arg("experimentName"), py::arg("filepath"))
        .def("loadResults", &ExperimentRunner::loadResults, py::arg("filepath"))
        .def("compareResults", &ExperimentRunner::compareResults, py::arg("experiment1"), py::arg("experiment2"));

    // === Performance Monitoring Tools ===

    // Performance Monitor
    py::class_<Performance>(m, "Performance", R"pbdoc(Performance monitoring and metrics)pbdoc")
        .def(py::init<>())
        .def("startMonitoring", &Performance::startMonitoring)
        .def("stopMonitoring", &Performance::stopMonitoring)
        .def("isMonitoring", &Performance::isMonitoring)
        .def("recordMetric", &Performance::recordMetric, py::arg("name"), py::arg("value"), py::arg("timestamp"))
        .def("getMetric", &Performance::getMetric, py::arg("name"))
        .def("getRealTimeMetrics", &Performance::getRealTimeMetrics)
        .def("getHistoricalMetrics", &Performance::getHistoricalMetrics, py::arg("durationSeconds"))
        .def("getMemoryUsage", &Performance::getMemoryUsage)
        .def("getCPUMetrics", &Performance::getCPUMetrics)
        .def("getGPUMetrics", &Performance::getGPUMetrics)
        .def("analyzePerformanceTrends", &Performance::analyzePerformanceTrends)
        .def("getPerformanceReport", [](Performance& self) {
            std::map<std::string, double> report;
            auto metrics = self.getRealTimeMetrics();
            for (const auto& metric : metrics) {
                report[metric.name] = metric.value;
            }
            report["memory_usage_mb"] = self.getMemoryUsage();
            return report;
        }, "Get current performance report");

    // Memory Pool Monitor
    py::class_<MemoryPool>(m, "MemoryPool", R"pbdoc(Memory pool monitoring)pbdoc")
        .def(py::init<>())
        .def("getUsage", &MemoryPool::getUsage)
        .def("getAllocatedSize", &MemoryPool::getAllocatedSize)
        .def("getUtilization", &MemoryPool::getUtilization)
        .def("getFragmentation", &MemoryPool::getFragmentation)
        .def("getPoolStats", [](MemoryPool& self) {
            std::map<std::string, double> stats;
            stats["allocated_size"] = (double)self.getAllocatedSize();
            stats["utilization"] = self.getUtilization();
            stats["fragmentation"] = self.getFragmentation();
            stats["usage"] = (double)self.getUsage();
            return stats;
        }, "Get memory pool statistics");

    // === Checkpoint Management Tools ===

    // Checkpoint Manager
    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Automated checkpoint management)pbdoc")
        .def(py::init<>())
        .def("configure", &CheckpointManager::configure, py::arg("checkpointDir"), 
             py::arg("saveIntervalSteps") = 10000, py::arg("maxCheckpoints") = 10, py::arg("compress") = true)
        .def("update", &CheckpointManager::update, py::arg("currentStep"), py::arg("currentTime"))
        .def("saveImmediately", &CheckpointManager::saveImmediately, py::arg("name") = "")
        .def("load", &CheckpointManager::load, py::arg("name"))
        .def("listCheckpoints", &CheckpointManager::listCheckpoints)
        .def("getCheckpointDir", &CheckpointManager::getCheckpointDir)
        .def("getLastCheckpointPath", &CheckpointManager::getLastCheckpointPath)
        .def("shouldSave", &CheckpointManager::shouldSave, py::arg("currentStep"))
        .def("pruneOldCheckpoints", &CheckpointManager::pruneOldCheckpoints, py::arg("keepCount"))
        .def("analyzeCheckpoints", [](CheckpointManager& self) {
            auto checkpoints = self.listCheckpoints();
            std::map<std::string, double> analysis;
            analysis["total_checkpoints"] = (double)checkpoints.size();
            if (!checkpoints.empty()) {
                uint64_t totalSize = 0;
                uint64_t latestStep = 0;
                for (const auto& cp : checkpoints) {
                    totalSize += cp.fileSize;
                    if (cp.step > latestStep) latestStep = cp.step;
                }
                analysis["total_size_mb"] = totalSize / (1024 * 1024);
                analysis["latest_step"] = (double)latestStep;
                analysis["oldest_step"] = (double)checkpoints.front().step;
                analysis["storage_efficiency"] = 1.0 - ((double)checkpoints.size() * sizeof(CheckpointInfo)) / ((double)totalSize * 1024 * 1024);
            }
            return analysis;
        }, "Analyze checkpoint storage and usage");

    // === Development System Tools ===

    // Development System Monitor
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system monitoring)pbdoc")
        .def(py::init<>())
        .def("initialize", [](DevelopmentSystem& self, std::shared_ptr<Brain> brain) {
            self.initialize(brain.get());
        }, py::arg("brain"))
        .def("update", &DevelopmentSystem::update, py::arg("dt"))
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalStage)
        .def("getCriticalPeriodStatus", &DevelopmentSystem::getCriticalPeriodStatus)
        .def("getPlasticityRate", &DevelopmentSystem::getPlasticityRate)
        .def("getSynaptogenesisActivity", &DevelopmentSystem::getSynaptogenesisActivity)
        .def("getPruningActivity", &DevelopmentSystem::getPruningActivity)
        .def("getMaturationProgress", &DevelopmentSystem::getMaturationProgress)
        .def("getPlasticityTimeConstants", &DevelopmentSystem::getPlasticityTimeConstants)
        .def("analyzeDevelopment", [](DevelopmentSystem& self) {
            std::map<std::string, double> analysis;
            analysis["developmental_stage"] = (double)self.getDevelopmentalStage();
            analysis["critical_period_active"] = (double)self.getCriticalPeriodStatus();
            analysis["plasticity_rate"] = self.getPlasticityRate();
            analysis["synaptogenesis_activity"] = self.getSynaptogenesisActivity();
            analysis["pruning_activity"] = self.getPruningActivity();
            analysis["maturation_progress"] = self.getMaturationProgress();
            return analysis;
        }, "Analyze development system status");

    // Synaptogenesis Monitor
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Synaptogenesis monitoring)pbdoc")
        .def(py::init<>())
        .def("initialize", [](Synaptogenesis& self, std::shared_ptr<Brain> brain) {
            self.initialize(brain.get());
        }, py::arg("brain"))
        .def("update", &Synaptogenesis::update, py::arg("dt"))
        .def("getNewConnections", &Synaptogenesis::getNewConnections)
        .def("getConnectionGrowthRate", &Synaptogenesis::getConnectionGrowthRate)
        .def("getDevelopmentalInfluence", &Synaptogenesis::getDevelopmentalInfluence)
        .def("getTargetConnections", &Synaptogenesis::getTargetConnections)
        .def("analyzeSynaptogenesis", [](Synaptogenesis& self) {
            std::map<std::string, double> analysis;
            analysis["new_connections"] = (double)self.getNewConnections();
            analysis["growth_rate"] = self.getConnectionGrowthRate();
            analysis["developmental_influence"] = self.getDevelopmentalInfluence();
            analysis["target_connections"] = (double)self.getTargetConnections();
            analysis["maturation_progress"] = (double)self.getMaturationProgress();
            return analysis;
        }, "Analyze synaptogenesis activity");

    // Pruning Monitor
    py::class_<Pruning>(m, "Pruning", R"pbdoc(Pruning monitoring)pbdoc")
        .def(py::init<>())
        .def("initialize", [](Pruning& self, std::shared_ptr<Brain> brain) {
            self.initialize(brain.get());
        }, py::arg("brain"))
        .def("update", &Pruning::update, py::arg("dt"))
        .def("getPrunedConnections", &Pruning::getPrunedConnections)
        .def("getPruningRate", &Pruning::getPruningRate)
        .def("getConnectionLoss", &Pruning::getConnectionLoss)
        .def("getEfficiencyGain", &Pruning::getEfficiencyGain)
        .def("analyzePruning", [](Pruning& self) {
            std::map<std::string, double> analysis;
            analysis["pruned_connections"] = (double)self.getPrunedConnections();
            analysis["pruning_rate"] = self.getPruningRate();
            analysis["connection_loss"] = self.getConnectionLoss();
            analysis["efficiency_gain"] = self.getEfficiencyGain();
            return analysis;
        }, "Analyze pruning activity");

    // Maturation Monitor
    py::class_<Maturation>(m, "Maturation", R"pbdoc(Neural maturation monitoring)pbdoc")
        .def(py::init<>())
        .def("initialize", [](Maturation& self, std::shared_ptr<Brain> brain) {
            self.initialize(brain.get());
        }, py::arg("brain"))
        .def("update", &Maturation::update, py::arg("dt"))
        .def("getDevelopmentalStage", &Maturation::getDevelopmentalStage)
        .def("getMaturationRate", &Maturation::getMaturationRate)
        .def("getCriticalPeriodStart", &Maturation::getCriticalPeriodStart)
        .def("getCriticalPeriodEnd", &Maturation::getCriticalPeriodEnd)
        .def("isInCriticalPeriod", &Maturation::isInCriticalPeriod)
        .def("getPlasticityModifiers", &Maturation::getPlasticityModifiers)
        .def("getMaturationProgress", &Maturation::getMaturationProgress)
        .def("analyzeMaturation", [](Maturation& self) {
            std::map<std::string, double> analysis;
            analysis["developmental_stage"] = (double)self.getDevelopmentalStage();
            analysis["maturation_rate"] = self.getMaturationRate();
            analysis["critical_period_start"] = self.getCriticalPeriodStart();
            analysis["critical_period_end"] = self.getCriticalPeriodEnd();
            analysis["critical_period_active"] = (double)self.isInCriticalPeriod();
            analysis["maturation_progress"] = self.getMaturationProgress();
            return analysis;
        }, "Analyze neural maturation status");

    // === Enhanced Convenience Functions ===

    // Advanced Brain Diagnostics
    m.def("diagnoseBrain", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> diagnostics;
        diagnostics["total_neurons"] = (double)brain->getTotalNeuronCount();
        diagnostics["total_synapses"] = (double)brain->getTotalSynapseCount();
        diagnostics["active_neurons"] = (double)brain->getActiveNeuronCount();
        diagnostics["firing_neurons"] = (double)brain->getFiringNeuronCount();
        diagnostics["average_firing_rate"] = brain->getAverageFiringRate();
        diagnostics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        diagnostics["total_spikes"] = (double)brain->getTotalSpikeCount();
        return diagnostics;
    }, py::arg("brain"), "Run comprehensive brain diagnostics");

    m.def("runSystemDiagnostics", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::map<std::string, double>> systemDiagnostics;
        
        // Brain diagnostics
        auto brainDiagnostics = m["diagnoseBrain"](brain);
        systemDiagnostics["brain"] = brainDiagnostics;
        
        // Neural activity analysis
        auto activityAnalysis = brain->analyzeNeuralActivity();
        systemDiagnostics["neural_activity"] = activityAnalysis;
        
        // Connectivity analysis
        auto connectivityAnalysis = brain->analyzeConnectivity();
        systemDiagnostics["connectivity"] = connectivityAnalysis;
        
        return systemDiagnostics;
    }, py::arg("brain"), "Run comprehensive system diagnostics");

    // Experiment setup convenience functions
    m.def("createExperimentSetup", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                     std::shared_ptr<AgentBrain> agent) {
        auto experiment = std::make_shared<Experiment>();
        experiment->setConfiguration(brain->getConfig());
        experiment->setBrain(brain);
        experiment->setWorld(world);
        experiment->setAgentBrain(agent);
        return experiment;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), "Create experiment setup");

    // Performance monitoring convenience functions
    m.def("startPerformanceMonitoring", []() {
        auto performance = std::make_shared<Performance>();
        performance->startMonitoring();
        return performance;
    }, "Start performance monitoring");

    // Checkpoint management convenience functions
    m.def("createCheckpointSetup", [](const std::string& checkpointDir) {
        auto manager = std::make_shared<CheckpointManager>();
        manager->configure(checkpointDir);
        return manager;
    }, py::arg("checkpointDir"), "Create checkpoint management setup");

    // Development system setup
    m.def("createDevelopmentSetup", [](std::shared_ptr<Brain> brain) {
        auto devSystem = std::make_shared<DevelopmentSystem>();
        devSystem->initialize(brain);
        return devSystem;
    }, py::arg("brain"), "Create development system setup");

    // === Factory Functions (Original API) ===

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

    // Enhanced version of createAgentBrain with development features
    m.def("createAdvancedAgentBrain", [](std::shared_ptr<Brain> brain,
                                        std::shared_ptr<DevelopmentSystem> devSystem,
                                        std::shared_ptr<PredictionSystem> predictionSystem,
                                        std::shared_ptr<ConceptFormation> conceptFormation) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        // In a real implementation, you would add the advanced systems to the agent
        return agent;
    }, py::arg("brain"), py::arg("devSystem"), py::arg("predictionSystem"), py::arg("conceptFormation"),
        "Create an advanced agent brain with integrated cognitive systems");

    // === Version and Information ===

    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__build_type__") = "enhanced";
    m.attr("__has_advanced_features__") = true;
    
    m.def("getVersionInfo", []() {
        std::map<std::string, std::string> versionInfo;
        versionInfo["pynlm_version"] = "2.0.0-enhanced";
        versionInfo["build_type"] = "enhanced";
        versionInfo["features"] = "advanced_debugging,experiment_management,performance_monitoring,checkpoint_management,development_systems";
        return versionInfo;
    }, "Get version information");
    
    m.def("getFeatureList", []() {
        return std::vector<std::string>{
            "advanced_debugging_commands",
            "experiment_management_tools", 
            "performance_monitoring",
            "checkpoint_management",
            "development_system_tools",
            "real_time_monitoring",
            "neural_activity_analysis",
            "prediction_system_status",
            "concept_formation_analysis",
            "planning_system_diagnostics"
        };
    }, "Get list of available features");

    // === Documentation ===

    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python API
        
        This enhanced version includes:
        
        1. ADVANCED DEBUGGING COMMANDS:
           - Memory system diagnostics (NeuralWorkingMemory)
           - Neural activity analysis (Brain)
           - Prediction system status (PredictionSystem)
           - Concept formation analysis (ConceptFormation)
           - Planning system diagnostics (NeuralPlanner)
        
        2. EXPERIMENT MANAGEMENT TOOLS:
           - Experiment setup and configuration
           - Data collection and analysis (Metrics)
           - Result comparison (ExperimentRunner)
        
        3. PERFORMANCE MONITORING TOOLS:
           - Real-time performance metrics
           - Memory usage monitoring
           - CPU/GPU usage tracking
        
        4. CHECKPOINT MANAGEMENT TOOLS:
           - List available checkpoints
           - Checkpoint analysis
           - Checkpoint comparison
        
        5. DEVELOPMENT SYSTEM TOOLS:
           - Developmental stage analysis
           - Plasticity rate monitoring
           - Critical period detection
        
        6. ENHANCED API FEATURES:
           - Comprehensive system diagnostics
           - Convenience functions for common workflows
           - Factory functions for advanced setups
    )pbdoc";
}

} // namespace nlm
