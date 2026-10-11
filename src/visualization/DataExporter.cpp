// DataExporter.cpp - Data export system implementation

#include "DataExporter.hpp"
#include "Brain.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "SpikeEvent.hpp"
#include "SpikeSystem.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <limits>

namespace nlm {

struct DataExporter::Impl {
    ExportConfig config;
    ExportCallback progressCallback;
    std::vector<DataTransform> dataTransforms;
    std::vector<DatasetInfo> exportStats;
    bool initialized;
    std::chrono::steady_clock::time_point lastProgressUpdate;
    
    Impl() : initialized(false), lastProgressUpdate(std::chrono::steady_clock::now()) {
    }
};

DataExporter::DataExporter() : pImpl(std::make_unique<Impl>()) {
}

DataExporter::DataExporter(const ExportConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
}

DataExporter::~DataExporter() {
}

void DataExporter::configure(const ExportConfig& config) {
    pImpl->config = config;
}

bool DataExporter::exportBrainData(const Brain& brain, const std::string& datasetName) const {
    return exportCustomData(convertBrainToRows(brain), datasetName);
}

bool DataExporter::exportNeuronData(const Brain& brain, const std::string& datasetName) const {
    std::vector<std::vector<std::string>> neuronData;
    
    // Add header
    neuronData.push_back({"neuronId", "region", "type", "x", "y", "z", 
                         "radius", "inDegree", "outDegree", "totalDegree",
                         "excitatoryConnections", "inhibitoryConnections"});
    
    for (const auto& neuron : brain.getNeurons()) {
        neuronData.push_back({
            std::to_string(neuron.getId()),
            neuron.getRegion(),
            neuron.getType(),
            std::to_string(neuron.getPosition().x),
            std::to_string(neuron.getPosition().y),
            std::to_string(neuron.getPosition().z),
            std::to_string(neuron.getRadius()),
            std::to_string(calculateInDegree(neuron, brain)),
            std::to_string(calculateOutDegree(neuron, brain)),
            std::to_string(calculateTotalDegree(neuron, brain)),
            std::to_string(countConnections(neuron, brain, "excitatory")),
            std::to_string(countConnections(neuron, brain, "inhibitory"))
        });
    }
    
    return exportCustomData(neuronData, datasetName);
}

bool DataExporter::exportSynapseData(const Brain& brain, const std::string& datasetName) const {
    std::vector<std::vector<std::string>> synapseData;
    
    // Add header
    synapseData.push_back({"synapseId", "preNeuronId", "postNeuronId", "weight",
                         "delay", "conductance", "type", "plasticityRule",
                         "isExcitatory", "isActive", "lastUpdate"});
    
    for (const auto& synapse : brain.getSynapses()) {
        synapseData.push_back({
            std::to_string(synapse.getId()),
            std::to_string(synapse.getPreNeuronId()),
            std::to_string(synapse.getPostNeuronId()),
            std::to_string(synapse.getWeight()),
            std::to_string(synapse.getDelay()),
            std::to_string(synapse.getConductance()),
            synapse.getType(),
            synapse.getPlasticityRule(),
            synapse.isExcitatory() ? "true" : "false",
            synapse.isActive() ? "true" : "false",
            std::to_string(synapse.getLastUpdate())
        });
    }
    
    return exportCustomData(synapseData, datasetName);
}

bool DataExporter::exportSpikeData(const std::vector<SpikeEvent>& spikes, const std::string& datasetName) const {
    std::vector<std::vector<std::string>> spikeData;
    
    // Add header
    spikeData.push_back({"time", "neuronId", "neuronType", "region", "spikeCount",
                         "firingRate", "isSynchronized", "correlationWithPrevious"});
    
    // Group spikes by neuron
    std::map<int64_t, std::vector<SpikeEvent>> spikesByNeuron;
    for (const auto& spike : spikes) {
        spikesByNeuron[spike.neuronId].push_back(spike);
    }
    
    // Calculate statistics
    for (const auto& pair : spikesByNeuron) {
        int64_t neuronId = pair.first;
        const auto& neuronSpikes = pair.second;
        
        // Find neuron info
        std::string neuronType = "unknown";
        std::string region = "unknown";
        
        // In real implementation, would find neuron from brain
        
        // Sort spikes by time
        std::vector<int64_t> spikeTimes;
        for (const auto& spike : neuronSpikes) {
            spikeTimes.push_back(spike.time);
        }
        std::sort(spikeTimes.begin(), spikeTimes.end());
        
        // Calculate firing rate
        double firingRate = 0.0;
        if (!spikeTimes.empty() && spikeTimes.size() > 1) {
            double timeSpan = spikeTimes.back() - spikeTimes.front();
            if (timeSpan > 0) {
                firingRate = (spikeTimes.size() - 1) / timeSpan;
            }
        }
        
        // Calculate synchronization (simplified)
        bool isSynchronized = false;
        double correlationWithPrevious = 0.0;
        
        spikeData.push_back({
            "[" + vectorToString(spikeTimes) + "]",
            std::to_string(neuronId),
            neuronType,
            region,
            std::to_string(spikeTimes.size()),
            std::to_string(firingRate),
            isSynchronized ? "true" : "false",
            std::to_string(correlationWithPrevious)
        });
    }
    
    return exportCustomData(spikeData, datasetName);
}

bool DataExporter::exportPerformanceData(const std::string& datasetName) const {
    std::vector<std::vector<std::string>> performanceData;
    
    // Add header
    performanceData.push_back({"timestamp", "simulationTime", "neuronCount", "synapseCount",
                               "spikeCount", "simulationStep", "memoryUsageMB",
                               "cpuUsagePercent", "simulationTimeMs", "performanceScore",
                               "learningProgress", "convergenceRate"});
    
    // In real implementation, would collect performance metrics from simulation
    // This is a simplified placeholder
    
    performanceData.push_back({
        getCurrentTime(),
        "0.0",
        "0",
        "0",
        "0",
        "0",
        "0.0",
        "0.0",
        "0.0",
        "0.0",
        "0.0",
        "0.0"
    });
    
    return exportCustomData(performanceData, datasetName);
}

bool DataExporter::exportConfigData(const std::string& datasetName) const {
    // Export configuration as JSON
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"version\": \"" << pImpl->config.version << "\",\n";
    oss << "  \"format\": \"" << exportFormatToString(pImpl->config.format) << "\",\n";
    oss << "  \"experimentName\": \"" << pImpl->config.experimentName << "\",\n";
    oss << "  \"outputPath\": \"" << pImpl->config.outputPath << "\",\n";
    oss << "  \"compress\": " << (pImpl->config.compress ? "true" : "false") << ",\n";
    oss << "  \"includeMetadata\": " << (pImpl->config.includeMetadata ? "true" : "false") << ",\n";
    oss << "  \"delimiter\": \"" << escapeJsonString(pImpl->config.delimiter) << "\",\n";
    oss << "  \"encoding\": \"" << pImpl->config.encoding << "\"\n";
    oss << "}\n";
    
    std::string jsonContent = oss.str();
    
    // Write to file
    std::string fullPath = pImpl->config.outputPath + "/" + datasetName + ".json";
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + fullPath);
        return false;
    }
    
    file << jsonContent;
    file.close();
    
    // Record dataset info
    DatasetInfo info;
    info.name = datasetName;
    info.format = "JSON";
    info.sizeMB = static_cast<double>(jsonContent.size()) / (1024.0 * 1024.0);
    info.created = std::chrono::system_clock::now();
    info.creator = "NLM DataExporter";
    
    pImpl->exportStats.push_back(info);
    
    NLM_LOG_INFO("Configuration exported as JSON: " + fullPath);
    return true;
}

bool DataExporter::exportExperimentResults(const std::string& experimentPath, const std::string& datasetName) const {
    // Export experiment results by recursively copying data files
    std::error_code ec;
    bool success = std::filesystem::copy(experimentPath, pImpl->config.outputPath + "/" + datasetName, 
                                         std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing, ec);
    
    if (ec) {
        NLM_LOG_ERROR("Failed to copy experiment results: " + ec.message());
        return false;
    }
    
    NLM_LOG_INFO("Experiment results exported: " + experimentPath + " -> " + datasetName);
    return true;
}

bool DataExporter::exportCustomData(const std::vector<std::vector<std::string>>& data, const std::string& datasetName) const {
    if (data.empty()) {
        NLM_LOG_WARNING("Empty data provided for export");
        return false;
    }
    
    // Apply data transformations
    std::vector<std::vector<std::string>> transformedData = data;
    for (const auto& transform : pImpl->dataTransforms) {
        transformedData = transform(transformedData);
    }
    
    // Determine output format
    ExportFormat format = pImpl->config.format;
    
    // Create output directory if it doesn't exist
    std::error_code ec;
    std::filesystem::create_directories(pImpl->config.outputPath, ec);
    if (ec) {
        NLM_LOG_ERROR("Failed to create output directory: " + ec.message());
        return false;
    }
    
    // Generate full filename
    std::string extension = getExportFormatExtension(format);
    std::string fullFilename = pImpl->config.filename;
    if (!extension.empty()) {
        fullFilename += "_" + datasetName + extension;
    } else {
        fullFilename += "_" + datasetName + "." + exportFormatToString(format);
    }
    
    std::string fullPath = pImpl->config.outputPath + "/" + fullFilename;
    
    // Export based on format
    bool result = false;
    switch (format) {
        case ExportFormat::CSV:
            result = exportToCsv(transformedData, fullPath);
            break;
        case ExportFormat::JSON:
            result = exportToJson(transformedData, fullPath, datasetName);
            break;
        case ExportFormat::HDF5:
            result = exportToHdf5(transformedData, fullPath, datasetName);
            break;
        case ExportFormat::MATLAB:
            result = exportToMatlab(transformedData, fullPath, datasetName);
            break;
        case ExportFormat::SQLITE:
            result = exportToSqlite(transformedData, fullPath, datasetName);
            break;
        case ExportFormat::XML:
            result = exportToXml(transformedData, fullPath, datasetName);
            break;
        case ExportFormat::BINARY:
            result = exportToBinary(transformedData, fullPath, datasetName);
            break;
        case ExportFormat::PARQUET:
            result = exportToParquet(transformedData, fullPath, datasetName);
            break;
        case ExportFormat::TEXT:
            result = exportToText(transformedData, fullPath, datasetName);
            break;
        case ExportFormat::YAML:
            result = exportToYaml(transformedData, fullPath, datasetName);
            break;
        case ExportFormat::TOML:
            result = exportToToml(transformedData, fullPath, datasetName);
            break;
        default:
            result = exportToCsv(transformedData, fullPath);
            break;
    }
    
    if (result) {
        // Record dataset info
        DatasetInfo info;
        info.name = datasetName;
        info.format = exportFormatToString(format);
        info.rows = data.size() - 1; // Subtract header
        info.columns = data.empty() ? 0 : data[0].size();
        info.sizeMB = static_cast<double>(data.size() * info.columns * 10) / (1024.0 * 1024.0); // Rough estimate
        info.created = std::chrono::system_clock::now();
        info.creator = "NLM DataExporter";
        
        pImpl->exportStats.push_back(info);
        
        NLM_LOG_INFO("Data exported as " + exportFormatToString(format) + ": " + fullPath);
    }
    
    return result;
}

void DataExporter::setExportCallback(const ExportCallback& callback) {
    pImpl->progressCallback = callback;
}

void DataExporter::addDataTransformation(const DataTransform& transform) {
    pImpl->dataTransforms.push_back(transform);
}

std::vector<DataExporter::DatasetInfo> DataExporter::getExportStats() const {
    return pImpl->exportStats;
}

DataExporter::ExportConfig DataExporter::getConfig() const {
    return pImpl->config;
}

bool DataExporter::isFormatSupported(ExportFormat format) {
    switch (format) {
        case ExportFormat::CSV:
        case ExportFormat::JSON:
        case ExportFormat::MATLAB:
        case ExportFormat::TEXT:
            return true;
        default:
            return false;
    }
}

std::vector<DataExporter::ExportFormat> DataExporter::getSupportedFormats() {
    return {
        ExportFormat::CSV,
        ExportFormat::JSON,
        ExportFormat::MATLAB,
        ExportFormat::TEXT
    };
}

// Private helper methods
bool DataExporter::exportToCsv(const std::vector<std::vector<std::string>>& data, const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[i].size(); ++j) {
            file << data[i][j];
            if (j < data[i].size() - 1) {
                file << pImpl->config.delimiter;
            }
        }
        if (i < data.size() - 1) {
            file << "\n";
        }
    }
    
    file.close();
    return true;
}

bool DataExporter::exportToJson(const std::vector<std::vector<std::string>>& data, const std::string& filename, const std::string& datasetName) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    file << "{\n";
    file << "  \"dataset\": \"" << datasetName << "\",\n";
    file << "  \"format\": \"CSV\",\n";
    file << "  \"rows\": " << (data.size() - 1) << ",\n";
    file << "  \"columns\": " << (data.empty() ? 0 : data[0].size()) << ",\n";
    file << "  \"data\": [\n";
    
    for (size_t i = 1; i < data.size(); ++i) { // Skip header
        file << "    [";
        for (size_t j = 0; j < data[i].size(); ++j) {
            file << "\"" << escapeJsonString(data[i][j]) << "\"";
            if (j < data[i].size() - 1) {
                file << ", ";
            }
        }
        file << "]";
        if (i < data.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    return true;
}

bool DataExporter::exportToHdf5(const std::vector<std::vector<std::string>>& data, const std::string& filename, const std::string& datasetName) const {
    // Placeholder for HDF5 export
    NLM_LOG_WARNING("HDF5 export not yet implemented for DataExporter");
    return false;
}

bool DataExporter::exportToMatlab(const std::vector<std::vector<std::string>>& data, const std::string& filename, const std::string& datasetName) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    // Export as MATLAB .mat file format (simplified as text)
    file << "% MATLAB data export from NLM\n";
    file << "% Dataset: " << datasetName << "\n";
    file << "% Rows: " << (data.size() - 1) << "\n";
    file << "% Columns: " << (data.empty() ? 0 : data[0].size()) << "\n\n";
    
    // Write data as matrix
    file << "data = [\n";
    for (size_t i = 1; i < data.size(); ++i) { // Skip header
        file << "  [";
        for (size_t j = 0; j < data[i].size(); ++j) {
            file << data[i][j];
            if (j < data[i].size() - 1) {
                file << " , ";
            }
        }
        file << "]";
        if (i < data.size() - 1) {
            file << ";\n";
        } else {
            file << "\n";
        }
    }
    file << "];\n\n";
    
    // Write header if present
    if (!data.empty()) {
        file << "header = {";
        for (size_t j = 0; j < data[0].size(); ++j) {
            file << "\"" << escapeMatlabString(data[0][j]) << "\"";
            if (j < data[0].size() - 1) {
                file << ", ";
            }
        }
        file << "};\n";
    }
    
    file.close();
    return true;
}

bool DataExporter::exportToSqlite(const std::vector<std::vector<std::string>>& data, const std::string& filename, const std::string& datasetName) const {
    // Placeholder for SQLite export
    NLM_LOG_WARNING("SQLite export not yet implemented for DataExporter");
    return false;
}

bool DataExporter::exportToXml(const std::vector<std::vector<std::string>>& data, const std::string& filename, const std::string& datasetName) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<dataset name=\"" << datasetName << "\">\n";
    file << "  <metadata>\n";
    file << "    <creator>NLM DataExporter</creator>\n";
    file << "    <format>CSV export</format>\n";
    file << "  </metadata>\n";
    file << "  <data>\n";
    
    for (size_t i = 1; i < data.size(); ++i) { // Skip header
        file << "    <row>\n";
        for (size_t j = 0; j < data[i].size(); ++j) {
            file << "      <col name=\"col" << j << "\">" << escapeXmlString(data[i][j]) << "</col>\n";
        }
        file << "    </row>\n";
    }
    
    file << "  </data>\n";
    file << "</dataset>\n";
    
    file.close();
    return true;
}

bool DataExporter::exportToBinary(const std::vector<std::vector<std::string>>& data, const std::string& filename, const std::string& datasetName) const {
    // Placeholder for binary export
    NLM_LOG_WARNING("Binary export not yet implemented for DataExporter");
    return false;
}

bool DataExporter::exportToParquet(const std::vector<std::vector<std::string>>& data, const std::string& filename, const std::string& datasetName) const {
    // Placeholder for Parquet export
    NLM_LOG_WARNING("Parquet export not yet implemented for DataExporter");
    return false;
}

bool DataExporter::exportToText(const std::vector<std::vector<std::string>>& data, const std::string& filename, const std::string& datasetName) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    file << "Dataset: " << datasetName << "\n";
    file << "Rows: " << (data.size() - 1) << "\n";
    file << "Columns: " << (data.empty() ? 0 : data[0].size()) << "\n\n";
    
    if (!data.empty()) {
        // Print header
        for (size_t j = 0; j < data[0].size(); ++j) {
            file << std::left << std::setw(20) << data[0][j];
        }
        file << "\n";
        
        // Print separator
        for (size_t j = 0; j < data[0].size(); ++j) {
            file << std::left << std::setw(20) << std::string(20, '-');
        }
        file << "\n";
        
        // Print data rows
        for (size_t i = 1; i < data.size(); ++i) {
            for (size_t j = 0; j < data[i].size(); ++j) {
                file << std::left << std::setw(20) << data[i][j];
            }
            file << "\n";
        }
    }
    
    file.close();
    return true;
}

bool DataExporter::exportToYaml(const std::vector<std::vector<std::string>>& data, const std::string& filename, const std::string& datasetName) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    file << "# YAML export from NLM\n";
    file << "dataset: " << datasetName << "\n";
    file << "format: CSV export\n";
    file << "rows: " << (data.size() - 1) << "\n";
    file << "columns: " << (data.empty() ? 0 : data[0].size()) << "\n";
    file << "\n";
    file << "data:\n";
    
    for (size_t i = 1; i < data.size(); ++i) { // Skip header
        file << "  row_" << i-1 << ":\n";
        for (size_t j = 0; j < data[i].size(); ++j) {
            file << "    col_" << j << ": " << data[i][j] << "\n";
        }
    }
    
    file.close();
    return true;
}

bool DataExporter::exportToToml(const std::vector<std::vector<std::string>>& data, const std::string& filename, const std::string& datasetName) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    file << "# TOML export from NLM\n";
    file << "[dataset]\n";
    file << "name = \"" << datasetName << "\"\n";
    file << "format = \"CSV export\"\n";
    file << "rows = " << (data.size() - 1) << "\n";
    file << "columns = " << (data.empty() ? 0 : data[0].size()) << "\n\n";
    
    file << "[metadata]\n";
    file << "creator = \"NLM DataExporter\"\n";
    
    file << "\n[data]\n";
    for (size_t i = 1; i < data.size(); ++i) { // Skip header
        file << "row_" << i-1 << " = {\n";
        for (size_t j = 0; j < data[i].size(); ++j) {
            file << "  col_" << j << " = \"" << escapeTomlString(data[i][j]) << "\"\n";
        }
        file << "}\n";
    }
    
    file.close();
    return true;
}

std::vector<std::vector<std::string>> DataExporter::convertBrainToRows(const Brain& brain) const {
    std::vector<std::vector<std::string>> result;
    
    // Add metadata header
    result.push_back({"type", "id", "region", "x", "y", "z", "radius", 
                     "inDegree", "outDegree", "excitatoryConns", "inhibitoryConns",
                     "weightSum", "avgWeight", "minWeight", "maxWeight"});
    
    // Add neuron data
    for (const auto& neuron : brain.getNeurons()) {
        result.push_back({
            "neuron",
            std::to_string(neuron.getId()),
            neuron.getRegion(),
            std::to_string(neuron.getPosition().x),
            std::to_string(neuron.getPosition().y),
            std::to_string(neuron.getPosition().z),
            std::to_string(neuron.getRadius()),
            std::to_string(calculateInDegree(neuron, brain)),
            std::to_string(calculateOutDegree(neuron, brain)),
            std::to_string(countConnections(neuron, brain, "excitatory")),
            std::to_string(countConnections(neuron, brain, "inhibitory")),
            std::to_string(calculateWeightSum(neuron, brain)),
            std::to_string(calculateAverageWeight(neuron, brain)),
            std::to_string(calculateMinWeight(neuron, brain)),
            std::to_string(calculateMaxWeight(neuron, brain))
        });
    }
    
    // Add synapse data
    result.push_back({"type", "id", "preNeuronId", "postNeuronId", 
                     "weight", "delay", "conductance", "plasticityRule",
                     "isExcitatory", "isActive", "lastUpdate"});
    
    for (const auto& synapse : brain.getSynapses()) {
        result.push_back({
            "synapse",
            std::to_string(synapse.getId()),
            std::to_string(synapse.getPreNeuronId()),
            std::to_string(synapse.getPostNeuronId()),
            std::to_string(synapse.getWeight()),
            std::to_string(synapse.getDelay()),
            std::to_string(synapse.getConductance()),
            synapse.getPlasticityRule(),
            synapse.isExcitatory() ? "true" : "false",
            synapse.isActive() ? "true" : "false",
            std::to_string(synapse.getLastUpdate())
        });
    }
    
    return result;
}

int DataExporter::calculateInDegree(const Neuron& neuron, const Brain& brain) const {
    int count = 0;
    for (const auto& synapse : brain.getSynapses()) {
        if (synapse.getPostNeuronId() == neuron.getId()) {
            count++;
        }
    }
    return count;
}

int DataExporter::calculateOutDegree(const Neuron& neuron, const Brain& brain) const {
    int count = 0;
    for (const auto& synapse : brain.getSynapses()) {
        if (synapse.getPreNeuronId() == neuron.getId()) {
            count++;
        }
    }
    return count;
}

int DataExporter::calculateTotalDegree(const Neuron& neuron, const Brain& brain) const {
    return calculateInDegree(neuron, brain) + calculateOutDegree(neuron, brain);
}

int DataExporter::countConnections(const Neuron& neuron, const Brain& brain, const std::string& type) const {
    int count = 0;
    for (const auto& synapse : brain.getSynapses()) {
        if (synapse.getType() == type) {
            if (synapse.getPreNeuronId() == neuron.getId() || synapse.getPostNeuronId() == neuron.getId()) {
                count++;
            }
        }
    }
    return count;
}

double DataExporter::calculateWeightSum(const Neuron& neuron, const Brain& brain) const {
    double sum = 0.0;
    for (const auto& synapse : brain.getSynapses()) {
        if (synapse.getPreNeuronId() == neuron.getId() || synapse.getPostNeuronId() == neuron.getId()) {
            sum += synapse.getWeight();
        }
    }
    return sum;
}

double DataExporter::calculateAverageWeight(const Neuron& neuron, const Brain& brain) const {
    double sum = calculateWeightSum(neuron, brain);
    int count = calculateTotalDegree(neuron, brain);
    return (count > 0) ? sum / count : 0.0;
}

double DataExporter::calculateMinWeight(const Neuron& neuron, const Brain& brain) const {
    double min = std::numeric_limits<double>::max();
    bool found = false;
    
    for (const auto& synapse : brain.getSynapses()) {
        if (synapse.getPreNeuronId() == neuron.getId() || synapse.getPostNeuronId() == neuron.getId()) {
            if (synapse.getWeight() < min) {
                min = synapse.getWeight();
                found = true;
            }
        }
    }
    
    return found ? min : 0.0;
}

double DataExporter::calculateMaxWeight(const Neuron& neuron, const Brain& brain) const {
    double max = std::numeric_limits<double>::lowest();
    bool found = false;
    
    for (const auto& synapse : brain.getSynapses()) {
        if (synapse.getPreNeuronId() == neuron.getId() || synapse.getPostNeuronId() == neuron.getId()) {
            if (synapse.getWeight() > max) {
                max = synapse.getWeight();
                found = true;
            }
        }
    }
    
    return found ? max : 0.0;
}

std::string DataExporter::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), pImpl->config.dateTimeFormat.c_str());
    return oss.str();
}

std::string DataExporter::exportFormatToString(ExportFormat format) {
    switch (format) {
        case ExportFormat::CSV: return "CSV";
        case ExportFormat::JSON: return "JSON";
        case ExportFormat::HDF5: return "HDF5";
        case ExportFormat::MATLAB: return "MATLAB";
        case ExportFormat::SQLITE: return "SQLite";
        case ExportFormat::XML: return "XML";
        case ExportFormat::BINARY: return "BINARY";
        case ExportFormat::PARQUET: return "PARQUET";
        case ExportFormat::TEXT: return "TEXT";
        case ExportFormat::YAML: return "YAML";
        case ExportFormat::TOML: return "TOML";
        default: return "CSV";
    }
}

std::string DataExporter::getExportFormatExtension(ExportFormat format) const {
    switch (format) {
        case ExportFormat::CSV: return ".csv";
        case ExportFormat::JSON: return ".json";
        case ExportFormat::HDF5: return ".h5";
        case ExportFormat::MATLAB: return ".mat";
        case ExportFormat::SQLITE: return ".db";
        case ExportFormat::XML: return ".xml";
        case ExportFormat::BINARY: return ".bin";
        case ExportFormat::PARQUET: return ".parquet";
        case ExportFormat::TEXT: return ".txt";
        case ExportFormat::YAML: return ".yaml";
        case ExportFormat::TOML: return ".toml";
        default: return "";
    }
}

std::string DataExporter::escapeJsonString(const std::string& str) const {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\\': oss << "\\\\"; break;
            case '\\': oss << "\\\\"; break;
            case '\\': oss << "\\\\"; break;
            case '\\': oss << "\\\\"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string DataExporter::escapeMatlabString(const std::string& str) const {
    // In real implementation, this would properly escape MATLAB strings
    // For now, just replace quotes and backslashes
    std::string result = str;
    std::replace(result.begin(), result.end(), '\\', '\\\\');
    std::replace(result.begin(), result.end(), '\"', "\\\"");
    return result;
}

std::string DataExporter::escapeXmlString(const std::string& str) const {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '&': oss << "&amp;"; break;
            case '<': oss << "&lt;"; break;
            case '>': oss << "&gt;"; break;
            case '\"': oss << "&quot;"; break;
            case '\'': oss << "&apos;"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string DataExporter::escapeTomlString(const std::string& str) const {
    // In real implementation, this would properly escape TOML strings
    // For now, just replace quotes and backslashes
    std::string result = str;
    std::replace(result.begin(), result.end(), '\\', "\\\\");
    std::replace(result.begin(), result.end(), '\"', "\\\"");
    if (result.find('\n') != std::string::npos) {
        result = "\"" + result + "\"";
    }
    return result;
}

std::string DataExporter::vectorToString(const std::vector<int64_t>& vec) const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        if (i < vec.size() - 1) {
            oss << ", ";
        }
    }
    oss << "]";
    return oss.str();
}

} // namespace nlm
