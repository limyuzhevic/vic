// Performance Monitoring Unit Tests using Google Test Framework
// Comprehensive unit tests for NLM performance monitoring system
// Tests performance metrics, profiling, and performance analysis

#include "fixtures/TestHelpers.hpp"
#include "performance_monitoring/PerformanceMonitor.hpp"
#include "performance_monitoring/Profiler.hpp"
#include "performance_monitoring/PerformanceMetrics.hpp"
#include "performance_monitoring/PerformanceAnalysis.hpp"
#include "performance_monitoring/PerformanceDashboard.hpp"
#include "performance_monitoring/PerformanceReporter.hpp"
#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <cmath>

namespace test {

// Test 8.1: Performance Monitor Initialization
TEST_F(PerformanceTest, PerformanceMonitorInitialization) {
    nlm::PerformanceMonitor monitor("TestMonitor");
    
    EXPECT_TRUE(monitor.isRunning());
    EXPECT_FALSE(monitor.isPaused());
    EXPECT_DOUBLE_EQ(monitor.getStartTime(), 0.0);  // Default start time
    EXPECT_DOUBLE_EQ(monitor.getCurrentFPS(), 0.0); // Default FPS
}

// Test 8.2: Performance Metrics Collection
TEST_F(PerformanceTest, PerformanceMetricsCollection) {
    nlm::PerformanceMonitor monitor("TestMetrics");
    
    // Record some metrics
    monitor.recordFrameTime(16.67);  // ~60 FPS
    monitor.recordFrameTime(16.67);
    monitor.recordFrameTime(16.67);
    
    auto metrics = monitor.getMetrics();
    EXPECT_GT(metrics.frameCount, 0);
    EXPECT_DOUBLE_EQ(metrics.averageFrameTime, 16.67);
    EXPECT_DOUBLE_EQ(metrics.currentFPS, 60.0);
}

// Test 8.3: Performance Profiling
TEST_F(PerformanceTest, PerformanceProfiling) {
    nlm::Profiler profiler("TestProfile");
    
    // Start profiling
    profiler.start();
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // Stop profiling
    profiler.stop();
    
    EXPECT_TRUE(profiler.isActive());
    EXPECT_GT(profiler.getLastDuration(), 10.0);  // Should be > 10ms
    EXPECT_GT(profiler.getTotalTime(), 0.0);
    EXPECT_GT(profiler.getMaxTime(), 0.0);
    EXPECT_GT(profiler.getMinTime(), 0.0);
    EXPECT_GT(profiler.getAverageTime(), 0.0);
}

// Test 8.4: Performance Metrics Accuracy
TEST_F(PerformanceTest, PerformanceMetricsAccuracy) {
    nlm::PerformanceMetrics metrics;
    
    // Add sample data
    metrics.addFrameTime(10.0);
    metrics.addFrameTime(20.0);
    metrics.addFrameTime(30.0);
    
    EXPECT_DOUBLE_EQ(metrics.getAverageFrameTime(), 20.0);
    EXPECT_DOUBLE_EQ(metrics.getMinFrameTime(), 10.0);
    EXPECT_DOUBLE_EQ(metrics.getMaxFrameTime(), 30.0);
    EXPECT_DOUBLE_EQ(metrics.currentFPS(), 30.0);  // 1000 / average frame time
}

// Test 8.5: Performance Analysis
TEST_F(PerformanceTest, PerformanceAnalysis) {
    nlm::PerformanceAnalysis analysis("TestAnalysis");
    
    // Add performance data
    for (int i = 0; i < 100; ++i) {
        analysis.addFrameTime(16.67 + (i % 10));  // Varying frame times
    }
    
    analysis.calculateStatistics();
    
    EXPECT_GT(analysis.getAverageFPS(), 0.0);
    EXPECT_GT(analysis.getMinFPS(), 0.0);
    EXPECT_GT(analysis.getMaxFPS(), 0.0);
    EXPECT_GT(analysis.getFPSVariability(), 0.0);
    EXPECT_GT(analysis.getDroppedFrames(), 0.0);
}

// Test 8.6: Performance Dashboard
TEST_F(PerformanceTest, PerformanceDashboard) {
    nlm::PerformanceDashboard dashboard("TestDashboard");
    
    // Add metrics to dashboard
    nlm::PerformanceMetrics metrics;
    metrics.addFrameTime(16.67);
    metrics.addFrameTime(16.67);
    metrics.addFrameTime(16.67);
    
    dashboard.addMetrics(metrics);
    
    EXPECT_GT(dashboard.getMetricCount(), 0);
    EXPECT_DOUBLE_EQ(dashboard.getAverageFPS(), 60.0);
}

// Test 8.7: Performance Reporter
TEST_F(PerformanceTest, PerformanceReporter) {
    nlm::PerformanceReporter reporter("TestReporter");
    
    // Add metrics
    nlm::PerformanceMetrics metrics;
    metrics.addFrameTime(16.67);
    reporter.addMetrics(metrics);
    
    EXPECT_GT(reporter.getMetricCount(), 0);
    EXPECT_NO_THROW(reporter.generateReport());  // Should not crash
}

// Test 8.8: Performance Monitoring Integration
TEST_F(PerformanceTest, PerformanceMonitoringIntegration) {
    nlm::PerformanceMonitor monitor("IntegrationMonitor");
    
    // Simulate realistic performance monitoring
    monitor.start();
    
    for (int i = 0; i < 10; ++i) {
        // Simulate variable work
        std::this_thread::sleep_for(std::chrono::milliseconds(5 + i * 2));
        monitor.recordFrameTime(5.0 + i * 2);
    }
    
    monitor.stop();
    
    auto metrics = monitor.getMetrics();
    EXPECT_GT(metrics.frameCount, 0);
    EXPECT_GT(metrics.averageFrameTime, 0.0);
    EXPECT_GT(metrics.currentFPS(), 0.0);
    EXPECT_LE(metrics.currentFPS(), 200.0);  // Should be reasonable
}

// Test 8.9: Performance Metrics Thread Safety
TEST_F(PerformanceTest, PerformanceMetricsThreadSafety) {
    nlm::PerformanceMetrics metrics;
    
    // Simulate concurrent access
    for (int i = 0; i < 100; ++i) {
        metrics.addFrameTime(16.67 + (i % 5));
    }
    
    EXPECT_GT(metrics.getFrameCount(), 0);
    EXPECT_GT(metrics.getAverageFrameTime(), 0.0);
}

// Test 8.10: Performance Monitoring Events
TEST_F(PerformanceTest, PerformanceMonitoringEvents) {
    nlm::PerformanceMonitor monitor("EventMonitor");
    
    // Monitor should handle events
    monitor.recordFrameTime(16.67);
    monitor.recordFrameTime(16.67);
    
    // Should not crash
    auto metrics = monitor.getMetrics();
    EXPECT_GT(metrics.frameCount, 0);
}

// Test 8.11: Performance Metrics Filtering
TEST_F(PerformanceTest, PerformanceMetricsFiltering) {
    nlm::PerformanceMetrics metrics;
    
    // Add data
    metrics.addFrameTime(16.67);
    metrics.addFrameTime(33.33);  // Slower frame
    metrics.addFrameTime(16.67);
    
    // Filter by threshold
    auto filtered = metrics.getFramesAboveThreshold(20.0);
    EXPECT_EQ(filtered.size(), 1);  // Only 33.33 is above 20.0
}

// Test 8.12: Performance Analysis Trends
TEST_F(PerformanceTest, PerformanceAnalysisTrends) {
    nlm::PerformanceAnalysis analysis("TrendAnalysis");
    
    // Add data showing trend
    for (int i = 0; i < 20; ++i) {
        // Decreasing frame time (improving performance)
        analysis.addFrameTime(30.0 - i * 0.5);
    }
    
    analysis.calculateStatistics();
    
    EXPECT_GT(analysis.getAverageFPS(), 0.0);
    EXPECT_GT(analysis.getMinFPS(), 0.0);
    EXPECT_GT(analysis.getMaxFPS(), 0.0);
}

// Test 8.13: Performance Dashboard Updates
TEST_F(PerformanceTest, PerformanceDashboardUpdates) {
    nlm::PerformanceDashboard dashboard("UpdateDashboard");
    
    // Update dashboard multiple times
    for (int i = 0; i < 5; ++i) {
        nlm::PerformanceMetrics metrics;
        metrics.addFrameTime(16.67 + i);
        dashboard.addMetrics(metrics);
        
        EXPECT_NO_THROW(dashboard.update());
    }
    
    EXPECT_GT(dashboard.getMetricCount(), 0);
}

// Test 8.14: Performance Reporter Output
TEST_F(PerformanceTest, PerformanceReporterOutput) {
    nlm::PerformanceReporter reporter("OutputReporter");
    
    // Add metrics
    nlm::PerformanceMetrics metrics;
    metrics.addFrameTime(16.67);
    metrics.addFrameTime(16.67);
    reporter.addMetrics(metrics);
    
    // Generate report should not crash
    EXPECT_NO_THROW(reporter.generateReport());
}

// Test 8.15: Performance Monitoring Accuracy
TEST_F(PerformanceTest, PerformanceMonitoringAccuracy) {
    nlm::PerformanceMonitor monitor("AccuracyMonitor");
    
    // Record known frame times
    double expectedFrameTime = 16.67;
    monitor.recordFrameTime(expectedFrameTime);
    
    auto metrics = monitor.getMetrics();
    EXPECT_DOUBLE_EQ(metrics.averageFrameTime, expectedFrameTime);
    EXPECT_DOUBLE_EQ(metrics.currentFPS(), 1000.0 / expectedFrameTime);
}

// Test 8.16: Performance Monitoring Boundaries
TEST_F(PerformanceTest, PerformanceMonitoringBoundaries) {
    nlm::PerformanceMetrics metrics;
    
    // Test boundary values
    metrics.addFrameTime(0.001);  // Very fast frame
    metrics.addFrameTime(1000.0); // Very slow frame
    
    EXPECT_DOUBLE_EQ(metrics.getMinFrameTime(), 0.001);
    EXPECT_DOUBLE_EQ(metrics.getMaxFrameTime(), 1000.0);
    EXPECT_NEAR(metrics.getAverageFrameTime(), 500.0005, 0.001);
}

// Test 8.17: Performance Monitoring Memory Usage
TEST_F(PerformanceTest, PerformanceMonitoringMemoryUsage) {
    nlm::PerformanceMonitor monitor("MemoryMonitor");
    
    // Add many metrics
    for (int i = 0; i < 1000; ++i) {
        monitor.recordFrameTime(16.67);
    }
    
    auto metrics = monitor.getMetrics();
    EXPECT_EQ(metrics.frameCount, 1000);
    EXPECT_DOUBLE_EQ(metrics.averageFrameTime, 16.67);
}

// Test 8.18: Performance Analysis Reporting
TEST_F(PerformanceTest, PerformanceAnalysisReporting) {
    nlm::PerformanceAnalysis analysis("ReportingAnalysis");
    
    // Add data
    for (int i = 0; i < 50; ++i) {
        analysis.addFrameTime(16.67 + (i % 3));
    }
    
    analysis.calculateStatistics();
    
    // Should be able to generate report without crashing
    EXPECT_NO_THROW(analysis.generateReport());
    EXPECT_GT(analysis.getAverageFPS(), 0.0);
    EXPECT_GT(analysis.getMinFPS(), 0.0);
    EXPECT_GT(analysis.getMaxFPS(), 0.0);
}

// Parameterized test for different performance scenarios
class PerformanceScenarioTest : public PerformanceTest, public ::testing::WithParamInterface<double> {
};

TEST_P(PerformanceScenarioTest, DifferentPerformanceScenarios) {
    double frameTime = GetParam();
    nlm::PerformanceMetrics metrics;
    
    metrics.addFrameTime(frameTime);
    
    EXPECT_DOUBLE_EQ(metrics.getAverageFrameTime(), frameTime);
    EXPECT_DOUBLE_EQ(metrics.currentFPS(), 1000.0 / frameTime);
}

INSTANTIATE_TEST_SUITE_P(
    PerformanceScenarios,
    PerformanceScenarioTest,
    ::testing::Values(1.0, 5.0, 10.0, 16.67, 33.33, 50.0, 100.0, 500.0, 1000.0)
);

// Performance stress test
TEST_F(PerformanceTest, PerformanceStressTest) {
    nlm::PerformanceMonitor monitor("StressMonitor");
    
    // High stress test
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        monitor.recordFrameTime(0.1);  // Very fast frames
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    auto metrics = monitor.getMetrics();
    EXPECT_EQ(metrics.frameCount, 10000);
    EXPECT_DOUBLE_EQ(metrics.averageFrameTime, 0.1);
    
    // Should complete quickly
    EXPECT_LT(duration.count(), 100);  // Should complete in under 100ms
}

} // namespace test