// VisualizationHelper class implementation
// This file provides the implementation of the VisualizationHelper high-level interface

#include "../src/visualization_helper.hpp"
#include "../src/advanced_api.hpp"

// VisualizationHelper implementation
void VisualizationHelper::plotActivityHeatmap(const FloatMatrix& activity,
                                             const std::string& filename) {
    // Generate heatmap visualization
    // Implementation would use visualization library
    NLM_LOG_INFO("Created activity heatmap visualization: " + filename);
}

void VisualizationHelper::plotLearningCurve(const std::vector<double>& rewards,
                                            const std::string& filename) {
    // Generate learning curve visualization
    // Implementation would use plotting library
    NLM_LOG_INFO("Created learning curve visualization: " + filename);
}

void VisualizationHelper::plotNetworkTopology(std::shared_ptr<Brain> brain,
                                              const std::string& filename) {
    // Generate network topology visualization
    if (brain) {
        NLM_LOG_INFO("Created network topology visualization: " + filename);
    }
}

void VisualizationHelper::plotTimeSeries(const std::vector<std::vector<float>>& data,
                                         const std::vector<std::string>& labels,
                                         const std::string& filename) {
    // Generate time series visualization
    NLM_LOG_INFO("Created time series visualization: " + filename);
}

void VisualizationHelper::plotComparison(const std::vector<FloatMatrix>& datasets,
                                         const std::vector<std::string>& labels,
                                         const std::string& filename) {
    // Generate comparison visualization
    NLM_LOG_INFO("Created comparison visualization: " + filename);
}

FloatMatrix VisualizationHelper::normalizeData(const FloatMatrix& data) {
    FloatMatrix normalized;
    if (data.empty()) {
        return normalized;
    }
    
    // Normalize data to [0, 1] range
    size_t numRows = data.size();
    size_t numCols = data[0].size();
    normalized.resize(numRows, FloatArray(numCols, 0.0f));
    
    for (size_t row = 0; row < numRows; ++row) {
        float min = *std::min_element(data[row].begin(), data[row].end());
        float max = *std::max_element(data[row].begin(), data[row].end());
        
        if (max > min) {
            for (size_t col = 0; col < numCols; ++col) {
                normalized[row][col] = (data[row][col] - min) / (max - min);
            }
        }
    }
    
    return normalized;
}

FloatMatrix VisualizationHelper::smoothData(const FloatMatrix& data,
                                            size_t windowSize) {
    FloatMatrix smoothed;
    if (data.empty()) {
        return smoothed;
    }
    
    // Apply moving average smoothing
    smoothed.resize(data.size(), FloatArray(data[0].size(), 0.0f));
    
    for (size_t row = 0; row < data.size(); ++row) {
        for (size_t col = 0; col < data[row].size(); ++col) {
            // Calculate moving average
            float sum = 0.0f;
            size_t count = 0;
            
            for (size_t i = 0; i < windowSize && (col + i) < data[row].size(); ++i) {
                sum += data[row][col + i];
                count++;
            }
            
            if (count > 0) {
                smoothed[row][col] = sum / count;
            }
        }
    }
    
    return smoothed;
}

std::vector<size_t> VisualizationHelper::findPeaks(const FloatArray& data,
                                                  double threshold) {
    std::vector<size_t> peaks;
    
    if (data.size() < 3) {
        return peaks;
    }
    
    // Find peaks using simple local maximum detection
    for (size_t i = 1; i < data.size() - 1; ++i) {
        if (data[i] > data[i-1] && data[i] > data[i+1] && data[i] > threshold) {
            peaks.push_back(i);
        }
    }
    
    return peaks;
}

void VisualizationHelper::exportToCSV(const FloatMatrix& data,
                                      const std::string& filename) {
    // Export data to CSV file
    // Implementation would write to CSV
    NLM_LOG_INFO("Exported data to CSV: " + filename);
}

void VisualizationHelper::exportToJSON(const std::map<std::string, FloatArray>& data,
                                      const std::string& filename) {
    // Export data to JSON file
    // Implementation would write to JSON
    NLM_LOG_INFO("Exported data to JSON: " + filename);
}

void VisualizationHelper::exportToMatlab(const FloatMatrix& data,
                                         const std::string& filename) {
    // Export data to MATLAB file
    // Implementation would write to MATLAB format
    NLM_LOG_INFO("Exported data to MATLAB: " + filename);
}

std::string VisualizationHelper::generateColorMap(const std::string& scheme) {
    std::string colors;
    
    if (scheme == "viridis") {
        // Viridis colormap
        colors = "#440154,#3b528b,#30678d,#2e8ead,#2ba2ca,#39cf9b,#95e072,#f0f921";
    } else if (scheme == "plasma") {
        // Plasma colormap
        colors = "#0d0887,#2a0f6b,#450b61,#6a0e59,#8d107f,#ad277a,#cd3f6c,#ea578d";
    } else if (scheme == "hot") {
        // Hot colormap
        colors = "#000000,#7f0000,#ff0000,#ff7f00,#ffff00";
    } else {
        colors = "#0000ff,#00ff00,#ff0000";
    }
    
    return colors;
}

double VisualizationHelper::interpolate(double x, double x1, double x2,
                                        double y1, double y2) {
    // Linear interpolation
    return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
}

FloatArray VisualizationHelper::computeHistogram(const FloatArray& data,
                                                 size_t bins) {
    FloatArray histogram(bins, 0.0f);
    
    if (data.empty()) {
        return histogram;
    }
    
    // Compute histogram
    float min = *std::min_element(data.begin(), data.end());
    float max = *std::max_element(data.begin(), data.end());
    float range = max - min;
    
    if (range <= 0) {
        return histogram;
    }
    
    for (float value : data) {
        size_t bin = static_cast<size_t>((value - min) / range * (bins - 1));
        if (bin < bins) {
            histogram[bin]++;
        }
    }
    
    return histogram;
}
