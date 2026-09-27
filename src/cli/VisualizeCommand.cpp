#include "cli/VisualizeCommand.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cmath>

using json = nlohmann::json;

using namespace nlm;
namespace cli {

VisualizeCommand::VisualizeCommand() 
    : CommandBase("visualize", "Create visualizations of brain activity"),
      data_file_(""),
      output_format_("png"),
      visualization_type_("spikes") {
}

bool VisualizeCommand::execute(const std::map<std::string, std::string>& args) {
    std::cout << "Creating visualization: " << visualization_type_ << std::endl;
    std::cout << "Data file: " << data_file_ << std::endl;
    std::cout << "Output format: " << output_format_ << std::endl;
    
    if (data_file_.empty()) {
        std::cerr << "Error: No data file specified" << std::endl;
        return false;
    }
    
    // Load data
    json data = loadDataFile(data_file_);
    
    // Create visualization based on type
    bool success = false;
    if (visualization_type_ == "spikes") {
        success = createSpikeVisualization(data, output_format_);
    } else if (visualization_type_ == "activity") {
        success = createActivityVisualization(data, output_format_);
    } else if (visualization_type_ == "connections") {
        success = createConnectionVisualization(data, output_format_);
    } else if (visualization_type_ == "weights") {
        success = createWeightVisualization(data, output_format_);
    } else if (visualization_type_ == "heatmap") {
        success = createHeatmapVisualization(data, output_format_);
    } else if (visualization_type_ == "timeline") {
        success = createTimelineVisualization(data, output_format_);
    } else if (visualization_type_ == "network") {
        success = createNetworkVisualization(data, output_format_);
    }
    
    if (success) {
        std::cout << "Visualization created successfully!" << std::endl;
    }
    
    return success;
}

void VisualizeCommand::printHelp() const {
    std::cout << "Usage: nlm-cli visualize <data_file> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Create visualizations from data files." << std::endl;
    std::cout << std::endl;
    std::cout << "Arguments:" << std::endl;
    std::cout << "  <data_file>          Data file to visualize (.json, .csv)" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -f, --format <fmt>    Output image format" << std::endl;
    std::cout << "  -t, --type <type>     Visualization type" << std::endl;
    std::cout << "  -q, --quiet           Suppress console output" << std::endl;
    std::cout << "  -v, --verbose         Enable verbose logging" << std::endl;
}

void VisualizeCommand::printOptions() const {
    std::cout << "  [command options]" << std::endl;
}

json VisualizeCommand::loadDataFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open data file: " + filepath);
    }
    
    json data;
    try {
        file >> data;
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid data file format: " + std::string(e.what()));
    }
    
    file.close();
    return data;
}

bool VisualizeCommand::createSpikeVisualization(const json& data, const std::string& format) {
    std::cout << "  Creating spike visualization in " << format << " format..." << std::endl;
    
    // Extract spike data
    std::vector<int> spike_times;
    std::vector<int> neuron_ids;
    
    if (data.contains("spikes")) {
        for (const auto& spike : data["spikes"]) {
            spike_times.push_back(spike.value("time", 0));
            neuron_ids.push_back(spike.value("neuron_id", 0));
        }
    }
    
    // Generate visualization based on format
    if (format == "ascii") {
        return createASCIISpikePlot(spike_times, neuron_ids);
    } else if (format == "json") {
        return exportSpikeVisualizationAsJSON(data);
    } else {
        std::cout << "  Warning: Format " << format << " not fully supported, exporting as JSON" << std::endl;
        return exportSpikeVisualizationAsJSON(data);
    }
}

bool VisualizeCommand::createActivityVisualization(const json& data, const std::string& format) {
    std::cout << "  Creating activity visualization in " << format << " format..." << std::endl;
    
    if (format == "ascii") {
        return createASCIIActivityHeatmap(data);
    } else {
        std::cout << "  Warning: Format " << format << " not fully supported" << std::endl;
        return false;
    }
}

bool VisualizeCommand::createConnectionVisualization(const json& data, const std::string& format) {
    std::cout << "  Creating connection visualization in " << format << " format..." << std::endl;
    
    if (format == "ascii") {
        return createASCIINetworkDiagram(data);
    } else {
        std::cout << "  Warning: Format " << format << " not fully supported" << std::endl;
        return false;
    }
}

bool VisualizeCommand::createWeightVisualization(const json& data, const std::string& format) {
    std::cout << "  Creating weight visualization in " << format << " format..." << std::endl;
    
    if (format == "ascii") {
        return createASCIIWeightDistribution(data);
    } else {
        std::cout << "  Warning: Format " << format << " not fully supported" << std::endl;
        return false;
    }
}

bool VisualizeCommand::createHeatmapVisualization(const json& data, const std::string& format) {
    std::cout << "  Creating heatmap visualization in " << format << " format..." << std::endl;
    
    if (format == "ascii") {
        return createASCIIHeatmap(data);
    } else {
        std::cout << "  Warning: Format " << format << " not fully supported" << std::endl;
        return false;
    }
}

bool VisualizeCommand::createTimelineVisualization(const json& data, const std::string& format) {
    std::cout << "  Creating timeline visualization in " << format << " format..." << std::endl;
    
    if (format == "ascii") {
        return createASCIITimeline(data);
    } else {
        std::cout << "  Warning: Format " << format << " not fully supported" << std::endl;
        return false;
    }
}

bool VisualizeCommand::createNetworkVisualization(const json& data, const std::string& format) {
    std::cout << "  Creating network visualization in " << format << " format..." << std::endl;
    
    if (format == "ascii") {
        return createASCIIFile("network_visualization.txt");
    } else {
        std::cout << "  Warning: Format " << format << " not fully supported" << std::endl;
        return false;
    }
}

bool VisualizeCommand::createASCIISpikePlot(const std::vector<int>& spike_times, const std::vector<int>& neuron_ids) {
    std::string filename = "visualizations/spike_raster_" + getTimestamp() + ".txt";
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create output file: " << filename << std::endl;
        return false;
    }
    
    file << "Spike Raster Plot" << std::endl;
    file << "===================" << std::endl;
    file << "Total spikes: " << spike_times.size() << std::endl;
    file << std::endl;
    
    if (!spike_times.empty() && !neuron_ids.empty()) {
        file << "Spike times and neuron IDs:" << std::endl;
        for (size_t i = 0; i < spike_times.size(); ++i) {
            file << "Time: " << spike_times[i] 
                 << " | Neuron: " << neuron_ids[i] << std::endl;
        }
    }
    
    file.close();
    std::cout << "Spike visualization saved to: " << filename << std::endl;
    
    return true;
}

bool VisualizeCommand::exportSpikeVisualizationAsJSON(const json& data) {
    json viz_data;
    viz_data["type"] = "spike_raster";
    viz_data["timestamp"] = getTimestamp();
    
    // Copy spike data
    if (data.contains("spikes")) {
        viz_data["spikes"] = data["spikes"];
    }
    
    std::string filename = "visualizations/spike_visualization_" + getTimestamp() + ".json";
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create output file: " << filename << std::endl;
        return false;
    }
    
    file << viz_data.dump(2);
    file.close();
    std::cout << "Spike visualization saved to: " << filename << std::endl;
    
    return true;
}

bool VisualizeCommand::createASCIIActivityHeatmap(const json& data) {
    std::string filename = "visualizations/activity_heatmap_" + getTimestamp() + ".txt";
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create output file: " << filename << std::endl;
        return false;
    }
    
    file << "Activity Heatmap" << std::endl;
    file << "==================" << std::endl;
    
    if (data.contains("activity")) {
        json activity = data["activity"];
        file << "Activity data:" << std::endl;
        file << activity.dump(2) << std::endl;
    }
    
    file.close();
    std::cout << "Activity heatmap saved to: " << filename << std::endl;
    
    return true;
}

bool VisualizeCommand::createASCIINetworkDiagram(const json& data) {
    std::string filename = "visualizations/network_diagram_" + getTimestamp() + ".txt";
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create output file: " << filename << std::endl;
        return false;
    }
    
    file << "Network Diagram" << std::endl;
    file << "===============" << std::endl;
    
    if (data.contains("connections")) {
        json connections = data["connections"];
        file << "Connections:" << std::endl;
        file << connections.dump(2) << std::endl;
    }
    
    file.close();
    std::cout << "Network diagram saved to: " << filename << std::endl;
    
    return true;
}

bool VisualizeCommand::createASCIIWeightDistribution(const json& data) {
    std::string filename = "visualizations/weight_distribution_" + getTimestamp() + ".txt";
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create output file: " << filename << std::endl;
        return false;
    }
    
    file << "Weight Distribution" << std::endl;
    file << "====================" << std::endl;
    
    if (data.contains("weights")) {
        json weights = data["weights"];
        file << "Weights:" << std::endl;
        file << weights.dump(2) << std::endl;
    }
    
    file.close();
    std::cout << "Weight distribution saved to: " << filename << std::endl;
    
    return true;
}

bool VisualizeCommand::createASCIIHeatmap(const json& data) {
    std::string filename = "visualizations/heatmap_" + getTimestamp() + ".txt";
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create output file: " << filename << std::endl;
        return false;
    }
    
    file << "Heatmap Visualization" << std::endl;
    file << "=======================" << std::endl;
    
    file << "Data:" << std::endl;
    file << data.dump(2) << std::endl;
    
    file.close();
    std::cout << "Heatmap saved to: " << filename << std::endl;
    
    return true;
}

bool VisualizeCommand::createASCIITimeline(const json& data) {
    std::string filename = "visualizations/timeline_" + getTimestamp() + ".txt";
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create output file: " << filename << std::endl;
        return false;
    }
    
    file << "Timeline Visualization" << std::endl;
    file << "=======================" << std::endl;
    
    file << "Timeline data:" << std::endl;
    file << data.dump(2) << std::endl;
    
    file.close();
    std::cout << "Timeline saved to: " << filename << std::endl;
    
    return true;
}

bool VisualizeCommand::createASCIIFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create output file: " << filename << std::endl;
        return false;
    }
    
    file << "Network visualization placeholder" << std::endl;
    file << "(Real implementation would create graphical network diagram)" << std::endl;
    
    file.close();
    std::cout << "Network visualization saved to: " << filename << std::endl;
    
    return true;
}

std::string VisualizeCommand::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

} // namespace cli
} // namespace nlm
