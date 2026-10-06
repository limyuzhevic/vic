// Copyright (c) 2026 NLM System
// CSVExporter header for NLM Data Export Tool

#pragma once

#include "DataExportTool.hpp"

namespace nlm {

class DataExportTool::CSVExporter {
public:
    bool export(const CompleteExportData& data, const std::string& filepath);

private:
    void exportNeuralDataCSV(std::ofstream& file,
        const NeuralConnectivityExportData& data);
    void exportMetricsCSV(std::ofstream& file,
        const MetricsExportData& data);
    void exportBehavioralDataCSV(std::ofstream& file,
        const BehavioralDataExportData& data);
    void exportMemoryDataCSV(std::ofstream& file,
        const MemoryExportData& data);
    void exportNeuromodulatorDataCSV(std::ofstream& file,
        const std::vector<NeuromodulatorExportData>& data);

    std::string getTimestampString(
        const std::chrono::system_clock::time_point& tp);
    std::string neuronTypeToString(NeuronType type);
    std::string synapseTypeToString(SynapseType type);
    std::string actionTypeToString(ActionType type);
    std::string neuromodulatorTypeToString(const char* name);
};

} // namespace nlm
