// VisualizationTools_example.cpp
/**
 * Example demonstrating the VisualizationTools class for NLM
 * 
 * This example shows how to use VisualizationTools to:
 * 1. Create interactive web-based dashboards
 * 2. Generate 3D visualizations of neural networks
 * 3. Create animated visualizations
 * 4. Generate comprehensive reports
 * 5. Create VR-compatible visualizations
 */

#include "visualization/VisualizationTools.hpp"
#include "visualization/VisualizationTools_config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/NeuronId.hpp"
#include "core/Types/Types.hpp"
#include <memory>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

void exampleBasicVisualization() {
    std::cout << "=== Basic Visualization Example ===\n";
    
    // Create visualization tools
    auto vizTools = std::make_unique<nlm::VisualizationTools>();
    
    // Configure with custom settings
    nlm::VisualizationToolsConfig config;
    config.outputDirectory = "./examples_output";
    config.enableWebGL = true;
    config.enable3D = true;
    config.enableVR = true;
    
    // Initialize visualization
    if (!vizTools->initialize("./examples_output")) {
        std::cerr << "Failed to initialize visualization tools!" << std::endl;
        return;
    }
    
    // Create a brain simulation (placeholder)
    // In a real implementation, you would have an actual brain object
    
    // Generate different types of visualizations
    vizTools->generateVisualization(nlm::VisualizationType::NETWORK_GRAPH, "network_graph");
    vizTools->generateVisualization(nlm::VisualizationType::NEURON_ACTIVITY, "neuron_activity");
    vizTools->generateVisualization(nlm::VisualizationType::SPIKE_RASTER, "spike_raster");
    vizTools->generateVisualization(nlm::VisualizationType::WEIGHT_MATRIX, "weight_matrix");
    vizTools->generateVisualization(nlm::VisualizationType::POPULATION_STATISTICS, "population_stats");
    
    std::cout << "Generated basic visualizations successfully!\n";
}

void example3DVisualization() {
    std::cout << "\n=== 3D Visualization Example ===\n";
    
    // Create 3D scene configuration
    nlm::SceneConfig sceneConfig;
    sceneConfig.name = "3D Neural Network Visualization";
    sceneConfig.cameraDistance = 50.0f;
    sceneConfig.fieldOfView = 75.0f;
    sceneConfig.backgroundColor = "#000000";
    sceneConfig.lightingMode = "phong";
    sceneConfig.enableShadows = true;
    sceneConfig.enableAntialiasing = true;
    
    // Create visualization tools
    auto vizTools = nlm::VisualizationTools::create3DVisualizer(sceneConfig);
    
    if (!vizTools->initialize("./examples_output")) {
        std::cerr << "Failed to initialize 3D visualization!" << std::endl;
        return;
    }
    
    // Create a brain simulation (placeholder)
    // In a real implementation, you would have an actual brain object
    
    // Generate 3D visualization
    vizTools->generate3DVisualization(*reinterpret_cast<nlm::Brain*>(nullptr), 
                                      "neural_network_3d.html", sceneConfig);
    
    std::cout << "Generated 3D visualization successfully!\n";
}

void exampleVRVisualization() {
    std::cout << "\n=== VR Visualization Example ===\n";
    
    // Create VR configuration
    nlm::VRConfig vrConfig;
    vrConfig.trackingSystem = "head-mounted-display";
    vrConfig.displayMode = "stereoscopic";
    vrConfig.enablePhysics = true;
    vrConfig.enableInteractions = true;
    vrConfig.interactionMode = "hand-tracking";
    vrConfig.supportedDevices = {"HMD", "Controllers"};
    
    // Create visualization tools
    auto vizTools = nlm::VisualizationTools::createVRVisualizer(vrConfig);
    
    if (!vizTools->initialize("./examples_output")) {
        std::cerr << "Failed to initialize VR visualization!" << std::endl;
        return;
    }
    
    // Create a brain simulation (placeholder)
    
    // Generate VR visualization
    vizTools->generateVRScene(*reinterpret_cast<nlm::Brain*>(nullptr), 
                             "neural_network_vr.vrscene", vrConfig);
    
    std::cout << "Generated VR visualization successfully!\n";
}

void exampleInteractiveDashboard() {
    std::cout << "\n=== Interactive Dashboard Example ===\n";
    
    // Create dashboard configuration
    nlm::DashboardConfig dashboardConfig;
    dashboardConfig.title = "NLM Neural Network Dashboard";
    dashboardConfig.widgets = {
        "Network Graph",
        "Activity Timeline",
        "Spike Raster Plot",
        "Population Statistics",
        "Weight Matrix Viewer",
        "3D Network View"
    };
    dashboardConfig.enableRealTimeUpdates = true;
    dashboardConfig.updateInterval = 1000.0; // ms
    dashboardConfig.layout = "grid";
    
    // Create interactive dashboard
    auto vizTools = nlm::VisualizationTools::createInteractiveDashboard(dashboardConfig.title);
    
    if (!vizTools->initialize("./examples_output")) {
        std::cerr << "Failed to initialize dashboard!" << std::endl;
        return;
    }
    
    // Generate interactive dashboard
    vizTools->generateInteractiveDashboard("neural_dashboard.html", dashboardConfig);
    
    std::cout << "Generated interactive dashboard successfully!\n";
}

void exampleAnimation() {
    std::cout << "\n=== Animation Example ===\n";
    
    // Create visualization tools
    auto vizTools = std::make_unique<nlm::VisualizationTools>();
    
    if (!vizTools->initialize("./examples_output")) {
        std::cerr << "Failed to initialize animation tools!" << std::endl;
        return;
    }
    
    // Create brain simulation (placeholder)
    
    // Create animation parameters
    nlm::AnimationParams animationParams;
    animationParams.duration = 10.0;           // 10 seconds
    animationParams.frameRate = 30.0;         // 30 fps
    animationParams.loop = true;
    animationParams.easing = "ease-in-out";
    animationParams.interpolation = "cubic";
    
    // Generate animation
    vizTools->generateTimeSeriesAnimation(*reinterpret_cast<nlm::Brain*>(nullptr), 
                                         "neural_animation.html", animationParams);
    
    std::cout << "Generated animation successfully!\n";
}

void exampleReportGeneration() {
    std::cout << "\n=== Report Generation Example ===\n";
    
    // Create visualization tools
    auto vizTools = std::make_unique<nlm::VisualizationTools>();
    
    if (!vizTools->initialize("./examples_output")) {
        std::cerr << "Failed to initialize report generation!" << std::endl;
        return;
    }
    
    // Create brain simulation (placeholder)
    
    // Generate comprehensive report
    if (vizTools->generateComprehensiveReport(*reinterpret_cast<nlm::Brain*>(nullptr), 
                                            "neural_network_report.html")) {
        std::cout << "Generated comprehensive report successfully!\n";
    } else {
        std::cerr << "Failed to generate comprehensive report!" << std::endl;
    }
}

void exampleWebGLVisualization() {
    std::cout << "\n=== WebGL Visualization Example ===\n";
    
    // Create visualization tools
    auto vizTools = std::make_unique<nlm::VisualizationTools>();
    
    if (!vizTools->initialize("./examples_output")) {
        std::cerr << "Failed to initialize WebGL tools!" << std::endl;
        return;
    }
    
    // Create brain simulation (placeholder)
    
    // Generate WebGL visualization
    vizTools->generateWebGLVisualization(*reinterpret_cast<nlm::Brain*>(nullptr), "neural_network_webgl.html");
    
    std::cout << "Generated WebGL visualization successfully!\n";
}

void exampleBatchProcessing() {
    std::cout << "\n=== Batch Processing Example ===\n";
    
    // Create visualization tools
    auto vizTools = std::make_unique<nlm::VisualizationTools>();
    
    if (!vizTools->initialize("./examples_output")) {
        std::cerr << "Failed to initialize batch processing tools!" << std::endl;
        return;
    }
    
    // Start batch mode
    vizTools->startBatchMode(5); // Process 5 visualizations in batch
    
    // Generate multiple visualizations in batch
    vizTools->exportVisualization(nlm::VisualizationType::NETWORK_GRAPH, "batch_network_graph.json", nlm::VisualizationFormat::JSON);
    vizTools->exportVisualization(nlm::VisualizationType::NEURON_ACTIVITY, "batch_neuron_activity.html", nlm::VisualizationFormat::HTML);
    vizTools->exportVisualization(nlm::VisualizationType::SPIKE_RASTER, "batch_spike_raster.csv", nlm::VisualizationFormat::CSV);
    vizTools->exportVisualization(nlm::VisualizationType::WEIGHT_MATRIX, "batch_weight_matrix.html", nlm::VisualizationFormat::HTML);
    vizTools->exportVisualization(nlm::VisualizationType::POPULATION_STATISTICS, "batch_population_stats.json", nlm::VisualizationFormat::JSON);
    
    // End batch mode
    vizTools->endBatchMode();
    
    std::cout << "Completed batch processing successfully!\n";
}

void exampleRealTimeUpdates() {
    std::cout << "\n=== Real-Time Updates Example ===\n";
    
    // Create visualization tools
    auto vizTools = nlm::VisualizationTools::createInteractiveDashboard("Real-Time Dashboard");
    
    if (!vizTools->initialize("./examples_output")) {
        std::cerr << "Failed to initialize real-time dashboard!" << std::endl;
        return;
    }
    
    // Set progress callback
    vizTools->setProgressCallback([](double progress) {
        if (progress > 0.0 && progress < 1.0) {
            std::cout << "Progress: " << (progress * 100) << "%\n";
        }
    });
    
    // Set error callback
    vizTools->setErrorCallback([](const std::string& error) {
        std::cerr << "Real-time Error: " << error << std::endl;
    });
    
    // Simulate real-time updates
    std::cout << "Starting real-time simulation... (Press Ctrl+C to stop)\n";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto lastRender = startTime;
    
    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRender).count();
        
        if (elapsed >= 1000) { // Update every second
            double deltaTime = elapsed / 1000.0;
            vizTools->update(deltaTime);
            
            // Generate real-time visualization
            vizTools->generateVisualization(nlm::VisualizationType::NEURON_ACTIVITY, "real_time_activity");
            
            lastRender = currentTime;
            
            // Simulate stop after 5 seconds
            auto totalElapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
            if (totalElapsed >= 5) {
                break;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::cout << "Real-time simulation completed!\n";
}

int main() {
    std::cout << "NLM VisualizationTools Examples\n";
    std::cout << "==============================\n";
    
    try {
        exampleBasicVisualization();
        example3DVisualization();
        exampleVRVisualization();
        exampleInteractiveDashboard();
        exampleAnimation();
        exampleReportGeneration();
        exampleWebGLVisualization();
        exampleBatchProcessing();
        exampleRealTimeUpdates();
        
        std::cout << "\n=== All examples completed successfully! ===\n";
        std::cout << "Output files have been generated in ./examples_output/\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}