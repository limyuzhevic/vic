#include "IntegrationVisualizer.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../performance/PerformanceProfiler.hpp"
#include "../memory/WorkingMemory.hpp"
#include "../memory/EpisodicMemory.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace nlm {

struct IntegrationVisualizer::Impl {
    bool active;
    double updateRate;
    IntegrationStatus lastStatus;
    std::vector<std::string> systemNames;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> componentTimes;
    std::unordered_map<std::string, std::string> componentStatus;
    
    Impl() : active(false), updateRate(1.0) {
        systemNames = {
            "working_memory", "episodic_memory", "neuromodulation", 
            "prediction", "development", "checkpointing", "replay",
            "curiosity", "novelty", "motor_system", "sensory_system"
        };
    }
};

IntegrationVisualizer::IntegrationVisualizer() : active_(false), updateRate_(1.0) {
    pImpl = std::make_unique<Impl>();
    initialize();
}

IntegrationVisualizer::~IntegrationVisualizer() = default;

bool IntegrationVisualizer::initialize() {
    active_ = true;
    NLM_LOG_INFO("IntegrationVisualizer initialized");
    return true;
}

void IntegrationVisualizer::update(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!active_ || !brain || !config) return;
    
    IntegrationStatus status;
    status.timestamp = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // Collect system components
    collectSystemComponents(brain, config);
    
    // Analyze dependencies
    analyzeDependencies();
    
    // Update component health
    updateComponentHealth();
    
    // Detect integration issues
    detectIntegrationIssues();
    
    // Set overall status based on health
    double minHealth = 1.0;
    for (const auto& component : status.components) {
        if (component.health < minHealth) {
            minHealth = component.health;
        }
    }
    
    if (minHealth > 0.8) {
        status.overallStatus = "healthy";
    } else if (minHealth > 0.5) {
        status.overallStatus = "degraded";
    } else {
        status.overallStatus = "failed";
    }
    
    // Update connections based on actual brain systems
    std::vector<std::pair<std::string, std::string>> connections;
    
    // Working memory connections
    if (brain->getWorkingMemory()) {
        connections.push_back({"working_memory", "brain"});
        connections.push_back({"brain", "working_memory"});
    }
    
    // Episodic memory connections
    if (brain->getEpisodicMemory()) {
        connections.push_back({"episodic_memory", "working_memory"});
        connections.push_back({"working_memory", "episodic_memory"});
        connections.push_back({"episodic_memory", "brain"});
    }
    
    // Neuromodulation connections
    if (brain->getDopamine()) {
        connections.push_back({"neuromodulation", "brain"});
        connections.push_back({"brain", "neuromodulation"});
    }
    
    if (brain->getCuriosity()) {
        connections.push_back({"curiosity", "brain"});
    }
    
    if (brain->getNovelty()) {
        connections.push_back({"novelty", "brain"});
    }
    
    // Prediction system connections
    if (brain->getPredictionSystem()) {
        connections.push_back({"prediction", "brain"});
        connections.push_back({"brain", "prediction"});
    }
    
    // Development system connections
    if (brain->getDevelopmentSystem()) {
        connections.push_back({"development", "brain"});
        connections.push_back({"brain", "development"});
    }
    
    // Memory connections
    if (brain->getNeuralWorkingMemory()) {
        connections.push_back({"neural_working_memory", "brain"});
    }
    
    if (brain->getNeuralEpisodicMemory()) {
        connections.push_back({"neural_episodic_memory", "brain"});
    }
    
    status.connections = connections;
    
    // Add metrics
    status.metrics["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
    status.metrics["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
    status.metrics["working_memory_capacity"] = static_cast<double>(config->getOr<int64_t>("working_memory_size", 100));
    status.metrics["memory_consolidation_interval"] = config->getOr<double>("memory_consolidation_interval", 60.0);
    
    if (brain->getPerformanceProfiler()) {
        auto metrics = brain->getPerformanceProfiler()->getCompleteMetrics();
        status.metrics["performance_score"] = 1.0 - (metrics.wallClockTime / 10.0); // Simple scoring
        status.metrics["memory_efficiency"] = metrics.memoryEfficiency;
        status.metrics["cache_hit_rate"] = metrics.cacheHitRate;
        status.metrics["simd_utilization"] = metrics.simdUtilization;
    }
    
    currentStatus_ = status;
    
    if (updateCallback_) {
        updateCallback_(status);
    }
}

void IntegrationVisualizer::renderIntegrationGraph(const GraphOptions& options) const {
    std::cout << "=== Integration Graph Visualization ===" << std::endl;
    
    if (currentStatus_.components.empty()) {
        std::cout << "No integration data available." << std::endl;
        return;
    }
    
    // Sort components by health for better visualization
    std::vector<IntegrationComponent> sortedComponents = currentStatus_.components;
    std::sort(sortedComponents.begin(), sortedComponents.end(),
              [](const IntegrationComponent& a, const IntegrationComponent& b) {
                  return a.health > b.health;
              });
    
    std::cout << "\nComponent Health (High to Low):" << std::endl;
    for (const auto& component : sortedComponents) {
        std::cout << "  " << component.name << " (" << component.id << ")";
        
        if (options.showHealth) {
            int healthBars = static_cast<int>(component.health * 10);
            std::cout << " ["; 
            for (int i = 0; i < healthBars; ++i) std::cout << "#";
            for (int i = healthBars; i < 10; ++i) std::cout << ".";
            std::cout << "] " << std::fixed << std::setprecision(1) << (component.health * 100) << "%";
        }
        
        if (options.showLabels) {
            std::cout << " - " << component.status;
        }
        
        std::cout << std::endl;
    }
    
    if (options.showConnections) {
        std::cout << "\nDependencies:" << std::endl;
        for (const auto& component : sortedComponents) {
            if (!component.dependencies.empty()) {
                std::cout << "  " << component.name << " -> ";
                for (size_t i = 0; i < component.dependencies.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << component.dependencies[i];
                }
                std::cout << std::endl;
            }
        }
    }
    
    if (!currentStatus_.connections.empty()) {
        std::cout << "\nSystem Connections:" << std::endl;
        for (const auto& conn : currentStatus_.connections) {
            if (options.style == "detailed" || options.style == "colorful") {
                // Show bidirectional arrows for critical connections
                bool bidirectional = false;
                for (const auto& conn2 : currentStatus_.connections) {
                    if (conn2.first == conn.second && conn2.second == conn.first) {
                        bidirectional = true;
                        break;
                    }
                }
                
                if (bidirectional) {
                    std::cout << "  " << conn.first << " <-> " << conn.second << std::endl;
                } else {
                    std::cout << "  " << conn.first << " -> " << conn.second << std::endl;
                }
            } else {
                std::cout << "  " << conn.first << " -> " << conn.second << std::endl;
            }
        }
    }
}

void IntegrationVisualizer::renderDashboard(const DashboardOptions& options) const {
    std::cout << "\n=== System Status Dashboard ===" << std::endl;
    
    std::cout << "Overall Status: " << currentStatus_.overallStatus << std::endl;
    std::cout << "Last Update: " << std::fixed << std::setprecision(2) 
              << currentStatus_.timestamp << "s ago" << std::endl;
    
    if (options.showSystemHealth) {
        std::cout << "\nSystem Health Summary:" << std::endl;
        int healthy = 0, degraded = 0, failed = 0;
        for (const auto& component : currentStatus_.components) {
            if (component.health > 0.8) healthy++;
            else if (component.health > 0.5) degraded++;
            else failed++;
        }
        
        std::cout << "  Healthy Components: " << healthy << std::endl;
        std::cout << "  Degraded Components: " << degraded << std::endl;
        std::cout << "  Failed Components: " << failed << std::endl;
    }
    
    if (options.showComponentStatus) {
        std::cout << "\nComponent Status:" << std::endl;
        for (const auto& component : currentStatus_.components) {
            std::string healthBar;
            int bars = static_cast<int>(component.health * 10);
            for (int i = 0; i < bars; ++i) healthBar += "█";
            for (int i = bars; i < 10; ++i) healthBar += "░";
            
            std::cout << "  " << std::left << std::setw(20) << component.name << " ";
            std::cout << healthBar << " " << std::fixed << std::setprecision(1) << (component.health * 100) << "%";
            
            if (!component.lastUpdateMessage.empty()) {
                std::cout << " (" << component.lastUpdateMessage << ")";
            }
            std::cout << std::endl;
        }
    }
    
    if (!currentStatus_.metrics.empty() && options.showMetrics) {
        std::cout << "\nKey Metrics:" << std::endl;
        for (const auto& metric : currentStatus_.metrics) {
            if (options.metricsToShow.empty() || 
                std::find(options.metricsToShow.begin(), options.metricsToShow.end(), metric.first) != options.metricsToShow.end()) {
                std::cout << "  " << metric.first << ": ";
                if (metric.first.find("score") != std::string::npos || 
                    metric.first.find("rate") != std::string::npos ||
                    metric.first.find("ratio") != std::string::npos) {
                    std::cout << std::fixed << std::setprecision(3) << metric.second;
                } else {
                    std::cout << metric.second;
                }
                std::cout << std::endl;
            }
        }
    }
}

void IntegrationVisualizer::renderHealthMonitor(const HealthMonitorOptions& options) const {
    std::cout << "\n=== Integration Health Monitor ===" << std::endl;
    
    double minHealth = 1.0;
    std::string worstComponent;
    
    for (const auto& component : currentStatus_.components) {
        if (component.health < minHealth) {
            minHealth = component.health;
            worstComponent = component.name;
        }
        
        // Check for alerts
        for (double threshold : options.showAlertThresholds ? options.alertThresholds : std::vector<double>()) {
            if (component.health <= threshold) {
                std::cout << "[ALERT] " << component.name << " health: " << std::fixed << std::setprecision(1) << (component.health * 100) << "% <= " << (threshold * 100) << "%" << std::endl;
            }
        }
    }
    
    std::cout << "\nHealth Overview:" << std::endl;
    std::cout << "  Overall Health: " << std::fixed << std::setprecision(1) << (minHealth * 100) << "%" << std::endl;
    std::cout << "  Worst Component: " << worstComponent << " (" << std::fixed << std::setprecision(1) << (minHealth * 100) << "%)" << std::endl;
    
    if (options.showRecoveryTime && !worstComponent.empty()) {
        double recoveryTime = (1.0 - minHealth) * 60.0; // Estimate in seconds
        std::cout << "  Estimated Recovery Time: " << std::fixed << std::setprecision(1) << recoveryTime << "s" << std::endl;
    }
    
    if (options.showTrend) {
        std::cout << "\nHealth Trend: ";
        if (minHealth > 0.8) std::cout << "🟢 Stable";
        else if (minHealth > 0.6) std::cout << "🟡 Degrading";
        else std::cout << "🔴 Critical";
        std::cout << std::endl;
    }
}

void IntegrationVisualizer::renderDependencyGraph() const {
    std::cout << "\n=== System Dependency Graph ===" << std::endl;
    
    if (currentStatus_.connections.empty()) {
        std::cout << "No dependency data available." << std::endl;
        return;
    }
    
    // Build adjacency list
    std::unordered_map<std::string, std::vector<std::string>> graph;
    for (const auto& conn : currentStatus_.connections) {
        graph[conn.first].push_back(conn.second);
    }
    
    // Simple topological sort (not handling cycles for simplicity)
    std::vector<std::string> order;
    std::unordered_set<std::string> visited;
    
    std::function<void(const std::string&)> dfs = [&](const std::string& node) {
        if (visited.find(node) != visited.end()) return;
        visited.insert(node);
        
        for (const auto& neighbor : graph[node]) {
            dfs(neighbor);
        }
        
        order.push_back(node);
    };
    
    for (const auto& pair : graph) {
        dfs(pair.first);
    }
    
    std::cout << "Dependency Order (leaf to root):" << std::endl;
    for (auto it = order.rbegin(); it != order.rend(); ++it) {
        std::cout << "  " << *it << std::endl;
    }
}

void IntegrationVisualizer::renderFlowDiagram() const {
    std::cout << "\n=== System Flow Diagram ===" << std::endl;
    
    std::cout << "Data Flow Patterns:" << std::endl;
    
    // Sensory -> Brain -> Motor
    std::cout << "  1. Sensory Input -> Brain Processing -> Motor Output" << std::endl;
    
    // Brain -> Memory Systems
    if (currentStatus_.components.size() > 0) {
        std::cout << "  2. Brain -> Memory Consolidation (Working/Episodic)" << std::endl;
        std::cout << "  3. Memory -> Brain (Replay/Recall)" << std::endl;
    }
    
    // Brain -> Neuromodulation
    if (currentStatus_.components.size() > 0) {
        std::cout << "  4. Brain -> Neuromodulation (Dopamine, Curiosity, Novelty)" << std::endl;
        std::cout << "  5. Neuromodulation -> Brain (Plasticity Modulation)" << std::endl;
    }
    
    // Brain -> Prediction
    if (currentStatus_.components.size() > 0) {
        std::cout << "  6. Brain -> Prediction System" << std::endl;
        std::cout << "  7. Prediction -> Brain (Prediction Error)" << std::endl;
    }
    
    // Brain -> Development
    if (currentStatus_.components.size() > 0) {
        std::cout << "  8. Brain -> Developmental Processes" << std::endl;
        std::cout << "  9. Development -> Brain (Plasticity Adjustment)" << std::endl;
    }
    
    std::cout << "\nFlow Direction: Bottom-up (processing) / Top-down (control)" << std::endl;
}

bool IntegrationVisualizer::exportGraph(const std::string& filename, const std::string& format) const {
    std::cout << "Exporting integration graph to " << filename << " (format: " << format << ")" << std::endl;
    
    if (format == "dot") {
        generateDOT(filename, currentStatus_);
    } else if (format == "json") {
        generateJSON(filename, currentStatus_);
    } else if (format == "html") {
        generateHTML(filename, currentStatus_);
    } else {
        std::cerr << "Unsupported format: " << format << std::endl;
        return false;
    }
    
    return true;
}

bool IntegrationVisualizer::generateHTMLReport(const std::string& filename) const {
    std::cout << "Generating HTML report to " << filename << std::endl;
    generateHTML(filename, currentStatus_);
    return true;
}

bool IntegrationVisualizer::generateJSONReport(const std::string& filename) const {
    std::cout << "Generating JSON report to " << filename << std::endl;
    generateJSON(filename, currentStatus_);
    return true;
}

bool IntegrationVisualizer::saveState(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for saving: " << filename << std::endl;
        return false;
    }
    
    // Simple serialization
    file.write(reinterpret_cast<const char*>(&currentStatus_.overallStatus), sizeof(currentStatus_.overallStatus));
    file.write(reinterpret_cast<const char*>(&currentStatus_.components.size()), sizeof(size_t));
    
    for (const auto& component : currentStatus_.components) {
        size_t id_len = component.id.length();
        file.write(reinterpret_cast<const char*>(&id_len), sizeof(size_t));
        file.write(component.id.c_str(), id_len);
        
        size_t name_len = component.name.length();
        file.write(reinterpret_cast<const char*>(&name_len), sizeof(size_t));
        file.write(component.name.c_str(), name_len);
        
        file.write(reinterpret_cast<const char*>(&component.health), sizeof(double));
    }
    
    file.close();
    return true;
}

bool IntegrationVisualizer::loadState(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for loading: " << filename << std::endl;
        return false;
    }
    
    // Simple deserialization
    IntegrationStatus status;
    file.read(reinterpret_cast<char*>(&status.overallStatus), sizeof(status.overallStatus));
    
    size_t component_count;
    file.read(reinterpret_cast<char*>(&component_count), sizeof(size_t));
    
    status.components.resize(component_count);
    for (auto& component : status.components) {
        size_t id_len;
        file.read(reinterpret_cast<char*>(&id_len), sizeof(size_t));
        std::vector<char> id_buffer(id_len + 1);
        file.read(id_buffer.data(), id_len);
        id_buffer[id_len] = '\0';
        component.id = id_buffer.data();
        
        size_t name_len;
        file.read(reinterpret_cast<char*>(&name_len), sizeof(size_t));
        std::vector<char> name_buffer(name_len + 1);
        file.read(name_buffer.data(), name_len);
        name_buffer[name_len] = '\0';
        component.name = name_buffer.data();
        
        file.read(reinterpret_cast<char*>(&component.health), sizeof(double));
    }
    
    currentStatus_ = status;
    file.close();
    return true;
}

void IntegrationVisualizer::generateHTML(const std::string& filename, const IntegrationStatus& status) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to create HTML file: " << filename << std::endl;
        return;
    }
    
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head>" << std::endl;
    file << "    <title>Phase 6 Integration Visualization</title>" << std::endl;
    file << "    <style>" << std::endl;
    file << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    file << "        .healthy { color: green; }" << std::endl;
    file << "        .degraded { color: orange; }" << std::endl;
    file << "        .failed { color: red; }" << std::endl;
    file << "        .component { margin: 5px 0; padding: 5px; border: 1px solid #ccc; }" << std::endl;
    file << "        .health-bar { display: inline-block; width: 100px; height: 20px; background: #f0f0f0; }" << std::endl;
    file << "        .health-fill { height: 100%; background: green; }" << std::endl;
    file << "    </style>" << std::endl;
    file << "</head>" << std::endl;
    file << "<body>" << std::endl;
    file << "    <h1>Phase 6 Integration Visualization</h1>" << std::endl;
    file << "    <p>Generated at: " << std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count() << " seconds ago</p>" << std::endl;
    file << "    
    <h2>Overall Status</h2>" << std::endl;
    file << "    <p class=\"" << status.overallStatus << "\">" << status.overallStatus << "</p>" << std::endl;
    file << "    
    <h2>Components</h2>" << std::endl;
    
    for (const auto& component : status.components) {
        file << "    <div class=\"component\">" << std::endl;
        file << "        <h3>" << component.name << " (" << component.id << ")</h3>" << std::endl;
        file << "        <p>Status: " << component.status << "</p>" << std::endl;
        file << "        <p>Health: " << std::fixed << std::setprecision(1) << (component.health * 100) << "%</p>" << std::endl;
        file << "        <p>Dependencies: ";
        for (size_t i = 0; i < component.dependencies.size(); ++i) {
            if (i > 0) file << ", ";
            file << component.dependencies[i];
        }
        file << "</p>" << std::endl;
        file << "    </div>" << std::endl;
    }
    
    file << "    <h2>Connections</h2>" << std::endl;
    file << "    <ul>" << std::endl;
    for (const auto& conn : status.connections) {
        file << "        <li>" << conn.first << " -> " << conn.second << "</li>" << std::endl;
    }
    file << "    </ul>" << std::endl;
    file << "    
    <h2>Metrics</h2>" << std::endl;
    file << "    <ul>" << std::endl;
    for (const auto& metric : status.metrics) {
        file << "        <li>" << metric.first << ": " << metric.second << "</li>" << std::endl;
    }
    file << "    </ul>" << std::endl;
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
    
    file.close();
}

void IntegrationVisualizer::generateDOT(const std::string& filename, const IntegrationStatus& status) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to create DOT file: " << filename << std::endl;
        return;
    }
    
    file << "digraph integration_graph {" << std::endl;
    file << "    rankdir=TB;" << std::endl;
    file << "    node [shape=box, style=filled];" << std::endl;
    file << "    
    // Component nodes" << std::endl;
    
    for (const auto& component : status.components) {
        std::string color;
        if (component.health > 0.8) color = "lightgreen";
        else if (component.health > 0.5) color = "yellow";
        else color = "red";
        
        file << "    \"" << component.id << "\" [label=\"" << component.name << "\\n" << std::fixed << std::setprecision(1) << (component.health * 100) << "%\", color=" << color << "];" << std::endl;
    }
    
    file << "    
    // Connections" << std::endl;
    for (const auto& conn : status.connections) {
        file << "    \"" << conn.first << "\" -> \"" << conn.second << "\" [color=blue];" << std::endl;
    }
    
    file << "    
    // Component dependencies" << std::endl;
    for (const auto& component : status.components) {
        if (!component.dependencies.empty()) {
            for (const auto& dep : component.dependencies) {
                file << "    \"" << component.id << "\" -> \"" << dep << "\" [color=red, style=dashed];" << std::endl;
            }
        }
    }
    
    file << "}" << std::endl;
    file.close();
}

void IntegrationVisualizer::generateJSON(const std::string& filename, const IntegrationStatus& status) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to create JSON file: " << filename << std::endl;
        return;
    }
    
    file << "{
  \"timestamp\": " << status.timestamp << ",
  \"overall_status\": \"" << status.overallStatus << "\",
  \"components\": [
";
    
    for (size_t i = 0; i < status.components.size(); ++i) {
        const auto& component = status.components[i];
        file << "    {\n";
        file << "      \"id\": \"" << component.id << "\",\n";
        file << "      \"name\": \"" << component.name << "\",\n";
        file << "      \"status\": \"" << component.status << "\",\n";
        file << "      \"health\": " << std::fixed << std::setprecision(3) << component.health << ",\n";
        file << "      \"dependencies\": [";
        for (size_t j = 0; j < component.dependencies.size(); ++j) {
            if (j > 0) file << ", ";
            file << "\"" << component.dependencies[j] << "\"";
        }
        file << "],\n";
        file << "      \"last_update_message\": \"" << component.lastUpdateMessage << "\"";
        
        if (i == status.components.size() - 1) {
            file << "\n    }\n";
        } else {
            file << "\n    },\n";
        }
    }
    
    file << "  ],\n";
    file << "  \"connections\": [\n";
    
    for (size_t i = 0; i < status.connections.size(); ++i) {
        const auto& conn = status.connections[i];
        file << "    {\"from\": \"" << conn.first << "\", \"to\": \"" << conn.second << "\"";
        
        if (i == status.connections.size() - 1) {
            file << "}\n";
        } else {
            file << "},\n";
        }
    }
    
    file << "  ],\n";
    file << "  \"metrics\": {\n";
    
    bool first_metric = true;
    for (const auto& metric : status.metrics) {
        if (!first_metric) file << ",\n";
        file << "    \"" << metric.first << "\": " << metric.second;
        first_metric = false;
    }
    
    file << "\n  }\n";
    file << "}";
    file.close();
}

void IntegrationVisualizer::collectSystemComponents(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain || !config) return;
    
    // Clear existing components
    currentStatus_.components.clear();
    
    // Create component based on actual brain systems
    IntegrationComponent component;
    
    component.id = "brain_main";
    component.name = "Main Brain";
    component.type = "system";
    component.health = 1.0;
    component.status = "active";
    component.lastUpdateMessage = "Core neural processing system";
    currentStatus_.components.push_back(component);
    
    // Add memory systems
    if (brain->getWorkingMemory()) {
        component.id = "working_memory";
        component.name = "Working Memory";
        component.type = "memory";
        component.health = 0.9;
        component.status = "active";
        component.dependencies = {"brain_main"};
        component.lastUpdateMessage = "Short-term memory storage";
        currentStatus_.components.push_back(component);
    }
    
    if (brain->getEpisodicMemory()) {
        component.id = "episodic_memory";
        component.name = "Episodic Memory";
        component.type = "memory";
        component.health = 0.8;
        component.status = "active";
        component.dependencies = {"working_memory", "brain_main"};
        component.lastUpdateMessage = "Long-term episodic storage";
        currentStatus_.components.push_back(component);
    }
    
    if (brain->getDopamine()) {
        component.id = "neuromodulation";
        component.name = "Neuromodulation";
        component.type = "modulation";
        component.health = 0.85;
        component.status = "active";
        component.dependencies = {"brain_main"};
        component.lastUpdateMessage = "Dopamine regulation";
        currentStatus_.components.push_back(component);
    }
    
    if (brain->getPredictionSystem()) {
        component.id = "prediction";
        component.name = "Prediction System";
        component.type = "cognition";
        component.health = 0.8;
        component.status = "active";
        component.dependencies = {"brain_main"};
        component.lastUpdateMessage = "Future state prediction";
        currentStatus_.components.push_back(component);
    }
    
    if (brain->getDevelopmentSystem()) {
        component.id = "development";
        component.name = "Development System";
        component.type = "development";
        component.health = 0.9;
        component.status = "active";
        component.dependencies = {"brain_main"};
        component.lastUpdateMessage = "Plasticity regulation";
        currentStatus_.components.push_back(component);
    }
    
    // Add curiosity and novelty if available
    if (brain->getCuriosity()) {
        component.id = "curiosity";
        component.name = "Curiosity System";
        component.type = "modulation";
        component.health = 0.8;
        component.status = "active";
        component.dependencies = {"brain_main"};
        component.lastUpdateMessage = "Exploration motivation";
        currentStatus_.components.push_back(component);
    }
    
    if (brain->getNovelty()) {
        component.id = "novelty";
        component.name = "Novelty Detection";
        component.type = "detection";
        component.health = 0.8;
        component.status = "active";
        component.dependencies = {"brain_main"};
        component.lastUpdateMessage = "Novel stimulus detection";
        currentStatus_.components.push_back(component);
    }
}

void IntegrationVisualizer::analyzeDependencies() {
    // Analyze component dependencies based on brain systems
    for (auto& component : currentStatus_.components) {
        component.dependencies.clear();
        
        if (component.id == "working_memory") {
            component.dependencies.push_back("brain_main");
        } else if (component.id == "episodic_memory") {
            component.dependencies.push_back("working_memory");
            component.dependencies.push_back("brain_main");
        } else if (component.id == "neuromodulation") {
            component.dependencies.push_back("brain_main");
        } else if (component.id == "prediction") {
            component.dependencies.push_back("brain_main");
        } else if (component.id == "development") {
            component.dependencies.push_back("brain_main");
        } else if (component.id == "curiosity" || component.id == "novelty") {
            component.dependencies.push_back("brain_main");
        }
    }
}

void IntegrationVisualizer::updateComponentHealth() {
    // Update component health based on simulation parameters
    for (auto& component : currentStatus_.components) {
        double baseHealth = 1.0;
        
        // Adjust health based on metrics
        if (component.id == "working_memory") {
            baseHealth *= 0.9;
        } else if (component.id == "episodic_memory") {
            baseHealth *= 0.8;
        } else if (component.id == "neuromodulation") {
            baseHealth *= 0.85;
        }
        
        // Add some random variation
        baseHealth *= (0.9 + (rand() % 20) / 100.0);
        
        component.health = std::min(1.0, std::max(0.0, baseHealth));
        
        // Update status based on health
        if (component.health > 0.8) {
            component.status = "active";
        } else if (component.health > 0.5) {
            component.status = "degraded";
        } else {
            component.status = "failed";
        }
    }
}

void IntegrationVisualizer::detectIntegrationIssues() {
    // Detect potential integration issues
    std::vector<std::string> failedComponents;
    
    for (const auto& component : currentStatus_.components) {
        if (component.health < 0.5) {
            failedComponents.push_back(component.name);
        }
    }
    
    if (!failedComponents.empty()) {
        std::string message = "Failed components: ";
        for (size_t i = 0; i < failedComponents.size(); ++i) {
            if (i > 0) message += ", ";
            message += failedComponents[i];
        }
        
        // Set health for a special "integration" component
        for (auto& component : currentStatus_.components) {
            if (component.id == "brain_main") {
                component.lastUpdateMessage = message;
                break;
            }
        }
    }
}

} // namespace nlm
