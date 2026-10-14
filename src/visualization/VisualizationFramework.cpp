#include "VisualizationFramework.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>

namespace nlm {

struct VisualizationFramework::Impl {
    std::unordered_map<std::string, std::unique_ptr<IVisualizationBackend>> backends;
    std::string activeBackend;
    VisualizationMode mode;
    double updateInterval;
    double lastUpdateTime;
    bool active;
    std::vector<VisualizationData> data;
    std::unordered_map<std::string, std::function<void(const VisualizationData&)>> eventListeners;
    EnhancedConfig config;
    
    Impl() : mode(VisualizationMode::RealTime), updateInterval(1.0), 
             lastUpdateTime(0.0), active(false) {}
};

VisualizationFramework::VisualizationFramework() : pImpl(std::make_unique<Impl>()) {}

bool VisualizationFramework::initialize(const EnhancedConfig& config) {
    try {
        pImpl->config = config;
        
        // Initialize configuration
        if (config.has("visualization.backend")) {
            pImpl->activeBackend = config.getString("visualization.backend");
        }
        
        if (config.has("visualization.mode")) {
            std::string modeStr = config.getString("visualization.mode");
            if (modeStr == "static") pImpl->mode = VisualizationMode::Static;
            else if (modeStr == "realtime") pImpl->mode = VisualizationMode::RealTime;
            else if (modeStr == "buffered") pImpl->mode = VisualizationMode::Buffered;
            else if (modeStr == "triggered") pImpl->mode = VisualizationMode::Triggered;
            else if (modeStr == "scheduled") pImpl->mode = VisualizationMode::Scheduled;
        }
        
        if (config.has("visualization.update_interval")) {
            pImpl->updateInterval = config.getDouble("visualization.update_interval");
        }
        
        // Create and initialize backends
        if (config.has("visualization.enable_console")) {
            auto consoleBackend = std::make_unique<ConsoleVisualizationBackend>();
            std::unordered_map<std::string, std::any> consoleConfig;
            consoleConfig["width"] = config.getInt("visualization.console_width", 80);
            consoleConfig["height"] = config.getInt("visualization.console_height", 24);
            consoleConfig["style"] = config.getString("visualization.console_style", "default");
            consoleConfig["colors"] = config.getBool("visualization.console_colors", true);
            consoleConfig["update_interval"] = config.getDouble("visualization.console_update_interval", 1.0);
            
            if (consoleBackend->initialize(consoleConfig)) {
                pImpl->backends["console"] = std::move(consoleBackend);
            }
        }
        
        if (config.has("visualization.enable_json")) {
            auto jsonBackend = std::make_unique<JSONVisualizationBackend>();
            std::unordered_map<std::string, std::any> jsonConfig;
            jsonConfig["pretty_print"] = config.getBool("visualization.json_pretty_print", false);
            jsonConfig["indentation"] = config.getInt("visualization.json_indentation", 2);
            
            if (jsonBackend->initialize(jsonConfig)) {
                pImpl->backends["json"] = std::move(jsonBackend);
            }
        }
        
        if (config.has("visualization.enable_csv")) {
            auto csvBackend = std::make_unique<CSVVisualizationBackend>();
            std::unordered_map<std::string, std::any> csvConfig;
            csvConfig["separator"] = config.getString("visualization.csv_separator", ",").front();
            csvConfig["quote"] = config.getString("visualization.csv_quote", "\"").front();
            csvConfig["headers"] = config.getBool("visualization.csv_headers", true);
            
            if (csvBackend->initialize(csvConfig)) {
                pImpl->backends["csv"] = std::move(csvBackend);
            }
        }
        
        if (config.has("visualization.enable_web")) {
            auto webBackend = std::make_unique<WebVisualizationBackend>();
            std::unordered_map<std::string, std::any> webConfig;
            webConfig["port"] = config.getInt("visualization.web_port", 8080);
            webConfig["host"] = config.getString("visualization.web_host", "localhost");
            webConfig["auto_open"] = config.getBool("visualization.web_auto_open", true);
            
            // Add JavaScript
            if (config.has("visualization.web_javascript")) {
                std::string js = config.getString("visualization.web_javascript");
                webBackend->addJavaScript(js);
            }
            
            // Add CSS
            if (config.has("visualization.web_css")) {
                std::string css = config.getString("visualization.web_css");
                webBackend->addCSS(css);
            }
            
            if (webBackend->initialize(webConfig)) {
                pImpl->backends["web"] = std::move(webBackend);
            }
        }
        
        // Set active backend
        if (!pImpl->activeBackend.empty() && pImpl->backends.find(pImpl->activeBackend) != pImpl->backends.end()) {
            pImpl->active = true;
        }
        
        return pImpl->active;
    } catch (...) {
        return false;
    }
}

void VisualizationFramework::shutdown() {
    pImpl->backends.clear();
    pImpl->activeBackend.clear();
    pImpl->data.clear();
    pImpl->eventListeners.clear();
    pImpl->active = false;
}

void VisualizationFramework::addBackend(std::unique_ptr<IVisualizationBackend> backend) {
    if (backend) {
        std::string name = backend->getName();
        pImpl->backends[name] = std::move(backend);
    }
}

bool VisualizationFramework::setActiveBackend(const std::string& name) {
    if (pImpl->backends.find(name) != pImpl->backends.end()) {
        pImpl->activeBackend = name;
        pImpl->active = true;
        return true;
    }
    return false;
}

std::string VisualizationFramework::getActiveBackend() const {
    return pImpl->activeBackend;
}

void VisualizationFramework::update(const Brain& brain, std::chrono::system_clock::time_point timestamp) {
    if (!pImpl->active) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    
    // Check if it's time to update
    if (pImpl->mode == VisualizationMode::RealTime && 
        (now - pImpl->lastUpdateTime).count() >= pImpl->updateInterval) {
        
        // Collect data from brain
        VisualizationData data;
        data.timestamp = now;
        data.format = "console";  // Default format
        data.content = VisualizationUtils::formatNetworkTopology(brain);
        
        // Update data
        pImpl->data.push_back(std::move(data));
        
        // Trigger event listeners
        triggerEvent("visualization_update", data);
        
        // Update backends
        for (auto& pair : pImpl->backends) {
            pair.second->update(data);
        }
        
        pImpl->lastUpdateTime = now;
    }
}

bool VisualizationFramework::render(const std::string& output) {
    if (!pImpl->active || pImpl->data.empty()) {
        return false;
    }
    
    // Render with active backend
    if (!pImpl->activeBackend.empty() && pImpl->backends.find(pImpl->activeBackend) != pImpl->backends.end()) {
        return pImpl->backends[pImpl->activeBackend]->render(output);
    }
    
    // If no active backend or render failed, try all backends
    for (auto& pair : pImpl->backends) {
        if (pair.second->render(output)) {
            return true;
        }
    }
    
    return false;
}

bool VisualizationFramework::isActive() const {
    return pImpl->active;
}

void VisualizationFramework::setMode(VisualizationMode mode) {
    pImpl->mode = mode;
}

VisualizationMode VisualizationFramework::getMode() const {
    return pImpl->mode;
}

void VisualizationFramework::setUpdateInterval(double interval) {
    pImpl->updateInterval = interval;
}

double VisualizationFramework::getUpdateInterval() const {
    return pImpl->updateInterval;
}

void VisualizationFramework::addEventListener(const std::string& event,
                                             std::function<void(const VisualizationData&)> callback) {
    pImpl->eventListeners[event] = std::move(callback);
}

void VisualizationFramework::removeEventListener(const std::string& event) {
    pImpl->eventListeners.erase(event);
}

void VisualizationFramework::triggerEvent(const std::string& event, const VisualizationData& data) {
    auto it = pImpl->eventListeners.find(event);
    if (it != pImpl->eventListeners.end()) {
        it->second(data);
    }
}

bool VisualizationFramework::generateReport(const Brain& brain, const std::string& outputDir) {
    bool success = true;
    
    // Generate spike raster visualization
    if (!generateSpikeRaster(brain, outputDir + "/spike_raster.csv")) {
        NLM_LOG_ERROR("Failed to generate spike raster visualization");
        success = false;
    }
    
    // Generate network topology visualization
    if (!generateNetworkTopology(brain, outputDir + "/network_topology.html")) {
        NLM_LOG_ERROR("Failed to generate network topology visualization");
        success = false;
    }
    
    // Generate activity visualization
    if (!generateActivityVisualization(brain, outputDir + "/activity.html")) {
        NLM_LOG_ERROR("Failed to generate activity visualization");
        success = false;
    }
    
    // Generate weight matrix visualization
    if (!generateWeightMatrix(brain, outputDir + "/weight_matrix.csv")) {
        NLM_LOG_ERROR("Failed to generate weight matrix visualization");
        success = false;
    }
    
    // Generate HTML report
    std::string reportPath = outputDir + "/visualization_report.html";
    if (!render(reportPath)) {
        NLM_LOG_ERROR("Failed to generate HTML report");
        success = false;
    }
    
    return success;
}

bool VisualizationFramework::generateSpikeRaster(const Brain& brain, const std::string& output) {
    try {
        // Get spike data from brain
        std::vector<SpikeEvent> spikes;
        // In a real implementation, this would extract spike events from the brain
        
        // Format spike data
        std::string spikeData = VisualizationUtils::formatSpikeData(spikes, brain);
        
        // Save spike data
        std::ofstream file(output);
        if (file.is_open()) {
            file << "Time,Neuron,Amplitude\n";
            file << spikeData;
            file.close();
            return true;
        }
        
        return false;
    } catch (...) {
        return false;
    }
}

bool VisualizationFramework::generateNetworkTopology(const Brain& brain, const std::string& output) {
    try {
        // Generate network topology visualization
        std::string topologyData = VisualizationUtils::formatNetworkTopology(brain);
        
        // Create HTML visualization
        std::string html = VisualizationUtils::generateHTMLVisualization(
            "Network Topology", topologyData);
        
        // Save HTML file
        std::ofstream file(output);
        if (file.is_open()) {
            file << html;
            file.close();
            return true;
        }
        
        return false;
    } catch (...) {
        return false;
    }
}

bool VisualizationFramework::generateActivityVisualization(const Brain& brain, const std::string& output) {
    try {
        // Get activity data from brain
        size_t timeStep = 0;  // In real implementation, this would be current time step
        
        // Format activity data
        std::string activityData = VisualizationUtils::formatActivityData(brain, timeStep);
        
        // Create HTML visualization
        std::string html = VisualizationUtils::generateHTMLVisualization(
            "Neural Activity", activityData);
        
        // Save HTML file
        std::ofstream file(output);
        if (file.is_open()) {
            file << html;
            file.close();
            return true;
        }
        
        return false;
    } catch (...) {
        return false;
    }
}

bool VisualizationFramework::generateWeightMatrix(const Brain& brain, const std::string& output) {
    try {
        // Generate weight matrix data
        std::string weightData = VisualizationUtils::formatWeightMatrix(brain);
        
        // Save as CSV
        std::ofstream file(output);
        if (file.is_open()) {
            file << weightData;
            file.close();
            return true;
        }
        
        return false;
    } catch (...) {
        return false;
    }
}

std::string VisualizationFramework::getStatistics() const {
    std::ostringstream oss;
    oss << "Visualization Statistics:" << std::endl;
    oss << "  Active: " << (isActive() ? "Yes" : "No") << std::endl;
    oss << "  Active Backend: " << getActiveBackend() << std::endl;
    oss << "  Mode: " << static_cast<int>(getMode()) << std::endl;
    oss << "  Update Interval: " << getUpdateInterval() << "s" << std::endl;
    oss << "  Data Points: " << pImpl->data.size() << std::endl;
    oss << "  Backends: " << pImpl->backends.size() << std::endl;
    
    for (const auto& pair : pImpl->backends) {
        oss << "    - " << pair.first << ": " << (pair.second->isActive() ? "Active" : "Inactive") << std::endl;
    }
    
    return oss.str();
}

std::vector<VisualizationData> VisualizationFramework::getData() const {
    return pImpl->data;
}

void VisualizationFramework::clearData() {
    pImpl->data.clear();
}

} // namespace nlm
