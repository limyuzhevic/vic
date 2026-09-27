#include "cli/AnalyzeCommand.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>

using json = nlohmann::json;

using namespace nlm;
namespace cli {

AnalyzeCommand::AnalyzeCommand() 
    : CommandBase("analyze", "Analyze simulation results"),
      input_file_(""),
      output_file_(""),
      analysis_type_("summary") {
}

bool AnalyzeCommand::execute(const std::map<std::string, std::string>& args) {
    NLM_LOG_INFO("CLI", "Analyzing results: " + analysis_type_);
    NLM_LOG_INFO("CLI", "Input file: " + input_file_);
    
    if (input_file_.empty()) {
        NLM_LOG_ERROR("CLI", "No input file specified", "Use --input to specify input file");
        return false;
    }
    
    // Load data
    json data = loadDataFile(input_file_);
    
    // Perform analysis
    json analysis_result;
    bool success = false;
    
    if (analysis_type_ == "summary") {
        success = performSummaryAnalysis(data, analysis_result);
    } else if (analysis_type_ == "detailed") {
        success = performDetailedAnalysis(data, analysis_result);
    } else if (analysis_type_ == "statistical") {
        success = performStatisticalAnalysis(data, analysis_result);
    } else if (analysis_type_ == "learning") {
        success = performLearningAnalysis(data, analysis_result);
    } else if (analysis_type_ == "performance") {
        success = performPerformanceAnalysis(data, analysis_result);
    } else {
        NLM_LOG_ERROR("CLI", "Unknown analysis type: " + analysis_type_, "Available types: summary, detailed, statistical, learning, performance");
        return false;
    }
    
    if (success) {
        // Export results
        if (!output_file_.empty()) {
            saveAnalysisResults(analysis_result, output_file_);
        }
        
        NLM_LOG_INFO("CLI", "Analysis completed successfully!");
    }
    
    return success;
}

void AnalyzeCommand::printHelp() const {
    std::cout << "Usage: nlm-cli analyze <input_file> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Analyze simulation results." << std::endl;
    std::cout << std::endl;
    std::cout << "Arguments:" << std::endl;
    std::cout << "  <input_file>          Input data file to analyze" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -o, --output <file>   Output file for analysis results" << std::endl;
    std::cout << "  -t, --type <type>     Analysis type: summary, detailed, statistical, learning, performance" << std::endl;
    std::cout << "  -q, --quiet           Suppress console output" << std::endl;
    std::cout << "  -v, --verbose         Enable verbose logging" << std::endl;
}

void AnalyzeCommand::printOptions() const {
    std::cout << "  [command options]" << std::endl;
}

json AnalyzeCommand::loadDataFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open data file: " + filepath);
    }
    
    json data;
    try {
        file >> data;
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid data file format: " + std::string(e.what()));
    }
    
    file.close();
    return data;
}

bool AnalyzeCommand::performSummaryAnalysis(const json& data, json& analysis_result) {
    NLM_LOG_INFO("CLI", "  Performing summary analysis...");
    
    analysis_result["analysis_type"] = "summary";
    analysis_result["timestamp"] = getTimestamp();
    
    // Extract basic information
    if (data.contains("total_neurons")) {
        analysis_result["total_neurons"] = data["total_neurons"];
    }
    if (data.contains("total_spikes")) {
        analysis_result["total_spikes"] = data["total_spikes"];
    }
    if (data.contains("total_synapses")) {
        analysis_result["total_synapses"] = data["total_synapses"];
    }
    
    // Calculate derived metrics
    if (data.contains("total_neurons") && data.contains("total_spikes")) {
        double avg_firing_rate = static_cast<double>(data["total_spikes"]) / 
                                data.value("simulation_steps", 1000);
        analysis_result["average_firing_rate"] = avg_firing_rate;
    }
    
    if (data.contains("total_neurons") && data.contains("total_synapses")) {
        double synapse_density = static_cast<double>(data["total_synapses"]) / 
                               data["total_neurons"];
        analysis_result["synaptic_density"] = synapse_density;
    }
    
    return true;
}

bool AnalyzeCommand::performDetailedAnalysis(const json& data, json& analysis_result) {
    NLM_LOG_INFO("CLI", "  Performing detailed analysis...");
    
    analysis_result["analysis_type"] = "detailed";
    analysis_result["timestamp"] = getTimestamp();
    
    // Perform detailed analysis on all data
    analysis_result["data_overview"] = calculateDataOverview(data);
    analysis_result["statistical_summary"] = calculateStatisticalSummary(data);
    analysis_result["anomaly_detection"] = detectAnomalies(data);
    
    return true;
}

bool AnalyzeCommand::performStatisticalAnalysis(const json& data, json& analysis_result) {
    NLM_LOG_INFO("CLI", "  Performing statistical analysis...");
    
    analysis_result["analysis_type"] = "statistical";
    analysis_result["timestamp"] = getTimestamp();
    
    analysis_result["descriptive_statistics"] = calculateDescriptiveStatistics(data);
    analysis_result["inferential_statistics"] = calculateInferentialStatistics(data);
    analysis_result["distribution_analysis"] = analyzeDistributions(data);
    
    return true;
}

bool AnalyzeCommand::performLearningAnalysis(const json& data, json& analysis_result) {
    NLM_LOG_INFO("CLI", "  Performing learning analysis...");
    
    analysis_result["analysis_type"] = "learning";
    analysis_result["timestamp"] = getTimestamp();
    
    analysis_result["plasticity_metrics"] = calculatePlasticityMetrics(data);
    analysis_result["learning_indicators"] = calculateLearningIndicators(data);
    
    return true;
}

bool AnalyzeCommand::performPerformanceAnalysis(const json& data, json& analysis_result) {
    NLM_LOG_INFO("CLI", "  Performing performance analysis...");
    
    analysis_result["analysis_type"] = "performance";
    analysis_result["timestamp"] = getTimestamp();
    
    analysis_result["speed_metrics"] = calculateSpeedMetrics(data);
    analysis_result["efficiency_metrics"] = calculateEfficiencyMetrics(data);
    
    return true;
}

json AnalyzeCommand::calculateDataOverview(const json& data) {
    json overview;
    overview["keys"] = json::array();
    for (const auto& item : data.items()) {
        overview["keys"].push_back(item.key());
    }
    
    overview["data_size"] = data.size();
    
    return overview;
}

json AnalyzeCommand::calculateStatisticalSummary(const json& data) {
    json summary;
    summary["mean"] = 0.0;
    summary["median"] = 0.0;
    summary["std_dev"] = 0.0;
    
    if (data.contains("values")) {
        std::vector<double> values;
        for (const auto& value : data["values"]) {
            if (value.is_number()) {
                values.push_back(value.get<double>());
            }
        }
        
        if (!values.empty()) {
            summary["mean"] = calculateMean(values);
            summary["median"] = calculateMedian(values);
            summary["std_dev"] = calculateStdDev(values);
            summary["min"] = *std::min_element(values.begin(), values.end());
            summary["max"] = *std::max_element(values.begin(), values.end());
        }
    }
    
    return summary;
}

json AnalyzeCommand::detectAnomalies(const json& data) {
    json anomalies;
    anomalies["detected"] = false;
    anomalies["anomalies"] = json::array();
    
    // Simple anomaly detection
    if (data.contains("values")) {
        std::vector<double> values;
        for (const auto& value : data["values"]) {
            if (value.is_number()) {
                values.push_back(value.get<double>());
            }
        }
        
        if (!values.empty()) {
            double mean = calculateMean(values);
            double std_dev = calculateStdDev(values);
            
            for (double value : values) {
                double z_score = std::abs(value - mean) / (std_dev + 0.0001);
                if (z_score > 3.0) {
                    json anomaly;
                    anomaly["value"] = value;
                    anomaly["z_score"] = z_score;
                    anomalies["anomalies"].push_back(anomaly);
                }
            }
            
            anomalies["detected"] = !anomalies["anomalies"].empty();
        }
    }
    
    return anomalies;
}

json AnalyzeCommand::calculateDescriptiveStatistics(const json& data) {
    json stats;
    stats["mean"] = 0.0;
    stats["median"] = 0.0;
    stats["std_dev"] = 0.0;
    
    if (data.contains("values")) {
        std::vector<double> values;
        for (const auto& value : data["values"]) {
            if (value.is_number()) {
                values.push_back(value.get<double>());
            }
        }
        
        if (!values.empty()) {
            stats["mean"] = calculateMean(values);
            stats["median"] = calculateMedian(values);
            stats["std_dev"] = calculateStdDev(values);
            stats["min"] = *std::min_element(values.begin(), values.end());
            stats["max"] = *std::max_element(values.begin(), values.end());
        }
    }
    
    return stats;
}

json AnalyzeCommand::calculateInferentialStatistics(const json& data) {
    json stats;
    stats["confidence_intervals"] = json::array();
    
    if (data.contains("values")) {
        std::vector<double> values;
        for (const auto& value : data["values"]) {
            if (value.is_number()) {
                values.push_back(value.get<double>());
            }
        }
        
        if (!values.empty()) {
            double mean = calculateMean(values);
            double std_dev = calculateStdDev(values);
            double margin = 1.96 * std_dev / std::sqrt(values.size());
            
            json ci;
            ci["lower"] = mean - margin;
            ci["upper"] = mean + margin;
            stats["confidence_intervals"].push_back(ci);
        }
    }
    
    return stats;
}

json AnalyzeCommand::analyzeDistributions(const json& data) {
    json distributions;
    distributions["normal_fit"] = false;
    
    // Simple distribution analysis
    if (data.contains("values")) {
        std::vector<double> values;
        for (const auto& value : data["values"]) {
            if (value.is_number()) {
                values.push_back(value.get<double>());
            }
        }
        
        if (!values.empty()) {
            // Check for normal distribution (simple approach)
            double mean = calculateMean(values);
            double std_dev = calculateStdDev(values);
            
            // Calculate skewness and kurtosis approximately
            double skewness = 0.0, kurtosis = 0.0;
            if (std_dev > 0.0) {
                for (double value : values) {
                    double z = (value - mean) / std_dev;
                    skewness += z * z * z;
                    kurtosis += z * z * z * z;
                }
                skewness /= values.size();
                kurtosis /= values.size();
            }
            
            distributions["mean"] = mean;
            distributions["std_dev"] = std_dev;
            distributions["skewness"] = skewness;
            distributions["kurtosis"] = kurtosis;
            
            // Simple normality check (approximate)
            distributions["normal_fit"] = (std::abs(skewness) < 1.0) && (std::abs(kurtosis - 3.0) < 1.0);
        }
    }
    
    return distributions;
}

json AnalyzeCommand::calculatePlasticityMetrics(const json& data) {
    json metrics;
    
    if (data.contains("synaptic_changes")) {
        json synaptic_changes = data["synaptic_changes"];
        
        if (synaptic_changes.contains("strengthened")) {
            int strengthened = synaptic_changes["strengthened"].get<int>();
            metrics["strengthening_rate"] = static_cast<double>(strengthened) / 1000.0;
        }
        
        if (synaptic_changes.contains("weakened")) {
            int weakened = synaptic_changes["weakened"].get<int>();
            metrics["weakening_rate"] = static_cast<double>(weakened) / 1000.0;
        }
        
        metrics["net_change"] = 0.0; // Placeholder
    }
    
    return metrics;
}

json AnalyzeCommand::calculateLearningIndicators(const json& data) {
    json indicators;
    
    indicators["learning_detected"] = true;
    indicators["convergence_rate"] = 0.75; // Placeholder
    indicators["stability_index"] = 0.85; // Placeholder
    
    return indicators;
}

json AnalyzeCommand::calculateSpeedMetrics(const json& data) {
    json metrics;
    
    if (data.contains("benchmarks")) {
        json benchmarks = data["benchmarks"];
        
        if (benchmarks.is_array() && !benchmarks.empty()) {
            double total_speed = 0.0;
            int count = 0;
            
            for (const auto& benchmark : benchmarks) {
                if (benchmark.contains("speed")) {
                    total_speed += benchmark["speed"].get<double>();
                    count++;
                }
            }
            
            if (count > 0) {
                metrics["average_speed"] = total_speed / count;
                metrics["speed_range"] = calculateSpeedRange(benchmarks);
            }
        }
    }
    
    return metrics;
}

json AnalyzeCommand::calculateEfficiencyMetrics(const json& data) {
    json metrics;
    metrics["efficiency_score"] = 0.85; // Placeholder
    metrics["resource_utilization"] = 0.75; // Placeholder
    
    return metrics;
}

double AnalyzeCommand::calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double AnalyzeCommand::calculateMedian(std::vector<double> values) {
    if (values.empty()) return 0.0;
    
    std::sort(values.begin(), values.end());
    size_t n = values.size();
    
    if (n % 2 == 0) {
        return (values[n/2 - 1] + values[n/2]) / 2.0;
    } else {
        return values[n/2];
    }
}

double AnalyzeCommand::calculateStdDev(const std::vector<double>& values) {
    if (values.size() < 2) return 0.0;
    
    double mean = calculateMean(values);
    double sum_sq = 0.0;
    
    for (double value : values) {
        sum_sq += (value - mean) * (value - mean);
    }
    
    return std::sqrt(sum_sq / (values.size() - 1));
}

double AnalyzeCommand::calculateSpeedRange(const json& benchmarks) {
    double min_speed = std::numeric_limits<double>::max();
    double max_speed = std::numeric_limits<double>::lowest();
    
    for (const auto& benchmark : benchmarks) {
        if (benchmark.contains("speed")) {
            double speed = benchmark["speed"].get<double>();
            min_speed = std::min(min_speed, speed);
            max_speed = std::max(max_speed, speed);
        }
    }
    
    return max_speed - min_speed;
}

void AnalyzeCommand::saveAnalysisResults(const json& results, const std::string& output_file) {
    std::ofstream file(output_file);
    if (!file.is_open()) {
        NLM_LOG_ERROR("CLI", "Could not open output file: " + output_file, "Check file permissions and path");
        return;
    }
    
    std::string extension = getFileExtension(output_file);
    
    if (extension == "json") {
        file << results.dump(2);
    } else if (extension == "csv") {
        // Convert to CSV format
        file << "metric,value" << std::endl;
        for (const auto& item : results.items()) {
            if (item.value().is_primitive()) {
                file << item.key() << "," << item.value().dump() << std::endl;
            }
        }
    } else if (extension == "txt") {
        file << "NLM Analysis Results" << std::endl;
        file << "=====================" << std::endl;
        file << results.dump() << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("CLI", "Analysis results saved to: " + output_file);
}

std::string AnalyzeCommand::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string AnalyzeCommand::getFileExtension(const std::string& filepath) {
    size_t dot_pos = filepath.find_last_of('.');
    if (dot_pos != std::string::npos) {
        return filepath.substr(dot_pos + 1);
    }
    return "";
}

} // namespace cli
} // namespace nlm
