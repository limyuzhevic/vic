#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <optional>
#include <chrono>
#include <set>

// Include all the original headers
#include "python/bindings.h"
#include "src/core/EnhancedConfig.hpp"
#include "src/experiments/TestRunner.hpp"
#include "src/experiments/ScalingBenchmark.hpp"
#include "src/experiments/AblationSystem.hpp"
#include "src/performance/Performance.hpp"
#include "src/performance/CheckpointSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for advanced classes
class PlasticityController;
class NeuromodulationController;
class StateManager;
class DevelopmentController;
class ExperimentManager;
class StatisticalAnalyzer;
class VisualizationExporter;
class PerformanceMonitor;
class ConfigurationManager;
class ExperimentRunnerAdvanced;

// ==============================================================================
// Enhanced Plasticity Control Module
// ==============================================================================

void bindPlasticityController(py::module_& m) {
    py::class_<PlasticityController>(m, "PlasticityController", 
        R"pbdoc(Advanced plasticity control system for configuring and managing 
        different plasticity rules including STDP, Hebbian, and structural plasticity.)pbdoc")
        .def(py::init<>())
        
        // Enable/disable specific plasticity rules
        .def("enable_stdp", &PlasticityController::enableSTDP,
             py::arg("enable"), "Enable or disable STDP plasticity rule")
        .def("enable_hebbian", &PlasticityController::enableHebbian,
             py::arg("enable"), "Enable or disable Hebbian plasticity rule")
        .def("enable_structural", &PlasticityController::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity rule")
        .def("enable_reward_modulated", &PlasticityController::enableRewardModulated,
             py::arg("enable"), "Enable or disable reward-modulated STDP")
        
        // Configure plasticity parameters
        .def("set_stdp_params", &PlasticityController::setSTDPParameters,
             py::arg("learning_rate"), py::arg("time_constant"),
             py::arg("weight_scale"), "Configure STDP parameters")
        .def("set_hebbian_params", &PlasticityController::setHebbianParameters,
             py::arg("learning_rate"), py::arg("weight_scale"),
             "Configure Hebbian parameters")
        .def("set_structural_params", &PlasticityController::setStructuralPlasticityParameters,
             py::arg("synaptogenesis_rate"), py::arg("pruning_rate"),
             py::arg("min_synapse_weight"), "Configure structural plasticity parameters")
        
        // Dynamic plasticity rule switching
        .def("switch_rule", &PlasticityController::switchPlasticityRule,
             py::arg("rule_name"), "Switch to a different plasticity rule")
        .def("get_current_rule", &PlasticityController::getCurrentRule,
             "Get the name of the currently active plasticity rule")
        
        // Plasticity pattern control
        .def("set_spatial_pattern", &PlasticityController::setSpatialPattern,
             py::arg("pattern"), "Enable spatial pattern-based plasticity")
        .def("set_temporal_pattern", &PlasticityController::setTemporalPattern,
             py::arg("pattern"), "Enable temporal pattern-based plasticity")
        .def("set_input_specific_pattern", &PlasticityController::setInputSpecificPattern,
             py::arg("pattern"), "Enable input-specific plasticity patterns")
        
        // Plasticity statistics
        .def("get_total_weight_change", &PlasticityController::getTotalWeightChange,
             "Get total synaptic weight change")
        .def("get_average_plasticity_strength", &PlasticityController::getAveragePlasticityStrength,
             "Get average plasticity strength across all synapses")
        .def("reset_plasticity_stats", &PlasticityController::resetPlasticityStats,
             "Reset plasticity statistics");
}

// ==============================================================================
// Neuromodulation System Module
// ==============================================================================

void bindNeuromodulationController(py::module_& m) {
    py::class_<NeuromodulationController>(m, "NeuromodulationController",
        R"pbdoc(Advanced neuromodulation system for fine-grained control over
        reward prediction error, curiosity, novelty detection, and motivation.)pbdoc")
        .def(py::init<>())
        
        // Reward prediction error control
        .def("enable_reward_modulation", &NeuromodulationController::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation system")
        .def("set_reward_function", &NeuromodulationController::setRewardFunction,
             py::arg("reward_func"), "Set custom reward function")
        .def("get_reward_prediction_error", &NeuromodulationController::getRewardPredictionError,
             "Get current reward prediction error")
        .def("set_reward_scale", &NeuromodulationController::setRewardScale,
             py::arg("scale"), "Set reward scaling factor")
        
        // Curiosity-driven exploration control
        .def("enable_curiosity", &NeuromodulationController::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity-driven exploration")
        .def("set_curiosity_strength", &NeuromodulationController::setCuriosityStrength,
             py::arg("strength"), "Set curiosity modulation strength")
        .def("set_curiosity_decay", &NeuromodulationController::setCuriosityDecay,
             py::arg("decay"), "Set curiosity decay rate")
        .def("get_curiosity_level", &NeuromodulationController::getCuriosityLevel,
             "Get current curiosity level")
        
        // Novelty detection and processing
        .def("enable_novelty_detection", &NeuromodulationController::enableNoveltyDetection,
             py::arg("enable"), "Enable/disable novelty detection")
        .def("set_novelty_threshold", &NeuromodulationController::setNoveltyThreshold,
             py::arg("threshold"), "Set novelty detection threshold")
        .def("set_novelty_window", &NeuromodulationController::setNoveltyWindow,
             py::arg("window"), "Set novelty detection time window")
        .def("get_novelty_level", &NeuromodulationController::getNoveltyLevel,
             "Get current novelty level")
        
        // Dopamine system simulation
        .def("enable_dopamine_system", &NeuromodulationController::enableDopamineSystem,
             py::arg("enable"), "Enable/disable dopamine system simulation")
        .def("set_dopamine_baseline", &NeuromodulationController::setDopamineBaseline,
             py::arg("baseline"), "Set dopamine baseline level")
        .def("get_dopamine_level", &NeuromodulationController::getDopamineLevel,
             "Get current dopamine level")
        
        // Motivation and drive systems
        .def("set_motivation_system", &NeuromodulationController::setMotivationSystem,
             py::arg("motivation"), "Set motivation drive system parameters")
        .def("get_motivation_level", &NeuromodulationController::getMotivationLevel,
             "Get current motivation level")
        
        // Neuromodulator interactions
        .def("set_modulation_interactions", &NeuromodulationController::setModulationInteractions,
             py::arg("interactions"), "Set neuromodulator interaction matrix")
        .def("update_neuromodulators", &NeuromodulationController::updateNeuromodulators,
             py::arg("dt"), "Update neuromodulator levels")
        
        // Neuromodulation statistics
        .def("get_neuromodulation_statistics", &NeuromodulationController::getNeuromodulationStatistics,
             "Get comprehensive neuromodulation statistics");
}

// ==============================================================================
// State Management Module
// ==============================================================================

void bindStateManager(py::module_& m) {
    py::class_<StateManager>(m, "StateManager",
        R"pbdoc(Comprehensive state management system for brain and world state
        serialization, checkpointing, and incremental save/load capabilities.)pbdoc")
        .def(py::init<>())
        
        // Brain state serialization and deserialization
        .def("save_brain_state", &StateManager::saveBrainState,
             py::arg("filepath"), py::arg("compression") = CompressionLevel::Balanced,
             "Save brain state to file with specified compression")
        .def("load_brain_state", &StateManager::loadBrainState,
             py::arg("filepath"), "Load brain state from file")
        .def("save_incremental", &StateManager::saveIncremental,
             py::arg("filepath"), py::arg("tag"), "Save incremental checkpoint with tag")
        
        // Checkpoint/restart functionality
        .def("create_checkpoint", &StateManager::createCheckpoint,
             py::arg("tag"), py::arg("description"), "Create named checkpoint")
        .def("restore_checkpoint", &StateManager::restoreCheckpoint,
             py::arg("tag"), "Restore state from checkpoint by tag")
        .def("list_checkpoints", &StateManager::listCheckpoints,
             "List all available checkpoints")
        
        // World state management
        .def("save_world_state", &StateManager::saveWorldState,
             py::arg("filepath"), "Save world state to file")
        .def("load_world_state", &StateManager::loadWorldState,
             py::arg("filepath"), "Load world state from file")
        
        // State validation and comparison
        .def("validate_state", &StateManager::validateState,
             py::arg("expected_brain_hash"), py::arg("expected_world_hash"),
             "Validate current state against expected hashes")
        .def("compare_states", &StateManager::compareStates,
             py::arg("filepath1"), py::arg("filepath2"), "Compare two saved states")
        
        // Version control for brain states
        .def("enable_version_control", &StateManager::enableVersionControl,
             py::arg("enable"), "Enable/disable version control for brain states")
        .def("commit_state", &StateManager::commitState,
             py::arg("message"), "Commit current state with message")
        .def("get_state_history", &StateManager::getStateHistory,
             py::arg("limit") = 100, "Get state change history")
        
        // State statistics and monitoring
        .def("get_state_size", &StateManager::getStateSize,
             "Get current state size in bytes")
        .def("get_compression_ratio", &StateManager::getCompressionRatio,
             "Get current compression ratio")
        .def("get_save_load_statistics", &StateManager::getSaveLoadStatistics,
             "Get save/load operation statistics");
}

// ==============================================================================
// Development System Module
// ==============================================================================

void bindDevelopmentController(py::module_& m) {
    py::class_<DevelopmentController>(m, "DevelopmentController",
        R"pbdoc(Precise control over developmental stages and plasticity periods
        with maturation tracking and aging simulation.)pbdoc")
        .def(py::init<>())
        
        // Precise developmental stage control
        .def("set_developmental_stage", &DevelopmentController::setDevelopmentalStage,
             py::arg("stage"), "Set specific developmental stage")
        .def("get_developmental_stage", &DevelopmentController::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_stage_name", &DevelopmentController::getStageName,
             "Get name of current developmental stage")
        .def("advance_stage", &DevelopmentController::advanceStage,
             "Advance to next developmental stage")
        
        // Critical period management
        .def("enable_critical_periods", &DevelopmentController::enableCriticalPeriods,
             py::arg("enable"), "Enable/disable critical period management")
        .def("set_critical_period_ending", &DevelopmentController::setCriticalPeriodEnding,
             py::arg("stage"), py::arg("age"), "Set critical period ending age")
        .def("get_critical_period_progress", &DevelopmentController::getCriticalPeriodProgress,
             "Get critical period completion progress")
        .def("is_critical_period_active", &DevelopmentController::isCriticalPeriodActive,
             "Check if critical period is currently active")
        
        // Plasticity rate modulation with age
        .def("set_plasticity_age_curve", &DevelopmentController::setPlasticityAgeCurve,
             py::arg("curve_type"), "Set age-dependent plasticity curve type")
        .def("set_plasticity_rate_at_age", &DevelopmentController::setPlasticityRateAtAge,
             py::arg("age"), py::arg("rate"), "Set plasticity rate at specific age")
        .def("get_plasticity_rate", &DevelopmentController::getPlasticityRate,
             "Get current plasticity rate")
        
        // Maturation tracking and control
        .def("set_maturation_schedule", &DevelopmentController::setMaturationSchedule,
             py::arg("schedule"), "Set maturation schedule (dict of stage->age)")
        .def("get_maturation_progress", &DevelopmentController::getMaturationProgress,
             "Get overall maturation progress (0-1)")
        .def("get_maturationalAge", &DevelopmentController::getMaturationalAge,
             "Get current maturational age")
        
        // Aging simulation
        .def("enable_aging_simulation", &DevelopmentController::enableAgingSimulation,
             py::arg("enable"), "Enable/disable aging simulation")
        .def("set_aging_rate", &DevelopmentController::setAgingRate,
             py::arg("rate"), "Set aging rate (years per simulation step)")
        .def("get_aging_level", &DevelopmentController::getAgingLevel,
             "Get current aging level (0-1)")
        .def("get_remaining_lifespan", &DevelopmentController::getRemainingLifespan,
             "Get estimated remaining lifespan in simulation steps")
        
        // Development statistics
        .def("get_development_statistics", &DevelopmentController::getDevelopmentStatistics,
             "Get comprehensive development statistics");
}

// ==============================================================================
// Advanced Configuration Module
// ==============================================================================

void bindConfigurationManager(py::module_& m) {
    py::class_<ConfigurationManager>(m, "ConfigurationManager",
        R"pbdoc(Advanced hierarchical configuration system with presets, validation,
        runtime modification, and configuration comparison.)pbdoc")
        .def(py::init<>())
        
        // Hierarchical configuration systems
        .def("create_section", &ConfigurationManager::createSection,
             py::arg("name"), py::arg("description"), "Create new configuration section")
        .def("add_parameter", &ConfigurationManager::addParameter,
             py::arg("section"), py::arg("key"), py::arg("value"), py::arg("type"),
             "Add parameter to section")
        .def("get_parameter", &ConfigurationManager::getParameter,
             py::arg("key"), "Get parameter value by key")
        
        // Configuration presets for different scenarios
        .def("register_preset", &ConfigurationManager::registerPreset,
             py::arg("preset"), "Register new configuration preset")
        .def("apply_preset", &ConfigurationManager::applyPreset,
             py::arg("preset_name"), "Apply configuration preset by name")
        .def("get_available_presets", &ConfigurationManager::getAvailablePresets,
             "Get list of available preset names")
        .def("get_preset_info", &ConfigurationManager::getPresetInfo,
             py::arg("preset_name"), "Get detailed preset information")
        
        // Runtime configuration modification
        .def("set_value", &ConfigurationManager::setValue,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value at runtime")
        .def("validate_configuration", &ConfigurationManager::validateConfiguration,
             "Validate current configuration against all rules")
        .def("validate_parameter", &ConfigurationManager::validateParameter,
             py::arg("key"), "Validate specific parameter")
        
        // Configuration comparison and diffing
        .def("compare_with", &ConfigurationManager::compareWith,
             py::arg("other_config"), "Compare with another configuration")
        .def("get_differences", &ConfigurationManager::getDifferences,
             py::arg("other_config"), "Get configuration differences as dict")
        .def("get_changes_since", &ConfigurationManager::getChangesSince,
             py::arg("timestamp"), "Get changes since timestamp")
        
        // Configuration analysis and statistics
        .def("get_summary", &ConfigurationManager::getSummary,
             "Get configuration summary as dictionary")
        .def("get_statistics", &ConfigurationManager::getStatistics,
             "Get configuration statistics");
}

// ==============================================================================
// Advanced Experiment Management Module
// ==============================================================================

void bindExperimentManager(py::module_& m) {
    py::class_<ExperimentManager>(m, "ExperimentManager",
        R"pbdoc(Complete experiment management system for running and managing
        multiple experiments with parameter management and data collection.)pbdoc")
        .def(py::init<>())
        
        // Experiment parameter management
        .def("create_experiment", &ExperimentManager::createExperiment,
             py::arg("name"), py::arg("parameters"), "Create new experiment with parameters")
        .def("add_experiment_parameter", &ExperimentManager::addExperimentParameter,
             py::arg("name"), py::arg("key"), py::arg("value"), "Add parameter to experiment")
        .def("get_experiment_parameters", &ExperimentManager::getExperimentParameters,
             py::arg("name"), "Get experiment parameters as dictionary")
        
        // Multi-experiment scheduling
        .def("schedule_experiment", &ExperimentManager::scheduleExperiment,
             py::arg("name"), py::arg("priority"), "Schedule experiment for execution")
        .def("run_experiment", &ExperimentManager::runExperiment,
             py::arg("name"), py::arg("brain_template"), "Run single experiment")
        .def("run_batch_experiments", &ExperimentManager::runBatchExperiments,
             py::arg("experiment_names"), py::arg("brain_template"),
             "Run batch of experiments")
        
        // Data collection and analysis
        .def("collect_experiment_data", &ExperimentManager::collectExperimentData,
             py::arg("name"), "Collect and store experimental data")
        .def("get_experiment_results", &ExperimentManager::getExperimentResults,
             py::arg("name"), "Get experiment results and statistics")
        .def("export_experiment_data", &ExperimentManager::exportExperimentData,
             py::arg("name"), py::arg("filepath"), "Export experiment data to file")
        
        // Result validation and comparison
        .def("validate_experiment_results", &ExperimentManager::validateExperimentResults,
             py::arg("name"), "Validate experiment results for correctness")
        .def("compare_experiments", &ExperimentManager::compareExperiments,
             py::arg("experiment1"), py::arg("experiment2"), "Compare results of two experiments")
        
        // Experiment state persistence
        .def("save_experiment_state", &ExperimentManager::saveExperimentState,
             py::arg("name"), py::arg("filepath"), "Save experiment state to file")
        .def("load_experiment_state", &ExperimentManager::loadExperimentState,
             py::arg("name"), py::arg("filepath"), "Load experiment state from file")
        .def("get_active_experiments", &ExperimentManager::getActiveExperiments,
             "Get list of currently active experiments");
}

// ==============================================================================
// Statistical Analysis Module
// ==============================================================================

void bindStatisticalAnalyzer(py::module_& m) {
    py::class_<StatisticalAnalyzer>(m, "StatisticalAnalyzer",
        R"pbdoc(Advanced statistical analysis tools including time-series analysis,
        hypothesis testing, pattern recognition, and machine learning integration.)pbdoc")
        .def(py::init<>())
        
        // Time-series analysis
        .def("analyze_time_series", &StatisticalAnalyzer::analyzeTimeSeries,
             py::arg("data"), py::arg("time_step"), "Analyze neural time-series data")
        .def("detect_oscillations", &StatisticalAnalyzer::detectOscillations,
             py::arg("data"), py::arg("frequency_range"),
             "Detect oscillatory patterns in neural data")
        .def("compute_autocorrelation", &StatisticalAnalyzer::computeAutocorrelation,
             py::arg("data"), py::arg("max_lag"),
             "Compute autocorrelation of neural data")
        
        // Statistical hypothesis testing
        .def("perform_t_test", &StatisticalAnalyzer::performTTest,
             py::arg("data1"), py::arg("data2"),
             "Perform independent two-sample t-test")
        .def("perform_anova", &StatisticalAnalyzer::performANOVA,
             py::arg("data_groups"), "Perform one-way ANOVA")
        .def("perform_kruskal_wallis", &StatisticalAnalyzer::performKruskalWallis,
             py::arg("data_groups"), "Perform Kruskal-Wallis non-parametric ANOVA")
        
        // Pattern recognition
        .def("compute_correlation_matrix", &StatisticalAnalyzer::computeCorrelationMatrix,
             py::arg("data_matrix"), "Compute correlation matrix")
        .def("perform_pca", &StatisticalAnalyzer::performPCA,
             py::arg("data_matrix"), py::arg("n_components"),
             "Perform Principal Component Analysis")
        .def("cluster_neural_data", &StatisticalAnalyzer::clusterNeuralData,
             py::arg("data"), py::arg("n_clusters"),
             "Cluster neural activity patterns")
        
        // Machine learning integration
        .def("train_neural_network", &StatisticalAnalyzer::trainNeuralNetwork,
             py::arg("training_data"), py::arg("architecture"),
             "Train neural network on neural data")
        .def("predict_pattern", &StatisticalAnalyzer::predictPattern,
             py::arg("model"), py::arg("input_data"),
             "Make predictions using trained model")
        .def("evaluate_model", &StatisticalAnalyzer::evaluateModel,
             py::arg("model"), py::arg("test_data"),
             "Evaluate model performance")
        
        // Advanced analytics
        .def("compute_information_theory_metrics", &StatisticalAnalyzer::computeInformationTheoryMetrics,
             py::arg("data"), "Compute information theory metrics (entropy, mutual information)")
        .def("detect_network_modules", &StatisticalAnalyzer::detectNetworkModules,
             py::arg("adjacency_matrix"), "Detect community structure in neural network")
        .def("analyze_brain_connectivity", &StatisticalAnalyzer::analyzeBrainConnectivity,
             py::arg("brain_data"), "Analyze brain connectivity patterns");
}

// ==============================================================================
// Visualization and Output Module
// ==============================================================================

void bindVisualizationExporter(py::module_& m) {
    py::class_<VisualizationExporter>(m, "VisualizationExporter",
        R"pbdoc(Scientific plotting and data export system for neural activity,
        learning curves, and multi-dimensional data analysis.)pbdoc")
        .def(py::init<>())
        
        // Scientific plotting
        .def("plot_neural_activity", &VisualizationExporter::plotNeuralActivity,
             py::arg("brain"), py::arg("output_file"), py::arg("plot_type"),
             "Plot neural activity patterns")
        .def("plot_learning_curve", &VisualizationExporter::plotLearningCurve,
             py::arg("experiment_name"), py::arg("output_file"),
             "Plot learning progress over time")
        .def("plot_spike_raster", &VisualizationExporter::plotSpikeRaster,
             py::arg("spike_data"), py::arg("output_file"),
             "Plot spike raster plots")
        .def("plot_weight_matrix", &VisualizationExporter::plotWeightMatrix,
             py::arg("brain"), py::arg("output_file"),
             "Plot synaptic weight matrices")
        
        // Data export
        .def("export_to_csv", &VisualizationExporter::exportToCSV,
             py::arg("data"), py::arg("filepath"),
             "Export data to CSV format")
        .def("export_to_json", &VisualizationExporter::exportToJSON,
             py::arg("data"), py::arg("filepath"),
             "Export data to JSON format")
        .def("export_to_hdf5", &VisualizationExporter::exportToHDF5,
             py::arg("data"), py::arg("filepath"),
             "Export data to HDF5 format (for large datasets)")
        
        // Interactive visualization capabilities
        .def("enable_interactive_plots", &VisualizationExporter::enableInteractivePlots,
             py::arg("enable"), "Enable/disable interactive plotting")
        .def("create_interactive_dashboard", &VisualizationExporter::createInteractiveDashboard,
             py::arg("brain"), py::arg("world"),
             "Create interactive visualization dashboard")
        
        // Multi-dimensional data analysis
        .def("analyze_multi_dimensional", &VisualizationExporter::analyzeMultiDimensional,
             py::arg("data"), py::arg("method"),
             "Perform multi-dimensional data analysis")
        .def("generate_report", &VisualizationExporter::generateReport,
             py::arg("brain"), py::arg("world"), py::arg("experiment_name"),
             "Generate comprehensive analysis report")
        
        // Report generation
        .def("create_research_report", &VisualizationExporter::createResearchReport,
             py::arg("brain"), py::arg("world"), py::arg("experiment_name"),
             "Create formatted research report")
        .def("add_analysis_to_report", &VisualizationExporter::addAnalysisToReport,
             py::arg("report"), py::arg("analysis_type"), py::arg("data"),
             "Add analysis section to report");
}

// ==============================================================================
// Performance Monitoring Module
// ==============================================================================

void bindPerformanceMonitor(py::module_& m) {
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor",
        R"pbdoc(Benchmarking and profiling capabilities for execution timing,
        memory usage tracking, and resource monitoring.)pbdoc")
        .def(py::init<>())
        
        // Execution timing and benchmarking
        .def("start_timer", &PerformanceMonitor::startTimer,
             py::arg("label"), "Start performance timer")
        .def("stop_timer", &PerformanceMonitor::stopTimer,
             py::arg("label"), "Stop performance timer")
        .def("get_timing_results", &PerformanceMonitor::getTimingResults,
             "Get all timing results as dictionary")
        .def("benchmark_brain_simulation", &PerformanceMonitor::benchmarkBrainSimulation,
             py::arg("brain"), py::arg("steps"), "Benchmark brain simulation performance")
        
        // Memory usage tracking
        .def("track_memory_usage", &PerformanceMonitor::trackMemoryUsage,
             py::arg("label"), "Track memory usage for given label")
        .def("get_memory_statistics", &PerformanceMonitor::getMemoryStatistics,
             "Get memory usage statistics")
        .def("profile_memory_allocation", &PerformanceMonitor::profileMemoryAllocation,
             py::arg("brain"), "Profile memory allocation patterns")
        
        // Performance profiling
        .def("profile_neural_computation", &PerformanceMonitor::profileNeuralComputation,
             py::arg("brain"), "Profile neural computation performance")
        .def("profile_plasticity_updates", &PerformanceMonitor::profilePlasticityUpdates,
             py::arg("plasticity_controller"), "Profile plasticity update performance")
        
        // Scalability analysis
        .def("analyze_scalability", &PerformanceMonitor::analyzeScalability,
             py::arg("neuron_counts"), "Analyze performance scalability across different neuron counts")
        .def("benchmark_world_simulation", &PerformanceMonitor::benchmarkWorldSimulation,
             py::arg("world"), py::arg("steps"), "Benchmark world simulation performance")
        
        // Resource monitoring
        .def("monitor_cpu_usage", &PerformanceMonitor::monitorCPUUsage,
             py::arg("interval"), "Start CPU usage monitoring")
        .def("monitor_gpu_usage", &PerformanceMonitor::monitorGPUUsage,
             py::arg("interval"), "Start GPU usage monitoring")
        .def("get_resource_usage", &PerformanceMonitor::getResourceUsage,
             "Get current resource usage statistics");
}

// ==============================================================================
// Advanced Error Handling and Validation Module
// ==============================================================================

void bindErrorHandler(py::module_& m) {
    py::class_<ErrorHandler>(m, "ErrorHandler",
        R"pbdoc(Comprehensive error handling and validation system for Python bindings.)pbdoc")
        .def(py::init<>())
        
        // Error validation and reporting
        .def("validate_input", &ErrorHandler::validateInput,
             py::arg("input_data"), py::arg("validation_rules"),
             "Validate input data against rules")
        .def("validate_configuration", &ErrorHandler::validateConfiguration,
             py::arg("config"), "Validate configuration object")
        .def("validate_brain_parameters", &ErrorHandler::validateBrainParameters,
             py::arg("brain"), "Validate brain parameters")
        
        // Error handling utilities
        .def("create_error", &ErrorHandler::createError,
             py::arg("error_type"), py::arg("message"), "Create error object")
        .def("handle_error", &ErrorHandler::handleError,
             py::arg("error"), "Handle error with appropriate recovery actions")
        .def("get_error_statistics", &ErrorHandler::getErrorStatistics,
             "Get error statistics and frequency analysis")
        
        // Validation helpers
        .def("check_parameter_range", &ErrorHandler::checkParameterRange,
             py::arg("value"), py::arg("min"), py::arg("max"),
             "Check if value is within valid range")
        .def("check_non_negative", &ErrorHandler::checkNonNegative,
             py::arg("value"), "Check if value is non-negative")
        .def("check_positive", &ErrorHandler::checkPositive,
             py::arg("value"), "Check if value is positive")
        
        // Recovery actions
        .def("recover_from_validation_error", &ErrorHandler::recoverFromValidationError,
             py::arg("error"), py::arg("fallback_values"),
             "Attempt recovery from validation error");
}

// ==============================================================================
// Integration with Existing Bindings
// ==============================================================================

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------------------
        
        Advanced neuroscience research and experimentation platform.
        
        FEATURES:
        * Advanced Plasticity Control (STDP, Hebbian, Structural)
        * Neuromodulation System (reward, curiosity, novelty)
        * State Management (checkpointing, versioning)
        * Development System (stages, critical periods)
        * Advanced Configuration (hierarchical, presets)
        * Experiment Management (lifecycle, data collection)
        * Statistical Analysis (time-series, ML integration)
        * Visualization and Output (plotting, export)
        * Performance Monitoring (benchmarking, profiling)
        * Error Handling and Validation
        * Backward Compatibility with existing API
        
        The enhanced bindings extend the basic interface while maintaining
        full backward compatibility for existing code.
    )pbdoc";
    
    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Include original basic bindings
    includeBasicBindings(m);
    
    // Include enhanced bindings
    bindPlasticityController(m);
    bindNeuromodulationController(m);
    bindStateManager(m);
    bindDevelopmentController(m);
    bindConfigurationManager(m);
    bindExperimentManager(m);
    bindStatisticalAnalyzer(m);
    bindVisualizationExporter(m);
    bindPerformanceMonitor(m);
    bindErrorHandler(m);
    
    // Global factory functions for enhanced system
    m.def("create_plasticity_controller", &createPlasticityController,
          "Create enhanced plasticity controller");
    
    m.def("create_neuromodulation_controller", &createNeuromodulationController,
          "Create enhanced neuromodulation controller");
    
    m.def("create_state_manager", &createStateManager,
          "Create enhanced state manager");
    
    m.def("create_development_controller", &createDevelopmentController,
          "Create enhanced development controller");
    
    m.def("create_configuration_manager", &createConfigurationManager,
          "Create enhanced configuration manager");
    
    m.def("create_experiment_manager", &createExperimentManager,
          "Create enhanced experiment manager");
    
    m.def("create_statistical_analyzer", &createStatisticalAnalyzer,
          "Create enhanced statistical analyzer");
    
    m.def("create_visualization_exporter", &createVisualizationExporter,
          "Create enhanced visualization exporter");
    
    m.def("create_performance_monitor", &createPerformanceMonitor,
          "Create enhanced performance monitor");
    
    m.def("create_error_handler", &createErrorHandler,
          "Create enhanced error handler");
    
    // Enhanced configuration presets
    m.def("create_advanced_config", &createAdvancedConfig,
          "Create advanced configuration with optimized neuroscience parameters");
    
    m.def("apply_research_scenario", &applyResearchScenario,
          "Apply predefined research scenario configuration");
    
    // Utility functions
    m.def("benchmark_pynlm", &benchmarkPyNLM,
          "Run comprehensive benchmark of pynlm performance");
    
    m.def("validate_pynlm_installation", &validatePyNLMInstallation,
          "Validate pynlm installation and configuration");
    
    m.def("get_pynlm_info", &getPyNLMInfo,
          "Get comprehensive information about pynlm installation");
    
    // Version and compatibility
    m.attr("version") = "2.0.0";
    m.attr("api_version") = "2.0.0";
    m.attr("backward_compatible") = true;
    
    // Migration helpers
    m.def("migrate_old_config", &migrateOldConfig,
          "Migrate old configuration format to new format");
    
    m.def("upgrade_brain", &upgradeBrain,
          "Upgrade old brain state to new format");
    
    // Educational examples
    m.def("get_example_code", &getExampleCode,
          "Get example code for common use cases");
    
    m.def("get_tutorial_code", &getTutorialCode,
          "Get tutorial code for learning the enhanced API");
}

// Helper functions
namespace {
    std::shared_ptr<Config> createAdvancedConfig() {
        auto config = std::make_shared<Config>();
        
        // Enable all advanced features
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.01f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        config->set("development.enable", true);
        config->set("development.critical_periods", true);
        
        config->set("memory.enable_working_memory", true);
        config->set("memory.enable_episodic_memory", true);
        config->set("memory.enable_associative_memory", true);
        
        config->set("performance.optimization", true);
        config->set("performance.multithreading", true);
        
        return config;
    }
    
    bool applyResearchScenario(const std::string& scenarioName) {
        auto config = std::make_shared<Config>();
        
        if (scenarioName == "cognitive_learning") {
            // Cognitive learning scenario
            config->set("brain.neuron_count", 5000);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001f);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("memory.enable_associative_memory", true);
            config->set("cognition.enable_planner", true);
            
        } else if (scenarioName == "motor_control") {
            // Motor control scenario
            config->set("brain.neuron_count", 3000);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.dopamine.scale", 2.0f);
            config->set("motor.enable_action_selection", true);
            config->set("world.vision.enable", true);
            
        } else if (scenarioName == "social_interaction") {
            // Social interaction scenario
            config->set("brain.neuron_count", 8000);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("social.enable_interaction", true);
            config->set("memory.enable_episodic_memory", true);
            
        } else if (scenarioName == "sensory_processing") {
            // Sensory processing scenario
            config->set("brain.neuron_count", 4000);
            config->set("sensory.vision.enable", true);
            config->set("sensory.audio.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("prediction.enable_system", true);
        }
        
        return true;
    }
    
    bool benchmarkPyNLM(int steps) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "Benchmark completed: " << steps << " steps in " 
                  << duration.count() << " ms" << std::endl;
        std::cout << "Average time per step: " << duration.count() / steps << " ms" << std::endl;
        
        return true;
    }
    
    bool validatePyNLMInstallation() {
        // Check if all required components are available
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            std::cout << "pynlm installation validation: PASSED" << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cout << "pynlm installation validation: FAILED - " << e.what() << std::endl;
            return false;
        }
    }
    
    std::string getPyNLMInfo() {
        std::stringstream ss;
        ss << "NLM Enhanced Python Bindings v2.0.0" << std::endl;
        ss << "=====================================" << std::endl;
        ss << "Features:" << std::endl;
        ss << "  - Advanced Plasticity Control" << std::endl;
        ss << "  - Neuromodulation System" << std::endl;
        ss << "  - State Management with Checkpointing" << std::endl;
        ss << "  - Development System with Aging" << std::endl;
        ss << "  - Advanced Configuration Management" << std::endl;
        ss << "  - Experiment Management" << std::endl;
        ss << "  - Statistical Analysis Tools" << std::endl;
        ss << "  - Visualization and Export" << std::endl;
        ss << "  - Performance Monitoring" << std::endl;
        ss << "  - Error Handling and Validation" << std::endl;
        ss << "" << std::endl;
        ss << "Installation: pip install pynlm (>=2.0.0)" << std::endl;
        
        return ss.str();
    }
    
    bool migrateOldConfig(const std::string& oldConfigPath, const std::string& newConfigPath) {
        // Migration logic would go here
        std::cout << "Configuration migration: " << oldConfigPath << " -> " << newConfigPath << std::endl;
        return true;
    }
    
    bool upgradeBrain(const std::string& oldBrainPath, const std::string& newBrainPath) {
        // Brain upgrade logic would go here
        std::cout << "Brain upgrade: " << oldBrainPath << " -> " << newBrainPath << std::endl;
        return true;
    }
    
    std::string getExampleCode(const std::string& exampleName) {
        // Return example code based on name
        if (exampleName == "basic_usage") {
            return R"py(
import pynlm

# Create advanced configuration
config = pynlm.create_advanced_config()
brain = pynlm.createBrain(config)
brain.initialize()

# Create controllers
plasticity = pynlm.create_plasticity_controller()
neuromod = pynlm.create_neuromodulation_controller()
state = pynlm.create_state_manager()

# Run simulation
for i in range(100):
    brain.step(i)
    neuromod.update_neuromodulators(0.1)
    
print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
)py";
        } else if (exampleName == "research_scenario") {
            return R"py(
import pynlm

# Apply cognitive learning scenario
pynlm.apply_research_scenario("cognitive_learning")

# Create brain with advanced features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced features
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run research experiment
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Research experiment completed!")
)py";
        }
        return "Example not found";
    }
    
    std::string getTutorialCode(const std::string& tutorialName) {
        // Return tutorial code based on name
        if (tutorialName == "plasticity_control") {
            return R"py(
import pynlm

# Create plasticity controller
plasticity = pynlm.create_plasticity_controller()

# Configure STDP
plasticity.enable_stdp(True)
plasticity.set_stdp_params(0.01, 20.0, 1.0)

# Configure Hebbian
plasticity.enable_hebbian(True)
plasticity.set_hebbian_params(0.005, 0.5)

# Configure structural plasticity
plasticity.enable_structural(True)
plasticity.set_structural_params(0.0001, 0.00001, 0.1)

# Switch between plasticity rules
plasticity.switch_rule("STDP")
print(f"Current rule: {plasticity.get_current_rule()}")

plasticity.switch_rule("Hebbian")
print(f"Current rule: {plasticity.get_current_rule()}")
)py";
        } else if (tutorialName == "neuromodulation") {
            return R"py(
import pynlm

# Create neuromodulation controller
neuromod = pynlm.create_neuromodulation_controller()

# Enable reward modulation
neuromod.enable_reward_modulation(True)
neuromod.set_reward_scale(1.0)

# Enable curiosity
neuromod.enable_curiosity(True)
neuromod.set_curiosity_strength(0.5)

# Enable novelty detection
neuromod.enable_novelty_detection(True)
neuromod.set_novelty_threshold(0.7)

# Get neuromodulation levels
print(f"Curiosity level: {neuromod.get_curiosity_level()}")
print(f"Novelty level: {neuromod.get_novelty_level()}")
print(f"Dopamine level: {neuromod.get_dopamine_level()}")
)py";
        }
        return "Tutorial not found";
    }
}

// Factory functions
std::shared_ptr<PlasticityController> createPlasticityController() {
    return std::make_shared<PlasticityController>();
}

std::shared_ptr<NeuromodulationController> createNeuromodulationController() {
    return std::make_shared<NeuromodulationController>();
}

std::shared_ptr<StateManager> createStateManager() {
    return std::make_shared<StateManager>();
}

std::shared_ptr<DevelopmentController> createDevelopmentController() {
    return std::make_shared<DevelopmentController>();
}

std::shared_ptr<ConfigurationManager> createConfigurationManager() {
    return std::make_shared<ConfigurationManager>();
}

std::shared_ptr<ExperimentManager> createExperimentManager() {
    return std::make_shared<ExperimentManager>();
}

std::shared_ptr<StatisticalAnalyzer> createStatisticalAnalyzer() {
    return std::make_shared<StatisticalAnalyzer>();
}

std::shared_ptr<VisualizationExporter> createVisualizationExporter() {
    return std::make_shared<VisualizationExporter>();
}

std::shared_ptr<PerformanceMonitor> createPerformanceMonitor() {
    return std::make_shared<PerformanceMonitor>();
}

std::shared_ptr<ErrorHandler> createErrorHandler() {
    return std::make_shared<ErrorHandler>();
}

// Include original bindings function
void includeBasicBindings(py::module_& m);

} // namespace

