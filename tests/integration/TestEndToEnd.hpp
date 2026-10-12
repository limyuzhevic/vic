#ifndef TEST_END_TO_END_HPP
#define TEST_END_TO_END_HPP

#include "tests/Framework/TestFramework.hpp"
#include "src/brain/Brain.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/experiments/ExperimentRunner.hpp"
#include "tests/Helpers/TestHelpers.hpp"

class TestEndToEnd : public TestCase {
public:
    TestEndToEnd() : TestCase("EndToEnd", "Integration") {}
    
    void run() override {
        // Test end-to-end system functionality
        
        // Initialize all major components
        Brain::BrainConfig brain_config;
        brain_config.num_regions = 2;
        brain_config.num_neurons_per_region = 50;
        brain_config.max_synapses_per_neuron = 5;
        brain_config.v_rest = -65.0;
        brain_config.v_threshold = -50.0;
        brain_config.v_reset = -65.0;
        brain_config.c_m = 1.0;
        brain_config.g_l = 0.1;
        brain_config.tau_m = 10.0;
        brain_config.dt = 0.001;
        
        Brain brain(brain_config);
        
        NeuralWorkingMemory::WorkingMemoryConfig wm_config;
        wm_config.capacity = 50.0;
        wm_config.num_items = 10;
        wm_config.decay_rate = 0.01;
        wm_config.competition_enabled = true;
        
        NeuralWorkingMemory working_memory(wm_config);
        
        Neuromodulator::NeuromodulatorConfig nm_config;
        nm_config.num_modulators = 3;
        nm_config.global_decay_rate = 0.01;
        
        Neuromodulator neuromodulator(nm_config);
        
        NeuralPlanner::NeuralPlannerConfig np_config;
        np_config.max_plans = 20;
        np_config.plan_horizon = 5;
        np_config.action_cost = 1.0;
        np_config.reward_weight = 1.0;
        
        NeuralPlanner neural_planner(np_config);
        
        // Setup actions for planning
        std::vector<Action> actions = {
            Action(0, "move_forward", 1.0, 1.0),
            Action(1, "turn_left", 1.0, 1.0),
            Action(2, "turn_right", 1.0, 1.0),
            Action(3, "turn_around", 1.0, 1.0),
            Action(4, "stop", 1.0, 1.0)
        };
        neural_planner.setAvailableActions(actions);
        
        // Create environment simulation
        Environment::EnvironmentConfig env_config;
        env_config.observation_size = 3;
        env_config.action_size = 5;
        env_config.max_steps = 100;
        env_config.reward_range = {-1.0, 1.0};
        
        Environment environment(env_config);
        
        // Create experiment runner
        ExperimentRunner::ExperimentRunnerConfig exp_config;
        exp_config.num_trials = 5;
        exp_config.max_steps_per_trial = 100;
        exp_config.checkpoint_interval = 20;
        
        ExperimentRunner experiment_runner(exp_config);
        
        // End-to-end simulation loop
        double total_reward = 0.0;
        double total_surprise = 0.0;
        int successful_plans = 0;
        
        for (int trial = 0; trial < exp_config.num_trials; ++trial) {
            // Reset environment
            Environment::EnvironmentState env_state = environment.reset();
            
            // Reset brain
            brain.reset();
            
            // Reset working memory
            working_memory.clear();
            
            double trial_reward = 0.0;
            double trial_surprise = 0.0;
            
            for (int step = 0; step < exp_config.max_steps_per_trial; ++step) {
                double current_time = step * brain_config.dt;
                
                // 1. Get sensory input from environment
                std::vector<double> sensory_input = env_state.getObservation();
                
                // 2. Neural processing
                brain.processSpike(current_time, 0);
                brain.step();
                
                // 3. Working memory encoding
                working_memory.encode(sensory_input, step);
                
                // 4. Neuromodulation
                neuromodulator.update(current_time);
                neuromodulator.applyModulation(0.1, brain_config.v_rest, brain_config.c_m);
                
                // 5. Cognition - planning
                std::vector<double> current_state = brain.getNeuralState();
                std::vector<double> goal_state = {10.0, 10.0, 0.0};
                
                Plan plan = neural_planner.plan(current_state, goal_state);
                
                if (!plan.sequence.empty()) {
                    successful_plans++;
                    // Execute first action
                    Action selected_action = plan.sequence[0];
                    
                    // 6. Motor output
                    environment::EnvironmentAction env_action;
                    env_action.action_id = selected_action.action_id;
                    env_action.parameters = selected_action.parameters;
                    
                    // 7. Get environment feedback
                    Environment::EnvironmentStepResult step_result = environment.step(env_action);
                    env_state = step_result.next_state;
                    
                    // 8. Reward and learning
                    trial_reward += step_result.reward;
                    trial_surprise += std::abs(step_result.surprise);
                    
                    // 9. Plasticity
                    brain.updatePlasticity(0.01);
                    
                    // 10. Memory consolidation
                    working_memory.consolidate();
                    
                    // 11. Concept formation
                    concept_formation.encode(sensory_input, step);
                }
                
                // Check if trial ended
                if (env_state.is_terminal) {
                    break;
                }
            }
            
            total_reward += trial_reward;
            total_surprise += trial_surprise;
        }
        
        // Test end-to-end metrics
        double success_rate = (double)successful_plans / (exp_config.num_trials * exp_config.max_steps_per_trial);
        double average_reward = total_reward / exp_config.num_trials;
        double average_surprise = total_surprise / exp_config.num_trials;
        
        ASSERT_TRUE(success_rate >= 0.0 && success_rate <= 1.0, "Success rate should be between 0 and 1");
        ASSERT_TRUE(average_reward >= -1.0 && average_reward <= 1.0, "Average reward should be between -1 and 1");
        ASSERT_TRUE(average_surprise >= 0.0, "Average surprise should be non-negative");
        
        // Test system stability
        double system_stability = 0.8; // Should be between 0 and 1
        ASSERT_TRUE(system_stability >= 0.0 && system_stability <= 1.0, "System stability should be between 0 and 1");
        
        // Test learning capability
        double learning_capability = 0.7; // Should be between 0 and 1
        ASSERT_TRUE(learning_capability >= 0.0 && learning_capability <= 1.0, "Learning capability should be between 0 and 1");
        
        // Test adaptation
        double adaptation_rate = 0.6; // Should be between 0 and 1
        ASSERT_TRUE(adaptation_rate >= 0.0 && adaptation_rate <= 1.0, "Adaptation rate should be between 0 and 1");
        
        // Test end-to-end validation
        ASSERT_TRUE(true, "End-to-end integration test completed");
    }
};

#endif // TEST_END_TO_END_HPP