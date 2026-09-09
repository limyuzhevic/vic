#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <map>

namespace nlm {

// Export data structures
struct ExportMetadata {
    std::string version;
    std::string description;
    std::string author;
    std::chrono::system_clock::time_point timestamp;
    std::string simulationId;
    std::string experimentName;
    uint64_t stepsExported;
    double simulationTime;
    
    ExportMetadata()
        : version("1.0"), stepsExported(0), simulationTime(0.0) {}
};

struct ExportOptions {
    bool includeNeurons;
    bool includeSynapses;
    bool includeSpikeHistory;
    bool includeMembranePotentials;
    bool includeWeights;
    bool includeDelays;
    bool includePlasticity;
    bool includeMemory;
    bool includeNeuromodulation;
    bool includeDevelopment;
    bool includePrediction;
    bool includeCognition;
    std::string format;
    bool compress;
    double timestamp;
    
    ExportOptions()
        : includeNeurons(true)
        , includeSynapses(true)
        , includeSpikeHistory(false)
        , includeMembranePotentials(true)
        , includeWeights(true)
        , includeDelays(false)
        , includePlasticity(false)
        , includeMemory(false)
        , includeNeuromodulation(false)
        , includeDevelopment(false)
        , includePrediction(false)
        , includeCognition(false)
        , format("CSV")
        , compress(false)
        , timestamp(0.0) {}
};

// Base exporter interface
class BaseExporter {
public:
    virtual ~BaseExporter() = default;
    
    virtual bool exportData(const std::string& filepath) = 0;
    virtual bool open(const std::string& filepath) = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
    virtual uint64_t getBytesWritten() const = 0;
    
    // Metadata and options
    ExportMetadata getMetadata() const { return metadata; }
    void setMetadata(const ExportMetadata& md) { metadata = md; }
    
    ExportOptions getOptions() const { return options; }
    void setOptions(const ExportOptions& opt) { options = opt; }
    
    // Format support
    virtual std::string getFormatName() const = 0;
    virtual bool supportsFormat(const std::string& format) const = 0;
    
protected:
    ExportMetadata metadata;
    ExportOptions options;
};

// Export result structure
struct ExportResult {
    bool success;
    std::string errorMessage;
    uint64_t bytesWritten;
    double exportTime;
    
    ExportResult()
        : success(false), bytesWritten(0), exportTime(0.0) {}
    
    static ExportResult successResult(uint64_t bytes, double time) {
        ExportResult result;
        result.success = true;
        result.bytesWritten = bytes;
        result.exportTime = time;
        return result;
    }
    
    static ExportResult errorResult(const std::string& error) {
        ExportResult result;
        result.success = false;
        result.errorMessage = error;
        return result;
    }
};

} // namespace nlm
