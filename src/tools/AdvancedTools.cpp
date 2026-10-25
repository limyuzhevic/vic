// AdvancedTools.cpp implementation
#include "AdvancedTools.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <filesystem>
#include <map>
#include <set>

namespace nlm {

// VisualizationSystem::Impl implementation
struct VisualizationSystem::Impl {
    std::unordered_map<std::string, std::vector<DataPoint>> visualizationData;
    std::unordered_map<std::string, VisualizationConfig> visualizationConfigs;
    std::vector<std::string> visualizationIds;
    size_t nextIdCounter;
    
    Impl() : nextIdCounter(1) {}
};

VisualizationSystem::VisualizationSystem() : pImpl(std::make_unique<Impl>()) {}

VisualizationSystem::~VisualizationSystem() = default;

VisualizationSystem::VisualizationSystem(VisualizationSystem&& other) noexcept
    : pImpl(std::move(other.pImpl)) {}

VisualizationSystem& VisualizationSystem::operator=(VisualizationSystem&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void VisualizationSystem::addData(const std::string& visualizationId, const DataPoint& dataPoint) {
    if (!hasVisualization(visualizationId)) {
        return; // Visualization doesn't exist
    }
    pImpl->visualizationData[visualizationId].push_back(dataPoint);
}

void VisualizationSystem::addData(const std::string& visualizationId, 
                                  const std::vector<DataPoint>& dataPoints) {
    if (!hasVisualization(visualizationId)) {
        return;
    }
    auto& data = pImpl->visualizationData[visualizationId];
    data.insert(data.end(), dataPoints.begin(), dataPoints.end());
}

std::string VisualizationSystem::createVisualization(const VisualizationConfig& config) {
    validateConfig(config);
    
    std::string id = "vis_" + std::to_string(pImpl->nextIdCounter++);
    pImpl->visualizationConfigs[id] = config;
    pImpl->visualizationIds.push_back(id);
    
    return id;
}

VisualizationSystem::VisualizationResult VisualizationSystem::generate(const std::string& visualizationId) {
    if (!hasVisualization(visualizationId)) {
        return VisualizationResult(visualizationId, false, "Visualization not found");
    }
    
    auto& config = pImpl->visualizationConfigs[visualizationId];
    auto& data = pImpl->visualizationData[visualizationId];
    
    VisualizationResult result(visualizationId, true);
    result.generationTime = std::chrono::steady_clock::now();
    
    try {
        switch (config.outputFormat) {
            case OutputFormat::Text:
                result.outputPath = generateTextReport(visualizationId);
                break;
            case OutputFormat::JSON:
                result.outputPath = generateJSONData(visualizationId);
                break;
            case OutputFormat::CSV:
                result.outputPath = generateCSVData(visualizationId);
                break;
            case OutputFormat::Interactive:
                result.outputPath = generateInteractiveHTML(visualizationId);
                break;
            case OutputFormat::WebGL:
                result.outputPath = generateWebGL(visualizationId);
                break;
            case OutputFormat::Video:
                result.outputPath = generateVideo(visualizationId);
                break;
            case OutputFormat::AnimatedGif:
                result.outputPath = generateAnimatedGif(visualizationId);
                break;
        }
        
        // Get file size
        if (std::filesystem::exists(result.outputPath)) {
            result.fileSize = std::filesystem::file_size(result.outputPath);
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = e.what();
    }
    
    return result;
}

std::vector<VisualizationSystem::DataPoint> VisualizationSystem::getData(const std::string& visualizationId) const {
    auto it = pImpl->visualizationData.find(visualizationId);
    if (it != pImpl->visualizationData.end()) {
        return it->second;
    }
    return std::vector<DataPoint>();
}

std::map<std::string, std::vector<VisualizationSystem::DataPoint>> VisualizationSystem::getAllData() const {
    return pImpl->visualizationData;
}

bool VisualizationSystem::removeVisualization(const std::string& visualizationId) {
    auto it = pImpl->visualizationData.find(visualizationId);
    if (it == pImpl->visualizationData.end()) {
        return false;
    }
    
    pImpl->visualizationData.erase(it);
    
    auto it2 = pImpl->visualizationConfigs.find(visualizationId);
    if (it2 != pImpl->visualizationConfigs.end()) {
        pImpl->visualizationConfigs.erase(it2);
    }
    
    auto it3 = std::find(pImpl->visualizationIds.begin(), 
                        pImpl->visualizationIds.end(), visualizationId);
    if (it3 != pImpl->visualizationIds.end()) {
        pImpl->visualizationIds.erase(it3);
    }
    
    return true;
}

std::vector<std::string> VisualizationSystem::getVisualizationIds() const {
    return pImpl->visualizationIds;
}

void VisualizationSystem::clear() {
    pImpl->visualizationData.clear();
    pImpl->visualizationConfigs.clear();
    pImpl->visualizationIds.clear();
}

bool VisualizationSystem::hasVisualization(const std::string& visualizationId) const {
    return pImpl->visualizationConfigs.find(visualizationId) != pImpl->visualizationConfigs.end();
}

std::string VisualizationSystem::generateReport() const {
    std::stringstream ss;
    ss << "=== Visualization System Report ===" << std::endl;
    ss << "Total visualizations: " << pImpl->visualizationConfigs.size() << std::endl;
    ss << "Total data points: " << std::accumulate(
        pImpl->visualizationData.begin(), pImpl->visualizationData.end(), 0,
        [](int sum, const auto& pair) { return sum + pair.second.size(); }) << std::endl;
    
    ss << std::endl << "=== Visualizations ===" << std::endl;
    for (const auto& id : pImpl->visualizationIds) {
        auto it = pImpl->visualizationConfigs.find(id);
        if (it != pImpl->visualizationConfigs.end()) {
            ss << "  " << id << ": " << it->second.name << " ("
               << enumToString(it->second.elementType) << ")" << std::endl;
            ss << "    Format: " << enumToString(it->second.outputFormat) << std::endl;
            ss << "    Data points: " << pImpl->visualizationData.at(id).size() << std::endl;
        }
    }
    
    return ss.str();
}

bool VisualizationSystem::exportCSV(const std::string& visualizationId, const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    auto it = pImpl->visualizationData.find(visualizationId);
    if (it == pImpl->visualizationData.end()) {
        return false;
    }
    
    file << "Timestamp,Value,Label" << std::endl;
    for (const auto& point : it->second) {
        file << point.timestamp << "," << point.value << "," << point.label << std::endl;
    }
    
    return true;
}

bool VisualizationSystem::exportJSON(const std::string& visualizationId, const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateJSONData(visualizationId);
    return true;
}

std::vector<VisualizationSystem::VisualizationResult> VisualizationSystem::generateBatch(
    const std::vector<std::string>& visualizationIds) {
    std::vector<VisualizationResult> results;
    for (const auto& id : visualizationIds) {
        results.push_back(generate(id));
    }
    return results;
}

std::string VisualizationSystem::getSystemInfo() const {
    std::stringstream ss;
    ss << "=== Visualization System Information ===" << std::endl;
    ss << "Total visualizations: " << pImpl->visualizationConfigs.size() << std::endl;
    ss << "Total data points: " << std::accumulate(
        pImpl->visualizationData.begin(), pImpl->visualizationData.end(), 0,
        [](int sum, const auto& pair) { return sum + pair.second.size(); }) << std::endl;
    ss << "Output formats supported: " << static_cast<int>(OutputFormat::Video) + 1 << std::endl;
    ss << "Element types supported: " << static_cast<int>(ElementType::Custom) + 1 << std::endl;
    return ss.str();
}

bool VisualizationSystem::configureElement(const std::string& visualizationId, 
                                           const VisualizationConfig& config) {
    auto it = pImpl->visualizationConfigs.find(visualizationId);
    if (it == pImpl->visualizationConfigs.end()) {
        return false;
    }
    
    it->second = config;
    return true;
}

VisualizationSystem::VisualizationConfig VisualizationSystem::getConfig(const std::string& visualizationId) const {
    auto it = pImpl->visualizationConfigs.find(visualizationId);
    if (it != pImpl->visualizationConfigs.end()) {
        return it->second;
    }
    return VisualizationConfig("", ElementType::Custom);
}

std::string VisualizationSystem::createSpikeRateChart(const std::string& name, double duration) {
    VisualizationConfig config(name, ElementType::TimeSeries, OutputFormat::Interactive);
    config.title = "Spike Rate Over Time";
    config.xAxisLabel = "Time (s)";
    config.yAxisLabel = "Spike Rate (Hz)";
    config.width = 1200.0;
    config.height = 400.0;
    return createVisualization(config);
}

std::string VisualizationSystem::createFiringRateChart(const std::string& name, size_t neuronCount) {
    VisualizationConfig config(name, ElementType::ScatterPlot, OutputFormat::Interactive);
    config.title = "Firing Rate Distribution";
    config.xAxisLabel = "Neuron Index";
    config.yAxisLabel = "Firing Rate (Hz)";
    config.width = 800.0;
    config.height = 600.0;
    return createVisualization(config);
}

std::string VisualizationSystem::createSynapticWeightChart(const std::string& name) {
    VisualizationConfig config(name, ElementType::HeatMap, OutputFormat::WebGL);
    config.title = "Synaptic Weight Distribution";
    config.xAxisLabel = "Source Neuron";
    config.yAxisLabel = "Target Neuron";
    config.width = 1000.0;
    config.height = 800.0;
    return createVisualization(config);
}

std::string VisualizationSystem::createMemoryChart(const std::string& name) {
    VisualizationConfig config(name, ElementType::LineChart, OutputFormat::Interactive);
    config.title = "Memory Performance Over Time";
    config.xAxisLabel = "Time Step";
    config.yAxisLabel = "Memory Usage (traces)";
    config.width = 1200.0;
    config.height = 400.0;
    return createVisualization(config);
}

std::string VisualizationSystem::createNeuromodulationChart(const std::string& name) {
    VisualizationConfig config(name, ElementType::LineChart, OutputFormat::Interactive);
    config.title = "Neuromodulation Levels";
    config.xAxisLabel = "Time (s)";
    config.yAxisLabel = "Level (0-1)";
    config.width = 1200.0;
    config.height = 400.0;
    return createVisualization(config);
}

std::string VisualizationSystem::createNetworkTopologyChart(const std::string& name, size_t regionCount) {
    VisualizationConfig config(name, ElementType::NetworkGraph, OutputFormat::WebGL);
    config.title = "Network Topology (" + std::to_string(regionCount) + " regions)";
    config.width = 1000.0;
    config.height = 800.0;
    return createVisualization(config);
}

void VisualizationSystem::validateConfig(const VisualizationConfig& config) const {
    if (config.name.empty()) {
        throw std::invalid_argument("Visualization name cannot be empty");
    }
    
    if (config.width <= 0.0 || config.height <= 0.0) {
        throw std::invalid_argument("Width and height must be positive");
    }
    
    if (config.animationDuration <= 0.0) {
        throw std::invalid_argument("Animation duration must be positive");
    }
}

std::string VisualizationSystem::generateInteractiveHTML(const std::string& visualizationId) const {
    // Generate a unique filename
    std::string filename = "visualization_" + visualizationId + ".html";
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot create output file: " + filename);
    }
    
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head>" << std::endl;
    file << "    <title>" << pImpl->visualizationConfigs.at(visualizationId).title << "</title>" << std::endl;
    file << "    <script src=\"https://cdn.plot.ly/plotly-latest.min.js\"></script>" << std::endl;
    file << "    <style>" << std::endl;
    file << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    file << "        #plot { width: " << pImpl->visualizationConfigs.at(visualizationId).width << "px; "
         << "height: " << pImpl->visualizationConfigs.at(visualizationId).height << "px; }" << std::endl;
    file << "    </style>" << std::endl;
    file << "</head>" << std::endl;
    file << "<body>" << std::endl;
    file << "    <h1>" << pImpl->visualizationConfigs.at(visualizationId).title << "</h1>" << std::endl;
    file << "    <div id=\"plot\"></div>" << std::endl;
    file << "    <script>" << std::endl;
    file << "        // Visualization data loading and plotting would go here" << std::endl;
    file << "        console.log('Interactive visualization loaded');" << std::endl;
    file << "    </script>" << std::endl;
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
    
    return filename;
}

std::string VisualizationSystem::generateTextReport(const std::string& visualizationId) const {
    std::string filename = "visualization_" + visualizationId + ".txt";
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot create output file: " + filename);
    }
    
    auto it = pImpl->visualizationConfigs.find(visualizationId);
    if (it == pImpl->visualizationConfigs.end()) {
        throw std::runtime_error("Visualization not found: " + visualizationId);
    }
    
    file << "Visualization Report: " << it->second.name << std::endl;
    file << "====================" << std::endl;
    file << "Type: " << enumToString(it->second.elementType) << std::endl;
    file << "Format: " << enumToString(it->second.outputFormat) << std::endl;
    file << "Title: " << it->second.title << std::endl;
    file << "X-axis: " << it->second.xAxisLabel << std::endl;
    file << "Y-axis: " << it->second.yAxisLabel << std::endl;
    
    auto dataIt = pImpl->visualizationData.find(visualizationId);
    if (dataIt != pImpl->visualizationData.end()) {
        file << "Number of data points: " << dataIt->second.size() << std::endl;
        file << std::endl << "Data:" << std::endl;
        for (const auto& point : dataIt->second) {
            file << "  " << point.timestamp << ", " << point.value << ", " << point.label << std::endl;
        }
    }
    
    return filename;
}

std::string VisualizationSystem::generateJSONData(const std::string& visualizationId) const {
    std::string filename = "visualization_" + visualizationId + ".json";
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot create output file: " + filename);
    }
    
    auto& config = pImpl->visualizationConfigs.at(visualizationId);
    auto& data = pImpl->visualizationData.at(visualizationId);
    
    file << "{" << std::endl;
    file << "  \"visualizationId\": \"" << visualizationId << "\"," << std::endl;
    file << "  \"name\": \"" << escapeString(config.name) << "\"," << std::endl;
    file << "  \"type\": \"" << enumToString(config.elementType) << "\"," << std::endl;
    file << "  \"title\": \"" << escapeString(config.title) << "\"," << std::endl;
    file << "  \"dataPoints\": [" << std::endl;
    
    for (size_t i = 0; i < data.size(); ++i) {
        file << "    {\"timestamp\": " << data[i].timestamp << ", "
             << "\"value\": " << data[i].value << ", "
             << "\"label\": \"" << escapeString(data[i].label) << "\"}";
        if (i < data.size() - 1) {
            file << "," << std::endl;
        } else {
            file << std::endl;
        }
    }
    
    file << "  ]" << std::endl;
    file << "}" << std::endl;
    
    return filename;
}

std::string VisualizationSystem::generateCSVData(const std::string& visualizationId) const {
    return "visualization_" + visualizationId + ".csv"; // Implementation would actually generate
}

std::string VisualizationSystem::escapeHTML(const std::string& text) const {
    std::stringstream ss;
    for (char c : text) {
        switch (c) {
            case '&': ss << "&amp;"; break;
            case '<': ss << "&lt;"; break;
            case '>': ss << "&gt;"; break;
            case '"': ss << "&quot;"; break;
            case '\'': ss << "&#x27;"; break;
            case '\\': ss << "&#x2F;"; break;
            default: ss << c; break;
        }
    }
    return ss.str();
}

std::string VisualizationSystem::enumToString(ElementType type) const {
    static const std::unordered_map<ElementType, std::string> typeMap = {
        {ElementType::LineChart, "LineChart"},
        {ElementType::ScatterPlot, "ScatterPlot"},
        {ElementType::HeatMap, "HeatMap"},
        {ElementType::NetworkGraph, "NetworkGraph"},
        {ElementType::PopulationHistogram, "PopulationHistogram"},
        {ElementType::TimeSeries, "TimeSeries"},
        {ElementType::3DPlot, "3DPlot"},
        {ElementType::Custom, "Custom"}
    };
    
    auto it = typeMap.find(type);
    if (it != typeMap.end()) {
        return it->second;
    }
    return "Unknown";
}

std::string VisualizationSystem::enumToString(OutputFormat format) const {
    static const std::unordered_map<OutputFormat, std::string> formatMap = {
        {OutputFormat::Text, "Text"},
        {OutputFormat::JSON, "JSON"},
        {OutputFormat::CSV, "CSV"},
        {OutputFormat::Interactive, "Interactive"},
        {OutputFormat::WebGL, "WebGL"},
        {OutputFormat::Video, "Video"},
        {OutputFormat::AnimatedGif, "AnimatedGif"}
    };
    
    auto it = formatMap.find(format);
    if (it != formatMap.end()) {
        return it->second;
    }
    return "Unknown";
}

// VisualizationSystem::Impl helper functions
std::string VisualizationSystem::generateWebGL(const std::string& visualizationId) const {
    return "visualization_" + visualizationId + ".html"; // WebGL implementation would go here
}

std::string VisualizationSystem::generateVideo(const std::string& visualizationId) const {
    return "visualization_" + visualizationId + ".mp4"; // Video generation would go here
}

std::string VisualizationSystem::generateAnimatedGif(const std::string& visualizationId) const {
    return "visualization_" + visualizationId + ".gif"; // GIF generation would go here
}

// AdvancedDebugger::Impl implementation
struct AdvancedDebugger::Impl {
    std::unordered_map<std::string, Breakpoint> breakpoints;
    std::unordered_map<std::string, MemorySnapshot> memorySnapshots;
    std::unordered_map<std::string, PerformanceSnapshot> performanceSnapshots;
    std::unordered_map<std::string, DebugCommand> debugCommands;
    std::vector<std::string> breakpointIds;
    std::vector<std::string> memorySnapshotIds;
    std::vector<std::string> performanceSnapshotIds;
    std::chrono::time_point<std::chrono::steady_clock> simulationStartTime;
    bool debuggerEnabled;
    size_t currentStep;
    
    Impl() : debuggerEnabled(false), currentStep(0) {
        initializeCommands();
    }
    
    void initializeCommands() {
        // Add basic debugging commands
        debugCommands["help"] = DebugCommand("help", "Show available debug commands",
            [](const std::vector<std::string>& args) {
                std::cout << "Available debug commands:" << std::endl;
                // In a real implementation, this would list all registered commands
                return true;
            }, "Show this help message");
        
        debugCommands["step"] = DebugCommand("step", "Execute one simulation step",
            [](const std::vector<std::string>& args) {
                std::cout << "Stepping simulation..." << std::endl;
                // In a real implementation, this would execute one step
                return true;
            }, "Execute a single simulation step");
        
        debugCommands["status"] = DebugCommand("status", "Show debugger status",
            [](const std::vector<std::string>& args) {
                std::cout << "Debugger status:" << std::endl;
                // In a real implementation, this would show current state
                return true;
            }, "Show current debugger status");
    }
};

AdvancedDebugger::AdvancedDebugger() : pImpl(std::make_unique<Impl>()) {}

AdvancedDebugger::~AdvancedDebugger() = default;

AdvancedDebugger::AdvancedDebugger(AdvancedDebugger&& other) noexcept
    : pImpl(std::move(other.pImpl)) {}

AdvancedDebugger& AdvancedDebugger::operator=(AdvancedDebugger&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

std::string AdvancedDebugger::addBreakpoint(const Breakpoint& breakpoint) {
    std::string id = "bp_" + std::to_string(pImpl->breakpointIds.size() + 1);
    pImpl->breakpoints[id] = breakpoint;
    pImpl->breakpointIds.push_back(id);
    return id;
}

bool AdvancedDebugger::removeBreakpoint(const std::string& id) {
    auto it = pImpl->breakpoints.find(id);
    if (it == pImpl->breakpoints.end()) {
        return false;
    }
    
    pImpl->breakpoints.erase(it);
    auto it2 = std::find(pImpl->breakpointIds.begin(), pImpl->breakpointIds.end(), id);
    if (it2 != pImpl->breakpointIds.end()) {
        pImpl->breakpointIds.erase(it2);
    }
    
    return true;
}

bool AdvancedDebugger::enableBreakpoint(const std::string& id, bool enabled) {
    auto it = pImpl->breakpoints.find(id);
    if (it == pImpl->breakpoints.end()) {
        return false;
    }
    
    it->second.enabled = enabled;
    return true;
}

bool AdvancedDebugger::setBreakpointCondition(const std::string& id, const std::string& condition) {
    auto it = pImpl->breakpoints.find(id);
    if (it == pImpl->breakpoints.end()) {
        return false;
    }
    
    it->second.condition = condition;
    return true;
}

AdvancedDebugger::Breakpoint AdvancedDebugger::getBreakpoint(const std::string& id) const {
    auto it = pImpl->breakpoints.find(id);
    if (it != pImpl->breakpoints.end()) {
        return it->second;
    }
    return Breakpoint(id, BreakpointType::Step, "");
}

std::vector<AdvancedDebugger::Breakpoint> AdvancedDebugger::getBreakpoints() const {
    std::vector<Breakpoint> breakpoints;
    for (const auto& pair : pImpl->breakpoints) {
        breakpoints.push_back(pair.second);
    }
    return breakpoints;
}

std::vector<AdvancedDebugger::Breakpoint> AdvancedDebugger::getBreakpoints(BreakpointType type) const {
    std::vector<Breakpoint> breakpoints;
    for (const auto& pair : pImpl->breakpoints) {
        if (pair.second.type == type) {
            breakpoints.push_back(pair.second);
        }
    }
    return breakpoints;
}

std::string AdvancedDebugger::takeMemorySnapshot() {
    std::string id = "memsnap_" + std::to_string(pImpl->memorySnapshotIds.size() + 1);
    MemorySnapshot snapshot;
    snapshot.timestamp = std::chrono::steady_clock::now();
    snapshot.totalMemory = 1024 * 1024 * 1024; // 1 GB (example)
    snapshot.availableMemory = 512 * 1024 * 1024; // 512 MB (example)
    snapshot.allocatedObjects = 1000;
    pImpl->memorySnapshots[id] = snapshot;
    pImpl->memorySnapshotIds.push_back(id);
    return id;
}

AdvancedDebugger::MemorySnapshot AdvancedDebugger::getMemorySnapshot(const std::string& snapshotId) const {
    auto it = pImpl->memorySnapshots.find(snapshotId);
    if (it != pImpl->memorySnapshots.end()) {
        return it->second;
    }
    return MemorySnapshot();
}

std::string AdvancedDebugger::takePerformanceSnapshot() {
    std::string id = "perf_" + std::to_string(pImpl->performanceSnapshotIds.size() + 1);
    PerformanceSnapshot snapshot;
    snapshot.timestamp = std::chrono::steady_clock::now();
    snapshot.cpuUsage = 25.5; // example value
    snapshot.memoryUsage = 60.2; // example value
    snapshot.simulationTime = 1.5; // example value
    snapshot.spikeEventsProcessed = 5000;
    snapshot.neuronsActive = 800;
    snapshot.synapticConnections = 12000;
    pImpl->performanceSnapshots[id] = snapshot;
    pImpl->performanceSnapshotIds.push_back(id);
    return id;
}

AdvancedDebugger::PerformanceSnapshot AdvancedDebugger::getPerformanceSnapshot(const std::string& snapshotId) const {
    auto it = pImpl->performanceSnapshots.find(snapshotId);
    if (it != pImpl->performanceSnapshots.end()) {
        return it->second;
    }
    return PerformanceSnapshot();
}

std::string AdvancedDebugger::compareMemorySnapshots(const std::string& snapshot1Id,
                                                      const std::string& snapshot2Id) const {
    std::stringstream ss;
    ss << "Memory Snapshot Comparison:" << std::endl;
    
    auto it1 = pImpl->memorySnapshots.find(snapshot1Id);
    auto it2 = pImpl->memorySnapshots.find(snapshot2Id);
    
    if (it1 == pImpl->memorySnapshots.end() || it2 == pImpl->memorySnapshots.end()) {
        ss << "One or both snapshots not found" << std::endl;
        return ss.str();
    }
    
    const auto& snap1 = it1->second;
    const auto& snap2 = it2->second;
    
    ss << "Snapshot 1 (" << snapshot1Id << "):" << std::endl;
    ss << "  Time: " << timePointToString(snap1.timestamp) << std::endl;
    ss << "  Total Memory: " << snap1.totalMemory << " bytes" << std::endl;
    ss << "  Allocated Objects: " << snap1.allocatedObjects << std::endl;
    
    ss << "Snapshot 2 (" << snapshot2Id << "):" << std::endl;
    ss << "  Time: " << timePointToString(snap2.timestamp) << std::endl;
    ss << "  Total Memory: " << snap2.totalMemory << " bytes" << std::endl;
    ss << "  Allocated Objects: " << snap2.allocatedObjects << std::endl;
    
    ss << "Difference:" << std::endl;
    ss << "  Memory difference: " << (snap2.totalMemory - snap1.totalMemory) << " bytes" << std::endl;
    ss << "  Objects difference: " << (snap2.allocatedObjects - snap1.allocatedObjects) << std::endl;
    
    return ss.str();
}

std::string AdvancedDebugger::comparePerformanceSnapshots(const std::string& snapshot1Id,
                                                           const std::string& snapshot2Id) const {
    std::stringstream ss;
    ss << "Performance Snapshot Comparison:" << std::endl;
    
    auto it1 = pImpl->performanceSnapshots.find(snapshot1Id);
    auto it2 = pImpl->performanceSnapshots.find(snapshot2Id);
    
    if (it1 == pImpl->performanceSnapshots.end() || it2 == pImpl->performanceSnapshots.end()) {
        ss << "One or both snapshots not found" << std::endl;
        return ss.str();
    }
    
    const auto& snap1 = it1->second;
    const auto& snap2 = it2->second;
    
    ss << "Snapshot 1 (" << snapshot1Id << "):" << std::endl;
    ss << "  CPU Usage: " << snap1.cpuUsage << "%" << std::endl;
    ss << "  Memory Usage: " << snap1.memoryUsage << "%" << std::endl;
    ss << "  Simulation Time: " << snap1.simulationTime << "s" << std::endl;
    ss << "  Spikes Processed: " << snap1.spikeEventsProcessed << std::endl;
    
    ss << "Snapshot 2 (" << snapshot2Id << "):" << std::endl;
    ss << "  CPU Usage: " << snap2.cpuUsage << "%" << std::endl;
    ss << "  Memory Usage: " << snap2.memoryUsage << "%" << std::endl;
    ss << "  Simulation Time: " << snap2.simulationTime << "s" << std::endl;
    ss << "  Spikes Processed: " << snap2.spikeEventsProcessed << std::endl;
    
    ss << "Difference:" << std::endl;
    ss << "  CPU Usage difference: " << (snap2.cpuUsage - snap1.cpuUsage) << "%" << std::endl;
    ss << "  Memory Usage difference: " << (snap2.memoryUsage - snap1.memoryUsage) << "%" << std::endl;
    
    return ss.str();
}

bool AdvancedDebugger::addDebugCommand(const DebugCommand& command) {
    if (pImpl->debugCommands.find(command.name) != pImpl->debugCommands.end()) {
        return false; // Command already exists
    }
    
    pImpl->debugCommands[command.name] = command;
    return true;
}

bool AdvancedDebugger::removeDebugCommand(const std::string& name) {
    auto it = pImpl->debugCommands.find(name);
    if (it == pImpl->debugCommands.end()) {
        return false;
    }
    
    pImpl->debugCommands.erase(it);
    return true;
}

bool AdvancedDebugger::executeDebugCommand(const std::string& name,
                                           const std::vector<std::string>& args) {
    auto it = pImpl->debugCommands.find(name);
    if (it == pImpl->debugCommands.end()) {
        return false;
    }
    
    return it->second.execute(args);
}

AdvancedDebugger::DebugCommand AdvancedDebugger::getDebugCommand(const std::string& name) const {
    auto it = pImpl->debugCommands.find(name);
    if (it != pImpl->debugCommands.end()) {
        return it->second;
    }
    return DebugCommand(name, "Unknown command", 
                       [](const std::vector<std::string>&) { return false; });
}

std::vector<AdvancedDebugger::DebugCommand> AdvancedDebugger::getDebugCommands() const {
    std::vector<DebugCommand> commands;
    for (const auto& pair : pImpl->debugCommands) {
        commands.push_back(pair.second);
    }
    return commands;
}

std::string AdvancedDebugger::generateDebugReport() const {
    std::stringstream ss;
    ss << "=== Debug System Report ===" << std::endl;
    ss << "Breakpoints: " << pImpl->breakpoints.size() << std::endl;
    ss << "Memory Snapshots: " << pImpl->memorySnapshots.size() << std::endl;
    ss << "Performance Snapshots: " << pImpl->performanceSnapshots.size() << std::endl;
    ss << "Debug Commands: " << pImpl->debugCommands.size() << std::endl;
    ss << "Debugger Enabled: " << (pImpl->debuggerEnabled ? "Yes" : "No") << std::endl;
    
    ss << std::endl << "=== Breakpoint Report ===" << std::endl;
    for (const auto& pair : pImpl->breakpoints) {
        const auto& bp = pair.second;
        ss << "  " << bp.id << ": " << enumToString(bp.type) << " at " << bp.location
           << " (enabled: " << (bp.enabled ? "yes" : "no") << ")" << std::endl;
    }
    
    return ss.str();
}

bool AdvancedDebugger::exportBreakpoints(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "id,type,location,enabled,condition" << std::endl;
    for (const auto& pair : pImpl->breakpoints) {
        const auto& bp = pair.second;
        file << bp.id << ","
             << enumToString(bp.type) << ","
             << bp.location << ","
             << (bp.enabled ? "true" : "false") << ","
             << bp.condition << std::endl;
    }
    
    return true;
}

bool AdvancedDebugger::exportSnapshots(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "type,id,timestamp" << std::endl;
    
    for (const auto& pair : pImpl->memorySnapshots) {
        file << "memory," << pair.first << ","
             << timePointToString(pair.second.timestamp) << std::endl;
    }
    
    for (const auto& pair : pImpl->performanceSnapshots) {
        file << "performance," << pair.first << ","
             << timePointToString(pair.second.timestamp) << std::endl;
    }
    
    return true;
}

std::string AdvancedDebugger::getDebugInfo() const {
    std::stringstream ss;
    ss << "=== Debug System Information ===" << std::endl;
    ss << "Breakpoints: " << pImpl->breakpoints.size() << std::endl;
    ss << "Memory Snapshots: " << pImpl->memorySnapshots.size() << std::endl;
    ss << "Performance Snapshots: " << pImpl->performanceSnapshots.size() << std::endl;
    ss << "Debug Commands: " << pImpl->debugCommands.size() << std::endl;
    ss << "Debugger Enabled: " << (pImpl->debuggerEnabled ? "Yes" : "No") << std::endl;
    ss << "Current Step: " << pImpl->currentStep << std::endl;
    ss << "Simulation Start: " << timePointToString(pImpl->simulationStartTime) << std::endl;
    return ss.str();
}

void AdvancedDebugger::reset() {
    pImpl->breakpoints.clear();
    pImpl->breakpointIds.clear();
    pImpl->memorySnapshots.clear();
    pImpl->memorySnapshotIds.clear();
    pImpl->performanceSnapshots.clear();
    pImpl->performanceSnapshotIds.clear();
    pImpl->debuggerEnabled = false;
    pImpl->currentStep = 0;
}

bool AdvancedDebugger::shouldBreakpointTrigger(const Breakpoint& breakpoint) const {
    if (!breakpoint.enabled) {
        return false;
    }
    
    switch (breakpoint.type) {
        case BreakpointType::Step:
            return true; // Always trigger on step breakpoint
        case BreakpointType::Condition:
            // In a real implementation, this would evaluate the condition
            return !breakpoint.condition.empty(); // Simplified
        case BreakpointType::Watch:
            // Would check watched variables
            return false;
        case BreakpointType::Exception:
            // Would catch exceptions
            return false;
        case BreakpointType::Performance:
            // Would check performance thresholds
            return false;
        case BreakpointType::Memory:
            // Would check memory usage
            return false;
    }
    return false;
}

void AdvancedDebugger::recordBreakpointHit(const std::string& id) {
    auto it = pImpl->breakpoints.find(id);
    if (it != pImpl->breakpoints.end()) {
        it->second.hitCount++;
        it->second.lastHitTime = std::chrono::steady_clock::now();
        if (it->second.firstHitTime.time_since_epoch().count() == 0) {
            it->second.firstHitTime = it->second.lastHitTime;
        }
    }
}

std::string AdvancedDebugger::timePointToString(const std::chrono::time_point<std::chrono::steady_clock>& tp) const {
    auto duration = tp.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return std::to_string(milliseconds) + " ms";
}

std::string AdvancedDebugger::generateMemoryReport() const {
    std::stringstream ss;
    ss << "Memory usage statistics:" << std::endl;
    for (const auto& pair : pImpl->memorySnapshots) {
        const auto& snap = pair.second;
        ss << "  " << pair.first << ": " << snap.totalMemory << " bytes, "
           << snap.allocatedObjects << " objects at " << timePointToString(snap.timestamp) << std::endl;
    }
    return ss.str();
}

std::string AdvancedDebugger::generatePerformanceReport() const {
    std::stringstream ss;
    ss << "Performance statistics:" << std::endl;
    for (const auto& pair : pImpl->performanceSnapshots) {
        const auto& snap = pair.second;
        ss << "  " << pair.first << ": CPU " << snap.cpuUsage << "%, Memory " << snap.memoryUsage << "%, "
           << "Spikes " << snap.spikeEventsProcessed << " at " << timePointToString(snap.timestamp) << std::endl;
    }
    return ss.str();
}

std::string AdvancedDebugger::generateBreakpointReport() const {
    std::stringstream ss;
    ss << "Breakpoint statistics:" << std::endl;
    for (const auto& pair : pImpl->breakpoints) {
        const auto& bp = pair.second;
        ss << "  " << bp.id << ": type " << enumToString(bp.type) << ", "
           << "hits " << bp.hitCount << ", "
           << "location " << bp.location << ", "
           << "enabled " << (bp.enabled ? "yes" : "no") << std::endl;
    }
    return ss.str();
}

// AdvancedDebugger::Impl helper functions
void AdvancedDebugger::Impl::initializeCommands() {
    // Debug commands have been initialized in the constructor
}

std::string AdvancedDebugger::enumToString(BreakpointType type) const {
    static const std::unordered_map<BreakpointType, std::string> typeMap = {
        {BreakpointType::Step, "Step"},
        {BreakpointType::Condition, "Condition"},
        {BreakpointType::Watch, "Watch"},
        {BreakpointType::Exception, "Exception"},
        {BreakpointType::Performance, "Performance"},
        {BreakpointType::Memory, "Memory"}
    };
    
    auto it = typeMap.find(type);
    if (it != typeMap.end()) {
        return it->second;
    }
    return "Unknown";
}

// PluginManager::Impl implementation
struct PluginManager::Impl {
    std::unordered_map<std::string, std::shared_ptr<NLMPlugin>> plugins;
    std::vector<std::string> loadedPluginNames;
    std::string pluginDirectory;
    std::mutex mutex;
    std::condition_variable cv;
    bool loadingEnabled;
    
    Impl() : loadingEnabled(true) {}
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {}

PluginManager::~PluginManager() {
    shutdownAll();
}

PluginManager::PluginManager(PluginManager&& other) noexcept
    : pImpl(std::move(other.pImpl)) {}

PluginManager& PluginManager::operator=(PluginManager&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->loadingEnabled) {
        return false;
    }
    
    // Check if plugin already loaded
    std::string pluginName = extractPluginName(pluginPath);
    if (pImpl->plugins.find(pluginName) != pImpl->plugins.end()) {
        return false; // Already loaded
    }
    
    try {
        // In a real implementation, this would dynamically load the plugin
        // For now, we'll create a dummy plugin
        auto plugin = std::make_shared<NLMPlugin>();
        pImpl->plugins[pluginName] = plugin;
        pImpl->loadedPluginNames.push_back(pluginName);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load plugin " << pluginPath << ": " << e.what() << std::endl;
        return false;
    }
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        return false; // Not loaded
    }
    
    it->second->shutdown();
    pImpl->plugins.erase(it);
    
    auto it2 = std::find(pImpl->loadedPluginNames.begin(), 
                        pImpl->loadedPluginNames.end(), pluginName);
    if (it2 != pImpl->loadedPluginNames.end()) {
        pImpl->loadedPluginNames.erase(it2);
    }
    
    return true;
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->loadedPluginNames;
}

std::shared_ptr<NLMPlugin> PluginManager::getPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        return it->second;
    }
    
    return nullptr;
}

bool PluginManager::initializeAll(const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& pluginConfigs) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->plugins) {
        const auto& plugin = pair.second;
        std::string name = pair.first;
        
        // Get plugin configuration
        auto configIt = pluginConfigs.find(name);
        std::unordered_map<std::string, std::string> config;
        if (configIt != pluginConfigs.end()) {
            config = configIt->second;
        }
        
        if (!plugin->initialize(config)) {
            std::cerr << "Failed to initialize plugin: " << name << std::endl;
            return false;
        }
    }
    
    return true;
}

void PluginManager::shutdownAll() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->plugins) {
        pair.second->shutdown();
    }
    
    pImpl->plugins.clear();
    pImpl->loadedPluginNames.clear();
}

bool PluginManager::executePluginAction(const std::string& pluginName, 
                                       const std::string& action,
                                       const std::vector<std::string>& args) {
    auto plugin = getPlugin(pluginName);
    if (!plugin) {
        return false;
    }
    
    return plugin->execute(action, args);
}

std::string PluginManager::getPluginInfo(const std::string& name) const {
    auto plugin = getPlugin(name);
    if (!plugin) {
        return "Plugin not found: " + name;
    }
    
    std::stringstream ss;
    ss << "Plugin: " << plugin->getName() << std::endl;
    ss << "Version: " << plugin->getVersion() << std::endl;
    ss << "Description: " << plugin->getDescription() << std::endl;
    ss << "Author: " << plugin->getAuthor() << std::endl;
    ss << "Dependencies: ";
    bool first = true;
    for (const auto& dep : plugin->getDependencies()) {
        if (!first) ss << ", ";
        ss << dep;
        first = false;
    }
    ss << std::endl;
    ss << "Capabilities: ";
    first = true;
    for (const auto& cap : plugin->getCapabilities()) {
        if (!first) ss << ", ";
        ss << cap;
        first = false;
    }
    ss << std::endl;
    ss << "Initialized: " << (plugin->isInitialized() ? "Yes" : "No") << std::endl;
    
    return ss.str();
}

std::string PluginManager::getAllPluginsInfo() const {
    std::stringstream ss;
    ss << "=== Loaded Plugins ===" << std::endl;
    
    for (const auto& name : pImpl->loadedPluginNames) {
        ss << getPluginInfo(name) << std::endl;
        ss << "---" << std::endl;
    }
    
    return ss.str();
}

std::vector<std::string> PluginManager::scanPlugins(const std::string& directory) const {
    std::vector<std::string> plugins;
    
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
        return plugins;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && isPluginFile(entry.path())) {
            plugins.push_back(entry.path().string());
        }
    }
    
    return plugins;
}

void PluginManager::setPluginDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->pluginDirectory = directory;
}

std::string PluginManager::getPluginDirectory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->pluginDirectory;
}

// PluginManager helper functions
std::string PluginManager::extractPluginName(const std::string& pluginPath) const {
    size_t lastSlash = pluginPath.find_last_of('/');
    if (lastSlash != std::string::npos) {
        return pluginPath.substr(lastSlash + 1);
    }
    return pluginPath;
}

bool PluginManager::isPluginFile(const std::filesystem::path& path) const {
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    // Check for common plugin extensions
    static const std::set<std::string> pluginExtensions = {
        ".so", ".dll", ".dylib", ".plugin", ".nlm"
    };
    
    return pluginExtensions.find(extension) != pluginExtensions.end();
}

} // namespace nlm
