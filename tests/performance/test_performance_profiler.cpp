# Performance Profiling Tests
// Enhanced test framework for PerformanceProfiler testing
// Tests comprehensive profiling capabilities including metrics collection,
// neural activity monitoring, bottleneck detection, and performance analysis

#include "profiling/PerformanceProfiler.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <random>

namespace test_performance_profiler {

void testPerformanceProfilerInitialization() {
    std::cout << "=== Testing PerformanceProfiler Initialization ===\n";
    
    nlm::PerformanceProfiler profiler;
    
    // Test initialization
    assert(!profiler.isInitialized());
    assert(!profiler.isProfiling());
    assert(!profiler.isRealTimeMonitoringEnabled());
    assert(profiler.isBenchmarking() == false);
    
    assert(profiler.initialize());
    assert(profiler.isInitialized());
    
    // Test default configuration
    assert(profiler.getCurrentMetrics().cpuUsage == 0.0);
    assert(profiler.getCurrentMetrics().memoryUsed == 0);
    assert(profiler.getCurrentMetrics().totalSpikes == 0);
    
    // Test profiling modes
    profiler.setProfilingMode(nlm::ProfilingMode::Realtime);
    profiler.setSamplingInterval(0.5);
    profiler.setMaxHistorySize(1000);
    profiler.setEnableResourceMonitoring(true);
    profiler.setEnableNeuralActivityMonitoring(true);
    profiler.setEnableBottleneckDetection(true);
    profiler.setEnableRegressionDetection(true);
    
    profiler.shutdown();
    assert(!profiler.isInitialized());
    
    std::cout << "✓ PerformanceProfiler initialization tests passed\n";
}

void testPerformanceMetricsCollection() {
    std::cout << "\n=== Testing Performance Metrics Collection ===\n";
    
    nlm::PerformanceProfiler profiler;
    profiler.initialize();
    profiler.startProfiling();
    
    // Wait for some metrics collection
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    auto metrics = profiler.getCurrentMetrics();
    
    // Verify metrics collection
    assert(metrics.cpuUsage >= 0.0 && metrics.cpuUsage <= 100.0);
    assert(metrics.memoryUsed >= 0);
    assert(metrics.memoryPeak >= 0);
    assert(metrics.simulationStep == 0); // Default
    assert(metrics.totalSpikes >= 0);
    assert(metrics.activeNeurons >= 0);
    assert(metrics.firingNeurons >= 0);
    
    // Test recent metrics retrieval
    auto recentMetrics = profiler.getRecentMetrics(10);
    assert(recentMetrics.size() > 0);
    
    // Test time series retrieval
    auto timeSeries = profiler.getTimeSeries(10);
    assert(timeSeries.size() > 0);
    
    profiler.stopProfiling();
    profiler.shutdown();
    
    std::cout << "✓ Performance metrics collection tests passed\n";
}

void testNeuralActivityMonitoring() {
    std::cout << "\n=== Testing Neural Activity Monitoring ===\n";
    
    nlm::PerformanceProfiler profiler;
    profiler.initialize();
    profiler.setEnableNeuralActivityMonitoring(true);
    profiler.startProfiling();
    
    // Wait for activity monitoring
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Capture activity heatmap
    profiler.captureActivityHeatmap();
    
    // Get heatmaps
    auto heatmaps = profiler.getActivityHeatmaps(5);
    assert(heatmaps.size() > 0);
    
    // Verify heatmap data
    for (const auto& heatmap : heatmaps) {
        assert(heatmap.step >= 0);
        assert(heatmap.timestamp >= 0.0);
        assert(heatmap.regionCount > 0);
        assert(heatmap.neuronsPerRegion > 0);
        assert(heatmap.activityData.size() == static_cast<size_t>(heatmap.regionCount * heatmap.neuronsPerRegion));
        assert(heatmap.normalizedData.size() == heatmap.activityData.size());
        
        // Verify normalized data is between 0 and 1
        for (float val : heatmap.normalizedData) {
            assert(val >= 0.0f && val <= 1.0f);
        }
    }
    
    // Test heatmap region analysis
    for (int regionId = 0; regionId < heatmaps[0].regionCount; ++regionId) {
        auto regionData = profiler.analyzeHeatmapRegion(regionId);
        assert(regionData.size() == heatmaps[0].neuronsPerRegion);
        
        double activityScore = profiler.calculateHeatmapActivityScore(regionId);
        assert(activityScore >= 0.0 && activityScore <= 1.0);
    }
    
    profiler.stopProfiling();
    profiler.shutdown();
    
    std::cout << "✓ Neural activity monitoring tests passed\n";
}

void testBottleneckDetection() {
    std::cout << "\n=== Testing Bottleneck Detection ===\n";
    
    nlm::PerformanceProfiler profiler;
    profiler.initialize();
    profiler.startProfiling();
    
    // Let it run for a bit to collect data
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    // Analyze bottlenecks
    auto bottleneck = profiler.analyzeBottlenecks();
    
    // Verify bottleneck analysis
    assert(bottleneck.severity >= 0.0 && bottleneck.severity <= 1.0);
    
    // Test performance scoring
    double score = profiler.calculatePerformanceScore();
    assert(score >= 0.0 && score <= 100.0);
    
    profiler.stopProfiling();
    profiler.shutdown();
    
    std::cout << "✓ Bottleneck detection tests passed\n";
}

void testProgressReporting() {
    std::cout << "\n=== Testing Progress Reporting ===\n";
    
    nlm::PerformanceProfiler profiler;
    profiler.initialize();
    
    // Set progress callback
    bool callbackCalled = false;
    profiler.setProgressCallback([&](const nlm::ProgressReport& report) {
        callbackCalled = true;
        assert(report.progress >= 0.0 && report.progress <= 1.0);
        assert(report.currentStep >= 0);
        assert(report.totalSteps > 0);
        assert(report.estimatedTimeRemaining >= 0.0);
        assert(report.progressPerSecond >= 0.0);
    });
    
    // Set progress
    profiler.setProgress(0.5, "Test operation", 50, 100);
    
    auto progress = profiler.getCurrentProgress();
    assert(progress.progress == 0.5);
    assert(progress.message == "Test operation");
    assert(progress.currentStep == 50);
    assert(progress.totalSteps == 100);
    
    profiler.shutdown();
    
    std::cout << "✓ Progress reporting tests passed\n";
}

void testExportFunctionality() {
    std::cout << "\n=== Testing Export Functionality ===\n";
    
    nlm::PerformanceProfiler profiler;
    profiler.initialize();
    profiler.setOutputDirectory("/tmp/test_profiling");
    
    profiler.startProfiling();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    profiler.stopProfiling();
    
    // Test metrics export
    assert(profiler.exportMetrics("/tmp/test_metrics.csv"));
    
    // Test heatmaps export
    profiler.captureActivityHeatmap();
    assert(profiler.exportHeatmaps("/tmp/test_heatmaps.csv"));
    
    // Test time series export
    assert(profiler.exportTimeSeries("/tmp/test_time_series.csv"));
    
    // Test analysis report export
    assert(profiler.exportAnalysisReport("/tmp/test_analysis_report.txt"));
    
    // Verify files were created
    assert(std::filesystem::exists("/tmp/test_metrics.csv"));
    assert(std::filesystem::exists("/tmp/test_heatmaps.csv"));
    assert(std::filesystem::exists("/tmp/test_time_series.csv"));
    assert(std::filesystem::exists("/tmp/test_analysis_report.txt"));
    
    // Test session management
    assert(profiler.saveSession("test_session"));
    
    auto sessions = profiler.listSessions();
    assert(std::find(sessions.begin(), sessions.end(), "test_session") != sessions.end());
    
    assert(profiler.loadSession("test_session"));
    
    assert(profiler.deleteSession("test_session"));
    
    sessions = profiler.listSessions();
    assert(std::find(sessions.begin(), sessions.end(), "test_session") == sessions.end());
    
    profiler.shutdown();
    
    std::cout << "✓ Export functionality tests passed\n";
}

void testRealTimeMonitoring() {
    std::cout << "\n=== Testing Real-time Monitoring ===\n";
    
    nlm::PerformanceProfiler profiler;
    profiler.initialize();
    
    // Enable real-time monitoring
    profiler.enableRealTimeMonitoring(true);
    assert(profiler.isRealTimeMonitoringEnabled());
    
    // Wait for real-time updates
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    // Get updated metrics
    auto metrics = profiler.getCurrentMetrics();
    assert(metrics.cpuUsage >= 0.0);
    
    // Disable real-time monitoring
    profiler.enableRealTimeMonitoring(false);
    assert(!profiler.isRealTimeMonitoringEnabled());
    
    profiler.shutdown();
    
    std::cout << "✓ Real-time monitoring tests passed\n";
}

void testBenchmarking() {
    std::cout << "\n=== Testing Benchmarking ===\n";
    
    nlm::PerformanceProfiler profiler;
    profiler.initialize();
    
    // Start benchmark
    profiler.startBenchmark("test_benchmark");
    assert(profiler.isBenchmarking());
    assert(profiler.getCurrentBenchmarkName() == "test_benchmark");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // End benchmark
    profiler.endBenchmark();
    assert(!profiler.isBenchmarking());
    assert(profiler.getCurrentBenchmarkName().empty());
    
    profiler.shutdown();
    
    std::cout << "✓ Benchmarking tests passed\n";
}

void testEventDrivenMetrics() {
    std::cout << "\n=== Testing Event-driven Metrics ===\n";
    
    nlm::PerformanceProfiler profiler;
    profiler.initialize();
    profiler.startProfiling();
    
    // Get initial metrics
    auto initialMetrics = profiler.getCurrentMetrics();
    uint64_t initialSpikes = initialMetrics.totalSpikes;
    
    // Simulate events
    profiler.onSpikeGenerated();
    profiler.onNeuronActivated(1);
    profiler.onNeuronActivated(2);
    
    // Update to capture events
    profiler.update();
    
    auto updatedMetrics = profiler.getCurrentMetrics();
    assert(updatedMetrics.totalSpikes > initialSpikes);
    
    // Test checkpoint events
    profiler.onCheckpointSaved("/tmp/test_checkpoint", 1024);
    profiler.onCheckpointLoaded("/tmp/test_checkpoint");
    
    profiler.stopProfiling();
    profiler.shutdown();
    
    std::cout << "✓ Event-driven metrics tests passed\n";
}

void runAll() {
    testPerformanceProfilerInitialization();
    testPerformanceMetricsCollection();
    testNeuralActivityMonitoring();
    testBottleneckDetection();
    testProgressReporting();
    testExportFunctionality();
    testRealTimeMonitoring();
    testBenchmarking();
    testEventDrivenMetrics();
    
    std::cout << "\n=== All PerformanceProfiler Tests Completed Successfully ===\n";
}

} // namespace test_performance_profiler
