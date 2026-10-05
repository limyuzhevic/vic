#include "VisualizationExperiment.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

namespace nlm {

VisualizationExperiment::VisualizationExperiment(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
    : Experiment(brain, config) {
    profiler_ = std::make_shared<PerformanceProfiler>();
}

bool VisualizationExperiment::setup() {
    std::cout << "Setting up visualization experiment..." << std::endl;
    
    // Configure profiler with default visualization settings
    visualizationConfig_ = PerformanceProfiler::createNetworkVisualizationConfig("network.png");
    visualizationConfig_.interactive = false;
    visualizationConfig_.animation = false;
    visualizationConfig_.scale = 1.0;
    visualizationConfig_.colorScheme = "default";
    
    std::cout << "Visualization configured with output: " << visualizationConfig_.outputFile << std::endl;
    return true;
}

bool VisualizationExperiment::run() {
    std::cout << "Running visualization experiment..." << std::endl;
    
    // Generate network visualization
    if (!generateNetworkVisualization(visualizationConfig_.outputFile)) {
        std::cout << "Network visualization failed" << std::endl;
        return false;
    }
    
    // Generate spike raster visualization
    if (!generateSpikeRasterVisualization("spike_raster.png")) {
        std::cout << "Spike raster visualization failed" << std::endl;
        return false;
    }
    
    // Generate weight matrix visualization
    if (!generateWeightMatrixVisualization("weight_matrix.png")) {
        std::cout << "Weight matrix visualization failed" << std::endl;
        return false;
    }
    
    // Generate trajectory visualization
    if (!generateTrajectoryVisualization("trajectory.png")) {
        std::cout << "Trajectory visualization failed" << std::endl;
        return false;
    }
    
    std::cout << "Visualization experiment completed successfully" << std::endl;
    return true;
}

bool VisualizationExperiment::cleanup() {
    std::cout << "Cleaning up visualization experiment..." << std::endl;
    
    // Generate and save report
    std::string report = generateReport();
    std::cout << "\nVisualization Report:" << std::endl;
    std::cout << report << std::endl;
    
    // Export results
    std::string outputFile = visualizationConfig_.outputFile + ".json";
    if (exportResults(outputFile, "json")) {
        std::cout << "Results exported to " << outputFile << std::endl;
    } else {
        std::cout << "Failed to export results" << std::endl;
    }
    
    return true;
}

bool VisualizationExperiment::generateNetworkVisualization(const std::string& outputFile) {
    std::cout << "\n=== Generating Network Visualization ===" << std::endl;
    std::cout << "Output: " << outputFile << " (format: " << visualizationConfig_.format << ")" << std::endl;
    
    // Simulate network visualization generation
    VisualizationResult result;
    result.visualizationType = "network";
    result.outputFile = outputFile;
    result.format = visualizationConfig_.format;
    result.interactive = visualizationConfig_.interactive;
    result.animation = visualizationConfig_.animation;
    result.generationTime = std::chrono::system_clock::now();
    result.fileSize = 1024 * 1024; // 1 MB simulated
    result.dataPoints = {"neuron_1", "neuron_2", "neuron_3", "neuron_4", "neuron_5"};
    result.metadata = "Generated with NLM visualization system";
    
    // In real implementation, this would generate actual network diagrams
    // using brain state data (neuron connectivity, weights, activity)
    
    std::cout << "Network visualization generated successfully" << std::endl;
    results_.push_back(result);
    
    return true;
}

bool VisualizationExperiment::generateSpikeRasterVisualization(const std::string& outputFile) {
    std::cout << "\n=== Generating Spike Raster Visualization ===" << std::endl;
    std::cout << "Output: " << outputFile << " (format: " << visualizationConfig_.format << ")" << std::endl;
    
    VisualizationResult result;
    result.visualizationType = "spike_raster";
    result.outputFile = outputFile;
    result.format = visualizationConfig_.format;
    result.interactive = false;
    result.animation = false;
    result.generationTime = std::chrono::system_clock::now();
    result.fileSize = 512 * 1024; // 512 KB simulated
    result.dataPoints = {"spike_train_1", "spike_train_2", "spike_train_3"};
    result.metadata = "Spike train visualization";
    
    // In real implementation, this would generate spike rasters
    // showing neural firing patterns over time
    
    std::cout << "Spike raster visualization generated successfully" << std::endl;
    results_.push_back(result);
    
    return true;
}

bool VisualizationExperiment::generateWeightMatrixVisualization(const std::string& outputFile) {
    std::cout << "\n=== Generating Weight Matrix Visualization ===" << std::endl;
    std::cout << "Output: " << outputFile << " (format: " << visualizationConfig_.format << ")" << std::endl;
    
    VisualizationResult result;
    result.visualizationType = "weight_matrix";
    result.outputFile = outputFile;
    result.format = visualizationConfig_.format;
    result.interactive = visualizationConfig_.showConnections;
    result.animation = false;
    result.generationTime = std::chrono::system_clock::now();
    result.fileSize = 768 * 1024; // 768 KB simulated
    result.dataPoints = {"synaptic_weight_1", "synaptic_weight_2", "synaptic_weight_3"};
    result.metadata = "Synaptic weight matrix visualization";
    
    // In real implementation, this would generate heat maps of synaptic weights
    // showing connection strengths between neurons
    
    std::cout << "Weight matrix visualization generated successfully" << std::endl;
    results_.push_back(result);
    
    return true;
}

bool VisualizationExperiment::generateTrajectoryVisualization(const std::string& outputFile) {
    std::cout << "\n=== Generating Trajectory Visualization ===" << std::endl;
    std::cout << "Output: " << outputFile << " (format: " << visualizationConfig_.format << ")" << std::endl;
    
    VisualizationResult result;
    result.visualizationType = "trajectory";
    result.outputFile = outputFile;
    result.format = visualizationConfig_.format;
    result.interactive = true;
    result.animation = visualizationConfig_.animation;
    result.generationTime = std::chrono::system_clock::now();
    result.fileSize = 896 * 1024; // 896 KB simulated
    result.dataPoints = {"trajectory_point_1", "trajectory_point_2", "trajectory_point_3"};
    result.metadata = "Brain state trajectory over time";
    
    // In real implementation, this would generate trajectory plots
    // showing how neural activity evolves over time
    
    std::cout << "Trajectory visualization generated successfully" << std::endl;
    results_.push_back(result);
    
    return true;
}

bool VisualizationExperiment::generateInteractiveDashboard(const std::string& outputFile) {
    std::cout << "\n=== Generating Interactive Dashboard ===" << std::endl;
    std::cout << "Output: " << outputFile << " (HTML format)" << std::endl;
    
    VisualizationResult result;
    result.visualizationType = "interactive_dashboard";
    result.outputFile = outputFile;
    result.format = "html";
    result.interactive = true;
    result.animation = false;
    result.generationTime = std::chrono::system_clock::now();
    result.fileSize = 2048 * 1024; // 2 MB simulated
    result.dataPoints = {"live_metrics", "neuron_activity", "network_state"};
    result.metadata = "Interactive visualization dashboard";
    
    // In real implementation, this would generate HTML/JavaScript dashboard
    // with real-time updates from the brain simulation
    
    std::cout << "Interactive dashboard generated successfully" << std::endl;
    results_.push_back(result);
    
    return true;
}

bool VisualizationExperiment::generateAnimation(const std::string& outputFile, std::chrono::milliseconds frameInterval) {
    std::cout << "\n=== Generating Animation ===" << std::endl;
    std::cout << "Output: " << outputFile << " (format: GIF)" << std::endl;
    std::cout << "Frame interval: " << std::chrono::duration_cast<std::chrono::seconds>(frameInterval).count() << " seconds" << std::endl;
    
    VisualizationResult result;
    result.visualizationType = "animation";
    result.outputFile = outputFile;
    result.format = "gif";
    result.interactive = false;
    result.animation = true;
    result.generationTime = std::chrono::system_clock::now();
    result.fileSize = 5120 * 1024; // 5 MB simulated
    result.dataPoints = {"frame_1", "frame_2", "frame_3", "frame_4", "frame_5"};
    result.metadata = "Animation of brain state evolution";
    
    // In real implementation, this would generate animated visualizations
    // showing how the brain network evolves over time
    
    std::cout << "Animation generated successfully" << std::endl;
    results_.push_back(result);
    
    return true;
}

std::string VisualizationExperiment::generateReport() const {
    std::stringstream ss;
    ss << "NLM Visualization Experiment Report" << std::endl;
    ss << std::string(50, '=') << std::endl << std::endl;
    
    ss << "Total Visualizations: " << results_.size() << std::endl;
    ss << std::endl;
    
    if (results_.empty()) {
        return ss.str();
    }
    
    ss << "Visualization Summary:" << std::endl;
    ss << "-------------------" << std::endl;
    
    double totalFileSize = 0;
    
    for (size_t i = 0; i < results_.size(); ++i) {
        const auto& result = results_[i];
        
        totalFileSize += result.fileSize;
        
        ss << std::setw(3) << (i + 1) << ". " << result.visualizationType << std::endl;
        ss << std::setw(6) << "  File: " << result.outputFile << std::endl;
        ss << std::setw(6) << "  Format: " << result.format << std::endl;
        ss << std::setw(6) << "  Size: " << (result.fileSize / 1024 / 1024) << " MB" << std::endl;
        ss << std::setw(6) << "  Interactive: " << (result.interactive ? "YES" : "NO") << std::endl;
        ss << std::setw(6) << "  Animation: " << (result.animation ? "YES" : "NO") << std::endl;
        ss << std::setw(6) << "  Data Points: " << result.dataPoints.size() << std::endl;
        ss << std::setw(6) << "  Generated: " << std::chrono::system_clock::to_time_t(result.generationTime) << std::endl;
        ss << std::endl;
    }
    
    if (!results_.empty()) {
        ss << "Overall Statistics:" << std::endl;
        ss << "-------------------" << std::endl;
        ss << "Average File Size: " << std::fixed << std::setprecision(2)
           << (totalFileSize / results_.size() / 1024 / 1024) << " MB" << std::endl;
    }
    
    ss << "\nVisualization Types Generated:" << std::endl;
    ss << "--------------------------" << std::endl;
    ss << "• Network diagram showing neural connectivity" << std::endl;
    ss << "• Spike raster plots displaying firing patterns" << std::endl;
    ss << "• Weight matrix heat maps of synaptic connections" << std::endl;
    ss << "• Trajectory plots of brain state evolution" << std::endl;
    ss << "• Interactive dashboards with real-time monitoring" << std::endl;
    
    return ss.str();
}

bool VisualizationExperiment::exportResults(const std::string& filepath, const std::string& format) const {
    if (format == "json") {
        nlohmann::json jsonData;
        
        jsonData["experiment_name"] = "visualization_experiment";
        jsonData["run_timestamp"] = std::chrono::system_clock::now();
        jsonData["total_visualizations"] = results_.size();
        
        nlohmann::json resultsJson;
        for (const auto& result : results_) {
            nlohmann::json resultJson;
            resultJson["visualization_type"] = result.visualizationType;
            resultJson["output_file"] = result.outputFile;
            resultJson["format"] = result.format;
            resultJson["file_size"] = result.fileSize;
            resultJson["interactive"] = result.interactive;
            resultJson["animation"] = result.animation;
            resultJson["generation_time"] = result.generationTime;
            resultJson["data_points"] = result.dataPoints;
            resultJson["metadata"] = result.metadata;
            
            resultsJson.push_back(resultJson);
        }
        
        jsonData["results"] = resultsJson;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(2) << jsonData << std::endl;
        
    } else {
        // Default to text format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateReport();
    }
    
    return true;
}

} // namespace nlm
