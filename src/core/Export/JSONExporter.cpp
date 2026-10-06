// Copyright (c) 2026 NLM System
// JSONExporter implementation for NLM Data Export Tool

#include "JSONExporter.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>

namespace nlm {

bool DataExportTool::JSONExporter::export(
    const CompleteExportData& data, const std::string& filepath) {
    nlohmann::json j = data;

    std::ofstream file(filepath);
    if (!file) return false;

    file << j.dump(2);
    file.close();
    return true;
}

std::string DataExportTool::JSONExporter::exportToString(
    const CompleteExportData& data) {
    nlohmann::json j = data;
    return j.dump(2);
}

} // namespace nlm
