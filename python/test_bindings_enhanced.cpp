#include "bindings_enhanced.h"
#include <iostream>
#include <cassert>

void test_core_types() {
    std::cout << "Testing core types..." << std::endl;
    
    nlm::NeuronId id1;
    nlm::NeuronId id2(42);
    
    assert(id1.value == 0);
    assert(id2.value == 42);
    assert(id2.index() == 0);
    assert(id1 != id2);
    assert(id1 == nlm::NeuronId(0));
    
    // Test hash
    std::unordered_set<nlm::NeuronId> ids;
    ids.insert(id1);
    ids.insert(id2);
    assert(ids.size() == 2);
    
    std::cout << "✓ Core types test passed" << std::endl;
}

void test_config() {
    std::cout << "Testing config..." << std::endl;
    
    nlm::Config config;
    
    assert(!config.has("test"));
    config.set("test", "value");
    assert(config.has("test"));
    
    auto keys = config.getKeys();
    assert(std::find(keys.begin(), keys.end(), "test") != keys.end());
    
    config.clear();
    assert(!config.has("test"));
    
    std::cout << "✓ Config test passed" << std::endl;
}

void test_config_utilities() {
    std::cout << "Testing config utilities..." << std::endl;
    
    // Test create_default_config
    auto default_config = config::get_default_config();
    assert(!default_config.empty());
    assert(default_config.find("simulation.steps_per_episode") != default_config.end());
    
    // Test create_training_config
    auto training_config = config::create_training_config("", 0.01f, 1000, 100);
    assert(training_config->has("brain.learning_rate"));
    
    // Test merge_config
    nlm::Config config;
    config::merge_config(config, default_config);
    
    std::cout << "✓ Config utilities test passed" << std::endl;
}

void test_brain_factory() {
    std::cout << "Testing brain factory..." << std::endl;
    
    // Create config
    auto config = config::create_training_config("", 0.01f, 100, 10);
    
    // Create brain
    auto brain = brain::create_brain(config, true, "TestBrain");
    assert(brain != nullptr);
    assert(brain->initialize());
    
    // Test basic brain operations
    assert(brain->getTotalNeuronCount() >= 0);
    assert(brain->getTotalSynapseCount() >= 0);
    
    // Test brain methods
    brain->reset();
    
    std::cout << "✓ Brain factory test passed" << std::endl;
}

void test_agent() {
    std::cout << "Testing agent..." << std::endl;
    
    auto config = config::create_training_config("", 0.01f, 100, 10);
    auto brain = brain::create_brain(config, true, "TestBrain");
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    assert(agent->getBrain() == brain.get());
    assert(!agent->isRewardModulationEnabled());
    assert(!agent->isCuriosityEnabled());
    
    // Test enable/disable methods
    agent->enableRewardModulation(true);
    assert(agent->isRewardModulationEnabled());
    
    agent->enableCuriosity(true);
    assert(agent->isCuriosityEnabled());
    
    std::cout << "✓ Agent test passed" << std::endl;
}

void test_world() {
    std::cout << "Testing world..." << std::endl;
    
    nlm::SimpleWorld world;
    world.configure(100, 100, 16, 16);
    
    assert(world.getWidth() == 100);
    assert(world.getHeight() == 100);
    assert(world.getAgentBody().x >= 0);
    assert(world.getAgentBody().y >= 0);
    
    // Test world operations
    world.reset();
    world.setAgentStart(50, 50);
    
    std::cout << "✓ World test passed" << std::endl;
}

void test_sensory() {
    std::cout << "Testing sensory..." << std::endl;
    
    nlm::Vision vision(32, 32, 3);
    std::vector<float> data(32 * 32 * 3, 0.5f);
    
    assert(vision.getWidth() == 32);
    assert(vision.getHeight() == 32);
    assert(vision.getChannels() == 3);
    
    nlm::Audio audio(44100, 44100);
    assert(audio.getSampleRate() == 44100);
    assert(audio.getNumSamples() == 44100);
    
    std::cout << "✓ Sensory test passed" << std::endl;
}

void test_action() {
    std::cout << "Testing action..." << std::endl;
    
    nlm::Action action1;
    assert(action1.getType() == nlm::ActionType::Wait);
    
    nlm::Action action2(nlm::ActionType::MoveForward);
    assert(action2.getType() == nlm::ActionType::MoveForward);
    
    nlm::Action action3(nlm::ActionType::Custom, std::vector<float>{1.0f, 2.0f, 3.0f});
    assert(action3.getType() == nlm::ActionType::Custom);
    assert(action3.getParameters().size() == 3);
    
    // Test clone
    auto action4 = action3.clone();
    assert(action4->getType() == action3.getType());
    assert(action4->getParameters() == action3.getParameters());
    
    std::cout << "✓ Action test passed" << std::endl;
}

void test_memory() {
    std::cout << "Testing memory utilities..." << std::endl;
    
    auto config = config::create_training_config("", 0.01f, 100, 10);
    auto brain1 = brain::create_brain(config, true, "Brain1");
    
    // Test clone
    auto brain2 = memory::clone_brain(*brain1);
    assert(brain2 != nullptr);
    assert(brain2->getTotalNeuronCount() == brain1->getTotalNeuronCount());
    
    // Test memory stats
    auto stats = memory::getMemoryStats();
    assert(!stats.empty());
    
    std::cout << "✓ Memory utilities test passed" << std::endl;
}

void test_training_utilities() {
    std::cout << "Testing training utilities..." << std::endl;
    
    auto config = config::create_training_config("", 0.01f, 100, 10);
    auto brain = brain::create_brain(config, true, "TestBrain");
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    auto world = std::make_shared<nlm::SimpleWorld>();
    
    world->configure(50, 50, 8, 8);
    agent->initialize(*world);
    
    // Test evaluation
    float reward = training::evaluate(*brain, *agent, *world, 5, 10);
    assert(reward >= 0.0f); // Reward should be non-negative
    
    std::cout << "✓ Training utilities test passed" << std::endl;
}

void test_visualization() {
    std::cout << "Testing visualization utilities..." << std::endl;
    
    std::vector<training::EpisodeResult> metrics;
    training::EpisodeResult result;
    result.steps = 100;
    result.total_reward = 10.0f;
    result.average_reward = 0.1f;
    result.max_reward = 1.0f;
    result.std_reward = 0.2f;
    result.final_reward = 0.05f;
    result.success = true;
    metrics.push_back(result);
    
    // Test plotting (just ensure it doesn't crash)
    visualization::plot_training_metrics(metrics, "test_metrics.png", false);
    
    // Test brain visualization
    auto config = config::create_training_config("", 0.01f, 100, 10);
    auto brain = brain::create_brain(config, true, "TestBrain");
    visualization::visualize_brain(*brain, "test_brain.png", true, true, false);
    
    // Test dashboard
    visualization::create_training_dashboard(metrics, "test_dashboard");
    
    std::cout << "✓ Visualization utilities test passed" << std::endl;
}

void test_context_manager() {
    std::cout << "Testing context manager..." << std::endl;
    
    auto config = config::create_training_config("", 0.01f, 100, 10);
    auto brain = brain::create_brain(config, true, "TestBrain");
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    auto world = std::make_shared<nlm::SimpleWorld>();
    
    world->configure(50, 50, 8, 8);
    agent->initialize(*world);
    
    {
        BrainSession session(brain, agent, world);
        
        // Test session methods
        assert(&session.get_brain() == brain.get());
        assert(&session.get_agent() == agent.get());
        assert(&session.get_world() == world.get());
        
        // Test step
        bool step_result = session.step(1, 0.0f);
        assert(step_result); // Should succeed
        
        // Test reset
        session.reset();
        
        // Test initialize
        session.initialize();
    }
    
    std::cout << "✓ Context manager test passed" << std::endl;
}

void test_export() {
    std::cout << "Testing export utilities..." << std::endl;
    
    auto config = config::create_training_config("", 0.01f, 100, 10);
    auto brain = brain::create_brain(config, true, "TestBrain");
    
    // Test TensorFlow export
    export::export_to_tensorflow(*brain, "test_tf_model");
    
    // Test ONNX export
    std::vector<std::vector<size_t>> input_shapes = {{1, 3, 32, 32}};
    std::vector<std::string> input_names = {"vision"};
    std::vector<std::vector<size_t>> output_shapes = {{1, 10}};
    std::vector<std::string> output_names = {"action"};
    export::export_to_onnx(*brain, "test_onnx_model", input_shapes, input_names, output_shapes, output_names);
    
    // Test pickle export
    export::export_to_pickle(*brain, "test_pickle_model", true, true);
    
    std::cout << "✓ Export utilities test passed" << std::endl;
}

int main() {
    try {
        std::cout << "Running enhanced Python bindings tests..." << std::endl;
        std::cout << "=============================================" << std::endl;
        
        test_core_types();
        test_config();
        test_config_utilities();
        test_brain_factory();
        test_agent();
        test_world();
        test_sensory();
        test_action();
        test_memory();
        test_training_utilities();
        test_visualization();
        test_context_manager();
        test_export();
        
        std::cout << "=============================================" << std::endl;
        std::cout << "All tests passed! ✓" << std::endl;
        std::cout << "Enhanced Python bindings are working correctly." << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}
