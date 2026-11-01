#include "BatchProcessor.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>

namespace py = pybind11;

namespace nlm {

// Python bindings for BatchProcessor
void exportBatchProcessor(py::module_& m) {
    py::class_<BatchProcessor::BatchConfig>(m, "BatchConfig")
        .def(py::init<>())
        .def_readwrite("numIterations", &BatchProcessor::BatchConfig::numIterations)
        .def_readwrite("enableProgressReporting", &BatchProcessor::BatchConfig::enableProgressReporting)
        .def_readwrite("progressInterval", &BatchProcessor::BatchConfig::progressInterval)
        .def_readwrite("enableCheckpointing", &BatchProcessor::BatchConfig::enableCheckpointing)
        .def_readwrite("checkpointDirectory", &BatchProcessor::BatchConfig::checkpointDirectory)
        .def_readwrite("checkpointInterval", &BatchProcessor::BatchConfig::checkpointInterval)
        .def_readwrite("enableParallelExecution", &BatchProcessor::BatchConfig::enableParallelExecution)
        .def_readwrite("maxParallelThreads", &BatchProcessor::BatchConfig::maxParallelThreads)
        .def_readwrite("enableResultAggregation", &BatchProcessor::BatchConfig::enableResultAggregation)
        .def_readwrite("saveIndividualResults", &BatchProcessor::BatchConfig::saveIndividualResults)
        .def_readwrite("resultsDirectory", &BatchProcessor::BatchConfig::resultsDirectory)
        .def_readwrite("enablePerformanceMonitoring", &BatchProcessor::BatchConfig::enablePerformanceMonitoring)
        .def_readwrite("reportTimingDetails", &BatchProcessor::BatchConfig::reportTimingDetails);

    py::class_<BatchProcessor::BatchResult>(m, "BatchResult")
        .def(py::init<>())
        .def_readwrite("totalIterations", &BatchProcessor::BatchResult::totalIterations)
        .def_readwrite("successfulIterations", &BatchProcessor::BatchResult::successfulIterations)
        .def_readwrite("failedIterations", &BatchProcessor::BatchResult::failedIterations)
        .def_readwrite("successRate", &BatchProcessor::BatchResult::successRate)
        .def_readwrite("totalTimeSeconds", &BatchProcessor::BatchResult::totalTimeSeconds)
        .def_readwrite("averageTimePerIteration", &BatchProcessor::BatchResult::averageTimePerIteration)
        .def_readwrite("fastestIteration", &BatchProcessor::BatchResult::fastestIteration)
        .def_readwrite("slowestIteration", &BatchProcessor::BatchResult::slowestIteration)
        .def_readwrite("totalSpikes", &BatchProcessor::BatchResult::totalSpikes)
        .def_readwrite("averageSpikesPerIteration", &BatchProcessor::BatchResult::averageSpikesPerIteration)
        .def_readwrite("maxSpikesInIteration", &BatchProcessor::BatchResult::maxSpikesInIteration)
        .def_readwrite("averageDevelopmentStage", &BatchProcessor::BatchResult::averageDevelopmentStage)
        .def_readwrite("averageRegionCount", &BatchProcessor::BatchResult::averageRegionCount)
        .def_readwrite("peakMemoryUsage", &BatchProcessor::BatchResult::peakMemoryUsage)
        .def_readwrite("errors", &BatchProcessor::BatchResult::errors)
        .def_readwrite("batchId", &BatchProcessor::BatchResult::batchId);

    py::class_<BatchProcessor::IterationResult>(m, "IterationResult")
        .def(py::init<>())
        .def_readwrite("iterationNumber", &BatchProcessor::IterationResult::iterationNumber)
        .def_readwrite("iterationTimeSeconds", &BatchProcessor::IterationResult::iterationTimeSeconds)
        .def_readwrite("spikeCount", &BatchProcessor::IterationResult::spikeCount)
        .def_readwrite("developmentStage", &BatchProcessor::IterationResult::developmentStage)
        .def_readwrite("regionCount", &BatchProcessor::IterationResult::regionCount)
        .def_readwrite("neuronCount", &BatchProcessor::IterationResult::neuronCount)
        .def_readwrite("synapseCount", &BatchProcessor::IterationResult::synapseCount)
        .def_readwrite("avgFiringRate", &BatchProcessor::IterationResult::avgFiringRate)
        .def_readwrite("excitationInhibitionRatio", &BatchProcessor::IterationResult::excitationInhibitionRatio)
        .def_readwrite("checkpointSaved", &BatchProcessor::IterationResult::checkpointSaved)
        .def_readwrite("checkpointPath", &BatchProcessor::IterationResult::checkpointPath)
        .def_readwrite("errorMessage", &BatchProcessor::IterationResult::errorMessage)
        .def_readwrite("success", &BatchProcessor::IterationResult::success);

    py::class_<BatchProcessor>(m, "BatchProcessor")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"))
        .def("runBatch", &BatchProcessor::runBatch, py::arg("config"))
        .def("runBatch", [](BatchProcessor& self, const BatchProcessor::BatchConfig& config, 
                           const std::vector<std::shared_ptr<Brain>>& brains) {
            return self.runBatch(config, brains);
        }, py::arg("config"), py::arg("brains"))
        .def("executeIteration", &BatchProcessor::executeIteration, py::arg("iteration"), py::arg("brain"))
        .def("saveCheckpoint", &BatchProcessor::saveCheckpoint, py::arg("brain"), py::arg("filepath"))
        .def("loadCheckpoint", &BatchProcessor::loadCheckpoint, py::arg("brain"), py::arg("filepath"))
        .def("listCheckpoints", &BatchProcessor::listCheckpoints, py::arg("directory"))
        .def("cleanupOldCheckpoints", &BatchProcessor::cleanupOldCheckpoints, py::arg("directory"), py::arg("keepCount"))
        .def("reportProgress", &BatchProcessor::reportProgress, py::arg("current"), py::arg("total"), py::arg("result"))
        .def("saveIndividualResults", &BatchProcessor::saveIndividualResults, py::arg("result"))
        .def("saveBatchResults", &BatchProcessor::saveBatchResults, py::arg("result"))
        .def("createBatchConfig", &BatchProcessor::createBatchConfig, py::arg("batchConfig"))
        .def("applyBatchConfig", &BatchProcessor::applyBatchConfig, py::arg("brain"), py::arg("config"))
        .def("executeParallel", &BatchProcessor::executeParallel, py::arg("numIterations"), py::arg("results"))
        .def("executeParallel", [](BatchProcessor& self, std::shared_ptr<Brain> brain, 
                                  std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent,
                                  size_t numIterations, std::vector<BatchResult>& results) {
            return self.executeParallel(brain, world, agent, numIterations, results);
        }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numIterations"), py::arg("results"))
        .def("analyzeResults", &BatchProcessor::analyzeResults, py::arg("result"))
        .def("aggregateResults", &BatchProcessor::aggregateResults, py::arg("results"), py::arg("aggregated"))
        .def("getFormattedOutputPath", &BatchProcessor::getFormattedOutputPath, py::arg("subdirectory"), py::arg("filename"))
        .def("ensureDirectoryExists", &BatchProcessor::ensureDirectoryExists, py::arg("path"))
        .def("exportResultsToJSON", &BatchProcessor::exportResultsToJSON, py::arg("result"))
        .def("exportResultsToCSV", &BatchProcessor::exportResultsToCSV, py::arg("result"), py::arg("filepath"))
        // Static configuration factory methods
        .def("createExplorationConfig", &BatchProcessor::createExplorationConfig, py::arg("iterations"), py::arg("checkpointDir"))
        .def("createBenchmarkConfig", &BatchProcessor::createBenchmarkConfig, py::arg("iterations"), py::arg("resultsDir"))
        .def("createDevelopmentConfig", &BatchProcessor::createDevelopmentConfig, py::arg("iterations"))
        .def("createPerformanceConfig", &BatchProcessor::createPerformanceConfig, py::arg("iterations"));
}

// Python bindings for Debugger
void exportDebugger(py::module_& m) {
    py::class_<Debugger::DebugConfig>(m, "DebugConfig")
        .def(py::init<>())
        .def_readwrite("enableVerboseLogging", &Debugger::DebugConfig::enableVerboseLogging)
        .def_readwrite("enableDetailedTracing", &Debugger::DebugConfig::enableDetailedTracing)
        .def_readwrite("enableMemoryTracking", &Debugger::DebugConfig::enableMemoryTracking)
        .def_readwrite("enablePerformanceProfiling", &Debugger::DebugConfig::enablePerformanceProfiling)
        .def_readwrite("logFile", &Debugger::DebugConfig::logFile)
        .def_readwrite("enableConsoleOutput", &Debugger::DebugConfig::enableConsoleOutput)
        .def_readwrite("enableFileOutput", &Debugger::DebugConfig::enableFileOutput)
        .def_readwrite("enableNetworkOutput", &Debugger::DebugConfig::enableNetworkOutput)
        .def_readwrite("logLevelFilter", &Debugger::DebugConfig::logLevelFilter)
        .def_readwrite("componentFilter", &Debugger::DebugConfig::componentFilter)
        .def_readwrite("enableStructuredLogging", &Debugger::DebugConfig::enableStructuredLogging)
        .def_readwrite("performanceSampleInterval", &Debugger::DebugConfig::performanceSampleInterval)
        .def_readwrite("enableRealTimeMetrics", &Debugger::DebugConfig::enableRealTimeMetrics)
        .def_readwrite("metricsEndpoint", &Debugger::DebugConfig::metricsEndpoint)
        .def_readwrite("enableMemoryLeakDetection", &Debugger::DebugConfig::enableMemoryLeakDetection)
        .def_readwrite("memoryCheckInterval", &Debugger::DebugConfig::memoryCheckInterval)
        .def_readwrite("memoryAllocationThreshold", &Debugger::DebugConfig::memoryAllocationThreshold);

    py::enum_<DebugLevel>(m, "DebugLevel")
        .value("DEBUG", DebugLevel::DEBUG)
        .value("INFO", DebugLevel::INFO)
        .value("WARN", DebugLevel::WARN)
        .value("ERROR", DebugLevel::ERROR)
        .value("FATAL", DebugLevel::FATAL)
        .export_values();

    py::class_<Debugger::DebugSession>(m, "DebugSession")
        .def(py::init<>())
        .def_readwrite("startTime", &Debugger::DebugSession::startTime)
        .def_readwrite("endTime", &Debugger::DebugSession::endTime)
        .def_readwrite("logEntries", &Debugger::DebugSession::logEntries)
        .def_readwrite("memoryUsage", &Debugger::DebugSession::memoryUsage)
        .def_readwrite("performanceMetrics", &Debugger::DebugSession::performanceMetrics)
        .def_readwrite("errorMessages", &Debugger::DebugSession::errorMessages)
        .def_readwrite("metadata", &Debugger::DebugSession::metadata)
        .def_readwrite("isActive", &Debugger::DebugSession::isActive);

    py::class_<Debugger>(m, "Debugger")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"))
        .def("startDebugSession", &Debugger::startDebugSession, py::arg("sessionId"), py::arg("config"))
        .def("stopDebugSession", &Debugger::stopDebugSession)
        .def("pauseDebugSession", &Debugger::pauseDebugSession)
        .def("resumeDebugSession", &Debugger::resumeDebugSession)
        .def("isDebugSessionActive", &Debugger::isDebugSessionActive)
        .def("logDebug", &Debugger::logDebug, py::arg("message"), py::arg("level") = DebugLevel::INFO)
        .def("logNeuralActivity", &Debugger::logNeuralActivity, py::arg("brain"), py::arg("iteration"))
        .def("logWorldState", &Debugger::logWorldState, py::arg("world"), py::arg("iteration"))
        .def("logAgentBehavior", &Debugger::logAgentBehavior, py::arg("agent"), py::arg("iteration"))
        .def("startPerformanceMeasurement", &Debugger::startPerformanceMeasurement, py::arg("operationName"))
        .def("stopPerformanceMeasurement", &Debugger::stopPerformanceMeasurement, py::arg("operationName"))
        .def("getElapsedTime", &Debugger::getElapsedTime, py::arg("operationName"))
        .def("collectPerformanceMetrics", &Debugger::collectPerformanceMetrics)
        .def("getPerformanceReport", &Debugger::getPerformanceReport)
        .def("startMemoryTracking", &Debugger::startMemoryTracking)
        .def("stopMemoryTracking", &Debugger::stopMemoryTracking)
        .def("collectMemoryStats", &Debugger::collectMemoryStats)
        .def("getMemoryReport", &Debugger::getMemoryReport)
        .def("detectMemoryLeaks", &Debugger::detectMemoryLeaks)
        .def("debugNeuralConnectivity", &Debugger::debugNeuralConnectivity, py::arg("brain"))
        .def("debugSynapticStrength", &Debugger::debugSynapticStrength, py::arg("brain"))
        .def("debugNeuronStates", &Debugger::debugNeuronStates, py::arg("brain"))
        .def("debugPlasticityRules", &Debugger::debugPlasticityRules, py::arg("brain"))
        .def("debugWorldPhysics", &Debugger::debugWorldPhysics, py::arg("world"))
        .def("debugAgentPerception", &Debugger::debugAgentPerception, py::arg("agent"))
        .def("debugAgentActionSelection", &Debugger::debugAgentActionSelection, py::arg("agent"))
        .def("debugRewardSignals", &Debugger::debugRewardSignals, py::arg("agent"))
        .def("analyzeNeuralStability", &Debugger::analyzeNeuralStability, py::arg("brain"))
        .def("analyzeLearningProgress", &Debugger::analyzeLearningProgress, py::arg("brain"))
        .def("detectAnomalies", &Debugger::detectAnomalies, py::arg("brain"))
        .def("identifyPerformanceBottlenecks", &Debugger::identifyPerformanceBottlenecks)
        .def("exportDebugLog", &Debugger::exportDebugLog, py::arg("filepath"))
        .def("exportPerformanceData", &Debugger::exportPerformanceData, py::arg("filepath"))
        .def("exportMemoryData", &Debugger::exportMemoryData, py::arg("filepath"))
        .def("exportNeuralAnalysis", &Debugger::exportNeuralAnalysis, py::arg("filepath"))
        .def("loadDebugConfig", &Debugger::loadDebugConfig, py::arg("configFile"))
        .def("saveDebugConfig", &Debugger::saveDebugConfig, py::arg("configFile"))
        .def("getDebugConfig", &Debugger::getDebugConfig)
        .def("updateDebugConfig", &Debugger::updateDebugConfig, py::arg("config"))
        .def("profileNeuralSimulation", &Debugger::profileNeuralSimulation, py::arg("iterations"))
        .def("profileAgentBehavior", &Debugger::profileAgentBehavior, py::arg("iterations"))
        .def("profileWorldInteraction", &Debugger::profileWorldInteraction, py::arg("iterations"))
        .def("generateProfilingReport", &Debugger::generateProfilingReport);
}

// Python bindings for Visualizer
void exportVisualizer(py::module_& m) {
    py::class_<Visualizer::VisualizationConfig>(m, "VisualizationConfig")
        .def(py::init<>())
        .def_readwrite("enableImageExport", &Visualizer::VisualizationConfig::enableImageExport)
        .def_readwrite("enableVideoExport", &Visualizer::VisualizationConfig::enableVideoExport)
        .def_readwrite("enableWebExport", &Visualizer::VisualizationConfig::enableWebExport)
        .def_readwrite("enableInteractivePlots", &Visualizer::VisualizationConfig::enableInteractivePlots)
        .def_readwrite("imageFormat", &Visualizer::VisualizationConfig::imageFormat)
        .def_readwrite("imageDPI", &Visualizer::VisualizationConfig::imageDPI)
        .def_readwrite("imageWidth", &Visualizer::VisualizationConfig::imageWidth)
        .def_readwrite("imageHeight", &Visualizer::VisualizationConfig::imageHeight)
        .def_readwrite("videoFormat", &Visualizer::VisualizationConfig::videoFormat)
        .def_readwrite("videoFPS", &Visualizer::VisualizationConfig::videoFPS)
        .def_readwrite("videoDuration", &Visualizer::VisualizationConfig::videoDuration)
        .def_readwrite("webFormat", &Visualizer::VisualizationConfig::webFormat)
        .def_readwrite("webTitle", &Visualizer::VisualizationConfig::webTitle)
        .def_readwrite("enableWebInteractive", &Visualizer::VisualizationConfig::enableWebInteractive)
        .def_readwrite("colormap", &Visualizer::VisualizationConfig::colormap)
        .def_readwrite("neuralColorScheme", &Visualizer::VisualizationConfig::neuralColorScheme)
        .def_readwrite("enableStatisticalAnalysis", &Visualizer::VisualizationConfig::enableStatisticalAnalysis)
        .def_readwrite("enablePatternDetection", &Visualizer::VisualizationConfig::enablePatternDetection)
        .def_readwrite("enableAnomalyDetection", &Visualizer::VisualizationConfig::enableAnomalyDetection)
        .def_readwrite("outputDirectory", &Visualizer::VisualizationConfig::outputDirectory)
        .def_readwrite("imageSubdirectory", &Visualizer::VisualizationConfig::imageSubdirectory)
        .def_readwrite("videoSubdirectory", &Visualizer::VisualizationConfig::videoSubdirectory)
        .def_readwrite("webSubdirectory", &Visualizer::VisualizationConfig::webSubdirectory);

    py::class_<Visualizer>(m, "Visualizer")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"))
        .def("visualizeNeuralTopology", &Visualizer::visualizeNeuralTopology, py::arg("brain"), py::arg("outputPath"))
        .def("visualizeSynapticConnections", &Visualizer::visualizeSynapticConnections, py::arg("brain"), py::arg("outputPath"))
        .def("visualizeNeuralActivity", &Visualizer::visualizeNeuralActivity, py::arg("brain"), py::arg("iteration"), py::arg("outputPath"))
        .def("visualizeBrainRegions", &Visualizer::visualizeBrainRegions, py::arg("brain"), py::arg("outputPath"))
        .def("visualizePopulationDynamics", &Visualizer::visualizePopulationDynamics, py::arg("brain"), py::arg("outputPath"))
        .def("visualizeWorldTopology", &Visualizer::visualizeWorldTopology, py::arg("world"), py::arg("outputPath"))
        .def("visualizeAgentTrajectory", &Visualizer::visualizeAgentTrajectory, py::arg("world"), py::arg("agent"), py::arg("outputPath"))
        .def("visualizeWorldResources", &Visualizer::visualizeWorldResources, py::arg("world"), py::arg("outputPath"))
        .def("visualizeSensoryInput", &Visualizer::visualizeSensoryInput, py::arg("agent"), py::arg("outputPath"))
        .def("visualizeTrainingProgress", &Visualizer::visualizeTrainingProgress, py::arg("metrics"), py::arg("outputPath"))
        .def("visualizeLearningCurves", &Visualizer::visualizeLearningCurves, py::arg("learningMetrics"), py::arg("outputPath"))
        .def("visualizePerformanceMetrics", &Visualizer::visualizePerformanceMetrics, py::arg("performanceMetrics"), py::arg("outputPath"))
        .def("visualizeSpikeActivity", &Visualizer::visualizeSpikeActivity, py::arg("spikeCounts"), py::arg("outputPath"))
        .def("createStatisticalSummary", &Visualizer::createStatisticalSummary, py::arg("batchResult"), py::arg("outputPath"))
        .def("createErrorAnalysis", &Visualizer::createErrorAnalysis, py::arg("batchResult"), py::arg("outputPath"))
        .def("createPerformanceProfile", &Visualizer::createPerformanceProfile, py::arg("batchResult"), py::arg("outputPath"))
        .def("createMemoryAnalysis", &Visualizer::createMemoryAnalysis, py::arg("batchResult"), py::arg("outputPath"))
        .def("createNeuralSimulationVideo", &Visualizer::createNeuralSimulationVideo, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("duration"), py::arg("outputPath"))
        .def("createWorldSimulationVideo", &Visualizer::createWorldSimulationVideo, py::arg("world"), py::arg("agent"), py::arg("duration"), py::arg("outputPath"))
        .def("createCombinedSimulationVideo", &Visualizer::createCombinedSimulationVideo, py::arg("iterations"), py::arg("outputPath"))
        .def("createWebInterface", &Visualizer::createWebInterface, py::arg("batchResult"), py::arg("outputPath"))
        .def("createInteractiveDashboard", &Visualizer::createInteractiveDashboard, py::arg("batchResult"), py::arg("outputPath"))
        .def("createNeuralNetworkVisualizer", &Visualizer::createNeuralNetworkVisualizer, py::arg("batchResult"), py::arg("outputPath"))
        .def("createWorldSimulationVisualizer", &Visualizer::createWorldSimulationVisualizer, py::arg("batchResult"), py::arg("outputPath"))
        .def("generateAnalysisReport", &Visualizer::generateAnalysisReport, py::arg("batchResult"), py::arg("outputPath"))
        .def("generateResearchPaperFigures", &Visualizer::generateResearchPaperFigures, py::arg("batchResult"), py::arg("outputPath"))
        .def("generateTechnicalDocumentation", &Visualizer::generateTechnicalDocumentation, py::arg("batchResult"), py::arg("outputPath"))
        .def("loadVisualizationConfig", &Visualizer::loadVisualizationConfig, py::arg("configFile"))
        .def("saveVisualizationConfig", &Visualizer::saveVisualizationConfig, py::arg("configFile"))
        .def("getVisualizationConfig", &Visualizer::getVisualizationConfig)
        .def("updateVisualizationConfig", &Visualizer::updateVisualizationConfig, py::arg("config"));
}
// Python bindings for BatchUtils
void exportBatchUtils(py::module_& m) {
    m.def("runQuickTest", &BatchUtils::runQuickTest, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("iterations") = 100);
    m.def("runExplorationBatch", &BatchUtils::runExplorationBatch, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("checkpointDir"));
    m.def("runBenchmarkBatch", &BatchUtils::runBenchmarkBatch, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("resultsDir"));
    
    m.def("captureNeuralSnapshot", &BatchUtils::captureNeuralSnapshot, py::arg("brain"), py::arg("snapshotPath"));
    m.def("restoreNeuralSnapshot", &BatchUtils::restoreNeuralSnapshot, py::arg("brain"), py::arg("snapshotPath"));
    m.def("analyzeNeuralStability", &BatchUtils::analyzeNeuralStability, py::arg("brain"));
    m.def("detectPerformanceIssues", &BatchUtils::detectPerformanceIssues, py::arg("brain"));
    
    m.def("createOptimizedConfig", &BatchUtils::createOptimizedConfig, py::arg("useCase"));
    m.def("createResearchConfig", &BatchUtils::createResearchConfig, py::arg("neuronCount"), py::arg("connectionProbability"));
    m.def("createProductionConfig", &BatchUtils::createProductionConfig, py::arg("neuronCount"));
    
    m.def("formatBatchResult", &BatchUtils::formatBatchResult, py::arg("result"));
    m.def("generateBatchReport", &BatchUtils::generateBatchReport, py::arg("result"));
    m.def("calculatePerformanceScore", &BatchUtils::calculatePerformanceScore, py::arg("result"));
    
    m.def("getSystemMemoryUsage", &BatchUtils::getSystemMemoryUsage);
    m.def("getSystemCPUUsage", &BatchUtils::getSystemCPUUsage);
    m.def("getSystemInfo", &BatchUtils::getSystemInfo);
    
    m.def("createDirectory", &BatchUtils::createDirectory, py::arg("path"));
    m.def("getFilesInDirectory", &BatchUtils::getFilesInDirectory, py::arg("directory"), py::arg("extension"));
    m.def("copyFile", &BatchUtils::copyFile, py::arg("source"), py::arg("destination"));
    m.def("deleteFile", &BatchUtils::deleteFile, py::arg("path"));
    m.def("deleteDirectory", &BatchUtils::deleteDirectory, py::arg("path"));
}
PYBIND11_MODULE(pynlm_advanced, m) {
    exportBatchProcessor(m);
    exportDebugger(m);
    exportVisualizer(m);
    exportBatchUtils(m);
    
    // Export convenience types
    py::class_<std::vector<double>>(m, "DoubleVector");
    py::class_<std::vector<std::string>>(m, "StringVector");
    py::class_<std::map<std::string, std::string>>(m, "StringMap");
    
    // Export debug level
    py::enum_<DebugLevel>(m, "DebugLevel");
    
    // Export visualization mode
    py::enum_<VisualizationMode>(m, "VisualizationMode");
    
    // Export performance metrics
    py::class_<PerformanceMetrics>(m, "PerformanceMetrics");
    
    // Export batch statistics
    py::class_<BatchStatistics>(m, "BatchStatistics");
}

} // namespace nlm

// Debug level enumeration
enum class DebugLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

// Visualization mode enumeration
enum class VisualizationMode {
    NETWORK,
    ACTIVITY,
    STATISTICAL,
    TIME_SERIES,
    COMPARISON
};

// Performance metrics structure
struct PerformanceMetrics {
    double cpuUsage = 0.0;
    size_t memoryUsage = 0;
    double gpuUsage = 0.0;
    std::vector<double> iterationTimes;
    std::vector<size_t> spikeCounts;
    std::vector<double> firingRates;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    double totalTime() const {
        return std::chrono::duration<double>(endTime - startTime).count();
    }
};

// Batch statistics structure
struct BatchStatistics {
    size_t totalBatches = 0;
    size_t successfulBatches = 0;
    size_t failedBatches = 0;
    double averageBatchTime = 0.0;
    double averageSuccessRate = 0.0;
    size_t totalIterations = 0;
    size_t totalSpikes = 0;
    double totalTrainingTime = 0.0;
    std::map<std::string, double> performanceScores;
    std::vector<std::string> performanceTrends;
};

} // namespace nlm

// Debug level definition (duplicate for Python binding compatibility)
enum DebugLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
    FATAL = 4
};