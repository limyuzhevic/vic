// Copyright (c) 2026 NLM System
// ParquetExporter header for NLM Data Export Tool

#pragma once

#include "DataExportTool.hpp"
#include <memory>

namespace nlm {

class DataExportTool::ParquetExporter {
public:
    bool export(const CompleteExportData& data, const std::string& filepath);

private:
    std::shared_ptr<arrow::RecordBatch> createNeuralDataBatch(
        const NeuralConnectivityExportData& data);
    std::shared_ptr<arrow::RecordBatch> createMetricsDataBatch(
        const MetricsExportData& data);
    std::shared_ptr<arrow::RecordBatch> createBehavioralDataBatch(
        const BehavioralDataExportData& data);
};

} // namespace nlm
