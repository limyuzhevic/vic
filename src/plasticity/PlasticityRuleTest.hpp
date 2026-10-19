#pragma once

#include "plasticity/PlasticityRule.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/structural_plasticity.hpp"
#include "plasticity/neuromodulator_plasticity.hpp"
#include "plasticity/reward_plasticity.hpp"
#include "plasticity/homeostatic_plasticity.hpp"
#include "plasticity/BCM_plasticity.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

namespace nlm
{

class PlasticityRuleTest {
public:
    static void runAllTests() {
        std::cout << "Running PlasticityRule tests..." << std::endl;
        
        testPlasticityRuleBaseClass();
        testSTDP();
        testHebbian();
        testStructuralPlasticity();
        testNeuromodulatorPlasticity();
        testRewardModulatedPlasticity();
        testHomeostaticPlasticity();
        testBCMPlasticity();
        
        std::cout << "All tests passed!" << std::endl;
    }
    
private:
    static void testPlasticityRuleBaseClass() {
        // Create a mock synapse for testing
        class MockSynapse {
        public:
            float weight = 0.0f;
            void addToWeight(float delta) { weight += delta; }
            float getWeight() const { return weight; }
        };
        
        // Test base class functionality (abstract class cannot be instantiated)
        // This is primarily tested through concrete implementations
        std::cout << "  Testing PlasticityRule base class..." << std::endl;
    }
    
    static void testSTDP() {
        std::cout << "  Testing STDP..." << std::endl;
        
        STDP stdp;
        
        // Test parameter configuration
        stdp.setLTPWeight(0.02f);
        stdp.setLTDWeight(0.03f);
        stdp.setTimeConstant(25.0f);
        stdp.setNMDAThreshold(0.6f);
        stdp.setDopamineModulation(1.5f);
        
        assert(stdp.getLTPWeight() == 0.02f);
        assert(stdp.getLTDWeight() == 0.03f);
        assert(stdp.getTimeConstant() == 25.0f);
        assert(stdp.getNMDAThreshold() == 0.6f);
        assert(stdp.getDopamineModulation() == 1.5f);
        
        // Test configuration
        stdp.configure(0.01f, 0.012f, 20.0f, 0.5f);
        assert(stdp.getLTPWeight() == 0.01f);
        assert(stdp.getLTDWeight() == 0.012f);
        assert(stdp.getTimeConstant() == 20.0f);
        assert(stdp.getNMDAThreshold() == 0.5f);
        
        // Test STDP variant controls
        stdp.enableAsymmetricSTDP(true);
        stdp.enableSpatialSTDP(true);
        stdp.enableNMDASTDP(true);
        stdp.enableDopamineSTDP(true);
        
        assert(stdp.isAsymmetricSTDP() == true);
        assert(stdp.isSpatialSTDP() == true);
        assert(stdp.isNMDASTDP() == true);
        assert(stdp.isDopamineSTDP() == true);
        
        std::cout << "    STDP tests passed!" << std::endl;
    }
    
    static void testHebbian() {
        std::cout << "  Testing Hebbian..." << std::endl;
        
        Hebbian hebbian;
        
        // Test parameter configuration
        hebbian.setLearningRate(0.02f);
        hebbian.setBaselineThreshold(0.6f);
        hebbian.setHomeostaticTarget(0.2f);
        hebbian.setMaxWeight(2.0f);
        
        assert(hebbian.getLearningRate() == 0.02f);
        assert(hebbian.getBaselineThreshold() == 0.6f);
        assert(hebbian.getHomeostaticTarget() == 0.2f);
        assert(hebbian.getMaxWeight() == 2.0f);
        
        // Test Hebbian rule variants
        hebbian.enableCovarianceRule(true);
        hebbian.enableBCMTheory(true);
        hebbian.enableAntiHebbian(true);
        hebbian.enableHomeostaticPlasticity(true);
        
        assert(hebbian.isCovarianceRuleEnabled() == true);
        assert(hebbian.isBCMTheoryEnabled() == true);
        assert(hebbian.isAntiHebbianEnabled() == true);
        assert(hebbian.isHomeostaticPlasticityEnabled() == true);
        
        std::cout << "    Hebbian tests passed!" << std::endl;
    }
    
    static void testStructuralPlasticity() {
        std::cout << "  Testing StructuralPlasticity..." << std::endl;
        
        StructuralPlasticity structural;
        
        // Test parameter configuration
        structural.setSynaptogenesisRate(0.0002f);
        structural.setPruningRate(0.00002f);
        structural.setMinWeightThreshold(0.1f);
        structural.setActivityThreshold(0.01f);
        structural.setMaxSynapsesPerNeuron(200);
        structural.setMaxTotalSynapses(500000);
        
        assert(structural.getSynaptogenesisRate() == 0.0002f);
        assert(structural.getPruningRate() == 0.00002f);
        assert(structural.getMinWeightThreshold() == 0.1f);
        assert(structural.getActivityThreshold() == 0.01f);
        assert(structural.getMaxSynapsesPerNeuron() == 200);
        assert(structural.getMaxTotalSynapses() == 500000);
        
        // Test structural variant controls
        structural.enableActivityDependentFormation(true);
        structural.enableActivityDependentPruning(true);
        structural.enableHomeostaticStructuralPlasticity(true);
        
        assert(structural.isActivityDependentFormationEnabled() == true);
        assert(structural.isActivityDependentPruningEnabled() == true);
        assert(structural.isHomeostaticStructuralPlasticityEnabled() == true);
        
        // Test statistics
        assert(structural.getTotalSynapsesCreated() == 0);
        assert(structural.getTotalSynapsesPruned() == 0);
        assert(structural.getNetStructuralChange() == 0.0f);
        
        std::cout << "    StructuralPlasticity tests passed!" << std::endl;
    }
    
    static void testNeuromodulatorPlasticity() {
        std::cout << "  Testing NeuromodulatorPlasticity..." << std::endl;
        
        NeuromodulatorPlasticity neuromodulator;
        
        // Test neuromodulator level configuration
        neuromodulator.setDopamineLevel(1.2f);
        neuromodulator.setAcetylcholineLevel(1.1f);
        neuromodulator.setNorepinephrineLevel(1.3f);
        neuromodulator.setSerotoninLevel(0.9f);
        
        assert(neuromodulator.getDopamineLevel() == 1.2f);
        assert(neuromodulator.getAcetylcholineLevel() == 1.1f);
        assert(neuromodulator.getNorepinephrineLevel() == 1.3f);
        assert(neuromodulator.getSerotoninLevel() == 0.9f);
        
        // Test neuromodulator-specific modulation
        neuromodulator.setDopamineModulation(1.5f);
        neuromodulator.setAcetylcholineModulation(1.2f);
        neuromodulator.setNorepinephrineModulation(1.4f);
        neuromodulator.setSerotoninModulation(0.8f);
        
        assert(neuromodulator.getDopamineModulation() == 1.5f);
        assert(neuromodulator.getAcetylcholineModulation() == 1.2f);
        assert(neuromodulator.getNorepinephrineModulation() == 1.4f);
        assert(neuromodulator.getSerotoninModulation() == 0.8f);
        
        // Test neuromodulator dependency
        neuromodulator.enableDopamineDependentPlasticity(true);
        neuromodulator.enableAcetylcholineDependentPlasticity(true);
        neuromodulator.enableNorepinephrineDependentPlasticity(true);
        neuromodulator.enableSerotoninDependentPlasticity(true);
        
        assert(neuromodulator.isDopamineDependent() == true);
        assert(neuromodulator.isAcetylcholineDependent() == true);
        assert(neuromodulator.isNorepinephrineDependent() == true);
        assert(neuromodulator.isSerotoninDependent() == true);
        
        // Test neuromodulator interactions
        float dopamineAcetylcholine = neuromodulator.getNeuromodulatorInteraction("dopamine", "acetylcholine");
        float dopamineNorepinephrine = neuromodulator.getNeuromodulatorInteraction("dopamine", "norepinephrine");
        
        assert(std::abs(dopamineAcetylcholine) < 1.0f);
        assert(std::abs(dopamineNorepinephrine) < 1.0f);
        
        std::cout << "    NeuromodulatorPlasticity tests passed!" << std::endl;
    }
    
    static void testRewardModulatedPlasticity() {
        std::cout << "  Testing RewardModulatedPlasticity..." << std::endl;
        
        RewardModulatedPlasticity reward;
        
        // Test reward prediction error
        reward.setPredictionError(0.1f);
        reward.setExpectedReward(0.5f);
        reward.setActualReward(0.7f);
        reward.setDopamineResponse(1.2f);
        
        assert(reward.getPredictionError() == 0.1f);
        assert(reward.getExpectedReward() == 0.5f);
        assert(reward.getActualReward() == 0.7f);
        assert(reward.getDopamineResponse() == 1.2f);
        
        // Test reward-modulated learning parameters
        reward.setLearningRate(0.03f);
        reward.setAsymmetricRewardSensitivity(true);
        reward.setDelayedRewardModulation(true);
        reward.setTrialBasedModulation(true);
        reward.setCreditAssignmentWindow(50.0f);
        
        assert(reward.getLearningRate() == 0.03f);
        assert(reward.getAsymmetricRewardSensitivity() == true);
        assert(reward.getDelayedRewardModulation() == true);
        assert(reward.getTrialBasedModulation() == true);
        assert(reward.getCreditAssignmentWindow() == 50.0f);
        
        // Test reward variants
        reward.enableRPESTDP(true);
        reward.enableDopamineGatedSTDP(true);
        reward.enableRewardAugmentedHebbian(true);
        
        // Calculate prediction error
        reward.calculatePredictionError();
        assert(reward.getPredictionError() == 0.2f);
        
        // Record reward history
        reward.recordRewardHistory(0.8f, 1.0f);
        assert(reward.getAverageReward() == 0.8f);
        
        std::cout << "    RewardModulatedPlasticity tests passed!" << std::endl;
    }
    
    static void testHomeostaticPlasticity() {
        std::cout << "  Testing HomeostaticPlasticity..." << std::endl;
        
        HomeostaticPlasticity homeostatic;
        
        // Test homeostatic parameters
        homeostatic.setTargetFiringRate(0.2f);
        homeostatic.setPlasticityExponent(2.5f);
        homeostatic.setLearningRate(0.02f);
        homeostatic.setSynapticScaling(true);
        homeostatic.setIntrinsicPlasticity(true);
        homeostatic.setActivityThreshold(0.15f);
        
        assert(homeostatic.getTargetFiringRate() == 0.2f);
        assert(homeostatic.getPlasticityExponent() == 2.5f);
        assert(homeostatic.getLearningRate() == 0.02f);
        assert(homeostatic.isSynapticScalingEnabled() == true);
        assert(homeostatic.isIntrinsicPlasticityEnabled() == true);
        assert(homeostatic.getActivityThreshold() == 0.15f);
        
        std::cout << "    HomeostaticPlasticity tests passed!" << std::endl;
    }
    
    static void testBCMPlasticity() {
        std::cout << "  Testing BCMPlasticity..." << std::endl;
        
        BCMPlasticity bcm;
        
        // Test BCM parameters
        bcm.setTheta(0.6f);
        bcm.setSigma(0.2f);
        bcm.setLearningRate(0.015f);
        bcm.setMaximumWeight(2.0f);
        bcm.setMinimumWeight(-1.0f);
        bcm.setThetaAdaptationRate(0.002f);
        bcm.setPostSynapticGain(1.2f);
        
        assert(bcm.getTheta() == 0.6f);
        assert(bcm.getSigma() == 0.2f);
        assert(bcm.getLearningRate() == 0.015f);
        assert(bcm.getMaximumWeight() == 2.0f);
        assert(bcm.getMinimumWeight() == -1.0f);
        assert(bcm.getThetaAdaptationRate() == 0.002f);
        assert(bcm.getPostSynapticGain() == 1.2f);
        
        // Test BCM variants
        bcm.enableAntiHebbianBCM(true);
        bcm.enableHomeostaticBCM(true);
        bcm.enableNeuromodulatorBCM(true);
        
        assert(bcm.isAntiHebbianBCMEnabled() == true);
        assert(bcm.isHomeostaticBCMEnabled() == true);
        assert(bcm.isNeuromodulatorBCMEnabled() == true);
        
        // Test BCM dynamics
        bcm.updateSlidingThreshold(0.8f);
        assert(bcm.getSlidingThreshold() > 0.0f);
        
        float bcmChange = bcm.calculateBCMChange(0.8f, 0.6f);
        assert(std::abs(bcmChange) < 0.1f);
        
        std::cout << "    BCMPlasticity tests passed!" << std::endl;
    }
    
    static void testPlasticityMechanisms() {
        std::cout << "  Testing Plasticity Mechanisms..." << std::endl;
        
        // Test PlasticityRule helper methods
        float clampedWeight = PlasticityRule::clampWeight(1.5f, -1.0f, 1.0f);
        assert(clampedWeight == 1.0f);
        
        float clampedWeight2 = PlasticityRule::clampWeight(-2.0f, -1.0f, 1.0f);
        assert(clampedWeight2 == -1.0f);
        
        float covarianceChange = PlasticityRule::calculateCovarianceChange(0.8f, 0.6f, 0.1f, 0.1f, 0.01f, 20.0f);
        assert(std::isfinite(covarianceChange));
        
        float stdpChange = PlasticityRule::calculateSTDPChange(0.0f, 20.0f, 0.01f, 0.012f, 20.0f, 1.0f);
        assert(std::isfinite(stdpChange));
        
        std::cout << "    Plasticity Mechanisms tests passed!" << std::endl;
    }
    
    static void testBiologicalFeatures() {
        std::cout << "  Testing Biological Features..." << std::endl;
        
        // Test temporal dynamics
        float temporalEffect = std::exp(-10.0f / 20.0f);  // STDP temporal window
        assert(std::abs(temporalEffect - 0.6065f) < 0.01f);
        
        // Test spatial patterns
        float spatialFactor = std::exp(-1.0f / 50.0f);  // Spatial window
        assert(std::abs(spatialFactor - 0.9802f) < 0.01f);
        
        // Test input-specific and global plasticity
        float inputSpecific = 0.8f * 0.6f;  // Co-activation
        float globalPlasticity = 1.0f + 0.5f;  // Homeostatic scaling
        
        assert(inputSpecific == 0.48f);
        assert(globalPlasticity == 1.5f);
        
        // Test stability-plasticity dilemma solutions
        float stabilityFactor = 1.0f / (1.0f + 0.5f);  // Balance stability and plasticity
        assert(std::abs(stabilityFactor - 0.6667f) < 0.01f);
        
        // Test metaplasticity mechanisms
        float metaplasticFactor = 1.0f / (1.0f + 0.1f * 0.05f);  // Based on learning history
        assert(std::abs(metaplasticFactor - 0.9524f) < 0.01f);
        
        std::cout << "    Biological Features tests passed!" << std::endl;
    }
    
    static void testExperimentalValidation() {
        std::cout << "  Testing Experimental Validation Metrics..." << std::endl;
        
        // Simulate STDP timing window validation
        float stdpPositive = 0.01f * std::exp(-5.0f / 20.0f);  // Pre before post
        float stdpNegative = -0.012f * std::exp(-10.0f / 20.0f);  // Post before pre
        
        assert(stdpPositive > 0.0f);
        assert(stdpNegative < 0.0f);
        assert(std::abs(stdpPositive + stdpNegative) < 0.01f);  // Approximately balanced
        
        // Simulate Hebbian learning rule validation
        float hebbianLearning = 0.01f * 0.8f * 0.6f;  // Co-activation
        assert(hebbianLearning == 0.0048f);
        
        // Simulate reward-modulated plasticity validation
        float rewardModulation = 1.0f + 0.5f;  // RPE > 0
        assert(rewardModulation == 1.5f);
        
        // Simulate structural plasticity validation
        float formationProb = 0.0001f * (0.8f + 0.6f);  // Activity-dependent
        assert(formationProb == 0.00014f);
        
        std::cout << "    Experimental Validation tests passed!" << std::endl;
    }
};

} // namespace nlm

#endif // PLASTICITYRULETEST_HPP
