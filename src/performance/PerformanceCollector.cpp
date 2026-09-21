// Performance module implementation
// 
// This module integrates with the brain and other systems to collect
// real-time performance metrics and statistics.

#include "performance/PerformanceMonitor.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace nlm {

class PerformanceCollector {
public:
    PerformanceCollector()
        : monitor_(), collecting_(false), samplingThread_(), updateInterval_(0.1)
        , brain_(nullptr), useRealBrain_(false) {}
    
    ~PerformanceCollector() {
        stopCollecting();
    }
    
    void startCollecting(std::shared_ptr<Brain> brain, double samplingInterval = 0.1) {
        brain_ = brain;
        useRealBrain_ = (brain != nullptr);
        updateInterval_ = samplingInterval;
        
        collecting_ = true;
        samplingThread_ = std::thread(&PerformanceCollector::samplingLoop, this);
        
        NLM_LOG_INFO("Performance monitoring started with sampling interval: " + 
                    std::to_string(updateInterval_) + " s");
    }
    
    void stopCollecting() {
        if (collecting_) {
            collecting_ = false;
            if (samplingThread_.joinable()) {
                samplingThread_.join();
            }
            NLM_LOG_INFO("Performance monitoring stopped");
        }
    }
    
    void setSamplingInterval(double interval) {
        updateInterval_ = interval;
    }
    
    PerformanceMonitor& getMonitor() {
        return monitor_;
    }
    
    const PerformanceMonitor& getMonitor() const {
        return monitor_;
    }
    
    std::string getPerformanceReport() const {
        std::stringstream report;
        report << "=== Performance Report ===" << std::endl;
        report << monitor_.getStatsSummary() << std::endl;
        report << monitor_.getOperationTimesSummary() << std::endl;
        report << monitor_.getThresholdViolations() << std::endl;
        return report.str();
    }
    
    void savePerformanceReport(const std::string& filename) const {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << getPerformanceReport();
            file.close();
            NLM_LOG_INFO("Performance report saved to: " + filename);
        } else {
            NLM_LOG_ERROR("Failed to save performance report to: " + filename);
        }
    }
    
    double getSystemMemoryUsage() {
        // Get system memory usage in MB
        // This is a simplified implementation
        // In a real implementation, we'd use platform-specific APIs
        
#ifdef __linux__
        std::ifstream memInfo("/proc/meminfo");
        if (memInfo.is_open()) {
            std::string line;
            long memTotal = 0;
            long memAvailable = 0;
            
            while (std::getline(memInfo, line)) {
                if (line.find("MemTotal:") == 0) {
                    memTotal = std::stol(line.substr(8));
                } else if (line.find("MemAvailable:") == 0) {
                    memAvailable = std::stol(line.substr(13));
                }
            }
            
            memInfo.close();
            
            if (memTotal > 0) {
                // Convert to MB
                return static_cast<double>(memAvailable) / 1024.0;
            }
        }
#endif
        
        // Fallback: return 0
        return 0.0;
    }
    
    double getCpuUsage() {
        // Get CPU usage percentage
        // This is a simplified implementation
        // In a real implementation, we'd use platform-specific APIs
        
        double cpuUsage = 0.0;
        
#ifdef __linux__
        std::ifstream statFile("/proc/stat");
        if (statFile.is_open()) {
            std::string line;
            std::getline(statFile, line);
            statFile.close();
            
            if (!line.empty()) {
                std::istringstream iss(line);
                std::string cpu;
                unsigned long user, nice, system, idle, iowait, irq, softirq, steal;
                
                if (iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal) {
                    unsigned long total = user + nice + system + idle + iowait + irq + softirq + steal;
                    unsigned long idleTotal = idle + iowait;
                    
                    static unsigned long prevTotal = total;
                    static unsigned long prevIdle = idleTotal;
                    static auto prevTime = std::chrono::high_resolution_clock::now();
                    
                    auto currentTime = std::chrono::high_resolution_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - prevTime).count();
                    
                    if (elapsed >= 100) { // Update every 100ms
                        double totalDiff = total - prevTotal;
                        double idleDiff = idleTotal - prevIdle;
                        
                        if (totalDiff > 0) {
                            cpuUsage = (totalDiff - idleDiff) / totalDiff * 100.0;
                        }
                        
                        prevTotal = total;
                        prevIdle = idleTotal;
                        prevTime = currentTime;
                    }
                }
            }
        }
#endif
        
        return cpuUsage;
    }
    
private:
    void samplingLoop() {
        auto lastUpdate = std::chrono::high_resolution_clock::now();
        
        while (collecting_) {
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration<double>(now - lastUpdate).count();
            
            if (elapsed >= updateInterval_) {
                updatePerformanceData(elapsed);
                lastUpdate = now;
            }
            
            // Sleep a bit to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    void updatePerformanceData(double elapsedTime) {
        PerformanceStats stats;
        
        // Get brain stats if available
        if (useRealBrain_ && brain_) {
            stats.totalSpikes = brain_->getTotalSpikeCount();
            stats.totalNeurons = brain_->getTotalNeuronCount();
            stats.totalSynapses = brain_->getTotalSynapseCount();
            stats.activeNeurons = brain_->getActiveNeuronCount();
            stats.firingNeurons = brain_->getFiringNeuronCount();
            stats.avgSynapticWeight = brain_->getExcitationInhibitionRatio();
            
            // Update simulation steps
            static size_t stepsCollected = 0;
            stepsCollected++;
            stats.simulationSteps = stepsCollected;
            
            // Get development stage
            stats.developmentalStage = static_cast<int>(brain_->getDevelopmentalStage());
            
            // Get neuromodulation levels
            if (brain_->getDopamine()) {
                stats.dopamineLevel = brain_->getDopamine()->getLevel();
            }
            if (brain_->getCuriosity()) {
                stats.curiosityLevel = brain_->getCuriosity()->getLevel();
            }
            if (brain_->getNovelty()) {
                stats.noveltyLevel = brain_->getNovelty()->getLevel();
            }
            
            // Get memory system stats
            if (brain_->getWorkingMemory()) {
                stats.workingMemoryTraces = brain_->getWorkingMemory()->getActiveTraces();
            }
            if (brain_->getEpisodicMemory()) {
                stats.episodicMemoryEpisodes = brain_->getEpisodicMemory()->getEpisodeCount();
            }
        }
        
        // Get system stats
        stats.cpuTime = getCpuUsage();
        stats.memoryUsage = getSystemMemoryUsage();
        stats.wallClockTime = elapsedTime;
        
        // Update performance monitor
        double timestamp = static_cast<double>(stepsCollected) * 0.001; // Assuming 1ms timestep
        monitor_.update(stats, timestamp, stepsCollected);
    }
};

} // namespace nlm
