#pragma once

#include "enhanced_experiments.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

class ResultExporter {
public:
    ResultExporter() {
    }

    // Export experiment results to CSV format
    bool exportToCSV(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const;

    // Export experiment results to JSON format
    bool exportToJSON(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const;

    // Export experiment results to Markdown format
    bool exportToMarkdown(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const;

    // Export experiment results to HTML format
    bool exportToHTML(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const;

    // Export experiment results to PDF format (requires external library)
    bool exportToPDF(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const;

    // Batch export multiple experiments
    bool exportBatch(const std::vector<std::shared_ptr<BaseExperiment>>& experiments, 
                    const std::string& outputDirectory, const std::string& format = "csv") const;

private:
    std::string escapeCSVValue(const std::string& value) const;
    std::string generateHTMLReport(const std::shared_ptr<BaseExperiment>& experiment) const;
};

} // namespace nlm