// Copyright (c) 2026 NLM System
// HDF5Exporter header for NLM Data Export Tool

#pragma once

#include "DataExportTool.hpp"

namespace nlm {

class DataExportTool::HDF5Exporter {
public:
    bool export(const CompleteExportData& data, const std::string& filepath);

private:
    // Neural data export
    void writeNeuralData(hid_t groupId, const NeuralConnectivityExportData& data);
    void writeNeuronsDataset(hid_t groupId, const std::vector<NeuronExportData>& neurons);
    void writeSynapsesDataset(hid_t groupId, const std::vector<SynapseExportData>& synapses);
    void writeRegionsDataset(hid_t groupId, const std::vector<NeuralRegionExportData>& regions);

    // Metrics export
    void writeMetricsData(hid_t groupId, const MetricsExportData& data);
    void writeStringDataset(hid_t groupId, const std::string& name,
        const std::vector<std::string>& data);
    void writeDoubleDataset(hid_t groupId, const std::string& name,
        const std::vector<double>& data);
    void writeFloatDataset(hid_t groupId, const std::string& name,
        const std::vector<float>& data);
    void writeIntDataset(hid_t groupId, const std::string& name,
        const std::vector<int>& data);

    // Behavioral data export
    void writeBehavioralData(hid_t groupId, const BehavioralDataExportData& data);
    void writeObservationsDataset(hid_t groupId,
        const std::vector<BehavioralObservationExportData>& observations);

    // Memory export
    void writeMemoryData(hid_t groupId, const MemoryExportData& data);
    void writeWorkingMemory(hid_t groupId, const WorkingMemoryExportData& data);
    void writeEpisodicMemory(hid_t groupId,
        const std::vector<EpisodicMemoryExportData>& episodes);
    void writeSemanticMemory(hid_t groupId,
        const std::vector<SemanticMemoryExportData>& facts);
    void writeProceduralMemory(hid_t groupId, const std::vector<Skill>& skills);
    void writeAssociativeMemory(hid_t groupId,
        const std::vector<std::pair<NeuronId, NeuronId>>& associations,
        const std::map<NeuronId, float>& strengths);
    void createNeuromodulatorDataset(hid_t groupId, const std::string& path,
        const NeuromodulatorExportData& data);

    // Metadata export
    void writeMetadata(hid_t fileId, const ExportConfig& config,
        const std::map<std::string, std::string>& systemInfo);
    void writeConfigDataset(hid_t groupId, const ExportConfig& config);
    void writeSystemInfoDataset(hid_t groupId,
        const std::map<std::string, std::string>& systemInfo);

    // Attribute writing helpers
    void writeFloatAttribute(hid_t groupId, const std::string& name,
        const float* value, int rank = 1);
    void writeSizeTAttribute(hid_t groupId, const std::string& name,
        const size_t* value, int rank = 1);
    void writeStringAttribute(hid_t groupId, const std::string& name,
        const std::string& value);
    void writeBoolAttribute(hid_t groupId, const std::string& name, bool value);
    void writeIntAttribute(hid_t groupId, const std::string& name, int value);

    // Enum conversion helpers
    std::string neuronTypeToString(NeuronType type);
    std::string synapseTypeToString(SynapseType type);
    std::string actionTypeToString(ActionType type);
};

} // namespace nlm
