#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <chrono>
#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Maturation.hpp"
#include "development/Pruning.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

namespace test_development_advanced {

void testCriticalPeriod() {
    std::cout << "Testing critical period..." << std::endl;
    
    nlm::DevelopmentSystem devSystem;
    assert(devSystem.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    assert(devSystem.getCriticalPeriodStart() == 0.0f);
    assert(devSystem.getCriticalPeriodEnd() > 0.0f);
    
    float critical_period_duration = devSystem.getCriticalPeriodDuration();
    assert(critical_period_duration > 0.0f);
    
    devSystem.update(5.0f);
    assert(devSystem.getDevelopmentalStage() != nlm::DevelopmentalStage::Initial);
    
    devSystem.update(critical_period_duration * 1.5f);
    assert(devSystem.getDevelopmentalStage() != nlm::DevelopmentalStage::Initial);
    
    std::cout << "  PASSED" << std::endl;
}

void testMaturation() {
    std::cout << "Testing maturation..." << std::endl;
    
    nlm::Maturation maturation(0.3f, 0.9f, 1.0f, 0.1f);
    assert(maturation.getMaturationRate() == 0.3f);
    assert(maturation.getTargetMaturity() == 0.9f);
    assert(maturation.getTimeToMaturity() == 1.0f);
    assert(maturation.getPruningThreshold() == 0.1f);
    
    float level_at_start = maturation.getMaturationLevel(0.0f);
    assert(level_at_start >= 0.0f);
    
    float level_at_time = maturation.getMaturationLevel(0.5f);
    assert(level_at_time >= level_at_start);
    assert(level_at_time <= maturation.getTargetMaturity());
    
    float level_at_end = maturation.getMaturationLevel(1.0f);
    assert(level_at_end >= level_at_time);
    assert(level_at_end <= maturation.getTargetMaturity());
    
    std::cout << "  PASSED" << std::endl;
}

void testPruning() {
    std::cout << "Testing pruning..." << std::endl;
    
    nlm::Pruning pruning(0.4f, 0.6f, 0.15f, 0.8f);
    assert(pruning.getPruningRate() == 0.4f);
    assert(pruning.getRecoveryRate() == 0.6f);
    assert(pruning.getActivityThreshold() == 0.15f);
    assert(pruning.getMaxPruning() == 0.8f);
    
    float prob_low_activity = pruning.getPruningProbability(0.1f);
    assert(prob_low_activity < pruning.getActivityThreshold());
    
    float prob_medium_activity = pruning.getPruningProbability(0.5f);
    assert(prob_medium_activity >= pruning.getActivityThreshold());
    assert(prob_medium_activity <= pruning.getMaxPruning());
    
    float prob_high_activity = pruning.getPruningProbability(0.9f);
    assert(prob_high_activity <= pruning.getMaxPruning());
    
    std::cout << "  PASSED" << std::endl;
}

void testSynaptogenesis() {
    std::cout << "Testing synaptogenesis..." << std::endl;
    
    nlm::Synaptogenesis synaptogenesis(0.1f, 0.5f, 0.2f, 0.8f);
    assert(synaptogenesis.getInitialRate() == 0.1f);
    assert(synaptogenesis.getPeakRate() == 0.5f);
    assert(synaptogenesis.getGrowthPeriod() == 0.2f);
    assert(synaptogenesis.getDeclinePeriod() == 0.8f);
    
    float rate_at_start = synaptogenesis.getConnectionProbability(0.0f);
    assert(rate_at_start >= 0.0f);
    
    float rate_at_growth_end = synaptogenesis.getConnectionProbability(0.2f);
    assert(rate_at_growth_end >= rate_at_start);
    
    float rate_at_steady_state = synaptogenesis.getConnectionProbability(0.5f);
    assert(rate_at_steady_state <= synaptogenesis.getPeakRate());
    
    float rate_at_decline_start = synaptogenesis.getConnectionProbability(0.8f);
    assert(rate_at_decline_start >= 0.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "Testing development system integration..." << std::endl;
    
    nlm::DevelopmentSystem devSystem;
    assert(devSystem.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    devSystem.update(5.0f);
    assert(devSystem.getDevelopmentalStage() != nlm::DevelopmentalStage::Initial);
    
    devSystem.update(10.0f);
    assert(devSystem.getDevelopmentalStage() != nlm::DevelopmentalStage::Initial);
    
    nlm::DevelopmentalStage stage = devSystem.getCurrentStage();
    assert(stage != nlm::DevelopmentalStage::Initial);
    
    float age = devSystem.getDevelopmentalAge();
    assert(age >= 0.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testDevelopmentPlasticityIntegration() {
    std::cout << "Testing development-plasticity integration..." << std::endl;
    
    nlm::DevelopmentSystem devSystem;
    nlm::Synaptogenesis synaptogenesis(0.1f, 0.5f, 0.2f, 0.8f);
    nlm::Maturation maturation(0.3f, 0.9f, 1.0f, 0.1f);
    nlm::Pruning pruning(0.4f, 0.6f, 0.15f, 0.8f);
    
    float age = 0.5f;
    
    float synaptogenesis_rate = synaptogenesis.getConnectionProbability(age);
    float maturation_level = maturation.getMaturationLevel(age);
    float pruning_prob = pruning.getPruningProbability(maturation_level);
    
    assert(synaptogenesis_rate >= 0.0f && synaptogenesis_rate <= 1.0f);
    assert(maturation_level >= 0.0f && maturation_level <= 1.0f);
    assert(pruning_prob >= 0.0f && pruning_prob <= 1.0f);
    
    age = 1.5f;
    synaptogenesis_rate = synaptogenesis.getConnectionProbability(age);
    maturation_level = maturation.getMaturationLevel(age);
    pruning_prob = pruning.getPruningProbability(maturation_level);
    
    assert(synaptogenesis_rate >= 0.0f && synaptogenesis_rate <= 1.0f);
    assert(maturation_level >= 0.0f && maturation_level <= 1.0f);
    assert(pruning_prob >= 0.0f && pruning_prob <= 1.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Advanced Development Tests ===" << std::endl;
    
    testCriticalPeriod();
    testMaturation();
    testPruning();
    testSynaptogenesis();
    testDevelopmentSystemIntegration();
    testDevelopmentPlasticityIntegration();
    
    std::cout << "=== All Advanced Development Tests PASSED ===" << std::endl;
}

} // namespace test_development_advanced
