// Example demonstrating all advanced features of the NLM framework

#include <iostream>
#include <memory>
#include <vector>

#include "src/agent/AdvancedActions.hpp"
#include "src/core/AdvancedConfig.hpp"
#include "src/performance/BatchProcessor.hpp"
#include "src/visualization/AdvancedVisualization.hpp"
#include "src/learning/AdvancedLearning.hpp"
#include "src/extensions/PluginSystem.hpp"
#include "src/brain/Integration.hpp"

using namespace nlm;

void demonstrate_advanced_actions() {
    std::cout << "=== Advanced Actions Demo ===" << std::endl;
    
    AdvancedActions actions;
    
    // Create action template
    ActionTemplate template("MoveRobot", "Robot movement with parameters");
    template.add_parameter(ActionParameter("Speed", ActionParameter::Type::Float, 1.0f));
    template.add_parameter(ActionParameter("Direction", ActionParameter::Type::Int, 0));
    template.register_template(template);
    
    // Create action sequence
    ActionSequence sequence("RobotMovement");
    sequence.add_action(std::make_unique<Action>(ActionType::MoveForward));
    sequence.add_action(std::make_unique<Action>(ActionType::TurnRight), true); // Optional
    sequence.add_action(std::make_unique<Action>(ActionType::Look));
    
    // Execute sequence
    auto results = sequence.execute();
    std::cout << "Sequence executed " << results.size() << " actions" << std::endl;
    
    // Demonstrate validation and recovery
    ActionValidator validator;
    ActionRecovery recovery;
    
    auto action = std::make_unique<Action>(ActionType::MoveForward);
    auto result = actions.execute_action_with_recovery(std::move(action), recovery);
    
    std::cout << "Action result: " << (result.is_success() ? "Success" : "Failed") << std::endl;
}

void demonstrate_advanced_config() {
    std::cout << "\n=== Advanced Configuration Demo ===" << std::endl;
    
    AdvancedConfig config;
    
    // Load configuration
    config.loadConfig("config/advanced_config.json");
    
    // Validate configuration
    if (config.validateConfig()) {
        std::cout << "Configuration is valid" << std::endl;
    } else {
        auto errors = config.getConfigErrors();
        std::cout << "Configuration errors:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    // Use templates
    ConfigTemplates templates;
    nlohmann::json template_config = {
        {"speed", 2.0},
        {"precision", 0.99},
        {"timeout", 30.0}
    };
    templates.addTemplate("fast_mode", template_config);
    
    // Apply template
    auto fast_config = templates.applyTemplate("fast_mode");
    std::cout << "Applied template: " << fast_config.dump() << std::endl;
}

void demonstrate_batch_processing() {
    std::cout << "\n=== Batch Processing Demo ===" << std::endl;
    
    BatchProcessor processor;
    
    // Configure processor
    BatchConfig batch_config;
    batch_config.batch_size = 50;
    batch_config.max_threads = 4;
    batch_config.enable_parallel_processing = true;
    
    processor.setConfig(batch_config);
    
    // Create actions
    std::vector<std::unique_ptr<Action>> actions;
    for (int i = 0; i < 10; ++i) {
        actions.push_back(std::make_unique<Action>(ActionType::MoveForward));
    }
    
    // Process batch
    auto result = processor.processBatch(actions);
    
    std::cout << "Batch processed: " << result.successfulActions << " successful, " 
              << result.failedActions << " failed" << std::endl;
    
    // Monitor progress
    auto stats = processor.getStats();
    std::cout << "Throughput: " << stats.throughput << " actions/second" << std::endl;
}

void demonstrate_advanced_visualization() {
    std::cout << "\n=== Advanced Visualization Demo ===" << std::endl;
    
    AdvancedVisualization viz;
    
    // Configure visualization
    VisualizationConfig viz_config;
    viz_config.backend = "opengl";
    viz_config.enable_real_time = true;
    viz_config.render_mode = "solid";
    viz_config.color_scheme = "thermal";
    
    viz.setConfig(viz_config);
    
    // Create neural data
    std::vector<NeuronVisualization> neurons;
    neurons.push_back({0.0f, 0.0f, 0.0f, 0.5f, 0.2f, "#FF0000", true, false});
    neurons.push_back({1.0f, 0.0f, 0.0f, 0.8f, 0.1f, "#00FF00", false, true});
    
    std::vector<SynapseVisualization> synapses;
    synapses.push_back({0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.7f, "#808080", true});
    
    std::vector<RegionVisualization> regions;
    regions.push_back({0.0f, 0.0f, 0.0f, 2.0f, 2.0f, 2.0f, "#808080", 0.5f, "Cortex"});
    
    // Update visualization
    viz.updateNeuralData(neurons, synapses, regions);
    
    // Save visualization
    if (viz.saveCurrentView("neural_network.png", ExportFormat.PNG)) {
        std::cout << "Visualization saved successfully" << std::endl;
    }
    
    // Get statistics
    auto stats = viz.getStats();
    std::cout << "FPS: " << stats.fps << ", Triangles: " << stats.trianglesRendered << std::endl;
}

void demonstrate_advanced_learning() {
    std::cout << "\n=== Advanced Learning Demo ===" << std::endl;
    
    AdvancedLearning learner;
    
    // Configure learning
    LearningConfig learn_config;
    learn_config.learning_algorithm = "reinforcement_learning";
    learn_config.learning_rate = 0.01;
    learn_config.discount_factor = 0.99;
    learn_config.exploration_rate = 0.1;
    learn_config.enable_transfer_learning = true;
    learn_config.enable_meta_learning = true;
    
    learner.configure(learn_config);
    
    // Create learning experiences
    std::vector<LearningState> experiences;
    
    for (int i = 0; i < 5; ++i) {
        LearningState state;
        state.state_vector = {static_cast<float>(i) * 0.1f, 0.5f, 0.8f};
        state.action = (i % 2 == 0) ? ActionType::MoveForward : ActionType::TurnRight;
        state.reward = static_cast<float>(i + 1);
        state.next_state = {static_cast<float>(i) * 0.1f + 0.1f, 0.6f, 0.7f};
        state.done = (i == 4);
        state.episode_id = "episode_1";
        state.step_in_episode = i;
        
        experiences.push_back(state);
    }
    
    // Start training
    learner.startTraining(experiences);
    
    // Train on each experience
    for (const auto& experience : experiences) {
        learner.trainStep(experience);
    }
    
    // Stop training
    learner.stopTraining();
    
    // Get learning statistics
    auto stats = learner.getStats();
    std::cout << "Training completed: " << stats.totalSteps << " steps, " 
              << "average reward: " << stats.averageReward << std::endl;
    
    // Save model
    learner.saveModel("trained_model.model");
}

void demonstrate_plugin_system() {
    std::cout << "\n=== Plugin System Demo ===" << std::endl;
    
    PluginManager plugin_manager;
    
    // Initialize plugin manager
    if (plugin_manager.initialize()) {
        std::cout << "Plugin manager initialized successfully" << std::endl;
        
        // Load a plugin (example)
        std::string plugin_name = plugin_manager.loadPlugin("examples/my_plugin.so");
        
        if (!plugin_name.empty()) {
            std::cout << "Plugin loaded: " << plugin_name << std::endl;
            
            // Get plugin metadata
            auto metadata = plugin_manager.getPluginMetadata(plugin_name);
            std::cout << "Plugin version: " << metadata.version << std::endl;
            
            // Enable plugin
            plugin_manager.enablePlugin(plugin_name);
            
            // Call plugin API
            auto result = plugin_manager.callPluginApi(
                plugin_name,
                "get_info",
                nlohmann::json::object()
            );
            
            std::cout << "Plugin API result: " << result.dump() << std::endl;
        } else {
            std::cout << "Failed to load plugin" << std::endl;
        }
        
        // Shutdown plugins
        plugin_manager.shutdownAllPlugins();
    } else {
        std::cout << "Failed to initialize plugin manager" << std::endl;
    }
}

void demonstrate_integration() {
    std::cout << "\n=== Integration Demo ===" << std::endl;
    
    // Create brain (example - would use actual brain constructor)
    std::shared_ptr<Brain> brain = std::make_shared<Brain>();
    
    // Create integration
    BrainAdvancedIntegration integration;
    
    // Initialize integration
    if (integration.initialize(brain.get())) {
        std::cout << "Integration initialized successfully" << std::endl;
        
        // Get advanced actions
        auto action = integration.produceAdvancedAction();
        std::cout << "Advanced action produced" << std::endl;
        
        // Execute with neural feedback
        auto result = integration.executeActionInContext(
            std::move(action),
            {{ "energy", 0.8 }, { "confidence", 0.9 }}
        );
        
        std::cout << "Action executed: " << (result.is_success() ? "Success" : "Failed") << std::endl;
        
        // Get integration statistics
        auto stats = integration.getAdvancedActionStats();
        std::cout << "Integration stats: " << stats.totalActionsProduced << " actions" << std::endl;
    } else {
        std::cout << "Failed to initialize integration" << std::endl;
    }
}

int main() {
    std::cout << "NLM Advanced Features Demonstration" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        demonstrate_advanced_actions();
        demonstrate_advanced_config();
        demonstrate_batch_processing();
        demonstrate_advanced_visualization();
        demonstrate_advanced_learning();
        demonstrate_plugin_system();
        demonstrate_integration();
        
        std::cout << "\n" << std::endl;
        std::cout << "All demonstrations completed successfully!" << std::endl;
        std::cout << "\n" << std::endl;
        std::cout << "Advanced features are ready for use in your NLM applications." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during demonstration: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
