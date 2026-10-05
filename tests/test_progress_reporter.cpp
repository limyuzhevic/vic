// Test ProgressReporter functionality
// Verifies progress tracking, monitoring, and reporting features

#include "ProgressReporter.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_progress_reporter {

void testProgressBarStyle() {
    nlm::ProgressReporter reporter;
    
    // Test progress bar style setting
    reporter.setProgressBarStyle(nlm::ProgressBarStyle::Simple);
    reporter.setShowPercentage(true);
    reporter.setShowTimeRemaining(true);
    reporter.setUpdateInterval(std::chrono::milliseconds(100));
    reporter.setUseColors(false);
    
    std::cout << "✓ ProgressReporter configuration test passed" << std::endl;
}

void testProgressTracking() {
    nlm::ProgressReporter reporter;
    
    // Test simple progress tracking
    reporter.startProgress("Test Operation", 100);
    for (int i = 0; i <= 100; i += 10) {
        reporter.updateProgress(i, "Processing step " + std::to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    reporter.stopProgress();
    
    std::cout << "✓ Progress tracking test passed" << std::endl;
}

void testMonitoring() {
    nlm::ProgressReporter reporter;
    
    // Test monitoring functionality
    reporter.startMonitoring("Resource Monitoring", std::chrono::seconds(2));
    
    for (double progress = 0.0; progress <= 1.0; progress += 0.2) {
        reporter.updateMonitoring(progress, "Monitoring system resources");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    reporter.stopMonitoring();
    
    std::cout << "✓ Monitoring test passed" << std::endl;
}

void testExperimentProgress() {
    nlm::ProgressReporter reporter;
    
    // Test experiment progress reporting
    reporter.startExperimentProgress("Neural Simulation", 5);
    
    for (int phase = 1; phase <= 5; ++phase) {
        reporter.updateExperimentPhase(phase, "Phase " + std::to_string(phase), 
                                        static_cast<double>(phase) / 5.0 * 100.0);
        
        for (int step = 1; step <= 3; ++step) {
            reporter.updateExperimentStep(step, "Executing neural computation");
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    reporter.stopExperimentProgress();
    
    std::cout << "✓ Experiment progress test passed" << std::endl;
}

void testFileOperationProgress() {
    nlm::ProgressReporter reporter;
    
    // Test file operation progress
    reporter.startFileOperationProgress("File Copy", "test.bin", 1024 * 1024); // 1MB
    
    for (size_t bytes = 0; bytes <= 1024 * 1024; bytes += 100 * 1024) {
        reporter.updateFileOperationProgress(bytes, "test.bin");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    reporter.stopFileOperationProgress();
    
    std::cout << "✓ File operation progress test passed" << std::endl;
}

void testProgressReportGeneration() {
    nlm::ProgressReporter reporter;
    
    // Test progress report generation
    reporter.startProgress("Test Report", 50);
    reporter.updateProgress(25, "Mid-point progress");
    reporter.updateProgress(50, "Completed");
    
    std::string report = reporter.generateProgressReport();
    assert(!report.empty());
    assert(report.find("Test Report") != std::string::npos);
    assert(report.find("25.0%") != std::string::npos);
    
    reporter.stopProgress();
    
    std::cout << "✓ Progress report generation test passed" << std::endl;
}

void testProgressExport() {
    nlm::ProgressReporter reporter;
    
    // Test progress data export
    reporter.startProgress("Export Test", 10);
    for (int i = 0; i <= 10; ++i) {
        reporter.updateProgress(i);
    }
    
    bool exportSuccess = reporter.exportProgressData("test_progress.csv", "csv");
    assert(exportSuccess);
    
    reporter.stopProgress();
    
    std::cout << "✓ Progress export test passed" << std::endl;
}

void testCallback() {
    nlm::ProgressReporter reporter;
    
    // Test progress callback
    bool callbackCalled = false;
    reporter.setProgressCallback([&callbackCalled](double progress, const std::string&, const std::string&) {
        callbackCalled = true;
    });
    
    reporter.startProgress("Callback Test", 10);
    reporter.updateProgress(5, "Callback test");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(150)); // Wait for callback
    
    reporter.stopProgress();
    
    assert(callbackCalled);
    std::cout << "✓ Progress callback test passed" << std::endl;
}

void runAll() {
    std::cout << "=== Testing ProgressReporter ===" << std::endl;
    testProgressBarStyle();
    testProgressTracking();
    testMonitoring();
    testExperimentProgress();
    testFileOperationProgress();
    testProgressReportGeneration();
    testProgressExport();
    testCallback();
    
    std::cout << "=== All ProgressReporter Tests PASSED ===" << std::endl;
}

} // namespace test_progress_reporter
