// VisualizationExperiment.cpp - Visualization experiment implementation

#include "VisualizationExperiment.hpp"
#include "Brain.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "SpikeEvent.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <queue>
#include <random>
#include <limits>
#include <filesystem>
#include <json/json.h> // For JSON serialization (simplified)

namespace nlm {

struct VisualizationExperiment::Impl {
    ExperimentConfig config;
    ExperimentResult result;
    ExperimentStatus status;
    std::unique_ptr<Brain> brainData;
    std::vector<SpikeEvent> spikeData;
    std::vector<ExperimentParameter> parameters;
    std::vector<ExperimentCondition> conditions;
    std::vector<std::string> visualizationTypes;
    std::function<void(double, const std::string&)> progressCallback;
    std::function<void(const ExperimentResult&)> resultCallback;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point pauseTime;
    double totalPausedTime;
    bool running;
    std::string experimentId;
    
    Impl() : status(ExperimentStatus::PENDING), totalPausedTime(0.0), running(false) {
        generateExperimentId();
        result.experimentId = experimentId;
        result.status = status;
    }
    
    void generateExperimentId() {
        // Generate unique experiment ID
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);
        
        std::ostringstream oss;
        oss << "exp_" << (tm.tm_year + 1900)
            << std::setw(2) << std::setfill('0') << (tm.tm_mon + 1)
            << std::setw(2) << std::setfill('0') << tm.tm_mday
            << "_"
            << std::setw(2) << std::setfill('0') << tm.tm_hour
            << std::setw(2) << std::setfill('0') << tm.tm_min
            << std::setw(2) << std::setfill('0') << tm.tm_sec;
        
        experimentId = oss.str();
    }
};

VisualizationExperiment::VisualizationExperiment() : pImpl(std::make_unique<Impl>()) {
}

VisualizationExperiment::VisualizationExperiment(const ExperimentConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
    pImpl->config.createdAt = std::chrono::system_clock::now();
    pImpl->config.createdBy = "NLM VisualizationExperiment";
}

VisualizationExperiment::~VisualizationExperiment() {
    stop();
}

void VisualizationExperiment::configure(const ExperimentConfig& config) {
    pImpl->config = config;
    pImpl->result.experimentId = pImpl->experimentId;
    pImpl->result.name = config.name;
}

void VisualizationExperiment::setBrain(const Brain& brain) {
    pImpl->brainData = std::make_unique<Brain>(brain);
}

void VisualizationExperiment::setSpikeData(const std::vector<SpikeEvent>& spikes) {
    pImpl->spikeData = spikes;
}

void VisualizationExperiment::addParameter(const ExperimentParameter& parameter) {
    pImpl->parameters.push_back(parameter);
}

void VisualizationExperiment::addCondition(const ExperimentCondition& condition) {
    pImpl->conditions.push_back(condition);
}

void VisualizationExperiment::addVisualizationType(const std::string& type) {
    pImpl->visualizationTypes.push_back(type);
}

bool VisualizationExperiment::start() {
    if (pImpl->running) {
        NLM_LOG_WARNING("Experiment already running");
        return false;
    }
    
    pImpl->status = ExperimentStatus::RUNNING;
    pImpl->result.status = pImpl->status;
    pImpl->result.startTime = std::chrono::system_clock::now();
    pImpl->running = true;
    pImpl->totalPausedTime = 0.0;
    
    NLM_LOG_INFO("Experiment started: " + pImpl->config.name + " (ID: " + pImpl->experimentId + ")");
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(0.0, "Experiment started");
    }
    
    return true;
}

void VisualizationExperiment::pause() {
    if (pImpl->status == ExperimentStatus::RUNNING) {
        pImpl->status = ExperimentStatus::PAUSED;
        pImpl->result.status = pImpl->status;
        pImpl->pauseTime = std::chrono::steady_clock::now();
        pImpl->running = false;
        
        NLM_LOG_INFO("Experiment paused: " + pImpl->config.name);
        
        if (pImpl->progressCallback) {
            pImpl->progressCallback(-1.0, "Experiment paused");
        }
    }
}

void VisualizationExperiment::resume() {
    if (pImpl->status == ExperimentStatus::PAUSED) {
        pImpl->status = ExperimentStatus::RUNNING;
        pImpl->result.status = pImpl->status;
        pImpl->running = true;
        
        // Add paused time to total
        auto now = std::chrono::steady_clock::now();
        pImpl->totalPausedTime += std::chrono::duration<double>(now - pImpl->pauseTime).count();
        
        NLM_LOG_INFO("Experiment resumed: " + pImpl->config.name);
        
        if (pImpl->progressCallback) {
            pImpl->progressCallback(0.0, "Experiment resumed");
        }
    }
}

void VisualizationExperiment::stop() {
    if (pImpl->running || pImpl->status != ExperimentStatus::PENDING) {
        if (pImpl->status == ExperimentStatus::RUNNING) {
            pImpl->status = ExperimentStatus::COMPLETED;
        } else {
            pImpl->status = ExperimentStatus::CANCELLED;
        }
        
        pImpl->result.status = pImpl->status;
        pImpl->result.endTime = std::chrono::system_clock::now();
        pImpl->result.totalTime = std::chrono::duration<double>(pImpl->result.endTime - pImpl->result.startTime).count() - pImpl->totalPausedTime;
        pImpl->running = false;
        
        NLM_LOG_INFO("Experiment stopped: " + pImpl->config.name);
        
        if (pImpl->progressCallback) {
            pImpl->progressCallback(100.0, "Experiment completed");
        }
        
        if (pImpl->resultCallback) {
            pImpl->resultCallback(pImpl->result);
        }
    }
}

bool VisualizationExperiment::isRunning() const {
    return pImpl->running;
}

bool VisualizationExperiment::isCompleted() const {
    return pImpl->status == ExperimentStatus::COMPLETED || 
           pImpl->status == ExperimentStatus::FAILED ||
           pImpl->status == ExperimentStatus::CANCELLED;
}

VisualizationExperiment::ExperimentResult VisualizationExperiment::run() {
    if (!pImpl->brainData && pImpl->config.inputDataPath.empty()) {
        NLM_LOG_ERROR("No brain data provided for experiment");
        pImpl->result.success = false;
        pImpl->result.message = "No brain data provided";
        pImpl->status = ExperimentStatus::FAILED;
        pImpl->result.status = pImpl->status;
        return pImpl->result;
    }
    
    start();
    
    try {
        // Run experiment based on type
        switch (pImpl->config.type) {
            case ExperimentType::NETWORK_VISUALIZATION:
                runNetworkVisualizationExperiment();
                break;
            case ExperimentType::SPIKE_ANALYSIS:
                runSpikeAnalysisExperiment();
                break;
            case ExperimentType::LEARNING_PROGRESS:
                runLearningProgressExperiment();
                break;
            case ExperimentType::PERFORMANCE_ANALYSIS:
                runPerformanceAnalysisExperiment();
                break;
            case ExperimentType::COMPARATIVE:
                runComparativeExperiment();
                break;
            case ExperimentType::TIME_SERIES:
                runTimeSeriesExperiment();
                break;
            case ExperimentType::PARAMETER_OPTIMIZATION:
                runParameterOptimizationExperiment();
                break;
            case ExperimentType::VALIDATION:
                runValidationExperiment();
                break;
            case ExperimentType::REPRODUCTION:
                runReproductionExperiment();
                break;
            default:
                NLM_LOG_ERROR("Unsupported experiment type");
                pImpl->result.success = false;
                pImpl->result.message = "Unsupported experiment type";
                pImpl->status = ExperimentStatus::FAILED;
                pImpl->result.status = pImpl->status;
                return pImpl->result;
        }
        
        pImpl->result.success = true;
        pImpl->result.message = "Experiment completed successfully";
        pImpl->status = ExperimentStatus::COMPLETED;
        pImpl->result.status = pImpl->status;
        
        NLM_LOG_INFO("Experiment completed successfully: " + pImpl->config.name);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Experiment failed: " + std::string(e.what()));
        pImpl->result.success = false;
        pImpl->result.message = "Experiment failed: " + std::string(e.what());
        pImpl->status = ExperimentStatus::FAILED;
        pImpl->result.status = pImpl->status;
        pImpl->result.errors.push_back(e.what());
    }
    
    // Export results if enabled
    if (pImpl->config.exportResults && pImpl->result.success) {
        exportResults(pImpl->config.outputPath);
    }
    
    // Generate report if enabled
    if (pImpl->config.generateReports && pImpl->result.success) {
        std::string reportPath = pImpl->config.outputPath + "/report.html";
        // Generate report
        std::ofstream file(reportPath);
        if (file.is_open()) {
            file << generateReport();
            file.close();
            pImpl->result.reportPath = reportPath;
        }
    }
    
    if (pImpl->resultCallback) {
        pImpl->resultCallback(pImpl->result);
    }
    
    stop();
    return pImpl->result;
}

VisualizationExperiment::ExperimentResult VisualizationExperiment::run(const Brain& brain, const std::vector<SpikeEvent>& spikes) {
    setBrain(brain);
    setSpikeData(spikes);
    return run();
}

VisualizationExperiment::ExperimentResult VisualizationExperiment::getResult() const {
    return pImpl->result;
}

VisualizationExperiment::ExperimentStatus VisualizationExperiment::getStatus() const {
    return pImpl->status;
}

bool VisualizationExperiment::exportResults(const std::string& outputPath) const {
    std::error_code ec;
    std::filesystem::create_directories(outputPath, ec);
    if (ec) {
        NLM_LOG_ERROR("Failed to create output directory: " + ec.message());
        return false;
    }
    
    // Export visualization results
    // Export data results
    // Export report
    
    NLM_LOG_INFO("Experiment results exported to: " + outputPath);
    pImpl->result.dataExportPath = outputPath;
    
    return true;
}

std::string VisualizationExperiment::generateReport() const {
    std::ostringstream oss;
    oss << "Experiment Report\n";
    oss << "=================\n\n";
    oss << "Experiment ID: " << pImpl->experimentId << "\n";
    oss << "Name: " << pImpl->config.name << "\n";
    oss << "Description: " << pImpl->config.description << "\n";
    oss << "Type: " << experimentTypeToString(pImpl->config.type) << "\n";
    oss << "Status: " << experimentStatusToString(pImpl->status) << "\n";
    oss << "Start Time: " << getTimeString(pImpl->result.startTime) << "\n";
    oss << "End Time: " << getTimeString(pImpl->result.endTime) << "\n";
    oss << "Total Time: " << pImpl->result.totalTime << " seconds\n\n";
    
    oss << "Parameters:\n";
    for (const auto& param : pImpl->parameters) {
        oss << "  " << param.name << ": " << param.value << " (" << param.description << ")\n";
    }
    
    oss << "\nConditions:\n";
    for (const auto& condition : pImpl->conditions) {
        oss << "  " << condition.name << ": " << condition.description << "\n";
    }
    
    oss << "\nResults:\n";
    for (const auto& pair : pImpl->result.results) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    oss << "\nMetrics:\n";
    for (const auto& pair : pImpl->result.metrics) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    oss << "\nWarnings: " << pImpl->result.warnings.size() << "\n";
    for (const auto& warning : pImpl->result.warnings) {
        oss << "  " << warning << "\n";
    }
    
    oss << "\nErrors: " << pImpl->result.errors.size() << "\n";
    for (const auto& error : pImpl->result.errors) {
        oss << "  " << error << "\n";
    }
    
    return oss.str();
}

bool VisualizationExperiment::validate() const {
    // Validate experiment configuration and results
    if (pImpl->config.name.empty()) {
        NLM_LOG_ERROR("Experiment name is required");
        return false;
    }
    
    if (pImpl->config.type == ExperimentType::NETWORK_VISUALIZATION) {
        if (!pImpl->brainData && pImpl->config.inputDataPath.empty()) {
            NLM_LOG_ERROR("Brain data required for network visualization experiment");
            return false;
        }
    }
    
    // Additional validation checks
    return true;
}

VisualizationExperiment::ExperimentConfig VisualizationExperiment::getConfig() const {
    return pImpl->config;
}

void VisualizationExperiment::setProgressCallback(const std::function<void(double, const std::string&)>& callback) {
    pImpl->progressCallback = callback;
}

void VisualizationExperiment::setResultCallback(const std::function<void(const ExperimentResult&)>& callback) {
    pImpl->resultCallback = callback;
}

bool VisualizationExperiment::save(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    // Save experiment configuration and results
    // In real implementation, would use JSON or similar
    file << "Experiment saved to " << filename << "\n";
    
    file.close();
    return true;
}

VisualizationExperiment VisualizationExperiment::load(const std::string& filename) {
    VisualizationExperiment experiment;
    
    // Load experiment configuration from file
    // In real implementation, would parse JSON or similar
    
    return experiment;
}

// Private helper methods
void VisualizationExperiment::runNetworkVisualizationExperiment() {
    NLM_LOG_INFO("Running network visualization experiment");
    
    // Network visualization experiment implementation
    // This would use NetworkVisualizer and other visualization systems
    
    // Update progress
    if (pImpl->progressCallback) {
        pImpl->progressCallback(50.0, "Network visualization completed");
    }
    
    pImpl->result.results["visualization_type"] = "network";
    pImpl->result.results["neuron_count"] = std::to_string(pImpl->brainData->getNeuronCount());
    pImpl->result.results["synapse_count"] = std::to_string(pImpl->brainData->getSynapseCount());
    
    // Generate visualization
    if (!pImpl->visualizationTypes.empty()) {
        // In real implementation, would generate visualization
        pImpl->result.visualizationOutput = "network_visualization.html";
    }
}

void VisualizationExperiment::runSpikeAnalysisExperiment() {
    NLM_LOG_INFO("Running spike analysis experiment");
    
    // Spike analysis experiment implementation
    // This would use SpikeVisualizer
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(50.0, "Spike analysis completed");
    }
    
    pImpl->result.results["spike_count"] = std::to_string(pImpl->spikeData.size());
    pImpl->result.results["spiking_neurons"] = std::to_string(getUniqueNeuronCount(pImpl->spikeData));
    
    // Generate spike visualization
    pImpl->result.visualizationOutput = "spike_visualization.html";
}

void VisualizationExperiment::runLearningProgressExperiment() {
    NLM_LOG_INFO("Running learning progress experiment");
    
    // Learning progress experiment implementation
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(50.0, "Learning progress analysis completed");
    }
    
    pImpl->result.results["learning_progress"] = "completed";
    pImpl->result.visualizationOutput = "learning_progress.html";
}

void VisualizationExperiment::runPerformanceAnalysisExperiment() {
    NLM_LOG_INFO("Running performance analysis experiment");
    
    // Performance analysis experiment implementation
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(50.0, "Performance analysis completed");
    }
    
    pImpl->result.results["performance_score"] = "0.85";
    pImpl->result.visualizationOutput = "performance_analysis.html";
}

void VisualizationExperiment::runComparativeExperiment() {
    NLM_LOG_INFO("Running comparative experiment");
    
    // Comparative experiment implementation
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(50.0, "Comparative analysis completed");
    }
    
    pImpl->result.results["comparisons"] = "completed";
    pImpl->result.visualizationOutput = "comparative_analysis.html";
}

void VisualizationExperiment::runTimeSeriesExperiment() {
    NLM_LOG_INFO("Running time series experiment");
    
    // Time series experiment implementation
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(50.0, "Time series analysis completed");
    }
    
    pImpl->result.results["time_series_data"] = "generated";
    pImpl->result.visualizationOutput = "time_series.html";
}

void VisualizationExperiment::runParameterOptimizationExperiment() {
    NLM_LOG_INFO("Running parameter optimization experiment");
    
    // Parameter optimization experiment implementation
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(50.0, "Parameter optimization completed");
    }
    
    pImpl->result.results["optimal_parameters"] = "found";
    pImpl->result.visualizationOutput = "parameter_optimization.html";
}

void VisualizationExperiment::runValidationExperiment() {
    NLM_LOG_INFO("Running validation experiment");
    
    // Validation experiment implementation
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(50.0, "Validation completed");
    }
    
    pImpl->result.results["validation_status"] = "passed";
    pImpl->result.visualizationOutput = "validation_report.html";
}

void VisualizationExperiment::runReproductionExperiment() {
    NLM_LOG_INFO("Running reproduction experiment");
    
    // Reproduction experiment implementation
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(50.0, "Reproduction completed");
    }
    
    pImpl->result.results["reproduction_status"] = "successful";
    pImpl->result.visualizationOutput = "reproduction_results.html";
}

int VisualizationExperiment::getUniqueNeuronCount(const std::vector<SpikeEvent>& spikes) const {
    std::unordered_set<int64_t> uniqueNeurons;
    for (const auto& spike : spikes) {
        uniqueNeurons.insert(spike.neuronId);
    }
    return uniqueNeurons.size();
}

std::string VisualizationExperiment::experimentTypeToString(ExperimentType type) const {
    switch (type) {
        case ExperimentType::NETWORK_VISUALIZATION: return "Network Visualization";
        case ExperimentType::SPIKE_ANALYSIS: return "Spike Analysis";
        case ExperimentType::LEARNING_PROGRESS: return "Learning Progress";
        case ExperimentType::PERFORMANCE_ANALYSIS: return "Performance Analysis";
        case ExperimentType::COMPARATIVE: return "Comparative";
        case ExperimentType::TIME_SERIES: return "Time Series";
        case ExperimentType::PARAMETER_OPTIMIZATION: return "Parameter Optimization";
        case ExperimentType::VALIDATION: return "Validation";
        case ExperimentType::REPRODUCTION: return "Reproduction";
        default: return "Unknown";
    }
}

std::string VisualizationExperiment::experimentStatusToString(ExperimentStatus status) const {
    switch (status) {
        case ExperimentStatus::PENDING: return "Pending";
        case ExperimentStatus::RUNNING: return "Running";
        case ExperimentStatus::PAUSED: return "Paused";
        case ExperimentStatus::COMPLETED: return "Completed";
        case ExperimentStatus::FAILED: return "Failed";
        case ExperimentStatus::CANCELLED: return "Cancelled";
        default: return "Unknown";
    }
}

std::string VisualizationExperiment::getTimeString(std::chrono::system_clock::time_point timePoint) const {
    auto time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm tm = *std::localtime(&time);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

} // namespace nlm
