#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <thread>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

// NLM headers
#include "src/agent/VisualizationAPI.hpp"
#include "src/agent/AdvancedPerformance.hpp"
#include "src/brain/Brain.hpp"

using namespace nlm;

// Visualization example
class VisualizationExample {
public:
    VisualizationExample() {
        std::cout << "=== NLM Visualization Example ===" << std::endl;
    }
    
    void runExample() {
        std::cout << "Running visualization example..." << std::endl;
        
        // Initialize visualization system
        initializeVisualizationSystem();
        
        // Step 1: Set up visualization modes
        setupVisualizationModes();
        
        // Step 2: Create neural network visualization
        createNeuralNetworkVisualization();
        
        // Step 3: Create performance dashboard
        createPerformanceDashboard();
        
        // Step 4: Create debug visualizations
        createDebugVisualizations();
        
        // Step 5: Set up interactive features
        setupInteractiveFeatures();
        
        // Step 6: Export visualizations
        exportVisualizations();
        
        // Step 7: Process visualization data
        processVisualizationData();
        
        // Step 8: Update visualization components
        updateVisualizationComponents();
        
        // Step 9: Show visualizations
        showVisualizations();
        
        // Clean up
        shutdownVisualizationSystem();
        
        std::cout << "Visualization example completed successfully!" << std::endl;
    }
    
private:
    void initializeVisualizationSystem() {
        std::cout << "\n1. Initializing visualization system..." << std::endl;
        
        VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
        
        // Initialize visualization system
        if (vizAPI.initialize()) {
            std::cout << "  Visualization system initialized successfully!" << std::endl;
            
            // Set visualization configuration
            nlm::Config vizConfig;
            vizConfig.setValue("visualization.mode", "realtime");
            vizConfig.setValue("visualization.export_formats", std::vector<std::string>{"png", "svg", "json"});
            vizConfig.setValue("visualization.interactive_enabled", true);
            vizConfig.setValue("visualization.performance_monitoring", true);
            vizConfig.setValue("visualization.memory_tracking", true);
            
            vizAPI.setVisualizationConfig(vizConfig);
            std::cout << "  Visualization configuration set successfully!" << std::endl;
            
            // Get visualization statistics
            VisualizationAPI::VisualizationStats stats = vizAPI.getStats();
            std::cout << "  Initial statistics: " << stats.totalComponents << " components, "
                      << stats.activeComponents << " active" << std::endl;
        } else {
            std::cout << "  Failed to initialize visualization system!" << std::endl;
        }
    }
    
    void setupVisualizationModes() {
        std::cout << "\n2. Setting up visualization modes..." << std::endl;
        
        VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
        
        // Set visualization mode to real-time
        vizAPI.setVisualizationMode(VisualizationMode::RealTime);
        std::cout << "  Visualization mode set to RealTime!" << std::endl;
        
        // Enable interactive mode
        if (vizAPI.enableInteractiveMode()) {
            std::cout << "  Interactive mode enabled successfully!" << std::endl;
        } else {
            std::cout << "  Failed to enable interactive mode!" << std::endl;
        }
        
        // Check visualization status
        if (vizAPI.isActive()) {
            std::cout << "  Visualization system is active!" << std::endl;
        } else {
            std::cout << "  Visualization system is not active!" << std::endl;
        }
        
        if (vizAPI.isInteractiveModeEnabled()) {
            std::cout << "  Interactive mode is enabled!" << std::endl;
        } else {
            std::cout << "  Interactive mode is not enabled!" << std::endl;
        }
    }
    
    void createNeuralNetworkVisualization() {
        std::cout << "\n3. Creating neural network visualization..." << std::endl;
        
        VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
        
        // Get brain instance (mock for demonstration)
        std::cout << "  Creating mock brain instance..." << std::endl;
        
        // Create neural network visualization data
        NeuralNetworkVisualization networkViz;
        
        // Populate neuron positions
        std::vector<std::vector<float>> neuronPositions;
        createNeuronPositions(neuronPositions);
        networkViz.neuronPositions = neuronPositions;
        
        // Populate neuron sizes
        std::vector<std::vector<float>> neuronSizes;
        createNeuronSizes(neuronSizes);
        networkViz.neuronSizes = neuronSizes;
        
        // Populate neuron activations
        std::vector<std::vector<float>> neuronActivations;
        createNeuronActivations(neuronActivations);
        networkViz.neuronActivations = neuronActivations;
        
        // Populate neuron colors
        std::vector<std::array<float, 3>> neuronColors;
        createNeuronColors(neuronColors);
        networkViz.neuronColors = neuronColors;
        
        // Populate synapse connections
        std::vector<std::pair<size_t, size_t>> synapses;
        createSynapses(synapses);
        networkViz.synapses = synapses;
        
        // Populate synapse weights
        std::vector<float> synapseWeights;
        createSynapseWeights(synapseWeights);
        networkViz.synapseWeights = synapseWeights;
        
        // Populate synapse colors
        std::vector<std::array<float, 3>> synapseColors;
        createSynapseColors(synapseColors);
        networkViz.synapseColors = synapseColors;
        
        // Populate population centers
        std::vector<std::vector<float>> populationCenters;
        createPopulationCenters(populationCenters);
        networkViz.populationCenters = populationCenters;
        
        // Populate population radii
        std::vector<float> populationRadii;
        createPopulationRadii(populationRadii);
        networkViz.populationRadii = populationRadii;
        
        // Register visualization components
        registerVisualizationComponents(vizAPI);
        
        // Visualize network structure
        if (vizAPI.visualizeNetworkStructure(getMockBrain())) {
            std::cout << "  Neural network structure visualized successfully!" << std::endl;
        } else {
            std::cout << "  Failed to visualize neural network structure!" << std::endl;
        }
        
        // Visualize neuron activity
        if (vizAPI.visualizeNeuronActivity(getMockBrain(), 0.5)) {
            std::cout << "  Neuron activity visualized successfully!" << std::endl;
        } else {
            std::cout << "  Failed to visualize neuron activity!" << std::endl;
        }
        
        // Visualize synapse weights
        if (vizAPI.visualizeSynapseWeights(getMockBrain(), 0.0, 1.0)) {
            std::cout << "  Synapse weights visualized successfully!" << std::endl;
        } else {
            std::cout << "  Failed to visualize synapse weights!" << std::endl;
        }
        
        // Create spike rasters
        std::vector<std::vector<float>> spikes;
        createSpikeRasters(spikes);
        if (vizAPI.visualizeSpikeRasters(spikes, 1.0)) {
            std::cout << "  Spike rasters visualized successfully!" << std::endl;
        } else {
            std::cout << "  Failed to visualize spike rasters!" << std::endl;
        }
        
        // Visualize network with custom visualization data
        if (vizAPI.visualizeNetwork(getMockBrain(), networkViz)) {
            std::cout << "  Neural network visualization created successfully!" << std::endl;
        } else {
            std::cout << "  Failed to create neural network visualization!" << std::endl;
        }
    }
    
    void createPerformanceDashboard() {
        std::cout << "\n4. Creating performance dashboard..." << std::endl;
        
        VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
        
        // Create performance dashboard
        std::vector<std::string> metricNames = {
            "FiringRate",
            "MemoryUsage",
            "CPUUsage",
            "ResponseTime",
            "Accuracy"
        };
        
        std::vector<std::vector<double>> metricValues = {
            {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9},
            {100.0, 200.0, 150.0, 180.0, 160.0, 170.0, 190.0, 210.0, 205.0},
            {10.0, 15.0, 12.0, 18.0, 14.0, 16.0, 19.0, 22.0, 20.0},
            {50.0, 60.0, 55.0, 70.0, 65.0, 75.0, 80.0, 85.0, 82.0},
            {0.8, 0.85, 0.9, 0.88, 0.92, 0.87, 0.91, 0.89, 0.93}
        };
        
        std::vector<std::vector<double>> metricTimestamps = {
            {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0},
            {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0},
            {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0},
            {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0},
            {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0}
        };
        
        if (vizAPI.createPerformanceDashboard(metricNames, metricValues)) {
            std::cout << "  Performance dashboard created successfully!" << std::endl;
            
            // Update performance dashboard
            PerformanceDashboard dashboard;
            dashboard.metricNames = metricNames;
            dashboard.metricValues = metricValues;
            dashboard.metricTimestamps = metricTimestamps;
            
            if (vizAPI.updatePerformanceDashboard(dashboard)) {
                std::cout << "  Performance dashboard updated successfully!" << std::endl;
            } else {
                std::cout << "  Failed to update performance dashboard!" << std::endl;
            }
            
            // Visualize performance metrics
            if (vizAPI.visualizePerformanceMetrics("performance_session_1")) {
                std::cout << "  Performance metrics visualized successfully!" << std::endl;
            } else {
                std::cout << "  Failed to visualize performance metrics!" << std::endl;
            }
        } else {
            std::cout << "  Failed to create performance dashboard!" << std::endl;
        }
    }
    
    void createDebugVisualizations() {
        std::cout << "\n5. Creating debug visualizations..." << std::endl;
        
        VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
        
        // Create heatmap visualization
        createHeatmapVisualization(vizAPI);
        
        // Create scatter plot visualization
        createScatterPlotVisualization(vizAPI);
        
        // Create contour plot visualization
        createContourPlotVisualization(vizAPI);
        
        // Create graph visualization
        createGraphVisualization(vizAPI);
        
        // Update debug visualization
        updateDebugVisualization(vizAPI);
        
        // Show debug visualization
        showDebugVisualization(vizAPI);
    }
    
    void createHeatmapVisualization(VisualizationAPI& vizAPI) {
        std::cout << "  Creating heatmap visualization..." << std::endl;
        
        // Create debug visualization for heatmap
        DebugVisualization heatmapViz;
        heatmapViz.visualizationType = "heatmap";
        heatmapViz.mode = "heatmap";
        heatmapViz.title = "Neuron Activity Heatmap";
        heatmapViz.description = "Visualization of neuron activity patterns over time";
        
        // Create heatmap data
        std::vector<std::vector<float>> heatmapData;
        for (int i = 0; i < 10; ++i) {
            std::vector<float> row;
            for (int j = 0; j < 10; ++j) {
                row.push_back(static_cast<float>(i + j) / 20.0f);
            }
            heatmapData.push_back(row);
        }
        heatmapViz.debugData = heatmapData;
        
        if (vizAPI.createDebugVisualization(heatmapViz)) {
            std::cout << "    Heatmap visualization created successfully!" << std::endl;
        } else {
            std::cout << "    Failed to create heatmap visualization!" << std::endl;
        }
    }
    
    void createScatterPlotVisualization(VisualizationAPI& vizAPI) {
        std::cout << "  Creating scatter plot visualization..." << std::endl;
        
        // Create debug visualization for scatter plot
        DebugVisualization scatterViz;
        scatterViz.visualizationType = "scatter";
        scatterViz.mode = "scatter";
        scatterViz.title = "Neuron Firing Rates vs Memory Usage";
        scatterViz.description = "Scatter plot showing relationship between firing rates and memory usage";
        
        // Create scatter plot data
        std::vector<std::vector<float>> scatterData;
        for (int i = 0; i < 20; ++i) {
            std::vector<float> point;
            point.push_back(static_cast<float>(i) / 20.0f);  // Firing rate
            point.push_back(static_cast<float>(i * 10) / 20.0f);  // Memory usage
            scatterData.push_back(point);
        }
        scatterViz.debugData = scatterData;
        
        if (vizAPI.createDebugVisualization(scatterViz)) {
            std::cout << "    Scatter plot visualization created successfully!" << std::endl;
        } else {
            std::cout << "    Failed to create scatter plot visualization!" << std::endl;
        }
    }
    
    void createContourPlotVisualization(VisualizationAPI& vizAPI) {
        std::cout << "  Creating contour plot visualization..." << std::endl;
        
        // Create debug visualization for contour plot
        DebugVisualization contourViz;
        contourViz.visualizationType = "contour";
        contourViz.mode = "contour";
        contourViz.title = "Performance Contour Map";
        contourViz.description = "Contour plot showing performance across different configurations";
        
        // Create contour plot data
        std::vector<std::vector<float>> contourData;
        for (int i = 0; i < 10; ++i) {
            std::vector<float> row;
            for (int j = 0; j < 10; ++j) {
                float x = static_cast<float>(i) / 9.0f;
                float y = static_cast<float>(j) / 9.0f;
                row.push_back(sin(x * 3.14) * cos(y * 3.14) + 1.0f);
            }
            contourData.push_back(row);
        }
        contourViz.debugData = contourData;
        
        if (vizAPI.createDebugVisualization(contourViz)) {
            std::cout << "    Contour plot visualization created successfully!" << std::endl;
        } else {
            std::cout << "    Failed to create contour plot visualization!" << std::endl;
        }
    }
    
    void createGraphVisualization(VisualizationAPI& vizAPI) {
        std::cout << "  Creating graph visualization..." << std::endl;
        
        // Create debug visualization for graph
        DebugVisualization graphViz;
        graphViz.visualizationType = "graph";
        graphViz.mode = "graph";
        graphViz.title = "Neural Network Topology";
        graphViz.description = "Graph visualization showing neural network topology";
        
        // Create graph visualization data
        std::vector<std::vector<float>> graphData;
        for (int i = 0; i < 5; ++i) {
            std::vector<float> node;
            node.push_back(static_cast<float>(i));  // Node index
            node.push_back(0.0f);  // X position
            node.push_back(0.0f);  // Y position
            graphData.push_back(node);
        }
        graphViz.debugData = graphData;
        
        if (vizAPI.createDebugVisualization(graphViz)) {
            std::cout << "    Graph visualization created successfully!" << std::endl;
        } else {
            std::cout << "    Failed to create graph visualization!" << std::endl;
        }
    }
    
    void updateDebugVisualization(VisualizationAPI& vizAPI) {
        std::cout << "  Updating debug visualizations..." << std::endl;
        
        // Create updated debug visualization data
        std::vector<std::vector<float>> updatedData;
        for (int i = 0; i < 10; ++i) {
            std::vector<float> row;
            for (int j = 0; j < 10; ++j) {
                row.push_back(static_cast<float>(i * j) / 100.0f);
            }
            updatedData.push_back(row);
        }
        
        // Note: In real implementation, this would update an existing debug visualization
        // For demonstration, we'll just create a new one
        DebugVisualization updatedViz;
        updatedViz.visualizationType = "heatmap";
        updatedViz.mode = "heatmap";
        updatedViz.title = "Updated Neuron Activity Heatmap";
        updatedViz.debugData = updatedData;
        
        if (vizAPI.createDebugVisualization(updatedViz)) {
            std::cout << "    Updated debug visualization created successfully!" << std::endl;
        } else {
            std::cout << "    Failed to update debug visualization!" << std::endl;
        }
    }
    
    void showDebugVisualization(VisualizationAPI& vizAPI) {
        std::cout << "  Showing debug visualizations..." << std::endl;
        
        // Note: In real implementation, this would show an existing debug visualization
        // For demonstration, we'll just log the action
        std::cout << "    Debug visualization would be displayed here..." << std::endl;
    }
    
    void setupInteractiveFeatures(VisualizationAPI& vizAPI) {
        std::cout << "\n6. Setting up interactive features..." << std::endl;
        
        // Add update listener
        std::string listenerId = "visualization_listener";
        vizAPI.addUpdateListener([listenerId](const VisualizationUpdate& update) {
            // Handle visualization update
            std::cout << "    Visualization update received: " << update.eventType << std::endl;
            std::cout << "    Component: " << update.componentName << std::endl;
        });
        
        std::cout << "  Interactive feature setup completed!" << std::endl;
        
        // Set up component processing
        std::vector<float> inputData = {0.1, 0.2, 0.3, 0.4, 0.5};
        std::vector<float> outputData;
        
        if (vizAPI.processComponent("neural_network_analyzer", inputData, outputData)) {
            std::cout << "  Component processing completed successfully!" << std::endl;
            std::cout << "  Input data: " << inputData.size() << " elements" << std::endl;
            std::cout << "  Output data: " << outputData.size() << " elements" << std::endl;
        } else {
            std::cout << "  Component processing failed!" << std::endl;
        }
    }
    
    void exportVisualizations() {
        std::cout << "\n7. Exporting visualizations..." << std::endl;
        
        VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
        
        // Create export configuration for PNG
        ExportConfig pngConfig;
        pngConfig.format = ExportFormat::PNG;
        pngConfig.filepath = "./output/visualization.png";
        pngConfig.filename = "neural_network_viz";
        pngConfig.title = "Neural Network Visualization";
        pngConfig.description = "Visualization of neural network structure and activity";
        pngConfig.includeMetadata = true;
        pngConfig.compress = false;
        
        // Create export configuration for SVG
        ExportConfig svgConfig;
        svgConfig.format = ExportFormat::SVG;
        svgConfig.filepath = "./output/visualization.svg";
        svgConfig.filename = "neural_network_viz_svg";
        svgConfig.title = "Neural Network Visualization (SVG)";
        svgConfig.description = "Scalable vector graphics of neural network structure";
        svgConfig.includeMetadata = true;
        svgConfig.compress = false;
        
        // Create export configuration for JSON
        ExportConfig jsonConfig;
        jsonConfig.format = ExportFormat::JSON;
        jsonConfig.filepath = "./output/visualization.json";
        jsonConfig.filename = "neural_network_viz_json";
        jsonConfig.title = "Neural Network Visualization (JSON)";
        jsonConfig.description = "JSON data for neural network visualization";
        jsonConfig.includeMetadata = true;
        jsonConfig.compress = false;
        
        // Create neural network visualization data
        NeuralNetworkVisualization networkData;
        createVisualizationData(networkData);
        
        // Create performance dashboard data
        PerformanceDashboard perfData;
        createPerformanceDashboardData(perfData);
        
        // Create debug visualization data
        std::vector<DebugVisualization> debugVizs;
        createDebugVisualizationData(debugVizs);
        
        // Export to PNG
        if (vizAPI.exportToImage(pngConfig.filepath, pngConfig)) {
            std::cout << "  Visualization exported to PNG successfully!" << std::endl;
        } else {
            std::cout << "  Failed to export visualization to PNG!" << std::endl;
        }
        
        // Export to SVG
        if (vizAPI.exportToImage(svgConfig.filepath, svgConfig)) {
            std::cout << "  Visualization exported to SVG successfully!" << std::endl;
        } else {
            std::cout << "  Failed to export visualization to SVG!" << std::endl;
        }
        
        // Export to JSON (using exportVisualization for full export)
        if (vizAPI.exportVisualization(jsonConfig, networkData, perfData, debugVizs)) {
            std::cout << "  Visualization exported to JSON successfully!" << std::endl;
        } else {
            std::cout << "  Failed to export visualization to JSON!" << std::endl;
        }
        
        std::cout << "  Visualization export completed!" << std::endl;
    }
    
    void processVisualizationData() {
        std::cout << "\n8. Processing visualization data..." << std::endl;
        
        VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
        
        // Process all components
        std::vector<float> inputData = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
        std::vector<float> outputData = vizAPI.processAllComponents(inputData);
        
        std::cout << "  Data processed successfully!" << std::endl;
        std::cout << "    Input data size: " << inputData.size() << " elements" << std::endl;
        std::cout << "    Output data size: " << outputData.size() << " elements" << std::endl;
        
        // Get visualization statistics
        VisualizationAPI::VisualizationStats stats = vizAPI.getStats();
        std::cout << "    Statistics: " << stats.totalComponents << " components, "
                  << stats.activeComponents << " active, "
                  << stats.totalUpdates << " updates, "
                  << "Processing time: " << stats.totalProcessingTime << " ms" << std::endl;
    }
    
    void updateVisualizationComponents() {
        std::cout << "\n9. Updating visualization components..." << std::endl;
        
        VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
        
        // Update neural network visualization
        std::cout << "  Updating neural network visualization..." << std::endl;
        
        // Note: In real implementation, this would update neural network visualization
        // For demonstration, we'll just log the action
        std::cout << "    Neural network visualization updated..." << std::endl;
        
        // Update performance dashboard
        std::cout << "  Updating performance dashboard..." << std::endl;
        
        // Note: In real implementation, this would update performance dashboard
        // For demonstration, we'll just log the action
        std::cout << "    Performance dashboard updated..." << std::endl;
        
        std::cout << "  Visualization components updated successfully!" << std::endl;
    }
    
    void showVisualizations() {
        std::cout << "\n10. Showing visualizations..." << std::endl;
        
        VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
        
        // Note: In real implementation, this would display visualizations
        // For demonstration, we'll just log the action
        std::cout << "  Visualization display would be shown here..." << std::endl;
        std::cout << "    - Neural network structure visualization" << std::endl;
        std::cout << "    - Neuron activity visualization" << std::endl;
        std::cout << "    - Performance dashboard visualization" << std::endl;
        std::cout << "    - Debug visualization dashboard" << std::endl;
        
        std::cout << "  Visualizations displayed successfully!" << std::endl;
    }
    
    void shutdownVisualizationSystem() {
        std::cout << "\n11. Shutting down visualization system..." << std::endl;
        
        VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
        
        if (vizAPI.isActive()) {
            // Note: In real implementation, this would shutdown visualization system
            // For demonstration, we'll just log the action
            std::cout << "  Visualization system shutdown..." << std::endl;
        }
        
        std::cout << "  Visualization system shutdown completed!" << std::endl;
    }
    
    // Mock brain for demonstration
    class MockBrain : public Brain {
    public:
        MockBrain() {
            // Mock brain properties
        }
        
        int getTotalNeuronCount() const override {
            return 1000;
        }
        
        double getAverageFiringRate() const override {
            return 0.5;
        }
        
        int getTotalSynapseCount() const override {
            return 10000;
        }
    };
    
    MockBrain getMockBrain() {
        return MockBrain();
    }
    
    void createNeuronPositions(std::vector<std::vector<float>>& positions) {
        // Create random neuron positions
        for (int i = 0; i < 50; ++i) {
            std::vector<float> position(3);
            for (int j = 0; j < 3; ++j) {
                position[j] = static_cast<float>(rand()) / RAND_MAX * 10.0f;
            }
            positions.push_back(position);
        }
    }
    
    void createNeuronSizes(std::vector<std::vector<float>>& sizes) {
        // Create random neuron sizes
        for (int i = 0; i < 50; ++i) {
            std::vector<float> size(3);
            for (int j = 0; j < 3; ++j) {
                size[j] = static_cast<float>(rand()) / RAND_MAX * 2.0f + 0.5f;
            }
            sizes.push_back(size);
        }
    }
    
    void createNeuronActivations(std::vector<std::vector<float>>& activations) {
        // Create random neuron activations
        for (int i = 0; i < 50; ++i) {
            std::vector<float> activation(3);
            for (int j = 0; j < 3; ++j) {
                activation[j] = static_cast<float>(rand()) / RAND_MAX;
            }
            activations.push_back(activation);
        }
    }
    
    void createNeuronColors(std::vector<std::array<float, 3>>& colors) {
        // Create random neuron colors
        for (int i = 0; i < 50; ++i) {
            std::array<float, 3> color;
            for (int j = 0; j < 3; ++j) {
                color[j] = static_cast<float>(rand()) / RAND_MAX;
            }
            colors.push_back(color);
        }
    }
    
    void createSynapses(std::vector<std::pair<size_t, size_t>>& synapses) {
        // Create random synapse connections
        for (int i = 0; i < 100; ++i) {
            size_t source = static_cast<size_t>(rand()) % 50;
            size_t target = static_cast<size_t>(rand()) % 50;
            synapses.push_back(std::make_pair(source, target));
        }
    }
    
    void createSynapseWeights(std::vector<float>& weights) {
        // Create random synapse weights
        for (int i = 0; i < 100; ++i) {
            weights.push_back(static_cast<float>(rand()) / RAND_MAX);
        }
    }
    
    void createSynapseColors(std::vector<std::array<float, 3>>& colors) {
        // Create random synapse colors
        for (int i = 0; i < 100; ++i) {
            std::array<float, 3> color;
            for (int j = 0; j < 3; ++j) {
                color[j] = static_cast<float>(rand()) / RAND_MAX;
            }
            colors.push_back(color);
        }
    }
    
    void createPopulationCenters(std::vector<std::vector<float>>& centers) {
        // Create random population centers
        for (int i = 0; i < 10; ++i) {
            std::vector<float> center(3);
            for (int j = 0; j < 3; ++j) {
                center[j] = static_cast<float>(rand()) / RAND_MAX * 10.0f;
            }
            centers.push_back(center);
        }
    }
    
    void createPopulationRadii(std::vector<float>& radii) {
        // Create random population radii
        for (int i = 0; i < 10; ++i) {
            radii.push_back(static_cast<float>(rand()) / RAND_MAX * 2.0f + 0.5f);
        }
    }
    
    void createSpikeRasters(std::vector<std::vector<float>>& spikes) {
        // Create spike raster data
        for (int i = 0; i < 20; ++i) {
            std::vector<float> spikeTrain;
            for (int j = 0; j < 50; ++j) {
                if (static_cast<float>(rand()) / RAND_MAX < 0.3f) {
                    spikeTrain.push_back(static_cast<float>(j));
                }
            }
            spikes.push_back(spikeTrain);
        }
    }
    
    void createVisualizationData(NeuralNetworkVisualization& vizData) {
        // Create visualization data for export
        vizData.neuronPositions.clear();
        vizData.neuronSizes.clear();
        vizData.neuronActivations.clear();
        vizData.neuronColors.clear();
        vizData.synapses.clear();
        vizData.synapseWeights.clear();
        vizData.synapseColors.clear();
        vizData.populationCenters.clear();
        vizData.populationRadii.clear();
        
        // Add sample data
        for (int i = 0; i < 10; ++i) {
            vizData.neuronPositions.push_back({static_cast<float>(i), 0.0f, 0.0f});
            vizData.neuronSizes.push_back({1.0f, 1.0f, 1.0f});
            vizData.neuronActivations.push_back({0.5f, 0.5f, 0.5f});
            vizData.neuronColors.push_back({1.0f, 0.0f, 0.0f});
        }
        
        for (int i = 0; i < 5; ++i) {
            vizData.synapses.push_back({static_cast<size_t>(i), static_cast<size_t>(i + 1)});
            vizData.synapseWeights.push_back(0.5f);
            vizData.synapseColors.push_back({0.0f, 1.0f, 0.0f});
        }
        
        for (int i = 0; i < 5; ++i) {
            vizData.populationCenters.push_back({static_cast<float>(i * 2), 0.0f, 0.0f});
            vizData.populationRadii.push_back(1.0f);
        }
    }
    
    void createPerformanceDashboardData(PerformanceDashboard& dashboard) {
        // Create performance dashboard data
        dashboard.metricNames = {"FiringRate", "MemoryUsage", "CPUUsage"};
        dashboard.metricValues = {
            {0.1, 0.2, 0.3, 0.4, 0.5},
            {100.0, 200.0, 150.0, 180.0, 160.0},
            {10.0, 15.0, 12.0, 18.0, 14.0}
        };
        dashboard.metricTimestamps = {
            {0.0, 1.0, 2.0, 3.0, 4.0},
            {0.0, 1.0, 2.0, 3.0, 4.0},
            {0.0, 1.0, 2.0, 3.0, 4.0}
        };
        dashboard.metricColors = {
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}
        };
        dashboard.metricUnits = {"Hz", "MB", "Percent"};
        dashboard.metricThresholds = {100.0, 1024.0, 100.0};
        dashboard.metricDescriptions = {
            {"FiringRate", "Neuronal firing rate"},
            {"MemoryUsage", "Memory usage in MB"},
            {"CPUUsage", "CPU usage percentage"}
        };
    }
    
    void createDebugVisualizationData(std::vector<DebugVisualization>& debugVizs) {
        // Create debug visualization data
        DebugVisualization heatmapViz;
        heatmapViz.visualizationType = "heatmap";
        heatmapViz.mode = "heatmap";
        heatmapViz.title = "Performance Heatmap";
        heatmapViz.debugData = {{0.1f, 0.2f}, {0.3f, 0.4f}};
        debugVizs.push_back(heatmapViz);
    }
    
    void registerVisualizationComponents(VisualizationAPI& vizAPI) {
        // Register visualization components
        VisualizationComponent neuralComponent;
        neuralComponent.name = "neural_analyzer";
        neuralComponent.type = "neural_network";
        neuralComponent.description = "Analyzes neural network data";
        neuralComponent.implementation = "Standard neural network analysis";
        neuralComponent.isActive = true;
        
        if (vizAPI.registerComponent(neuralComponent)) {
            std::cout << "    Visualization component registered successfully!" << std::endl;
        } else {
            std::cout << "    Failed to register visualization component!" << std::endl;
        }
    }
};

int main() {
    std::cout << "=== NLM Visualization Example ===" << std::endl;
    
    // Create example instance
    VisualizationExample example;
    
    // Run example
    example.runExample();
    
    std::cout << "\n=== Example Complete ===" << std::endl;
    return 0;
}
