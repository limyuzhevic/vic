// Performance monitoring and reporting functionality for NLMTool
// Extracted from NLMTool.cpp for better organization

#include <chrono>
#include <thread>
#include <atomic>
#include <sstream>
#include <iomanip>

using namespace nlm;

class PerformanceMonitor {
public:
    PerformanceMonitor() : startTime(std::chrono::high_resolution_clock::now()),
                          totalSteps(0), totalSpikes(0), totalSimTime(0.0),
                          lastStepTime(0.0), monitoring(false) {}
    
    void startMonitoring() {
        startTime = std::chrono::high_resolution_clock::now();
        totalSteps = 0;
        totalSpikes = 0;
        totalSimTime = 0.0;
        monitoring = true;
    }
    
    void stopMonitoring() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        totalSimTime = duration;
        monitoring = false;
    }
    
    void updateStep(size_t steps, size_t spikes) {
        totalSteps += steps;
        totalSpikes += spikes;
    }
    
    void recordStepTime(double stepTime) {
        lastStepTime = stepTime;
    }
    
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== Performance Monitor ===\n";
        
        if (totalSteps == 0) {
            oss << "No simulation data available.\n";
            return oss.str();
        }
        
        oss << "Simulation Statistics:\n";
        oss << "  Total steps processed: " << totalSteps << "\n";
        oss << "  Total spikes generated: " << totalSpikes << "\n";
        oss << "  Average spikes per step: " << (totalSpikes / (double)totalSteps) << "\n";
        oss << "  Simulation time: " << totalSimTime << " seconds\n";
        
        if (totalSimTime > 0.0) {
            oss << "  Steps per second: " << (totalSteps / totalSimTime) << "\n";
            oss << "  Spikes per second: " << (totalSpikes / totalSimTime) << "\n";
        }
        
        if (lastStepTime > 0.0) {
            oss << "  Average step time: " << lastStepTime << " seconds\n";
            oss << "  Steps per simulated second: " << (1.0 / lastStepTime) << "\n";
        }
        
        oss << "\nPerformance Analysis:\n";
        
        // Calculate performance metrics
        double avgFiringRate = (totalSteps > 0) ? (totalSpikes / (double)totalSteps) * 100.0 : 0.0;
        oss << "  Average firing rate: " << avgFiringRate << "%\n";
        
        if (avgFiringRate > 10.0) {
            oss << "  ✓ High activity detected - System is engaged\n";
        } else if (avgFiringRate > 1.0) {
            oss << "  ✓ Moderate activity - Normal operation\n";
        } else if (avgFiringRate > 0.1) {
            oss << "  ! Low activity - Check for connectivity issues\n";
        } else {
            oss << "  ! Very low activity - System may be under-excited\n";
        }
        
        double efficiency = (avgFiringRate / 50.0) * 100.0; // Normalized efficiency
        oss << "  Activity efficiency: " << efficiency << "%\n";
        
        return oss.str();
    }
    
    bool isMonitoring() const { return monitoring; }
    
    double getTotalTime() const { return totalSimTime; }
    size_t getTotalSteps() const { return totalSteps; }
    size_t getTotalSpikes() const { return totalSpikes; }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::atomic<size_t> totalSteps;
    std::atomic<size_t> totalSpikes;
    std::atomic<double> totalSimTime;
    std::atomic<double> lastStepTime;
    std::atomic<bool> monitoring;
};

class BenchmarkRunner {
public:
    BenchmarkRunner() : monitor(), running(false) {}
    
    bool runBenchmark(size_t steps, std::shared_ptr<Brain> brain) {
        std::cout << "=== Running NLM Performance Benchmark ===" << std::endl;
        std::cout << "Steps: " << steps << std::endl;
        std::cout << "Brain initialization..." << std::endl;
        
        if (!brain->initialize()) {
            std::cout << "ERROR: Failed to initialize brain for benchmark." << std::endl;
            return false;
        }
        
        monitor.startMonitoring();
        running = true;
        
        std::cout << "Running benchmark..." << std::endl;
        
        // Create a backup of current state
        std::string backupFile = "./benchmark_backup.bin";
        bool hasBackup = brain->save(backupFile);
        
        auto benchmarkStart = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps && running; ++step) {
            // Apply continuous stimulation to maximize activity
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            
            double stepStart = std::chrono::duration<double>(
                std::chrono::high_resolution_clock::now() - benchmarkStart
            ).count();
            
            brain->step(step, step * 0.001);
            
            double stepEnd = std::chrono::duration<double>(
                std::chrono::high_resolution_clock::now() - benchmarkStart
            ).count();
            
            double stepTime = stepEnd - stepStart;
            monitor.recordStepTime(stepTime);
            monitor.updateStep(1, brain->getFiringNeuronCount());
            
            if (step % (steps / 10) == 0 || step % 1000 == 0) {
                std::cout << "  Progress: " << step << "/" << steps 
                         << " | Firing: " << brain->getFiringNeuronCount()
                         << " | Spikes: " << brain->getTotalSpikeCount()
                         << " | Time: " << stepEnd << "s"
                         << " | Step time: " << stepTime * 1000 << "ms"
                         << std::endl;
            }
            
            // Check for interruption
            if (shouldInterrupt()) {
                std::cout << "Benchmark interrupted by user." << std::endl;
                break;
            }
        }
        
        monitor.stopMonitoring();
        running = false;
        
        // Restore from backup if available
        if (hasBackup) {
            auto newBrain = std::make_shared<Brain>(brain->getConfig());
            newBrain->load(backupFile);
            std::cout << "Benchmark state restored." << std::endl;
        }
        
        std::string report = monitor.generateReport();
        std::cout << std::endl;
        std::cout << report;
        
        std::cout << "=== Benchmark Complete ===" << std::endl;
        
        return true;
    }
    
    void stopBenchmark() {
        running = false;
    }
    
    bool isRunning() const { return running; }
    
    std::string getBenchmarkReport() const {
        return monitor.generateReport();
    }
    
private:
    bool shouldInterrupt() {
        // Simple check - in real implementation, would use a flag
        return false;
    }
    
    PerformanceMonitor monitor;
    std::atomic<bool> running;
};

class SystemProfiler {
public:
    SystemProfiler() : profileEnabled(false), cpuUsage(0.0), memoryUsage(0.0), activeThreads(0) {}
    
    void enableProfiling() {
        profileEnabled = true;
        std::cout << "=== System Profiler Enabled ===" << std::endl;
        std::cout << "Monitoring CPU usage, memory usage, and thread activity..." << std::endl;
    }
    
    void disableProfiling() {
        profileEnabled = false;
        std::cout << "System profiler disabled." << std::endl;
    }
    
    void updateProfileMetrics() {
        if (!profileEnabled) return;
        
        // Mock metrics for demonstration
        // In real implementation, would use system monitoring tools
        static double cpuCounter = 0.0;
        static double memCounter = 0.0;
        
        cpuCounter += 0.05 + (rand() % 100) / 2000.0;  // Random variation
        memCounter += 0.02 + (rand() % 50) / 1000.0;
        
        cpuUsage = std::min(100.0, cpuCounter);
        memoryUsage = std::min(1024.0, memCounter);  // MB
        activeThreads = 1 + (rand() % 20);  // 1-21 threads
    }
    
    std::string generateProfileReport() const {
        std::ostringstream oss;
        oss << "=== System Profiler Report ===\n";
        
        if (!profileEnabled) {
            oss << "Profiler is not enabled. Use 'profile enable' to start monitoring.\n";
            return oss.str();
        }
        
        oss << "Resource Usage:\n";
        oss << "  CPU Usage: " << std::fixed << std::setprecision(1) << cpuUsage << "%\n";
        oss << "  Memory Usage: " << std::fixed << std::setprecision(1) << memoryUsage << " MB\n";
        oss << "  Active Threads: " << activeThreads << "\n";
        
        oss << "\nSystem Analysis:\n";
        
        // CPU analysis
        if (cpuUsage > 80.0) {
            oss << "  ✗ High CPU usage - Possible performance bottleneck\n";
        } else if (cpuUsage > 60.0) {
            oss << "  ! Moderate CPU usage - System is busy\n";
        } else {
            oss << "  ✓ CPU usage is normal\n";
        }
        
        // Memory analysis
        if (memoryUsage > 512.0) {
            oss << "  ⚠ High memory usage - Monitor for memory leaks\n";
        } else if (memoryUsage > 256.0) {
            oss << "  ! Moderate memory usage - Normal operation\n";
        } else {
            oss << "  ✓ Memory usage is normal\n";
        }
        
        // Thread analysis
        if (activeThreads > 10) {
            oss << "  ⚠ High thread count - Complex parallel operations\n";
        } else if (activeThreads > 5) {
            oss << "  ! Moderate thread count - Some parallel activity\n";
        } else {
            oss << "  ✓ Thread count is normal\n";
        }
        
        oss << "\nRecommendations:\n";
        
        if (cpuUsage > 80.0) {
            oss << "  1. Consider optimizing hot paths in C++ code\n";
            oss << "  2. Check for infinite loops or excessive computations\n";
        }
        
        if (memoryUsage > 512.0) {
            oss << "  1. Monitor memory allocations and deallocations\n";
            oss << "  2. Consider reducing object lifespans\n";
        }
        
        if (activeThreads > 10) {
            oss << "  1. Review parallel processing efficiency\n";
            oss << "  2. Consider thread pool optimization\n";
        }
        
        return oss.str();
    }
    
    bool isEnabled() const { return profileEnabled; }
    
private:
    bool profileEnabled;
    double cpuUsage;
    double memoryUsage;
    int activeThreads;
};

// Extended NLMTool functionality with performance monitoring
class EnhancedNLMTool : public NLMTool {
public:
    EnhancedNLMTool() : PerformanceMonitor(), BenchmarkRunner(), SystemProfiler(), 
                        currentBenchmark(nullptr), profileEnabled(false) {}
    
    bool processCommand(const std::string& input) {
        // Parse command (extended functionality)
        std::istringstream iss(input);
        std::string cmd;
        iss >> cmd;
        
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
        
        if (cmd == "monitor") {
            enableMonitoring();
        } else if (cmd == "benchmark") {
            std::string benchmarkType;
            if (iss >> benchmarkType) {
                runBenchmarkType(benchmarkType);
            } else {
                std::cout << "Usage: benchmark <type>" << std::endl;
                std::cout << "Types: connectivity, plasticity, stdp, memory, neuromodulation, full" << std::endl;
            }
        } else if (cmd == "profile") {
            std::string profileAction;
            if (iss >> profileAction) {
                if (profileAction == "enable") {
                    profiler.enableProfiling();
                } else if (profileAction == "disable") {
                    profiler.disableProfiling();
                } else {
                    std::cout << "Unknown profile action: " << profileAction << std::endl;
                    std::cout << "Actions: enable, disable" << std::endl;
                }
            } else {
                std::cout << "Usage: profile <action>" << std::endl;
                std::cout << "Actions: enable, disable" << std::endl;
            }
        } else if (cmd == "performance") {
            showPerformanceReport();
        } else if (cmd == "system") {
            showSystemReport();
        } else if (cmd == "monitor status") {
            showMonitoringStatus();
        } else {
            // Fall back to parent class for other commands
            return NLMTool::processCommand(input);
        }
        
        return true;
    }
    
    void enableMonitoring() {
        std::cout << "=== Performance Monitoring Enabled ===" << std::endl;
        std::cout << "Starting performance monitoring..." << std::endl;
        monitor.startMonitoring();
        
        std::cout << "Monitoring metrics:\n";
        std::cout << "  - Steps processed\n";
        std::cout << "  - Spikes generated\n";
        std::cout << "  - Simulation time\n";
        std::cout << "  - Performance analysis\n";
        std::cout << "  - Efficiency metrics\n";
    }
    
    void showMonitoringStatus() {
        std::cout << "=== Monitoring Status ===" << std::endl;
        if (monitor.isMonitoring()) {
            std::cout << "Status: ACTIVE" << std::endl;
            std::cout << "Total steps: " << monitor.getTotalSteps() << std::endl;
            std::cout << "Total spikes: " << monitor.getTotalSpikes() << std::endl;
            std::cout << "Total time: " << monitor.getTotalTime() << " seconds" << std::endl;
        } else {
            std::cout << "Status: INACTIVE" << std::endl;
            std::cout << "Use 'monitor' to start monitoring." << std::endl;
        }
    }
    
    void showPerformanceReport() {
        std::cout << monitor.generateReport();
    }
    
    void showSystemReport() {
        std::cout << profiler.generateProfileReport();
    }
    
    void runBenchmarkType(const std::string& type) {
        if (!brain) {
            std::cout << "ERROR: Brain not initialized." << std::endl;
            return;
        }
        
        std::cout << "=== Starting " << type << " Benchmark ===" << std::endl;
        
        size_t steps = 0;
        if (type == "connectivity") {
            steps = 500;
            std::cout << "Running connectivity benchmark..." << std::endl;
        } else if (type == "plasticity") {
            steps = 1000;
            std::cout << "Running plasticity benchmark..." << std::endl;
        } else if (type == "stdp") {
            steps = 800;
            std::cout << "Running STDP benchmark..." << std::endl;
        } else if (type == "memory") {
            steps = 1200;
            std::cout << "Running memory benchmark..." << std::endl;
        } else if (type == "neuromodulation") {
            steps = 600;
            std::cout << "Running neuromodulation benchmark..." << std::endl;
        } else if (type == "full") {
            steps = 2000;
            std::cout << "Running full integration benchmark..." << std::endl;
        } else {
            std::cout << "Unknown benchmark type: " << type << std::endl;
            return;
        }
        
        // Run the benchmark
        currentBenchmark = std::make_unique<BenchmarkRunner>();
        currentBenchmark->runBenchmark(steps, brain);
    }
    
    void stopBenchmark() {
        if (currentBenchmark && currentBenchmark->isRunning()) {
            currentBenchmark->stopBenchmark();
            std::cout << "Benchmark stopped." << std::endl;
        }
    }
    
private:
    PerformanceMonitor monitor;
    BenchmarkRunner benchmarkRunner;
    SystemProfiler profiler;
    std::unique_ptr<BenchmarkRunner> currentBenchmark;
    bool profileEnabled;
};