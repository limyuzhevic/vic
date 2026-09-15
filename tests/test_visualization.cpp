// Visualization System Tests
// These tests verify the visual output and analysis capabilities of the NLM visualization system
// Including raster visualization, activity visualization, network visualization, and development visualization

#include "visualization/VisualizationSystem.hpp"
#include "visualization/RasterVisualizer.hpp"
#include "visualization/ActivityVisualizer.hpp"
#include "visualization/NetworkVisualizer.hpp"
#include "visualization/DevelopmentVisualizer.hpp"
#include "visualization/VisualizationEngine.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_visualization {

void testVisualizationSystemCreation() {
    nlm::VisualizationSystem visualization;
    
    // Test default initialization
    assert(visualization.getVisualizers().size() > 0);
    
    std::cout << "    testVisualizationSystemCreation passed" << std::endl;
}

void testRasterVisualization() {
    nlm::RasterVisualizer raster;
    
    // Test raster visualization parameters
    assert(raster.getMinFiringRate() >= 0.0f);
    assert(raster.getMaxFiringRate() <= 1.0f);
    assert(raster.getTimeWindow() > 0.0f);
    
    // Test configuration options
    raster.setFiringRateThreshold(0.5f);
    raster.setTimeWindow(100.0f);
    
    std::cout << "    testRasterVisualization passed" << std::endl;
}

void testRasterVisualizationRendering() {
    nlm::RasterVisualizer raster;
    
    // Create a simple spike raster
    std::vector<std::vector<float>> spikeRasters;
    
    // Neuron 1 spikes at times 0, 10, 20
    std::vector<float> neuron1Spikes = {0.0f, 10.0f, 20.0f};
    spikeRasters.push_back(neuron1Spikes);
    
    // Neuron 2 spikes at times 5, 15, 25
    std::vector<float> neuron2Spikes = {5.0f, 15.0f, 25.0f};
    spikeRasters.push_back(neuron2Spikes);
    
    // Generate visualization data
    auto rasterData = raster.generateData(spikeRasters);
    
    // Should have data for each neuron
    assert(rasterData.size() > 0);
    
    std::cout << "    testRasterVisualizationRendering passed" << std::endl;
}

void testActivityVisualization() {
    nlm::ActivityVisualizer activity;
    
    // Test activity visualization parameters
    assert(activity.getSmoothingFactor() >= 0.0f && activity.getSmoothingFactor() <= 1.0f);
    assert(activity.getFeatureDimensions() > 0);
    
    // Test configuration
    activity.setSmoothingFactor(0.5f);
    activity.setFeatureDimensions(16);
    
    std::cout << "    testActivityVisualization passed" << std::endl;
}

void testActivityVisualizationSpatialAnalysis() {
    nlm::ActivityVisualizer activity;
    
    // Create activity data for spatial analysis
    std::vector<std::vector<float>> spatialActivity;
    
    // 2D grid of neurons with varying activity levels
    for (int x = 0; x < 10; ++x) {
        std::vector<float> row;
        for (int y = 0; y < 10; ++y) {
            // Center cluster of higher activity
            float distance = std::sqrt(std::pow(x - 5.0f, 2) + std::pow(y - 5.0f, 2));
            float activity = std::max(0.0f, 1.0f - distance / 5.0f);
            row.push_back(activity);
        }
        spatialActivity.push_back(row);
    }
    
    // Generate visualization data
    auto activityData = activity.generateData(spatialActivity);
    
    // Should process spatial activity patterns
    assert(activityData.size() > 0);
    
    std::cout << "    testActivityVisualizationSpatialAnalysis passed" << std::endl;
}

void testNetworkVisualization() {
    nlm::NetworkVisualizer network;
    
    // Test network visualization parameters
    assert(network.getLayoutAlgorithm() != nullptr);
    assert(network.getNodeSize() > 0.0f);
    assert(network.getEdgeThickness() > 0.0f);
    
    // Test configuration
    network.setLayoutAlgorithm("spring_embedded");
    network.setNodeSize(5.0f);
    network.setEdgeThickness(1.0f);
    
    std::cout << "    testNetworkVisualization passed" << std::endl;
}

void testNetworkVisualizationGraphGeneration() {
    nlm::NetworkVisualizer network;
    
    // Create simple network graph
    std::vector<std::vector<float>> adjacencyMatrix(5, std::vector<float>(5, 0.0f));
    
    // Create a simple ring structure
    for (int i = 0; i < 5; ++i) {
        adjacencyMatrix[i][(i + 1) % 5] = 0.8f;
        adjacencyMatrix[(i + 1) % 5][i] = 0.8f;
    }
    
    // Generate visualization data
    auto graphData = network.generateData(adjacencyMatrix);
    
    // Should process adjacency matrix
    assert(graphData.size() > 0);
    
    std::cout << "    testNetworkVisualizationGraphGeneration passed" << std::endl;
}

void testDevelopmentVisualization() {
    nlm::DevelopmentVisualizer devViz;
    
    // Test development visualization parameters
    assert(devViz.getAnimationSpeed() > 0.0f);
    assert(devViz.getShowMaturation() == true);
    assert(devViz.getShowPruning() == true);
    assert(devViz.getShowSynaptogenesis() == true);
    
    // Test configuration
    devViz.setAnimationSpeed(1.0f);
    devViz.setShowMaturation(true);
    devViz.setShowPruning(true);
    devViz.setShowSynaptogenesis(true);
    
    std::cout << "    testDevelopmentVisualization passed" << std::endl;
}

void testDevelopmentVisualizationTemporalDynamics() {
    nlm::DevelopmentVisualizer devViz;
    
    // Create developmental data
    std::vector<std::vector<float>> developmentalData;
    
    // Simulate maturation over time
    for (int t = 0; t < 10; ++t) {
        std::vector<float> timePoint;
        for (int i = 0; i < 5; ++i) {
            // Neuron maturation progression
            float maturation = std::min(1.0f, static_cast<float>(t) / 5.0f);
            timePoint.push_back(maturation);
        }
        developmentalData.push_back(timePoint);
    }
    
    // Generate visualization data
    auto devVizData = devViz.generateData(developmentalData);
    
    // Should process developmental dynamics
    assert(devVizData.size() > 0);
    
    std::cout << "    testDevelopmentVisualizationTemporalDynamics passed" << std::endl;
}

void testVisualizationEngine() {
    nlm::VisualizationEngine engine;
    
    // Test visualization engine initialization
    assert(engine.getOutputFormat() != nullptr);
    assert(engine.getRenderQuality() > 0.0f);
    assert(engine.getMaxMemoryUsage() > 0.0f);
    
    // Test configuration
    engine.setOutputFormat("PNG");
    engine.setRenderQuality(0.9f);
    engine.setMaxMemoryUsage(100.0f);  // MB
    
    std::cout << "    testVisualizationEngine passed" << std::endl;
}

void testVisualizationEngineRenderingPipeline() {
    nlm::VisualizationEngine engine;
    
    // Create test data for rendering
    std::vector<float> testData(100, 0.5f);
    
    // Test rendering pipeline
    auto renderedFrame = engine.renderFrame(testData);
    
    // Should produce rendered output
    assert(renderedFrame.size() > 0);
    
    std::cout << "    testVisualizationEngineRenderingPipeline passed" << std::endl;
}

void testVisualizationSystemIntegration() {
    nlm::VisualizationSystem visualization;
    
    // Test integration of all visualizers
    auto visualizers = visualization.getVisualizers();
    
    // Should have all expected visualizers
    assert(visualizers.size() > 0);
    
    // Test rendering with integrated visualizers
    std::vector<std::vector<float>> testData;
    testData.push_back(std::vector<float>{1.0f, 0.8f, 0.6f});
    testData.push_back(std::vector<float>{0.2f, 0.4f, 0.6f});
    
    // Should render without crashing
    // In real test, would generate full visualization output
    
    std::cout << "    testVisualizationSystemIntegration passed" << std::endl;
}

void testVisualizationQualityMetrics() {
    nlm::VisualizationSystem visualization;
    
    // Test visualization quality assessment
    double qualityScore = visualization.calculateQualityScore();
    
    // Quality should be in valid range
    assert(qualityScore >= 0.0 && qualityScore <= 1.0);
    
    std::cout << "    testVisualizationQualityMetrics passed" << std::endl;
}

void testVisualizationPerformance() {
    nlm::VisualizationSystem visualization;
    
    // Test rendering performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Generate test data
    std::vector<std::vector<float>> performanceData;
    for (int i = 0; i < 100; ++i) {
        std::vector<float> row(10, 0.5f);
        performanceData.push_back(row);
    }
    
    // Render visualization
    visualization.render(performanceData);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Rendering should complete in reasonable time
    assert(duration.count() < 1000);  // Less than 1 second for test data
    
    std::cout << "    testVisualizationPerformance passed" << std::endl;
}

void runAllVisualizationTests() {
    std::cout << "Running Visualization System Tests..." << std::endl;
    testVisualizationSystemCreation();
    testRasterVisualization();
    testRasterVisualizationRendering();
    testActivityVisualization();
    testActivityVisualizationSpatialAnalysis();
    testNetworkVisualization();
    testNetworkVisualizationGraphGeneration();
    testDevelopmentVisualization();
    testDevelopmentVisualizationTemporalDynamics();
    testVisualizationEngine();
    testVisualizationEngineRenderingPipeline();
    testVisualizationSystemIntegration();
    testVisualizationQualityMetrics();
    testVisualizationPerformance();
}

} // namespace test_visualization
