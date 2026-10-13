#include "visualization/VisualizationUtils.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <random>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <iomanip>

namespace nlm {
namespace visualization {

// DataExporter implementation
bool DataExporter::exportToText(const NetworkStats& stats, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== NLM Network Statistics ===\n";
    file << "Generated: " << viz_helpers::getCurrentTimestamp() << "\n\n";
    file << "Basic Statistics:\n";
    file << "  Neurons: " << stats.neuronCount << "\n";
    file << "  Regions: " << stats.regionCount << "\n";
    file << "  Synapses: " << stats.synapseCount << "\n";
    file << "  Total Spikes: " << stats.totalSpikeCount << "\n";
    file << "  Average Firing Rate: " << stats.averageFiringRate << " Hz\n";
    file << "  Excitation/Inhibition Ratio: " << stats.excitationInhibitionRatio << "\n";
    file << "  Timestamp: " << viz_helpers::formatTimestamp(stats.timestamp) << "\n";
    
    return file.good();
}

bool DataExporter::exportToCSV(const NetworkStats& stats, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "metric,value\n";
    file << "neuron_count," << stats.neuronCount << "\n";
    file << "region_count," << stats.regionCount << "\n";
    file << "synapse_count," << stats.synapseCount << "\n";
    file << "total_spikes," << stats.totalSpikeCount << "\n";
    file << "average_firing_rate," << stats.averageFiringRate << "\n";
    file << "excitation_inhibition_ratio," << stats.excitationInhibitionRatio << "\n";
    file << "timestamp," << viz_helpers::formatTimestamp(stats.timestamp) << "\n";
    
    return file.good();
}

bool DataExporter::exportToJSON(const NetworkStats& stats, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    file << "  \"neuron_count\": " << stats.neuronCount << ",\n";
    file << "  \"region_count\": " << stats.regionCount << ",\n";
    file << "  \"synapse_count\": " << stats.synapseCount << ",\n";
    file << "  \"total_spikes\": " << stats.totalSpikeCount << ",\n";
    file << "  \"average_firing_rate\": " << stats.averageFiringRate << ",\n";
    file << "  \"excitation_inhibition_ratio\": " << stats.excitationInhibitionRatio << ",\n";
    file << "  \"timestamp\": \"" << viz_helpers::formatTimestamp(stats.timestamp) << "\"\n";
    file << "}\n";
    
    return file.good();
}

bool DataExporter::exportToHTML(const NetworkStats& stats, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << viz_helpers::generateHTMLHeader("NLM Network Statistics");
    file << "<div class=\"container\">\n";
    file << "  <h1>NLM Network Statistics</h1>\n";
    file << "  <p><strong>Generated:</strong> " << viz_helpers::getCurrentTimestamp() << "</p>\n\n";
    file << "  <table class=\"stats-table\">\n";
    file << "    <tr><th>Metric</th><th>Value</th></tr>\n";
    file << "    <tr><td>Neurons</td><td>" << stats.neuronCount << "</td></tr>\n";
    file << "    <tr><td>Regions</td><td>" << stats.regionCount << "</td></tr>\n";
    file << "    <tr><td>Synapses</td><td>" << stats.synapseCount << "</td></tr>\n";
    file << "    <tr><td>Total Spikes</td><td>" << stats.totalSpikeCount << "</td></tr>\n";
    file << "    <tr><td>Average Firing Rate</td><td>" << stats.averageFiringRate << " Hz</td></tr>\n";
    file << "    <tr><td>Excitation/Inhibition Ratio</td><td>" << stats.excitationInhibitionRatio << "</td></tr>\n";
    file << "    <tr><td>Timestamp</td><td>" << viz_helpers::formatTimestamp(stats.timestamp) << "</td></tr>\n";
    file << "  </table>\n";
    file << "</div>\n";
    file << viz_helpers::generateHTMLFooter();
    file << viz_helpers::generateD3JSChart("stats-chart");
    
    return file.good();
}

bool DataExporter::exportNetworkData(const class Brain& brain, const std::string& filepath) {
    NetworkStats stats;
    stats.neuronCount = brain.getTotalNeuronCount();
    stats.regionCount = brain.getRegionCount();
    stats.synapseCount = brain.getTotalSynapseCount();
    stats.totalSpikeCount = brain.getTotalSpikeCount();
    stats.averageFiringRate = brain.getAverageFiringRate();
    stats.excitationInhibitionRatio = brain.getExcitationInhibitionRatio();
    stats.timestamp = std::chrono::system_clock::now();
    
    return exportToText(stats, filepath);
}

bool DataExporter::exportSpikeData(const SpikeData& spikes, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== Spike Data ===\n";
    file << "Total Spikes: " << spikes.totalSpikes << "\n";
    file << "Duration: " << spikes.duration << " seconds\n";
    file << "Average Firing Rate: " << (spikes.duration > 0 ? spikes.totalSpikes / spikes.duration : 0.0f) << " Hz\n\n";
    
    file << "Spike Times (neuron_id, time):\n";
    for (const auto& spike : spikes.spikes) {
        file << spike.first << "," << spike.second << "\n";
    }
    
    return file.good();
}

bool DataExporter::exportActivityData(const ActivityData& activity, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== Activity Data ===\n";
    file << "Time Step: " << activity.timeStep << " seconds\n";
    file << "Steps: " << activity.steps << "\n";
    file << "Duration: " << activity.timeStep * activity.steps << " seconds\n\n";
    
    file << "Firing Rates:\n";
    for (size_t i = 0; i < activity.firingRates.size(); ++i) {
        file << "Neuron " << i << ": " << activity.firingRates[i] << " Hz\n";
    }
    
    file << "\nActive Neurons: \n";
    for (size_t neuron : activity.activeNeurons) {
        file << neuron << " ";
    }
    file << "\n";
    
    return file.good();
}

bool DataExporter::exportWeightData(const WeightData& weights, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== Weight Data ===\n";
    file << "Min Weight: " << weights.minWeight << "\n";
    file << "Max Weight: " << weights.maxWeight << "\n";
    file << "Weight Range: " << (weights.maxWeight - weights.minWeight) << "\n";
    file << "Time: " << weights.time << " seconds\n\n";
    
    file << "Weight Matrix (" << weights.weightMatrix.size() << " x " << 
        (weights.weightMatrix.empty() ? 0 : weights.weightMatrix[0].size()) << "):\n";
    
    for (const auto& row : weights.weightMatrix) {
        for (size_t j = 0; j < row.size(); ++j) {
            file << viz_helpers::formatNumber(row[j], 4) << " ";
        }
        file << "\n";
    }
    
    return file.good();
}

bool DataExporter::exportData(const std::string& data, const std::string& filepath, OutputFormat format) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file << data;
    return file.good();
}

OutputFormat DataExporter::detectFormat(const std::string& filepath) {
    std::string extension = filepath;
    size_t dotPos = extension.find_last_of('.');
    if (dotPos == std::string::npos) {
        return OutputFormat::TEXT;
    }
    
    extension = extension.substr(dotPos + 1);
    
    if (extension == "txt" || extension == "text") return OutputFormat::TEXT;
    if (extension == "csv") return OutputFormat::CSV;
    if (extension == "json") return OutputFormat::JSON;
    if (extension == "html" || extension == "htm") return OutputFormat::HTML;
    if (extension == "png") return OutputFormat::IMAGE_PNG;
    if (extension == "svg") return OutputFormat::IMAGE_SVG;
    if (extension == "pdf") return OutputFormat::PDF;
    
    return OutputFormat::TEXT;
}

bool DataExporter::ensureDirectory(const std::string& filepath) {
    size_t lastSlash = filepath.find_last_of('/');
    if (lastSlash == std::string::npos) {
        lastSlash = filepath.find_last_of('\\');
    }
    
    if (lastSlash != std::string::npos) {
        std::string dir = filepath.substr(0, lastSlash);
        return std::filesystem::create_directories(dir);
    }
    
    return true;
}

// viz_helpers implementation
namespace viz_helpers {
    std::string formatNumber(double value, int precision) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        return oss.str();
    }
    
    std::string formatTime(double seconds) {
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds % 3600) / 60);
        int secs = static_cast<int>(seconds % 60);
        int millisecs = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
        
        std::ostringstream oss;
        if (hours > 0) oss << hours << "h ";
        if (minutes > 0 || hours > 0) oss << minutes << "m ";
        oss << secs << "." << std::setfill('0') << std::setw(3) << millisecs << "s";
        
        return oss.str();
    }
    
    std::string formatPercentage(float value) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << (value * 100.0f) << "%";
        return oss.str();
    }
    
    std::string formatRate(float rate) {
        std::ostringstream oss;
        if (rate >= 1000.0f) {
            oss << (rate / 1000.0f) << " kHz";
        } else {
            oss << rate << " Hz";
        }
        return oss.str();
    }
    
    std::string getColorForValue(float value, float min, float max, const std::string& scheme) {
        if (max <= min) return "gray";
        
        float normalized = (value - min) / (max - min);
        return getColorFromScheme(normalized, scheme);
    }
    
    std::string getColorFromScheme(float value, const std::string& scheme) {
        if (scheme == "blues") {
            // Blue gradient: white to dark blue
            int intensity = static_cast<int>(255 * value);
            return "#" + std::string(2 - std::to_string(intensity).length(), '0') + 
                   std::to_string(intensity) + " " + std::string(2 - std::to_string(intensity).length(), '0') + 
                   std::to_string(intensity) + "FF";
        } else if (scheme == "reds") {
            // Red gradient: light red to dark red
            int intensity = static_cast<int>(255 * value);
            return "#FF" + std::string(2 - std::to_string(intensity).length(), '0') + 
                   std::to_string(intensity) + "00";
        } else if (scheme == "greens") {
            // Green gradient: light green to dark green
            int intensity = static_cast<int>(255 * value);
            return "#" + std::string(2 - std::to_string(intensity).length(), '0') + 
                   std::to_string(intensity) + " " + std::string(2 - std::to_string(intensity).length(), '0') + 
                   std::to_string(intensity) + "00";
        } else if (scheme == "sequential") {
            // Sequential: blue -> cyan -> green -> yellow -> red
            if (value < 0.25f) {
                // Blue to cyan
                int blue = static_cast<int>(255 * value * 4);
                return "#00" + std::string(2 - std::to_string(blue).length(), '0') + 
                       std::to_string(blue) + "FF";
            } else if (value < 0.5f) {
                // Cyan to green
                int green = static_cast<int>(255 * (value - 0.25f) * 4);
                return "#00" + std::string(2 - std::to_string(green).length(), '0') + 
                       std::to_string(green) + "FF";
            } else if (value < 0.75f) {
                // Green to yellow
                int red = static_cast<int>(255 * (value - 0.5f) * 4);
                return "#" + std::string(2 - std::to_string(red).length(), '0') + 
                       std::to_string(red) + "FF";
            } else {
                // Yellow to red
                int red = static_cast<int>(255 * (value - 0.75f) * 4);
                return "#FF" + std::string(2 - std::to_string(red).length(), '0') + 
                       std::to_string(red) + "00";
            }
        } else {
            // Default: sequential scheme
            return getColorFromScheme(value, "sequential");
        }
    }
    
    std::string generateHTMLHeader(const std::string& title) {
        return "<!DOCTYPE html>\n<html>\n<head>\n"
               "  <title>" + title + "</title>\n"
               "  <style>\n" + generatePlotCSS() + "\n  </style>\n"
               "  <script src=\"https://d3js.org/d3.v7.min.js\"></script>\n"
               "</head>\n<body>\n";
    }
    
    std::string generateHTMLFooter() {
        return "</body>\n</html>";
    }
    
    std::string generatePlotCSS() {
        return "\n    .container {
        max-width: 1200px;
        margin: 0 auto;
        padding: 20px;
        font-family: Arial, sans-serif;
    }
    
    .stats-table {
        border-collapse: collapse;
        width: 100%;
        margin: 20px 0;
    }
    
    .stats-table th, .stats-table td {
        border: 1px solid #ddd;
        padding: 8px;
        text-align: left;
    }
    
    .stats-table th {
        background-color: #f2f2f2;
        font-weight: bold;
    }
    
    .stats-table tr:nth-child(even) {
        background-color: #f9f9f9;
    }
    
    .chart {
        margin: 20px 0;
        border: 1px solid #ccc;
    }";
    }
    
    std::string generateD3JSChart(const std::string& dataId) {
        return "\n  <div id=\"" + dataId + "\" class=\"chart\"></div>\n"
               "  <script>\n"
               "    // D3.js visualization code would go here\n"
               "    // This is a placeholder for actual chart generation\n"
               "    console.log('D3.js chart for ' + '" + dataId + "');\n"
               "  </script>\n";
    }
    
    std::string generateNetworkReport(const NetworkStats& stats) {
        std::ostringstream oss;
        oss << "=== NLM Network Statistics ===\n";
        oss << "Generated: " << getCurrentTimestamp() << "\n\n";
        oss << "Basic Statistics:\n";
        oss << "  Total Neurons: " << stats.neuronCount << "\n";
        oss << "  Total Regions: " << stats.regionCount << "\n";
        oss << "  Total Synapses: " << stats.synapseCount << "\n";
        oss << "  Total Spike Count: " << stats.totalSpikeCount << "\n";
        oss << "  Average Firing Rate: " << stats.averageFiringRate << " Hz\n";
        oss << "  Excitation/Inhibition Ratio: " << stats.excitationInhibitionRatio << "\n";
        oss << "  Network Density: " << (stats.neuronCount > 0 ? (stats.synapseCount * 2.0 / (stats.neuronCount * stats.neuronCount)) : 0.0) << "\n";
        oss << "  Activity Level: " << (stats.totalSpikeCount > 0 ? "Active" : "Inactive") << "\n";
        return oss.str();
    }
    
    std::string generateActivityReport(const ActivityData& activity) {
        std::ostringstream oss;
        oss << "=== Activity Report ===\n";
        oss << "Time Period: " << formatTime(activity.timeStep * activity.steps) << "\n";
        oss << "Average Firing Rate: " << calculateMean(activity.firingRates) << " Hz\n";
        oss << "Max Firing Rate: " << (activity.firingRates.empty() ? 0.0f : *std::max_element(activity.firingRates.begin(), activity.firingRates.end())) << " Hz\n";
        oss << "Min Firing Rate: " << (activity.firingRates.empty() ? 0.0f : *std::min_element(activity.firingRates.begin(), activity.firingRates.end())) << " Hz\n";
        oss << "Average Voltage: " << calculateMean(activity.voltages) << "\n";
        oss << "Active Neurons: " << activity.activeNeurons.size() << " out of " << activity.firingRates.size() << "\n";
        oss << "Spike Count: " << activity.spikeTimes.size() << "\n";
        return oss.str();
    }
    
    std::string generateSpikeReport(const SpikeData& spikes) {
        std::ostringstream oss;
        oss << "=== Spike Report ===\n";
        oss << "Total Spikes: " << spikes.totalSpikes << "\n";
        oss << "Duration: " << formatTime(spikes.duration) << "\n";
        oss << "Average Firing Rate: " << (spikes.duration > 0 ? spikes.totalSpikes / spikes.duration : 0.0f) << " Hz\n";
        oss << "Peak Firing Rate: " << (spikes.firingRates.empty() ? 0 : *std::max_element(spikes.firingRates.begin(), spikes.firingRates.end())) << " Hz\n";
        oss << "Average Inter-Spike Interval: " << (spikes.totalSpikes > 1 ? spikes.duration / (spikes.totalSpikes - 1) : 0.0) << " seconds\n";
        return oss.str();
    }
    
    std::string generateWeightReport(const WeightData& weights) {
        std::ostringstream oss;
        oss << "=== Weight Matrix Report ===\n";
        oss << "Matrix Size: " << weights.weightMatrix.size() << " x " << 
            (weights.weightMatrix.empty() ? 0 : weights.weightMatrix[0].size()) << "\n";
        oss << "Min Weight: " << weights.minWeight << "\n";
        oss << "Max Weight: " << weights.maxWeight << "\n";
        oss << "Weight Range: " << (weights.maxWeight - weights.minWeight) << "\n";
        oss << "Mean Weight: " << (weights.weightMatrix.empty() ? 0.0f : calculateMean(
            std::vector<float>(weights.weightMatrix.begin()->begin(), weights.weightMatrix.begin()->end()))) << "\n";
        oss << "Weight Standard Deviation: " << (weights.weightMatrix.empty() ? 0.0f : 
            calculateStdDev(std::vector<float>(weights.weightMatrix.begin()->begin(), weights.weightMatrix.begin()->end()), 
                           (weights.weightMatrix.empty() ? 0.0f : *std::min_element(weights.weightMatrix.begin()->begin(), weights.weightMatrix.begin()->end())))) << "\n";
        return oss.str();
    }
    
    float calculateMean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }
    
    float calculateStdDev(const std::vector<float>& values, float mean) {
        if (values.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (float value : values) {
            float diff = value - mean;
            sum += diff * diff;
        }
        
        return std::sqrt(sum / values.size());
    }
    
    float calculateVariance(const std::vector<float>& values, float mean) {
        if (values.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (float value : values) {
            float diff = value - mean;
            sum += diff * diff;
        }
        
        return sum / values.size();
    }
    
    size_t findMaxValue(const std::vector<float>& values) {
        if (values.empty()) return static_cast<size_t>(-1);
        return static_cast<size_t>(std::distance(values.begin(), std::max_element(values.begin(), values.end())));
    }
    
    size_t findMinValue(const std::vector<float>& values) {
        if (values.empty()) return static_cast<size_t>(-1);
        return static_cast<size_t>(std::distance(values.begin(), std::min_element(values.begin(), values.end())));
    }
    
    std::vector<float> filterOutliers(const std::vector<float>& values, float threshold) {
        if (values.empty()) return values;
        
        float mean = calculateMean(values);
        float stddev = calculateStdDev(values, mean);
        
        std::vector<float> filtered;
        for (float value : values) {
            if (std::abs(value - mean) <= threshold * stddev) {
                filtered.push_back(value);
            }
        }
        
        return filtered;
    }
    
    std::vector<float> normalizeData(const std::vector<float>& values, float min, float max) {
        if (values.empty()) return values;
        
        std::vector<float> normalized;
        normalized.reserve(values.size());
        
        for (float value : values) {
            if (max > min) {
                normalized.push_back((value - min) / (max - min));
            } else {
                normalized.push_back(0.0f);
            }
        }
        
        return normalized;
    }
    
    std::vector<float> smoothData(const std::vector<float>& values, int windowSize) {
        if (values.empty() || windowSize <= 0) return values;
        
        std::vector<float> smoothed;
        smoothed.reserve(values.size());
        
        for (size_t i = 0; i < values.size(); ++i) {
            size_t start = std::max<size_t>(0, i - windowSize / 2);
            size_t end = std::min(values.size(), i + windowSize / 2 + 1);
            
            float sum = 0.0f;
            size_t count = 0;
            
            for (size_t j = start; j < end; ++j) {
                sum += values[j];
                count++;
            }
            
            smoothed.push_back(sum / count);
        }
        
        return smoothed;
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%d %H:%M:%S UTC");
        return oss.str();
    }
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& tp) {
        auto in_time_t = std::chrono::system_clock::to_time_t(tp);
        
        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%d %H:%M:%S UTC");
        return oss.str();
    }
    
    double getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0;
    }
}

// viz_factory implementation
namespace viz_factory {
    NetworkStats extractNetworkStats(const class Brain& brain) {
        NetworkStats stats;
        stats.neuronCount = brain.getTotalNeuronCount();
        stats.regionCount = brain.getRegionCount();
        stats.synapseCount = brain.getTotalSynapseCount();
        stats.totalSpikeCount = brain.getTotalSpikeCount();
        stats.averageFiringRate = brain.getAverageFiringRate();
        stats.excitationInhibitionRatio = brain.getExcitationInhibitionRatio();
        stats.timestamp = std::chrono::system_clock::now();
        return stats;
    }
    
    ActivityData extractActivityData(const class Brain& brain, size_t steps) {
        ActivityData activity;
        activity.timeStep = brain.getConfig()->getOr<double>("simulation_timestep", 0.001);
        activity.steps = steps;
        
        // This is a simplified implementation - in a real system you would collect
        // actual neural activity data over time
        activity.firingRates.resize(brain.getTotalNeuronCount());
        activity.voltages.resize(brain.getTotalNeuronCount());
        activity.activeNeurons.reserve(brain.getTotalNeuronCount());
        
        for (size_t i = 0; i < brain.getTotalNeuronCount(); ++i) {
            activity.firingRates[i] = static_cast<float>(std::rand() % 100) / 100.0f;
            activity.voltages[i] = static_cast<float>(std::rand() % 100) / 100.0f;
            if (activity.firingRates[i] > 0.1f) {
                activity.activeNeurons.push_back(i);
            }
        }
        
        activity.spikeTimes.resize(brain.getTotalSpikeCount());
        for (size_t i = 0; i < activity.spikeTimes.size(); ++i) {
            activity.spikeTimes[i] = static_cast<size_t>(std::rand() % (steps * 10));
        }
        
        return activity;
    }
    
    SpikeData extractSpikeData(const class Brain& brain, double duration) {
        SpikeData spikes;
        spikes.totalSpikes = brain.getTotalSpikeCount();
        spikes.duration = duration;
        
        // This is a simplified implementation
        size_t maxNeurons = brain.getTotalNeuronCount();
        spikes.spikes.reserve(spikes.totalSpikes);
        spikes.firingRates.resize(maxNeurons);
        
        for (size_t i = 0; i < maxNeurons; ++i) {
            if (std::rand() % 10 == 0) {  // 10% chance of neuron spiking
                double spikeTime = static_cast<double>(std::rand() % static_cast<int>(duration * 1000)) / 1000.0;
                spikes.spikes.push_back(std::make_pair(i, spikeTime));
                spikes.firingRates[i]++;
            }
        }
        
        return spikes;
    }
    
    WeightData extractWeightData(const class Brain& brain, size_t maxRegions) {
        WeightData weights;
        weights.time = getCurrentTime();
        
        size_t regionCount = brain.getRegionCount();
        size_t maxNeurons = brain.getTotalNeuronCount();
        
        // Simplified weight matrix - in a real system you would extract actual weights
        weights.weightMatrix.resize(std::min(regionCount, maxRegions));
        for (auto& row : weights.weightMatrix) {
            row.resize(std::min(maxNeurons, static_cast<size_t>(10)));
            for (auto& weight : row) {
                weight = static_cast<float>(std::rand() % 100) / 100.0f - 0.5f; // -0.5 to 0.5
            }
        }
        
        // Set min/max weights
        for (const auto& row : weights.weightMatrix) {
            for (float weight : row) {
                if (weight < weights.minWeight) weights.minWeight = weight;
                if (weight > weights.maxWeight) weights.maxWeight = weight;
            }
        }
        
        return weights;
    }
    
    std::vector<NetworkStats> createComparativeStats(const std::vector<std::shared_ptr<class Brain>>& brains) {
        std::vector<NetworkStats> stats;
        stats.reserve(brains.size());
        
        for (const auto& brain : brains) {
            stats.push_back(extractNetworkStats(*brain));
        }
        
        return stats;
    }
    
    std::vector<ActivityData> createComparativeActivity(const std::vector<ActivityData>& activities) {
        return activities; // Just return as-is for now
    }
    
    std::vector<std::pair<double, float>> createTimeSeries(const ActivityData& activity) {
        std::vector<std::pair<double, float>> timeSeries;
        timeSeries.reserve(activity.steps);
        
        double timeStep = activity.timeStep;
        for (size_t i = 0; i < activity.steps; ++i) {
            double time = i * timeStep;
            float avgRate = calculateMean(activity.firingRates);
            timeSeries.push_back(std::make_pair(time, avgRate));
        }
        
        return timeSeries;
    }
    
    std::vector<std::pair<double, size_t>> createSpikeTimeSeries(const SpikeData& spikes) {
        std::vector<std::pair<double, size_t>> timeSeries;
        timeSeries.reserve(spikes.totalSpikes);
        
        for (const auto& spike : spikes.spikes) {
            timeSeries.push_back(spike);
        }
        
        std::sort(timeSeries.begin(), timeSeries.end(),
                 [](const auto& a, const auto& b) { return a.first < b.first; });
        
        return timeSeries;
    }
}

// VisualizationContext implementation (simplified)
struct VisualizationContext::Impl {
    VisualizationOptions options;
    std::shared_ptr<class Brain> brain;
    NetworkStats networkStats;
    ActivityData activityData;
    SpikeData spikeData;
    WeightData weightData;
    bool initialized;
    std::function<void(double)> progressCallback;
    std::function<void(const std::string&)> errorCallback;
    
    Impl() : initialized(false), progressCallback(nullptr), errorCallback(nullptr) {}
};

VisualizationContext::VisualizationContext(const VisualizationOptions& options) : pImpl(std::make_unique<Impl>()) {
    pImpl->options = options;
    pImpl->initialized = false;
}

VisualizationContext::~VisualizationContext() = default;

void VisualizationContext::setBrain(std::shared_ptr<class Brain> brain) {
    pImpl->brain = brain;
    if (brain) {
        pImpl->networkStats = viz_factory::extractNetworkStats(*brain);
        pImpl->activityData = viz_factory::extractActivityData(*brain);
        pImpl->spikeData = viz_factory::extractSpikeData(*brain);
        pImpl->weightData = viz_factory::extractWeightData(*brain);
        pImpl->initialized = true;
    }
}

void VisualizationContext::setNetworkStats(const NetworkStats& stats) {
    pImpl->networkStats = stats;
    pImpl->initialized = true;
}

void VisualizationContext::setActivityData(const ActivityData& activity) {
    pImpl->activityData = activity;
    pImpl->initialized = true;
}

void VisualizationContext::setSpikeData(const SpikeData& spikes) {
    pImpl->spikeData = spikes;
    pImpl->initialized = true;
}

void VisualizationContext::setWeightData(const WeightData& weights) {
    pImpl->weightData = weights;
    pImpl->initialized = true;
}

bool VisualizationContext::generateVisualization() {
    if (!pImpl->initialized) {
        if (pImpl->errorCallback) {
            pImpl->errorCallback("Visualization not initialized");
        }
        return false;
    }
    
    // Generate visualization based on options
    std::string outputPath = pImpl->options.outputPath;
    if (outputPath.empty()) {
        outputPath = "visualization_" + viz_helpers::getCurrentTimestamp();
    }
    
    DataExporter::ensureDirectory(outputPath);
    
    bool success = false;
    switch (pImpl->options.outputFormat) {
        case OutputFormat::TEXT:
            success = DataExporter::exportNetworkData(*pImpl->brain, outputPath + ".txt");
            break;
        case OutputFormat::CSV:
            success = DataExporter::exportNetworkData(*pImpl->brain, outputPath + ".csv");
            break;
        case OutputFormat::JSON:
            success = DataExporter::exportNetworkData(*pImpl->brain, outputPath + ".json");
            break;
        case OutputFormat::HTML:
            success = DataExporter::exportNetworkData(*pImpl->brain, outputPath + ".html");
            break;
        default:
            success = DataExporter::exportNetworkData(*pImpl->brain, outputPath + ".txt");
    }
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(success ? 1.0 : 0.0);
    }
    
    return success;
}

bool VisualizationContext::saveToFile(const std::string& filepath) {
    if (!pImpl->initialized) {
        if (pImpl->errorCallback) {
            pImpl->errorCallback("Visualization not initialized");
        }
        return false;
    }
    
    OutputFormat format = DataExporter::detectFormat(filepath);
    pImpl->options.outputPath = filepath;
    pImpl->options.outputFormat = format;
    
    return generateVisualization();
}

bool VisualizationContext::saveToMultipleFormats(const std::string& basePath) {
    if (!pImpl->initialized) {
        if (pImpl->errorCallback) {
            pImpl->errorCallback("Visualization not initialized");
        }
        return false;
    }
    
    bool allSuccess = true;
    
    for (int format = 0; format < static_cast<int>(OutputFormat::PDF); ++format) {
        OutputFormat outputFormat = static_cast<OutputFormat>(format);
        std::string filepath = basePath + "." + getFormatExtension(outputFormat);
        pImpl->options.outputPath = filepath;
        pImpl->options.outputFormat = outputFormat;
        
        if (!generateVisualization()) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

std::string VisualizationContext::getInfo() const {
    std::ostringstream oss;
    oss << "Visualization Context Information:\n";
    oss << "  Initialized: " << (pImpl->initialized ? "Yes" : "No") << "\n";
    oss << "  Brain: " << (pImpl->brain ? "Yes" : "No") << "\n";
    oss << "  Network Stats: " << (pImpl->networkStats.neuronCount > 0 ? "Yes" : "No") << "\n";
    oss << "  Activity Data: " << (pImpl->activityData.steps > 0 ? "Yes" : "No") << "\n";
    oss << "  Spike Data: " << (pImpl->spikeData.totalSpikes > 0 ? "Yes" : "No") << "\n";
    oss << "  Weight Data: " << (pImpl->weightData.weightMatrix.size() > 0 ? "Yes" : "No") << "\n";
    oss << "  Output Format: " << getFormatName(pImpl->options.outputFormat) << "\n";
    return oss.str();
}

std::vector<std::string> VisualizationContext::getAvailableFormats() const {
    std::vector<std::string> formats;
    formats.push_back("TEXT (.txt)");
    formats.push_back("CSV (.csv)");
    formats.push_back("JSON (.json)");
    formats.push_back("HTML (.html)");
    formats.push_back("IMAGE_PNG (.png)");
    formats.push_back("IMAGE_SVG (.svg)");
    formats.push_back("PDF (.pdf)");
    return formats;
}

void VisualizationContext::setOptions(const VisualizationOptions& options) {
    pImpl->options = options;
}

const VisualizationOptions& VisualizationContext::getOptions() const {
    return pImpl->options;
}

std::string VisualizationContext::exportToString(OutputFormat format) {
    std::ostringstream oss;
    oss << "Visualization Data (format: " << getFormatName(format) << ")\n";
    oss << "Network Statistics:\n";
    oss << viz_helpers::generateNetworkReport(pImpl->networkStats);
    oss << "\nActivity Data:\n";
    oss << viz_helpers::generateActivityReport(pImpl->activityData);
    oss << "\nSpike Data:\n";
    oss << viz_helpers::generateSpikeReport(pImpl->spikeData);
    oss << "\nWeight Data:\n";
    oss << viz_helpers::generateWeightReport(pImpl->weightData);
    
    return oss.str();
}

bool VisualizationContext::exportToStream(std::ostream& stream, OutputFormat format) {
    stream << exportToString(format);
    return stream.good();
}

void VisualizationContext::setProgressCallback(std::function<void(double)> callback) {
    pImpl->progressCallback = callback;
}

void VisualizationContext::setErrorCallback(std::function<void(const std::string&)> callback) {
    pImpl->errorCallback = callback;
}

bool VisualizationContext::isInitialized() const {
    return pImpl->initialized;
}

void VisualizationContext::clear() {
    pImpl->brain.reset();
    pImpl->networkStats = NetworkStats();
    pImpl->activityData = ActivityData();
    pImpl->spikeData = SpikeData();
    pImpl->weightData = WeightData();
    pImpl->initialized = false;
}

// Helper functions
std::string VisualizationContext::getFormatName(OutputFormat format) {
    switch (format) {
        case OutputFormat::TEXT: return "TEXT";
        case OutputFormat::CSV: return "CSV";
        case OutputFormat::JSON: return "JSON";
        case OutputFormat::HTML: return "HTML";
        case OutputFormat::IMAGE_PNG: return "PNG";
        case OutputFormat::IMAGE_SVG: return "SVG";
        case OutputFormat::PDF: return "PDF";
        default: return "UNKNOWN";
    }
}

std::string VisualizationContext::getFormatExtension(OutputFormat format) {
    switch (format) {
        case OutputFormat::TEXT: return "txt";
        case OutputFormat::CSV: return "csv";
        case OutputFormat::JSON: return "json";
        case OutputFormat::HTML: return "html";
        case OutputFormat::IMAGE_PNG: return "png";
        case OutputFormat::IMAGE_SVG: return "svg";
        case OutputFormat::PDF: return "pdf";
        default: return "txt";
    }
}

} // namespace visualization
} // namespace nlm