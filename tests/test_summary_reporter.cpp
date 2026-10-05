// Test SummaryReporter functionality
// Verifies summary generation, statistical analysis, and export features

#include "SummaryReporter.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace test_summary_reporter {

void testSummaryReporterConfiguration() {
    nlm::SummaryReporter reporter;
    
    // Test summary reporter configuration
    reporter.setUseColors(true);
    reporter.setIncludeStatistics(true);
    reporter.setIncludeRawData(true);
    reporter.setIncludeBrainData(true);
    reporter.setIncludePerformanceData(true);
    reporter.setIncludeExperimentData(true);
    
    std::cout << "✓ Summary reporter configuration test passed" << std::endl;
}

void testExperimentSummaryGeneration() {
    nlm::SummaryReporter reporter;
    
    // Test experiment result summary
    nlm::ExperimentResultSummary experiment;
    experiment.experimentName = "Neural Simulation Test";
    experiment.experimentType = "Simulation";
    experiment.success = true;
    experiment.executionTime = 120.5;
    experiment.memoryUsage = 512 * 1024 * 1024; // 512 MB
    experiment.peakMemoryUsage = 768 * 1024 * 1024; // 768 MB
    experiment.throughput = 1500.0;
    experiment.efficiency = 0.85;
    experiment.operationsCompleted = 100000;
    experiment.status = "completed";
    experiment.notes = "All simulations completed successfully";
    
    experiment.metrics.push_back({"accuracy", 0.95});
    experiment.metrics.push_back({"precision", 0.92});
    experiment.metrics.push_back({"recall", 0.88});
    
    std::string summary = reporter.generateExperimentSummary(experiment);
    assert(!summary.empty());
    assert(summary.find("Neural Simulation Test") != std::string::npos);
    assert(summary.find("completed") != std::string::npos);
    
    std::cout << "✓ Experiment summary generation test passed" << std::endl;
}

void testExperimentComparison() {
    nlm::SummaryReporter reporter;
    
    // Test experiment comparison
    nlm::ComparisonResult comparison;
    comparison.comparisonName = "Performance Comparison";
    
    nlm::ExperimentResultSummary exp1;
    exp1.experimentName = "Run A";
    exp1.success = true;
    exp1.executionTime = 100.0;
    exp1.efficiency = 0.85;
    
    nlm::ExperimentResultSummary exp2;
    exp2.experimentName = "Run B";
    exp2.success = true;
    exp2.executionTime = 120.0;
    exp2.efficiency = 0.90;
    
    nlm::ExperimentResultSummary exp3;
    exp3.experimentName = "Run C";
    exp3.success = false;
    exp3.executionTime = 90.0;
    exp3.efficiency = 0.80;
    
    comparison.runs.push_back(exp1);
    comparison.runs.push_back(exp2);
    comparison.runs.push_back(exp3);
    
    std::string comparisonReport = reporter.generateExperimentComparison(comparison);
    assert(!comparisonReport.empty());
    assert(comparisonReport.find("Performance Comparison") != std::string::npos);
    
    std::cout << "✓ Experiment comparison test passed" << std::endl;
}

void testExperimentStatistics() {
    nlm::SummaryReporter reporter;
    
    // Test statistical analysis
    nlm::StatisticalAnalysis stats;
    stats.analysisType = "Performance Analysis";
    
    // Add sample data
    stats.data = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    
    stats = reporter.calculateStatistics(stats.data);
    
    assert(stats.mean == 5.5);
    assert(stats.median == 5.5);
    assert(stats.minValue == 1.0);
    assert(stats.maxValue == 10.0);
    assert(stats.range == 9.0);
    
    std::string statsReport = reporter.generateExperimentStatistics(stats);
    assert(!statsReport.empty());
    assert(statsReport.find("Performance Analysis") != std::string::npos);
    
    std::cout << "✓ Experiment statistics test passed" << std::endl;
}

void testPerformanceSummaryGeneration() {
    nlm::SummaryReporter reporter;
    
    // Test performance summary
    nlm::PerformanceSummary performance;
    performance.benchmarkName = "CPU Benchmark";
    performance.avgExecutionTime = 45.2;
    performance.avgCPUUsage = 85.5;
    performance.avgMemoryUsage = 512.0;
    performance.avgThroughput = 2500.0;
    performance.avgLatency = 23.1;
    performance.reliability = 0.97;
    
    performance.executionTimes = {40.1, 45.2, 50.3, 39.9, 47.1};
    performance.memoryUsages = {512.0, 528.0, 540.0, 504.0, 516.0};
    
    std::string perfSummary = reporter.generatePerformanceSummary(performance);
    assert(!perfSummary.empty());
    assert(perfSummary.find("CPU Benchmark") != std::string::npos);
    
    std::cout << "✓ Performance summary generation test passed" << std::endl;
}

void testPerformanceReportGeneration() {
    nlm::SummaryReporter reporter;
    
    // Test performance report with multiple summaries
    nlm::PerformanceSummary perf1;
    perf1.benchmarkName = "Benchmark 1";
    perf1.avgExecutionTime = 100.0;
    perf1.avgCPUUsage = 80.0;
    perf1.avgMemoryUsage = 512.0;
    perf1.avgThroughput = 1000.0;
    
    nlm::PerformanceSummary perf2;
    perf2.benchmarkName = "Benchmark 2";
    perf2.avgExecutionTime = 150.0;
    perf2.avgCPUUsage = 90.0;
    perf2.avgMemoryUsage = 768.0;
    perf2.avgThroughput = 1200.0;
    
    std::vector<nlm::PerformanceSummary> summaries = {perf1, perf2};
    
    std::string perfReport = reporter.generatePerformanceReport(summaries);
    assert(!perfReport.empty());
    assert(perfReport.find("Performance Report") != std::string::npos);
    
    std::cout << "✓ Performance report generation test passed" << std::endl;
}

void testStatisticalAnalysisReportGeneration() {
    nlm::SummaryReporter reporter;
    
    // Test statistical analysis report
    nlm::StatisticalAnalysis stats;
    stats.analysisType = "Neural Activity Analysis";
    stats.data = {65.2, 66.8, 65.1, 67.3, 64.9, 66.1, 65.7, 66.4};
    stats = reporter.calculateStatistics(stats.data);
    
    std::string statsReport = reporter.generateStatisticalAnalysisReport(stats);
    assert(!statsReport.empty());
    assert(statsReport.find("Neural Activity Analysis") != std::string::npos);
    
    std::cout << "✓ Statistical analysis report generation test passed" << std::endl;
}

void testStatisticalAnalysisSummaryGeneration() {
    nlm::SummaryReporter reporter;
    
    // Test statistical analysis summary
    std::vector<nlm::StatisticalAnalysis> analyses;
    
    nlm::StatisticalAnalysis stats1;
    stats1.analysisType = "Performance Analysis";
    stats1.data = {1.0, 2.0, 3.0};
    stats1 = reporter.calculateStatistics(stats1.data);
    analyses.push_back(stats1);
    
    nlm::StatisticalAnalysis stats2;
    stats2.analysisType = "Memory Analysis";
    stats2.data = {512.0, 528.0, 516.0};
    stats2 = reporter.calculateStatistics(stats2.data);
    analyses.push_back(stats2);
    
    std::string statsSummary = reporter.generateStatisticalAnalysisSummary(analyses);
    assert(!statsSummary.empty());
    assert(statsSummary.find("Statistical Analysis Summary") != std::string::npos);
    
    std::cout << "✓ Statistical analysis summary generation test passed" << std::endl;
}

void testComparisonReportGeneration() {
    nlm::SummaryReporter reporter;
    
    // Test comparison report
    nlm::ComparisonResult comparison;
    comparison.comparisonName = "Experiment Comparison";
    
    nlm::ExperimentResultSummary exp1;
    exp1.experimentName = "Version 1";
    exp1.executionTime = 100.0;
    exp1.efficiency = 0.85;
    
    nlm::ExperimentResultSummary exp2;
    exp2.experimentName = "Version 2";
    exp2.executionTime = 80.0;
    exp2.efficiency = 0.90;
    
    comparison.runs = {exp1, exp2};
    
    std::string compReport = reporter.generateComparisonReport(comparison);
    assert(!compReport.empty());
    assert(compReport.find("Experiment Comparison") != std::string::npos);
    
    std::cout << "✓ Comparison report generation test passed" << std::endl;
}

void testPerformanceComparison() {
    nlm::SummaryReporter reporter;
    
    // Test performance comparison
    nlm::PerformanceSummary perf1;
    perf1.benchmarkName = "Benchmark A";
    perf1.avgExecutionTime = 100.0;
    
    nlm::PerformanceSummary perf2;
    perf2.benchmarkName = "Benchmark B";
    perf2.avgExecutionTime = 80.0;
    
    nlm::PerformanceSummary perf3;
    perf3.benchmarkName = "Benchmark C";
    perf3.avgExecutionTime = 120.0;
    
    std::vector<nlm::PerformanceSummary> summaries = {perf1, perf2, perf3};
    
    std::string perfCompReport = reporter.generatePerformanceComparison(summaries);
    assert(!perfCompReport.empty());
    assert(perfCompReport.find("Performance Comparison") != std::string::npos);
    
    std::cout << "✓ Performance comparison test passed" << std::endl;
}

void testComprehensiveReportGeneration() {
    nlm::SummaryReporter reporter;
    
    // Test comprehensive report
    std::vector<nlm::ExperimentResultSummary> experiments;
    
    nlm::ExperimentResultSummary exp1;
    exp1.experimentName = "Simulation 1";
    exp1.success = true;
    exp1.executionTime = 100.0;
    exp1.efficiency = 0.85;
    experiments.push_back(exp1);
    
    std::vector<nlm::PerformanceSummary> performances;
    
    nlm::PerformanceSummary perf1;
    perf1.benchmarkName = "Benchmark 1";
    perf1.avgExecutionTime = 50.0;
    performances.push_back(perf1);
    
    std::vector<nlm::StatisticalAnalysis> statistics;
    
    nlm::StatisticalAnalysis stats;
    stats.analysisType = "Neural Analysis";
    stats.data = {65.2, 66.8, 65.1};
    stats = reporter.calculateStatistics(stats.data);
    statistics.push_back(stats);
    
    std::string comprehensiveReport = reporter.generateComprehensiveReport(experiments, performances, statistics);
    assert(!comprehensiveReport.empty());
    assert(comprehensiveReport.find("Comprehensive NLM Report") != std::string::npos);
    
    std::cout << "✓ Comprehensive report generation test passed" << std::endl;
}

void testReportExport() {
    nlm::SummaryReporter reporter;
    
    // Test report export
    nlm::ExperimentResultSummary experiment;
    experiment.experimentName = "Export Test";
    experiment.success = true;
    
    bool exportSuccess = reporter.exportExperimentSummary("test_experiment.json", experiment, "json");
    assert(exportSuccess);
    
    std::cout << "✓ Report export test passed" << std::endl;
}

void testReportTemplates() {
    nlm::SummaryReporter reporter;
    
    // Test report templates
    std::string experimentTemplate = nlm::SummaryReporter::getExperimentReportTemplate();
    assert(!experimentTemplate.empty());
    assert(experimentTemplate.find("{name}") != std::string::npos);
    
    std::string performanceTemplate = nlm::SummaryReporter::getPerformanceReportTemplate();
    assert(!performanceTemplate.empty());
    assert(performanceTemplate.find("{benchmark}") != std::string::npos);
    
    std::string analysisTemplate = nlm::SummaryReporter::getAnalysisReportTemplate();
    assert(!analysisTemplate.empty());
    assert(analysisTemplate.find("{type}") != std::string::npos);
    
    std::string comparisonTemplate = nlm::SummaryReporter::getComparisonReportTemplate();
    assert(!comparisonTemplate.empty());
    assert(comparisonTemplate.find("{name}") != std::string::npos);
    
    std::cout << "✓ Report templates test passed" << std::endl;
}

void testDataManagement() {
    nlm::SummaryReporter reporter;
    
    // Test data management
    nlm::ExperimentResultSummary experiment;
    experiment.experimentName = "Data Management Test";
    
    nlm::PerformanceSummary performance;
    performance.benchmarkName = "Performance Test";
    
    nlm::StatisticalAnalysis analysis;
    analysis.analysisType = "Test Analysis";
    analysis.data = {1.0, 2.0, 3.0};
    
    reporter.addExperimentResult(experiment);
    reporter.addPerformanceResult(performance);
    reporter.addStatisticalAnalysis(analysis);
    
    // Test clearing data
    reporter.clearExperimentResults();
    reporter.clearPerformanceResults();
    reporter.clearStatisticalAnalyses();
    
    std::cout << "✓ Data management test passed" << std::endl;
}

void runAll() {
    std::cout << "=== Testing SummaryReporter ===" << std::endl;
    testSummaryReporterConfiguration();
    testExperimentSummaryGeneration();
    testExperimentComparison();
    testExperimentStatistics();
    testPerformanceSummaryGeneration();
    testPerformanceReportGeneration();
    testStatisticalAnalysisReportGeneration();
    testStatisticalAnalysisSummaryGeneration();
    testComparisonReportGeneration();
    testPerformanceComparison();
    testComprehensiveReportGeneration();
    testReportExport();
    testReportTemplates();
    testDataManagement();
    
    std::cout << "=== All SummaryReporter Tests PASSED ===" << std::endl;
}

} // namespace test_summary_reporter
