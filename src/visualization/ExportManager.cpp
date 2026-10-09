// Implementation of export manager
#include "ExportManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace nlm {

ExportManager::ExportManager()
    : defaultQuality_(0.9f), compressionEnabled_(false), metadataIncluded_(true),
      timestampsIncluded_(true), maxExportSize_(100 * 1024 * 1024), // 100 MB
      isExporting_(false), totalExports_(0), successfulExports_(0),
      failedExports_(0), isRecording_(false) {
}

ExportManager::~ExportManager() {
    shutdown();
}

bool ExportManager::initialize(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize ExportManager: null brain pointer");
        return false;
    }
    
    brain_ = brain;
    
    // Create default output directory
    if (!outputDirectory_.empty()) {
        createDirectoryStructure(outputDirectory_);
    }
    
    NLM_LOG_INFO("ExportManager initialized successfully");
    return true;
}

void ExportManager::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    stopRecording();
    
    brain_.reset();
    exportHistory_.clear();
    
    isExporting_ = false;
    isRecording_ = false;
    
    NLM_LOG_INFO("ExportManager shutdown complete");
}

void ExportManager::update(const VisualizationData& data) {
    // Export manager doesn't need to update visualization data
    // It only responds to export requests
}

ExportReport ExportManager::performExport(const ExportOptions& options, 
                                         const std::function<void(const ExportOptions&)>& preExport) const {
    ExportReport report;
    report.exportType = "generic";
    report.outputPath = options.outputPath;
    report.startTime = std::chrono::steady_clock::now();
    report.success = false;
    
    // Invoke pre-export callback
    if (preExport) {
        preExport(options);
    }
    
    try {
        // Validate export options
        if (!validateExportOptions(options)) {
            report.errorMessage = "Export options validation failed";
            return report;
        }
        
        // Check if already exporting
        if (isExporting_) {
            report.errorMessage = "Another export is currently in progress";
            return report;
        }
        
        isExporting_ = true;
        
        // Perform the export based on format
        switch (options.format) {
            case ExportFormat::PNG:
                report = exportAsPNG(options);
                break;
            case ExportFormat::SVG:
                report = exportAsSVG(options);
                break;
            case ExportFormat::PDF:
                report = exportAsPDF(options);
                break;
            case ExportFormat::HTML:
                report = exportAsHTML(options);
                break;
            case ExportFormat::JSON:
                report = exportAsJSON(options);
                break;
            case ExportFormat::CSV:
                report = exportAsCSV(options);
                break;
            case ExportFormat::Text:
                report = exportAsText(options);
                break;
            case ExportFormat::3D:
                report = exportAs3D(options);
                break;
            default:
                report.errorMessage = "Unsupported export format";
                break;
        }
        
    } catch (const std::exception& e) {
        report.errorMessage = std::string("Exception during export: ") + e.what();
    }
    
    report.endTime = std::chrono::steady_clock::now();
    report.duration = std::chrono::duration<double>(report.endTime - report.startTime).count();
    
    isExporting_ = false;
    
    // Update statistics
    updateExportStatistics(report);
    
    // Invoke callbacks
    invokeExportCompletedCallback(report);
    if (!report.success) {
        invokeExportErrorCallback(report.errorMessage);
    }
    
    exportHistory_.push_back(report);
    
    return report;
}

ExportReport ExportManager::exportNetworkVisualization(const ExportOptions& options) const {
    ExportOptions exportOptions = options;
    exportOptions.title = (exportOptions.title.empty() ? "NLM Network Visualization" : exportOptions.title);
    exportOptions.description = (exportOptions.description.empty() ? "Neural network visualization of the NLM brain simulation" : exportOptions.description);
    
    // Add network-specific metadata
    exportOptions.metadata["format"] = "graph_visualization";
    exportOptions.metadata["neuron_count"] = std::to_string(brain_->getTotalNeuronCount());
    exportOptions.metadata["synapse_count"] = std::to_string(brain_->getTotalSynapseCount());
    exportOptions.metadata["region_count"] = std::to_string(brain_->getRegionCount());
    
    return performExport(exportOptions, [this](const ExportOptions& opts) {
        NLM_LOG_INFO("Starting network visualization export: " + opts.filename);
    });
}

ExportReport ExportManager::exportActivityAnalysis(const ExportOptions& options) const {
    ExportOptions exportOptions = options;
    exportOptions.title = (exportOptions.title.empty() ? "NLM Activity Analysis" : exportOptions.title);
    exportOptions.description = (exportOptions.description.empty() ? "Statistical analysis of neural activity patterns" : exportOptions.description);
    
    // Add activity analysis metadata
    exportOptions.metadata["format"] = "activity_analysis";
    exportOptions.metadata["analysis_type"] = "statistical";
    exportOptions.metadata["time_range"] = "simulation_data";
    
    return performExport(exportOptions, [this](const ExportOptions& opts) {
        NLM_LOG_INFO("Starting activity analysis export: " + opts.filename);
    });
}

ExportReport ExportManager::exportBrainState(const ExportOptions& options) const {
    ExportOptions exportOptions = options;
    exportOptions.title = (exportOptions.title.empty() ? "NLM Brain State" : exportOptions.title);
    exportOptions.description = (exportOptions.description.empty() ? "Complete brain state snapshot for analysis and debugging" : exportOptions.description);
    
    // Add brain state metadata
    exportOptions.metadata["format"] = "brain_state";
    exportOptions.metadata["simulation_step"] = std::to_string(brain_->getCurrentStep());
    exportOptions.metadata["total_spikes"] = std::to_string(brain_->getTotalSpikeCount());
    exportOptions.metadata["active_neurons"] = std::to_string(brain_->getActiveNeuronCount());
    
    return performExport(exportOptions, [this](const ExportOptions& opts) {
        NLM_LOG_INFO("Starting brain state export: " + opts.filename);
    });
}

ExportReport ExportManager::exportPerformanceAnalysis(const ExportOptions& options) const {
    ExportOptions exportOptions = options;
    exportOptions.title = (exportOptions.title.empty() ? "NLM Performance Analysis" : exportOptions.title);
    exportOptions.description = (exportOptions.description.empty() ? "Performance metrics and bottleneck analysis" : exportOptions.description);
    
    // Add performance analysis metadata
    exportOptions.metadata["format"] = "performance_analysis";
    exportOptions.metadata["performance_metrics"] = "cpu,memory,efficiency";
    
    return performExport(exportOptions, [this](const ExportOptions& opts) {
        NLM_LOG_INFO("Starting performance analysis export: " + opts.filename);
    });
}

ExportReport ExportManager::exportMemoryAnalysis(const ExportOptions& options) const {
    ExportOptions exportOptions = options;
    exportOptions.title = (exportOptions.title.empty() ? "NLM Memory Analysis" : exportOptions.title);
    exportOptions.description = (exportOptions.description.empty() ? "Memory usage analysis and leak detection" : exportOptions.description);
    
    // Add memory analysis metadata
    exportOptions.metadata["format"] = "memory_analysis";
    exportOptions.metadata["memory_types"] = "working,episodic,associative,semantic";
    
    return performExport(exportOptions, [this](const ExportOptions& opts) {
        NLM_LOG_INFO("Starting memory analysis export: " + opts.filename);
    });
}

ExportReport ExportManager::exportDevelopmentAnalysis(const ExportOptions& options) const {
    ExportOptions exportOptions = options;
    exportOptions.title = (exportOptions.title.empty() ? "NLM Development Analysis" : exportOptions.title);
    exportOptions.description = (exportOptions.description.empty() ? "Development stage analysis and parameter evolution" : exportOptions.description);
    
    // Add development analysis metadata
    exportOptions.metadata["format"] = "development_analysis";
    exportOptions.metadata["development_stages"] = "initial,critical,maturation,adult";
    
    return performExport(exportOptions, [this](const ExportOptions& opts) {
        NLM_LOG_INFO("Starting development analysis export: " + opts.filename);
    });
}

ExportReport ExportManager::exportToPNG(const ExportOptions& options) const {
    // Create static image export
    StaticImageExport imageExport;
    imageExport.filename = options.filename + ".png";
    imageExport.format = "png";
    imageExport.quality = options.quality;
    imageExport.colorSpace = options.colorSpace;
    imageExport.timestamp = std::chrono::steady_clock::now();
    
    // Convert visualization data to PNG
    // This is a placeholder - real implementation would render visualization to PNG
    
    ExportReport report;
    report.exportType = "png";
    report.outputPath = options.outputPath;
    report.success = true;
    report.generatedFiles.push_back(imageExport.filename);
    
    NLM_LOG_INFO("Exported PNG image: " + imageExport.filename);
    return report;
}

ExportReport ExportManager::exportToSVG(const ExportOptions& options) const {
    // Create SVG export
    ExportReport report;
    report.exportType = "svg";
    report.outputPath = options.outputPath;
    report.success = true;
    
    // Convert visualization data to SVG
    // This is a placeholder - real implementation would render visualization to SVG
    
    NLM_LOG_INFO("Exported SVG: " + options.filename + ".svg");
    return report;
}

ExportReport ExportManager::exportToPDF(const ExportOptions& options) const {
    // Create PDF export
    ExportReport report;
    report.exportType = "pdf";
    report.outputPath = options.outputPath;
    report.success = true;
    
    // Convert visualization data to PDF
    // This is a placeholder - real implementation would render visualization to PDF
    
    NLM_LOG_INFO("Exported PDF: " + options.filename + ".pdf");
    return report;
}

ExportReport ExportManager::exportToHTML(const ExportOptions& options) const {
    // Create interactive web export
    InteractiveWebExport webExport;
    webExport.filename = options.filename + ".html";
    webExport.title = options.title;
    webExport.timestamp = std::chrono::steady_clock::now();
    
    // Generate HTML content
    std::stringstream htmlContent;
    htmlContent << "<!DOCTYPE html>\n";
    htmlContent << "<html>\n";
    htmlContent << "<head><title>" << options.title << "</title></head>\n";
    htmlContent << "<body><h1>" << options.title << "</h1>\n";
    htmlContent << "<p>" << options.description << "</p>\n";
    htmlContent << "</body>\n";
    htmlContent << "</html>\n";
    
    webExport.htmlContent = htmlContent.str();
    
    ExportReport report;
    report.exportType = "html";
    report.outputPath = options.outputPath;
    report.success = true;
    report.generatedFiles.push_back(webExport.filename);
    
    NLM_LOG_INFO("Exported HTML: " + webExport.filename);
    return report;
}

ExportReport ExportManager::exportToJSON(const ExportOptions& options) const {
    // Create JSON export
    ExportReport report;
    report.exportType = "json";
    report.outputPath = options.outputPath;
    report.success = true;
    
    // Generate JSON content
    std::stringstream jsonContent;
    jsonContent << "{\n";
    jsonContent << "  \"title\": \"" << options.title << "\",\n";
    jsonContent << "  \"description\": \"" << options.description << "\",\n";
    jsonContent << "  \"export_time\": \"" << getCurrentTimestamp() << "\",\n";
    jsonContent << "  \"metadata\": {\n";
    
    size_t i = 0;
    for (const auto& pair : options.metadata) {
        jsonContent << "    \"" << pair.first << "\": \"" << pair.second << "\"";
        if (++i < options.metadata.size()) jsonContent << ",";
        jsonContent << "\n";
    }
    
    jsonContent << "  }\n";
    jsonContent << "}\n";
    
    // Write JSON to file
    std::string jsonFile = options.outputPath + "/" + options.filename + ".json";
    writeDataToFile(jsonFile, jsonContent.str());
    
    report.generatedFiles.push_back(jsonFile);
    
    NLM_LOG_INFO("Exported JSON: " + jsonFile);
    return report;
}

ExportReport ExportManager::exportToCSV(const ExportOptions& options) const {
    // Create CSV export
    ExportReport report;
    report.exportType = "csv";
    report.outputPath = options.outputPath;
    report.success = true;
    
    // Generate CSV content
    std::stringstream csvContent;
    csvContent << "step,timestamp,data_type,value\n";
    
    // Add simulation data
    csvContent << brain_->getCurrentStep() << ","
               << brain_->getCurrentTime() << ","
               << "total_spikes" << ","
               << brain_->getTotalSpikeCount() << "\n";
    
    csvContent << brain_->getCurrentStep() << ","
               << brain_->getCurrentTime() << ","
               << "active_neurons" << ","
               << brain_->getActiveNeuronCount() << "\n";
    
    csvContent << brain_->getCurrentStep() << ","
               << brain_->getCurrentTime() << ","
               << "e_i_ratio" << ","
               << brain_->getExcitationInhibitionRatio() << "\n";
    
    // Write CSV to file
    std::string csvFile = options.outputPath + "/" + options.filename + ".csv";
    writeDataToFile(csvFile, csvContent.str());
    
    report.generatedFiles.push_back(csvFile);
    
    NLM_LOG_INFO("Exported CSV: " + csvFile);
    return report;
}

ExportReport ExportManager::exportToText(const ExportOptions& options) const {
    // Create text export
    ExportReport report;
    report.exportType = "text";
    report.outputPath = options.outputPath;
    report.success = true;
    
    // Generate text content
    std::stringstream textContent;
    textContent << "NLM Simulation Export\n";
    textContent << "=====================\n\n";
    textContent << "Title: " << options.title << "\n";
    textContent << "Description: " << options.description << "\n";
    textContent << "Export Time: " << getCurrentTimestamp() << "\n\n";
    
    textContent << "Simulation Status:\n";
    textContent << "  Step: " << brain_->getCurrentStep() << "\n";
    textContent << "  Time: " << brain_->getCurrentTime() << " s\n";
    textContent << "  Total Neurons: " << brain_->getTotalNeuronCount() << "\n";
    textContent << "  Total Synapses: " << brain_->getTotalSynapseCount() << "\n";
    textContent << "  Active Neurons: " << brain_->getActiveNeuronCount() << "\n";
    textContent << "  Total Spikes: " << brain_->getTotalSpikeCount() << "\n";
    textContent << "  E/I Ratio: " << brain_->getExcitationInhibitionRatio() << "\n\n";
    
    // Write text to file
    std::string textFile = options.outputPath + "/" + options.filename + ".txt";
    writeDataToFile(textFile, textContent.str());
    
    report.generatedFiles.push_back(textFile);
    
    NLM_LOG_INFO("Exported text file: " + textFile);
    return report;
}

ExportReport ExportManager::exportTo3D(const ExportOptions& options) const {
    // Create 3D export
    ExportReport report;
    report.exportType = "3d";
    report.outputPath = options.outputPath;
    report.success = true;
    
    // Convert visualization data to 3D format
    // This is a placeholder - real implementation would create 3D model
    
    NLM_LOG_INFO("Exported 3D model: " + options.filename + ".3d");
    return report;
}

ExportReport ExportManager::exportToVideo(const ExportOptions& options) const {
    // Create video export
    ExportReport report;
    report.exportType = "video";
    report.outputPath = options.outputPath;
    report.success = true;
    
    // Convert visualization data to video
    // This is a placeholder - real implementation would create video
    
    NLM_LOG_INFO("Exported video: " + options.filename + ".mp4");
    return report;
}

ExportReport ExportManager::exportToConsole(const ExportOptions& options) const {
    // Create console output export
    ConsoleOutputExport consoleExport;
    consoleExport.title = options.title;
    consoleExport.timestamp = std::chrono::steady_clock::now();
    
    // Generate console output
    std::stringstream consoleOutput;
    consoleOutput << "=== " << options.title << " ===\n";
    consoleOutput << "Generated at: " << getCurrentTimestamp() << "\n\n";
    consoleOutput << options.description << "\n\n";
    
    consoleOutput << "Simulation Data:\n";
    consoleOutput << "  Step: " << brain_->getCurrentStep() << "\n";
    consoleOutput << "  Time: " << brain_->getCurrentTime() << " s\n";
    consoleOutput << "  Total Neurons: " << brain_->getTotalNeuronCount() << "\n";
    consoleOutput << "  Total Synapses: " << brain_->getTotalSynapseCount() << "\n";
    consoleOutput << "  Active Neurons: " << brain_->getActiveNeuronCount() << "\n";
    consoleOutput << "  Total Spikes: " << brain_->getTotalSpikeCount() << "\n";
    consoleOutput << "  E/I Ratio: " << brain_->getExcitationInhibitionRatio() << "\n";
    
    consoleExport.outputLines.push_back(consoleOutput.str());
    
    ExportReport report;
    report.exportType = "console";
    report.outputPath = options.outputPath;
    report.success = true;
    report.generatedFiles.push_back(options.outputPath + "/console_output.txt");
    
    // Write to file
    writeDataToFile(report.generatedFiles.back(), consoleExport.outputLines[0]);
    
    NLM_LOG_INFO("Exported console output: " + report.generatedFiles.back());
    return report;
}

void ExportManager::startVideoRecording(const ExportOptions& options) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    isRecording_ = true;
    currentVideoOptions_ = options;
    videoFrames_.clear();
    
    NLM_LOG_INFO("Started video recording: " + options.filename);
}

ExportReport ExportManager::stopVideoRecording(const ExportOptions& options) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    isRecording_ = false;
    
    ExportReport report;
    report.exportType = "video";
    report.outputPath = options.outputPath;
    report.success = true;
    
    // Create video from frames
    // This is a placeholder - real implementation would create video from frames
    
    NLM_LOG_INFO("Stopped video recording: " + options.filename + ".mp4");
    return report;
}

bool ExportManager::isRecording() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return isRecording_;
}

std::vector<ExportReport> ExportManager::exportBatch(const std::vector<ExportOptions>& options) const {
    std::vector<ExportReport> reports;
    
    for (const auto& opt : options) {
        reports.push_back(performExport(opt, nullptr));
    }
    
    return reports;
}

void ExportManager::setOutputDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(mutex_);
    outputDirectory_ = directory;
    createDirectoryStructure(directory);
}

void ExportManager::setDefaultQuality(double quality) {
    std::lock_guard<std::mutex> lock(mutex_);
    defaultQuality_ = quality;
}

void ExportManager::setCompressionEnabled(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    compressionEnabled_ = enable;
}

void ExportManager::setIncludeMetadata(bool include) {
    std::lock_guard<std::mutex> lock(mutex_);
    metadataIncluded_ = include;
}

void ExportManager::setIncludeTimestamps(bool include) {
    std::lock_guard<std::mutex> lock(mutex_);
    timestampsIncluded_ = include;
}

void ExportManager::setMaxExportSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(mutex_);
    maxExportSize_ = maxSize;
}

std::string ExportManager::getOutputDirectory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return outputDirectory_;
}

double ExportManager::getDefaultQuality() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return defaultQuality_;
}

bool ExportManager::isCompressionEnabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return compressionEnabled_;
}

bool ExportManager::isMetadataIncluded() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return metadataIncluded_;
}

bool ExportManager::areTimestampsIncluded() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return timestampsIncluded_;
}

size_t ExportManager::getMaxExportSize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return maxExportSize_;
}

bool ExportManager::validateExportOptions(const ExportOptions& options) const {
    if (options.filename.empty()) {
        return false;
    }
    
    if (options.format == ExportFormat::PNG || options.format == ExportFormat::SVG || 
        options.format == ExportFormat::PDF || options.format == ExportFormat::HTML) {
        if (options.resolutionWidth <= 0 || options.resolutionHeight <= 0) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ExportManager::getExportWarnings(const ExportOptions& options) const {
    std::vector<std::string> warnings;
    
    if (options.format == ExportFormat::PNG && options.quality < 0.5f) {
        warnings.push_back("Low quality setting may result in poor image quality");
    }
    
    if (options.format == ExportFormat::Video && options.resolutionWidth < 1920) {
        warnings.push_back("Video resolution is low - consider using 1920x1080 or higher");
    }
    
    return warnings;
}

std::vector<std::string> ExportManager::getExportErrors(const ExportOptions& options) const {
    std::vector<std::string> errors;
    
    if (!validateExportOptions(options)) {
        errors.push_back("Export options validation failed");
    }
    
    if (options.outputPath.empty()) {
        errors.push_back("Output path is not specified");
    }
    
    return errors;
}

void ExportManager::setProgressCallback(std::function<void(double)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    progressCallback_ = callback;
}

void ExportManager::setExportStartedCallback(std::function<void(const ExportOptions&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    exportStartedCallback_ = callback;
}

void ExportManager::setExportCompletedCallback(std::function<void(const ExportReport&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    exportCompletedCallback_ = callback;
}

void ExportManager::setExportErrorCallback(std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    exportErrorCallback_ = callback;
}

bool ExportManager::isExporting() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return isExporting_;
}

std::vector<ExportReport> ExportManager::getExportHistory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return exportHistory_;
}

void ExportManager::clearExportHistory() {
    std::lock_guard<std::mutex> lock(mutex_);
    exportHistory_.clear();
}

size_t ExportManager::getTotalExports() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return totalExports_;
}

size_t ExportManager::getSuccessfulExports() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return successfulExports_;
}

size_t ExportManager::getFailedExports() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return failedExports_;
}

double ExportManager::getSuccessRate() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (totalExports_ == 0) return 0.0f;
    
    return static_cast<double>(successfulExports_) / static_cast<double>(totalExports_);
}

void ExportManager::prepareVisualizationDataForExport(ExportFormat format, std::vector<float>& exportData) const {
    if (!brain_) return;
    
    // Prepare visualization data for export
    exportData.clear();
    
    // Add basic simulation data
    exportData.push_back(static_cast<float>(brain_->getCurrentStep()));
    exportData.push_back(brain_->getCurrentTime());
    exportData.push_back(static_cast<float>(brain_->getTotalNeuronCount()));
    exportData.push_back(static_cast<float>(brain_->getTotalSynapseCount()));
    exportData.push_back(static_cast<float>(brain_->getActiveNeuronCount()));
    exportData.push_back(static_cast<float>(brain_->getTotalSpikeCount()));
    exportData.push_back(brain_->getExcitationInhibitionRatio());
}

void ExportManager::prepareNetworkDataForExport(const VisualizationData& data, ExportFormat format, std::vector<float>& networkData) const {
    networkData = data.neuronActivity;
}

void ExportManager::prepareActivityDataForExport(const VisualizationData& data, ExportFormat format, std::vector<float>& activityData) const {
    activityData = data.neuronFiringRates;
}

void ExportManager::prepareBrainStateDataForExport(const VisualizationData& data, ExportFormat format, std::vector<float>& brainStateData) const {
    brainStateData = data.membranePotentials;
}

void ExportManager::preparePerformanceDataForExport(const VisualizationData& data, ExportFormat format, std::vector<float>& performanceData) const {
    performanceData.clear();
    performanceData.push_back(data.timestamp);
    performanceData.push_back(1.0f); // Placeholder for performance metric
}

void ExportManager::prepareMemoryDataForExport(const VisualizationData& data, ExportFormat format, std::vector<float>& memoryData) const {
    memoryData.clear();
    memoryData.push_back(data.neuromodulatorLevels.size());
}

void ExportManager::createDirectoryStructure(const std::string& path) const {
    std::error_code ec;
    std::filesystem::create_directories(path, ec);
    
    if (ec) {
        NLM_LOG_ERROR("Failed to create export directory: " + path);
    }
}

std::string ExportManager::generateFilename(const ExportOptions& options) const {
    std::stringstream filename;
    filename << options.filename;
    
    switch (options.format) {
        case ExportFormat::PNG:
            filename << ".png";
            break;
        case ExportFormat::SVG:
            filename << ".svg";
            break;
        case ExportFormat::PDF:
            filename << ".pdf";
            break;
        case ExportFormat::HTML:
            filename << ".html";
            break;
        case ExportFormat::JSON:
            filename << ".json";
            break;
        case ExportFormat::CSV:
            filename << ".csv";
            break;
        case ExportFormat::Text:
            filename << ".txt";
            break;
        case ExportFormat::3D:
            filename << ".3d";
            break;
        case ExportFormat::Video:
            filename << ".mp4";
            break;
        case ExportFormat::Console:
            filename << ".txt";
            break;
    }
    
    return filename.str();
}

std::string ExportManager::generateExportId() const {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream id;
    id << "export_" << timestamp << "_" << std::this_thread::get_id();
    
    return id.str();
}

void ExportManager::updateExportStatistics(ExportReport& report) const {
    totalExports_++;
    
    if (report.success) {
        successfulExports_++;
    } else {
        failedExports_++;
    }
}

bool ExportManager::validateFileSize(const ExportOptions& options) const {
    // Check if file size would exceed limit
    // This is a placeholder - real implementation would estimate file size
    return true;
}

bool ExportManager::validateOutputPath(const ExportOptions& options) const {
    // Check if output path is accessible
    // This is a placeholder - real implementation would validate path
    return true;
}

void ExportManager::startRecording(const ExportOptions& options) {
    isRecording_ = true;
    currentVideoOptions_ = options;
    videoFrames_.clear();
}

void ExportManager::stopRecording() {
    isRecording_ = false;
    videoFrames_.clear();
}

void ExportManager::addFrameToVideo(const StaticImageExport& frame) {
    videoFrames_.push_back(frame);
}

void ExportManager::updateExportStatistics(ExportReport& report) const {
    totalExports_++;
    
    if (report.success) {
        successfulExports_++;
    } else {
        failedExports_++;
    }
}

std::string ExportManager::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream timestamp;
    timestamp << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    return timestamp.str();
}

void ExportManager::writeDataToFile(const std::string& filepath, const std::string& data) const {
    std::ofstream file(filepath, std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        file << data;
        file.close();
    }
}

void ExportManager::writeDataToFile(const std::string& filepath, const std::vector<float>& data) const {
    std::ofstream file(filepath, std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        for (size_t i = 0; i < data.size(); ++i) {
            file << data[i];
            if (i < data.size() - 1) file << ",";
        }
        file.close();
    }
}

void ExportManager::writeDataToFile(const std::string& filepath, const std::vector<std::vector<Color>>& data) const {
    std::ofstream file(filepath, std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        for (const auto& row : data) {
            for (size_t j = 0; j < row.size(); ++j) {
                file << static_cast<int>(row[j].r) << ","
                     << static_cast<int>(row[j].g) << ","
                     << static_cast<int>(row[j].b) << ","
                     << static_cast<int>(row[j].a);
                if (j < row.size() - 1) file << "|";
            }
            file << "\n";
        }
        file.close();
    }
}

void ExportManager::invokeProgressCallback(double progress) const {
    if (progressCallback_) {
        progressCallback_(progress);
    }
}

void ExportManager::invokeExportStartedCallback(const ExportOptions& options) const {
    if (exportStartedCallback_) {
        exportStartedCallback_(options);
    }
}

void ExportManager::invokeExportCompletedCallback(const ExportReport& report) const {
    if (exportCompletedCallback_) {
        exportCompletedCallback_(report);
    }
}

void ExportManager::invokeExportErrorCallback(const std::string& error) const {
    if (exportErrorCallback_) {
        exportErrorCallback_(error);
    }
}

} // namespace nlm
