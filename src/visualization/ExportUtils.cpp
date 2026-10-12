#include "ExportUtils.hpp"
#include "../core/Logger.hpp"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <cmath>

namespace nlm {

struct ExportUtils::Impl {
    // Export configuration
    std::string exportDirectory;
    int compressionLevel;
    size_t maxFileSize;
    bool enableProgressReporting;
    OutputFormat defaultExportFormat;
    
    // Export tracking
    std::map<std::string, ExportStatus> operationStatus;
    std::map<std::string, ExportProgress> operationProgress;
    std::queue<ExportProgress> exportQueue;
    
    // Export statistics
    size_t totalExports;
    size_t successfulExports;
    size_t failedExports;
    double totalExportTime;
    
    // Video recording
    bool isRecordingVideo;
    std::string currentVideoFile;
    VideoConfig currentVideoConfig;
    std::thread videoRecordingThread;
    bool videoThreadRunning;
    
    // WebSocket
    bool isWebSocketConnected;
    std::string websocketHost;
    int websocketPort;
    std::string websocketPath;
    std::thread websocketThread;
    std::queue<std::string> websocketMessageQueue;
    bool websocketThreadRunning;
    
    // Synchronization
    mutable std::mutex exportMutex;
    
    Impl() : compressionLevel(6), maxFileSize(100 * 1024 * 1024), // 100 MB
             enableProgressReporting(true), defaultExportFormat(OutputFormat::JSON),
             totalExports(0), successfulExports(0), failedExports(0), totalExportTime(0.0),
             isRecordingVideo(false), videoThreadRunning(false),
             isWebSocketConnected(false), websocketThreadRunning(false) {}
};

ExportUtils::ExportUtils() : pImpl(std::make_unique<Impl>()) {}
ExportUtils::~ExportUtils() {
    stopVideoRecording();
    disconnectFromWebSocket();
}

bool ExportUtils::initialize() {
    try {
        // Create export directory if it doesn't exist
        std::filesystem::create_directories(pImpl->exportDirectory);
        
        Logger::getInstance().logInfo("Export utilities initialized");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to initialize export utilities: " + std::string(e.what()));
        return false;
    }
}

bool ExportUtils::exportToJSON(const std::string& filename, const std::string& data) {
    try {
        std::string fullPath = pImpl->exportDirectory + "/" + filename;
        if (fullPath.size() > pImpl->maxFileSize) {
            Logger::getInstance().logError("File too large: " + fullPath);
            return false;
        }
        
        // Parse existing data to maintain structure
        nlohmann::json jsonData = nlohmann::json::parse(data);
        
        // Write to file
        std::ofstream file(fullPath);
        if (!file.is_open()) {
            Logger::getInstance().logError("Failed to open file for writing: " + fullPath);
            return false;
        }
        
        file << std::setw(2) << jsonData;
        file.close();
        
        // Update statistics
        {
            std::lock_guard<std::mutex> lock(pImpl->exportMutex);
            pImpl->totalExports++;
            pImpl->successfulExports++;
            auto startTime = std::chrono::high_resolution_clock::now();
            auto endTime = std::chrono::high_resolution_clock::now();
            pImpl->totalExportTime += std::chrono::duration<double>(endTime - startTime).count();
        }
        
        Logger::getInstance().logInfo("JSON export completed: " + fullPath);
        return true;
        
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to export to JSON: " + std::string(e.what()));
        return false;
    }
}

bool ExportUtils::exportToCSV(const std::string& filename, const std::string& data) {
    try {
        std::string fullPath = pImpl->exportDirectory + "/" + filename;
        if (fullPath.size() > pImpl->maxFileSize) {
            Logger::getInstance().logError("File too large: " + fullPath);
            return false;
        }
        
        std::ofstream file(fullPath);
        if (!file.is_open()) {
            Logger::getInstance().logError("Failed to open file for writing: " + fullPath);
            return false;
        }
        
        // Write CSV data (data is expected to be in CSV format)
        file << data;
        file.close();
        
        // Update statistics
        {
            std::lock_guard<std::mutex> lock(pImpl->exportMutex);
            pImpl->totalExports++;
            pImpl->successfulExports++;
            auto startTime = std::chrono::high_resolution_clock::now();
            auto endTime = std::chrono::high_resolution_clock::now();
            pImpl->totalExportTime += std::chrono::duration<double>(endTime - startTime).count();
        }
        
        Logger::getInstance().logInfo("CSV export completed: " + fullPath);
        return true;
        
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to export to CSV: " + std::string(e.what()));
        return false;
    }
}

bool ExportUtils::exportToBinary(const std::string& filename, const std::string& data) {
    try {
        std::string fullPath = pImpl->exportDirectory + "/" + filename;
        if (fullPath.size() > pImpl->maxFileSize) {
            Logger::getInstance().logError("File too large: " + fullPath);
            return false;
        }
        
        std::ofstream file(fullPath, std::ios::binary);
        if (!file.is_open()) {
            Logger::getInstance().logError("Failed to open file for writing: " + fullPath);
            return false;
        }
        
        // Write binary data
        file.write(data.c_str(), data.size());
        file.close();
        
        // Update statistics
        {
            std::lock_guard<std::mutex> lock(pImpl->exportMutex);
            pImpl->totalExports++;
            pImpl->successfulExports++;
            auto startTime = std::chrono::high_resolution_clock::now();
            auto endTime = std::chrono::high_resolution_clock::now();
            pImpl->totalExportTime += std::chrono::duration<double>(endTime - startTime).count();
        }
        
        Logger::getInstance().logInfo("Binary export completed: " + fullPath);
        return true;
        
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to export to binary: " + std::string(e.what()));
        return false;
    }
}

bool ExportUtils::exportToText(const std::string& filename, const std::string& data) {
    try {
        std::string fullPath = pImpl->exportDirectory + "/" + filename;
        
        std::ofstream file(fullPath);
        if (!file.is_open()) {
            Logger::getInstance().logError("Failed to open file for writing: " + fullPath);
            return false;
        }
        
        file << data;
        file.close();
        
        // Update statistics
        {
            std::lock_guard<std::mutex> lock(pImpl->exportMutex);
            pImpl->totalExports++;
            pImpl->successfulExports++;
            auto startTime = std::chrono::high_resolution_clock::now();
            auto endTime = std::chrono::high_resolution_clock::now();
            pImpl->totalExportTime += std::chrono::duration<double>(endTime - startTime).count();
        }
        
        Logger::getInstance().logInfo("Text export completed: " + fullPath);
        return true;
        
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to export to text: " + std::string(e.what()));
        return false;
    }
}

bool ExportUtils::exportToHTML(const std::string& filename, const std::string& data) {
    try {
        std::string fullPath = pImpl->exportDirectory + "/" + filename;
        
        std::ofstream file(fullPath);
        if (!file.is_open()) {
            Logger::getInstance().logError("Failed to open file for writing: " + fullPath);
            return false;
        }
        
        // Generate HTML page
        file << "<!DOCTYPE html>" << std::endl;
        file << "<html>" << std::endl;
        file << "<head>" << std::endl;
        file << "    <title>NLM Visualization Export</title>" << std::endl;
        file << "    <style>" << std::endl;
        file << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
        file << "        .container { max-width: 1200px; margin: 0 auto; }" << std::endl;
        file << "        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }" << std::endl;
        file << "        .data { margin-top: 20px; }" << std::endl;
        file << "        pre { background-color: #f8f8f8; padding: 15px; border-radius: 5px; overflow-x: auto; }" << std::endl;
        file << "    </style>" << std::endl;
        file << "</head>" << std::endl;
        file << "<body>" << std::endl;
        file << "    <div class=\"container\">" << std::endl;
        file << "        <div class=\"header\">" << std::endl;
        file << "            <h1>NLM Visualization Export</h1>" << std::endl;
        file << "            <p>Exported at: " << getCurrentTimestamp() << "</p>" << std::endl;
        file << "        </div>" << std::endl;
        file << "        <div class=\"data\">" << std::endl;
        file << "            <h2>Exported Data</h2>" << std::endl;
        file << "            <pre>" << data << "</pre>" << std::endl;
        file << "        </div>" << std::endl;
        file << "    </div>" << std::endl;
        file << "</body>" << std::endl;
        file << "</html>" << std::endl;
        
        file.close();
        
        // Update statistics
        {
            std::lock_guard<std::mutex> lock(pImpl->exportMutex);
            pImpl->totalExports++;
            pImpl->successfulExports++;
            auto startTime = std::chrono::high_resolution_clock::now();
            auto endTime = std::chrono::high_resolution_clock::now();
            pImpl->totalExportTime += std::chrono::duration<double>(endTime - startTime).count();
        }
        
        Logger::getInstance().logInfo("HTML export completed: " + fullPath);
        return true;
        
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to export to HTML: " + std::string(e.what()));
        return false;
    }
}

bool ExportUtils::exportVisualization(const std::string& filename, OutputFormat format) {
    try {
        // This would typically export the current visualization state
        // For now, create a simple placeholder implementation
        std::string data = "{" + "\"exportType\": \"visualization\"," + 
                          "\"timestamp\": " + std::to_string(getCurrentTimestamp()) + ","
                          "\"format\": " + std::to_string(static_cast<int>(format)) + "}";
        
        switch (format) {
            case OutputFormat::JSON:
                return exportToJSON(filename, data);
            case OutputFormat::CSV:
                return exportToCSV(filename, data);
            case OutputFormat::Binary:
                return exportToBinary(filename, data);
            case OutputFormat::Text:
                return exportToText(filename, data);
            case OutputFormat::HTML:
                return exportToHTML(filename, data);
            default:
                return exportToText(filename, data);
        }
        
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to export visualization: " + std::string(e.what()));
        return false;
    }
}

bool ExportUtils::startVideoRecording(const std::string& filename, const VideoConfig& config) {
    try {
        pImpl->isRecordingVideo = true;
        pImpl->currentVideoFile = filename;
        pImpl->currentVideoConfig = config;
        
        // Start video recording thread (placeholder)
        pImpl->videoThreadRunning = true;
        pImpl->videoRecordingThread = std::thread(&ExportUtils::videoRecordingWorker, this, filename, config);
        
        Logger::getInstance().logInfo("Video recording started: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to start video recording: " + std::string(e.what()));
        return false;
    }
}

void ExportUtils::stopVideoRecording() {
    if (pImpl->isRecordingVideo) {
        pImpl->isRecordingVideo = false;
        if (pImpl->videoThreadRunning) {
            pImpl->videoRecordingThread.join();
            pImpl->videoThreadRunning = false;
        }
        Logger::getInstance().logInfo("Video recording stopped");
    }
}

bool ExportUtils::connectToWebSocket(const WebSocketConfig& config) {
    try {
        pImpl->isWebSocketConnected = true;
        pImpl->websocketHost = config.host;
        pImpl->websocketPort = config.port;
        pImpl->websocketPath = config.path;
        
        // Start WebSocket thread (placeholder)
        pImpl->websocketThreadRunning = true;
        pImpl->websocketThread = std::thread(&ExportUtils::websocketWorker, this);
        
        Logger::getInstance().logInfo("Connected to WebSocket: " + config.host + ":" + std::to_string(config.port));
        return true;
        
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to connect to WebSocket: " + std::string(e.what()));
        return false;
    }
}

void ExportUtils::disconnectFromWebSocket() {
    if (pImpl->isWebSocketConnected) {
        pImpl->isWebSocketConnected = false;
        if (pImpl->websocketThreadRunning) {
            pImpl->websocketThreadRunning = false;
            pImpl->websocketThread.join();
        }
        Logger::getInstance().logInfo("Disconnected from WebSocket");
    }
}

bool ExportUtils::sendToWebSocket(const std::string& message) {
    if (pImpl->isWebSocketConnected) {
        pImpl->websocketMessageQueue.push(message);
        return true;
    }
    return false;
}

ExportStatus ExportUtils::getExportStatus(const std::string& operationId) const {
    std::lock_guard<std::mutex> lock(pImpl->exportMutex);
    auto it = pImpl->operationStatus.find(operationId);
    return (it != pImpl->operationStatus.end()) ? it->second : ExportStatus::Pending;
}

std::vector<ExportProgress> ExportUtils::getExportQueue() const {
    std::lock_guard<std::mutex> lock(pImpl->exportMutex);
    std::vector<ExportProgress> queue;
    queue.reserve(pImpl->exportQueue.size());
    
    auto temp = std::move(pImpl->exportQueue);
    while (!temp.empty()) {
        queue.push_back(temp.front());
        temp.pop();
    }
    
    return queue;
}

void ExportUtils::cancelExport(const std::string& operationId) {
    std::lock_guard<std::mutex> lock(pImpl->exportMutex);
    auto it = pImpl->operationStatus.find(operationId);
    if (it != pImpl->operationStatus.end()) {
        it->second = ExportStatus::Cancelled;
    }
}

void ExportUtils::waitForExport(const std::string& operationId) {
    // Wait for export to complete (simplified)
    while (getExportStatus(operationId) != ExportStatus::Completed &&
           getExportStatus(operationId) != ExportStatus::Failed) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void ExportUtils::setExportDirectory(const std::string& directory) {
    pImpl->exportDirectory = directory;
}

void ExportUtils::setCompressionLevel(int level) {
    pImpl->compressionLevel = std::max(0, std::min(9, level));
}

void ExportUtils::setMaxFileSize(size_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

void ExportUtils::enableProgressReporting(bool enable) {
    pImpl->enableProgressReporting = enable;
}

void ExportUtils::setExportFormat(OutputFormat format) {
    pImpl->defaultExportFormat = format;
}

std::string ExportUtils::prepareBrainData(const Brain& brain) {
    // Prepare comprehensive brain data for export
    nlohmann::json jsonData;
    
    jsonData["timestamp"] = getCurrentTimestamp();
    jsonData["brainInfo"]["neuronCount"] = brain.getTotalNeuronCount();
    jsonData["brainInfo"]["synapseCount"] = brain.getTotalSynapseCount();
    jsonData["brainInfo"]["regionCount"] = brain.getRegionCount();
    jsonData["brainInfo"]["firingRate"] = brain.getAverageFiringRate();
    
    // Add more brain data...
    
    return jsonData.dump(2);
}

std::string ExportUtils::prepareActivityData(const Brain& brain, Timestamp startTime, Timestamp endTime) {
    // Prepare activity data
    nlohmann::json jsonData;
    jsonData["timestamp"] = getCurrentTimestamp();
    jsonData["type"] = "activity_data";
    
    // Add activity data...
    
    return jsonData.dump(2);
}

std::string ExportUtils::prepareMemoryData(const Brain& brain, Timestamp startTime, Timestamp endTime) {
    // Prepare memory data
    nlohmann::json jsonData;
    jsonData["timestamp"] = getCurrentTimestamp();
    jsonData["type"] = "memory_data";
    
    // Add memory data...
    
    return jsonData.dump(2);
}

std::string ExportUtils::prepareNeuromodulationData(const Brain& brain, Timestamp startTime, Timestamp endTime) {
    // Prepare neuromodulation data
    nlohmann::json jsonData;
    jsonData["timestamp"] = getCurrentTimestamp();
    jsonData["type"] = "neuromodulation_data";
    
    // Add neuromodulation data...
    
    return jsonData.dump(2);
}

std::string ExportUtils::prepareDevelopmentData(const Brain& brain, Timestamp startTime, Timestamp endTime) {
    // Prepare development data
    nlohmann::json jsonData;
    jsonData["timestamp"] = getCurrentTimestamp();
    jsonData["type"] = "development_data";
    
    // Add development data...
    
    return jsonData.dump(2);
}

bool ExportUtils::exportAll(const std::string& baseFilename) {
    // Export all available data
    bool success = true;
    
    success &= exportToText(baseFilename + "_brain.txt", prepareBrainData(Brain()));
    success &= exportToText(baseFilename + "_activity.txt", prepareActivityData(Brain(), 0, 0));
    success &= exportToText(baseFilename + "_memory.txt", prepareMemoryData(Brain(), 0, 0));
    success &= exportToText(baseFilename + "_neuromodulation.txt", prepareNeuromodulationData(Brain(), 0, 0));
    success &= exportToText(baseFilename + "_development.txt", prepareDevelopmentData(Brain(), 0, 0));
    
    return success;
}

bool ExportUtils::exportWithProgress(const std::string& filename, const std::string& data) {
    ExportProgress progress;
    progress.operation = "export_" + std::to_string(pImpl->totalExports);
    progress.filename = filename;
    progress.status = ExportStatus::InProgress;
    progress.progress = 0.0f;
    progress.totalBytes = data.size();
    progress.bytesProcessed = 0;
    progress.estimatedTimeRemaining = 1.0;
    progress.currentStep = "Exporting to file";
    progress.startTime = getCurrentTimestamp();
    progress.lastUpdateTime = progress.startTime;
    
    // Add to export queue
    {
        std::lock_guard<std::mutex> lock(pImpl->exportMutex);
        pImpl->exportQueue.push(progress);
        pImpl->operationStatus[progress.operation] = ExportStatus::InProgress;
        pImpl->operationProgress[progress.operation] = progress;
    }
    
    // Perform export (simplified)
    ExportStatus result = exportToText(filename, data) ? ExportStatus::Completed : ExportStatus::Failed;
    
    // Update progress
    {
        std::lock_guard<std::mutex> lock(pImpl->exportMutex);
        progress.status = result;
        progress.progress = (result == ExportStatus::Completed) ? 1.0f : 0.0f;
        pImpl->operationStatus[progress.operation] = result;
        pImpl->operationProgress[progress.operation] = progress;
        
        if (result == ExportStatus::Completed) {
            pImpl->totalExports++;
            pImpl->successfulExports++;
        } else {
            pImpl->totalExports++;
            pImpl->failedExports++;
        }
    }
    
    return (result == ExportStatus::Completed);
}

size_t ExportUtils::getTotalExports() const {
    return pImpl->totalExports;
}

size_t ExportUtils::getSuccessfulExports() const {
    return pImpl->successfulExports;
}

size_t ExportUtils::getFailedExports() const {
    return pImpl->failedExports;
}

double ExportUtils::getAverageExportTime() const {
    if (pImpl->totalExports == 0) return 0.0;
    return pImpl->totalExportTime / pImpl->totalExports;
}

// Helper function
std::string ExportUtils::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Video recording worker (simplified)
void ExportUtils::videoRecordingWorker(const std::string& filename, const VideoConfig& config) {
    // Simplified video recording - would use actual video libraries
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        file << "VIDEO_DATA_PLACEHOLDER";
        file.close();
    }
}

// WebSocket worker (simplified)
void ExportUtils::websocketWorker() {
    // Simplified WebSocket connection - would use actual WebSocket library
    std::this_thread::sleep_for(std::chrono::seconds(10));
}

} // namespace nlm