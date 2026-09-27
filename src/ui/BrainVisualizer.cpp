#include "BrainVisualizer.hpp"
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <iomanip>

namespace nlm {

BrainVisualizer::BrainVisualizer()
    : simulationMonitoringEnabled(false), monitoringInterval(1.0) {
    visualizationOptions["showConnections"] = true;
    visualizationOptions["showLabels"] = true;
    visualizationOptions["colorByActivity"] = true;
    visualizationOptions["opacity"] = 1.0;
}

BrainVisualizer::~BrainVisualizer() {}

void BrainVisualizer::addBrainRegion(const BrainRegionVisualization& region) {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    brainRegions.push_back(region);
}

void BrainVisualizer::addConnection(const ConnectionVisualization& connection) {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    connections.push_back(connection);
}

bool BrainVisualizer::removeBrainRegion(const std::string& regionName) {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    
    auto it = std::remove_if(brainRegions.begin(), brainRegions.end(),
        [&regionName](const BrainRegionVisualization& region) {
            return region.name == regionName;
        });n
    if (it != brainRegions.end()) {
        brainRegions.erase(it, brainRegions.end());
        return true;
    }
    return false;
}

bool BrainVisualizer::removeConnection(const std::string& fromRegion, const std::string& toRegion) {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    
    auto it = std::remove_if(connections.begin(), connections.end(),
        [&fromRegion, &toRegion](const ConnectionVisualization& conn) {
            return conn.fromRegion == fromRegion && conn.toRegion == toRegion;
        });n
    if (it != connections.end()) {
        connections.erase(it, connections.end());
        return true;
    }
    return false;
}

void BrainVisualizer::setTimeSeriesData(const TimeSeriesData& timeSeries) {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    timeSeriesData = timeSeries;
}

void BrainVisualizer::clearRegions() {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    brainRegions.clear();
}

void BrainVisualizer::clearConnections() {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    connections.clear();
}

VisualizationOutput BrainVisualizer::generateVisualization(VisualizationFormat format) const {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    VisualizationOutput output;
    output.timestamp = std::chrono::system_clock::now();

    switch (format) {
        case VisualizationFormat::ASCII_ART:
            output.content = generateASCIIArt();
            output.format = "ascii_art";
            output.filename = "brain_ascii_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                output.timestamp.time_since_epoch()).count()) + ".txt";
            break;
        case VisualizationFormat::SVG:
            output.content = generateSVG();
            output.format = "svg";
            output.filename = "brain_visualization.svg";
            break;
        case VisualizationFormat::PNG:
            output.content = generatePNG();
            output.format = "png";
            output.filename = "brain_visualization.png";
            break;
        case VisualizationFormat::GLTF:
            output.content = generateGLTF();
            output.format = "gltf";
            output.filename = "brain_visualization.gltf";
            break;
        case VisualizationFormat::JSON:
            output.content = generateJSON();
            output.format = "json";
            output.filename = "brain_visualization.json";
            break;
        case VisualizationFormat::YAML:
            output.content = generateYAML();
            output.format = "yaml";
            output.filename = "brain_visualization.yaml";
            break;
        case VisualizationFormat::HTML:
            output.content = generateHTML();
            output.format = "html";
            output.filename = "brain_visualization.html";
            break;
        case VisualizationFormat::TEXT:
            output.content = generateText();
            output.format = "text";
            output.filename = "brain_visualization.txt";
            break;
    }

    return output;
}

VisualizationOutput BrainVisualizer::generate2DVisualization() const {
    VisualizationOutput output;
    output.timestamp = std::chrono::system_clock::now();
    output.content = generate2DASCII();
    output.format = "2d_ascii";
    output.filename = "brain_2d_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
        output.timestamp.time_since_epoch()).count()) + ".txt";
    return output;
}

VisualizationOutput BrainVisualizer::generate3DVisualization() const {
    VisualizationOutput output;
    output.timestamp = std::chrono::system_clock::now();
    output.content = generate3DASCII();
    output.format = "3d_ascii";
    output.filename = "brain_3d_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
        output.timestamp.time_since_epoch()).count()) + ".txt";
    return output;
}

VisualizationOutput BrainVisualizer::generateAnimatedVisualization(double duration) const {
    VisualizationOutput output;
    output.timestamp = std::chrono::system_clock::now();
    output.content = generateAnimation(duration);
    output.format = "animation";
    output.filename = "brain_animation_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
        output.timestamp.time_since_epoch()).count()) + ".txt";
    return output;
}

bool BrainVisualizer::saveVisualization(const VisualizationOutput& output) const {
    std::ofstream file(output.filename);
    if (!file.is_open()) {
        return false;
    }
    file << output.content;
    file.close();
    return true;
}

std::vector<std::string> BrainVisualizer::getBrainRegionNames() const {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    std::vector<std::string> names;
    names.reserve(brainRegions.size());
    
    for (const auto& region : brainRegions) {
        names.push_back(region.name);
    }
    
    return names;
}

std::optional<BrainRegionVisualization> BrainVisualizer::getBrainRegion(const std::string& regionName) const {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    
    for (const auto& region : brainRegions) {
        if (region.name == regionName) {
            return region;
        }
    }
    
    return std::nullopt;
}

std::vector<ConnectionVisualization> BrainVisualizer::getConnections() const {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    return connections;
}

std::optional<TimeSeriesData> BrainVisualizer::getTimeSeriesData() const {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    return timeSeriesData;
}

void BrainVisualizer::setVisualizationOption(const std::string& key, const std::any& value) {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    visualizationOptions[key] = value;
}

std::optional<std::any> BrainVisualizer::getVisualizationOption(const std::string& key) const {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    
    auto it = visualizationOptions.find(key);
    if (it != visualizationOptions.end()) {
        return it->second;
    }
    
    return std::nullopt;
}

std::map<std::string, std::any> BrainVisualizer::getAllVisualizationOptions() const {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    return visualizationOptions;
}

void BrainVisualizer::enableSimulationMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    simulationMonitoringEnabled = enable;
}

void BrainVisualizer::setMonitoringInterval(double interval) {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    monitoringInterval = interval;
}

void BrainVisualizer::startSimulationMonitoring() {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    simulationMonitoringEnabled = true;
}

void BrainVisualizer::stopSimulationMonitoring() {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    simulationMonitoringEnabled = false;
}

void BrainVisualizer::updateVisualization(const std::map<std::string, double>& currentState) {
    std::lock_guard<std::mutex> lock(visualizationMutex);
    
    lastUpdateTime = std::chrono::system_clock::now();
    
    for (auto& region : brainRegions) {
        if (currentState.find(region.name) != currentState.end()) {
            region.activityLevel = currentState.at(region.name);
        }
    }
}

std::vector<VisualizationOutput> BrainVisualizer::exportMultipleFormats(const std::vector<VisualizationFormat>& formats) const {
    std::vector<VisualizationOutput> outputs;
    
    for (const auto& format : formats) {
        outputs.push_back(generateVisualization(format));
    }
    
    return outputs;
}

// Helper functions for visualization generation
std::string BrainVisualizer::generateASCIIArt() const {
    std::stringstream ss;
    ss << "=== Brain Visualization (ASCII Art) ===" << std::endl;
    ss << std::endl;
    
    for (const auto& region : brainRegions) {
        ss << "Region: " << region.name << " (Activity: " << region.activityLevel << ")" << std::endl;
        ss << "  Position: (" << region.x << ", " << region.y << ", " << region.z << ")" << std::endl;
        ss << "  Size: " << region.size << std::endl;
        ss << "  Color: " << region.color << std::endl;
        ss << std::endl;
    }
    
    ss << "Connections:" << std::endl;
    for (const auto& conn : connections) {
        ss << "  " << conn.fromRegion << " -> " << conn.toRegion 
           << " (Strength: " << conn.strength << ")" << std::endl;
    }
    
    return ss.str();
}

std::string BrainVisualizer::generate2DASCII() const {
    std::stringstream ss;
    ss << "=== 2D Brain Map ===" << std::endl;
    
    double minX = 0, maxX = 100, minY = 0, maxY = 100;
    
    for (double y = maxY; y >= minY; y -= 10) {
        ss << std::fixed << std::setprecision(1);
        ss << y << " | ";
        
        for (double x = minX; x <= maxX; x += 5) {
            bool found = false;
            for (const auto& region : brainRegions) {
                if (std::abs(x - region.x) < 2.5 && std::abs(y - region.y) < 2.5) {
                    ss << "*";
                    found = true;
                    break;
                }
            }
            if (!found) ss << ".";
        }
        ss << std::endl;
    }
    
    return ss.str();
}

std::string BrainVisualizer::generate3DASCII() const {
    std::stringstream ss;
    ss << "=== 3D Brain Visualization ===" << std::endl;
    
    for (const auto& region : brainRegions) {
        ss << "Region: " << region.name << std::endl;
        ss << "  Depth: " << std::string(static_cast<size_t>(region.z / 10), ' ') 
           << std::string(static_cast<size_t>(region.size / 10), '*') << std::endl;
        ss << "  Activity: " << std::string(static_cast<size_t>(region.activityLevel * 10), '=') << "|" 
           << std::string(static_cast<size_t>(10 - region.activityLevel * 10), '-') << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

std::string BrainVisualizer::generateAnimation(double duration) const {
    std::stringstream ss;
    ss << "=== Animated Brain Visualization (Duration: " << duration << "s) ===" << std::endl;
    
    int frames = static_cast<int>(duration * 10); // 10 frames per second
    
    for (int frame = 0; frame < frames; ++frame) {
        ss << "Frame " << frame << ": " << std::endl;
        
        for (const auto& region : brainRegions) {
            double animatedActivity = region.activityLevel * 
                (0.5 + 0.5 * std::sin(2 * M_PI * frame / frames));
            ss << "  " << region.name << ": " 
               << std::string(static_cast<size_t>(animatedActivity * 20), '*') << std::endl;
        }
        
        ss << std::endl;
    }
    
    return ss.str();
}

std::string BrainVisualizer::generateSVG() const {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<svg width=\"1000px\" height=\"1000px\" xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;
    
    for (const auto& conn : connections) {
        ss << "  <line x1=\"" << conn.fromRegion << "\" y1=\"0\" x2=\"" << conn.toRegion 
           << "\" y2=\"1\" stroke=\"" << conn.color << "\" stroke-width=\"" << conn.width 
           << "\" stroke-dasharray=\"" << (conn.style == "dashed" ? "5,5" : 
               conn.style == "dotted" ? "2,2" : "") << "\" />" << std::endl;
    }
    
    ss << "</svg>" << std::endl;
    return ss.str();
}

std::string BrainVisualizer::generatePNG() const {
    return "PNG format not implemented in ASCII fallback";
}

std::string BrainVisualizer::generateGLTF() const {
    std::stringstream ss;
    ss << "glTF format for 3D brain visualization" << std::endl;
    ss << "{\"nodes\": [" << std::endl;
    
    for (size_t i = 0; i < brainRegions.size(); ++i) {
        ss << "{\"position\": [" << brainRegions[i].x << ", " << brainRegions[i].y 
           << ", " << brainRegions[i].z << "], \"activity\": " << brainRegions[i].activityLevel 
           << "}" << (i < brainRegions.size() - 1 ? "," : "") << std::endl;
    }
    
    ss << "]}" << std::endl;
    return ss.str();
}

std::string BrainVisualizer::generateJSON() const {
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"brainRegions\": [" << std::endl;
    
    for (size_t i = 0; i < brainRegions.size(); ++i) {
        const auto& region = brainRegions[i];
        ss << "    {\"name\": \"" << region.name << "\", \"x\": " << region.x 
           << ", \"y\": " << region.y << ", \"z\": " << region.z 
           << ", \"activity\": " << region.activityLevel << "}" << 
           (i < brainRegions.size() - 1 ? "," : "") << std::endl;
    }
    
    ss << "  ]," << std::endl;
    ss << "  \"connections\": [" << std::endl;
    
    for (size_t i = 0; i < connections.size(); ++i) {
        const auto& conn = connections[i];
        ss << "    {\"from\": \"" << conn.fromRegion << "\", \"to\": \"" << conn.toRegion 
           << "\", \"strength\": " << conn.strength << "}" << 
           (i < connections.size() - 1 ? "," : "") << std::endl;
    }
    
    ss << "  ]" << std::endl;
    ss << "}" << std::endl;
    return ss.str();
}

std::string BrainVisualizer::generateYAML() const {
    std::stringstream ss;
    ss << "brain_regions:" << std::endl;
    
    for (const auto& region : brainRegions) {
        ss << "  - name: " << region.name << std::endl;
        ss << "    position: [" << region.x << ", " << region.y << ", " << region.z << "]" << std::endl;
        ss << "    size: " << region.size << std::endl;
        ss << "    activity: " << region.activityLevel << std::endl;
    }
    
    ss << "connections:" << std::endl;
    for (const auto& conn : connections) {
        ss << "  - from: " << conn.fromRegion << std::endl;
        ss << "    to: " << conn.toRegion << std::endl;
        ss << "    strength: " << conn.strength << std::endl;
    }
    
    return ss.str();
}

std::string BrainVisualizer::generateHTML() const {
    std::stringstream ss;
    ss << "<html><head><title>Brain Visualization</title></head><body>" << std::endl;
    ss << "<h1>Brain Visualization</h1>" << std::endl;
    ss << "<ul>" << std::endl;
    
    for (const auto& region : brainRegions) {
        ss << "<li>" << region.name << " (Activity: " << region.activityLevel << ")</li>" << std::endl;
    }
    
    ss << "</ul><h2>Connections:</h2><ul>" << std::endl;
    for (const auto& conn : connections) {
        ss << "<li>" << conn.fromRegion << " -> " << conn.toRegion 
           << " (Strength: " << conn.strength << ")</li>" << std::endl;
    }
    
    ss << "</ul></body></html>" << std::endl;
    return ss.str();
}

std::string BrainVisualizer::generateText() const {
    std::stringstream ss;
    ss << "Brain Visualization Summary" << std::endl;
    ss << "==========================" << std::endl;
    ss << std::endl;
    
    ss << "Regions: " << brainRegions.size() << std::endl;
    ss << "Connections: " << connections.size() << std::endl;
    
    if (timeSeriesData) {
        ss << "Time Series: " << timeSeriesData->timestamps.size() << " data points" << std::endl;
    }
    
    ss << std::endl;
    ss << "Region Details:" << std::endl;
    for (const auto& region : brainRegions) {
        ss << "  - " << region.name << ": Activity=" << region.activityLevel 
           << ", Size=" << region.size << std::endl;
    }
    
    ss << std::endl;
    ss << "Connection Details:" << std::endl;
    for (const auto& conn : connections) {
        ss << "  - " << conn.fromRegion << " -> " << conn.toRegion 
           << " (Strength: " << conn.strength << ", Color: " << conn.color << ")" << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm