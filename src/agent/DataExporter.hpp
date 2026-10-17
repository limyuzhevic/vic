#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Export format enumeration
enum class ExportFormat {
    JSON,
    CSV,
    TSV,
    HDF5,
    PNG,
    PDF,
    Text,
    YAML
};

// Export options
struct ExportOptions {
    ExportFormat format;
    std::string filepath;
    std::map<std::string, std::string> parameters;
    bool includeMetadata;
    bool compressOutput;
    double compressionLevel;
    
    ExportOptions() : format(ExportFormat::JSON), filepath(""), 
                      includeMetadata(true), compressOutput(false), 
                      compressionLevel(0.5) {}
    
    ExportOptions(ExportFormat fmt, const std::string& path) 
        : format(fmt), filepath(path), includeMetadata(true), 
          compressOutput(false), compressionLevel(0.5) {}
};

// Data exporter interface
class DataExporter {
public:
    virtual ~DataExporter() = default;
    
    // Export brain state
    virtual bool exportBrainState(std::shared_ptr<Brain> brain, 
                                  const ExportOptions& options) = 0;
    
    // Export simulation results
    virtual bool exportSimulationResults(const std::string& name,
                                         const std::vector<std::map<std::string, double>>& results,
                                         const ExportOptions& options) = 0;
    
    // Export configuration
    virtual bool exportConfiguration(std::shared_ptr<const Config> config,
                                     const ExportOptions& options) = 0;
    
    // Get supported formats
    virtual std::vector<ExportFormat> getSupportedFormats() const = 0;
    
    // Check if format is supported
    virtual bool supportsFormat(ExportFormat format) const = 0;
};

// JSON exporter implementation
class JSONExporter : public DataExporter {
public:
    bool exportBrainState(std::shared_ptr<Brain> brain, 
                         const ExportOptions& options) override {
        if (options.format != ExportFormat::JSON) return false;
        
        nlohmann::json exportData;
        
        // Basic brain statistics
        exportData["metadata"]["format"] = "nlm_brain_state";
        exportData["metadata"]["version"] = "2.0";
        exportData["metadata"]["timestamp"] = getCurrentTimestamp();
        
        // Brain information
        exportData["brain"]["neuron_count"] = brain->getTotalNeuronCount();
        exportData["brain"]["synapse_count"] = brain->getTotalSynapseCount();
        exportData["brain"]["firing_neuron_count"] = brain->getFiringNeuronCount();
        exportData["brain"]["total_spikes"] = brain->getTotalSpikeCount();
        exportData["brain"]["average_firing_rate"] = brain->getAverageFiringRate();
        exportData["brain"]["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        
        // Regional information
        auto regions = brain->getRegions();
        nlohmann::json regionsJson = nlohmann::json::array();
        for (size_t i = 0; i < regions.size(); ++i) {
            auto region = regions[i].get();
            nlohmann::json regionJson;
            regionJson["id"] = i;
            regionJson["neuron_count"] = region->getNeuronCount();
            regionJson["synapse_count"] = region->getSynapseCount();
            
            // Neuron information
            nlohmann::json neuronsJson = nlohmann::json::array();
            auto neurons = region->getAllNeurons();
            for (size_t j = 0; j < neurons.size(); ++j++) {
                auto neuron = neurons[j];
                nlohmann::json neuronJson;
                neuronJson["id"] = j;
                neuronJson["type"] = static_cast<int>(neuron->getNeuronType());
                neuronJson["membrane_potential"] = neuron->getMembranePotential();
                neuronJson["threshold"] = neuron->getThreshold();
                neuronJson["resting_potential"] = neuron->getState().restingPotential;
                neuronsJson.push_back(neuronJson);
            }
            regionJson["neurons"] = neuronsJson;
            
            regionsJson.push_back(regionJson);
        }
        exportData["brain"]["regions"] = regionsJson;
        
        // Connection information
        nlohmann::json connectionsJson = nlohmann::json::array();
        for (const auto& region : regions) {
            auto synapses = region->getSynapses();
            for (const auto& synapse : synapses) {
                nlohmann::json connectionJson;
                connectionJson["source"] = synapse->getSourceNeuronId();
                connectionJson["target"] = synapse->getTargetNeuronId();
                connectionJson["weight"] = synapse->getWeight();
                connectionJson["delay"] = synapse->getDelay();
                connectionJson["plasticity_enabled"] = synapse->isPlasticityEnabled();
                connectionsJson.push_back(connectionJson);
            }
        }
        exportData["brain"]["connections"] = connectionsJson;
        
        // Save to file
        return saveJSON(exportData, options.filepath);
    }
    
    bool exportSimulationResults(const std::string& name,
                                 const std::vector<std::map<std::string, double>>& results,
                                 const ExportOptions& options) override {
        if (options.format != ExportFormat::JSON) return false;
        
        nlohmann::json exportData;
        exportData["metadata"]["name"] = name;
        exportData["metadata"]["version"] = "2.0";
        exportData["metadata"]["timestamp"] = getCurrentTimestamp();
        exportData["metadata"]["num_steps"] = results.size();
        
        // Simulation results
        nlohmann::json stepsJson = nlohmann::json::array();
        for (size_t i = 0; i < results.size(); ++i) {
            nlohmann::json stepJson;
            stepJson["step"] = i;
            stepJson["data"] = results[i];
            stepsJson.push_back(stepJson);
        }
        exportData["results"]["steps"] = stepsJson;
        
        return saveJSON(exportData, options.filepath);
    }
    
    bool exportConfiguration(std::shared_ptr<const Config> config,
                             const ExportOptions& options) override {
        if (options.format != ExportFormat::JSON) return false;
        
        nlohmann::json exportData;
        exportData["metadata"]["format"] = "nlm_configuration";
        exportData["metadata"]["version"] = "2.0";
        exportData["metadata"]["timestamp"] = getCurrentTimestamp();
        
        // Configuration entries
        auto entries = config->getAllEntries();
        nlohmann::json configJson = nlohmann::json::array();
        for (const auto& entry : entries) {
            nlohmann::json entryJson;
            entryJson["key"] = entry.key;
            entryJson["value"] = entry.value; // This will work with nlohmann::json
            entryJson["source"] = static_cast<int>(entry.source);
            entryJson["description"] = entry.description;
            configJson.push_back(entryJson);
        }
        exportData["configuration"] = configJson;
        
        return saveJSON(exportData, options.filepath);
    }
    
    std::vector<ExportFormat> getSupportedFormats() const override {
        return {ExportFormat::JSON, ExportFormat::CSV, ExportFormat::TSV, ExportFormat::Text};
    }
    
    bool supportsFormat(ExportFormat format) const override {
        return format == ExportFormat::JSON || format == ExportFormat::CSV || 
               format == ExportFormat::TSV || format == ExportFormat::Text;
    }

private:
    bool saveJSON(const nlohmann::json& data, const std::string& filepath) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            if (true) { // Compression not implemented
                file << data.dump(2);
            } else {
                file << data.dump(2);
            }
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error saving JSON file: " << e.what() << std::endl;
            return false;
        }
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// CSV exporter implementation
class CSVExporter : public DataExporter {
public:
    bool exportBrainState(std::shared_ptr<Brain> brain, 
                         const ExportOptions& options) override {
        if (options.format != ExportFormat::CSV) return false;
        
        std::ofstream file(options.filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write header
        file << "Field,Value,Description" << std::endl;
        
        // Write basic statistics
        file << "neuron_count," << brain->getTotalNeuronCount() << "," << "Total neurons" << std::endl;
        file << "synapse_count," << brain->getTotalSynapseCount() << "," << "Total synapses" << std::endl;
        file << "firing_neuron_count," << brain->getFiringNeuronCount() << "," << "Currently firing neurons" << std::endl;
        file << "total_spikes," << brain->getTotalSpikeCount() << "," << "Total spike events" << std::endl;
        file << "average_firing_rate," << brain->getAverageFiringRate() << "," << "Average firing rate (Hz)" << std::endl;
        file << "excitation_inhibition_ratio," << brain->getExcitationInhibitionRatio() << "," << "E/I balance" << std::endl;
        
        // Write regional data
        file << std::endl << "Region,Neuron_Count,Synapse_Count" << std::endl;
        auto regions = brain->getRegions();
        for (size_t i = 0; i < regions.size(); ++i) {
            auto region = regions[i].get();
            file << "Region_" << i << "," 
                 << region->getNeuronCount() << "," 
                 << region->getSynapseCount() << std::endl;
        }
        
        return true;
    }
    
    bool exportSimulationResults(const std::string& name,
                                 const std::vector<std::map<std::string, double>>& results,
                                 const ExportOptions& options) override {
        if (options.format != ExportFormat::CSV) return false;
        
        std::ofstream file(options.filepath);
        if (!file.is_open()) {
            return false;
        }
        
        if (!results.empty()) {
            // Write header
            const auto& firstRow = results[0];
            file << "Step,";
            for (const auto& [key, value] : firstRow) {
                file << key << ",";
            }
            file << std::endl;
            
            // Write data
            for (size_t i = 0; i < results.size(); ++i) {
                file << i << ",";
                for (const auto& [key, value] : results[i]) {
                    file << value << ",";
                }
                file << std::endl;
            }
        }
        
        return true;
    }
    
    bool exportConfiguration(std::shared_ptr<const Config> config,
                             const ExportOptions& options) override {
        if (options.format != ExportFormat::CSV) return false;
        
        std::ofstream file(options.filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write header
        file << "Key,Value,Type,Source,Description" << std::endl;
        
        // Write configuration entries
        auto entries = config->getAllEntries();
        for (const auto& entry : entries) {
            std::string valueStr = std::visit([](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    return "\"" + arg + "\"";
                } else {
                    return std::to_string(arg);
                }
            }, entry.value);
            
            file << entry.key << ",";
            file << valueStr << ",";
            file << getTypeName(entry.value) << ",";
            file << static_cast<int>(entry.source) << ",";
            file << entry.description << std::endl;
        }
        
        return true;
    }
    
    std::vector<ExportFormat> getSupportedFormats() const override {
        return {ExportFormat::CSV, ExportFormat::TSV};
    }
    
    bool supportsFormat(ExportFormat format) const override {
        return format == ExportFormat::CSV || format == ExportFormat::TSV;
    }

private:
    std::string getTypeName(const ConfigValue& value) const {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return "string";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return "int64";
            } else if constexpr (std::is_same_v<T, double>) {
                return "double";
            } else if constexpr (std::is_same_v<T, bool>) {
                return "bool";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return "vector<int>";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return "vector<double>";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                return "vector<string>";
            }
        }, value);
        return "unknown";
    }
};

// TSV exporter implementation (similar to CSV but with tab separators)
class TSVExporter : public CSVExporter {
public:
    std::vector<ExportFormat> getSupportedFormats() const override {
        return {ExportFormat::TSV};
    }
    
    bool supportsFormat(ExportFormat format) const override {
        return format == ExportFormat::TSV;
    }
    
protected:
    bool exportBrainState(std::shared_ptr<Brain> brain, 
                         const ExportOptions& options) override {
        return CSVExporter::exportBrainState(brain, options);
    }
    
    bool exportSimulationResults(const std::string& name,
                                 const std::vector<std::map<std::string, double>>& results,
                                 const ExportOptions& options) override {
        return CSVExporter::exportSimulationResults(name, results, options);
    }
    
    bool exportConfiguration(std::shared_ptr<const Config> config,
                             const ExportOptions& options) override {
        return CSVExporter::exportConfiguration(config, options);
    }
};

// Data visualizer interface
class DataVisualizer {
public:
    virtual ~DataVisualizer() = default;
    
    // Create neuron visualization
    virtual bool createNeuronVisualization(std::shared_ptr<Brain> brain,
                                           const std::string& outputPath,
                                           const std::map<std::string, std::string>& options) = 0;
    
    // Create spike raster plot
    virtual bool createSpikeRasterPlot(std::shared_ptr<Brain> brain,
                                       const std::string& outputPath,
                                       const std::map<std::string, std::string>& options) = 0;
    
    // Create connectivity matrix
    virtual bool createConnectivityMatrix(std::shared_ptr<Brain> brain,
                                          const std::string& outputPath,
                                          const std::map<std::string, std::string>& options) = 0;
    
    // Create firing rate distribution
    virtual bool createFiringRateDistribution(std::shared_ptr<Brain> brain,
                                             const std::string& outputPath,
                                             const std::map<std::string, std::string>& options) = 0;
    
    // Create time series plot
    virtual bool createTimeSeriesPlot(const std::string& name,
                                     const std::vector<std::map<std::string, double>>& data,
                                     const std::string& outputPath,
                                     const std::map<std::string, std::string>& options) = 0;
};

// Text visualizer implementation
class TextVisualizer : public DataVisualizer {
public:
    bool createNeuronVisualization(std::shared_ptr<Brain> brain,
                                   const std::string& outputPath,
                                   const std::map<std::string, std::string>& options) override {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Neuron Visualization" << std::endl;
        file << "=======================" << std::endl;
        file << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        file << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        
        // Regional information
        file << std::endl << "Regional Breakdown:" << std::endl;
        auto regions = brain->getRegions();
        for (size_t i = 0; i < regions.size(); ++i) {
            auto region = regions[i].get();
            file << "Region " << i << ": " << region->getNeuronCount() 
                 << " neurons, " << region->getSynapseCount() 
                 << " synapses" << std::endl;
        }
        
        // Firing statistics
        file << std::endl << "Firing Statistics:" << std::endl;
        file << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        file << "Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        return true;
    }
    
    bool createSpikeRasterPlot(std::shared_ptr<Brain> brain,
                               const std::string& outputPath,
                               const std::map<std::string, std::string>& options) override {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Spike Raster Plot" << std::endl;
        file << "===================" << std::endl;
        file << "This is a placeholder for spike raster visualization." << std::endl;
        file << "Total spikes recorded: " << brain->getTotalSpikeCount() << std::endl;
        file << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        
        return true;
    }
    
    bool createConnectivityMatrix(std::shared_ptr<Brain> brain,
                                  const std::string& outputPath,
                                  const std::map<std::string, std::string>& options) override {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Connectivity Matrix" << std::endl;
        file << "=====================" << std::endl;
        file << "This is a placeholder for connectivity matrix visualization." << std::endl;
        file << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "Connection density: " << 
            static_cast<double>(brain->getTotalSynapseCount()) / 
            (brain->getTotalNeuronCount() * brain->getTotalNeuronCount()) << std::endl;
        
        return true;
    }
    
    bool createFiringRateDistribution(std::shared_ptr<Brain> brain,
                                      const std::string& outputPath,
                                      const std::map<std::string, std::string>& options) override {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Firing Rate Distribution" << std::endl;
        file << "=========================" << std::endl;
        file << "This is a placeholder for firing rate distribution visualization." << std::endl;
        file << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        file << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        
        return true;
    }
    
    bool createTimeSeriesPlot(const std::string& name,
                             const std::vector<std::map<std::string, double>>& data,
                             const std::string& outputPath,
                             const std::map<std::string, std::string>& options) override {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Time Series Plot: " << name << std::endl;
        file << "===================" << std::endl;
        file << "This is a placeholder for time series visualization." << std::endl;
        file << "Number of data points: " << data.size() << std::endl;
        
        return true;
    }
};

// Data exporter manager
class DataExporterManager {
public:
    DataExporterManager() {
        // Register exporters
        exporters_[ExportFormat::JSON] = std::make_unique<JSONExporter>();
        exporters_[ExportFormat::CSV] = std::make_unique<CSVExporter>();
        exporters_[ExportFormat::TSV] = std::make_unique<TSVExporter>();
        
        // Register visualizers
        visualizers_["text"] = std::make_unique<TextVisualizer>();
    }
    
    // Export data using specified format
    bool exportData(std::shared_ptr<Brain> brain,
                    ExportFormat format,
                    const std::string& filepath,
                    const std::map<std::string, std::string>& options = {}) {
        auto it = exporters_.find(format);
        if (it == exporters_.end()) {
            return false;
        }
        
        ExportOptions exportOptions;
        exportOptions.format = format;
        exportOptions.filepath = filepath;
        exportOptions.parameters = options;
        
        return it->second->exportBrainState(brain, exportOptions);
    }
    
    // Export simulation results
    bool exportSimulationResults(const std::string& name,
                                 const std::vector<std::map<std::string, double>>& results,
                                 ExportFormat format,
                                 const std::string& filepath,
                                 const std::map<std::string, std::string>& options = {}) {
        auto it = exporters_.find(format);
        if (it == exporters_.end()) {
            return false;
        }
        
        ExportOptions exportOptions;
        exportOptions.format = format;
        exportOptions.filepath = filepath;
        exportOptions.parameters = options;
        
        return it->second->exportSimulationResults(name, results, exportOptions);
    }
    
    // Create visualization
    bool createVisualization(std::shared_ptr<Brain> brain,
                              const std::string& type,
                              const std::string& outputPath,
                              const std::map<std::string, std::string>& options = {}) {
        auto it = visualizers_.find(type);
        if (it == visualizers_.end()) {
            return false;
        }
        
        return it->second->createNeuronVisualization(brain, outputPath, options);
    }
    
    // Get supported formats
    std::vector<ExportFormat> getSupportedFormats(ExportFormat format) const {
        std::vector<ExportFormat> allFormats;
        for (const auto& pair : exporters_) {
            allFormats.push_back(pair.first);
        }
        return allFormats;
    }
    
    // Get available visualizers
    std::vector<std::string> getAvailableVisualizers() const {
        std::vector<std::string> visualizers;
        for (const auto& pair : visualizers_) {
            visualizers.push_back(pair.first);
        }
        return visualizers;
    }

private:
    std::map<ExportFormat, std::unique_ptr<DataExporter>> exporters_;
    std::map<std::string, std::unique_ptr<DataVisualizer>> visualizers_;
};

} // namespace nlm