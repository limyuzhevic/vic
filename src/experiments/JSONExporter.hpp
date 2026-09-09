#pragma once

#include "Exporters.hpp"

namespace nlm {

class JSONExporter : public BaseExporter {
public:
    JSONExporter() = default;
    ~JSONExporter() override = default;
    
    bool exportData(const std::string& filepath) override;
    bool open(const std::string& filepath) override;
    void close() override;
    bool isOpen() const override;
    uint64_t getBytesWritten() const override;
    
    std::string getFormatName() const override;
    bool supportsFormat(const std::string& format) const override;
    
    // Helper functions for JSON export
    json exportNeurons() const;
    json exportSynapses() const;
    json exportSpikeHistory() const;
    json exportMembranePotentials() const;
    json exportSynapseWeights() const;
    
private:
    std::string filepath_;
    uint64_t bytesWritten_ = 0;
};

} // namespace nlm
