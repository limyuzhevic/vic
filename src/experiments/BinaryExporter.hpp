#pragma once

#include "Exporters.hpp"
#include <zlib.h>
#include <bzlib.h>
#include <lzma.h>

namespace nlm {

class BinaryExporter : public BaseExporter {
public:
    BinaryExporter() = default;
    ~BinaryExporter() override = default;
    
    bool exportData(const std::string& filepath) override;
    bool open(const std::string& filepath) override;
    void close() override;
    bool isOpen() const override;
    uint64_t getBytesWritten() const override;
    
    std::string getFormatName() const override;
    bool supportsFormat(const std::string& format) const override;
    
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
    
    void writeHeader(const BinaryHeader& header);
    void writeOptions();
    void writeNeurons();
    void writeSynapses();
    void writeSpikeHistory();
    void writeMembranePotentials();
    void writeWeights();
    void compressFile();
    
private:
    std::string filepath_;
    std::string rawFilePath_;
    std::ofstream rawFile_;
    std::ofstream compFile_;
    uint64_t bytesWritten_ = 0;
};

} // namespace nlm
