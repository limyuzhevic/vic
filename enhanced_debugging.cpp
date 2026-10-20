// Enhanced Python bindings for advanced debugging and monitoring

// This file adds advanced debugging and monitoring capabilities to the Python API
// including performance monitoring, detailed logging, and real-time analytics

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <numeric>
#include <map>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace py = pybind11;
namespace nlm {

// Advanced logging system
class AdvancedLogger {
public:
    enum class LogLevel {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };
    
    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        LogLevel level;
        std::string message;
        std::string source;
        std::map<std::string, std::string> metadata;
    };
    
    AdvancedLogger() : minLevel(LogLevel::Info), logToFile(false), maxLogFileSize(10 * 1024 * 1024) {
        // Setup console logger
        consoleHandler = [this](const LogEntry& entry) {
            this->writeToConsole(entry);
        };
    }
    
    void setLogLevel(LogLevel level) {
        minLevel = level;
    }
    
    void enableFileLogging(const std::string& filepath, size_t maxSize = 10 * 1024 * 1024) {
        logToFile = true;
        logFilePath = filepath;
        maxLogFileSize = maxSize;
        
        // Create log file directory if it doesn't exist
        std::filesystem::path logDir = std::filesystem::path(filepath).parent_path();
        if (!logDir.empty() && !std::filesystem::exists(logDir)) {
            std::filesystem::create_directories(logDir);
        }
    }
    
    void log(LogLevel level, const std::string& message, const std::string& source = "") {
        if (level < minLevel) {
            return;
        }
        
        LogEntry entry;
        entry.timestamp = std::chrono::system_clock::now();
        entry.level = level;
        entry.message = message;
        entry.source = source;
        
        // Call handlers
        for (auto& handler : handlers) {
            try {
                handler(entry);
            } catch (const std::exception& e) {
                writeToConsole({std::chrono::system_clock::now(), LogLevel::Error, 
                               std::string("Log handler error: ") + e.what(), "logger"});
            }
        }
    }
    
    void addHandler(std::function<void(const LogEntry&)>&& handler) {
        handlers.push_back(std::move(handler));
    }
    
    std::vector<LogEntry> getRecentLogs(size_t count = 100, LogLevel minLevel = LogLevel::Info) {
        std::lock_guard<std::mutex> lock(logMutex);
        
        std::vector<LogEntry> recentLogs;
        recentLogs.reserve(std::min(count, logHistory.size()));
        
        auto it = logHistory.end();
        for (size_t i = 0; i < std::min(count, logHistory.size()); ++i) {
            if (it == logHistory.begin()) {
                it = logHistory.end();
            }
            --it;
            if (it->level >= minLevel) {
                recentLogs.push_back(*it);
            }
        }
        
        return recentLogs;
    }
    
    std::string getLogsAsString(size_t count = 50, LogLevel minLevel = LogLevel::Info) {
        auto logs = getRecentLogs(count, minLevel);
        
        std::ostringstream oss;
        oss << "=== NLM LOG REPORT ===" << std::endl;
        oss << "Generated at: " << getTimestampString(std::chrono::system_clock::now()) << std::endl;
        oss << std::endl;
        
        for (const auto& entry : logs) {
            oss << "[" << getLogLevelString(entry.level) << "] " 
                << getTimestampString(entry.timestamp) << " " 
                << entry.source << ": " << entry.message << std::endl;
            
            if (!entry.metadata.empty()) {
                oss << "    Metadata:" << std::endl;
                for (const auto& [key, value] : entry.metadata) {
                    oss << "      " << key << ": " << value << std::endl;
                }
            }
        }
        
        return oss.str();
    }
    
    void exportLogsToFile(const std::string& filepath, size_t count = 1000) {
        auto logs = getRecentLogs(count);
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open log file for writing: " + filepath);
        }
        
        file << getLogsAsString(count);
        file.close();
    }
    
    size_t getLogCount(LogLevel minLevel = LogLevel::Info) const {
        std::lock_guard<std::mutex> lock(logMutex);
        
        size_t count = 0;
        for (const auto& entry : logHistory) {
            if (entry.level >= minLevel) {
                count++;
            }
        }
        
        return count;
    }
    
    std::map<std::string, size_t> getLogCountsByLevel() const {
        std::lock_guard<std::mutex> lock(logMutex);
        
        std::map<std::string, size_t> counts;
        counts["TRACE"] = 0;
        counts["DEBUG"] = 0;
        counts["INFO"] = 0;
        counts["WARNING"] = 0;
        counts["ERROR"] = 0;
        counts["CRITICAL"] = 0;
        
        for (const auto& entry : logHistory) {
            switch (entry.level) {
                case LogLevel::Trace:
                    counts["TRACE"]++;
                    break;
                case LogLevel::Debug:
                    counts["DEBUG"]++;
                    break;
                case LogLevel::Info:
                    counts["INFO"]++;
                    break;
                case LogLevel::Warning:
                    counts["WARNING"]++;
                    break;
                case LogLevel::Error:
                    counts["ERROR"]++;
                    break;
                case LogLevel::Critical:
                    counts["CRITICAL"]++;
                    break;
            }
        }
        
        return counts;
    }
    
private:
    void writeToConsole(const LogEntry& entry) {
        std::string levelStr = getLogLevelString(entry.level);
        std::string timestampStr = getTimestampString(entry.timestamp);
        
        std::cout << "[" << levelStr << "] " << timestampStr;
        if (!entry.source.empty()) {
            std::cout << " " << entry.source;
        }
        std::cout << ": " << entry.message << std::endl;
    }
    
    std::string getLogLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
    
    std::string getTimestampString(const std::chrono::system_clock::time_point& timestamp) {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            timestamp.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
           << "." << std::setfill('0') << std::setw(3) << ms.count();
        
        return ss.str();
    }
    
    // Data members
    LogLevel minLevel;
    bool logToFile;
    std::string logFilePath;
    size_t maxLogFileSize;
    std::vector<std::function<void(const LogEntry&)>> handlers;
    mutable std::mutex logMutex;
    std::vector<LogEntry> logHistory;
    std::ofstream logFileStream;
};

// Performance analyzer for NLM simulations
class PerformanceAnalyzer {
public:
    struct TimeSeriesData {
        std::vector<double> timestamps;
        std::vector<double> values;
        std::string name;
        std::string unit;
    };
    
    PerformanceAnalyzer() : samplingInterval(100), maxSamples(10000) {}
    
    void startMonitoring(std::function<std::map<std::string, double>()> metricCollector) {
        stopMonitoring();
        
        collector = std::move(metricCollector);
        running = true;
        
        monitorThread = std::thread(&PerformanceAnalyzer::monitoringWorker, this);
    }
    
    void stopMonitoring() {
        if (running) {
            running = false;
            if (monitorThread.joinable()) {
                monitorThread.join();
            }
            collector = nullptr;
        }
    }
    
    void addMetric(std::string name, std::function<double()> getter) {
        metricGetters[name] = std::move(getter);
    }
    
    void setSamplingInterval(size_t intervalMs) {
        samplingInterval = intervalMs;
    }
    
    TimeSeriesData getTimeSeries(const std::string& metricName, 
                               size_t samples = 100) const {
        TimeSeriesData data;
        data.name = metricName;
        
        std::lock_guard<std::mutex> lock(dataMutex);
        
        auto it = timeSeriesData.find(metricName);
        if (it == timeSeriesData.end()) {
            return data; // Return empty data
        }
        
        const auto& series = it->second;
        
        // Extract requested number of samples
        size_t startIndex = std::max(0, static_cast<long>(series.size() - samples));
        
        data.timestamps.reserve(series.size() - startIndex);
        data.values.reserve(series.size() - startIndex);
        
        double currentTime = 0.0;
        for (size_t i = startIndex; i < series.size(); ++i) {
            data.timestamps.push_back(currentTime);
            data.values.push_back(series[i].second);
            currentTime += 0.001; // Assuming 1ms intervals
        }
        
        return data;
    }
    
    std::map<std::string, double> getStatisticalSummary(const std::string& metricName) const {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        std::map<std::string, double> summary;
        
        auto it = timeSeriesData.find(metricName);
        if (it == timeSeriesData.end()) {
            return summary; // Return empty summary
        }
        
        const auto& series = it->second;
        if (series.empty()) {
            return summary;
        }
        
        std::vector<double> values;
        values.reserve(series.size());
        for (const auto& point : series) {
            values.push_back(point.second);
        }
        
        // Calculate statistics
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        double mean = sum / values.size();
        
        double min = *std::min_element(values.begin(), values.end());
        double max = *std::max_element(values.begin(), values.end());
        
        double variance = 0.0;
        for (double val : values) {
            variance += (val - mean) * (val - mean);
        }
        variance /= values.size();
        double stddev = std::sqrt(variance);
        
        summary["mean"] = mean;
        summary["min"] = min;
        summary["max"] = max;
        summary["stddev"] = stddev;
        summary["count"] = static_cast<double>(values.size());
        
        return summary;
    }
    
    std::string generatePerformanceReport() const {
        std::ostringstream oss;
        oss << "=== NLM PERFORMANCE ANALYSIS ===" << std::endl;
        oss << "Report generated at: " << getCurrentTimestamp() << std::endl;
        oss << std::endl;
        
        oss << "Monitored Metrics:" << std::endl;
        for (const auto& [name, _] : metricGetters) {
            oss << "  - " << name << std::endl;
        }
        oss << std::endl;
        
        oss << "Statistical Summaries:" << std::endl;
        for (const auto& [name, _] : metricGetters) {
            auto summary = getStatisticalSummary(name);
            if (!summary.empty()) {
                oss << "  " << name << ":" << std::endl;
                oss << "    Mean: " << summary["mean"] << std::endl;
                oss << "    Min:  " << summary["min"] << std::endl;
                oss << "    Max:  " << summary["max"] << std::endl;
                oss << "    Std:  " << summary["stddev"] << std::endl;
                oss << "    Count:" << summary["count"] << std::endl;
            }
        }
        
        return oss.str();
    }
    
    void exportPerformanceData(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open performance data file for writing: " + filepath);
        }
        
        file << generatePerformanceReport() << std::endl;
        file << std::endl;
        
        // Export time series data
        file << "TIME_SERIES_DATA:" << std::endl;
        for (const auto& [name, series] : timeSeriesData) {
            file << "Metric: " << name << std::endl;
            file << "Timestamp,Value" << std::endl;
            
            double currentTime = 0.0;
            for (const auto& point : series) {
                file << currentTime << "," << point.second << std::endl;
                currentTime += 0.001; // Assuming 1ms intervals
            }
            file << std::endl;
        }
        
        file.close();
    }
    
    bool isMonitoring() const {
        return running;
    }
    
private:
    void monitoringWorker() {
        while (running) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            if (collector) {
                auto metrics = collector();
                
                std::lock_guard<std::mutex> lock(dataMutex);
                double currentTime = 0.0;
                
                for (const auto& [name, value] : metrics) {
                    timeSeriesData[name].push_back(std::make_pair(currentTime, value));
                    
                    // Update metric getters
                    if (metricGetters.find(name) != metricGetters.end()) {
                        metricGetters[name] = [value]() { return value; };
                    }
                }
                
                // Clean up old data
                cleanupOldData();
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            // Sleep for sampling interval
            if (duration.count() < samplingInterval) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(samplingInterval - duration.count()));
            }
        }
    }
    
    void cleanupOldData() {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        auto currentTime = std::chrono::system_clock::now();
        auto cutoffTime = currentTime - std::chrono::hours(1); // Keep 1 hour of data
        
        for (auto& [name, series] : timeSeriesData) {
            // Remove old entries (simplified - in practice you'd track timestamps properly)
            if (series.size() > maxSamples) {
                size_t removeCount = series.size() - maxSamples;
                series.erase(series.begin(), series.begin() + removeCount);
            }
        }
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        return ss.str();
    }
    
    // Data members
    bool running;
    size_t samplingInterval;
    size_t maxSamples;
    std::thread monitorThread;
    std::function<std::map<std::string, double>()> collector;
    std::map<std::string, std::function<double()>> metricGetters;
    
    mutable std::mutex dataMutex;
    std::map<std::string, std::vector<std::pair<double, double>>> timeSeriesData;
};

// Real-time visualization data structures
struct VisualizationData {
    std::vector<double> neuronActivity;  // Normalized firing rates
    std::vector<double> synapticWeights; // Synaptic strength values
    std::vector<std::pair<double, double>> neuronPositions; // x, y coordinates
    std::vector<std::pair<int, int>> connections; // (pre, post) neuron indices
    std::vector<double> timeStamps;
    
    std::string getJsonRepresentation() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"neuron_activity\": [";
        for (size_t i = 0; i < neuronActivity.size(); ++i) {
            oss << neuronActivity[i];
            if (i < neuronActivity.size() - 1) oss << ", ";
        }
        oss << "],\n";
        
        oss << "  \"synaptic_weights\": [";
        for (size_t i = 0; i < synapticWeights.size(); ++i) {
            oss << synapticWeights[i];
            if (i < synapticWeights.size() - 1) oss << ", ";
        }
        oss << "],\n";
        
        oss << "  \"neuron_positions\": [";
        for (size_t i = 0; i < neuronPositions.size(); ++i) {
            oss << "[" << neuronPositions[i].first << ", " << neuronPositions[i].second << "]";
            if (i < neuronPositions.size() - 1) oss << ", ";
        }
        oss << "],\n";
        
        oss << "  \"connections\": [";
        for (size_t i = 0; i < connections.size(); ++i) {
            oss << "[" << connections[i].first << ", " << connections[i].second << "]";
            if (i < connections.size() - 1) oss << ", ";
        }
        oss << "],\n";
        
        oss << "  \"time_stamps\": [";
        for (size_t i = 0; i < timeStamps.size(); ++i) {
            oss << timeStamps[i];
            if (i < timeStamps.size() - 1) oss << ", ";
        }
        oss << "]\n";
        
        oss << "}";
        
        return oss.str();
    }
};

// Simulation profiler for performance analysis
class SimulationProfiler {
public:
    struct ProfileEvent {
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        std::string threadId;
        std::map<std::string, std::string> metadata;
    };
    
    SimulationProfiler() : recording(false), maxEvents(10000) {}
    
    void startRecording() {
        stopRecording();
        recording = true;
        eventStack.clear();
        events.clear();
    }
    
    void stopRecording() {
        recording = false;
        eventStack.clear();
    }
    
    void beginEvent(const std::string& name, const std::map<std::string, std::string>& metadata = {}) {
        if (!recording) return;
        
        ProfileEvent event;
        event.name = name;
        event.startTime = std::chrono::high_resolution_clock::now();
        event.threadId = getCurrentThreadId();
        event.metadata = metadata;
        
        eventStack.push(event);
    }
    
    void endEvent() {
        if (eventStack.empty()) return;
        
        ProfileEvent event = eventStack.top();
        event.endTime = std::chrono::high_resolution_clock::now();
        
        events.push_back(event);
        eventStack.pop();
        
        // Clean up old events
        if (events.size() > maxEvents) {
            events.erase(events.begin(), events.begin() + (events.size() - maxEvents));
        }
    }
    
    std::string getProfileReport() const {
        std::ostringstream oss;
        oss << "=== SIMULATION PROFILING REPORT ===" << std::endl;
        oss << "Generated at: " << getCurrentTimestamp() << std::endl;
        oss << std::endl;
        oss << "Total Events: " << events.size() << std::endl;
        oss << "Active Events: " << eventStack.size() << std::endl;
        oss << std::endl;
        
        // Calculate statistics
        std::map<std::string, std::vector<double>> eventDurations;
        
        for (const auto& event : events) {
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                event.endTime - event.startTime).count() / 1000.0; // Convert to milliseconds
            
            eventDurations[event.name].push_back(duration);
        }
        
        oss << "Performance Summary:" << std::endl;
        for (const auto& [name, durations] : eventDurations) {
            if (durations.empty()) continue;
            
            double total = std::accumulate(durations.begin(), durations.end(), 0.0);
            double avg = total / durations.size();
            double min = *std::min_element(durations.begin(), durations.end());
            double max = *std::max_element(durations.begin(), durations.end());
            
            oss << "  " << name << ":" << std::endl;
            oss << "    Calls: " << durations.size() << std::endl;
            oss << "    Total: " << total << " ms" << std::endl;
            oss << "    Average: " << avg << " ms" << std::endl;
            oss << "    Min: " << min << " ms" << std::endl;
            oss << "    Max: " << max << " ms" << std::endl;
        }
        
        return oss.str();
    }
    
    void exportProfileData(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open profile data file for writing: " + filepath);
        }
        
        file << getProfileReport() << std::endl;
        
        // Export raw event data
        file << "RAW_EVENTS:" << std::endl;
        for (const auto& event : events) {
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                event.endTime - event.startTime).count() / 1000.0;
            
            file << event.name << "," << duration << "," << event.threadId << ",";
            
            // Export metadata
            bool first = true;
            for (const auto& [key, value] : event.metadata) {
                if (!first) file << ";";
                file << key << "=" << value;
                first = false;
            }
            file << std::endl;
        }
        
        file.close();
    }
    
    size_t getEventCount() const {
        return events.size();
    }
    
    size_t getActiveEventCount() const {
        return eventStack.size();
    }
    
private:
    std::string getCurrentThreadId() const {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        return ss.str();
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        return ss.str();
    }
    
    // Data members
    bool recording;
    size_t maxEvents;
    std::stack<ProfileEvent> eventStack;
    std::vector<ProfileEvent> events;
};

// Enhanced Python bindings for debugging and monitoring
void addAdvancedDebuggingBindings(py::module_& m) {
    // Advanced logger
    py::class_<AdvancedLogger>(m, "AdvancedLogger")
        .def(py::init<>())
        .def("set_log_level", &AdvancedLogger::setLogLevel, py::arg("level"),
             "Set minimum log level")
        .def("enable_file_logging", &AdvancedLogger::enableFileLogging, 
             py::arg("filepath"), py::arg("maxSize") = 10 * 1024 * 1024,
             "Enable file logging with size limit")
        .def("log", &AdvancedLogger::log, py::arg("level"), py::arg("message"), 
             py::arg("source") = "",
             "Log a message at specified level")
        .def("add_handler", &AdvancedLogger::addHandler, py::arg("handler"),
             "Add custom log handler")
        .def("get_recent_logs", &AdvancedLogger::getRecentLogs,
             py::arg("count") = 100, py::arg("minLevel") = py::arg("level"),
             "Get recent log entries")
        .def("get_logs_as_string", &AdvancedLogger::getLogsAsString,
             py::arg("count") = 50, py::arg("minLevel") = py::arg("level"),
             "Get logs as formatted string")
        .def("export_logs_to_file", &AdvancedLogger::exportLogsToFile,
             py::arg("filepath"), py::arg("count") = 1000,
             "Export logs to file");
    
    // Performance analyzer
    py::class_<PerformanceAnalyzer>(m, "PerformanceAnalyzer")
        .def(py::init<>())
        .def("start_monitoring", &PerformanceAnalyzer::startMonitoring,
             py::arg("metricCollector"),
             "Start performance monitoring")
        .def("stop_monitoring", &PerformanceAnalyzer::stopMonitoring,
             "Stop performance monitoring")
        .def("add_metric", &PerformanceAnalyzer::addMetric,
             py::arg("name"), py::arg("getter"),
             "Add custom metric to monitor")
        .def("set_sampling_interval", &PerformanceAnalyzer::setSamplingInterval,
             py::arg("intervalMs"),
             "Set monitoring sampling interval")
        .def("get_time_series", &PerformanceAnalyzer::getTimeSeries,
             py::arg("metricName"), py::arg("samples") = 100,
             "Get time series data for metric")
        .def("get_statistical_summary", &PerformanceAnalyzer::getStatisticalSummary,
             py::arg("metricName"),
             "Get statistical summary for metric")
        .def("generate_performance_report", &PerformanceAnalyzer::generatePerformanceReport,
             "Generate performance analysis report")
        .def("export_performance_data", &PerformanceAnalyzer::exportPerformanceData,
             py::arg("filepath"),
             "Export performance data to file")
        .def("is_monitoring", &PerformanceAnalyzer::isMonitoring,
             "Check if monitoring is active");
    
    // Simulation profiler
    py::class_<SimulationProfiler>(m, "SimulationProfiler")
        .def(py::init<>())
        .def("start_recording", &SimulationProfiler::startRecording,
             "Start recording simulation events")
        .def("stop_recording", &SimulationProfiler::stopRecording,
             "Stop recording simulation events")
        .def("begin_event", &SimulationProfiler::beginEvent,
             py::arg("name"), py::arg("metadata") = py::dict(),
             "Begin profiling an event")
        .def("end_event", &SimulationProfiler::endEvent,
             "End current profiling event")
        .def("get_profile_report", &SimulationProfiler::getProfileReport,
             "Get profiling report")
        .def("export_profile_data", &SimulationProfiler::exportProfileData,
             py::arg("filepath"),
             "Export profile data to file")
        .def("get_event_count", &SimulationProfiler::getEventCount,
             "Get total number of events recorded")
        .def("get_active_event_count", &SimulationProfiler::getActiveEventCount,
             "Get number of currently active events");
    
    // Visualization data
    py::class_<VisualizationData>(m, "VisualizationData")
        .def(py::init<>())
        .def_readwrite("neuron_activity", &VisualizationData::neuronActivity)
        .def_readwrite("synaptic_weights", &VisualizationData::synapticWeights)
        .def_readwrite("neuron_positions", &VisualizationData::neuronPositions)
        .def_readwrite("connections", &VisualizationData::connections)
        .def_readwrite("time_stamps", &VisualizationData::timeStamps)
        .def("get_json_representation", &VisualizationData::getJsonRepresentation,
             "Get visualization data as JSON string");
    
    // Convenience functions for debugging
    m.def("create_debug_logger", []() {
        return std::make_shared<AdvancedLogger>();
    }, "Create advanced debug logger");
    
    m.def("create_performance_analyzer", []() {
        return std::make_shared<PerformanceAnalyzer>();
    }, "Create performance analyzer");
    
    m.def("create_simulation_profiler", []() {
        return std::make_shared<SimulationProfiler>();
    }, "Create simulation profiler");
    
    // Integration with existing NLM components
    m.def("start_system_monitoring", [](std::shared_ptr<Brain> brain,
                                       std::shared_ptr<AdvancedLogger> logger) {
        // Setup monitoring for brain operations
        // This would integrate with the existing NLM logging system
        return true;
    }, py::arg("brain"), py::arg("logger"),
       "Start system monitoring with brain integration");
    
    m.def("capture_performance_snapshot", [](std::shared_ptr<Brain> brain,
                                             std::shared_ptr<PerformanceAnalyzer> analyzer) {
        // Capture current performance metrics
        // This would collect brain-specific metrics
        return std::map<std::string, double>();
    }, py::arg("brain"), py::arg("analyzer"),
       "Capture performance snapshot for analysis");
}

// Integration function to add all advanced debugging bindings
void addAllAdvancedDebugging(py::module_& m) {
    addAdvancedDebuggingBindings(m);
    
    // Add version information
    m.attr("__version__") = "2.0.0-advanced-debugging";
    
    // Add debugging utilities
    m.def("debug_version", []() {
        return "NLM Advanced Debugging Tools v2.0.0";
    }, "Get debugging tools version");
    
    m.def("debug_info", []() {
        std::map<std::string, std::string> info;
        info["version"] = "2.0.0-advanced-debugging";
        info["features"] = "Advanced logging, Performance analysis, Event profiling";
        info["status"] = "Ready for enhanced debugging";
        return info;
    }, "Get debugging system information");
}