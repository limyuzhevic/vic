// Copyright (c) 2026 NLM System
// JSONExporter header for NLM Data Export Tool

#pragma once

#include "DataExportTool.hpp"
#include <string>

namespace nlm {

class DataExportTool::JSONExporter {
public:
    bool export(const CompleteExportData& data, const std::string& filepath);
    std::string exportToString(const CompleteExportData& data);
};

} // namespace nlm
