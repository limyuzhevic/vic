#include "VisualizationExporter.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <filesystem>

namespace nlm {

struct VisualizationExporter::Impl {
    std::map<std::string, std::string> exportOptions;
    std::map<std::string, ExportFormat> supportedFormats;
    std::map<std::string, double> exportTimes;
    std::map<std::string, size_t> exportCounts;
    
    size_t totalExports = 0;
    size_t successfulExports = 0;
    size_t failedExports = 0;
    double totalExportTime = 0.0;
    
    Impl() {
        initializeFormats();
        exportOptions = {
            {"csv.delimiter", ","},
            {"csv.quote", "\""},
            {"json.pretty", "true"},
            {"json.indent", "2"},
            {"bin.compression", "gzip"},
            {"text.max_columns", "20"},
            {"text.precision", "6"}
        };
    }
    
    void initializeFormats() {
        // CSV format
        ExportFormat csvFormat;
        csvFormat.name = "CSV";
        csvFormat.extension = "csv";
        csvFormat.mimeType = "text/csv";
        csvFormat.supports2D = true;
        csvFormat.supports3D = false;
        csvFormat.supportsAnimation = false;
        csvFormat.supportsInteractivity = false;
        csvFormat.isTextBased = true;
        csvFormat.isBinaryFormat = false;
        supportedFormats["CSV"] = csvFormat;
        
        // JSON format
        ExportFormat jsonFormat;
        jsonFormat.name = "JSON";
        jsonFormat.extension = "json";
        jsonFormat.mimeType = "application/json";
        jsonFormat.supports2D = true;
        jsonFormat.supports3D = true;
        jsonFormat.supportsAnimation = true;
        jsonFormat.supportsInteractivity = true;
        jsonFormat.isTextBased = true;
        jsonFormat.isBinaryFormat = false;
        supportedFormats["JSON"] = jsonFormat;
        
        // Text format
        ExportFormat textFormat;
        textFormat.name = "TEXT";
        textFormat.extension = "txt";
        textFormat.mimeType = "text/plain";
        textFormat.supports2D = true;
        textFormat.supports3D = false;
        textFormat.supportsAnimation = false;
        textFormat.supportsInteractivity = false;
        textFormat.isTextBased = true;
        textFormat.isBinaryFormat = false;
        supportedFormats["TEXT"] = textFormat;
        
        // Binary format
        ExportFormat binFormat;
        binFormat.name = "BIN";
        binFormat.extension = "bin";
        binFormat.mimeType = "application/octet-stream";
        binFormat.supports2D = true;
        binFormat.supports3D = true;
        binFormat.supportsAnimation = true;
        binFormat.supportsInteractivity = true;
        binFormat.isTextBased = false;
        binFormat.isBinaryFormat = true;
        supportedFormats["BIN"] = binFormat;
    }
};

VisualizationExporter::VisualizationExporter() : pImpl(new Impl) {}

VisualizationExporter::~VisualizationExporter() {
    // Cleanup on destruction
}

void VisualizationExporter::configureExportOptions(const std::map<std::string, std::string>& options) {
    pImpl->exportOptions = options;
    NLM_LOG_INFO("Visualization export options configured");
}

std::map<std::string, std::string> VisualizationExporter::getExportOptions() const {
    return pImpl->exportOptions;
}

VisualizationExportResult VisualizationExporter::exportToCSV(const VisualizationData& data, const std::string& filepath) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    VisualizationExportResult result;
    result.format = "CSV";
    result.filepath = filepath;
    result.exportTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    result.success = false;
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            result.errorMessage = "Failed to open file for writing";
            recordExportStats("CSV", false, 0.0, 0);
            return result;
        }
        
        std::string csvContent = generateCSV(data);
        file.write(csvContent.c_str(), csvContent.size());
        
        file.close();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        size_t dataSize = csvContent.size();
        
        result.success = true;
        result.dataSize = dataSize;
        result.exportDuration = duration;
        
        recordExportStats("CSV", true, duration, dataSize);
        
        NLM_LOG_INFO("CSV export successful: " + filepath + " (" + std::to_string(dataSize) + " bytes)");
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Exception: ") + e.what();
        recordExportStats("CSV", false, 0.0, 0);
    }
    
    return result;
}

VisualizationExportResult VisualizationExporter::exportToJSON(const VisualizationData& data, const std::string& filepath) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    VisualizationExportResult result;
    result.format = "JSON";
    result.filepath = filepath;
    result.exportTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    result.success = false;
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            result.errorMessage = "Failed to open file for writing";
            recordExportStats("JSON", false, 0.0, 0);
            return result;
        }
        
        std::string jsonContent = generateJSON(data);
        file.write(jsonContent.c_str(), jsonContent.size());
        
        file.close();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        size_t dataSize = jsonContent.size();
        
        result.success = true;
        result.dataSize = dataSize;
        result.exportDuration = duration;
        
        recordExportStats("JSON", true, duration, dataSize);
        
        NLM_LOG_INFO("JSON export successful: " + filepath + " (" + std::to_string(dataSize) + " bytes)");
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Exception: ") + e.what();
        recordExportStats("JSON", false, 0.0, 0);
    }
    
    return result;
}

VisualizationExportResult VisualizationExporter::exportToText(const VisualizationData& data, const std::string& filepath) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    VisualizationExportResult result;
    result.format = "TEXT";
    result.filepath = filepath;
    result.exportTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    result.success = false;
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            result.errorMessage = "Failed to open file for writing";
            recordExportStats("TEXT", false, 0.0, 0);
            return result;
        }
        
        std::string textContent = generateText(data);
        file.write(textContent.c_str(), textContent.size());
        
        file.close();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        size_t dataSize = textContent.size();
        
        result.success = true;
        result.dataSize = dataSize;
        result.exportDuration = duration;
        
        recordExportStats("TEXT", true, duration, dataSize);
        
        NLM_LOG_INFO("Text export successful: " + filepath + " (" + std::to_string(dataSize) + " bytes)");
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Exception: ") + e.what();
        recordExportStats("TEXT", false, 0.0, 0);
    }
    
    return result;
}

VisualizationExportResult VisualizationExporter::exportToBin(const VisualizationData& data, const std::string& filepath) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    VisualizationExportResult result;
    result.format = "BIN";
    result.filepath = filepath;
    result.exportTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    result.success = false;
    
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            result.errorMessage = "Failed to open file for writing";
            recordExportStats("BIN", false, 0.0, 0);
            return result;
        }
        
        std::vector<uint8_t> binContent = generateBinary(data);
        file.write(reinterpret_cast<const char*>(binContent.data()), binContent.size());
        
        file.close();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        size_t dataSize = binContent.size();
        
        result.success = true;
        result.dataSize = dataSize;
        result.exportDuration = duration;
        
        recordExportStats("BIN", true, duration, dataSize);
        
        NLM_LOG_INFO("Binary export successful: " + filepath + " (" + std::to_string(dataSize) + " bytes)");
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Exception: ") + e.what();
        recordExportStats("BIN", false, 0.0, 0);
    }
    
    return result;
}

VisualizationExportResult VisualizationExporter::exportToCustomFormat(const VisualizationData& data, const std::string& filepath, 
                                                   const std::string& format) const {
    // For now, delegate to standard formats
    return exportToFormat(data, filepath, format);
}

VisualizationExportResult VisualizationExporter::exportExperimentResults(const std::shared_ptr<Experiment>& experiment,
                                                         const std::string& filepath) const {
    VisualizationData data = collectDataFromExperiment(experiment);
    if (!validateData(data)) {
        VisualizationExportResult result;
        result.format = "EXPERIMENT";
        result.filepath = filepath;
        result.success = false;
        result.errorMessage = "Invalid experiment data";
        return result;
    }
    
    std::string format = detectFormatFromExtension(filepath);
    if (format.empty()) {
        format = "CSV"; // Default format
    }
    
    return exportToFormat(data, filepath, format);
}

VisualizationExportResult VisualizationExporter::exportBrainVisualization(const std::shared_ptr<Brain>& brain,
                                                   const std::string& filepath) const {
    VisualizationData data = collectDataFromBrain(brain);
    if (!validateData(data)) {
        VisualizationExportResult result;
        result.format = "BRAIN";
        result.filepath = filepath;
        result.success = false;
        result.errorMessage = "Invalid brain data";
        return result;
    }
    
    std::string format = detectFormatFromExtension(filepath);
    if (format.empty()) {
        format = "JSON"; // Default format for brain visualization
    }
    
    return exportToFormat(data, filepath, format);
}

VisualizationExportResult VisualizationExporter::exportPerformanceMetrics(const PerformanceMetrics& metrics,
                                                   const std::string& filepath) const {
    VisualizationData data = collectDataFromMetrics(metrics);
    if (!validateData(data)) {
        VisualizationExportResult result;
        result.format = "METRICS";
        result.filepath = filepath;
        result.success = false;
        result.errorMessage = "Invalid metrics data";
        return result;
    }
    
    std::string format = detectFormatFromExtension(filepath);
    if (format.empty()) {
        format = "CSV"; // Default format for metrics
    }
    
    return exportToFormat(data, filepath, format);
}

std::vector<VisualizationExportResult> VisualizationExporter::exportBatch(const std::vector<VisualizationData>& dataList) {
    std::vector<VisualizationExportResult> results;
    results.reserve(dataList.size());
    
    for (const auto& data : dataList) {
        // For batch export, use default format (CSV)
        VisualizationExportResult result = exportToCSV(data, ""); // Empty path for batch exports
        results.push_back(result);
    }
    
    return results;
}

std::string VisualizationExporter::generateCSV(const VisualizationData& data) const {
    std::stringstream ss;
    
    // Write title and metadata
    ss << "# " << data.title << std::endl;
    ss << "# " << data.description << std::endl;
    ss << "# Generated: " << data.timestamp << std::endl;
    ss << "# Experiment: " << data.experimentName << std::endl;
    ss << "# Type: " << data.experimentType << std::endl;
    
    // Write time-series data
    ss << "step,neurons,synapses,spikes,firing_rate" << std::endl;
    for (size_t i = 0; i < data.timestamps.size(); ++i) {
        ss << data.timestamps[i] << ","
           << data.neuronCounts[i] << ","
           << data.synapseCounts[i] << ","
           << data.spikeCounts[i] << ","
           << data.avgFiringRates[i] << std::endl;
    }
    
    // Write neuron data
    ss << "\n# Neuron Data" << std::endl;
    ss << "neuron_id,membrane_potential,firing_rate,active,neuron_type" << std::endl;
    for (size_t i = 0; i < data.neuronIds.size(); ++i) {
        ss << data.neuronIds[i] << ","
           << data.neuronMembranePotentials[i] << ","
           << data.neuronFiringRates[i] << ","
           << (data.neuronActiveStates[i] ? "1" : "0") << ","
           << data.neuronTypes[i] << std::endl;
    }
    
    // Write synapse data
    ss << "\n# Synapse Data" << std::endl;
    ss << "pre_neuron,post_neuron,weight,delay" << std::endl;
    for (size_t i = 0; i < data.synapseConnections.size(); ++i) {
        ss << data.synapseConnections[i].first << ","
           << data.synapseConnections[i].second << ","
           << data.synapseWeights[i] << ","
           << data.synapseDelays[i] << std::endl;
    }
    
    return ss.str();
}

std::string VisualizationExporter::generateJSON(const VisualizationData& data) const {
    std::stringstream ss;
    ss << "{" << std::endl;
    
    // Basic metadata
    ss << "  \"title\": \"" << escapeJSON(data.title) << "\"," << std::endl;
    ss << "  \"description\": \"" << escapeJSON(data.description) << "\"," << std::endl;
    ss << "  \"timestamp\": " << data.timestamp << "," << std::endl;
    ss << "  \"experiment_name\": \"" << escapeJSON(data.experimentName) << "\"," << std::endl;
    ss << "  \"experiment_type\": \"" << escapeJSON(data.experimentType) << "\"," << std::endl;
    ss << "  \"seed\": " << data.seed << "," << std::endl;
    
    // Time-series data
    ss << "  \"time_series\": [" << std::endl;
    for (size_t i = 0; i < data.timestamps.size(); ++i) {
        ss << "    {\"timestamp\": " << data.timestamps[i] << ",\"step\": " << i
           << ",\"neurons\": " << data.neuronCounts[i]
           << ",\"synapses\": " << data.synapseCounts[i]
           << ",\"spikes\": " << data.spikeCounts[i]
           << ",\"firing_rate\": " << data.avgFiringRates[i] << "}";
        if (i < data.timestamps.size() - 1) ss << ",";
        ss << std::endl;
    }
    ss << "  ]," << std::endl;
    
    // Neuron data
    ss << "  \"neurons\": [" << std::endl;
    for (size_t i = 0; i < data.neuronIds.size(); ++i) {
        ss << "    {\"id\": " << data.neuronIds[i]
           << ",\"membrane_potential\": " << data.neuronMembranePotentials[i]
           << ",\"firing_rate\": " << data.neuronFiringRates[i]
           << ",\"active\": " << (data.neuronActiveStates[i] ? "true" : "false")
           << ",\"type\": " << data.neuronTypes[i] << "}";
        if (i < data.neuronIds.size() - 1) ss << ",";
        ss << std::endl;
    }
    ss << "  ]," << std::endl;
    
    // Synapse data
    ss << "  \"synapses\": [" << std::endl;
    for (size_t i = 0; i < data.synapseConnections.size(); ++i) {
        ss << "    {\"pre\": " << data.synapseConnections[i].first
           << ",\"post\": " << data.synapseConnections[i].second
           << ",\"weight\": " << data.synapseWeights[i]
           << ",\"delay\": " << data.synapseDelays[i] << "}";
        if (i < data.synapseConnections.size() - 1) ss << ",";
        ss << std::endl;
    }
    ss << "  ]" << std::endl;
    ss << "}" << std::endl;
    
    return ss.str();
}

std::string VisualizationExporter::generateText(const VisualizationData& data) const {
    std::stringstream ss;
    
    ss << "Visualization Data Export" << std::endl;
    ss << "=========================" << std::endl;
    ss << std::endl;
    
    ss << "Experiment Information:" << std::endl;
    ss << "  Title: " << data.title << std::endl;
    ss << "  Description: " << data.description << std::endl;
    ss << "  Experiment: " << data.experimentName << std::endl;
    ss << "  Type: " << data.experimentType << std::endl;
    ss << "  Seed: " << data.seed << std::endl;
    ss << "  Start Step: " << data.startStep << std::endl;
    ss << "  End Step: " << data.endStep << std::endl;
    ss << std::endl;
    
    ss << "Summary Statistics:" << std::endl;
    ss << "  Total Neurons: " << data.neuronIds.size() << std::endl;
    ss << "  Total Synapses: " << data.synapseConnections.size() << std::endl;
    ss << "  Time Points: " << data.timestamps.size() << std::endl;
    ss << "  Performance Metrics: " << data.performanceMetrics.size() << std::endl;
    ss << std::endl;
    
    // Show some key metrics
    if (!data.performanceMetrics.empty()) {
        ss << "Key Metrics:" << std::endl;
        for (const auto& pair : data.performanceMetrics) {
            if (!pair.second.empty()) {
                double sum = std::accumulate(pair.second.begin(), pair.second.end(), 0.0);
                double avg = sum / pair.second.size();
                ss << "  " << pair.first << ": " << avg << " (" << pair.second.size() << " samples)" << std::endl;
            }
        }
    }
    
    return ss.str();
}

std::vector<uint8_t> VisualizationExporter::generateBinary(const VisualizationData& data) const {
    std::vector<uint8_t> buffer;
    
    // Simple serialization (would use proper serialization in production)
    auto writeToBuffer = [&buffer](const void* ptr, size_t size) {
        const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(ptr);
        buffer.insert(buffer.end(), bytePtr, bytePtr + size);
    };
    
    // Write magic number
    uint32_t magic = 0xVIZDATA;
    writeToBuffer(&magic, sizeof(magic));
    
    // Write version
    uint32_t version = 1;
    writeToBuffer(&version, sizeof(version));
    
    // Write data
    writeToBuffer(&data.title, data.title.size() + 1);
    writeToBuffer(&data.description, data.description.size() + 1);
    writeToBuffer(&data.timestamp, sizeof(data.timestamp));
    writeToBuffer(&data.startStep, sizeof(data.startStep));
    writeToBuffer(&data.endStep, sizeof(data.endStep));
    
    // Write arrays
    size_t size = data.neuronIds.size();
    writeToBuffer(&size, sizeof(size));
    for (size_t id : data.neuronIds) writeToBuffer(&id, sizeof(id));
    
    size = data.neuronMembranePotentials.size();
    writeToBuffer(&size, sizeof(size));
    for (float val : data.neuronMembranePotentials) writeToBuffer(&val, sizeof(val));
    
    return buffer;
}

VisualizationData VisualizationExporter::collectDataFromExperiment(const std::shared_ptr<Experiment>& experiment) const {
    VisualizationData data;
    
    if (!experiment) {
        return data;
    }
    
    data.title = "Experiment Results: " + experiment->getName();
    data.description = "Visualization of experiment " + experiment->getName();
    data.experimentName = experiment->getName();
    data.seed = experiment->getSeed();
    data.experimentType = "Experiment"; // Would need to map to enum string
    
    // Get experiment step data
    const auto& stepData = experiment->getStepData();
    for (const auto& step : stepData) {
        data.timestamps.push_back(static_cast<Timestamp>(step.step));
        data.neuronCounts.push_back(step.neuronCount);
        data.synapseCounts.push_back(step.synapseCount);
        data.spikeCounts.push_back(step.firingCount);
        data.avgFiringRates.push_back(step.avgFiringRate);
    }
    
    return data;
}

VisualizationData VisualizationExporter::collectDataFromBrain(const std::shared_ptr<Brain>& brain) const {
    VisualizationData data;
    
    if (!brain) {
        return data;
    }
    
    data.title = "Brain State Visualization";
    data.description = "Visualization of brain neural activity";
    data.experimentName = "Brain Analysis";
    data.experimentType = "Brain State";
    data.seed = 0; // Would get from brain config
    
    // Get brain statistics
    size_t neuronCount = brain->getTotalNeuronCount();
    size_t synapseCount = brain->getTotalSynapseCount();
    size_t spikeCount = brain->getTotalSpikeCount();
    
    // Simplified data collection (would access brain internals in full implementation)
    data.neuronIds.resize(neuronCount);
    data.neuronMembranePotentials.resize(neuronCount, -70.0f);
    data.neuronFiringRates.resize(neuronCount, 0.0f);
    data.neuronActiveStates.resize(neuronCount, false);
    data.neuronTypes.resize(neuronCount, 0.0f);
    
    data.synapseConnections.resize(std::min(neuronCount * 10, synapseCount));
    data.synapseWeights.resize(data.synapseConnections.size(), 0.5f);
    data.synapseDelays.resize(data.synapseConnections.size(), 1.0f);
    
    return data;
}

VisualizationData VisualizationExporter::collectDataFromMetrics(const PerformanceMetrics& metrics) const {
    VisualizationData data;
    
    data.title = "Performance Metrics Visualization";
    data.description = "Visualization of performance metrics";
    data.experimentName = "Performance Analysis";
    data.experimentType = "Metrics";
    
    // Add metrics as performance data
    for (const std::string& name : metrics.getMetricNames()) {
        std::vector<double> history = metrics.getMetricHistory(name);
        data.performanceMetrics[name] = history;
        
        // Add to time series if empty
        if (data.timestamps.empty() && !history.empty()) {
            for (size_t i = 0; i < history.size(); ++i) {
                data.timestamps.push_back(static_cast<Timestamp>(i * 0.1)); // 0.1s intervals
                data.neuronCounts.push_back(static_cast<size_t>(i * 100)); // Mock data
                data.spikeCounts.push_back(static_cast<size_t>(history[i]));
                data.avgFiringRates.push_back(history[i]);
            }
        }
    }
    
    return data;
}

VisualizationData VisualizationExporter::collectCombinedData(const std::shared_ptr<Experiment>& experiment,
                                             const std::shared_ptr<Brain>& brain,
                                             const PerformanceMetrics& metrics) const {
    VisualizationData data;
    
    // Combine data from all sources
    if (experiment) {
        VisualizationData expData = collectDataFromExperiment(experiment);
        data = expData;
    }
    
    if (brain) {
        VisualizationData brainData = collectDataFromBrain(brain);
        if (data.experimentName.empty()) {
            data = brainData;
        } else {
            // Merge data
            // Simplified merging
        }
    }
    
    if (metrics.getMetricNames().empty() == false) {
        VisualizationData metricsData = collectDataFromMetrics(metrics);
        if (data.experimentName.empty()) {
            data = metricsData;
        } else {
            // Merge data
            // Simplified merging
        }
    }
    
    return data;
}

bool VisualizationExporter::validateData(const VisualizationData& data) const {
    if (data.neuronIds.size() != data.neuronMembranePotentials.size() ||
        data.neuronIds.size() != data.neuronFiringRates.size() ||
        data.neuronIds.size() != data.neuronActiveStates.size() ||
        data.neuronIds.size() != data.neuronTypes.size()) {
        return false;
    }
    
    if (data.synapseConnections.size() != data.synapseWeights.size() ||
        data.synapseConnections.size() != data.synapseDelays.size()) {
        return false;
    }
    
    if (data.neuronCounts.size() != data.synapseCounts.size() ||
        data.neuronCounts.size() != data.spikeCounts.size() ||
        data.neuronCounts.size() != data.avgFiringRates.size()) {
        return false;
    }
    
    return true;
}

VisualizationData VisualizationExporter::preprocessData(const VisualizationData& data) const {
    VisualizationData processed = data;
    
    // Normalize firing rates
    if (!processed.neuronFiringRates.empty()) {
        double maxRate = *std::max_element(processed.neuronFiringRates.begin(), processed.neuronFiringRates.end());
        if (maxRate > 0) {
            for (float& rate : processed.neuronFiringRates) {
                rate /= maxRate;
            }
        }
    }
    
    // Normalize weights
    if (!processed.synapseWeights.empty()) {
        double maxWeight = *std::max_element(processed.synapseWeights.begin(), processed.synapseWeights.end());
        if (maxWeight > 0) {
            for (float& weight : processed.synapseWeights) {
                weight /= maxWeight;
            }
        }
    }
    
    return processed;
}

std::string VisualizationExporter::detectFormatFromExtension(const std::string& filepath) const {
    if (filepath.empty()) {
        return "";
    }
    
    std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "csv") return "CSV";
    if (extension == "json") return "JSON";
    if (extension == "txt") return "TEXT";
    if (extension == "bin") return "BIN";
    
    return "";
}

bool VisualizationExporter::canExportFormat(const std::string& format) const {
    return pImpl->supportedFormats.find(format) != pImpl->supportedFormats.end();
}

std::vector<std::string> VisualizationExporter::getSupportedFormats() const {
    std::vector<std::string> formats;
    for (const auto& pair : pImpl->supportedFormats) {
        formats.push_back(pair.first);
    }
    return formats;
}

double VisualizationExporter::getExportTime(const std::string& format) const {
    auto it = pImpl->exportTimes.find(format);
    if (it == pImpl->exportTimes.end() || it->second == 0.0) {
        return 0.0;
    }
    return it->second / pImpl->exportCounts.at(format);
}

size_t VisualizationExporter::getExportCount(const std::string& format) const {
    auto it = pImpl->exportCounts.find(format);
    if (it == pImpl->exportCounts.end()) {
        return 0;
    }
    return it->second;
}

std::map<std::string, double> VisualizationExporter::getPerformanceStats() const {
    std::map<std::string, double> stats;
    
    for (const auto& pair : pImpl->exportTimes) {
        const std::string& format = pair.first;
        double totalTime = pair.second;
        size_t count = pImpl->exportCounts.at(format);
        
        if (count > 0) {
            stats[format + "_avg_time"] = totalTime / count;
            stats[format + "_success_rate"] = static_cast<double>(pImpl->exportCounts.at(format)) / pImpl->totalExports;
        }
    }
    
    stats["total_exports"] = static_cast<double>(pImpl->totalExports);
    stats["success_rate"] = static_cast<double>(pImpl->successfulExports) / pImpl->totalExports;
    
    return stats;
}

void VisualizationExporter::resetStatistics() {
    pImpl->totalExports = 0;
    pImpl->successfulExports = 0;
    pImpl->failedExports = 0;
    pImpl->totalExportTime = 0.0;
    
    pImpl->exportTimes.clear();
    pImpl->exportCounts.clear();
}

size_t VisualizationExporter::getTotalExports() const {
    return pImpl->totalExports;
}

size_t VisualizationExporter::getSuccessfulExports() const {
    return pImpl->successfulExports;
}

size_t VisualizationExporter::getFailedExports() const {
    return pImpl->failedExports;
}

double VisualizationExporter::getSuccessRate() const {
    if (pImpl->totalExports == 0) {
        return 0.0;
    }
    return static_cast<double>(pImpl->successfulExports) / pImpl->totalExports;
}

VisualizationExportResult VisualizationExporter::exportToFormat(const VisualizationData& data, const std::string& filepath,
                                                        const std::string& format) const {
    if (!canExportFormat(format)) {
        VisualizationExportResult result;
        result.format = format;
        result.filepath = filepath;
        result.success = false;
        result.errorMessage = "Unsupported export format: " + format;
        return result;
    }
    
    if (format == "CSV") {
        return exportToCSV(data, filepath);
    } else if (format == "JSON") {
        return exportToJSON(data, filepath);
    } else if (format == "TEXT") {
        return exportToText(data, filepath);
    } else if (format == "BIN") {
        return exportToBin(data, filepath);
    } else {
        VisualizationExportResult result;
        result.format = format;
        result.filepath = filepath;
        result.success = false;
        result.errorMessage = "Unknown export format: " + format;
        return result;
    }
}

void VisualizationExporter::recordExportStats(const std::string& format, bool success, double duration, size_t dataSize) {
    pImpl->totalExports++;
    pImpl->totalExportTime += duration;
    
    if (success) {
        pImpl->successfulExports++;
    } else {
        pImpl->failedExports++;
    }
    
    pImpl->exportTimes[format] += duration;
    pImpl->exportCounts[format]++;
}

std::string VisualizationExporter::escapeJSON(const std::string& value) const {
    std::string escaped = value;
    // Simple JSON escaping (would use proper implementation)
    size_t pos = 0;
    while ((pos = escaped.find('"', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\\"");
        pos += 2;
    }
    return escaped;
}

std::string VisualizationExporter::escapeCSV(const std::string& value) const {
    std::string escaped = value;
    // Simple CSV escaping
    if (value.find(',') != std::string::npos || value.find('"') != std::string::npos || value.find('\n') != std::string::npos) {
        escaped = "\"" + escaped + "\"";
    }
    return escaped;
}

} // namespace nlm
