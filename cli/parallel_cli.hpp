// NLM CLI Parallel - Parallel execution commands header
// Handles multi-process execution, GPU acceleration, and distributed computing

#ifndef NLM_CLI_PARALLEL_HPP
#define NLM_CLI_PARALLEL_HPP

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <future>
#include <numeric>

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

namespace nlm {
namespace cli {

class ParallelCLI {
public:
    ParallelCLI();
    
    int run(int argc, char** argv);
    
private:
    void initializeLogger();
    
    // Parallel commands
    int handleParallelExecuteCommand(int argc, char** argv);
    int handleGPUExecuteCommand(int argc, char** argv);
    int handleDistributedExecuteCommand(int argc, char** argv);
    int handleResourceCommand(int argc, char** argv);
    int handleResourceMonitorCommand(int argc, char** argv);
    int handleResultAggregationCommand(int argc, char** argv);
    int handleSchedulingCommand(int argc, char** argv);
    int handleWorkloadCommand(int argc, char** argv);
    int handleParallelProfileCommand(int argc, char** argv);
    
    // Helper structures
    struct ParallelConfig {
        size_t numProcesses;
        size_t gpusPerProcess;
        size_t memoryPerProcess;
        std::string scheduler;
        bool enableResourceAllocation;
        double timeout;
    };
    
    struct ExperimentResults {
        size_t successfulRuns;
        size_t failedRuns;
        double averageTime;
        std::vector<double> executionTimes;
        std::vector<std::string> failedExperiments;
    };
    
    struct GPUResults {
        size_t successfulRuns;
        std::vector<double> gpuUtilization;
        std::vector<size_t> memoryUsages;
    };
    
    struct DistributedResults {
        size_t successfulRuns;
        size_t totalProcesses;
        double averageTime;
    };
    
    struct ResourceInfo {
        size_t cpuCores;
        size_t totalMemoryMB;
        size_t gpuCount;
        size_t availableStorageMB;
    };
    
    struct ResourceSample {
        std::chrono::system_clock::time_point timestamp;
        double cpuUsage;
        size_t memoryUsageMB;
        double gpuUsage;
    };
    
    struct AggregatedResults {
        size_t totalExperiments;
        size_t successfulExperiments;
        double averageScore;
        std::vector<double> scoreDistribution;
        double performanceTrend;
    };
    
    struct WorkloadSchedule {
        std::string scheduleId;
        double estimatedDuration;
        std::string priority;
    };
    
    struct WorkloadSpecification {
        std::string workloadId;
        std::map<std::string, size_t> requiredResources;
        double estimatedTime;
    };
    
    struct ParallelProfile {
        double totalTime;
        double averageProcessTime;
        double scalingEfficiency;
        double weakScaling;
    };
    
    // Helper functions
    void parseParallelArgs(ParallelConfig& config, std::string& experimentType, 
                          size_t& steps, std::string& outputDir,
                          int argc, char** argv);
    ExperimentResults runParallelExperiments(const ParallelConfig& config, 
                                             const std::string& experimentType,
                                             size_t steps,
                                             const std::string& outputDir);
    ExperimentResults runSingleExperiment(const std::string& experimentType,
                                         size_t steps,
                                         const std::string& outputDir);
    void runPhase3Experiment(std::shared_ptr<Brain> brain, size_t steps, const std::string& outputDir);
    void runPhase4Experiment(std::shared_ptr<Brain> brain, size_t steps, const std::string& outputDir);
    void runPhase5Experiment(std::shared_ptr<Brain> brain, size_t steps, const std::string& outputDir);
    void runPhase6Experiment(std::shared_ptr<Brain> brain, size_t steps, const std::string& outputDir);
    GPUResults runGPUExperiments(const std::string& experimentType,
                                 size_t steps,
                                 const std::string& outputDir);
    DistributedResults runDistributedExperiments(size_t steps,
                                                const std::string& outputDir);
    ResourceInfo collectResourceInfo();
    ResourceSample collectResourceSample();
    AggregatedResults aggregateResults(const std::string& inputDir);
    WorkloadSchedule createWorkloadSchedule();
    WorkloadSpecification createWorkloadSpecification();
    ParallelProfile profileParallelExecution(size_t duration);
    
    // Placeholder implementations for MPI and GPU
    bool initializeMPI();
    bool initializeGPUResources();
    void cleanupMPI();
    void cleanupGPUResources();
    
    void generateParallelReport(const ExperimentResults& results, 
                               const std::string& outputFile);
    void generateGPUReport(const GPUResults& results, const std::string& outputFile);
    void generateDistributedReport(const DistributedResults& results,
                                  const std::string& outputFile);
    void generateResourceReport(const ResourceInfo& info, const std::string& outputFile);
    void generateResourceMonitorReport(const std::vector<ResourceSample>& samples,
                                      const std::string& outputFile);
    void generateAggregationReport(const AggregatedResults& results,
                                  const std::string& outputFile);
    void generateParallelProfileReport(const ParallelProfile& profile,
                                      const std::string& outputFile);
    
    void saveSchedule(const WorkloadSchedule& schedule, const std::string& filename);
    void saveWorkload(const WorkloadSpecification& workload, const std::string& filename);
    
    void printUsage();
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_PARALLEL_HPP
