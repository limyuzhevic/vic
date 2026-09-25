#include "advanced/RealTimeMonitoring.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <atomic>

namespace nlm {

RealTimeMonitor::RealTimeMonitor() : running(false) {
}

RealTimeMonitor::~RealTimeMonitor() {
    stopMonitoring();
}

void RealTimeMonitor::startMonitoring(const std::shared_ptr<Brain>& brainRef) {
    brain = brainRef;
    running = true;
    startTime = std::chrono::steady_clock::now();
    
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      NLM Real-Time Monitoring Interface                           ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Press Ctrl+C to stop monitoring                                               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    while (running) {
        displayRealTimeStats();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void RealTimeMonitor::stopMonitoring() {
    running = false;
}

void RealTimeMonitor::displayRealTimeStats() const {
    system("clear"); // Clear screen (Linux/macOS)
    
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
    
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                              NLM Real-Time Monitoring                             ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Simulation Time: " << elapsed << "s                                               ║" << std::endl;
    std::cout << "║ Current Step: " << brain->getCurrentStep() << "                                        ║" << std::endl;
    std::cout << "║ Total Neurons: " << brain->getTotalNeuronCount() << "                                      ║" << std::endl;
    std::cout << "║ Total Synapses: " << brain->getTotalSynapseCount() << "                                     ║" << std::endl;
    std::cout << "║ Total Spikes: " << brain->getTotalSpikeCount() << "                                        ║" << std::endl;
    std::cout << "║ Firing Rate: " << std::fixed << std::setprecision(1) 
              << (brain->getTotalSpikeCount() / std::max(1.0, elapsed)) << " spikes/s       ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    // CPU and Memory Usage (simulated)
    double cpuUsage = calculateCPUUsage();
    double memoryUsage = calculateMemoryUsage();
    
    std::cout << "║ CPU Usage: " << std::setw(5) << std::fixed << std::setprecision(1) << cpuUsage << "%      "
              << "Memory: " << std::setw(5) << std::fixed << std::setprecision(1) << memoryUsage << "%   ║" << std::endl;
    
    // Progress bar
    double progress = (elapsed > 0) ? std::min(100.0, (brain->getCurrentStep() / 1000.0 * 100)) : 0.0;
    printProgressBar(progress);
    
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "Updating in 1 second... (Press Ctrl+C to stop)" << std::endl;
}

void RealTimeMonitor::updateStatistics() const {
    // Update real-time statistics
    // This would integrate with the brain's performance monitoring
}

double RealTimeMonitor::calculateCPUUsage() const {
    // Simulate CPU usage based on brain activity
    double baseCpu = 20.0; // Base CPU usage
    double activityFactor = std::min(80.0, brain->getTotalSpikeCount() / 10.0); // Activity-based
    return baseCpu + activityFactor;
}

double RealTimeMonitor::calculateMemoryUsage() const {
    // Simulate memory usage based on brain size
    double baseMemory = 100.0; // Base memory usage
    double brainSizeFactor = brain->getTotalNeuronCount() * 0.5; // Neurons contribute to memory
    return baseMemory + brainSizeFactor;
}

void RealTimeMonitor::printProgressBar(double progress) const {
    std::cout << "║ Progress: [";
    int barWidth = 30;
    int filledWidth = static_cast<int>(barWidth * progress / 100.0);
    
    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) {
            std::cout << "=";
        } else if (i == filledWidth) {
            std::cout << ">";
        } else {
            std::cout << " ";
        }
    }
    
    std::cout << std::fixed << std::setprecision(1) << progress << "%]          "
              << " (" << brain->getCurrentStep() << "/1000 steps)        " << std::endl;
}

} // namespace nlm