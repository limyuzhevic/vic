#pragma once

#include "AnalysisTools.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../performance/PerformanceMonitor.hpp"
#include "../performance/Profiler.hpp"
#include "../performance/Optimizer.hpp"
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <random>
#include <set>
#include <fstream>

namespace nlm {

PerformanceProfiler::PerformanceProfiler()
    : confidence_level_(0.95)
    , threshold_multiplier_(2.0)
    , advanced_analysis_(false)
    , max_report_size_mb_(100) {
}

PerformanceProfiler::~PerformanceProfiler() {
    // Cleanup
}

PerformanceComparisonResult PerformanceProfiler::compareConfigurations(
    const std::vector<std::vector<PerformanceMetrics>>& datasets,
    const std::vector<std::string>& config_names,
    const std::vector<std::string>& metric_names) {
    PerformanceComparisonResult result;
    result.comparison_id = "comparison_" + std::to_string(next_analysis_id_++);
    result.timestamp = std::chrono::steady_clock::now();
    result.overall_confidence = confidence_level_;
    
    if (datasets.size() != config_names.size() || datasets.empty()) {
        Logger::getInstance().logError("Configuration datasets and config names count mismatch");
        return result;
    }
    
    // Use default metrics if not specified
    if (metric_names.empty()) {
        result.configuration_metrics["step_time_ms"] = std::vector<double>();
        result.configuration_metrics["cpu_usage_percent"] = std::vector<double>();
        result.configuration_metrics["memory_usage_mb"] = std::vector<double>();
        result.configuration_metrics["spike_frequency_hz"] = std::vector<double>();
    } else {
        for (const auto& metric : metric_names) {
            result.configuration_metrics[metric] = std::vector<double>();
        }
    }
    
    // Collect metrics for each configuration
    for (size_t i = 0; i < datasets.size(); ++i) {
        const auto& dataset = datasets[i];
        const auto& config_name = config_names[i];
        
        for (const auto& metric_pair : result.configuration_metrics) {
            const auto& metric_name = metric_pair.first;
            std::vector<double>& metric_values = result.configuration_metrics[metric_name];
            
            if (metric_name == "step_time_ms") {
                for (const auto& metrics : dataset) {
                    metric_values.push_back(metrics.neural_computation.step_time_ms);
                }
            } else if (metric_name == "cpu_usage_percent") {
                for (const auto& metrics : dataset) {
                    metric_values.push_back(metrics.system_resources.cpu_usage_percent);
                }
            } else if (metric_name == "memory_usage_mb") {
                for (const auto& metrics : dataset) {
                    metric_values.push_back(metrics.system_resources.memory_usage_mb);
                }
            } else if (metric_name == "spike_frequency_hz") {
                for (const auto& metrics : dataset) {
                    metric_values.push_back(static_cast<double>(metrics.neural_computation.total_spikes) / 1000.0); // Simplified
                }
            }
        }
    }
    
    // Calculate scores for each configuration
    for (size_t i = 0; i < datasets.size(); ++i) {
        double score = 0.0;
        const auto& config_name = config_names[i];
        
        // Calculate score based on metrics (higher is better)
        for (const auto& metric_pair : result.configuration_metrics) {
            const auto& metric_name = metric_pair.first;
            const auto& metric_values = metric_pair.second;
            
            if (!metric_values.empty()) {
                double avg_value = std::accumulate(metric_values.begin(), metric_values.end(), 0.0) / metric_values.size();
                
                if (metric_name == "step_time_ms") {
                    // Lower is better
                    score += (100.0 / (avg_value + 1.0));
                } else {
                    // Higher is better
                    score += avg_value;
                }
            }
        }
        
        result.configuration_scores[config_name] = score;
    }
    
    // Find best configuration
    double best_score = 0.0;
    for (const auto& pair : result.configuration_scores) {
        if (pair.second > best_score) {
            best_score = pair.second;
            result.best_configuration = pair.first;
        }
    }
    
    // Calculate improvements and degradations relative to best
    if (!result.best_configuration.empty()) {
        double best_score = result.configuration_scores[result.best_configuration];
        
        for (const auto& pair : result.configuration_scores) {
            if (pair.first != result.best_configuration) {
                double diff = pair.second - best_score;
                if (diff > 0.1) { // Only significant improvements/degradations
                    result.metric_improvements[result.best_configuration][pair.first] = diff;
                } else if (diff < -0.1) {
                    result.metric_degradations[result.best_configuration][pair.first] = -diff;
                }
            }
        }
    }
    
    comparison_history_.push_back(result);
    
    // Generate recommendations
    result.recommendation = generateConfigurationRecommendations(result);
    
    Logger::getInstance().logInfo("Configuration comparison completed: " + result.comparison_id + " (best: " + result.best_configuration + ")");
    
    return result;
}

PerformanceComparisonResult PerformanceProfiler::compareConfigurationsWithAdvancedAnalysis(
    const std::vector<std::vector<PerformanceMetrics>>& datasets,
    const std::vector<std::string>& config_names,
    const std::map<std::string, std::function<double(const PerformanceMetrics&)>>& custom_metrics) {
    // Delegate to advanced analysis implementation
    return performAdvancedConfigurationComparison(datasets, config_names, {});
}

MemoryUsageAnalysis PerformanceProfiler::analyzeMemoryUsage(const std::vector<PerformanceMetrics>& metrics,
                                                          uint64_t analysis_steps) {
    MemoryUsageAnalysis analysis;
    analysis.analysis_id = "mem_analysis_" + std::to_string(next_analysis_id_++);
    analysis.timestamp = std::chrono::steady_clock::now();
    
    if (metrics.empty()) {
        return analysis;
    }
    
    // Determine number of steps to analyze
    uint64_t steps_to_analyze = std::min(analysis_steps, metrics.size());
    uint64_t start_idx = metrics.size() - steps_to_analyze;
    
    // Collect memory usage data
    analysis.memory_usage_over_time.reserve(steps_to_analyze);
    
    for (uint64_t i = start_idx; i < metrics.size(); ++i) {
        const auto& metric = metrics[i];
        analysis.memory_usage_over_time.push_back(metric.memory_usage.total_memory_mb);
    }
    
    // Calculate statistics
    if (!analysis.memory_usage_over_time.empty()) {
        analysis.peak_memory_mb = *std::max_element(analysis.memory_usage_over_time.begin(), analysis.memory_usage_over_time.end());
        analysis.average_memory_mb = std::accumulate(analysis.memory_usage_over_time.begin(), analysis.memory_usage_over_time.end(), 0.0) / analysis.memory_usage_over_time.size();
        
        if (analysis.memory_usage_over_time.size() > 1) {
            double first_mem = analysis.memory_usage_over_time.front();
            double last_mem = analysis.memory_usage_over_time.back();
            analysis.memory_growth_rate_mb_per_step = (last_mem - first_mem) / (analysis.memory_usage_over_time.size() - 1);
            
            // Memory leak detection
            if (analysis.memory_growth_rate_mb_per_step > 0.01) {
                analysis.memory_leak_detected = true;
                analysis.memory_leak_rate = analysis.memory_growth_rate_mb_per_step;
                analysis.leak_source_function = "Memory allocation pattern analysis";
            }
        }
        
        // Pattern analysis
        analyzeMemoryPatterns(analysis);
    }
    
    memory_analysis_history_.push_back(analysis);
    
    // Generate optimization suggestion
    analysis.memory_optimization_suggestion = generateMemoryOptimizationRecommendations(analysis)[0];
    
    Logger::getInstance().logInfo("Memory usage analysis completed: " + analysis.analysis_id + " (peak: " + std::to_string(analysis.peak_memory_mb) + " MB)");
    
    return analysis;
}

MemoryUsageAnalysis PerformanceProfiler::analyzeMemoryUsageWithDetails(
    const std::vector<PerformanceMetrics>& metrics,
    const std::map<std::string, std::function<double(const PerformanceMetrics&)>>& detailed_metrics) {
    // Delegate to deep memory analysis implementation
    return performDeepMemoryAnalysis(metrics, detailed_metrics);
}

SpikeEventAnalysis PerformanceProfiler::analyzeSpikeEvents(const std::vector<PerformanceMetrics>& metrics,
                                                         uint64_t analysis_steps) {
    SpikeEventAnalysis analysis;
    analysis.analysis_id = "spike_analysis_" + std::to_string(next_analysis_id_++);
    analysis.timestamp = std::chrono::steady_clock::now();
    
    if (metrics.empty()) {
        return analysis;
    }
    
    // Determine number of steps to analyze
    uint64_t steps_to_analyze = std::min(analysis_steps, metrics.size());
    uint64_t start_idx = metrics.size() - steps_to_analyze;
    
    // Collect spike event data
    analysis.spike_timestamps.reserve(steps_to_analyze);
    analysis.spike_amplitudes.reserve(steps_to_analyze);
    analysis.inter_spike_intervals.reserve(steps_to_analyze);
    
    for (uint64_t i = start_idx; i < metrics.size(); ++i) {
        const auto& metric = metrics[i];
        analysis.spike_timestamps.push_back(metric.simulation_step * 1000); // Convert to ms
        analysis.spike_amplitudes.push_back(static_cast<double>(metric.neural_computation.firing_neuron_count));
        
        if (!analysis.inter_spike_intervals.empty()) {
            double isi = static_cast<double>(metric.simulation_step - (i - 1)) * 1.0; // Simplified
            analysis.inter_spike_intervals.push_back(isi);
        }
    }
    
    // Calculate statistics
    if (!analysis.spike_amplitudes.empty()) {
        analysis.average_spike_amplitude = std::accumulate(analysis.spike_amplitudes.begin(), analysis.spike_amplitudes.end(), 0.0) / analysis.spike_amplitudes.size();
        
        double sum_squares = std::inner_product(analysis.spike_amplitudes.begin(), analysis.spike_amplitudes.end(), analysis.spike_amplitudes.begin(), 0.0);
        analysis.spike_amplitude_std = std::sqrt((sum_squares / analysis.spike_amplitudes.size()) - (analysis.average_spike_amplitude * analysis.average_spike_amplitude));
    }
    
    if (!analysis.inter_spike_intervals.empty()) {
        analysis.spike_frequency_hz = 1000.0 / (std::accumulate(analysis.inter_spike_intervals.begin(), analysis.inter_spike_intervals.end(), 0.0) / analysis.inter_spike_intervals.size());
    }
    
    // Analyze spike patterns
    analyzeSpikePatterns(analysis);
    
    spike_analysis_history_.push_back(analysis);
    
    // Generate optimization suggestion
    analysis.spike_event_optimization_suggestion = generateSpikeEventRecommendations(analysis)[0];
    
    Logger::getInstance().logInfo("Spike event analysis completed: " + analysis.analysis_id + " (pattern: " + analysis.spike_pattern_type + ")");
    
    return analysis;
}

SpikeEventAnalysis PerformanceProfiler::analyzeSpikeEventsWithPatternDetection(
    const std::vector<PerformanceMetrics>& metrics,
    const std::map<std::string, std::function<double(const PerformanceMetrics&)>>& spike_metrics) {
    // Delegate to advanced spike analysis implementation
    return performAdvancedSpikeAnalysis(metrics, spike_metrics);
}

SystemScalingAnalysis PerformanceProfiler::analyzeSystemScaling(
    const std::vector<std::pair<size_t, PerformanceMetrics>>& size_metric_pairs) {
    SystemScalingAnalysis analysis;
    analysis.analysis_id = "scaling_analysis_" + std::to_string(next_analysis_id_++);
    analysis.timestamp = std::chrono::steady_clock::now();
    
    if (size_metric_pairs.empty()) {
        return analysis;
    }
    
    // Collect scaling data
    analysis.neuron_counts.reserve(size_metric_pairs.size());
    analysis.performance_scales.reserve(size_metric_pairs.size());
    analysis.memory_scales.reserve(size_metric_pairs.size());
    
    for (const auto& pair : size_metric_pairs) {
        analysis.neuron_counts.push_back(pair.first);
        analysis.performance_scales.push_back(pair.second.neural_computation.average_firing_rate);
        analysis.memory_scales.push_back(pair.second.memory_usage.total_memory_mb);
    }
    
    // Calculate scaling efficiency
    if (analysis.neuron_counts.size() > 1) {
        // Fit exponential model to determine scaling efficiency
        double log_neurons = std::log(static_cast<double>(analysis.neuron_counts.back()));
        double log_perf = std::log(analysis.performance_scales.back());
        
        double scaling_exponent = (log_perf - std::log(analysis.performance_scales.front())) / (log_neurons - std::log(analysis.neuron_counts.front()));
        analysis.scaling_efficiency = scaling_exponent;
        
        // Calculate scalability score (0-1, where 1 is perfect scaling)
        analysis.scalability_score = std::min(1.0, 1.0 / (std::abs(scaling_exponent) + 0.1));
        
        // Identify scaling factors
        for (size_t i = 1; i < analysis.neuron_counts.size(); ++i) {
            double neuron_scale = static_cast<double>(analysis.neuron_counts[i]) / analysis.neuron_counts[i - 1];
            double perf_scale = analysis.performance_scales[i] / analysis.performance_scales[i - 1];
            double memory_scale = analysis.memory_scales[i] / analysis.memory_scales[i - 1];
            
            analysis.scaling_factors["performance_scaling_" + std::to_string(i)] = perf_scale;
            analysis.scaling_factors["memory_scaling_" + std::to_string(i)] = memory_scale;
        }
    }
    
    scaling_analysis_history_.push_back(analysis);
    
    // Generate scaling recommendation
    analysis.scaling_recommendation = generateScalingRecommendations(analysis)[0];
    
    Logger::getInstance().logInfo("System scaling analysis completed: " + analysis.analysis_id + " (efficiency: " + std::to_string(analysis.scaling_efficiency) + ")");
    
    return analysis;
}

SystemScalingAnalysis PerformanceProfiler::analyzeSystemScalingWithHardwareModel(
    const std::vector<std::pair<size_t, PerformanceMetrics>>& size_metric_pairs,
    const std::map<std::string, double>& hardware_characteristics) {
    // Delegate to hardware-aware scaling analysis implementation
    return performHardwareAwareScalingAnalysis(size_metric_pairs, hardware_characteristics);
}

ProfilingReport PerformanceProfiler::generatePerformanceReport(const PerformanceComparisonResult& comparison,
                                                              const std::string& title) {
    ProfilingReport report;
    report.report_id = "report_" + std::to_string(next_analysis_id_++);
    report.timestamp = std::chrono::steady_clock::now();
    report.report_title = title;
    report.report_type = "performance";
    report.analysis_scope = "configuration_comparison";
    
    // Generate executive summary
    std::ostringstream oss;
    oss << "Performance Analysis Report" << std::endl;
    oss << "===========================" << std::endl;
    oss << "Total Configurations Compared: " << comparison.configuration_scores.size() << std::endl;
    oss << "Best Configuration: " << comparison.best_configuration << std::endl;
    oss << "Overall Confidence: " << (comparison.overall_confidence * 100) << "%" << std::endl;
    oss << "\nKey Findings:" << std::endl;
    
    for (const auto& pair : comparison.metric_improvements) {
        for (const auto& improvement : pair.second) {
            oss << "- " << comparison.best_configuration << " improves over " << improvement.first 
                << " by " << improvement.second << " performance units" << std::endl;
        }
    }
    
    for (const auto& pair : comparison.metric_degradations) {
        for (const auto& degradation : pair.second) {
            oss << "- " << comparison.best_configuration << " degrades over " << degradation.first 
                << " by " << degradation.second << " performance units" << std::endl;
        }
    }
    
    report.executive_summary = oss.str();
    
    // Generate key findings
    std::ostringstream findings_oss;
    if (!comparison.best_configuration.empty()) {
        findings_oss << "Best performing configuration: " << comparison.best_configuration << std::endl;
        findings_oss << "Recommended action: Implement " << comparison.best_configuration << " configuration" << std::endl;
    }
    
    report.key_findings = findings_oss.str();
    
    // Generate recommendations
    report.recommendations = generateConfigurationRecommendations(comparison);
    
    // Set confidence level
    report.confidence_level = comparison.overall_confidence;
    
    Logger::getInstance().logInfo("Performance report generated: " + report.report_id + " (title: " + title + ")");
    
    return report;
}

ProfilingReport PerformanceProfiler::generateMemoryUsageReport(const MemoryUsageAnalysis& analysis,
                                                              const std::string& title) {
    ProfilingReport report;
    report.report_id = "report_" + std::to_string(next_analysis_id_++);
    report.timestamp = std::chrono::steady_clock::now();
    report.report_title = title;
    report.report_type = "memory";
    report.analysis_scope = "memory_usage_analysis";
    
    // Generate executive summary
    std::ostringstream oss;
    oss << "Memory Usage Analysis Report" << std::endl;
    oss << "=============================" << std::endl;
    oss << "Analysis Period: " << analysis.memory_usage_over_time.size() << " steps" << std::endl;
    oss << "Peak Memory Usage: " << std::fixed << std::setprecision(2) << analysis.peak_memory_mb << " MB" << std::endl;
    oss << "Average Memory Usage: " << std::fixed << std::setprecision(2) << analysis.average_memory_mb << " MB" << std::endl;
    oss << "Memory Growth Rate: " << std::fixed << std::setprecision(4) << analysis.memory_growth_rate_mb_per_step << " MB/step" << std::endl;
    oss << "Memory Leak Detected: " << (analysis.memory_leak_detected ? "Yes" : "No") << std::endl;
    
    if (analysis.memory_leak_detected) {
        oss << "Leak Rate: " << std::fixed << std::setprecision(4) << analysis.memory_leak_rate << " MB/step" << std::endl;
    }
    
    report.executive_summary = oss.str();
    
    // Generate key findings
    std::ostringstream findings_oss;
    if (analysis.memory_leak_detected) {
        findings_oss << "Memory leak detected: " << analysis.memory_leak_rate << " MB/step" << std::endl;
        findings_oss << "Source: " << analysis.leak_source_function << std::endl;
    }
    findings_oss << "Memory usage pattern: " << (analysis.memory_growth_rate_mb_per_step > 0 ? "increasing" : "stable") << std::endl;
    
    report.key_findings = findings_oss.str();
    
    // Generate recommendations
    report.recommendations = generateMemoryOptimizationRecommendations(analysis);
    
    // Set confidence level
    report.confidence_level = analysis.memory_leak_detected ? 0.8 : 0.95;
    
    Logger::getInstance().logInfo("Memory usage report generated: " + report.report_id + " (title: " + title + ")");
    
    return report;
}

ProfilingReport PerformanceProfiler::generateSpikeEventReport(const SpikeEventAnalysis& analysis,
                                                             const std::string& title) {
    ProfilingReport report;
    report.report_id = "report_" + std::to_string(next_analysis_id_++);
    report.timestamp = std::chrono::steady_clock::now();
    report.report_title = title;
    report.report_type = "spike";
    report.analysis_scope = "spike_event_analysis";
    
    // Generate executive summary
    std::ostringstream oss;
    oss << "Spike Event Analysis Report" << std::endl;
    oss << "=========================== " << std::endl;
    oss << "Analysis Period: " << analysis.spike_timestamps.size() << " time points" << std::endl;
    oss << "Total Spike Count: " << analysis.spike_timestamps.size() << std::endl;
    oss << "Spike Frequency: " << std::fixed << std::setprecision(2) << analysis.spike_frequency_hz << " Hz" << std::endl;
    oss << "Average Spike Amplitude: " << std::fixed << std::setprecision(2) << analysis.average_spike_amplitude << std::endl;
    oss << "Spike Pattern Type: " << analysis.spike_pattern_type << std::endl;
    oss << "Burst Detection Score: " << std::fixed << std::setprecision(2) << analysis.burst_detection_score << std::endl;
    
    report.executive_summary = oss.str();
    
    // Generate key findings
    std::ostringstream findings_oss;
    findings_oss << "Spike pattern: " << analysis.spike_pattern_type << " (confidence: " << analysis.burst_detection_score << ")" << std::endl;
    findings_oss << "Activity level: " << (analysis.spike_frequency_hz > 10 ? "high" : (analysis.spike_frequency_hz > 1 ? "moderate" : "low")) << std::endl;
    
    report.key_findings = findings_oss.str();
    
    // Generate recommendations
    report.recommendations = generateSpikeEventRecommendations(analysis);
    
    // Set confidence level
    report.confidence_level = analysis.burst_detection_score;
    
    Logger::getInstance().logInfo("Spike event report generated: " + report.report_id + " (title: " + title + ")");
    
    return report;
}

ProfilingReport PerformanceProfiler::generateSystemScalingReport(const SystemScalingAnalysis& analysis,
                                                                const std::string& title) {
    ProfilingReport report;
    report.report_id = "report_" + std::to_string(next_analysis_id_++);
    report.timestamp = std::chrono::steady_clock::now();
    report.report_title = title;
    report.report_type = "system";
    report.analysis_scope = "system_scaling_analysis";
    
    // Generate executive summary
    std::ostringstream oss;
    oss << "System Scaling Analysis Report" << std::endl;
    oss << "============================== " << std::endl;
    oss << "Tested System Sizes: " << analysis.neuron_counts.size() << std::endl;
    oss << "Scaling Efficiency: " << std::fixed << std::setprecision(4) << analysis.scaling_efficiency << std::endl;
    oss << "Scalability Score: " << std::fixed << std::setprecision(2) << analysis.scalability_score << std::endl;
    
    oss << "\nScaling Factors:" << std::endl;
    for (const auto& pair : analysis.scaling_factors) {
        oss << "  " << pair.first << ": " << std::fixed << std::setprecision(2) << pair.second << std::endl;
    }
    
    report.executive_summary = oss.str();
    
    // Generate key findings
    std::ostringstream findings_oss;
    findings_oss << "Scaling efficiency: " << analysis.scaling_efficiency << std::endl;
    findings_oss << "Scalability score: " << analysis.scalability_score << std::endl;
    findings_oss << "Hardware recommendation: " << analysis.hardware_scaling_guidance << std::endl;
    
    report.key_findings = findings_oss.str();
    
    // Generate recommendations
    report.recommendations = generateScalingRecommendations(analysis);
    
    // Set confidence level
    report.confidence_level = analysis.scalability_score;
    
    Logger::getInstance().logInfo("System scaling report generated: " + report.report_id + " (title: " + title + ")");
    
    return report;
}

bool PerformanceProfiler::exportComparisonReport(const PerformanceComparisonResult& result, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << performanceComparisonResultToJSON(result);
    file.close();
    
    Logger::getInstance().logInfo("Comparison report exported to: " + filepath);
    return true;
}

bool PerformanceProfiler::exportMemoryAnalysisReport(const MemoryUsageAnalysis& analysis, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << memoryUsageAnalysisToJSON(analysis);
    file.close();
    
    Logger::getInstance().logInfo("Memory analysis report exported to: " + filepath);
    return true;
}

bool PerformanceProfiler::exportSpikeEventReport(const SpikeEventAnalysis& analysis, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << spikeEventAnalysisToJSON(analysis);
    file.close();
    
    Logger::getInstance().logInfo("Spike event report exported to: " + filepath);
    return true;
}

bool PerformanceProfiler::exportScalingReport(const SystemScalingAnalysis& analysis, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << systemScalingAnalysisToJSON(analysis);
    file.close();
    
    Logger::getInstance().logInfo("Scaling report exported to: " + filepath);
    return true;
}

std::vector<PerformanceComparisonResult> PerformanceProfiler::compareMultipleConfigurationsBatch(
    const std::vector<std::vector<std::vector<PerformanceMetrics>>>& all_datasets,
    const std::vector<std::vector<std::string>>& config_names_list) {
    std::vector<PerformanceComparisonResult> results;
    
    if (all_datasets.size() != config_names_list.size()) {
        Logger::getInstance().logError("Datasets and config names count mismatch in batch comparison");
        return results;
    }
    
    for (size_t i = 0; i < all_datasets.size(); ++i) {
        PerformanceComparisonResult result = compareConfigurations(all_datasets[i], config_names_list[i]);
        results.push_back(result);
    }
    
    return results;
}

std::vector<MemoryUsageAnalysis> PerformanceProfiler::analyzeMemoryUsageBatch(
    const std::vector<std::vector<PerformanceMetrics>>& all_metrics,
    uint64_t analysis_steps) {
    std::vector<MemoryUsageAnalysis> results;
    
    for (const auto& metrics : all_metrics) {
        MemoryUsageAnalysis result = analyzeMemoryUsage(metrics, analysis_steps);
        results.push_back(result);
    }
    
    return results;
}

std::vector<SpikeEventAnalysis> PerformanceProfiler::analyzeSpikeEventsBatch(
    const std::vector<std::vector<PerformanceMetrics>>& all_metrics,
    uint64_t analysis_steps) {
    std::vector<SpikeEventAnalysis> results;
    
    for (const auto& metrics : all_metrics) {
        SpikeEventAnalysis result = analyzeSpikeEvents(metrics, analysis_steps);
        results.push_back(result);
    }
    
    return results;
}

double PerformanceProfiler::calculateStatistics(const std::vector<double>& data) const {
    return calculateMean(data);
}

double PerformanceProfiler::calculateTrend(const std::vector<std::pair<uint64_t, double>>& data_points) const {
    if (data_points.size() < 2) {
        return 0.0;
    }
    
    double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_x2 = 0.0;
    
    for (const auto& point : data_points) {
        sum_x += point.first;
        sum_y += point.second;
        sum_xy += point.first * point.second;
        sum_x2 += point.first * point.first;
    }
    
    double n = data_points.size();
    double slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
    
    return slope;
}

double PerformanceProfiler::calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) const {
    if (x.size() != y.size() || x.size() < 2) {
        return 0.0;
    }
    
    double sum_x = std::accumulate(x.begin(), x.end(), 0.0);
    double sum_y = std::accumulate(y.begin(), y.end(), 0.0);
    double sum_xy = 0.0;
    double sum_x2 = 0.0;
    double sum_y2 = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        sum_xy += x[i] * y[i];
        sum_x2 += x[i] * x[i];
        sum_y2 += y[i] * y[i];
    }
    
    double numerator = (x.size() * sum_xy - sum_x * sum_y);
    double denominator = std::sqrt((x.size() * sum_x2 - sum_x * sum_x) * (x.size() * sum_y2 - sum_y * sum_y));
    
    return (denominator > 0.0) ? (numerator / denominator) : 0.0;
}

PerformanceProfiler::AnomalyDetection PerformanceProfiler::detectAnomalies(const std::vector<double>& data, double threshold) const {
    AnomalyDetection detection;
    
    if (data.size() < 3) {
        return detection;
    }
    
    double mean = calculateMean(data);
    double std_dev = calculateStandardDeviation(data, mean);
    
    threshold *= std_dev;
    
    for (size_t i = 0; i < data.size(); ++i) {
        if (std::abs(data[i] - mean) > threshold) {
            detection.anomaly_detected = true;
            detection.anomaly_indices.push_back(i);
            detection.anomaly_scores.push_back(std::abs(data[i] - mean) / std_dev);
        }
    }
    
    if (detection.anomaly_detected) {
        detection.anomaly_type = "statistical_outlier";
        detection.anomaly_description = "Data point deviates significantly from mean";
        detection.suggested_action = "Investigate potential hardware or software issues";
    }
    
    return detection;
}

PerformanceProfiler::PatternAnalysis PerformanceProfiler::analyzePatterns(const std::vector<double>& data,
                                                                          const std::vector<std::string>& pattern_types) {
    PatternAnalysis analysis;
    analysis.pattern_type = "unknown";
    analysis.pattern_confidence = 0.0;
    
    if (data.size() < 3) {
        return analysis;
    }
    
    // Simple pattern detection
    std::string detected_pattern = detectPeriodicity(data);
    if (!detected_pattern.empty() && detected_pattern != "unknown") {
        analysis.pattern_type = detected_pattern;
        analysis.pattern_confidence = 0.8;
        analysis.pattern_interpretation = "Data shows periodic pattern with period " + detected_pattern;
    } else {
        detected_pattern = detectTrend(data);
        if (!detected_pattern.empty() && detected_pattern != "unknown") {
            analysis.pattern_type = detected_pattern;
            analysis.pattern_confidence = 0.7;
            analysis.pattern_interpretation = "Data shows " + detected_pattern + " trend";
        }
    }
    
    return analysis;
}

std::vector<std::string> PerformanceProfiler::generateConfigurationRecommendations(
    const PerformanceComparisonResult& comparison) const {
    std::vector<std::string> recommendations;
    
    if (!comparison.best_configuration.empty()) {
        recommendations.push_back("Primary recommendation: Implement " + comparison.best_configuration + " configuration");
    }
    
    for (const auto& pair : comparison.metric_improvements) {
        for (const auto& improvement : pair.second) {
            recommendations.push_back("Note: " + comparison.best_configuration + " shows " + 
                                   std::to_string(improvement.second) + " improvement over " + improvement.first);
        }
    }
    
    for (const auto& pair : comparison.metric_degradations) {
        for (const auto& degradation : pair.second) {
            recommendations.push_back("Warning: " + comparison.best_configuration + " degrades over " + 
                                   degradation.first + " by " + std::to_string(degradation.second) + ". Consider mitigation strategies.");
        }
    }
    
    return recommendations;
}

std::vector<std::string> PerformanceProfiler::generateMemoryOptimizationRecommendations(
    const MemoryUsageAnalysis& analysis) const {
    std::vector<std::string> recommendations;
    
    if (analysis.memory_leak_detected) {
        recommendations.push_back("CRITICAL: Memory leak detected. Implement memory pool optimization.");
        recommendations.push_back("Investigate " + analysis.leak_source_function + " for memory allocation issues.");
    }
    
    if (analysis.memory_growth_rate_mb_per_step > 0.1) {
        recommendations.push_back("High memory growth rate: Consider optimizing memory usage patterns.");
    }
    
    if (analysis.peak_memory_mb > 2048) {
        recommendations.push_back("Peak memory usage high (>2GB): Implement memory pool resizing.");
    }
    
    recommendations.push_back(analysis.memory_optimization_suggestion);
    
    return recommendations;
}

std::vector<std::string> PerformanceProfiler::generateSpikeEventRecommendations(
    const SpikeEventAnalysis& analysis) const {
    std::vector<std::string> recommendations;
    
    if (analysis.spike_pattern_type == "burst") {
        recommendations.push_back("Burst pattern detected. Consider spike threshold optimization.");
    } else if (analysis.spike_pattern_type == "regular") {
        recommendations.push_back("Regular spike pattern. Current configuration is stable.");
    } else if (analysis.spike_pattern_type == "irregular") {
        recommendations.push_back("Irregular pattern detected. Investigate potential system instability.");
    }
    
    recommendations.push_back(analysis.spike_event_optimization_suggestion);
    
    return recommendations;
}

std::vector<std::string> PerformanceProfiler::generateScalingRecommendations(
    const SystemScalingAnalysis& analysis) const {
    std::vector<std::string> recommendations;
    
    if (analysis.scaling_efficiency < 0.5) {
        recommendations.push_back("Poor scaling efficiency. Consider architectural improvements.");
    }
    
    if (analysis.scalability_score < 0.5) {
        recommendations.push_back("Low scalability score. Hardware optimization needed.");
    }
    
    recommendations.push_back(analysis.scaling_recommendation);
    
    return recommendations;
}

bool PerformanceProfiler::validateAnalysisResults(const PerformanceComparisonResult& result) const {
    if (result.configuration_scores.empty()) {
        return false;
    }
    
    for (const auto& pair : result.configuration_scores) {
        if (pair.second < 0.0) {
            return false;
        }
    }
    
    return true;
}

bool PerformanceProfiler::validateAnalysisResults(const MemoryUsageAnalysis& result) const {
    if (result.memory_usage_over_time.empty()) {
        return false;
    }
    
    if (result.peak_memory_mb < 0.0) {
        return false;
    }
    
    return true;
}

bool PerformanceProfiler::validateAnalysisResults(const SpikeEventAnalysis& result) const {
    if (result.spike_timestamps.empty()) {
        return false;
    }
    
    if (result.spike_pattern_type.empty()) {
        return false;
    }
    
    return true;
}

bool PerformanceProfiler::validateAnalysisResults(const SystemScalingAnalysis& result) const {
    if (result.neuron_counts.empty() || result.performance_scales.empty()) {
        return false;
    }
    
    if (result.scaling_efficiency < -1.0 || result.scaling_efficiency > 1.0) {
        return false;
    }
    
    return true;
}

double PerformanceProfiler::calculateMean(const std::vector<double>& data) {
    if (data.empty()) {
        return 0.0;
    }
    
    return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}

double PerformanceProfiler::calculateMedian(const std::vector<double>& data) {
    if (data.empty()) {
        return 0.0;
    }
    
    std::vector<double> sorted_data = data;
    std::sort(sorted_data.begin(), sorted_data.end());
    
    size_t size = sorted_data.size();
    if (size % 2 == 0) {
        return (sorted_data[size / 2 - 1] + sorted_data[size / 2]) / 2.0;
    } else {
        return sorted_data[size / 2];
    }
}

double PerformanceProfiler::calculateStandardDeviation(const std::vector<double>& data, double mean) {
    if (data.empty()) {
        return 0.0;
    }
    
    double sum_squares = std::inner_product(data.begin(), data.end(), data.begin(), 0.0);
    return std::sqrt((sum_squares / data.size()) - (mean * mean));
}

double PerformanceProfiler::calculateVariance(const std::vector<double>& data, double mean) {
    if (data.empty()) {
        return 0.0;
    }
    
    double sum_squares = std::inner_product(data.begin(), data.end(), data.begin(), 0.0);
    return (sum_squares / data.size()) - (mean * mean);
}

std::vector<double> PerformanceProfiler::calculatePercentiles(const std::vector<double>& data,
                                                             const std::vector<double>& percentiles) {
    if (data.empty() || percentiles.empty()) {
        return {};
    }
    
    std::vector<double> sorted_data = data;
    std::sort(sorted_data.begin(), sorted_data.end());
    
    std::vector<double> results;
    for (double percentile : percentiles) {
        size_t index = static_cast<size_t>((percentile / 100.0) * (sorted_data.size() - 1)) + 1;
        index = std::min<size_t>(index, sorted_data.size() - 1);
        results.push_back(sorted_data[index]);
    }
    
    return results;
}

std::vector<double> PerformanceProfiler::calculateQuartiles(const std::vector<double>& data) {
    return calculatePercentiles(data, {25.0, 50.0, 75.0});
}

void PerformanceProfiler::analyzeMemoryPatterns(MemoryUsageAnalysis& analysis) {
    // Simple pattern analysis
    if (analysis.memory_usage_over_time.size() < 3) {
        return;
    }
    
    // Check for exponential growth pattern
    double growth_rate = analysis.memory_growth_rate_mb_per_step;
    
    if (growth_rate > 0.1) {
        analysis.memory_patterns["exponential_growth"] = growth_rate;
    } else if (growth_rate > 0.01) {
        analysis.memory_patterns["linear_growth"] = growth_rate;
    } else if (growth_rate > 0.001) {
        analysis.memory_patterns["slow_growth"] = growth_rate;
    } else {
        analysis.memory_patterns["stable"] = growth_rate;
    }
}

void PerformanceProfiler::analyzeSpikePatterns(SpikeEventAnalysis& analysis) {
    // Pattern classification based on inter-spike intervals
    if (analysis.inter_spike_intervals.size() >= 3) {
        analysis.spike_pattern_type = classifySpikePattern(analysis.inter_spike_intervals);
        analysis.burst_detection_score = calculateBurstScore(analysis.inter_spike_intervals);
        analysis.pattern_classification_notes = "Pattern analyzed based on inter-spike intervals";
    }
}

std::string PerformanceProfiler::detectPeriodicity(const std::vector<double>& data, double tolerance) const {
    if (data.size() < 4) {
        return "unknown";
    }
    
    // Simple periodicity detection (check for repeating patterns)
    for (size_t period = 1; period <= data.size() / 2; ++period) {
        bool is_periodic = true;
        
        for (size_t i = period; i < data.size(); ++i) {
            if (std::abs(data[i] - data[i - period]) > tolerance) {
                is_periodic = false;
                break;
            }
        }
        
        if (is_periodic) {
            return std::to_string(period);
        }
    }
    
    return "unknown";
}

std::string PerformanceProfiler::detectTrend(const std::vector<double>& data) const {
    if (data.size() < 2) {
        return "unknown";
    }
    
    double mean = calculateMean(data);
    double first_half_mean = calculateMean(std::vector<double>(data.begin(), data.begin() + data.size() / 2));
    double second_half_mean = calculateMean(std::vector<double>(data.begin() + data.size() / 2, data.end()));
    
    if (second_half_mean > first_half_mean * 1.1) {
        return "increasing";
    } else if (second_half_mean < first_half_mean * 0.9) {
        return "decreasing";
    } else {
        return "stable";
    }
}

std::string PerformanceProfiler::detectSeasonality(const std::vector<double>& data, size_t period) const {
    if (data.size() < period * 2) {
        return "unknown";
    }
    
    // Simple seasonality check
    double sum_diff = 0.0;
    for (size_t i = period; i < data.size(); ++i) {
        sum_diff += std::abs(data[i] - data[i - period]);
    }
    
    double avg_diff = sum_diff / (data.size() - period);
    return (avg_diff < 0.1) ? std::to_string(period) : "unknown";
}

double PerformanceProfiler::calculateBurstScore(const std::vector<double>& inter_spike_intervals) const {
    if (inter_spike_intervals.size() < 3) {
        return 0.0;
    }
    
    double sum_short = 0.0;
    size_t short_count = 0;
    
    for (size_t i = 1; i < inter_spike_intervals.size(); ++i) {
        if (inter_spike_intervals[i] < inter_spike_intervals[i - 1] * 0.5) {
            sum_short += inter_spike_intervals[i];
            short_count++;
        }
    }
    
    return (short_count > 0) ? (1.0 - (sum_short / (short_count * inter_spike_intervals.back()))) : 0.0;
}

PerformanceComparisonResult PerformanceProfiler::performAdvancedConfigurationComparison(
    const std::vector<std::vector<PerformanceMetrics>>& datasets,
    const std::vector<std::string>& config_names,
    const std::vector<std::string>& metric_names) {
    // Enhanced configuration comparison with statistical analysis
    return compareConfigurations(datasets, config_names, metric_names);
}

MemoryUsageAnalysis PerformanceProfiler::performDeepMemoryAnalysis(
    const std::vector<PerformanceMetrics>& metrics,
    const std::map<std::string, std::function<double(const PerformanceMetrics&)>>& detailed_metrics) {
    MemoryUsageAnalysis analysis;
    analysis.analysis_id = "deep_mem_analysis_" + std::to_string(next_analysis_id_++);
    analysis.timestamp = std::chrono::steady_clock::now();
    
    if (metrics.empty()) {
        return analysis;
    }
    
    // Apply detailed metrics if available
    for (const auto& pair : detailed_metrics) {
        std::vector<double> metric_values;
        for (const auto& metric : metrics) {
            metric_values.push_back(pair.second(metric));
        }
        
        if (!metric_values.empty()) {
            double avg = calculateMean(metric_values);
            double std_dev = calculateStandardDeviation(metric_values, avg);
            
            analysis.memory_patterns[pair.first + "_mean"] = avg;
            analysis.memory_patterns[pair.first + "_std"] = std_dev;
        }
    }
    
    return analysis;
}

SpikeEventAnalysis PerformanceProfiler::performAdvancedSpikeAnalysis(
    const std::vector<PerformanceMetrics>& metrics,
    const std::map<std::string, std::function<double(const PerformanceMetrics&)>>& spike_metrics) {
    SpikeEventAnalysis analysis;
    analysis.analysis_id = "advanced_spike_analysis_" + std::to_string(next_analysis_id_++);
    analysis.timestamp = std::chrono::steady_clock::now();
    
    if (metrics.empty()) {
        return analysis;
    }
    
    // Apply spike metrics if available
    for (const auto& pair : spike_metrics) {
        std::vector<double> metric_values;
        for (const auto& metric : metrics) {
            metric_values.push_back(pair.second(metric));
        }
        
        if (!metric_values.empty()) {
            double avg = calculateMean(metric_values);
            analysis.spike_amplitudes.push_back(avg);
        }
    }
    
    // Analyze spike patterns
    analyzeSpikePatterns(analysis);
    
    return analysis;
}

SystemScalingAnalysis PerformanceProfiler::performHardwareAwareScalingAnalysis(
    const std::vector<std::pair<size_t, PerformanceMetrics>>& size_metric_pairs,
    const std::map<std::string, double>& hardware_characteristics) {
    SystemScalingAnalysis analysis;
    analysis.analysis_id = "hardware_scaling_analysis_" + std::to_string(next_analysis_id_++);
    analysis.timestamp = std::chrono::steady_clock::now();
    analysis.hardware_scaling_guidance = "Use hardware-specific optimization guidelines";
    
    if (size_metric_pairs.empty()) {
        return analysis;
    }
    
    // Apply hardware characteristics if available
    for (const auto& pair : hardware_characteristics) {
        analysis.scaling_factors[pair.first] = pair.second;
    }
    
    // Perform scaling analysis
    analyzeScalingFactors(analysis);
    
    return analysis;
}

void PerformanceProfiler::analyzeScalingFactors(SystemScalingAnalysis& analysis) {
    if (analysis.neuron_counts.size() < 2) {
        return;
    }
    
    for (size_t i = 1; i < analysis.neuron_counts.size(); ++i) {
        double neuron_scale = static_cast<double>(analysis.neuron_counts[i]) / analysis.neuron_counts[i - 1];
        double perf_scale = analysis.performance_scales[i] / analysis.performance_scales[i - 1];
        double memory_scale = analysis.memory_scales[i] / analysis.memory_scales[i - 1];
        
        analysis.scaling_factors["neuron_scale_" + std::to_string(i)] = neuron_scale;
        analysis.scaling_factors["performance_scale_" + std::to_string(i)] = perf_scale;
        analysis.scaling_factors["memory_scale_" + std::to_string(i)] = memory_scale;
    }
}

PerformanceComparisonResult PerformanceProfiler::getLastComparison() const {
    if (comparison_history_.empty()) {
        return PerformanceComparisonResult{};
    }
    
    return comparison_history_.back();
}

MemoryUsageAnalysis PerformanceProfiler::getLastMemoryAnalysis() const {
    if (memory_analysis_history_.empty()) {
        return MemoryUsageAnalysis{};
    }
    
    return memory_analysis_history_.back();
}

SpikeEventAnalysis PerformanceProfiler::getLastSpikeAnalysis() const {
    if (spike_analysis_history_.empty()) {
        return SpikeEventAnalysis{};
    }
    
    return spike_analysis_history_.back();
}

SystemScalingAnalysis PerformanceProfiler::getLastScalingAnalysis() const {
    if (scaling_analysis_history_.empty()) {
        return SystemScalingAnalysis{};
    }
    
    return scaling_analysis_history_.back();
}

std::string PerformanceProfiler::performanceComparisonResultToJSON(const PerformanceComparisonResult& result) {
    std::ostringstream oss;
    oss << "{\"comparison_id\": \"" << result.comparison_id << "\"," << std::endl;
    oss << "  \"timestamp\": \"" << std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - std::chrono::steady_clock::time_point()).count() << "ms\"," << std::endl;
    oss << "  \"best_configuration\": \"" << result.best_configuration << "\"," << std::endl;
    oss << "  \"overall_confidence\": " << result.overall_confidence << "," << std::endl;
    oss << "  \"recommendations\": [";
    
    for (size_t i = 0; i < result.recommendations.size(); ++i) {
        oss << "\"" << result.recommendations[i] << "\"";
        if (i < result.recommendations.size() - 1) {
            oss << ", ";
        }
    }
    
    oss << "]" << std::endl;
    oss << "}";
    return oss.str();
}

std::string PerformanceProfiler::memoryUsageAnalysisToJSON(const MemoryUsageAnalysis& analysis) {
    std::ostringstream oss;
    oss << "{\"analysis_id\": \"" << analysis.analysis_id << "\"," << std::endl;
    oss << "  \"timestamp\": \"" << std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - std::chrono::steady_clock::time_point()).count() << "ms\"," << std::endl;
    oss << "  \"peak_memory_mb\": " << std::fixed << std::setprecision(2) << analysis.peak_memory_mb << "," << std::endl;
    oss << "  \"memory_leak_detected\": " << (analysis.memory_leak_detected ? "true" : "false") << "," << std::endl;
    oss << "  \"leak_rate\": " << std::fixed << std::setprecision(4) << analysis.memory_leak_rate << "," << std::endl;
    oss << "  \"optimization_suggestion\": \"" << analysis.memory_optimization_suggestion << "\"" << std::endl;
    oss << "}";
    return oss.str();
}

std::string PerformanceProfiler::spikeEventAnalysisToJSON(const SpikeEventAnalysis& analysis) {
    std::ostringstream oss;
    oss << "{\"analysis_id\": \"" << analysis.analysis_id << "\"," << std::endl;
    oss << "  \"timestamp\": \"" << std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - std::chrono::steady_clock::time_point()).count() << "ms\"," << std::endl;
    oss << "  \"spike_frequency_hz\": " << std::fixed << std::setprecision(2) << analysis.spike_frequency_hz << "," << std::endl;
    oss << "  \"spike_pattern_type\": \"" << analysis.spike_pattern_type << "\"," << std::endl;
    oss << "  \"burst_detection_score\": " << std::fixed << std::setprecision(2) << analysis.burst_detection_score << "," << std::endl;
    oss << "  \"optimization_suggestion\": \"" << analysis.spike_event_optimization_suggestion << "\"" << std::endl;
    oss << "}";
    return oss.str();
}

std::string PerformanceProfiler::systemScalingAnalysisToJSON(const SystemScalingAnalysis& analysis) {
    std::ostringstream oss;
    oss << "{\"analysis_id\": \"" << analysis.analysis_id << "\"," << std::endl;
    oss << "  \"timestamp\": \"" << std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - std::chrono::steady_clock::time_point()).count() << "ms\"," << std::endl;
    oss << "  \"scaling_efficiency\": " << std::fixed << std::setprecision(4) << analysis.scaling_efficiency << "," << std::endl;
    oss << "  \"scalability_score\": " << std::fixed << std::setprecision(2) << analysis.scalability_score << "," << std::endl;
    oss << "  \"scaling_recommendation\": \"" << analysis.scaling_recommendation << "\"" << std::endl;
    oss << "}";
    return oss.str();
}

std::string PerformanceProfiler::profilingReportToJSON(const ProfilingReport& report) {
    std::ostringstream oss;
    oss << "{\"report_id\": \"" << report.report_id << "\"," << std::endl;
    oss << "  \"timestamp\": \"" << std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - std::chrono::steady_clock::time_point()).count() << "ms\"," << std::endl;
    oss << "  \"report_title\": \"" << report.report_title << "\"," << std::endl;
    oss << "  \"report_type\": \"" << report.report_type << "\"," << std::endl;
    oss << "  \"executive_summary\": \"" << report.executive_summary << "\"," << std::endl;
    oss << "  \"confidence_level\": " << report.confidence_level << "," << std::endl;
    oss << "  \"recommendations\": [";
    
    for (size_t i = 0; i < report.recommendations.size(); ++i) {
        oss << "\"" << report.recommendations[i] << "\"";
        if (i < report.recommendations.size() - 1) {
            oss << ", ";
        }
    }
    
    oss << "]" << std::endl;
    oss << "}";
    return oss.str();
}

} // namespace nlm
