#pragma once

#include <gtest/gtest.h>
#include <memory>
#include "experiments/Experiment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Metrics.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"

using namespace nlm;

class ExperimentTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", uint64_t(42));
        
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            FAIL() << "Failed to initialize brain";
        }
        
        environment = std::make_shared<Environment>();
        if (!environment->reset()) {
            FAIL() << "Failed to reset environment";
        }
    }
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Environment> environment;
};

TEST_F(ExperimentTest, ExperimentCreation) {
    Experiment experiment("TestExperiment");
    EXPECT_EQ(experiment.getName(), "TestExperiment");
    EXPECT_EQ(experiment.getSeed(), 0ULL);
}

TEST_F(ExperimentTest, ExperimentConfiguration) {
    Experiment experiment("TestExperiment");
    experiment.setConfigValue("param1", "value1");
    experiment.setConfigValue("param2", "value2");
    
    EXPECT_EQ(experiment.getConfigValue("param1"), "value1");
    EXPECT_EQ(experiment.getConfigValue("param2"), "value2");
    EXPECT_EQ(experiment.getConfigValue("nonexistent"), "");
}

TEST_F(ExperimentTest, ExperimentMetrics) {
    Experiment experiment("TestExperiment");
    
    experiment.recordMetric("accuracy", 0.85f);
    experiment.recordMetric("loss", 0.15f);
    experiment.recordMetric("time", 123.5);
    
    EXPECT_TRUE(experiment.hasMetric("accuracy"));
    EXPECT_TRUE(experiment.hasMetric("loss"));
    EXPECT_TRUE(experiment.hasMetric("time"));
    
    EXPECT_DOUBLE_EQ(experiment.getMetric("accuracy"), 0.85);
    EXPECT_DOUBLE_EQ(experiment.getMetric("loss"), 0.15);
    EXPECT_DOUBLE_EQ(experiment.getMetric("time"), 123.5);
}

TEST_F(ExperimentTest, ExperimentStepData) {
    Experiment experiment("TestExperiment");
    
    experiment.recordStep(1, 100, 500, 20, 0.2f);
    experiment.recordStep(2, 102, 510, 25, 0.25f);
    
    const auto& stepData = experiment.getStepData();
    EXPECT_EQ(stepData.size(), 2U);
    
    EXPECT_EQ(stepData[0].step, 1U);
    EXPECT_EQ(stepData[0].neuronCount, 100U);
    EXPECT_EQ(stepData[0].synapseCount, 500U);
    EXPECT_EQ(stepData[0].firingCount, 20U);
    EXPECT_FLOAT_EQ(stepData[0].avgFiringRate, 0.2f);
}

TEST_F(ExperimentTest, ExperimentRunnerCreateExperiment) {
    ExperimentRunner runner;
    
    auto experiment = runner.createExperiment("TestExperiment2");
    EXPECT_NE(experiment, nullptr);
    EXPECT_EQ(experiment->getName(), "TestExperiment2");
    EXPECT_NE(experiment->getSeed(), 0ULL);
}

TEST_F(ExperimentTest, ExperimentRunnerRunExperiment) {
    ExperimentRunner runner;
    
    auto experiment = runner.createExperiment("TestExperiment3");
    EXPECT_TRUE(runner.runExperiment(experiment, brain, environment, 100));
    
    // Check that experiment recorded data
    const auto& stepData = experiment->getStepData();
    EXPECT_FALSE(stepData.empty());
}

TEST_F(ExperimentTest, MetricsBasicOperations) {
    Metrics metrics;
    
    metrics.record("test_metric", 1.0);
    metrics.record("test_metric", 2.0);
    metrics.record("test_metric", 3.0);
    
    const auto& values = metrics.getValues("test_metric");
    EXPECT_EQ(values.size(), 3U);
    EXPECT_DOUBLE_EQ(values[0], 1.0);
    EXPECT_DOUBLE_EQ(values[1], 2.0);
    EXPECT_DOUBLE_EQ(values[2], 3.0);
    
    EXPECT_DOUBLE_EQ(metrics.getMean("test_metric"), 2.0);
    EXPECT_DOUBLE_EQ(metrics.getMin("test_metric"), 1.0);
    EXPECT_DOUBLE_EQ(metrics.getMax("test_metric"), 3.0);
    EXPECT_DOUBLE_EQ(metrics.getStdDev("test_metric"), std::sqrt(1.0));
}

TEST_F(ExperimentTest, MetricsNonexistent) {
    Metrics metrics;
    
    EXPECT_EQ(metrics.getValues("nonexistent").size(), 0U);
    EXPECT_DOUBLE_EQ(metrics.getMean("nonexistent"), 0.0);
    EXPECT_DOUBLE_EQ(metrics.getMin("nonexistent"), 0.0);
    EXPECT_DOUBLE_EQ(metrics.getMax("nonexistent"), 0.0);
    EXPECT_DOUBLE_EQ(metrics.getStdDev("nonexistent"), 0.0);
}

TEST_F(ExperimentTest, MetricsClear) {
    Metrics metrics;
    
    metrics.record("temp_metric", 1.0);
    metrics.record("temp_metric", 2.0);
    
    metrics.clear();
    
    EXPECT_EQ(metrics.getValues("temp_metric").size(), 0U);
    EXPECT_DOUBLE_EQ(metrics.getMean("temp_metric"), 0.0);
}

TEST_F(ExperimentTest, MetricsSummary) {
    Metrics metrics;
    
    metrics.record("accuracy", 0.8);
    metrics.record("accuracy", 0.9);
    metrics.record("loss", 0.2);
    metrics.record("loss", 0.1);
    
    std::string summary = metrics.summary();
    EXPECT_FALSE(summary.empty());
    EXPECT_TRUE(summary.find("accuracy") != std::string::npos);
    EXPECT_TRUE(summary.find("loss") != std::string::npos);
}

TEST_F(ExperimentTest, ExperimentRunnerBatch) {
    ExperimentRunner runner;
    
    std::vector<std::shared_ptr<Experiment>> experiments;
    experiments.push_back(runner.createExperiment("BatchExp1"));
    experiments.push_back(runner.createExperiment("BatchExp2"));
    
    auto results = runner.runBatch(experiments, brain, environment, 50);
    
    EXPECT_EQ(results.size(), 2U);
    for (const auto& exp : results) {
        EXPECT_NE(exp, nullptr);
    }
}

TEST_F(ExperimentTest, ExperimentRunnerClear) {
    ExperimentRunner runner;
    
    auto exp1 = runner.createExperiment("TempExp1");
    auto exp2 = runner.createExperiment("TempExp2");
    
    EXPECT_EQ(runner.getExperiments().size(), 2U);
    
    runner.clearExperiments();
    EXPECT_EQ(runner.getExperiments().size(), 0U);
}

TEST_F(ExperimentTest, ExperimentRunnerGetByName) {
    ExperimentRunner runner;
    
    auto exp1 = runner.createExperiment("NamedExp");
    auto exp2 = runner.getExperiment("NamedExp");
    
    EXPECT_NE(exp2, nullptr);
    EXPECT_EQ(exp2->getName(), "NamedExp");
}

TEST_F(ExperimentTest, ExperimentSetters) {
    Experiment experiment("SetterTest");
    
    experiment.setName("NewName");
    EXPECT_EQ(experiment.getName(), "NewName");
    
    experiment.setSeed(12345);
    EXPECT_EQ(experiment.getSeed(), 12345ULL);
    
    experiment.setStartStep(100);
    experiment.setEndStep(200);
    EXPECT_EQ(experiment.getStartStep(), 100U);
    EXPECT_EQ(experiment.getEndStep(), 200U);
    
    experiment.addNote("Test note");
    const auto& notes = experiment.getNotes();
    EXPECT_EQ(notes.size(), 1U);
    EXPECT_EQ(notes[0], "Test note");
}

class MetricsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Empty setup for metrics tests
    }
};

TEST_F(MetricsTest, MetricsConstructor) {
    Metrics metrics;
    metrics.record("test", 1.0);
    
    auto values = metrics.getValues("test");
    EXPECT_EQ(values.size(), 1U);
}

TEST_F(MetricsTest, MetricsDestructor) {
    // This test ensures the destructor doesn't crash
    {
        Metrics metrics;
        metrics.record("temp", 1.0);
    }
    // Metrics object should be destroyed here
}

TEST_F(MetricsTest, MetricsMultipleNames) {
    Metrics metrics;
    
    metrics.record("metric1", 1.0);
    metrics.record("metric1", 2.0);
    metrics.record("metric2", 3.0);
    metrics.record("metric2", 4.0);
    
    auto names = metrics.getMetricNames();
    EXPECT_EQ(names.size(), 2U);
    
    auto values1 = metrics.getValues("metric1");
    auto values2 = metrics.getValues("metric2");
    
    EXPECT_EQ(values1.size(), 2U);
    EXPECT_EQ(values2.size(), 2U);
}

TEST_F(MetricsTest, MetricsEmptyAfterClear) {
    Metrics metrics;
    
    metrics.record("test", 1.0);
    metrics.clear();
    
    auto names = metrics.getMetricNames();
    EXPECT_EQ(names.size(), 0U);
}

GTEST_MAIN()