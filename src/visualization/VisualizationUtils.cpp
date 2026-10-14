#include "VisualizationFramework.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cmath>

namespace nlm {
namespace VisualizationUtils {

std::string formatSpikeData(const std::vector<SpikeEvent>& spikes, const Brain& brain) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
    
    for (const auto& spike : spikes) {
        oss << spike.time << "," << spike.neuronId << "," << spike.amplitude << std::endl;
    }
    
    return oss.str();
}

std::string formatNetworkTopology(const Brain& brain) {
    std::ostringstream oss;
    oss << "NeuronCount," << brain.getTotalNeuronCount() << std::endl;
    oss << "SynapseCount," << brain.getTotalSynapseCount() << std::endl;
    oss << "RegionCount," << brain.getRegionCount() << std::endl;
    
    // Add detailed information
    oss << "NeuronTypes,";
    bool first = true;
    for (const auto& type : {NeuronType::Sensory, NeuronType::Internal, 
                             NeuronType::Motor, NeuronType::Output}) {
        if (!first) oss << ";";
        oss << static_cast<int>(type);
        first = false;
    }
    oss << std::endl;
    
    return oss.str();
}

std::string formatActivityData(const Brain& brain, size_t timeStep) {
    std::ostringstream oss;
    oss << "TimeStep," << timeStep << std::endl;
    oss << "FiringRate," << brain.getFiringRate() << std::endl;
    oss << "ActiveNeurons," << brain.getFiringNeuronCount() << std::endl;
    oss << "TotalSpikes," << brain.getTotalSpikeCount() << std::endl;
    
    return oss.str();
}

std::string formatWeightMatrix(const Brain& brain) {
    std::ostringstream oss;
    oss << "Source,Destination,Weight" << std::endl;
    
    // In a real implementation, this would extract all synapses and their weights
    // For now, return empty with headers
    
    return oss.str();
}

std::string generateHTMLVisualization(const std::string& title, const std::string& data) {
    std::ostringstream html;
    html << "<!DOCTYPE html>" << std::endl;
    html << "<html>" << std::endl;
    html << "<head>" << std::endl;
    html << "<title>" << title << "</title>" << std::endl;
    html << "<style>" << std::endl;
    html << "body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    html << "h1 { color: #333; }" << std::endl;
    html << "table { border-collapse: collapse; width: 100%; }" << std::endl;
    html << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
    html << "th { background-color: #f2f2f2; }" << std::endl;
    html << "</style>" << std::endl;
    html << "</head>" << std::endl;
    html << "<body>" << std::endl;
    html << "<h1>" << title << "</h1>" << std::endl;
    html << "<pre>" << std::endl;
    html << data << std::endl;
    html << "</pre>" << std::endl;
    html << "</body>" << std::endl;
    html << "</html>" << std::endl;
    
    return html.str();
}

std::string generateCSVVisualization(const std::string& title, const std::string& data) {
    std::ostringstream csv;
    csv << "# " << title << std::endl;
    csv << data;
    return csv.str();
}

std::string generateConsoleVisualization(const std::string& title, const std::string& data) {
    std::ostringstream console;
    console << "=== " << title << " ===" << std::endl;
    console << data << std::endl;
    return console.str();
}

bool saveVisualization(const VisualizationData& data, const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << data.content;
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

VisualizationData loadVisualization(const std::string& filepath) {
    VisualizationData data;
    data.format = "unknown";
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return data;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        data.content = buffer.str();
        file.close();
        
        // Determine format based on file extension
        if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".csv") {
            data.format = "csv";
        } else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
            data.format = "json";
        } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".xml") {
            data.format = "xml";
        } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".html") {
            data.format = "html";
        }
        
    } catch (...) {
        // Return empty data on error
    }
    
    return data;
}

std::string generateProgressReport(const ProgressTracker& progress) {
    std::ostringstream oss;
    oss << "=== " << progress.operation << " ===" << std::endl;
    oss << progress.getProgressString() << std::endl;
    oss << "Estimated time remaining: " << progress.getEstimatedTimeString() << std::endl;
    return oss.str();
}

std::string createColorScheme(const std::string& scheme) {
    if (scheme == "default") {
        return "#636e72, #ffeaa7, #55a3d1, #ff7675, #fdcb6e, #a29bfe";
    } else if (scheme == "dark") {
        return "#2d3436, #636e72, #b2bec3, #dfe6e9, #fdcb6e, #a29bfe";
    } else if (scheme == "light") {
        return "#f9f9f9, #dfe6e9, #b2bec3, #636e72, #2d3436, #000000";
    }
    return "";
}

std::string applyStyle(const std::string& style, const std::string& data) {
    if (style == "compact") {
        return "  " + data;  // Add indentation
    } else if (style == "boxed") {
        return "+" + std::string(data.size() + 2, '-') + "+\n| " + data + " |\n+" + std::string(data.size() + 2, '-') + "+";
    }
    return data;
}

bool validateVisualizationData(const VisualizationData& data) {
    return !data.content.empty() && !data.format.empty();
}

std::string generateMetadata(const VisualizationData& data, const Brain& brain) {
    std::ostringstream oss;
    oss << "Generated at: " << std::chrono::system_clock::to_time_t(data.timestamp) << std::endl;
    oss << "Format: " << data.format << std::endl;
    oss << "Content length: " << data.content.size() << " characters" << std::endl;
    oss << "Brain neurons: " << brain.getTotalNeuronCount() << std::endl;
    oss << "Brain synapses: " << brain.getTotalSynapseCount() << std::endl;
    return oss.str();
}

} // namespace VisualizationUtils

} // namespace nlm
