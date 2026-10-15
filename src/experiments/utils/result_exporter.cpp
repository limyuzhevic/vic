#include "result_exporter.hpp"
#include "enhanced_experiments.hpp"
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

bool ResultExporter::exportToCSV(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Header
    file << "Experiment Name,Experiment Type,Progress,Current Step,Total Steps,Performance Score" << std::endl;
    
    // Experiment data
    file << experiment->getName() << ",";
    file << experiment->getExperimentType() << ",";
    file << experiment->getProgress() << ",";
    file << experiment->getCurrentStep() << ",";
    file << experiment->getTotalSteps() << ",";
    file << experiment->calculatePerformanceScore() << std::endl;
    
    // Metrics
    file << "Metric,Value" << std::endl;
    auto metrics = experiment->getMetrics();
    for (const auto& metric : metrics) {
        file << metric.first << "," << metric.second << std::endl;
    }
    
    file.close();
    return true;
}

bool ResultExporter::exportToJSON(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const {
    json j;
    j["experiment_name"] = experiment->getName();
    j["experiment_type"] = experiment->getExperimentType();
    j["progress"] = experiment->getProgress();
    j["current_step"] = experiment->getCurrentStep();
    j["total_steps"] = experiment->getTotalSteps();
    j["performance_score"] = experiment->calculatePerformanceScore();
    
    // Add metrics
    j["metrics"] = json::object();
    auto metrics = experiment->getMetrics();
    for (const auto& metric : metrics) {
        j["metrics"][metric.first] = metric.second;
    }
    
    // Add step data
    j["step_data"] = json::array();
    const auto& stepData = experiment->getStepData();
    for (const auto& step : stepData) {
        json stepJson;
        stepJson["step"] = step.step;
        stepJson["neuron_count"] = step.neuronCount;
        stepJson["synapse_count"] = step.synapseCount;
        stepJson["firing_count"] = step.firingCount;
        stepJson["avg_firing_rate"] = step.avgFiringRate;
        j["step_data"].push_back(stepJson);
    }
    
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << j.dump(2);
        file.close();
        return true;
    }
    
    return false;
}

bool ResultExporter::exportToMarkdown(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# Experiment Report" << std::endl;
    file << std::endl;
    file << "**Experiment Name**: " << experiment->getName() << std::endl;
    file << "**Experiment Type**: " << experiment->getExperimentType() << std::endl;
    file << "**Progress**: " << (experiment->getProgress() * 100) << "%" << std::endl;
    file << "**Current Step**: " << experiment->getCurrentStep() << " / " << experiment->getTotalSteps() << std::endl;
    file << "**Performance Score**: " << experiment->calculatePerformanceScore() << std::endl;
    file << std::endl;
    
    file << "## Metrics" << std::endl;
    file << std::endl;
    
    auto metrics = experiment->getMetrics();
    if (!metrics.empty()) {
        file << "| Metric | Value |" << std::endl;
        file << "|--------|-------|" << std::endl;
        for (const auto& metric : metrics) {
            file << "| " << metric.first << " | " << metric.second << " |" << std::endl;
        }
    }
    
    file << std::endl;
    
    file << "## Step Data" << std::endl;
    file << std::endl;
    
    const auto& stepData = experiment->getStepData();
    if (!stepData.empty()) {
        file << "| Step | Neurons | Synapses | Firing | Avg Firing Rate |" << std::endl;
        file << "|------|---------|----------|--------|-----------------|" << std::endl;
        for (const auto& step : stepData) {
            file << "| " << step.step << " | " << step.neuronCount << " | " << step.synapseCount << " | ";
            file << step.firingCount << " | " << std::fixed << std::setprecision(2) << step.avgFiringRate << " |" << std::endl;
        }
    }
    
    file.close();
    return true;
}

bool ResultExporter::exportToHTML(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head>" << std::endl;
    file << "    <title>Experiment Report: " << experiment->getName() << "</title>" << std::endl;
    file << "    <style>" << std::endl;
    file << "        body { font-family: Arial, sans-serif; margin: 40px; }" << std::endl;
    file << "        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }" << std::endl;
    file << "        .metric { background-color: #f9f9f9; padding: 10px; margin: 5px 0; }" << std::endl;
    file << "        table { border-collapse: collapse; width: 100%; margin: 20px 0; }" << std::endl;
    file << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
    file << "        th { background-color: #f2f2f2; }" << std::endl;
    file << "    </style>" << std::endl;
    file << "</head>" << std::endl;
    file << "<body>" << std::endl;
    file << "    <div class='header'>" << std::endl;
    file << "        <h1>Experiment Report</h1>" << std::endl;
    file << "        <h2>" << experiment->getName() << "</h2>" << std::endl;
    file << "        <p><strong>Type:</strong> " << experiment->getExperimentType() << "</p>" << std::endl;
    file << "        <p><strong>Progress:</strong> " << (experiment->getProgress() * 100) << "%</p>" << std::endl;
    file << "        <p><strong>Current Step:</strong> " << experiment->getCurrentStep() << " / " << experiment->getTotalSteps() << "</p>" << std::endl;
    file << "        <p><strong>Performance Score:</strong> " << experiment->calculatePerformanceScore() << "</p>" << std::endl;
    file << "    </div>" << std::endl;
    file << "" << std::endl;
    file << "    <h3>Metrics</h3>" << std::endl;
    file << "    <table>" << std::endl;
    file << "        <tr><th>Metric</th><th>Value</th></tr>" << std::endl;
    
    auto metrics = experiment->getMetrics();
    for (const auto& metric : metrics) {
        file << "        <tr><td>" << metric.first << "</td><td>" << metric.second << "</td></tr>" << std::endl;
    }
    
    file << "    </table>" << std::endl;
    file << "    <h3>Step Data</h3>" << std::endl;
    file << "    <table>" << std::endl;
    file << "        <tr><th>Step</th><th>Neurons</th><th>Synapses</th><th>Firing</th><th>Avg Firing Rate</th></tr>" << std::endl;
    
    const auto& stepData = experiment->getStepData();
    for (const auto& step : stepData) {
        file << "        <tr>";
        file << "<td>" << step.step << "</td><td>" << step.neuronCount << "</td><td>" << step.synapseCount << "</td>";
        file << "<td>" << step.firingCount << "</td><td>" << std::fixed << std::setprecision(2) << step.avgFiringRate << "</td>";
        file << "</tr>" << std::endl;
    }
    
    file << "    </table>" << std::endl;
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
    
    file.close();
    return true;
}

bool ResultExporter::exportToPDF(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const {
    // PDF export requires external library (e.g., libharu, wkhtmltopdf)
    // This is a placeholder implementation
    std::cerr << "Warning: PDF export not implemented (requires external library)" << std::endl;
    return false;
}

bool ResultExporter::exportBatch(const std::vector<std::shared_ptr<BaseExperiment>>& experiments, 
                                 const std::string& outputDirectory, const std::string& format) const {
    // Ensure output directory exists
    if (!std::filesystem::exists(outputDirectory)) {
        if (!std::filesystem::create_directories(outputDirectory)) {
            std::cerr << "Error: Cannot create output directory: " << outputDirectory << std::endl;
            return false;
        }
    }
    
    bool allSuccess = true;
    for (size_t i = 0; i < experiments.size(); ++i) {
        std::string filename = outputDirectory + "/experiment_" + std::to_string(i) + "." + format;
        
        bool success = false;
        if (format == "csv") {
            success = exportToCSV(experiments[i], filename);
        } else if (format == "json") {
            success = exportToJSON(experiments[i], filename);
        } else if (format == "markdown") {
            success = exportToMarkdown(experiments[i], filename);
        } else if (format == "html") {
            success = exportToHTML(experiments[i], filename);
        } else {
            std::cerr << "Warning: Unknown format " << format << " for batch export" << std::endl;
            success = false;
        }
        
        if (!success) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

std::string ResultExporter::escapeCSVValue(const std::string& value) const {
    // Simple CSV escaping - escape quotes and wrap in quotes if contains comma or quote
    if (value.find(',') != std::string::npos || value.find('"') != std::string::npos) {
        std::string escaped = value;
        for (char& c : escaped) {
            if (c == '"') {
                c = '"'; // Escape quote by doubling
            }
        }
        return '"' + escaped + '"';
    }
    return value;
}

std::string ResultExporter::generateHTMLReport(const std::shared_ptr<BaseExperiment>& experiment) const {
    // Generate HTML report (simplified)
    std::stringstream html;
    html << "<html><body><h1>" << experiment->getName() << "</h1>";
    html << "<p>Progress: " << (experiment->getProgress() * 100) << "%</p>";
    html << "<p>Performance Score: " << experiment->calculatePerformanceScore() << "</p>";
    html << "</body></html>";
    return html.str();
}

} // namespace nlm