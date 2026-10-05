#include "OutputFormatter.hpp"
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <nlohmann/json.hpp>

namespace nlm {

struct OutputFormatter::Impl {
    std::string title;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::pair<std::string, std::string>> sections;
    std::vector<std::tuple<std::string, double, std::string>> metrics;
    std::vector<PerformanceResult> performanceResults;
    std::vector<std::tuple<std::string, bool, std::string>> experimentResults;
    
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AdvancedConfigManager> config;
    std::shared_ptr<PerformanceProfiler> profiler;
};

// HumanReadableFormatter implementation
struct HumanReadableFormatter::Impl {
    std::string formatBrainStatus(const Brain* brain) const;
    std::string formatConfiguration(const Config* config) const;
    std::string formatPerformance(const std::vector<PerformanceResult>& results) const;
    std::string formatExperiments(const std::vector<std::tuple<std::string, bool, std::string>>& experiments) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::string formatMetric(const std::tuple<std::string, double, std::string>& metric) const;
};

HumanReadableFormatter::HumanReadableFormatter() : pImpl(std::make_unique<Impl>()) {}

void HumanReadableFormatter::setBrain(std::shared_ptr<Brain> brain) {
    pImpl->brain = brain;
}

void HumanReadableFormatter::setConfig(std::shared_ptr<AdvancedConfigManager> config) {
    pImpl->config = config;
}

void HumanReadableFormatter::setProfiler(std::shared_ptr<PerformanceProfiler> profiler) {
    pImpl->profiler = profiler;
}

void HumanReadableFormatter::setTitle(const std::string& title) {
    pImpl->title = title;
}

void HumanReadableFormatter::setTimestamp(const std::chrono::system_clock::time_point& timestamp) {
    pImpl->timestamp = timestamp;
}

std::string HumanReadableFormatter::format() const {
    std::stringstream ss;
    
    if (!pImpl->title.empty()) {
        ss << "=" << std::string(pImpl->title.length() + 4, '=') << std::endl;
        ss << " " << pImpl->title << " " << std::string(pImpl->title.length() + 4, '=') << std::endl;
        ss << std::endl;
    }
    
    if (pImpl->timestamp.time_since_epoch().count() > 0) {
        ss << "Generated: " << formatTimestamp(pImpl->timestamp) << std::endl;
        ss << std::endl;
    }
    
    if (pImpl->brain) {
        ss << "=== BRAIN STATUS ===" << std::endl;
        ss << formatBrainStatus(pImpl->brain.get()) << std::endl;
        ss << std::endl;
    }
    
    if (pImpl->config) {
        ss << "=== CONFIGURATION ===" << std::endl;
        ss << formatConfiguration(pImpl->config.get()) << std::endl;
        ss << std::endl;
    }
    
    if (pImpl->profiler && !pImpl->profiler->getPerformanceData().empty()) {
        ss << "=== PERFORMANCE ===" << std::endl;
        ss << formatPerformance(pImpl->profiler->getPerformanceData()) << std::endl;
        ss << std::endl;
    }
    
    if (!pImpl->experimentResults.empty()) {
        ss << "=== EXPERIMENTS ===" << std::endl;
        ss << formatExperiments(pImpl->experimentResults) << std::endl;
        ss << std::endl;
    }
    
    for (const auto& section : pImpl->sections) {
        ss << "=== " << section.first << " ===" << std::endl;
        ss << section.second << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

bool HumanReadableFormatter::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << format();
    return true;
}

void HumanReadableFormatter::addSection(const std::string& name, const std::string& content) {
    pImpl->sections.emplace_back(name, content);
}

void HumanReadableFormatter::addMetric(const std::string& name, double value, const std::string& unit) {
    pImpl->metrics.emplace_back(name, value, unit);
}

void HumanReadableFormatter::addPerformanceResult(const PerformanceResult& result) {
    pImpl->performanceResults.push_back(result);
}

void HumanReadableFormatter::addExperimentResult(const std::string& name, bool success, const std::string& details) {
    pImpl->experimentResults.emplace_back(name, success, details);
}

void HumanReadableFormatter::clear() {
    pImpl->sections.clear();
    pImpl->metrics.clear();
    pImpl->performanceResults.clear();
    pImpl->experimentResults.clear();
}

std::string HumanReadableFormatter::formatBrainStatus() const {
    if (!pImpl->brain) {
        return "No brain data available.";
    }
    
    std::stringstream ss;
    ss << "Neuron Count: " << pImpl->brain->getTotalNeuronCount() << std::endl;
    ss << "Synapse Count: " << pImpl->brain->getTotalSynapseCount() << std::endl;
    ss << "Active Neurons: " << pImpl->brain->getActiveNeuronCount() << std::endl;
    ss << "Firing Neurons: " << pImpl->brain->getFiringNeuronCount() << std::endl;
    ss << "Total Spikes: " << pImpl->brain->getTotalSpikeCount() << std::endl;
    ss << "Average Firing Rate: " << std::fixed << std::setprecision(2) 
       << pImpl->brain->getAverageFiringRate() << std::endl;
    ss << "Excitation/Inhibition Ratio: " << std::fixed << std::setprecision(2)
       << pImpl->brain->getExcitationInhibitionRatio() << std::endl;
    
    return ss.str();
}

std::string HumanReadableFormatter::formatConfiguration() const {
    if (!pImpl->config) {
        return "No configuration data available.";
    }
    
    std::stringstream ss;
    auto keys = pImpl->config->getKeys();
    
    for (const auto& key : keys) {
        if (pImpl->config->has(key)) {
            ss << key << ": ";
            
            if (pImpl->config->has<int64_t>(key)) {
                ss << pImpl->config->get<int64_t>(key).value();
            } else if (pImpl->config->has<double>(key)) {
                ss << std::fixed << std::setprecision(6) << pImpl->config->get<double>(key).value();
            } else if (pImpl->config->has<bool>(key)) {
                ss << (pImpl->config->get<bool>(key).value() ? "true" : "false");
            } else if (pImpl->config->has<std::string>(key)) {
                ss << pImpl->config->get<std::string>(key).value();
            } else {
                ss << "[unknown]";
            }
            
            ss << std::endl;
        }
    }
    
    ss << "\nValidation: " << (pImpl->config->isValid() ? "PASSED" : "FAILED") << std::endl;
    if (!pImpl->config->isValid()) {
        ss << "Errors:" << std::endl;
        for (const auto& error : pImpl->config->getErrors()) {
            ss << "  - " << error.key << ": " << error.message << std::endl;
        }
    }
    
    return ss.str();
}

std::string HumanReadableFormatter::formatPerformance() const {
    std::stringstream ss;
    
    if (pImpl->performanceResults.empty()) {
        return "No performance data available.";
    }
    
    ss << "Total Runs: " << pImpl->performanceResults.size() << std::endl;
    ss << std::endl;
    
    double totalWallTime = 0;
    double totalCpuTime = 0;
    double totalThroughput = 0;
    
    for (size_t i = 0; i < pImpl->performanceResults.size(); ++i) {
        const auto& result = pImpl->performanceResults[i];
        
        totalWallTime += result.wallTime.count();
        totalCpuTime += result.cpuTime.count();
        totalThroughput += result.throughput;
        
        ss << std::setw(4) << (i + 1) << ". " << result.operationName << std::endl;
        ss << std::setw(8) << "  Wall Time: " << std::fixed << std::setprecision(4) 
           << result.wallTime.count() << "s" << std::endl;
        ss << std::setw(8) << "  CPU Time: " << std::fixed << std::setprecision(4) 
           << result.cpuTime.count() << "s" << std::endl;
        ss << std::setw(8) << "  Throughput: " << std::fixed << std::setprecision(2)
           << result.throughput << " ops/sec" << std::endl;
        ss << std::setw(8) << "  Memory: " << (result.memoryUsage / 1024 / 1024) << " MB" << std::endl;
        ss << std::endl;
    }
    
    if (!pImpl->performanceResults.empty()) {
        ss << "=== SUMMARY ===" << std::endl;
        ss << "Average Wall Time: " << std::fixed << std::setprecision(4)
           << (totalWallTime / pImpl->performanceResults.size()) << "s" << std::endl;
        ss << "Average CPU Time: " << std::fixed << std::setprecision(4)
           << (totalCpuTime / pImpl->performanceResults.size()) << "s" << std::endl;
        ss << "Average Throughput: " << std::fixed << std::setprecision(2)
           << (totalThroughput / pImpl->performanceResults.size()) << " ops/sec" << std::endl;
    }
    
    return ss.str();
}

std::string HumanReadableFormatter::formatExperiments() const {
    std::stringstream ss;
    
    if (pImpl->experimentResults.empty()) {
        return "No experiment data available.";
    }
    
    for (size_t i = 0; i < pImpl->experimentResults.size(); ++i) {
        const auto& [name, success, details] = pImpl->experimentResults[i];
        
        ss << std::setw(4) << (i + 1) << ". " << name << std::endl;
        ss << std::setw(8) << "  Status: " << (success ? "PASSED" : "FAILED") << std::endl;
        if (!details.empty()) {
            ss << std::setw(8) << "  Details: " << details << std::endl;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

std::string HumanReadableFormatter::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count()
       << " UTC";
    
    return ss.str();
}

std::string HumanReadableFormatter::formatMetric(const std::tuple<std::string, double, std::string>& metric) const {
    std::stringstream ss;
    ss << std::get<0>(metric) << ": " << std::fixed << std::setprecision(4) << std::get<1>(metric);
    if (!std::get<2>(metric).empty()) {
        ss << " " << std::get<2>(metric);
    }
    return ss.str();
}

// JsonFormatter implementation
struct JsonFormatter::Impl {
    nlohmann::json generateBrainJson(const Brain* brain) const;
    nlohmann::json generateConfigurationJson(const Config* config) const;
    nlohmann::json generatePerformanceJson(const std::vector<PerformanceResult>& results) const;
    nlohmann::json generateExperimentJson(const std::vector<std::tuple<std::string, bool, std::string>>& experiments) const;
    nlohmann::json generateMetadataJson() const;
};

JsonFormatter::JsonFormatter() : pImpl(std::make_unique<Impl>()) {}

void JsonFormatter::setBrain(std::shared_ptr<Brain> brain) {
    pImpl->brain = brain;
}

void JsonFormatter::setConfig(std::shared_ptr<AdvancedConfigManager> config) {
    pImpl->config = config;
}

void JsonFormatter::setProfiler(std::shared_ptr<PerformanceProfiler> profiler) {
    pImpl->profiler = profiler;
}

void JsonFormatter::setTitle(const std::string& title) {
    pImpl->title = title;
}

void JsonFormatter::setTimestamp(const std::chrono::system_clock::time_point& timestamp) {
    pImpl->timestamp = timestamp;
}

std::string JsonFormatter::format() const {
    nlohmann::json jsonData = generateMetadataJson();
    
    if (pImpl->brain) {
        jsonData["brain"] = generateBrainJson(pImpl->brain.get());
    }
    
    if (pImpl->config) {
        jsonData["configuration"] = generateConfigurationJson(pImpl->config.get());
    }
    
    if (pImpl->profiler && !pImpl->profiler->getPerformanceData().empty()) {
        jsonData["performance"] = generatePerformanceJson(pImpl->profiler->getPerformanceData());
    }
    
    if (!pImpl->experimentResults.empty()) {
        jsonData["experiments"] = generateExperimentJson(pImpl->experimentResults);
    }
    
    for (const auto& section : pImpl->sections) {
        jsonData["sections"][section.first] = section.second;
    }
    
    for (const auto& metric : pImpl->metrics) {
        nlohmann::json metricJson;
        metricJson["name"] = std::get<0>(metric);
        metricJson["value"] = std::get<1>(metric);
        metricJson["unit"] = std::get<2>(metric);
        jsonData["metrics"].push_back(metricJson);
    }
    
    for (const auto& result : pImpl->performanceResults) {
        jsonData["performance_results"].push_back(
            nlohmann::json{
                {"operation", result.operationName},
                {"wall_time", result.wallTime.count()},
                {"cpu_time", result.cpuTime.count()},
                {"memory_usage", result.memoryUsage},
                {"peak_memory_usage", result.peakMemoryUsage},
                {"instruction_count", result.instructionCount},
                {"cache_misses", result.cacheMisses},
                {"throughput", result.throughput}
            }
        );
    }
    
    return jsonData.dump(2);
}

bool JsonFormatter::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << format();
    return true;
}

void JsonFormatter::addSection(const std::string& name, const std::string& content) {
    pImpl->sections.emplace_back(name, content);
}

void JsonFormatter::addMetric(const std::string& name, double value, const std::string& unit) {
    pImpl->metrics.emplace_back(name, value, unit);
}

void JsonFormatter::addPerformanceResult(const PerformanceResult& result) {
    pImpl->performanceResults.push_back(result);
}

void JsonFormatter::addExperimentResult(const std::string& name, bool success, const std::string& details) {
    pImpl->experimentResults.emplace_back(name, success, details);
}

void JsonFormatter::clear() {
    pImpl->sections.clear();
    pImpl->metrics.clear();
    pImpl->performanceResults.clear();
    pImpl->experimentResults.clear();
}

nlohmann::json JsonFormatter::generateBrainData() const {
    nlohmann::json brainJson;
    
    if (!pImpl->brain) {
        brainJson["error"] = "No brain data available";
        return brainJson;
    }
    
    brainJson["neuron_count"] = pImpl->brain->getTotalNeuronCount();
    brainJson["synapse_count"] = pImpl->brain->getTotalSynapseCount();
    brainJson["active_neurons"] = pImpl->brain->getActiveNeuronCount();
    brainJson["firing_neurons"] = pImpl->brain->getFiringNeuronCount();
    brainJson["total_spikes"] = pImpl->brain->getTotalSpikeCount();
    brainJson["average_firing_rate"] = pImpl->brain->getAverageFiringRate();
    brainJson["excitation_inhibition_ratio"] = pImpl->brain->getExcitationInhibitionRatio();
    
    return brainJson;
}

nlohmann::json JsonFormatter::generateConfigurationData() const {
    nlohmann::json configJson;
    
    if (!pImpl->config) {
        configJson["error"] = "No configuration data available";
        return configJson;
    }
    
    auto keys = pImpl->config->getKeys();
    for (const auto& key : keys) {
        if (pImpl->config->has(key)) {
            nlohmann::json valueJson;
            
            if (pImpl->config->has<int64_t>(key)) {
                valueJson = pImpl->config->get<int64_t>(key).value();
            } else if (pImpl->config->has<double>(key)) {
                valueJson = pImpl->config->get<double>(key).value();
            } else if (pImpl->config->has<bool>(key)) {
                valueJson = pImpl->config->get<bool>(key).value();
            } else if (pImpl->config->has<std::string>(key)) {
                valueJson = pImpl->config->get<std::string>(key).value();
            }
            
            configJson[key] = valueJson;
        }
    }
    
    configJson["valid"] = pImpl->config->isValid();
    if (!pImpl->config->isValid()) {
        nlohmann::json errorsJson;
        for (const auto& error : pImpl->config->getErrors()) {
            nlohmann::json errorJson;
            errorJson["key"] = error.key;
            errorJson["message"] = error.message;
            errorsJson.push_back(errorJson);
        }
        configJson["errors"] = errorsJson;
    }
    
    return configJson;
}

nlohmann::json JsonFormatter::generatePerformanceData() const {
    nlohmann::json perfJson;
    
    if (!pImpl->profiler || pImpl->profiler->getPerformanceData().empty()) {
        perfJson["error"] = "No performance data available";
        return perfJson;
    }
    
    auto results = pImpl->profiler->getPerformanceData();
    perfJson["run_count"] = results.size();
    
    double totalWallTime = 0;
    double totalCpuTime = 0;
    double totalThroughput = 0;
    
    nlohmann::json resultsJson;
    for (const auto& result : results) {
        nlohmann::json resultJson;
        resultJson["operation"] = result.operationName;
        resultJson["wall_time"] = result.wallTime.count();
        resultJson["cpu_time"] = result.cpuTime.count();
        resultJson["memory_usage"] = result.memoryUsage;
        resultJson["peak_memory_usage"] = result.peakMemoryUsage;
        resultJson["instruction_count"] = result.instructionCount;
        resultJson["cache_misses"] = result.cacheMisses;
        resultJson["throughput"] = result.throughput;
        
        resultsJson.push_back(resultJson);
        
        totalWallTime += result.wallTime.count();
        totalCpuTime += result.cpuTime.count();
        totalThroughput += result.throughput;
    }
    
    perfJson["results"] = resultsJson;
    
    if (!results.empty()) {
        nlohmann::json summaryJson;
        summaryJson["average_wall_time"] = totalWallTime / results.size();
        summaryJson["average_cpu_time"] = totalCpuTime / results.size();
        summaryJson["average_throughput"] = totalThroughput / results.size();
        
        perfJson["summary"] = summaryJson;
    }
    
    return perfJson;
}

nlohmann::json JsonFormatter::generateExperimentData() const {
    nlohmann::json expJson;
    
    if (pImpl->experimentResults.empty()) {
        expJson["error"] = "No experiment data available";
        return expJson;
    }
    
    nlohmann::json experimentsJson;
    for (const auto& [name, success, details] : pImpl->experimentResults) {
        nlohmann::json expResultJson;
        expResultJson["name"] = name;
        expResultJson["success"] = success;
        expResultJson["details"] = details;
        
        experimentsJson.push_back(expResultJson);
    }
    
    expJson["experiments"] = experimentsJson;
    return expJson;
}

nlohmann::json JsonFormatter::generateMetadata() const {
    nlohmann::json metadataJson;
    
    metadataJson["title"] = pImpl->title;
    
    if (pImpl->timestamp.time_since_epoch().count() > 0) {
        auto time_t = std::chrono::system_clock::to_time_t(pImpl->timestamp);
        metadataJson["timestamp"] = std::string(std::ctime(&time_t));
    }
    
    metadataJson["version"] = "1.0.0";
    metadataJson["format"] = "json";
    metadataJson["generator"] = "NLM Output Formatter";
    
    return metadataJson;
}

void JsonFormatter::exportToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << format();
    }
}

// CsvFormatter implementation
struct CsvFormatter::Impl {
    std::string formatBrainCsv(const Brain* brain) const;
    std::string formatConfigurationCsv(const Config* config) const;
    std::string formatPerformanceCsv(const std::vector<PerformanceResult>& results) const;
    std::string formatExperimentCsv(const std::vector<std::tuple<std::string, bool, std::string>>& experiments) const;
};

CsvFormatter::CsvFormatter() : pImpl(std::make_unique<Impl>()) {}

void CsvFormatter::setBrain(std::shared_ptr<Brain> brain) {
    pImpl->brain = brain;
}

void CsvFormatter::setConfig(std::shared_ptr<AdvancedConfigManager> config) {
    pImpl->config = config;
}

void CsvFormatter::setProfiler(std::shared_ptr<PerformanceProfiler> profiler) {
    pImpl->profiler = profiler;
}

void CsvFormatter::setTitle(const std::string& title) {
    pImpl->title = title;
}

void CsvFormatter::setTimestamp(const std::chrono::system_clock::time_point& timestamp) {
    pImpl->timestamp = timestamp;
}

std::string CsvFormatter::format() const {
    std::stringstream ss;
    
    if (!pImpl->title.empty()) {
        ss << "# " << pImpl->title << std::endl;
        ss << std::endl;
    }
    
    if (pImpl->timestamp.time_since_epoch().count() > 0) {
        auto time_t = std::chrono::system_clock::to_time_t(pImpl->timestamp);
        ss << "# Generated: " << std::string(std::ctime(&time_t));
        ss << std::endl << std::endl;
    }
    
    if (pImpl->brain) {
        ss << formatBrainCsv(pImpl->brain.get()) << std::endl;
        ss << std::endl;
    }
    
    if (pImpl->config) {
        ss << formatConfigurationCsv(pImpl->config.get()) << std::endl;
        ss << std::endl;
    }
    
    if (pImpl->profiler && !pImpl->profiler->getPerformanceData().empty()) {
        ss << formatPerformanceCsv(pImpl->profiler->getPerformanceData()) << std::endl;
        ss << std::endl;
    }
    
    if (!pImpl->experimentResults.empty()) {
        ss << formatExperimentCsv(pImpl->experimentResults) << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

bool CsvFormatter::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << format();
    return true;
}

void CsvFormatter::addSection(const std::string& name, const std::string& content) {
    pImpl->sections.emplace_back(name, content);
}

void CsvFormatter::addMetric(const std::string& name, double value, const std::string& unit) {
    pImpl->metrics.emplace_back(name, value, unit);
}

void CsvFormatter::addPerformanceResult(const PerformanceResult& result) {
    pImpl->performanceResults.push_back(result);
}

void CsvFormatter::addExperimentResult(const std::string& name, bool success, const std::string& details) {
    pImpl->experimentResults.emplace_back(name, success, details);
}

void CsvFormatter::clear() {
    pImpl->sections.clear();
    pImpl->metrics.clear();
    pImpl->performanceResults.clear();
    pImpl->experimentResults.clear();
}

std::string CsvFormatter::formatBrainCsv() const {
    std::stringstream ss;
    
    if (!pImpl->brain) {
        ss << "# No brain data available";
        return ss.str();
    }
    
    ss << "Metric,Value" << std::endl;
    ss << "neuron_count," << pImpl->brain->getTotalNeuronCount() << std::endl;
    ss << "synapse_count," << pImpl->brain->getTotalSynapseCount() << std::endl;
    ss << "active_neurons," << pImpl->brain->getActiveNeuronCount() << std::endl;
    ss << "firing_neurons," << pImpl->brain->getFiringNeuronCount() << std::endl;
    ss << "total_spikes," << pImpl->brain->getTotalSpikeCount() << std::endl;
    ss << "average_firing_rate," << std::fixed << std::setprecision(6) 
       << pImpl->brain->getAverageFiringRate() << std::endl;
    ss << "excitation_inhibition_ratio," << std::fixed << std::setprecision(6)
       << pImpl->brain->getExcitationInhibitionRatio() << std::endl;
    
    return ss.str();
}

std::string CsvFormatter::formatConfigurationCsv() const {
    std::stringstream ss;
    
    if (!pImpl->config) {
        ss << "# No configuration data available";
        return ss.str();
    }
    
    auto keys = pImpl->config->getKeys();
    
    if (!keys.empty()) {
        ss << keys[0];
        for (size_t i = 1; i < keys.size(); ++i) {
            ss << "," << keys[i];
        }
        ss << std::endl;
        
        for (size_t row = 0; row < 1; ++row) {
            for (size_t col = 0; col < keys.size(); ++col) {
                if (col > 0) ss << ",";
                
                const std::string& key = keys[col];
                if (pImpl->config->has(key)) {
                    if (pImpl->config->has<int64_t>(key)) {
                        ss << pImpl->config->get<int64_t>(key).value();
                    } else if (pImpl->config->has<double>(key)) {
                        ss << std::fixed << std::setprecision(6) << pImpl->config->get<double>(key).value();
                    } else if (pImpl->config->has<bool>(key)) {
                        ss << (pImpl->config->get<bool>(key).value() ? "true" : "false");
                    } else if (pImpl->config->has<std::string>(key)) {
                        ss << pImpl->config->get<std::string>(key).value();
                    } else {
                        ss << "";
                    }
                } else {
                    ss << "";
                }
            }
            ss << std::endl;
        }
    }
    
    ss << "\n# Validation," << (pImpl->config->isValid() ? "PASSED" : "FAILED") << std::endl;
    
    return ss.str();
}

std::string CsvFormatter::formatPerformanceCsv() const {
    std::stringstream ss;
    
    if (!pImpl->profiler || pImpl->profiler->getPerformanceData().empty()) {
        ss << "# No performance data available";
        return ss.str();
    }
    
    auto results = pImpl->profiler->getPerformanceData();
    
    ss << "Run,Operation,Wall_Time,CPU_Time,Memory_Usage,Throughput" << std::endl;
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        ss << (i + 1) << "," << result.operationName << "," 
           << std::fixed << std::setprecision(6) << result.wallTime.count() << "," 
           << std::fixed << std::setprecision(6) << result.cpuTime.count() << "," 
           << result.memoryUsage << "," 
           << std::fixed << std::setprecision(2) << result.throughput << std::endl;
    }
    
    ss << "\n# Summary" << std::endl;
    ss << "Metric,Value" << std::endl;
    
    double totalWallTime = 0;
    double totalCpuTime = 0;
    double totalThroughput = 0;
    
    for (const auto& result : results) {
        totalWallTime += result.wallTime.count();
        totalCpuTime += result.cpuTime.count();
        totalThroughput += result.throughput;
    }
    
    ss << "Average_Wall_Time," << std::fixed << std::setprecision(6) 
       << (totalWallTime / results.size()) << std::endl;
    ss << "Average_CPU_Time," << std::fixed << std::setprecision(6) 
       << (totalCpuTime / results.size()) << std::endl;
    ss << "Average_Throughput," << std::fixed << std::setprecision(2) 
       << (totalThroughput / results.size()) << std::endl;
    
    return ss.str();
}

std::string CsvFormatter::formatExperimentCsv() const {
    std::stringstream ss;
    
    if (pImpl->experimentResults.empty()) {
        ss << "# No experiment data available";
        return ss.str();
    }
    
    ss << "Run,Name,Success,Details" << std::endl;
    
    for (size_t i = 0; i < pImpl->experimentResults.size(); ++i) {
        const auto& [name, success, details] = pImpl->experimentResults[i];
        ss << (i + 1) << "," << name << "," << (success ? "true" : "false") << "," 
           << details << std::endl;
    }
    
    return ss.str();
}

void CsvFormatter::exportToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << format();
    }
}

// MachineReadableFormatter implementation
struct MachineReadableFormatter::Impl {
    nlohmann::json generateBrainMachineJson(const Brain* brain) const;
    nlohmann::json generateConfigurationMachineJson(const Config* config) const;
    nlohmann::json generatePerformanceMachineJson(const std::vector<PerformanceResult>& results) const;
    nlohmann::json generateExperimentMachineJson(const std::vector<std::tuple<std::string, bool, std::string>>& experiments) const;
    nlohmann::json generateSystemInfoJson() const;
    nlohmann::json generateMetadataMachineJson() const;
    std::string jsonToProse(const nlohmann::json& json) const;
    std::string jsonToYaml(const nlohmann::json& json) const;
};

MachineReadableFormatter::MachineReadableFormatter() : pImpl(std::make_unique<Impl>()) {}

void MachineReadableFormatter::setBrain(std::shared_ptr<Brain> brain) {
    pImpl->brain = brain;
}

void MachineReadableFormatter::setConfig(std::shared_ptr<AdvancedConfigManager> config) {
    pImpl->config = config;
}

void MachineReadableFormatter::setProfiler(std::shared_ptr<PerformanceProfiler> profiler) {
    pImpl->profiler = profiler;
}

void MachineReadableFormatter::setTitle(const std::string& title) {
    pImpl->title = title;
}

void MachineReadableFormatter::setTimestamp(const std::chrono::system_clock::time_point& timestamp) {
    pImpl->timestamp = timestamp;
}

std::string MachineReadableFormatter::format() const {
    nlohmann::json jsonData;
    
    if (pImpl->title == "json") {
        jsonData = generateBrainMachineJson(pImpl->brain.get());
        jsonData.update(generateConfigurationMachineJson(pImpl->config.get()));
        jsonData.update(generatePerformanceMachineJson(pImpl->profiler->getPerformanceData()));
        jsonData.update(generateExperimentMachineJson(pImpl->experimentResults));
    } else if (pImpl->title == "yaml") {
        return jsonToYaml(generateBrainMachineJson(pImpl->brain.get()));
    }
    
    return jsonData.dump(2);
}

bool MachineReadableFormatter::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << format();
    return true;
}

void MachineReadableFormatter::addSection(const std::string& name, const std::string& content) {
    pImpl->sections.emplace_back(name, content);
}

void MachineReadableFormatter::addMetric(const std::string& name, double value, const std::string& unit) {
    pImpl->metrics.emplace_back(name, value, unit);
}

void MachineReadableFormatter::addPerformanceResult(const PerformanceResult& result) {
    pImpl->performanceResults.push_back(result);
}

void MachineReadableFormatter::addExperimentResult(const std::string& name, bool success, const std::string& details) {
    pImpl->experimentResults.emplace_back(name, success, details);
}

void MachineReadableFormatter::clear() {
    pImpl->sections.clear();
    pImpl->metrics.clear();
    pImpl->performanceResults.clear();
    pImpl->experimentResults.clear();
}

nlohmann::json MachineReadableFormatter::generateBrainMachineData() const {
    nlohmann::json brainJson;
    
    if (!pImpl->brain) {
        brainJson["error"] = "No brain data available";
        return brainJson;
    }
    
    brainJson["type"] = "brain_state";
    brainJson["neuron_count"] = pImpl->brain->getTotalNeuronCount();
    brainJson["synapse_count"] = pImpl->brain->getTotalSynapseCount();
    brainJson["active_neurons"] = pImpl->brain->getActiveNeuronCount();
    brainJson["firing_neurons"] = pImpl->brain->getFiringNeuronCount();
    brainJson["total_spikes"] = pImpl->brain->getTotalSpikeCount();
    brainJson["average_firing_rate"] = pImpl->brain->getAverageFiringRate();
    brainJson["excitation_inhibition_ratio"] = pImpl->brain->getExcitationInhibitionRatio();
    
    return brainJson;
}

nlohmann::json MachineReadableFormatter::generateConfigurationMachineData() const {
    nlohmann::json configJson;
    
    if (!pImpl->config) {
        configJson["error"] = "No configuration data available";
        return configJson;
    }
    
    configJson["type"] = "configuration";
    
    auto keys = pImpl->config->getKeys();
    nlohmann::json valuesJson;
    for (const auto& key : keys) {
        if (pImpl->config->has(key)) {
            if (pImpl->config->has<int64_t>(key)) {
                valuesJson[key] = pImpl->config->get<int64_t>(key).value();
            } else if (pImpl->config->has<double>(key)) {
                valuesJson[key] = pImpl->config->get<double>(key).value();
            } else if (pImpl->config->has<bool>(key)) {
                valuesJson[key] = pImpl->config->get<bool>(key).value();
            } else if (pImpl->config->has<std::string>(key)) {
                valuesJson[key] = pImpl->config->get<std::string>(key).value();
            }
        }
    }
    
    configJson["values"] = valuesJson;
    configJson["valid"] = pImpl->config->isValid();
    
    return configJson;
}

nlohmann::json MachineReadableFormatter::generatePerformanceMachineData() const {
    nlohmann::json perfJson;
    
    if (!pImpl->profiler || pImpl->profiler->getPerformanceData().empty()) {
        perfJson["error"] = "No performance data available";
        return perfJson;
    }
    
    perfJson["type"] = "performance_metrics";
    
    auto results = pImpl->profiler->getPerformanceData();
    perfJson["run_count"] = results.size();
    
    nlohmann::json resultsJson;
    for (const auto& result : results) {
        nlohmann::json resultJson;
        resultJson["operation"] = result.operationName;
        resultJson["wall_time_seconds"] = result.wallTime.count();
        resultJson["cpu_time_seconds"] = result.cpuTime.count();
        resultJson["memory_usage_bytes"] = result.memoryUsage;
        resultJson["peak_memory_usage_bytes"] = result.peakMemoryUsage;
        resultJson["instruction_count"] = result.instructionCount;
        resultJson["cache_misses"] = result.cacheMisses;
        resultJson["throughput_ops_per_second"] = result.throughput;
        
        resultsJson.push_back(resultJson);
    }
    
    perfJson["results"] = resultsJson;
    return perfJson;
}

nlohmann::json MachineReadableFormatter::generateExperimentMachineData() const {
    nlohmann::json expJson;
    
    if (pImpl->experimentResults.empty()) {
        expJson["error"] = "No experiment data available";
        return expJson;
    }
    
    expJson["type"] = "experiment_results";
    
    nlohmann::json experimentsJson;
    for (const auto& [name, success, details] : pImpl->experimentResults) {
        nlohmann::json expResultJson;
        expResultJson["name"] = name;
        expResultJson["success"] = success;
        expResultJson["details"] = details;
        
        experimentsJson.push_back(expResultJson);
    }
    
    expJson["experiments"] = experimentsJson;
    return expJson;
}

nlohmann::json MachineReadableFormatter::generateMetadataMachineJson() const {
    nlohmann::json metadataJson;
    
    metadataJson["type"] = "metadata";
    metadataJson["title"] = pImpl->title;
    metadataJson["version"] = "1.0.0";
    metadataJson["generator"] = "NLM Machine Readable Formatter";
    
    if (pImpl->timestamp.time_since_epoch().count() > 0) {
        auto time_t = std::chrono::system_clock::to_time_t(pImpl->timestamp);
        metadataJson["timestamp"] = std::string(std::ctime(&time_t));
    }
    
    return metadataJson;
}

std::string MachineReadableFormatter::jsonToProse(const nlohmann::json& json) const {
    std::stringstream ss;
    ss << "{\n";
    for (auto it = json.begin(); it != json.end(); ++it) {
        ss << "  \"" << it.key() << "\": " << it.value().dump() << ",\n";
    }
    ss << "}";
    return ss.str();
}

std::string MachineReadableFormatter::jsonToYaml(const nlohmann::json& json) const {
    // Simplified YAML generation - in production would use a proper YAML library
    std::stringstream ss;
    ss << "# Generated YAML output from NLM\n";
    ss << "format: yaml\n";
    ss << "version: 1.0.0\n";
    
    for (auto it = json.begin(); it != json.end(); ++it) {
        ss << it.key() << ": " << std::setw(2) << it.value() << "\n";
    }
    
    return ss.str();
}

void MachineReadableFormatter::exportToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << format();
    }
}

// OutputFormatterFactory implementation
const std::vector<std::string> OutputFormatterFactory::supportedFormats = {
    "human", "json", "csv", "machine"
};

std::unique_ptr<OutputFormatter> OutputFormatterFactory::createFormatter(const std::string& format) {
    if (format == "human") {
        return std::make_unique<HumanReadableFormatter>();
    } else if (format == "json") {
        return std::make_unique<JsonFormatter>();
    } else if (format == "csv") {
        return std::make_unique<CsvFormatter>();
    } else if (format == "machine") {
        return std::make_unique<MachineReadableFormatter>();
    }
    
    return nullptr;
}

bool OutputFormatterFactory::saveFormattedOutput(const std::string& format, const std::string& filepath,
                                                 std::shared_ptr<Brain> brain,
                                                 std::shared_ptr<AdvancedConfigManager> config,
                                                 std::shared_ptr<PerformanceProfiler> profiler) {
    auto formatter = createFormatter(format);
    if (!formatter) {
        return false;
    }
    
    formatter->setBrain(brain);
    formatter->setConfig(config);
    formatter->setProfiler(profiler);
    
    return formatter->saveToFile(filepath);
}

std::string OutputFormatterFactory::getSupportedFormats() {
    std::stringstream ss;
    ss << "Supported output formats: ";
    for (size_t i = 0; i < supportedFormats.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << supportedFormats[i];
    }
    return ss.str();
}

bool OutputFormatterFactory::validateFormat(const std::string& format) {
    return std::find(supportedFormats.begin(), supportedFormats.end(), format) != supportedFormats.end();
}

} // namespace nlm
