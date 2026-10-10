#include "ExportRenderer.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <ctime>

namespace nlm {

ExportRenderer::ExportRenderer()
    : compressionLevel_(6), exportTheme_("default"), timeFormat_("iso"), dateFormat_("medium"),
      sessionId_(""), progressCallback_(nullptr) {
    initializeDefaultOptions();
    initializeEnabledFormats();
}

void ExportRenderer::initializeDefaultOptions() {
    defaultOptions_.format = ExportFormat::ASCII;
    defaultOptions_.scope = ExportScope::CurrentView;
    defaultOptions_.includeMetadata = true;
    defaultOptions_.compress = false;
    defaultOptions_.title = "NLM Visualization Export";
    defaultOptions_.description = "Neural Network Visualization Data Export";
    defaultOptions_.author = "NLM System";
    defaultOptions_.timestamp = std::chrono::system_clock::now();
}

void ExportRenderer::initializeEnabledFormats() {
    enabledFormats_[ExportFormat::ASCII] = true;
    enabledFormats_[ExportFormat::CSV] = true;
    enabledFormats_[ExportFormat::JSON] = true;
    enabledFormats_[ExportFormat::HTML] = true;
    enabledFormats_[ExportFormat::XML] = true;
    enabledFormats_[ExportFormat::TXT] = true;
    enabledFormats_[ExportFormat::PDF] = false; // External dependency
    enabledFormats_[ExportFormat::PNG] = false; // External dependency
    enabledFormats_[ExportFormat::SVG] = false; // External dependency
}

bool ExportRenderer::exportData(const ExportOptions& options, const std::string& sessionId) {
    // Validate options
    if (!validateExportOptions(options)) {
        lastError_ = "Invalid export options";
        return false;
    }
    
    setSessionId(sessionId);
    
    std::string content;
    std::string fileExtension = getFileExtension(options.format);
    
    switch (options.format) {
        case ExportFormat::ASCII:
            content = exportToASCII(collectActivityData());
            break;
        case ExportFormat::CSV:
            content = exportToCSV(collectActivityData());
            break;
        case ExportFormat::JSON:
            content = exportToJSON(collectActivityData());
            break;
        case ExportFormat::HTML:
            content = exportToHTML(collectActivityData());
            break;
        case ExportFormat::XML:
            content = exportToXML(collectActivityData());
            break;
        case ExportFormat::TXT:
            content = exportToTXT(collectActivityData());
            break;
        default:
            lastError_ = "Unsupported export format";
            return false;
    }
    
    // Add metadata if requested
    if (options.includeMetadata) {
        content = generateMetadata(options) + "\n\n" + content;
    }
    
    // Compress if requested
    if (options.compress) {
        content = compressData(content);
    }
    
    // Generate filename
    std::string filename = generateFilename(options);
    
    // Write to file
    if (!writeToFile(content, filename)) {
        lastError_ = "Failed to write export file: " + filename;
        return false;
    }
    
    // Generate report
    generateExportReport(options);
    
    return true;
}

bool ExportRenderer::exportBrainStructure(const ExportOptions& options, const Brain& brain) {
    // Implementation for brain structure export
    // This would collect structural data from the brain
    std::vector<ActivityData> brainData;
    
    // Extract structural data
    auto regions = brain.getRegions();
    for (size_t i = 0; i < regions.size() && i < 10; ++i) {
        const auto& region = regions[i];
        
        ActivityData data;
        data.neuronId = "region_" + std::to_string(region->getId().index());
        data.activity = static_cast<float>(region->getTotalNeuronCount());
        data.timestamp = brain.getSimulationTime();
        data.step = brain.getCurrentStep();
        data.regionId = region->getId().index() ? "left" : "right";
        data.populationId = "";
        
        brainData.push_back(data);
    }
    
    // Export based on format
    std::string content;
    switch (options.format) {
        case ExportFormat::ASCII:
            content = exportToASCII(brainData);
            break;
        case ExportFormat::CSV:
            content = exportToCSV(brainData);
            break;
        case ExportFormat::JSON:
            content = exportToJSON(brainData);
            break;
        default:
            lastError_ = "Brain structure export not supported for this format";
            return false;
    }
    
    if (!writeToFile(content, generateFilename(options))) {
        lastError_ = "Failed to write brain structure export";
        return false;
    }
    
    return true;
}

bool ExportRenderer::exportActivityData(const ExportOptions& options, const std::vector<ActivityData>& activityData) {
    std::string content;
    
    switch (options.format) {
        case ExportFormat::ASCII:
            content = exportToASCII(activityData);
            break;
        case ExportFormat::CSV:
            content = exportToCSV(activityData);
            break;
        case ExportFormat::JSON:
            content = exportToJSON(activityData);
            break;
        case ExportFormat::HTML:
            content = exportToHTML(activityData);
            break;
        case ExportFormat::XML:
            content = exportToXML(activityData);
            break;
        case ExportFormat::TXT:
            content = exportToTXT(activityData);
            break;
        default:
            lastError_ = "Activity data export not supported for this format";
            return false;
    }
    
    if (!writeToFile(content, generateFilename(options))) {
        lastError_ = "Failed to write activity data export";
        return false;
    }
    
    return true;
}

bool ExportRenderer::exportMemoryData(const ExportOptions& options, const std::vector<MemoryEntry>& memoryData) {
    // Convert memory entries to activity data for export
    std::vector<ActivityData> memoryActivity;
    
    for (const auto& entry : memoryData) {
        ActivityData data;
        data.neuronId = entry.id;
        data.activity = entry.strength;
        data.timestamp = entry.timestamp;
        data.step = entry.step;
        data.regionId = "memory_" + std::to_string(static_cast<int>(entry.type));
        data.populationId = "";
        
        memoryActivity.push_back(data);
    }
    
    return exportActivityData(options, memoryActivity);
}

bool ExportRenderer::exportProgressData(const ExportOptions& options, const std::vector<ProgressData>& progressData) {
    std::string content;
    
    switch (options.format) {
        case ExportFormat::ASCII:
            content = exportToASCII(std::vector<ActivityData>()); // Progress data handling
            break;
        case ExportFormat::CSV:
            content = exportToCSV(std::vector<ActivityData>());
            break;
        case ExportFormat::JSON:
            content = exportToJSON(std::vector<ActivityData>());
            break;
        default:
            lastError_ = "Progress data export not supported for this format";
            return false;
    }
    
    if (!writeToFile(content, generateFilename(options))) {
        lastError_ = "Failed to write progress data export";
        return false;
    }
    
    return true;
}

std::string ExportRenderer::exportToASCII(const std::vector<ActivityData>& activityData) const {
    std::stringstream ss;
    
    ss << "NLM Visualization Data Export" << std::endl;
    ss << "=============================" << std::endl;
    ss << "Format: ASCII Text" << std::endl;
    ss << "Generated: " << formatDate(std::chrono::system_clock::now()) << std::endl;
    ss << "Session ID: " << sessionId_ << std::endl;
    ss << std::endl;
    
    ss << "Activity Data:" << std::endl;
    ss << "----------------" << std::endl;
    
    for (size_t i = 0; i < activityData.size(); ++i) {
        const auto& data = activityData[i];
        ss << "Entry " << i << ":" << std::endl;
        ss << "  Neuron ID: " << data.neuronId << std::endl;
        ss << "  Activity: " << data.activity << std::endl;
        ss << "  Timestamp: " << data.timestamp << std::endl;
        ss << "  Step: " << data.step << std::endl;
        ss << "  Region ID: " << data.regionId << std::endl;
        ss << "  Population ID: " << data.populationId << std::endl;
        ss << std::endl;
    }
    
    ss << "Total Entries: " << activityData.size() << std::endl;
    
    return ss.str();
}

std::string ExportRenderer::exportToCSV(const std::vector<ActivityData>& activityData) const {
    std::stringstream ss;
    
    // Write CSV header
    ss << "NeuronID,Activity,Timestamp,Step,RegionID,PopulationID" << std::endl;
    
    // Write data rows
    for (const auto& data : activityData) {
        ss << data.neuronId << ",";
        ss << data.activity << ",";
        ss << data.timestamp << ",";
        ss << data.step << ",";
        ss << data.regionId << ",";
        ss << data.populationId << ",";
        ss << std::endl;
    }
    
    return ss.str();
}

std::string ExportRenderer::exportToJSON(const std::vector<ActivityData>& activityData) const {
    std::stringstream ss;
    
    ss << "{" << std::endl;
    ss << "  \"format\": \"NLM Activity Data\"," << std::endl;
    ss << "  \"generated\": \"" << formatDate(std::chrono::system_clock::now()) << "\"," << std::endl;
    ss << "  \"sessionId\": \"" << sessionId_ << "\"," << std::endl;
    ss << "  \"entries\": [" << std::endl;
    
    for (size_t i = 0; i < activityData.size(); ++i) {
        const auto& data = activityData[i];
        
        ss << "    {" << std::endl;
        ss << "      \"neuronId\": \"" << data.neuronId << "\"," << std::endl;
        ss << "      \"activity\": " << data.activity << "," << std::endl;
        ss << "      \"timestamp\": " << data.timestamp << ",\" << std::endl;
        ss << "      \"step\": " << data.step << "," << std::endl;
        ss << "      \"regionId\": \"" << data.regionId << "\"," << std::endl;
        ss << "      \"populationId\": \"" << data.populationId << "\"" << std::endl;
        
        if (i < activityData.size() - 1) {
            ss << "    },";
        } else {
            ss << "    }";
        }
        
        ss << std::endl;
    }
    
    ss << "  ]," << std::endl;
    ss << "  \"totalEntries\": " << activityData.size() << std::endl;
    ss << "}" << std::endl;
    
    return ss.str();
}

std::string ExportRenderer::exportToHTML(const std::vector<ActivityData>& activityData) const {
    std::stringstream ss;
    
    ss << "<!DOCTYPE html>" << std::endl;
    ss << "<html>" << std::endl;
    ss << "<head>" << std::endl;
    ss << "  <title>NLM Activity Data Export</title>" << std::endl;
    ss << "  <style>" << std::endl;
    ss << "    body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    ss << "    table { border-collapse: collapse; width: 100%; }" << std::endl;
    ss << "    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
    ss << "    th { background-color: #f2f2f2; }" << std::endl;
    ss << "    tr:nth-child(even) { background-color: #f9f9f9; }" << std::endl;
    ss << "  </style>" << std::endl;
    ss << "</head>" << std::endl;
    ss << "<body>" << std::endl;
    ss << "  <h1>NLM Activity Data Export</h1>" << std::endl;
    ss << "  <p><strong>Generated:</strong> " << formatDate(std::chrono::system_clock::now()) << "</p>" << std::endl;
    ss << "  <p><strong>Session ID:</strong> " << sessionId_ << "</p>" << std::endl;
    ss << "  <table>" << std::endl;
    ss << "    <thead>" << std::endl;
    ss << "      <tr>" << std::endl;
    ss << "        <th>Neuron ID</th>" << std::endl;
    ss << "        <th>Activity</th>" << std::endl;
    ss << "        <th>Timestamp</th>" << std::endl;
    ss << "        <th>Step</th>" << std::endl;
    ss << "        <th>Region ID</th>" << std::endl;
    ss << "        <th>Population ID</th>" << std::endl;
    ss << "      </tr>" << std::endl;
    ss << "    </thead>" << std::endl;
    ss << "    <tbody>" << std::endl;
    
    for (const auto& data : activityData) {
        ss << "      <tr>" << std::endl;
        ss << "        <td>" << escapeHTML(data.neuronId) << "</td>" << std::endl;
        ss << "        <td>" << data.activity << "</td>" << std::endl;
        ss << "        <td>" << data.timestamp << "</td>" << std::endl;
        ss << "        <td>" << data.step << "</td>" << std::endl;
        ss << "        <td>" << escapeHTML(data.regionId) << "</td>" << std::endl;
        ss << "        <td>" << escapeHTML(data.populationId) << "</td>" << std::endl;
        ss << "      </tr>" << std::endl;
    }
    
    ss << "    </tbody>" << std::endl;
    ss << "  </table>" << std::endl;
    ss << "  <p><strong>Total Entries:</strong> " << activityData.size() << "</p>" << std::endl;
    ss << "</body>" << std::endl;
    ss << "</html>" << std::endl;
    
    return ss.str();
}

std::string ExportRenderer::exportToXML(const std::vector<ActivityData>& activityData) const {
    std::stringstream ss;
    
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<nlm_export>" << std::endl;
    ss << "  <metadata>" << std::endl;
    ss << "    <format>ASCII</format>" << std::endl;
    ss << "    <generated>" << formatDate(std::chrono::system_clock::now()) << "</generated>" << std::endl;
    ss << "    <sessionId>" << sessionId_ << "</sessionId>" << std::endl;
    ss << "  </metadata>" << std::endl;
    ss << "  <activity_data>" << std::endl;
    
    for (const auto& data : activityData) {
        ss << "    <entry>" << std::endl;
        ss << "      <neuronId>" << escapeXML(data.neuronId) << "</neuronId>" << std::endl;
        ss << "      <activity>" << data.activity << "</activity>" << std::endl;
        ss << "      <timestamp>" << data.timestamp << "</timestamp>" << std::endl;
        ss << "      <step>" << data.step << "</step>" << std::endl;
        ss << "      <regionId>" << escapeXML(data.regionId) << "</regionId>" << std::endl;
        ss << "      <populationId>" << escapeXML(data.populationId) << "</populationId>" << std::endl;
        ss << "    </entry>" << std::endl;
    }
    
    ss << "  </activity_data>" << std::endl;
    ss << "  <statistics>" << std::endl;
    ss << "    <total_entries>" << activityData.size() << "</total_entries>" << std::endl;
    ss << "  </statistics>" << std::endl;
    ss << "</nlm_export>" << std::endl;
    
    return ss.str();
}

std::string ExportRenderer::exportToTXT(const std::vector<ActivityData>& activityData) const {
    std::stringstream ss;
    
    ss << "NLM Visualization Data" << std::endl;
    ss << "=======================" << std::endl;
    ss << std::endl;
    ss << "This file contains neural activity data exported from the NLM system." << std::endl;
    ss << "Session ID: " << sessionId_ << std::endl;
    ss << "Generated: " << formatDate(std::chrono::system_clock::now()) << std::endl;
    ss << std::endl;
    ss << "Data Summary:" << std::endl;
    ss << "-------------" << std::endl;
    ss << "Total Entries: " << activityData.size() << std::endl;
    ss << "Data Types: " << (activityData.empty() ? "None" : "Activity Data") << std::endl;
    ss << std::endl;
    
    if (!activityData.empty()) {
        ss << "Sample Data:" << std::endl;
        ss << "-----------" << std::endl;
        for (size_t i = 0; i < std::min<size_t>(10, activityData.size()); ++i) {
            const auto& data = activityData[i];
            ss << "Entry " << i << ":" << std::endl;
            ss << "  Neuron: " << data.neuronId << std::endl;
            ss << "  Activity Level: " << data.activity << std::endl;
            ss << "  Time: " << data.timestamp << std::endl;
            ss << "  Step: " << data.step << std::endl;
            ss << std::endl;
        }
    }
    
    ss << "End of export file." << std::endl;
    
    return ss.str();
}

std::string ExportRenderer::compressData(const std::string& data) const {
    // Simple compression simulation - in real implementation would use actual compression
    // For demonstration, we'll just add a header
    std::stringstream ss;
    ss << "COMPRESSED:" << std::endl;
    ss << "// " << data.length() << " bytes of data" << std::endl;
    ss << "// Compression level: " << compressionLevel_ << std::endl;
    ss << std::endl;
    ss << data;
    return ss.str();
}

std::string ExportRenderer::generateFilename(const ExportOptions& options) const {
    std::stringstream ss;
    
    // Get file extension
    std::string extension = getFileExtension(options.format);
    
    // Create base filename
    std::string baseName = "nlm_export_";
    
    // Add timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time_t, &tm_buf);
    
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y%m%d_%H%M%S", &tm_buf);
    
    baseName += std::string(timeStr);
    baseName += "_" + sessionId_;
    
    // Add extension
    baseName += "." + extension;
    
    return baseName;
}

std::string ExportRenderer::getFileExtension(ExportFormat format) const {
    switch (format) {
        case ExportFormat::ASCII:
            return "txt";
        case ExportFormat::CSV:
            return "csv";
        case ExportFormat::JSON:
            return "json";
        case ExportFormat::HTML:
            return "html";
        case ExportFormat::XML:
            return "xml";
        case ExportFormat::TXT:
            return "txt";
        case ExportFormat::PDF:
            return "pdf";
        case ExportFormat::PNG:
            return "png";
        case ExportFormat::SVG:
            return "svg";
        default:
            return "dat";
    }
}

bool ExportRenderer::writeToFile(const std::string& content, const std::string& filePath) const {
    std::ofstream file(filePath, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    
    return true;
}

bool ExportRenderer::createDirectory(const std::string& path) const {
    return std::filesystem::create_directories(path);
}

bool ExportRenderer::validateExportOptions(const ExportOptions& options) const {
    if (options.outputPath.empty()) {
        lastError_ = "Output path is empty";
        return false;
    }
    
    if (!enabledFormats_.count(options.format) || !enabledFormats_.at(options.format)) {
        lastError_ = "Export format not enabled";
        return false;
    }
    
    return true;
}

bool ExportRenderer::checkFileWritePermissions(const std::string& filePath) const {
    // Check if we can write to the file
    std::ofstream testFile(filePath, std::ios::out);
    bool canWrite = testFile.good();
    testFile.close();
    
    // Clean up if file was created
    if (canWrite) {
        std::remove(filePath.c_str());
    }
    
    return canWrite;
}

bool ExportRenderer::validateDataConsistency() const {
    // Validate that collected data is consistent
    // This is a simplified validation
    return true;
}

void ExportRenderer::generateExportReport(const ExportOptions& options) const {
    // Generate a simple report of the export operation
    // In a real implementation, this would create a more detailed report
    std::cout << "Export completed successfully:" << std::endl;
    std::cout << "  Format: " << static_cast<int>(options.format) << std::endl;
    std::cout << "  Scope: " << static_cast<int>(options.scope) << std::endl;
    std::cout << "  Size: " << getExportSize(options) << " bytes" << std::endl;
    std::cout << "  Location: " << options.outputPath << std::endl;
}

std::string ExportRenderer::getExportStatistics() const {
    std::stringstream ss;
    ss << "Export Statistics:" << std::endl;
    ss << "=================" << std::endl;
    ss << "Session ID: " << sessionId_ << std::endl;
    ss << "Total Exports: N/A" << std::endl;
    ss << "Enabled Formats: N/A" << std::endl;
    ss << "Last Error: " << lastError_ << std::endl;
    
    return ss.str();
}

double ExportRenderer::getExportSize(const ExportOptions& options) const {
    // Estimate export size based on data
    double estimatedSize = 0.0;
    
    // Get data
    std::vector<ActivityData> activityData = collectActivityData();
    
    // Estimate based on data size
    for (const auto& data : activityData) {
        estimatedSize += sizeof(data.neuronId) + sizeof(data.activity) + 
                        sizeof(data.timestamp) + sizeof(data.step) + 
                        sizeof(data.regionId) + sizeof(data.populationId);
    }
    
    // Add overhead for headers and formatting
    estimatedSize *= 2.0; // Rough estimate of 2x overhead
    
    return estimatedSize;
}

std::string ExportRenderer::formatTimestamp(Timestamp timestamp) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << timestamp;
    return ss.str();
}

std::string ExportRenderer::formatDate(std::chrono::system_clock::time_point point) const {
    auto time_t = std::chrono::system_clock::to_time_t(point);
    std::tm tm_buf;
    gmtime_r(&time_t, &tm_buf);
    
    char dateStr[32];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M:%S", &tm_buf);
    
    return std::string(dateStr);
}

std::string ExportRenderer::escapeHTML(const std::string& text) const {
    std::string escaped = text;
    std::replace(escaped.begin(), escaped.end(), '&', "&amp;");
    std::replace(escaped.begin(), escaped.end(), '<', "&lt;");
    std::replace(escaped.begin(), escaped.end(), '>', "&gt;");
    std::replace(escaped.begin(), escaped.end(), '"', "&quot;");
    std::replace(escaped.begin(), escaped.end(), '\'', "&apos;");
    return escaped;
}

std::string ExportRenderer::escapeXML(const std::string& text) const {
    std::string escaped = text;
    std::replace(escaped.begin(), escaped.end(), '&', "&amp;");
    std::replace(escaped.begin(), escaped.end(), '<', "&lt;");
    std::replace(escaped.begin(), escaped.end(), '>', "&gt;");
    std::replace(escaped.begin(), escaped.end(), '"', "&quot;");
    std::replace(escaped.begin(), escaped.end(), '\'', "&apos;");
    return escaped;
}

std::string ExportRenderer::escapeCSV(const std::string& text) const {
    std::string escaped = text;
    std::replace(escaped.begin(), escaped.end(), '"', '""');
    return '"' + escaped + '"';
}

std::vector<ExportRenderer::ActivityData> ExportRenderer::collectActivityData() const {
    // Collect activity data from the visualization system
    // This is a placeholder - in a real implementation, you would
    // integrate with the actual activity collection system
    std::vector<ActivityData> data;
    
    // Add some sample data
    ActivityData sample1;
    sample1.neuronId = "neuron_001";
    sample1.activity = 0.75f;
    sample1.timestamp = 12.345f;
    sample1.step = 123;
    sample1.regionId = "cortex";
    sample1.populationId = "exc_pop";
    
    data.push_back(sample1);
    
    return data;
}

std::vector<ExportRenderer::MemoryEntry> ExportRenderer::collectMemoryData() const {
    // Collect memory data from the visualization system
    // This is a placeholder
    return std::vector<MemoryEntry>();
}

std::vector<ExportRenderer::ProgressData> ExportRenderer::collectProgressData() const {
    // Collect progress data from the visualization system
    // This is a placeholder
    return std::vector<ProgressData>();
}

std::vector<ExportRenderer::ConnectionInfo> ExportRenderer::collectConnectionData() const {
    // Collect connection data from the visualization system
    // This is a placeholder
    return std::vector<ConnectionInfo>();
}

std::string ExportRenderer::generateMetadata(const ExportOptions& options) const {
    std::stringstream ss;
    
    ss << "Metadata:" << std::endl;
    ss << "========" << std::endl;
    ss << "Title: " << options.title << std::endl;
    ss << "Description: " << options.description << std::endl;
    ss << "Author: " << options.author << std::endl;
    ss << "Generated: " << formatDate(options.timestamp) << std::endl;
    ss << "Format: " << static_cast<int>(options.format) << std::endl;
    ss << "Scope: " << static_cast<int>(options.scope) << std::endl;
    ss << "Include Metadata: " << (options.includeMetadata ? "Yes" : "No") << std::endl;
    ss << "Compressed: " << (options.compress ? "Yes" : "No") << std::endl;
    ss << "Session ID: " << sessionId_ << std::endl;
    ss << std::endl;
    
    ss << "Custom Fields:" << std::endl;
    for (const auto& field : options.customFields) {
        ss << "  " << field.first << ": " << field.second << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm
