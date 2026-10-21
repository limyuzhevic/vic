#!/usr/bin/env g++
// NLM Monitor Tool
// Real-time monitoring of NLM simulation state

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <queue>

// NLM core headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

// Brain headers
#include "brain/Brain.hpp"

using namespace nlm;

struct MonitorData {
    double timestamp;
    std::string eventType;
    std::string message;
    std::map<std::string, std::string> metrics;
};

class NLMMonitor {
private:
    std::shared_ptr<Config> config;
    std::vector<MonitorData> monitorLog;
    std::queue<MonitorData> eventQueue;
    bool monitoringActive;
    std::mutex monitorMutex;
    double monitoringInterval;
    bool consoleOutput;
    bool fileOutput;
    std::string outputFile;
    
public:
    NLMMonitor() {
        config = std::make_shared<Config>();
        config->loadFromFile("configs/default.cfg");
        monitoringActive = false;
        monitoringInterval = 1.0;
        consoleOutput = true;
        fileOutput = false;
    }
    
    void startMonitoring(const std::string& brainName) {
        std::lock_guard<std::mutex> lock(monitorMutex);
        std::cout << "Starting NLM monitor for: " << brainName << std::endl;
        
        monitoringActive = true;
        monitorThread = std::thread(&NLMMonitor::monitoringLoop, this, brainName);
        
        std::cout << "Monitor started. Listening for events..." << std::endl;
    }
    
    void stopMonitoring() {
        std::lock_guard<std::mutex> lock(monitorMutex);
        
        if (monitoringActive) {
            monitoringActive = false;
            monitorCondition.notify_all();
            
            if (monitorThread.joinable()) {
                monitorThread.join();
            }
            
            std::cout << "Monitoring stopped." << std::endl;
        }
    }
    
    void logEvent(const std::string& eventType, const std::string& message) {
        std::lock_guard<std::mutex> lock(monitorMutex);
        
        MonitorData event;
        event.timestamp = getCurrentTimestamp();
        event.eventType = eventType;
        event.message = message;
        
        eventQueue.push(event);
        monitorCondition.notify_one();
        
        // Also log to monitor log for historical data
        monitorLog.push_back(event);
        
        // Print to console if enabled
        if (consoleOutput) {
            std::cout << "[" << formatTimestamp(event.timestamp) << "] "
                     << eventType << ": " << event.message << std::endl;
        }
        
        // Write to file if enabled
        if (fileOutput) {
            logToFile(event);
        }
    }
    
    void monitorBrain(std::shared_ptr<Brain> brain, const std::string& brainName) {
        std::cout << "=== Starting Brain Monitor for: " << brainName << " ===" << std::endl;
        
        // Initialize monitoring
        logEvent("MONITOR_START", "Brain monitoring started for " + brainName);
        
        // Monitor brain state for a period
        for (int step = 0; step < 100 && monitoringActive; ++step) {
            // Get brain status
            brain->logStatus();
            
            // Log key metrics
            logEvent("SIMULATION_STEP", "Step " + std::to_string(step) +
                    ", Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                    ", Neurons: " + std::to_string(brain->getTotalNeuronCount()));
            
            // Check for significant events
            if (brain->getTotalSpikeCount() > 0) {
                logEvent("SPIKE_EVENT", "Generated " + std::to_string(brain->getTotalSpikeCount()) + " spikes");
            }
            
            // Monitor memory usage
            auto* workingMemory = brain->getWorkingMemory();
            if (workingMemory) {
                logEvent("MEMORY_CHECK", "Working memory active");
            }
            
            // Monitor plasticity
            auto* episodicMemory = brain->getEpisodicMemory();
            if (episodicMemory) {
                logEvent("MEMORY_CHECK", "Episodic memory active");
            }
            
            // Sleep for monitoring interval
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(monitoringInterval * 1000)));
        }
        
        if (monitoringActive) {
            logEvent("MONITOR_STOP", "Brain monitoring completed for " + brainName);
        }
    }
    
    void monitoringLoop(const std::string& brainName) {
        std::unique_lock<std::mutex> lock(monitorMutex);
        
        while (monitoringActive) {
            monitorCondition.wait_for(lock, std::chrono::milliseconds(static_cast<int>(monitoringInterval * 1000)));
            
            if (monitoringActive) {
                // Process events in queue
                std::lock_guard<std::mutex> eventLock(monitorMutex);
                while (!eventQueue.empty()) {
                    MonitorData event = eventQueue.front();
                    eventQueue.pop();
                    
                    // Print event
                    std::cout << "[" << formatTimestamp(event.timestamp) << "] "
                             << event.eventType << ": " << event.message << std::endl;
                }
            }
        }
    }
    
    void logToFile(const MonitorData& event) {
        if (!fileOutput || outputFile.empty()) {
            return;
        }
        
        std::ofstream file(outputFile, std::ios::app);
        if (file.is_open()) {
            file << formatTimestamp(event.timestamp) << " "
                 << event.eventType << " "
                 << event.message << std::endl;
            file.close();
        }
    }
    
    void printCurrentStatus() {
        std::cout << "=== NLM Monitor Status ===" << std::endl;
        std::cout << "Monitoring active: " << (monitoringActive ? "Yes" : "No") << std::endl;
        std::cout << "Monitoring interval: " << monitoringInterval << "s" << std::endl;
        std::cout << "Console output: " << (consoleOutput ? "Yes" : "No") << std::endl;
        std::cout << "File output: " << (fileOutput ? "Yes" : "No") << std::endl;
        if (fileOutput) {
            std::cout << "Log file: " << outputFile << std::endl;
        }
        std::cout << "Total events logged: " << monitorLog.size() << std::endl;
        std::cout << "Events in queue: " << eventQueue.size() << std::endl;
    }
    
    void printEventHistory() {
        std::cout << "=== NLM Monitor Event History ===" << std::endl;
        
        if (monitorLog.empty()) {
            std::cout << "No events recorded." << std::endl;
            return;
        }
        
        std::cout << std::left << std::setw(20) << "Timestamp"
                  << std::setw(15) << "Event Type"
                  << std::setw(50) << "Message" << std::endl;
        std::cout << std::string(85, '-') << std::endl;
        
        for (const auto& event : monitorLog) {
            std::cout << std::left << std::setw(20) << formatTimestamp(event.timestamp)
                      << std::setw(15) << event.eventType
                      << std::setw(50) << event.message << std::endl;
        }
    }
    
    void printUsage(const std::string& programName) {
        std::cout << "Usage: " << programName << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --brain <name>           Brain name to monitor" << std::endl;
        std::cout << "  --interval <seconds>      Monitoring interval (default: 1.0)" << std::endl;
        std::cout << "  --console-off            Disable console output" << std::endl;
        std::cout << "  --file <file>            Log events to file" << std::endl;
        std::cout << "  --duration <seconds>     Monitoring duration (default: 10.0)" << std::endl;
        std::cout << "  --status                 Show current monitor status" << std::endl;
        std::cout << "  --history                Show event history" << std::endl;
        std::cout << "  --list-events           List available event types" << std::endl;
    }
    
private:
    std::thread monitorThread;
    std::condition_variable monitorCondition;
    std::chrono::high_resolution_clock::time_point startTime;
    double getCurrentTimestamp() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration<double>(duration).count();
    }
    
    std::string formatTimestamp(double timestamp) {
        std::time_t time = static_cast<std::time_t>(timestamp);
        std::tm* tm = std::localtime(&time);
        std::stringstream ss;
        ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

int main(int argc, char** argv) {
    std::string programName = "nlm-monitor";
    std::string brainName = "NLM_Brain";
    double interval = 1.0;
    double duration = 10.0;
    bool consoleEnabled = true;
    bool fileEnabled = false;
    std::string outputFile;
    std::string action = "monitor"; // monitor, status, history
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            NLMMonitor monitor;
            monitor.printUsage(programName);
            return 0;
        } else if (arg == "--brain") {
            if (i + 1 < argc) {
                brainName = argv[++i];
            }
        } else if (arg == "--interval") {
            if (i + 1 < argc) {
                interval = std::stod(argv[++i]);
            }
        } else if (arg == "--console-off") {
            consoleEnabled = false;
        } else if (arg == "--file") {
            if (i + 1 < argc) {
                fileEnabled = true;
                outputFile = argv[++i];
            }
        } else if (arg == "--duration") {
            if (i + 1 < argc) {
                duration = std::stod(argv[++i]);
            }
        } else if (arg == "--status") {
            action = "status";
        } else if (arg == "--history") {
            action = "history";
        } else if (arg == "--list-events") {
            action = "list-events";
        }
    }
    
    NLMMonitor monitor;
    monitor.monitoringInterval = interval;
    monitor.consoleOutput = consoleEnabled;
    monitor.fileOutput = fileEnabled;
    monitor.outputFile = outputFile;
    
    if (action == "monitor") {
        // Create a simple brain for demonstration
        auto config = std::make_shared<Config>();
        config->loadFromFile("configs/default.cfg");
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        monitor.startMonitoring(brainName);
        monitor.monitorBrain(brain, brainName);
        monitor.stopMonitoring();
    } else if (action == "status") {
        monitor.printCurrentStatus();
    } else if (action == "history") {
        monitor.printEventHistory();
    } else if (action == "list-events") {
        std::cout << "Available event types:" << std::endl;
        std::cout << "  MONITOR_START - Monitor started" << std::endl;
        std::cout << "  MONITOR_STOP - Monitor stopped" << std::endl;
        std::cout << "  SIMULATION_STEP - Simulation step completed" << std::endl;
        std::cout << "  SPIKE_EVENT - Spikes generated" << std::endl;
        std::cout << "  MEMORY_CHECK - Memory system check" << std::endl;
        std::cout << "  NEURON_EVENT - Neuron-specific event" << std::endl;
        std::cout << "  PLACILITYITY_EVENT - Plasticity event" << std::endl;
    }
    
    return 0;
}
