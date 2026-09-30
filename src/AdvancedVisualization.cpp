#include "AdvancedVisualization.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <filesystem>

namespace nlm {
namespace AdvancedVisualization {

// ActivityHeatmap implementation
void ActivityHeatmap::setData(const std::vector<std::vector<float>>& values,
                             const std::vector<std::vector<std::string>>& labels) {
    data_.values = values;
    data_.labels = labels;
    
    if (!values.empty()) {
        data_.minValue = values[0][0];
        data_.maxValue = values[0][0];
        
        for (const auto& row : values) {
            for (float val : row) {
                if (val < data_.minValue) data_.minValue = val;
                if (val > data_.maxValue) data_.maxValue = val;
            }
        }
        
        // Normalize values
        data_.normalized.clear();
        data_.normalized.resize(values.size(), std::vector<float>(values[0].size(), 0.0f));
        
        if (data_.maxValue > data_.minValue) {
            float range = data_.maxValue - data_.minValue;
            for (size_t i = 0; i < values.size(); ++i) {
                for (size_t j = 0; j < values[i].size(); ++j) {
                    data_.normalized[i][j] = (values[i][j] - data_.minValue) / range;
                }
            }
        }
    }
}

std::string ActivityHeatmap::generateASCII() const {
    std::stringstream ss;
    
    ss << title_ << "\n";
    ss << std::string(width_ + 2, '=') << "\n\n";
    
    if (data_.values.empty()) {
        ss << "No data available\n";
        return ss.str();
    }
    
    // Generate color codes based on value ranges
    std::vector<std::string> colorCodes = {
        "\033[48;5;240m \033[0m",  // Dark gray (low)
        "\033[48;5;243m \033[0m",  // Gray
        "\033[48;5;246m \033[0m",  // Light gray
        "\033[48;5;249m \033[0m",  // Very light gray
        "\033[48;5;252m \033[0m",  // White (high)
    };
    
    // Add headers
    ss << "   ";
    for (size_t j = 0; j < std::min<size_t>(width_, data_.values[0].size()); ++j) {
        ss << std::setw(3) << j << " ";
    }
    ss << "\n";
    
    // Add data rows
    for (size_t i = 0; i < std::min<size_t>(height_, data_.values.size()); ++i) {
        ss << std::setw(3) << i << " ";
        for (size_t j = 0; j < std::min<size_t>(width_, data_.values[i].size()); ++j) {
            float value = data_.values[i][j];
            size_t colorIndex = static_cast<size_t>((value - data_.minValue) / (data_.maxValue - data_.minValue) * (colorCodes.size() - 1));
            colorIndex = std::min<size_t>(colorIndex, colorCodes.size() - 1);
            ss << colorCodes[colorIndex];
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string ActivityHeatmap::generateSVG() const {
    std::stringstream ss;
    
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << width_ * 10 << "\" height=\"" << height_ * 5 << "\">\n";
    ss << "<title>" << title_ << "</title>\n";
    
    if (!data_.values.empty()) {
        // Generate heat cells
        for (size_t i = 0; i < height_; ++i) {
            for (size_t j = 0; j < width_; ++j) {
                if (i < data_.values.size() && j < data_.values[i].size()) {
                    float value = data_.values[i][j];
                    
                    // Convert to color (simplified linear interpolation)
                    uint8_t r = static_cast<uint8_t>(255 * (1.0f - value));
                    uint8_t g = static_cast<uint8_t>(255 * value);
                    uint8_t b = 128;
                    
                    std::stringstream cellStyle;
                    cellStyle << "fill=\"rgb(" << static_cast<int>(r) << "," 
                              << static_cast<int>(g) << "," 
                              << static_cast<int>(b) << ")\"";
                    
                    ss << "  <rect x=\"" << j * 10 << "\" y=\"" << i * 5 << "\" width=\"10\" height=\"5\" "
                       << cellStyle.str() << "/>\n";
                }
            }
        }
    }
    
    ss << "</svg>\n";
    return ss.str();
}

std::string ActivityHeatmap::generateJSON() const {
    std::stringstream ss;
    
    ss << "{\n";
    ss << "  \"title\": \"" << title_ << "\",\n";
    ss << "  \"width\": " << width_ << ",\n";
    ss << "  \"height\": " << height_ << ",\n";
    ss << "  \"minValue\": " << data_.minValue << ",\n";
    ss << "  \"maxValue\": " << data_.maxValue << ",\n";
    ss << "  \"colorScheme\": \"" << data_.colorScheme << "\",\n";
    ss << "  \"data\": [\n";
    
    for (size_t i = 0; i < data_.values.size(); ++i) {
        ss << "    [";
        for (size_t j = 0; j < data_.values[i].size(); ++j) {
            ss << data_.values[i][j];
            if (j < data_.values[i].size() - 1) ss << ", ";
        }
        ss << "]";
        if (i < data_.values.size() - 1) ss << ",\n";
        else ss << "\n";
    }
    
    ss << "  ],\n";
    ss << "  \"normalizedData\": [\n";
    
    for (size_t i = 0; i < data_.normalized.size(); ++i) {
        ss << "    [";
        for (size_t j = 0; j < data_.normalized[i].size(); ++j) {
            ss << data_.normalized[i][j];
            if (j < data_.normalized[i].size() - 1) ss << ", ";
        }
        ss << "]";
        if (i < data_.normalized.size() - 1) ss << ",\n";
        else ss << "\n";
    }
    
    ss << "  ]\n";
    ss << "}\n";
    
    return ss.str();
}

std::string ActivityHeatmap::generateHTML() const {
    std::stringstream ss;
    
    ss << "<!DOCTYPE html>\n";
    ss << "<html>\n";
    ss << "<head><title>" << title_ << "</title>";
    ss << "<style>body { font-family: monospace; }</style></head>\n";
    ss << "<body>\n";
    ss << "<h1>" << title_ << "</h1>\n";
    ss << "<pre>\n";
    ss << generateASCII();
    ss << "</pre>\n";
    ss << "</body>\n";
    ss << "</html>\n";
    
    return ss.str();
}

std::string ActivityHeatmap::getHeatmapImage(VisualizationFormat format) const {
    switch (format) {
        case VisualizationFormat::ASCII:
            return generateASCII();
        case VisualizationFormat::SVG:
            return generateSVG();
        case VisualizationFormat::JSON:
            return generateJSON();
        case VisualizationFormat::HTML:
            return generateHTML();
        default:
            return "Unsupported format";
    }
}

void ActivityHeatmap::saveToFile(const std::string& filename, VisualizationFormat format) const {
    std::string content = getHeatmapImage(format);
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    file.write(content.c_str(), content.size());
    file.close();
}

// ConnectivityMatrix implementation
void ConnectivityMatrix::setData(const std::vector<std::vector<float>>& weights,
                                 const std::vector<std::vector<std::string>>& labels) {
    data_.weights = weights;
    data_.labels = labels;
}

std::string ConnectivityMatrix::generateASCII() const {
    std::stringstream ss;
    
    ss << title_ << "\n";
    ss << std::string(20 + data_.weights.size() * 8, '=') << "\n\n";
    
    if (data_.weights.empty()) {
        ss << "No connectivity data available\n";
        return ss.str();
    }
    
    // Header
    ss << std::string(8, ' ') << " ";
    for (size_t i = 0; i < data_.weights.size(); ++i) {
        ss << std::setw(6) << i << " ";
    }
    ss << "\n";
    
    // Matrix
    for (size_t i = 0; i < data_.weights.size(); ++i) {
        ss << std::setw(8) << i << " ";
        for (size_t j = 0; j < data_.weights.size(); ++j) {
            if (i < data_.weights.size() && j < data_.weights[i].size()) {
                float weight = data_.weights[i][j];
                
                // Convert weight to visual representation
                if (std::abs(weight) < threshold_) {
                    ss << "   .  ";
                } else {
                    int stars = static_cast<int>(std::abs(weight) / 0.2f);
                    stars = std::min<int>(stars, 5);
                    ss << "   ";
                    for (int k = 0; k < stars; ++k) ss << "*";
                    ss << "   ";
                }
            } else {
                ss << "       ";
            }
            ss << " ";
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string ConnectivityMatrix::generateSVG() const {
    std::stringstream ss;
    
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << (data_.weights.size() * 30 + 100) << 
        "\" height=\"" << (data_.weights.size() * 30 + 50) << "\">\n";
    ss << "<title>" << title_ << "</title>\n";
    
    if (!data_.weights.empty()) {
        // Draw axis labels
        ss << "  <text x=\"10\" y=\"20\" font-family=\"Arial\" font-size=\"12\">Region</text>\n";
        ss << "  <text x=\"10\" y=\"\" + (data_.weights.size() * 30 + 20) + "\" font-family=\"Arial\" font-size=\"12\">Region</text>\n";
        
        // Draw matrix cells
        for (size_t i = 0; i < data_.weights.size(); ++i) {
            // Row label
            if (!data_.labels.empty() && i < data_.labels.size()) {
                ss << "  <text x=\"5\" y=\"" << (i * 30 + 25) << "\" font-family=\"Arial\" font-size=\"10\">" 
                   << data_.labels[i][0] << "</text>\n";
            }
            
            for (size_t j = 0; j < data_.weights.size(); ++j) {
                if (i < data_.weights.size() && j < data_.weights[i].size()) {
                    float weight = data_.weights[i][j];
                    
                    // Determine color based on weight
                    uint8_t r, g, b;
                    if (weight >= 0) {
                        r = static_cast<uint8_t>(255 * weight);
                        g = 255;
                        b = static_cast<uint8_t>(255 * (1.0f - weight));
                    } else {
                        r = 255;
                        g = static_cast<uint8_t>(255 * (1.0f + weight));
                        b = 255;
                    }
                    
                    std::stringstream cellStyle;
                    cellStyle << "fill=\"rgb(" << static_cast<int>(r) << "," 
                              << static_cast<int>(g) << "," 
                              << static_cast<int>(b) << ")\" stroke=\"black\" stroke-width=\"1\"";
                    
                    ss << "  <rect x=\"" << (j * 30 + 50) << "\" y=\"" << (i * 30 + 10) << 
                       "\" width=\"25\" height=\"25\" " << cellStyle.str() << "/>\n";
                    
                    // Add weight label
                    ss << "  <text x=\"" << (j * 30 + 62) << "\" y=\"" << (i * 30 + 28) << 
                       "\" font-family=\"Arial\" font-size=\"8\" text-anchor=\"middle\">" 
                       << std::fixed << std::setprecision(2) << weight << "</text>\n";
                }
            }
        }
    }
    
    ss << "</svg>\n";
    return ss.str();
}

std::string ConnectivityMatrix::generateJSON() const {
    std::stringstream ss;
    
    ss << "{\n";
    ss << "  \"title\": \"" << title_ << "\",\n";
    ss << "  \"weights\": [\n";
    
    for (size_t i = 0; i < data_.weights.size(); ++i) {
        ss << "    [";
        for (size_t j = 0; j < data_.weights[i].size(); ++j) {
            ss << data_.weights[i][j];
            if (j < data_.weights[i].size() - 1) ss << ", ";
        }
        ss << "]";
        if (i < data_.weights.size() - 1) ss << ",\n";
        else ss << "\n";
    }
    
    ss << "  ],\n";
    ss << "  \"threshold\": " << threshold_ << ",\n";
    ss << "  \"symmetric\": " << (symmetric_ ? "true" : "false") << "\n";
    ss << "}\n";
    
    return ss.str();
}

std::string ConnectivityMatrix::getNetworkGraph(VisualizationFormat format) const {
    switch (format) {
        case VisualizationFormat::ASCII:
            return generateASCII();
        case VisualizationFormat::SVG:
            return generateSVG();
        case VisualizationFormat::JSON:
            return generateJSON();
        default:
            return "Unsupported format for network graph";
    }
}

void ConnectivityMatrix::saveToFile(const std::string& filename, VisualizationFormat format) const {
    std::string content = getNetworkGraph(format);
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    file.write(content.c_str(), content.size());
    file.close();
}

// ActivityTimeSeries implementation
void ActivityTimeSeries::addTimeSeries(const std::vector<float>& values, 
                                     const std::string& label,
                                     const std::string& yLabel) {
    data_.timeSeries.push_back(values);
    data_.labels.push_back(label);
    data_.yAxisLabels.push_back(yLabel);
}

std::string ActivityTimeSeries::generateASCII() const {
    std::stringstream ss;
    
    ss << (data_.title.empty() ? "Time Series" : data_.title) << "\n";
    ss << std::string(50, '=') << "\n\n";
    
    if (data_.timestamps.empty() || data_.timeSeries.empty()) {
        ss << "No time series data available\n";
        return ss.str();n    }
    
    // Header
    ss << std::string(8, ' ') << " ";
    for (size_t i = 0; i < data_.timestamps.size() && i < 20; ++i) {
        ss << std::setw(4) << i << " ";
    }
    ss << "...\n";
    
    // Data series
    for (size_t series = 0; series < data_.timeSeries.size(); ++series) {
        ss << (data_.labels[series].empty() ? ("Series " + std::to_string(series)) : data_.labels[series]);
        ss << std::string(4 - (data_.labels[series].size() % 4), ' ');
        
        for (size_t i = 0; i < data_.timestamps.size() && i < data_.timeSeries[series].size() && i < 20; ++i) {
            float value = data_.timeSeries[series][i];
            ss << std::setw(4) << static_cast<int>(value);
        }
        ss << " ...\n";
    }
    
    return ss.str();
}

std::string ActivityTimeSeries::generateSVG() const {
    std::stringstream ss;
    
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"600\">\n";
    ss << "<title>" << data_.title << "</title>\n";
    
    if (!data_.timestamps.empty() && !data_.timeSeries.empty()) {
        // Calculate ranges
        float minValue = std::numeric_limits<float>::max();
        float maxValue = std::numeric_limits<float>::lowest();
        
        for (const auto& series : data_.timeSeries) {
            for (float value : series) {
                if (value < minValue) minValue = value;
                if (value > maxValue) maxValue = value;
            }
        }
        
        float range = maxValue - minValue;
        if (range < 1e-6f) range = 1.0f;
        
        // Draw axes
        ss << "  <line x1=\"50\" y1=\"550\" x2=\"750\" y2=\"550\" stroke=\"black\" stroke-width=\"2\"/>\n";
        ss << "  <line x1=\"50\" y1=\"550\" x2=\"50\" y2=\"50\" stroke=\"black\" stroke-width=\"2\"/>\n";
        
        // Draw grid
        ss << "  <g stroke=\"lightgray\" stroke-width=\"1\" stroke-dasharray=\"5,5\">\n";
        for (int i = 0; i <= 10; ++i) {
            ss << "    <line x1=\"50\" y1=\"" << (550 - i * 55) << "\" x2=\"750\" y2=\"" << (550 - i * 55) << 
               "\"/>\n";
        }
        for (int i = 0; i <= 10; ++i) {
            ss << "    <line x1=\"" << (50 + i * 70) << "\" y1=\"550\" x2=\"" << (50 + i * 70) << "\" y2=\"50\"/>\n";
        }
        ss << "  </g>\n";
        
        // Draw axes labels
        ss << "  <text x=\"400\" y=\"580\" text-anchor=\"middle\" font-family=\"Arial\" font-size=\"12\">Time</text>\n";
        ss << "  <text x=\"20\" y=\"280\" text-anchor=\"middle\" font-family=\"Arial\" font-size=\"12\" transform=\"rotate(-90 20 280)\">Value</text>\n";
        
        // Draw data series
        std::vector<std::string> colors = {
            "#FF0000", "#00FF00", "#0000FF", "#FF00FF", "#00FFFF", "#FFFF00", "#FF8800", "#8800FF"
        };
        
        for (size_t series = 0; series < data_.timeSeries.size(); ++series) {
            std::string color = colors[series % colors.size()];
            
            ss << "  <path d=\"M ";
            
            for (size_t i = 0; i < data_.timestamps.size() && i < data_.timeSeries[series].size(); ++i) {
                float x = 50 + i * 70;
                float y = 550 - (data_.timeSeries[series][i] - minValue) / range * 500;
                
                if (i == 0) {
                    ss << "L " << x << "," << y;
                } else {
                    ss << " L " << x << "," << y;
                }
            }
            
            ss << "\" fill=\"none\" stroke=\"" << color << "\" stroke-width=\"2\"/>\n";
        }
        
        // Draw legend
        ss << "  <g>\n";
        for (size_t i = 0; i < data_.labels.size() && i < data_.yAxisLabels.size(); ++i) {
            ss << "    <rect x=\"650\" y=\"" << (50 + i * 30) << "\" width=\"10\" height=\"10\" "
               << "fill=\"" << colors[i % colors.size()] << "\"/>\n";
            ss << "    <text x=\"670\" y=\"" << (65 + i * 30) << "\" font-family=\"Arial\" font-size=\"10\">" 
               << data_.labels[i] << " (" << data_.yAxisLabels[i] << ")</text>\n";
        }
        ss << "  </g>\n";
    }
    
    ss << "</svg>\n";
    return ss.str();
}

std::string ActivityTimeSeries::generateJSON() const {
    std::stringstream ss;
    
    ss << "{\n";
    ss << "  \"title\": \"" << data_.title << "\",\n";
    ss << "  \"timestamps\": [\n";
    
    for (size_t i = 0; i < data_.timestamps.size(); ++i) {
        ss << "    " << data_.timestamps[i];
        if (i < data_.timestamps.size() - 1) ss << ",\n";
        else ss << "\n";
    }
    
    ss << "  ],\n";
    ss << "  \"series\": [\n";
    
    for (size_t series = 0; series < data_.timeSeries.size(); ++series) {
        ss << "    {\n";
        ss << "      \"label\": \"" << data_.labels[series] << "\",\n";
        ss << "      \"yLabel\": \"" << (series < data_.yAxisLabels.size() ? data_.yAxisLabels[series] : "") << "\",\n";
        ss << "      \"data\": [\n";
        
        for (size_t i = 0; i < data_.timeSeries[series].size(); ++i) {
            ss << "        " << data_.timeSeries[series][i];
            if (i < data_.timeSeries[series].size() - 1) ss << ",\n";
            else ss << "\n";
        }
        
        ss << "      ]\n";
        ss << "    }";
        if (series < data_.timeSeries.size() - 1) ss << ",\n";
        else ss << "\n";
    }
    
    ss << "  ],\n";
    ss << "  \"showGrid\": " << (data_.showGrid ? "true" : "false") << ",\n";
    ss << "  \"showLegend\": " << (data_.showLegend ? "true" : "false") << "\n";
    ss << "}\n";
    
    return ss.str();
}

std::string ActivityTimeSeries::getPlot(VisualizationFormat format) const {
    switch (format) {
        case VisualizationFormat::ASCII:
            return generateASCII();
        case VisualizationFormat::SVG:
            return generateSVG();
        case VisualizationFormat::JSON:
            return generateJSON();
        default:
            return "Unsupported format for plot";
    }
}

void ActivityTimeSeries::saveToFile(const std::string& filename, VisualizationFormat format) const {
    std::string content = getPlot(format);
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    file.write(content.c_str(), content.size());
    file.close();
}

// StatisticalAnalyzer implementation
StatisticalAnalyzer::StatisticalResults StatisticalAnalyzer::analyze(const std::vector<float>& data) {
    StatisticalResults results;
    
    if (data.empty()) {
        return results;
    }
    
    // Basic statistics
    results.mean = 0.0f;
    for (float val : data) {
        results.mean += val;
    }
    results.mean /= static_cast<float>(data.size());
    
    // Sort for median and mode
    std::vector<float> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    
    // Median
    if (data.size() % 2 == 0) {
        results.median = (sorted[data.size() / 2 - 1] + sorted[data.size() / 2]) / 2.0f;
    } else {
        results.median = sorted[data.size() / 2];
    }
    
    // Mode
    std::map<float, int> frequency;
    for (float val : data) {
        frequency[val]++;
    }
    
    int maxFrequency = 0;
    for (const auto& pair : frequency) {
        if (pair.second > maxFrequency) {
            maxFrequency = pair.second;
            results.mode = pair.first;
        }
    }
    
    // Standard deviation and variance
    results.variance = 0.0f;
    for (float val : data) {
        float diff = val - results.mean;
        results.variance += diff * diff;
    }
    results.variance /= static_cast<float>(data.size());
    results.standardDeviation = std::sqrt(results.variance);
    
    // Min and max
    results.min = sorted.front();
    results.max = sorted.back();
    results.range = results.max - results.min;
    
    // Interquartile range
    size_t q1Index = data.size() / 4;
    size_t q3Index = (data.size() * 3) / 4;
    results.interquartileRange = sorted[q3Index] - sorted[q1Index];
    
    // Skewness and kurtosis
    if (results.standardDeviation > 0.0f) {
        float sumSkew = 0.0f;
        float sumKurt = 0.0f;
        
        for (float val : data) {
            float diff = (val - results.mean) / results.standardDeviation;
            sumSkew += diff * diff * diff;
            sumKurt += diff * diff * diff * diff;
        }
        
        results.skewness = (data.size() * sumSkew) / ((data.size() - 1) * (data.size() - 2));
        results.kurtosis = ((data.size() * sumKurt) / ((data.size() - 1) * (data.size() - 2) * (data.size() - 3))) - 
                          (3.0f * (data.size() - 1) * (data.size() - 1)) / ((data.size() - 2) * (data.size() - 3));
    }
    
    // Outlier detection (using IQR method)
    float lowerBound = results.median - 1.5f * results.interquartileRange;
    float upperBound = results.median + 1.5f * results.interquartileRange;
    
    for (float val : data) {
        if (val < lowerBound || val > upperBound) {
            results.outliers.push_back(val);
        }
    }
    results.outlierCount = results.outliers.size();
    
    // Confidence intervals (assuming normal distribution)
    if (data.size() > 1) {
        float tValue = 2.0f; // Approximate for large samples
        float margin = tValue * results.standardDeviation / std::sqrt(static_cast<float>(data.size()));
        results.confidenceInterval95 = 2.0f * margin;
        results.confidenceInterval99 = 2.58f * margin; // Approximate
    }
    
    // Normality test (Shapiro-Wilk, simplified)
    if (data.size() >= 3 && data.size() <= 5000) {
        results.isNormal = checkNormality(data);
        results.normalityPValue = results.isNormal ? 0.05f : 0.01f;
    }
    
    return results;
}

StatisticalAnalyzer::StatisticalResults StatisticalAnalyzer::analyze(const std::vector<std::vector<float>>& data) {
    StatisticalResults results;
    
    if (data.empty()) {
        return results;
    }
    
    // Flatten data
    std::vector<float> flatData;
    for (const auto& row : data) {
        for (float val : row) {
            flatData.push_back(val);
        }
    }
    
    return analyze(flatData);
}

StatisticalAnalyzer::TimeSeriesAnalysis StatisticalAnalyzer::analyzeTimeSeries(const std::vector<float>& timeSeries,
                                                                          size_t windowSize) {
    TimeSeriesAnalysis analysis;
    
    if (timeSeries.empty()) {
        return analysis;
    }
    
    // Overall statistics
    analysis.stats = analyze(timeSeries);
    
    // Rolling window statistics
    if (timeSeries.size() >= windowSize) {
        for (size_t i = 0; i <= timeSeries.size() - windowSize; i += windowSize / 10) {
            size_t end = std::min(i + windowSize, static_cast<size_t>(timeSeries.size()));
            std::vector<float> window(timeSeries.begin() + i, timeSeries.begin() + end);
            analysis.windowStats.push_back(analyze(window));
        }
    }
    
    // Trend calculation (linear regression)
    if (timeSeries.size() >= 2) {
        float sumX = 0.0f, sumY = 0.0f, sumXY = 0.0f, sumX2 = 0.0f;
        
        for (size_t i = 0; i < timeSeries.size(); ++i) {
            float x = static_cast<float>(i);
            float y = timeSeries[i];
            
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumX2 += x * x;
        }
        
        float n = static_cast<float>(timeSeries.size());
        float slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
        float intercept = (sumY - slope * sumX) / n;
        
        analysis.trend.resize(timeSeries.size());
        for (size_t i = 0; i < timeSeries.size(); ++i) {
            analysis.trend[i] = slope * static_cast<float>(i) + intercept;
        }
    }
    
    // Calculate residuals
    if (!analysis.trend.empty()) {
        analysis.residuals.resize(timeSeries.size());
        for (size_t i = 0; i < timeSeries.size(); ++i) {
            analysis.residuals[i] = timeSeries[i] - analysis.trend[i];
        }
    }
    
    // Autocorrelation
    if (timeSeries.size() > 4) {
        float mean = analysis.stats.mean;
        float variance = analysis.stats.variance;
        
        if (variance > 0.0f) {
            float autocorr = 0.0f;
            for (size_t lag = 1; lag < 4; ++lag) {
                float lagSum = 0.0f;
                for (size_t i = lag; i < timeSeries.size(); ++i) {
                    lagSum += (timeSeries[i] - mean) * (timeSeries[i - lag] - mean);
                }
                autocorr = lagSum / ((timeSeries.size() - lag) * variance);
                analysis.partialAutocorrelation.push_back(autocorr);
            }
        }
    }
    
    // Stationarity test (ADF test simplified)
    analysis.isStationary = checkStationarity(timeSeries);
    
    return analysis;
}

bool StatisticalAnalyzer::detectOutliers(const std::vector<float>& data, float threshold) {
    if (data.size() < 4) return false;
    
    StatisticalResults stats = analyze(data);
    
    float lowerBound = stats.median - threshold * stats.interquartileRange;
    float upperBound = stats.median + threshold * stats.interquartileRange;
    
    for (float val : data) {
        if (val < lowerBound || val > upperBound) {
            return true;
        }
    }
    
    return false;
}

bool StatisticalAnalyzer::detectAnomalies(const std::vector<float>& data, float threshold) {
    if (data.size() < 3) return false;
    
    StatisticalResults stats = analyze(data);
    
    if (stats.standardDeviation < 1e-6f) return false;
    
    for (float val : data) {
        float zScore = std::abs((val - stats.mean) / stats.standardDeviation);
        if (zScore > threshold) {
            return true;
        }
    }
    
    return false;
}

bool StatisticalAnalyzer::detectTrends(const std::vector<float>& data, float threshold) {
    if (data.size() < 3) return false;
    
    StatisticalResults stats = analyze(data);
    TimeSeriesAnalysis tsAnalysis = analyzeTimeSeries(data);
    
    // Check if slope is significant
    if (!tsAnalysis.trend.empty()) {
        float maxDifference = 0.0f;
        for (size_t i = 1; i < data.size(); ++i) {
            float diff = std::abs(data[i] - data[i-1]);
            if (diff > maxDifference) maxDifference = diff;
        }
        
        return (maxDifference / stats.range) > threshold;
    }
    
    return false;
}

bool StatisticalAnalyzer::isNormallyDistributed(const std::vector<float>& data, float alpha) {
    return checkNormality(data);
}

float StatisticalAnalyzer::calculateSkewness(const std::vector<float>& data) {
    StatisticalResults stats = analyze(data);
    return stats.skewness;
}

float StatisticalAnalyzer::calculateKurtosis(const std::vector<float>& data) {
    StatisticalResults stats = analyze(data);
    return stats.kurtosis;
}

std::vector<float> StatisticalAnalyzer::forecastARIMA(const std::vector<float>& data, size_t steps) {
    // Simple ARIMA forecast (placeholder)
    std::vector<float> forecast;
    if (data.empty()) return forecast;
    
    float lastValue = data.back();
    for (size_t i = 0; i < steps; ++i) {
        forecast.push_back(lastValue * 0.9f + rand() / (RAND_MAX + 1.0f) * 0.2f - 0.1f);
    }
    
    return forecast;
}

std::vector<float> StatisticalAnalyzer::forecastExponentialSmoothing(const std::vector<float>& data, size_t steps) {
    // Simple exponential smoothing forecast (placeholder)
    std::vector<float> forecast;
    if (data.empty()) return forecast;
    
    float alpha = 0.3f; // Smoothing factor
    float forecastValue = data.back();
    
    for (size_t i = 0; i < steps; ++i) {
        forecastValue = alpha * data.back() + (1 - alpha) * forecastValue;
        forecast.push_back(forecastValue);
    }
    
    return forecast;
}

float StatisticalAnalyzer::calculateNetworkEfficiency(const std::vector<std::vector<float>>& adjacencyMatrix) {
    if (adjacencyMatrix.empty()) return 0.0f;
    
    size_t n = adjacencyMatrix.size();
    float totalEfficiency = 0.0f;
    
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (i != j) {
                // Calculate shortest path length (simplified)
                float pathLength = 1.0f / (1.0f + adjacencyMatrix[i][j]);
                totalEfficiency += pathLength;
            }
        }
    }
    
    return totalEfficiency / (n * (n - 1));
}

float StatisticalAnalyzer::calculateClusteringCoefficient(const std::vector<std::vector<float>>& adjacencyMatrix) {
    if (adjacencyMatrix.empty()) return 0.0f;
    
    size_t n = adjacencyMatrix.size();
    float totalClustering = 0.0f;
    size_t connectedPairs = 0;
    
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (adjacencyMatrix[i][j] > 0.0f) {
                // Calculate triangles (simplified)
                float triangles = 0.0f;
                for (size_t k = 0; k < n; ++k) {
                    if (k != i && k != j && adjacencyMatrix[i][k] > 0.0f && adjacencyMatrix[j][k] > 0.0f) {
                        triangles++;
                    }
                }
                
                float maxTriangles = 2.0f; // Maximum possible triangles for undirected graph
                totalClustering += triangles / maxTriangles;
                connectedPairs++;
            }
        }
    }
    
    return connectedPairs > 0 ? totalClustering / connectedPairs : 0.0f;
}

float StatisticalAnalyzer::calculateModularity(const std::vector<std::vector<float>>& adjacencyMatrix) {
    // Placeholder for modularity calculation
    return 0.5f;
}

float StatisticalAnalyzer::calculateBurstIndex(const std::vector<float>& spikeTrain) {
    // Calculate burstiness index from spike train
    if (spikeTrain.empty()) return 0.0f;
    
    std::vector<float> intervals;
    for (size_t i = 1; i < spikeTrain.size(); ++i) {
        intervals.push_back(spikeTrain[i] - spikeTrain[i-1]);
    }
    
    if (intervals.empty()) return 0.0f;
    
    StatisticalResults stats = analyze(intervals);
    return stats.standardDeviation / (stats.mean + 1e-6f);
}

float StatisticalAnalyzer::calculateCoefficientOfVariation(const std::vector<float>& data) {
    StatisticalResults stats = analyze(data);
    return stats.mean > 0.0f ? stats.standardDeviation / stats.mean : 0.0f;
}

float StatisticalAnalyzer::calculateSignalToNoiseRatio(const std::vector<float>& signal, const std::vector<float>& noise) {
    if (signal.empty() || noise.empty()) return 0.0f;
    
    float signalPower = 0.0f;
    float noisePower = 0.0f;
    
    for (size_t i = 0; i < signal.size(); ++i) {
        signalPower += signal[i] * signal[i];
        if (i < noise.size()) {
            noisePower += noise[i] * noise[i];
        }
    }
    
    if (noisePower < 1e-6f) return std::numeric_limits<float>::max();
    
    return signalPower / noisePower;
}

bool StatisticalAnalyzer::checkNormality(const std::vector<float>& data) {
    // Shapiro-Wilk test (simplified)
    if (data.size() < 3) return true;
    
    // Calculate skewness and kurtosis
    float skew = calculateSkewness(data);
    float kurt = calculateKurtosis(data);
    
    // Use approximate normality criteria
    bool isNormal = std::abs(skew) < 1.0f && std::abs(kurt - 3.0f) < 1.0f;
    return isNormal;
}

bool StatisticalAnalyzer::checkStationarity(const std::vector<float>& data) {
    // Augmented Dickey-Fuller test (simplified)
    if (data.size() < 10) return false;
    
    // Check if variance is constant over time
    size_t segmentSize = data.size() / 4;
    float firstMean = 0.0f, lastMean = 0.0f;
    
    for (size_t i = 0; i < segmentSize; ++i) {
        firstMean += data[i];
    }
    firstMean /= segmentSize;
    
    for (size_t i = data.size() - segmentSize; i < data.size(); ++i) {
        lastMean += data[i];
    }
    lastMean /= segmentSize;
    
    return std::abs(firstMean - lastMean) < 0.1f * (calculateSkewness(data) + 1.0f);
}

// RealTimeMonitor implementation
void RealTimeMonitor::startMonitoring(double interval) {
    if (monitoringActive_) {
        return;
    }
    
    monitoringActive_ = true;
    monitoringThread_ = std::thread(&RealTimeMonitor::monitoringLoop, this, interval);
}

void RealTimeMonitor::stopMonitoring() {
    monitoringActive_ = false;
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

void RealTimeMonitor::updateMonitorData(const MonitorData& data) {
    std::lock_guard<std::mutex> lock(std::mutex());
    
    currentData_ = data;
    history_.push_back(data);
    
    // Keep only recent history
    if (history_.size() > historySize_) {
        history_.erase(history_.begin());
    }
    
    processData(data);
}

void RealTimeMonitor::addAlert(const Alert& alert) {
    std::lock_guard<std::mutex> lock(std::mutex());
    
    alerts_.push_back(alert);
    
    if (alertCallback_) {
        alertCallback_(alert);
    }
}

std::vector<RealTimeMonitor::Alert> RealTimeMonitor::getAlerts(double since) const {
    std::lock_guard<std::mutex> lock(std::mutex());
    
    std::vector<Alert> recentAlerts;
    double currentTime = getCurrentTime();
    
    for (const auto& alert : alerts_) {
        if (currentTime - alert.timestamp <= since) {
            recentAlerts.push_back(alert);
        }
    }
    
    return recentAlerts;
}

float RealTimeMonitor::calculateAverageMetric(const std::string& metricName, double timeWindow) const {
    std::lock_guard<std::mutex> lock(std::mutex());
    
    float sum = 0.0f;
    size_t count = 0;
    double currentTime = getCurrentTime();
    
    for (const auto& data : history_) {
        if (currentTime - data.timestamp <= timeWindow) {
            if (metricName == "cpu") {
                sum += data.cpuUsage;
                count++;
            } else if (metricName == "memory") {
                sum += data.memoryUsage;
                count++;
            } else if (metricName == "neurons") {
                sum += data.activeNeurons;
                count++;
            } else if (metricName == "firing_rate") {
                sum += data.firingRate;
                count++;
            }
        }
    }
    
    return count > 0 ? sum / count : 0.0f;
}

float RealTimeMonitor::calculatePeakMetric(const std::string& metricName, double timeWindow) const {
    std::lock_guard<std::mutex> lock(std::mutex());
    
    float peak = 0.0f;
    double currentTime = getCurrentTime();
    
    for (const auto& data : history_) {
        if (currentTime - data.timestamp <= timeWindow) {
            float value = 0.0f;
            if (metricName == "cpu") {
                value = data.cpuUsage;
            } else if (metricName == "memory") {
                value = data.memoryUsage;
            } else if (metricName == "neurons") {
                value = data.activeNeurons;
            } else if (metricName == "firing_rate") {
                value = data.firingRate;
            }
            
            peak = std::max(peak, value);
        }
    }
    
    return peak;
}

float RealTimeMonitor::calculateTrend(const std::string& metricName, double timeWindow) const {
    std::lock_guard<std::mutex> lock(std::mutex());
    
    std::vector<float> values;
    double currentTime = getCurrentTime();
    
    for (const auto& data : history_) {
        if (currentTime - data.timestamp <= timeWindow) {
            if (metricName == "cpu") {
                values.push_back(data.cpuUsage);
            } else if (metricName == "memory") {
                values.push_back(data.memoryUsage);
            } else if (metricName == "neurons") {
                values.push_back(data.activeNeurons);
            } else if (metricName == "firing_rate") {
                values.push_back(data.firingRate);
            }
        }
    }
    
    if (values.size() < 2) return 0.0f;
    
    // Simple linear trend
    float sumX = 0.0f, sumY = 0.0f, sumXY = 0.0f, sumX2 = 0.0f;
    
    for (size_t i = 0; i < values.size(); ++i) {
        float x = static_cast<float>(i);
        float y = values[i];
        
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    float n = static_cast<float>(values.size());
    float slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    
    return slope;
}

std::string RealTimeMonitor::exportData(const std::string& format) const {
    if (format == "csv") {
        std::stringstream ss;
        ss << "timestamp,cpu,memory,neurons,firing_rate\n";
        
        for (const auto& data : history_) {
            ss << data.timestamp << "," << data.cpuUsage << "," << data.memoryUsage << 
               "," << data.activeNeurons << "," << data.firingRate << "\n";
        }
        
        return ss.str();
    } else if (format == "json") {
        std::stringstream ss;
        ss << "{\n  \"history\": [\n";
        
        for (size_t i = 0; i < history_.size(); ++i) {
            const auto& data = history_[i];
            ss << "    {\n      \"timestamp\": " << data.timestamp << ",\n      \"cpu\": " << data.cpuUsage << ",\n      \"memory\": " << data.memoryUsage << ",\n      \"neurons\": " << data.activeNeurons << ",\n      \"firing_rate\": " << data.firingRate << "\n    }";
            if (i < history_.size() - 1) ss << ",\n";
            else ss << "\n";
        }
        
        ss << "  ]\n}\n";
        return ss.str();
    }
    
    return "Unsupported format";
}

void RealTimeMonitor::saveToFile(const std::string& filename, const std::string& format) const {
    std::string content = exportData(format);
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    file.write(content.c_str(), content.size());
    file.close();
}

void RealTimeMonitor::integrateWithBrain(Brain* brain) {
    if (!brain) return;
    
    // Update monitor data with brain statistics
    MonitorData data;
    data.timestamp = getCurrentTime();
    data.activeNeurons = brain->getActiveNeuronCount();
    data.firingRate = brain->getTotalSpikeCount();
    data.averageFiringRate = brain->getAverageFiringRate();
    
    // Estimate CPU and memory usage (simplified)
    size_t neuronCount = brain->getTotalNeuronCount();
    data.cpuUsage = static_cast<float>(neuronCount) / 10000.0f; // Scale to 0-1
    data.memoryUsage = 0.5f; // Placeholder
    
    updateMonitorData(data);
}

void RealTimeMonitor::integrateWithAgentBrain(AgentBrain* agentBrain) {
    if (!agentBrain) return;
    
    // Update monitor data with agent brain statistics
    MonitorData data;
    data.timestamp = getCurrentTime();
    
    // Get statistics from agent brain
    auto stats = agentBrain->getStats();
    
    data.cpuUsage = stats.find("cpu_usage") != stats.end() ? stats["cpu_usage"] : 0.0f;
    data.memoryUsage = stats.find("memory_usage") != stats.end() ? stats["memory_usage"] : 0.0f;
    data.activeNeurons = stats.find("active_neurons") != stats.end() ? static_cast<size_t>(stats["active_neurons"]) : 0;
    data.firingRate = stats.find("firing_rate") != stats.end() ? stats["firing_rate"] : 0.0f;
    data.averageFiringRate = stats.find("average_firing_rate") != stats.end() ? stats["average_firing_rate"] : 0.0f;
    
    updateMonitorData(data);
}

void RealTimeMonitor::monitoringLoop(double interval) {
    while (monitoringActive_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(interval * 1000)));
        
        // Update system metrics (simplified)
        MonitorData data;
        data.timestamp = getCurrentTime();
        
        // Simple system metric simulation
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        
        data.cpuUsage = dis(gen);
        data.memoryUsage = dis(gen);
        data.activeNeurons = static_cast<size_t>(dis(gen) * 1000);
        data.firingRate = static_cast<size_t>(dis(gen) * 100);
        data.averageFiringRate = data.firingRate / static_cast<float>(data.activeNeurons > 0 ? data.activeNeurons : 1);
        
        updateMonitorData(data);
    }
}

void RealTimeMonitor::processData(const MonitorData& data) {
    // Check for threshold violations
    if (data.cpuUsage > resourceThreshold_) {
        Alert alert;
        alert.type = "threshold";
        alert.message = "High CPU usage: " + std::to_string(data.cpuUsage);
        alert.severity = data.cpuUsage / resourceThreshold_;
        alert.timestamp = data.timestamp;
        alert.source = "system_monitor";
        alert.context["cpu_usage"] = data.cpuUsage;
        
        addAlert(alert);
    }
    
    if (data.memoryUsage > resourceThreshold_) {
        Alert alert;
        alert.type = "threshold";
        alert.message = "High memory usage: " + std::to_string(data.memoryUsage);
        alert.severity = data.memoryUsage / resourceThreshold_;
        alert.timestamp = data.timestamp;
        alert.source = "system_monitor";
        alert.context["memory_usage"] = data.memoryUsage;
        
        addAlert(alert);
    }
    
    if (data.firingRate > firingRateThreshold_) {
        Alert alert;
        alert.type = "threshold";
        alert.message = "High firing rate: " + std::to_string(data.firingRate);
        alert.severity = data.firingRate / firingRateThreshold_;
        alert.timestamp = data.timestamp;
        alert.source = "brain_monitor";
        alert.context["firing_rate"] = data.firingRate;
        
        addAlert(alert);
    }
}

// VisualizationDashboard implementation
void VisualizationDashboard::startDashboard() {
    if (dashboardActive_) {
        return;
    }
    
    dashboardActive_ = true;
    dashboardThread_ = std::thread(&VisualizationDashboard::dashboardLoop, this);
}

void VisualizationDashboard::stopDashboard() {
    dashboardActive_ = false;
    if (dashboardThread_.joinable()) {
        dashboardThread_.join();
    }
}

void VisualizationDashboard::addWidget(const WidgetData& widget) {
    widgets_.push_back(widget);
}

bool VisualizationDashboard::removeWidget(const std::string& widgetId) {
    for (auto it = widgets_.begin(); it != widgets_.end(); ++it) {
        if (it->widgetId == widgetId) {
            widgets_.erase(it);
            return true;
        }
    }
    return false;
}

WidgetData* VisualizationDashboard::getWidget(const std::string& widgetId) {
    for (auto& widget : widgets_) {
        if (widget.widgetId == widgetId) {
            return &widget;
        }
    }
    return nullptr;
}

void VisualizationDashboard::setWidgetData(const std::string& widgetId, const std::vector<float>& data) {
    widgetData_[widgetId] = data;
}

std::vector<float> VisualizationDashboard::getWidgetData(const std::string& widgetId) const {
    auto it = widgetData_.find(widgetId);
    if (it != widgetData_.end()) {
        return it->second;
    }
    return std::vector<float>();
}

void VisualizationDashboard::integrateWithBrain(Brain* brain) {
    if (!brain) return;
    
    // Extract and process data for dashboard widgets
    for (auto& widget : widgets_) {
        if (widget.dataSource == "brain") {
            std::vector<float> data;
            
            if (widget.widgetType == "heatmap") {
                // Generate activity heatmap data
                size_t width = 10, height = 10;
                for (size_t i = 0; i < height; ++i) {
                    std::vector<float> row;
                    for (size_t j = 0; j < width; ++j) {
                        row.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                    }
                    data.insert(data.end(), row.begin(), row.end());
                }
            } else if (widget.widgetType == "timeseries") {
                // Generate time series data
                for (size_t i = 0; i < 100; ++i) {
                    data.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                }
            }
            
            setWidgetData(widget.widgetId, data);
        }
    }
}

void VisualizationDashboard::integrateWithAgentBrain(AgentBrain* agentBrain) {
    if (!agentBrain) return;
    
    // Extract and process data from agent brain
    for (auto& widget : widgets_) {
        if (widget.dataSource == "agent_brain") {
            std::vector<float> data;
            
            if (widget.widgetType == "metrics") {
                // Get agent brain statistics
                auto stats = agentBrain->getStats();
                
                for (const auto& pair : stats) {
                    data.push_back(pair.second);
                }
            } else if (widget.widgetType == "alerts") {
                // Get agent brain alerts
                // Placeholder
            }
            
            setWidgetData(widget.widgetId, data);
        }
    }
}

std::string VisualizationDashboard::generateHTML() const {
    std::stringstream ss;
    
    ss << "<!DOCTYPE html>\n";
    ss << "<html>\n";
    ss << "<head><title>" << config_.title << "</title>";
    ss << "<style>";
    ss << "body { font-family: Arial, sans-serif; margin: 0; padding: 20px; }";
    ss << ".dashboard { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; }";
    ss << ".widget { background: white; border-radius: 8px; padding: 20px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }";
    ss << ".widget h2 { margin-top: 0; }";
    ss << "</style></head>\n";
    ss << "<body>\n";
    ss << "<h1>" << config_.title << "</h1>\n";
    ss << "<div class=\"dashboard\">\n";
    
    for (const auto& widget : widgets_) {
        ss << "  <div class=\"widget\" style=\"grid-column: span " << 
           (static_cast<int>((1.0 / config_.width) * 100)) << "%;\n\" >\n";
        ss << "    <h2>" << widget.title << "</h2>\n";
        ss << "    <div id=\"" << widget.widgetId << "\">Loading...</div>\n";
        ss << "  </div>\n";
    }
    
    ss << "</div>\n";
    ss << "<script>\n";
    ss << "// Dashboard data and visualization would be rendered here\n";
    ss << "</script>\n";
    ss << "</body>\n";
    ss << "</html>\n";
    
    return ss.str();
}

std::string VisualizationDashboard::generateSVG() const {
    std::stringstream ss;
    
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"600\">\n";
    ss << "<title>" << config_.title << "</title>\n";
    ss << "<rect width=\"800\" height=\"600\" fill=\"#f5f5f5\"/>\n";
    
    // Draw widgets
    for (const auto& widget : widgets_) {
        ss << "<rect x=\"" << widget.x * 800 << "\" y=\"" << widget.y * 600 << 
           "\" width=\"" << widget.width * 800 << "\" height=\"" << widget.height * 600 << 
           "\" fill=\"white\" stroke=\"#ddd\" stroke-width=\"2\" rx=\"5\"/>\n";
        
        ss << "<text x=\"" << (widget.x * 800 + widget.width * 800 / 2) << "\" y=\"" << 
           (widget.y * 600 + 30) << "\" font-family=\"Arial\" font-size=\"16\" "
           "text-anchor=\"middle\" font-weight=\"bold\">" << widget.title << "</text>\n";
    }
    
    ss << "</svg>\n";
    return ss.str();
}

std::string VisualizationDashboard::generateJSON() const {
    std::stringstream ss;
    
    ss << "{\n";
    ss << "  \"title\": \"" << config_.title << "\",\n";
    ss << "  \"widgets\": [\n";
    
    for (size_t i = 0; i < widgets_.size(); ++i) {
        const auto& widget = widgets_[i];
        ss << "    {\n";
        ss << "      \"id\": \"" << widget.widgetId << "\",\n";
        ss << "      \"type\": \"" << widget.widgetType << "\",\n";
        ss << "      \"title\": \"" << widget.title << "\",\n";
        ss << "      \"x\": " << widget.x << ",\n";
        ss << "      \"y\": " << widget.y << ",\n";
        ss << "      \"width\": " << widget.width << ",\n";
        ss << "      \"height\": " << widget.height << ",\n";
        ss << "      \"data\": ";
        
        auto it = widgetData_.find(widget.widgetId);
        if (it != widgetData_.end()) {
            ss << "[\n";
            for (size_t j = 0; j < it->second.size(); ++j) {
                ss << "        " << it->second[j];
                if (j < it->second.size() - 1) ss << ",\n";
                else ss << "\n";
            }
            ss << "      ]\n";
        } else {
            ss << "[]\n";
        }
        
        ss << "    }";
        if (i < widgets_.size() - 1) ss << ",\n";
        else ss << "\n";
    }
    
    ss << "  ],\n";
    ss << "  \"config\": {\n";
    ss << "    \"refreshInterval\": " << config_.refreshInterval << ",\n";
    ss << "    \"autoRefresh\": " << (config_.autoRefresh ? "true" : "false") << ",\n";
    ss << "    \"showTimestamps\": " << (config_.showTimestamps ? "true" : "false") << ",\n";
    ss << "    \"showLegend\": " << (config_.showLegend ? "true" : "false") << "\n";
    ss << "  }\n";
    ss << "}\n";
    
    return ss.str();
}

std::string VisualizationDashboard::getDashboardImage(VisualizationFormat format) const {
    switch (format) {
        case VisualizationFormat::HTML:
            return generateHTML();
        case VisualizationFormat::SVG:
            return generateSVG();
        case VisualizationFormat::JSON:
            return generateJSON();
        default:
            return "Unsupported format for dashboard";
    }
}

void VisualizationDashboard::saveToFile(const std::string& filename, VisualizationFormat format) const {
    std::string content = getDashboardImage(format);
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    file.write(content.c_str(), content.size());
    file.close();
}

void VisualizationDashboard::analyzeNetworkTopology(Brain* brain) {
    if (!brain) return;
    
    // Analyze network topology for visualization
    size_t regionCount = brain->getRegionCount();
    size_t neuronCount = brain->getTotalNeuronCount();
    size_t synapseCount = brain->getTotalSynapseCount();
    
    // Generate statistics for widgets
    for (auto& widget : widgets_) {
        if (widget.widgetType == "metrics") {
            std::vector<float> data;
            data.push_back(static_cast<float>(regionCount));
            data.push_back(static_cast<float>(neuronCount));
            data.push_back(static_cast<float>(synapseCount));
            data.push_back(brain->getAverageFiringRate());
            
            setWidgetData(widget.widgetId, data);
        }
    }
}

void VisualizationDashboard::analyzeActivityPatterns(Brain* brain) {
    if (!brain) return;
    
    // Analyze activity patterns for visualization
    for (auto& widget : widgets_) {
        if (widget.widgetType == "heatmap") {
            // Generate activity heatmap
            ActivityHeatmap heatmap(10, 10);
            std::vector<std::vector<float>> heatmapData;
            
            for (size_t i = 0; i < 10; ++i) {
                std::vector<float> row;
                for (size_t j = 0; j < 10; ++j) {
                    row.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                }
                heatmapData.push_back(row);
            }
            
            heatmap.setData(heatmapData);
            std::string heatmapContent = heatmap.generateJSON();
            
            // Convert to float vector for storage
            std::vector<float> data;
            for (const auto& row : heatmapData) {
                for (float val : row) {
                    data.push_back(val);
                }
            }
            
            setWidgetData(widget.widgetId, data);
        }
    }
}

void VisualizationDashboard::analyzeLearningDynamics(Brain* brain) {
    if (!brain) return;
    
    // Analyze learning dynamics for visualization
    for (auto& widget : widgets_) {
        if (widget.widgetType == "timeseries") {
            // Generate time series data
            std::vector<float> timeSeries;
            
            for (size_t i = 0; i < 100; ++i) {
                timeSeries.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
            }
            
            setWidgetData(widget.widgetId, timeSeries);
        }
    }
}

void VisualizationDashboard::dashboardLoop() {
    while (dashboardActive_) {
        std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(config_.refreshInterval)));
        
        updateWidgetData();
    }
}

void VisualizationDashboard::updateWidgetData() {
    for (auto& widget : widgets_) {
        std::vector<float> data;
        
        if (widget.widgetType == "metrics") {
            // Get current metrics data
            if (monitor_.isMonitoring()) {
                MonitorData currentData = monitor_.getCurrentData();
                
                data.push_back(currentData.cpuUsage);
                data.push_back(currentData.memoryUsage);
                data.push_back(static_cast<float>(currentData.activeNeurons));
                data.push_back(currentData.firingRate);
                data.push_back(currentData.averageFiringRate);
            }
        } else if (widget.widgetType == "heatmap") {
            // Generate heatmap data
            size_t width = 10, height = 10;
            for (size_t i = 0; i < height; ++i) {
                for (size_t j = 0; j < width; ++j) {
                    data.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                }
            }
        } else if (widget.widgetType == "timeseries") {
            // Generate time series data
            for (size_t i = 0; i < 50; ++i) {
                data.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
            }
        }
        
        setWidgetData(widget.widgetId, data);
    }
}

void VisualizationDashboard::renderWidgets() {
    // Widget rendering would be implemented here
    // This could involve updating HTML, SVG, or other visual representations
}

} // namespace AdvancedVisualization
} // namespace nlm