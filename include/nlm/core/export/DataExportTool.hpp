// Copyright (c) 2026 NLM System
// Data Export Tool for NLM - Multi-format data export functionality

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <optional>
#include <variant>

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Brain.hpp"
#include "../memory/Memory.hpp"
#include "../experiments/Metrics.hpp"
#include "../motor/Action.hpp"
#include "../environment/Observation.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Export configuration
struct ExportConfig {
    std::string exportPath;
    std::string filenamePrefix;
    std::chrono::system_clock::time_point timestamp;
    bool includeRawData{true};
    bool includeMetrics{true};
    bool includeNeuralState{true};
    bool includeBehavioralData{true};
    bool compress{false};
    std::string compressionAlgorithm{ "none" };
    int dataSamplingRate{1}; // Sample every Nth time step
    bool createMetadata{true};
};

// Neural State Export Structures
struct NeuronExportData {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    std::vector<NeuronId> incomingSynapseIds;
    std::vector<NeuronId> outgoingSynapseIds;
    std::vector<Timestamp> spikeHistory;
    PlasticityFlags plasticity;
    float adaptationVariable;
    float totalCurrent;
};

struct SynapseExportData {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    SynapticWeight weight;
    Delay delay;
    SynapseType type;
    Timestamp preSpikeTime;
    Timestamp postSpikeTime;
    PlasticityFlags plasticity;
    float eligibilityTrace;
    float efficacy;
    bool isActive;
};

struct NeuralRegionExportData {
    RegionId id;
    std::string name;
    std::vector<NeuronId> neuronIds;
    std::vector<SynapseId> synapseIds;
    std::vector<PopulationId> populationIds;
    float averageFiringRate;
    float synapticDensity;
    float averageSynapticWeight;
    size_t totalNeuronCount;
    size_t activeNeuronCount;
    size_t firingNeuronCount;
};

struct NeuralConnectivityExportData {
    std::vector<NeuronExportData> neurons;
    std::vector<SynapseExportData> synapses;
    std::vector<NeuralRegionExportData> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    float excitationInhibitionRatio;
    size_t totalSpikeCount;
    size_t pendingSpikeEvents;
};

// Metrics Export Structures
struct MetricsExportData {
    std::vector<std::string> metricNames;
    std::map<std::string, std::vector<double>> metricValues;
    std::map<std::string, double> meanValues;
    std::map<std::string, double> stdDevValues;
    std::map<std::string, double> minValues;
    std::map<std::string, double> maxValues;
    std::chrono::system_clock::time_point exportTime;
    double simulationTime; // in seconds
};

// Behavioral Data Export Structures
struct BehavioralObservationExportData {
    std::chrono::system_clock::time_point timestamp;
    SimulationStep step;
    double simulationTime;
    std::vector<float> sensoryData;
    ActionType actionType;
    std::vector<float> actionParameters;
    float reward;
    float predictionError;
    std::string behavioralContext;
    std::vector<NeuronId> activeNeurons;
    float arousal;
    float attention;
};

struct BehavioralDataExportData {
    std::vector<BehavioralObservationExportData> observations;
    std::vector<std::string> actionNames;
    std::map<std::string, float> actionFrequencies;
    float averageReward;
    float averagePredictionError;
    size_t totalActions;
};

// Memory Export Structures
struct WorkingMemoryExportData {
    std::map<NeuronId, float> neuronValues;
    std::chrono::system_clock::time_point lastUpdate;
};

struct EpisodicMemoryExportData {
    size_t episodeIndex;
    SimulationStep timestamp;
    std::vector<NeuronId> neurons;
    std::vector<float> values;
    std::string metadata;
};

struct SemanticMemoryExportData {
    std::string key;
    std::string value;
};

struct MemoryExportData {
    WorkingMemoryExportData workingMemory;
    std::vector<EpisodicMemoryExportData> episodicMemory;
    std::vector<SemanticMemoryExportData> semanticMemory;
    std::vector<Skill> proceduralMemory;
    std::vector<std::pair<NeuronId, NeuronId>> associations;
    std::map<NeuronId, float> associationStrengths;
};

// Neuromodulator Export Structures
struct NeuromodulatorExportData {
    std::string name;
    float level;
    float plasticityFactor;
    std::chrono::system_clock::time_point lastUpdate;
};

// Combined Complete Export Data
struct CompleteExportData {
    ExportConfig config;
    NeuralConnectivityExportData neuralState;
    MetricsExportData metrics;
    BehavioralDataExportData behavioralData;
    MemoryExportData memoryData;
    std::vector<NeuromodulatorExportData> neuromodulators;
    std::map<std::string, std::string> systemInfo;
};

// File format enumeration
enum class ExportFormat {
    CSV,
    JSON,
    Binary,
    Parquet,
    HDF5,
    CompressedCSV,
    CompressedJSON,
    CompressedBinary
};

// Export Progress Information
struct ExportProgress {
    std::string currentOperation;
    size_t totalSteps;
    size_t completedSteps;
    float progressPercentage;
    std::string currentFile;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::duration elapsedTime;
};

// Data Export Tool Class
class DataExportTool {
public:
    DataExportTool();
    ~DataExportTool();

    // Configuration and setup
    bool initialize(const ExportConfig& config);
    void shutdown();

    // Export entry points
    bool exportCompleteData(const std::string& outputPath, ExportFormat format);
    bool exportNeuralState(const std::string& outputPath, ExportFormat format);
    bool exportMetrics(const std::string& outputPath, ExportFormat format);
    bool exportBehavioralData(const std::string& outputPath, ExportFormat format);
    bool exportMemoryData(const std::string& outputPath, ExportFormat format);

    // Data collection from NLM system
    void setBrain(const Brain* brain);
    void setMetrics(const Metrics* metrics);
    void setMemory(const Memory* memory);
    void setNeuromodulators(const std::vector<const Neuromodulator*>& modulators);
    void setObservation(const Observation* observation);
    void setAction(const Action* action);
    void setConfig(const Config* config);

    // Export format writers
    class CSVExporter;
    class JSONExporter;
    class BinaryExporter;
    class ParquetExporter;
    class HDF5Exporter;

    // Utility functions
    static std::string getCurrentTimestamp();
    static std::string generateFilename(const std::string& prefix, ExportFormat format);
    static void createDirectories(const std::string& path);
    static std::string getFileExtension(ExportFormat format);

    // Progress monitoring
    ExportProgress getProgress() const;
    void setProgressCallback(std::function<void(const ExportProgress&)>
        callback);

    // Data validation
    bool validateExportData() const;
    std::string getValidationErrors() const;

    // File operations
    bool exportToFile(const CompleteExportData& data, const std::string& filepath,
        ExportFormat format);
    bool importFromFile(CompleteExportData& data, const std::string& filepath,
        ExportFormat format);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Private helper methods
    void collectNeuralStateData(NeuralConnectivityExportData& data) const;
    void collectMetricsData(MetricsExportData& data) const;
    void collectBehavioralData(BehavioralDataExportData& data) const;
    void collectMemoryData(MemoryExportData& data) const;
    void collectNeuromodulatorData(
        std::vector<NeuromodulatorExportData>& data) const;
    void collectSystemInfo(std::map<std::string, std::string>& info) const;
    void updateProgress(const std::string& operation, size_t current,
        size_t total);
    void writeMetadata(const ExportConfig& config, const std::string& filepath);
    bool createBackupIfNeeded(const std::string& filepath);

    // Helper functions
    static float calculateAverageFiringRate(const std::vector<Neuron*>& neurons) const;
    static size_t countActiveNeurons(const std::vector<Neuron*>& neurons) const;
    static size_t countFiringNeurons(const std::vector<Neuron*>& neurons) const;
    static Timestamp getCurrentTime() const;
    static SimulationStep getCurrentStep() const;
    static double getSimulationTime() const;
    static bool compressAndExport(const std::string& data, const std::string& filepath);

    // Enum conversion helpers
    static std::string neuronTypeToString(NeuronType type);
    static std::string synapseTypeToString(SynapseType type);
    static std::string actionTypeToString(ActionType type);

    // Memory export helpers
    static std::vector<EpisodicMemoryExportData> getEpisodicMemoryData();
    static std::vector<SemanticMemoryExportData> getSemanticMemoryData();
    static std::vector<Skill> getProceduralMemoryData();
    static std::vector<std::pair<NeuronId, NeuronId>> getAssociativeMemoryData();
    static std::map<NeuronId, float> getAssociationStrengthData();

    friend class CSVExporter;
    friend class JSONExporter;
    friend class BinaryExporter;
    friend class ParquetExporter;
    friend class HDF5Exporter;
};

} // namespace nlm
