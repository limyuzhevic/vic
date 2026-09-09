#pragma once

#include "Exporters.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

class CSVExporter : public BaseExporter {
public:
    CSVExporter() = default;
    ~CSVExporter() override = default;
    
    bool exportData(const std::string& filepath) override {
        // Generate current timestamp
        auto now = std::chrono::system_clock::now();
        metadata.timestamp = now;
        
        // Start CSV export
        if (!open(filepath)) {
            return false;
        }
        
        try {
            // Write metadata header
            writeMetadataHeader();
            
            // Write neural data based on options
            if (options.includeNeurons) {
                writeNeuronsCSV();
            }
            
            if (options.includeSynapses) {
                writeSynapsesCSV();
            }
            
            if (options.includeSpikeHistory) {
                writeSpikeHistoryCSV();
            }
            
            if (options.includeMembranePotentials) {
                writeMembranePotentialsCSV();
            }
            
            if (options.includeWeights) {
                writeWeightsCSV();
            }
            
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
        file_.open(filepath);
        return file_.is_open();
    }
    
    void close() override {
        if (file_.is_open()) {
            file_.close();
        }
        filepath_.clear();
    }
    
    bool isOpen() const override {
        return file_.is_open();
    }
    
    uint64_t getBytesWritten() const override {
        return bytesWritten_;
    }
    
    std::string getFormatName() const override {
        return "CSV";
    }
    
    bool supportsFormat(const std::string& format) const override {
        return format == "CSV" || format == "TSV" || format == "CSV_COMPRESSED";
    }
    
    // Helper functions for CSV export
    void writeMetadataHeader() {
        file_ << "# NLM Neural Data Export" << std::endl;
        file_ << "# Version: " << metadata.version << std::endl;
        file_ << "# Description: " << metadata.description << std::endl;
        file_ << "# Author: " << metadata.author << std::endl;
        file_ << "# Timestamp: " << std::chrono::system_clock::to_time_t(metadata.timestamp) << std::endl;
        file_ << "# Simulation ID: " << metadata.simulationId << std::endl;
        file_ << "# Experiment: " << metadata.experimentName << std::endl;
        file_ << "# Steps Exported: " << metadata.stepsExported << std::endl;
        file_ << "# Simulation Time: " << metadata.simulationTime << std::endl;
        file_ << std::endl;
        
        // Write options as comments
        file_ << "# Export Options:" << std::endl;
        file_ << "# Neurons: " << (options.includeNeurons ? "YES" : "NO") << std::endl;
        file_ << "# Synapses: " << (options.includeSynapses ? "YES" : "NO") << std::endl;
        file_ << "# Spike History: " << (options.includeSpikeHistory ? "YES" : "NO") << std::endl;
        file_ << "# Membrane Potentials: " << (options.includeMembranePotentials ? "YES" : "NO") << std::endl;
        file_ << "# Weights: " << (options.includeWeights ? "YES" : "NO") << std::endl;
        file_ << std::endl;
    }
    
    void writeNeuronsCSV() {
        file_ << "Neuron Data" << std::endl;
        file_ << "neuron_id,region_id,population_id,type,membrane_potential,threshold,resting_potential,reset_potential,leak_conductance,firing_state,refractory_remaining,refractory_period,last_spike_time,plasticity_flags" << std::endl;
        
        // TODO: Populate with actual neuron data from Brain
        // For now, write empty data
        file_ << "No neuron data available in this export" << std::endl;
        
        file_ << std::endl;
        bytesWritten_ += 100; // Estimate
    }
    
    void writeSynapsesCSV() {
        file_ << "Synapse Data" << std::endl;
        file_ << "synapse_id,source_neuron,destination_neuron,weight,delay,type,plasticity_flags,eligibility_trace,efficacy" << std::endl;
        
        // TODO: Populate with actual synapse data from Brain
        // For now, write empty data
        file_ << "No synapse data available in this export" << std::endl;
        
        file_ << std::endl;
        bytesWritten_ += 100; // Estimate
    }
    
    void writeSpikeHistoryCSV() {
        file_ << "Spike History" << std::endl;
        file_ << "neuron_id,timestamp,step" << std::endl;
        
        // TODO: Populate with actual spike history from Brain
        // For now, write empty data
        file_ << "No spike history data available in this export" << std::endl;
        
        file_ << std::endl;
        bytesWritten_ += 100; // Estimate
    }
    
    void writeMembranePotentialsCSV() {
        file_ << "Membrane Potential Time Series" << std::endl;
        file_ << "neuron_id,step,membrane_potential,timestamp" << std::endl;
        
        // TODO: Populate with actual membrane potential data from Brain
        // For now, write empty data
        file_ << "No membrane potential data available in this export" << std::endl;
        
        file_ << std::endl;
        bytesWritten_ += 100; // Estimate
    }
    
    void writeWeightsCSV() {
        file_ << "Synapse Weights Time Series" << std::endl;
        file_ << "synapse_id,source_neuron,destination_neuron,weight,timestamp,step" << std::endl;
        
        // TODO: Populate with actual weight data from Brain
        // For now, write empty data
        file_ << "No synapse weight data available in this export" << std::endl;
        
        file_ << std::endl;
        bytesWritten_ += 100; // Estimate
    }
    
private:
    std::string filepath_;
    std::ofstream file_;
    uint64_t bytesWritten_ = 0;
};

} // namespace nlm
