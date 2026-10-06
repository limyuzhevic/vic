// Copyright (c) 2026 NLM System
// BinaryExporter header for NLM Data Export Tool

#pragma once

#include "DataExportTool.hpp"
#include <string>
#include <sstream>

namespace nlm {

class DataExportTool::BinaryExporter {
public:
    bool export(const CompleteExportData& data, const std::string& filepath);
    std::string exportToString(const CompleteExportData& data);

private:
    void writeBinaryHeader(std::ostream& stream, const CompleteExportData& data);
    void writeNeuralDataBinary(std::ostream& stream,
        const NeuralConnectivityExportData& data);
    void writeMetricsDataBinary(std::ostream& stream,
        const MetricsExportData& data);
    void writeBehavioralDataBinary(std::ostream& stream,
        const BehavioralDataExportData& data);
    void writeMemoryDataBinary(std::ostream& stream,
        const MemoryExportData& data);
    void writeNeuromodulatorDataBinary(std::ostream& stream,
        const std::vector<NeuromodulatorExportData>& data);
    void writeSystemInfoBinary(std::ostream& stream,
        const std::map<std::string, std::string>& systemInfo);

    // Individual data serialization methods
    void writeNeuronBinary(std::ostream& stream, const NeuronExportData& neuron);
    void writeSynapseBinary(std::ostream& stream, const SynapseExportData& synapse);
    void writeRegionBinary(std::ostream& stream, const NeuralRegionExportData& region);
    void writeBehavioralObservationBinary(std::ostream& stream,
        const BehavioralObservationExportData& obs);

    // Helper serialization methods
    void writeStringBinary(std::ostream& stream, const std::string& str);
    void writeNeuronIdBinary(std::ostream& stream, NeuronId id);
    void writeSynapseIdBinary(std::ostream& stream, SynapseId id);
    void writeRegionIdBinary(std::ostream& stream, RegionId id);
    void writePopulationIdBinary(std::ostream& stream, PopulationId id);
    void writeNeuronTypeBinary(std::ostream& stream, NeuronType type);
    void writeSynapseTypeBinary(std::ostream& stream, SynapseType type);
    void writePlasticityFlagsBinary(std::ostream& stream,
        const PlasticityFlags& flags);
    void writeActionTypeBinary(std::ostream& stream, ActionType type);

    // Memory serialization methods
    void writeWorkingMemoryBinary(std::ostream& stream,
        const WorkingMemoryExportData& data);
    void writeEpisodicMemoryBinary(std::ostream& stream,
        const std::vector<EpisodicMemoryExportData>& episodes);
    void writeSemanticMemoryBinary(std::ostream& stream,
        const std::vector<SemanticMemoryExportData>& facts);
    void writeProceduralMemoryBinary(std::ostream& stream,
        const std::vector<Skill>& skills);
    void writeAssociativeMemoryBinary(std::ostream& stream,
        const std::vector<std::pair<NeuronId, NeuronId>>& associations,
        const std::map<NeuronId, float>& strengths);
    void writeNeuromodulatorBinary(std::ostream& stream,
        const NeuromodulatorExportData& data);
};

} // namespace nlm
