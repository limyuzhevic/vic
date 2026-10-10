#include "VisualizationInfrastructure.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <unordered_map>

namespace nlm {

// DebugVisualizer implementation
struct DebugVisualizer::Impl {
    std::mutex mutex;
};

DebugVisualizer::DebugVisualizer() : pImpl(std::make_unique<Impl>()) {}

DebugVisualizer::~DebugVisualizer() = default;

std::string DebugVisualizer::visualizeDebugEntries(const DebugVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Debug Entries Visualization ===\n"
        << "Session: " << data.sessionId << "\n"
        << "Time Range: " << std::chrono::system_clock::to_time_t(data.startTime)
        << " - " << std::chrono::system_clock::to_time_t(data.endTime) << "\n"
        << "Total Entries: " << data.debugEntries.size() << "\n\n";
    
    // Group entries by level
    std::map<std::string, size_t> levelCounts;
    for (const auto& entry : data.debugEntries) {
        std::string levelStr;
        switch (entry.level) {
            case DebugLevel::TRACE: levelStr = "TRACE"; break;
            case DebugLevel::DEBUG: levelStr = "DEBUG"; break;
            case DebugLevel::INFO: levelStr = "INFO"; break;
            case DebugLevel::WARN: levelStr = "WARN"; break;
            case DebugLevel::ERROR: levelStr = "ERROR"; break;
            case DebugLevel::FATAL: levelStr = "FATAL"; break;
            default: levelStr = "UNKNOWN"; break;
        }
        levelCounts[levelStr]++;
    }
    
    oss << "Entries by Level:\n";
    for (const auto& pair : levelCounts) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    return oss.str();
}

std::string DebugVisualizer::visualizeDebugTimeline(const DebugVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Debug Timeline ===\n"
        << "Session: " << data.sessionId << "\n"
        << "File Filter: " << (data.fileFilter.empty() ? "None" : data.fileFilter) << "\n"
        << "Level Filter: " << (data.levelFilter.empty() ? "None" : data.levelFilter) << "\n"
        << "\n";
    
    // Create a simple timeline representation
    oss << "Timeline (first 50 entries):\n";
    size_t count = std::min(static_cast<size_t>(50), data.debugEntries.size());
    for (size_t i = 0; i < count; ++i) {
        const auto& entry = data.debugEntries[i];
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
            entry.timestamp.time_since_epoch()).count() % 10000;
        
        oss << "[" << time << "ms] ";
        
        // Add level indicator
        switch (entry.level) {
            case DebugLevel::DEBUG: oss << "D "; break;
            case DebugLevel::INFO: oss << "I "; break;
            case DebugLevel::WARN: oss << "W "; break;
            case DebugLevel::ERROR: oss << "E "; break;
            case DebugLevel::FATAL: oss << "F "; break;
            default: oss << "? "; break;
        }
        
        oss << entry.message << "\n";
    }
    
    return oss.str();
}

std::string DebugVisualizer::visualizeDebugCallStack(const DebugVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Call Stack Visualization ===\n"
        << "Session: " << data.sessionId << "\n"
        << "Total Call Stacks: " << data.callStacks.size() << "\n\n";
    
    for (const auto& stack : data.callStacks) {
        oss << "Call Stack ID: " << stack.id << "\n"
            << "Timestamp: " << std::chrono::system_clock::to_time_t(stack.timestamp) << "\n"
            << "Thread: " << stack.threadId << "\n"
            << "Description: " << stack.description << "\n";
        
        oss << "Frames:";
        for (const auto& frame : stack.frames) {
            oss << "\n  " << frame.function << " (" << frame.file << ":" << frame.line << ")";
        }
        oss << "\n\n";
    }
    
    return oss.str();
}

std::string DebugVisualizer::visualizeDebugVariables(const DebugVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Variable Inspection ===\n"
        << "Session: " << data.sessionId << "\n"
        << "Variables: " << data.variableValues.size() << "\n\n";
    
    oss << "Variable Values:\n";
    for (const auto& pair : data.variableValues) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    oss << "\nPerformance Metrics:\n";
    for (const auto& pair : data.performanceMetrics) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    return oss.str();
}

std::string DebugVisualizer::generateDebugReport(const DebugVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Comprehensive Debug Report ===\n"
        << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "\n"
        << "\n";
    
    // Add all visualizations
    oss << visualizeDebugEntries(data) << "\n";
    oss << visualizeDebugTimeline(data) << "\n";
    oss << visualizeDebugCallStack(data) << "\n";
    oss << visualizeDebugVariables(data);
    
    return oss.str();
}

bool DebugVisualizer::exportVisualization(const VisualizationData& vizData, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (vizData.format == "text" || vizData.format.empty()) {
        file << vizData.content;
    } else if (vizData.format == "csv") {
        // Convert to CSV format
        file << "Type,Title,Timestamp\n";
        file << vizData.type << "," << vizData.title << ","
             << std::chrono::system_clock::to_time_t(vizData.timestamp) << "\n";
        
        file << "Metric,Value\n";
        for (const auto& pair : vizData.metrics) {
            file << pair.first << "," << pair.second << "\n";
        }
    } else if (vizData.format == "json") {
        // Convert to JSON format (simplified)
        file << "{\"type\": \"" << vizData.type << "\", \"title\": \"" << vizData.title
             << "\", \"timestamp\": " << std::chrono::system_clock::to_time_t(vizData.timestamp)
             << "}";
    }
    
    file.flush();
    return true;
}

std::vector<std::string> DebugVisualizer::getSupportedFormats() const {
    return {"text", "csv", "json", "html", "svg"};
}

DebugVisualizer& DebugVisualizer::getGlobal() {
    static DebugVisualizer globalVisualizer;
    return globalVisualizer;
}

void DebugVisualizer::setGlobal(std::shared_ptr<DebugVisualizer> visualizer) {
    // This is a placeholder for advanced global visualizer management
}

// PerformanceVisualizer implementation
struct PerformanceVisualizer::Impl {
    std::mutex mutex;
};

PerformanceVisualizer::PerformanceVisualizer() : pImpl(std::make_unique<Impl>()) {}

PerformanceVisualizer::~PerformanceVisualizer() = default;

std::string PerformanceVisualizer::visualizePerformanceData(const PerformanceVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Performance Data Visualization ===\n"
        << "Session: " << data.sessionId << "\n"
        << "Time Range: " << std::chrono::system_clock::to_time_t(data.startTime)
        << " - " << std::chrono::system_clock::to_time_t(data.endTime) << "\n"
        << "Total Records: " << data.performanceData.size() << "\n"
        << "Operation Filter: " << (data.operationFilter.empty() ? "None" : data.operationFilter) << "\n"
        << "\n";
    
    // Calculate statistics
    std::map<std::string, std::vector<double>> metricValues;
    for (const auto& perf : data.performanceData) {
        for (const auto& pair : perf.metrics) {
            metricValues[pair.first].push_back(pair.second);
        }
    }
    
    oss << "Performance Metrics Statistics:\n";
    for (const auto& pair : metricValues) {
        const auto& values = pair.second;
        if (!values.empty()) {
            double sum = std::accumulate(values.begin(), values.end(), 0.0);
            double avg = sum / values.size();
            double min = *std::min_element(values.begin(), values.end());
            double max = *std::max_element(values.begin(), values.end());
            
            oss << "  " << pair.first << ":\n"
                << "    Average: " << avg << "\n"
                << "    Min: " << min << "\n"
                << "    Max: " << max << "\n"
                << "    Count: " << values.size() << "\n";
        }
    }
    
    return oss.str();
}

std::string PerformanceVisualizer::visualizePerformanceTimeline(const PerformanceVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Performance Timeline ===\n"
        << "Session: " << data.sessionId << "\n"
        << "Time Window: " << data.timeWindowSeconds << " seconds\n\n";
    
    // Create timeline data
    oss << "Timeline Data:\n";
    size_t count = std::min(static_cast<size_t>(100), data.performanceData.size());
    for (size_t i = 0; i < count; ++i) {
        const auto& perf = data.performanceData[i];
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
            perf.timestamp.time_since_epoch()).count() % 10000;
        
        oss << "[" << time << "ms] " << perf.operationName << ": " << perf.duration
            << "s (CPU: " << perf.cpuUsage << "%, Memory: " << perf.memoryUsed << " bytes)\n";
    }
    
    return oss.str();
}

std::string PerformanceVisualizer::visualizePerformanceComparison(const PerformanceVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Performance Comparison ===\n"
        << "Session: " << data.sessionId << "\n"
        << "Category Filter: " << (data.categoryFilter.empty() ? "None" : data.categoryFilter) << "\n\n";
    
    // Group by category
    std::map<std::string, std::vector<PerformanceInfo>> categorizedData;
    for (const auto& perf : data.performanceData) {
        // Extract category from metrics or use placeholder
        std::string category = "unknown";
        auto it = perf.metrics.find("category");
        if (it != perf.metrics.end()) {
            category = it->second;
        }
        
        if (data.categoryFilter.empty() || category == data.categoryFilter) {
            categorizedData[category].push_back(perf);
        }
    }
    
    oss << "Performance by Category:\n";
    for (const auto& pair : categorizedData) {
        const auto& category = pair.first;
        const auto& records = pair.second;
        
        double totalTime = std::accumulate(records.begin(), records.end(), 0.0,
                                         [](double sum, const PerformanceInfo& perf) {
                                             return sum + perf.duration;
                                         });
        
        oss << "  " << category << ": " << records.size() << " records, "
            << "total time: " << totalTime << "s, "
            << "avg: " << (records.empty() ? 0.0 : totalTime / records.size()) << "s per record\n";
    }
    
    return oss.str();
}

std::string PerformanceVisualizer::visualizePerformanceHeatmap(const PerformanceVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Performance Heatmap ===\n"
        << "Session: " << data.sessionId << "\n"
        << "Size: 50x50 matrix (time x operations)\n\n";
    
    // Create a simple heatmap representation
    oss << "Heatmap Legend:\n"
        << "  █ High Performance (green)\n"
        << "  █ Medium Performance (yellow)\n"
        << "  █ Low Performance (red)\n\n";
    
    oss << "Heatmap Visualization (simplified):\n";
    
    // Create a 10x10 grid for demonstration
    std::vector<std::vector<double>> grid(10, std::vector<double>(10, 0.0));
    
    for (size_t i = 0; i < std::min(static_cast<size_t>(10), data.performanceData.size()); ++i) {
        const auto& perf = data.performanceData[i];
        int row = i / 10;
        int col = i % 10;
        
        if (row < 10 && col < 10) {
            // Normalize performance (lower time is better)
            double normalizedPerf = std::max(0.0, 1.0 - (perf.duration / 1.0));
            grid[row][col] = normalizedPerf;
        }
    }
    
    // Display grid
    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 10; ++col) {
            double value = grid[row][col];
            if (value > 0.7) oss << "█"; // Green
            else if (value > 0.4) oss << "▌"; // Yellow
            else oss << "░"; // Red
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string PerformanceVisualizer::generatePerformanceReport(const PerformanceVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Performance Analysis Report ===\n"
        << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "\n"
        << "\n";
    
    // Add all visualizations
    oss << visualizePerformanceData(data) << "\n";
    oss << visualizePerformanceTimeline(data) << "\n";
    oss << visualizePerformanceComparison(data) << "\n";
    oss << visualizePerformanceHeatmap(data);
    
    return oss.str();
}

bool PerformanceVisualizer::exportVisualization(const VisualizationData& vizData, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (vizData.format == "text" || vizData.format.empty()) {
        file << vizData.content;
    } else if (vizData.format == "csv") {
        file << "type,title,timestamp\n";
        file << vizData.type << "," << vizData.title << ","
             << std::chrono::system_clock::to_time_t(vizData.timestamp) << "\n";
        
        file << "metric,value\n";
        for (const auto& pair : vizData.metrics) {
            file << pair.first << "," << pair.second << "\n";
        }
    }
    
    file.flush();
    return true;
}

std::vector<std::string> PerformanceVisualizer::getSupportedFormats() const {
    return {"text", "csv", "json", "html", "svg"};
}

PerformanceVisualizer& PerformanceVisualizer::getGlobal() {
    static PerformanceVisualizer globalVisualizer;
    return globalVisualizer;
}

void PerformanceVisualizer::setGlobal(std::shared_ptr<PerformanceVisualizer> visualizer) {
    // This is a placeholder for advanced global visualizer management
}

// MemoryVisualizer implementation
struct MemoryVisualizer::Impl {
    std::mutex mutex;
};

MemoryVisualizer::MemoryVisualizer() : pImpl(std::make_unique<Impl>()) {}

MemoryVisualizer::~MemoryVisualizer() = default;

std::string MemoryVisualizer::visualizeMemoryUsage(const MemoryVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Memory Usage Visualization ===\n"
        << "Session: " << data.sessionId << "\n"
        << "Time Range: " << std::chrono::system_clock::to_time_t(data.startTime)
        << " - " << std::chrono::system_clock::to_time_t(data.endTime) << "\n"
        << "Total Memory: " << data.totalMemory << " bytes\n"
        << "Used Memory: " << data.usedMemory << " bytes\n"
        << "Usage: " << (data.totalMemory > 0 ? (data.usedMemory * 100.0 / data.totalMemory) : 0.0) << "%\n"
        << "Allocations: " << data.allocations.size() << "\n"
        << "Memory Blocks: " << data.memoryBlocks.size() << "\n"
        << "Allocation Type Filter: " << (data.allocationTypeFilter.empty() ? "None" : data.allocationTypeFilter) << "\n"
        << "Show Leaks: " << (data.showLeaks ? "Yes" : "No") << "\n\n";
    
    if (data.showLeaks) {
        oss << "Memory Leaks:\n";
        for (const auto& pair : data.allocations) {
            oss << "  Address: 0x" << std::hex << pair.first << " (Type: " << pair.second << ")\n";
        }
    }
    
    return oss.str();
}

std::string MemoryVisualizer::visualizeMemoryTimeline(const MemoryVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Memory Timeline ===\n"
        << "Session: " << data.sessionId << "\n\n";
    
    // Create timeline data
    oss << "Memory Timeline (first 50 allocations):\n";
    size_t count = std::min(static_cast<size_t>(50), data.allocations.size());
    size_t index = 0;
    for (const auto& pair : data.allocations) {
        if (index >= count) break;
        
        uintptr_t address = pair.first;
        size_t size = data.allocationSizes.at(address);
        std::string type = pair.second;
        
        auto time = static_cast<uintptr_t>(address % 10000);
        
        oss << "[" << time << "] 0x" << std::hex << address << " " << size << " bytes " << type << "\n";
        
        index++;
    }
    
    return oss.str();
}

std::string MemoryVisualizer::visualizeMemoryAllocationHeatmap(const MemoryVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Memory Allocation Heatmap ===\n"
        << "Session: " << data.sessionId << "\n"
        << "Size: " << data.memoryBlocks.size() << " allocations\n\n";
    
    oss << "Heatmap Legend:\n"
        << "  █ Large Allocations (red)\n"
        << "  █ Medium Allocations (yellow)\n"
        << "  █ Small Allocations (green)\n\n";
    
    oss << "Allocation Heatmap Visualization:\n";
    
    // Create a simple heatmap representation
    if (!data.memoryBlocks.empty()) {
        // Find min and max sizes for normalization
        size_t minSize = SIZE_MAX;
        size_t maxSize = 0;
        for (const auto& block : data.memoryBlocks) {
            if (block.second < minSize) minSize = block.second;
            if (block.second > maxSize) maxSize = block.second;
        }
        
        // Create a 10x10 grid for demonstration
        std::vector<std::vector<size_t>> grid(10, std::vector<size_t>(10, 0));
        
        size_t count = std::min(static_cast<size_t>(10), data.memoryBlocks.size());
        for (size_t i = 0; i < count; ++i) {
            const auto& block = data.memoryBlocks[i];
            int row = i / 10;
            int col = i % 10;
            
            if (row < 10 && col < 10) {
                grid[row][col] = block.second;
            }
        }
        
        // Display grid
        for (int row = 0; row < 10; ++row) {
            for (int col = 0; col < 10; ++col) {
                size_t size = grid[row][col];
                if (maxSize > minSize) {
                    double normalized = (size - minSize) / static_cast<double>(maxSize - minSize);
                    if (normalized > 0.7) oss << "█"; // Red
                    else if (normalized > 0.4) oss << "▌"; // Yellow
                    else if (normalized > 0.0) oss << "░"; // Green
                    else oss << " "; // White
                } else {
                    oss << "░"; // Default green
                }
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

std::string MemoryVisualizer::visualizeMemoryLeaks(const MemoryVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Memory Leak Detection ===\n"
        << "Session: " << data.sessionId << "\n"
        << "Memory Leaks Found: " << data.allocations.size() << "\n\n";
    
    if (data.allocations.empty()) {
        oss << "No memory leaks detected.\n";
    } else {
        oss << "Leaked Allocations:\n";
        for (const auto& pair : data.allocations) {
            uintptr_t address = pair.first;
            size_t size = data.allocationSizes.at(address);
            std::string type = pair.second;
            
            oss << "  Address: 0x" << std::hex << address << "\n"
                << "    Size: " << size << " bytes\n"
                << "    Type: " << type << "\n"
                << "    Stack: [Not available in this visualization]\n\n";
        }
    }
    
    oss << "Memory Leak Analysis:\n"
        << "  Total Leaked Memory: " << (data.allocations.size() > 0 ? "unknown" : "0") << " bytes\n"
        << "  Allocation Types:\n";
    
    // Count allocation types
    std::map<std::string, size_t> typeCounts;
    for (const auto& pair : data.allocations) {
        typeCounts[pair.second]++;
    }
    
    for (const auto& pair : typeCounts) {
        oss << "    " << pair.first << ": " << pair.second << "\n";
    }
    
    return oss.str();
}

std::string MemoryVisualizer::generateMemoryReport(const MemoryVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Memory Analysis Report ===\n"
        << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "\n"
        << "\n";
    
    // Add all visualizations
    oss << visualizeMemoryUsage(data) << "\n";
    oss << visualizeMemoryTimeline(data) << "\n";
    oss << visualizeMemoryAllocationHeatmap(data) << "\n";
    oss << visualizeMemoryLeaks(data);
    
    return oss.str();
}

bool MemoryVisualizer::exportVisualization(const VisualizationData& vizData, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (vizData.format == "text" || vizData.format.empty()) {
        file << vizData.content;
    } else if (vizData.format == "csv") {
        file << "type,title,timestamp\n";
        file << vizData.type << "," << vizData.title << ","
             << std::chrono::system_clock::to_time_t(vizData.timestamp) << "\n";
        
        file << "metric,value\n";
        for (const auto& pair : vizData.metrics) {
            file << pair.first << "," << pair.second << "\n";
        }
    }
    
    file.flush();
    return true;
}

std::vector<std::string> MemoryVisualizer::getSupportedFormats() const {
    return {"text", "csv", "json", "html", "svg"};
}

MemoryVisualizer& MemoryVisualizer::getGlobal() {
    static MemoryVisualizer globalVisualizer;
    return globalVisualizer;
}

void MemoryVisualizer::setGlobal(std::shared_ptr<MemoryVisualizer> visualizer) {
    // This is a placeholder for advanced global visualizer management
}

// ThreadVisualizer implementation
struct ThreadVisualizer::Impl {
    std::mutex mutex;
};

ThreadVisualizer::ThreadVisualizer() : pImpl(std::make_unique<Impl>()) {}

ThreadVisualizer::~ThreadVisualizer() = default;

std::string ThreadVisualizer::visualizeThreadTimeline(const ThreadVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Thread Timeline ===\n"
        << "Session: " << data.sessionId << "\n"
        << "Time Range: " << std::chrono::system_clock::to_time_t(data.startTime)
        << " - " << std::chrono::system_clock::to_time_t(data.endTime) << "\n"
        << "Active Threads: " << data.activeThreads.size() << "\n"
        << "Thread Filter: " << (data.threadFilter.empty() ? "None" : data.threadFilter) << "\n"
        << "Show Stack Traces: " << (data.showStackTraces ? "Yes" : "No") << "\n\n";
    
    oss << "Thread Timeline:\n";
    
    // Create timeline data
    for (const auto& thread : data.activeThreads) {
        oss << "  " << thread;
        if (data.showStackTraces) {
            auto it = data.threadActivities.find(thread);
            if (it != data.threadActivities.end()) {
                oss << " (stack: " << it->second.size() << " frames)";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string ThreadVisualizer::visualizeThreadActivityChart(const ThreadVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Thread Activity Chart ===\n"
        << "Session: " << data.sessionId << "\n\n";
    
    // Create activity chart representation
    oss << "Activity Chart Legend:\n"
        << "  █ Active Threads (green)\n"
        << "  █ CPU Usage (blue)\n"
        << "  █ Thread Creation (yellow)\n\n";
    
    oss << "Thread Activity Visualization:\n";
    
    // Create a simple representation
    std::map<std::string, double> cpuUsage = data.threadCpuUsage;
    
    // Display thread activity
    for (const auto& thread : data.activeThreads) {
        double cpu = 0.0;
        auto it = cpuUsage.find(thread);
        if (it != cpuUsage.end()) {
            cpu = it->second;
        }
        
        oss << "  " << thread << ": ";
        
        // Create CPU bar
        int barLength = static_cast<int>(cpu / 10.0); // Scale to 10 units
        for (int i = 0; i < barLength; ++i) oss << "█";
        for (int i = barLength; i < 10; ++i) oss << "░";
        
        oss << " (" << cpu << "% CPU)\n";
    }
    
    return oss.str();
}

std::string ThreadVisualizer::visualizeThreadCallStack(const ThreadVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Thread Call Stack Visualization ===\n"
        << "Session: " << data.sessionId << "\n"
        << "Show Stack Traces: " << (data.showStackTraces ? "Yes" : "No") << "\n\n";
    
    if (data.showStackTraces) {
        oss << "Thread Stack Traces:\n";
        
        // In a real implementation, this would capture actual stack traces
        // For now, show placeholder data
        for (const auto& thread : data.activeThreads) {
            oss << "  " << thread << ":\n";
            
            auto it = data.threadActivities.find(thread);
            if (it != data.threadActivities.end()) {
                for (const auto& activity : it->second) {
                    oss << "    " << activity << "\n";
                }
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

std::string ThreadVisualizer::visualizeThreadCpuUsage(const ThreadVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Thread CPU Usage Visualization ===\n"
        << "Session: " << data.sessionId << "\n\n";
    
    oss << "CPU Usage by Thread:\n";
    
    // Sort threads by CPU usage (highest first)
    std::vector<std::pair<std::string, double>> sortedThreads;
    for (const auto& pair : data.threadCpuUsage) {
        sortedThreads.push_back(pair);
    }
    
    std::sort(sortedThreads.begin(), sortedThreads.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (const auto& pair : sortedThreads) {
        const std::string& thread = pair.first;
        double cpu = pair.second;
        
        oss << "  " << thread << ": ";
        
        // Create CPU bar
        int barLength = static_cast<int>(cpu);
        for (int i = 0; i < barLength; ++i) oss << "█";
        for (int i = barLength; i < 100; i += 10) oss << "░";
        
        oss << " (" << cpu << "%)" << std::endl;
    }
    
    return oss.str();
}

std::string ThreadVisualizer::generateThreadReport(const ThreadVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Thread Analysis Report ===\n"
        << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "\n"
        << "\n";
    
    // Add all visualizations
    oss << visualizeThreadTimeline(data) << "\n";
    oss << visualizeThreadActivityChart(data) << "\n";
    oss << visualizeThreadCallStack(data) << "\n";
    oss << visualizeThreadCpuUsage(data);
    
    return oss.str();
}

bool ThreadVisualizer::exportVisualization(const VisualizationData& vizData, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (vizData.format == "text" || vizData.format.empty()) {
        file << vizData.content;
    } else if (vizData.format == "csv") {
        file << "type,title,timestamp\n";
        file << vizData.type << "," << vizData.title << ","
             << std::chrono::system_clock::to_time_t(vizData.timestamp) << "\n";
        
        file << "metric,value\n";
        for (const auto& pair : vizData.metrics) {
            file << pair.first << "," << pair.second << "\n";
        }
    }
    
    file.flush();
    return true;
}

std::vector<std::string> ThreadVisualizer::getSupportedFormats() const {
    return {"text", "csv", "json", "html", "svg"};
}

ThreadVisualizer& ThreadVisualizer::getGlobal() {
    static ThreadVisualizer globalVisualizer;
    return globalVisualizer;
}

void ThreadVisualizer::setGlobal(std::shared_ptr<ThreadVisualizer> visualizer) {
    // This is a placeholder for advanced global visualizer management
}

// NetworkVisualizer implementation
struct NetworkVisualizer::Impl {
    std::mutex mutex;
};

NetworkVisualizer::NetworkVisualizer() : pImpl(std::make_unique<Impl>()) {}

NetworkVisualizer::~NetworkVisualizer() = default;

std::string NetworkVisualizer::visualizeNetworkTimeline(const NetworkVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Network Timeline ===\n"
        << "Session: " << data.sessionId << "\n"
        << "Time Range: " << std::chrono::system_clock::to_time_t(data.startTime)
        << " - " << std::chrono::system_clock::to_time_t(data.endTime) << "\n"
        << "Hosts Tracked: " << data.connectionsByHost.size() << "\n"
        << "Host Filter: " << (data.hostFilter.empty() ? "None" : data.hostFilter) << "\n"
        << "Show Protocol Breakdown: " << (data.showProtocolBreakdown ? "Yes" : "No") << "\n\n";
    
    oss << "Network Timeline:\n";
    
    // Create timeline data
    for (const auto& pair : data.connectionsByHost) {
        const std::string& host = pair.first;
        size_t connections = pair.second;
        
        oss << "  " << host << ": " << connections << " connections\n";
        
        if (data.showProtocolBreakdown) {
            auto it = data.bytesTransferred.find(host);
            if (it != data.bytesTransferred.end()) {
                oss << "    Bytes transferred: " << it->second << "\n";
            }
        }
    }
    
    return oss.str();
}

std::string NetworkVisualizer::visualizeNetworkTrafficChart(const NetworkVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Network Traffic Chart ===\n"
        << "Session: " << data.sessionId << "\n\n";
    
    oss << "Traffic Chart Legend:\n"
        << "  █ Upload Traffic (blue)\n"
        << "  █ Download Traffic (green)\n"
        << "  █ Connection Events (yellow)\n\n";
    
    oss << "Network Traffic Visualization:\n";
    
    // Create traffic visualization
    std::map<std::string, size_t> totalBytes(data.bytesTransferred);
    
    for (const auto& pair : totalBytes) {
        const std::string& host = pair.first;
        size_t bytes = pair.second;
        
        oss << "  " << host << ": ";
        
        // Create traffic bar (convert to KB for display)
        size_t kb = bytes / 1024;
        int barLength = static_cast<int>(kb / 10.0); // Scale to 10 units
        for (int i = 0; i < barLength; ++i) oss << "█";
        for (int i = barLength; i < 20; i += 2) oss << "░";
        
        oss << " (" << kb << " KB)" << std::endl;
    }
    
    return oss.str();
}

std::string NetworkVisualizer::visualizeNetworkConnectionGraph(const NetworkVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Network Connection Graph ===\n"
        << "Session: " << data.sessionId << "\n\n";
    
    oss << "Connection Graph Legend:\n"
        << "  ● Direct Connection (green)\n"
        << "  ● Indirect Connection (blue)\n"
        << "  ● Connection Attempt (yellow)\n\n";
    
    oss << "Connection Graph Visualization:\n";
    
    // Create connection graph representation
    // In a real implementation, this would create an actual graph
    // For now, show a simple text representation
    oss << "  Local Host\n";
    
    for (const auto& pair : data.connectionsByHost) {
        const std::string& host = pair.first;
        size_t connections = pair.second;
        
        // Determine connection type based on connection count
        std::string symbol;
        if (connections == 0) symbol = "○"; // No connection
        else if (connections == 1) symbol = "●"; // Direct
        else if (connections <= 3) symbol = "◉"; // Indirect
        else symbol = "⚑"; // High activity
        
        oss << "  └── " << symbol << " " << host << " (" << connections << " connections)\n";
    }
    
    return oss.str();
}

std::string NetworkVisualizer::visualizeNetworkProtocolBreakdown(const NetworkVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Network Protocol Breakdown ===\n"
        << "Session: " << data.sessionId << "\n\n";
    
    oss << "Protocol Breakdown Legend:\n"
        << "  █ HTTP/HTTPS (cyan)\n"
        << "  █ TCP (blue)\n"
        << "  █ UDP (yellow)\n"
        << "  █ ICMP (magenta)\n\n";
    
    oss << "Protocol Distribution:\n";
    
    // Simple protocol breakdown (in real implementation would track protocols)
    oss << "  HTTP/HTTPS: Placeholder data\n"
        << "  TCP: Placeholder data\n"
        << "  UDP: Placeholder data\n"
        << "  ICMP: Placeholder data\n";
    
    return oss.str();
}

std::string NetworkVisualizer::generateNetworkReport(const NetworkVisualizationData& data) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Network Analysis Report ===\n"
        << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "\n"
        << "\n";
    
    // Add all visualizations
    oss << visualizeNetworkTimeline(data) << "\n";
    oss << visualizeNetworkTrafficChart(data) << "\n";
    oss << visualizeNetworkConnectionGraph(data) << "\n";
    oss << visualizeNetworkProtocolBreakdown(data);
    
    return oss.str();
}

bool NetworkVisualizer::exportVisualization(const VisualizationData& vizData, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (vizData.format == "text" || vizData.format.empty()) {
        file << vizData.content;
    } else if (vizData.format == "csv") {
        file << "type,title,timestamp\n";
        file << vizData.type << "," << vizData.title << ","
             << std::chrono::system_clock::to_time_t(vizData.timestamp) << "\n";
        
        file << "metric,value\n";
        for (const auto& pair : vizData.metrics) {
            file << pair.first << "," << pair.second << "\n";
        }
    }
    
    file.flush();
    return true;
}

std::vector<std::string> NetworkVisualizer::getSupportedFormats() const {
    return {"text", "csv", "json", "html", "svg"};
}

NetworkVisualizer& NetworkVisualizer::getGlobal() {
    static NetworkVisualizer globalVisualizer;
    return globalVisualizer;
}

void NetworkVisualizer::setGlobal(std::shared_ptr<NetworkVisualizer> visualizer) {
    // This is a placeholder for advanced global visualizer management
}

} // namespace nlm
