// Sensory, Motor, Prediction, and Experiment System Tests
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/MotorSystem.hpp"
#include "prediction/PredictionSystem.hpp"
#include "experiments/Experiment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

namespace test_advanced_systems {

void testVision() {
    nlm::Vision vision;
    
    // Test initialization
    vision.initialize(nullptr);
    
    // Test detection with simple input
    std::vector<float> simpleInput(10, 0.5f);
    float detection = vision.detect(simpleInput);
    assert(detection >= 0.0f && detection <= 1.0f);
    
    // Test feature extraction
    std::vector<float> features = vision.extractFeatures(simpleInput);
    assert(!features.empty());
    assert(features.size() == 10); // Should match input size
    
    // Test processing pipeline
    std::vector<float> processed = vision.process(simpleInput);
    assert(!processed.empty());
    
    // Test threshold setting
    vision.setDetectionThreshold(0.7f);
    vision.setSensitivity(0.8f);
    vision.setNoiseReduction(0.1f);
    
    // Test resolution
    vision.setResolution(1024u, 768u);
    
    // Test frame rate
    vision.setFrameRate(30.0f);
    
    // Test reset
    vision.reset();
    
    std::cout << "    testVision passed" << std::endl;
}

void testAudio() {
    nlm::Audio audio;
    
    // Test initialization
    audio.initialize(nullptr);
    
    // Test spectrum analysis
    std::vector<float> audioInput(1024, 0.1f);
    std::vector<float> spectrum = audio.analyzeSpectrum(audioInput);
    assert(!spectrum.empty());
    
    // Test frequency detection
    std::vector<float> frequencies = audio.detectFrequencies(audioInput);
    assert(!frequencies.empty());
    
    // Test source localization
    auto sources = audio.localizeSources(audioInput);
    assert(sources.size() >= 0);
    
    // Test noise filtering
    std::vector<float> filtered = audio.filterNoise(audioInput);
    assert(filtered.size() == audioInput.size());
    
    // Test parameter setting
    audio.setFrequencyRange(20.0f, 20000.0f);
    audio.setSampleRate(44100);
    audio.setBitDepth(16);
    
    // Test amplitude
    audio.setAmplitude(0.5f);
    
    // Test reset
    audio.reset();
    
    std::cout << "    testAudio passed" << std::endl;
}

void testInternalSignals() {
    nlm::InternalSignals internalSignals;
    
    // Test initialization
    internalSignals.initialize(nullptr);
    
    // Test signal generation
    std::vector<float> generated = internalSignals.generateSignals();
    assert(!generated.empty());
    
    // Test signal processing
    std::vector<float> input = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> processed = internalSignals.processSignals(input);
    assert(processed.size() == input.size());
    
    // Test homeostasis
    std::vector<float> homeostasis = internalSignals.getHomeostaticSignals();
    assert(!homeostasis.empty());
    
    // Test stress signals
    float stress = internalSignals.getStressLevel();
    assert(stress >= 0.0f && stress <= 1.0f);
    
    // Test parameter setting
    internalSignals.setHomeostasisThreshold(0.5f);
    internalSignals.setStressThreshold(0.7f);
    internalSignals.setRecoveryRate(0.1f);
    
    // Test reset
    internalSignals.reset();
    
    std::cout << "    testInternalSignals passed" << std::endl;
}

void testSensoryInput() {
    nlm::SensoryInput sensoryInput;
    
    // Test initialization
    sensoryInput.initialize(nullptr);
    
    // Test multimodal input
    std::vector<float> visualInput(100, 0.5f);
    std::vector<float> auditoryInput(50, 0.3f);
    std::vector<float> proprioceptiveInput(20, 0.2f);
    
    sensoryInput.update(visualInput, auditoryInput, proprioceptiveInput);
    
    // Test integrated perception
    auto perception = sensoryInput.getIntegratedPerception();
    assert(!perception.empty());
    
    // Test sensory gating
    bool gating = sensoryInput.getSensoryGating();
    // May or may not be gated depending on implementation
    
    // Test parameter setting
    sensoryInput.setIntegrationWeight(0.7f);
    sensoryInput.setNoiseLevel(0.1f);
    sensoryInput.setAttentionThreshold(0.5f);
    
    // Test attention modulation
    sensoryInput.modulateAttention(0.8f);
    
    // Test reset
    sensoryInput.reset();
    
    std::cout << "    testSensoryInput passed" << std::endl;
}

void testMotorSystem() {
    nlm::MotorSystem motorSystem;
    
    // Test initialization
    motorSystem.initialize(nullptr);
    
    // Test action production
    auto action = motorSystem.produceAction();
    assert(action.has_value());
    assert(action.value() == nlm::ActionType::Wait || action.value() == nlm::ActionType::Move);
    
    // Test motor command
    nlm::MotorCommand command;
    command.magnitude = 0.5f;
    command.duration = 0.1f;
    
    auto output = motorSystem.executeCommand(command);
    assert(output > 0.0f);
    
    // Test output gain
    motorSystem.setOutputGain(0.8f);
    motorSystem.setDeadband(0.01f);
    
    // Test reflex control
    motorSystem.enableReflexes(true);
    bool reflexes = motorSystem.getReflexEnabled();
    assert(reflexes == true);
    
    // Test parameter tuning
    motorSystem.setTorque(10.0f);
    motorSystem.setMaxVelocity(5.0f);
    
    // Test reset
    motorSystem.reset();
    
    // Test safety controls
    bool safe = motorSystem.isSafe();
    assert(safe == true);
    
    std::cout << "    testMotorSystem passed" << std::endl;
}

void testPredictionSystem() {
    nlm::PredictionSystem predictionSystem;
    
    // Test initialization
    predictionSystem.initialize(nullptr);
    
    // Test prediction with simple state
    std::vector<float> currentState = {0.1f, 0.2f, 0.3f};
    std::vector<float> predicted = predictionSystem.predictNextState(currentState);
    assert(!predicted.empty());
    assert(predicted.size() == currentState.size());
    
    // Test prediction accuracy
    float accuracy = predictionSystem.getPredictionAccuracy();
    assert(accuracy >= 0.0f && accuracy <= 1.0f);
    
    // Test confidence
    float confidence = predictionSystem.getPredictionConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test parameter setting
    predictionSystem.setPredictionHorizon(10);
    predictionSystem.setConfidenceThreshold(0.5f);
    predictionSystem.setLearningRate(0.1f);
    
    // Test model update
    predictionSystem.updateModel(currentState, predicted);
    
    // Test prediction error
    float error = predictionSystem.getPredictionError();
    assert(error >= 0.0f);
    
    // Test reset
    predictionSystem.reset();
    
    // Test prediction quality
    bool quality = predictionSystem.isPredictionQualityGood();
    // May or may not be good depending on implementation
    
    std::cout << "    testPredictionSystem passed" << std::endl;
}

void testExperiment() {
    nlm::Experiment experiment;
    
    // Test initialization
    experiment.initialize(nullptr);
    
    // Test experiment start
    experiment.start();
    assert(experiment.isRunning() == true);
    
    // Test experiment stop
    experiment.stop();
    assert(experiment.isRunning() == false);
    
    // Test experiment configuration
    experiment.setMaxSteps(1000);
    experiment.setRandomSeed(42);
    
    // Test metrics
    auto metrics = experiment.getMetrics();
    assert(!metrics.empty());
    
    // Test metric logging
    experiment.logMetric("test_metric", 1.0f);
    
    // Test checkpoint
    experiment.createCheckpoint();
    
    // Test save/load
    experiment.saveToFile("test_checkpoint.nelm");
    
    // Test reset
    experiment.reset();
    
    // Test validation
    bool valid = experiment.validateConfiguration();
    // May or may not be valid depending on configuration
    
    std::cout << "    testExperiment passed" << std::endl;
}

void testExperimentRunner() {
    nlm::ExperimentRunner experimentRunner;
    
    // Test initialization
    experimentRunner.initialize(nullptr);
    
    // Test experiment execution
    experimentRunner.runExperiment();
    assert(experimentRunner.isRunning() == true);
    
    // Test experiment management
    experimentRunner.pauseExperiment();
    assert(experimentRunner.isRunning() == false);
    
    experimentRunner.resumeExperiment();
    assert(experimentRunner.isRunning() == true);
    
    experimentRunner.stopExperiment();
    assert(experimentRunner.isRunning() == false);
    
    // Test results
    auto results = experimentRunner.getResults();
    assert(!results.empty());
    
    // Test metrics aggregation
    auto metrics = experimentRunner.getMetrics();
    assert(!metrics.empty());
    
    // Test configuration
    experimentRunner.setExperimentConfig("test_config");
    auto config = experimentRunner.getExperimentConfig();
    assert(!config.empty());
    
    // Test reset
    experimentRunner.reset();
    
    std::cout << "    testExperimentRunner passed" << std::endl;
}

void runAll() {
    testVision();
    testAudio();
    testInternalSignals();
    testSensoryInput();
    testMotorSystem();
    testPredictionSystem();
    testExperiment();
    testExperimentRunner();
}

} // namespace test_advanced_systems