// PerformanceDashboard.cpp
#include "PerformanceDashboard.cpp"
#include <nlohmann/json.hpp>
#include <algorithm>

using json = nlohmann::json;

namespace nlm {

PerformanceDashboard::PerformanceDashboard() = default;
PerformanceDashboard::~PerformanceDashboard() = default;

void PerformanceDashboard::addProfiler(std::shared_ptr<PerformanceProfiler> profiler) {
    profilers_.push_back(std::move(profiler));
}

void PerformanceDashboard::addTimelineProfiler(std::shared_ptr<TimelineProfiler> timelineProfiler) {
    timelineProfilers_.push_back(std::move(timelineProfiler));
}

void PerformanceDashboard::addMemoryProfiler(std::shared_ptr<MemoryProfiler> memoryProfiler) {
    memoryProfilers_.push_back(std::move(memoryProfiler));
}

void PerformanceDashboard::addCPUProfiler(std::shared_ptr<CPUProfiler> cpuProfiler) {
    cpuProfilers_.push_back(std::move(cpuProfiler));
}

void PerformanceDashboard::addThreadProfiler(std::shared_ptr<ThreadProfiler> threadProfiler) {
    threadProfilers_.push_back(std::move(threadProfiler));
}

void PerformanceDashboard::addMonitor(std::shared_ptr<PerformanceMonitor> monitor) {
    monitors_.push_back(std::move(monitor));
}

std::string PerformanceDashboard::generateJSONDashboard() const {
    json j;
    
    // Add profiler data
    j["profilers"] = json::array();
    for (const auto& profiler : profilers_) {
        j["profilers"].push_back(json::parse(profiler->generateReport(true)));
    }
    
    // Add timeline data
    j["timeline"] = json::array();
    for (const auto& timelineProfiler : timelineProfilers_) {
        j["timeline"].push_back(json::parse(timelineProfiler->generateTimelineReport()));
    }
    
    // Add memory data
    j["memory"] = json::array();
    for (const auto& memoryProfiler : memoryProfilers_) {
        j["memory"].push_back(json::parse(memoryProfiler->generateMemoryReport()));
    }
    
    // Add CPU data
    j["cpu"] = json::array();
    for (const auto& cpuProfiler : cpuProfilers_) {
        j["cpu"].push_back(json::parse(cpuProfiler->generateCPUReport()));
    }
    
    // Add thread data
    j["threads"] = json::array();
    for (const auto& threadProfiler : threadProfilers_) {
        j["threads"].push_back(json::parse(threadProfiler->generateThreadReport()));
    }
    
    // Add monitor data
    j["monitor"] = json::array();
    for (const auto& monitor : monitors_) {
        j["monitor"].push_back(json::parse(monitor->generatePerformanceReport()));
        j["monitor"].push_back(json::parse(monitor->generateDashboardData()));
    }
    
    return j.dump(2);
}

std::string PerformanceDashboard::generateHTMLDashboard() const {
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n";
    ss << "<html>\n";
    ss << "<head>\n";
    ss << "  <title>NLM Performance Dashboard</title>\n";
    ss << "  <style>\n";
    ss << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
    ss << "    h1 { color: #333; }\n";
    ss << "    h2 { color: #666; border-bottom: 1px solid #ccc; padding-bottom: 10px; }\n";
    ss << "    .metric { background-color: #f5f5f5; padding: 15px; margin: 10px 0; border-radius: 5px; }\n";
    ss << "    .good { border-left: 5px solid #4CAF50; }\n";
    ss << "    .warning { border-left: 5px solid #FF9800; }\n";
    ss << "    .bad { border-left: 5px solid #F44336; }\n";
    ss << "    pre { background-color: #f8f8f8; padding: 10px; overflow-x: auto; }\n";
    ss << "  </style>\n";
    ss << "</head>\n";
    ss << "<body>\n";
    ss << "  <h1>NLM Performance Dashboard</h1>\n";
    ss << "  <p>Last updated: " << std::chrono::system_clock::now() << "</p>\n";
    
    // Generate sections for each data source
    ss << "  <h2>Profiler Data</h2>\n";
    for (size_t i = 0; i < profilers_.size(); ++i) {
        ss << "  <div class=\"metric\">\n";
        ss << "    <h3>Profiler " << i << "</h3>\n";
        ss << "    <pre>" << profilers_[i]->generateReport(true) << "</pre>\n";
        ss << "  </div>\n";
    }
    
    ss << "  <h2>Timeline Data</h2>\n";
    for (size_t i = 0; i < timelineProfilers_.size(); ++i) {
        ss << "  <div class=\"metric\">\n";
        ss << "    <h3>Timeline " << i << "</h3>\n";
        ss << "    <pre>" << timelineProfilers_[i]->generateTimelineReport() << "</pre>\n";
        ss << "  </div>\n";
    }
    
    ss << "  <h2>Memory Data</h2>\n";
    for (size_t i = 0; i < memoryProfilers_.size(); ++i) {
        ss << "  <div class=\"metric\">\n";
        ss << "    <h3>Memory " << i << "</h3>\n";
        ss << "    <pre>" << memoryProfilers_[i]->generateMemoryReport() << "</pre>\n";
        ss << "  </div>\n";
    }
    
    ss << "  <h2>CPU Data</h2>\n";
    for (size_t i = 0; i < cpuProfilers_.size(); ++i) {
        ss << "  <div class=\"metric\">\n";
        ss << "    <h3>CPU " << i << "</h3>\n";
        ss << "    <pre>" << cpuProfilers_[i]->generateCPUReport() << "</pre>\n";
        ss << "  </div>\n";
    }
    
    ss << "  <h2>Thread Data</h2>\n";
    for (size_t i = 0; i < threadProfilers_.size(); ++i) {
        ss << "  <div class=\"metric\">\n";
        ss << "    <h3>Thread " << i << "</h3>\n";
        ss << "    <pre>" << threadProfilers_[i]->generateThreadReport() << "</pre>\n";
        ss << "  </div>\n";
    }
    
    ss << "  <h2>Monitor Data</h2>\n";
    for (size_t i = 0; i < monitors_.size(); ++i) {
        ss << "  <div class=\"metric\">\n";
        ss << "    <h3>Monitor " << i << "</h3>\n";
        ss << "    <pre>" << monitors_[i]->generatePerformanceReport() << "</pre>\n";
        ss << "  </div>\n";
    }
    
    ss << "</body>\n";
    ss << "</html>\n";
    
    return ss.str();
}

std::string PerformanceDashboard::generateMarkdownDashboard() const {
    std::stringstream ss;
    ss << "# NLM Performance Dashboard\n\n";
    ss << "Generated on: " << std::chrono::system_clock::now() << "\n\n";
    
    ss << "## Profiler Data\n\n";
    for (size_t i = 0; i < profilers_.size(); ++i) {
        ss << "### Profiler " << i << "\n\n";
        ss << "```\n" << profilers_[i]->generateReport(true) << "\n```\n\n";
    }
    
    ss << "## Timeline Data\n\n";
    for (size_t i = 0; i < timelineProfilers_.size(); ++i) {
        ss << "### Timeline " << i << "\n\n";
        ss << "```\n" << timelineProfilers_[i]->generateTimelineReport() << "\n```\n\n";
    }
    
    ss << "## Memory Data\n\n";
    for (size_t i = 0; i < memoryProfilers_.size(); ++i) {
        ss << "### Memory " << i << "\n\n";
        ss << "```\n" << memoryProfilers_[i]->generateMemoryReport() << "\n```\n\n";
    }
    
    ss << "## CPU Data\n\n";
    for (size_t i = 0; i < cpuProfilers_.size(); ++i) {
        ss << "### CPU " << i << "\n\n";
        ss << "```\n" << cpuProfilers_[i]->generateCPUReport() << "\n```\n\n";
    }
    
    ss << "## Thread Data\n\n";
    for (size_t i = 0; i < threadProfilers_.size(); ++i) {
        ss << "### Thread " << i << "\n\n";
        ss << "```\n" << threadProfilers_[i]->generateThreadReport() << "\n```\n\n";
    }
    
    ss << "## Monitor Data\n\n";
    for (size_t i = 0; i < monitors_.size(); ++i) {
        ss << "### Monitor " << i << "\n\n";
        ss << "```\n" << monitors_[i]->generatePerformanceReport() << "\n```\n\n";
    }
    
    return ss.str();
}

bool PerformanceDashboard::saveReport(const std::string& filename, const std::string& format) {
    if (format == "json") {
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        file << generateJSONDashboard();
        file.close();
        return true;
    } else if (format == "html") {
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        file << generateHTMLDashboard();
        file.close();
        return true;
    } else if (format == "md" || format == "markdown") {
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        file << generateMarkdownDashboard();
        file.close();
        return true;
    }
    
    return false;
}

} // namespace nlm
