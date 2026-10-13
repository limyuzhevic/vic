#include "ExperimentResult.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace nlm {

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    try {
        // Validate filepath
        if (filepath.empty()) {
            NLM_LOG_ERROR("saveResults: Empty filepath provided");
            return false;
        }
        
        // Ensure directory exists
        std::filesystem::path path(filepath);
        std::filesystem::path dir = path.parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            if (!std::filesystem::create_directories(dir)) {
                NLM_LOG_ERROR("saveResults: Failed to create directory: " + dir.string());
                return false;
            }
        }
        
        // Create ExperimentResult from current experiments
        ExperimentResult result;
        
        // Set experiment metadata
        result.experimentName = "ExperimentRunner_Results";
        result.config.globalSeed = pImpl->globalSeed;
        
        // Collect results from all experiments
        for (const auto& experiment : pImpl->experiments) {
            // Add experiment metrics
            const auto& metrics = experiment->getMetrics();
            double total_reward = 0.0;
            double avg_firing_rate = 0.0;
            size_t memory_episodes_stored = 0;
            
            // Sum up metrics from all recorded metrics
            for (const auto& metric : metrics) {
                if (metric.first == "total_reward") {
                    total_reward = metric.second;
                } else if (metric.first == "avg_firing_rate") {
                    avg_firing_rate = metric.second;
                }
            }
            
            result.metrics.total_reward = total_reward;
            result.metrics.avg_firing_rate = avg_firing_rate;
            
            // Get step data from experiment
            const auto& stepData = experiment->getStepData();
            for (const auto& step : stepData) {
                ExperimentResult::StepData resultStep;
                resultStep.step = step.step;
                resultStep.neuronCount = step.neuronCount;
                resultStep.synapseCount = step.synapseCount;
                resultStep.firingCount = step.firingCount;
                resultStep.avgFiringRate = step.avgFiringRate;
                result.steps.push_back(resultStep);
            }
            
            // Add experiment notes
            const auto& notes = experiment->getNotes();
            result.notes.insert(result.notes.end(), notes.begin(), notes.end());
        }
        
        // Add current timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        result.timestamp = std::ctime(&time);
        if (!result.timestamp.empty() && result.timestamp.back() == '\n') {
            result.timestamp.pop_back();
        }
        
        result.wallClockTimeSeconds = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
        
        // Convert to JSON and save
        nlohmann::json j = result.toJson();
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("saveResults: Failed to open file for writing: " + filepath);
            return false;
        }
        
        file << j.dump(4); // Pretty-print JSON with indentation
        file.close();
        
        NLM_LOG_INFO("saveResults: Successfully saved experiment results to " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("saveResults: Exception while saving results: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentRunner::loadResults(const std::string& filepath, ExperimentResult& result) {
    try {
        if (filepath.empty()) {
            NLM_LOG_ERROR("loadResults: Empty filepath provided");
            return false;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("loadResults: Failed to open file for reading: " + filepath);
            return false;
        }
        
        nlohmann::json j;
        try {
            file >> j;
        } catch (const nlohmann::json::parse_error& e) {
            NLM_LOG_ERROR("loadResults: JSON parse error: " + std::string(e.what()));
            return false;
        }
        
        result = ExperimentResult::fromJson(j);
        
        NLM_LOG_INFO("loadResults: Successfully loaded experiment results from " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("loadResults: Exception while loading results: " + std::string(e.what()));
        return false;
    }
}

std::string ExperimentRunner::analyzeResults(const ExperimentResult& result) const {
    return ResultsAnalyzer::generateStatisticalSummary(result);
}

std::string ExperimentRunner::compareResults(const ExperimentResult& result1, const ExperimentResult& result2) const {
    return ResultsAnalyzer::generateComparisonReport(result1, result2);
}

std::string ExperimentRunner::exportVisualizationData(const ExperimentResult& result) const {
    return ResultsAnalyzer::exportVisualizationData(result);
}

bool ExperimentRunner::validateResults(const ExperimentResult& result) const {
    if (result.experimentName.empty()) {
        NLM_LOG_ERROR("validateResults: Experiment name cannot be empty");
        return false;
    }
    
    if (result.steps.empty()) {
        NLM_LOG_WARNING("validateResults: No step data found");
    }
    
    // Check for reasonable metric values
    if (result.metrics.total_reward < 0.0) {
        NLM_LOG_WARNING("validateResults: Negative total reward");
    }
    
    if (result.metrics.avg_firing_rate < 0.0 || result.metrics.avg_firing_rate > 100.0) {
        NLM_LOG_WARNING("validateResults: Unreasonable firing rate: " + std::to_string(result.metrics.avg_firing_rate));
    }
    
    return true;
}

} // namespace nlm