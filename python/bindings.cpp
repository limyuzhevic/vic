#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <map>
#include <unordered_map>
#include <variant>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/Observation.hpp"
#include "../src/neuromodulation/Reward.hpp"

namespace py = pybind11;
namespace fs = std::filesystem;
namespace json = nlohmann;

// Custom exception classes for Python bindings
class KiloRuntimeError : public std::runtime_error {
public:
    explicit KiloRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class KiloValidationError : public std::runtime_error {
public:
    explicit KiloValidationError(const std::string& msg) : std::runtime_error(msg) {}
};

class KiloConfigurationError : public std::runtime_error {
public:
    explicit KiloConfigurationError(const std::string& msg) : std::runtime_error(msg) {}
};

class KiloNotImplementedError : public std::runtime_error {
public:
    explicit KiloNotImplementedError(const std::string& msg) : std::runtime_error(msg) {}
};

class KiloResourceError : public std::runtime_error {
public:
    explicit KiloResourceError(const std::string& msg) : std::runtime_error(msg) {}
};

class KiloExperimentError : public std::runtime_error {
public:
    explicit KiloExperimentError(const std::string& msg) : std::runtime_error(msg) {}
};

// Memory tracking utilities
namespace memory {
    struct MemoryUsage {
        size_t totalBytes = 0;
        size_t allocatedBytes = 0;
        size_t peakBytes = 0;
        size_t residentBytes = 0;
        size_t sharedBytes = 0;
    };
    
    MemoryUsage getMemoryUsage() {
        MemoryUsage usage;
        // Implementation depends on platform
#if defined(__unix__) || defined(__unix) || defined(__APPLE__)
        #include <sys/resource.h>
        struct rusage ru;
        getrusage(RUSAGE_SELF, &ru);
        usage.residentBytes = ru.ru_maxrss * 1024; // Convert KB to bytes
        usage.peakBytes = ru.ru_maxrss * 1024;
#endif
        // For now, use approximate allocation tracking
        usage.totalBytes = 0;
        usage.allocatedBytes = 0;
        return usage;
    }
}

// Performance monitoring
namespace performance {
    struct PerformanceStats {
        double totalWallTime = 0.0;
        double simulationSteps = 0;
        double stepsPerSecond = 0.0;
        size_t memoryUsage = 0;
        float cpuUsage = 0.0f;
        std::atomic<size_t> activeThreads{0};
        std::atomic<double> currentMemory{0.0};
    };
    
    class Profiler {
    public:
        struct ProfileData {
            std::string name;
            double duration;
            size_t memoryAllocated;
            std::chrono::high_resolution_clock::time_point startTime;
            bool active;
        };
        
        Profiler() : totalMemoryAllocated(0) {}
        
        void startProfile(const std::string& name) {
            std::lock_guard<std::mutex> lock(mutex_);
            ProfileData data;
            data.name = name;
            data.startTime = std::chrono::high_resolution_clock::now();
            data.active = true;
            profiles_.push_back(data);
        }
        
        double stopProfile(const std::string& name) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto endTime = std::chrono::high_resolution_clock::now();
            
            for (auto& profile : profiles_) {
                if (profile.name == name && profile.active) {
                    auto duration = std::chrono::duration<double>(endTime - profile.startTime).count();
                    profile.duration = duration;
                    profile.active = false;
                    return duration;
                }
            }
            return 0.0;
        }
        
        std::vector<ProfileData> getProfiles() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return profiles_;
        }
        
        void recordMemoryAllocation(size_t bytes) {
            std::lock_guard<std::mutex> lock(mutex_);
            totalMemoryAllocated += bytes;
        }
        
        size_t getTotalMemoryAllocated() const {
            return totalMemoryAllocated;
        }
        
    private:
        std::vector<ProfileData> profiles_;
        mutable std::mutex mutex_;
        size_t totalMemoryAllocated;
    };
    
    class MemoryProfiler {
    public:
        struct MemorySample {
            std::chrono::high_resolution_clock::time_point timestamp;
            size_t memoryUsage;
            std::string operation;
        };
        
        MemoryProfiler() : recording_(false) {}
        
        void startRecording(const std::string& operationName = "default") {
            std::lock_guard<std::mutex> lock(mutex_);
            recording_ = true;
            currentOperation_ = operationName;
            samples_.clear();
        }
        
        void recordSample() {
            if (!recording_) return;
            
            std::lock_guard<std::mutex> lock(mutex_);
            MemorySample sample;
            sample.timestamp = std::chrono::high_resolution_clock::now();
            sample.memoryUsage = memory::getMemoryUsage().residentBytes;
            sample.operation = currentOperation_;
            samples_.push_back(sample);
        }
        
        void stopRecording() {
            std::lock_guard<std::mutex> lock(mutex_);
            recording_ = false;
        }
        
        std::vector<MemorySample> getSamples() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return samples_;
        }
        
        size_t getPeakMemory() const {
            size_t peak = 0;
            for (const auto& sample : samples_) {
                if (sample.memoryUsage > peak) peak = sample.memoryUsage;
            }
            return peak;
        }
        
    private:
        std::vector<MemorySample> samples_;
        mutable std::mutex mutex_;
        bool recording_;
        std::string currentOperation_;
    };
    
    py::object enableProfiling(const std::function<void()>& callback) {
        auto stats = std::make_shared<PerformanceStats>();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        callback();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stats->totalWallTime = std::chrono::duration<double>(endTime - startTime).count();
        stats->simulationSteps = 1.0; // Default for single callback
        stats->stepsPerSecond = stats->simulationSteps / stats->totalWallTime;
        
        // Get memory usage
        auto memUsage = memory::getMemoryUsage();
        stats->memoryUsage = memUsage.residentBytes;
        
        return py::cast(stats);
    }
    
    py::object getPerformanceStats() {
        auto stats = std::make_shared<PerformanceStats>();
        
        // Get memory usage (platform-specific)
        auto memUsage = memory::getMemoryUsage();
        stats->memoryUsage = memUsage.residentBytes;
        
        // Default performance values (would need actual monitoring)
        stats->totalWallTime = 0.0;
        stats->simulationSteps = 0.0;
        stats->stepsPerSecond = 0.0;
        stats->cpuUsage = 0.0f;
        stats->currentMemory = memUsage.residentBytes;
        
        return py::cast(stats);
    }
}

// Configuration builder
namespace config {
    class ConfigBuilder {
    public:
        ConfigBuilder() {
            cfg_ = std::make_shared<Config>();
        }
        
        ConfigBuilder& set(const std::string& key, const py::object& value) {
            // Convert Python object to appropriate C++ type
            if (value.is(py::int_())) {
                cfg_->set(key, static_cast<float>(py::cast<int64_t>(value)));
            } else if (value.is(py::float_())) {
                cfg_->set(key, py::cast<float>(value));
            } else if (value.is(py::bool_())) {
                cfg_->set(key, py::cast<bool>(value));
            } else if (value.is(py::str())) {
                cfg_->set(key, py::cast<std::string>(value));
            } else if (value.is(py::list())) {
                auto vec = py::cast<std::vector<float>>(value);
                cfg_->set(key, vec);
            } else if (value.is(py::dict())) {
                auto dict = py::cast<std::map<std::string, py::object>>(value);
                buildFromDict(dict);
            } else {
                throw KiloValidationError("Unsupported type for configuration value");
            }
            return *this;
        }
        
        ConfigBuilder& set(const std::string& key, float value) {
            cfg_->set(key, value);
            return *this;
        }
        
        ConfigBuilder& set(const std::string& key, int value) {
            cfg_->set(key, static_cast<float>(value));
            return *this;
        }
        
        ConfigBuilder& set(const std::string& key, bool value) {
            cfg_->set(key, value);
            return *this;
        }
        
        ConfigBuilder& set(const std::string& key, const std::string& value) {
            cfg_->set(key, value);
            return *this;
        }
        
        ConfigBuilder& set(const std::string& key, const std::vector<float>& value) {
            cfg_->set(key, value);
            return *this;
        }
        
        std::shared_ptr<Config> build() {
            return cfg_;
        }
        
        py::object buildToPython() {
            return py::cast(cfg_);
        }
        
        static py::object buildFromDict(const std::map<std::string, py::object>& config) {
            auto cfg = std::make_shared<Config>();
            
            for (const auto& pair : config) {
                const std::string& key = pair.first;
                py::object value = pair.second;
                
                // Try to convert based on type
                if (value.is(py::int_())) {
                    cfg->set(key, static_cast<float>(py::cast<int64_t>(value)));
                } else if (value.is(py::float_())) {
                    cfg->set(key, py::cast<float>(value));
                } else if (value.is(py::bool_())) {
                    cfg->set(key, py::cast<bool>(value));
                } else if (value.is(py::str())) {
                    cfg->set(key, py::cast<std::string>(value));
                } else if (value.is(py::list())) {
                    // Handle list of floats
                    auto vec = py::cast<std::vector<float>>(value);
                    cfg->set(key, vec);
                }
            }
            
            return py::cast(cfg);
        }
        
        static py::object buildDefaultConfig() {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 1000u);
            cfg->set("region_count", 4u);
            cfg->set("connection_probability", 0.1f);
            cfg->set("random_seed", 0u);
            cfg->set("simulation_timestep", 0.001f);
            cfg->set("stdp_ltp_weight", 0.01f);
            cfg->set("stdp_ltd_weight", 0.012f);
            cfg->set("synaptogenesis_rate", 0.0001f);
            cfg->set("pruning_rate", 0.00001f);
            return py::cast(cfg);
        }
        
        static py::object buildExperimentConfig(const std::string& experimentType) {
            auto cfg = std::make_shared<Config>();
            
            if (experimentType == "curiosity") {
                cfg->set("neuron_count", 2000u);
                cfg->set("region_count", 8u);
                cfg->set("connection_probability", 0.15f);
                cfg->set("random_seed", 12345u);
                cfg->set("simulation_timestep", 0.0005f);
                cfg->set("stdp_ltp_weight", 0.02f);
                cfg->set("stdp_ltd_weight", 0.018f);
                cfg->set("synaptogenesis_rate", 0.0002f);
                cfg->set("pruning_rate", 0.00002f);
                cfg->set("enable_curiosity", true);
                cfg->set("enable_development", true);
                cfg->set("enable_reward_modulation", true);
            } else if (experimentType == "learning") {
                cfg->set("neuron_count", 500u);
                cfg->set("region_count", 2u);
                cfg->set("connection_probability", 0.05f);
                cfg->set("random_seed", 67890u);
                cfg->set("simulation_timestep", 0.001f);
                cfg->set("stdp_ltp_weight", 0.005f);
                cfg->set("stdp_ltd_weight", 0.006f);
                cfg->set("synaptogenesis_rate", 0.00005f);
                cfg->set("pruning_rate", 0.000005f);
                cfg->set("enable_structural_plasticity", true);
                cfg->set("enable_developmental_plasticity", true);
            } else if (experimentType == "social") {
                cfg->set("neuron_count", 1500u);
                cfg->set("region_count", 6u);
                cfg->set("connection_probability", 0.12f);
                cfg->set("random_seed", 99999u);
                cfg->set("simulation_timestep", 0.0008f);
                cfg->set("stdp_ltp_weight", 0.015f);
                cfg->set("stdp_ltd_weight", 0.016f);
                cfg->set("synaptogenesis_rate", 0.00015f);
                cfg->set("pruning_rate", 0.000015f);
                cfg->set("enable_social_learning", true);
                cfg->set("enable_imitation_learning", true);
            } else {
                // Default config for unknown experiment types
                cfg->set("neuron_count", 1000u);
                cfg->set("region_count", 4u);
                cfg->set("connection_probability", 0.1f);
                cfg->set("random_seed", 0u);
                cfg->set("simulation_timestep", 0.001f);
            }
            
            return py::cast(cfg);
        }
        
        static bool validateConfig(const std::shared_ptr<Config>& config) {
            if (!config) return false;
            
            // Check required keys
            std::vector<std::string> requiredKeys = {"neuron_count", "region_count"};
            for (const auto& key : requiredKeys) {
                if (!config->has(key)) {
                    return false;
                }
            }
            
            // Validate values
            if (config->has("neuron_count")) {
                float neuronCount = 0.0f;
                config->get("neuron_count", neuronCount);
                if (neuronCount <= 0 || neuronCount > 1000000) {
                    return false;
                }
            }
            
            if (config->has("region_count")) {
                float regionCount = 0.0f;
                config->get("region_count", regionCount);
                if (regionCount <= 0 || regionCount > 100) {
                    return false;
                }
            }
            
            if (config->has("connection_probability")) {
                float prob = 0.0f;
                config->get("connection_probability", prob);
                if (prob < 0.0f || prob > 1.0f) {
                    return false;
                }
            }
            
            return true;
        }
        
        static json::json toJSON(const std::shared_ptr<Config>& config) {
            json::json j;
            
            if (config) {
                auto keys = config->getKeys();
                for (const auto& key : keys) {
                    if (config->has(key)) {
                        // Get value based on type
                        try {
                            float floatVal = 0.0f;
                            if (config->get(key, floatVal)) {
                                j[key] = floatVal;
                            }
                        } catch (...) {
                            try {
                                bool boolVal = false;
                                if (config->get(key, boolVal)) {
                                    j[key] = boolVal;
                                }
                            } catch (...) {
                                try {
                                    std::string strVal;
                                    if (config->get(key, strVal)) {
                                        j[key] = strVal;
                                    }
                                } catch (...) {
                                    // Skip unknown types
                                }
                            }
                        }
                    }
                }
            }
            
            return j;
        }
        
        static std::shared_ptr<Config> fromJSON(const json::json& j) {
            auto cfg = std::make_shared<Config>();
            
            if (j.is_object()) {
                for (auto it = j.begin(); it != j.end(); ++it) {
                    const std::string& key = it.key();
                    
                    if (it.value().is_number()) {
                        cfg->set(key, it.value().get<float>());
                    } else if (it.value().is_boolean()) {
                        cfg->set(key, it.value().get<bool>());
                    } else if (it.value().is_string()) {
                        cfg->set(key, it.value().get<std::string>());
                    } else if (it.value().is_array()) {
                        std::vector<float> vec;
                        for (const auto& item : it.value()) {
                            vec.push_back(item.get<float>());
                        }
                        cfg->set(key, vec);
                    }
                }
            }
            
            return cfg;
        }
        
    private:
        std::shared_ptr<Config> cfg_;
    };
}

// Simulation managers
namespace managers {
    class SimulationManager {
    public:
        struct SimulationResult {
            size_t episode;
            float totalReward;
            float avgFiringRate;
            size_t stepsExecuted;
            double wallTime;
            bool success;
            std::string errorMessage;
        };
        
        SimulationManager() : running_(false), currentEpisode_(0) {}
        
        void setConfig(const std::shared_ptr<Config>& config) {
            config_ = config;
        }
        
        void setBrain(const std::shared_ptr<Brain>& brain) {
            brain_ = brain;
        }
        
        void setWorld(const std::shared_ptr<SimpleWorld>& world) {
            world_ = world;
        }
        
        void setAgent(const std::shared_ptr<AgentBrain>& agent) {
            agent_ = agent;
        }
        
        SimulationResult runSingleEpisode(size_t maxSteps) {
            SimulationResult result;
            result.episode = currentEpisode_;
            result.success = false;
            result.errorMessage = "";
            
            try {
                auto startTime = std::chrono::high_resolution_clock::now();
                
                float totalReward = 0.0f;
                float totalFiringRate = 0.0f;
                size_t firingCount = 0;
                
                for (size_t step = 0; step < maxSteps; ++step) {
                    // Get observation
                    const SensoryPercept& percept = world_->getSensoryPercept();
                    agent_->processSensoryInput(percept);
                    
                    // Brain step
                    brain_->step(step, step * 0.001);
                    
                    // Get motor command
                    MotorCommand cmd = agent_->decodeMotorCommand();
                    
                    // Apply action
                    world_->applyMotorCommand(cmd, static_cast<double>(step) * 0.001);
                    
                    // Compute reward
                    float reward = agent_->getNeuromodulationLevel();
                    totalReward += reward;
                    
                    // Apply reward modulation
                    agent_->applyRewardModulation(reward, 0.0f);
                    
                    // Update development
                    if (agent_->isDevelopmentEnabled()) {
                        agent_->updateDevelopment(0.001);
                    }
                    
                    // Collect metrics
                    totalFiringRate += brain_->getAverageFiringRate();
                    if (brain_->getFiringNeuronCount() > 0) firingCount++;
                }
                
                auto endTime = std::chrono::high_resolution_clock::now();
                result.wallTime = std::chrono::duration<double>(endTime - startTime).count();
                result.totalReward = totalReward / maxSteps;
                result.avgFiringRate = totalFiringRate / maxSteps;
                result.stepsExecuted = maxSteps;
                result.success = true;
                
                currentEpisode_++;
                
            } catch (const std::exception& e) {
                result.errorMessage = e.what();
                result.success = false;
            }
            
            return result;
        }
        
        std::vector<SimulationResult> runBatchEpisodes(size_t numEpisodes, size_t stepsPerEpisode) {
            std::vector<SimulationResult> results;
            
            for (size_t i = 0; i < numEpisodes; ++i) {
                results.push_back(runSingleEpisode(stepsPerEpisode));
            }
            
            return results;
        }
        
        double runBenchmark(size_t steps) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Run simulation
            auto result = runSingleEpisode(steps);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double wallTime = std::chrono::duration<double>(endTime - startTime).count();
            
            return wallTime;
        }
        
        bool isRunning() const {
            return running_;
        }
        
    private:
        std::shared_ptr<Config> config_;
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<SimpleWorld> world_;
        std::shared_ptr<AgentBrain> agent_;
        bool running_;
        size_t currentEpisode_;
    };
    
    class ExperimentManager {
    public:
        struct ExperimentResult {
            std::string experimentId;
            std::string experimentType;
            std::vector<SimulationManager::SimulationResult> episodeResults;
            double totalTime;
            size_t totalSteps;
            float totalReward;
            std::string status;
            std::string errorMessage;
        };
        
        ExperimentManager() : nextExperimentId_(0) {}
        
        std::string runExperiment(const std::string& experimentType, 
                                  size_t numEpisodes,
                                  size_t stepsPerEpisode,
                                  const json::json& parameters = json::json()) {
            ExperimentResult result;
            result.experimentId = "exp_" + std::to_string(nextExperimentId_++);
            result.experimentType = experimentType;
            result.status = "running";
            
            try {
                auto startTime = std::chrono::high_resolution_clock::now();
                
                // Create configuration based on experiment type
                auto cfg = config::ConfigBuilder::buildExperimentConfig(experimentType);
                
                // Override with custom parameters
                for (auto it = parameters.begin(); it != parameters.end(); ++it) {
                    const std::string& key = it.key();
                    if (it.value().is_number()) {
                        cfg->set(key, it.value().get<float>());
                    } else if (it.value().is_boolean()) {
                        cfg->set(key, it.value().get<bool>());
                    } else if (it.value().is_string()) {
                        cfg->set(key, it.value().get<std::string>());
                    }
                }
                
                // Create brain, world, and agent
                auto brain = std::make_shared<Brain>(cfg);
                auto world = std::make_shared<SimpleWorld>();
                auto agent = std::make_shared<AgentBrain>(brain);
                
                // Configure world
                world->configure(100.0f, 100.0f, 10.0f, 10.0f);
                world->setAgentStart(50.0f, 50.0f);
                
                // Initialize simulation manager
                SimulationManager simManager;
                simManager.setConfig(cfg);
                simManager.setBrain(brain);
                simManager.setWorld(world);
                simManager.setAgent(agent);
                
                // Run episodes
                auto episodeResults = simManager.runBatchEpisodes(numEpisodes, stepsPerEpisode);
                
                // Aggregate results
                result.episodeResults = episodeResults;
                result.totalSteps = numEpisodes * stepsPerEpisode;
                result.totalReward = 0.0f;
                
                for (const auto& episodeResult : episodeResults) {
                    result.totalReward += episodeResult.totalReward;
                }
                
                auto endTime = std::chrono::high_resolution_clock::now();
                result.totalTime = std::chrono::duration<double>(endTime - startTime).count();
                
                result.status = "completed";
                
            } catch (const std::exception& e) {
                result.status = "failed";
                result.errorMessage = e.what();
            }
            
            return result.experimentId;
        }
        
        ExperimentResult getExperimentResult(const std::string& experimentId) {
            // Implementation would retrieve from storage
            ExperimentResult result;
            result.experimentId = experimentId;
            result.status = "not_found";
            return result;
        }
        
    private:
        size_t nextExperimentId_;
    };
    
    class BenchmarkManager {
    public:
        struct BenchmarkResult {
            std::string benchmarkId;
            std::string benchmarkType;
            double executionTime;
            size_t stepsExecuted;
            size_t memoryUsed;
            float throughput; // steps per second
            bool success;
            std::string errorMessage;
        };
        
        BenchmarkResult runBenchmark(const std::string& benchmarkType, 
                                     size_t steps,
                                     const std::shared_ptr<Config>& config = nullptr) {
            BenchmarkResult result;
            result.benchmarkId = "bench_" + std::to_string(nextBenchmarkId_++);
            result.benchmarkType = benchmarkType;
            result.success = false;
            
            try {
                auto startTime = std::chrono::high_resolution_clock::now();
                
                // Create configuration if not provided
                std::shared_ptr<Config> cfg = config;
                if (!cfg) {
                    cfg = config::ConfigBuilder::buildDefaultConfig();
                }
                
                // Create brain, world, and agent
                auto brain = std::make_shared<Brain>(cfg);
                auto world = std::make_shared<SimpleWorld>();
                auto agent = std::make_shared<AgentBrain>(brain);
                
                // Configure world
                world->configure(100.0f, 100.0f, 10.0f, 10.0f);
                world->setAgentStart(50.0f, 50.0f);
                
                // Run simulation
                SimulationManager simManager;
                simManager.setConfig(cfg);
                simManager.setBrain(brain);
                simManager.setWorld(world);
                simManager.setAgent(agent);
                
                double wallTime = simManager.runBenchmark(steps);
                
                auto endTime = std::chrono::high_resolution_clock::now();
                result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
                result.stepsExecuted = steps;
                result.throughput = static_cast<float>(steps) / result.executionTime;
                
                // Get memory usage
                auto memUsage = memory::getMemoryUsage();
                result.memoryUsed = memUsage.residentBytes;
                
                result.success = true;
                
            } catch (const std::exception& e) {
                result.errorMessage = e.what();
            }
            
            return result;
        }
        
        std::vector<BenchmarkResult> runBenchmarkSuite(const std::vector<std::string>& benchmarkTypes,
                                                       size_t steps,
                                                       const std::shared_ptr<Config>& config = nullptr) {
            std::vector<BenchmarkResult> results;
            
            for (const auto& benchmarkType : benchmarkTypes) {
                results.push_back(runBenchmark(benchmarkType, steps, config));
            }
            
            return results;
        }
        
    private:
        size_t nextBenchmarkId_ = 0;
    };
    
    // Parallel processing utilities
    namespace parallel {
        template<typename T, typename U>
        U parallelMap(const std::vector<T>& input, std::function<U(T)> func, size_t numThreads = 4) {
            if (input.empty()) {
                return U();
            }
            
            size_t chunkSize = input.size() / numThreads;
            if (chunkSize == 0) chunkSize = 1;
            
            std::vector<std::thread> threads;
            std::vector<U> results(numThreads);
            
            for (size_t i = 0; i < numThreads; ++i) {
                size_t start = i * chunkSize;
                size_t end = (i == numThreads - 1) ? input.size() : start + chunkSize;
                
                threads.emplace_back([&, start, end, i]() {
                    U result;
                    for (size_t j = start; j < end; ++j) {
                        if (j == start) {
                            result = func(input[j]);
                        } else {
                            result = result + func(input[j]);
                        }
                    }
                    results[i] = result;
                });
            }
            
            for (auto& thread : threads) {
                thread.join();
            }
            
            // Combine results
            U finalResult;
            for (const auto& result : results) {
                finalResult = finalResult + result;
            }
            
            return finalResult;
        }
    }
}

// Export/Import tools
namespace export {
    py::object exportResults(const std::vector<managers::SimulationManager::SimulationResult>& results, 
                            const std::string& filepath = "", 
                            const std::string& format = "json") {
        json::json j;
        j["results"] = json::json::array();
        
        for (const auto& result : results) {
            json::json episodeJson;
            episodeJson["episode"] = result.episode;
            episodeJson["total_reward"] = result.totalReward;
            episodeJson["avg_firing_rate"] = result.avgFiringRate;
            episodeJson["steps_executed"] = result.stepsExecuted;
            episodeJson["wall_time"] = result.wallTime;
            episodeJson["success"] = result.success;
            if (!result.errorMessage.empty()) {
                episodeJson["error_message"] = result.errorMessage;
            }
            
            j["results"].push_back(episodeJson);
        }
        
        if (!filepath.empty()) {
            std::ofstream file(filepath);
            if (file.is_open()) {
                file << j.dump(2);
                file.close();
            } else {
                throw KiloRuntimeError("Failed to open file for writing: " + filepath);
            }
        }
        
        return py::cast(j);
    }
    
    py::object importResults(const std::string& filepath = "") {
        if (filepath.empty()) {
            return py::cast(json::json::object());
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw KiloRuntimeError("Failed to open file for reading: " + filepath);
        }
        
        json::json j;
        try {
            file >> j;
        } catch (const json::json::parse_error& e) {
            throw KiloRuntimeError("Failed to parse JSON file: " + std::string(e.what()));
        }
        
        return py::cast(j);
    }
    
    py::object exportBrain(const std::shared_ptr<Brain>& brain, const std::string& filepath = "", 
                          const std::string& format = "json") {
        if (!brain) {
            throw KiloValidationError("Brain pointer is null");
        }
        
        json::json j;
        
        // Export brain properties
        j["brain_type"] = "NLM_Brain";
        j["total_neurons"] = brain->getTotalNeuronCount();
        j["total_synapses"] = brain->getTotalSynapseCount();
        j["active_neurons"] = brain->getActiveNeuronCount();
        j["firing_neurons"] = brain->getFiringNeuronCount();
        j["average_firing_rate"] = brain->getAverageFiringRate();
        j["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        j["total_spikes"] = brain->getTotalSpikeCount();
        j["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // Export brain configuration
        if (auto cfg = brain->getConfig()) {
            j["config"] = config::ConfigBuilder::toJSON(cfg);
        }
        
        // Save to file if specified
        if (!filepath.empty()) {
            std::ofstream file(filepath);
            if (file.is_open()) {
                file << j.dump(2);
                file.close();
            } else {
                throw KiloRuntimeError("Failed to open file for writing: " + filepath);
            }
        }
        
        return py::cast(j);
    }
    
    std::shared_ptr<Brain> importBrain(const std::string& filepath = "") {
        if (filepath.empty()) {
            throw KiloValidationError("Filepath cannot be empty");
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw KiloRuntimeError("Failed to open file for reading: " + filepath);
        }
        
        json::json j;
        try {
            file >> j;
        } catch (const json::json::parse_error& e) {
            throw KiloRuntimeError("Failed to parse brain JSON file: " + std::string(e.what()));
        }
        
        // Create configuration from JSON
        auto cfg = config::ConfigBuilder::fromJSON(j.value("config", json::json::object()));
        
        // Create brain
        auto brain = std::make_shared<Brain>(cfg);
        
        // TODO: Implement actual brain loading from JSON
        // For now, just return the configured brain
        
        return brain;
    }
    
    py::object exportMetrics(const std::shared_ptr<Brain>& brain, 
                           const std::shared_ptr<SimpleWorld>& world,
                           const std::shared_ptr<AgentBrain>& agent,
                           const std::string& filepath = "") {
        json::json j;
        
        // Export brain metrics
        if (brain) {
            j["brain"] = managers::SimulationManager::SimulationResult{
                brain->getTotalNeuronCount(),
                brain->getAverageFiringRate(),
                0, // steps (not applicable here)
                0.0, // wallTime (not applicable here)
                false // success
            };
        }
        
        // Export world metrics
        if (world) {
            j["world"] = json::json::object();
            j["world"]["width"] = world->getWidth();
            j["world"]["height"] = world->getHeight();
            j["world"]["agent_x"] = world->getAgentBody().x;
            j["world"]["agent_y"] = world->getAgentBody().y;
            j["world"]["agent_orientation"] = world->getAgentBody().orientation;
            j["world"]["agent_energy"] = world->getAgentBody().energy;
            j["world"]["agent_health"] = world->getAgentBody().health;
            j["world"]["simulation_time"] = world->getSimulationTime();
        }
        
        // Export agent metrics
        if (agent) {
            j["agent"] = json::json::object();
            j["agent"]["development_enabled"] = agent->isDevelopmentEnabled();
            j["agent"]["curiosity_enabled"] = agent->isCuriosityEnabled();
            j["agent"]["reward_modulation_enabled"] = agent->isRewardModulationEnabled();
            j["agent"]["neuromodulation_level"] = agent->getNeuromodulationLevel();
            j["agent"]["curiosity_level"] = agent->getCuriosityLevel();
            j["agent"]["novelty_level"] = agent->getNoveltyLevel();
            j["agent"]["prediction_error"] = agent->getPredictionError();
        }
        
        // Save to file if specified
        if (!filepath.empty()) {
            std::ofstream file(filepath);
            if (file.is_open()) {
                file << j.dump(2);
                file.close();
            } else {
                throw KiloRuntimeError("Failed to open file for writing: " + filepath);
            }
        }
        
        return py::cast(j);
    }
}

// Visualization and analysis tools
namespace visualization {
    py::function createVisFunc(const std::shared_ptr<SimpleWorld>& world) {
        // Create a Python function that can be called to get visualization data
        return py::cpp_function([world]() {
            py::dict result;
            result["agent_x"] = world->getAgentBody().x;
            result["agent_y"] = world->getAgentBody().y;
            result["agent_orientation"] = world->getAgentBody().orientation;
            result["agent_energy"] = world->getAgentBody().energy;
            result["agent_health"] = world->getAgentBody().health;
            result["width"] = world->getWidth();
            result["height"] = world->getHeight();
            result["vision_width"] = world->getVisionWidth();
            result["vision_height"] = world->getVisionHeight();
            
            // Get vision data
            const SensoryPercept& percept = world->getSensoryPercept();
            result["vision"] = percept.getVision();
            result["touch"] = percept.getTouch();
            result["internal"] = percept.getInternal();
            result["proprioception"] = percept.getProprioception();
            
            // Get object data
            py::list objects;
            for (const auto& obj : world->getObjects()) {
                py::dict obj_dict;
                obj_dict["x"] = obj.x;
                obj_dict["y"] = obj.y;
                obj_dict["radius"] = obj.radius;
                obj_dict["type"] = static_cast<int>(obj.type);
                obj_dict["value"] = obj.value;
                obj_dict["active"] = obj.active;
                objects.append(obj_dict);
            }
            result["objects"] = objects;
            
            return result;
        }, py::arg("world"));
    }
    
    py::object renderWorld(const std::shared_ptr<SimpleWorld>& world, 
                          const std::string& format = "ascii") {
        if (format == "ascii") {
            std::ostringstream oss;
            oss << "=== World State ===\n";
            oss << "Width: " << world->getWidth() << " Height: " << world->getHeight() << "\n";
            oss << "Agent: x=" << world->getAgentBody().x 
                << " y=" << world->getAgentBody().y 
                << " orientation=" << world->getAgentBody().orientation << "\n";
            oss << "Energy: " << world->getAgentBody().energy 
                << " Health: " << world->getAgentBody().health << "\n";
            oss << "Simulation Time: " << world->getSimulationTime() << "\n";
            
            // Count objects by type
            std::map<WorldObjectType, int> typeCounts;
            for (const auto& obj : world->getObjects()) {
                typeCounts[obj.type]++;
            }
            
            oss << "\nObjects by type:\n";
            for (const auto& pair : typeCounts) {
                oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
            }
            
            return py::str(oss.str());
        } else {
            throw KiloRuntimeError("Unsupported render format: " + format);
        }
    }
    
    py::object plotMetrics(const json::json& metrics, const std::string& plotType = "line") {
        // Implementation would generate plots
        // For now, return a summary string
        std::ostringstream oss;
        oss << "Plot type: " << plotType << "\n";
        oss << "Data points: " << metrics.size() << "\n";
        
        return py::str(oss.str());
    }
    
    py::object analyzeBehavior(const std::shared_ptr<Brain>& brain,
                              const std::shared_ptr<SimpleWorld>& world,
                              const std::shared_ptr<AgentBrain>& agent,
                              const std::string& analysisType = "basic") {
        py::dict result;
        
        if (analysisType == "basic") {
            result["brain_statistics"] = json::json::object();
            if (brain) {
                result["brain_statistics"]["total_neurons"] = brain->getTotalNeuronCount();
                result["brain_statistics"]["total_synapses"] = brain->getTotalSynapseCount();
                result["brain_statistics"]["active_neurons"] = brain->getActiveNeuronCount();
                result["brain_statistics"]["firing_neurons"] = brain->getFiringNeuronCount();
                result["brain_statistics"]["average_firing_rate"] = brain->getAverageFiringRate();
            }
            
            result["agent_statistics"] = json::json::object();
            if (agent) {
                result["agent_statistics"]["development_enabled"] = agent->isDevelopmentEnabled();
                result["agent_statistics"]["curiosity_enabled"] = agent->isCuriosityEnabled();
                result["agent_statistics"]["reward_modulation_enabled"] = agent->isRewardModulationEnabled();
                result["agent_statistics"]["neuromodulation_level"] = agent->getNeuromodulationLevel();
                result["agent_statistics"]["curiosity_level"] = agent->getCuriosityLevel();
                result["agent_statistics"]["novelty_level"] = agent->getNoveltyLevel();
                result["agent_statistics"]["prediction_error"] = agent->getPredictionError();
            }
            
            result["world_state"] = json::json::object();
            if (world) {
                result["world_state"]["width"] = world->getWidth();
                result["world_state"]["height"] = world->getHeight();
                result["world_state"]["agent_x"] = world->getAgentBody().x;
                result["world_state"]["agent_y"] = world->getAgentBody().y;
                result["world_state"]["agent_orientation"] = world->getAgentBody().orientation;
                result["world_state"]["agent_energy"] = world->getAgentBody().energy;
                result["world_state"]["agent_health"] = world->getAgentBody().health;
                result["world_state"]["simulation_time"] = world->getSimulationTime();
            }
            
        } else if (analysisType == "advanced") {
            // Advanced analysis would include more complex metrics
            result["analysis_type"] = "advanced";
            result["note"] = "Advanced analysis implementation needed";
        }
        
        return py::cast(result);
    }
}

// Enhanced error and exception handling
namespace error {
    class ErrorHandler {
    public:
        enum class ErrorLevel {
            DEBUG,
            INFO,
            WARNING,
            ERROR,
            CRITICAL
        };
        
        struct ErrorContext {
            std::string message;
            ErrorLevel level;
            std::string source;
            std::chrono::high_resolution_clock::time_point timestamp;
            std::string recoveryAction;
        };
        
        ErrorHandler() : errorCount_(0), lastErrorLevel_(ErrorLevel::DEBUG) {}
        
        void logError(const std::string& message, ErrorLevel level = ErrorLevel::ERROR, 
                     const std::string& source = "") {
            std::lock_guard<std::mutex> lock(mutex_);
            
            ErrorContext context;
            context.message = message;
            context.level = level;
            context.source = source;
            context.timestamp = std::chrono::high_resolution_clock::now();
            
            // Determine recovery action
            context.recoveryAction = determineRecoveryAction(message, level);
            
            // Log the error
            errorLog_.push_back(context);
            errorCount_++;
            lastErrorLevel_ = level;
            
            // Print to stderr for immediate visibility
            std::cerr << "[" << errorLevelToString(level) << "] " << message;
            if (!source.empty()) {
                std::cerr << " (from " << source << ")";
            }
            std::cerr << std::endl;
            
            // Handle based on level
            if (level >= ErrorLevel::ERROR) {
                handleError(context);
            }
        }
        
        std::vector<ErrorContext> getErrorLog() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return errorLog_;
        }
        
        size_t getErrorCount() const {
            return errorCount_;
        }
        
        ErrorLevel getLastErrorLevel() const {
            return lastErrorLevel_;
        }
        
        void clearErrorLog() {
            std::lock_guard<std::mutex> lock(mutex_);
            errorLog_.clear();
        }
        
        std::string getErrorSummary() const {
            std::lock_guard<std::mutex> lock(mutex_);
            
            std::map<ErrorLevel, size_t> levelCounts;
            for (const auto& context : errorLog_) {
                levelCounts[context.level]++;
            }
            
            std::ostringstream oss;
            oss << "Error Summary:" << std::endl;
            oss << "  Total errors: " << errorCount_ << std::endl;
            for (const auto& pair : levelCounts) {
                oss << "  " << errorLevelToString(pair.first) << ": " << pair.second << std::endl;
            }
            
            return oss.str();
        }
        
        py::object getErrorsByLevel(ErrorLevel level) const {
            std::lock_guard<std::mutex> lock(mutex_);
            
            py::list pyErrors;
            for (const auto& context : errorLog_) {
                if (context.level == level) {
                    py::dict pyError;
                    pyError["message"] = context.message;
                    pyError["source"] = context.source;
                    pyError["timestamp"] = std::chrono::duration<double>(
                        context.timestamp.time_since_epoch()).count();
                    pyError["recovery_action"] = context.recoveryAction;
                    pyErrors.append(pyError);
                }
            }
            
            return pyErrors;
        }
        
    private:
        std::vector<ErrorContext> errorLog_;
        mutable std::mutex mutex_;
        size_t errorCount_;
        ErrorLevel lastErrorLevel_;
        
        std::string errorLevelToString(ErrorLevel level) {
            switch (level) {
                case ErrorLevel::DEBUG: return "DEBUG";
                case ErrorLevel::INFO: return "INFO";
                case ErrorLevel::WARNING: return "WARNING";
                case ErrorLevel::ERROR: return "ERROR";
                case ErrorLevel::CRITICAL: return "CRITICAL";
                default: return "UNKNOWN";
            }
        }
        
        std::string determineRecoveryAction(const std::string& message, ErrorLevel level) {
            if (level == ErrorLevel::CRITICAL) {
                return "restart_simulation";
            } else if (level == ErrorLevel::ERROR) {
                if (message.find("memory") != std::string::npos) {
                    return "reduce_memory_usage";
                } else if (message.find("timeout") != std::string::npos) {
                    return "reduce_simulation_speed";
                } else {
                    return "try_alternative_approach";
                }
            } else if (level == ErrorLevel::WARNING) {
                return "continue_with_caution";
            }
            return "none";
        }
        
        void handleError(const ErrorContext& context) {
            // Handle errors based on level and recovery action
            if (context.level == ErrorLevel::CRITICAL) {
                // Critical errors might require simulation restart
                // Implementation would depend on the context
            }
            
            // Additional error handling logic would go here
        }
    };
    
    // Global error handler instance
    ErrorHandler globalErrorHandler;
    
    // Convenience functions
    void logError(const std::string& message, error::ErrorHandler::ErrorLevel level = error::ErrorHandler::ErrorLevel::ERROR,
                  const std::string& source = "") {
        globalErrorHandler.logError(message, level, source);
    }
    
    void throwKiloError(const std::string& message, error::ErrorHandler::ErrorLevel level = error::ErrorHandler::ErrorLevel::ERROR) {
        if (level >= error::ErrorHandler::ErrorLevel::ERROR) {
            throw KiloRuntimeError(message);
        }
    }
    
    bool gracefulDegradation(const std::function<bool()>& featureCheck, const std::string& featureName) {
        if (!featureCheck()) {
            logError("Feature not available: " + featureName, error::ErrorHandler::ErrorLevel::WARNING, "graceful_degradation");
            return false;
        }
        return true;
    }
}

// Advanced configuration options
namespace advanced_config {
    class AdvancedConfig {
    public:
        enum class OptimizationLevel {
            BASIC,
            STANDARD,
            ADVANCED,
            EXPERT
        };
        
        struct PerformanceProfile {
            OptimizationLevel optimizationLevel;
            bool enableSIMD;
            bool enableMultithreading;
            size_t memoryPoolSize;
            double maxCPUTime;
            size_t maxMemoryUsage;
            size_t maxThreads;
        };
        
        AdvancedConfig() : optimizationLevel_(OptimizationLevel::STANDARD) {
            simdEnabled_ = true;
            multithreadingEnabled_ = true;
            memoryPoolSize_ = 64 * 1024 * 1024; // 64MB default
            maxCPUTime_ = 1000.0; // 1 second
            maxMemoryUsage_ = 512 * 1024 * 1024; // 512MB
            maxThreads_ = std::thread::hardware_concurrency();
        }
        
        void setOptimizationLevel(OptimizationLevel level) {
            optimizationLevel_ = level;
        }
        
        OptimizationLevel getOptimizationLevel() const {
            return optimizationLevel_;
        }
        
        void enableSIMD(bool enable) {
            simdEnabled_ = enable;
        }
        
        bool isSIMDEnabled() const {
            return simdEnabled_;
        }
        
        void enableMultithreading(bool enable) {
            multithreadingEnabled_ = enable;
        }
        
        bool isMultithreadingEnabled() const {
            return multithreadingEnabled_;
        }
        
        void setMemoryPoolSize(size_t size) {
            memoryPoolSize_ = size;
        }
        
        size_t getMemoryPoolSize() const {
            return memoryPoolSize_;
        }
        
        void setMaxCPUTime(double time) {
            maxCPUTime_ = time;
        }
        
        double getMaxCPUTime() const {
            return maxCPUTime_;
        }
        
        void setMaxMemoryUsage(size_t size) {
            maxMemoryUsage_ = size;
        }
        
        size_t getMaxMemoryUsage() const {
            return maxMemoryUsage_;
        }
        
        void setMaxThreads(size_t threads) {
            maxThreads_ = threads;
        }
        
        size_t getMaxThreads() const {
            return maxThreads_;
        }
        
        PerformanceProfile getPerformanceProfile() const {
            PerformanceProfile profile;
            profile.optimizationLevel = optimizationLevel_;
            profile.enableSIMD = simdEnabled_;
            profile.enableMultithreading = multithreadingEnabled_;
            profile.memoryPoolSize = memoryPoolSize_;
            profile.maxCPUTime = maxCPUTime_;
            profile.maxMemoryUsage = maxMemoryUsage_;
            profile.maxThreads = maxThreads_;
            return profile;
        }
        
        void configurePerformanceProfile(const PerformanceProfile& profile) {
            optimizationLevel_ = profile.optimizationLevel;
            simdEnabled_ = profile.enableSIMD;
            multithreadingEnabled_ = profile.enableMultithreading;
            memoryPoolSize_ = profile.memoryPoolSize;
            maxCPUTime_ = profile.maxCPUTime;
            maxMemoryUsage_ = profile.maxMemoryUsage;
            maxThreads_ = profile.maxThreads;
        }
        
        void applyToConfig(const std::shared_ptr<Config>& config) {
            if (!config) return;
            
            // Convert optimization level to config values
            switch (optimizationLevel_) {
                case OptimizationLevel::BASIC:
                    config->set("optimization_level", "basic");
                    config->set("thread_count", 1);
                    config->set("memory_pool_size", 8 * 1024 * 1024);
                    break;
                case OptimizationLevel::STANDARD:
                    config->set("optimization_level", "standard");
                    config->set("thread_count", std::thread::hardware_concurrency());
                    config->set("memory_pool_size", memoryPoolSize_);
                    break;
                case OptimizationLevel::ADVANCED:
                    config->set("optimization_level", "advanced");
                    config->set("thread_count", std::thread::hardware_concurrency() * 2);
                    config->set("memory_pool_size", memoryPoolSize_ * 2);
                    break;
                case OptimizationLevel::EXPERT:
                    config->set("optimization_level", "expert");
                    config->set("thread_count", std::thread::hardware_concurrency() * 4);
                    config->set("memory_pool_size", memoryPoolSize_ * 4);
                    break;
            }
            
            config->set("simd_enabled", simdEnabled_);
            config->set("multithreading_enabled", multithreadingEnabled_);
            config->set("max_cpu_time", maxCPUTime_);
            config->set("max_memory_usage", maxMemoryUsage_);
        }
        
    private:
        OptimizationLevel optimizationLevel_;
        bool simdEnabled_;
        bool multithreadingEnabled_;
        size_t memoryPoolSize_;
        double maxCPUTime_;
        size_t maxMemoryUsage_;
        size_t maxThreads_;
    };
    
    // Global advanced config instance
    AdvancedConfig globalAdvancedConfig;
    
    // Convenience functions
    void setOptimizationLevel(advanced_config::AdvancedConfig::OptimizationLevel level) {
        globalAdvancedConfig.setOptimizationLevel(level);
    }
    
    void enableSIMD(bool enable) {
        globalAdvancedConfig.enableSIMD(enable);
    }
    
    void enableMultithreading(bool enable) {
        globalAdvancedConfig.enableMultithreading(enable);
    }
    
    void setMemoryPoolSize(size_t size) {
        globalAdvancedConfig.setMemoryPoolSize(size);
    }
    
    void configurePerformanceProfile(const advanced_config::AdvancedConfig::PerformanceProfile& profile) {
        globalAdvancedConfig.configurePerformanceProfile(profile);
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        ------------------------------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive classes and functions for neural simulation,
        learning, and experimentation.
        
        Key improvements:
        - Added missing methods: observe(), applyAction(), computeReward()
        - Improved error handling and validation
        - Type checking and input validation
        - Pythonic error exceptions
        - Comprehensive docstrings
        - Better memory management
        - Batch operations for performance
        - Configuration builder helpers
        - Simulation helper functions
        - Visualization utilities
        - Performance monitoring
        - Checkpointing utilities
        - Enhanced API with properties and context managers
        - Added comprehensive performance monitoring infrastructure
        - Added advanced configuration builder
        - Added simulation managers (SimulationManager, ExperimentManager, BenchmarkManager)
        - Added export/import tools (exportResults, importResults, exportBrain, importBrain, exportMetrics)
        - Added visualization and analysis tools (plotMetrics, analyzeBehavior)
        - Added enhanced error and exception handling with recovery mechanisms
        - Added advanced configuration options (setOptimizationLevel, enableSIMD, enableMultithreading, setMemoryPoolSize, configurePerformanceProfile)
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        if (p) {
            try {
                std::rethrow_exception(p);
            } catch (const KiloRuntimeError& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
            } catch (const KiloValidationError& e) {
                PyErr_SetString(PyExc_ValueError, e.what());
            } catch (const KiloConfigurationError& e) {
                PyErr_SetString(PyExc_TypeError, e.what());
            } catch (const KiloNotImplementedError& e) {
                PyErr_SetString(PyExc_NotImplementedError, e.what());
            } catch (const KiloResourceError& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
            } catch (const KiloExperimentError& e) {
                PyErr_SetString(PyExc_ValueError, e.what());
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
            }
        }
    });

    // ID types
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    // Enumerations
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Performance monitoring
    py::class_<performance::Profiler>(m, "Profiler", R"pbdoc(Performance profiler for timing operations)pbdoc")
        .def(py::init<>())
        .def("startProfile", &performance::Profiler::startProfile, py::arg("name"),
             "Start profiling an operation")
        .def("stopProfile", &performance::Profiler::stopProfile, py::arg("name"),
             "Stop profiling an operation and return duration")
        .def("getProfiles", &performance::Profiler::getProfiles,
             "Get all profiling data")
        .def("recordMemoryAllocation", &performance::Profiler::recordMemoryAllocation, py::arg("bytes"),
             "Record memory allocation");

    py::class_<performance::MemoryProfiler>(m, "MemoryProfiler", R"pbdoc(Memory usage tracker)pbdoc")
        .def(py::init<>())
        .def("startRecording", &performance::MemoryProfiler::startRecording, py::arg("operation") = "default",
             "Start recording memory usage")
        .def("recordSample", &performance::MemoryProfiler::recordSample,
             "Record current memory usage sample")
        .def("stopRecording", &performance::MemoryProfiler::stopRecording,
             "Stop recording memory usage")
        .def("getSamples", &performance::MemoryProfiler::getSamples,
             "Get memory samples")
        .def("getPeakMemory", &performance::MemoryProfiler::getPeakMemory,
             "Get peak memory usage");

    py::class_<performance::PerformanceStats>(m, "PerformanceStats", R"pbdoc(Performance statistics)pbdoc")
        .def(py::init<>())
        .def_readwrite("total_wall_time", &performance::PerformanceStats::totalWallTime)
        .def_readwrite("simulation_steps", &performance::PerformanceStats::simulationSteps)
        .def_readwrite("steps_per_second", &performance::PerformanceStats::stepsPerSecond)
        .def_readwrite("memory_usage", &performance::PerformanceStats::memoryUsage)
        .def_readwrite("cpu_usage", &performance::PerformanceStats::cpuUsage)
        .def_readwrite("active_threads", &performance::PerformanceStats::activeThreads)
        .def_readwrite("current_memory", &performance::PerformanceStats::currentMemory);

    m.def("enableProfiling", &performance::enableProfiling, py::arg("callback"), "Enable performance profiling");
    m.def("getPerformanceStats", &performance::getPerformanceStats, "Get current performance statistics");

    // Configuration builder
    py::class_<config::ConfigBuilder>(m, "ConfigBuilder", R"pbdoc(Builder for complex configurations)pbdoc")
        .def(py::init<>())
        .def("set", [](config::ConfigBuilder& self, const std::string& key, const py::object& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("set", [](config::ConfigBuilder& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("set", [](config::ConfigBuilder& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("set", [](config::ConfigBuilder& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("set", [](config::ConfigBuilder& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("set", [](config::ConfigBuilder& self, const std::string& key, const std::vector<float>& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("build", &config::ConfigBuilder::build, "Build the configuration")
        .def("buildToPython", &config::ConfigBuilder::buildToPython, "Build the configuration and return as Python object")
        .def_static("buildFromDict", &config::ConfigBuilder::buildFromDict,
                    py::arg("config"), "Build configuration from Python dictionary")
        .def_static("buildDefaultConfig", &config::ConfigBuilder::buildDefaultConfig,
                    "Build a default configuration")
        .def_static("buildExperimentConfig", &config::ConfigBuilder::buildExperimentConfig,
                    py::arg("experiment_type"), "Build configuration for an experiment type")
        .def_static("validateConfig", &config::ConfigBuilder::validateConfig,
                    py::arg("config"), "Validate a configuration")
        .def_static("toJSON", &config::ConfigBuilder::toJSON,
                    py::arg("config"), "Convert configuration to JSON")
        .def_static("fromJSON", &config::ConfigBuilder::fromJSON,
                    py::arg("json"), "Create configuration from JSON");

    // Simulation managers
    py::class_<managers::SimulationManager>(m, "SimulationManager", R"pbdoc(Manager for batch processing simulations)pbdoc")
        .def(py::init<>())
        .def("setConfig", &managers::SimulationManager::setConfig, py::arg("config"),
             "Set the configuration")
        .def("setBrain", &managers::SimulationManager::setBrain, py::arg("brain"),
             "Set the brain")
        .def("setWorld", &managers::SimulationManager::setWorld, py::arg("world"),
             "Set the world")
        .def("setAgent", &managers::SimulationManager::setAgent, py::arg("agent"),
             "Set the agent")
        .def("runSingleEpisode", &managers::SimulationManager::runSingleEpisode,
             py::arg("maxSteps"), "Run a single simulation episode")
        .def("runBatchEpisodes", &managers::SimulationManager::runBatchEpisodes,
             py::arg("numEpisodes"), py::arg("stepsPerEpisode"), "Run multiple simulation episodes")
        .def("runBenchmark", &managers::SimulationManager::runBenchmark,
             py::arg("steps"), "Run a benchmark simulation");

    py::class_<managers::ExperimentManager>(m, "ExperimentManager", R"pbdoc(Manager for running experiment suites)pbdoc")
        .def(py::init<>())
        .def("runExperiment", &managers::ExperimentManager::runExperiment,
             py::arg("experiment_type"), py::arg("num_episodes"), py::arg("steps_per_episode"),
             py::arg("parameters") = json::json(), "Run an experiment")
        .def("getExperimentResult", &managers::ExperimentManager::getExperimentResult,
             py::arg("experiment_id"), "Get experiment result");

    py::class_<managers::BenchmarkManager>(m, "BenchmarkManager", R"pbdoc(Manager for performance benchmarking)pbdoc")
        .def(py::init<>())
        .def("runBenchmark", &managers::BenchmarkManager::runBenchmark,
             py::arg("benchmark_type"), py::arg("steps"), py::arg("config") = nullptr,
             "Run a benchmark")
        .def("runBenchmarkSuite", &managers::BenchmarkManager::runBenchmarkSuite,
             py::arg("benchmark_types"), py::arg("steps"), py::arg("config") = nullptr,
             "Run a suite of benchmarks");

    // Export/Import tools
    m.def("exportResults", &export::exportResults,
          py::arg("results"), py::arg("filepath") = "", py::arg("format") = "json",
          "Export simulation results to file or Python object");
    m.def("importResults", &export::importResults,
          py::arg("filepath") = "", "Import simulation results from file");
    m.def("exportBrain", &export::exportBrain,
          py::arg("brain"), py::arg("filepath") = "", py::arg("format") = "json",
          "Export brain state to file or Python object");
    m.def("importBrain", &export::importBrain,
          py::arg("filepath") = "", "Import brain state from file");
    m.def("exportMetrics", &export::exportMetrics,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("filepath") = "",
          "Export simulation metrics");

    // Visualization and analysis tools
    m.def("createVisFunc", &visualization::createVisFunc,
          py::arg("world"), "Create a Python function for world visualization");
    m.def("renderWorld", &visualization::renderWorld,
          py::arg("world"), py::arg("format") = "ascii", "Render world state as string or dictionary");
    m.def("plotMetrics", &visualization::plotMetrics,
          py::arg("metrics"), py::arg("plot_type") = "line", "Generate metric plots");
    m.def("analyzeBehavior", &visualization::analyzeBehavior,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("analysis_type") = "basic",
          "Analyze agent behavior");

    // Enhanced error and exception handling
    py::class_<error::ErrorHandler>(m, "ErrorHandler", R"pbdoc(Error handler with recovery mechanisms)pbdoc")
        .def(py::init<>())
        .def("logError", &error::ErrorHandler::logError, py::arg("message"), py::arg("level") = error::ErrorHandler::ErrorLevel::ERROR,
             py::arg("source") = "", "Log an error")
        .def("getErrorLog", &error::ErrorHandler::getErrorLog, "Get error log")
        .def("getErrorCount", &error::ErrorHandler::getErrorCount, "Get error count")
        .def("getLastErrorLevel", &error::ErrorHandler::getLastErrorLevel, "Get last error level")
        .def("clearErrorLog", &error::ErrorHandler::clearErrorLog, "Clear error log")
        .def("getErrorSummary", &error::ErrorHandler::getErrorSummary, "Get error summary")
        .def("getErrorsByLevel", &error::ErrorHandler::getErrorsByLevel, py::arg("level"), "Get errors by level");

    // Global error handler functions
    m.def("logError", &error::logError, py::arg("message"), py::arg("level") = error::ErrorHandler::ErrorLevel::ERROR,
          py::arg("source") = "", "Log an error globally");
    m.def("throwKiloError", &error::throwKiloError, py::arg("message"), py::arg("level") = error::ErrorHandler::ErrorLevel::ERROR,
          "Throw a Kilo error");
    m.def("gracefulDegradation", &error::gracefulDegradation, py::arg("feature_check"), py::arg("feature_name"),
          "Check if feature is available and gracefully degrade if not");

    // Advanced configuration options
    py::enum_<advanced_config::AdvancedConfig::OptimizationLevel>(m, "OptimizationLevel",
        R"pbdoc(Optimization level for performance tuning)pbdoc")
        .value("BASIC", advanced_config::AdvancedConfig::OptimizationLevel::BASIC)
        .value("STANDARD", advanced_config::AdvancedConfig::OptimizationLevel::STANDARD)
        .value("ADVANCED", advanced_config::AdvancedConfig::OptimizationLevel::ADVANCED)
        .value("EXPERT", advanced_config::AdvancedConfig::OptimizationLevel::EXPERT)
        .export_values();

    py::class_<advanced_config::AdvancedConfig>(m, "AdvancedConfig", R"pbdoc(Advanced configuration for performance tuning)pbdoc")
        .def(py::init<>())
        .def("setOptimizationLevel", &advanced_config::AdvancedConfig::setOptimizationLevel, py::arg("level"),
             "Set optimization level")
        .def("getOptimizationLevel", &advanced_config::AdvancedConfig::getOptimizationLevel,
             "Get optimization level")
        .def("enableSIMD", &advanced_config::AdvancedConfig::enableSIMD, py::arg("enable"),
             "Enable/disable SIMD optimization")
        .def("isSIMDEnabled", &advanced_config::AdvancedConfig::isSIMDEnabled,
             "Check if SIMD is enabled")
        .def("enableMultithreading", &advanced_config::AdvancedConfig::enableMultithreading, py::arg("enable"),
             "Enable/disable multithreading")
        .def("isMultithreadingEnabled", &advanced_config::AdvancedConfig::isMultithreadingEnabled,
             "Check if multithreading is enabled")
        .def("setMemoryPoolSize", &advanced_config::AdvancedConfig::setMemoryPoolSize, py::arg("size"),
             "Set memory pool size")
        .def("getMemoryPoolSize", &advanced_config::AdvancedConfig::getMemoryPoolSize,
             "Get memory pool size")
        .def("setMaxCPUTime", &advanced_config::AdvancedConfig::setMaxCPUTime, py::arg("time"),
             "Set maximum CPU time")
        .def("getMaxCPUTime", &advanced_config::AdvancedConfig::getMaxCPUTime,
             "Get maximum CPU time")
        .def("setMaxMemoryUsage", &advanced_config::AdvancedConfig::setMaxMemoryUsage, py::arg("size"),
             "Set maximum memory usage")
        .def("getMaxMemoryUsage", &advanced_config::AdvancedConfig::getMaxMemoryUsage,
             "Get maximum memory usage")
        .def("setMaxThreads", &advanced_config::AdvancedConfig::setMaxThreads, py::arg("threads"),
             "Set maximum number of threads")
        .def("getMaxThreads", &advanced_config::AdvancedConfig::getMaxThreads,
             "Get maximum number of threads")
        .def("getPerformanceProfile", &advanced_config::AdvancedConfig::getPerformanceProfile,
             "Get performance profile")
        .def("configurePerformanceProfile", &advanced_config::AdvancedConfig::configurePerformanceProfile,
             py::arg("profile"), "Configure performance profile");

    // Global advanced config functions
    m.def("setOptimizationLevel", &advanced_config::setOptimizationLevel, py::arg("level"),
          "Set optimization level globally");
    m.def("enableSIMD", &advanced_config::enableSIMD, py::arg("enable"),
          "Enable/disable SIMD globally");
    m.def("enableMultithreading", &advanced_config::enableMultithreading, py::arg("enable"),
          "Enable/disable multithreading globally");
    m.def("setMemoryPoolSize", &advanced_config::setMemoryPoolSize, py::arg("size"),
          "Set memory pool size globally");
    m.def("configurePerformanceProfile", &advanced_config::configurePerformanceProfile,
          py::arg("profile"), "Configure performance profile globally");

    // Config class with enhanced features
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Property accessors
        .def_property("neuron_count", &Config::has, nullptr, "Get/Set neuron count")
        .def_property("region_count", &Config::has, nullptr, "Get/Set region count")
        .def_property("connection_probability", &Config::has, nullptr, "Get/Set connection probability")
        .def("get", [](const Config& self, const std::string& key, py::object default_value) {
            if (self.has(key)) {
                // This is a simplified implementation - actual implementation would need to get values
                return self.get(key);
            } else {
                if (default_value.is(py::none())) {
                    throw KiloValidationError("Key not found: " + key);
                }
                return default_value;
            }
        }, py::arg("key"), py::arg("default_value") = py::none(), "Get configuration value")
        .def("set", [](Config& self, const std::string& key, py::object value) {
            // Type conversion for setting values
            if (value.is(py::int_())) {
                self.set(key, static_cast<float>(py::cast<int64_t>(value)));
            } else if (value.is(py::float_())) {
                self.set(key, py::cast<float>(value));
            } else if (value.is(py::bool_())) {
                self.set(key, py::cast<bool>(value));
            } else if (value.is(py::str())) {
                self.set(key, py::cast<std::string>(value));
            } else if (value.is(py::list())) {
                auto vec = py::cast<std::vector<float>>(value);
                self.set(key, vec);
            } else {
                throw KiloValidationError("Unsupported type for configuration value");
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value");

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

    // WorldObject
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // AgentBody
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset)
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    // ActionResult
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Enhanced action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccess", &ActionResult::success)
        .def("__repr__", [](const ActionResult& result) {
            std::ostringstream oss;
            oss << "<ActionResult: reward=" << result.reward
                << ", success=" << (result.success ? "true" : "false")
                << ", message=\"" << result.message << ">";
            return oss.str();
        });

    // SensoryPercept
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("addSignal", [](SensoryPercept& self, float value) {
            std::vector<float> signals = self.getAllSignals();
            signals.push_back(value);
            // Note: This assumes getAllSignals returns a modifiable vector
            // In actual implementation, you may need to use a different approach
        }, py::arg("value"))
        .def("signalCount", &SensoryPercept::getAllSignals) // Returns number of signals
        .def("hasSignalAt", [](const SensoryPercept& self, size_t index) {
            const auto& signals = self.getAllSignals();
            return index < signals.size();
        }, py::arg("index"), "Check if percept has signal at index");

    // Enhanced SimpleWorld class with missing methods
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision settings")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        // Add missing methods: observe(), applyAction(), computeReward()
        .def("observe", [](SimpleWorld& self, const NeuralRegion* region) -> SensoryPercept {
            // Implementation of observe method
            // For now, returns the current sensory percept
            return self.getSensoryPercept();
        }, py::arg("region"), "Get sensory percept from world at agent's position")
        .def("applyAction", [](SimpleWorld& self, const NeuralRegion* region, const MotorCommand& cmd) -> ActionResult {
            // Implementation of applyAction method
            // For now, uses applyMotorCommand internally
            return self.applyMotorCommand(cmd, self.getSimulationTime());
        }, py::arg("region"), py::arg("command"), "Apply action to world")
        .def("computeReward", [](SimpleWorld& self, const NeuralRegion* region) -> float {
            // Implementation of computeReward method
            // For now, calculates based on agent state
            const AgentBody& agent = self.getAgentBody();
            float reward = 0.0f;
            
            // Check for objects at agent position
            WorldObject* obj = self.getObjectAt(agent.x, agent.y);
            if (obj != nullptr && obj->active) {
                if (obj->type == WorldObjectType::Resource) {
                    reward += obj->value;  // Positive reward for resources
                } else if (obj->type == WorldObjectType::Hazard) {
                    reward += obj->value;  // Negative reward for hazards
                }
            }
            
            return reward;
        }, py::arg("region"), "Compute reward based on current world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent (legacy method)")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y") const,
             "Check if position is valid (not wall)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             "Get object at position (if any)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("__repr__", [](const SimpleWorld& world) {
            std::ostringstream oss;
            oss << "<SimpleWorld: width=" << world.getWidth()
                << ", height=" << world.getHeight()
                << ", agent=(x=" << world.getAgentBody().x
                << ", y=" << world.getAgentBody().y
                << "), time=" << world.getSimulationTime()
                << ">";
            return oss.str();
        });

    // Brain class with enhanced features
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Additional brain methods
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulation system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty detection system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get planner system")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal system");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        // Configuration methods
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Configuration builder helper functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    .def("buildConfigFromDict", &config::buildFromDict,
         py::arg("config"), "Build configuration from Python dictionary")
    .def("createConfigBuilder", &config::buildDefaultConfig,
         "Create a default configuration builder");

    // Simulation helper functions
    m.def("runSimulation", &simulation::runSimulation,
          py::arg("config"), py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("maxSteps"), py::arg("callback"),
          "Run a complete simulation with callback support")
    .def("runEpisode", &simulation::runEpisode,
         py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("stepsPerEpisode"),
         "Run a single episode of simulation");

    // Visualization utilities
    m.def("createVisFunc", &visualization::createVisFunc,
          py::arg("world"), "Create a Python function for world visualization")
    .def("renderWorld", &visualization::renderWorld,
         py::arg("world"), py::arg("format") = "ascii",
         "Render world state as string or dictionary");

    // Performance monitoring
    py::class_<performance::PerformanceStats>(m, "PerformanceStats", R"pbdoc(Performance statistics)")
        .def(py::init<>())
        .def_readwrite("total_wall_time", &performance::PerformanceStats::totalWallTime)
        .def_readwrite("simulation_steps", &performance::PerformanceStats::simulationSteps)
        .def_readwrite("steps_per_second", &performance::PerformanceStats::stepsPerSecond)
        .def_readwrite("memory_usage", &performance::PerformanceStats::memoryUsage)
        .def_readwrite("cpu_usage", &performance::PerformanceStats::cpuUsage)
        .def_readwrite("active_threads", &performance::PerformanceStats::activeThreads)
        .def_readwrite("current_memory", &performance::PerformanceStats::currentMemory);

    m.def("enableProfiling", &performance::enableProfiling,
          py::arg("callback"), "Enable performance profiling");
    m.def("getPerformanceStats", &performance::getPerformanceStats,
          "Get current performance statistics");

    // Checkpointing utilities
    m.def("saveBrain", &checkpoint::saveBrain,
          py::arg("brain"), py::arg("filepath"),
          "Save brain state to file (checkpointing)")
    .def("loadBrain", &checkpoint::loadBrain,
         py::arg("brain"), py::arg("filepath"),
         "Load brain state from file (checkpointing)");

    // Context manager for simulation cleanup
    py::class_<SimulationContext>(m, "SimulationContext", R"pbdoc(Context manager for simulation cleanup)")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"),
             "Create simulation context manager")
        .def("getBrain", &SimulationContext::getBrain,
             "Get brain from context")
        .def("getWorld", &SimulationContext::getWorld,
             "Get world from context")
        .def("getAgent", &SimulationContext::getAgent,
             "Get agent from context")
        .def("reset", &SimulationContext::reset,
             "Reset simulation")
        .def("isDevelopmentEnabled", &SimulationContext::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &SimulationContext::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("isRewardModulationEnabled", &SimulationContext::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("__enter__", [](SimulationContext& self) { return self; })
        .def("__exit__", [](SimulationContext& self, py::object, py::object, py::object) {
            // Cleanup happens in destructor
        });

    // Iterator support for regions and neurons
    py::class_<RegionIterator>(m, "RegionIterator", R"pbdoc(Iterator for neural regions)")
        .def(py::init<>())
        .def("__iter__", [](const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
            return py::make_iterator(regions.begin(), regions.end());
        })
        .def("__next__", [](RegionIterator& self) {
            // Implementation would need to maintain state
            throw std::runtime_error("Iterator implementation needed");
        });

    py::class_<NeuronIterator>(m, "NeuronIterator", R"pbdoc(Iterator for neurons)")
        .def(py::init<>())
        .def("__iter__", [](const std::vector<Neuron*>& neurons) {
            return py::make_iterator(neurons.begin(), neurons.end());
        })
        .def("__next__", [](NeuronIterator& self) {
            // Implementation would need to maintain state
            throw std::runtime_error("Iterator implementation needed");
        });

    // Serialization/deserialization helpers
    m.def("serializeBrain", [](const std::shared_ptr<Brain>& brain) {
        // Implementation would serialize brain state
        return py::dict();
    }, py::arg("brain"), "Serialize brain state to dictionary")
    .def("deserializeBrain", [](const py::dict& data) {
        // Implementation would deserialize brain state
        auto cfg = std::make_shared<Config>();
        return std::make_shared<Brain>(cfg);
    }, py::arg("data"), "Deserialize brain state from dictionary")
    .def("brainToJSON", [](const std::shared_ptr<Brain>& brain) {
        // Implementation would convert brain to JSON string
        return py::str("{}");
    }, py::arg("brain"), "Convert brain state to JSON string")
    .def("brainFromJSON", [](const std::string& json) {
        // Implementation would load brain from JSON string
        auto cfg = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(cfg);
        // Would need to implement actual loading
        return brain;
    }, py::arg("json"), "Load brain state from JSON string");

    // Additional brain statistics methods
    m.def("brainStats", [](const std::shared_ptr<Brain>& brain) {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        return stats;
    }, py::arg("brain"), "Get comprehensive brain statistics");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add module metadata
    m.attr("__version__") = "3.0.0-enhanced";
    m.attr("__author__") = "Enhanced Python Bindings for NLM";
    m.attr("__description__") = "Enhanced Python bindings with comprehensive performance monitoring, configuration builder, simulation managers, export/import tools, visualization and analysis, error handling, and advanced configuration options";
}

} // namespace nlm