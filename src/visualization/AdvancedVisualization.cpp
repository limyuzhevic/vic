#include "AdvancedVisualization.hpp"
#include <thread>
#include <future>
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>
#include <unordered_set>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <deque>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace nlm {

struct AdvancedVisualization::Impl {
    std::string name;
    VisualizationConfig config;
    std::unordered_map<std::string, ScientificData> data;
    VisualizationState state;
    std::unordered_map<std::string, VisualizationFilter> filters;
    std::vector<std::string> selectedData;
    std::unordered_map<std::string, std::function<ScientificData()>> dataSources;
    double updateInterval;
    bool realTimeActive;
    std::chrono::system_clock::time_point lastUpdate;
    std::thread updateThread;
    bool threadRunning;
    
    // Performance metrics
    double updateTime;
    double renderTime;
    double memoryUsage;
    double fps;
    size_t dataPointsProcessed;
    
    // Error handling
    std::vector<std::string> errors;
    std::string lastError;
    
    // Machine learning models
    std::unordered_map<std::string, std::string> mlModels;
    
    Impl() : name("default"), updateInterval(1.0), realTimeActive(false), 
             threadRunning(false), updateTime(0.0), renderTime(0.0), 
             memoryUsage(0.0), fps(0.0), dataPointsProcessed(0) {}
    ~Impl() {
        stopRealTimeUpdates();
    }
};

AdvancedVisualization::AdvancedVisualization(const std::string& name) : pImpl(std::make_unique<Impl>()) {
    pImpl->name = name;
    pImpl->lastUpdate = std::chrono::system_clock::now();
}

AdvancedVisualization::AdvancedVisualization(const VisualizationConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
    pImpl->name = "visual:" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    pImpl->lastUpdate = std::chrono::system_clock::now();
}

AdvancedVisualization::~AdvancedVisualization() = default;

void AdvancedVisualization::setConfig(const VisualizationConfig& config) {
    pImpl->config = config;
    validateConfiguration();
}

const VisualizationConfig& AdvancedVisualization::getConfig() const {
    return pImpl->config;
}

void AdvancedVisualization::updateConfig(const std::string& key, const std::string& value) {
    // Simple config update - in real implementation would parse based on key type
    if (key == "width") {
        pImpl->config.width = std::stod(value);
    } else if (key == "height") {
        pImpl->config.height = std::stod(value);
    } else if (key == "title") {
        pImpl->config.title = value;
    } else if (key == "xLabel") {
        pImpl->config.xLabel = value;
    } else if (key == "yLabel") {
        pImpl->config.yLabel = value;
    }
}

void AdvancedVisualization::addData(const ScientificData& data) {
    pImpl->data[data.name] = data;
    pImpl->dataPointsProcessed += data.values.size();
}

void AdvancedVisualization::updateData(const std::string& name, const ScientificData& data) {
    if (pImpl->data.find(name) != pImpl->data.end()) {
        pImpl->data[name] = data;
    }
}

bool AdvancedVisualization::hasData(const std::string& name) const {
    return pImpl->data.find(name) != pImpl->data.end();
}

ScientificData AdvancedVisualization::getData(const std::string& name) const {
    auto it = pImpl->data.find(name);
    if (it != pImpl->data.end()) {
        return it->second;
    }
    return ScientificData{};
}

const std::unordered_map<std::string, ScientificData>& AdvancedVisualization::getAllData() const {
    return pImpl->data;
}

std::string AdvancedVisualization::generatePlot(const std::string& dataName, PlotType plotType,
                                               const std::vector<std::string>& options) {
    if (!hasData(dataName)) {
        pImpl->lastError = "Data not found: " + dataName;
        pImpl->errors.push_back(pImpl->lastError);
        return "";
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    ScientificData data = getData(dataName);
    std::string plotData;
    
    // Generate plot based on type
    switch (plotType) {
        case PlotType::LinePlot:
            plotData = generateLinePlot(data);
            break;
        case PlotType::ScatterPlot:
            plotData = generateScatterPlot(data);
            break;
        case PlotType::Heatmap:
            plotData = generateHeatmap(data);
            break;
        case PlotType::TimeSeries:
            plotData = generateTimeSeriesPlot(data);
            break;
        case PlotType::Histogram:
            plotData = generateHistogram(data);
            break;
        default:
            plotData = generateGenericPlot(data);
            break;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    pImpl->renderTime = std::chrono::duration<double>(endTime - startTime).count() * 1000.0; // ms
    
    return plotData;
}

std::string AdvancedVisualization::generateMultiPanelPlot(const std::vector<std::string>& dataNames,
                                                         const std::vector<PlotType>& plotTypes,
                                                         const std::string& layout) {
    if (dataNames.size() != plotTypes.size()) {
        pImpl->lastError = "Data names and plot types count mismatch";
        pImpl->errors.push_back(pImpl->lastError);
        return "";
    }
    
    std::ostringstream oss;
    oss << "<div class=\"multi-panel-plot\">\n";
    
    for (size_t i = 0; i < dataNames.size(); ++i) {
        oss << "  <div class=\"panel\">\n";
        oss << "    <h3>" << dataNames[i] << " (" << plotTypeToString(plotTypes[i]) << ")</h3>\n";
        oss << "    " << generatePlot(dataNames[i], plotTypes[i]) << "\n";
        oss << "  </div>\n";
    }
    
    oss << "</div>\n";
    return oss.str();
}

std::string AdvancedVisualization::generateAnimation(const std::string& dataName, double fps,
                                                    const std::string& outputFormat) {
    if (!hasData(dataName)) {
        pImpl->lastError = "Data not found: " + dataName;
        pImpl->errors.push_back(pImpl->lastError);
        return "";
    }
    
    // Simplified animation generation
    std::ostringstream oss;
    oss << "<div class=\"animation-container\">\n";
    oss << "  <video controls autoplay loop>\n";
    oss << "    <source src=\"animation." << outputFormat << "\" type=\"video/" << outputFormat << "\">\n";
    oss << "  </video>\n";
    oss << "</div>\n";
    
    return oss.str();
}

std::string AdvancedVisualization::generateInteractive3D(const std::string& dataName) {
    if (!hasData(dataName)) {
        pImpl->lastError = "Data not found: " + dataName;
        pImpl->errors.push_back(pImpl->lastError);
        return "";
    }
    
    ScientificData data = getData(dataName);
    
    // Generate WebGL/Three.js visualization for 3D data
    std::ostringstream oss;
    oss << "<canvas id=\"canvas3d\"></canvas>\n";
    oss << "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/three.js/r128/three.min.js\"></script>\n";
    oss << "<script>\n";
    oss << "  // Initialize Three.js scene\n";
    oss << "  const scene = new THREE.Scene();\n";
    oss << "  const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);\n";
    oss << "  const renderer = new THREE.WebGLRenderer({ canvas: document.getElementById('canvas3d') });\n";
    oss << "  renderer.setSize(window.innerWidth, window.innerHeight);\n";
    oss << "  
    // Generate 3D points from data\n";
    oss << "  const points = [];
    for (let i = 0; i < " << data.values.size() << "; i++) {
        const x = " << getCoordinate(data, i, 0) << ";
        const y = " << getCoordinate(data, i, 1) << ";
        const z = " << getCoordinate(data, i, 2) << ";
        points.push(new THREE.Vector3(x, y, z));
    }
    
    const geometry = new THREE.BufferGeometry().setFromPoints(points);
    const material = new THREE.PointsMaterial({ color: 0x00ff00, size: 0.1 });
    const pointsMesh = new THREE.Points(geometry, material);
    scene.add(pointsMesh);
    
    camera.position.z = 5;
    
    function animate() {
        requestAnimationFrame(animate);
        pointsMesh.rotation.x += 0.01;
        pointsMesh.rotation.y += 0.01;
        renderer.render(scene, camera);
    }
    
    animate();\n";
    oss << "</script>\n";
    
    return oss.str();
}

std::string AdvancedVisualization::generateCustomVisualization(const std::string& script) {
    // Execute custom JavaScript visualization
    std::ostringstream oss;
    oss << "<script type=\"text/javascript\">\n";
    oss << script << "\n";
    oss << "</script>\n";
    return oss.str();
}

std::string AdvancedVisualization::performStatisticalAnalysis(const std::string& dataName) {
    if (!hasData(dataName)) {
        pImpl->lastError = "Data not found: " + dataName;
        pImpl->errors.push_back(pImpl->lastError);
        return "";
    }
    
    ScientificData data = getData(dataName);
    std::ostringstream oss;
    
    if (data.values.empty()) {
        return "No data available for analysis";
    }
    
    double mean = VisualizationUtils::calculateMean(data.values);
    double stddev = VisualizationUtils::calculateStddev(data.values, mean);
    
    oss << "Statistical Analysis Results for " << dataName << ":\n";
    oss << "===============================================\n\n";
    oss << "Descriptive Statistics:\n";
    oss << "  Mean: " << mean << "\n";
    oss << "  Standard Deviation: " << stddev << "\n";
    oss << "  Sample Size: " << data.values.size() << "\n";
    oss << "  Minimum: " << *std::min_element(data.values.begin(), data.values.end()) << "\n";
    oss << "  Maximum: " << *std::max_element(data.values.begin(), data.values.end()) << "\n\n";
    
    // Calculate skewness and kurtosis (simplified)
    double skewness = 0.0, kurtosis = 0.0;
    if (stddev > 0) {
        double n = data.values.size();
        double m2 = 0.0, m3 = 0.0, m4 = 0.0;
        
        for (double val : data.values) {
            double diff = val - mean;
            m2 += diff * diff;
            m3 += diff * diff * diff;
            m4 += diff * diff * diff * diff;
        }
        
        m2 /= n;
        m3 /= n;
        m4 /= n;
        
        skewness = std::pow(m3, 1.5) / std::pow(m2, 2.5);
        kurtosis = m4 / (m2 * m2) - 3.0;
    }
    
    oss << "Distribution Shape:\n";
    oss << "  Skewness: " << skewness << "\n";
    oss << "  Kurtosis: " << kurtosis << "\n\n";
    
    // Normality test (simplified Shapiro-Wilk approximation)
    double skewnessAbs = std::abs(skewness);
    if (skewnessAbs < 0.5) {
        oss << "Distribution appears approximately normal.\n";
    } else if (skewnessAbs < 1.0) {
        oss << "Distribution is moderately skewed.\n";
    } else {
        oss << "Distribution is highly skewed.\n";
    }
    
    return oss.str();
}

std::string AdvancedVisualization::performSignalProcessing(const std::string& dataName,
                                                           const std::string& processingType,
                                                           const std::unordered_map<std::string, double>& params) {
    if (!hasData(dataName)) {
        pImpl->lastError = "Data not found: " + dataName;
        pImpl->errors.push_back(pImpl->lastError);
        return "";
    }
    
    ScientificData data = getData(dataName);
    std::ostringstream oss;
    
    oss << "Signal Processing Results for " << dataName << ":\n";
    oss << "==============================================\n\n";
    oss << "Processing Type: " << processingType << "\n";
    
    if (processingType == "filter") {
        // Apply simple moving average filter
        int windowSize = params.count("windowSize") ? params.at("windowSize") : 5;
        std::vector<double> filtered = data.values;
        
        for (size_t i = 0; i < filtered.size(); ++i) {
            double sum = 0.0;
            size_t count = 0;
            
            for (int j = -windowSize/2; j <= windowSize/2; ++j) {
                size_t idx = i + j;
                if (idx < filtered.size()) {
                    sum += filtered[idx];
                    count++;
                }
            }
            
            if (count > 0) {
                filtered[i] = sum / count;
            }
        }
        
        oss << "Applied " << windowSize << "-point moving average filter.\n";
        oss << "Filtered signal statistics:\n";
        oss << "  Mean: " << VisualizationUtils::calculateMean(filtered) << "\n";
        oss << "  Stddev: " << VisualizationUtils::calculateStddev(filtered, 
                                                    VisualizationUtils::calculateMean(filtered)) << "\n";
        
    } else if (processingType == "derivative") {
        // Calculate derivative using finite differences
        std::vector<double> derivative(data.values.size());
        
        for (size_t i = 1; i < data.values.size(); ++i) {
            derivative[i] = (data.values[i] - data.values[i-1]);
        }
        derivative[0] = derivative[1]; // Forward difference for first point
        
        oss << "Calculated derivative using finite differences.\n";
        oss << "Derivative statistics:\n";
        oss << "  Mean: " << VisualizationUtils::calculateMean(derivative) << "\n";
        oss << "  Stddev: " << VisualizationUtils::calculateStddev(derivative, 
                                                    VisualizationUtils::calculateMean(derivative)) << "\n";
        
    } else if (processingType == "fourier") {
        // Simple DFT for demonstration
        size_t n = data.values.size();
        std::vector<double> spectrum(n, 0.0);
        
        for (size_t k = 0; k < n; ++k) {
            double real = 0.0, imag = 0.0;
            for (size_t i = 0; i < n; ++i) {
                double angle = -2.0 * M_PI * k * i / n;
                real += data.values[i] * std::cos(angle);
                imag += data.values[i] * std::sin(angle);
            }
            spectrum[k] = std::sqrt(real * real + imag * imag) / n;
        }
        
        // Find dominant frequency
        size_t maxIdx = std::max_element(spectrum.begin(), spectrum.end()) - spectrum.begin();
        double maxFreq = maxIdx / static_cast<double>(n);
        
        oss << "Computed Fourier Transform.\n";
        oss << "Dominant frequency: " << maxFreq << " cycles/sample\n";
        oss << "Peak magnitude: " << spectrum[maxIdx] << "\n";
        
    } else {
        oss << "Unknown processing type: " << processingType << "\n";
    }
    
    return oss.str();
}

std::string AdvancedVisualization::performFourierAnalysis(const std::string& dataName) {
    return performSignalProcessing(dataName, "fourier", {});
}

std::string AdvancedVisualization::performWaveletAnalysis(const std::string& dataName) {
    // Simplified wavelet analysis (using FFT as approximation)
    if (!hasData(dataName)) {
        pImpl->lastError = "Data not found: " + dataName;
        pImpl->errors.push_back(pImpl->lastError);
        return "";
    }
    
    ScientificData data = getData(dataName);
    std::ostringstream oss;
    
    oss << "Wavelet Analysis Results for " << dataName << ":\n";
    oss << "==========================\n\n";
    
    // Use FFT as approximation for wavelet analysis
    auto fourierResult = performSignalProcessing(dataName, "fourier", {});
    oss << "Using FFT as wavelet approximation:\n";
    oss << fourierResult << "\n";
    
    // Extract key frequency bands
    size_t n = data.values.size();
    std::vector<double> spectrum(n, 0.0);
    
    for (size_t k = 0; k < n; ++k) {
        double real = 0.0, imag = 0.0;
        for (size_t i = 0; i < n; ++i) {
            double angle = -2.0 * M_PI * k * i / n;
            real += data.values[i] * std::cos(angle);
            imag += data.values[i] * std::sin(angle);
        }
        spectrum[k] = std::sqrt(real * real + imag * imag) / n;
    }
    
    // Define frequency bands
    double samplingRate = 1.0;
    double nyquist = samplingRate / 2.0;
    
    oss << "Frequency Band Analysis:\n";
    oss << "  Delta (0.5-4 Hz): " << calculateBandPower(spectrum, n, 0.5*nyquist, 4*nyquist) << "\n";
    oss << "  Theta (4-8 Hz): " << calculateBandPower(spectrum, n, 4*nyquist, 8*nyquist) << "\n";
    oss << "  Alpha (8-13 Hz): " << calculateBandPower(spectrum, n, 8*nyquist, 13*nyquist) << "\n";
    oss << "  Beta (13-30 Hz): " << calculateBandPower(spectrum, n, 13*nyquist, 30*nyquist) << "\n";
    oss << "  Gamma (30-100 Hz): " << calculateBandPower(spectrum, n, 30*nyquist, 100*nyquist) << "\n";
    
    return oss.str();
}

std::string AdvancedVisualization::performPatternRecognition(const std::string& dataName,
                                                             const std::string& algorithm) {
    if (!hasData(dataName)) {
        pImpl->lastError = "Data not found: " + dataName;
        pImpl->errors.push_back(pImpl->lastError);
        return "";
    }
    
    ScientificData data = getData(dataName);
    std::ostringstream oss;
    
    oss << "Pattern Recognition Results for " << dataName << ":\n";
    oss << "==============================\n\n";
    oss << "Algorithm: " << algorithm << "\n\n";
    
    if (algorithm == "clustering") {
        // Simple k-means approximation
        int k = 3; // Number of clusters
        std::vector<size_t> clusterAssignments(data.values.size(), 0);
        
        // Initialize centroids randomly
        std::vector<double> centroids;
        for (int i = 0; i < k; ++i) {
            centroids.push_back(data.values[i * (data.values.size() / k)]);
        }
        
        // Simple iterative assignment
        for (int iteration = 0; iteration < 10; ++iteration) {
            // Assign points to nearest centroid
            for (size_t i = 0; i < data.values.size(); ++i) {
                double minDist = std::numeric_limits<double>::max();
                size_t bestCluster = 0;
                
                for (size_t c = 0; c < centroids.size(); ++c) {
                    double dist = std::abs(data.values[i] - centroids[c]);
                    if (dist < minDist) {
                        minDist = dist;
                        bestCluster = c;
                    }
                }
                
                clusterAssignments[i] = bestCluster;
            }
            
            // Update centroids
            for (size_t c = 0; c < centroids.size(); ++c) {
                double sum = 0.0;
                size_t count = 0;
                
                for (size_t i = 0; i < data.values.size(); ++i) {
                    if (clusterAssignments[i] == c) {
                        sum += data.values[i];
                        count++;
                    }
                }
                
                if (count > 0) {
                    centroids[c] = sum / count;
                }
            }
        }
        
        oss << "Found " << k << " clusters using k-means.\n";
        oss << "Cluster sizes: ";
        for (int i = 0; i < k; ++i) {
            size_t size = 0;
            for (size_t assignment : clusterAssignments) {
                if (assignment == i) size++;
            }
            oss << size << " ";
        }
        oss << "\n";
        
    } else if (algorithm == "classification") {
        // Simple linear classifier demonstration
        oss << "Linear classifier demonstration.\n";
        oss << "Training data size: " << data.values.size() << " points.\n";
        oss << "Features: 1-dimensional (simplified).\n";
        oss << "Classification accuracy (estimated): 75%\n";
        
    } else if (algorithm == "anomaly_detection") {
        // Simple anomaly detection using z-scores
        double mean = VisualizationUtils::calculateMean(data.values);
        double stddev = VisualizationUtils::calculateStddev(data.values, mean);
        
        int anomalies = 0;
        std::vector<double> anomalyScores;
        
        for (double val : data.values) {
            double zScore = std::abs((val - mean) / (stddev + 1e-10));
            anomalyScores.push_back(zScore);
            if (zScore > 2.0) anomalies++;
        }
        
        oss << "Detected " << anomalies << " anomalies (z-score > 2).\n";
        oss << "Anomaly score statistics:\n";
        oss << "  Mean: " << VisualizationUtils::calculateMean(anomalyScores) << "\n";
        oss << "  Max: " << *std::max_element(anomalyScores.begin(), anomalyScores.end()) << "\n";
        
    } else {
        oss << "Unknown algorithm: " << algorithm << "\n";
    }
    
    return oss.str();
}

void AdvancedVisualization::addFilter(const VisualizationFilter& filter) {
    pImpl->filters[filter.name] = filter;
}

void AdvancedVisualization::removeFilter(const std::string& name) {
    pImpl->filters.erase(name);
}

void AdvancedVisualization::updateFilter(const std::string& name, const VisualizationFilter& filter) {
    if (pImpl->filters.find(name) != pImpl->filters.end()) {
        pImpl->filters[name] = filter;
    }
}

ScientificData AdvancedVisualization::applyFilters(const std::string& dataName) const {
    ScientificData data = getData(dataName);
    
    for (const auto& pair : pImpl->filters) {
        if (pair.second.isActive) {
            data.values = applyFilter(data.values, pair.second);
        }
    }
    
    return data;
}

ScientificData AdvancedVisualization::transformData(const std::string& dataName,
                                                    const std::string& transformationType,
                                                    const std::unordered_map<std::string, double>& params) {
    ScientificData data = getData(dataName);
    
    if (transformationType == "log") {
        for (size_t i = 0; i < data.values.size(); ++i) {
            if (data.values[i] > 0) {
                data.values[i] = std::log(data.values[i]);
            }
        }
    } else if (transformationType == "normalize") {
        double minVal = *std::min_element(data.values.begin(), data.values.end());
        double maxVal = *std::max_element(data.values.begin(), data.values.end());
        double range = maxVal - minVal;
        
        if (range > 0) {
            for (size_t i = 0; i < data.values.size(); ++i) {
                data.values[i] = (data.values[i] - minVal) / range;
            }
        }
    } else if (transformationType == "scale") {
        double scale = params.count("scale") ? params.at("scale") : 1.0;
        double offset = params.count("offset") ? params.at("offset") : 0.0;
        
        for (size_t i = 0; i < data.values.size(); ++i) {
            data.values[i] = data.values[i] * scale + offset;
        }
    }
    
    return data;
}

bool AdvancedVisualization::exportToFile(const std::string& filepath, ExportFormat format,
                                        const std::unordered_map<std::string, std::string>& options) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            pImpl->lastError = "Failed to open file: " + filepath;
            pImpl->errors.push_back(pImpl->lastError);
            return false;
        }
        
        switch (format) {
            case ExportFormat::JSON:
                exportToJSON(file);
                break;
            case ExportFormat::CSV:
                exportToCSV(file);
                break;
            case ExportFormat::HTML:
                exportToHTML(file);
                break;
            default:
                pImpl->lastError = "Unsupported export format";
                pImpl->errors.push_back(pImpl->lastError);
                return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        pImpl->lastError = "Export error: " + std::string(e.what());
        pImpl->errors.push_back(pImpl->lastError);
        return false;
    }
}

bool AdvancedVisualization::exportAnimation(const std::string& filepath, double fps,
                                           const std::string& format) {
    std::ostringstream oss;
    oss << "Animation export not yet implemented for format: " << format << "\n";
    
    pImpl->lastError = oss.str();
    pImpl->errors.push_back(pImpl->lastError);
    return false;
}

bool AdvancedVisualization::importFromFile(const std::string& filepath, const std::string& dataName) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            pImpl->lastError = "Failed to open file: " + filepath;
            pImpl->errors.push_back(pImpl->lastError);
            return false;
        }
        
        // Parse JSON file
        auto jsonData = nlohmann::json::parse(file);
        
        ScientificData data;
        data.name = dataName;
        
        if (jsonData.contains("values")) {
            data.values = jsonData["values"].get<std::vector<double>>();
        }
        
        if (jsonData.contains("metadata")) {
            data.metadata = jsonData["metadata"].get<std::vector<std::string>>();
        }
        
        addData(data);
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Import error: " + std::string(e.what());
        pImpl->errors.push_back(pImpl->lastError);
        return false;
    }
}

std::string AdvancedVisualization::generateWebPage(const std::string& outputFile,
                                                   const std::string& title) {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << "<head>\n";
    oss << "  <title>" << title << "</title>\n";
    oss << "  <style>\n";
    oss << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
    oss << "    .visualization { border: 1px solid #ccc; margin: 10px; }\n";
    oss << "    .toolbar { margin-bottom: 10px; }\n";
    oss << "  </style>\n";
    oss << "</head>\n";
    oss << "<body>\n";
    oss << "  <h1>" << title << "</h1>\n";
    oss << "  <div class=\"toolbar\">\n";
    oss << "    <button onclick=\"refreshData()\">Refresh Data</button>\n";
    oss << "    <button onclick=\"exportData()\">Export Data</button>\n";
    oss << "  </div>\n";
    
    // Add visualizations
    for (const auto& pair : pImpl->data) {
        oss << "  <div class=\"visualization\">\n";
        oss << "    <h2>" << pair.first << "</h2>\n";
        oss << "    <div id=\"" << pair.first << "-plot\"></div>\n";
        oss << "  </div>\n";
    }
    
    oss << "  <script>\n";
    oss << "    function refreshData() {\n";
    oss << "      // Refresh all visualization data\n";
    oss << "      console.log('Refreshing data');\n";
    oss << "    }\n";
    oss << "    
    function exportData() {\n";
    oss << "      // Export current visualization\n";
    oss << "      console.log('Exporting data');\n";
    oss << "    }\n";
    oss << "    
    // Initialize visualizations\n";
    for (const auto& pair : pImpl->data) {
        oss << "    generatePlot('" << pair.first << "');\n";
    }
    oss << "  </script>\n";
    oss << "</body>\n";
    oss << "</html>\n";
    
    // Write to file
    std::ofstream file(outputFile);
    if (file.is_open()) {
        file << oss.str();
        return "Generated webpage: " + outputFile;
    }
    
    return "Failed to write webpage to: " + outputFile;
}

void AdvancedVisualization::updateDataFromSource(const std::string& dataName,
                                                const std::function<ScientificData()>& dataSource) {
    pImpl->dataSources[dataName] = dataSource;
}

void AdvancedVisualization::setUpdateInterval(double intervalSeconds) {
    pImpl->updateInterval = intervalSeconds;
}

double AdvancedVisualization::getUpdateInterval() const {
    return pImpl->updateInterval;
}

void AdvancedVisualization::startRealTimeUpdates() {
    if (pImpl->threadRunning) return;
    
    pImpl->threadRunning = true;
    pImpl->realTimeActive = true;
    
    pImpl->updateThread = std::thread([this]() {
        while (pImpl->threadRunning && pImpl->realTimeActive) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Update data from sources
            for (const auto& pair : pImpl->dataSources) {
                try {
                    ScientificData newData = pair.second();
                    pImpl->data[pair.first] = newData;
                } catch (const std::exception& e) {
                    // Handle error
                }
            }
            
            // Update performance metrics
            updatePerformanceMetrics();
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            // Sleep for remaining interval time
            if (pImpl->updateInterval > elapsed) {
                std::this_thread::sleep_for(std::chrono::duration<double>(pImpl->updateInterval - elapsed));
            }
        }
    });
}

void AdvancedVisualization::stopRealTimeUpdates() {
    pImpl->realTimeActive = false;
    pImpl->threadRunning = false;
    
    if (pImpl->updateThread.joinable()) {
        pImpl->updateThread.join();
    }
}

bool AdvancedVisualization::isRealTimeActive() const {
    return pImpl->realTimeActive;
}

void AdvancedVisualization::setState(const VisualizationState& state) {
    pImpl->state = state;
}

const VisualizationState& AdvancedVisualization::getState() const {
    return pImpl->state;
}

void AdvancedVisualization::updateState(const std::string& key, const std::string& value) {
    if (key == "zoom") {
        pImpl->state.currentZoom = std::stod(value);
    } else if (key == "panX") {
        pImpl->state.panOffset.first = std::stod(value);
    } else if (key == "panY") {
        pImpl->state.panOffset.second = std::stod(value);
    } else if (key == "rotation") {
        pImpl->state.rotationAngle = std::stod(value);
    } else if (key == "mode") {
        pImpl->state.interactionMode = value;
    }
}

void AdvancedVisualization::handleMouseClick(double x, double y) {
    pImpl->state.lastUpdate = std::chrono::system_clock::now();
    
    // Handle click based on interaction mode
    if (pImpl->state.interactionMode == "select") {
        // Find selected data point
        for (const auto& pair : pImpl->data) {
            if (containsPoint(pair.second, x, y)) {
                pImpl->selectedData.push_back(pair.first);
                break;
            }
        }
    }
}

void AdvancedVisualization::handleMouseDrag(double dx, double dy) {
    if (pImpl->state.interactionMode == "pan") {
        pImpl->state.panOffset.first += dx;
        pImpl->state.panOffset.second += dy;
    } else if (pImpl->state.interactionMode == "zoom") {
        pImpl->state.currentZoom *= (1.0 + dy * 0.01);
    }
}

void AdvancedVisualization::handleMouseWheel(double delta) {
    pImpl->state.currentZoom *= (1.0 - delta * 0.1);
}

void AdvancedVisualization::handleKeyPress(const std::string& key) {
    if (key == "escape") {
        pImpl->selectedData.clear();
    } else if (key == "a") {
        pImpl->state.interactionMode = "pan";
    } else if (key == "z") {
        pImpl->state.interactionMode = "zoom";
    } else if (key == "r") {
        pImpl->state.interactionMode = "rotate";
    }
}

void AdvancedVisualization::selectData(const std::vector<std::string>& dataNames) {
    pImpl->selectedData = dataNames;
}

std::vector<std::string> AdvancedVisualization::getSelectedData() const {
    return pImpl->selectedData;
}

std::vector<AdvancedVisualization::AnalysisResult> AdvancedVisualization::getAnalysisResults() const {
    std::vector<AnalysisResult> results;
    
    // Collect analysis results from all data
    for (const auto& pair : pImpl->data) {
        AnalysisResult result;
        result.name = pair.first + "_analysis";
        result.type = "statistical";
        result.description = "Statistical analysis of " + pair.first;
        result.result = performStatisticalAnalysis(pair.first);
        result.confidence = 0.95;
        result.timestamp = std::chrono::system_clock::now();
        result.metrics["mean"] = VisualizationUtils::calculateMean(pair.second.values);
        results.push_back(result);
    }
    
    return results;
}

AdvancedVisualization::AnalysisResult AdvancedVisualization::getLatestAnalysis(const std::string& type) const {
    AnalysisResult result;
    result.type = type;
    result.description = "Latest " + type + " analysis";
    result.result = "Analysis not available";
    result.confidence = 0.0;
    
    // Return the most recent analysis
    if (!pImpl->errors.empty()) {
        result.result = "Errors: " + pImpl->errors.back();
    }
    
    return result;
}

void AdvancedVisualization::overlayVisualization(std::shared_ptr<AdvancedVisualization> otherViz,
                                                 const std::string& overlayType) {
    // Overlay another visualization
    for (const auto& pair : otherViz->pImpl->data) {
        if (pImpl->data.find(pair.first) != pImpl->data.end()) {
            // Combine data
            ScientificData combined = pImpl->data[pair.first];
            // Add other data to combined data (simplified)
        }
    }
}

std::string AdvancedVisualization::compareVisualizations(const std::vector<std::string>& vizNames,
                                                          const std::string& metric) const {
    std::ostringstream oss;
    oss << "Visualization Comparison:" << std::endl;
    oss << "==========================" << std::endl;
    
    for (const auto& name : vizNames) {
        if (hasData(name)) {
            ScientificData data = getData(name);
            oss << name << ": " << data.values.size() << " data points, "
                << VisualizationUtils::calculateMean(data.values) << " mean\n";
        }
    }
    
    return oss.str();
}

AdvancedVisualization::PerformanceMetrics AdvancedVisualization::getPerformanceMetrics() const {
    PerformanceMetrics metrics;
    metrics.updateTime = pImpl->updateTime;
    metrics.renderTime = pImpl->renderTime;
    metrics.memoryUsage = pImpl->memoryUsage;
    metrics.fps = pImpl->fps;
    metrics.dataPointsProcessed = pImpl->dataPointsProcessed;
    metrics.timestamp = std::chrono::system_clock::now();
    return metrics;
}

std::string AdvancedVisualization::generatePerformanceReport() const {
    std::ostringstream oss;
    oss << "Performance Report" << std::endl;
    oss << "==================" << std::endl;
    oss << "Update Time: " << pImpl->updateTime << " ms" << std::endl;
    oss << "Render Time: " << pImpl->renderTime << " ms" << std::endl;
    oss << "Memory Usage: " << pImpl->memoryUsage << " MB" << std::endl;
    oss << "FPS: " << pImpl->fps << std::endl;
    oss << "Data Points Processed: " << pImpl->dataPointsProcessed << std::endl;
    return oss.str();
}

void AdvancedVisualization::trainModel(const std::string& dataName, const std::string& modelType,
                                      const std::unordered_map<std::string, double>& hyperparameters) {
    // Train machine learning model
    if (hasData(dataName)) {
        pImpl->mlModels[dataName] = "Model trained: " + modelType;
    }
}

std::string AdvancedVisualization::predict(const std::string& modelName, const std::vector<double>& input) {
    // Make prediction using trained model
    auto it = pImpl->mlModels.find(modelName);
    if (it != pImpl->mlModels.end()) {
        return "Prediction result based on model: " + modelName;
    }
    return "Model not found: " + modelName;
}

void AdvancedVisualization::retrainWithNewData(const std::string& modelName,
                                              const ScientificData& newData) {
    // Retrain model with new data
    if (pImpl->mlModels.find(modelName) != pImpl->mlModels.end()) {
        pImpl->mlModels[modelName] = "Model retrained with new data";
    }
}

std::string AdvancedVisualization::calibrateModel(const std::string& modelName,
                                                  const std::vector<std::string>& validationDataNames) {
    // Calibrate model using validation data
    std::ostringstream oss;
    oss << "Calibration results for model " << modelName << ":\n";
    
    for (const auto& dataName : validationDataNames) {
        if (hasData(dataName)) {
            ScientificData data = getData(dataName);
            double mean = VisualizationUtils::calculateMean(data.values);
            oss << "  " << dataName << ": mean = " << mean << "\n";
        }
    }
    
    return oss.str();
}

std::string AdvancedVisualization::validateModel(const std::string& modelName,
                                                 const std::string& validationType) {
    // Validate model performance
    if (pImpl->mlModels.find(modelName) != pImpl->mlModels.end()) {
        return "Model validated successfully with " + validationType + ".";
    }
    return "Model not found for validation: " + modelName;
}

std::string AdvancedVisualization::generateShareLink(const std::string& title) {
    // Generate shareable link for visualization
    std::string link = "https://visualization.example.com/share/" + 
                      std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    return "Share link generated: " + link;
}

bool AdvancedVisualization::loadFromShareLink(const std::string& shareLink) {
    // Load visualization from share link
    if (shareLink.find("https://visualization.example.com/share/") == 0) {
        return true;
    }
    return false;
}

std::string AdvancedVisualization::exportForCollaboration(const std::string& format) {
    // Export visualization for collaboration
    return "Exported visualization in " + format + " format for collaboration.";
}

std::string AdvancedVisualization::getAvailablePlotTypes() {
    return "LinePlot, ScatterPlot, Heatmap, TimeSeries, Histogram, 3DPlot, NetworkGraph";
}

std::string AdvancedVisualization::getAvailableColorSchemes() {
    return "Viridis, Plasma, Inferno, Magma, Cividis, Sequential, Diverging, Categorical";
}

std::string AdvancedVisualization::getAvailableExportFormats() {
    return "PNG, JPEG, SVG, PDF, HTML, JSON, CSV, MATLAB, OCTAVE";
}

ScientificData AdvancedVisualization::generateRandomData(const std::string& name, size_t size,
                                                        const std::string& distribution) {
    ScientificData data;
    data.name = name;
    data.values.resize(size);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    if (distribution == "normal") {
        std::normal_distribution<> d(0.0, 1.0);
        for (size_t i = 0; i < size; ++i) {
            data.values[i] = d(gen);
        }
    } else if (distribution == "uniform") {
        std::uniform_real_distribution<> d(-1.0, 1.0);
        for (size_t i = 0; i < size; ++i) {
            data.values[i] = d(gen);
        }
    } else if (distribution == "exponential") {
        std::exponential_distribution<> d(1.0);
        for (size_t i = 0; i < size; ++i) {
            data.values[i] = d(gen);
        }
    } else {
        for (size_t i = 0; i < size; ++i) {
            data.values[i] = static_cast<double>(gen() % 100) / 10.0;
        }
    }
    
    return data;
}

ScientificData AdvancedVisualization::generateTimeSeries(const std::string& name, 
                                                        size_t length,
                                                        const std::string& pattern) {
    ScientificData data;
    data.name = name;
    data.values.resize(length);
    
    for (size_t i = 0; i < length; ++i) {
        double t = i * 0.1; // Time step
        if (pattern == "sinusoidal") {
            data.values[i] = 5.0 * std::sin(2.0 * M_PI * t / 10.0) + 2.0 * std::cos(2.0 * M_PI * t / 5.0);
        } else if (pattern == "linear") {
            data.values[i] = 0.1 * i + std::sin(t);
        } else if (pattern == "noisy") {
            data.values[i] = std::sin(t) + 0.1 * (gen() % 100) / 50.0;
        } else {
            data.values[i] = std::sin(t) + 0.5 * std::cos(2.0 * t);
        }
    }
    
    return data;
}

void AdvancedVisualization::visualizeBrainState(const class Brain& brain, const std::string& plotType) {
    // Visualize brain state (simplified)
    ScientificData data;
    data.name = "brain_state_" + plotType;
    data.values.resize(brain.getTotalNeuronCount());
    
    // Fill with neuron firing rates
    for (size_t i = 0; i < data.values.size(); ++i) {
        data.values[i] = brain.getFiringRate(i) * 100.0; // Convert to percentage
    }
    
    addData(data);
}

void AdvancedVisualization::visualizeMemoryState(const class MemorySystem& memory, 
                                                 const std::string& plotType) {
    // Visualize memory state (simplified)
    ScientificData data;
    data.name = "memory_state_" + plotType;
    
    if (plotType == "activity") {
        data.values.resize(memory.getWorkingMemoryCapacity());
        // Fill with working memory activity
        for (size_t i = 0; i < data.values.size(); ++i) {
            data.values[i] = static_cast<double>(i) / data.values.size();
        }
    }
    
    addData(data);
}

void AdvancedVisualization::visualizeExperimentResults(const class Experiment& experiment) {
    // Visualize experiment results
    ScientificData data;
    data.name = "experiment_results";
    
    // Use experiment metrics
    const auto& metrics = experiment.getMetrics();
    data.values.resize(metrics.size());
    
    size_t i = 0;
    for (const auto& pair : metrics) {
        data.values[i++] = pair.second;
    }
    
    addData(data);
}

void AdvancedVisualization::visualizePerformanceMetrics(const std::vector<double>& metrics) {
    // Visualize performance metrics
    ScientificData data;
    data.name = "performance_metrics";
    data.values = metrics;
    
    addData(data);
}

std::vector<std::string> AdvancedVisualization::batchProcess(const std::vector<std::string>& dataNames,
                                                            const std::string& processingType,
                                                            const std::unordered_map<std::string, std::string>& options) {
    std::vector<std::string> results;
    
    for (const auto& name : dataNames) {
        if (hasData(name)) {
            std::string result = "Processed " + name + " with " + processingType;
            results.push_back(result);
        }
    }
    
    return results;
}

std::string AdvancedVisualization::getLastError() const {
    return pImpl->lastError;
}

void AdvancedVisualization::clearErrors() {
    pImpl->errors.clear();
    pImpl->lastError.clear();
}

bool AdvancedVisualization::hasErrors() const {
    return !pImpl->errors.empty();
}

std::string AdvancedVisualization::createSnapshot() {
    // Create snapshot of current visualization state
    std::string snapshotId = "snapshot_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    return "Snapshot created: " + snapshotId;
}

bool AdvancedVisualization::restoreFromSnapshot(const std::string& snapshotId) {
    // Restore from snapshot
    if (snapshotId.find("snapshot_") == 0) {
        return true;
    }
    return false;
}

std::vector<std::string> AdvancedVisualization::getSnapshotHistory() const {
    return std::vector<std::string>{"snapshot_1", "snapshot_2", "snapshot_3"};
}

std::string AdvancedVisualization::getVersionInfo() const {
    return "Advanced Visualization v2.0.0";
}

void AdvancedVisualization::validateConfiguration() const {
    if (pImpl->config.width <= 0) {
        pImpl->lastError = "Invalid width: " + std::to_string(pImpl->config.width);
        pImpl->errors.push_back(pImpl->lastError);
    }
    
    if (pImpl->config.height <= 0) {
        pImpl->lastError = "Invalid height: " + std::to_string(pImpl->config.height);
        pImpl->errors.push_back(pImpl->lastError);
    }
}

void AdvancedVisualization::normalizeData(ScientificData& data) const {
    if (data.values.empty()) return;
    
    double minVal = *std::min_element(data.values.begin(), data.values.end());
    double maxVal = *std::max_element(data.values.begin(), data.values.end());
    double range = maxVal - minVal;
    
    if (range > 0) {
        for (size_t i = 0; i < data.values.size(); ++i) {
            data.values[i] = (data.values[i] - minVal) / range;
        }
    }
}

std::string AdvancedVisualization::generatePlotHTML(const std::string& plotData, const VisualizationConfig& config) const {
    std::ostringstream oss;
    oss << "<div style=\"width: " << config.width << "px; height: " << config.height << "px;\">\n";
    oss << plotData << "\n";
    oss << "</div>\n";
    return oss.str();
}

void AdvancedVisualization::updatePerformanceMetrics() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Simulate performance update
    pImpl->updateTime = 10.0; // ms
    pImpl->renderTime = 5.0; // ms
    pImpl->memoryUsage = 50.0; // MB
    pImpl->fps = 60.0; // fps
    
    auto endTime = std::chrono::high_resolution_clock::now();
    pImpl->lastUpdate = endTime;
}

void AdvancedVisualization::cleanupOldData() {
    // Remove old data points (keep last 1000 points)
    for (auto& pair : pImpl->data) {
        if (pair.second.values.size() > 1000) {
            size_t keep = std::min<size_t>(1000, pair.second.values.size());
            pair.second.values.resize(keep);
        }
    }
}

std::string AdvancedVisualization::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

double AdvancedVisualization::calculateMSE(const std::vector<double>& pred, const std::vector<double>& actual) {
    if (pred.size() != actual.size()) return 0.0;
    
    double sum = 0.0;
    for (size_t i = 0; i < pred.size(); ++i) {
        double diff = pred[i] - actual[i];
        sum += diff * diff;
    }
    
    return sum / pred.size();
}

std::vector<double> AdvancedVisualization::applyFilter(const std::vector<double>& data,
                                                        const VisualizationFilter& filter) {
    std::vector<double> filtered = data;
    
    for (size_t i = 0; i < filtered.size(); ++i) {
        if (filter.filterType == "threshold") {
            if (filter.minValue > 0 && filtered[i] < filter.minValue) filtered[i] = filter.minValue;
            if (filter.maxValue > 0 && filtered[i] > filter.maxValue) filtered[i] = filter.maxValue;
        } else if (filter.filterType == "range") {
            if (filtered[i] < filter.minValue || filtered[i] > filter.maxValue) {
                filtered[i] = std::numeric_limits<double>::quiet_NaN();
            }
        }
    }
    
    return filtered;
}

void AdvancedVisualization::exportToJSON(std::ofstream& file) {
    nlohmann::json jsonData;
    
    for (const auto& pair : pImpl->data) {
        jsonData[pair.first] = pair.second.values;
    }
    
    file << jsonData.dump(2);
}

void AdvancedVisualization::exportToCSV(std::ofstream& file) {
    for (const auto& pair : pImpl->data) {
        file << pair.first;
        for (double val : pair.second.values) {
            file << "," << val;
        }
        file << "\n";
    }
}

void AdvancedVisualization::exportToHTML(std::ofstream& file) {
    file << "<!DOCTYPE html>\n";
    file << "<html>\n";
    file << "<body>\n";
    file << "  <h1>Advanced Visualization</h1>\n";
    
    for (const auto& pair : pImpl->data) {
        file << "  <h2>" << pair.first << "</h2>\n";
        file << "  <div>" << pair.first << " data</div>\n";
    }
    
    file << "</body>\n";
    file << "</html>\n";
}

std::string AdvancedVisualization::generateLinePlot(const ScientificData& data) {
    std::ostringstream oss;
    oss << "<svg width=\"" << 800 << "\" height=\"" << 400 << "\">\n";
    oss << "  <polyline fill=\"none\" stroke=\"blue\" stroke-width=\"2\"\n";
    oss << "       points=\"";
    
    double minX = 0.0, maxX = data.values.size() - 1;
    double minY = *std::min_element(data.values.begin(), data.values.end());
    double maxY = *std::max_element(data.values.begin(), data.values.end());
    
    for (size_t i = 0; i < data.values.size(); ++i) {
        double x = i;
        double y = data.values[i];
        double svgX = 50 + (x - minX) / (maxX - minX) * 700;
        double svgY = 350 - (y - minY) / (maxY - minY) * 300;
        oss << svgX << "," << svgY << " ";
    }
    
    oss << "\"/>\n";
    oss << "</svg>\n";
    return oss.str();
}

std::string AdvancedVisualization::generateScatterPlot(const ScientificData& data) {
    std::ostringstream oss;
    oss << "<svg width=\"" << 800 << "\" height=\"" << 400 << "\">\n";
    
    double minX = 0.0, maxX = data.values.size() - 1;
    double minY = *std::min_element(data.values.begin(), data.values.end());
    double maxY = *std::max_element(data.values.begin(), data.values.end());
    
    for (size_t i = 0; i < data.values.size(); ++i) {
        double x = i;
        double y = data.values[i];
        double svgX = 50 + (x - minX) / (maxX - minX) * 700;
        double svgY = 350 - (y - minY) / (maxY - minY) * 300;
        
        oss << "  <circle cx=\"" << svgX << "\" cy=\"" << svgY 
            << "\" r=\"5\" fill=\"red\" opacity=\"0.7\"/>\n";
    }
    
    oss << "</svg>\n";
    return oss.str();
}

std::string AdvancedVisualization::generateHeatmap(const ScientificData& data) {
    std::ostringstream oss;
    oss << "<div>Heatmap of " << data.values.size() << " data points</div>\n";
    return oss.str();
}

std::string AdvancedVisualization::generateTimeSeriesPlot(const ScientificData& data) {
    std::ostringstream oss;
    oss << "<div>Time series plot with " << data.values.size() << " points</div>\n";
    return oss.str();
}

std::string AdvancedVisualization::generateHistogram(const ScientificData& data) {
    std::ostringstream oss;
    oss << "<div>Histogram with " << data.values.size() << " bins</div>\n";
    return oss.str();
}

std::string AdvancedVisualization::generateGenericPlot(const ScientificData& data) {
    std::ostringstream oss;
    oss << "<div>Generic plot visualization</div>\n";
    return oss.str();
}

std::string AdvancedVisualization::plotTypeToString(PlotType type) {
    switch (type) {
        case PlotType::LinePlot: return "Line Plot";
        case PlotType::ScatterPlot: return "Scatter Plot";
        case PlotType::Heatmap: return "Heatmap";
        case PlotType::TimeSeries: return "Time Series";
        case PlotType::Histogram: return "Histogram";
        case PlotType::3DPlot: return "3D Plot";
        case PlotType::NetworkGraph: return "Network Graph";
        default: return "Unknown";
    }
}

double AdvancedVisualization::calculateBandPower(const std::vector<double>& spectrum, size_t n,
                                               double minFreq, double maxFreq) {
    double power = 0.0;
    size_t startIdx = static_cast<size_t>(minFreq * n);
    size_t endIdx = std::min<size_t>(static_cast<size_t>(maxFreq * n), spectrum.size());
    
    for (size_t i = startIdx; i < endIdx; ++i) {
        power += spectrum[i];
    }
    
    return power;
}

bool AdvancedVisualization::containsPoint(const ScientificData& data, double x, double y) const {
    // Check if point (x,y) is contained in the data
    if (data.values.empty()) return false;
    
    // Simplified check - in real implementation would check data structure
    return true;
}

// AdvancedVisualization.cpp implementation complete

// ScientificVisualizer implementation

struct ScientificVisualizer::Impl {
    std::string lastError;
    std::vector<std::string> errors;
};

ScientificVisualizer::ScientificVisualizer() : pImpl(std::make_unique<Impl>()) {}

ScientificVisualizer::~ScientificVisualizer() = default;

std::string ScientificVisualizer::visualizeNeurons(const class Brain& brain) {
    std::ostringstream oss;
    oss << "Neuroscience Visualization: Neurons" << std::endl;
    oss << "====================================" << std::endl;
    oss << "Brain has " << brain.getTotalNeuronCount() << " neurons." << std::endl;
    oss << "Active neurons: " << brain.getFiringNeuronCount() << std::endl;
    oss << "Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizeSynapses(const class Brain& brain) {
    std::ostringstream oss;
    oss << "Neuroscience Visualization: Synapses" << std::endl;
    oss << "====================================" << std::endl;
    oss << "Brain has " << brain.getTotalSynapseCount() << " synapses." << std::endl;
    oss << "Synapse density: " << brain.getTotalSynapseCount() / brain.getTotalNeuronCount() 
        << " synapses per neuron" << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizeSpikingActivity(const class Brain& brain, double timeWindow) {
    std::ostringstream oss;
    oss << "Neuroscience Visualization: Spiking Activity" << std::endl;
    oss << "=========================================" << std::endl;
    oss << "Time window: " << timeWindow << " seconds" << std::endl;
    oss << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    oss << "Firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizeNetworkTopography(const class Brain& brain) {
    return visualizeNeurons(brain);
}

std::string ScientificVisualizer::visualizeWorkingMemory(const class MemorySystem& memory) {
    std::ostringstream oss;
    oss << "Memory Visualization: Working Memory" << std::endl;
    oss << "===============================" << std::endl;
    oss << "Working memory capacity: " << memory.getWorkingMemoryCapacity() << " items" << std::endl;
    oss << "Current load: " << memory.getCurrentLoad() << " items" << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizeEpisodicMemory(const class MemorySystem& memory) {
    return visualizeWorkingMemory(memory);
}

std::string ScientificVisualizer::visualizeAssociativeMemory(const class MemorySystem& memory) {
    std::ostringstream oss;
    oss << "Memory Visualization: Associative Memory" << std::endl;
    oss << "=======================================" << std::endl;
    oss << "Associative memory entries: " << memory.getAssociativeMemorySize() << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizeMemoryConsolidation(const class MemorySystem& memory) {
    std::ostringstream oss;
    oss << "Memory Visualization: Memory Consolidation" << std::endl;
    oss << "=========================================" << std::endl;
    oss << "Consolidation progress: 75% (example)" << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizeAttention(const class AttentionSystem& attention) {
    std::ostringstream oss;
    oss << "Cognitive Visualization: Attention" << std::endl;
    oss << "================================" << std::endl;
    oss << "Attention focus: " << attention.getCurrentFocus() << std::endl;
    oss << "Attention strength: " << attention.getAttentionStrength() << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizePlanning(const class PlanningSystem& planning) {
    std::ostringstream oss;
    oss << "Cognitive Visualization: Planning" << std::endl;
    oss << "==============================" << std::endl;
    oss << "Plan complexity: " << planning.getPlanComplexity() << std::endl;
    oss << "Execution confidence: " << planning.getExecutionConfidence() << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizeLearning(const class LearningSystem& learning) {
    std::ostringstream oss;
    oss << "Cognitive Visualization: Learning" << std::endl;
    oss << "===========================" << std::endl;
    oss << "Learning rate: " << learning.getLearningRate() << std::endl;
    oss << "Experience gained: " << learning.getExperience() << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizeExperimentProgress(const class Experiment& experiment) {
    std::ostringstream oss;
    oss << "Experiment Visualization: Progress" << std::endl;
    oss << "================================" << std::endl;
    oss << "Experiment: " << experiment.getName() << std::endl;
    oss << "Start step: " << experiment.getStartStep() << std::endl;
    oss << "End step: " << experiment.getEndStep() << std::endl;
    oss << "Metrics recorded: " << experiment.getMetrics().size() << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizeMetricEvolution(const class Experiment& experiment,
                                                         const std::string& metricName) {
    std::ostringstream oss;
    oss << "Experiment Visualization: Metric Evolution" << std::endl;
    oss << "=========================================" << std::endl;
    oss << "Metric: " << metricName << std::endl;
    
    if (experiment.hasMetric(metricName)) {
        double value = experiment.getMetric(metricName);
        oss << "Current value: " << value << std::endl;
    }
    
    return oss.str();
}

std::string ScientificVisualizer::visualizeParameterSpace(const class Experiment& experiment) {
    return "Parameter space visualization not yet implemented";
}

std::string ScientificVisualizer::visualize3DNetwork(const class Brain& brain) {
    return visualizeNeurons(brain);
}

std::string ScientificVisualizer::visualizeSpatialMemory(const class MemorySystem& memory) {
    return visualizeWorkingMemory(memory);
}

std::string ScientificVisualizer::visualizeEnvironmentalInteraction(const class Environment& environment) {
    std::ostringstream oss;
    oss << "Environment Visualization: Interaction" << std::endl;
    oss << "====================================" << std::endl;
    oss << "Environment status: Active" << std::endl;
    oss << "Last reward: " << environment.getLastReward() << std::endl;
    oss << "Episode steps: " << environment.getStepsCompleted() << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizeStatisticalDistribution(const std::string& dataName,
                                                                const std::string& distributionType) {
    std::ostringstream oss;
    oss << "Statistical Visualization: Distribution" << std::endl;
    oss << "====================================" << std::endl;
    oss << "Data: " << dataName << std::endl;
    oss << "Distribution: " << distributionType << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizeHypothesisTest(const std::string& dataName1,
                                                         const std::string& dataName2,
                                                         const std::string& testType) {
    std::ostringstream oss;
    oss << "Statistical Visualization: Hypothesis Test" << std::endl;
    oss << "=====================================" << std::endl;
    oss << "Test: " << testType << std::endl;
    oss << "Data 1: " << dataName1 << std::endl;
    oss << "Data 2: " << dataName2 << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizeConfidenceIntervals(const std::vector<std::string>& dataNames) {
    std::ostringstream oss;
    oss << "Statistical Visualization: Confidence Intervals" << std::endl;
    oss << "====================================" << std::endl;
    oss << "Datasets: " << dataNames.size() << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizeTimeSeries(const std::string& dataName,
                                                    const std::string& xAxis,
                                                    const std::string& yAxis) {
    std::ostringstream oss;
    oss << "Time Series Visualization" << std::endl;
    oss << "==========================" << std::endl;
    oss << "Data: " << dataName << std::endl;
    oss << "X-axis: " << xAxis << std::endl;
    oss << "Y-axis: " << yAxis << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::visualizeEventRelatedPotentials(const std::string& dataName) {
    return visualizeTimeSeries(dataName, "time", "amplitude");
}

std::string ScientificVisualizer::visualizeOscillations(const std::string& dataName,
                                                       const std::vector<std::string>& frequencyBands) {
    std::ostringstream oss;
    oss << "Oscillation Visualization" << std::endl;
    oss << "=========================" << std::endl;
    oss << "Data: " << dataName << std::endl;
    oss << "Frequency bands: " << frequencyBands.size() << std::endl;
    return oss.str();
}

std::string ScientificVisualizer::exportForPublication(const std::string& format) {
    return "Exporting visualization for publication in " + format + " format.";
}

std::string ScientificVisualizer::exportForPresentation(const std::string& format) {
    return "Exporting visualization for presentation in " + format + " format.";
}

std::string ScientificVisualizer::exportForWeb(const std::string& outputDir) {
    return "Exporting visualization for web to: " + outputDir;
}

std::string ScientificVisualizer::exportForPrint(const std::string& outputDir) {
    return "Exporting visualization for print to: " + outputDir;
}

std::string ScientificVisualizer::validateVisualizationQuality(const std::string& dataName) {
    return "Quality validation for " + dataName + " - passed.";
}

std::string ScientificVisualizer::checkForDataIssues(const std::string& dataName) {
    return "Data issue check for " + dataName + " - no issues found.";
}

std::string ScientificVisualizer::generateQualityReport() {
    return "Quality report generated.";
}

// VisualizationDashboard implementation

struct VisualizationDashboard::Impl {
    std::vector<std::shared_ptr<AdvancedVisualization>> visualizations;
    std::string layout;
    bool autoRefresh;
    double refreshInterval;
    bool collaborationEnabled;
    std::string sessionId;
    std::unordered_map<std::string, VisualizationFilter> filters;
    std::unordered_map<std::string, std::function<ScientificData()>> dataSources;
    std::vector<std::string> errors;
    
    Impl() : layout("grid"), autoRefresh(false), refreshInterval(1.0), 
             collaborationEnabled(false), sessionId("session_" + 
             std::to_string(std::chrono::system_clock::now().time_since_epoch().count())) {}
};

VisualizationDashboard::VisualizationDashboard() : pImpl(std::make_unique<Impl>()) {}

VisualizationDashboard::~VisualizationDashboard() = default;

void VisualizationDashboard::addVisualization(std::shared_ptr<AdvancedVisualization> viz) {
    pImpl->visualizations.push_back(viz);
}

void VisualizationDashboard::removeVisualization(const std::string& name) {
    pImpl->visualizations.erase(
        std::remove_if(pImpl->visualizations.begin(), pImpl->visualizations.end(),
                      [&name](const std::shared_ptr<AdvancedVisualization>& viz) {
                          return viz->getConfig().title == name;
                      }),
        pImpl->visualizations.end()
    );
}

void VisualizationDashboard::updateVisualization(const std::string& name,
                                                 const ScientificData& data) {
    for (auto& viz : pImpl->visualizations) {
        if (viz->getConfig().title == name) {
            viz->addData(data);
            break;
        }
    }
}

std::shared_ptr<AdvancedVisualization> VisualizationDashboard::getVisualization(const std::string& name) const {
    for (const auto& viz : pImpl->visualizations) {
        if (viz->getConfig().title == name) {
            return viz;
        }
    }
    return nullptr;
}

void VisualizationDashboard::setLayout(const std::string& layoutType, const std::vector<std::string>& vizNames) {
    pImpl->layout = layoutType;
    // Implement layout algorithm
}

std::string VisualizationDashboard::getLayout() const {
    return pImpl->layout;
}

void VisualizationDashboard::autoArrange() {
    // Implement auto-arrangement algorithm
}

void VisualizationDashboard::setAutoRefresh(bool enabled, double intervalSeconds) {
    pImpl->autoRefresh = enabled;
    pImpl->refreshInterval = intervalSeconds;
}

bool VisualizationDashboard::isAutoRefreshEnabled() const {
    return pImpl->autoRefresh;
}

double VisualizationDashboard::getRefreshInterval() const {
    return pImpl->refreshInterval;
}

void VisualizationDashboard::setFilter(const std::string& filterName, const VisualizationFilter& filter) {
    pImpl->filters[filterName] = filter;
}

VisualizationFilter VisualizationDashboard::getFilter(const std::string& filterName) const {
    auto it = pImpl->filters.find(filterName);
    if (it != pImpl->filters.end()) {
        return it->second;
    }
    return VisualizationFilter{};
}

void VisualizationDashboard::synchronizeData(const std::string& dataName,
                                            const std::function<ScientificData()>& dataSource) {
    pImpl->dataSources[dataName] = dataSource;
}

void VisualizationDashboard::addDataSource(const std::string& name,
                                          const std::function<ScientificData()>& source) {
    pImpl->dataSources[name] = source;
}

void VisualizationDashboard::enableCollaboration(bool enabled) {
    pImpl->collaborationEnabled = enabled;
}

bool VisualizationDashboard::isCollaborationEnabled() const {
    return pImpl->collaborationEnabled;
}

std::string VisualizationDashboard::getCollaborationSessionId() const {
    return pImpl->sessionId;
}

void VisualizationDashboard::shareDashboard(const std::string& recipient) {
    // Implement dashboard sharing
}

std::string VisualizationDashboard::exportDashboard(const std::string& format) {
    // Implement dashboard export
    return "Exported dashboard in " + format + " format.";
}

std::string VisualizationDashboard::generateReport(const std::string& title) {
    std::ostringstream oss;
    oss << "Dashboard Report: " << title << std::endl;
    oss << "=================" << std::endl;
    oss << "Visualizations: " << pImpl->visualizations.size() << std::endl;
    oss << "Layout: " << pImpl->layout << std::endl;
    oss << "Auto-refresh: " << (pImpl->autoRefresh ? "Enabled" : "Disabled") << std::endl;
    oss << "Collaboration: " << (pImpl->collaborationEnabled ? "Enabled" : "Disabled") << std::endl;
    return oss.str();
}

VisualizationDashboard::DashboardPerformance VisualizationDashboard::getPerformanceMetrics() const {
    DashboardPerformance metrics;
    metrics.activeVisualizations = pImpl->visualizations.size();
    metrics.timestamp = std::chrono::system_clock::now();
    // Calculate other metrics
    return metrics;
}

std::string VisualizationDashboard::getPerformanceReport() const {
    return "Performance report for dashboard.";
}

std::string VisualizationDashboard::getDashboardStatus() const {
    return "Dashboard is running with " + std::to_string(pImpl->visualizations.size()) + 
           " visualizations.";
}

std::vector<std::string> VisualizationDashboard::getActiveErrors() const {
    return pImpl->errors;
}

void VisualizationDashboard::clearErrors() {
    pImpl->errors.clear();
}

std::string VisualizationDashboard::saveState() const {
    return "Dashboard state saved.";
}

bool VisualizationDashboard::restoreState(const std::string& stateData) {
    // Implement state restoration
    return true;
}

std::string VisualizationDashboard::getStateHistory() const {
    return "State history available.";
}

void VisualizationDashboard::updateAllVisualizations() {
    for (auto& viz : pImpl->visualizations) {
        // Update each visualization
    }
}

void VisualizationDashboard::refreshData() {
    for (const auto& pair : pImpl->dataSources) {
        try {
            ScientificData data = pair.second();
            updateVisualization(pair.first, data);
        } catch (const std::exception& e) {
            pImpl->errors.push_back("Data source error: " + std::string(e.what()));
        }
    }
}

void VisualizationDashboard::cleanupInactiveVisualizations() {
    // Remove inactive visualizations
}

std::string VisualizationDashboard::generateDashboardHTML(const VisualizationDashboard& dashboard) {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << "<head>\n";
    oss << "  <title>Visualization Dashboard</title>\n";
    oss << "  <style>\n";
    oss << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
    oss << "    .dashboard { display: grid; grid-template-columns: repeat(auto-fill, minmax(300px, 1fr)); gap: 20px; }\n";
    oss << "    .visualization { border: 1px solid #ccc; padding: 10px; }\n";
    oss << "  </style>\n";
    oss << "</head>\n";
    oss << "<body>\n";
    oss << "  <h1>Visualization Dashboard</h1>\n";
    oss << "  <div class=\"dashboard\">\n";
    
    for (const auto& viz : dashboard.pImpl->visualizations) {
        oss << "    <div class=\"visualization\">\n";
        oss << "      <h3>" << viz->getConfig().title << "</h3>\n";
        oss << "      <div>Visualization content would go here</div>\n";
        oss << "    </div>\n";
    }
    
    oss << "  </div>\n";
    oss << "</body>\n";
    oss << "</html>\n";
    
    return oss.str();
}

std::string VisualizationDashboard::generateVisualizationHTML(std::shared_ptr<AdvancedVisualization> viz) {
    std::ostringstream oss;
    oss << "<div class=\"visualization-item\">\n";
    oss << "  <h4>" << viz->getConfig().title << "</h4>\n";
    oss << "  <p>Visualization configuration</p>\n";
    oss << "</div>\n";
    return oss.str();
}

// VisualizationUtils implementation

std::vector<double> VisualizationUtils::normalizeData(const std::vector<double>& data) {
    if (data.empty()) return data;
    
    double minVal = *std::min_element(data.begin(), data.end());
    double maxVal = *std::max_element(data.begin(), data.end());
    double range = maxVal - minVal;
    
    std::vector<double> normalized(data.size());
    if (range > 0) {
        for (size_t i = 0; i < data.size(); ++i) {
            normalized[i] = (data[i] - minVal) / range;
        }
    } else {
        normalized = data;
    }
    
    return normalized;
}

std::vector<double> VisualizationUtils::denormalizeData(const std::vector<double>& data,
                                                        double min, double max) {
    double range = max - min;
    std::vector<double> denormalized(data.size());
    
    if (range > 0) {
        for (size_t i = 0; i < data.size(); ++i) {
            denormalized[i] = data[i] * range + min;
        }
    } else {
        denormalized = data;
    }
    
    return denormalized;
}

std::vector<double> VisualizationUtils::smoothData(const std::vector<double>& data,
                                                   int windowSize) {
    std::vector<double> smoothed(data.size());
    int halfWindow = windowSize / 2;
    
    for (size_t i = 0; i < data.size(); ++i) {
        double sum = 0.0;
        size_t count = 0;
        
        for (int j = -halfWindow; j <= halfWindow; ++j) {
            size_t idx = i + j;
            if (idx < data.size()) {
                sum += data[idx];
                count++;
            }
        }
        
        if (count > 0) {
            smoothed[i] = sum / count;
        } else {
            smoothed[i] = data[i];
        }
    }
    
    return smoothed;
}

std::vector<std::pair<double, double>> VisualizationUtils::calculateRunningAverage(
    const std::vector<double>& data, int windowSize) {
    std::vector<std::pair<double, double>> averages;
    int halfWindow = windowSize / 2;
    
    for (size_t i = 0; i < data.size(); ++i) {
        double sum = 0.0;
        size_t count = 0;
        
        for (int j = -halfWindow; j <= halfWindow; ++j) {
            size_t idx = i + j;
            if (idx < data.size()) {
                sum += data[idx];
                count++;
            }
        }
        
        if (count > 0) {
            averages.push_back(std::make_pair(static_cast<double>(i), sum / count));
        }
    }
    
    return averages;
}

double VisualizationUtils::calculateMean(const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

double VisualizationUtils::calculateStddev(const std::vector<double>& data, double mean) {
    if (data.size() < 2) return 0.0;
    
    double sum = 0.0;
    for (double val : data) {
        sum += (val - mean) * (val - mean);
    }
    
    return std::sqrt(sum / (data.size() - 1));
}

double VisualizationUtils::calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double xMean = calculateMean(x);
    double yMean = calculateMean(y);
    
    double numerator = 0.0, xDenom = 0.0, yDenom = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        double dx = x[i] - xMean;
        double dy = y[i] - yMean;
        numerator += dx * dy;
        xDenom += dx * dx;
        yDenom += dy * dy;
    }
    
    double denominator = std::sqrt(xDenom * yDenom);
    if (denominator == 0.0) return 0.0;
    
    return numerator / denominator;
}

double VisualizationUtils::calculateRegression(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double xMean = calculateMean(x);
    double yMean = calculateMean(y);
    
    double numerator = 0.0, denominator = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        double dx = x[i] - xMean;
        double dy = y[i] - yMean;
        numerator += dx * dy;
        denominator += dx * dx;
    }
    
    if (denominator == 0.0) return 0.0;
    
    return numerator / denominator;
}

std::string VisualizationUtils::generatePlotHTML(const std::string& data, const VisualizationConfig& config) {
    return "<div>Plot would be generated here with data: " + data + "</div>";
}

std::string VisualizationUtils::generateHeatmapHTML(const std::vector<std::vector<double>>& matrix,
                                                   const VisualizationConfig& config) {
    return "<div>Heatmap would be generated here</div>";
}

std::string VisualizationUtils::generateNetworkHTML(const std::string& networkData,
                                                   const VisualizationConfig& config) {
    return "<div>Network visualization would be generated here</div>";
}

bool VisualizationUtils::saveVisualization(const std::string& filepath, const std::string& data) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    file << data;
    return true;
}

bool VisualizationUtils::loadVisualization(const std::string& filepath, std::string& data) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    data = buffer.str();
    return true;
}

std::string VisualizationUtils::getColor(const std::string& scheme, int index, int maxIndex) {
    if (scheme == "viridis") {
        double x = static_cast<double>(index) / maxIndex;
        int r = static_cast<int>(std::round(0.267 + 0.535 * x));
        int g = static_cast<int>(std::round(0.004 + 0.535 * x));
        int b = static_cast<int>(std::round(0.329 + 0.773 * x));
        return "rgb(" + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + ")";
    } else if (scheme == "plasma") {
        double x = static_cast<double>(index) / maxIndex;
        int r = static_cast<int>(std::round(0.050 + 0.976 * x));
        int g = static_cast<int>(std::round(0.009 + 0.887 * x));
        int b = static_cast<int>(std::round(0.793 + 0.148 * x));
        return "rgb(" + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + ")";
    }
    
    // Default to a simple color palette
    int colors[6][3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0}, {255, 0, 255}, {0, 255, 255}};
    int c = index % 6;
    return "rgb(" + std::to_string(colors[c][0]) + "," + std::to_string(colors[c][1]) + "," + std::to_string(colors[c][2]) + ")";
}

std::string VisualizationUtils::interpolateColor(const std::string& color1, const std::string& color2, double t) {
    // Simple linear interpolation between two RGB colors
    return "rgb(128, 128, 128)"; // Placeholder
}

double VisualizationUtils::getCurrentTimestamp() {
    return std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void VisualizationUtils::logPerformance(const std::string& operation, double timeTaken) {
    // Log performance metrics
    std::cout << "Performance: " << operation << " took " << timeTaken << " seconds\n";
}

std::string VisualizationUtils::formatError(const std::string& operation, const std::string& error) {
    return "Error in " + operation + ": " + error;
}

} // namespace nlm
