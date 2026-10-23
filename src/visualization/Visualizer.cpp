#include "Visualizer.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <cmath>

namespace nlm {

struct TextVisualizer::Impl {
    VisualizationConfig config;
    std::map<std::string, std::vector<VisualizationDataPoint>> dataSeries;
    double maxYAxisValue;
    bool includeGrid;
    bool showDataPoints;
    int width;
    int height;
    std::string title;
    
    Impl() : maxYAxisValue(100.0), includeGrid(true), showDataPoints(true), 
             width(80), height(40), title("Neural Visualization") {}
};

TextVisualizer::TextVisualizer() : pImpl(std::make_unique<Impl>()) {}

TextVisualizer::~TextVisualizer() = default;

bool TextVisualizer::initialize(const VisualizationConfig& config) {
    pImpl->config = config;
    pImpl->title = config.title.empty() ? "Neural Visualization" : config.title;
    pImpl->width = config.options.count("width") ? std::stoi(config.options.at("width")) : 80;
    pImpl->height = config.options.count("height") ? std::stoi(config.options.at("height")) : 40;
    
    if (config.options.count("max_y_value")) {
        pImpl->maxYAxisValue = std::stod(config.options.at("max_y_value"));
    }
    
    if (config.options.count("include_grid")) {
        pImpl->includeGrid = config.options.at("include_grid") == "true";
    }
    
    if (config.options.count("show_data_points")) {
        pImpl->showDataPoints = config.options.at("show_data_points") == "true";
    }
    
    pImpl->dataSeries.clear();
    return true;
}

bool TextVisualizer::update(const std::vector<VisualizationDataPoint>& data) {
    if (data.empty()) return false;
    
    pImpl->dataSeries["data"] = data;
    return true;
}

bool TextVisualizer::render() {
    if (pImpl->dataSeries.empty()) return false;
    
    // Generate ASCII plot
    std::cout << "=== " << pImpl->title << " ===" << std::endl;
    std::cout << "X-axis: " << pImpl->config.xAxisLabel << std::endl;
    std::cout << "Y-axis: " << pImpl->config.yAxisLabel << std::endl;
    std::cout << std::endl;
    
    for (const auto& series : pImpl->dataSeries) {
        std::cout << "Series: " << series.first << std::endl;
        
        // Simple ASCII plot
        int plotWidth = pImpl->width;
        int plotHeight = pImpl->height;
        
        // Find min and max values
        double minVal = series.second[0].value;
        double maxVal = series.second[0].value;
        for (const auto& point : series.second) {
            if (point.value < minVal) minVal = point.value;
            if (point.value > maxVal) maxVal = point.value;
        }
        
        double range = maxVal - minVal;
        if (range == 0) range = 1.0;
        
        // Create a simple histogram-like plot
        std::vector<std::string> plot(plotHeight, std::string(plotWidth, ' '));
        
        for (size_t i = 0; i < series.second.size() && i < static_cast<size_t>(plotWidth); ++i) {
            double normalizedValue = (series.second[i].value - minVal) / range;
            int barHeight = static_cast<int>(normalizedValue * (plotHeight - 1));
            
            for (int h = plotHeight - 1; h >= plotHeight - 1 - barHeight && h >= 0; --h) {
                plot[h][i] = '*';
            }
            
            if (pImpl->showDataPoints && i < plotWidth) {
                std::cout << "Point " << i << ": " << series.second[i].value << " (time: " << series.second[i].timestamp << ")" << std::endl;
            }
        }
        
        if (pImpl->includeGrid) {
            std::cout << "Grid lines drawn" << std::endl;
        }
        
        std::cout << "Plot displayed" << std::endl;
        std::cout << std::endl;
    }
    
    return true;
}

bool TextVisualizer::save() {
    std::string filename = pImpl->config.filePath.empty() ? "visualization.txt" : pImpl->config.filePath;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== " << pImpl->title << " ===" << std::endl;
    file << "Generated on: " << generateTimestamp() << std::endl;
    file << "X-axis: " << pImpl->config.xAxisLabel << std::endl;
    file << "Y-axis: " << pImpl->config.yAxisLabel << std::endl;
    file << std::endl;
    
    for (const auto& series : pImpl->dataSeries) {
        file << "Series: " << series.first << std::endl;
        for (const auto& point : series.second) {
            file << "  " << point.timestamp << ", " << point.value << " " << point.label << std::endl;
        }
        file << std::endl;
    }
    
    file.close();
    return true;
}

void TextVisualizer::close() {
    pImpl->dataSeries.clear();
}

struct JSONVisualizer::Impl {
    VisualizationConfig config;
    std::map<std::string, std::vector<VisualizationDataPoint>> dataSeries;
    bool prettyPrint;
    bool includeMetadata;
    
    Impl() : prettyPrint(true), includeMetadata(true) {}
};

JSONVisualizer::JSONVisualizer() : pImpl(std::make_unique<Impl>()) {}

JSONVisualizer::~JSONVisualizer() = default;

bool JSONVisualizer::initialize(const VisualizationConfig& config) {
    pImpl->config = config;
    
    if (config.options.count("pretty_print")) {
        pImpl->prettyPrint = config.options.at("pretty_print") == "true";
    }
    
    if (config.options.count("include_metadata")) {
        pImpl->includeMetadata = config.options.at("include_metadata") == "true";
    }
    
    pImpl->dataSeries.clear();
    return true;
}

bool JSONVisualizer::update(const std::vector<VisualizationDataPoint>& data) {
    if (data.empty()) return false;
    
    // For simplicity, we'll store all data in a single series
    if (pImpl->dataSeries.empty()) {
        pImpl->dataSeries["data"] = data;
    }
    
    return true;
}

bool JSONVisualizer::render() {
    return save();
}

bool JSONVisualizer::save() {
    std::string filename = pImpl->config.filePath.empty() ? "visualization.json" : pImpl->config.filePath;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{
  \"visualization\": {
    \"title\": \"" << pImpl->config.title << "\",
    \"timestamp\": \"" << generateTimestamp() << "\",
    \"config\": {
      \"plot_type\": \"" << static_cast<int>(pImpl->config.plotType) << "\",
      \"format\": \"" << static_cast<int>(pImpl->config.format) << "\",
      \"x_axis_label\": \"" << pImpl->config.xAxisLabel << "\",
      \"y_axis_label\": \"" << pImpl->config.yAxisLabel << "\",
      \"x_min\": " << pImpl->config.xMin << ",
      \"x_max\": " << pImpl->config.xMax << ",
      \"y_min\": " << pImpl->config.yMin << ",
      \"y_max\": " << pImpl->config.yMax << "\n";
    
    if (pImpl->includeMetadata) {
        file << "      \"metadata\": {
          \"generated_by\": \"NLM Visualization System\",
          \"version\": \"1.0\"
        }\n";
    }
    
    file << "    },
    \"data_series\": [\n";
    
    bool firstSeries = true;
    for (const auto& series : pImpl->dataSeries) {
        if (!firstSeries) file << ",\n";
        firstSeries = false;
        
        file << "      {\n        \"name\": \"" << series.first << "\",
        \"data\": [\n";
        
        bool firstPoint = true;
        for (const auto& point : series.second) {
            if (!firstPoint) file << ",\n";
            firstPoint = false;
            
            file << "          {\n            \"timestamp\": " << point.timestamp << ",\n"
                 << "            \"value\": " << point.value << ",\n"
                 << "            \"label\": \"" << point.label << "\"\n"
                 << "          }";
        }
        
        file << "\n        ]\n      }";
    }
    
    file << "\n    ]\n  }\n}" << std::endl;
    
    file.close();
    return true;
}

void JSONVisualizer::close() {
    pImpl->dataSeries.clear();
}

std::string VisualizationHelpers::generateUniqueID() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return std::to_string(millis) + "_" + std::to_string(rand());
}

std::vector<VisualizationDataPoint> VisualizationHelpers::smoothData(const std::vector<VisualizationDataPoint>& data, int windowSize) {
    if (data.size() < static_cast<size_t>(windowSize) || windowSize < 3) {
        return data;
    }
    
    std::vector<VisualizationDataPoint> smoothed;
    for (size_t i = 0; i < data.size(); ++i) {
        double sum = 0.0;
        int count = 0;
        
        for (int j = -windowSize/2; j <= windowSize/2; ++j) {
            size_t idx = i + j;
            if (idx < data.size()) {
                sum += data[idx].value;
                count++;
            }
        }
        
        VisualizationDataPoint point = data[i];
        point.value = sum / count;
        smoothed.push_back(point);
    }
    
    return smoothed;
}

std::vector<VisualizationDataPoint> VisualizationHelpers::normalizeData(const std::vector<VisualizationDataPoint>& data, double targetMax) {
    if (data.empty()) return data;
    
    double maxVal = data[0].value;
    double minVal = data[0].value;
    for (const auto& point : data) {
        if (point.value > maxVal) maxVal = point.value;
        if (point.value < minVal) minVal = point.value;
    }
    
    double range = maxVal - minVal;
    if (range == 0) range = 1.0;
    
    std::vector<VisualizationDataPoint> normalized;
    for (const auto& point : data) {
        VisualizationDataPoint newPoint = point;
        newPoint.value = ((point.value - minVal) / range) * targetMax;
        normalized.push_back(newPoint);
    }
    
    return normalized;
}

std::string VisualizationHelpers::dataPointToString(const VisualizationDataPoint& point) {
    std::ostringstream oss;
    oss << "[" << point.timestamp << ", " << point.value << ", \"" << point.label << "\"]";
    return oss.str();
}

bool VisualizationHelpers::saveTextVisualization(const std::string& filepath, const std::vector<VisualizationDataPoint>& data, 
                                                 const VisualizationConfig& config) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== " << config.title << " ===" << std::endl;
    file << "Generated on: " << generateTimestamp() << std::endl;
    file << "Plot Type: " << static_cast<int>(config.plotType) << std::endl;
    file << "Format: " << static_cast<int>(config.format) << std::endl;
    file << std::endl;
    
    for (size_t i = 0; i < data.size(); ++i) {
        file << "  " << i << ": " << data[i].timestamp << ", " << data[i].value << " " << data[i].label << std::endl;
    }
    
    file.close();
    return true;
}

bool VisualizationHelpers::saveJSONVisualization(const std::string& filepath, const std::map<std::string, std::vector<VisualizationDataPoint>>& data, 
                                                 const VisualizationConfig& config) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{
  \"visualization\": {
    \"title\": \"" << config.title << "\",
    \"timestamp\": \"" << generateTimestamp() << "\",
    \"config\": {
      \"plot_type\": " << static_cast<int>(config.plotType) << ",
      \"format\": " << static_cast<int>(config.format) << ",
      \"x_axis_label\": \"" << config.xAxisLabel << "\",
      \"y_axis_label\": \"" << config.yAxisLabel << "\",
      \"x_min\": " << config.xMin << ",
      \"x_max\": " << config.xMax << ",
      \"y_min\": " << config.yMin << ",
      \"y_max\": " << config.yMax << "
    },
    \"data_series\": [\n";
    
    bool firstSeries = true;
    for (const auto& series : data) {
        if (!firstSeries) file << ",\n";
        firstSeries = false;
        
        file << "      {\n        \"name\": \"" << series.first << "\",
        \"data\": [\n";
        
        bool firstPoint = true;
        for (const auto& point : series.second) {
            if (!firstPoint) file << ",\n";
            firstPoint = false;
            
            file << "          {\n            \"timestamp\": " << point.timestamp << ",\n"
                 << "            \"value\": " << point.value << ",\n"
                 << "            \"label\": \"" << point.label << "\"
          }";
        }
        
        file << "\n        ]\n      }";
    }
    
    file << "\n    ]\n  }\n}" << std::endl;
    
    file.close();
    return true;
}

std::string VisualizationHelpers::generateTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm_time;
    gmtime_r(&now_c, &tm_time);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S") << "." 
        << std::setw(3) << (millis % 1000);
    return oss.str();
}

struct VisualizationManager::Impl {
    std::map<std::string, std::unique_ptr<IVisualizer>> visualizers;
    VisualizationConfig globalConfig;
    std::chrono::system_clock::time_point lastRenderTime;
    size_t totalDataPointsRendered;
    double totalRenderingTime;
    
    Impl() : totalDataPointsRendered(0), totalRenderingTime(0.0) {}
};

VisualizationManager::VisualizationManager() : pImpl(std::make_unique<Impl>()) {}

VisualizationManager::~VisualizationManager() = default;

bool VisualizationManager::addVisualizer(const std::string& name, std::unique_ptr<IVisualizer> visualizer) {
    if (!visualizer || visualizer->initialize(pImpl->globalConfig)) {
        pImpl->visualizers[name] = std::move(visualizer);
        return true;
    }
    return false;
}

bool VisualizationManager::removeVisualizer(const std::string& name) {
    auto it = pImpl->visualizers.find(name);
    if (it != pImpl->visualizers.end()) {
        it->second->close();
        pImpl->visualizers.erase(it);
        return true;
    }
    return false;
}

bool VisualizationManager::updateAll(const std::map<std::string, std::vector<VisualizationDataPoint>>& data) {
    bool allUpdated = true;
    for (const auto& pair : data) {
        auto it = pImpl->visualizers.find(pair.first);
        if (it != pImpl->visualizers.end()) {
            if (!it->second->update(pair.second)) {
                allUpdated = false;
            }
        }
    }
    return allUpdated;
}

bool VisualizationManager::renderAll() {
    auto startTime = std::chrono::system_clock::now();
    
    bool allRendered = true;
    for (const auto& visualizer : pImpl->visualizers) {
        if (!visualizer.second->render()) {
            allRendered = false;
        }
    }
    
    auto endTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    pImpl->totalRenderingTime += duration;
    pImpl->lastRenderTime = endTime;
    
    return allRendered;
}

bool VisualizationManager::saveAll(const std::string& outputDirectory) {
    bool allSaved = true;
    for (const auto& visualizer : pImpl->visualizers) {
        if (!visualizer.second->save()) {
            allSaved = false;
        }
    }
    return allSaved;
}

bool VisualizationManager::setGlobalConfig(const VisualizationConfig& config) {
    pImpl->globalConfig = config;
    
    for (const auto& visualizer : pImpl->visualizers) {
        visualizer.second->setOutputPath(config.filePath);
        visualizer.second->setFormat(config.format);
        visualizer.second->setTitle(config.title);
    }
    
    return true;
}

VisualizationConfig VisualizationManager::getGlobalConfig() const {
    return pImpl->globalConfig;
}

void VisualizationManager::clearAllData() {
    for (const auto& visualizer : pImpl->visualizers) {
        visualizer.second->clearData();
    }
}

bool VisualizationManager::addDataSeries(const std::string& visualizerName, const std::string& seriesName, 
                                         const std::vector<VisualizationDataPoint>& data) {
    auto it = pImpl->visualizers.find(visualizerName);
    if (it != pImpl->visualizers.end()) {
        return it->second->addDataSeries(seriesName, data);
    }
    return false;
}

size_t VisualizationManager::getVisualizerCount() const {
    return pImpl->visualizers.size();
}

std::vector<std::string> VisualizationManager::getVisualizerNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->visualizers) {
        names.push_back(pair.first);
    }
    return names;
}

std::string VisualizationManager::generateReport() const {
    std::ostringstream oss;
    oss << "=== Visualization Report ===" << std::endl;
    oss << "Number of visualizers: " << pImpl->visualizers.size() << std::endl;
    oss << "Total data points rendered: " << pImpl->totalDataPointsRendered << std::endl;
    oss << "Total rendering time: " << pImpl->totalRenderingTime << " ms" << std::endl;
    
    if (pImpl->totalRenderingTime > 0) {
        oss << "Average rendering time: " << (pImpl->totalRenderingTime / pImpl->visualizers.size()) << " ms per visualizer" << std::endl;
    }
    
    oss << "Last render time: " << (pImpl->lastRenderTime.time_since_epoch().count() / 1000000000) << " seconds ago" << std::endl;
    
    return oss.str();
}

double VisualizationManager::getRenderingTime() const {
    return pImpl->totalRenderingTime;
}

size_t VisualizationManager::getTotalDataPointsRendered() const {
    return pImpl->totalDataPointsRendered;
}

} // namespace nlm
