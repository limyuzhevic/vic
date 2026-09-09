#pragma once

#include "Exporters.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <nlohmann/json.hpp>

namespace nlm {

class JSONExporter : public BaseExporter {
public:
    JSONExporter() = default;
    ~JSONExporter() override = default;
    
    bool exportData(const std::string& filepath) override {
        // Generate current timestamp
        auto now = std::chrono::system_clock::now();
        metadata.timestamp = now;
        
        // Start JSON export
        if (!open(filepath)) {
            return false;
        }
        
        try {
            json exportJson;
            
            // Add metadata
            exportJson["metadata"] = {
                {"version", metadata.version},
                {"description", metadata.description},
                {"author", metadata.author},
                {"timestamp", std::chrono::system_clock::to_time_t(metadata.timestamp)},
                {"simulationId", metadata.simulationId},
                {"experimentName", metadata.experimentName},
                {"stepsExported", metadata.stepsExported},
                {"simulationTime", metadata.simulationTime}
            };
            
            // Add export options
            json optionsJson;
            optionsJson["format"] = options.format;
            optionsJson["compress"] = options.compress;
            optionsJson["timestamp"] = options.timestamp;
            
            json includeMap;
            includeMap["neurons"] = options.includeNeurons;
            includeMap["synapses"] = options.includeSynapses;
            includeMap["spikeHistory"] = options.includeSpikeHistory;
            includeMap["membranePotentials"] = options.includeMembranePotentials;
            includeMap["weights"] = options.includeWeights;
            includeMap["delays"] = options.includeDelays;
            includeMap["plasticity"] = options.includePlasticity;
            includeMap["memory"] = options.includeMemory;
            includeMap["neuromodulation"] = options.includeNeuromodulation;
            includeMap["development"] = options.includeDevelopment;
            includeMap["prediction"] = options.includePrediction;
            includeMap["cognition"] = options.includeCognition;
            optionsJson["include"] = includeMap;
            exportJson["exportOptions"] = optionsJson;
            
            // Add neural data based on options
            if (options.includeNeurons) {
                exportJson["neurons"] = exportNeurons();
            }
            
            if (options.includeSynapses) {
                exportJson["synapses"] = exportSynapses();
            }
            
            if (options.includeSpikeHistory) {
                exportJson["spikeHistory"] = exportSpikeHistory();
            }
            
            if (options.includeMembranePotentials) {
                exportJson["membranePotentials"] = exportMembranePotentials();
            }
            
            if (options.includeWeights) {
                exportJson["synapseWeights"] = exportSynapseWeights();
            }
            
            // Export to file
            std::ofstream file(filepath);
            if (!file.is_open()) {
                close();
                return false;
            }
            
            file << exportJson.dump(2);
            file.close();
            
            bytesWritten_ = file.tellp();
            
            close();
            return true;
            
        } catch (const std::exception& e) {
            close();
            return false;
        }
    }
    
    bool open(const std::string& filepath) override {
        close();
        filepath_ = filepath;
        return true;
    }
    
    void close() override {
        filepath_.clear();
    }
    
    bool isOpen() const override {
        return !filepath_.empty();
    }
    
    uint64_t getBytesWritten() const override {
        return bytesWritten_;
    }
    
    std::string getFormatName() const override {
        return "JSON";
    }
    
    bool supportsFormat(const std::string& format) const override {
        return format == "JSON" || format == "JSONL" || format == "JSONCOMPRESSED";
    }
    
    // Helper functions for JSON export
    json exportNeurons() const {
        json neurons;
        // TODO: Populate with actual neuron data from Brain
        // For now, return empty array
        return neurons;
    }
    
    json exportSynapses() const {
        json synapses;
        // TODO: Populate with actual synapse data from Brain
        // For now, return empty array
        return synapses;
    }
    
    json exportSpikeHistory() const {
        json spikeHistory;
        // TODO: Populate with actual spike history data from Brain
        // For now, return empty array
        return spikeHistory;
    }
    
    json exportMembranePotentials() const {
        json potentials;
        // TODO: Populate with actual membrane potential data from Brain
        // For now, return empty array
        return potentials;
    }
    
    json exportSynapseWeights() const {
        json weights;
        // TODO: Populate with actual synapse weight data from Brain
        // For now, return empty array
        return weights;
    }
    
private:
    std::string filepath_;
    uint64_t bytesWritten_ = 0;
};

} // namespace nlm
