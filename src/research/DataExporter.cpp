#include "DataExporter.hpp"

#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <zlib.h> // For compression if available
#include <nlohmann/json.hpp> // For JSON export

namespace nlm {

struct DataExporter::Impl {
    ExportConfig config;
    ExportStats stats;
    ExportProgressCallback progressCallback;
    
    std::vector<std::string> exportedFiles;
    std::atomic<double> progress{0.0};
    std::atomic<bool> running{false};
    std::string currentStatus;
    
    std::mutex mutex;
    std::chrono::steady_clock::time_point startTime;
    
    Impl() : progress(0.0), stats{0, 0, 0, 0.0, std::chrono::steady_clock::time_point(), {}} {
        startTime = std::chrono::steady_clock::now();
        
        // Create default output directory if it doesn't exist
        if (!std::filesystem::exists(".")) {
            std::filesystem::create_directories(".");
        }
    }
    
    ~Impl() {
        // Clean up
    }
    
    void updateProgress(double p, const std::string& status) {
        progress = p;
        currentStatus = status;
        
        if (progressCallback) {
            progressCallback(progress, status);
        }
    }
    
    void updateStats(bool success) {
        std::lock_guard<std::mutex> lock(mutex);
        stats.totalExports++;
        if (success) {
            stats.successfulExports++;
        } else {
            stats.failedExports++;
        }
        
        auto now = std::chrono::steady_clock::now();
        stats.lastExportTime = now - startTime;
        stats.totalTime = std::chrono::duration<double>(stats.lastExportTime).count();
    }
    
    std::string generateTimestampedFilename(const std::string& baseName) const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        
        std::string extension;
        switch (config.format) {
            case ExportFormat::JSON: extension = ".json"; break;
            case ExportFormat::CSV: extension = ".csv"; break;
            case ExportFormat::XML: extension = ".xml"; break;
            case ExportFormat::HDF5: extension = ".h5"; break;
            case ExportFormat::Parquet: extension = ".parquet"; break;
            case ExportFormat::MATLAB: extension = ".mat"; break;
            case ExportFormat::Python: extension = ".pydata"; break;
            case ExportFormat::Binary: extension = ".bin"; break;
            default: extension = ".dat"; break;
        }
        
        return config.filenamePrefix + baseName + "_" + ss.str() + config.filenameSuffix + extension;
    }
    
    bool exportToJSON(const std::string& filename, const nlohmann::json& data) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        if (config.compress) {
            // TODO: Implement compression
            file << data.dump(2);
        } else {
            file << data.dump(2);
        }
        
        file.close();
        return true;
    }
    
    bool exportToCSV(const std::string& filename, const nlohmann::json& data) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Simple CSV export - could be enhanced for complex data
        if (data.is_array()) {
            for (size_t i = 0; i < data.size(); ++i) {
                file << data[i];
                if (i < data.size() - 1) {
                    file << "\n";
                }
            }
        }
        
        file.close();
        return true;
    }
    
    bool exportToBinary(const std::string& filename, const nlohmann::json& data) const {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Simple binary export - could be enhanced
        std::string jsonStr = data.dump();
        file.write(jsonStr.c_str(), jsonStr.size());
        
        file.close();
        return true;
    }
    
    nlohmann::json createExperimentJSON(const Experiment& experiment) const {
        nlohmann::json json;
        
        json["name"] = experiment.getName();
        json["seed"] = experiment.getSeed();
        json["startStep"] = experiment.getStartStep();
        json["endStep"] = experiment.getEndStep();
        json["metrics"] = experiment.getMetrics();
        json["stepData"] = experiment.getStepData();
        json["notes"] = experiment.getNotes();
        
        // Add timestamp if enabled
        if (config.includeTimestamps) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            json["exportTime"] = std::string(std::ctime(&time));
        }
        
        // Add metadata
        if (!config.metadata.empty()) {
            json["metadata"] = config.metadata;
        }
        
        return json;
    }
    
    nlohmann::json createBrainJSON(const Brain& brain) const {
        nlohmann::json json;
        
        json["totalNeuronCount"] = brain.getTotalNeuronCount();
        json["totalSynapseCount"] = brain.getTotalSynapseCount();
        json["activeNeuronCount"] = brain.getActiveNeuronCount();
        json["firingNeuronCount"] = brain.getFiringNeuronCount();
        json["averageFiringRate"] = brain.getAverageFiringRate();
        json["totalSpikeCount"] = brain.getTotalSpikeCount();
        json["pendingSpikeEventCount"] = brain.getPendingSpikeEventCount();
        json["excitationInhibitionRatio"] = brain.getExcitationInhibitionRatio();
        
        // Region information
        json["regionCount"] = brain.getRegionCount();
        for (const auto& regionId : brain.getRegionIds()) {
            const NeuralRegion* region = brain.getRegion(regionId);
            if (region) {
                nlohmann::json regionJson;
                regionJson["id"] = regionId.value;
                regionJson["name"] = region->getName();
                regionJson["neuronCount"] = region->getNeuronCount();
                regionJson["firingCount"] = region->getFiringNeuronCount();
                json["regions"].push_back(regionJson);
            }
        }
        
        // Developmental stage
        json["developmentalStage"] = static_cast<int>(brain.getDevelopmentalStage());
        
        return json;
    }
    
    nlohmann::json createBatchResultJSON(const BatchProcessor::BatchResult& result) const {
        nlohmann::json json;
        
        json["experimentId"] = result.experimentId;
        json["experimentName"] = result.experimentName;
        json["success"] = result.success;
        json["executionTime"] = result.executionTime;
        json["totalSteps"] = result.totalSteps;
        json["completedSteps"] = result.completedSteps;
        json["status"] = result.status;
        json["metrics"] = result.metrics;
        
        return json;
    }
    
    nlohmann::json createOptimizationResultJSON(const ParameterOptimizer::OptimizationResult& result) const {
        nlohmann::json json;
        
        json["experimentId"] = result.experimentId;
        json["experimentName"] = result.experimentName;
        json["score"] = result.score;
        json["executionTime"] = result.executionTime;
        
        // Parameters
        nlohmann::json paramsJson;
        for (const auto& param : result.parameters) {
            nlohmann::json paramJson;
            paramJson["name"] = param.name;
            paramJson["currentValue"] = param.currentValue;
            paramJson["minValue"] = param.minValue;
            paramJson["maxValue"] = param.maxValue;
            paramJson["isDiscrete"] = param.isDiscrete;
            paramsJson[param.name] = param.currentValue;
        }
        json["parameters"] = paramsJson;
        
        // Metrics
        json["metricValues"] = result.metricValues;
        
        return json;
    }
};

DataExporter::DataExporter() : pImpl(std::make_unique<Impl>()) {}

DataExporter::~DataExporter() = default;

bool DataExporter::initialize(const ExportConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->config = config;
    
    // Create output directory if it doesn't exist
    if (!std::filesystem::exists(pImpl->config.outputDirectory)) {
        std::filesystem::create_directories(pImpl->config.outputDirectory);
    }
    
    // Clear previous exports
    pImpl->exportedFiles.clear();
    pImpl->updateProgress(0.0, "Initializing data exporter...");
    
    return true;
}

bool DataExporter::exportExperiment(const std::shared_ptr<Experiment>& experiment, const std::string& outputPath) {
    if (!experiment) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string filename = outputPath.empty() ? 
        pImpl->generateTimestampedFilename("experiment") : 
        outputPath;
    
    pImpl->updateProgress(0.1, "Exporting experiment: " + experiment->getName());
    
    // Create JSON data
    nlohmann::json data = pImpl->createExperimentJSON(*experiment);
    
    // Export based on format
    bool success = false;
    switch (pImpl->config.format) {
        case ExportFormat::JSON:
            success = pImpl->exportToJSON(filename, data);
            break;
        case ExportFormat::CSV:
            success = pImpl->exportToCSV(filename, data);
            break;
        case ExportFormat::Binary:
            success = pImpl->exportToBinary(filename, data);
            break;
        default:
            success = pImpl->exportToJSON(filename, data); // Default to JSON
            break;
    }
    
    if (success) {
        pImpl->exportedFiles.push_back(filename);
        pImpl->updateStats(true);
        pImpl->updateProgress(1.0, "Experiment exported successfully: " + filename);
    } else {
        pImpl->updateStats(false);
        pImpl->updateProgress(0.0, "Failed to export experiment: " + experiment->getName());
    }
    
    return success;
}

bool DataExporter::exportBatchResults(const std::vector<BatchProcessor::BatchResult>& batchResults, const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string filename = outputPath.empty() ? 
        pImpl->generateTimestampedFilename("batch_results") : 
        outputPath;
    
    pImpl->updateProgress(0.1, "Exporting batch results: " + std::to_string(batchResults.size()) + " results");
    
    // Create JSON array of batch results
    nlohmann::json data = nlohmann::json::array();
    for (const auto& result : batchResults) {
        data.push_back(pImpl->createBatchResultJSON(result));
    }
    
    // Add metadata
    nlohmann::json metadata;
    metadata["totalResults"] = batchResults.size();
    metadata["successfulResults"] = std::count_if(batchResults.begin(), batchResults.end(),
                                                [](const BatchProcessor::BatchResult& r) { return r.success; });
    metadata["failedResults"] = batchResults.size() - metadata["successfulResults"];
    data["metadata"] = metadata;
    
    // Export based on format
    bool success = false;
    switch (pImpl->config.format) {
        case ExportFormat::JSON:
            success = pImpl->exportToJSON(filename, data);
            break;
        case ExportFormat::CSV:
            success = pImpl->exportToCSV(filename, data);
            break;
        case ExportFormat::Binary:
            success = pImpl->exportToBinary(filename, data);
            break;
        default:
            success = pImpl->exportToJSON(filename, data); // Default to JSON
            break;
    }
    
    if (success) {
        pImpl->exportedFiles.push_back(filename);
        pImpl->updateStats(true);
        pImpl->updateProgress(1.0, "Batch results exported successfully: " + filename);
    } else {
        pImpl->updateStats(false);
        pImpl->updateProgress(0.0, "Failed to export batch results");
    }
    
    return success;
}

bool DataExporter::exportOptimizationResults(const std::vector<ParameterOptimizer::OptimizationResult>& optimizationResults, const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string filename = outputPath.empty() ? 
        pImpl->generateTimestampedFilename("optimization_results") : 
        outputPath;
    
    pImpl->updateProgress(0.1, "Exporting optimization results: " + std::to_string(optimizationResults.size()) + " results");
    
    // Create JSON array of optimization results
    nlohmann::json data = nlohmann::json::array();
    for (const auto& result : optimizationResults) {
        data.push_back(pImpl->createOptimizationResultJSON(result));
    }
    
    // Add metadata
    nlohmann::json metadata;
    metadata["totalResults"] = optimizationResults.size();
    metadata["bestScore"] = 0.0;
    double maxScore = std::max_element(optimizationResults.begin(), optimizationResults.end(),
                                      [](const ParameterOptimizer::OptimizationResult& a, const ParameterOptimizer::OptimizationResult& b) {
                                          return a.score < b.score;
                                      })->score;
    metadata["bestScore"] = maxScore;
    data["metadata"] = metadata;
    
    // Export based on format
    bool success = false;
    switch (pImpl->config.format) {
        case ExportFormat::JSON:
            success = pImpl->exportToJSON(filename, data);
            break;
        case ExportFormat::CSV:
            success = pImpl->exportToCSV(filename, data);
            break;
        case ExportFormat::Binary:
            success = pImpl->exportToBinary(filename, data);
            break;
        default:
            success = pImpl->exportToJSON(filename, data); // Default to JSON
            break;
    }
    
    if (success) {
        pImpl->exportedFiles.push_back(filename);
        pImpl->updateStats(true);
        pImpl->updateProgress(1.0, "Optimization results exported successfully: " + filename);
    } else {
        pImpl->updateStats(false);
        pImpl->updateProgress(0.0, "Failed to export optimization results");
    }
    
    return success;
}

bool DataExporter::exportBrainState(const Brain& brain, const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string filename = outputPath.empty() ? 
        pImpl->generateTimestampedFilename("brain_state") : 
        outputPath;
    
    pImpl->updateProgress(0.1, "Exporting brain state...");
    
    // Create JSON data
    nlohmann::json data = pImpl->createBrainJSON(brain);
    
    // Export based on format
    bool success = false;
    switch (pImpl->config.format) {
        case ExportFormat::JSON:
            success = pImpl->exportToJSON(filename, data);
            break;
        case ExportFormat::CSV:
            success = pImpl->exportToCSV(filename, data);
            break;
        case ExportFormat::Binary:
            success = pImpl->exportToBinary(filename, data);
            break;
        default:
            success = pImpl->exportToJSON(filename, data); // Default to JSON
            break;
    }
    
    if (success) {
        pImpl->exportedFiles.push_back(filename);
        pImpl->updateStats(true);
        pImpl->updateProgress(1.0, "Brain state exported successfully: " + filename);
    } else {
        pImpl->updateStats(false);
        pImpl->updateProgress(0.0, "Failed to export brain state");
    }
    
    return success;
}

bool DataExporter::exportNeuralData(const Brain& brain, const std::vector<SimulationStep>& steps, const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string filename = outputPath.empty() ? 
        pImpl->generateTimestampedFilename("neural_data") : 
        outputPath;
    
    pImpl->updateProgress(0.1, "Exporting neural data for " + std::to_string(steps.size()) + " steps");
    
    // Create neural data JSON
    nlohmann::json data = nlohmann::json::array();
    
    for (SimulationStep step : steps) {
        nlohmann::json stepJson;
        stepJson["step"] = step;
        stepJson["neuronCount"] = brain.getTotalNeuronCount();
        stepJson["synapseCount"] = brain.getTotalSynapseCount();
        stepJson["firingCount"] = brain.getFiringNeuronCount();
        stepJson["avgFiringRate"] = brain.getAverageFiringRate();
        
        // Add neural region data
        for (const auto& regionId : brain.getRegionIds()) {
            const NeuralRegion* region = brain.getRegion(regionId);
            if (region) {
                nlohmann::json regionJson;
                regionJson["regionId"] = regionId.value;
                regionJson["neuronCount"] = region->getNeuronCount();
                regionJson["firingCount"] = region->getFiringNeuronCount();
                stepJson["regions"].push_back(regionJson);
            }
        }
        
        data.push_back(stepJson);
    }
    
    // Export based on format
    bool success = false;
    switch (pImpl->config.format) {
        case ExportFormat::JSON:
            success = pImpl->exportToJSON(filename, data);
            break;
        case ExportFormat::CSV:
            success = pImpl->exportToCSV(filename, data);
            break;
        case ExportFormat::Binary:
            success = pImpl->exportToBinary(filename, data);
            break;
        default:
            success = pImpl->exportToJSON(filename, data); // Default to JSON
            break;
    }
    
    if (success) {
        pImpl->exportedFiles.push_back(filename);
        pImpl->updateStats(true);
        pImpl->updateProgress(1.0, "Neural data exported successfully: " + filename);
    } else {
        pImpl->updateStats(false);
        pImpl->updateProgress(0.0, "Failed to export neural data");
    }
    
    return success;
}

bool DataExporter::exportEnvironmentData(const Environment& environment, const std::vector<SimulationStep>& steps, const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string filename = outputPath.empty() ? 
        pImpl->generateTimestampedFilename("environment_data") : 
        outputPath;
    
    pImpl->updateProgress(0.1, "Exporting environment data for " + std::to_string(steps.size()) + " steps");
    
    // Create environment data JSON
    nlohmann::json data = nlohmann::json::array();
    
    for (SimulationStep step : steps) {
        nlohmann::json stepJson;
        stepJson["step"] = step;
        // In real implementation, would get actual environment state
        // For now, placeholder
        stepJson["status"] = "placeholder";
        data.push_back(stepJson);
    }
    
    // Export based on format
    bool success = false;
    switch (pImpl->config.format) {
        case ExportFormat::JSON:
            success = pImpl->exportToJSON(filename, data);
            break;
        case ExportFormat::CSV:
            success = pImpl->exportToCSV(filename, data);
            break;
        case ExportFormat::Binary:
            success = pImpl->exportToBinary(filename, data);
            break;
        default:
            success = pImpl->exportToJSON(filename, data); // Default to JSON
            break;
    }
    
    if (success) {
        pImpl->exportedFiles.push_back(filename);
        pImpl->updateStats(true);
        pImpl->updateProgress(1.0, "Environment data exported successfully: " + filename);
    } else {
        pImpl->updateStats(false);
        pImpl->updateProgress(0.0, "Failed to export environment data");
    }
    
    return success;
}

bool DataExporter::exportDebugResults(const BrainInspector& inspector, const MemoryProfiler& profiler, const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string filename = outputPath.empty() ? 
        pImpl->generateTimestampedFilename("debug_results") : 
        outputPath;
    
    pImpl->updateProgress(0.1, "Exporting debug results...");
    
    // Create debug results JSON
    nlohmann::json data;
    
    // Inspector results
    nlohmann::json inspectorJson;
    inspectorJson["progress"] = inspector.getInspectionProgress();
    inspectorJson["complete"] = inspector.isInspectionComplete();
    inspectorJson["report"] = inspector.getInspectionReport();
    inspectorJson["anomalies"] = inspector.getDetectedAnomalies();
    
    // Profiler results
    nlohmann::json profilerJson;
    profilerJson["progress"] = profiler.getProfileProgress();
    profilerJson["complete"] = profiler.isProfileComplete();
    profilerJson["report"] = profiler.getProfileReport();
    profilerJson["anomalies"] = profiler.getDetectedAnomalies();
    
    data["inspector"] = inspectorJson;
    data["profiler"] = profilerJson;
    
    // Export based on format
    bool success = false;
    switch (pImpl->config.format) {
        case ExportFormat::JSON:
            success = pImpl->exportToJSON(filename, data);
            break;
        case ExportFormat::CSV:
            success = pImpl->exportToCSV(filename, data);
            break;
        case ExportFormat::Binary:
            success = pImpl->exportToBinary(filename, data);
            break;
        default:
            success = pImpl->exportToJSON(filename, data); // Default to JSON
            break;
    }
    
    if (success) {
        pImpl->exportedFiles.push_back(filename);
        pImpl->updateStats(true);
        pImpl->updateProgress(1.0, "Debug results exported successfully: " + filename);
    } else {
        pImpl->updateStats(false);
        pImpl->updateProgress(0.0, "Failed to export debug results");
    }
    
    return success;
}

bool DataExporter::exportVisualizationFrames(VisualizationInterface& vizInterface, const std::vector<std::chrono::steady_clock::time_point>& frameTimes, const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string filename = outputPath.empty() ? 
        pImpl->generateTimestampedFilename("visualization_frames") : 
        outputPath;
    
    pImpl->updateProgress(0.1, "Exporting visualization frames: " + std::to_string(frameTimes.size()) + " frames");
    
    // Create visualization frames JSON
    nlohmann::json data;
    data["frameCount"] = frameTimes.size();
    data["exporter"] = "NLM Data Exporter";
    data["format"] = "placeholder";
    
    // Export based on format
    bool success = false;
    switch (pImpl->config.format) {
        case ExportFormat::JSON:
            success = pImpl->exportToJSON(filename, data);
            break;
        case ExportFormat::CSV:
            success = pImpl->exportToCSV(filename, data);
            break;
        case ExportFormat::Binary:
            success = pImpl->exportToBinary(filename, data);
            break;
        default:
            success = pImpl->exportToJSON(filename, data); // Default to JSON
            break;
    }
    
    if (success) {
        pImpl->exportedFiles.push_back(filename);
        pImpl->updateStats(true);
        pImpl->updateProgress(1.0, "Visualization frames exported successfully: " + filename);
    } else {
        pImpl->updateStats(false);
        pImpl->updateProgress(0.0, "Failed to export visualization frames");
    }
    
    return success;
}

bool DataExporter::exportResearchDataset(const std::vector<std::shared_ptr<Experiment>>& experiments, 
                                       std::shared_ptr<Brain> brainTemplate,
                                       std::shared_ptr<Environment> environmentTemplate,
                                       SimulationStep maxSteps,
                                       const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string filename = outputPath.empty() ? 
        pImpl->generateTimestampedFilename("research_dataset") : 
        outputPath;
    
    pImpl->updateProgress(0.1, "Exporting research dataset with " + std::to_string(experiments.size()) + " experiments");
    
    // Create research dataset JSON
    nlohmann::json data;
    data["datasetType"] = "research_dataset";
    data["exporter"] = "NLM Data Exporter";
    data["exportTime"] = std::chrono::system_clock::now();
    data["experimentCount"] = experiments.size();
    
    // Export based on format
    bool success = false;
    switch (pImpl->config.format) {
        case ExportFormat::JSON:
            success = pImpl->exportToJSON(filename, data);
            break;
        case ExportFormat::CSV:
            success = pImpl->exportToCSV(filename, data);
            break;
        case ExportFormat::Binary:
            success = pImpl->exportToBinary(filename, data);
            break;
        default:
            success = pImpl->exportToJSON(filename, data); // Default to JSON
            break;
    }
    
    if (success) {
        pImpl->exportedFiles.push_back(filename);
        pImpl->updateStats(true);
        pImpl->updateProgress(1.0, "Research dataset exported successfully: " + filename);
    } else {
        pImpl->updateStats(false);
        pImpl->updateProgress(0.0, "Failed to export research dataset");
    }
    
    return success;
}

std::vector<std::string> DataExporter::getExportedFiles() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->exportedFiles;
}

void DataExporter::clearExports() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->exportedFiles.clear();
    pImpl->updateProgress(0.0, "All exports cleared");
}

bool DataExporter::removeExport(const std::string& filename) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find(pImpl->exportedFiles.begin(), pImpl->exportedFiles.end(), filename);
    if (it != pImpl->exportedFiles.end()) {
        pImpl->exportedFiles.erase(it);
        
        // Try to remove file from disk
        std::error_code ec;
        std::filesystem::remove(filename, ec);
        
        pImpl->updateProgress(0.0, "Export removed: " + filename);
        return true;
    }
    
    return false;
}

bool DataExporter::removeAllExports() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    bool success = true;
    for (const auto& filename : pImpl->exportedFiles) {
        std::error_code ec;
        if (std::filesystem::exists(filename, ec)) {
            if (!std::filesystem::remove(filename, ec)) {
                success = false;
            }
        }
    }
    
    pImpl->exportedFiles.clear();
    pImpl->updateProgress(0.0, "All exports removed");
    
    return success;
}

double DataExporter::getExportProgress() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->progress;
}

std::string DataExporter::getCurrentExportStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentStatus;
}

void DataExporter::setExportFormat(ExportFormat format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.format = format;
}

void DataExporter::setCompression(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.compress = enabled;
}

void DataExporter::setOutputDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.outputDirectory = directory;
    
    // Create directory if it doesn't exist
    if (!std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
    }
}

void DataExporter::setFilenamePrefix(const std::string& prefix) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.filenamePrefix = prefix;
}

void DataExporter::addMetadata(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.metadata[key] = value;
}

void DataExporter::setProgressCallback(ExportProgressCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->progressCallback = callback;
}

DataExporter::ExportStats DataExporter::getExportStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

bool DataExporter::enableDataCompression(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.compress = enable;
    return true;
}

bool DataExporter::enableIncrementalExport(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // TODO: Implement incremental export
    return true;
}

bool DataExporter::enableExportVerification(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // TODO: Implement export verification
    return true;
}

bool DataExporter::validateExportData(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Basic validation - check if file exists and is not empty
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.seekg(0, std::ios::end);
    bool isValid = file.tellg() > 0;
    file.close();
    
    return isValid;
}

bool DataExporter::verifyExportIntegrity(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Simple integrity check - could be enhanced
    return validateExportData(filename);
}

bool DataExporter::exportWithAnalysis(const Brain& brain,
                                     const Environment& environment,
                                     const std::shared_ptr<Experiment>& experiment,
                                     bool includeDebugAnalysis,
                                     bool includePerformanceAnalysis) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string baseFilename = "research_dataset_with_analysis";
    
    // Export experiment
    bool experimentSuccess = exportExperiment(experiment, "");
    
    // Export brain state
    bool brainSuccess = exportBrainState(brain, "");
    
    // Export debug results if requested
    bool debugSuccess = true;
    if (includeDebugAnalysis) {
        // In real implementation, would create actual BrainInspector and MemoryProfiler
        // For now, placeholder
        debugSuccess = true;
    }
    
    // Export performance metrics if requested
    bool performanceSuccess = true;
    if (includePerformanceAnalysis) {
        // In real implementation, would collect actual performance metrics
        // For now, placeholder
        performanceSuccess = true;
    }
    
    return experimentSuccess && brainSuccess && debugSuccess && performanceSuccess;
}

bool DataExporter::exportForPublication(const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Set publication-specific configuration
    ExportConfig pubConfig = pImpl->config;
    pubConfig.format = ExportFormat::JSON;
    pubConfig.compress = true;
    pubConfig.includeTimestamps = true;
    
    // Add publication metadata
    pubConfig.metadata["dataset_type"] = "publication_ready";
    pubConfig.metadata["compliance"] = "research_standard";
    pubConfig.metadata["version"] = "1.0";
    
    // Initialize with publication config
    initialize(pubConfig);
    
    // Export with analysis for publication
    return true; // Placeholder
}

bool DataExporter::exportForReproducibility(const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Set reproducibility-specific configuration
    ExportConfig reproConfig = pImpl->config;
    reproConfig.format = ExportFormat::Binary;
    reproConfig.compress = false; // Important for reproducibility
    reproConfig.includeTimestamps = true;
    
    // Add reproducibility metadata
    reproConfig.metadata["dataset_type"] = "reproducible";
    reproConfig.metadata["random_seed"] = "42"; // Example
    reproConfig.metadata["version"] = "1.0";
    
    // Initialize with reproducibility config
    initialize(reproConfig);
    
    return true; // Placeholder
}

bool DataExporter::exportForArchiving(const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Set archiving-specific configuration
    ExportConfig archiveConfig = pImpl->config;
    archiveConfig.format = ExportFormat::HDF5;
    archiveConfig.compress = true;
    archiveConfig.includeTimestamps = true;
    
    // Add archiving metadata
    archiveConfig.metadata["dataset_type"] = "archival";
    archiveConfig.metadata["retention_policy"] = "10_years";
    archiveConfig.metadata["format"] = "long_term_storage";
    
    // Initialize with archiving config
    initialize(archiveConfig);
    
    return true; // Placeholder
}

} // namespace nlm
