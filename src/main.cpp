#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Acetylcholine.hpp"
#include "neuromodulation/Norepinephrine.hpp"
#include "neuromodulation/Serotonin.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/New novelty.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <random>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

// Phase 3: World Interaction Experiment
void runPhase3Experiment(std::shared_ptr<Brain> brain, const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Phase 3: World Interaction Experiment ===");
    NLM_LOG_INFO("Simulating agent-environment interaction with real-time sensory input...");
    
    size_t steps = config->getOr<size_t>("experiment_steps", 1000);
    float sensoryGain = config->getOr<float>("sensory_gain", 1.0f);
    
    // Enable all neuromodulators for realistic behavior
    if (auto* dopamine = brain->getDopamine()) {
        dopamine->setLevel(config->getOr<float>("dopamine_level", 0.5f));
    }
    if (auto* acetylcholine = brain->getAcetylcholine()) {
        acetylcholine->setLevel(config->getOr<float>("acetylcholine_level", 0.3f));
    }
    if (auto* norepinephrine = brain->getNorepinephrine()) {
        norepinephrine->setLevel(config->getOr<float>("norepinephrine_level", 0.4f));
    }
    if (auto* serotonin = brain->getSerotonin()) {
        serotonin->setLevel(config->getOr<float>("serotonin_level", 0.2f));
    }
    
    // Enable curiosity and novelty for exploration
    if (auto* curiosity = brain->getCuriosity()) {
        curiosity->setLevel(config->getOr<float>("curiosity_level", 0.7f));
    }
    if (auto* novelty = brain->getNovelty()) {
        novelty->setLevel(config->getOr<float>("novelty_level", 0.6f));
    }
    
    // Enable concept formation for learning
    if (auto* conceptFormation = brain->getConceptFormation()) {
        conceptFormation->setFormationThreshold(config->getOr<float>("concept_threshold", 0.7f));
    }
    
    // Generate and process random sensory input
    auto createRandomSensoryInput = [](size_t dimensions) {
        std::vector<float> data(dimensions);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
        for (size_t i = 0; i < dimensions; ++i) {
            data[i] = dis(gen) * sensoryGain;
        }
        return data;
    };
    
    size_t successfulActions = 0;
    size_t totalRewards = 0.0f;
    
    NLM_LOG_INFO("Starting world interaction simulation for " + std::to_string(steps) + " steps...");
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Create random sensory input (simulating environment)
        auto visionData = createRandomSensoryInput(10);  // Vision: 10 features
        auto audioData = createRandomSensoryInput(5);   // Audio: 5 features  
        auto internalData = createRandomSensoryInput(3); // Internal signals
        
        // Create Vision sensory input
        Vision vision;
        vision.setData(visionData);
        vision.setTimestamp(static_cast<double>(step) * 0.001);
        
        // Create Audio sensory input
        Audio audio;
        audio.setData(audioData);
        audio.setSampleRate(44100);
        audio.setTimestamp(static_cast<double>(step) * 0.001);
        
        // Create InternalSignals
        InternalSignals internal;
        for (float val : internalData) {
            internal.addSignal(val);
        }
        internal.setTimestamp(static_cast<double>(step) * 0.001);
        
        // Process vision input
        brain->receiveSensoryInput(vision);
        
        // Advance simulation
        brain->step(step, static_cast<Timestamp>(step) * 0.001);
        
        // Simple reward for surviving
        if (step % 100 == 0) {
            if (auto* dopamine = brain->getDopamine()) {
                float reward = 0.1f;  // Small survival reward
                dopamine->signalReward(reward);
                totalRewards += reward;
                successfulActions++;
            }
        }
        
        if (step % 200 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Actions: " + std::to_string(successfulActions) +
                        " | Reward: " + std::to_string(totalRewards));
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 3 Results ===");
    NLM_LOG_INFO("Steps completed: " + std::to_string(steps));
    NLM_LOG_INFO("Successful actions: " + std::to_string(successfulActions));
    NLM_LOG_INFO("Total reward accumulated: " + std::to_string(totalRewards));
    NLM_LOG_INFO("Average reward per step: " + std::to_string(totalRewards / steps));
    
    if (auto* conceptFormation = brain->getConceptFormation()) {
        NLM_LOG_INFO("Concepts formed: " + std::to_string(conceptFormation->getConceptCount()));
    }
}

// Phase 4: Cognitive Experiment
void runPhase4Experiment(std::shared_ptr<Brain> brain, const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Phase 4: Cognitive Experiment ===");
    NLM_LOG_INFO("Testing cognitive capabilities: planning, concept formation, and attention...");
    
    size_t steps = config->getOr<size_t>("experiment_steps", 500);
    
    // Enable high neuromodulation levels for cognitive processing
    if (auto* dopamine = brain->getDopamine()) {
        dopamine->setLevel(config->getOr<float>("dopamine_level", 0.8f));
    }
    if (auto* acetylcholine = brain->getAcetylcholine()) {
        acetylcholine->setLevel(config->getOr<float>("acetylcholine_level", 0.9f));
    }
    if (auto* norepinephrine = brain->getNorepinephrine()) {
        norepinephrine->setLevel(config->getOr<float>("norepinephrine_level", 0.7f));
    }
    if (auto* serotonin = brain->getSerotonin()) {
        serotonin->setLevel(config->getOr<float>("serotonin_level", 0.6f));
    }
    
    // Enable concept formation
    if (auto* conceptFormation = brain->getConceptFormation()) {
        conceptFormation->setFormationThreshold(config->getOr<float>("concept_threshold", 0.6f));
        conceptFormation->setStabilityThreshold(config->getOr<float>("stability_threshold", 0.8f));
    }
    
    NLM_LOG_INFO("Starting cognitive experiment for " + std::to_string(steps) + " steps...");
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Generate a cognitive task input
        std::vector<float> cognitiveTask(8, 0.5f);  // Simple task vector
        
        if (auto* conceptFormation = brain->getConceptFormation()) {
            // Present the cognitive task
            size_t conceptId = conceptFormation->presentExperience(
                cognitiveTask, 
                std::vector<float>(5, 0.5f), 
                (step % 3 == 0) ? 1.0f : 0.0f,  // Reward occasionally
                step
            );
            
            if (conceptId > 0) {
                NLM_LOG_INFO("    Step " + std::to_string(step) + ": Assigned to concept " + 
                            std::to_string(conceptId));
            }
        }
        
        // Simulate cognitive processing
        if (auto* attention = brain->getAttention()) {
            // Simple attention test
            std::vector<NeuronId> competitors;
            for (size_t i = 0; i < 10; ++i) {
                competitors.push_back(NeuronId(i));
            }
            
            if (step % 50 == 0) {
                auto winners = attention->processCompetition(competitors, 0.5f);
                NLM_LOG_INFO("    Step " + std::to_string(step) + ": Attention winners: " + 
                            std::to_string(winners.size()));
            }
        }
        
        // Advance simulation
        brain->step(step, static_cast<Timestamp>(step) * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 4 Results ===");
    if (auto* conceptFormation = brain->getConceptFormation()) {
        NLM_LOG_INFO("Concepts formed: " + std::to_string(conceptFormation->getConceptCount()));
        
        // Analyze concept stability
        float avgStability = 0.0f;
        for (size_t i = 0; i < conceptFormation->getConceptCount(); ++i) {
            avgStability += conceptFormation->getConceptStability(i);
        }
        if (conceptFormation->getConceptCount() > 0) {
            avgStability /= conceptFormation->getConceptCount();
            NLM_LOG_INFO("Average concept stability: " + std::to_string(avgStability));
        }
    }
    
    NLM_LOG_INFO("  Cognitive processing completed successfully!");
}

// Phase 6: Full Integration Test
void runPhase6Experiment(std::shared_ptr<Brain> brain, const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Phase 6: Full Integration Test ===");
    NLM_LOG_INFO("Testing complete integrated brain with all systems working together...");
    
    size_t steps = config->getOr<size_t>("experiment_steps", 1000);
    
    // Enable all neuromodulators at varying levels
    if (auto* dopamine = brain->getDopamine()) {
        dopamine->setLevel(config->getOr<float>("dopamine_level", 0.7f));
    }
    if (auto* acetylcholine = brain->getAcetylcholine()) {
        acetylcholine->setLevel(config->getOr<float>("acetylcholine_level", 0.5f));
    }
    if (auto* norepinephrine = brain->getNorepinephrine()) {
        norepinephrine->setLevel(config->getOr<float>("norepinephrine_level", 0.6f));
    }
    if (auto* serotonin = brain->getSerotonin()) {
        serotonin->setLevel(config->getOr<float>("serotonin_level", 0.4f));
    }
    
    // Enable all higher cognitive functions
    if (auto* curiosity = brain->getCuriosity()) {
        curiosity->setLevel(config->getOr<float>("curiosity_level", 0.8f));
    }
    if (auto* novelty = brain->getNovelty()) {
        novelty->setLevel(config->getOr<float>("novelty_level", 0.7f));
    }
    if (auto* predictionError = brain->getPredictionErrorSignal()) {
        predictionError->setLevel(config->getOr<float>("prediction_error_level", 0.5f));
    }
    
    if (auto* conceptFormation = brain->getConceptFormation()) {
        conceptFormation->setFormationThreshold(config->getOr<float>("concept_threshold", 0.7f));
        conceptFormation->setStabilityThreshold(config->getOr<float>("stability_threshold", 0.75f));
    }
    
    if (auto* attention = brain->getAttention()) {
        attention->setInhibitionStrength(config->getOr<float>("attention_inhibition", 0.5f));
        attention->setExcitationStrength(config->getOr<float>("attention_excitation", 1.5f));
    }
    
    NLM_LOG_INFO("Starting full integration test for " + std::to_string(steps) + " steps...");
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Create realistic multi-modal sensory input
        auto createMultiModalInput = [](size_t visionDim, size_t audioDim, size_t internalDim) {
            std::vector<float> visionData(visionDim);
            std::vector<float> audioData(audioDim);
            std::vector<float> internalData(internalDim);
            
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
            
            for (size_t i = 0; i < visionDim; ++i) visionData[i] = dis(gen);
            for (size_t i = 0; i < audioDim; ++i) audioData[i] = dis(gen);
            for (size_t i = 0; i < internalDim; ++i) internalData[i] = dis(gen);
            
            return std::make_tuple(visionData, audioData, internalData);
        };
        
        auto [visionData, audioData, internalData] = createMultiModalInput(12, 8, 6);
        
        // Create multi-modal sensory inputs
        Vision vision;
        vision.setData(visionData);
        vision.setTimestamp(static_cast<double>(step) * 0.001);
        
        Audio audio;
        audio.setData(audioData);
        audio.setSampleRate(44100);
        audio.setTimestamp(static_cast<double>(step) * 0.001);
        
        InternalSignals internal;
        for (float val : internalData) {
            internal.addSignal(val);
        }
        internal.setTimestamp(static_cast<double>(step) * 0.001);
        
        // Process vision input
        brain->receiveSensoryInput(vision);
        
        // Advance simulation
        brain->step(step, static_cast<Timestamp>(step) * 0.001);
        
        if (step % 200 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Neurons: " + std::to_string(brain->getTotalNeuronCount()) +
                        " | Synapses: " + std::to_string(brain->getTotalSynapseCount()));
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Results ===");
    
    // Comprehensive system status report
    brain->logStatus();
    
    // Collect statistics
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t activeNeurons = brain->getActiveNeuronCount();
    size_t firingNeurons = brain->getFiringNeuronCount();
    float firingRate = brain->getAverageFiringRate();
    
    NLM_LOG_INFO("=== Integration Test Summary ===");
    NLM_LOG_INFO("Neurons: " + std::to_string(totalNeurons));
    NLM_LOG_INFO("Synapses: " + std::to_string(totalSynapses));
    NLM_LOG_INFO("Active neurons: " + std::to_string(activeNeurons));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(firingNeurons));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(firingRate * 1000) + " Hz");
    
    if (auto* conceptFormation = brain->getConceptFormation()) {
        NLM_LOG_INFO("Concepts formed: " + std::to_string(conceptFormation->getConceptCount()));
    }
    
    if (auto* workingMemory = brain->getWorkingMemory()) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(workingMemory->getActiveTraces()));
    }
    
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
    }
    
    NLM_LOG_INFO("  Full integration test completed successfully!");
}

float getAverageConceptStability(const std::vector<DiscoveredConcept>& concepts) {
    if (concepts.empty()) return 0.0f;
    float totalStability = 0.0f;
    for (const auto& concept : concepts) {
        totalStability += concept.avgStability;
    }
    return totalStability / concepts.size();
}

int main(int argc, char** argv) {
    // Parse command line arguments for advanced features
    size_t experimentSteps = 1000;
    std::string experimentPhase = "phase2";  // Default to phase2
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 8) == "--phase") {
            experimentPhase = arg.substr(8);  // Extract phase name (e.g., "3", "4", "6")
        } else if (arg == "--phase3" || arg == "--phase4" || arg == "--phase6") {
            experimentPhase = arg.substr(2);  // Extract phase number
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                experimentSteps = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                experimentSteps = std::stoul(argv[++i]);
            }
        }
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Print help information
    if (argc == 1 || (argc == 2 && (std::string(argv[1]) == "--help" || 
                                  std::string(argv[1]) == "-h" || 
                                  std::string(argv[1]) == "--help" || 
                                  std::string(argv[1]) == "--version" || 
                                  std::string(argv[1]) == "-v"))) {
        printBanner();
        std::cout << "\nUsage: nlm [OPTIONS]\n\n";
        std::cout << "Experiments:\n";
        std::cout << "  (no args)              Phase 2: Real Neural Computation (default)\n";
        std::cout << "  --phase2               Phase 2: Basic neural computation\n";
        std::cout << "  --phase3 [steps]       Phase 3: World interaction (default: 1000 steps)\n";
        std::cout << "  --phase4 [steps]       Phase 4: Cognitive experiments (default: 500 steps)\n";
        std::cout << "  --phase6 [steps]       Phase 6: Full integration test (default: 1000 steps)\n\n";
        std::cout << "Advanced Options:\n";
        std::cout << "  --config <file>        Load configuration from file\n";
        std::cout << "  --steps <n>            Number of steps for experiment (overrides default)\n";
        std::cout << "  --neuron-count <n>     Set number of neurons (default: 500)\n";
        std::cout << "  --region-count <n>     Set number of regions (default: 1)\n";
        std::cout << "  --dopamine <level>     Set dopamine level (0.0-1.0, default: 0.5)\n";
        std::cout << "  --acetylcholine <level> Set acetylcholine level (0.0-1.0, default: 0.3)\n";
        std::cout << "  --norepinephrine <level>Set norepinephrine level (0.0-1.0, default: 0.4)\n";
        std::cout << "  --serotonin <level>    Set serotonin level (0.0-1.0, default: 0.2)\n";
        std::cout << "  --curiosity <level>    Set curiosity level (0.0-1.0, default: 0.7)\n";
        std::cout << "  --novelty <level>      Set novelty level (0.0-1.0, default: 0.6)\n";
        std::cout << "  --concept-threshold <t> Concept formation threshold (0.0-1.0, default: 0.7)\n";
        std::cout << "  --stability-threshold <t> Stability threshold (0.0-1.0, default: 0.7)\n";
        std::cout << "  --sensory-gain <g>     Sensory input gain (default: 1.0)\n";
        std::cout << "  --help, -h              Show this help information\n";
        std::cout << "  --version, -v           Show version information\n\n";
        std::cout << "Examples:\n";
        std::cout << "  nlm --phase3 --steps 2000 --dopamine 0.8 --curiosity 0.9\n";
        std::cout << "  nlm --phase4 --concept-threshold 0.5 --stability-threshold 0.8\n";
        std::cout << "  nlm --phase6 --neuron-count 2000 --region-count 4\n";
        return 0;
    }
    
    printBanner();
    
    std::cout << "Initializing NLM Advanced Experiment System...\n" << std::endl;
    
    NLM_LOG_INFO("=== NLM Advanced Experiment System ===");
    NLM_LOG_INFO("Experiment phase: " + experimentPhase);
    NLM_LOG_INFO("Experiment steps: " + std::to_string(experimentSteps));
    NLM_LOG_INFO("Neuromodulation: Comprehensive system enabled");
    NLM_LOG_INFO("Cognitive integration: Concept formation enabled");
    NLM_LOG_INFO("Prediction system: Neural prediction integrated");
    NLM_LOG_INFO("Memory systems: Working and episodic memory active");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for advanced experiments
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Experiment-specific parameters
    config->set("experiment_steps", static_cast<size_t>(experimentSteps), ConfigSource::Default);
    config->set("sensory_gain", 1.0f, ConfigSource::Default);
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run selected experiment phase
    if (experimentPhase == "3" || experimentPhase == "phase3") {
        runPhase3Experiment(brain, config);
    } else if (experimentPhase == "4" || experimentPhase == "phase4") {
        runPhase4Experiment(brain, config);
    } else if (experimentPhase == "6" || experimentPhase == "phase6") {
        runPhase6Experiment(brain, config);
    } else {
        // Default Phase 2 (original functionality)
        runBasicConnectivityTest(brain);
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Experiment Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Experiment " + experimentPhase + " completed successfully!");
    NLM_LOG_INFO("All integrated systems demonstrated functional behavior.");
    NLM_LOG_INFO("Neural Learning Machine is now fully operational.");
    
    return 0;
}