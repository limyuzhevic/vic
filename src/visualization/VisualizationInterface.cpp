#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <fstream>

namespace nlm {
namespace visualization {

NetworkMetrics ActivityAnalyzer::analyzeNetwork(const std::shared_ptr<nlm::Brain>& brain) {
    NetworkMetrics metrics;
    
    if (!brain) return metrics;
    
    // Calculate basic network statistics
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    if (totalNeurons == 0) return metrics;
    
    // Average degree (synapses per neuron)
    metrics.avgDegree = static_cast<double>(totalSynapses) / totalNeurons;
    
    // Calculate average synaptic weight
    float totalWeight = 0.0f;
    float weightSumSq = 0.0f;
    size_t synapseCount = 0;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            float weight = syn->getWeight();
            totalWeight += weight;
            weightSumSq += weight * weight;
            synapseCount++;
        }
    }
    
    if (synapseCount > 0) {
        metrics.averageWeight = totalWeight / synapseCount;
        metrics.weightStdDev = std::sqrt((weightSumSq / synapseCount) - metrics.averageWeight * metrics.averageWeight);
    }
    
    // Active neuron count
    metrics.activeNeuronCount = brain->getActiveNeuronCount();
    
    // Calculate network efficiency (simplified)
    metrics.globalEfficiency = (totalNeurons > 0) ? 
        static_cast<double>(metrics.activeNeuronCount) / totalNeurons : 0.0;
    
    return metrics;
}

HeatmapData ActivityAnalyzer::generateHeatmap(const std::shared_ptr<nlm::Brain>& brain, 
                                              int width, int height) {
    HeatmapData heatmap(width, height);
    
    if (!brain || brain->getTotalNeuronCount() == 0) {
        return heatmap;
    }
    
    // Get all neurons and their positions (simplified - using region and population)
    std::vector<nlm::Neuron*> neurons;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (nlm::Neuron* neuron : pop->getNeurons()) {
                neurons.push_back(neuron);
            }
        }
    }
    
    if (neurons.empty()) return heatmap;
    
    // Create spatial mapping based on population and region
    float maxActivity = 0.0f;
    
    for (nlm::Neuron* neuron : neurons) {
        // Simple spatial mapping: region index and population index
        int regionId = neuron->getRegionId().index() - 1;  // Convert to 0-based
        int popId = neuron->getPopulationId().index() - 1;
        
        // Map to heatmap coordinates
        int row = (regionId * 10 + popId) % height;
        int col = neuron->getId().index() % width;
        
        if (row >= 0 && row < height && col >= 0 && col < width) {
            float activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            heatmap.activityMatrix[row][col] += activity;
            if (activity > maxActivity) maxActivity = activity;
        }
    }
    
    heatmap.maxActivity = maxActivity;
    heatmap.minActivity = 0.0f;
    
    return heatmap;
}

std::vector<std::vector<float>> ActivityAnalyzer::calculateSpatialActivity(
    const std::shared_ptr<nlm::Brain>& brain,
    int gridWidth, int gridHeight) {
    
    std::vector<std::vector<float>> spatialActivity(gridHeight, std::vector<float>(gridWidth, 0.0f));
    
    if (!brain) return spatialActivity;
    
    // Simplified spatial activity calculation
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            // Create a simple distance-based influence from neuron positions
            float activity = 0.0f;
            
            for (const auto& region : brain->getRegions()) {
                for (const auto& pop : region->getPopulations()) {
                    for (nlm::Neuron* neuron : pop->getNeurons()) {
                        // Calculate distance from grid point to neuron
                        int neuronX = neuron->getId().index() % gridWidth;
                        int neuronY = (neuron->getPopulationId().index() + neuron->getRegionId().index()) % gridHeight;
                        
                        float distance = std::sqrt(std::pow(x - neuronX, 2) + std::pow(y - neuronY, 2));
                        float influence = std::exp(-distance / 5.0f);  // Gaussian influence
                        
                        float neuronActivity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                        activity += influence * neuronActivity;
                    }
                }
            }
            
            spatialActivity[y][x] = activity;
        }
    }
    
    return spatialActivity;
}

double ActivityAnalyzer::calculateFiringRate(const std::shared_ptr<nlm::Brain>& brain) {
    if (!brain || brain->getTotalNeuronCount() == 0) return 0.0;
    
    return static_cast<double>(brain->getTotalSpikeCount()) / brain->getTotalNeuronCount();
}

std::vector<float> ActivityAnalyzer::extractActivityTrace(const std::shared_ptr<nlm::Brain>& brain,
                                                         size_t steps) {
    std::vector<float> activityTrace;
    
    if (!brain) return activityTrace;
    
    // Simulate a few steps and extract activity
    for (size_t step = 0; step < steps; ++step) {
        double currentTime = step * 0.001;  // 1ms timestep
        brain->step(static_cast<nlm::SimulationStep>(step), currentTime);
        
        float activity = static_cast<float>(brain->getTotalSpikeCount() - 
                                          (step > 0 ? brain->getTotalSpikeCount() : 0));
        activityTrace.push_back(activity);
    }
    
    return activityTrace;
}

double ActivityAnalyzer::computeComplexity(const std::shared_ptr<nlm::Brain>& brain) {
    if (!brain) return 0.0;
    
    // Calculate network complexity based on multiple factors
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t activeNeurons = brain->getActiveNeuronCount();
    
    if (totalNeurons == 0) return 0.0;
    
    // Information entropy-based complexity measure
    double pActive = static_cast<double>(activeNeurons) / totalNeurons;
    double entropy = -pActive * std::log(pActive) - (1.0 - pActive) * std::log(1.0 - pActive);
    
    // Connectivity complexity
    double connectivityComplexity = std::min(1.0, static_cast<double>(totalSynapses) / (totalNeurons * totalNeurons * 0.1));
    
    // Combine metrics
    double complexity = (entropy * 0.6) + (connectivityComplexity * 0.4);
    
    return std::min(1.0, complexity);
}

std::string VisualizationExporter::exportHeatmapAsASCII(const HeatmapData& heatmap) {
    std::stringstream ss;
    
    ss << "NLM Neural Activity Heatmap" << std::endl;
    ss << "=" * 40 << std::endl;
    ss << "Dimensions: " << heatmap.width << " x " << heatmap.height << std::endl;
    ss << "Activity Range: [" << heatmap.minActivity << ", " << heatmap.maxActivity << "]" << std::endl;
    ss << std::endl;
    
    if (heatmap.maxActivity > 0.0f) {
        for (int y = heatmap.height - 1; y >= 0; --y) {
            for (int x = 0; x < heatmap.width; ++x) {
                float value = heatmap.activityMatrix[y][x];
                int intensity = static_cast<int>((value / heatmap.maxActivity) * 10);
                char symbol;
                
                if (intensity == 0) symbol = ' ';
                else if (intensity == 1) symbol = '.';
                else if (intensity == 2) symbol = '-';
                else if (intensity == 3) symbol = ':';
                else if (intensity == 4) symbol = '*';
                else if (intensity == 5) symbol = '+';
                else if (intensity == 6) symbol = 'X';
                else if (intensity == 7) symbol = '#';
                else if (intensity == 8) symbol = '%';
                else symbol = '@';
                
                ss << symbol;
            }
            ss << std::endl;
        }
    }
    
    return ss.str();
}

std::string VisualizationExporter::exportHeatmapAsJSON(const HeatmapData& heatmap) {
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"width\": " << heatmap.width << "," << std::endl;
    ss << "  \"height\": " << heatmap.height << "," << std::endl;
    ss << "  \"min_activity\": " << heatmap.minActivity << "," << std::endl;
    ss << "  \"max_activity\": " << heatmap.maxActivity << "," << std::endl;
    ss << "  \"activity_matrix\": [" << std::endl;
    
    for (int y = heatmap.height - 1; y >= 0; --y) {
        ss << "    [";
        for (int x = 0; x < heatmap.width; ++x) {
            ss << heatmap.activityMatrix[y][x];
            if (x < heatmap.width - 1) ss << ", ";
        }
        ss << "]";
        if (y > 0) ss << ",";
        ss << std::endl;
    }
    
    ss << "  ]" << std::endl;
    ss << "}" << std::endl;
    
    return ss.str();
}

std::string VisualizationExporter::exportHeatmapAsCSV(const HeatmapData& heatmap) {
    std::stringstream ss;
    
    ss << "NLM Neural Activity Heatmap (CSV)" << std::endl;
    ss << "row,col,activity" << std::endl;
    
    for (int y = heatmap.height - 1; y >= 0; --y) {
        for (int x = 0; x < heatmap.width; ++x) {
            ss << y << "," << x << "," << heatmap.activityMatrix[y][x] << std::endl;
        }
    }
    
    return ss.str();
}

std::string VisualizationExporter::exportHeatmapAsMatlab(const HeatmapData& heatmap) {
    std::stringstream ss;
    
    ss << "% NLM Neural Activity Heatmap" << std::endl;
    ss << "% Dimensions: " << heatmap.width << " x " << heatmap.height << std::endl;
    ss << "activity_matrix = [" << std::endl;
    
    for (int y = heatmap.height - 1; y >= 0; --y) {
        ss << "  [";
        for (int x = 0; x < heatmap.width; ++x) {
            ss << heatmap.activityMatrix[y][x];
            if (x < heatmap.width - 1) ss << " ";
        }
        ss << "]";
        if (y > 0) ss << ";";
        ss << std::endl;
    }
    
    ss << "];" << std::endl;
    ss << "x = 1:" << heatmap.width << ";" << std::endl;
    ss << "y = 1:" << heatmap.height << ";" << std::endl;
    ss << "figure; surf(x, y, activity_matrix);" << std::endl;
    ss << "title('NLM Neural Activity Heatmap');" << std::endl;
    ss << "colormap jet; colorbar;" << std::endl;
    
    return ss.str();
}

std::string VisualizationExporter::exportHeatmapAsPython(const HeatmapData& heatmap) {
    std::stringstream ss;
    
    ss << "import numpy as np" << std::endl;
    ss << "import matplotlib.pyplot as plt" << std::endl;
    ss << "import matplotlib.cm as cm" << std::endl;
    ss << std::endl;
    ss << "# Create heatmap data" << std::endl;
    ss << "activity_matrix = np.array([" << std::endl;
    
    for (int y = heatmap.height - 1; y >= 0; --y) {
        ss << "  [";
        for (int x = 0; x < heatmap.width; ++x) {
            ss << heatmap.activityMatrix[y][x];
            if (x < heatmap.width - 1) ss << ", ";
        }
        ss << "]";
        if (y > 0) ss << ",";
        ss << std::endl;
    }
    
    ss << "])" << std::endl;
    ss << "x = np.arange(" << heatmap.width << ")" << std::endl;
    ss << "y = np.arange(" << heatmap.height << ")" << std::endl;
    ss << "X, Y = np.meshgrid(x, y)" << std::endl;
    ss << std::endl;
    ss << "# Create visualization" << std::endl;
    ss << "plt.figure(figsize=(10, 8))" << std::endl;
    ss << "plt.pcolormesh(X, Y, activity_matrix, cmap=cm.viridis, shading='auto')" << std::endl;
    ss << "plt.colorbar(label='Activity')" << std::endl;
    ss << "plt.title('NLM Neural Activity Heatmap')" << std::endl;
    ss << "plt.xlabel('X Coordinate')" << std::endl;
    ss << "plt.ylabel('Y Coordinate')" << std::endl;
    ss << "plt.show()" << std::endl;
    
    return ss.str();
}

std::string VisualizationExporter::exportNetworkMetricsAsJSON(const NetworkMetrics& metrics) {
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"average_degree\": " << metrics.avgDegree << "," << std::endl;
    ss << "  \"clustering_coefficient\": " << metrics.clusteringCoefficient << "," << std::endl;
    ss << "  \"path_length\": " << metrics.pathLength << "," << std::endl;
    ss << "  \"average_weight\": " << metrics.averageWeight << "," << std::endl;
    ss << "  \"weight_std_dev\": " << metrics.weightStdDev << "," << std::endl;
    ss << "  \"active_neuron_count\": " << metrics.activeNeuronCount << "," << std::endl;
    ss << "  \"global_efficiency\": " << metrics.globalEfficiency << std::endl;
    ss << "}" << std::endl;
    
    return ss.str();
}

std::string VisualizationExporter::exportActivityTraceAsCSV(const std::vector<float>& trace) {
    std::stringstream ss;
    
    ss << "NLM Activity Trace (CSV)" << std::endl;
    ss << "step,activity" << std::endl;
    
    for (size_t step = 0; step < trace.size(); ++step) {
        ss << step << "," << trace[step] << std::endl;
    }
    
    return ss.str();
}

void VisualizationExporter::saveToFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

} // namespace visualization
} // namespace nlm
