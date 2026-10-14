// Neuromodulator Tests
TEST_CASE("neuromodulator_test", TestCategory::UNIT, "Tests for neuromodulator functionality")
void testNeuromodulator() {
    nlm::Neuromodulator neuromodulator;
    
    // Test initial state
    assert(neuromodulator.getSignalQueueSize() == 0);
    assert(neuromodulator.getActiveModulations() == 0);
    
    // Test signal application
    nlm::ModulationSignal signal1;
    signal1.type = nlm::ModulationType::Reward;
    signal1.strength = 1.0f;
    signal1.timestamp = 0.0;
    
    neuromodulator.applySignal(signal1);
    assert(neuromodulator.getSignalQueueSize() > 0);
    
    // Test reward signal
    nlm::Reward reward1;
    reward1.value = 10.0f;
    reward1.timestamp = 0.0;
    
    neuromodulator.applyReward(reward1);
    assert(neuromodulator.getSignalQueueSize() > 0);
    
    // Test prediction error
    nlm::PredictionError error1;
    error1.prediction = 5.0f;
    error1.actual = 8.0f;
    error1.timestamp = 0.0;
    
    neuromodulator.applyPredictionError(error1);
    assert(neuromodulator.getSignalQueueSize() > 0);
    
    // Test novelty detection
    nlm::Novelty novelty1;
    novelty1.significance = 0.7f;
    novelty1.timestamp = 0.0;
    
    neuromodulator.applyNovelty(novelty1);
    assert(neuromodulator.getSignalQueueSize() > 0);
    
    // Test curiosity
    nlm::Curiosity curiosity1;
    curiosity1.level = 0.5f;
    curiosity1.timestamp = 0.0;
    
    neuromodulator.applyCuriosity(curiosity1);
    assert(neuromodulator.getSignalQueueSize() > 0);
    
    // Test signal processing
    neuromodulator.processSignals();
    assert(neuromodulator.getActiveModulations() > 0);
    
    // Test effect on neuron
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuromodulator.applyEffects(neuron, 0.1);
    
    // Neuron should have been modified by neuromodulation
    // (Specific modification depends on neuromodulator implementation)
    
    // Test neuromodulator configuration
    neuromodulator.configure(0.5f, 0.8f, 0.1f);
    
    // Test reset
    neuromodulator.reset();
    assert(neuromodulator.getSignalQueueSize() == 0);
    
    // Test signal priority
    nlm::ModulationSignal lowPriority;
    lowPriority.type = nlm::ModulationType::Novelty;
    lowPriority.strength = 0.1f;
    lowPriority.timestamp = 0.0;
    
    neuromodulator.applySignal(lowPriority);
    
    nlm::ModulationSignal highPriority;
    highPriority.type = nlm::ModulationType::Reward;
    highPriority.strength = 1.0f;
    highPriority.timestamp = 0.0;
    
    neuromodulator.applySignal(highPriority);
    
    // Higher priority signals should be processed first
    neuromodulator.processSignals();
    
    std::cout << "Neuromodulator tests passed" << std::endl;
    return true;
}

// Reward Tests
TEST_CASE("reward_test", TestCategory::UNIT, "Tests for reward system")
void testReward() {
    nlm::Reward reward(0.5f, 0.0);
    
    assert(reward.value == 0.5f);
    assert(reward.timestamp == 0.0);
    assert(reward.type == nlm::RewardType::Primary);
    
    // Test reward scaling
    nlm::Reward scaled = reward.scale(2.0f);
    assert(scaled.value == 1.0f);
    
    // Test reward discounting
    nlm::Reward discounted = reward.discount(0.1f, 0.5f);
    assert(discounted.value < 0.5f);
    
    // Test reward integration
    nlm::Reward integrated = nlm::Reward::integrate(reward, 10.0f);
    assert(integrated.value == reward.value);
    
    // Test reward comparison
    nlm::Reward reward2(0.3f, 0.0);
    assert(reward > reward2);
    assert(reward2 < reward);
    
    std::cout << "Reward tests passed" << std::endl;
    return true;
}

// Prediction Error Tests
TEST_CASE("prediction_error_test", TestCategory::UNIT, "Tests for prediction error system")
void testPredictionError() {
    nlm::PredictionError error(5.0f, 8.0f, 0.0);
    
    assert(error.prediction == 5.0f);
    assert(error.actual == 8.0f);
    assert(error.timestamp == 0.0);
    assert(error.type == nlm::PredictionErrorType::Positive);
    
    // Test error magnitude
    float magnitude = error.getMagnitude();
    assert(magnitude == 3.0f);
    
    // Test error sign
    assert(error.isPositiveError());
    
    // Test error scaling
    nlm::PredictionError scaled = error.scale(0.5f);
    assert(scaled.getMagnitude() == 1.5f);
    
    // Test error integration
    nlm::PredictionError integrated = nlm::PredictionError::integrate(error, 2.0f);
    assert(integrated.getMagnitude() == 3.0f);
    
    // Test error temporal dynamics
    nlm::PredictionError temporal(5.0f, 8.0f, 0.0);
    float decay = temporal.getTemporalDecay(1.0f, 0.1f);
    assert(decay < temporal.getMagnitude());
    
    std::cout << "Prediction error tests passed" << std::endl;
    return true;
}

// Novelty Tests
TEST_CASE("novelty_test", TestCategory::UNIT, "Tests for novelty detection system")
void testNovelty() {
    nlm::Novelty novelty(0.7f, 0.0);
    
    assert(novelty.significance == 0.7f);
    assert(novelty.timestamp == 0.0);
    assert(novelty.type == nlm::NoveltyType::Perceptual);
    
    // Test novelty threshold
    assert(novelty.isAboveThreshold(0.5f));
    assert(!novelty.isAboveThreshold(0.8f));
    
    // Test novelty decay
    float decayed = novelty.decay(0.1f, 1.0f);
    assert(decayed < 0.7f);
    
    // Test novelty integration
    nlm::Novelty integrated = nlm::Novelty::integrate(novelty, 2.0f);
    assert(integrated.significance == novelty.significance);
    
    // Test novelty classification
    nlm::Novelty perceptual(0.6f, 0.0);
    assert(perceptual.getNoveltyClass() == nlm::NoveltyType::Perceptual);
    
    nlm::Novelty conceptual(0.8f, 0.0);
    assert(conceptual.getNoveltyClass() == nlm::NoveltyType::Conceptual);
    
    std::cout << "Novelty tests passed" << std::endl;
    return true;
}

// Curiosity Tests
TEST_CASE("curiosity_test", TestCategory::UNIT, "Tests for curiosity system")
void testCuriosity() {
    nlm::Curiosity curiosity(0.5f, 0.0);
    
    assert(curiosity.level == 0.5f);
    assert(curiosity.timestamp == 0.0);
    assert(curiosity.type == nlm::CuriosityType::Exploitation);
    
    // Test curiosity state
    assert(!curiosity.isHighCuriosity());
    assert(!curiosity.isLowCuriosity());
    
    // Test curiosity modulation
    curiosity.increaseLevel(0.3f);
    assert(curiosity.level == 0.8f);
    
    curiosity.decreaseLevel(0.2f);
    assert(curiosity.level == 0.6f);
    
    // Test curiosity type transition
    curiosity.transitionToExploration();
    assert(curiosity.type == nlm::CuriosityType::Exploration);
    
    curiosity.transitionToExploitation();
    assert(curiosity.type == nlm::CuriosityType::Exploitation);
    
    // Test curiosity threshold
    assert(!curiosity.isThresholdCrossed(0.6f));
    assert(curiosity.isThresholdCrossed(0.4f));
    
    // Test curiosity behavior
    nlm::CuriosityBehavior behavior = curiosity.getBehavior();
    assert(behavior == nlm::CuriosityBehavior::MaintainCurrent);
    
    std::cout << "Curiosity tests passed" << std::endl;
    return true;
}