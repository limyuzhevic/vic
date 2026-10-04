#include "DebugVisualizer.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

DebugVisualizer::DebugVisualizer()
    : port_(8080), host_("localhost"), enableWebInterface_(false),
      updateInterval_(0.1), serverRunning_(false) {
}

DebugVisualizer::DebugVisualizer(std::shared_ptr<DebuggingSystem> debuggingSystem)
    : debuggingSystem_(debuggingSystem),
      port_(8080), host_("localhost"), enableWebInterface_(false),
      updateInterval_(0.1), serverRunning_(false) {
}

DebugVisualizer::~DebugVisualizer() {
    stopServer();
}

void DebugVisualizer::startServer() {
    if (serverRunning_ || !enableWebInterface_) return;
    
    serverRunning_ = true;
    serverThread_ = std::thread(&DebugVisualizer::monitoringLoop, this);
    
    Logger::getGlobal().info("Debug visualizer server started on " + host_ + ":" + std::to_string(port_));
}

void DebugVisualizer::stopServer() {
    if (!serverRunning_) return;
    
    serverRunning_ = false;
    
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
}

void DebugVisualizer::monitoringLoop() {
    while (serverRunning_) {
        // Update visualization data
        // In a real implementation, this would update websocket connections
        std::this_thread::sleep_for(std::chrono::duration<double>(updateInterval_));
    }
}

std::string DebugVisualizer::generateDashboardHTML() const {
    std::ostringstream oss;
    oss << generateHTMLHeader();
    oss << "<h1>NLM Debug Dashboard</h1>\n";
    oss << "<div class='dashboard'>\n";
    oss << "<div class='card'>\n";
    oss << "<h2>Trace Events</h2>\n";
    oss << "<div id='trace-events'>" << (debuggingSystem_ ? std::to_string(debuggingSystem_->getTrace()->getEventCount()) : "0") << " events</div>\n";
    oss << "</div>\n";
    oss << "<div class='card'>\n";
    oss << "<h2>Performance</h2>\n";
    oss << "<div id='performance-metrics'>TBD</div>\n";
    oss << "</div>\n";
    oss << "<div class='card'>\n";
    oss << "<h2>Memory</h2>\n";
    oss << "<div id='memory-usage'>TBD</div>\n";
    oss << "</div>\n";
    oss << "<div class='card'>\n";
    oss << "<h2>Events</h2>\n";
    oss << "<div id='event-count'>TBD</div>\n";
    oss << "</div>\n";
    oss << "</div>\n";
    oss << generateHTMLFooter();
    return oss.str();
}

std::string DebugVisualizer::generateTraceView() const {
    std::ostringstream oss;
    oss << generateHTMLHeader();
    oss << "<h1>Trace Events</h1>\n";
    oss << "<div id='trace-container'>\n";
    oss << "<!-- Trace events will be displayed here -->\n";
    oss << "</div>\n";
    oss << generateHTMLFooter();
    return oss.str();
}

std::string DebugVisualizer::generatePerformanceView() const {
    std::ostringstream oss;
    oss << generateHTMLHeader();
    oss << "<h1>Performance Metrics</h1>\n";
    oss << "<div id='performance-container'>\n";
    oss << "<!-- Performance metrics will be displayed here -->\n";
    oss << "</div>\n";
    oss << generateHTMLFooter();
    return oss.str();
}

std::string DebugVisualizer::generateMemoryView() const {
    std::ostringstream oss;
    oss << generateHTMLHeader();
    oss << "<h1>Memory Usage</h1>\n";
    oss << "<div id='memory-container'>\n";
    oss << "<!-- Memory usage will be displayed here -->\n";
    oss << "</div>\n";
    oss << generateHTMLFooter();
    return oss.str();
}

std::string DebugVisualizer::generateEventView() const {
    std::ostringstream oss;
    oss << generateHTMLHeader();
    oss << "<h1>Event Log</h1>\n";
    oss << "<div id='event-container'>\n";
    oss << "<!-- Events will be displayed here -->\n";
    oss << "</div>\n";
    oss << generateHTMLFooter();
    return oss.str();
}

std::string DebugVisualizer::getAPIDebugInfo() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"status\": \"active\",\n";
    oss << "  \"traceEvents\": " << (debuggingSystem_ && debuggingSystem_->getTrace() ? debuggingSystem_->getTrace()->getEventCount() : 0) << ",\n";
    oss << "  \"version\": \"1.0.0\"\n";
    oss << "}\n";
    return oss.str();
}

std::string DebugVisualizer::getAPIPerformanceData() const {
    std::ostringstream oss;
    oss << "[\n";
    // In a real implementation, this would return actual performance data
    oss << "  {\"timestamp\": \"2026-01-01T00:00:00Z\", \"cpu\": 50.0, \"memory\": 25.0}\n";
    oss << "]\n";
    return oss.str();
}

std::string DebugVisualizer::getAPIMemoryData() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"currentUsage\": 0,\n";
    oss << "  \"totalAllocated\": 0,\n";
    oss << "  \"totalFreed\": 0\n";
    oss << "}\n";
    return oss.str();
}

std::string DebugVisualizer::getAPIEventsData() const {
    std::ostringstream oss;
    oss << "[]\n";
    return oss.str();
}

bool DebugVisualizer::exportVisualization(const std::string& directory) const {
    try {
        std::ofstream file(directory + "/dashboard.html");
        if (!file.is_open()) {
            return false;
        }
        
        file << generateDashboardHTML();
        file.close();
        
        return true;
    } catch (...) {
        return false;
    }
}

std::string DebugVisualizer::generateHTMLHeader() const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << "<head>\n";
    oss << "<title>NLM Debug Visualization</title>\n";
    oss << "<style>\n";
    oss << "body { font-family: Arial, sans-serif; margin: 20px; }\n";
    oss << ".dashboard { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 20px; }\n";
    oss << ".card { border: 1px solid #ddd; padding: 20px; border-radius: 5px; background: #f9f9f9; }\n";
    oss << "h1 { color: #333; }\n";
    oss << "h2 { color: #555; }\n";
    oss << "</style>\n";
    oss << "</head>\n";
    oss << "<body>\n";
    return oss.str();
}

std::string DebugVisualizer::generateHTMLFooter() const {
    std::ostringstream oss;
    oss << "</body>\n";
    oss << "</html>\n";
    return oss.str();
}

std::string DebugVisualizer::generateEventChart(const std::vector<SimulationEvent>& events) const {
    return "<!-- Event chart would go here -->\n";
}

std::string DebugVisualizer::generateMemoryChart(const std::vector<MemorySnapshot>& snapshots) const {
    return "<!-- Memory chart would go here -->\n";
}

std::string DebugVisualizer::generatePerformanceChart(const std::vector<std::string>& operations) const {
    return "<!-- Performance chart would go here -->\n";
}

std::string DebugVisualizer::escapeHTML(const std::string& text) const {
    std::ostringstream oss;
    for (char c : text) {
        switch (c) {
            case '&': oss << "&amp;"; break;
            case '<': oss << "&lt;"; break;
            case '>': oss << "&gt;"; break;
            case '"': oss << "&quot;"; break;
            case '\'': oss << "&#x27;"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

} // namespace nlm
