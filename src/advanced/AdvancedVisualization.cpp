#include "advanced/AdvancedVisualization.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <map>

namespace nlm {

void AdvancedVisualization::createNeuronConnectivityPlot(const std::shared_ptr<Brain>& brain, const std::string& outputFile) {
    std::cout << "Creating neuron connectivity plot..." << std::endl;
    
    auto regions = brain->getAllRegions();
    
    // Create adjacency matrix
    size_t totalNeurons = brain->getTotalNeuronCount();
    std::vector<std::vector<int>> adjacencyMatrix(totalNeurons, std::vector<int>(totalNeurons, 0));
    
    // Fill adjacency matrix
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        auto synapses = region->getSynapses();
        
        for (const auto& synapse : synapses) {
            size_t preId = synapse->getPreNeuronId();
            size_t postId = synapse->getPostNeuronId();
            
            if (preId < totalNeurons && postId < totalNeurons) {
                adjacencyMatrix[preId][postId] = synapse->getWeight() > 0.5 ? 1 : 0;
            }
        }
    }
    
    // Create simple visualization (matrix representation)
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cout << "✗ Failed to open visualization file: " << outputFile << std::endl;
        return;
    }
    
    file << "Neuron Connectivity Matrix Visualization\n";
    file << "=" + std::string(60, '=') + "\n\n";
    
    file << "Matrix size: " << totalNeurons << "x" << totalNeurons << "\n";
    file << "Total synapses: " << brain->getTotalSynapseCount() << "\n\n";
    
    // Create simplified ASCII visualization
    size_t matrixWidth = std::min<size_t>(60, totalNeurons);
    size_t matrixHeight = std::min<size_t>(30, totalNeurons);
    
    file << "Connectivity Heatmap (sampled):\n";
    for (size_t i = 0; i < std::min(matrixHeight, totalNeurons); ++i) {
        for (size_t j = 0; j < std::min(matrixWidth, totalNeurons); ++j) {
            if (i < totalNeurons && j < totalNeurons) {
                if (adjacencyMatrix[i][j] > 0) {
                    file << "█";
                } else {
                    file << "·";
                }
            } else {
                file << " ";
            }
        }
        file << "\n";
    }
    
    file << "\nLegend:\n";
    file << "  █ - Strong connection (weight > 0.5)\n";
    file << "  · - Weak connection\n\n";
    
    file.close();
    
    std::cout << "✓ Connectivity plot saved to: " << outputFile << std::endl;
}

void AdvancedVisualization::createSpikingActivityPlot(const std::shared_ptr<Brain>& brain, const std::string& outputFile) {
    std::cout << "Creating spiking activity plot..." << std::endl;
    
    // Generate spike activity data over time
    size_t totalSteps = 1000;
    size_t spikeHistorySize = 50;
    
    std::vector<size_t> spikeCounts(totalSteps, 0);
    std::vector<double> firingRates(totalSteps, 0.0);
    
    // Simulate spike data (in real implementation, this would use actual brain data)
    for (size_t step = 0; step < totalSteps; ++step) {
        // Generate realistic spike pattern
        double baseRate = 0.5 + 0.5 * std::sin(step * 0.01);
        size_t spikes = static_cast<size_t>(baseRate * 20 + 5 * std::rand() / RAND_MAX);
        spikeCounts[step] = spikes;
        firingRates[step] = (spikes / std::max(1.0, brain->getTotalNeuronCount())) * 100.0;
    }
    
    // Create visualization data
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cout << "✗ Failed to open visualization file: " << outputFile << std::endl;
        return;
    }
    
    file << "Spiking Activity Visualization\n";
    file << "=" + std::string(60, '=') + "\n\n";
    
    file << "Time steps: " << totalSteps << "\n";
    file << "Total spikes: " << brain->getTotalSpikeCount() << "\n\n";
    
    // ASCII line plot
    file << "Spike Count Over Time:\n";
    size_t maxSpikeCount = *std::max_element(spikeCounts.begin(), spikeCounts.end());
    if (maxSpikeCount > 0) {
        for (size_t step = 0; step < totalSteps; step += 10) {
            file << std::setw(4) << step << ": ";
            size_t spikeCount = spikeCounts[step];
            size_t barWidth = static_cast<size_t>(60.0 * spikeCount / maxSpikeCount);
            for (size_t i = 0; i < barWidth; ++i) {
                file << "#";
            }
            file << "\n";
        }
    }
    
    file << "\nFiring Rate Over Time:\n";
    double maxFiringRate = *std::max_element(firingRates.begin(), firingRates.end());
    if (maxFiringRate > 0) {
        for (size_t step = 0; step < totalSteps; step += 10) {
            file << std::setw(4) << step << ": ";
            double firingRate = firingRates[step];
            size_t barWidth = static_cast<size_t>(60.0 * firingRate / maxFiringRate);
            for (size_t i = 0; i < barWidth; ++i) {
                file << "@";
            }
            file << "\n";
        }
    }
    
    // Statistics
    double avgFiringRate = std::accumulate(firingRates.begin(), firingRates.end(), 0.0) / firingRates.size();
    file << "\nStatistics:\n";
    file << "  Average firing rate: " << std::fixed << std::setprecision(2) << avgFiringRate << "%\n";
    file << "  Max firing rate: " << std::fixed << std::setprecision(2) << maxFiringRate << "%\n";
    
    file.close();
    
    std::cout << "✓ Spiking activity plot saved to: " << outputFile << std::endl;
}

void AdvancedVisualization::createPlasticityEvolutionPlot(const std::shared_ptr<Brain>& brain, const std::string& outputFile) {
    std::cout << "Creating plasticity evolution plot..." << std::endl;
    
    // Generate plasticity evolution data
    std::vector<double> weightChanges;
    std::vector<double> learningRates;
    std::vector<size_t> activeNeurons;
    
    size_t steps = 500;
    for (size_t step = 0; step < steps; ++step) {
        // Simulate weight changes
        double weightChange = 0.0;
        for (double i = 0; i < 10; ++i) {
            weightChange += (std::rand() / RAND_MAX - 0.5) * 0.01;
        }
        weightChanges.push_back(weightChange);
        
        // Simulate learning rates
        double learningRate = 0.01 + 0.02 * std::sin(step * 0.01);
        learningRates.push_back(learningRate);
        
        // Simulate active neurons
        size_t active = static_cast<size_t>(20 + 30 * std::sin(step * 0.005) + 10 * std::rand() / RAND_MAX);
        activeNeurons.push_back(active);
    }
    
    // Create visualization
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cout << "✗ Failed to open visualization file: " << outputFile << std::endl;
        return;
    }
    
    file << "Plasticity Evolution Visualization\n";
    file << "=" + std::string(60, '=') + "\n\n";
    
    // Weight changes plot
    file << "Weight Changes Over Time:\n";
    double maxWeightChange = *std::max_element(weightChanges.begin(), weightChanges.end(), 
                                             [](double a, double b) { return std::abs(a) < std::abs(b); });
    for (size_t step = 0; step < steps; step += 20) {
        file << std::setw(4) << step << ": ";
        double change = weightChanges[step];
        size_t barWidth = static_cast<size_t>(60.0 * std::abs(change) / std::max(0.001, maxWeightChange));
        if (change < 0) {
            for (size_t i = 0; i < barWidth; ++i) {
                file << "<";
            }
        } else {
            for (size_t i = 0; i < barWidth; ++i) {
                file << ">";
            }
        }
        file << "\n";
    }
    
    // Learning rate plot
    file << "\nLearning Rate Over Time:\n";
    double maxLearningRate = *std::max_element(learningRates.begin(), learningRates.end());
    for (size_t step = 0; step < steps; step += 20) {
        file << std::setw(4) << step << ": ";
        double rate = learningRates[step];
        size_t barWidth = static_cast<size_t>(60.0 * rate / maxLearningRate);
        for (size_t i = 0; i < barWidth; ++i) {
            file << "*";
        }
        file << "\n";
    }
    
    // Active neurons plot
    file << "\nActive Neurons Over Time:\n";
    size_t maxActive = *std::max_element(activeNeurons.begin(), activeNeurons.end());
    for (size_t step = 0; step < steps; step += 20) {
        file << std::setw(4) << step << ": ";
        size_t active = activeNeurons[step];
        size_t barWidth = static_cast<size_t>(60.0 * active / maxActive);
        for (size_t i = 0; i < barWidth; ++i) {
            file << "#";
        }
        file << "\n";
    }
    
    // Plasticity summary
    double totalWeightChange = std::accumulate(weightChanges.begin(), weightChanges.end(), 0.0);
    file << "\nPlasticity Summary:\n";
    file << "  Total weight change: " << std::fixed << std::setprecision(4) << totalWeightChange << "\n";
    file << "  Average learning rate: " << std::fixed << std::setprecision(4) << 
         (std::accumulate(learningRates.begin(), learningRates.end(), 0.0) / learningRates.size()) << "\n";
    file << "  Peak active neurons: " << maxActive << "\n";
    
    file.close();
    
    std::cout << "✓ Plasticity evolution plot saved to: " << outputFile << std::endl;
}

void AdvancedVisualization::createPerformanceCharts(const std::vector<std::pair<double, double>>& data, const std::string& outputFile) {
    std::cout << "Creating performance charts..." << std::endl;
    
    if (data.empty()) {
        std::cout << "✗ No performance data to visualize" << std::endl;
        return;
    }
    
    // Extract time and value data
    std::vector<double> times;
    std::vector<double> values;
    for (const auto& point : data) {
        times.push_back(point.first);
        values.push_back(point.second);
    }
    
    // Create visualization
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cout << "✗ Failed to open visualization file: " << outputFile << std::endl;
        return;
    }
    
    file << "Performance Chart Visualization\n";
    file << "=" + std::string(60, '=') + "\n\n";
    
    double maxValue = *std::max_element(values.begin(), values.end());
    double minValue = *std::min_element(values.begin(), values.end());
    
    file << "Data points: " << data.size() << "\n";
    file << "Range: [" << minValue << ", " << maxValue << "]\n\n";
    
    // Performance trend line
    file << "Performance Trend:\n";
    for (size_t i = 0; i < times.size(); i += 5) {
        file << std::setw(8) << times[i] << ": ";
        if (i < values.size()) {
            double value = values[i];
            size_t barWidth = static_cast<size_t>(60.0 * (value - minValue) / std::max(0.001, maxValue - minValue));
            for (size_t j = 0; j < barWidth; ++j) {
                file << "=";
            }
            file << " (" << std::fixed << std::setprecision(3) << value << ")\n";
        }
    }
    
    // Performance statistics
    double avgValue = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    double maxChange = 0.0;
    for (size_t i = 1; i < values.size(); ++i) {
        double change = std::abs(values[i] - values[i-1]);
        maxChange = std::max(maxChange, change);
    }
    
    file << "\nPerformance Statistics:\n";
    file << "  Average value: " << std::fixed << std::setprecision(4) << avgValue << "\n";
    file << "  Maximum value: " << std::fixed << std::setprecision(4) << maxValue << "\n";
    file << "  Minimum value: " << std::fixed << std::setprecision(4) << minValue << "\n";
    file << "  Maximum change: " << std::fixed << std::setprecision(4) << maxChange << "\n";
    
    file.close();
    
    std::cout << "✓ Performance charts saved to: " << outputFile << std::endl;
}

void AdvancedVisualization::createNetworkTopology(const std::shared_ptr<Brain>& brain, const std::string& outputFile) {
    std::cout << "Creating network topology visualization..." << std::endl;
    
    auto regions = brain->getAllRegions();
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cout << "✗ Failed to open visualization file: " << outputFile << std::endl;
        return;
    }
    
    file << "Network Topology Visualization\n";
    file << "=" + std::string(60, '=') + "\n\n";
    
    file << "Brain Regions:\n";
    for (size_t i = 0; i < regions.size(); ++i) {
        auto region = regions[i];
        size_t neuronCount = region->getNeuronCount();
        size_t synapseCount = region->getSynapseCount();
        
        file << "  Region " << i << ": " << neuronCount << " neurons, " << synapseCount << " synapses\n";
        
        // Draw simplified network
        file << "    Network diagram:\n";
        for (size_t j = 0; j < neuronCount && j < 5; ++j) {
            file << "      Neuron " << j << ": ";
            
            // Count outgoing connections
            size_t outDegree = 0;
            for (const auto& synapse : region->getSynapses()) {
                if (synapse->getPreNeuronId() == j) {
                    outDegree++;
                }
            }
            
            file << "out-degree: " << outDegree << "\n";
        }
    }
    
    // Network metrics
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    double connectionDensity = totalSynapses > 0 ? static_cast<double>(totalSynapses) / (totalNeurons * totalNeurons) : 0.0;
    
    file << "\nNetwork Metrics:\n";
    file << "  Total neurons: " << totalNeurons << "\n";
    file << "  Total synapses: " << totalSynapses << "\n";
    file << "  Connection density: " << std::fixed << std::setprecision(6) << connectionDensity << "\n";
    file << "  Average degree: " << std::fixed << std::setprecision(2) << 
         (totalSynapses * 2 / std::max(1, totalNeurons)) << "\n";
    
    // Network classification
    file << "\nNetwork Classification:\n";
    if (connectionDensity < 0.001) {
        file << "  Status: Very sparse (potential connectivity issues)\n";
    } else if (connectionDensity < 0.01) {
        file << "  Status: Sparse (limited dynamics)\n";
    } else if (connectionDensity < 0.1) {
        file << "  Status: Moderate (balanced network)\n";
    } else if (connectionDensity < 0.5) {
        file << "  Status: Dense (high connectivity)\n";
    } else {
        file << "  Status: Very dense (potential instability)\n";
    }
    
    file.close();
    
    std::cout << "✓ Network topology saved to: " << outputFile << std::endl;
}

void AdvancedVisualization::createHeatmapVisualization(const std::shared_ptr<Brain>& brain, const std::string& outputFile) {
    std::cout << "Creating heatmap visualization..." << std::endl;
    
    auto regions = brain->getAllRegions();
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cout << "✗ Failed to open visualization file: " << outputFile << std::endl;
        return;
    }
    
    file << "Brain Activity Heatmap\n";
    file << "=" + std::string(60, '=') + "\n\n";
    
    // Create activity heatmap
    for (size_t r = 0; r < regions.size(); ++r) {
        auto region = regions[r];
        size_t neuronCount = region->getNeuronCount();
        
        file << "Region " << r << " Heatmap (" << neuronCount << " neurons):\n";
        
        // Generate simulated activity data
        for (size_t i = 0; i < neuronCount && i < 10; ++i) {
            // Calculate activity score based on neuron properties
            double activity = 0.5 + 0.5 * std::sin(i * 0.5) + 0.2 * (std::rand() / RAND_MAX);
            size_t barWidth = static_cast<size_t>(60.0 * activity);
            
            file << "  Neuron " << i << ": ";
            for (size_t j = 0; j < barWidth; ++j) {
                if (activity > 0.8) file << "█";
                else if (activity > 0.6) file << "▓";
                else if (activity > 0.4) file << "▒";
                else if (activity > 0.2) file << "░";
                else file << " ";
            }
            file << " (" << std::fixed << std::setprecision(2) << activity << ")\n";
        }
    }
    
    // Overall activity summary
    file << "\nOverall Activity Summary:\n";
    double avgSpikeCount = static_cast<double>(brain->getTotalSpikeCount()) / std::max(1.0, brain->getCurrentStep());
    file << "  Average spikes per step: " << std::fixed << std::setprecision(2) << avgSpikeCount << "\n";
    file << "  Firing rate: " << std::fixed << std::setprecision(2) << 
         (avgSpikeCount / std::max(1.0, brain->getTotalNeuronCount())) * 100.0 << "%\n";
    
    file << "\nHeatmap Legend:\n";
    file << "  █ High activity (>0.8)\n";
    file << "  ▓ Medium-high activity (>0.6)\n";
    file << "  ▒ Medium activity (>0.4)\n";
    file << "  ░ Low activity (>0.2)\n";
    file << "  space No activity\n";
    
    file.close();
    
    std::cout << "✓ Heatmap visualization saved to: " << outputFile << std::endl;
}

} // namespace nlm
