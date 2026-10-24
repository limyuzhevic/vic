#include "VisualizationMonitoring.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <sys/resource.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <atomic>

namespace nlm {
namespace vizmon {

// Visualization implementation functions
bool Visualization::initialize(const std::map<std::string, std::any>& config) {
    return true; // Override in derived classes
}

void Visualization::shutdown() {
    // Override in derived classes
}

bool Visualization::isInitialized() const {
    return false; // Override in derived classes
}

bool Visualization::canRender(const std::string& dataType) const {
    return true; // Override in derived classes
}

std::any Visualization::render(const std::string& dataType, const VisualizationData& data) {
    return std::any(); // Override in derived classes
}

bool Visualization::update(const VisualizationData& data) {
    return true; // Override in derived classes
}

bool Visualization::save(const std::string& filepath) const {
    return false; // Override in derived classes
}

bool Visualization::configure(const std::map<std::string, std::any>& config) {
    return true; // Override in derived classes
}

std::map<std::string, std::any> Visualization::getConfig() const {
    return std::map<std::string, std::any>(); // Override in derived classes
}

// ConsoleVisualization implementation
bool ConsoleVisualization::initialize(const std::map<std::string, std::any>& config) {
    initialized_ = true;
    
    // Parse configuration
    auto widthIt = config.find("width");
    if (widthIt != config.end()) {
        outputWidth_ = std::any_cast<size_t>(widthIt->second);
    }
    
    auto heightIt = config.find("height");
    if (heightIt != config.end()) {
        outputHeight_ = std::any_cast<size_t>(heightIt->second);
    }
    
    auto refreshIt = config.find("refresh_interval_ms");
    if (refreshIt != config.end()) {
        refreshInterval_ = std::chrono::milliseconds(std::any_cast<size_t>(refreshIt->second));
    }
    
    auto colorSchemeIt = config.find("color_scheme");
    if (colorSchemeIt != config.end()) {
        colorScheme_ = std::any_cast<std::string>(colorSchemeIt->second);
    }
    
    return true;
}

void ConsoleVisualization::shutdown() {
    initialized_ = false;
    recentData_.clear();
}

bool ConsoleVisualization::isInitialized() const {
    return initialized_;
}

bool ConsoleVisualization::canRender(const std::string& dataType) const {
    return dataType == "console" || dataType == "text" || dataType == "generic";
}

std::any ConsoleVisualization::render(const std::string& dataType, const VisualizationData& data) {
    if (!initialized_) return std::any();
    
    recentData_.push_back(data);
    if (recentData_.size() > 100) {
        recentData_.erase(recentData_.begin());
    }
    
    std::ostringstream oss;
    oss << "=== " << data.title << " ===" << std::endl;
    oss << "Time: " << utils::generateTimestamp() << std::endl;
    oss << std::endl;
    
    // Render data based on type
    if (dataType == "console") {
        oss << "Console Visualization:" << std::endl;
        for (const auto& pair : data.data) {
            oss << "  " << pair.first << ": ";
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, double>) {
                    oss << std::fixed << std::setprecision(2) << val;
                } else if constexpr (std::is_same_v<T, int>) {
                    oss << val;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    oss << val;
                } else {
                    oss << "N/A";
                }
            }, pair.second);
            oss << std::endl;
        }
    }
    
    return oss.str();
}

bool ConsoleVisualization::update(const VisualizationData& data) {
    if (!initialized_) return false;
    
    recentData_.push_back(data);
    if (recentData_.size() > 100) {
        recentData_.erase(recentData_.begin());
    }
    
    return true;
}

bool ConsoleVisualization::save(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.good()) {
        return false;
    }
    
    file << "=== Console Visualization ===" << std::endl;
    file << "Generated at: " << utils::generateTimestamp() << std::endl;
    file << std::endl;
    
    for (const auto& entry : recentData_) {
        file << "--- " << entry.title << " ---" << std::endl;
        file << "Timestamp: " << utils::generateTimestamp() << std::endl;
        
        for (const auto& pair : entry.data) {
            file << "  " << pair.first << ": ";
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, double>) {
                    file << std::fixed << std::setprecision(2) << val;
                } else if constexpr (std::is_same_v<T, int>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    file << val;
                }
            }, pair.second);
            file << std::endl;
        }
        
        file << std::endl;
    }
    
    return file.good();
}

bool ConsoleVisualization::configure(const std::map<std::string, std::any>& config) {
    bool success = true;
    
    auto widthIt = config.find("width");
    if (widthIt != config.end()) {
        outputWidth_ = std::any_cast<size_t>(widthIt->second);
    }
    
    auto heightIt = config.find("height");
    if (heightIt != config.end()) {
        outputHeight_ = std::any_cast<size_t>(heightIt->second);
    }
    
    auto refreshIt = config.find("refresh_interval_ms");
    if (refreshIt != config.end()) {
        refreshInterval_ = std::chrono::milliseconds(std::any_cast<size_t>(refreshIt->second));
    }
    
    auto colorSchemeIt = config.find("color_scheme");
    if (colorSchemeIt != config.end()) {
        colorScheme_ = std::any_cast<std::string>(colorSchemeIt->second);
    }
    
    return success;
}

std::map<std::string, std::any> ConsoleVisualization::getConfig() const {
    std::map<std::string, std::any> config;
    config["width"] = outputWidth_;
    config["height"] = outputHeight_;
    config["refresh_interval_ms"] = refreshInterval_.count();
    config["color_scheme"] = colorScheme_;
    return config;
}

void ConsoleVisualization::setOutputWidth(size_t width) {
    outputWidth_ = width;
}

void ConsoleVisualization::setOutputHeight(size_t height) {
    outputHeight_ = height;
}

void ConsoleVisualization::setRefreshInterval(std::chrono::milliseconds interval) {
    refreshInterval_ = interval;
}

void ConsoleVisualization::setColorScheme(const std::string& scheme) {
    colorScheme_ = scheme;
}

// WebVisualization implementation
bool WebVisualization::initialize(const std::map<std::string, std::any>& config) {
    initialized_ = true;
    serverRunning_ = false;
    
    // Parse configuration
    auto portIt = config.find("port");
    if (portIt != config.end()) {
        webPort_ = std::any_cast<int>(portIt->second);
    }
    
    auto hostIt = config.find("host");
    if (hostIt != config.end()) {
        webHost_ = std::any_cast<std::string>(hostIt->second);
    }
    
    auto autoRefreshIt = config.find("auto_refresh");
    if (autoRefreshIt != config.end()) {
        autoRefresh_ = std::any_cast<bool>(autoRefreshIt->second);
    }
    
    auto staticFilesIt = config.find("static_files");
    if (staticFilesIt != config.end()) {
        staticFiles_ = std::any_cast<std::string>(staticFilesIt->second);
    }
    
    auto addressIt = config.find("server_address");
    if (addressIt != config.end()) {
        serverAddress_ = std::any_cast<std::string>(addressIt->second);
    }
    
    return true;
}

void WebVisualization::shutdown() {
    if (serverRunning_) {
        // Stop web server
        serverRunning_ = false;
    }
    
    initialized_ = false;
}

bool WebVisualization::isInitialized() const {
    return initialized_ && serverRunning_;
}

bool WebVisualization::canRender(const std::string& dataType) const {
    return dataType == "web" || dataType == "html" || dataType == "http";
}

std::any WebVisualization::render(const std::string& dataType, const VisualizationData& data) {
    if (!initialized_ || !serverRunning_) {
        return std::any();
    }
    
    // Generate HTML content
    std::ostringstream oss;
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "  <head>" << std::endl;
    oss << "    <title>" << data.title << "</title>" << std::endl;
    oss << "    <meta charset=\"UTF-8\">" << std::endl;
    oss << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
    oss << "    <style>" << std::endl;
    oss << "      body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    oss << "      .metric { padding: 10px; margin: 5px 0; border: 1px solid #ddd; }" << std::endl;
    oss << "      .metric-name { font-weight: bold; }" << std::endl;
    oss << "      .metric-value { color: #0066cc; }" << std::endl;
    oss << "    </style>" << std::endl;
    oss << "  </head>" << std::endl;
    oss << "  <body>" << std::endl;
    oss << "    <h1>" << data.title << "</h1>" << std::endl;
    oss << "    <p>Generated at: " << utils::generateTimestamp() << "</p>" << std::endl;
    oss << "    <div id=\"metrics\">" << std::endl;
    
    // Add metrics
    for (const auto& pair : data.data) {
        oss << "      <div class=\"metric\">" << std::endl;
        oss << "        <span class=\"metric-name\">" << pair.first << ":</span> " << std::endl;
        oss << "        <span class=\"metric-value\">";
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(2) << val;
            } else if constexpr (std::is_same_v<T, int>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << val;
            } else {
                oss << "N/A";
            }
        }, pair.second);
        oss << "</span>" << std::endl;
        oss << "      </div>" << std::endl;
    }
    
    oss << "    </div>" << std::endl;
    if (autoRefresh_) {
        oss << "    <meta http-equiv=\"refresh\" content=\"5\">" << std::endl;
    }
    oss << "  </body>" << std::endl;
    oss << "</html>" << std::endl;
    
    return oss.str();
}

bool WebVisualization::update(const VisualizationData& data) {
    // In a real implementation, this would update the web server
    // For now, just return success
    return true;
}

bool WebVisualization::save(const std::string& filepath) const {
    // Save HTML file locally
    std::ofstream file(filepath);
    if (!file.good()) {
        return false;
    }
    
    // Generate HTML content
    std::ostringstream oss;
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "  <head>" << std::endl;
    oss << "    <title>" << "Web Visualization Export" << "</title>" << std::endl;
    oss << "    <style>" << std::endl;
    oss << "      body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    oss << "    </style>" << std::endl;
    oss << "  </head>" << std::endl;
    oss << "  <body>" << std::endl;
    oss << "    <h1>Web Visualization Export</h1>" << std::endl;
    oss << "    <p>Generated at: " << utils::generateTimestamp() << "</p>" << std::endl;
    
    file << oss.str();
    file.close();
    
    return true;
}

bool WebVisualization::configure(const std::map<std::string, std::any>& config) {
    bool success = true;
    
    auto portIt = config.find("port");
    if (portIt != config.end()) {
        webPort_ = std::any_cast<int>(portIt->second);
    }
    
    auto hostIt = config.find("host");
    if (hostIt != config.end()) {
        webHost_ = std::any_cast<std::string>(hostIt->second);
    }
    
    auto autoRefreshIt = config.find("auto_refresh");
    if (autoRefreshIt != config.end()) {
        autoRefresh_ = std::any_cast<bool>(autoRefreshIt->second);
    }
    
    auto staticFilesIt = config.find("static_files");
    if (staticFilesIt != config.end()) {
        staticFiles_ = std::any_cast<std::string>(staticFilesIt->second);
    }
    
    auto addressIt = config.find("server_address");
    if (addressIt != config.end()) {
        serverAddress_ = std::any_cast<std::string>(addressIt->second);
    }
    
    return success;
}

std::map<std::string, std::any> WebVisualization::getConfig() const {
    std::map<std::string, std::any> config;
    config["port"] = webPort_;
    config["host"] = webHost_;
    config["auto_refresh"] = autoRefresh_;
    config["static_files"] = staticFiles_;
    config["server_address"] = serverAddress_;
    return config;
}

void WebVisualization::setWebPort(int port) {
    webPort_ = port;
}

void WebVisualization::setWebHost(const std::string& host) {
    webHost_ = host;
}

void WebVisualization::setAutoRefresh(bool enabled) {
    autoRefresh_ = enabled;
}

void WebVisualization::setStaticFiles(const std::string& directory) {
    staticFiles_ = directory;
}

void WebVisualization::setServerAddress(const std::string& address) {
    serverAddress_ = address;
}

// GraphVisualization implementation
bool GraphVisualization::initialize(const std::map<std::string, std::any>& config) {
    initialized_ = true;
    
    // Parse configuration
    auto graphTypeIt = config.find("graph_type");
    if (graphTypeIt != config.end()) {
        graphType_ = std::any_cast<std::string>(graphTypeIt->second);
    }
    
    auto graphStyleIt = config.find("graph_style");
    if (graphStyleIt != config.end()) {
        graphStyle_ = std::any_cast<std::string>(graphStyleIt->second);
    }
    
    auto layoutAlgorithmIt = config.find("layout_algorithm");
    if (layoutAlgorithmIt != config.end()) {
        layoutAlgorithm_ = std::any_cast<std::string>(layoutAlgorithmIt->second);
    }
    
    auto nodeColorMappingIt = config.find("node_color_mapping");
    if (nodeColorMappingIt != config.end()) {
        nodeColorMapping_ = std::any_cast<std::map<std::string, std::string>>(nodeColorMappingIt->second);
    }
    
    auto edgeWidthMappingIt = config.find("edge_width_mapping");
    if (edgeWidthMappingIt != config.end()) {
        edgeWidthMapping_ = std::any_cast<std::map<std::string, double>>(edgeWidthMappingIt->second);
    }
    
    return true;
}

void GraphVisualization::shutdown() {
    initialized_ = false;
    graphData_.clear();
}

bool GraphVisualization::isInitialized() const {
    return initialized_;
}

bool GraphVisualization::canRender(const std::string& dataType) const {
    return dataType == "graph" || dataType == "network" || dataType == "relationship";
}

std::any GraphVisualization::render(const std::string& dataType, const VisualizationData& data) {
    if (!initialized_) return std::any();
    
    graphData_.push_back(data);
    if (graphData_.size() > 100) {
        graphData_.erase(graphData_.begin());
    }
    
    // Generate graph data in JSON format for web frontend
    std::ostringstream oss;
    oss << "{\"type\": \"" << graphType_ << "\", \"style\": \"" << graphStyle_ << "\", \"layout\": \"" << layoutAlgorithm_ << "\", \"nodes\": [";
    
    bool firstNode = true;
    for (const auto& entry : graphData_) {
        for (const auto& pair : entry.data) {
            if (!firstNode) oss << ", ";
            firstNode = false;
            
            oss << "{\"id\": \"" << pair.first << "\", \"value\": ";
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, double>) {
                    oss << val;
                } else if constexpr (std::is_same_v<T, int>) {
                    oss << val;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << val << "\"";
                }
            }, pair.second);
            oss << "}";
        }
    }
    
    oss << "], \"edges\": []}";
    
    return oss.str();
}

bool GraphVisualization::update(const VisualizationData& data) {
    if (!initialized_) return false;
    
    graphData_.push_back(data);
    if (graphData_.size() > 100) {
        graphData_.erase(graphData_.begin());
    }
    
    return true;
}

bool GraphVisualization::save(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.good()) {
        return false;
    }
    
    // Save graph data in JSON format
    std::ostringstream oss;
    oss << "{\"visualization_type\": \"graph\", \"style\": \"" << graphStyle_ << "\"}";
    
    file << oss.str();
    return file.good();
}

bool GraphVisualization::configure(const std::map<std::string, std::any>& config) {
    bool success = true;
    
    auto graphTypeIt = config.find("graph_type");
    if (graphTypeIt != config.end()) {
        graphType_ = std::any_cast<std::string>(graphTypeIt->second);
    }
    
    auto graphStyleIt = config.find("graph_style");
    if (graphStyleIt != config.end()) {
        graphStyle_ = std::any_cast<std::string>(graphStyleIt->second);
    }
    
    auto layoutAlgorithmIt = config.find("layout_algorithm");
    if (layoutAlgorithmIt != config.end()) {
        layoutAlgorithm_ = std::any_cast<std::string>(layoutAlgorithmIt->second);
    }
    
    auto nodeColorMappingIt = config.find("node_color_mapping");
    if (nodeColorMappingIt != config.end()) {
        nodeColorMapping_ = std::any_cast<std::map<std::string, std::string>>(nodeColorMappingIt->second);
    }
    
    auto edgeWidthMappingIt = config.find("edge_width_mapping");
    if (edgeWidthMappingIt != config.end()) {
        edgeWidthMapping_ = std::any_cast<std::map<std::string, double>>(edgeWidthMappingIt->second);
    }
    
    return success;
}

std::map<std::string, std::any> GraphVisualization::getConfig() const {
    std::map<std::string, std::any> config;
    config["graph_type"] = graphType_;
    config["graph_style"] = graphStyle_;
    config["layout_algorithm"] = layoutAlgorithm_;
    config["node_color_mapping"] = nodeColorMapping_;
    config["edge_width_mapping"] = edgeWidthMapping_;
    return config;
}

void GraphVisualization::setGraphType(const std::string& type) {
    graphType_ = type;
}

void GraphVisualization::setGraphStyle(const std::string& style) {
    graphStyle_ = style;
}

void GraphVisualization::setLayoutAlgorithm(const std::string& algorithm) {
    layoutAlgorithm_ = algorithm;
}

void GraphVisualization::setNodeColorMapping(const std::map<std::string, std::string>& mapping) {
    nodeColorMapping_ = mapping;
}

void GraphVisualization::setEdgeWidthMapping(const std::map<std::string, double>& mapping) {
    edgeWidthMapping_ = mapping;
}

// TimelineVisualization implementation
bool TimelineVisualization::initialize(const std::map<std::string, std::any>& config) {
    initialized_ = true;
    
    // Parse configuration
    auto timeStartIt = config.find("time_start");
    if (timeStartIt != config.end()) {
        timeStart_ = std::any_cast<double>(timeStartIt->second);
    }
    
    auto timeEndIt = config.find("time_end");
    if (timeEndIt != config.end()) {
        timeEnd_ = std::any_cast<double>(timeEndIt->second);
    }
    
    auto timeAxisLabelIt = config.find("time_axis_label");
    if (timeAxisLabelIt != config.end()) {
        timeAxisLabel_ = std::any_cast<std::string>(timeAxisLabelIt->second);
    }
    
    auto dataSeriesIt = config.find("data_series");
    if (dataSeriesIt != config.end()) {
        dataSeries_ = std::any_cast<std::vector<std::string>>(dataSeriesIt->second);
    }
    
    auto aggregationMethodIt = config.find("aggregation_method");
    if (aggregationMethodIt != config.end()) {
        aggregationMethod_ = std::any_cast<std::string>(aggregationMethodIt->second);
    }
    
    return true;
}

void TimelineVisualization::shutdown() {
    initialized_ = false;
    timelineData_.clear();
}

bool TimelineVisualization::isInitialized() const {
    return initialized_;
}

bool TimelineVisualization::canRender(const std::string& dataType) const {
    return dataType == "timeline" || dataType == "temporal" || dataType == "time_series";
}

std::any TimelineVisualization::render(const std::string& dataType, const VisualizationData& data) {
    if (!initialized_) return std::any();
    
    timelineData_.push_back(data);
    if (timelineData_.size() > 100) {
        timelineData_.erase(timelineData_.begin());
    }
    
    // Generate timeline data in CSV format
    std::ostringstream oss;
    oss << "timestamp,value,series" << std::endl;
    
    for (const auto& entry : timelineData_) {
        double timestamp = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
            entry.timestamp.time_since_epoch()).count()) / 1000.0;
        
        for (const auto& pair : entry.data) {
            oss << timestamp << ",";
            std::visit([&](const auto& val) {
                oss << val;
            }, pair.second);
            oss << "," << pair.first << std::endl;
        }
    }
    
    return oss.str();
}

bool TimelineVisualization::update(const VisualizationData& data) {
    if (!initialized_) return false;
    
    timelineData_.push_back(data);
    if (timelineData_.size() > 100) {
        timelineData_.erase(timelineData_.begin());
    }
    
    return true;
}

bool TimelineVisualization::save(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.good()) {
        return false;
    }
    
    // Save timeline data in CSV format
    file << "timestamp,value,series" << std::endl;
    
    for (const auto& entry : timelineData_) {
        double timestamp = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
            entry.timestamp.time_since_epoch()).count()) / 1000.0;
        
        for (const auto& pair : entry.data) {
            file << timestamp << ",";
            std::visit([&](const auto& val) {
                file << val;
            }, pair.second);
            file << "," << pair.first << std::endl;
        }
    }
    
    return file.good();
}

bool TimelineVisualization::configure(const std::map<std::string, std::any>& config) {
    bool success = true;
    
    auto timeStartIt = config.find("time_start");
    if (timeStartIt != config.end()) {
        timeStart_ = std::any_cast<double>(timeStartIt->second);
    }
    
    auto timeEndIt = config.find("time_end");
    if (timeEndIt != config.end()) {
        timeEnd_ = std::any_cast<double>(timeEndIt->second);
    }
    
    auto timeAxisLabelIt = config.find("time_axis_label");
    if (timeAxisLabelIt != config.end()) {
        timeAxisLabel_ = std::any_cast<std::string>(timeAxisLabelIt->second);
    }
    
    auto dataSeriesIt = config.find("data_series");
    if (dataSeriesIt != config.end()) {
        dataSeries_ = std::any_cast<std::vector<std::string>>(dataSeriesIt->second);
    }
    
    auto aggregationMethodIt = config.find("aggregation_method");
    if (aggregationMethodIt != config.end()) {
        aggregationMethod_ = std::any_cast<std::string>(aggregationMethodIt->second);
    }
    
    return success;
}

std::map<std::string, std::any> TimelineVisualization::getConfig() const {
    std::map<std::string, std::any> config;
    config["time_start"] = timeStart_;
    config["time_end"] = timeEnd_;
    config["time_axis_label"] = timeAxisLabel_;
    config["data_series"] = dataSeries_;
    config["aggregation_method"] = aggregationMethod_;
    return config;
}

void TimelineVisualization::setTimeRange(double start, double end) {
    timeStart_ = start;
    timeEnd_ = end;
}

void TimelineVisualization::setTimeAxisLabel(const std::string& label) {
    timeAxisLabel_ = label;
}

void TimelineVisualization::setDataSeries(const std::vector<std::string>& series) {
    dataSeries_ = series;
}

void TimelineVisualization::setAggregationMethod(const std::string& method) {
    aggregationMethod_ = method;
}

// VisualizationExporter implementations
bool ImageVisualizationExporter::exportVisualization(const VisualizationData& data,
                                                   const std::string& filepath) {
    // In a real implementation, this would convert visualization data to an image
    std::ofstream file(filepath);
    if (!file.good()) {
        return false;
    }
    
    file << "Image visualization export for: " << data.title << std::endl;
    file << "Timestamp: " << utils::generateTimestamp() << std::endl;
    return file.good();
}

bool ImageVisualizationExporter::exportVisualization(std::ostream& stream,
                                                     const VisualizationData& data) {
    stream << "Image visualization export for: " << data.title << std::endl;
    stream << "Timestamp: " << utils::generateTimestamp() << std::endl;
    return true;
}

std::string ImageVisualizationExporter::exportVisualizationToString(const VisualizationData& data) {
    std::ostringstream oss;
    oss << "Image visualization export for: " << data.title << std::endl;
    oss << "Timestamp: " << utils::generateTimestamp() << std::endl;
    return oss.str();
}

bool SVGVisualizationExporter::exportVisualization(const VisualizationData& data,
                                                   const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.good()) {
        return false;
    }
    
    // Generate SVG content
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    oss << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"600\">" << std::endl;
    oss << "  <rect width=\"100%\" height=\"100%\" fill=\"#f0f0f0\"/>" << std::endl;
    oss << "  <text x=\"10\" y=\"20\" font-family=\"Arial\" font-size=\"16\" fill=\"#333\">";
    oss << "SVG Visualization: " << data.title << "</text>" << std::endl;
    oss << "</svg>" << std::endl;
    
    file << oss.str();
    return file.good();
}

bool SVGVisualizationExporter::exportVisualization(std::ostream& stream,
                                                   const VisualizationData& data) {
    // Generate SVG content
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    oss << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"600\">" << std::endl;
    oss << "  <rect width=\"100%\" height=\"100%\" fill=\"#f0f0f0\"/>" << std::endl;
    oss << "  <text x=\"10\" y=\"20\" font-family=\"Arial\" font-size=\"16\" fill=\"#333\">";
    oss << "SVG Visualization: " << data.title << "</text>" << std::endl;
    oss << "</svg>" << std::endl;
    
    stream << oss.str();
    return true;
}

std::string SVGVisualizationExporter::exportVisualizationToString(const VisualizationData& data) {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    oss << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"600\">" << std::endl;
    oss << "  <rect width=\"100%\" height=\"100%\" fill=\"#f0f0f0\"/>" << std::endl;
    oss << "  <text x=\"10\" y=\"20\" font-family=\"Arial\" font-size=\"16\" fill=\"#333\">";
    oss << "SVG Visualization: " << data.title << "</text>" << std::endl;
    oss << "</svg>" << std::endl;
    return oss.str();
}

bool JSONVisualizationExporter::exportVisualization(const VisualizationData& data,
                                                   const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.good()) {
        return false;
    }
    
    // Generate JSON content
    std::ostringstream oss;
    oss << "{\"visualization\": {\"title\": \"" << data.title << "\", \"type\": \"JSON\", \"timestamp\": \"";
    auto timestamp = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm = *std::localtime(&time_t);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    oss << buffer << "\", \"data\": {";
    
    bool first = true;
    for (const auto& pair : data.data) {
        if (!first) oss << ", ";
        first = false;
        
        oss << "\"" << pair.first << "\": ";
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, double>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, int>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else {
                oss << "null";
            }
        }, pair.second);
    }
    
    oss << "}}";
    
    file << oss.str();
    return file.good();
}

bool JSONVisualizationExporter::exportVisualization(std::ostream& stream,
                                                     const VisualizationData& data) {
    // Generate JSON content
    std::ostringstream oss;
    oss << "{\"visualization\": {\"title\": \"" << data.title << "\", \"type\": \"JSON\", \"timestamp\": \"";
    auto timestamp = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm = *std::localtime(&time_t);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    oss << buffer << "\", \"data\": {";
    
    bool first = true;
    for (const auto& pair : data.data) {
        if (!first) oss << ", ";
        first = false;
        
        oss << "\"" << pair.first << "\": ";
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, double>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, int>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else {
                oss << "null";
            }
        }, pair.second);
    }
    
    oss << "}}";
    
    stream << oss.str();
    return true;
}

std::string JSONVisualizationExporter::exportVisualizationToString(const VisualizationData& data) {
    std::ostringstream oss;
    oss << "{\"visualization\": {\"title\": \"" << data.title << "\", \"type\": \"JSON\", \"timestamp\": \"";
    auto timestamp = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm = *std::localtime(&time_t);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    oss << buffer << "\", \"data\": {";
    
    bool first = true;
    for (const auto& pair : data.data) {
        if (!first) oss << ", ";
        first = false;
        
        oss << "\"" << pair.first << "\": ";
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, double>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, int>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else {
                oss << "null";
            }
        }, pair.second);
    }
    
    oss << "}}";
    return oss.str();
}

} // namespace vizmon
} // namespace nlm