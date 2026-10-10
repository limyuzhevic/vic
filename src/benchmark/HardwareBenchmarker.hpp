#pragma once

#include "BenchmarkRunner.hpp"
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <atomic>
#include <cmath>
#include <algorithm>

namespace nlm {

// Hardware-specific metrics
struct HardwareMetrics {
    // CPU metrics
    double cpuFrequency;              // CPU frequency in GHz
    size_t cpuCores;                   // Number of CPU cores
    size_t cpuThreads;                 // Number of CPU threads
    double cpuCacheL1;                 // L1 cache size in KB
    double cpuCacheL2;                 // L2 cache size in KB
    double cpuCacheL3;                 // L3 cache size in KB
    std::vector<double> cpuTemperature;// CPU temperature readings
    std::vector<double> cpuFrequencyHistory; // CPU frequency over time
    
    // Memory metrics
    size_t totalRAM;                   // Total RAM in bytes
    size_t availableRAM;               // Available RAM in bytes
    size_t usedRAM;                    // Used RAM in bytes
    double memoryFrequency;            // Memory frequency in MHz
    size_t memoryChannels;              // Number of memory channels
    std::vector<size_t> memoryModuleSizes; // Memory module sizes in MB
    
    // Storage metrics
    std::string storageType;           // Storage type (SSD, HDD, etc.)
    size_t storageCapacity;            // Storage capacity in bytes
    double storageReadSpeed;           // Read speed in MB/s
    double storageWriteSpeed;          // Write speed in MB/s
    double storageIOPS;                // IOPS (Input/Output Operations Per Second)
    
    // Network metrics
    std::string networkType;           // Network type (Ethernet, WiFi, etc.)
    double networkDownloadSpeed;       // Download speed in Mbps
    double networkUploadSpeed;         // Upload speed in Mbps
    double networkLatency;             // Network latency in ms
    
    // GPU metrics (if available)
    bool gpuAvailable;                 // Whether GPU is available
    std::string gpuModel;              // GPU model name
    double gpuMemory;                  // GPU memory in GB
    double gpuComputeCapability;       // GPU compute capability
    double gpuClockSpeed;              // GPU clock speed in MHz
    double gpuMemoryBandwidth;         // GPU memory bandwidth in GB/s
    std::vector<double> gpuTemperatures; // GPU temperature readings
    
    // Thermal metrics
    double cpuThermalThrottling;       // CPU thermal throttling percentage
    double gpuThermalThrottling;       // GPU thermal throttling percentage
    double systemTemperature;          // System temperature in Celsius
    
    // Power metrics
    double cpuPowerConsumption;        // CPU power consumption in watts
    double gpuPowerConsumption;        // GPU power consumption in watts
    double systemPowerConsumption;     // Total system power consumption in watts
    
    // Timestamp
    std::chrono::high_resolution_clock::time_point timestamp;
};

// Hardware configuration
struct HardwareConfig {
    std::string platform;              // Platform (Windows, Linux, macOS)
    std::string architecture;          // Architecture (x86_64, ARM64, etc.)
    std::string compiler;              // Compiler (GCC, Clang, MSVC, etc.)
    std::string compilerVersion;       // Compiler version
    std::string buildType;             // Build type (Debug, Release, RelWithDebInfo)
    
    // Performance tuning parameters
    bool enableSIMD;                   // Enable SIMD optimizations
    bool enableMultithreading;         // Enable multithreading
    bool enableMemoryPool;             // Enable memory pool optimizations
    bool enableEventQueue;             // Enable event queue optimizations
    bool enableSparseConnectivity;     // Enable sparse connectivity optimizations
    
    // Hardware-specific settings
    size_t threadCount;                // Number of threads to use
    size_t memoryPoolSize;             // Memory pool size in MB
    std::string simdExtension;         // SIMD extension (SSE, AVX, AVX2, AVX512)
    
    // Performance targets
    double targetPerformance;          // Target performance in operations per second
    size_t targetMemoryUsage;          // Target memory usage in bytes
    double targetCpuUsage;             // Target CPU usage percentage
};

class HardwareBenchmarker : public Benchmark {
private:
    std::string benchmarkName;
    BenchmarkCategory category;
    HardwareConfig hardwareConfig;
    HardwareMetrics currentHardware;
    
    // Benchmark configuration
    size_t neuronCount;
    size_t synapseCount;
    size_t simulationSteps;
    
    // Benchmark state
    bool isSetup;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::mutex hardwareMutex;
    
    // Hardware detection
    void detectHardware();
    void detectCpuInfo();
    void detectMemoryInfo();
    void detectStorageInfo();
    void detectNetworkInfo();
    void detectGpuInfo();
    void detectThermalInfo();
    void detectPowerInfo();
    
    // Hardware monitoring
    void monitorCpuUsage();
    void monitorMemoryUsage();
    void monitorGpuUsage();
    void monitorThermalState();
    void monitorPowerConsumption();
    
    // Performance testing
    void stressTestCpu(double duration = 60.0);
    void stressTestMemory(size_t size = 1024 * 1024 * 1024); // 1GB
    void stressTestIo(double duration = 30.0);
    
    // Platform-specific implementations
    void detectWindowsHardware();
    void detectLinuxHardware();
    void detectMacOSHardware();
    
    // Configuration management
    void applyHardwareConfig();
    void optimizeForHardware();
    
public:
    HardwareBenchmarker(size_t neurons = 1000, 
                       size_t steps = 1000,
                       const HardwareConfig& config = HardwareConfig());
    ~HardwareBenchmarker() override = default;
    
    // Benchmark interface implementation
    std::string getName() const override;
    BenchmarkCategory getCategory() const override;
    void setup() override;
    BenchmarkMetrics run() override;
    void teardown() override;
    bool validate() const override;
    
    // Configuration
    void setNeuronCount(size_t count);
    void setSimulationSteps(size_t steps);
    void setHardwareConfig(const HardwareConfig& config);
    
    size_t getNeuronCount() const;
    size_t getSimulationSteps() const;
    const HardwareConfig& getHardwareConfig() const;
    const HardwareMetrics& getHardwareMetrics() const;
    
    // Hardware information
    std::string getPlatform() const;
    std::string getArchitecture() const;
    std::string getCompiler() const;
    std::string getCompilerVersion() const;
    
    // Hardware metrics
    double getCpuFrequency() const;
    size_t getCpuCores() const;
    size_t getCpuThreads() const;
    double getCpuCacheL1() const;
    double getCpuCacheL2() const;
    double getCpuCacheL3() const;
    size_t getTotalRAM() const;
    size_t getAvailableRAM() const;
    bool getGpuAvailable() const;
    std::string getGpuModel() const;
    double getGpuMemory() const;
    double getGpuComputeCapability() const;
    
    // Performance analysis
    double calculateCpuEfficiency() const;
    double calculateMemoryEfficiency() const;
    double calculateCacheEfficiency() const;
    double calculateThermalEfficiency() const;
    double calculatePowerEfficiency() const;
    
    // Hardware-specific benchmarking
    BenchmarkMetrics runCpuBenchmark() const;
    BenchmarkMetrics runMemoryBenchmark() const;
    BenchmarkMetrics runIoBenchmark() const;
    BenchmarkMetrics runCacheBenchmark() const;
    BenchmarkMetrics runThermalBenchmark() const;
    BenchmarkMetrics runPowerBenchmark() const;
    
    // Scalability analysis
    BenchmarkMetrics runHardwareScalabilityTest(const std::vector<size_t>& neuronCounts) const;
    std::vector<BenchmarkMetrics> runHardwareBenchmarkSuite() const;
    
    // Optimization recommendations
    std::vector<std::string> getOptimizationRecommendations() const;
    std::vector<std::string> getHardwareSpecificTips() const;
    
    // Stress testing
    BenchmarkMetrics runCpuStressTest(double duration = 60.0) const;
    BenchmarkMetrics runMemoryStressTest(size_t size = 1024 * 1024 * 1024) const;
    BenchmarkMetrics runIoStressTest(double duration = 30.0) const;
    
    // Hardware monitoring
    std::vector<double> getCpuTemperatureHistory() const;
    std::vector<double> getGpuTemperatureHistory() const;
    std::vector<double> getSystemTemperatureHistory() const;
    std::vector<double> getCpuFrequencyHistory() const;
    std::vector<double> getMemoryUsageHistory() const;
    
private:
    void collectHardwareMetrics();
    void updateHardwareMetrics();
    BenchmarkMetrics convertHardwareMetricsToBenchmark(const HardwareMetrics& hardware) const;
};

} // namespace nlm