#include "VisualizationReporter.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>
#include <nlohmann/json.hpp>

namespace nlm {

// Helper functions for visualization
namespace VisualizationHelpers {
    std::string escapeHTML(const std::string& text) {
        std::ostringstream oss;
        for (char c : text) {
            switch (c) {
                case '&': oss << "&amp;"; break;
                case '<': oss << "&lt;"; break;
                case '>': oss << "&gt;"; break;
                case '"': oss << "&quot;"; break;
                case '\'': oss << "&#39;"; break;
                default: oss << c; break;
            }
        }
        return oss.str();
    }
    
    std::string escapeLaTeX(const std::string& text) {
        std::ostringstream oss;
        for (char c : text) {
            switch (c) {
                case '%': oss << "\%"; break;
                case '&': oss << "\&"; break;
                case '_': oss << "\_"; break;
                case '#': oss << "\#"; break;
                case '$': oss << "\$"; break;
                case '{': oss << "\{"; break;
                case '}': oss << "\}"; break;
                case '~': oss << "\textasciitilde{}'; break;
                case '^': oss << "\^{}"; break;
                default: oss << c; break;
            }
        }
        return oss.str();
    }
    
    std::string escapeMarkdown(const std::string& text) {
        std::ostringstream oss;
        for (char c : text) {
            switch (c) {
                case '*': oss << "\\*"; break;
                case '_': oss << "\\_"; break;
                case '`': oss << "\\`"; break;
                case '[': oss << "\\["; break;
                case ']': oss << "\\]"; break;
                case '(' : oss << "\\("; break;
                case ')': oss << "\\)"; break;
                case '#': oss << "\\#"; break;
                case '+': oss << "\\+"; break;
                case '-': oss << "\\-"; break;
                case '.': oss << "\\."; break;
                case '!': oss << "\\!"; break;
                default: oss << c; break;
            }
        }
        return oss.str();
    }
    
    std::string generateUUID() {
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937 gen(static_cast<unsigned int>(seed));
        std::uniform_int_distribution<int> dis(0, 15);
        std::uniform_int_distribution<int> dis2(8, 11);
        
        std::stringstream ss;
        int count = 0;
        bool isHyphen = false;
        for (int i = 0; i < 36; ++i) {
            if (count == 8 || count == 13 || count == 18 || count == 23) {
                ss << "-";
                isHyphen = true;
            } else if (count == 14) {
                ss << "4";
                isHyphen = false;
            } else if (count == 19) {
                ss << "8";
                isHyphen = false;
            } else {
                int digit = dis(gen);
                ss << std::hex << digit;
                isHyphen = false;
            }
            ++count;
        }
        return ss.str();
    }
    
    std::string generateTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string formatNumber(double value, int precision) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        return oss.str();
    }
    
    std::vector<std::string> getColorPalette(size_t count) {
        // Common scientific visualization color palette
        const std::vector<std::string> defaultPalette = {
            "#1f77b4",  // Blue
            "#ff7f0e",  // Orange
            "#2ca02c",  // Green
            "#d62728",  // Red
            "#9467bd",  // Purple
            "#8c564b",  // Brown
            "#e377c2",  // Pink
            "#7f7f7f",  // Gray
            "#bcbd22",  // Lime
            "#17becf"   // Cyan
        };
        
        if (count <= defaultPalette.size()) {
            return std::vector<std::string>(defaultPalette.begin(), defaultPalette.begin() + count);
        }
        
        // Generate additional colors by interpolating
        std::vector<std::string> extendedPalette;
        extendedPalette.reserve(count);
        
        for (size_t i = 0; i < count; ++i) {
            double hue = static_cast<double>(i) / static_cast<double>(count) * 360.0;
            // Convert HSL to RGB
            double c = 1.0;
            double x = c * (1.0 - std::abs(std::fmod(hue / 60.0, 2.0) - 1.0));
            double m = 0.0;
            
            double r = 0.0, g = 0.0, b = 0.0;
            if (hue >= 0 && hue < 60) {
                r = c; g = x; b = 0;
            } else if (hue >= 60 && hue < 120) {
                r = x; g = c; b = 0;
            } else if (hue >= 120 && hue < 180) {
                r = 0; g = c; b = x;
            } else if (hue >= 180 && hue < 240) {
                r = 0; g = x; b = c;
            } else if (hue >= 240 && hue < 300) {
                r = x; g = 0; b = c;
            } else if (hue >= 300 && hue < 360) {
                r = c; g = 0; b = x;
            }
            
            // Convert to hex
            std::ostringstream oss;
            oss << "#";
            oss << std::hex << std::uppercase;
            oss << std::setw(2) << static_cast<int>((r + m) * 255);
            oss << std::setw(2) << static_cast<int>((g + m) * 255);
            oss << std::setw(2) << static_cast<int>((b + m) * 255);
            
            extendedPalette.push_back(oss.str());
        }
        
        return extendedPalette;
    }
    
    DataStatistics calculateStatistics(const std::vector<double>& data) {
        DataStatistics stats;
        
        if (data.empty()) {
            return stats;
        }
        
        stats.mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
        stats.min = *std::min_element(data.begin(), data.end());
        stats.max = *std::max_element(data.begin(), data.end());
        
        // Calculate standard deviation and variance
        double sqSum = 0.0;
        for (double value : data) {
            double diff = value - stats.mean;
            sqSum += diff * diff;
        }
        stats.variance = sqSum / data.size();
        stats.stddev = std::sqrt(stats.variance);
        
        // Calculate median
        std::vector<double> sortedData = data;
        std::sort(sortedData.begin(), sortedData.end());
        size_t n = sortedData.size();
        if (n % 2 == 0) {
            stats.median = (sortedData[n/2 - 1] + sortedData[n/2]) / 2.0;
        } else {
            stats.median = sortedData[n/2];
        }
        
        // Calculate quartiles
        if (n >= 4) {
            stats.q1 = sortedData[n/4];
            stats.q3 = sortedData[3*n/4];
        }
        
        return stats;
    }
    
    bool detectOutlier(const double& value, const DataStatistics& stats, const std::string& method) {
        if (method == "iqr") {
            double iqr = stats.q3 - stats.q1;
            return (value < stats.q1 - 1.5 * iqr) || (value > stats.q3 + 1.5 * iqr);
        } else if (method == "zscore") {
            return std::abs(value - stats.mean) > 3.0 * stats.stddev;
        } else if (method == "median_absolute_dev") {
            double mad = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
            double modifiedZ = (value - stats.median) / (0.6745 * mad);
            return std::abs(modifiedZ) > 3.5;
        }
        return false;
    }
}

// Report generation implementations
std::string AdvancedVisualizationReporter::generateExperimentReport(
    const std::vector<ExperimentResult>& results,
    const std::string& title,
    ReportFormat format) {
    
    std::ostringstream report;
    
    if (format == ReportFormat::Text) {
        report << "=== " << title << " ===\n\n";
        report << "Generated: " << VisualizationHelpers::generateTimestamp() << "\n\n";
        
        if (results.empty()) {
            report << "No experiment results to report.\n";
            return report.str();
        }
        
        report << "Summary Statistics:\n";
        report << "  Total Experiments: " << results.size() << "\n";
        
        size_t successful = 0;
        for (const auto& result : results) {
            if (result.success) successful++;
        }
        
        report << "  Successful Experiments: " << successful << "\n";
        report << "  Success Rate: " << std::fixed << std::setprecision(2)
               << (results.size() > 0 ? (static_cast<double>(successful) / results.size() * 100.0) : 0.0) << "%\n\n";
        
        // Generate experiment details
        report << "Experiment Details:\n";
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            report << "  Experiment " << i+1 << ":\n";
            report << "    Name: " << (result.experiment ? result.experiment->getName() : "Unknown") << "\n";
            report << "    Success: " << (result.success ? "Yes" : "No") << "\n";
            report << "    Execution Time: " << result.executionTime << " ms\n";
            if (!result.errorMessage.empty()) {
                report << "    Error: " << result.errorMessage << "\n";
            }
            report << "\n";
        }
        
    } else if (format == ReportFormat::HTML) {
        report << "<!DOCTYPE html>\n";
        report << "<html>\n";
        report << "<head><title>" << VisualizationHelpers::escapeHTML(title) << "</title>";
        report << "<style>";
        report << "body { font-family: Arial, sans-serif; margin: 40px; }";
        report << "h1 { color: #333; }";
        report << "table { border-collapse: collapse; width: 100%; }";
        report << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }";
        report << "th { background-color: #f2f2f2; }";
        report << ".success { background-color: #d4edda; }";
        report << ".failure { background-color: #f8d7da; }";
        report << "</style></head>";
        report << "<body><h1>" << VisualizationHelpers::escapeHTML(title) << "</h1>";
        report << "<p>Generated: " << VisualizationHelpers::generateTimestamp() << "</p>";
        
        if (results.empty()) {
            report << "<p>No experiment results to report.</p>";
        } else {
            report << "<table><tr><th>#</th><th>Name</th><th>Success</th><th>Execution Time (ms)</th><th>Error Message</th></tr>";
            for (size_t i = 0; i < results.size(); ++i) {
                const auto& result = results[i];
                std::string rowClass = result.success ? "success" : "failure";
                report << "<tr class=\"" << rowClass << "\">";
                report << "<td>" << i+1 << "</td>";
                report << "<td>" << VisualizationHelpers::escapeHTML(result.experiment ? result.experiment->getName() : "Unknown") << "</td>";
                report << "<td>" << (result.success ? "Yes" : "No") << "</td>";
                report << "<td>" << result.executionTime << "</td>";
                report << "<td>" << VisualizationHelpers::escapeHTML(result.errorMessage) << "</td>";
                report << "</tr>";
            }
            report << "</table>";
        }
        
        report << "</body></html>";
        
    } else if (format == ReportFormat::JSON) {
        nlohmann::json json;
        json["title"] = title;
        json["generated_at"] = VisualizationHelpers::generateTimestamp();
        json["experiment_count"] = results.size();
        
        json["summary"] = {
            {"total_experiments", results.size()},
            {"successful_experiments", std::count_if(results.begin(), results.end(),
                [](const ExperimentResult& r) { return r.success; })},
            {"success_rate", results.size() > 0 ?
                (static_cast<double>(std::count_if(results.begin(), results.end(),
                    [](const ExperimentResult& r) { return r.success; })) / results.size()) : 0.0}
        };
        
        json["experiments"] = nlohmann::json::array();
        for (const auto& result : results) {
            nlohmann::json experiment;
            experiment["name"] = result.experiment ? result.experiment->getName() : "Unknown";
            experiment["success"] = result.success;
            experiment["execution_time_ms"] = result.executionTime;
            experiment["error_message"] = result.errorMessage;
            json["experiments"].push_back(experiment);
        }
        
        report << json.dump(2);
        
    } else if (format == ReportFormat::LaTeX) {
        report << "\\documentclass{article}\n";
        report << "\\title{" << VisualizationHelpers::escapeLaTeX(title) << "}\n";
        report << "\\author{NLM Brain Research Team}\n";
        report << "\\date{\today}\n";
        report << "\\begin{document}\n\n";
        report << "\\maketitle\n\n";
        report << "\\section{Experiment Summary}\n";
        report << "\\begin{itemize}\n";
        report << "\\item Total Experiments: " << results.size() << "\n";
        
        size_t successful = std::count_if(results.begin(), results.end(),
            [](const ExperimentResult& r) { return r.success; });
        
        report << "\\item Successful Experiments: " << successful << "\n";
        report << "\\item Success Rate: " << std::fixed << std::setprecision(2)
               << (results.size() > 0 ? (static_cast<double>(successful) / results.size() * 100.0) : 0.0) << "%\n";
        report << "\\end{itemize}\n\n";
        
        report << "\\section{Experiment Details}\n";
        report << "\\begin{tabular}{lllll}\n";
        report << "\\textbf{Index} & \\textbf{Name} & \\textbf{Success} & \\textbf{Execution Time (ms)} & \\textbf{Error Message} \\\\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            report << i+1 << " & "
                   << VisualizationHelpers::escapeLaTeX(result.experiment ? result.experiment->getName() : "Unknown") << " & "
                   << (result.success ? "Yes" : "No") << " & "
                   << result.executionTime << " & "
                   << VisualizationHelpers::escapeLaTeX(result.errorMessage) << " \\\"\n";
        }
        
        report << "\\end{tabular}\n\n";
        report << "\\end{document}\n";
        
    } else if (format == ReportFormat::Markdown) {
        report << "# " << title << "\n\n";
        report << "Generated: " << VisualizationHelpers::generateTimestamp() << "\n\n";
        
        if (results.empty()) {
            report << "No experiment results to report.\n";
            return report.str();
        }
        
        report << "## Summary Statistics\n\n";
        report << "- Total Experiments: " << results.size() << "\n";
        
        size_t successful = std::count_if(results.begin(), results.end(),
            [](const ExperimentResult& r) { return r.success; });
        
        report << "- Successful Experiments: " << successful << "\n";
        report << "- Success Rate: " << std::fixed << std::setprecision(2)
               << (results.size() > 0 ? (static_cast<double>(successful) / results.size() * 100.0) : 0.0) << "%\n\n";
        
        report << "## Experiment Details\n\n";
        report << "| # | Name | Success | Execution Time (ms) | Error Message |\n";
        report << "|---|---|---|---|---|\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            report << "| " << i+1 << " | "
                   << VisualizationHelpers::escapeMarkdown(result.experiment ? result.experiment->getName() : "Unknown") << " | "
                   << (result.success ? "Yes" : "No") << " | "
                   << result.executionTime << " | "
                   << VisualizationHelpers::escapeMarkdown(result.errorMessage) << " |\n";
        }
        
    } else if (format == ReportFormat::CSV) {
        report << "Index,Name,Success,Execution Time (ms),Error Message\n";
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            report << i+1 << ",";
            report << "\"" << VisualizationHelpers::escapeHTML(result.experiment ? result.experiment->getName() : "Unknown") << "\",";
            report << (result.success ? "Yes" : "No") << ",";
            report << result.executionTime << ",";
            report << "\"" << VisualizationHelpers::escapeHTML(result.errorMessage) << "\"";
            if (i < results.size() - 1) report << "\n";
        }
        
    } else if (format == ReportFormat::HTML) {
        // Already handled above, this is a duplicate for safety
        report << "<!DOCTYPE html>\n";
        report << "<html><head><title>" << VisualizationHelpers::escapeHTML(title) << "</title></head>";
        report << "<body><h1>Report generation not implemented</h1></body></html>";
    }
    
    return report.str();
}

std::string AdvancedVisualizationReporter::generateStatisticalReport(
    const std::vector<ExperimentResult>& results,
    const std::string& title,
    ReportFormat format) {
    
    std::ostringstream report;
    
    // Analyze results
    ExperimentAnalysis analysis = analyzeExperimentResults(results);
    
    if (format == ReportFormat::Text) {
        report << "=== " << title << " ===\n\n";
        report << "Generated: " << VisualizationHelpers::generateTimestamp() << "\n\n";
        
        report << "Statistical Analysis:\n";
        report << "  Average Performance: " << VisualizationHelpers::formatNumber(analysis.averagePerformance) << "\n";
        report << "  Performance Standard Deviation: " << VisualizationHelpers::formatNumber(analysis.performanceStdDev) << "\n";
        report << "  Performance Variance: " << VisualizationHelpers::formatNumber(analysis.performanceVariance) << "\n";
        report << "  Coefficient of Variation: " << VisualizationHelpers::formatNumber(analysis.coefficientOfVariation) << "\n";
        report << "  Minimum Performance: " << VisualizationHelpers::formatNumber(analysis.minPerformance) << "\n";
        report << "  Maximum Performance: " << VisualizationHelpers::formatNumber(analysis.maxPerformance) << "\n";
        report << "  Median Performance: " << VisualizationHelpers::formatNumber(analysis.medianPerformance) << "\n";
        report << "  25th Percentile: " << VisualizationHelpers::formatNumber(analysis.percentile25) << "\n";
        report << "  75th Percentile: " << VisualizationHelpers::formatNumber(analysis.percentile75) << "\n";
        report << "  95th Percentile: " << VisualizationHelpers::formatNumber(analysis.percentile95) << "\n";
        report << "  Success Count: " << analysis.successCount << "\n";
        report << "  Failure Count: " << analysis.failureCount << "\n";
        report << "  Success Rate: " << std::fixed << std::setprecision(2)
               << analysis.successRate << "%\n\n";
        
        report << "Metric Statistics:\n";
        for (const auto& pair : analysis.metricStatistics) {
            report << "  " << pair.first << ": " << VisualizationHelpers::formatNumber(pair.second) << "\n";
        }
        
    } else if (format == ReportFormat::HTML) {
        report << "<!DOCTYPE html>\n";
        report << "<html>\n";
        report << "<head><title>" << VisualizationHelpers::escapeHTML(title) << "</title>";
        report << "<style>";
        report << "body { font-family: Arial, sans-serif; margin: 40px; }";
        report << "h1 { color: #333; }";
        report << "table { border-collapse: collapse; width: 100%; margin-bottom: 20px; }";
        report << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }";
        report << "th { background-color: #f2f2f2; }";
        report << ".metric { background-color: #f9f9f9; }";
        report << "</style></head>";
        report << "<body><h1>" << VisualizationHelpers::escapeHTML(title) << "</h1>";
        report << "<p>Generated: " << VisualizationHelpers::generateTimestamp() << "</p>";
        
        report << "<h2>Statistical Summary</h2>\n";
        report << "<table><tr><th>Statistic</th><th>Value</th></tr>";
        report << "<tr><td>Average Performance</td><td>" << VisualizationHelpers::formatNumber(analysis.averagePerformance) << "</td></tr>";
        report << "<tr><td>Performance Standard Deviation</td><td>" << VisualizationHelpers::formatNumber(analysis.performanceStdDev) << "</td></tr>";
        report << "<tr><td>Performance Variance</td><td>" << VisualizationHelpers::formatNumber(analysis.performanceVariance) << "</td></tr>";
        report << "<tr><td>Coefficient of Variation</td><td>" << VisualizationHelpers::formatNumber(analysis.coefficientOfVariation) << "</td></tr>";
        report << "<tr><td>Minimum Performance</td><td>" << VisualizationHelpers::formatNumber(analysis.minPerformance) << "</td></tr>";
        report << "<tr><td>Maximum Performance</td><td>" << VisualizationHelpers::formatNumber(analysis.maxPerformance) << "</td></tr>";
        report << "<tr><td>Median Performance</td><td>" << VisualizationHelpers::formatNumber(analysis.medianPerformance) << "</td></tr>";
        report << "<tr><td>25th Percentile</td><td>" << VisualizationHelpers::formatNumber(analysis.percentile25) << "</td></tr>";
        report << "<tr><td>75th Percentile</td><td>" << VisualizationHelpers::formatNumber(analysis.percentile75) << "</td></tr>";
        report << "<tr><td>95th Percentile</td><td>" << VisualizationHelpers::formatNumber(analysis.percentile95) << "</td></tr>";
        report << "<tr><td>Success Count</td><td>" << analysis.successCount << "</td></tr>";
        report << "<tr><td>Failure Count</td><td>" << analysis.failureCount << "</td></tr>";
        report << "<tr><td>Success Rate</td><td>" << std::fixed << std::setprecision(2) << analysis.successRate << "%</td></tr>";
        report << "</table>\n";
        
        report << "<h2>Metric Statistics</h2>\n";
        report << "<table><tr><th>Metric</th><th>Value</th></tr>";
        for (const auto& pair : analysis.metricStatistics) {
            report << "<tr class=\"metric\"><td>" << VisualizationHelpers::escapeHTML(pair.first) << "</td><td>" << VisualizationHelpers::formatNumber(pair.second) << "</td></tr>";
        }
        report << "</table>\n";
        
        report << "</body></html>";
        
    } else if (format == ReportFormat::JSON) {
        nlohmann::json json;
        json["title"] = title;
        json["generated_at"] = VisualizationHelpers::generateTimestamp();
        json["experiment_count"] = analysis.successCount + analysis.failureCount;
        
        json["statistical_summary"] = {
            {"average_performance", analysis.averagePerformance},
            {"performance_std_dev", analysis.performanceStdDev},
            {"performance_variance", analysis.performanceVariance},
            {"coefficient_of_variation", analysis.coefficientOfVariation},
            {"min_performance", analysis.minPerformance},
            {"max_performance", analysis.maxPerformance},
            {"median_performance", analysis.medianPerformance},
            {"percentile_25", analysis.percentile25},
            {"percentile_75", analysis.percentile75},
            {"percentile_95", analysis.percentile95},
            {"success_count", analysis.successCount},
            {"failure_count", analysis.failureCount},
            {"success_rate", analysis.successRate}
        };
        
        json["metric_statistics"] = analysis.metricStatistics;
        
        report << json.dump(2);
        
    } else if (format == ReportFormat::LaTeX) {
        report << "\\documentclass{article}\n";
        report << "\\title{" << VisualizationHelpers::escapeLaTeX(title) << "}\n";
        report << "\\author{NLM Brain Research Team}\n";
        report << "\\date{\today}\n";
        report << "\\begin{document}\n\n";
        report << "\\maketitle\n\n";
        report << "\\section{Statistical Summary}\n";
        report << "\\begin{tabular}{ll}\n";
        report << "\\textbf{Statistic} & \\textbf{Value} \\\"\n";
        report << "Average Performance & " << VisualizationHelpers::formatNumber(analysis.averagePerformance) << " \\\"\n";
        report << "Performance Standard Deviation & " << VisualizationHelpers::formatNumber(analysis.performanceStdDev) << " \\\"\n";
        report << "Performance Variance & " << VisualizationHelpers::formatNumber(analysis.performanceVariance) << " \\\"\n";
        report << "Coefficient of Variation & " << VisualizationHelpers::formatNumber(analysis.coefficientOfVariation) << " \\\"\n";
        report << "Minimum Performance & " << VisualizationHelpers::formatNumber(analysis.minPerformance) << " \\\"\n";
        report << "Maximum Performance & " << VisualizationHelpers::formatNumber(analysis.maxPerformance) << " \\\"\n";
        report << "Median Performance & " << VisualizationHelpers::formatNumber(analysis.medianPerformance) << " \\\"\n";
        report << "25th Percentile & " << VisualizationHelpers::formatNumber(analysis.percentile25) << " \\\"\n";
        report << "75th Percentile & " << VisualizationHelpers::formatNumber(analysis.percentile75) << " \\\"\n";
        report << "95th Percentile & " << VisualizationHelpers::formatNumber(analysis.percentile95) << " \\\"\n";
        report << "Success Count & " << analysis.successCount << " \\\"\n";
        report << "Failure Count & " << analysis.failureCount << " \\\"\n";
        report << "Success Rate & " << std::fixed << std::setprecision(2) << analysis.successRate << "% \\\"\n";
        report << "\\end{tabular}\n\n";
        
        report << "\\section{Metric Statistics}\n";
        report << "\\begin{tabular}{ll}\n";
        report << "\\textbf{Metric} & \\textbf{Value} \\\"\n";
        for (const auto& pair : analysis.metricStatistics) {
            report << VisualizationHelpers::escapeLaTeX(pair.first) << " & " << VisualizationHelpers::formatNumber(pair.second) << " \\\"\n";
        }
        report << "\\end{tabular}\n\n";
        report << "\\end{document}\n";
        
    } else if (format == ReportFormat::Markdown) {
        report << "# " << title << "\n\n";
        report << "Generated: " << VisualizationHelpers::generateTimestamp() << "\n\n";
        
        report << "## Statistical Summary\n\n";
        report << "| Statistic | Value |\n";
        report << "|---|---|\n";
        report << "Average Performance | " << VisualizationHelpers::formatNumber(analysis.averagePerformance) << "|\n";
        report << "Performance Standard Deviation | " << VisualizationHelpers::formatNumber(analysis.performanceStdDev) << "|\n";
        report << "Performance Variance | " << VisualizationHelpers::formatNumber(analysis.performanceVariance) << "|\n";
        report << "Coefficient of Variation | " << VisualizationHelpers::formatNumber(analysis.coefficientOfVariation) << "|\n";
        report << "Minimum Performance | " << VisualizationHelpers::formatNumber(analysis.minPerformance) << "|\n";
        report << "Maximum Performance | " << VisualizationHelpers::formatNumber(analysis.maxPerformance) << "|\n";
        report << "Median Performance | " << VisualizationHelpers::formatNumber(analysis.medianPerformance) << "|\n";
        report << "25th Percentile | " << VisualizationHelpers::formatNumber(analysis.percentile25) << "|\n";
        report << "75th Percentile | " << VisualizationHelpers::formatNumber(analysis.percentile75) << "|\n";
        report << "95th Percentile | " << VisualizationHelpers::formatNumber(analysis.percentile95) << "|\n";
        report << "Success Count | " << analysis.successCount << "|\n";
        report << "Failure Count | " << analysis.failureCount << "|\n";
        report << "Success Rate | " << std::fixed << std::setprecision(2) << analysis.successRate << "%|\n";
        
        report << "\n## Metric Statistics\n\n";
        report << "| Metric | Value |\n";
        report << "|---|---|\n";
        for (const auto& pair : analysis.metricStatistics) {
            report << "| " << VisualizationHelpers::escapeMarkdown(pair.first) << " | " << VisualizationHelpers::formatNumber(pair.second) << " |\n";
        }
        
    } else if (format == ReportFormat::CSV) {
        report << "Statistic,Value\n";
        report << "Average Performance," << analysis.averagePerformance << "\n";
        report << "Performance Standard Deviation," << analysis.performanceStdDev << "\n";
        report << "Performance Variance," << analysis.performanceVariance << "\n";
        report << "Coefficient of Variation," << analysis.coefficientOfVariation << "\n";
        report << "Minimum Performance," << analysis.minPerformance << "\n";
        report << "Maximum Performance," << analysis.maxPerformance << "\n";
        report << "Median Performance," << analysis.medianPerformance << "\n";
        report << "25th Percentile," << analysis.percentile25 << "\n";
        report << "75th Percentile," << analysis.percentile75 << "\n";
        report << "95th Percentile," << analysis.percentile95 << "\n";
        report << "Success Count," << analysis.successCount << "\n";
        report << "Failure Count," << analysis.failureCount << "\n";
        report << "Success Rate," << analysis.successRate << "\n";
        
        report << "\nMetric,Value\n";
        for (const auto& pair : analysis.metricStatistics) {
            report << pair.first << "," << pair.second << "\n";
        }
        
    } else {
        report << "Report format not implemented: " << static_cast<int>(format) << "\n";
    }
    
    return report.str();
}

ExperimentAnalysis AdvancedVisualizationReporter::analyzeExperimentResults(
    const std::vector<ExperimentResult>& results) {
    
    ExperimentAnalysis analysis;
    
    if (results.empty()) {
        return analysis;
    }
    
    // Extract performance data
    std::vector<double> performances;
    std::map<std::string, std::vector<double>> metricData;
    
    for (const auto& result : results) {
        if (result.success) {
            analysis.successCount++;
        } else {
            analysis.failureCount++;
        }
        
        // Collect performance metrics
        if (result.metrics.find("total_reward") != result.metrics.end()) {
            double reward = result.metrics.at("total_reward");
            performances.push_back(reward);
        } else if (result.metrics.find("firing_rate") != result.metrics.end()) {
            double firingRate = result.metrics.at("firing_rate");
            performances.push_back(firingRate * 10.0); // Scale for comparison
        }
        
        // Collect other metrics
        for (const auto& pair : result.metrics) {
            metricData[pair.first].push_back(pair.second);
        }
    }
    
    analysis.totalExperiments = results.size();
    
    if (!performances.empty()) {
        analysis.averagePerformance = std::accumulate(performances.begin(), performances.end(), 0.0) / performances.size();
        
        // Calculate variance and standard deviation
        double sqSum = 0.0;
        for (double p : performances) {
            double diff = p - analysis.averagePerformance;
            sqSum += diff * diff;
        }
        analysis.performanceVariance = sqSum / performances.size();
        analysis.performanceStdDev = std::sqrt(analysis.performanceVariance);
        
        // Calculate coefficient of variation
        if (analysis.averagePerformance > 0) {
            analysis.coefficientOfVariation = analysis.performanceStdDev / analysis.averagePerformance;
        }
        
        // Find min and max
        analysis.minPerformance = *std::min_element(performances.begin(), performances.end());
        analysis.maxPerformance = *std::max_element(performances.begin(), performances.end());
        
        // Calculate percentiles
        std::vector<double> sortedPerformances = performances;
        std::sort(sortedPerformances.begin(), sortedPerformances.end());
        
        size_t n = sortedPerformances.size();
        if (n >= 2) {
            analysis.medianPerformance = sortedPerformances[n/2];
        }
        
        if (n >= 4) {
            analysis.percentile25 = sortedPerformances[n/4];
            analysis.percentile75 = sortedPerformances[3*n/4];
        }
        
        if (n >= 20) {
            analysis.percentile95 = sortedPerformances[(19 * n) / 20];
        }
        
        analysis.performanceHistory = performances;
    }
    
    // Calculate success rate
    if (analysis.totalExperiments > 0) {
        analysis.successRate = (static_cast<double>(analysis.successCount) / analysis.totalExperiments) * 100.0;
    }
    
    // Calculate statistics for other metrics
    for (auto& pair : metricData) {
        const auto& values = pair.second;
        if (!values.empty()) {
            double sum = std::accumulate(values.begin(), values.end(), 0.0);
            double mean = sum / values.size();
            analysis.metricStatistics[pair.first] = mean;
        }
    }
    
    return analysis;
}

// Additional implementation continues...

} // namespace nlm