#include "Visualizer.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <random>
#ifdef __linux__
#include <filesystem>
#endif

namespace nlm {

struct Visualizer::Impl {
    std::shared_ptr<DebugConfig> config;
    std::atomic<bool> isRunning;
    std::atomic<bool> isInitialized;
    
    // Visualization cache
    std::vector<VisualizationResult> recentVisualizations;
    std::map<std::string, VisualizationResult> visualizationCache;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    
    // Configuration
    VisualizationFormat defaultFormat;
    std::string outputDirectory;
    bool autoSaveVisualizations;
    size_t maxRecentVisualizations;
    
    Impl() : isRunning(false), isInitialized(false), autoSaveVisualizations(true), maxRecentVisualizations(100) {
        startTime = std::chrono::steady_clock::now();
        lastUpdateTime = startTime;
        defaultFormat = VisualizationFormat::PNG;
        outputDirectory = "./visualizations";
    }
    
    ~Impl() {
        // Cleanup temporary files
        cleanupTemporaryFiles();
    }
    
    void cleanupTemporaryFiles() {
        try {
            for (const auto& entry : std::filesystem::directory_iterator(outputDirectory)) {
                if (entry.is_regular_file() && entry.path().extension() == ".tmp") {
                    std::filesystem::remove(entry.path());
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error cleaning up temporary visualization files: " + std::string(e.what()));
        }
    }
    
    std::string generateVisualizationId() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        return "vis_" + std::to_string(duration.count());
    }
    
    std::string generateFilePath(const std::string& extension, const std::string& prefix = "") {
        std::string filename = prefix + generateVisualizationId() + "." + extension;
        return outputDirectory + "/" + filename;
    }
    
    std::string formatTime(const std::chrono::steady_clock::time_point& timePoint) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch());
        return std::to_string(duration.count()) + "ms";
    }
    
    std::string generateImagePlaceholder(const VisualizationResult& result) {
        // Generate a simple text-based visualization
        std::stringstream ss;
        ss << "Visualization Result:\n";
        ss << "  ID: " << result.resultId << "\n";
        ss << "  Type: " << static_cast<int>(result.visualizationType) << "\n";
        ss << "  Format: " << static_cast<int>(result.outputFormat) << "\n";
        ss << "  Timestamp: " << formatTime(result.timestamp) << "\n";
        ss << "  Success: " << (result.success ? "Yes" : "No") << "\n";
        if (!result.errorMessage.empty()) {
            ss << "  Error: " << result.errorMessage << "\n";
        }
        ss << "  File: " << result.filePath << "\n";
        return ss.str();
    }
    
    std::string generateJSONVisualization(const VisualizationResult& result) {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"id\": \"" << result.resultId << "\",\n";
        ss << "  \"timestamp\": \"" << formatTime(result.timestamp) << "\",\n";
        ss << "  \"type\": \"" << static_cast<int>(result.visualizationType) << "\",\n";
        ss << "  \"format\": \"" << static_cast<int>(result.outputFormat) << "\",\n";
        ss << "  \"success\": " << (result.success ? "true" : "false") << ",\n";
        ss << "  \"filePath\": \"" << result.filePath << "\",\n";
        ss << "  \"metadata\": {\n";
        bool firstMeta = true;
        for (const auto& [key, value] : result.metadata) {
            if (!firstMeta) ss << ",\n";
            ss << "    \"" << key << "\": \"" << value << "\"";
            firstMeta = false;
        }
        ss << "\n  }\n";
        ss << "}\n";
        return ss.str();
    }
    
    std::string generateCSVVisualization(const VisualizationResult& result) {
        std::stringstream ss;
        ss << "id,timestamp,type,format,success,filePath\n";
        ss << result.resultId << ",";
        ss << formatTime(result.timestamp) << ",";
        ss << static_cast<int>(result.visualizationType) << ",";
        ss << static_cast<int>(result.outputFormat) << ",";
        ss << (result.success ? "true" : "false") << ";";
        ss << result.filePath << "\n";
        return ss.str();
    }
    
    std::string generateHTMLVisualization(const VisualizationResult& result) {
        std::stringstream ss;
        ss << "<!DOCTYPE html>\n<html>\n<head>\n";
        ss << "  <title>Visualization: " << result.resultId << "</title>\n";
        ss << "  <style>\n";
        ss << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
        ss << "    .visualization { border: 1px solid #ccc; padding: 10px; margin: 10px 0; }\n";
        ss << "    .metadata { background-color: #f5f5f5; padding: 10px; margin: 10px 0; }\n";
        ss << "  </style>\n";
        ss << "</head>\n<body>\n";
        ss << "  <h1>Visualization: " << result.resultId << "</h1>\n";
        ss << "  <div class=\"visualization\">\n";
        ss << "    <p><strong>Type:</strong> " << static_cast<int>(result.visualizationType) << "</p>\n";
        ss << "    <p><strong>Format:</strong> " << static_cast<int>(result.outputFormat) << "</p>\n";
        ss << "    <p><strong>Timestamp:</strong> " << formatTime(result.timestamp) << "</p>\n";
        ss << "    <p><strong>Success:</strong> " << (result.success ? "Yes" : "No") << "</p>\n";
        ss << "    <p><strong>File:</strong> <a href=\"" << result.filePath << "\">" << result.filePath << "</a></p>\n";
        ss << "  </div>\n";
        ss << "  <div class=\"metadata\">\n";
        ss << "    <h3>Metadata</h3>\n";
        ss << "    <ul>\n";
        for (const auto& [key, value] : result.metadata) {
            ss << "      <li><strong>" << key << "</strong>: " << value << "</li>\n";
        }
        ss << "    </ul>\n";
        ss << "  </div>\n";
        ss << "</body>\n</html>\n";
        return ss.str();
    }
};

Visualizer::Visualizer() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_DEBUG("Visualizer created");
}

Visualizer::~Visualizer() {
    NLM_LOG_DEBUG("Visualizer destroyed");
}

Visualizer::Visualizer(Visualizer&& other) noexcept : pImpl(std::move(other.pImpl)) {
}

Visualizer& Visualizer::operator=(Visualizer&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Visualizer::initialize(std::shared_ptr<DebugConfig> config) {
    pImpl->config = config;
    pImpl->isInitialized = true;
    
    // Load visualization configuration from config
    if (config) {
        // Set output directory
        std::string vizDir = config->getSessionDirectory() + "/visualizations";
        pImpl->outputDirectory = vizDir;
        
        // Create output directory if it doesn't exist
        std::filesystem::create_directories(pImpl->outputDirectory);
        
        // Set auto-save flag
        pImpl->autoSaveVisualizations = config->isSessionRecordingEnabled();
    }
    
    NLM_LOG_INFO("Visualizer initialized");
}

void Visualizer::start() {
    if (!pImpl->isInitialized) {
        NLM_LOG_WARNING("Visualizer not initialized, skipping start");
        return;
    }
    
    pImpl->isRunning = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    NLM_LOG_INFO("Visualizer started");
}

void Visualizer::stop() {
    pImpl->isRunning = false;
    NLM_LOG_INFO("Visualizer stopped");
}

void Visualizer::update() {
    if (!pImpl->isRunning) {
        return;
    }
    
    pImpl->lastUpdateTime = std::chrono::steady_clock::now();
}

VisualizationResult Visualizer::visualizeNeuralNetwork(const NeuralNetworkVisualization& network) {
    VisualizationResult result;
    result.resultId = pImpl->generateVisualizationId();
    result.timestamp = std::chrono::steady_clock::now();
    result.visualizationType = VisualizationType::NeuralNetwork;
    result.outputFormat = pImpl->defaultFormat;
    result.success = false;
    result.errorMessage = "";
    
    try {
        // Generate visualization data
        std::string encodedData;
        if (network.layerActivities.empty()) {
            result.errorMessage = "No neural network data to visualize";
            encodedData = pImpl->generateImagePlaceholder(result);
        } else {
            encodedData = generateNeuralNetworkVisualization(network);
        }
        
        result.encodedData = encodedData;
        
        // Save if auto-save is enabled
        if (pImpl->autoSaveVisualizations) {
            std::string extension;
            switch (pImpl->defaultFormat) {
                case VisualizationFormat::PNG: extension = "png"; break;
                case VisualizationFormat::JSON: extension = "json"; break;
                case VisualizationFormat::CSV: extension = "csv"; break;
                case VisualizationFormat::HTML: extension = "html"; break;
                case VisualizationFormat::Text: extension = "txt"; break;
                case VisualizationFormat::Custom: extension = "dat"; break;
            }
            
            result.filePath = pImpl->generateFilePath(extension, "neural_net_");
            
            if (saveVisualization(result, result.filePath)) {
                result.success = true;
            }
        }
        
        // Add metadata
        result.metadata["layer_count"] = std::to_string(network.layerActivities.size());
        result.metadata["time_series_points"] = std::to_string(network.timeSeriesData.size());
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string(e.what());
        NLM_LOG_ERROR("Error visualizing neural network: " + result.errorMessage);
    }
    
    // Add to recent visualizations
    pImpl->recentVisualizations.push_back(result);
    if (pImpl->recentVisualizations.size() > pImpl->maxRecentVisualizations) {
        pImpl->recentVisualizations.erase(pImpl->recentVisualizations.begin());
    }
    
    return result;
}

VisualizationResult Visualizer::visualizeActivityPatterns(const std::vector<ActivityPattern>& patterns) {
    VisualizationResult result;
    result.resultId = pImpl->generateVisualizationId();
    result.timestamp = std::chrono::steady_clock::now();
    result.visualizationType = VisualizationType::ActivityPattern;
    result.outputFormat = pImpl->defaultFormat;
    result.success = false;
    result.errorMessage = "";
    
    try {
        // Generate visualization data
        std::string encodedData;
        if (patterns.empty()) {
            result.errorMessage = "No activity patterns to visualize";
            encodedData = pImpl->generateImagePlaceholder(result);
        } else {
            encodedData = generateActivityPatternVisualization(patterns);
        }
        
        result.encodedData = encodedData;
        
        // Save if auto-save is enabled
        if (pImpl->autoSaveVisualizations) {
            std::string extension;
            switch (pImpl->defaultFormat) {
                case VisualizationFormat::PNG: extension = "png"; break;
                case VisualizationFormat::JSON: extension = "json"; break;
                case VisualizationFormat::CSV: extension = "csv"; break;
                case VisualizationFormat::HTML: extension = "html"; break;
                case VisualizationFormat::Text: extension = "txt"; break;
                case VisualizationFormat::Custom: extension = "dat"; break;
            }
            
            result.filePath = pImpl->generateFilePath(extension, "activity_pattern_");
            
            if (saveVisualization(result, result.filePath)) {
                result.success = true;
            }
        }
        
        // Add metadata
        result.metadata["pattern_count"] = std::to_string(patterns.size());
        result.metadata["total_duration"] = std::to_string(patterns.size() > 0 ? 
            std::chrono::duration<double, std::milli>(patterns.back().endTime - patterns.front().startTime).count() : 0) + "ms";
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string(e.what());
        NLM_LOG_ERROR("Error visualizing activity patterns: " + result.errorMessage);
    }
    
    // Add to recent visualizations
    pImpl->recentVisualizations.push_back(result);
    if (pImpl->recentVisualizations.size() > pImpl->maxRecentVisualizations) {
        pImpl->recentVisualizations.erase(pImpl->recentVisualizations.begin());
    }
    
    return result;
}

VisualizationResult Visualizer::visualizeResults(const std::map<std::string, double>& results) {
    VisualizationResult result;
    result.resultId = pImpl->generateVisualizationId();
    result.timestamp = std::chrono::steady_clock::now();
    result.visualizationType = VisualizationType::Results;
    result.outputFormat = pImpl->defaultFormat;
    result.success = false;
    result.errorMessage = "";
    
    try {
        // Generate visualization data
        std::string encodedData;
        if (results.empty()) {
            result.errorMessage = "No results to visualize";
            encodedData = pImpl->generateImagePlaceholder(result);
        } else {
            encodedData = generateResultsVisualization(results);
        }
        
        result.encodedData = encodedData;
        
        // Save if auto-save is enabled
        if (pImpl->autoSaveVisualizations) {
            std::string extension;
            switch (pImpl->defaultFormat) {
                case VisualizationFormat::PNG: extension = "png"; break;
                case VisualizationFormat::JSON: extension = "json"; break;
                case VisualizationFormat::CSV: extension = "csv"; break;
                case VisualizationFormat::HTML: extension = "html"; break;
                case VisualizationFormat::Text: extension = "txt"; break;
                case VisualizationFormat::Custom: extension = "dat"; break;
            }
            
            result.filePath = pImpl->generateFilePath(extension, "results_");
            
            if (saveVisualization(result, result.filePath)) {
                result.success = true;
            }
        }
        
        // Add metadata
        result.metadata["result_count"] = std::to_string(results.size());
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string(e.what());
        NLM_LOG_ERROR("Error visualizing results: " + result.errorMessage);
    }
    
    // Add to recent visualizations
    pImpl->recentVisualizations.push_back(result);
    if (pImpl->recentVisualizations.size() > pImpl->maxRecentVisualizations) {
        pImpl->recentVisualizations.erase(pImpl->recentVisualizations.begin());
    }
    
    return result;
}

VisualizationResult Visualizer::visualizeTimeSeries(const std::map<std::string, std::vector<float>>& timeSeriesData, const std::chrono::steady_clock::time_point& startTime, const std::chrono::steady_clock::time_point& endTime) {
    VisualizationResult result;
    result.resultId = pImpl->generateVisualizationId();
    result.timestamp = std::chrono::steady_clock::now();
    result.visualizationType = VisualizationType::TimeSeries;
    result.outputFormat = pImpl->defaultFormat;
    result.success = false;
    result.errorMessage = "";
    
    try {
        // Generate visualization data
        std::string encodedData;
        if (timeSeriesData.empty()) {
            result.errorMessage = "No time series data to visualize";
            encodedData = pImpl->generateImagePlaceholder(result);
        } else {
            encodedData = generateTimeSeriesVisualization(timeSeriesData, startTime, endTime);
        }
        
        result.encodedData = encodedData;
        
        // Save if auto-save is enabled
        if (pImpl->autoSaveVisualizations) {
            std::string extension;
            switch (pImpl->defaultFormat) {
                case VisualizationFormat::PNG: extension = "png"; break;
                case VisualizationFormat::JSON: extension = "json"; break;
                case VisualizationFormat::CSV: extension = "csv"; break;
                case VisualizationFormat::HTML: extension = "html"; break;
                case VisualizationFormat::Text: extension = "txt"; break;
                case VisualizationFormat::Custom: extension = "dat"; break;
            }
            
            result.filePath = pImpl->generateFilePath(extension, "time_series_");
            
            if (saveVisualization(result, result.filePath)) {
                result.success = true;
            }
        }
        
        // Add metadata
        result.metadata["series_count"] = std::to_string(timeSeriesData.size());
        result.metadata["duration"] = std::to_string(std::chrono::duration<double, std::milli>(endTime - startTime).count()) + "ms";
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string(e.what());
        NLM_LOG_ERROR("Error visualizing time series: " + result.errorMessage);
    }
    
    // Add to recent visualizations
    pImpl->recentVisualizations.push_back(result);
    if (pImpl->recentVisualizations.size() > pImpl->maxRecentVisualizations) {
        pImpl->recentVisualizations.erase(pImpl->recentVisualizations.begin());
    }
    
    return result;
}

VisualizationResult Visualizer::visualizeStatisticalData(const std::map<std::string, std::vector<float>>& statisticalData) {
    VisualizationResult result;
    result.resultId = pImpl->generateVisualizationId();
    result.timestamp = std::chrono::steady_clock::now();
    result.visualizationType = VisualizationType::Statistical;
    result.outputFormat = pImpl->defaultFormat;
    result.success = false;
    result.errorMessage = "";
    
    try {
        // Generate visualization data
        std::string encodedData;
        if (statisticalData.empty()) {
            result.errorMessage = "No statistical data to visualize";
            encodedData = pImpl->generateImagePlaceholder(result);
        } else {
            encodedData = generateStatisticalVisualization(statisticalData);
        }
        
        result.encodedData = encodedData;
        
        // Save if auto-save is enabled
        if (pImpl->autoSaveVisualizations) {
            std::string extension;
            switch (pImpl->defaultFormat) {
                case VisualizationFormat::PNG: extension = "png"; break;
                case VisualizationFormat::JSON: extension = "json"; break;
                case VisualizationFormat::CSV: extension = "csv"; break;
                case VisualizationFormat::HTML: extension = "html"; break;
                case VisualizationFormat::Text: extension = "txt"; break;
                case VisualizationFormat::Custom: extension = "dat"; break;
            }
            
            result.filePath = pImpl->generateFilePath(extension, "statistical_");
            
            if (saveVisualization(result, result.filePath)) {
                result.success = true;
            }
        }
        
        // Add metadata
        result.metadata["data_series_count"] = std::to_string(statisticalData.size());
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string(e.what());
        NLM_LOG_ERROR("Error visualizing statistical data: " + result.errorMessage);
    }
    
    // Add to recent visualizations
    pImpl->recentVisualizations.push_back(result);
    if (pImpl->recentVisualizations.size() > pImpl->maxRecentVisualizations) {
        pImpl->recentVisualizations.erase(pImpl->recentVisualizations.begin());
    }
    
    return result;
}

bool Visualizer::saveVisualization(const VisualizationResult& result, const std::string& filepath) {
    try {
        std::string content;
        switch (result.outputFormat) {
            case VisualizationFormat::JSON:
                content = pImpl->generateJSONVisualization(result);
                break;
            case VisualizationFormat::CSV:
                content = pImpl->generateCSVVisualization(result);
                break;
            case VisualizationFormat::HTML:
                content = pImpl->generateHTMLVisualization(result);
                break;
            case VisualizationFormat::Text:
                content = result.encodedData; // Already contains the text
                break;
            case VisualizationFormat::PNG:
            case VisualizationFormat::Custom:
            default:
                // For binary formats, just store the encoded data
                content = result.encodedData;
                break;
        }
        
        // Write to file
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to save visualization to file: " + filepath);
            return false;
        }
        
        file.write(content.c_str(), content.size());
        file.close();
        
        // Update result with file path
        const_cast<VisualizationResult&>(result).filePath = filepath;
        
        NLM_LOG_INFO("Visualization saved to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving visualization: " + std::string(e.what()));
        return false;
    }
}

std::vector<VisualizationResult> Visualizer::getRecentVisualizations(size_t count) const {
    if (count > pImpl->recentVisualizations.size()) {
        count = pImpl->recentVisualizations.size();
    }
    
    std::vector<VisualizationResult> recent;
    recent.reserve(count);
    
    auto start = pImpl->recentVisualizations.end() - count;
    for (auto it = start; it != pImpl->recentVisualizations.end(); ++it) {
        recent.push_back(*it);
    }
    
    return recent;
}

void Visualizer::clearVisualizationCache() {
    pImpl->recentVisualizations.clear();
    pImpl->visualizationCache.clear();
    NLM_LOG_INFO("Visualization cache cleared");
}

// Visualization generation implementations

std::string Visualizer::generateNeuralNetworkVisualization(const NeuralNetworkVisualization& network) {
    std::stringstream ss;
    ss << "Neural Network Visualization\n";
    ss << "===============================\n\n";
    
    ss << "Layers: " << network.layerActivities.size() << "\n";
    ss << "Total neurons: ";
    size_t totalNeurons = 0;
    for (const auto& layer : network.layerActivities) {
        totalNeurons += layer.size();
    }
    ss << totalNeurons << "\n\n";
    
    ss << "Activity Heatmap:\n";
    for (size_t layer = 0; layer < network.layerActivities.size(); ++layer) {
        ss << "Layer " << layer << ": ";
        for (size_t neuron = 0; neuron < network.layerActivities[layer].size(); ++neuron) {
            float activity = network.layerActivities[layer][neuron];
            ss << "[" << std::string(static_cast<size_t>(activity * 20), '*') << "]";
        }
        ss << "\n";
    }
    
    ss << "\nConnection Weights:\n";
    if (!network.connectionWeights.empty() && !network.connectionWeights[0].empty()) {
        for (size_t row = 0; row < std::min<size_t>(3, network.connectionWeights.size()); ++row) {
            ss << "Source " << row << " -> Target: ";
            for (size_t col = 0; col < std::min<size_t>(3, network.connectionWeights[row].size()); ++col) {
                ss << network.connectionWeights[row][col] << " ";
            }
            ss << "...\n";
        }
    }
    
    ss << "\nTime Series Data:\n";
    for (const auto& [name, values] : network.timeSeriesData) {
        ss << name << ": ["; 
        for (size_t i = 0; i < std::min<size_t>(5, values.size()); ++i) {
            ss << values[i];
            if (i < values.size() - 1) ss << ", ";
        }
        if (values.size() > 5) ss << ", ...";
        ss << "]\n";
    }
    
    return ss.str();
}

std::string Visualizer::generateActivityPatternVisualization(const std::vector<ActivityPattern>& patterns) {
    std::stringstream ss;
    ss << "Activity Pattern Visualization\n";
    ss << "============================\n\n";
    
    ss << "Total patterns: " << patterns.size() << "\n\n";
    
    ss << "Pattern Details:\n";
    for (size_t i = 0; i < patterns.size(); ++i) {
        const auto& pattern = patterns[i];
        ss << "Pattern " << i << ": " << pattern.patternId << "\n";
        ss << "  Type: " << pattern.patternType << "\n";
        ss << "  Duration: " << std::chrono::duration<double, std::milli>(pattern.endTime - pattern.startTime).count() << "ms\n";
        ss << "  Peak Activity: " << pattern.peakActivity << "\n";
        ss << "  Average Activity: " << pattern.averageActivity << "\n";
        ss << "  Activity Values: ["; 
        for (size_t j = 0; j < std::min<size_t>(5, pattern.activityValues.size()); ++j) {
            ss << pattern.activityValues[j];
            if (j < pattern.activityValues.size() - 1) ss << ", ";
        }
        if (pattern.activityValues.size() > 5) ss << ", ...";
        ss << "]\n\n";
    }
    
    return ss.str();
}

std::string Visualizer::generateResultsVisualization(const std::map<std::string, double>& results) {
    std::stringstream ss;
    ss << "Results Visualization\n";
    ss << "=====================\n\n";
    
    ss << "Results Summary:\n";
    size_t count = 0;
    for (const auto& [name, value] : results) {
        ss << "  " << name << ": " << value << "\n";
        count++;
        if (count >= 10) break; // Limit output
    }
    if (results.size() > 10) {
        ss << "  ... and " << (results.size() - 10) << " more results\n";
    }
    
    // Generate a simple bar chart
    ss << "\nBar Chart (scaled):\n";
    double maxValue = 0.0;
    for (const auto& [name, value] : results) {
        if (value > maxValue) maxValue = value;
    }
    
    if (maxValue > 0) {
        for (const auto& [name, value] : results) {
            if (name == "simulation_time") continue; // Skip from bar chart
            ss << "  " << name << ": ";
            int barLength = static_cast<int>(value / maxValue * 50);
            ss << std::string(barLength, '=') << " [" << value << "]\n";
        }
    }
    
    return ss.str();
}

std::string Visualizer::generateTimeSeriesVisualization(const std::map<std::string, std::vector<float>>& timeSeriesData, const std::chrono::steady_clock::time_point& startTime, const std::chrono::steady_clock::time_point& endTime) {
    std::stringstream ss;
    ss << "Time Series Visualization\n";
    ss << "========================\n\n";
    
    double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    ss << "Duration: " << duration << "ms\n";
    ss << "Data Series: " << timeSeriesData.size() << "\n\n";
    
    // Generate simple ASCII time series
    ss << "Time Series Data:\n";
    size_t maxPoints = 0;
    for (const auto& [name, values] : timeSeriesData) {
        if (values.size() > maxPoints) maxPoints = values.size();
    }
    
    if (maxPoints > 0 && maxPoints <= 50) {
        // Show first few points for each series
        for (const auto& [name, values] : timeSeriesData) {
            ss << name << ": ";
            for (size_t i = 0; i < std::min<size_t>(5, values.size()); ++i) {
                ss << values[i];
                if (i < values.size() - 1) ss << " ";
            }
            if (values.size() > 5) ss << " ...";
            ss << "\n";
        }
    }
    
    return ss.str();
}

std::string Visualizer::generateStatisticalVisualization(const std::map<std::string, std::vector<float>>& statisticalData) {
    std::stringstream ss;
    ss << "Statistical Visualization\n";
    ss << "=========================\n\n";
    
    ss << "Statistical Data Series: " << statisticalData.size() << "\n\n";
    
    // Calculate basic statistics for each series
    ss << "Statistical Summary:\n";
    for (const auto& [name, values] : statisticalData) {
        if (values.empty()) continue;
        
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        double mean = sum / values.size();
        
        double sqSum = 0.0;
        for (float value : values) {
            double diff = value - mean;
            sqSum += diff * diff;
        }
        double stdDev = std::sqrt(sqSum / values.size());
        
        double min = *std::min_element(values.begin(), values.end());
        double max = *std::max_element(values.begin(), values.end());
        
        ss << "  " << name << ":\n";
        ss << "    Count: " << values.size() << "\n";
        ss << "    Mean: " << mean << "\n";
        ss << "    Std Dev: " << stdDev << "\n";
        ss << "    Min: " << min << "\n";
        ss << "    Max: " << max << "\n";
    }
    
    return ss.str();
}

} // namespace nlm
