#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_experiment {

void testExperimentCreation() {
    nlm::Experiment experiment;
    assert(experiment.getExperimentType() == nlm::ExperimentType::Exploration);
    assert(experiment.isActive() == false);
    assert(experiment.getDuration() == 0.0f);
    std::cout << "    testExperimentCreation passed" << std::endl;
}

void testExperimentRunner() {
    nlm::ExperimentRunner runner;
    assert(runner.getRunningExperimentCount() == 0);
    assert(runner.isRunning() == false);
    std::cout << "    testExperimentRunner passed" << std::endl;
}

void testPhase3Experiment() {
    nlm::Phase3Experiment phase3Exp;
    assert(phase3Exp.getPhase() == 3);
    assert(phase3Exp.getStageCount() == 0);
    assert(phase3Exp.isCompleted() == false);
    std::cout << "    testPhase3Experiment passed" << std::endl;
}

void testPhase4Experiment() {
    nlm::Phase4Experiment phase4Exp;
    assert(phase4Exp.getPhase() == 4);
    assert(phase4Exp.getStageCount() == 0);
    assert(phase4Exp.isCompleted() == false);
    std::cout << "    testPhase4Experiment passed" << std::endl;
}

void testPhase5Experiment() {
    nlm::Phase5Experiment phase5Exp;
    assert(phase5Exp.getPhase() == 5);
    assert(phase5Exp.getStageCount() == 0);
    assert(phase5Exp.isCompleted() == false);
    std::cout << "    testPhase5Experiment passed" << std::endl;
}

void testPhase6IntegratedExperiment() {
    nlm::Phase6IntegratedExperiment phase6Exp;
    assert(phase6Exp.getPhase() == 6);
    assert(phase6Exp.getStageCount() == 0);
    assert(phase6Exp.isCompleted() == false);
    std::cout << "    testPhase6IntegratedExperiment passed" << std::endl;
}

void testExperimentSetup() {
    nlm::Phase3Experiment phase3Exp;
    
    // Setup experiment
    phase3Exp.setup(1000, 10);
    assert(phase3Exp.getStageCount() == 10);
    assert(phase3Exp.getDuration() == 1000.0f);
    
    // Run experiment
    phase3Exp.run();
    assert(phase3Exp.isActive() == true);
    assert(phase3Exp.isCompleted() == false);
    
    // Stop experiment
    phase3Exp.stop();
    assert(phase3Exp.isActive() == false);
    std::cout << "    testExperimentSetup passed" << std::endl;
}

void testExperimentMetrics() {
    nlm::Metrics metrics;
    assert(metrics.getEpoch() == 0);
    assert(metrics.getAccuracy() == 0.0f);
    assert(metrics.getLoss() == 0.0f);
    assert(metrics.getPrecision() == 0.0f);
    assert(metrics.getRecall() == 0.0f);
    std::cout << "    testExperimentMetrics passed" << std::endl;
}

void testScalingBenchmark() {
    nlm::ScalingBenchmark benchmark;
    assert(benchmark.getNetworkSize() == 100);
    assert(benchmark.getPerformance() == 0.0f);
    assert(benchmark.isCompleted() == false);
    std::cout << "    testScalingBenchmark passed" << std::endl;
}

void runAll() {
    std::cout << "Running Experiment tests..." << std::endl;
    testExperimentCreation();
    testExperimentRunner();
    testPhase3Experiment();
    testPhase4Experiment();
    testPhase5Experiment();
    testPhase6IntegratedExperiment();
    testExperimentSetup();
    testExperimentMetrics();
    testScalingBenchmark();
}

} // namespace test_experiment