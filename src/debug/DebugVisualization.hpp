#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <memory>
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Advanced visualization formats
enum class VisualizationFormat {
    ASCII_ART,
    JSON_DATA,
    CSV_DATA,
    HTML_REPORT,
    VR_SCENE,
    VR_MESH
};

// Time-series data for neural activity
struct NeuralTimeSeries {
    std::vector<double> timestamps;
    std::vector<double> firingRates;
    std::vector<double> membranePotentials;
    std::vector<double> synapticWeights;
    
    void addDataPoint(double timestamp, double firingRate, double membranePotential, double synapticWeight) {
        timestamps.push_back(timestamp);
        firingRates.push_back(firingRate);
        membranePotentials.push_back(membranePotential);
        synapticWeights.push_back(synapticWeight);
    }
    
    double getMaxFiringRate() const {
        if (firingRates.empty()) return 0.0;
        return *std::max_element(firingRates.begin(), firingRates.end());
    }
    
    double getAverageFiringRate() const {
        if (firingRates.empty()) return 0.0;
        double sum = std::accumulate(firingRates.begin(), firingRates.end(), 0.0);
        return sum / firingRates.size();
    }
};

// Brain region visualization data
struct RegionVisualizationData {
    std::string name;
    size_t id;
    float x, y, z;  // 3D position
    float radius;
    std::string type;
    float activity;
    float connectionStrength;
    std::vector<std::pair<size_t, float>> connections;  // target region, strength
};

// Neural network visualization
struct NeuralNetworkVisualization {
    std::vector<RegionVisualizationData> regions;
    std::vector<std::pair<size_t, size_t>> connections;
    std::map<size_t, std::string> regionTypes;
    
    void addRegion(const std::string& name, size_t id, float x, float y, float z, 
                   float radius, const std::string& type, float activity) {
        RegionVisualizationData region;
        region.name = name;
        region.id = id;
        region.x = x;
        region.y = y;
        region.z = z;
        region.radius = radius;
        region.type = type;
        region.activity = activity;
        region.connectionStrength = 0.0f;
        
        regions.push_back(region);
        regionTypes[id] = type;
    }
    
    void addConnection(size_t source, size_t target, float strength) {
        connections.push_back(std::make_pair(source, target));
    }
    
    size_t getRegionCount() const { return regions.size(); }
    size_t getConnectionCount() const { return connections.size(); }
};

// Advanced visualization helper class
class VisualizationHelper {
private:
    static constexpr float COLOR_MAP_RANGE = 1.0f;
    static constexpr int DEFAULT_CHARACTER_WIDTH = 80;
    
    struct HeatmapData {
        float minValue;
        float maxValue;
        std::vector<std::vector<float>> heatmap;
    };
    
public:
    // Generate ASCII art visualization of brain activity
    static std::string generateASCIIArtBrain(const Brain& brain, 
                                            const NeuralNetworkVisualization& network,
                                            int width = DEFAULT_CHARACTER_WIDTH,
                                            int height = 20) {
        std::stringstream ss;
        ss << "=== NLM Brain Visualization ===\n";
        ss << "Step: " << brain.getConfig()->get<size_t>("simulation.step", 0) << "\n";
        ss << "Neurons: " << brain.getTotalNeuronCount() << " | ";
        ss << "Firing: " << brain.getFiringNeuronCount() << " | ";
        ss << "Spikes: " << brain.getTotalSpikeCount() << "\n\n";
        
        // Create a simple 2D representation
        std::vector<std::vector<char>> canvas(height, std::vector<char>(width, ' '));
        
        // Place regions on canvas
        for (const auto& region : network.regions) {
            int x = static_cast<int>((region.x / 20.0f) * (width - 10)) + 5;
            int y = static_cast<int>((region.y / 20.0f) * (height - 2)) + 1;
            
            if (x >= 0 && x < width && y >= 0 && y < height) {
                float activityNorm = std::max(0.0f, std::min(1.0f, region.activity / 100.0f));
                char activityChar = getActivityChar(activityNorm);
                canvas[y][x] = activityChar;
            }
        }
        
        // Draw connections
        for (const auto& conn : network.connections) {
            if (conn.first < network.regions.size() && conn.second < network.regions.size()) {
                const auto& source = network.regions[conn.first];
                const auto& target = network.regions[conn.second];
                
                int x1 = static_cast<int>((source.x / 20.0f) * (width - 10)) + 5;
                int y1 = static_cast<int>((source.y / 20.0f) * (height - 2)) + 1;
                int x2 = static_cast<int>((target.x / 20.0f) * (width - 10)) + 5;
                int y2 = static_cast<int>((target.y / 20.0f) * (height - 2)) + 1;
                
                drawLine(canvas, x1, y1, x2, y2, '~');
            }
        }
        
        // Convert canvas to string
        for (int y = height - 1; y >= 0; --y) {
            for (int x = 0; x < width; ++x) {
                ss << canvas[y][x];
            }
            ss << "\n";
        }
        
        return ss.str();
    }
    
    // Generate JSON visualization data
    static std::string generateJSONVisualization(const Brain& brain,
                                                 const NeuralNetworkVisualization& network) {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"metadata\": {\n";
        ss << "    \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
        ss << "    \"brainId\": \"" << generateBrainId() << "\",\n";
        ss << "    \"totalNeurons\": " << brain.getTotalNeuronCount() << ",\n";
        ss << "    \"totalSpikes\": " << brain.getTotalSpikeCount() << ",\n";
        ss << "    \"firingRate\": " << brain.getAverageFiringRate() << "\n";
        ss << "  },\n";
        
        ss << "  \"regions\": [\n";
        for (size_t i = 0; i < network.regions.size(); ++i) {
            const auto& region = network.regions[i];
            ss << "    {\n";
            ss << "      \"name\": \"" << region.name << "\",\n";
            ss << "      \"id\": " << region.id << ",\n";
            ss << "      \"x\": " << region.x << ",\n";
            ss << "      \"y\": " << region.y << ",\n";
            ss << "      \"z\": " << region.z << ",\n";
            ss << "      \"radius\": " << region.radius << ",\n";
            ss << "      \"type\": \"" << region.type << "\",\n";
            ss << "      \"activity\": " << region.activity << ",\n";
            ss << "      \"connectionStrength\": " << region.connectionStrength << "\n";
            ss << "    }";
            
            if (i < network.regions.size() - 1) {
                ss << ",\n";
            } else {
                ss << "\n";
            }
        }
        
        ss << "  ],\n";
        
        ss << "  \"connections\": [\n";
        for (size_t i = 0; i < network.connections.size(); ++i) {
            const auto& conn = network.connections[i];
            ss << "    {\n";
            ss << "      \"source\": " << conn.first << ",\n";
            ss << "      \"target\": " << conn.second << "\n";
            ss << "    }";
            
            if (i < network.connections.size() - 1) {
                ss << ",\n";
            } else {
                ss << "\n";
            }
        }
        
        ss << "  ]\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    // Generate CSV data for neural activity
    static std::string generateCSVData(const NeuralTimeSeries& timeSeries) {
        std::stringstream ss;
        ss << "timestamp,firing_rate,membrane_potential,synaptic_weight\n";
        
        for (size_t i = 0; i < timeSeries.timestamps.size(); ++i) {
            ss << timeSeries.timestamps[i] << ",";
            ss << timeSeries.firingRates[i] << ",";
            ss << timeSeries.membranePotentials[i] << ",";
            ss << timeSeries.synapticWeights[i];
            
            if (i < timeSeries.timestamps.size() - 1) {
                ss << "\n";
            }
        }
        
        return ss.str();
    }
    
    // Generate HTML report
    static std::string generateHTMLReport(const Brain& brain,
                                         const NeuralNetworkVisualization& network,
                                         const std::vector<std::string>& events) {
        std::stringstream ss;
        ss << "<!DOCTYPE html>\n";
        ss << "<html>\n";
        ss << "<head>\n";
        ss << "    <title>NLM Brain Simulation Report</title>\n";
        ss << "    <style>\n";
        ss << "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
        ss << "        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }\n";
        ss << "        .section { margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }\n";
        ss << "        .metric { display: inline-block; margin: 10px; padding: 10px; background-color: #e9ecef; border-radius: 3px; }\n";
        ss << "        table { border-collapse: collapse; width: 100%; }\n";
        ss << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
        ss << "        th { background-color: #f2f2f2; }\n";
        ss << "    </style>\n";
        ss << "</head>\n";
        ss << "<body>\n";
        
        ss << "<div class=\"header\">\n";
        ss << "    <h1>NLM Brain Simulation Report</h1>\n";
        ss << "    <p>Generated: " << getCurrentTimestamp() << "</p>\n";
        ss << "</div>\n";
        
        ss << "<div class=\"section\">\n";
        ss << "    <h2>Simulation Metrics</h2>\n";
        ss << "    <div class=\"metric\">\n";
        ss << "        <h3>Total Neurons</h3>\n";
        ss << "        <p>" << brain.getTotalNeuronCount() << "</p>\n";
        ss << "    </div>\n";
        ss << "    <div class=\"metric\">\n";
        ss << "        <h3>Firing Neurons</h3>\n";
        ss << "        <p>" << brain.getFiringNeuronCount() << "</p>\n";
        ss << "    </div>\n";
        ss << "    <div class=\"metric\">\n";
        ss << "        <h3>Total Spikes</h3>\n";
        ss << "        <p>" << brain.getTotalSpikeCount() << "</p>\n";
        ss << "    </div>\n";
        ss << "    <div class=\"metric\">\n";
        ss << "        <h3>Average Firing Rate</h3>\n";
        ss << "        <p>" << brain.getAverageFiringRate() << " Hz</p>\n";
        ss << "    </div>\n";
        ss << "</div>\n";
        
        ss << "<div class=\"section\">\n";
        ss << "    <h2>Brain Regions</h2>\n";
        ss << "    <table>\n";
        ss << "        <tr><th>Name</th><th>ID</th><th>Type</th><th>Activity</th></tr>\n";
        
        for (const auto& region : network.regions) {
            ss << "        <tr><td>" << region.name << "</td><td>" << region.id << "</td>";
            ss << "<td>" << region.type << "</td><td>" << region.activity << "</td></tr>\n";
        }
        
        ss << "    </table>\n";
        ss << "</div>\n";
        
        ss << "<div class=\"section\">\n";
        ss << "    <h2>Simulation Events</h2>\n";
        ss << "    <ul>\n";
        for (const auto& event : events) {
            ss << "        <li>" << event << "</li>\n";
        }
        ss << "    </ul>\n";
        ss << "</div>\n";
        
        ss << "</body>\n";
        ss << "</html>\n";
        
        return ss.str();
    }
    
    // Generate 3D visualization data (VR format)
    static std::string generateVRData(const NeuralNetworkVisualization& network,
                                     VisualizationFormat format = VisualizationFormat::VR_MESH) {
        std::stringstream ss;
        
        if (format == VisualizationFormat::VR_MESH) {
            ss << "{\n";
            ss << "  \"format\": \"vr_mesh\",\n";
            ss << "  \"version\": \"1.0\",\n";
            ss << "  \"vertices\": [\n";
            
            for (const auto& region : network.regions) {
                ss << "    {\"x\": " << region.x << ", \"y\": " << region.y << ", \"z\": " << region.z << ", \"r\": " << region.radius << "}";
                
                if (&region != &network.regions.back()) {
                    ss << ",\n";
                } else {
                    ss << "\n";
                }
            }
            
            ss << "  ],\n";
            ss << "  \"edges\": [\n";
            
            for (const auto& conn : network.connections) {
                ss << "    {\"source\": " << conn.first << ", \"target\": " << conn.second << "}";
                
                if (&conn != &network.connections.back()) {
                    ss << ",\n";
                } else {
                    ss << "\n";
                }
            }
            
            ss << "  ]\n";
            ss << "}\n";
        }
        
        return ss.str();
    }
    
    // Generate heat map visualization
    static std::string generateHeatMap(const std::vector<std::vector<float>>& data,
                                       float minValue, float maxValue,
                                       const std::string& title = "Activity Heatmap") {
        HeatmapData heatmap;
        heatmap.minValue = minValue;
        heatmap.maxValue = maxValue;
        heatmap.heatmap = data;
        
        std::stringstream ss;
        ss << "=== " << title << " ===\n\n";
        
        for (const auto& row : heatmap.heatmap) {
            for (float value : row) {
                ss << getHeatmapChar(value, heatmap.minValue, heatmap.maxValue);
            }
            ss << "\n";
        }
        
        ss << "\nLegend: ";
        ss << getLegendChar('0', heatmap.minValue) << "=" << heatmap.minValue << " ";
        ss << getLegendChar('#', (heatmap.minValue + heatmap.maxValue) / 2) << "=Medium ";
        ss << getLegendChar('@', heatmap.maxValue) << "=" << heatmap.maxValue << "\n";
        
        return ss.str();
    }
    
    // Export visualization to file
    static bool exportVisualization(const std::string& data, 
                                    const std::string& filename,
                                    VisualizationFormat format) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filename);
            return false;
        }
        
        switch (format) {
            case VisualizationFormat::ASCII_ART:
            case VisualizationFormat::JSON_DATA:
            case VisualizationFormat::CSV_DATA:
            case VisualizationFormat::HTML_REPORT:
            case VisualizationFormat::VR_SCENE:
            case VisualizationFormat::VR_MESH:
                file << data;
                break;
        }
        
        file.close();
        return true;
    }
    
    // Export visualization to multiple formats
    static bool exportVisualizationToMultipleFormats(const std::string& baseFilename,
                                                     const std::string& data,
                                                     const std::vector<VisualizationFormat>& formats) {
        bool success = true;
        
        for (auto format : formats) {
            std::string filename = baseFilename;
            
            switch (format) {
                case VisualizationFormat::ASCII_ART:
                    filename += ".txt";
                    break;
                case VisualizationFormat::JSON_DATA:
                    filename += ".json";
                    break;
                case VisualizationFormat::CSV_DATA:
                    filename += ".csv";
                    break;
                case VisualizationFormat::HTML_REPORT:
                    filename += ".html";
                    break;
                case VisualizationFormat::VR_SCENE:
                    filename += ".vrscene";
                    break;
                case VisualizationFormat::VR_MESH:
                    filename += ".obj";
                    break;
            }
            
            if (!exportVisualization(data, filename, format)) {
                success = false;
            }
        }
        
        return success;
    }
    
private:
    static char getActivityChar(float activity) {
        activity = std::max(0.0f, std::min(1.0f, activity));
        
        if (activity < 0.2f) return '.';
        if (activity < 0.4f) return ',';
        if (activity < 0.6f) return '-';
        if (activity < 0.8f) return '+';
        return '@';
    }
    
    static void drawLine(std::vector<std::vector<char>>& canvas, 
                        int x1, int y1, int x2, int y2, char ch) {
        int dx = x2 - x1;
        int dy = y2 - y1;
        int steps = std::max(std::abs(dx), std::abs(dy));
        
        if (steps == 0) return;
        
        float xIncrement = static_cast<float>(dx) / steps;
        float yIncrement = static_cast<float>(dy) / steps;
        
        float x = x1;
        float y = y1;
        
        for (int i = 0; i <= steps; ++i) {
            int xi = static_cast<int>(std::round(x));
            int yi = static_cast<int>(std::round(y));
            
            if (xi >= 0 && xi < static_cast<int>(canvas[0].size()) &&
                yi >= 0 && yi < static_cast<int>(canvas.size())) {
                canvas[yi][xi] = ch;
            }
            
            x += xIncrement;
            y += yIncrement;
        }
    }
    
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    static std::string generateBrainId() {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        
        std::stringstream ss;
        ss << "brain_" << timestamp;
        return ss.str();
    }
    
    static char getHeatmapChar(float value, float min, float max) {
        if (max == min) return '.';
        
        float normalized = (value - min) / (max - min);
        normalized = std::max(0.0f, std::min(1.0f, normalized));
        
        if (normalized < 0.3f) return '.';
        if (normalized < 0.6f) return '-';
        return '@';
    }
    
    static char getLegendChar(char ch, float value) {
        return ch;
    }
};  // class VisualizationHelper

// Advanced debugging helper class
class DebugHelper {
public:
    // Memory leak detector
    class MemoryLeakDetector {
    private:
        std::unordered_map<void*, std::string> allocations_;
        std::chrono::steady_clock::time_point startTime_;
        
    public:
        MemoryLeakDetector() {
            startTime_ = std::chrono::steady_clock::now();
        }
        
        void recordAllocation(void* ptr, const std::string& description) {
            allocations_[ptr] = description;
        }
        
        void recordDeallocation(void* ptr) {
            allocations_.erase(ptr);
        }
        
        std::string generateReport() {
            std::stringstream ss;
            ss << "=== Memory Leak Detection Report ===\n";
            ss << "Detection started: " << getTimestamp() << "\n";
            ss << "Current allocations: " << allocations_.size() << "\n\n";
            
            if (allocations_.empty()) {
                ss << "✓ No memory leaks detected\n";
            } else {
                ss << "✗ Memory leaks detected:\n";
                for (const auto& pair : allocations_) {
                    ss << "  Pointer: " << pair.first << " | Description: " << pair.second << "\n";
                }
            }
            
            return ss.str();
        }
        
    private:
        static std::string getTimestamp() {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - startTime_).count();
            return std::to_string(duration) + " ms";
        }
    };
    
    // Performance profiler
    class PerformanceProfiler {
    private:
        struct ProfileData {
            std::string functionName;
            std::chrono::high_resolution_clock::time_point startTime;
            std::chrono::high_resolution_clock::time_point endTime;
            bool isActive;
        };
        
        std::vector<ProfileData> profiles_;
        bool profilingEnabled_;
        
    public:
        PerformanceProfiler() : profilingEnabled_(false) {}
        
        void startProfiling(const std::string& functionName) {
            if (!profilingEnabled_) return;
            
            ProfileData profile;
            profile.functionName = functionName;
            profile.startTime = std::chrono::high_resolution_clock::now();
            profile.isActive = true;
            
            profiles_.push_back(profile);
        }
        
        void stopProfiling(const std::string& functionName) {
            if (!profilingEnabled_) return;
            
            for (auto& profile : profiles_) {
                if (profile.functionName == functionName && profile.isActive) {
                    profile.endTime = std::chrono::high_resolution_clock::now();
                    profile.isActive = false;
                    break;
                }
            }
        }
        
        std::string generateReport() {
            std::stringstream ss;
            ss << "=== Performance Profiling Report ===\n";
            
            if (profiles_.empty()) {
                ss << "No profiling data available.\n";
                return ss.str();
            }
            
            ss << "Total profiled functions: " << profiles_.size() << "\n\n";
            
            for (const auto& profile : profiles_) {
                if (!profile.isActive) {
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                        profile.endTime - profile.startTime).count();
                    ss << profile.functionName << ": " << duration << " μs\n";
                }
            }
            
            return ss.str();
        }
        
        void enableProfiling(bool enable) {
            profilingEnabled_ = enable;
        }
    };
    
    // Thread profiler
    class ThreadProfiler {
    private:
        struct ThreadData {
            std::thread::id threadId;
            std::string threadName;
            std::chrono::high_resolution_clock::time_point startTime;
            std::chrono::high_resolution_clock::time_point endTime;
            bool isActive;
            size_t operations;
        };
        
        std::vector<ThreadData> threads_;
        bool threadProfilingEnabled_;
        
    public:
        ThreadProfiler() : threadProfilingEnabled_(false) {}
        
        void startThread(const std::string& threadName = "") {
            if (!threadProfilingEnabled_) return;
            
            ThreadData thread;
            thread.threadId = std::this_thread::get_id();
            thread.threadName = threadName.empty() ? ("Thread_" + std::to_string(threadIdCounter++)) : threadName;
            thread.startTime = std::chrono::high_resolution_clock::now();
            thread.isActive = true;
            thread.operations = 0;
            
            threads_.push_back(thread);
        }
        
        void stopThread() {
            if (!threadProfilingEnabled_) return;
            
            for (auto& thread : threads_) {
                if (thread.isActive && thread.threadId == std::this_thread::get_id()) {
                    thread.endTime = std::chrono::high_resolution_clock::now();
                    thread.isActive = false;
                    break;
                }
            }
        }
        
        void incrementOperationCount() {
            if (!threadProfilingEnabled_) return;
            
            for (auto& thread : threads_) {
                if (thread.isActive && thread.threadId == std::this_thread::get_id()) {
                    thread.operations++;
                    break;
                }
            }
        }
        
        std::string generateReport() {
            std::stringstream ss;
            ss << "=== Thread Profiling Report ===\n";
            
            if (threads_.empty()) {
                ss << "No thread profiling data available.\n";
                return ss.str();
            }
            
            ss << "Total profiled threads: " << threads_.size() << "\n\n";
            
            for (const auto& thread : threads_) {
                if (!thread.isActive) {
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        thread.endTime - thread.startTime).count();
                    ss << "Thread: " << thread.threadName << "\n";
                    ss << "  ID: " << thread.threadId << "\n";
                    ss << "  Duration: " << duration << " ms\n";
                    ss << "  Operations: " << thread.operations << "\n\n";
                }
            }
            
            return ss.str();
        }
        
        void enableThreadProfiling(bool enable) {
            threadProfilingEnabled_ = enable;
        }
        
    private:
        static size_t threadIdCounter;
    };
    
    // System resource monitor
    class SystemResourceMonitor {
    public:
        struct ResourceUsage {
            double cpuUsage;
            double memoryUsage;
            size_t activeThreads;
            size_t totalMemoryAllocated;
            std::chrono::high_resolution_clock::time_point timestamp;
        };
        
        std::vector<ResourceUsage> resourceHistory_;
        bool monitoringEnabled_;
        
    public:
        SystemResourceMonitor() : monitoringEnabled_(false) {}
        
        void recordResourceUsage() {
            if (!monitoringEnabled_) return;
            
            ResourceUsage usage;
            usage.timestamp = std::chrono::high_resolution_clock::now();
            usage.cpuUsage = getCPUUsage();
            usage.memoryUsage = getMemoryUsage();
            usage.activeThreads = getActiveThreadCount();
            usage.totalMemoryAllocated = getTotalMemoryAllocated();
            
            resourceHistory_.push_back(usage);
            
            // Keep only last 1000 samples
            if (resourceHistory_.size() > 1000) {
                resourceHistory_.erase(resourceHistory_.begin());
            }
        }
        
        std::string generateReport() {
            std::stringstream ss;
            ss << "=== System Resource Monitor Report ===\n";
            
            if (resourceHistory_.empty()) {
                ss << "No resource monitoring data available.\n";
                return ss.str();
            }
            
            size_t recentSamples = std::min<size_t>(100, resourceHistory_.size());
            
            ss << "Monitoring period: " << resourceHistory_.size() << " samples\n";
            ss << "Sample interval: 100ms\n";
            ss << "Recent average CPU usage: " << calculateAverageCPU() << "%\n";
            ss << "Recent average memory usage: " << calculateAverageMemory() << "%\n";
            ss << "Max CPU usage: " << getMaxCPU() << "%\n";
            ss << "Max memory usage: " << getMaxMemory() << "%\n\n";
            
            // Generate timeline visualization
            ss << "=== Resource Usage Timeline ===\n";
            ss << generateTimelineVisualization();
            
            return ss.str();
        }
        
        void enableMonitoring(bool enable) {
            monitoringEnabled_ = enable;
        }
        
    private:
        double getCPUUsage() {
            // Implement CPU usage calculation
            // This is a simplified version - in production, use platform-specific APIs
            return 0.0;  // Placeholder
        }
        
        double getMemoryUsage() {
            // Implement memory usage calculation
            // This is a simplified version - in production, use platform-specific APIs
            return 0.0;  // Placeholder
        }
        
        size_t getActiveThreadCount() {
            // Return number of active threads
            return std::thread::hardware_concurrency();  // Simplified
        }
        
        size_t getTotalMemoryAllocated() {
            // Return total memory allocated
            return 0;  // Placeholder
        }
        
        double calculateAverageCPU() {
            if (resourceHistory_.empty()) return 0.0;
            double sum = 0.0;
            size_t samples = std::min<size_t>(100, resourceHistory_.size());
            
            for (size_t i = resourceHistory_.size() - samples; i < resourceHistory_.size(); ++i) {
                sum += resourceHistory_[i].cpuUsage;
            }
            
            return sum / samples;
        }
        
        double calculateAverageMemory() {
            if (resourceHistory_.empty()) return 0.0;
            double sum = 0.0;
            size_t samples = std::min<size_t>(100, resourceHistory_.size());
            
            for (size_t i = resourceHistory_.size() - samples; i < resourceHistory_.size(); ++i) {
                sum += resourceHistory_[i].memoryUsage;
            }
            
            return sum / samples;
        }
        
        double getMaxCPU() {
            if (resourceHistory_.empty()) return 0.0;
            double max = 0.0;
            for (const auto& usage : resourceHistory_) {
                if (usage.cpuUsage > max) max = usage.cpuUsage;
            }
            return max;
        }
        
        double getMaxMemory() {
            if (resourceHistory_.empty()) return 0.0;
            double max = 0.0;
            for (const auto& usage : resourceHistory_) {
                if (usage.memoryUsage > max) max = usage.memoryUsage;
            }
            return max;
        }
        
        std::string generateTimelineVisualization() {
            std::stringstream ss;
            size_t samples = std::min<size_t>(50, resourceHistory_.size());
            
            for (size_t i = resourceHistory_.size() - samples; i < resourceHistory_.size(); ++i) {
                const auto& usage = resourceHistory_[i];
                double cpuPercent = usage.cpuUsage;
                double memoryPercent = usage.memoryUsage;
                
                ss << "CPU: ";
                for (size_t j = 0; j < 50; ++j) {
                    if (j < static_cast<size_t>(cpuPercent * 0.5)) {
                        ss << "#";
                    } else {
                        ss << "-";
                    }
                }
                ss << " (" << cpuPercent << "%)";
                
                ss << " | Memory: ";
                for (size_t j = 0; j < 50; ++j) {
                    if (j < static_cast<size_t>(memoryPercent * 0.5)) {
                        ss << "@";
                    } else {
                        ss << ".";
                    }
                }
                ss << " (" << memoryPercent << "%)";
                
                ss << "\n";
            }
            
            return ss.str();
        }
    };
    
    // Error tracer
    class ErrorTracer {
    private:
        std::vector<std::pair<std::chrono::high_resolution_clock::time_point, std::string>> errorLog_;
        std::vector<std::pair<std::chrono::high_resolution_clock::time_point, std::string>> stackTraceLog_;
        
    public:
        void logError(const std::string& error) {
            errorLog_.push_back(std::make_pair(
                std::chrono::high_resolution_clock::now(),
                error
            ));
        }
        
        void logStackTrace(const std::string& trace) {
            stackTraceLog_.push_back(std::make_pair(
                std::chrono::high_resolution_clock::now(),
                trace
            ));
        }
        
        std::string generateReport() {
            std::stringstream ss;
            ss << "=== Error Tracer Report ===\n";
            ss << "Total errors: " << errorLog_.size() << "\n";
            ss << "Total stack traces: " << stackTraceLog_.size() << "\n\n";
            
            if (!errorLog_.empty()) {
                ss << "=== Error Log ===\n";
                for (const auto& entry : errorLog_) {
                    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                        entry.first.time_since_epoch()).count();
                    ss << "[" << duration << "s] " << entry.second << "\n";
                }
            }
            
            if (!stackTraceLog_.empty()) {
                ss << "\n=== Stack Trace Log ===\n";
                for (const auto& entry : stackTraceLog_) {
                    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                        entry.first.time_since_epoch()).count();
                    ss << "[" << duration << "s] " << entry.second << "\n";
                }
            }
            
            return ss.str();
        }
        
        void clear() {
            errorLog_.clear();
            stackTraceLog_.clear();
        }
    };
    
    // Integration tester
    class IntegrationTester {
    public:
        struct TestResult {
            std::string testName;
            bool passed;
            std::string message;
            double executionTime;
        };
        
        std::vector<TestResult> testResults_;
        
    public:
        void addTestResult(const std::string& testName, bool passed, 
                          const std::string& message = "", 
                          double executionTime = 0.0) {
            TestResult result;
            result.testName = testName;
            result.passed = passed;
            result.message = message;
            result.executionTime = executionTime;
            
            testResults_.push_back(result);
        }
        
        std::string generateReport() {
            std::stringstream ss;
            ss << "=== Integration Test Report ===\n";
            ss << "Total tests: " << testResults_.size() << "\n";
            
            size_t passed = 0;
            size_t failed = 0;
            
            for (const auto& result : testResults_) {
                if (result.passed) {
                    passed++;
                    ss << "✓ " << result.testName << "\n";
                } else {
                    failed++;
                    ss << "✗ " << result.testName << " - " << result.message << "\n";
                }
            }
            
            ss << "\nTest Summary:\n";
            ss << "  Passed: " << passed << "\n";
            ss << "  Failed: " << failed << "\n";
            ss << "  Success Rate: " << (passed / (passed + failed) * 100.0) << "%\n";
            
            return ss.str();
        }
        
        void clear() {
            testResults_.clear();
        }
    };
};  // class DebugHelper

} // namespace nlm
