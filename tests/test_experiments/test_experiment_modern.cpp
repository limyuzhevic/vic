#include "framework/test_base.hpp"
#include "brain/Brain.hpp"
#include "experiments/Experiment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Metrics.hpp"
#include <algorithm>
#include <cmath>

class ExperimentTestSuite : public TestSuite {
public:
    ExperimentTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Experiment Test Suite..." << std::endl;
        
        testExperimentCreation();
        testExperimentMetrics();
        testExperimentStepRecording();
        testExperimentConfiguration();
        
        std::cout << "Experiment Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "ExperimentTestSuite";
    }
    
private:
    void testExperimentCreation() {
        nlm::Experiment exp("test_experiment");
        
        ASSERT_EQ(exp.getName(), "test_experiment");
        ASSERT_TRUE(exp.getMetricCount() == 0);
        
        std::cout << "    testExperimentCreation passed" << std::endl;
    }
    
    void testExperimentMetrics() {
        nlm::Experiment exp("test_metrics");
        
        exp.recordMetric("accuracy", 0.95);
        exp.recordMetric("loss", 0.05f);
        exp.recordMetric("epochs", 100);
        
        ASSERT_TRUE(exp.hasMetric("accuracy"));
        ASSERT_TRUE(std::abs(exp.getMetric("accuracy") - 0.95) < 0.001);
        ASSERT_TRUE(std::abs(exp.getMetric("loss") - 0.05) < 0.001);
        ASSERT_TRUE(exp.getMetric("epochs") == 100);
        
        std::cout << "    testExperimentMetrics passed" << std::endl;
    }
    
    void testExperimentStepRecording() {
        nlm::Experiment exp("test_steps");
        
        exp.recordStep(1, 1000, 10000, 5000, 0.5f);
        exp.recordStep(2, 1010, 10100, 5100, 0.51f);
        
        const auto& stepData = exp.getStepData();
        ASSERT_TRUE(stepData.size() == 2);
        ASSERT_TRUE(stepData[0].step == 1);
        ASSERT_TRUE(stepData[0].neuronCount == 1000);
        ASSERT_TRUE(stepData[0].avgFiringRate == 0.5f);
        
        std::cout << "    testExperimentStepRecording passed" << std::endl;
    }
    
    void testExperimentConfiguration() {
        nlm::Experiment exp("test_config");
        
        exp.setConfigValue("learning_rate", "0.001");
        exp.setConfigValue("batch_size", "32");
        
        ASSERT_EQ(exp.getConfigValue("learning_rate"), "0.001");
        ASSERT_EQ(exp.getConfigValue("batch_size"), "32");
        
        std::cout << "    testExperimentConfiguration passed" << std::endl;
    }
};

class ExperimentRunnerTestSuite : public TestSuite {
public:
    ExperimentRunnerTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Experiment Runner Test Suite..." << std::endl;
        
        testExperimentRunnerCreation();
        testExperimentRunnerExecution();
        testExperimentRunnerMetrics();
        
        std::cout << "Experiment Runner Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "ExperimentRunnerTestSuite";
    }
    
private:
    void testExperimentRunnerCreation() {
        nlm::ExperimentRunner runner("test_runner");
        
        ASSERT_EQ(runner.getName(), "test_runner");
        ASSERT_TRUE(runner.getExperimentCount() == 0);
        
        std::cout << "    testExperimentRunnerCreation passed" << std::endl;
    }
    
    void testExperimentRunnerExecution() {
        nlm::ExperimentRunner runner("test_exec");
        
        // Create a simple experiment
        nlm::Experiment exp("exp1");
        exp.recordMetric("test_metric", 1.0f);
        
        // Add experiment to runner (simplified)
        runner.addExperiment(&exp);
        
        ASSERT_TRUE(runner.getExperimentCount() > 0);
        
        std::cout << "    testExperimentRunnerExecution passed" << std::endl;
    }
    
    void testExperimentRunnerMetrics() {
        nlm::ExperimentRunner runner("test_metrics");
        
        nlm::Experiment exp("metrics_exp");
        exp.recordMetric("accuracy", 0.95f);
        exp.recordMetric("loss", 0.05f);
        
        runner.addExperiment(&exp);
        
        double avgAccuracy = runner.getAverageMetric("accuracy");
        ASSERT_TRUE(avgAccuracy > 0.9f && avgAccuracy < 1.0f);
        
        std::cout << "    testExperimentRunnerMetrics passed" << std::endl;
    }
};

class MetricsTestSuite : public TestSuite {
public:
    MetricsTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Metrics Test Suite..." << std::endl;
        
        testMetricsCreation();
        testMetricsCalculation();
        testMetricsAggregation();
        
        std::cout << "Metrics Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "MetricsTestSuite";
    }
    
private:
    void testMetricsCreation() {
        nlm::Metrics metrics;
        
        ASSERT_TRUE(metrics.getMetricCount() == 0);
        
        std::cout << "    testMetricsCreation passed" << std::endl;
    }
    
    void testMetricsCalculation() {
        nlm::Metrics metrics;
        
        metrics.recordAccuracy(0.95f);
        metrics.recordLoss(0.05f);
        metrics.recordPrecision(0.87f);
        
        ASSERT_TRUE(metrics.getAccuracy() == 0.95f);
        ASSERT_TRUE(metrics.getLoss() == 0.05f);
        ASSERT_TRUE(metrics.getPrecision() == 0.87f);
        
        std::cout << "    testMetricsCalculation passed" << std::endl;
    }
    
    void testMetricsAggregation() {
        nlm::Metrics metrics;
        
        metrics.recordAccuracy(0.90f);
        metrics.recordAccuracy(0.95f);
        metrics.recordAccuracy(0.85f);
        
        float avgAccuracy = metrics.getAverageAccuracy();
        ASSERT_TRUE(avgAccuracy > 0.85f && avgAccuracy < 0.95f);
        
        std::cout << "    testMetricsAggregation passed" << std::endl;
    }
};

extern "C" {
    TestSuite* createExperimentTestSuite() {
        return new ExperimentTestSuite();
    }
    
    TestSuite* createExperimentRunnerTestSuite() {
        return new ExperimentRunnerTestSuite();
    }
    
    TestSuite* createMetricsTestSuite() {
        return new MetricsTestSuite();
    }
}