// Integration and usage examples for the Advanced NLM Features

#include "BrainOptimized.hpp"
#include "EnhancedPlasticity.hpp"
#include "AdvancedNeuralNetwork.hpp"
#include "AdvancedMemory.hpp"
#include "AdvancedPrediction.hpp"
#include <iostream>
#include <memory>
#include <vector>

namespace nlm {

// Example 1: Advanced Neural Network Integration
class AdvancedNeuralNetworkExample {
public:
    AdvancedNeuralNetworkExample() : network_initialized_(false) {}
    
    void demonstrateDendriticNetworks() {
        std::cout << "=== Advanced Dendritic Neural Networks ===" << std::endl;
        
        DendriticNeuralNetwork dnn;
        dnn.buildNetwork(3, 100, 2);  // 3 layers, 100 units, 2 connections per unit
        
        // Create sample input
        std::vector<float> input(10, 0.5f);
        
        // Forward propagation
        std::vector<float> output = dnn.forwardPropagate(input, 0.01f);
        
        std::cout << "Input size: " << input.size() << std::endl;
        std::cout << "Output size: " << output.size() << std::endl;
        std::cout << "Network demonstration completed successfully!" << std::endl;
    }
    
    void demonstrateAttentionMechanisms() {
        std::cout << "\n=== Advanced Attention Mechanisms ===" << std::endl;
        
        AdvancedAttention attention;
        attention.setNumHeads(8);
        attention.setSequenceLength(100);
        attention.setModelDimension(512);
        
        // Create sample sequences
        std::vector<std::vector<float>> queries(10, std::vector<float>(512, 0.1f));
        std::vector<std::vector<float>> keys(10, std::vector<float>(512, 0.1f));
        std::vector<std::vector<float>> values(10, std::vector<float>(512, 0.1f));
        
        // Compute attention
        auto output = attention.computeMultiHeadAttention(queries, keys, values);
        
        std::cout << "Query sequences: " << queries.size() << std::endl;
        std::cout << "Key sequences: " << keys.size() << std::endl;
        std::cout << "Value sequences: " << values.size() << std::endl;
        std::cout << "Attention demonstration completed successfully!" << std::endl;
    }
    
    void demonstrateSkipConnections() {
        std::cout << "\n=== Advanced Skip Connections and Residual Networks ===" << std::endl;
        
        ResidualNetwork residual;
        residual.setNumBlocks(5);
        residual.setUseBatchNorm(true);
        residual.setUseSkipProjection(true);
        residual.setActivationFunction("leaky_relu");
        
        // Build network
        residual.buildResidualNetwork(5, 10, 64, 5);  // 5 blocks, 10->64->5
        
        // Create sample input
        std::vector<float> input(10, 0.5f);
        
        // Forward pass
        std::vector<float> output = residual.forward(input);
        
        std::cout << "Input dimension: " << input.size() << std::endl;
        std::cout << "Output dimension: " << output.size() << std::endl;
        std::cout << "Residual network demonstration completed successfully!" << std::endl;
    }
    
    void demonstrateFeatureExtraction() {
        std::cout << "\n=== Advanced Feature Extraction and Hierarchical Processing ===" << std::endl;
        
        HierarchicalFeatureExtractor extractor;
        extractor.setNumLevels(4);
        extractor.setBaseFeatureDim(64);
        extractor.setHierarchicalDepth(3);
        
        // Create sample input
        std::vector<float> input(100, 0.5f);
        
        // Extract hierarchical features
        auto features = extractor.extractHierarchicalFeatures(input);
        
        std::cout << "Input dimension: " << input.size() << std::endl;
        std::cout << "Number of hierarchical feature levels: " << features.size() << std::endl;
        for (size_t i = 0; i < features.size(); ++i) {
            std::cout << "Level " << i << " features: " << features[i].features.size() << std::endl;
        }
        std::cout << "Feature extraction demonstration completed successfully!" << std::endl;
    }
    
    void demonstrateAdvancedActivations() {
        std::cout << "\n=== Advanced Activation Functions ===" << std::endl;
        
        std::vector<float> test_values = {-2.0f, -1.0f, -0.5f, 0.0f, 0.5f, 1.0f, 2.0f};
        
        std::cout << "Testing different activation functions:" << std::endl;
        for (float val : test_values) {
            std::cout << "  Input: " << val << std::endl;
            std::cout << "    Swish: " << AdvancedActivations::swish(val) << std::endl;
            std::cout << "    Mish: " << AdvancedActivations::mish(val) << std::endl;
            std::cout << "    GELU: " << AdvancedActivations::gelu(val) << std::endl;
            std::cout << "    SELU: " << AdvancedActivations::selu(val) << std::endl;
        }
        
        std::cout << "Advanced activation functions demonstration completed successfully!" << std::endl;
    }
    
    void runAllNeuralNetworkExamples() {
        demonstrateDendriticNetworks();
        demonstrateAttentionMechanisms();
        demonstrateSkipConnections();
        demonstrateFeatureExtraction();
        demonstrateAdvancedActivations();
    }

private:
    bool network_initialized_;
};

// Example 2: Enhanced Plasticity Integration
class EnhancedPlasticityExample {
public:
    EnhancedPlasticityExample() : plasticity_initialized_(false) {}
    
    void demonstrateContextDependentHebbian() {
        std::cout << "\n=== Context-Dependent Hebbian Plasticity ===" << std::endl;
        
        ContextDependentHebbian hebbian;
        hebbian.setLearningRate(0.01f);
        hebbian.setContextDecay(0.9f);
        hebbian.setNoveltyWeight(1.0f);
        
        // Create sample context
        ContextDependentHebbian::HebbianContext context;
        context.pre_neuron = NeuronId(1);
        context.post_neuron = NeuronId(2);
        context.contextual_features = {0.1f, 0.2f, 0.3f};
        context.temporal_context = 1.0f;
        context.novelty_score = 0.5f;
        
        // Create mock synapse
        Synapse mock_synapse;
        
        // Compute weight change
        float weight_change = hebbian.computeWeightChange(&mock_synapse, context, 1.0f);
        
        std::cout << "Weight change: " << weight_change << std::endl;
        std::cout << "Context-dependent Hebbian demonstration completed successfully!" << std::endl;
    }
    
    void demonstrateMetaLearningPlasticity() {
        std::cout << "\n=== Meta-Learning Plasticity ===" << std::endl;
        
        MetaLearningPlasticity meta_learning;
        meta_learning.setBaseLearningRate(0.01f);
        meta_learning.setMetaLearningRate(0.001f);
        meta_learning.setAdaptationWindow(100);
        
        // Create mock synapse
        Synapse mock_synapse;
        SynapseId syn_id = SynapseId(1);
        
        // Create meta-learning state
        MetaLearningPlasticity::MetaLearningState meta_state;
        meta_state.learning_rate = 0.01f;
        meta_state.prediction_error = 0.5f;
        
        // Perform meta-learning update
        meta_learning.performMetaLearningUpdate(&mock_synapse, 0.8f, meta_state, 0.01f);
        
        // Get meta-learning state
        auto retrieved_state = meta_learning.getMetaLearningState(syn_id);
        
        std::cout << "Meta-learning state confidence: " << retrieved_state.confidence << std::endl;
        std::cout << "Meta-learning plasticity demonstration completed successfully!" << std::endl;
    }
    
    void demonstrateSynapticPruning() {
        std::cout << "\n=== Synaptic Pruning and Dropout ===" << std::endl;
        
        SynapticPruning pruning;
        pruning.setPruningRate(0.1f);
        pruning.setRecoveryRate(0.01f);
        pruning.setImportanceDecay(0.99f);
        pruning.setMinSynapses(10);
        
        // Create mock synapses
        std::vector<Synapse*> synapses;
        for (size_t i = 0; i < 20; ++i) {
            synapses.push_back(new Synapse(SynapseId(i), NeuronId(i), NeuronId(i + 1)));
        }
        
        // Apply synaptic dropout
        std::mt19937 rng(std::random_device{}());
        pruning.applySynapticDropout(synapses, 0.2f, rng);
        
        // Determine pruning targets
        SynapticPruning::PruningCriteria criteria;
        criteria.weight_threshold = 0.01f;
        criteria.activity_threshold = 0.001f;
        
        auto targets = pruning.determinePruningTargets(synapses, criteria);
        
        std::cout << "Synapses before dropout: " << synapses.size() << std::endl;
        std::cout << "Pruning targets: " << targets.size() << std::endl;
        
        // Cleanup
        for (Synapse* synapse : synapses) {
            delete synapse;
        }
        
        std::cout << "Synaptic pruning demonstration completed successfully!" << std::endl;
    }
    
    void demonstrateNeuromodulatorGatedPlasticity() {
        std::cout << "\n=== Neuromodulator-Gated Plasticity ===" << std::endl;
        
        NeuromodulatorGatedPlasticity neuromodulator;
        neuromodulator.setPlasticitySensitivity(1.0f);
        
        // Create mock synapse
        Synapse mock_synapse;
        
        // Create neuromodulator state
        NeuromodulatorGatedPlasticity::NeuromodulatorState neuromodulators;
        neuromodulators.dopamine = 1.0f;
        neuromodulators.acetylcholine = 0.8f;
        neuromodulators.serotonin = 0.6f;
        neuromodulators.noradrenaline = 0.9f;
        
        // Apply gated plasticity
        float raw_change = 0.1f;
        float gated_change = neuromodulator.applyGatedPlasticity(&mock_synapse, raw_change,
                                                                neuromodulators, 0.01f);
        
        std::cout << "Raw plasticity change: " << raw_change << std::endl;
        std::cout << "Gated plasticity change: " << gated_change << std::endl;
        std::cout << "Neuromodulator-gated plasticity demonstration completed successfully!" << std::endl;
    }
    
    void runAllPlasticityExamples() {
        demonstrateContextDependentHebbian();
        demonstrateMetaLearningPlasticity();
        demonstrateSynapticPruning();
        demonstrateNeuromodulatorGatedPlasticity();
    }

private:
    bool plasticity_initialized_;
};

// Example 3: Advanced Memory Systems Integration
class AdvancedMemoryExample {
public:
    AdvancedMemoryExample() : memory_initialized_(false) {}
    
    void demonstrateHierarchicalMemory() {
        std::cout << "\n=== Hierarchical Memory Organization ===" << std::endl;
        
        HierarchicalMemory memory;
        memory.setMaxMemorySize(10000);
        memory.setHierarchyDepth(5);
        memory.setConsolidationInterval(100);
        
        // Store some patterns
        std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        std::vector<float> pattern2 = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
        
        memory.store(pattern1, 0.8f, NeuronId(1), 1.0f);
        memory.store(pattern2, 0.9f, NeuronId(2), 2.0f);
        
        // Retrieve with temporal filtering
        auto result = memory.retrieve(2.0f, 1.5f);
        
        std::cout << "Stored patterns: 2" << std::endl;
        std::cout << "Retrieved pattern size: " << result.size() << std::endl;
        
        // Get memory statistics
        auto stats = memory.getMemoryStats();
        std::cout << "Total nodes: " << stats.total_nodes << std::endl;
        std::cout << "Average stability: " << stats.average_stability << std::endl;
        
        std::cout << "Hierarchical memory demonstration completed successfully!" << std::endl;
    }
    
    void demonstrateTemporalMemory() {
        std::cout << "\n=== Temporal Memory and Sequence Prediction ===" << std::endl;
        
        TemporalMemory temporal;
        temporal.setSequenceMemorySize(100);
        temporal.setPredictionWindow(10);
        
        // Store a sequence
        std::vector<std::vector<float>> sequence;
        std::vector<TemporalMemory::Timestamp> timestamps;
        
        for (int i = 0; i < 5; ++i) {
            sequence.push_back(std::vector<float>(3, static_cast<float>(i) * 0.1f));
            timestamps.push_back(static_cast<TemporalMemory::Timestamp>(i * 1.0f));
        }
        
        temporal.storeSequence(sequence, timestamps);
        
        // Predict next pattern
        TemporalMemory::PredictionContext context;
        context.current_state = {0.4f, 0.5f, 0.6f};
        context.recent_context = {0.3f, 0.4f, 0.5f};
        
        auto prediction = temporal.predictNextPattern(context);
        
        std::cout << "Sequence length: " << sequence.size() << std::endl;
        std::cout << "Predicted next pattern size: " << prediction.size() << std::endl;
        
        // Get temporal statistics
        auto stats = temporal.getTemporalStats();
        std::cout << "Total sequences: " << stats.total_sequences << std::endl;
        std::cout << "Average prediction accuracy: " << stats.average_prediction_accuracy << std::endl;
        
        std::cout << "Temporal memory demonstration completed successfully!" << std::endl;
    }
    
    void demonstrateMemoryConsolidation() {
        std::cout << "\n=== Memory Consolidation and Interference Management ===" << std::endl;
        
        MemoryConsolidation consolidation;
        consolidation.setConsolidationRate(0.05f);
        consolidation.setReactiveConsolidation(true);
        consolidation.setSystematicPhase(true);
        consolidation.setInterferenceManagement(true);
        
        // Create mock memory nodes (simplified)
        HierarchicalMemory::MemoryNode mock_node1;
        mock_node1.timestamp = 1.0f;
        mock_node1.source_neuron = NeuronId(1);
        mock_node1.pattern = {0.1f, 0.2f, 0.3f};
        mock_node1.strength = 0.8f;
        
        // Perform consolidation
        auto event = consolidation.consolidate(&mock_node1, 2.0f);
        
        std::cout << "Consolidation attempted: " << (event.was_consolidated ? "Yes" : "No") << std::endl;
        std::cout << "Interference reduction: " << event.interference_reduction << std::endl;
        
        // Get consolidation statistics
        auto stats = consolidation.getConsolidationStats();
        std::cout << "Total consolidation events: " << stats.total_events << std::endl;
        std::cout << "Successful consolidations: " << stats.successful_consolidations << std::endl;
        
        std::cout << "Memory consolidation demonstration completed successfully!" << std::endl;
    }
    
    void demonstrateMemoryReplay() {
        std::cout << "\n=== Memory Replay and Optimization ===" << std::endl;
        
        MemoryReplay replay;
        replay.setMaxReplayTraces(50);
        replay.setReplayFrequency(0.1f);
        replay.setOptimizationIterations(5);
        
        // Create mock replay trace (simplified)
        MemoryReplay::ReplayTrace mock_trace;
        mock_trace.trace_id = 1;
        mock_trace.trace_strength = 0.8f;
        
        // Replay the trace
        replay.replayTrace(mock_trace, 1.0f);
        
        // Get replay statistics
        auto stats = replay.getReplayStats();
        std::cout << "Total replays: " << stats.total_replays << std::endl;
        std::cout << "Optimal replays: " << stats.optimal_replays << std::endl;
        std::cout << "Average replay strength: " << stats.average_replay_strength << std::endl;
        
        std::cout << "Memory replay demonstration completed successfully!" << std::endl;
    }
    
    void demonstrateMetaLearningMemory() {
        std::cout << "\n=== Meta-Learning Memory ===" << std::endl;
        
        MetaLearningMemory meta_memory;
        meta_memory.setMetaMemorySize(100);
        meta_memory.setMetaLearningRate(0.01f);
        meta_memory.setMetaPredictive(true);
        
        // Store meta-learning experience
        MetaLearningMemory::MetaLearningEntry meta_entry;
        meta_entry.timestamp = 1.0f;
        meta_entry.meta_features = {0.1f, 0.2f, 0.3f};
        meta_entry.meta_target = 0.5f;
        meta_entry.meta_prediction = 0.6f;
        meta_entry.meta_gradient = 0.1f;
        meta_entry.learning_effectiveness = 0.8f;
        
        meta_memory.storeMetaExperience(meta_entry);
        
        // Predict learning outcome
        float prediction = meta_memory.predictLearningOutcome({0.2f, 0.3f, 0.4f});
        
        std::cout << "Predicted learning outcome: " << prediction << std::endl;
        
        // Update meta model
        meta_memory.updateMetaModel(meta_entry, 0.9f);
        
        // Get meta-learning statistics
        auto stats = meta_memory.getMetaLearningStats();
        std::cout << "Total meta entries: " << stats.total_entries << std::endl;
        std::cout << "Average prediction accuracy: " << stats.average_prediction_accuracy << std::endl;
        
        std::cout << "Meta-learning memory demonstration completed successfully!" << std::endl;
    }
    
    void runAllMemoryExamples() {
        demonstrateHierarchicalMemory();
        demonstrateTemporalMemory();
        demonstrateMemoryConsolidation();
        demonstrateMemoryReplay();
        demonstrateMetaLearningMemory();
    }

private:
    bool memory_initialized_;
};

// Example 4: Advanced Prediction and Planning Integration
class AdvancedPredictionExample {
public:
    AdvancedPredictionExample() : prediction_initialized_(false) {}
    
    void demonstrateHierarchicalPlanning() {
        std::cout << "\n=== Hierarchical Planning with Tree Search ===" << std::endl;
        
        HierarchicalPlanner planner;
        planner.setMaxTreeDepth(10);
        planner.setMCTSSimulations(1000);
        
        // Create sample state
        std::vector<float> current_state = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        
        // Create available actions
        std::vector<HierarchicalPlanner::PlanningAction> actions;
        for (size_t i = 0; i < 5; ++i) {
            HierarchicalPlanner::PlanningAction action;
            action.action_id = i;
            action.action_features = std::vector<float>(5, static_cast<float>(i) * 0.1f);
            action.expected_outcomes = std::vector<float>(3, static_cast<float>(i) * 0.2f);
            action.probability = 0.8f - i * 0.1f;
            action.value = 0.5f + i * 0.1f;
            actions.push_back(action);
        }
        
        // Plan actions
        auto planned_actions = planner.planActions(current_state, actions);
        
        std::cout << "Current state dimension: " << current_state.size() << std::endl;
        std::cout << "Available actions: " << actions.size() << std::endl;
        std::cout << "Planned actions: " << planned_actions.size() << std::endl;
        
        // Get planning statistics
        auto stats = planner.getPlanningStats();
        std::cout << "Total planning steps: " << stats.total_planning_steps << std::endl;
        std::cout << "Planning efficiency: " << stats.planning_efficiency << "%" << std::endl;
        
        std::cout << "Hierarchical planning demonstration completed successfully!" << std::endl;
    }
    
    void demonstrateMonteCarloTreeSearch() {
        std::cout << "\n=== Monte Carlo Tree Search ===" << std::endl;
        
        MonteCarloTreeSearch mcts;
        mcts.setMaxTreeSize(100);
        mcts.setSimulationHorizon(20);
        mcts.setRolloutPolicy("uct");
        
        // Create sample state and actions
        std::vector<float> initial_state = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        std::vector<std::vector<float>> available_actions;
        for (size_t i = 0; i < 5; ++i) {
            available_actions.push_back(std::vector<float>(5, static_cast<float>(i) * 0.1f));
        }
        
        // Perform MCTS search
        auto result = mcts.search(initial_state, available_actions);
        
        std::cout << "Selected action: " << result.selected_action << std::endl;
        std::cout << "Confidence: " << result.confidence << std::endl;
        std::cout << "Estimated value: " << result.estimated_value << std::endl;
        
        // Get MCTS statistics
        auto stats = mcts.getMCStats();
        std::cout << "Total simulations: " << stats.total_simulations << std::endl;
        std::cout << "Successful searches: " << stats.successful_searches << std::endl;
        
        std::cout << "Monte Carlo Tree Search demonstration completed successfully!" << std::endl;
    }
    
    void demonstrateForwardModeling() {
        std::cout << "\n=== Forward Modeling and Inverse Planning ===" << std::endl;
        
        ForwardModeling forward_model;
        forward_model.setLearningRate(0.01f);
        forward_model.setModelComplexity(5);
        forward_model.setHorizonLength(10);
        
        // Learn forward model
        forward_model.learnForwardModel(
            std::vector<float>{0.1f, 0.2f, 0.3f},
            std::vector<float>{0.2f, 0.3f, 0.4f},
            std::vector<float>{0.5f}
        );
        
        // Predict future state
        auto prediction = forward_model.predictFutureState(
            std::vector<float>{0.1f, 0.2f, 0.3f},
            std::vector<float>{0.5f},
            1.0f
        );
        
        std::cout << "Prediction size: " << prediction.size() << std::endl;
        
        // Perform inverse planning
        std::vector<HierarchicalPlanner::PlanningAction> actions;
        for (size_t i = 0; i < 3; ++i) {
            HierarchicalPlanner::PlanningAction action;
            action.action_id = i;
            action.action_features = std::vector<float>(3, static_cast<float>(i) * 0.1f);
            actions.push_back(action);
        }
        
        auto solution = forward_model.inversePlan(
            std::vector<float>{0.1f, 0.2f, 0.3f},
            actions,
            5
        );
        
        std::cout << "Best action: " << solution.best_action_id << std::endl;
        std::cout << "Solution cost: " << solution.solution_cost << std::endl;
        std::cout << "Solution confidence: " << solution.solution_confidence << std::endl;
        
        // Get forward model statistics
        auto stats = forward_model.getForwardModelStats();
        std::cout << "Total transitions learned: " << stats.total_transitions_learned << std::endl;
        std::cout << "Average model accuracy: " << stats.average_model_accuracy << std::endl;
        
        std::cout << "Forward modeling demonstration completed successfully!" << std::endl;
    }
    
    void demonstrateGoalConditionedPolicies() {
        std::cout << "\n=== Goal-Conditioned Policies ===" << std::endl;
        
        GoalConditionedPolicies goal_policies;
        goal_policies.setNumPolicies(3);
        goal_policies.setGoalLearningRate(0.01f);
        goal_policies.setPolicyAdaptationRate(0.1f);
        
        // Create policy state
        GoalConditionedPolicies::PolicyState policy_state;
        policy_state.current_goal = {0.1f, 0.2f, 0.3f};
        policy_state.goal_progress = 0.5f;
        policy_state.goal_urgency = 0.8f;
        
        // Create available actions
        std::vector<HierarchicalPlanner::PlanningAction> actions;
        for (size_t i = 0; i < 3; ++i) {
            HierarchicalPlanner::PlanningAction action;
            action.action_id = i;
            action.action_features = std::vector<float>(3, static_cast<float>(i) * 0.1f);
            actions.push_back(action);
        }
        
        // Execute policy
        auto result = goal_policies.executePolicy(policy_state, actions);
        
        std::cout << "Policy execution result size: " << result.size() << std::endl;
        
        // Learn policy
        goal_policies.learnPolicy(
            policy_state,
            std::vector<float>{0.2f, 0.3f, 0.4f},
            actions
        );
        
        // Set goal
        goal_policies.setGoal(
            std::vector<float>{0.5f, 0.6f, 0.7f},
            std::vector<float>{0.1f, 0.2f, 0.3f}
        );
        
        // Get policy statistics
        auto stats = goal_policies.getPolicyStats();
        std::cout << "Total policies learned: " << stats.total_policies_learned << std::endl;
        std::cout << "Goal achievement rate: " << stats.goal_achievement_rate << std::endl;
        
        std::cout << "Goal-conditioned policies demonstration completed successfully!" << std::endl;
    }
    
    void demonstratePreferenceLearning() {
        std::cout << "\n=== Reranking and Preference Learning ===" << std::endl;
        
        PreferenceLearning preference;
        preference.setPreferenceMemorySize(100);
        preference.setLearningRate(0.01f);
        preference.setRerankingIterations(3);
        
        // Learn preferences
        PreferenceLearning::PreferenceData pref_data;
        pref_data.timestamp = 1.0f;
        pref_data.query_context = {0.1f, 0.2f, 0.3f};
        pref_data.preferred_actions = {0, 1};
        pref_data.dispreferred_actions = {2, 3};
        pref_data.preference_strength = {0.8f, 0.7f, 0.3f, 0.2f};
        
        preference.learnFromPreferences(pref_data);
        
        // Create sample actions
        std::vector<HierarchicalPlanner::PlanningAction> actions;
        for (size_t i = 0; i < 4; ++i) {
            HierarchicalPlanner::PlanningAction action;
            action.action_id = i;
            action.action_features = std::vector<float>(3, static_cast<float>(i) * 0.1f);
            actions.push_back(action);
        }
        
        // Rank actions
        auto rankings = preference.rankActions(actions, pref_data.query_context);
        
        std::cout << "Action rankings: ";
        for (size_t action_id : rankings) {
            std::cout << action_id << " ";
        }
        std::cout << std::endl;
        
        // Get recommendations
        auto recommendations = preference.getRecommendations(actions, pref_data.query_context, 2);
        
        std::cout << "Top recommendations: ";
        for (size_t action_id : recommendations) {
            std::cout << action_id << " ";
        }
        std::cout << std::endl;
        
        // Get preference statistics
        auto stats = preference.getPreferenceStats();
        std::cout << "Total preferences learned: " << stats.total_preferences_learned << std::endl;
        std::cout << "Average preference strength: " << stats.average_preference_strength << std::endl;
        
        std::cout << "Preference learning demonstration completed successfully!" << std::endl;
    }
    
    void runAllPredictionExamples() {
        demonstrateHierarchicalPlanning();
        demonstrateMonteCarloTreeSearch();
        demonstrateForwardModeling();
        demonstrateGoalConditionedPolicies();
        demonstratePreferenceLearning();
    }

private:
    bool prediction_initialized_;
};

// Main demonstration function
void runComprehensiveAdvancedNLMDemonstration() {
    std::cout << "=== Comprehensive Advanced NLM Framework Demonstration ===" << std::endl;
    std::cout << std::endl;
    
    // Run all examples
    AdvancedNeuralNetworkExample neural_network_example;
    neural_network_example.runAllNeuralNetworkExamples();
    
    EnhancedPlasticityExample plasticity_example;
    plasticity_example.runAllPlasticityExamples();
    
    AdvancedMemoryExample memory_example;
    memory_example.runAllMemoryExamples();
    
    AdvancedPredictionExample prediction_example;
    prediction_example.runAllPredictionExamples();
    
    std::cout << std::endl;
    std::cout << "=== All Advanced NLM Features Demonstrated Successfully ===" << std::endl;
}

} // namespace nlm
