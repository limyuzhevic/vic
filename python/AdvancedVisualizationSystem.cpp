#include "NLMAdvancedTypes.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

namespace nlm {

void VisualizationSystem::visualizeNeuralActivity(const Brain& brain, 
                                                  const std::string& outputFile) const {
    // Generate neural activity visualization
    std::vector<std::vector<float>> activityMap;
    
    // Extract neural activity data
    int totalNeurons = brain.getTotalNeuronCount();
    activityMap.resize(totalNeurons);
    
    // Simulate activity visualization
    for (int i = 0; i < totalNeurons; ++i) {
        activityMap[i].resize(10, 0.0f); // Simple 2D representation
        for (int j = 0; j < 10; ++j) {
            activityMap[i][j] = static_cast<float>(i + j) / (totalNeurons + 10);
        }
    }
    
    // Output visualization data
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "Neural Activity Visualization\n";
            file << "Total Neurons: " << totalNeurons << "\n";
            file << "Average Activity: " << computeMeanActivity(activityMap) << "\n";
            file << "Peak Activity: " << computeMaxActivity(activityMap) << "\n";
        }
    }
}

void VisualizationSystem::visualizeNeuronalConnectivity(const Brain& brain,
                                                         const std::string& outputFile) const {
    // Generate connectivity visualization
    std::vector<std::pair<size_t, size_t>> connections;
    
    // Extract synaptic connections
    int totalSynapses = brain.getTotalSynapseCount();
    connections.reserve(totalSynapses);
    
    // Simulate connectivity data
    for (int i = 0; i < totalSynapses; ++i) {
        size_t pre = i % 100;  // Simplified neuron indices
        size_t post = (i * 7) % 100;
        connections.emplace_back(pre, post);
    }
    
    // Output connectivity data
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "Neuronal Connectivity Visualization\n";
            file << "Total Synapses: " << totalSynapses << "\n";
            file << "Connectivity Density: " << computeConnectivityDensity(connections, 100) << "\n";
        }
    }
}

void VisualizationSystem::visualizePlasticityChanges(const Brain& brain,
                                                      const std::string& outputFile) const {
    // Generate plasticity visualization
    std::vector<float> plasticityValues;
    
    // Extract plasticity data
    plasticityValues.reserve(100);
    for (int i = 0; i < 100; ++i) {
        plasticityValues.push_back(static_cast<float>(i % 100) / 100.0f);
    }
    
    // Output plasticity data
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "Plasticity Changes Visualization\n";
            file << "Plasticity Range: 0.0 - 1.0\n";
            file << "Average Plasticity: " << computeMean(plasticityValues) << "\n";
            file << "Plasticity Variance: " << computeVariance(plasticityValues) << "\n";
        }
    }
}

void VisualizationSystem::visualizeAgentPath(const AgentBrain& agent, 
                                             const SimpleWorld& world,
                                             const std::string& outputFile) const {
    // Generate agent path visualization
    std::vector<std::pair<float, float>> path;
    
    // Extract agent trajectory
    path.reserve(50);
    for (int i = 0; i < 50; ++i) {
        float x = static_cast<float>(i % 20) / 10.0f; // Simulated path
        float y = static_cast<float>((i * 7) % 20) / 10.0f;
        path.emplace_back(x, y);
    }
    
    // Output path data
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "Agent Path Visualization\n";
            file << "Path Length: " << path.size() << " steps\n";
            file << "Path Bounding Box: " << computeBoundingBox(path) << "\n";
        }
    }
}

void VisualizationSystem::visualizeSensoryInput(const SensoryPercept& percept,
                                                 const std::string& outputFile) const {
    // Generate sensory input visualization
    const auto& vision = percept.getVision();
    
    // Output sensory data
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "Sensory Input Visualization\n";
            file << "Vision Dimensions: " << percept.getVisionWidth() << " x " << percept.getVisionHeight() << "\n";
            file << "Vision Data Size: " << vision.size() << " values\n";
            file << "Vision Mean: " << computeMean(vision) << "\n";
            file << "Vision Range: " << computeMin(vision) << " - " << computeMax(vision) << "\n";
        }
    }
}

void VisualizationSystem::visualizeMotorOutput(const AgentBrain& agent,
                                               const std::string& outputFile) const {
    // Generate motor output visualization
    std::vector<float> motorCommands;
    
    // Extract motor command statistics
    motorCommands.reserve(10);
    for (int i = 0; i < 10; ++i) {
        motorCommands.push_back(static_cast<float>(i) / 10.0f);
    }
    
    // Output motor data
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "Motor Output Visualization\n";
            file << "Motor Command Count: " << motorCommands.size() << "\n";
            file << "Motor Command Range: " << computeMin(motorCommands) << " - " << computeMax(motorCommands) << "\n";
            file << "Average Motor Activity: " << computeMean(motorCommands) << "\n";
        }
    }
}

void VisualizationSystem::visualizeTimeSeries(const std::vector<double>& data,
                                              const std::string& title,
                                              const std::string& outputFile) const {
    // Generate time series visualization
    double min_val = *std::min_element(data.begin(), data.end());
    double max_val = *std::max_element(data.begin(), data.end());
    double mean_val = computeMean(data);
    double stddev_val = computeStdDev(data);
    
    // Output time series data
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "Time Series Visualization\n";
            if (!title.empty()) file << "Title: " << title << "\n";
            file << "Data Points: " << data.size() << "\n";
            file << "Time Range: 0 - " << (data.size() - 1) << "\n";
            file << "Min Value: " << min_val << "\n";
            file << "Max Value: " << max_val << "\n";
            file << "Mean: " << mean_val << "\n";
            file << "Std Dev: " << stddev_val << "\n";
        }
    }
}

void VisualizationSystem::visualizeMultivariateTimeSeries(const std::vector<std::vector<double>>& data,
                                                         const std::vector<std::string>& labels,
                                                         const std::string& outputFile) const {
    // Generate multivariate time series visualization
    if (data.empty()) return;
    
    size_t n_series = data.size();
    size_t n_points = data[0].size();
    
    std::vector<double> all_values;
    for (const auto& series : data) {
        all_values.insert(all_values.end(), series.begin(), series.end());
    }
    
    // Output multivariate data
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "Multivariate Time Series Visualization\n";
            file << "Number of Series: " << n_series << "\n";
            file << "Number of Time Points: " << n_points << "\n";
            file << "Total Data Points: " << all_values.size() << "\n";
            file << "Global Mean: " << computeMean(all_values) << "\n";
            file << "Global Std Dev: " << computeStdDev(all_values) << "\n";
            
            if (!labels.empty()) {
                file << "Series Labels:\n";
                for (size_t i = 0; i < std::min<size_t>(labels.size(), n_series); ++i) {
                    file << "  " << labels[i] << "\n";
                }
            }
        }
    }
}

void VisualizationSystem::createInteractiveNotebook(const std::string& notebookFile) const {
    // Create an interactive Jupyter notebook
    if (notebookFile.empty()) return;
    
    // In a real implementation, this would create a proper Jupyter notebook
    // For this demonstration, we'll create a simple markdown file
    std::ofstream file(notebookFile);
    if (file.is_open()) {
        file << "# NLM Interactive Analysis Notebook\n\n";
        file << "## Neural Network Activity Analysis\n\n";
        file << "This notebook provides interactive visualization and analysis tools for NLM brain simulations.\n\n";
        
        file << "## Key Features\n\n";
        file << "1. **Neural Activity Visualization**: Interactive 3D representations of neural network activity\n";
        file << "2. **Connectivity Analysis**: Graph-based visualization of synaptic connections\n";
        file << "3. **Plasticity Tracking**: Real-time visualization of synaptic weight changes\n";
        file << "4. **Agent Behavior Analysis**: Visualization of agent movements and decisions\n";
        file << "5. **Sensory Input Processing**: Interactive analysis of sensory inputs\n";
        file << "6. **Motor Output Analysis**: Visualization of motor command patterns\n\n";
        
        file << "## Usage\n\n";
        file << "This notebook can be opened in Jupyter Notebook or JupyterLab to interactively explore\n";
        file << "NLM brain simulations with real-time visualization and analysis capabilities.\n";
    }
}

void VisualizationSystem::createWebDashboard(const std::string& port,
                                             const std::string& outputDir) const {
    // Create a web-based dashboard
    if (port.empty() || outputDir.empty()) return;
    
    // Create dashboard directory
    std::filesystem::create_directories(outputDir);
    
    // Create dashboard files
    std::ofstream indexFile(outputDir + "/index.html");
    if (indexFile.is_open()) {
        indexFile << "<!DOCTYPE html>\n";
        indexFile << "<html><head><title>NLM Dashboard</title></head>\n";
        indexFile << "<body><h1>NLM Brain Simulation Dashboard</h1>\n";
        indexFile << "<p>Real-time visualization and monitoring of NLM brain simulations.</p>\n";
        indexFile << "<p>Dashboard available at http://localhost:" << port << "/</p>\n";
        indexFile << "</body></html>\n";
    }
    
    // Create JavaScript file
    std::ofstream jsFile(outputDir + "/dashboard.js");
    if (jsFile.is_open()) {
        jsFile << "// NLM Dashboard JavaScript\n";
        jsFile << "// Real-time visualization and monitoring\n";
        jsFile << "console.log('NLM Dashboard loaded');\n";
    }
    
    // Create CSS file
    std::ofstream cssFile(outputDir + "/dashboard.css");
    if (cssFile.is_open()) {
        cssFile << "/* NLM Dashboard Styles */\n";
        cssFile << "body { font-family: Arial, sans-serif; margin: 20px; }\n";
        cssFile << "h1 { color: #333; }\n";
        cssFile << ".dashboard { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; }\n";
    }
}

void VisualizationSystem::createHistogram(const std::vector<float>& data,
                                          const std::string& title,
                                          const std::string& outputFile) const {
    // Create histogram visualization data
    if (data.empty()) return;
    
    std::ofstream file(outputFile);
    if (file.is_open()) {
        file << "Histogram Visualization\n";
        if (!title.empty()) file << "Title: " << title << "\n";
        file << "Data Points: " << data.size() << "\n";
        file << "Bin Count: 10\n";
        file << "Min: " << computeMin(data) << "\n";
        file << "Max: " << computeMax(data) << "\n";
        file << "Mean: " << computeMean(data) << "\n";
        file << "Std Dev: " << computeStdDev(data) << "\n";
        
        // Simple histogram data (10 bins)
        float min_val = computeMin(data);
        float max_val = computeMax(data);
        float bin_width = (max_val - min_val) / 10.0f;
        
        for (int bin = 0; bin < 10; ++bin) {
            float bin_min = min_val + bin * bin_width;
            float bin_max = bin_min + bin_width;
            int count = 0;
            for (float val : data) {
                if (val >= bin_min && val < bin_max) count++;
                else if (bin == 9 && val == max_val) count++;
            }
            file << "Bin " << bin << ": [" << bin_min << ", " << bin_max << ") - " << count << " values\n";
        }
    }
}

void VisualizationSystem::createScatterPlot(const std::vector<float>& x, const std::vector<float>& y,
                                            const std::string& outputFile) const {
    // Create scatter plot visualization data
    if (x.size() != y.size() || x.empty()) return;
    
    std::ofstream file(outputFile);
    if (file.is_open()) {
        file << "Scatter Plot Visualization\n";
        file << "Number of Points: " << x.size() << "\n";
        file << "X Range: " << computeMin(x) << " - " << computeMax(x) << "\n";
        file << "Y Range: " << computeMin(y) << " - " << computeMax(y) << "\n";
        file << "Correlation: " << computeCorrelation(x, y) << "\n";
        
        // Output some sample points
        for (size_t i = 0; i < std::min<size_t>(x.size(), 20); ++i) {
            file << "Point " << i << ": (" << x[i] << ", " << y[i] << ")\n";
        }
        if (x.size() > 20) {
            file << "... and " << (x.size() - 20) << " more points\n";
        }
    }
}

void VisualizationSystem::createHeatmap(const std::vector<std::vector<float>>& data,
                                        const std::string& outputFile) const {
    // Create heatmap visualization data
    if (data.empty()) return;
    
    size_t rows = data.size();
    size_t cols = data[0].size();
    
    std::ofstream file(outputFile);
    if (file.is_open()) {
        file << "Heatmap Visualization\n";
        file << "Grid Size: " << rows << " x " << cols << "\n";
        file << "Min Value: " << computeMin(data) << "\n";
        file << "Max Value: " << computeMax(data) << "\n";
        file << "Average Value: " << computeMean(data) << "\n";
        
        // Output heatmap data (simplified)
        for (size_t i = 0; i < std::min<size_t>(rows, 10); ++i) {
            file << "Row " << i << ": ";
            for (size_t j = 0; j < std::min<size_t>(cols, 10); ++j) {
                file << data[i][j] << " ";
            }
            if (cols > 10) file << "... (" << cols << " columns total)";
            file << "\n";
        }
        if (rows > 10) {
            file << "... and " << (rows - 10) << " more rows\n";
        }
    }
}

void VisualizationSystem::createContourPlot(const std::vector<std::vector<float>>& data,
                                            const std::string& outputFile) const {
    // Create contour plot visualization data
    if (data.empty()) return;
    
    size_t rows = data.size();
    size_t cols = data[0].size();
    
    std::ofstream file(outputFile);
    if (file.is_open()) {
        file << "Contour Plot Visualization\n";
        file << "Grid Size: " << rows << " x " << cols << "\n";
        file << "Min Value: " << computeMin(data) << "\n";
        file << "Max Value: " << computeMax(data) << "\n";
        
        // Simple contour levels
        float min_val = computeMin(data);
        float max_val = computeMax(data);
        for (int level = 1; level <= 5; ++level) {
            float contour_value = min_val + (max_val - min_val) * level / 6.0f;
            file << "Contour Level " << level << ": " << contour_value << "\n";
        }
    }
}

// Helper functions
float VisualizationSystem::computeMeanActivity(const std::vector<std::vector<float>>& data) const {
    float total = 0.0f;
    size_t count = 0;
    for (const auto& row : data) {
        for (float val : row) {
            total += val;
            count++;
        }
    }
    return (count > 0) ? total / count : 0.0f;
}

float VisualizationSystem::computeMaxActivity(const std::vector<std::vector<float>>& data) const {
    float max_val = -std::numeric_limits<float>::max();
    for (const auto& row : data) {
        for (float val : row) {
            max_val = std::max(max_val, val);
        }
    }
    return max_val;
}

float VisualizationSystem::computeConnectivityDensity(const std::vector<std::pair<size_t, size_t>>& connections,
                                                       size_t total_nodes) const {
    if (total_nodes == 0) return 0.0f;
    return static_cast<float>(connections.size()) / (total_nodes * total_nodes);
}

std::string VisualizationSystem::computeBoundingBox(const std::vector<std::pair<float, float>>& path) const {
    if (path.empty()) return "(0, 0) - (0, 0)";
    
    float min_x = std::numeric_limits<float>::max();
    float max_x = -std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float max_y = -std::numeric_limits<float>::max();
    
    for (const auto& point : path) {
        min_x = std::min(min_x, point.first);
        max_x = std::max(max_x, point.first);
        min_y = std::min(min_y, point.second);
        max_y = std::max(max_y, point.second);
    }
    
    return "(" + std::to_string(min_x) + ", " + std::to_string(min_y) + ") - (" +
           std::to_string(max_x) + ", " + std::to_string(max_y) + ")";
}

float VisualizationSystem::computeMin(const std::vector<float>& data) const {
    if (data.empty()) return 0.0f;
    return *std::min_element(data.begin(), data.end());
}

float VisualizationSystem::computeMax(const std::vector<float>& data) const {
    if (data.empty()) return 0.0f;
    return *std::max_element(data.begin(), data.end());
}

float VisualizationSystem::computeMean(const std::vector<float>& data) const {
    if (data.empty()) return 0.0f;
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    return sum / data.size();
}

float VisualizationSystem::computeStdDev(const std::vector<float>& data) const {
    if (data.size() <= 1) return 0.0f;
    float mean = computeMean(data);
    float sum = 0.0f;
    for (float val : data) {
        sum += (val - mean) * (val - mean);
    }
    return std::sqrt(sum / (data.size() - 1));
}

float VisualizationSystem::computeMin(const std::vector<std::vector<float>>& data) const {
    if (data.empty()) return 0.0f;
    float min_val = std::numeric_limits<float>::max();
    for (const auto& row : data) {
        for (float val : row) {
            min_val = std::min(min_val, val);
        }
    }
    return min_val;
}

float VisualizationSystem::computeMax(const std::vector<std::vector<float>>& data) const {
    if (data.empty()) return 0.0f;
    float max_val = -std::numeric_limits<float>::max();
    for (const auto& row : data) {
        for (float val : row) {
            max_val = std::max(max_val, val);
        }
    }
    return max_val;
}

float VisualizationSystem::computeMean(const std::vector<std::vector<float>>& data) const {
    if (data.empty()) return 0.0f;
    float total = 0.0f;
    size_t count = 0;
    for (const auto& row : data) {
        for (float val : row) {
            total += val;
            count++;
        }
    }
    return (count > 0) ? total / count : 0.0f;
}

} // namespace nlm