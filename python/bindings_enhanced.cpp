// NLM Python Bindings - Enhanced API
// Provides comprehensive Python bindings for NLM C++ library

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "api/nlm_api.h"

namespace py = pybind11;

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        -----------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        Provides comprehensive access to neural computation, analysis, and control.
    )pbdoc";

    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Configuration API
    py::class_<nlm::ConfigAPI>(m, "ConfigAPI")
        .def_static("create_config", &nlm::ConfigAPI::createConfig, 
                   py::arg("filename") = "",
                   "Create a new configuration object")
        .def_static("load_config", &nlm::ConfigAPI::loadConfig,
                   py::arg("config"), py::arg("filename"),
                   "Load configuration from file")
        .def_static("save_config", &nlm::ConfigAPI::saveConfig,
                   py::arg("config"), py::arg("filename"),
                   "Save configuration to file")
        .def_static("validate_config", &nlm::ConfigAPI::validateConfig,
                   py::arg("config"),
                   "Validate configuration schema")
        .def_static("export_config", &nlm::ConfigAPI::exportConfig,
                   py::arg("config"), py::arg("format") = "json",
                   "Export configuration to string")
        .def_static("import_config", &nlm::ConfigAPI::importConfig,
                   py::arg("config"), py::arg("data"), py::arg("format") = "json",
                   "Import configuration from string")
        .def_static("config_diff", &nlm::ConfigAPI::configDiff,
                   py::arg("config1"), py::arg("config2"),
                   "Get difference between two configurations")
        .def_static("get_config_summary", &nlm::ConfigAPI::getConfigSummary,
                   py::arg("config"),
                   "Get configuration summary as string");

    // Brain API
    py::class_<nlm::BrainAPI>(m, "BrainAPI")
        .def_static("create_brain", &nlm::BrainAPI::createBrain,
                   py::arg("config_file") = "",
                   "Create a new brain simulation")
        .def_static("create_brain_from_config", &nlm::BrainAPI::createBrain,
                   py::arg("config"),
                   "Create a new brain with existing config")
        .def_static("initialize_brain", &nlm::BrainAPI::initializeBrain,
                   py::arg("brain"),
                   "Initialize brain simulation")
        .def_static("run_step", &nlm::BrainAPI::runStep,
                   py::arg("brain"), py::arg("step"), py::arg("time"),
                   "Run a single simulation step")
        .def_static("run_simulation", &nlm::BrainAPI::runSimulation,
                   py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
                   "Run complete simulation")
        .def_static("save_brain_state", &nlm::BrainAPI::saveBrainState,
                   py::arg("brain"), py::arg("filename"),
                   "Save brain state to file")
        .def_static("load_brain_state", &nlm::BrainAPI::loadBrainState,
                   py::arg("brain"), py::arg("filename"),
                   "Load brain state from file")
        .def_static("log_brain_status", &nlm::BrainAPI::logBrainStatus,
                   py::arg("brain"),
                   "Log brain status information")
        .def_static("get_total_neuron_count", &nlm::BrainAPI::getTotalNeuronCount,
                   py::arg("brain"),
                   "Get total neuron count")
        .def_static("get_total_synapse_count", &nlm::BrainAPI::getTotalSynapseCount,
                   py::arg("brain"),
                   "Get total synapse count")
        .def_static("get_total_spike_count", &nlm::BrainAPI::getTotalSpikeCount,
                   py::arg("brain"),
                   "Get total spike count")
        .def_static("get_firing_neuron_count", &nlm::BrainAPI::getFiringNeuronCount,
                   py::arg("brain"),
                   "Get firing neuron count")
        .def_static("get_memory_usage", &nlm::BrainAPI::getMemoryUsage,
                   py::arg("brain"),
                   "Get memory usage in bytes")
        .def_static("get_memory_access_count", &nlm::BrainAPI::getMemoryAccessCount,
                   py::arg("brain"),
                   "Get memory access count");

    // Experiment API
    py::class_<nlm::ExperimentAPI>(m, "ExperimentAPI")
        .def_static("create_experiment", &nlm::ExperimentAPI::createExperiment,
                   py::arg("name"), py::arg("type"),
                   "Create a new experiment")
        .def_static("add_metric", &nlm::ExperimentAPI::addMetric,
                   py::arg("experiment"), py::arg("metric_name"), py::arg("metric_func"),
                   "Add metric to experiment")
        .def_static("set_experiment_parameters", &nlm::ExperimentAPI::setExperimentParameters,
                   py::arg("experiment"), py::arg("parameters"),
                   "Set experiment parameters")
        .def_static("run_experiment", &nlm::ExperimentAPI::runExperiment,
                   py::arg("experiment"), py::arg("brain"), py::arg("runner"), py::arg("steps"),
                   "Run a single experiment")
        .def_static("save_experiment_results", &nlm::ExperimentAPI::saveExperimentResults,
                   py::arg("experiment"), py::arg("filename"),
                   "Save experiment results")
        .def_static("load_experiment_results", &nlm::ExperimentAPI::loadExperimentResults,
                   py::arg("experiment"), py::arg("filename"),
                   "Load experiment results")
        .def_static("get_experiment_info", &nlm::ExperimentAPI::getExperimentInfo,
                   py::arg("experiment"),
                   "Get experiment information as string")
        .def_static("create_experiment_runner", &nlm::ExperimentAPI::createExperimentRunner,
                   "Create a new experiment runner")
        .def_static("run_experiment_suite", &nlm::ExperimentAPI::runExperimentSuite,
                   py::arg("runner"), py::arg("brain"), py::arg("experiments"), py::arg("steps"),
                   "Run multiple experiments in suite");

    // Analysis API
    py::class_<nlm::AnalysisAPI>(m, "AnalysisAPI")
        .def_static("calculate_statistics", &nlm::AnalysisAPI::calculateStatistics,
                   py::arg("data"),
                   "Calculate statistical measures")
        .def_static("calculate_mean", &nlm::AnalysisAPI::calculateMean,
                   py::arg("data"),
                   "Calculate mean of data")
        .def_static("calculate_median", &nlm::AnalysisAPI::calculateMedian,
                   py::arg("data"),
                   "Calculate median of data")
        .def_static("calculate_stddev", &nlm::AnalysisAPI::calculateStddev,
                   py::arg("data"),
                   "Calculate standard deviation")
        .def_static("calculate_correlation", &nlm::AnalysisAPI::calculateCorrelation,
                   py::arg("data1"), py::arg("data2"),
                   "Calculate correlation between two datasets")
        .def_static("detect_anomalies", &nlm::AnalysisAPI::detectAnomalies,
                   py::arg("data"), py::arg("threshold") = 3.0,
                   "Detect anomalies in data")
        .def_static("perform_clustering", &nlm::AnalysisAPI::performClustering,
                   py::arg("data"), py::arg("num_clusters"),
                   "Perform clustering analysis")
        .def_static("perform_pca", &nlm::AnalysisAPI::performPCA,
                   py::arg("data"), py::arg("num_components") = 2,
                   "Perform Principal Component Analysis")
        .def_static("smooth_data", &nlm::AnalysisAPI::smoothData,
                   py::arg("data"), py::arg("window_size") = 5,
                   "Smooth data using moving average")
        .def_static("differentiate_data", &nlm::AnalysisAPI::differentiateData,
                   py::arg("data"), py::arg("dt") = 0.001,
                   "Differentiate data numerically")
        .def_static("integrate_data", &nlm::AnalysisAPI::integrateData,
                   py::arg("data"), py::arg("dt") = 0.001,
                   "Integrate data numerically")
        .def_static("normalize_data", &nlm::AnalysisAPI::normalizeData,
                   py::arg("data"), py::arg("mean") = 0.0, py::arg("stddev") = 1.0,
                   "Normalize data to zero mean, unit variance");

    // Neural network operations API
    py::class_<nlm::NeuralAPI>(m, "NeuralAPI")
        .def_static("matrix_multiply", &nlm::NeuralAPI::matrixMultiply,
                   py::arg("A"), py::arg("B"),
                   "Multiply two matrices")
        .def_static("matrix_add", &nlm::NeuralAPI::matrixAdd,
                   py::arg("A"), py::arg("B"),
                   "Add two matrices")
        .def_static("matrix_transpose", &nlm::NeuralAPI::matrixTranspose,
                   py::arg("matrix"),
                   "Transpose a matrix")
        .def_static("matrix_determinant", &nlm::NeuralAPI::matrixDeterminant,
                   py::arg("matrix"),
                   "Calculate matrix determinant")
        .def_static("matrix_inverse", &nlm::NeuralAPI::matrixInverse,
                   py::arg("matrix"),
                   "Calculate matrix inverse")
        .def_static("dot_product", &nlm::NeuralAPI::dotProduct,
                   py::arg("a"), py::arg("b"),
                   "Calculate dot product of vectors")
        .def_static("vector_norm", &nlm::NeuralAPI::vectorNorm,
                   py::arg("vector"), py::arg("p") = 2.0,
                   "Calculate vector norm")
        .def_static("vector_add", &nlm::NeuralAPI::vectorAdd,
                   py::arg("a"), py::arg("b"),
                   "Add two vectors")
        .def_static("vector_subtract", &nlm::NeuralAPI::vectorSubtract,
                   py::arg("a"), py::arg("b"),
                   "Subtract one vector from another")
        .def_static("sigmoid", &nlm::NeuralAPI::sigmoid,
                   py::arg("x"),
                   "Apply sigmoid activation function")
        .def_static("relu", &nlm::NeuralAPI::relu,
                   py::arg("x"),
                   "Apply ReLU activation function")
        .def_static("tanh", &nlm::NeuralAPI::tanh,
                   py::arg("x"),
                   "Apply tanh activation function")
        .def_static("softmax", &nlm::NeuralAPI::softmax,
                   py::arg("x"),
                   "Apply softmax activation function")
        .def_static("convolution2d", &nlm::NeuralAPI::convolution2D,
                   py::arg("input"), py::arg("kernel"), py::arg("stride") = 1, py::arg("padding") = 0,
                   "Perform 2D convolution")
        .def_static("cross_entropy_loss", &nlm::NeuralAPI::crossEntropyLoss,
                   py::arg("predictions"), py::arg("targets"),
                   "Calculate cross-entropy loss")
        .def_static("mean_squared_error", &nlm::NeuralAPI::meanSquaredError,
                   py::arg("predictions"), py::arg("targets"),
                   "Calculate mean squared error");

    // Pipeline API
    py::class_<nlm::PipelineAPI>(m, "PipelineAPI")
        .def_static("add_pipeline_step", &nlm::PipelineAPI::addPipelineStep,
                   py::arg("pipeline"), py::arg("name"), py::arg("function"),
                   "Add a step to the pipeline")
        .def_static("execute_pipeline", &nlm::PipelineAPI::executePipeline,
                   py::arg("pipeline"), py::arg("config"), py::arg("brain"),
                   "Execute entire pipeline")
        .def_static("save_pipeline", &nlm::PipelineAPI::savePipeline,
                   py::arg("pipeline"), py::arg("filename"),
                   "Save pipeline to file")
        .def_static("load_pipeline", &nlm::PipelineAPI::loadPipeline,
                   py::arg("pipeline"), py::arg("filename"),
                   "Load pipeline from file")
        .def_static("print_pipeline", &nlm::PipelineAPI::printPipeline,
                   py::arg("pipeline"),
                   "Print pipeline steps");

    // Batch API
    py::class_<nlm::BatchAPI>(m, "BatchAPI")
        .def_static("create_batch_parameters", &nlm::BatchAPI::createBatchParameters,
                   py::arg("num_experiments") = 10,
                   "Create batch processing parameters")
        .def_static("add_experiment_to_batch", &nlm::BatchAPI::addExperimentToBatch,
                   py::arg("params"), py::arg("type"), py::arg("steps"),
                   "Add experiment to batch")
        .def_static("execute_batch", &nlm::BatchAPI::executeBatch,
                   py::arg("params"),
                   "Execute batch of experiments")
        .def_static("aggregate_batch_results", &nlm::BatchAPI::aggregateBatchResults,
                   py::arg("output_directory"), py::arg("output_file"),
                   "Aggregate batch results")
        .def_static("export_batch_results", &nlm::BatchAPI::exportBatchResults,
                   py::arg("params"), py::arg("format") = "json",
                   "Export batch results in specified format");

    // Template API
    py::class_<nlm::TemplateAPI>(m, "TemplateAPI")
        .def_static("get_template", &nlm::TemplateAPI::getTemplate,
                   py::arg("template_name"),
                   "Get a configuration template")
        .def_static("add_template", &nlm::TemplateAPI::addTemplate,
                   py::arg("name"), py::arg("config"),
                   "Add a new template")
        .def_static("remove_template", &nlm::TemplateAPI::removeTemplate,
                   py::arg("name"),
                   "Remove a template")
        .def_static("update_template", &nlm::TemplateAPI::updateTemplate,
                   py::arg("name"), py::arg("config"),
                   "Update an existing template")
        .def_static("list_templates", &nlm::TemplateAPI::listTemplates,
                   "List all available templates")
        .def_static("merge_template_configs", &nlm::TemplateAPI::mergeTemplateConfigs,
                   py::arg("template_names"),
                   "Merge multiple template configurations")
        .def_static("create_neurodevelopmental_template", &nlm::TemplateAPI::createNeurodevelopmentalTemplate,
                   "Create neurodevelopmental template")
        .def_static("create_cognitive_template", &nlm::TemplateAPI::createCognitiveTemplate,
                   "Create cognitive template")
        .def_static("create_learning_template", &nlm::TemplateAPI::createLearningTemplate,
                   "Create learning template")
        .def_static("create_memory_template", &nlm::TemplateAPI::createMemoryTemplate,
                   "Create memory template");

    // Performance API
    py::class_<nlm::PerformanceAPI>(m, "PerformanceAPI")
        .def_static("get_performance_metrics", &nlm::PerformanceAPI::getPerformanceMetrics,
                   "Get current performance metrics")
        .def_static("monitor_performance", &nlm::PerformanceAPI::monitorPerformance,
                   py::arg("duration"), py::arg("interval"),
                   "Monitor performance over time")
        .def_static("calculate_performance_score", &nlm::PerformanceAPI::calculatePerformanceScore,
                   py::arg("metrics"),
                   "Calculate performance score from metrics")
        .def_static("save_performance_metrics", &nlm::PerformanceAPI::savePerformanceMetrics,
                   py::arg("metrics"), py::arg("filename"),
                   "Save performance metrics to file")
        .def_static("load_performance_metrics", &nlm::PerformanceAPI::loadPerformanceMetrics,
                   py::arg("metrics"), py::arg("filename"),
                   "Load performance metrics from file")
        .def_static("optimize_for_performance", &nlm::PerformanceAPI::optimizeForPerformance,
                   py::arg("config"),
                   "Optimize configuration for performance")
        .def_static("optimize_for_energy", &nlm::PerformanceAPI::optimizeForEnergy,
                   py::arg("config"),
                   "Optimize configuration for energy efficiency")
        .def_static("optimize_for_accuracy", &nlm::PerformanceAPI::optimizeForAccuracy,
                   py::arg("config"),
                   "Optimize configuration for accuracy");

    // Visualization API
    py::class_<nlm::VisualizationAPI>(m, "VisualizationAPI")
        .def_static("create_spike_raster_plot", &nlm::VisualizationAPI::createSpikeRasterPlot,
                   py::arg("spike_trains"), py::arg("output_file"), py::arg("title") = "Spike Raster Plot",
                   "Create spike raster plot")
        .def_static("create_firing_rate_plot", &nlm::VisualizationAPI::createFiringRatePlot,
                   py::arg("firing_rates"), py::arg("output_file"), py::arg("title") = "Firing Rate",
                   "Create firing rate plot")
        .def_static("create_synapse_weight_plot", &nlm::VisualizationAPI::createSynapseWeightPlot,
                   py::arg("weight_history"), py::arg("output_file"), py::arg("title") = "Synapse Weight Evolution",
                   "Create synapse weight evolution plot")
        .def_static("create_brain_activity_plot", &nlm::VisualizationAPI::createBrainActivityPlot,
                   py::arg("activity_data"), py::arg("output_file"), py::arg("title") = "Brain Activity",
                   "Create brain activity plot")
        .def_static("create_3d_neuron_visualization", &nlm::VisualizationAPI::create3DNeuronVisualization,
                   py::arg("brain"), py::arg("output_file"),
                   "Create 3D neuron visualization")
        .def_static("create_network_connectivity_plot", &nlm::VisualizationAPI::createNetworkConnectivityPlot,
                   py::arg("brain"), py::arg("output_file"), py::arg("title") = "Network Connectivity",
                   "Create network connectivity plot");

    // Utility API
    py::class_<nlm::UtilityAPI>(m, "UtilityAPI")
        .def_static("generate_unique_id", &nlm::UtilityAPI::generateUniqueId,
                   "Generate a unique identifier")
        .def_static("hash_string", &nlm::UtilityAPI::hashString,
                   py::arg("str"),
                   "Hash a string")
        .def_static("current_time", &nlm::UtilityAPI::currentTime,
                   "Get current simulation time")
        .def_static("sleep", &nlm::UtilityAPI::sleep,
                   py::arg("seconds"),
                   "Sleep for specified seconds")
        .def_static("format_string", &nlm::UtilityAPI::formatString,
                   py::arg("format"), py::arg("..."),
                   "Format a string with variable arguments")
        .def_static("get_platform_info", &nlm::UtilityAPI::getPlatformInfo,
                   "Get platform information")
        .def_static("get_version_info", &nlm::UtilityAPI::getVersionInfo,
                   "Get NLM version information")
        .def_static("file_exists", &nlm::UtilityAPI::fileExists,
                   py::arg("filename"),
                   "Check if file exists")
        .def_static("create_directory", &nlm::UtilityAPI::createDirectory,
                   py::arg("path"),
                   "Create a directory")
        .def_static("list_files", &nlm::UtilityAPI::listFiles,
                   py::arg("directory"),
                   "List files in directory")
        .def_static("remove_file", &nlm::UtilityAPI::removeFile,
                   py::arg("filename"),
                   "Remove a file")
        .def_static("copy_file", &nlm::UtilityAPI::copyFile,
                   py::arg("source"), py::arg("destination"),
                   "Copy a file")
        .def_static("escape_string", &nlm::UtilityAPI::escapeString,
                   py::arg("str"),
                   "Escape special characters in string")
        .def_static("unescape_string", &nlm::UtilityAPI::unescapeString,
                   py::arg("str"),
                   "Unescape special characters in string")
        .def_static("base64_encode", &nlm::UtilityAPI::base64Encode,
                   py::arg("data"),
                   "Base64 encode data")
        .def_static("base64_decode", &nlm::UtilityAPI::base64Decode,
                   py::arg("data"),
                   "Base64 decode data")
        .def_static("get_current_working_directory", &nlm::UtilityAPI::getCurrentWorkingDirectory,
                   "Get current working directory")
        .def_static("change_working_directory", &nlm::UtilityAPI::changeWorkingDirectory,
                   py::arg("path"),
                   "Change working directory")
        .def_static("get_environment_variables", &nlm::UtilityAPI::getEnvironmentVariables,
                   "Get environment variables")
        .def_static("get_environment_variable", &nlm::UtilityAPI::getEnvironmentVariable,
                   py::arg("name"), py::arg("default_value") = "",
                   "Get environment variable")
        .def_static("set_environment_variable", &nlm::UtilityAPI::setEnvironmentVariable,
                   py::arg("name"), py::arg("value"),
                   "Set environment variable");

    // Initialize and finalize functions
    m.def("initialize_nlm", &nlm::initializeNLM, "Initialize NLM library");
    m.def("finalize_nlm", &nlm::finalizeNLM, "Finalize NLM library");

    // Python types
    py::class_<nlm::PipelineAPI::PipelineStep>(m, "PipelineStep")
        .def(py::init<>())
        .def_readwrite("name", &nlm::PipelineAPI::PipelineStep::name)
        .def_readwrite("parameters", &nlm::PipelineAPI::PipelineStep::parameters);

    // Function type aliases for Python
    using MetricFunction = std::function<double(const nlm::Brain&, size_t)>;
    py::class_<MetricFunction>(m, "MetricFunction");

    using PipelineFunction = std::function<bool(std::shared_ptr<nlm::Config>, std::shared_ptr<nlm::Brain>, std::any&)>;
    py::class_<PipelineFunction>(m, "PipelineFunction");

    // Constants
    m.attr("VERSION") = "0.1.0";
    m.attr("AUTHOR") = "NLM Development Team";
    m.attr("DESCRIPTION") = "Neural Learning Machine - Enhanced Edition";
}
