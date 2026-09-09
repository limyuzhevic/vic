#pragma once

#include "Exporters.hpp"

namespace nlm {

class CSVExporter : public BaseExporter {
public:
    CSVExporter() = default;
    ~CSVExporter() override = default;
    
    bool exportData(const std::string& filepath) override;
    bool open(const std::string& filepath) override;
    void close() override;
    bool isOpen() const override;
    uint64_t getBytesWritten() const override;
    
    std::string getFormatName() const override;
    bool supportsFormat(const std::string& format) const override;
    
    // Helper functions for CSV export
    void writeMetadataHeader();
    void writeNeuronsCSV();
    void writeSynapsesCSV();
    void writeSpikeHistoryCSV();
    void writeMembranePotentialsCSV();
    void writeWeightsCSV();
    
private:
    std::string filepath_;
    std::ofstream file_;
    uint64_t bytesWritten_ = 0;
};

} // namespace nlm
