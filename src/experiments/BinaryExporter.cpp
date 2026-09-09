#pragma once

#include "Exporters.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <zlib.h>
#include <bzlib.h>
#include <lzma.h>

namespace nlm {

class BinaryExporter : public BaseExporter {
public:
    BinaryExporter() = default;
    ~BinaryExporter() override = default;
    
    bool exportData(const std::string& filepath) override {
        // Generate current timestamp
        auto now = std::chrono::system_clock::now();
        metadata.timestamp = now;
        
        // Start binary export
        if (!open(filepath)) {
            return false;
        }
        
        try {
            // Write header
            BinaryHeader header;
            std::string magic = "NLMBIN1"; // Binary exporter magic
            std::copy(magic.begin(), magic.end(), header.magic);
            header.version = 1;
            header.flags = 0;
            if (options.compress) {
                header.flags |= FLAG_COMPRESSED;
            }
            
            // Write metadata
            writeHeader(header);
            
            // Write export options
            writeOptions();
            
            // Write neural data based on options
            if (options.includeNeurons) {
                writeNeurons();
            }
            
            if (options.includeSynapses) {
                writeSynapses();
            }
            
            if (options.includeSpikeHistory) {
                writeSpikeHistory();
            }
            
            if (options.includeMembranePotentials) {
                writeMembranePotentials();
            }
            
            if (options.includeWeights) {
                writeWeights();
            }
            
            // Finalize
            if (options.compress) {
                compressFile();
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
        rawFilePath_ = filepath + ".raw";
        
        // Open raw file for writing
        rawFile_.open(rawFilePath_, std::ios::binary);
        if (!rawFile_.is_open()) {
            return false;
        }
        
        // Open compressed file if compression is enabled
        if (options.compress) {
            compFile_.open(filepath, std::ios::binary);
            if (!compFile_.is_open()) {
                rawFile_.close();
                return false;
            }
        }
        
        return true;
    }
    
    void close() override {
        if (rawFile_.is_open()) {
            rawFile_.close();
        }
        if (compFile_.is_open()) {
            compFile_.close();
        }
        
        std::remove(rawFilePath_.c_str());
        filepath_.clear();
        rawFilePath_.clear();
    }
    
    bool isOpen() const override {
        return rawFile_.is_open() || compFile_.is_open();
    }
    
    uint64_t getBytesWritten() const override {
        return bytesWritten_;
    }
    
    std::string getFormatName() const override {
        return "BINARY";
    }
    
    bool supportsFormat(const std::string& format) const override {
        return format == "BINARY" || format == "BINARY_COMPRESSED" || format == "BINARY_RAW";
    }
    
    // Helper functions for binary export
    struct BinaryHeader {
        char magic[8];
        uint32_t version;
        uint32_t flags;
        uint64_t timestamp;
        uint64_t metadataSize;
        uint64_t optionsSize;
        uint64_t neuronDataSize;
        uint64_t synapseDataSize;
        uint64_t spikeHistorySize;
        uint64_t membranePotentialSize;
        uint64_t weightsSize;
        uint64_t totalSize;
    };
    
    enum BinaryFlags {
        FLAG_COMPRESSED = 0x01,
        FLAG_CHECKSUM = 0x02,
        FLAG_METADATA = 0x04,
        FLAG_OPTIONS = 0x08
    };
    
    void writeHeader(const BinaryHeader& header) {
        rawFile_.write(reinterpret_cast<const char*>(&header), sizeof(BinaryHeader));
        bytesWritten_ += sizeof(BinaryHeader);
    }
    
    void writeOptions() {
        // Write export options as binary
        json optionsJson = {
            {"includeNeurons", options.includeNeurons},
            {"includeSynapses", options.includeSynapses},
            {"includeSpikeHistory", options.includeSpikeHistory},
            {"includeMembranePotentials", options.includeMembranePotentials},
            {"includeWeights", options.includeWeights},
            {"includeDelays", options.includeDelays},
            {"includePlasticity", options.includePlasticity},
            {"includeMemory", options.includeMemory},
            {"includeNeuromodulation", options.includeNeuromodulation},
            {"includeDevelopment", options.includeDevelopment},
            {"includePrediction", options.includePrediction},
            {"includeCognition", options.includeCognition},
            {"format", options.format},
            {"compress", options.compress},
            {"timestamp", options.timestamp}
        };
        
        std::string optionsStr = optionsJson.dump();
        uint64_t optionsSize = optionsStr.size();
        rawFile_.write(reinterpret_cast<const char*>(&optionsSize), sizeof(uint64_t));
        rawFile_.write(optionsStr.c_str(), optionsSize);
        bytesWritten_ += sizeof(uint64_t) + optionsSize;
    }
    
    void writeNeurons() {
        // Write neuron data as binary
        // TODO: Populate with actual neuron data from Brain
        uint64_t neuronDataSize = 0;
        rawFile_.write(reinterpret_cast<const char*>(&neuronDataSize), sizeof(uint64_t));
        bytesWritten_ += sizeof(uint64_t);
    }
    
    void writeSynapses() {
        // Write synapse data as binary
        uint64_t synapseDataSize = 0;
        rawFile_.write(reinterpret_cast<const char*>(&synapseDataSize), sizeof(uint64_t));
        bytesWritten_ += sizeof(uint64_t);
    }
    
    void writeSpikeHistory() {
        // Write spike history as binary
        uint64_t spikeHistorySize = 0;
        rawFile_.write(reinterpret_cast<const char*>(&spikeHistorySize), sizeof(uint64_t));
        bytesWritten_ += sizeof(uint64_t);
    }
    
    void writeMembranePotentials() {
        // Write membrane potentials as binary
        uint64_t membranePotentialSize = 0;
        rawFile_.write(reinterpret_cast<const char*>(&membranePotentialSize), sizeof(uint64_t));
        bytesWritten_ += sizeof(uint64_t);
    }
    
    void writeWeights() {
        // Write synapse weights as binary
        uint64_t weightsSize = 0;
        rawFile_.write(reinterpret_cast<const char*>(&weightsSize), sizeof(uint64_t));
        bytesWritten_ += sizeof(uint64_t);
    }
    
    void compressFile() {
        // Compress the raw file
        std::ifstream rawIn(rawFilePath_, std::ios::binary);
        if (!rawIn.is_open()) {
            return;
        }
        
        compFile_.write("BZIP2", 5); // Simple compression marker
        
        // For simplicity, just copy raw data to compressed file
        // In a real implementation, would use actual compression
        rawIn.seekg(0, std::ios::end);
        size_t size = rawIn.tellg();
        rawIn.seekg(0, std::ios::beg);
        
        char* buffer = new char[size];
        rawIn.read(buffer, size);
        compFile_.write(buffer, size);
        delete[] buffer;
        
        rawIn.close();
        std::remove(rawFilePath_.c_str());
    }
    
private:
    std::string filepath_;
    std::string rawFilePath_;
    std::ofstream rawFile_;
    std::ofstream compFile_;
    uint64_t bytesWritten_ = 0;
};

} // namespace nlm
