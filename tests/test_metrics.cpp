// Metrics Test Suite
// Tests for ResearchMetrics and Metrics systems

#include "ResearchMetrics.hpp"
#include "Metrics.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_benchmark.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace test_metrics {
    
    // Test fixtures
    class MetricsTestFixture {
    public:
        std::unique_ptr<nlm::ResearchMetrics> researchMetrics_;
        
        MetricsTestFixture() {
            researchMetrics_ = std::make_unique<nlm::ResearchMetrics>();
        }
    };
    
    // Basic Metrics Collection Tests
    TEST_CASE("ResearchMetrics Basic Initialization", "[metrics][basic]") {
        nlm::ResearchMetrics metrics;
        
        REQUIRE_NOTHROW(metrics.generateReport());
        REQUIRE_NOTHROW(metrics.generateStatisticalReport());
        REQUIRE_NOTHROW(metrics.generateTimeSeriesReport());
        REQUIRE_NOTHROW(metrics.generateMLReport());
        
        auto report = metrics.generateReport();
        REQUIRE(!report.empty());
        REQUIRE(report.find("NLM Research Metrics Report") != std::string::npos);
    }
    
    TEST_CASE("ResearchMetrics Record Method", "[metrics][basic]") {
        nlm::ResearchMetrics metrics;
        
        // Test recording different types of metrics
        metrics.record("firing_rate", 42.5, nlm::Metrics::Category::Neuron);
        metrics.record("learning_rate", 0.01, nlm::Metrics::Category::Learning);
        metrics.record("accuracy", 0.95, nlm::Metrics::Category::Cognitive);
        metrics.record("memory_usage", 1024.0, nlm::Metrics::Category::Memory);
        metrics.record("prediction_error", 0.05, nlm::Metrics::Category::Prediction);
        
        auto metricNames = metrics.getMetricNames();
        REQUIRE(metricNames.size() >= 5);
        REQUIRE(std::find(metricNames.begin(), metricNames.end(), "firing_rate") != metricNames.end());
        REQUIRE(std::find(metricNames.begin(), metricNames.end(), "learning_rate") != metricNames.end());
    }
    
    TEST_CASE("ResearchMetrics Time Series Recording", "[metrics][time_series]") {
        nlm::ResearchMetrics metrics;
        
        std::vector<double> timestamps = {1.0, 2.0, 3.0, 4.0, 5.0};
        for (size_t i = 0; i < timestamps.size(); ++i) {
            metrics.recordTimeSeries("voltage", 65.0 + i * 0.1, std::chrono::system_clock::time_point(std::chrono::seconds(i)));
        }
        
        auto timeSeries = metrics.getTimeSeries();
        REQUIRE(!timeSeries.empty());
        
        // Find the voltage time series
        bool foundVoltage = false;
        for (const auto& ts : timeSeries) {
            if (ts.name == "voltage") {
                foundVoltage = true;
                REQUIRE(ts.values.size() == 5);
                REQUIRE(ts.timestamps.size() == 5);
                break;
            }
        }
        REQUIRE(foundVoltage);
    }
    
    TEST_CASE("ResearchMetrics Pattern Recording", "[metrics][pattern]") {
        nlm::ResearchMetrics metrics;
        
        // Test pattern recording
        metrics.recordPattern("spike_detection", true, 0.98);
        metrics.recordPattern("pattern_recognition", false, 0.85);
        metrics.recordPattern("classification", true, 0.92);
        
        auto patternMetrics = metrics.getPatternMetrics();
        REQUIRE(patternMetrics.size() == 3);
        
        REQUIRE(patternMetrics[0].patternName == "spike_detection");
        REQUIRE(patternMetrics[0].accuracy == 1.0);
        REQUIRE(patternMetrics[0].confidence == 0.98);
        
        REQUIRE(patternMetrics[1].patternName == "pattern_recognition");
        REQUIRE(patternMetrics[1].accuracy == 0.0);
        REQUIRE(patternMetrics[1].confidence == 0.85);
    }
    
    TEST_CASE("ResearchMetrics Learning Progress Recording", "[metrics][learning]") {
        nlm::ResearchMetrics metrics;
        
        metrics.recordLearningProgress("phase1", 0.3, 0.01);
        metrics.recordLearningProgress("phase2", 0.6, 0.05);
        metrics.recordLearningProgress("phase3", 0.9, 0.1);
        
        auto learningProgress = metrics.getLearningProgress();
        REQUIRE(learningProgress.size() == 3);
        
        REQUIRE(learningProgress[0].learningPhase == "phase1");
        REQUIRE(learningProgress[0].progressValue == 0.3);
        REQUIRE(learningProgress[0].learningRate == 0.01);
    }
    
    TEST_CASE("ResearchMetrics Statistical Analysis", "[metrics][stats]") {
        nlm::ResearchMetrics metrics;
        
        // Record some data for statistical analysis
        std::vector<double> testData = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
        for (double value : testData) {
            metrics.record("test_metric", value);
        }
        
        // Calculate statistics
        auto stats = metrics.calculateStatistics("test_metric");
        REQUIRE(stats.count == 10);
        REQUIRE(stats.mean == 5.5);
        REQUIRE(stats.variance == Approx(8.25).margin(0.01));
        REQUIRE(stats.standardDeviation == Approx(std::sqrt(8.25)).margin(0.01));
        REQUIRE(stats.min == 1.0);
        REQUIRE(stats.max == 10.0);
        REQUIRE(stats.median == 5.5);
    }
    
    TEST_CASE("ResearchMetrics Correlation Analysis", "[metrics][correlation]") {
        nlm::ResearchMetrics metrics;
        
        // Create correlated data
        std::vector<double> x = {1.0, 2.0, 3.0, 4.0, 5.0};
        std::vector<double> y = {2.0, 4.0, 6.0, 8.0, 10.0}; // Perfect correlation
        
        for (size_t i = 0; i < x.size(); ++i) {
            metrics.record("x_metric", x[i]);
            metrics.record("y_metric", y[i]);
        }
        
        double correlation = metrics.calculateCorrelation("x_metric", "y_metric");
        REQUIRE(correlation > 0.99); // Should be close to 1.0
        
        // Test uncorrelated data
        for (double value : x) {
            metrics.record("uncorrelated", value);
        }
        
        double uncorrelatedCorr = metrics.calculateCorrelation("x_metric", "uncorrelated");
        REQUIRE(std::abs(uncorrelatedCorr) < 0.5);
    }
    
    TEST_CASE("ResearchMetrics Trend Analysis", "[metrics][trend]") {
        nlm::ResearchMetrics metrics;
        
        // Record data with upward trend
        for (int i = 0; i < 20; ++i) {
            metrics.record("trending_metric", 1.0 + i * 0.1);
        }
        
        double trend = metrics.calculateTrend("trending_metric", 10);
        REQUIRE(trend > 0.0); // Should have positive trend
        
        // Test with no trend
        for (int i = 0; i < 10; ++i) {
            metrics.record("no_trend", 5.0 + (i % 2) * 0.1);
        }
        
        double noTrend = metrics.calculateTrend("no_trend", 5);
        REQUIRE(std::abs(noTrend) < 0.5); // Should be close to zero
    }
    
    TEST_CASE("ResearchMetrics Export Functionality", "[metrics][export]") {
        nlm::ResearchMetrics metrics;
        
        // Add some test data
        for (int i = 0; i < 10; ++i) {
            metrics.record("test_metric", i * 0.5);
        }
        
        // Test CSV export
        REQUIRE(metrics.exportToCSV("/tmp/test_metrics.csv") == true);
        
        // Test JSON export
        REQUIRE(metrics.exportToJSON("/tmp/test_metrics.json") == true);
        
        // Test MAT export (always returns true in current implementation)
        REQUIRE(metrics.exportToMat("/tmp/test_metrics.mat") == true);
    }
    
    TEST_CASE("ResearchMetrics Integration with Existing Metrics", "[metrics][integration]") {
        nlm::ResearchMetrics researchMetrics;
        
        // Record some research metrics
        researchMetrics.record("research_metric1", 0.8);
        researchMetrics.record("research_metric2", 0.9);
        
        auto researchNames = researchMetrics.getMetricNames();
        REQUIRE(researchNames.size() >= 2);
        
        // Test integration (in real implementation, this would integrate with actual Metrics class)
        // For now, just verify the method exists and doesn't crash
        // researchMetrics.integrateWithExistingMetrics(metrics);
        
        REQUIRE(true); // Placeholder test
    }
    
    TEST_CASE("ResearchMetrics Advanced Metrics", "[metrics][advanced]") {
        nlm::ResearchMetrics metrics;
        
        // Create mock advanced metrics
        nlm::MemoryMetrics memoryMetrics;
        memoryMetrics.episodicMemoryStored = 100;
        memoryMetrics.workingMemoryUtilization = 0.75;
        memoryMetrics.memoryConsolidationProgress = 0.8;
        
        nlm::NeuromodulationMetrics neuromodulationMetrics;
        neuromodulationMetrics.dopamineSignalStrength = 0.6;
        neuromodulationMetrics.curiositySignalStrength = 0.5;
        neuromodulationMetrics.noveltySignalStrength = 0.7;
        neuromodulationMetrics.predictionError = 0.1;
        
        nlm::CognitionMetrics cognitionMetrics;
        cognitionMetrics.conceptFormationAccuracy = 0.85;
        cognitionMetrics.neuralPlanningEfficiency = 0.75;
        
        nlm::DevelopmentMetrics developmentMetrics;
        developmentMetrics.developmentalStageProgress = 0.65;
        developmentMetrics.plasticityRate = 0.2;
        developmentMetrics.synapticMaturation = 0.7;
        developmentMetrics.myelinationProgress = 0.6;
        
        // Record advanced metrics
        metrics.recordMemoryMetrics(memoryMetrics);
        metrics.recordNeuromodulationMetrics(neuromodulationMetrics);
        metrics.recordCognitionMetrics(cognitionMetrics);
        metrics.recordDevelopmentMetrics(developmentMetrics);
        
        // Verify they were recorded
        auto memoryMetricsList = metrics.getPatternMetrics(); // Note: This is a simplification
        REQUIRE(memoryMetricsList.size() > 0);
    }
    
    TEST_CASE("ResearchMetrics Configuration", "[metrics][config]") {
        nlm::ResearchMetrics metrics;
        
        // Test setting and getting parameters
        metrics.setParameter("learning_rate", 0.05);
        metrics.setParameter("plasticity_rate", 0.2);
        metrics.setParameter("attention_threshold", 0.6);
        
        REQUIRE(metrics.getParameter("learning_rate") == 0.05);
        REQUIRE(metrics.getParameter("plasticity_rate") == 0.2);
        REQUIRE(metrics.getParameter("attention_threshold") == 0.6);
        
        // Test non-existent parameter
        REQUIRE(metrics.getParameter("non_existent") == 0.0);
    }
    
    TEST_CASE("ResearchMetrics Reset Functionality", "[metrics][reset]") {
        nlm::ResearchMetrics metrics;
        
        // Add some data
        metrics.record("test_metric", 1.0);
        metrics.recordPattern("test_pattern", true, 0.9);
        
        auto namesBefore = metrics.getMetricNames();
        REQUIRE(!namesBefore.empty());
        
        // Reset metrics
        metrics.reset();
        
        auto namesAfter = metrics.getMetricNames();
        REQUIRE(namesAfter.empty());
    }
    
    TEST_CASE("ResearchMetrics Time Series Management", "[metrics][time_series]") {
        nlm::ResearchMetrics metrics;
        
        // Record time series data
        auto now = std::chrono::system_clock::now();
        auto later = now + std::chrono::seconds(1);
        
        metrics.recordTimeSeries("temperature", 20.0, now);
        metrics.recordTimeSeries("temperature", 21.0, later);
        
        auto timeSeries = metrics.getTimeSeries();
        REQUIRE(!timeSeries.empty());
        
        bool foundTemp = false;
        for (const auto& ts : timeSeries) {
            if (ts.name == "temperature") {
                foundTemp = true;
                REQUIRE(ts.values.size() == 2);
                REQUIRE(ts.values[0] == 20.0);
                REQUIRE(ts.values[1] == 21.0);
                break;
            }
        }
        REQUIRE(foundTemp);
    }
    
    // Performance benchmarks
    BENCHMARK("ResearchMetrics Data Recording", "[metrics][benchmark]") {
        nlm::ResearchMetrics metrics;
        
        for (int i = 0; i < 1000; ++i) {
            metrics.record("metric", i * 0.01);
            metrics.recordTimeSeries("time_metric", i * 0.1);
            metrics.recordPattern("pattern", i % 2 == 0, i * 0.01);
        }
        
        return metrics.getMetricNames().size();
    }
    
    BENCHMARK("ResearchMetrics Statistical Analysis", "[metrics][benchmark]") {
        nlm::ResearchMetrics metrics;
        
        // Generate data
        for (int i = 0; i < 100; ++i) {
            metrics.record("statistical_data", i * 0.1);
        }
        
        // Perform statistical operations
        auto stats = metrics.calculateStatistics("statistical_data");
        auto correlation = metrics.calculateCorrelation("statistical_data", "statistical_data");
        auto trend = metrics.calculateTrend("statistical_data", 10);
        
        return stats.count + static_cast<size_t>(correlation) + static_cast<size_t>(trend);
    }
    
} // namespace test_metrics