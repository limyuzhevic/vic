// Regression Testing Framework
// Automated regression detection and baseline comparison for NLM

#include "core/Types/Types.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace regression {

    // Regression detection configuration
    struct RegressionConfig {
        std::string baseline_file;
        std::string current_results_file;
        std::string regression_report_file;
        double performance_threshold;  // Percentage change to trigger regression
        double stability_threshold;     // Threshold for stability metrics
        int min_samples_for_comparison;
        bool enable_auto_regression_detection;
        std::vector<std::string> regression_tests;
    };

    // Regression test result
    struct RegressionTestResult {
        std::string test_name;
        bool regression_detected;
        double baseline_value;
        double current_value;
        double percentage_change;
        std::string trend;  // "improved", "degraded", "stable"
        std::string severity;  // "critical", "warning", "info"
        std::string recommendation;
        std::string regression_details;
        std::chrono::system_clock::time_point timestamp;
    };

    // Baseline result storage
    struct BaselineResult {
        std::string test_name;
        double performance_value;
        std::string configuration;
        std::chrono::system_clock::time_point timestamp;
        std::string git_commit;
        std::string platform_info;
    };

    // Baseline collector for regression testing
    class BaselineCollector {
    public:
        BaselineCollector() = default;
        
        // Collect baseline results from file
        std::vector<BaselineResult> collect_baseline(const std::string& filename) {
            std::vector<BaselineResult> baselines;
            
            if (!std::filesystem::exists(filename)) {
                std::cerr << "Warning: Baseline file not found: " << filename << std::endl;
                return baselines;
            }
            
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error: Cannot open baseline file: " << filename << std::endl;
                return baselines;
            }
            
            nlohmann::json json_data;
            try {
                file >> json_data;
            } catch (const nlohmann::json::parse_error& e) {
                std::cerr << "Error: Failed to parse baseline file: " << e.what() << std::endl;
                return baselines;
            }
            
            for (const auto& item : json_data) {
                BaselineResult baseline;
                baseline.test_name = item["test_name"].get<std::string>();
                baseline.performance_value = item["performance_value"].get<double>();
                baseline.configuration = item.value("configuration", "default");
                baseline.timestamp = std::chrono::system_clock::time_point(
                    std::chrono::duration<double>(item["timestamp"].get<double>()));
                baseline.git_commit = item.value("git_commit", "unknown");
                baseline.platform_info = item.value("platform_info", "unknown");
                
                baselines.push_back(baseline);
            }
            
            std::cout << "Collected " << baselines.size() << " baseline results from " << filename << std::endl;
            return baselines;
        }
        
        // Save current results as baseline
        bool save_as_baseline(const std::vector<BaselineResult>& results, 
                             const std::string& filename) {
            
            nlohmann::json json_data;
            
            for (const auto& result : results) {
                nlohmann::json item;
                item["test_name"] = result.test_name;
                item["performance_value"] = result.performance_value;
                item["configuration"] = result.configuration;
                item["timestamp"] = std::chrono::duration<double>(
                    result.timestamp.time_since_epoch()).count();
                item["git_commit"] = result.git_commit;
                item["platform_info"] = result.platform_info;
                
                json_data.push_back(item);
            }
            
            std::ofstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error: Cannot write baseline file: " << filename << std::endl;
                return false;
            }
            
            file << json_data.dump(2);
            file.close();
            
            std::cout << "Baseline results saved to: " << filename << std::endl;
            return true;
        }
        
        // Collect performance metrics from benchmark results
        BaselineResult collect_performance_metric(const std::string& test_name,
                                                  double performance_value,
                                                  const std::string& configuration = "default") {
            
            BaselineResult result;
            result.test_name = test_name;
            result.performance_value = performance_value;
            result.configuration = configuration;
            result.timestamp = std::chrono::system_clock::now();
            
            // Get git commit info (simplified)
            result.git_commit = "unknown";
            
            // Get platform info
            result.platform_info = "NLM Regression Testing Framework";
            
            return result;
        }
    };

    // Regression detector for comparing baseline and current results
    class RegressionDetector {
    public:
        RegressionDetector(double performance_threshold = 5.0,
                          double stability_threshold = 0.1)
            : performance_threshold_(performance_threshold),
              stability_threshold_(stability_threshold) {}
        
        // Detect regression in a single test
        RegressionTestResult detect_regression(const BaselineResult& baseline,
                                              const BaselineResult& current) {
            
            RegressionTestResult result;
            result.test_name = current.test_name;
            result.baseline_value = baseline.performance_value;
            result.current_value = current.performance_value;
            result.timestamp = current.timestamp;
            
            // Calculate percentage change
            if (baseline.performance_value != 0.0) {
                result.percentage_change = 
                    ((current.performance_value - baseline.performance_value) / baseline.performance_value) * 100.0;
            } else {
                result.percentage_change = (current.performance_value > 0.0) ? 100.0 : 0.0;
            }
            
            // Determine trend
            if (std::abs(result.percentage_change) < performance_threshold_) {
                result.trend = "stable";
                result.severity = "info";
                result.recommendation = "No action required";
            } else if (result.percentage_change < 0) {
                result.trend = "improved";
                result.severity = "info";
                result.recommendation = "Performance improved";
            } else {
                result.trend = "degraded";
                
                // Determine severity based on percentage change
                if (result.percentage_change > 20.0) {
                    result.severity = "critical";
                    result.recommendation = "Investigate immediately - critical performance regression";
                } else if (result.percentage_change > 10.0) {
                    result.severity = "warning";
                    result.recommendation = "Investigate performance degradation";
                } else {
                    result.severity = "warning";
                    result.recommendation = "Monitor performance changes";
                }
            }
            
            result.regression_detected = (result.trend == "degraded");
            
            // Generate regression details
            std::stringstream ss;
            ss << "Baseline: " << baseline.performance_value << " -> Current: " << current.performance_value;
            ss << " (Change: " << std::fixed << std::setprecision(2) << result.percentage_change << "%)";
            result.regression_details = ss.str();
            
            return result;
        }
        
        // Detect regression across multiple tests
        std::vector<RegressionTestResult> detect_regression(
            const std::vector<BaselineResult>& baseline,
            const std::vector<BaselineResult>& current) {
            
            std::vector<RegressionTestResult> results;
            
            for (const auto& curr : current) {
                // Find matching baseline
                auto it = std::find_if(baseline.begin(), baseline.end(),
                    [&curr](const BaselineResult& base) {
                        return base.test_name == curr.test_name &&
                               base.configuration == curr.configuration;
                    });n                
                if (it != baseline.end()) {
                    results.push_back(detect_regression(*it, curr));
                } else {
                    // No baseline found - treat as new test
                    RegressionTestResult result;
                    result.test_name = curr.test_name;
                    result.regression_detected = false;
                    result.baseline_value = 0.0;
                    result.current_value = curr.performance_value;
                    result.percentage_change = 100.0;  // New test
                    result.trend = "new";
                    result.severity = "info";
                    result.recommendation = "New test - establish baseline";
                    result.regression_details = "No baseline available - new test";
                    result.timestamp = curr.timestamp;
                    
                    results.push_back(result);
                }
            }
            
            return results;
        }
        
        // Get regression summary
        struct RegressionSummary {
            int total_tests;
            int regression_detected;
            int warnings;
            int critical_issues;
            int improved;
            int stable;
            double average_change_percentage;
        };
        
        RegressionSummary get_regression_summary(const std::vector<RegressionTestResult>& results) {
            RegressionSummary summary;
            summary.total_tests = results.size();
            summary.regression_detected = 0;
            summary.warnings = 0;
            summary.critical_issues = 0;
            summary.improved = 0;
            summary.stable = 0;
            summary.average_change_percentage = 0.0;
            
            double total_change = 0.0;
            int count_for_average = 0;
            
            for (const auto& result : results) {
                if (result.regression_detected) {
                    summary.regression_detected++;
                }
                
                if (result.severity == "critical") {
                    summary.critical_issues++;
                } else if (result.severity == "warning") {
                    summary.warnings++;
                }
                
                if (result.trend == "improved") {
                    summary.improved++;
                } else if (result.trend == "stable") {
                    summary.stable++;
                }
                
                if (result.trend != "new") {
                    total_change += std::abs(result.percentage_change);
                    count_for_average++;
                }
            }
            
            if (count_for_average > 0) {
                summary.average_change_percentage = total_change / count_for_average;
            }
            
            return summary;
        }
        
        // Generate regression report
        void generate_regression_report(const std::vector<RegressionTestResult>& results,
                                       const std::string& filename) {
            
            std::ofstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error: Cannot open report file: " << filename << std::endl;
                return;
            }
            
            file << "<!DOCTYPE html>" << std::endl;
            file << "<html><head><title>NLM Regression Test Report</title>" << std::endl;
            file << "<style>body { font-family: Arial, sans-serif; margin: 20px; } "
                 << "table { border-collapse: collapse; width: 100%; margin-bottom: 20px; } "
                 << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; } "
                 << "th { background-color: #f2f2f2; } "
                 << ".critical { background-color: #ffcccc; } "
                 << ".warning { background-color: #ffffcc; } "
                 << ".improved { background-color: #ccffcc; } "
                 << ".stable { background-color: #e6e6e6; } "
                 << ".new { background-color: #cce5ff; }" << std::endl;
            file << "</style></head><body>" << std::endl;
            
            file << "<h1>NLM Regression Test Report</h1>" << std::endl;
            file << "<p>Generated: " << std::chrono::system_clock::now() << "</p>" << std::endl;
            
            // Summary section
            RegressionSummary summary = get_regression_summary(results);
            
            file << "<h2>Summary</h2>" << std::endl;
            file << "<table>" << std::endl;
            file << "<tr><th>Metric</th><th>Count</th></tr>" << std::endl;
            file << "<tr><td>Total Tests</td><td>" << summary.total_tests << "</td></tr>" << std::endl;
            file << "<tr><td>Regressions Detected</td><td>" << summary.regression_detected << "</td></tr>" << std::endl;
            file << "<tr><td>Critical Issues</td><td>" << summary.critical_issues << "</td></tr>" << std::endl;
            file << "<tr><td>Warnings</td><td>" << summary.warnings << "</td></tr>" << std::endl;
            file << "<tr><td>Improvements</td><td>" << summary.improved << "</td></tr>" << std::endl;
            file << "<tr><td>Stable</td><td>" << summary.stable << "</td></tr>" << std::endl;
            file << "<tr><td>Average Change (%)</td><td>" << std::fixed << std::setprecision(2)
                 << summary.average_change_percentage << "</td></tr>" << std::endl;
            file << "</table>" << std::endl;
            
            // Detailed results section
            file << "<h2>Detailed Results</h2>" << std::endl;
            file << "<table>" << std::endl;
            file << "<tr><th>Test Name</th><th>Baseline</th><th>Current</th><th>Change (%)</th><th>Trend</th><th>Severity</th><th>Recommendation</th><th>Details</th></tr>" << std::endl;
            
            for (const auto& result : results) {
                file << "<tr class=\"" << result.severity << "\">" << std::endl;
                file << "<td>" << result.test_name << "</td>" << std::endl;
                file << "<td>" << std::fixed << std::setprecision(6) << result.baseline_value << "</td>" << std::endl;
                file << "<td>" << std::fixed << std::setprecision(6) << result.current_value << "</td>" << std::endl;
                file << "<td class=\"" << (result.percentage_change < 0 ? "improved" : "degraded") 
                     << "\">" << std::fixed << std::setprecision(2) << result.percentage_change << "%</td>" << std::endl;
                file << "<td>" << result.trend << "</td>" << std::endl;
                file << "<td>" << result.severity << "</td>" << std::endl;
                file << "<td>" << result.recommendation << "</td>" << std::endl;
                file << "<td>" << result.regression_details << "</td>" << std::endl;
                file << "</tr>" << std::endl;
            }
            
            file << "</table></body></html>" << std::endl;
            file.close();
            
            std::cout << "Regression report exported to: " << filename << std::endl;
        }
        
    private:
        double performance_threshold_;
        double stability_threshold_;
    };

    // Regression test analyzer for stability and performance metrics
    class RegressionAnalyzer {
    public:
        struct StabilityMetrics {
            double mean_performance;
            double std_deviation;
            double coefficient_of_variation;
            bool is_stable;
            double stability_score;
        };
        
        // Analyze stability of a test over time
        StabilityMetrics analyze_stability(const std::vector<BaselineResult>& results) {
            StabilityMetrics metrics;
            
            if (results.empty()) {
                metrics.is_stable = false;
                metrics.stability_score = 0.0;
                return metrics;
            }
            
            // Calculate basic statistics
            metrics.mean_performance = 0.0;
            double sum_squares = 0.0;
            
            for (const auto& result : results) {
                metrics.mean_performance += result.performance_value;
            }
            metrics.mean_performance /= results.size();
            
            for (const auto& result : results) {
                double diff = result.performance_value - metrics.mean_performance;
                sum_squares += diff * diff;
            }
            
            metrics.std_deviation = std::sqrt(sum_squares / results.size());
            
            // Calculate coefficient of variation
            if (metrics.mean_performance != 0.0) {
                metrics.coefficient_of_variation = metrics.std_deviation / metrics.mean_performance;
            } else {
                metrics.coefficient_of_variation = std::numeric_limits<double>::max();
            }
            
            // Determine stability
            metrics.is_stable = (metrics.coefficient_of_variation < stability_threshold_);
            
            // Calculate stability score (0-1, higher is more stable)
            if (metrics.coefficient_of_variation < stability_threshold_) {
                metrics.stability_score = 1.0 - (metrics.coefficient_of_variation / stability_threshold_);
            } else {
                metrics.stability_score = std::max(0.0, 1.0 - (metrics.coefficient_of_variation / 2.0));
            }
            
            return metrics;
        }
        
        // Compare two sets of results for stability
        bool compare_stability(const std::vector<BaselineResult>& baseline_results,
                              const std::vector<BaselineResult>& current_results) {
            
            if (baseline_results.size() < 2 || current_results.size() < 2) {
                return false;  // Not enough data for stability analysis
            }
            
            StabilityMetrics baseline_stability = analyze_stability(baseline_results);
            StabilityMetrics current_stability = analyze_stability(current_results);
            
            // Check if stability has improved or degraded
            return current_stability.is_stable && !baseline_stability.is_stable;
        }
        
        // Detect performance trends
        std::string detect_performance_trend(const std::vector<BaselineResult>& results) {
            if (results.size() < 2) {
                return "insufficient_data";
            }
            
            // Simple linear regression to detect trend
            double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_x2 = 0.0;
            int n = results.size();
            
            for (int i = 0; i < n; ++i) {
                double x = static_cast<double>(i);  // Time index
                double y = results[i].performance_value;
                
                sum_x += x;
                sum_y += y;
                sum_xy += x * y;
                sum_x2 += x * x;
            }
            
            double slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
            
            if (std::abs(slope) < 0.001) {
                return "stable";
            } else if (slope > 0) {
                return "improving";
            } else {
                return "degrading";
            }
        }
    };

    // Main regression testing framework
    class RegressionTestingFramework {
    public:
        RegressionTestingFramework(const RegressionConfig& config)
            : config_(config) {}
        
        // Run regression tests
        void run_regression_tests(const std::vector<BaselineResult>& baseline_results,
                                 const std::vector<BaselineResult>& current_results) {
            
            std::cout << "=== NLM Regression Testing ===" << std::endl;
            std::cout << "Running regression detection and analysis..." << std::endl;
            std::cout << std::endl;
            
            // Detect regression
            RegressionDetector detector(config_.performance_threshold, config_.stability_threshold);
            std::vector<RegressionTestResult> regression_results = 
                detector.detect_regression(baseline_results, current_results);
            
            // Generate report
            detector.generate_regression_report(regression_results, config_.regression_report_file);
            
            // Print summary
            RegressionSummary summary = detector.get_regression_summary(regression_results);
            print_regression_summary(summary);
            
            // Save regression results
            save_regression_results(regression_results);
            
            std::cout << std::endl;
            std::cout << "=== Regression Testing Complete ===" << std::endl;
        }
        
        // Analyze baseline stability
        void analyze_baseline_stability(const std::vector<BaselineResult>& baseline_results) {
            std::cout << "=== Baseline Stability Analysis ===" << std::endl;
            
            RegressionAnalyzer analyzer;
            
            for (const auto& test_name : config_.regression_tests) {
                std::vector<BaselineResult> test_results;
                for (const auto& result : baseline_results) {
                    if (result.test_name == test_name) {
                        test_results.push_back(result);
                    }
                }
                
                if (test_results.size() >= config_.min_samples_for_comparison) {
                    StabilityMetrics stability = analyzer.analyze_stability(test_results);
                    std::string trend = analyzer.detect_performance_trend(test_results);
                    
                    std::cout << test_name << ": " << std::endl;
                    std::cout << "  Mean: " << std::fixed << std::setprecision(6) 
                              << stability.mean_performance << std::endl;
                    std::cout << "  Std Dev: " << std::fixed << std::setprecision(6) 
                              << stability.std_deviation << std::endl;
                    std::cout << "  Coefficient of Variation: " << std::fixed << std::setprecision(6)
                              << stability.coefficient_of_variation << std::endl;
                    std::cout << "  Stability: " << (stability.is_stable ? "Stable" : "Unstable") << std::endl;
                    std::cout << "  Stability Score: " << std::fixed << std::setprecision(4)
                              << stability.stability_score << std::endl;
                    std::cout << "  Trend: " << trend << std::endl;
                    std::cout << std::endl;
                }
            }
        }
        
    private:
        void print_regression_summary(const RegressionSummary& summary) {
            std::cout << "=== Regression Testing Summary ===" << std::endl;
            std::cout << "Total Tests: " << summary.total_tests << std::endl;
            std::cout << "Regressions Detected: " << summary.regression_detected << std::endl;
            std::cout << "Critical Issues: " << summary.critical_issues << std::endl;
            std::cout << "Warnings: " << summary.warnings << std::endl;
            std::cout << "Improvements: " << summary.improved << std::endl;
            std::cout << "Stable: " << summary.stable << std::endl;
            std::cout << "Average Change (%): " << std::fixed << std::setprecision(2)
                      << summary.average_change_percentage << std::endl;
            std::cout << std::endl;
            
            if (summary.regression_detected > 0) {
                std::cout << "WARNING: Regression detected! Please review the regression report." << std::endl;
            }
        }
        
        void save_regression_results(const std::vector<RegressionTestResult>& results) {
            // Save regression results to file for future analysis
            std::string filename = "regression_results_" + 
                std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".json";
            
            nlohmann::json json_data;
            
            for (const auto& result : results) {
                nlohmann::json item;
                item["test_name"] = result.test_name;
                item["regression_detected"] = result.regression_detected;
                item["baseline_value"] = result.baseline_value;
                item["current_value"] = result.current_value;
                item["percentage_change"] = result.percentage_change;
                item["trend"] = result.trend;
                item["severity"] = result.severity;
                item["recommendation"] = result.recommendation;
                item["regression_details"] = result.regression_details;
                item["timestamp"] = std::chrono::duration<double>(
                    result.timestamp.time_since_epoch()).count();
                
                json_data.push_back(item);
            }
            
            std::ofstream file(filename);
            if (file.is_open()) {
                file << json_data.dump(2);
                file.close();
                std::cout << "Regression results saved to: " << filename << std::endl;
            } else {
                std::cerr << "Error: Cannot save regression results" << std::endl;
            }
        }
        
        RegressionConfig config_;
    };

} // namespace regression

// Entry point for regression testing
void run_regression_tests(const std::string& baseline_file,
                         const std::string& current_results_file,
                         const std::string& report_file) {
    
    // Configuration
    regression::RegressionConfig config;
    config.baseline_file = baseline_file;
    config.current_results_file = current_results_file;
    config.regression_report_file = report_file;
    config.performance_threshold = 5.0;  // 5% threshold
    config.stability_threshold = 0.1;   // 10% coefficient of variation
    config.min_samples_for_comparison = 5;
    config.enable_auto_regression_detection = true;
    
    // Collect baseline results
    regression::BaselineCollector collector;
    std::vector<regression::BaselineResult> baseline_results = collector.collect_baseline(baseline_file);
    
    // Collect current results
    std::vector<regression::BaselineResult> current_results = collector.collect_baseline(current_results_file);
    
    if (baseline_results.empty() || current_results.empty()) {
        std::cerr << "Error: No baseline or current results available" << std::endl;
        return;
    }
    
    // Run regression testing
    regression::RegressionTestingFramework framework(config);
    framework.run_regression_tests(baseline_results, current_results);
    
    // Analyze baseline stability
    framework.analyze_baseline_stability(baseline_results);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <baseline_file> <current_results_file> <report_file>" << std::endl;
        std::cerr << "  baseline_file: File containing baseline performance results (JSON)" << std::endl;
        std::cerr << "  current_results_file: File containing current performance results (JSON)" << std::endl;
        std::cerr << "  report_file: Output file for regression report (HTML)" << std::endl;
        return 1;
    }
    
    try {
        run_regression_tests(argv[1], argv[2], argv[3]);
        std::cout << std::endl;
        std::cout << "=== Regression Testing Complete ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error during regression testing: " << e.what() << std::endl;
        return 1;
    }
}