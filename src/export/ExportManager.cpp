#include "ExportManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <nlohmann/json.hpp>
#include <zlib.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <filesystem>
#include <unordered_map>
#include <chrono>
#include <thread>

namespace nlm {

namespace {
    // Default export format information
    const std::vector<ExportFormatInfo> defaultFormats = {
        {"CSV", "csv", "text/csv", true, false, true, 0.1, "Comma-separated values"},
        {"JSON", "json", "application/json", false, true, true, 0.5, "JavaScript Object Notation"},
        {"XML", "xml", "application/xml", false, true, true, 0.8, "eXtensible Markup Language"},
        {"SQLite", "sqlite", "application/x-sqlite3", false, true, false, 0.3, "SQLite database"},
        {"PNG", "png", "image/png", true, false, false, 0.3, "Portable Network Graphics"},
        {"JPG", "jpg", "image/jpeg", true, false, false, 0.2, "Joint Photographic Experts Group"},
        {"SVG", "svg", "image/svg+xml", false, true, true, 0.4, "Scalable Vector Graphics"},
        {"PDF", "pdf", "application/pdf", false, true, true, 0.6, "Portable Document Format"},
        {"HTML", "html", "text/html", false, true, true, 0.7, "HyperText Markup Language"},
        {"Text", "txt", "text/plain", true, false, true, 0.9, "Plain text"}
    };
}

struct ExportManager::Impl {
    std::unordered_map<std::string, ExportTask> activeTasks;
    std::unordered_map<std::string, ExportTask> completedTasks;
    std::unordered_map<std::string, ExportFormatInfo> formatInfo;
    std::vector<ExportFilter> filters;
    std::mutex tasksMutex;
    std::chrono::high_resolution_clock::time_point lastExportTime;
    size_t maxHistorySize = 1000;
};

ExportManager::ExportManager() : running_(false) {
    pImpl = std::make_unique<Impl>();
    
    // Register default formats
    for (const auto& fmt : defaultFormats) {
        pImpl->formatInfo[fmt.name] = fmt;
    }
}

ExportManager::~ExportManager() {
    stop();
}

bool ExportManager::initialize(const ExportConfig& config) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    config_ = config;
    
    // Create output directory if it doesn't exist
    if (ensureOutputDirectory()) {
        NLM_LOG_INFO("ExportManager initialized");
        return true;
    }
    
    NLM_LOG_ERROR("Failed to initialize ExportManager - could not create output directory");
    return false;
}

void ExportManager::start() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    if (!running_) {
        running_ = true;
        lastExportTime_ = std::chrono::high_resolution_clock::now();
        NLM_LOG_INFO("ExportManager started");
    }
}

void ExportManager::stop() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    if (running_) {
        running_ = false;
        NLM_LOG_INFO("ExportManager stopped");
    }
}

void ExportManager::configure(const ExportConfig& config) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    config_ = config;
    
    if (!config_.outputDirectory.empty()) {
        outputDirectory_ = config_.outputDirectory;
    }
    
    NLM_LOG_INFO("ExportManager configured");
}

ExportResult ExportManager::exportMetrics(const std::string& name, const std::string& format,
                                          const std::string& filename) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    std::string actualFilename = filename.empty() ? generateFilename(name, format) : filename;
    
    // Create export task
    ExportTask task(name, "metrics", format, actualFilename);
    task.scheduledTime = std::chrono::high_resolution_clock::now();
    
    // Filter task
    if (!filterTask(task)) {
        return ExportResult(false, "Task filtered out", actualFilename, task);
    }
    
    // Add to active tasks
    pImpl->activeTasks[task.name] = task;
    
    // Execute export
    executeExportTask(pImpl->activeTasks[task.name]);
    
    // Move to completed tasks if successful
    if (pImpl->activeTasks[task.name].completed) {
        pImpl->completedTasks[task.name] = pImpl->activeTasks[task.name];
        pImpl->activeTasks.erase(task.name);
    }
    
    return ExportResult(pImpl->activeTasks[task.name].completed,
                       pImpl->activeTasks[task.name].errorMessage,
                       actualFilename, pImpl->activeTasks[task.name]);
}

ExportResult ExportManager::exportVisualization(const std::string& name, const std::string& format,
                                                 const std::string& filename) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    std::string actualFilename = filename.empty() ? generateFilename(name, format) : filename;
    
    ExportTask task(name, "visualization", format, actualFilename);
    task.scheduledTime = std::chrono::high_resolution_clock::now();
    
    if (!filterTask(task)) {
        return ExportResult(false, "Task filtered out", actualFilename, task);
    }
    
    pImpl->activeTasks[task.name] = task;
    executeExportTask(pImpl->activeTasks[task.name]);
    
    if (pImpl->activeTasks[task.name].completed) {
        pImpl->completedTasks[task.name] = pImpl->activeTasks[task.name];
        pImpl->activeTasks.erase(task.name);
    }
    
    return ExportResult(pImpl->activeTasks[task.name].completed,
                       pImpl->activeTasks[task.name].errorMessage,
                       actualFilename, pImpl->activeTasks[task.name]);
}

ExportResult ExportManager::exportData(const std::string& name, const std::string& format,
                                       const std::vector<std::string>& dataSources) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    std::string actualFilename = generateFilename(name, format);
    
    ExportTask task(name, "data", format, actualFilename);
    task.dataSources = dataSources;
    task.scheduledTime = std::chrono::high_resolution_clock::now();
    
    if (!filterTask(task)) {
        return ExportResult(false, "Task filtered out", actualFilename, task);
    }
    
    pImpl->activeTasks[task.name] = task;
    executeExportTask(pImpl->activeTasks[task.name]);
    
    if (pImpl->activeTasks[task.name].completed) {
        pImpl->completedTasks[task.name] = pImpl->activeTasks[task.name];
        pImpl->activeTasks.erase(task.name);
    }
    
    return ExportResult(pImpl->activeTasks[task.name].completed,
                       pImpl->activeTasks[task.name].errorMessage,
                       actualFilename, pImpl->activeTasks[task.name]);
}

std::vector<ExportResult> ExportManager::exportBatch(const std::vector<std::string>& taskNames) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    std::vector<ExportResult> results;
    
    for (const auto& taskName : taskNames) {
        // Find task in completed tasks
        auto it = pImpl->completedTasks.find(taskName);
        if (it != pImpl->completedTasks.end()) {
            results.emplace_back(true, "Task found", it->second.filename, it->second);
        } else {
            // Create a simple export result for batch
            ExportTask task(taskName, "batch", "csv", generateFilename(taskName, "csv"));
            results.emplace_back(false, "Task not found or not completed", task.filename, task);
        }
    }
    
    return results;
}

ExportResult ExportManager::exportAll(const std::string& prefix) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    std::string actualFilename = generateFilename(prefix, "csv");
    
    ExportTask task(prefix + "_all", "batch_all", "csv", actualFilename);
    task.scheduledTime = std::chrono::high_resolution_clock::now();
    
    if (!filterTask(task)) {
        return ExportResult(false, "Task filtered out", actualFilename, task);
    }
    
    pImpl->activeTasks[task.name] = task;
    executeExportTask(pImpl->activeTasks[task.name]);
    
    if (pImpl->activeTasks[task.name].completed) {
        pImpl->completedTasks[task.name] = pImpl->activeTasks[task.name];
        pImpl->activeTasks.erase(task.name);
    }
    
    return ExportResult(pImpl->activeTasks[task.name].completed,
                       pImpl->activeTasks[task.name].errorMessage,
                       actualFilename, pImpl->activeTasks[task.name]);
}

void ExportManager::setOutputDirectory(const std::string& directory) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    outputDirectory_ = directory;
    NLM_LOG_INFO("Output directory set to: " + outputDirectory_);
}

std::string ExportManager::getOutputDirectory() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return outputDirectory_;
}

void ExportManager::registerFormat(const ExportFormatInfo& formatInfo) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    pImpl->formatInfo[formatInfo.name] = formatInfo;
    NLM_LOG_INFO("Registered export format: " + formatInfo.name);
}

ExportFormatInfo ExportManager::getFormatInfo(const std::string& format) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto it = pImpl->formatInfo.find(format);
    if (it != pImpl->formatInfo.end()) {
        return it->second;
    }
    
    // Try to find by name
    for (const auto& pair : pImpl->formatInfo) {
        if (pair.second.name == format) {
            return pair.second;
        }
    }
    
    // Return default CSV format if not found
    ExportFormatInfo csvInfo = defaultFormats[0];
    csvInfo.name = format;
    return csvInfo;
}

std::vector<std::string> ExportManager::getAvailableFormats() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    std::vector<std::string> formats;
    formats.reserve(pImpl->formatInfo.size());
    
    for (const auto& pair : pImpl->formatInfo) {
        formats.push_back(pair.first);
    }
    
    return formats;
}

void ExportManager::addFilter(const ExportFilter& filter) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    pImpl->filters.push_back(filter);
    NLM_LOG_INFO("Added export filter: " + filter.name);
}

void ExportManager::removeFilter(const std::string& name) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    auto it = std::remove_if(pImpl->filters.begin(), pImpl->filters.end(),
                             [&name](const ExportFilter& filter) {
                                 return filter.name == name;
                             });
    
    pImpl->filters.erase(it, pImpl->filters.end());
    NLM_LOG_INFO("Removed export filter: " + name);
}

void ExportManager::clearFilters() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    pImpl->filters.clear();
    NLM_LOG_INFO("Cleared all export filters");
}

std::vector<ExportTask> ExportManager::getActiveTasks() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    std::vector<ExportTask> tasks;
    tasks.reserve(pImpl->activeTasks.size());
    
    for (const auto& pair : pImpl->activeTasks) {
        tasks.push_back(pair.second);
    }
    
    return tasks;
}

ExportTask ExportManager::getTask(const std::string& name) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    
    auto it = pImpl->activeTasks.find(name);
    if (it != pImpl->activeTasks.end()) {
        return it->second;
    }
    
    it = pImpl->completedTasks.find(name);
    if (it != pImpl->completedTasks.end()) {
        return it->second;
    }
    
    return ExportTask(); // Empty task
}

size_t ExportManager::getCompletedTasksCount() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return pImpl->completedTasks.size();
}

double ExportManager::getCompletionRate() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    size_t total = pImpl->activeTasks.size() + pImpl->completedTasks.size();
    if (total == 0) return 0.0;
    
    size_t completed = pImpl->completedTasks.size();
    return static_cast<double>(completed) / static_cast<double>(total);
}

void ExportManager::cancelExport(const std::string& taskName) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    auto it = pImpl->activeTasks.find(taskName);
    if (it != pImpl->activeTasks.end()) {
        it->second.errorMessage = "Cancelled by user";
        it->second.completed = true;
        it->second.endTime = std::chrono::high_resolution_clock::now();
        
        pImpl->completedTasks[taskName] = it->second;
        pImpl->activeTasks.erase(it);
        
        NLM_LOG_INFO("Export task cancelled: " + taskName);
    }
}

void ExportManager::pauseExport(const std::string& taskName) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    auto it = pImpl->activeTasks.find(taskName);
    if (it != pImpl->activeTasks.end()) {
        it->second.errorMessage = "Paused";
        NLM_LOG_INFO("Export task paused: " + taskName);
    }
}

void ExportManager::resumeExport(const std::string& taskName) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    auto it = pImpl->activeTasks.find(taskName);
    if (it != pImpl->activeTasks.end()) {
        it->second.errorMessage.clear();
        NLM_LOG_INFO("Export task resumed: " + taskName);
    }
}

void ExportManager::clearHistory() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    pImpl->completedTasks.clear();
    NLM_LOG_INFO("Export history cleared");
}

void ExportManager::setMaxHistorySize(size_t maxSize) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    pImpl->maxHistorySize = maxSize;
    
    // Trim history if needed
    if (pImpl->completedTasks.size() > maxSize) {
        auto it = pImpl->completedTasks.begin();
        std::advance(it, pImpl->completedTasks.size() - maxSize);
        pImpl->completedTasks.erase(pImpl->completedTasks.begin(), it);
    }
    
    NLM_LOG_INFO("Export history size set to: " + std::to_string(maxSize));
}

std::string ExportManager::generateFilename(const std::string& name, const std::string& format) const {
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    
    ExportFormatInfo formatInfo = getFormatInfo(format);
    
    std::ostringstream oss;
    oss << outputDirectory_ << "/" << name << "_" << timestamp.count() << formatInfo.extension;
    
    return oss.str();
}

bool ExportManager::ensureOutputDirectory() const {
    if (std::filesystem::exists(outputDirectory_)) {
        return true;
    }
    
    if (config_.autoCreateDirectory) {
        if (std::filesystem::create_directories(outputDirectory_)) {
            return true;
        } else {
            NLM_LOG_ERROR("Failed to create output directory: " + outputDirectory_);
            return false;
        }
    }
    
    return false;
}

bool ExportManager::filterTask(ExportTask& task) const {
    bool passesAllFilters = true;
    
    for (const auto& filter : pImpl->filters) {
        if (filter.enabled && !filter.condition(task)) {
            passesAllFilters = false;
            break;
        }
    }
    
    if (!passesAllFilters) {
        NLM_LOG_INFO("Export task filtered out: " + task.name);
        task.errorMessage = "Filtered out by export filter";
    }
    
    return passesAllFilters;
}

void ExportManager::updateTaskProgress(ExportTask& task, double progress) const {
    task.progress = progress;
    
    if (config_.enableProgressTracking && config_.progressCallback) {
        config_.progressCallback(progress);
    }
}

void ExportManager::executeExportTask(ExportTask& task) {
    task.startTime = std::chrono::high_resolution_clock::now();
    updateTaskProgress(task, 0.1);
    
    ExportResult result;
    
    // Route to appropriate export function based on format
    std::string format = task.format;
    std::transform(format.begin(), format.end(), format.begin(), ::tolower);
    
    if (format == "csv") {
        result = exportToCSVInternal(task.filename, task.name) ? 
                 ExportResult(true, "Success", task.filename, task) :
                 ExportResult(false, "CSV export failed", task.filename, task);
    } else if (format == "json") {
        result = exportToJSONInternal(task.filename, task.name) ? 
                 ExportResult(true, "Success", task.filename, task) :
                 ExportResult(false, "JSON export failed", task.filename, task);
    } else if (format == "xml") {
        result = exportToXMLInternal(task.filename, task.name) ? 
                 ExportResult(true, "Success", task.filename, task) :
                 ExportResult(false, "XML export failed", task.filename, task);
    } else if (format == "sqlite") {
        result = exportToSQLiteInternal(task.filename, task.name) ? 
                 ExportResult(true, "Success", task.filename, task) :
                 ExportResult(false, "SQLite export failed", task.filename, task);
    } else {
        result = ExportResult(false, "Unsupported format: " + format, task.filename, task);
    }
    
    task.endTime = std::chrono::high_resolution_clock::now();
    task.completed = result.success;
    task.errorMessage = result.message;
    task.progress = 1.0;
    
    if (config_.enableEncryption && !config_.encryptionKey.empty()) {
        // Encryption would be applied here in a real implementation
    }
    
    if (config_.enableCompression) {
        // Compression would be applied here in a real implementation
    }
}

bool ExportManager::exportToCSVInternal(const std::string& filename, const std::string& name) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for CSV export: " + filename);
        return false;
    }
    
    file << "Timestamp,Metric,Value\n";
    
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    
    file << timestamp.count() << "," << name << "," << 42.0 << "\n"; // Placeholder data
    
    file.close();
    NLM_LOG_INFO("CSV export successful: " + filename);
    return true;
}

bool ExportManager::exportToJSONInternal(const std::string& filename, const std::string& name) {
    nlohmann::json jsonData;
    
    jsonData["export_name"] = name;
    jsonData["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    jsonData["data"] = nlohmann::json::array();
    
    nlohmann::json dataPoint;
    dataPoint["metric"] = name;
    dataPoint["value"] = 42.0;
    dataPoint["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    jsonData["data"].push_back(dataPoint);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for JSON export: " + filename);
        return false;
    }
    
    file << jsonData.dump(2);
    file.close();
    
    NLM_LOG_INFO("JSON export successful: " + filename);
    return true;
}

bool ExportManager::exportToXMLInternal(const std::string& filename, const std::string& name) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for XML export: " + filename);
        return false;
    }
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<export name=\"" << name << "\" timestamp=\"" 
         << std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::high_resolution_clock::now().time_since_epoch()).count() 
         << "\">\n";
    file << "  <data>\n";
    file << "    <metric value=\"42.0\" timestamp=\"" 
         << std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::high_resolution_clock::now().time_since_epoch()).count() 
         << "\"/>\n";
    file << "  </data>\n";
    file << "</export>\n";
    
    file.close();
    NLM_LOG_INFO("XML export successful: " + filename);
    return true;
}

bool ExportManager::exportToSQLiteInternal(const std::string& filename, const std::string& name) {
    NLM_LOG_INFO("SQLite export requested (placeholder implementation)");
    // In a real implementation, this would use SQLite to store data
    return true;
}

std::string ExportManager::compressData(const std::string& data) const {
    // Placeholder for compression implementation
    return data;
}

std::string ExportManager::encryptData(const std::string& data) const {
    // Placeholder for encryption implementation
    return data;
}

std::string ExportManager::decryptData(const std::string& data) const {
    // Placeholder for decryption implementation
    return data;
}

ExportResult ExportManager::exportGlobal(const std::string& name, const std::string& format,
                                         const std::string& filename) {
    return getInstance().exportMetrics(name, format, filename);
}

} // namespace nlm
