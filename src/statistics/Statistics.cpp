// NLM (熙然) - Neural Learning Machine
// Statistics Export Module Implementation
// Phase 2: Real Neural Computation

#include "statistics/Statistics.hpp"
#include <iomanip>
#include <ctime>
#include <sstream>

namespace nlm {

bool StatisticsExporter::exportToFile(const NeuralStatistics& stats, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filepath << std::endl;
        return false;
    }
    
    bool success = exportToJSON(stats, file);
    file.close();
    return success;
}

// Simple JSON serialization without external dependencies
static void escapeString(std::ostream& output, const std::string& str) {
    output << "\"";
    for (char c : str) {
        switch (c) {
            case '"': output << "\\\""; break;
            case '\\': output << "\\\\"; break;
            case '\b': output << "\\b"; break;
            case '\f': output << "\\f"; break;
            case '\n': output << "\\n"; break;
            case '\r': output << "\\r"; break;
            case '\t': output << "\\t"; break;
            default:
                if (c >= ' ' && c <= '~') {
                    output << c;
                } else {
                    // Simple escape for non-printable characters
                    output << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                }
                break;
        }
    }
    output << "\"";
}

// Simple JSON serialization without external dependencies
static void escapeString(std::ostream& output, const std::string& str) {
    output << "\"";
    for (char c : str) {
        switch (c) {
            case '"': output << "\\\""; break;
            case '\\': output << "\\\\"; break;
            case '\b': output << "\\b"; break;
            case '\f': output << "\\f"; break;
            case '\n': output << "\\n"; break;
            case '\r': output << "\\r"; break;
            case '\t': output << "\\t"; break;
            default:
                if (c >= ' ' && c <= '~') {
                    output << c;
                } else {
                    // Simple escape for non-printable characters
                    output << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                }
                break;
        }
    }
    output << "\"";
}

bool StatisticsExporter::exportToJSON(const NeuralStatistics& stats, std::ostream& output) const {
    try {
        output << "{" << std::endl;
        output << "  \"neural_statistics\": {" << std::endl;
        output << "    \"total_spikes\": " << stats.totalSpikes << "," << std::endl;
        output << "    \"active_neurons\": " << stats.activeNeurons << "," << std::endl;
        output << "    \"firing_neuron_count\": " << stats.firingNeuronCount << "," << std::endl;
        output << "    \"average_firing_rate\": " << stats.averageFiringRate << "," << std::endl;
        output << "    \"excitation_inhibition_ratio\": " << stats.excitationInhibitionRatio << std::endl;
        output << "  }," << std::endl;
        output << "  \"plasticity_statistics\": {" << std::endl;
        output << "    \"strengthened_synapses\": " << stats.strengthenedSynapses << "," << std::endl;
        output << "    \"weakened_synapses\": " << stats.weakenedSynapses << "," << std::endl;
        output << "    \"unchanged_synapses\": " << stats.unchangedSynapses << "," << std::endl;
        output << "    \"weight_change_mean\": " << stats.weightChangeMean << "," << std::endl;
        output << "    \"weight_change_std_dev\": " << stats.weightChangeStdDev << std::endl;
        output << "  }," << std::endl;
        output << "  \"learning_statistics\": {" << std::endl;
        output << "    \"learning_occurred\": " << (stats.learningOccurred ? "true" : "false") << "," << std::endl;
        output << "    \"learning_type\": \"" << stats.learningType << "\"" << std::endl;
        output << "  }," << std::endl;
        output << "  \"memory_statistics\": {" << std::endl;
        output << "    \"memory_capacity\": " << stats.memoryCapacity << "," << std::endl;
        output << "    \"memory_utilization\": " << stats.memoryUtilization << std::endl;
        output << "  }," << std::endl;
        output << "  \"timing_statistics\": {" << std::endl;
        output << "    \"simulation_time\": " << stats.simulationTime << "," << std::endl;
        output << "    \"wall_clock_time\": " << stats.wallClockTime << std::endl;
        output << "  }" << std::endl;
        
        if (!stats.customMetrics.empty()) {
            output << "," << std::endl;
            output << "  \"custom_metrics\": {" << std::endl;
            bool first = true;
            for (const auto& pair : stats.customMetrics) {
                if (!first) output << "," << std::endl;
                output << "    \"" << pair.first << "\": " << pair.second;
                first = false;
            }
            output << std::endl;
            output << "  }" << std::endl;
        }
        
        output << "}" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting to JSON: " << e.what() << std::endl;
        return false;
    }
}

bool StatisticsExporter::exportToCSV(const NeuralStatistics& stats, std::ostream& output) const {
    try {
        // Header
        output << "Metric,Value" << std::endl;
        
        // Basic metrics
        output << "total_spikes," << stats.totalSpikes << std::endl;
        output << "active_neurons," << stats.activeNeurons << std::endl;
        output << "firing_neuron_count," << stats.firingNeuronCount << std::endl;
        output << "average_firing_rate," << stats.averageFiringRate << std::endl;
        output << "excitation_inhibition_ratio," << stats.excitationInhibitionRatio << std::endl;
        
        // Plasticity metrics
        output << "strengthened_synapses," << stats.strengthenedSynapses << std::endl;
        output << "weakened_synapses," << stats.weakenedSynapses << std::endl;
        output << "unchanged_synapses," << stats.unchangedSynapses << std::endl;
        output << "weight_change_mean," << stats.weightChangeMean << std::endl;
        output << "weight_change_std_dev," << stats.weightChangeStdDev << std::endl;
        
        // Learning metrics
        output << "learning_occurred," << (stats.learningOccurred ? "true" : "false") << std::endl;
        output << "learning_type," << stats.learningType << std::endl;
        output << "learning_score," << stats.learningScore << std::endl;
        
        // Memory metrics
        output << "memory_capacity," << stats.memoryCapacity << std::endl;
        output << "memory_utilization," << stats.memoryUtilization << std::endl;
        
        // Timing metrics
        output << "simulation_time," << stats.simulationTime << std::endl;
        output << "wall_clock_time," << stats.wallClockTime << std::endl;
        
        // Custom metrics
        for (const auto& pair : stats.customMetrics) {
            output << pair.first << "," << pair.second << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting to CSV: " << e.what() << std::endl;
        return false;
    }
}

bool StatisticsExporter::exportToHTML(const NeuralStatistics& stats, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filepath << std::endl;
        return false;
    }
    
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head><title>NLM Statistics Report</title>" << std::endl;
    file << "<style>body { font-family: Arial, sans-serif; margin: 20px; } "
         << "table { border-collapse: collapse; width: 100%; } "
         << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; } "
         << "th { background-color: #f2f2f2; } "
         << "tr:nth-child(even) { background-color: #f9f9f9; }</style></head>" << std::endl;
    file << "<body><h1>NLM Neural Statistics Report</h1>" << std::endl;
    
    file << "<h2>Neural Statistics</h2><table>" << std::endl;
    file << "<tr><th>Metric</th><th>Value</th></tr>" << std::endl;
    file << "<tr><td>Total Spikes</td><td>" << stats.totalSpikes << "</td></tr>" << std::endl;
    file << "<tr><td>Active Neurons</td><td>" << stats.activeNeurons << "</td></tr>" << std::endl;
    file << "<tr><td>Firing Neuron Count</td><td>" << stats.firingNeuronCount << "</td></tr>" << std::endl;
    file << "<tr><td>Average Firing Rate</td><td>" << stats.averageFiringRate << "</td></tr>" << std::endl;
    file << "<tr><td>Excitation/Inhibition Ratio</td><td>" << stats.excitationInhibitionRatio << "</td></tr>" << std::endl;
    file << "</table>" << std::endl;
    
    file << "<h2>Plasticity Statistics</h2><table>" << std::endl;
    file << "<tr><th>Metric</th><th>Value</th></tr>" << std::endl;
    file << "<tr><td>Strengthened Synapses</td><td>" << stats.strengthenedSynapses << "</td></tr>" << std::endl;
    file << "<tr><td>Weakened Synapses</td><td>" << stats.weakenedSynapses << "</td></tr>" << std::endl;
    file << "<tr><td>Unchanged Synapses</td><td>" << stats.unchangedSynapses << "</td></tr>" << std::endl;
    file << "<tr><td>Weight Change Mean</td><td>" << stats.weightChangeMean << "</td></tr>" << std::endl;
    file << "<tr><td>Weight Change Std Dev</td><td>" << stats.weightChangeStdDev << "</td></tr>" << std::endl;
    file << "</table>" << std::endl;
    
    file << "<h2>Learning Statistics</h2><table>" << std::endl;
    file << "<tr><th>Metric</th><th>Value</th></tr>" << std::endl;
    file << "<tr><td>Learning Occurred</td><td>" << (stats.learningOccurred ? "Yes" : "No") << "</td></tr>" << std::endl;
    file << "<tr><td>Learning Type</td><td>" << stats.learningType << "</td></tr>" << std::endl;
    file << "<tr><td>Learning Score</td><td>" << stats.learningScore << "</td></tr>" << std::endl;
    file << "</table>" << std::endl;
    
    file << "<h2>Memory Statistics</h2><table>" << std::endl;
    file << "<tr><th>Metric</th><th>Value</th></tr>" << std::endl;
    file << "<tr><td>Memory Capacity</td><td>" << stats.memoryCapacity << "</td></tr>" << std::endl;
    file << "<tr><td>Memory Utilization</td><td>" << stats.memoryUtilization << "</td></tr>" << std::endl;
    file << "</table>" << std::endl;
    
    file << "<h2>Timing Statistics</h2><table>" << std::endl;
    file << "<tr><th>Metric</th><th>Value</th></tr>" << std::endl;
    file << "<tr><td>Simulation Time</td><td>" << stats.simulationTime << "s</td></tr>" << std::endl;
    file << "<tr><td>Wall Clock Time</td><td>" << stats.wallClockTime << "s</td></tr>" << std::endl;
    file << "</table>" << std::endl;
    
    if (!stats.customMetrics.empty()) {
        file << "<h2>Custom Metrics</h2><table>" << std::endl;
        file << "<tr><th>Metric</th><th>Value</th></tr>" << std::endl;
        for (const auto& pair : stats.customMetrics) {
            file << "<tr><td>" << pair.first << "</td><td>" << pair.second << "</td></tr>" << std::endl;
        }
        file << "</table>" << std::endl;
    }
    
    file << "</body></html>" << std::endl;
    file.close();
    return true;
}

bool StatisticsExporter::addCustomMetric(const std::string& name, float value) {
    customMetrics_[name] = value;
    return true;
}

bool StatisticsExporter::removeCustomMetric(const std::string& name) {
    auto it = customMetrics_.find(name);
    if (it != customMetrics_.end()) {
        customMetrics_.erase(it);
        return true;
    }
    return false;
}

float StatisticsExporter::getCustomMetric(const std::string& name) const {
    auto it = customMetrics_.find(name);
    return (it != customMetrics_.end()) ? it->second : 0.0f;
}

std::map<std::string, float> StatisticsExporter::getAllCustomMetrics() const {
    return customMetrics_;
}

std::string StatisticsExporter::generateReport(const NeuralStatistics& stats) const {
    std::stringstream ss;
    ss << "=== NLM Statistics Report ===" << std::endl;
    ss << "Total Spikes: " << stats.totalSpikes << std::endl;
    ss << "Active Neurons: " << stats.activeNeurons << std::endl;
    ss << "Firing Rate: " << stats.averageFiringRate << std::endl;
    ss << "Learning: " << (stats.learningOccurred ? "YES" : "NO") << std::endl;
    return ss.str();
}

std::string StatisticsExporter::generateDetailedReport(const NeuralStatistics& stats) const {
    std::stringstream ss;
    ss << "=== Detailed NLM Statistics Report ===" << std::endl;
    ss << std::endl;
    ss << "NEURAL STATISTICS:" << std::endl;
    ss << "  Total Spikes: " << stats.totalSpikes << std::endl;
    ss << "  Active Neurons: " << stats.activeNeurons << std::endl;
    ss << "  Firing Neuron Count: " << stats.firingNeuronCount << std::endl;
    ss << "  Average Firing Rate: " << stats.averageFiringRate << std::endl;
    ss << "  Excitation/Inhibition Ratio: " << stats.excitationInhibitionRatio << std::endl;
    ss << std::endl;
    ss << "PLASTICITY STATISTICS:" << std::endl;
    ss << "  Strengthened Synapses: " << stats.strengthenedSynapses << std::endl;
    ss << "  Weakened Synapses: " << stats.weakenedSynapses << std::endl;
    ss << "  Unchanged Synapses: " << stats.unchangedSynapses << std::endl;
    ss << "  Weight Change Mean: " << stats.weightChangeMean << std::endl;
    ss << "  Weight Change Std Dev: " << stats.weightChangeStdDev << std::endl;
    ss << std::endl;
    ss << "LEARNING STATISTICS:" << std::endl;
    ss << "  Learning Occurred: " << (stats.learningOccurred ? "YES" : "NO") << std::endl;
    ss << "  Learning Type: " << stats.learningType << std::endl;
    ss << "  Learning Score: " << stats.learningScore << std::endl;
    ss << std::endl;
    ss << "MEMORY STATISTICS:" << std::endl;
    ss << "  Memory Capacity: " << stats.memoryCapacity << std::endl;
    ss << "  Memory Utilization: " << stats.memoryUtilization << std::endl;
    ss << std::endl;
    ss << "TIMING STATISTICS:" << std::endl;
    ss << "  Simulation Time: " << stats.simulationTime << "s" << std::endl;
    ss << "  Wall Clock Time: " << stats.wallClockTime << "s" << std::endl;
    
    if (!stats.customMetrics.empty()) {
        ss << std::endl;
        ss << "CUSTOM METRICS:" << std::endl;
        for (const auto& pair : stats.customMetrics) {
            ss << "  " << pair.first << ": " << pair.second << std::endl;
        }
    }
    
    return ss.str();
}

} // namespace nlm