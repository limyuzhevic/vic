// Performance Analysis Tools Implementation
// Provides performance analysis and optimization recommendations
// Integrates with existing performance infrastructure

#include "performance/PerformanceAnalysis.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <thread>

namespace nlm {

// PerformanceAnalyzer Implementation

PerformanceAnalyzer::PerformanceAnalyzer() {
}

PerformanceAnalysis PerformanceAnalyzer::analyzeSamples(const std::vector<PerformanceSample>& samples) const {
    PerformanceAnalysis analysis("Sample Analysis");
    
    if (samples.empty()) {
        analysis.score = 0.0;
        analysis.warnings.push_back("No performance samples available for analysis");
        return analysis;
    }
    
    // Analyze different aspects
    PerformanceAnalysis simdAnalysis = analyzeSIMDEfficiency(samples);
    PerformanceAnalysis memoryAnalysis = analyzeMemoryEfficiency(samples);
    PerformanceAnalysis parallelAnalysis = analyzeParallelEfficiency(samples);
    PerformanceAnalysis numericalAnalysis = NumericalStabilityAnalyzer::getInstance().analyzeNumericalStability(samples);
    
    // Combine scores
    std::vector<PerformanceAnalysis> allAnalyses = {simdAnalysis, memoryAnalysis, parallelAnalysis, numericalAnalysis};
    
    double totalScore = 0.0;
    size_t validAnalyses = 0;
    
    for (const auto& a : allAnalyses) {
        totalScore += a.score;
        validAnalyses++;
    }
    
    if (validAnalyses > 0) {
        analysis.score = totalScore / static_cast<double>(validAnalyses);
    }
    
    // Generate recommendations
    for (const auto& a : allAnalyses) {
        analysis.recommendations.insert(analysis.recommendations.end(), 
                                       a.recommendations.begin(), a.recommendations.end());
        analysis.warnings.insert(analysis.warnings.end(), 
                               a.warnings.begin(), a.warnings.end());
    }
    
    // Add general recommendations
    if (analysis.score < 0.7) {
        analysis.recommendations.push_back("Consider enabling SIMD optimizations");
        analysis.recommendations.push_back("Review memory allocation patterns");
        analysis.recommendations.push_back("Optimize parallel processing efficiency");
    }
    
    return analysis;
}

PerformanceAnalysis PerformanceAnalyzer::analyzeMetrics(const std::unordered_map<PerformanceMetric, double>& metrics) const {
    PerformanceAnalysis analysis("Metric Analysis");
    
    if (metrics.empty()) {
        analysis.score = 0.0;
        analysis.warnings.push_back("No performance metrics available for analysis");
        return analysis;
    }
    
    // Calculate overall score based on metrics
    double totalScore = 0.0;
    size_t validMetrics = 0;
    
    for (const auto& [metric, value] : metrics) {
        double metricScore = 0.0;
        switch (metric) {
            case PerformanceMetric::NEURONS_PER_SECOND:
                // Higher is better
                metricScore = std::min(value / 10000.0, 1.0);  // Cap at 10000 neurons/sec
                break;
            case PerformanceMetric::SPIKES_PER_SECOND:
                // Higher is better
                metricScore = std::min(value / 5000.0, 1.0);  // Cap at 5000 spikes/sec
                break;
            case PerformanceMetric::SIMD_EFFICIENCY:
                // Higher is better
                metricScore = value;  // Already 0.0 to 1.0
                break;
            case PerformanceMetric::NUMERICAL_STABILITY:
                // Higher is better
                metricScore = value;  // Already 0.0 to 1.0
                break;
            case PerformanceMetric::REAL_TIME_FACTOR:
                // Higher is better (closer to 1.0 is better)
                metricScore = 1.0 - std::abs(value - 1.0) * 10.0;  // Penalize deviation from 1.0
                break;
            default:
                // Neutral score for other metrics
                metricScore = 0.5;
        }
        
        totalScore += metricScore;
        validMetrics++;
    }
    
    if (validMetrics > 0) {
        analysis.score = totalScore / static_cast<double>(validMetrics);
    }
    
    return analysis;
}

std::vector<BottleneckAnalysis> PerformanceAnalyzer::detectBottlenecks(const std::vector<PerformanceSample>& samples) const {
    std::vector<BottleneckAnalysis> bottlenecks;
    
    if (samples.empty()) {
        return bottlenecks;
    }
    
    // Analyze SIMD performance
    PerformanceAnalysis simdAnalysis = analyzeSIMDEfficiency(samples);
    if (simdAnalysis.score < 0.7) {
        bottlenecks.emplace_back("simd", 1.0 - simdAnalysis.score, "SIMD vectorization", 
                               1.0 - simdAnalysis.score, "Enable SIMD optimizations");
    }
    
    // Analyze memory efficiency
    PerformanceAnalysis memoryAnalysis = analyzeMemoryEfficiency(samples);
    if (memoryAnalysis.score < 0.7) {
        bottlenecks.emplace_back("memory", 1.0 - memoryAnalysis.score, "Memory access patterns",
                               1.0 - memoryAnalysis.score, "Optimize memory allocation and access patterns");
    }
    
    // Analyze parallel efficiency
    PerformanceAnalysis parallelAnalysis = analyzeParallelEfficiency(samples);
    if (parallelAnalysis.score < 0.7) {
        bottlenecks.emplace_back("parallel", 1.0 - parallelAnalysis.score, "Thread utilization",
                               1.0 - parallelAnalysis.score, "Optimize parallel processing");
    }
    
    return bottlenecks;
}

PerformanceAnalysis PerformanceAnalyzer::analyzeSIMDEfficiency(const std::vector<PerformanceSample>& samples) const {
    PerformanceAnalysis analysis("SIMD Efficiency Analysis");
    
    if (samples.empty()) {
        analysis.score = 0.0;
        analysis.warnings.push_back("No samples for SIMD analysis");
        return analysis;
    }
    
    analysis.score = calculateSIMDEfficiencyScore(samples);
    
    if (analysis.score < 0.5) {
        analysis.warnings.push_back("SIMD efficiency is below 50%");
        analysis.recommendations.push_back("Check for scalar fallbacks in hot paths");
        analysis.recommendations.push_back("Align memory for SIMD operations");
    } else if (analysis.score < 0.8) {
        analysis.recommendations.push_back("Consider enabling higher SIMD width (AVX-512)");
    }
    
    return analysis;
}

PerformanceAnalysis PerformanceAnalyzer::analyzeMemoryEfficiency(const std::vector<PerformanceSample>& samples) const {
    PerformanceAnalysis analysis("Memory Efficiency Analysis");
    
    if (samples.empty()) {
        analysis.score = 0.0;
        analysis.warnings.push_back("No samples for memory analysis");
        return analysis;
    }
    
    analysis.score = calculateMemoryEfficiencyScore(samples);
    
    if (analysis.score < 0.5) {
        analysis.warnings.push_back("Memory efficiency is below 50%");
        analysis.recommendations.push_back("Enable memory pools for allocation");
        analysis.recommendations.push_back("Use Structure-of-Arrays (SoA) layout");
        analysis.recommendations.push_back("Optimize cache usage patterns");
    }
    
    return analysis;
}

PerformanceAnalysis PerformanceAnalyzer::analyzeParallelEfficiency(const std::vector<PerformanceSample>& samples) const {
    PerformanceAnalysis analysis("Parallel Efficiency Analysis");
    
    if (samples.empty()) {
        analysis.score = 0.0;
        analysis.warnings.push_back("No samples for parallel analysis");
        return analysis;
    }
    
    analysis.score = calculateParallelEfficiencyScore(samples);
    
    if (analysis.score < 0.5) {
        analysis.warnings.push_back("Parallel efficiency is below 50%");
        analysis.recommendations.push_back("Tune thread count for your hardware");
        analysis.recommendations.push_back("Reduce contention on shared resources");
        analysis.recommendations.push_back("Optimize work distribution");
    }
    
    return analysis;
}

std::vector<std::string> PerformanceAnalyzer::generateRecommendations(const PerformanceAnalysis& analysis) const {
    std::vector<std::string> recommendations = analysis.recommendations;
    
    if (analysis.score < 0.5) {
        recommendations.push_back("CRITICAL: Performance score is below 50%");
        recommendations.push_back("Review all bottleneck analyses");
        recommendations.push_back("Consider hardware upgrade or algorithm redesign");
    } else if (analysis.score < 0.7) {
        recommendations.push_back("Performance score below 70% - optimize before production");
    }
    
    return recommendations;
}

double PerformanceAnalyzer::calculatePerformanceScore(const PerformanceAnalysis& analysis) const {
    return analysis.score;
}

std::vector<std::string> PerformanceAnalyzer::comparePerformance(const std::vector<PerformanceAnalysis>& analyses) const {
    std::vector<std::string> comparisons;
    
    if (analyses.size() < 2) {
        return comparisons;
    }
    
    std::vector<double> scores;
    for (const auto& analysis : analyses) {
        scores.push_back(analysis.score);
    }
    
    std::sort(scores.begin(), scores.end(), std::greater<double>());
    
    comparisons.push_back("Performance comparison (best to worst):");
    for (size_t i = 0; i < scores.size(); ++i) {
        comparisons.push_back("  " + std::to_string(i + 1) + ". Score: " + 
                             std::to_string(scores[i]));
    }
    
    if (scores.front() - scores.back() > 0.3) {
        comparisons.push_back("WARNING: Large performance variation detected");
    }
    
    return comparisons;
}

// Helper methods for PerformanceAnalyzer

double PerformanceAnalyzer::calculateSIMDEfficiencyScore(const std::vector<PerformanceSample>& samples) const {
    double totalEfficiency = 0.0;
    size_t validSamples = 0;
    
    for (const auto& sample : samples) {
        // This would be calculated from actual SIMD performance data
        // For now, use a placeholder based on stability score
        totalEfficiency += sample.stabilityScore;
        validSamples++;
    }
    
    return validSamples > 0 ? totalEfficiency / static_cast<double>(validSamples) : 0.0;
}

double PerformanceAnalyzer::calculateMemoryEfficiencyScore(const std::vector<PerformanceSample>& samples) const {
    double totalEfficiency = 0.0;
    size_t validSamples = 0;
    
    for (const auto& sample : samples) {
        // Calculate memory efficiency based on memory usage and allocation patterns
        double memoryScore = 1.0;
        
        // Lower memory usage is better
        if (sample.memoryUsage > 100.0) {
            memoryScore *= 1.0 - (sample.memoryUsage - 100.0) / 1000.0;
        }
        
        totalEfficiency += memoryScore;
        validSamples++;
    }
    
    return validSamples > 0 ? totalEfficiency / static_cast<double>(validSamples) : 0.0;
}

double PerformanceAnalyzer::calculateParallelEfficiencyScore(const std::vector<PerformanceSample>& samples) const {
    double totalEfficiency = 0.0;
    size_t validSamples = 0;
    
    for (const auto& sample : samples) {
        // Calculate parallel efficiency based on active threads vs utilization
        double parallelScore = 1.0;
        
        // This is a placeholder - would use actual thread utilization data
        if (sample.activeThreads > 0) {
            parallelScore = std::min(sample.activeThreads / static_cast<double>(std::thread::hardware_concurrency()), 1.0);
        }
        
        totalEfficiency += parallelScore;
        validSamples++;
    }
    
    return validSamples > 0 ? totalEfficiency / static_cast<double>(validSamples) : 0.0;
}

std::string PerformanceAnalyzer::getSIMDRecommendation(double efficiency) const {
    if (efficiency > 0.9) return "SIMD optimizations are working well";
    if (efficiency > 0.7) return "Enable higher SIMD width for better performance";
    if (efficiency > 0.5) return "Check for scalar fallbacks in hot paths";
    return "SIMD usage needs significant improvement";
}

std::string PerformanceAnalyzer::getMemoryRecommendation(double efficiency) const {
    if (efficiency > 0.9) return "Memory management is optimized";
    if (efficiency > 0.7) return "Consider enabling memory pools";
    if (efficiency > 0.5) return "Optimize memory access patterns";
    return "Memory efficiency needs major improvement";
}

std::string PerformanceAnalyzer::getParallelRecommendation(double efficiency) const {
    if (efficiency > 0.9) return "Parallel processing is optimized";
    if (efficiency > 0.7) return "Tune thread count for your hardware";
    if (efficiency > 0.5) return "Reduce parallel overhead";
    return "Parallel processing needs significant improvement";
}

// NumericalStabilityAnalyzer Implementation

NumericalStabilityAnalyzer::NumericalStabilityAnalyzer() {
}

PerformanceAnalysis NumericalStabilityAnalyzer::analyzeNumericalStability(const std::vector<PerformanceSample>& samples) const {
    PerformanceAnalysis analysis("Numerical Stability Analysis");
    
    if (samples.empty()) {
        analysis.score = 0.0;
        analysis.warnings.push_back("No samples for numerical stability analysis");
        return analysis;
    }
    
    NumericalMetrics metrics = calculateNumericalMetrics(samples);
    analysis.score = calculateStabilityScore(metrics);
    
    // Generate recommendations based on error metrics
    analysis.recommendations = generateStabilityRecommendations(metrics);
    
    if (metrics.maxError > 1e-3) {
        analysis.warnings.push_back("Numerical errors exceed 1e-3 threshold");
    }
    
    if (metrics.errorCount > 0) {
        analysis.warnings.push_back(std::to_string(metrics.errorCount) + " numerical errors detected");
    }
    
    return analysis;
}

std::vector<std::string> NumericalStabilityAnalyzer::detectInstabilityPatterns(const std::vector<PerformanceSample>& samples) const {
    std::vector<std::string> patterns;
    
    if (samples.size() < 3) {
        return patterns;
    }
    
    // Check for increasing error trend
    double firstThird = 0.0, secondThird = 0.0;
    size_t thirdSize = samples.size() / 3;
    
    for (size_t i = 0; i < thirdSize; ++i) {
        firstThird += samples[i].numericalError;
    }
    firstThird /= static_cast<double>(thirdSize);
    
    for (size_t i = thirdSize; i < 2 * thirdSize; ++i) {
        secondThird += samples[i].numericalError;
    }
    secondThird /= static_cast<double>(thirdSize);
    
    if (secondThird > firstThird * 1.5) {
        patterns.push_back("Increasing numerical error trend detected");
    }
    
    // Check for NaN/Inf values
    size_t nanInfCount = 0;
    for (const auto& sample : samples) {
        if (std::isnan(sample.numericalError) || std::isinf(sample.numericalError)) {
            nanInfCount++;
        }
    }
    
    if (nanInfCount > 0) {
        patterns.push_back(std::to_string(nanInfCount) + " NaN/Inf values in numerical errors");
    }
    
    return patterns;
}

NumericalStabilityAnalyzer::NumericalMetrics NumericalStabilityAnalyzer::calculateNumericalMetrics(const std::vector<PerformanceSample>& samples) const {
    NumericalMetrics metrics;
    
    if (samples.empty()) {
        return metrics;
    }
    
    std::vector<double> errors;
    for (const auto& sample : samples) {
        if (std::isfinite(sample.numericalError)) {
            errors.push_back(sample.numericalError);
        }
    }
    
    if (errors.empty()) {
        return metrics;
    }
    
    metrics.maxError = *std::max_element(errors.begin(), errors.end());
    metrics.meanError = std::accumulate(errors.begin(), errors.end(), 0.0) / static_cast<double>(errors.size());
    
    // Calculate standard deviation
    double mean = metrics.meanError;
    double variance = 0.0;
    for (double error : errors) {
        variance += (error - mean) * (error - mean);
    }
    metrics.errorStddev = std::sqrt(variance / static_cast<double>(errors.size()));
    
    metrics.errorCount = static_cast<uint64_t>(errors.size());
    metrics.finiteCheckCount = static_cast<uint64_t>(samples.size());
    
    return metrics;
}

std::vector<std::string> NumericalStabilityAnalyzer::generateStabilityRecommendations(const NumericalMetrics& metrics) const {
    std::vector<std::string> recommendations;
    
    if (metrics.maxError > 1e-3) {
        recommendations.push_back("Use double precision for critical calculations");
        recommendations.push_back("Add numerical stability checks");
        recommendations.push_back("Implement error compensation techniques");
    } else if (metrics.maxError > 1e-6) {
        recommendations.push_back("Consider using higher precision arithmetic");
    }
    
    if (metrics.errorStddev > metrics.meanError * 2.0) {
        recommendations.push_back("Reduce numerical noise");
        recommendations.push_back("Check for catastrophic cancellation");
    }
    
    if (metrics.maxError > 1e-9) {
        recommendations.push_back("Add numerical error bounds checking");
    }
    
    return recommendations;
}

double NumericalStabilityAnalyzer::calculateStabilityScore(const NumericalMetrics& metrics) const {
    if (metrics.maxError > 1e-3) return 0.0;
    if (metrics.maxError > 1e-6) return 0.2 + 0.8 * (1.0 - metrics.maxError / 1e-6);
    if (metrics.maxError > 1e-9) return 0.8 + 0.2 * (1.0 - metrics.maxError / 1e-9);
    return 1.0;
}

std::string NumericalStabilityAnalyzer::getErrorAnalysis(const NumericalMetrics& metrics) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Max error: " << metrics.maxError << ", Mean error: " << metrics.meanError << "; ";
    ss << "Stddev: " << metrics.errorStddev << ", Count: " << metrics.errorCount;
    return ss.str();
}

// PerformanceOptimizer Implementation

PerformanceOptimizer::PerformanceOptimizer() {
}

PerformanceOptimizer::SIMDOptimization PerformanceOptimizer::optimizeSIMDUsage(const std::vector<PerformanceSample>& samples) const {
    SIMDOptimization opt;
    
    if (samples.empty()) {
        return opt;
    }
    
    double totalStability = 0.0;
    for (const auto& sample : samples) {
        totalStability += sample.stabilityScore;
    }
    double avgStability = totalStability / static_cast<double>(samples.size());
    
    // Enable SIMD if stability is good
    opt.enableAVX512 = true;
    opt.enableAVX2 = true;
    opt.enableSSE = true;
    
    if (avgStability > 0.9) {
        opt.recommendedSIMDWidth = 16;  // AVX-512
        opt.recommendation = "Enable AVX-512 for maximum performance";
    } else if (avgStability > 0.7) {
        opt.recommendedSIMDWidth = 8;  // AVX2
        opt.recommendation = "Enable AVX2 for good performance";
    } else {
        opt.recommendedSIMDWidth = 4;  // SSE
        opt.recommendation = "Enable SSE for compatibility";
    }
    
    return opt;
}

PerformanceOptimizer::MemoryOptimization PerformanceOptimizer::optimizeMemoryUsage(const std::vector<PerformanceSample>& samples) const {
    MemoryOptimization opt;
    
    if (samples.empty()) {
        return opt;
    }
    
    double totalMemoryUsage = 0.0;
    for (const auto& sample : samples) {
        totalMemoryUsage += sample.memoryUsage;
    }
    double avgMemoryUsage = totalMemoryUsage / static_cast<double>(samples.size());
    
    // Enable memory pools if memory usage is high
    opt.enableMemoryPool = true;
    opt.memoryPoolSize = 1024;  // Configurable
    opt.enableSOA = true;
    opt.cacheLineSize = 64;
    
    if (avgMemoryUsage > 100.0) {
        opt.recommendation = "Reduce memory usage by enabling memory pools and SoA layout";
    } else {
        opt.recommendation = "Memory usage is acceptable";
    }
    
    return opt;
}

PerformanceOptimizer::ParallelOptimization PerformanceOptimizer::optimizeParallelProcessing(const std::vector<PerformanceSample>& samples) const {
    ParallelOptimization opt;
    
    if (samples.empty()) {
        return opt;
    }
    
    double totalThreads = 0.0;
    size_t validSamples = 0;
    
    for (const auto& sample : samples) {
        if (sample.activeThreads > 0) {
            totalThreads += static_cast<double>(sample.activeThreads);
            validSamples++;
        }
    }
    
    if (validSamples > 0) {
        double avgThreads = totalThreads / static_cast<double>(validSamples);
        
        // Set optimal thread count
        if (avgThreads > 6.0) {
            opt.recommendedThreadCount = 8;  // Common hardware
            opt.recommendation = "8 threads provide good balance for most hardware";
        } else {
            opt.recommendedThreadCount = std::max(1U, static_cast<unsigned int>(avgThreads * 0.9));
            opt.recommendation = "Tune thread count for your specific hardware";
        }
    }
    
    opt.enableWorkStealing = true;
    opt.recommendedGrainSize = 100;
    
    return opt;
}

PerformanceOptimizer::OptimizationPlan PerformanceOptimizer::generateOptimizationPlan(const PerformanceAnalysis& analysis) const {
    OptimizationPlan plan;
    
    plan.name = "Performance Optimization Plan";
    
    // Estimate expected improvements based on current score
    if (analysis.score > 0.9) {
        plan.expectedSpeedup = 1.05;  // 5% improvement
        plan.expectedMemoryReduction = 0.10;  // 10% reduction
        plan.expectedNumericalImprovement = 0.05;  // 5% improvement
    } else if (analysis.score > 0.7) {
        plan.expectedSpeedup = 1.20;  // 20% improvement
        plan.expectedMemoryReduction = 0.25;  // 25% reduction
        plan.expectedNumericalImprovement = 0.15;  // 15% improvement
    } else if (analysis.score > 0.5) {
        plan.expectedSpeedup = 1.50;  // 50% improvement
        plan.expectedMemoryReduction = 0.50;  // 50% reduction
        plan.expectedNumericalImprovement = 0.25;  // 25% improvement
    } else {
        plan.expectedSpeedup = 2.0;  // 100% improvement
        plan.expectedMemoryReduction = 0.75;  // 75% reduction
        plan.expectedNumericalImprovement = 0.5;  // 50% improvement
    }
    
    // Generate actions based on analysis
    for (const auto& rec : analysis.recommendations) {
        plan.actions.push_back("Action: " + rec);
    }
    
    // Add implementation effort estimate
    if (plan.actions.empty()) {
        plan.estimatedImplementationEffort = 1;  // 1 hour
    } else if (plan.actions.size() <= 3) {
        plan.estimatedImplementationEffort = 4;  // 4 hours
    } else if (plan.actions.size() <= 6) {
        plan.estimatedImplementationEffort = 8;  // 8 hours
    } else {
        plan.estimatedImplementationEffort = 16;  // 16 hours
    }
    
    return plan;
}

// Helper methods for PerformanceOptimizer

double PerformanceOptimizer::estimateSIMDOptimizationImpact(double currentEfficiency) const {
    if (currentEfficiency < 0.5) return 0.3;  // 30% improvement possible
    if (currentEfficiency < 0.7) return 0.15; // 15% improvement possible
    return 0.05; // 5% improvement possible
}

double PerformanceOptimizer::estimateMemoryOptimizationImpact(double currentUsage) const {
    if (currentUsage > 100.0) return 0.25; // 25% reduction possible
    if (currentUsage > 50.0) return 0.15; // 15% reduction possible
    return 0.05; // 5% reduction possible
}

double PerformanceOptimizer::estimateParallelOptimizationImpact(double currentUtilization) const {
    if (currentUtilization < 0.5) return 0.3; // 30% improvement possible
    if (currentUtilization < 0.8) return 0.15; // 15% improvement possible
    return 0.05; // 5% improvement possible
}

// PerformanceDocumentationGenerator Implementation

PerformanceDocumentationGenerator::PerformanceDocumentationGenerator() {
}

std::string PerformanceDocumentationGenerator::generatePerformanceDocumentation() const {
    std::stringstream ss;
    
    ss << generateHeader("NLM Performance Documentation") << std::endl;
    ss << std::endl;
    
    ss << generateSection("Overview", 
        "This document provides comprehensive performance documentation for the NLM (Neural Learning Machine) project. "
        "It covers performance infrastructure, monitoring tools, analysis capabilities, and optimization recommendations.") << std::endl;
    
    ss << generateSection("Performance Infrastructure", generateAPIDocumentation()) << std::endl;
    
    ss << generateSection("Performance Monitoring", generateConfigDocumentation()) << std::endl;
    
    ss << generateSection("Performance Analysis", 
        "Performance analysis provides automated detection of bottlenecks and optimization opportunities.") << std::endl;
    
    ss << generateSection("Numerical Stability", generateNumericalStabilityGuide()) << std::endl;
    
    ss << generateSection("Benchmarking", generateBenchmarkDocumentation()) << std::endl;
    
    ss << generateSection("Optimization Guide", generateOptimizationGuide()) << std::endl;
    
    return ss.str();
}

std::string PerformanceDocumentationGenerator::generateHeader(const std::string& title) const {
    std::stringstream ss;
    ss << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    ss << "║" << std::setw(79) << std::left << " " << "║" << std::endl;
    ss << "║ " << std::setw(77) << std::left << title << " ║" << std::endl;
    ss << "║" << std::setw(79) << std::left << " " << "║" << std::endl;
    ss << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    return ss.str();
}

std::string PerformanceDocumentationGenerator::generateSection(const std::string& title, const std::string& content) const {
    std::stringstream ss;
    ss << std::endl;
    ss << "╔ " << title << " ╗" << std::endl;
    ss << content;
    ss << "╚ " << title << " ╝" << std::endl;
    return ss.str();
}

std::string PerformanceDocumentationGenerator::generateTable(const std::vector<std::vector<std::string>>& rows) const {
    if (rows.empty()) return "";
    
    std::stringstream ss;
    
    // Calculate column widths
    std::vector<size_t> colWidths(rows[0].size(), 0);
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            colWidths[i] = std::max(colWidths[i], row[i].size());
        }
    }
    
    // Generate header
    ss << "╔";
    for (size_t i = 0; i < colWidths.size(); ++i) {
        ss << std::string(colWidths[i] + 2, '═');
        if (i < colWidths.size() - 1) ss << "╦";
    }
    ss << "╗" << std::endl;
    
    // Generate header row (assuming first row is header)
    ss << "║";
    for (size_t i = 0; i < rows[0].size(); ++i) {
        ss << " " << std::left << std::setw(colWidths[i]) << rows[0][i] << " ";
        if (i < colWidths.size() - 1) ss << "║";
    }
    ss << "║" << std::endl;
    
    // Generate separator
    ss << "╠";
    for (size_t i = 0; i < colWidths.size(); ++i) {
        ss << std::string(colWidths[i] + 2, '═');
        if (i < colWidths.size() - 1) ss << "╬";
    }
    ss << "╣" << std::endl;
    
    // Generate data rows
    for (size_t r = 1; r < rows.size(); ++r) {
        ss << "║";
        for (size_t i = 0; i < rows[r].size(); ++i) {
            ss << " " << std::left << std::setw(colWidths[i]) << rows[r][i] << " ";
            if (i < colWidths.size() - 1) ss << "║";
        }
        ss << "║" << std::endl;
    }
    
    // Generate footer
    ss << "╚";
    for (size_t i = 0; i < colWidths.size(); ++i) {
        ss << std::string(colWidths[i] + 2, '═');
        if (i < colWidths.size() - 1) ss << "╩";
    }
    ss << "╝" << std::endl;
    
    return ss.str();
}

bool PerformanceDocumentationGenerator::saveDocumentation(const std::string& outputDir) const {
    // Create output directory
    if (mkdir(outputDir.c_str(), 0755) != 0 && errno != EEXIST) {
        return false;
    }
    
    // Save main documentation
    std::string mainFile = outputDir + "/PERFORMANCE.md";
    std::ofstream file(mainFile);
    if (!file) {
        return false;
    }
    
    file << generatePerformanceDocumentation() << std::endl;
    file.close();
    
    return true;
}

} // namespace nlm
