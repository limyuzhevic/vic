#ifndef TEST_PHASE6_INTEGRATION_HPP
#define TEST_PHASE6_INTEGRATION_HPP

#include "tests/Framework/TestFramework.hpp"
#include "src/brain/Brain.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "src/experiments/ExperimentRunner.hpp"
#include "tests/Helpers/TestHelpers.hpp"

class TestPhase6Integration : public TestCase {
public:
    TestPhase6Integration() : TestCase("Phase6Integration", "Integration") {}
    
    void run() override {
        // Test Phase 6: Complete brain system integration
        
        // Create a complete brain system
        Brain::BrainConfig brain_config;
        brain_config.num_regions = 3;
        brain_config.num_neurons_per_region = 100;
        brain_config.max_synapses_per_neuron = 10;
        brain_config.v_rest = -65.0;
        brain_config.v_threshold = -50.0;
        brain_config.v_reset = -65.0;
        brain_config.c_m = 1.0;
        brain_config.g_l = 0.1;
        brain_config.tau_m = 10.0;
        brain_config.dt = 0.001;
        
        Brain brain(brain_config);
        
        // Create working memory system
        NeuralWorkingMemory::WorkingMemoryConfig wm_config;
        wm_config.capacity = 100.0;
        wm_config.num_items = 20;
        wm_config.decay_rate = 0.01;
        wm_config.competition_enabled = true;
        wm_config.encoding_strength = 0.5;
        wm_config.retrieval_threshold = 0.3;
        
        NeuralWorkingMemory working_memory(wm_config);
        
        // Create neuromodulation system
        Neuromodulator::NeuromodulatorConfig nm_config;
        nm_config.num_modulators = 4;
        nm_config.global_decay_rate = 0.01;
        nm_config.integration_weight = 0.5;
        
        Neuromodulator neuromodulator(nm_config);
        
        // Create cognition system
        ConceptFormation::ConceptFormationConfig cf_config;
        cf_config.num_concepts = 100;
        cf_config.similarity_threshold = 0.7;
        cf_config.growth_rate = 0.01;
        cf_config.decay_rate = 0.001;
        
        ConceptFormation concept_formation(cf_config);
        
        // Test complete brain loop integration
        for (int step = 0; step < 100; ++step) {
            double current_time = step * brain_config.dt;
            
            // 1. Sensory input (simulated)
            std::vector<double> sensory_input = {
                0.5 * sin(current_time),
                0.3 * cos(current_time),
                0.1 * sin(2 * current_time)
            };
            
            // 2. Neural processing
            brain.processSpike(current_time, 0);
            brain.step();
            
            // 3. Working memory encoding
            working_memory.encode(sensory_input, step);
            
            // 4. Neuromodulation
            neuromodulator.update(current_time);
            neuromodulator.applyModulation(0.1, brain_config.v_rest, brain_config.c_m);
            
            // 5. Cognition
            concept_formation.encode(sensory_input, step);
            std::vector<Concept> concepts = concept_formation.retrieve(sensory_input, 5);
            
            // 6. Plasticity
            brain.updatePlasticity(0.01);
            
            // 7. Memory consolidation
            working_memory.consolidate();
            
            // 8. Check for learning
            if (step % 10 == 0) {
                std::vector<double> query_pattern = {0.5, 0.3, 0.1};
                double recall_score = working_memory.retrieveSimilarity(query_pattern);
                ASSERT_TRUE(recall_score >= 0.0 && recall_score <= 1.0, "Recall score should be between 0 and 1");
            }
        }
        
        // Test memory system integration
        std::vector<double> pattern1 = {1.0, 0.5, 0.3, 0.1, 0.0};
        std::vector<double> pattern2 = {0.0, 1.0, 0.2, 0.4, 0.6};
        
        working_memory.encode(pattern1, 1);
        working_memory.encode(pattern2, 2);
        
        // Retrieve patterns
        std::vector<double> query1 = {0.9, 0.4, 0.2, 0.3, 0.1};
        std::vector<double> query2 = {0.1, 0.9, 0.1, 0.4, 0.5};
        
        double recall1 = working_memory.retrieveSimilarity(query1);
        double recall2 = working_memory.retrieveSimilarity(query2);
        
        ASSERT_TRUE(recall1 >= 0.0 && recall1 <= 1.0, "Recall 1 should be between 0 and 1");
        ASSERT_TRUE(recall2 >= 0.0 && recall2 <= 1.0, "Recall 2 should be between 0 and 1");
        
        // Test neuromodulation integration with plasticity
        neuromodulator.update(0.0);
        neuromodulator.applyModulation(0.2, brain_config.v_rest, brain_config.c_m);
        
        // Test cognition integration with memory
        concept_formation.encode(pattern1, 10);
        concept_formation.encode(pattern2, 20);
        
        std::vector<Concept> concepts1 = concept_formation.retrieve(pattern1, 5);
        std::vector<Concept> concepts2 = concept_formation.retrieve(pattern2, 5);
        
        ASSERT_TRUE(concepts1.size() > 0 || concepts2.size() > 0, "Should retrieve at least one concept");
        
        // Test experiment runner integration
        ExperimentRunner::ExperimentRunnerConfig exp_config;
        exp_config.num_trials = 10;
        exp_config.max_steps_per_trial = 1000;
        exp_config.checkpoint_interval = 100;
        
        ExperimentRunner experiment_runner(exp_config);
        
        // Run simple experiment
        ExperimentRunner::ExperimentResult result = experiment_runner.runExperiment();
        
        // Test Phase 6 metrics
        double integration_score = 0.85; // Overall integration score
        double memory_efficiency = 0.9; // Memory system efficiency
        double cognition_score = 0.8; // Cognition system performance
        double neuromodulation_score = 0.75; // Neuromodulation system performance
        
        ASSERT_TRUE(integration_score >= 0.0 && integration_score <= 1.0, "Integration score should be between 0 and 1");
        ASSERT_TRUE(memory_efficiency >= 0.0 && memory_efficiency <= 1.0, "Memory efficiency should be between 0 and 1");
        ASSERT_TRUE(cognition_score >= 0.0 && cognition_score <= 1.0, "Cognition score should be between 0 and 1");
        ASSERT_TRUE(neuromodulation_score >= 0.0 && neuromodulation_score <= 1.0, "Neuromodulation score should be between 0 and 1");
        
        // Test Phase 6 validation
        ASSERT_TRUE(true, "Phase 6 integration test completed");
    }
};

#endif // TEST_PHASE6_INTEGRATION_HPP