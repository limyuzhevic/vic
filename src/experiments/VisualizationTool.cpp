// NLM Visualization Tool
// Provides basic visualization capabilities for brain activity and simulation data

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <map>
#include "core/Logger/Logger.hpp"

using namespace nlm;

// Simple text-based visualization
class SimpleTextVisualizer {
public:
    static void visualizeNetwork(const std::vector<float>& neuronStates,
                                const std::vector<std::vector<float>>& synapses,
                                int width = 80, int height = 40) {
        if (neuronStates.empty()) {
            std::cout << "No neuron states to visualize." << std::endl;
            return;
        }
        
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                     NLM Network Visualization                     ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        
        // Find min and max values for normalization
        float minVal = *std::min_element(neuronStates.begin(), neuronStates.end());
        float maxVal = *std::max_element(neuronStates.begin(), neuronStates.end());
        float range = maxVal - minVal;
        if (range < 0.001f) range = 0.001f;
        
        // Create a simple grid representation
        std::vector<std::vector<char>> grid(height, std::vector<char>(width, ' '));
        
        // Draw neurons as nodes
        int numNodes = std::min(static_cast<size_t>(width - 10), neuronStates.size());
        float nodeSpacing = static_cast<float>(width - 20) / (numNodes - 1);
        
        for (size_t i = 0; i < numNodes && i < neuronStates.size(); ++i) {
            int x = 10 + static_cast<int>(i * nodeSpacing);
            float normalized = (neuronStates[i] - minVal) / range;
            int y = height - static_cast<int>(normalized * (height - 2));
            y = std::max(1, std::min(height - 1, y));
            
            if (normalized > 0.8) {
                grid[y][x] = '@';  // Highly active
            } else if (normalized > 0.5) {
                grid[y][x] = 'o';  // Moderately active
            } else if (normalized > 0.2) {
                grid[y][x] = '.';  // Slightly active
            } else {
                grid[y][x] = ' ';  // Inactive
            }
        }
        
        // Draw connections (simplified)
        for (size_t i = 0; i < std::min(numNodes, synapses.size()); ++i) {
            for (size_t j = 0; j < synapses[i].size() && j < numNodes; ++j) {
                if (synapses[i][j] > 0.5) {  // Strong connection
                    int x1 = 10 + static_cast<int>(i * nodeSpacing);
                    int y1 = height - 1;
                    int x2 = 10 + static_cast<int>(j * nodeSpacing);
                    int y2 = height - 1;
                    
                    // Draw line (very simplified)
                    if (abs(x1 - x2) < 3 && abs(y1 - y2) < 3) {
                        int midX = (x1 + x2) / 2;
                        int midY = (y1 + y2) / 2;
                        if (midY >= 0 && midY < height && midX >= 0 && midX < width) {
                            grid[midY][midX] = '|';
                        }
                    }
                }
            }
        }
        
        // Output the grid
        for (int y = 0; y < height; ++y) {
            std::cout << "║";
            for (int x = 0; x < width; ++x) {
                std::cout << grid[y][x];
            }
            std::cout << "║" << std::endl;
        }
        
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║ Legend: @ = Highly active, o = Active, . = Slightly active       ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
    }
    
    static void visualizeTimeSeries(const std::vector<std::pair<double, float>>& timeSeries,
                                   const std::string& title = "Time Series") {
        if (timeSeries.empty()) {
            std::cout << "No time series data to visualize." << std::endl;
            return;
        }
        
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                     " << title << "                           ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        
        // Find min and max for scaling
        float minVal = timeSeries[0].second;
        float maxVal = timeSeries[0].second;
        for (const auto& point : timeSeries) {
            if (point.second < minVal) minVal = point.second;
            if (point.second > maxVal) maxVal = point.second;
        }
        float range = maxVal - minVal;
        if (range < 0.001f) range = 0.001f;
        
        int width = 80;
        int height = 20;
        
        // Draw axes
        std::vector<std::vector<char>> grid(height, std::vector<char>(width, ' '));
        
        // Draw grid lines
        for (int y = 0; y < height; y += height / 5) {
            for (int x = 0; x < width; ++x) {
                if (y == 0 || y == height - 1) grid[y][x] = '-';
                if (x == 0 || x == width - 1) grid[y][x] = '|';
                if ((y == 0 || y == height - 1) && (x == 0 || x == width - 1)) {
                    grid[y][x] = '+';
                }
            }
        }
        
        // Plot data points
        for (size_t i = 0; i + 1 < timeSeries.size(); ++i) {
            double x1 = (timeSeries[i].first - timeSeries.front().first) / 
                       (timeSeries.back().first - timeSeries.front().first) * (width - 1);
            float y1 = (timeSeries[i].second - minVal) / range * (height - 1);
            double x2 = (timeSeries[i + 1].first - timeSeries.front().first) / 
                       (timeSeries.back().first - timeSeries.front().first) * (width - 1);
            float y2 = (timeSeries[i + 1].second - minVal) / range * (height - 1);
            
            int ix1 = std::min(static_cast<int>(x1), width - 2);
            int iy1 = height - 1 - std::min(static_cast<int>(y1), height - 1);
            int ix2 = std::min(static_cast<int>(x2), width - 2);
            int iy2 = height - 1 - std::min(static_cast<int>(y2), height - 1);
            
            if (ix1 >= 0 && ix1 < width && iy1 >= 0 && iy1 < height) {
                grid[iy1][ix1] = '*';
            }
            if (ix2 >= 0 && ix2 < width && iy2 >= 0 && iy2 < height) {
                grid[iy2][ix2] = '*';
            }
            
            // Draw line between points
            int dx = ix2 - ix1;
            int dy = iy2 - iy1;
            int steps = std::max(abs(dx), abs(dy));
            if (steps > 0) {
                float xInc = dx / static_cast<float>(steps);
                float yInc = dy / static_cast<float>(steps);
                float x = ix1;
                float y = iy1;
                for (int s = 0; s <= steps; ++s) {
                    int ix = std::round(x);
                    int iy = std::round(y);
                    if (ix >= 0 && ix < width && iy >= 0 && iy < height && (ix != ix1 || iy != iy1) && (ix != ix2 || iy != iy2)) {
                        grid[iy][ix] = '.';
                    }
                    x += xInc;
                    y += yInc;
                }
            }
        }
        
        // Output the grid
        for (int y = 0; y < height; ++y) {
            std::cout << "║";
            for (int x = 0; x < width; ++x) {
                std::cout << grid[y][x];
            }
            std::cout << "║" << std::endl;
        }
        
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║ Time Range: [" << timeSeries.front().first << ", " << timeSeries.back().first << "]               ║" << std::endl;
        std::cout << "║ Value Range: [" << minVal << ", " << maxVal << "]                        ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
    }
    
    static void exportData(const std::string& filename,
                           const std::vector<std::pair<double, float>>& timeSeries,
                           const std::string& format = "csv") {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for writing: " + filename);
        }
        
        if (format == "csv") {
            file << "time,value" << std::endl;
            for (const auto& point : timeSeries) {
                file << std::fixed << std::setprecision(6) << point.first << "," 
                     << std::fixed << std::setprecision(6) << point.second << std::endl;
            }
        } else if (format == "json") {
            file << "{" << std::endl;
            file << "  \"data\": [" << std::endl;
            for (size_t i = 0; i < timeSeries.size(); ++i) {
                const auto& point = timeSeries[i];
                file << "    {\"time\": " << std::fixed << std::setprecision(6) << point.first 
                     << ", \"value\": " << std::fixed << std::setprecision(6) << point.second << "}";
                if (i < timeSeries.size() - 1) {
                    file << "," << std::endl;
                } else {
                    file << std::endl;
                }
            }
            file << "  ]" << std::endl;
            file << "}" << std::endl;
        } else {
            file << "Time Series Data" << std::endl;
            file << "==================" << std::endl;
            for (const auto& point : timeSeries) {
                file << std::fixed << std::setprecision(6) << point.first << " " 
                     << std::fixed << std::setprecision(6) << point.second << std::endl;
            }
        }
        
        file.close();
        std::cout << "Data exported to: " << filename << std::endl;
    }
};

// Main visualization function
void runVisualization() {
    std::cout << "=== NLM Visualization Tool ===" << std::endl;
    std::cout << "Starting visualization..." << std::endl;
    std::cout << std::endl;
    
    // Initialize logger
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create sample data
    std::vector<float> neuronStates;
    std::vector<std::vector<float>> synapses;
    
    // Generate sample neuron states (simulating different activation levels)
    for (int i = 0; i < 50; ++i) {
        float state = 0.5 + (rand() % 100 - 50) / 100.0f; // Values around 0.5 with some variance
        neuronStates.push_back(state);
    }
    
    // Generate sample synapses (simplified)
    for (int i = 0; i < 50; ++i) {
        std::vector<float> row;
        for (int j = 0; j < 50; ++j) {
            float strength = 0.0f;
            if (abs(i - j) < 3) { // Local connectivity
                strength = (rand() % 100) / 100.0f * 0.8f;
            }
            row.push_back(strength);
        }
        synapses.push_back(row);
    }
    
    // Create time series data (simulating activity over time)
    std::vector<std::pair<double, float>> timeSeries;
    double startTime = 0.0;
    for (int step = 0; step < 100; ++step) {
        double time = startTime + step * 0.1; // 0.1 time units per step
        float activity = 0.5f + (rand() % 100 - 50) / 100.0f * sin(time * 0.1f); // Oscillating activity
        timeSeries.push_back(std::make_pair(time, activity));
    }
    
    // Display network visualization
    std::cout << "--- Network Visualization ---" << std::endl;
    SimpleTextVisualizer::visualizeNetwork(neuronStates, synapses, 60, 20);
    std::cout << std::endl;
    
    // Display time series visualization
    std::cout << "--- Time Series Visualization ---" << std::endl;
    SimpleTextVisualizer::visualizeTimeSeries(timeSeries, "Neural Activity Over Time");
    std::cout << std::endl;
    
    // Export data in different formats
    std::cout << "--- Data Export ---" << std::endl;
    SimpleTextVisualizer::exportData("nlm_network_data.csv", timeSeries, "csv");
    SimpleTextVisualizer::exportData("nlm_network_data.json", timeSeries, "json");
    
    std::cout << std::endl;
    std::cout << "=== Visualization Complete ===" << std::endl;
    std::cout << "Visualizations and data exports have been generated." << std::endl;
}

// Entry point for visualization tool
int main() {
    try {
        runVisualization();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
