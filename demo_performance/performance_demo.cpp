#!/usr/bin/env bash

"""
Demonstration script for NLM Performance Monitoring System

This script shows how to use the PerformanceMonitor to track resource usage,
simulation progress, and neural metrics during NLM simulations.
"""

# Create demonstration directory
mkdir -p demo_performance

# Create a simple demo C++ program
cat > demo_performance/performance_demo.cpp << 'EOF'
#include "performance/PerformanceMonitor.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>

using namespace nlm;

// Global flag for graceful shutdown
bool running = true;

void signal_handler(int signal) {
    std::cout << "\nReceived signal, shutting down..." << std::endl;
    running = false;
}

void monitor_performance() {
    // Configure performance monitoring
    PerformanceMonitor& monitor = PerformanceMonitor::getGlobal();
    
    PerformanceConfig config;
    config.enableResourceMonitoring = true;
    config.enableProgressTracking = true;
    config.enablePerformanceProfiling = true;
    config.enableAlerting = true;
    config.progressUpdateInterval = 0.5;
    config.resourceCheckInterval = 2.0;
    
    monitor.configure(config);
    monitor.start();
    
    std::cout << "Performance monitoring started!" << std::endl;
    std::cout << "Monitor commands:" << std::endl;
    std::cout << "  's' - Show summary" << std::endl;
    std::cout << "  'a' - Check for alerts" << std::endl;
    std::cout << "  'h' - Show historical data" << std::endl;
    std::cout << "  'e' - Export metrics to CSV" << std::endl;
    std::cout << "  'q' - Quit" << std::endl;
}

int main() {
    // Set up signal handling for graceful shutdown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    std::cout << "=== NLM Performance Monitoring Demo ===" << std::endl;
    
    // Initialize NLM components
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    
    std::cout << "Creating NLM brain..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Start performance monitoring
    monitor_performance();
    
    // Set initial progress
    PerformanceMonitor::getGlobal().setProgress(0, 1000, 0.0);
    
    // Run simulation with performance monitoring
    const size_t totalSteps = 1000;
    const double timestep = 0.001;
    
    std::cout << "Running simulation..." << std::endl;
    
    for (size_t step = 0; step < totalSteps && running; ++step) {
        double currentTime = step * timestep;
        
        // Update progress
        PerformanceMonitor::getGlobal().setProgress(
            step, totalSteps, currentTime);
        
        // Create neural metrics
        NeuralMetrics metrics;
        metrics.totalNeurons = 500;
        metrics.activeNeurons = 400;
        metrics.firingNeurons = 50 + (std::rand() % 30); // Simulate variation
        metrics.averageFiringRate = 8.5 + (std::rand() % 10) / 10.0f;
        metrics.maximumFiringRate = 25.0f;
        metrics.minimumFiringRate = 5.0f;
        metrics.averageSynapticWeight = 0.15f;
        metrics.totalSpikes = step * 50 + (std::rand() % 100);
        metrics.spikesPerStep = 50 + (std::rand() % 30);
        metrics.plasticityEvents = step * 2 + (std::rand() % 10);
        metrics.developmentEvents = step / 100;
        
        // Update neural metrics
        PerformanceMonitor::getGlobal().updateNeuralMetrics(metrics);
        
        // Add some custom metrics
        PerformanceMonitor::getGlobal().addCustomMetric("temperature", 25.0 + std::rand() % 10);
        PerformanceMonitor::getGlobal().addCustomMetric("pressure", 1.0 + std::rand() % 100 / 100.0);
        PerformanceMonitor::getGlobal().addCustomMetric("humidity", 30.0 + std::rand() % 40);
        
        // Simulate step
        brain->step(step, currentTime);
        
        // Check for alerts every 100 steps
        if (step % 100 == 0) {
            auto alerts = PerformanceMonitor::getGlobal().checkForIssues();
            if (!alerts.empty()) {
                std::cout << "ALERTS DETECTED:" << std::endl;
                for (const auto& alert : alerts) {
                    std::cout << "  [" << static_cast<int>(alert.level) << "] " 
                              << alert.message << std::endl;
                }
            }
        }
        
        // Wait for next progress update
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Finalize
    std::cout << "\nSimulation complete!" << std::endl;
    std::cout << monitor.getSummary() << std::endl;
    
    // Export metrics to CSV
    std::cout << "Exporting metrics to CSV..." << std::endl;
    PerformanceMonitor::getGlobal().exportMetrics("demo_performance/metrics.csv");
    
    // Stop monitoring
    PerformanceMonitor::getGlobal().stop();
    
    std::cout << "Demo completed successfully!" << std::endl;
    
    return 0;
}
EOF
