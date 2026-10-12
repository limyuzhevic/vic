// Visualization Test Suite
// Tests for NLMLiveVisualization and visualization system

#include "NLMLiveVisualization.hpp"
#include "VisualizationInterface.hpp"
#include "Brain.hpp"
#include "AgentBrain.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_benchmark.hpp>
#include <vector>
#include <memory>

namespace test_visualization {
    
    // Test fixtures
    class VisualizationTestFixture {
    public:
        std::unique_ptr<nlm::NLMLiveVisualization> visualizer_;
        std::shared_ptr<nlm::Config> config_;
        std::shared_ptr<nlm::Brain> brain_;
        std::unique_ptr<nlm::AgentBrain> agentBrain_;
        
        VisualizationTestFixture() {
            config_ = std::make_shared<nlm::Config>();
            config_->setVisionDimensions(32, 32);
            
            brain_ = std::make_shared<nlm::Brain>(config_);
            REQUIRE(brain_->initialize());
            
            agentBrain_ = std::make_unique<nlm::AgentBrain>(brain_, config_);
            
            // Initialize visualizer in headless mode
            visualizer_ = std::make_unique<nlm::NLMLiveVisualization>();
            REQUIRE(visualizer_->initialize(1920, 1080, true)); // Headless mode
        }
        
        ~VisualizationTestFixture() {
            if (visualizer_) {
                visualizer_->close();
            }
        }
    };
    
    // Visualization Initialization Tests
    TEST_CASE("NLMLiveVisualization Initialization", "[visualization][basic]") {
        nlm::NLMLiveVisualization visualizer;
        REQUIRE_NOTHROW(visualizer.initialize(1920, 1080, true));
        REQUIRE(visualizer.isActive());
        REQUIRE(visualizer.getUpdateRate() == 60.0);
    }
    
    TEST_CASE("NLMLiveVisualization Headless Mode", "[visualization][headless]") {
        nlm::NLMLiveVisualization visualizer;
        REQUIRE(visualizer.initialize(1024, 768, true));
        REQUIRE(visualizer.isActive());
        REQUIRE(visualizer.isPaused() == false);
        
        visualizer.togglePause(true);
        REQUIRE(visualizer.isPaused() == true);
        
        visualizer.togglePause(false);
        REQUIRE(visualizer.isPaused() == false);
    }
    
    TEST_CASE("Visualization Update Rate Control", "[visualization][config]") {
        nlm::NLMLiveVisualization visualizer;
        visualizer.initialize(1920, 1080, true);
        
        visualizer.setUpdateRate(30.0);
        REQUIRE(visualizer.getUpdateRate() == 30.0);
        
        visualizer.setUpdateRate(120.0);
        REQUIRE(visualizer.getUpdateRate() == 120.0);
    }
    
    TEST_CASE("Visualization Export Functionality", "[visualization][export]") {
        nlm::NLMLiveVisualization visualizer;
        visualizer.initialize(1920, 1080, true);
        
        // Test PNG export
        REQUIRE(visualizer.exportToPNG("/tmp/test_export.png") == true);
        
        // Test SVG export
        REQUIRE(visualizer.exportToSVG("/tmp/test_export.svg") == true);
        
        // Test JSON export
        REQUIRE(visualizer.exportToJSON("/tmp/test_export.json") == true);
        
        // Test MP4 export
        REQUIRE(visualizer.exportToMP4("/tmp/test_export.mp4", 30.0) == true);
    }
    
    TEST_CASE("Visualization Data Access", "[visualization][data]") {
        nlm::NLMLiveVisualization visualizer;
        visualizer.initialize(1920, 1080, true);
        
        // Get empty visualization data (before brain update)
        const auto& nodes = visualizer.getNeuralNodes();
        const auto& connections = visualizer.getNeuralConnections();
        
        // Initially should be empty
        REQUIRE(nodes.empty());
        REQUIRE(connections.empty());
    }
    
    TEST_CASE("Visualization Camera Control", "[visualization][camera]") {
        nlm::NLMLiveVisualization visualizer;
        visualizer.initialize(1920, 1080, true);
        
        visualizer.setCameraPosition(10.0f, 20.0f, 30.0f);
        visualizer.setZoomLevel(2.0f);
        visualizer.setRotation(45.0f, 30.0f, 15.0f);
        visualizer.setTimeScale(2.0);
        
        // Camera settings should be applied
        visualizer.setCameraPosition(0.0f, 0.0f, 0.0f);
        visualizer.setZoomLevel(1.0f);
        visualizer.setRotation(0.0f, 0.0f, 0.0f);
        visualizer.setTimeScale(1.0);
    }
    
    TEST_CASE("Visualization Mode Control", "[visualization][modes]") {
        nlm::NLMLiveVisualization visualizer;
        visualizer.initialize(1920, 1080, true);
        
        // Set different visualization modes
        visualizer.setNeuronRenderingMode(true, true, true);
        visualizer.setMemoryVisualizationMode(true, false);
        visualizer.setNeuromodulationVisualizationMode(true);
        visualizer.setDevelopmentVisualizationMode(true);
        visualizer.setIntegrationFlowVisualizationMode(true);
        
        // Should not crash
        visualizer.render();
    }
    
    TEST_CASE("Visualization Frame Capture", "[visualization][capture]") {
        nlm::NLMLiveVisualization visualizer;
        visualizer.initialize(1920, 1080, true);
        
        // Capture frames (visualizer is headless, so it logs but doesn't actually capture)
        visualizer.captureFrame();
        
        // Check that timestamp and frames are recorded
        const auto& timestamps = visualizer.getCaptureTimestamps();
        const auto& frames = visualizer.getCapturedFrames();
        
        REQUIRE(!timestamps.empty());
        REQUIRE(!frames.empty());
        REQUIRE(timestamps.size() == 1);
        REQUIRE(frames.size() == 1);
    }
    
    TEST_CASE("Visualization Integration with Brain", "[visualization][integration]") {
        nlm::NLMLiveVisualization visualizer;
        visualizer.initialize(1920, 1080, true);
        
        auto config = std::make_shared<nlm::Config>();
        config->setVisionDimensions(16, 16);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        REQUIRE(brain->initialize());
        
        auto agentBrain = std::make_unique<nlm::AgentBrain>(brain, config);
        nlm::SimpleWorld world(16, 16);
        agentBrain->initialize(world);
        
        // Update visualization with brain data
        visualizer.update(*brain, agentBrain.get());
        
        // Should have neural nodes after update
        const auto& nodes = visualizer.getNeuralNodes();
        const auto& connections = visualizer.getNeuralConnections();
        
        // Should have extracted data from brain
        REQUIRE(!nodes.empty()); // At least the brain regions
    }
    
    TEST_CASE("Visualization Performance Benchmark", "[visualization][benchmark]") {
        nlm::NLMLiveVisualization visualizer;
        visualizer.initialize(1920, 1080, true);
        
        auto config = std::make_shared<nlm::Config>();
        config->setVisionDimensions(32, 32);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        auto agentBrain = std::make_unique<nlm::AgentBrain>(brain, config);
        nlm::SimpleWorld world(32, 32);
        agentBrain->initialize(world);
        
        // Performance test - update visualization many times
        BENCHMARK("Visualization Update Performance", "[visualization][benchmark]") {
            nlm::SensoryPercept percept;
            percept.vision = std::vector<float>(32 * 32, 0.5f);
            
            for (int i = 0; i < 100; ++i) {
                visualizer.update(*brain, agentBrain.get());
            }
            
            return visualizer.getNeuralNodes().size();
        }
        
        return visualizer.getNeuralNodes().size();
    }
    
    TEST_CASE("Visualization Error Handling", "[visualization][error_handling]") {
        nlm::NLMLiveVisualization visualizer;
        
        // Initialize twice should not crash
        visualizer.initialize(1920, 1080, true);
        visualizer.initialize(1280, 720, true); // Second initialization
        
        // Close visualization
        visualizer.close();
        REQUIRE(!visualizer.isActive());
        
        // Should not crash when closing again
        visualizer.close();
    }
    
    TEST_CASE("Visualization Data Export", "[visualization][data_export]") {
        nlm::NLMLiveVisualization visualizer;
        visualizer.initialize(1920, 1080, true);
        
        // Add some mock data
        std::vector<nlm::NLMLiveVisualization::NeuralNode> mockNodes;
        nlm::NLMLiveVisualization::NeuralNode node;
        node.id = 1;
        node.x = 0.0f;
        node.y = 0.0f;
        node.z = 0.0f;
        node.radius = 2.0f;
        node.activation = 0.5f;
        node.isSpiking = false;
        node.color[0] = 1.0f;
        node.color[1] = 0.0f;
        node.color[2] = 0.0f;
        mockNodes.push_back(node);
        
        // The visualizer doesn't expose setters for these, so we just test export
        REQUIRE(visualizer.exportToJSON("/tmp/visualization_test.json") == true);
    }
    
    TEST_CASE("Visualization Memory Heatmap", "[visualization][heatmap]") {
        nlm::NLMLiveVisualization visualizer;
        visualizer.initialize(1920, 1080, true);
        
        // Get heatmap data (should be valid after update)
        const auto& heatmap = visualizer.getActivityHeatmap();
        REQUIRE(heatmap.gridWidth > 0);
        REQUIRE(heatmap.gridHeight > 0);
        REQUIRE(!heatmap.grid.empty());
    }
    
    TEST_CASE("Visualization System State", "[visualization][system_state]") {
        nlm::NLMLiveVisualization visualizer;
        visualizer.initialize(1920, 1080, true);
        
        // Get system state data
        const auto& memoryState = visualizer.getMemoryState();
        const auto& neuromodulationState = visualizer.getNeuromodulationState();
        const auto& developmentState = visualizer.getDevelopmentState();
        const auto& systemFlowState = visualizer.getSystemFlowState();
        
        // Should have valid structures (may have default values)
        REQUIRE(memoryState.workingMemoryActiveTraces >= 0);
        REQUIRE(memoryState.episodicMemoryCount >= 0);
        REQUIRE(neuromodulationState.dopamine >= 0.0f);
        REQUIRE(neuromodulationState.curiosity >= 0.0f);
        REQUIRE(neuromodulationState.novelty >= 0.0f);
        REQUIRE(developmentState.developmentalStage >= 0.0f);
        REQUIRE(!systemFlowState.brainStepStatus.empty());
    }
    
} // namespace test_visualization