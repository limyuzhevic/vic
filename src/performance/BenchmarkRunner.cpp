// BenchmarkRunner.cpp
#include "BenchmarkRunner.cpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <fstream>

using json = nlohmann::json;

namespace nlm {

BenchmarkRunner::BenchmarkRunner() = default;
BenchmarkRunner::~BenchmarkRunner() = default;

bool BenchmarkRunner::run(const BenchmarkConfig& config,
                         std::function<std::shared_ptr<class Brain>(const std::string& scale)> createBrain,
                         std::function<bool(class Brain* brain, uint64_t steps)> runSimulation,
                         std::function<BenchmarkMetrics(class Brain* brain)> collectMetrics) {
    results_.clear();
    
    // Warmup phase
    if (config.warmupSteps > 0) {
        auto warmupBrain = createBrain("warmup");
        runSimulation(warmupBrain.get(), config.warmupSteps);
    }
    
    // Run benchmarks for each scale
    for (const auto& scale : config.scales) {
        for (uint64_t seed : config.seeds) {
            BenchmarkRun run;
            run.name = config.name + "_" + scale + "_seed_" + std::to_string(seed);
            run.scale = scale;
            run.randomSeed = seed;
            run.config = {
                {"stepsPerScale", std::to_string(config.stepsPerScale)},
                {"warmupSteps", std::to_string(config.warmupSteps)},
                {"enableMultithreading", config.enableMultithreading ? "true" : "false"},
                {"enableSIMD", config.enableSIMD ? "true" : "false"},
                {"enableSparseConnectivity", config.enableSparseConnectivity ? "true" : "false"},
                {"enableEventDriven", config.enableEventDriven ? "true" : "false"},
                {"threadCount", std::to_string(config.threadCount)}
            };
            
            auto brain = createBrain(scale);
            brain->initialize();
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            if (runSimulation(brain.get(), config.stepsPerScale)) {
                run.stepsRun = config.stepsPerScale;
                
                auto endTime = std::chrono::high_resolution_clock::now();
                run.wallClockTime = std::chrono::duration<double>(endTime - startTime).count();
                run.simulatedTime = config.stepsPerScale * 0.001; // 1ms per step
                
                run.startTime = time(nullptr);
                run.endTime = time(nullptr);
                
                // Collect metrics
                run.metrics = collectMetrics(brain.get());
                
                // Get hardware info
                run.cpuModel = getHardwareInfo(); // Would need implementation
                run.gpuModel = "N/A"; // Would need GPU detection
                run.cpuCores = std::thread::hardware_concurrency();
                run.ramGB = 0; // Would need system memory detection
                run.vramGB = 0; // Would need GPU memory detection
                
                results_.push_back(run);
            }
        }
    }
    
    return !results_.empty();
}

std::string BenchmarkRunner::generateReport() const {
    std::stringstream ss;
    ss << "Benchmark Report\n";
    ss << "===============\n\n";
    
    if (results_.empty()) {
        ss << "No benchmark results to report.\n";
        return ss.str();
    }
    
    ss << "Benchmarks run: " << results_.size() << "\n\n";
    
    // Generate summary by scale
    std::unordered_map<std::string, std::vector<BenchmarkRun>> runsByScale;
    for (const auto& result : results_) {
        runsByScale[result.scale.name].push_back(result);
    }
    
    for (const auto& pair : runsByScale) {
        const auto& scaleRuns = pair.second;
        ss << "Scale: " << pair.first << " (" << scaleRuns[0].scale.neurons << " neurons)\n";
        
        double totalTime = 0;
        double totalStepsPerSecond = 0;
        
        for (const auto& run : scaleRuns) {
            totalTime += run.wallClockTime;
            totalStepsPerSecond += run.metrics.neuronsPerSecond;
        }
        
        double avgTime = totalTime / scaleRuns.size();
        double avgStepsPerSec = totalStepsPerSecond / scaleRuns.size();
        
        ss << "  Average wall clock time: " << avgTime << "s\n";
        ss << "  Average neurons per second: " << avgStepsPerSec << "\n";
        ss << "  Number of runs: " << scaleRuns.size() << "\n\n";
    }
    
    return ss.str();
}

bool BenchmarkRunner::saveResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    json j;
    j["runs"] = json::array();
    
    for (const auto& run : results_) {
        json jRun;
        jRun["name"] = run.name;
        jRun["scale"] = run.scale.name;
        jRun["neurons"] = run.scale.neurons;
        jRun["synapses"] = run.scale.expectedSynapses;
        jRun["randomSeed"] = run.randomSeed;
        jRun["stepsRun"] = run.stepsRun;
        jRun["wallClockTime"] = run.wallClockTime;
        jRun["simulatedTime"] = run.simulatedTime;
        jRun["cpuModel"] = run.cpuModel;
        jRun["gpuModel"] = run.gpuModel;
        jRun["cpuCores"] = run.cpuCores;
        jRun["ramGB"] = run.ramGB;
        jRun["vramGB"] = run.vramGB;
        jRun["config"] = run.config;
        jRun["startTime"] = run.startTime;
        jRun["endTime"] = run.endTime;
        
        // Add metrics
        json jMetrics;
        jMetrics["neuronsPerSecond"] = run.metrics.neuronsPerSecond;
        jMetrics["synapsesPerSecond"] = run.metrics.synapsesPerSecond;
        jMetrics["spikesPerSecond"] = run.metrics.spikesPerSecond;
        jMetrics["realTimeFactor"] = run.metrics.realTimeFactor;
        jMetrics["avgStepTimeMs"] = run.metrics.avgStepTimeMs;
        jMetrics["totalMemoryMB"] = run.metrics.totalMemoryMB;
        jMetrics["peakMemoryMB"] = run.metrics.peakMemoryMB;
        jMetrics["activeNeurons"] = run.metrics.activeNeurons;
        jMetrics["firingNeurons"] = run.metrics.firingNeurons;
        jMetrics["avgFiringRateHz"] = run.metrics.avgFiringRateHz;
        
        jRun["metrics"] = jMetrics;
        j["runs"].push_back(jRun);
    }
    
    file << j.dump(2);
    file.close();
    return true;
}

bool BenchmarkRunner::loadResults(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        file.close();
        return false;
    }
    
    clearResults();
    
    for (const auto& jRun : j["runs"]) {
        BenchmarkRun run;
        run.name = jRun["name"].get<std::string>();
        
        // Parse scale info from name or metadata
        size_t pos = run.name.find_last_of('_');
        if (pos != std::string::npos) {
            std::string scaleName = run.name.substr(pos + 1);
            run.scale.name = scaleName;
        }
        
        run.randomSeed = jRun["randomSeed"].get<uint64_t>();
        run.stepsRun = jRun["stepsRun"].get<uint64_t>();
        run.wallClockTime = jRun["wallClockTime"].get<double>();
        run.simulatedTime = jRun["simulatedTime"].get<double>();
        run.cpuModel = jRun.value("cpuModel", "");
        run.gpuModel = jRun.value("gpuModel", "");
        run.cpuCores = jRun.value("cpuCores", 0);
        run.ramGB = jRun.value("ramGB", 0);
        run.vramGB = jRun.value("vramGB", 0);
        run.config = jRun["config"].get<std::unordered_map<std::string, std::string>>();
        run.startTime = jRun["startTime"].get<time_t>();
        run.endTime = jRun["endTime"].get<time_t>();
        
        // Load metrics
        if (jRun.contains("metrics")) {
            const auto& jMetrics = jRun["metrics"];
            run.metrics.neuronsPerSecond = jMetrics.value("neuronsPerSecond", 0.0);
            run.metrics.synapsesPerSecond = jMetrics.value("synapsesPerSecond", 0.0);
            run.metrics.spikesPerSecond = jMetrics.value("spikesPerSecond", 0.0);
            run.metrics.realTimeFactor = jMetrics.value("realTimeFactor", 0.0);
            run.metrics.avgStepTimeMs = jMetrics.value("avgStepTimeMs", 0.0);
            run.metrics.totalMemoryMB = jMetrics.value("totalMemoryMB", 0);
            run.metrics.peakMemoryMB = jMetrics.value("peakMemoryMB", 0);
            run.metrics.activeNeurons = jMetrics.value("activeNeurons", 0);
            run.metrics.firingNeurons = jMetrics.value("firingNeurons", 0);
            run.metrics.avgFiringRateHz = jMetrics.value("avgFiringRateHz", 0.0);
        }
        
        results_.push_back(run);
    }
    
    file.close();
    return true;
}

void BenchmarkRunner::clearResults() {
    results_.clear();
}

} // namespace nlm
