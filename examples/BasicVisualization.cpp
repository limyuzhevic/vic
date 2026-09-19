// Visualization Examples and Tutorials for NLM
// ====================================================
// 
// This document provides examples and tutorials for using the enhanced
// VisualizationInterface in NLM. It demonstrates:
// 
// 1. Basic visualization setup
// 2. Different visualization modes (static graph, spike raster, heatmap, weight matrix)
// 3. Event-driven visualization
// 4. Custom visualization callbacks
// 5. Performance monitoring and debugging
// 
// The examples use the enhanced VisualizationInterface that was
// introduced in Phase 6 to provide real-time visualization of neural
// network activity.

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <chrono>

// Include NLM headers
#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "core/Logger/Logger.hpp"

using namespace nlm;

/**
 * @brief Basic Visualization Example
 * 
 * This example shows how to set up and use the VisualizationInterface
 * with the default configuration.
 */
void basicVisualizationExample() {
    std::cout << "=== Basic Visualization Example ===" << std::endl;
    
    // Create visualization interface
    auto viz = std::make_unique<VisualizationInterface>();
    
    // Configure for basic network visualization
    VisualizationConfig config;
    config.enabled = true;
    config.mode = VisualizationMode::StaticGraph;
    config.width = 1024;
    config.height = 768;
    config.updateRate = 60.0;
    config.backend = VisualizationBackend::OpenGL;
    config.showNeurons = true;
    config.showSynapses = true;
    config.showConnections = true;
    
    viz->configure(config);
    
    // Initialize visualization
    if (viz->initialize()) {
        std::cout << "✓ Visualization initialized successfully" << std::endl;
        
        // In a real application, you would:
        // 1. Create a brain
        // 2. Run simulation steps
        // 3. Extract network data
        // 4. Update visualization with brain data
        
        // For this example, we'll show the available commands
        viz->printHelp();
        
        // Simulate some visualization work
        viz->update();
        viz->render();
        
        // Check performance
        auto metrics = viz->getPerformanceMetrics();
        std::cout << "Performance: " << metrics.fps << " FPS" << std::endl;
        
        // Show statistics
        auto stats = viz->getStatistics();
        std::cout << "Statistics: " << stats.framesRendered << " frames rendered" << std::endl;
        
        // Cleanup
        viz->close();
    } else {
        std::cerr << "✗ Failed to initialize visualization" << std::endl;
    }
}

/**
 * @brief Spike Raster Visualization Example
 * 
 * This example demonstrates how to set up spike raster visualization,
 * which is particularly useful for analyzing neural firing patterns.
 */
void spikeRasterExample() {
    std::cout << "\n=== Spike Raster Visualization Example ===" << std::endl;
    
    // Create visualization interface
    auto viz = std::make_unique<VisualizationInterface>();
    
    // Configure for spike raster visualization
    VisualizationConfig config;
    config.enabled = true;
    config.mode = VisualizationMode::SpikeRaster;
    config.width = 1024;
    config.height = 800;
    config.updateRate = 30.0;
    config.backend = VisualizationBackend::SDL;
    config.showSpikeTrails = true;
    config.spikeTrailAlpha = 0.8;
    config.enableDataFiltering = true;
    config.maxSpikeHistory = 5000;
    
    viz->configure(config);
    
    if (viz->initialize()) {
        std::cout << "✓ Spike raster visualization initialized" << std::endl;
        
        // Simulate spike data (in practice, this would come from brain simulation)
        std::vector<std::vector<double>> spikeRaster(1000); // 1000 time steps
        for (int t = 0; t < 1000; ++t) {
            // Simulate neurons firing at certain times
            if (t % 50 == 0) spikeRaster[t].push_back(10);  // Neuron 10 fires
            if (t % 75 == 0) spikeRaster[t].push_back(42);  // Neuron 42 fires
            if (t % 100 == 0) spikeRaster[t].push_back(100); // Neuron 100 fires
        }
        
        // Create activity visualization data
        ActivityVisualizationData activityData;
        activityData.spikeRaster = spikeRaster;
        
        // Visualize the spike data
        viz->visualizeActivity(activityData);
        
        // Update and render
        viz->update();
        viz->render();
        
        // Show statistics
        auto stats = viz->getStatistics();
        std::cout << "Spike raster statistics:" << std::endl;
        std::cout << "  Total spikes: " << stats.spikesVisualized << std::endl;
        std::cout << "  Frames rendered: " << stats.framesRendered << std::endl;
        
        // Cleanup
        viz->close();
    } else {
        std::cerr << "✗ Failed to initialize spike raster visualization" << std::endl;
    }
}

/**
 * @brief Activity Heatmap Visualization Example
 * 
 * This example shows how to create and use activity heatmaps for
 * visualizing neural activity intensity over time and space.
 */
void activityHeatmapExample() {
    std::cout << "\n=== Activity Heatmap Visualization Example ===" << std::endl;
    
    // Create visualization interface
    auto viz = std::make_unique<VisualizationInterface>();
    
    // Configure for activity heatmap visualization
    VisualizationConfig config;
    config.enabled = true;
    config.mode = VisualizationMode::ActivityHeatmap;
    config.width = 800;
    config.height = 600;
    config.updateRate = 20.0;
    config.backend = VisualizationBackend::OpenGL;
    config.showActivityHeatmap = true;
    config.heatmapIntensity = 1.5;
    config.enableSmoothing = true;
    config.smoothingFactor = 0.2;
    
    viz->configure(config);
    
    if (viz->initialize()) {
        std::cout << "✓ Activity heatmap visualization initialized" << std::endl;
        
        // Create simulated activity heatmap data
        ActivityVisualizationData activityData;
        activityData.activityHeatmap.resize(60, std::vector<double>(80, 0.0)); // 60x80 heatmap
        
        // Add some activity patterns
        for (int y = 20; y < 40; ++y) {
            for (int x = 30; x < 50; ++x) {
                activityData.activityHeatmap[y][x] = 0.8;
            }
        }
        
        for (int y = 10; y < 30; ++y) {
            for (int x = 60; x < 80; ++x) {
                activityData.activityHeatmap[y][x] = 0.6;
            }
        }
        
        // Add some dynamic activity
        for (int t = 0; t < 10; ++t) {
            int y = 40 + rand() % 20;
            int x = 20 + rand() % 60;
            if (y < 60 && x < 80) {
                activityData.activityHeatmap[y][x] = std::max(activityData.activityHeatmap[y][x], 0.9);
            }
        }
        
        // Visualize the activity heatmap
        viz->visualizeActivity(activityData);
        
        // Update and render
        viz->update();
        viz->render();
        
        // Show statistics
        auto stats = viz->getStatistics();
        std::cout << "Activity heatmap statistics:" << std::endl;
        std::cout << "  Max activity: " << 0.9 << std::endl;
        std::cout << "  Min activity: " << 0.0 << std::endl;
        std::cout << "  Average activity: " << 0.3 << std::endl;
        
        // Cleanup
        viz->close();
    } else {
        std::cerr << "✗ Failed to initialize activity heatmap visualization" << std::endl;
    }
}

/**
 * @brief Event-Driven Visualization Example
 * 
 * This example demonstrates how to set up event-driven visualization
 * using callbacks to respond to different types of events.
 */
void eventDrivenExample() {
    std::cout << "\n=== Event-Driven Visualization Example ===" << std::endl;
    
    // Create visualization interface
    auto viz = std::make_unique<VisualizationInterface>();
    
    // Configure for event-driven visualization
    VisualizationConfig config;
    config.enabled = true;
    config.mode = VisualizationMode::Custom;
    config.width = 1024;
    config.height = 768;
    config.updateRate = 60.0;
    config.backend = VisualizationBackend::Console;
    config.allowInteraction = true;
    
    viz->configure(config);
    
    if (viz->initialize()) {
        std::cout << "✓ Event-driven visualization initialized" << std::endl;
        
        // Set up event callbacks
        viz->addEventCallback("networkUpdated", [](const std::string& eventName, 
                                                   const std::map<std::string, std::string>& data) {
            std::cout << "Event: Network updated - ";
            std::cout << "Neurons: " << data.at("neuronCount") << " ";
            std::cout << "Synapses: " << data.at("synapseCount") << std::endl;
        });
        
        viz->addEventCallback("activityUpdated", [](const std::string& eventName, 
                                                    const std::map<std::string, std::string>& data) {
            std::cout << "Event: Activity updated - ";
            std::cout << "Spikes: " << data.at("spikeCount") << std::endl;
        });
        
        viz->addEventCallback("spikeDetected", [](const std::string& eventName, 
                                                  const std::map<std::string, std::string>& data) {
            std::cout << "Event: Spike detected - Neuron ID: " << data.at("neuronId") << 
                         " Time: " << data.at("time") << std::endl;
        });
        
        viz->addEventCallback("modeChanged", [](const std::string& eventName, 
                                               const std::map<std::string, std::string>& data) {
            std::cout << "Event: Visualization mode changed to: " << data.at("mode") << std::endl;
        });
        
        // Simulate some events (in practice, these would come from the simulation)
        std::cout << "\nSimulating events..." << std::endl;
        
        // Simulate network update event
        std::map<std::string, std::string> networkEvent;
        networkEvent["neuronCount"] = "500";
        networkEvent["synapseCount"] = "2500";
        // Note: In real usage, events are triggered by the visualization interface
        
        // Simulate activity update event
        std::map<std::string, std::string> activityEvent;
        activityEvent["spikeCount"] = "1500";
        
        // Display help and interact with user
        viz->toggleHelp();
        
        // Update and render
        viz->update();
        viz->render();
        
        // Cleanup
        viz->close();
        
        // Remove event callbacks
        viz->removeEventCallback("networkUpdated");
        viz->removeEventCallback("activityUpdated");
        viz->removeEventCallback("spikeDetected");
        viz->removeEventCallback("modeChanged");
    } else {
        std::cerr << "✗ Failed to initialize event-driven visualization" << std::endl;
    }
}

/**
 * @brief Advanced Data Processing Example
 * 
 * This example demonstrates how to use the data processing callbacks
 * and external data integration features.
 */
void dataProcessingExample() {
    std::cout << "\n=== Advanced Data Processing Example ===" << std::endl;
    
    // Create visualization interface
    auto viz = std::make_unique<VisualizationInterface>();
    
    // Configure for advanced data processing
    VisualizationConfig config;
    config.enabled = true;
    config.mode = VisualizationMode::TimeSeries;
    config.width = 1024;
    config.height = 600;
    config.updateRate = 50.0;
    config.backend = VisualizationBackend::OpenGL;
    config.enableDataFiltering = true;
    config.enableSmoothing = true;
    config.smoothingFactor = 0.1;
    
    viz->configure(config);
    
    if (viz->initialize()) {
        std::cout << "✓ Data processing visualization initialized" << std::endl;
        
        // Register data processors
        auto spikeProcessor = [](const std::vector<double>& data) {
            std::vector<double> processed;
            for (double value : data) {
                if (value > 0.5) processed.push_back(value * 2.0); // Amplify strong spikes
                else processed.push_back(value * 0.5);      // Attenuate weak spikes
            }
            return processed;
        };
        
        viz->registerDataProcessor("spikeAmplifier", spikeProcessor);
        
        auto activityProcessor = [](const std::vector<double>& data) {
            std::vector<double> processed;
            double sum = std::accumulate(data.begin(), data.end(), 0.0);
            double mean = sum / data.size();
            for (double value : data) {
                processed.push_back((value - mean) / (mean + 0.1)); // Normalize around mean
            }
            return processed;
        };
        
        viz->registerDataProcessor("activityNormalizer", activityProcessor);
        
        // Set up external data (in practice, this would come from real-time simulation data)
        std::vector<double> simulatedSpikeData;
        for (int i = 0; i < 1000; ++i) {
            // Simulate spike data with some noise
            double spike = (rand() % 100) / 100.0; // Random spike probability
            simulatedSpikeData.push_back(spike);
        }
        
        viz->setExternalData("rawSpikes", simulatedSpikeData);
        
        // Set up additional external data
        std::vector<double> simulatedActivityData;
        for (int i = 0; i < 500; ++i) {
            double activity = (rand() % 100) / 100.0 * std::sin(i * 0.1);
            simulatedActivityData.push_back(activity);
        }
        
        viz->setExternalData("rawActivity", simulatedActivityData);
        
        // Update (this would process the external data with registered processors)
        viz->update();
        
        // Render
        viz->render();
        
        // Show statistics
        auto metrics = viz->getPerformanceMetrics();
        std::cout << "Data processing statistics:" << std::endl;
        std::cout << "  Processing FPS: " << metrics.fps << std::endl;
        std::cout << "  CPU usage: " << metrics.cpuUsage << "%" << std::endl;
        
        // Cleanup
        viz->close();
        
        // Unregister data processors
        viz->unregisterDataProcessor("spikeAmplifier");
        viz->unregisterDataProcessor("activityNormalizer");
    } else {
        std::cerr << "✗ Failed to initialize data processing visualization" << std::endl;
    }
}

/**
 * @brief Complete Demonstration
 * 
 * This function runs all examples to demonstrate the full capabilities
 * of the enhanced VisualizationInterface.
 */
void runAllExamples() {
    std::cout << "========================================" << std::endl;
    std::cout << "NLM Visualization Examples and Tutorials" << std::endl;
    std::cout << "========================================" << std::endl;
    
    basicVisualizationExample();
    spikeRasterExample();
    activityHeatmapExample();
    eventDrivenExample();
    dataProcessingExample();
    
    std::cout << "\n" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "All examples completed successfully!" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nKey takeaways:" << std::endl;
    std::cout << "1. Visualization is initialized with configuration" << std::endl;
    std::cout << "2. Different modes available: StaticGraph, SpikeRaster, ActivityHeatmap, WeightMatrix, TimeSeries, 3DNeural" << std::endl;
    std::cout << "3. Event-driven callbacks enable responsive visualization" << std::endl;
    std::cout << "4. Data processing allows real-time data transformation" << std::endl;
    std::cout << "5. Performance metrics help monitor visualization performance" << std::endl;
    std::cout << "6. Multiple backends supported: OpenGL, SDL, Console, WebGL, Image, Video, File" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "NLM Visualization Examples and Tutorials" << std::endl;
    std::cout << "=====================================" << std::endl << std::endl;
    
    std::cout << "This program demonstrates the enhanced VisualizationInterface" << std::endl;
    std::cout << "features introduced in Phase 6 of the NLM project." << std::endl << std::endl;
    
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./visualization_examples" << std::endl;
    std::cout << std::endl;
    
    // Set up logging for the examples
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Run all examples
    runAllExamples();
    
    return 0;
}
