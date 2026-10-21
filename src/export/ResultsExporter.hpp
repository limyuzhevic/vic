#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include <chrono>
#include "core/Config/Config.hpp"

namespace nlm {

/**
 * Neural data structure for export
 */
struct NeuralData {
    // Time series data
    struct TimeSeries {
        double timestamp;
        float firingRate;
        float populationActivity;
        float memoryActivation;
        float neuromodulationLevel;
        float predictionError;
    };
    
    // Neuron data
    struct Neuron {
        uint32_t id;
        float membranePotential;
        float threshold;
        float firingRate;
        float lastSpikeTime;
        float adaptationVariable;
        std::vector<float> synapticWeights;
        std::vector<uint32_t> incomingSynapses;
        std::vector<uint32_t> outgoingSynapses;
        float totalCurrent;
        bool isFiring;
        bool isRefractory;
    };
    
    // Synapse data
    struct Synapse {
        uint32_t id;
        uint32_t preNeuronId;
        uint32_t postNeuronId;
        float weight;
        float delay;
        bool isPlastic;
        float lastPreSpikeTime;
        float lastPostSpikeTime;
        float stdpValue;
    };
    
    // Memory data
    struct MemoryEntry {
        uint32_t id;
        uint32_t neuronId;
        std::string content;
        float activationStrength;
        double timestamp;
        float confidence;
    };
    
    // Brain region data
    struct BrainRegion {
        uint32_t id;
        std::string name;
        uint32_t neuronCount;
        uint32_t synapseCount;
        float activityLevel;
        float averageFiringRate;
        float excitationInhibitionRatio;
    };
    
    // Experiment metrics
    struct ExperimentMetrics {
        double totalRuntime;
        size_t totalSteps;
        size_t totalSpikes;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        float averageFiringRate;
        float avgSynapticWeight;
        float excitationInhibitionRatio;
        float memoryUtilization;
        float learningRate;
        float curiosityLevel;
        float noveltyLevel;
        float dopamineLevel;
        size_t memoryEpisodesStored;
        double memoryReplayCount;
    };
};

/**
 * Results exporter interface
 */
class ResultsExporter {
public:
    virtual ~ResultsExporter() = default;
    
    virtual bool exportNeuralData(const NeuralData& data, const std::string& filepath) = 0;
    virtual bool exportTimeSeries(const std::vector<NeuralData::TimeSeries>& timeSeries, const std::string& filepath) = 0;
    virtual bool exportNeurons(const std::vector<NeuralData::Neuron>& neurons, const std::string& filepath) = 0;
    virtual bool exportSynapses(const std::vector<NeuralData::Synapse>& synapses, const std::string& filepath) = 0;
    virtual bool exportMemory(const std::vector<NeuralData::MemoryEntry>& memoryEntries, const std::string& filepath) = 0;
    virtual bool exportBrainRegions(const std::vector<NeuralData::BrainRegion>& regions, const std::string& filepath) = 0;
    virtual bool exportMetrics(const NeuralData::ExperimentMetrics& metrics, const std::string& filepath) = 0;
    
    virtual bool exportAll(const NeuralData& data, const std::string& basePath) = 0;
    virtual bool setCompression(bool enabled) = 0;
    virtual bool setFormat(const std::string& format) = 0;
    virtual std::string getDefaultExtension() const = 0;
};

/**
 * JSON exporter implementation
 */
class JSONResultsExporter : public ResultsExporter {
public:
    JSONResultsExporter();
    ~JSONResultsExporter() override;
    
    bool exportNeuralData(const NeuralData& data, const std::string& filepath) override;
    bool exportTimeSeries(const std::vector<NeuralData::TimeSeries>& timeSeries, const std::string& filepath) override;
    bool exportNeurons(const std::vector<NeuralData::Neuron>& neurons, const std::string& filepath) override;
    bool exportSynapses(const std::vector<NeuralData::Synapse>& synapses, const std::string& filepath) override;
    bool exportMemory(const std::vector<NeuralData::MemoryEntry>& memoryEntries, const std::string& filepath) override;
    bool exportBrainRegions(const std::vector<NeuralData::BrainRegion>& regions, const std::string& filepath) override;
    bool exportMetrics(const NeuralData::ExperimentMetrics& metrics, const std::string& filepath) override;
    
    bool exportAll(const NeuralData& data, const std::string& basePath) override;
    bool setCompression(bool enabled) override;
    bool setFormat(const std::string& format) override;
    std::string getDefaultExtension() const override { return ".json"; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * CSV exporter implementation
 */
class CSVResultsExporter : public ResultsExporter {
public:
    CSVResultsExporter();
    ~CSVResultsExporter() override;
    
    bool exportNeuralData(const NeuralData& data, const std::string& filepath) override;
    bool exportTimeSeries(const std::vector<NeuralData::TimeSeries>& timeSeries, const std::string& filepath) override;
    bool exportNeurons(const std::vector<NeuralData::Neuron>& neurons, const std::string& filepath) override;
    bool exportSynapses(const std::vector<NeuralData::Synapse>& synapses, const std::string& filepath) override;
    bool exportMemory(const std::vector<NeuralData::MemoryEntry>& memoryEntries, const std::string& filepath) override;
    bool exportBrainRegions(const std::vector<NeuralData::BrainRegion>& regions, const std::string& filepath) override;
    bool exportMetrics(const NeuralData::ExperimentMetrics& metrics, const std::string& filepath) override;
    
    bool exportAll(const NeuralData& data, const std::string& basePath) override;
    bool setCompression(bool enabled) override;
    bool setFormat(const std::string& format) override;
    std::string getDefaultExtension() const override { return ".csv"; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * MATLAB exporter implementation
 */
class MATLABResultsExporter : public ResultsExporter {
public:
    MATLABResultsExporter();
    ~MATLABResultsExporter() override;
    
    bool exportNeuralData(const NeuralData& data, const std::string& filepath) override;
    bool exportTimeSeries(const std::vector<NeuralData::TimeSeries>& timeSeries, const std::string& filepath) override;
    bool exportNeurons(const std::vector<NeuralData::Neuron>& neurons, const std::string& filepath) override;
    bool exportSynapses(const std::vector<NeuralData::Synapse>& synapses, const std::string& filepath) override;
    bool exportMemory(const std::vector<NeuralData::MemoryEntry>& memoryEntries, const std::string& filepath) override;
    bool exportBrainRegions(const std::vector<NeuralData::BrainRegion>& regions, const std::string& filepath) override;
    bool exportMetrics(const NeuralData::ExperimentMetrics& metrics, const std::string& filepath) override;
    
    bool exportAll(const NeuralData& data, const std::string& basePath) override;
    bool setCompression(bool enabled) override;
    bool setFormat(const std::string& format) override;
    std::string getDefaultExtension() const override { return ".mat"; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Exporter factory for creating export handlers
 */
class ResultsExporterFactory {
public:
    virtual ~ResultsExporterFactory() = default;
    virtual std::unique_ptr<ResultsExporter> createExporter(const std::string& format) = 0;
    virtual std::vector<std::string> getSupportedFormats() const = 0;
};

/**
 * Default results exporter factory
 */
class DefaultResultsExporterFactory : public ResultsExporterFactory {
public:
    DefaultResultsExporterFactory();
    ~DefaultResultsExporterFactory() override;
    
    std::unique_ptr<ResultsExporter> createExporter(const std::string& format) override;
    std::vector<std::string> getSupportedFormats() const override {
        return {"json", "csv", "matlab", "xml"};
    }
};

/**
 * Neural data collector for exporting brain state
 */
class NeuralDataCollector {
public:
    NeuralDataCollector();
    ~NeuralDataCollector() = default;
    
    void update(const std::string& key, const std::string& value);
    void update(const std::string& key, double value);
    void update(const std::string& key, int value);
    void update(const std::string& key, bool value);
    
    const std::unordered_map<std::string, std::string>& getStringData() const { return stringData_; }
    const std::unordered_map<std::string, double>& getNumericData() const { return numericData_; }
    const std::unordered_map<std::string, int>& getIntegerData() const { return integerData_; }
    const std::unordered_map<std::string, bool>& getBooleanData() const { return booleanData_; }
    
    NeuralData convertToNeuralData() const;
    
    void clear() { stringData_.clear(); numericData_.clear(); integerData_.clear(); booleanData_.clear(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
