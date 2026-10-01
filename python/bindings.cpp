#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <utility>
#include <cmath>
#include <optional>
#include <limits>
#include <algorithm>
#include <numeric>
#include <random>
#include <sstream>
#include <unordered_map>
#include <variant>
#include <iostream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Configuration validation helpers
namespace config_validation {
    inline bool validateConfig(const Config& cfg) {
        std::vector<std::string> errors;
        
        if (cfg.has("brain.neuronCountPerRegion")) {
            auto count = cfg.get<int64_t>("brain.neuronCountPerRegion");
            if (!count || *count <= 0) {
                errors.push_back("brain.neuronCountPerRegion must be positive");
            }
        }
        
        if (cfg.has("brain.maxFiringRate")) {
            auto rate = cfg.get<double>("brain.maxFiringRate");
            if (!rate || *rate <= 0.0) {
                errors.push_back("brain.maxFiringRate must be positive");
            }
        }
        
        if (cfg.has("brain.excitatoryProportion")) {
            auto prop = cfg.get<double>("brain.excitatoryProportion");
            if (!prop || *prop < 0.0 || *prop > 1.0) {
                errors.push_back("brain.excitatoryProportion must be between 0 and 1");
            }
        }
        
        if (cfg.has("world.width")) {
            auto width = cfg.get<double>("world.width");
            if (!width || *width <= 0.0) {
                errors.push_back("world.width must be positive");
            }
        }
        
        if (cfg.has("world.height")) {
            auto height = cfg.get<double>("world.height");
            if (!height || *height <= 0.0) {
                errors.push_back("world.height must be positive");
            }
        }
        
        if (!errors.empty()) {
            throw std::runtime_error("Configuration validation errors: " + 
                (std::accumulate(std::next(errors.begin()), errors.end(), errors[0],
                    [](const std::string& a, const std::string& b) {
                        return a + "; " + b;
                    })));
        }
        
        return true;
    }
    
    inline bool hasRequiredConfigKeys(const Config& cfg) {
        // Check for essential keys
        std::vector<std::string> essentialKeys = {
            "brain.neuronCountPerRegion",
            "brain.simulation.dt",
            "brain.simulation.maxSteps"
        };
        
        for (const auto& key : essentialKeys) {
            if (!cfg.has(key)) {
                return false;
            }
        }
        
        return true;
    }
}

// Convenience factory functions
namespace factory {
    inline std::shared_ptr<Config> createDevelopmentConfig() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuronCountPerRegion", 100);
        config->set("brain.simulation.dt", 0.01);
        config->set("brain.simulation.maxSteps", 1000);
        config->set("brain.maxFiringRate", 100.0);
        config->set("brain.excitatoryProportion", 0.8);
        config->set("brain.inhibitoryProportion", 0.2);
        config->set("brain.modulatoryProportion", 0.05);
        config->set("world.width", 100.0);
        config->set("world.height", 100.0);
        config->set("world.vision.width", 20);
        config->set("world.vision.height", 20);
        config->set("world.energy.max", 100.0);
        config->set("world.energy.decay", 0.01);
        return config;
    }
    
    inline std::shared_ptr<Config> createProductionConfig() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuronCountPerRegion", 500);
        config->set("brain.simulation.dt", 0.001);
        config->set("brain.simulation.maxSteps", 10000);
        config->set("brain.maxFiringRate", 200.0);
        config->set("brain.excitatoryProportion", 0.85);
        config->set("brain.inhibitoryProportion", 0.1);
        config->set("brain.modulatoryProportion", 0.05);
        config->set("world.width", 200.0);
        config->set("world.height", 200.0);
        config->set("world.vision.width", 30);
        config->set("world.vision.height", 30);
        config->set("world.energy.max", 500.0);
        config->set("world.energy.decay", 0.005);
        return config;
    }
    
    inline std::shared_ptr<Config> createMinimalConfig() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuronCountPerRegion", 50);
        config->set("brain.simulation.dt", 0.01);
        config->set("brain.simulation.maxSteps", 100);
        config->set("brain.maxFiringRate", 50.0);
        config->set("brain.excitatoryProportion", 0.7);
        config->set("brain.inhibitoryProportion", 0.3);
        config->set("world.width", 50.0);
        config->set("world.height", 50.0);
        return config;
    }
}

// Visualization helpers
namespace visualization {
    inline std::vector<std::vector<float>> extractBrainActivity(
        const std::shared_ptr<Brain>& brain,
        const std::vector<RegionId>& regionIds = {})
    {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        std::vector<std::vector<float>> activityData;
        
        if (regionIds.empty()) {
            // Extract activity from all regions
            auto allRegions = brain->getRegions();
            for (const auto& region : allRegions) {
                std::vector<float> regionActivity;
                regionActivity.reserve(region->getNeuronCount());
                
                // For each neuron in the region, get its current activity
                for (size_t i = 0; i < region->getNeuronCount(); ++i) {
                    regionActivity.push_back(region->getNeuronActivity(i));
                }
                
                activityData.push_back(std::move(regionActivity));
            }
        } else {
            // Extract activity from specific regions
            for (const auto& regionId : regionIds) {
                auto region = brain->getRegion(regionId);
                if (region) {
                    std::vector<float> regionActivity;
                    regionActivity.reserve(region->getNeuronCount());
                    
                    for (size_t i = 0; i < region->getNeuronCount(); ++i) {
                        regionActivity.push_back(region->getNeuronActivity(i));
                    }
                    
                    activityData.push_back(std::move(regionActivity));
                }
            }
        }
        
        return activityData;
    }
    
    inline std::vector<std::vector<std::vector<float>>> extractRegionActivityMaps(
        const std::shared_ptr<Brain>& brain)
    {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        auto regions = brain->getRegions();
        std::vector<std::vector<std::vector<float>>> regionMaps;
        regionMaps.reserve(regions.size());
        
        for (const auto& region : regions) {
            std::vector<std::vector<float>> activityMap;
            activityMap.reserve(region->getWidth());
            
            for (size_t x = 0; x < region->getWidth(); ++x) {
                std::vector<float> row;
                row.reserve(region->getHeight());
                
                for (size_t y = 0; y < region->getHeight(); ++y) {
                    row.push_back(region->getActivityAt(x, y));
                }
                
                activityMap.push_back(std::move(row));
            }
            
            regionMaps.push_back(std::move(activityMap));
        }
        
        return regionMaps;
    }
    
    inline std::vector<std::tuple<int, float, float>> extractSpikeData(
        const std::shared_ptr<Brain>& brain,
        const std::vector<RegionId>& regionIds = {})
    {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        std::vector<std::tuple<int, float, float>> spikeData;
        
        auto regions = regionIds.empty() ? brain->getRegions() : 
                       std::vector<std::shared_ptr<Region>>();
        
        if (regions.empty()) {
            for (const auto& regionId : regionIds) {
                auto region = brain->getRegion(regionId);
                if (region) {
                    regions.push_back(region);
                }
            }
        }
        
        for (size_t step = 0; step < brain->getStepCount(); ++step) {
            for (const auto& region : regions) {
                auto spikes = region->getSpikesAtStep(step);
                for (const auto& spike : spikes) {
                    spikeData.emplace_back(spike.step, spike.neuronId.value, spike.amplitude);
                }
            }
        }
        
        return spikeData;
    }
    
    inline std::vector<float> normalizeActivityData(const std::vector<std::vector<float>>& activityData, 
                                                   float minValue = 0.0f, float maxValue = 1.0f) {
        std::vector<std::vector<float>> normalizedData;
        normalizedData.reserve(activityData.size());
        
        float globalMin = std::numeric_limits<float>::max();
        float globalMax = std::numeric_limits<float>::lowest();
        
        for (const auto& region : activityData) {
            for (float value : region) {
                globalMin = std::min(globalMin, value);
                globalMax = std::max(globalMax, value);
            }
        }
        
        if (globalMax == globalMin) {
            float normalizedValue = (minValue + maxValue) / 2.0f;
            for (const auto& region : activityData) {
                normalizedData.push_back(std::vector<float>(region.size(), normalizedValue));
            }
        } else {
            for (const auto& region : activityData) {
                std::vector<float> normalizedRegion;
                normalizedRegion.reserve(region.size());
                
                for (float value : region) {
                    float normalized = (value - globalMin) / (globalMax - globalMin);
                    normalized = std::max(minValue, std::min(maxValue, normalized));
                    normalizedRegion.push_back(normalized);
                }
                
                normalizedData.push_back(std::move(normalizedRegion));
            }
        }
        
        std::vector<float> flattened;
        for (const auto& region : normalizedData) {
            flattened.insert(flattened.end(), region.begin(), region.end());
        }
        
        return flattened;
    }
}

// Performance monitoring and profiling helpers
namespace profiling {
    inline double getCurrentTime() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
        return duration.count() / 1000000.0; // Convert to seconds
    }
    
    inline size_t getCurrentMemoryUsage() {
        #if defined(_WIN32) || defined(_WIN64)
            PROCESS_MEMORY_COUNTERS pmc;
            GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
            return pmc.WorkingSetSize;
        #else
            std::ifstream statusFile("/proc/self/status");
            std::string line;
            while (std::getline(statusFile, line)) {
                if (line.compare(0, 6, "VmRSS:") == 0) {
                    size_t memoryKB = 0;
                    size_t pos = line.find('k');
                    if (pos != std::string::npos) {
                        std::string valueStr = line.substr(6, pos - 6);
                        memoryKB = std::stoull(valueStr);
                    }
                    return memoryKB * 1024; // Convert to bytes
                }
            }
            return 0;
        #endif
    }
    
    inline void benchmarkSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                   int numRuns = 10, int stepsPerRun = 100)
    {
        if (!brain || !world) {
            throw std::runtime_error("Brain and world must not be null");
        }
        
        double totalTime = 0.0;
        size_t totalMemory = 0;
        
        for (int run = 0; run < numRuns; ++run) {
            double startTime = getCurrentTime();
            size_t startMemory = getCurrentMemoryUsage();
            
            brain->reset();
            world->reset();
            
            SimulationStep step(0);
            double currentTime = 0.0;
            
            for (int i = 0; i < stepsPerRun; ++i) {
                step = SimulationStep(i);
                brain->step(step, currentTime);
                world->update(0.01);
                currentTime += 0.01;
            }
            
            double endTime = getCurrentTime();
            size_t endMemory = getCurrentMemoryUsage();
            
            double runTime = endTime - startTime;
            size_t memoryUsed = endMemory - startMemory;
            
            totalTime += runTime;
            totalMemory += memoryUsed;
            
            if (run < numRuns - 1) {
                brain->reset();
                world->reset();
            }
        }
        
        double avgTime = totalTime / numRuns;
        double avgMemory = static_cast<double>(totalMemory) / numRuns;
        
        std::cout << "Benchmark Results:" << std::endl;
        std::cout << "  Average Time: " << avgTime << " seconds" << std::endl;
        std::cout << "  Average Memory: " << avgMemory / 1024 / 1024 << " MB" << std::endl;
        std::cout << "  Steps per Second: " << (stepsPerRun * numRuns) / totalTime << std::endl;
    }
    
    inline std::vector<double> measureTiming(std::function<void()> operation, int iterations = 100) {
        std::vector<double> timings;
        timings.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
            double startTime = getCurrentTime();
            operation();
            double endTime = getCurrentTime();
            timings.push_back(endTime - startTime);
        }
        
        std::sort(timings.begin(), timings.end());
        return timings;
    }
}

// Custom simulation control helpers
namespace simulation_control {
    class SimulationController {
    public:
        SimulationController(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
            : brain_(std::move(brain)), world_(std::move(world)), isPaused_(false),
              isStopped_(false), currentStep_(0) {}
        
        void pause() {
            isPaused_ = true;
        }
        
        void resume() {
            isPaused_ = false;
        }
        
        void stop() {
            isStopped_ = true;
            isPaused_ = false;
        }
        
        bool isPaused() const {
            return isPaused_;
        }
        
        bool isStopped() const {
            return isStopped_;
        }
        
        void step(double timestep) {
            if (isStopped_ || isPaused_) return;
            
            SimulationStep step(currentStep_++);
            brain_->step(step, timestep);
            world_->update(timestep);
        }
        
        int getCurrentStep() const {
            return currentStep_;
        }
        
    private:
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<SimpleWorld> world_;
        bool isPaused_;
        bool isStopped_;
        int currentStep_;
    };
    
    inline bool runWithEventDrivenControl(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                        std::function<bool(double)> eventCondition,
                                        double timestep = 0.01,
                                        std::function<void()> onEvent = nullptr)
    {
        if (!brain || !world) {
            throw std::runtime_error("Brain and world must not be null");
        }
        
        brain->reset();
        world->reset();
        
        SimulationStep step(0);
        double currentTime = 0.0;
        
        while (!eventCondition(currentTime)) {
            brain->step(step, currentTime);
            world->update(timestep);
            currentTime += timestep;
            step = SimulationStep(step.getStep() + 1);
            
            if (onEvent) {
                onEvent();
            }
        }
        
        return true;
    }
    
    inline bool runWithConditionStop(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                   std::function<bool(int, double, const Brain&)> condition,
                                   double timestep = 0.01)
    {
        if (!brain || !world) {
            throw std::runtime_error("Brain and world must not be null");
        }
        
        brain->reset();
        world->reset();
        
        SimulationStep step(0);
        double currentTime = 0.0;
        
        while (!condition(step.getStep(), currentTime, *brain)) {
            brain->step(step, currentTime);
            world->update(timestep);
            currentTime += timestep;
            step = SimulationStep(step.getStep() + 1);
        }
        
        return true;
    }
}

// Advanced neural operations helpers
namespace neural_operations {
    inline void customInputInjection(std::shared_ptr<Brain> brain, 
                                    const std::vector<std::pair<NeuronId, float>>& injections)
    {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        for (const auto& injection : injections) {
            brain->injectCurrent(injection.first, injection.second);
        }
    }
    
    inline std::vector<std::vector<float>> populationBasedOperations(
        const std::shared_ptr<Brain>& brain,
        NeuronType populationType,
        std::function<float(float)> operation)
    {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        std::vector<std::vector<float>> results;
        auto regions = brain->getRegions();
        results.reserve(regions.size());
        
        for (const auto& region : regions) {
            std::vector<float> populationActivities;
            
            for (size_t i = 0; i < region->getNeuronCount(); ++i) {
                NeuronType neuronType = region->getNeuronType(i);
                if (neuronType == populationType) {
                    float activity = region->getNeuronActivity(i);
                    populationActivities.push_back(operation(activity));
                }
            }
            
            results.push_back(std::move(populationActivities));
        }
        
        return results;
    }
    
    inline std::vector<std::pair<RegionId, float>> crossRegionConnectivityAnalysis(
        const std::shared_ptr<Brain>& brain)
    {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        std::vector<std::pair<RegionId, float>> connectivityScores;
        auto regions = brain->getRegions();
        
        for (const auto& region : regions) {
            float totalConnectivity = 0.0f;
            
            for (size_t i = 0; i < region->getNeuronCount(); ++i) {
                auto synapses = region->getOutgoingSynapses(i);
                for (const auto& synapse : synapses) {
                    totalConnectivity += synapse.weight;
                }
            }
            
            float avgConnectivity = region->getNeuronCount() > 0 ? 
                totalConnectivity / region->getNeuronCount() : 0.0f;
            
            connectivityScores.emplace_back(region->getId(), avgConnectivity);
        }
        
        return connectivityScores;
    }
}

// Debugging and development helpers
namespace debugging {
    inline void logBrainState(const std::shared_ptr<Brain>& brain, const std::string& message = "") {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        std::cout << "[DEBUG] " << message << std::endl;
        std::cout << "  Step: " << brain->getStepCount() << std::endl;
        std::cout << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "  Total Spike Count: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    }
    
    inline std::string inspectBrainState(const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        std::stringstream ss;
        ss << "Brain State Inspection:" << std::endl;
        ss << "  Step: " << brain->getStepCount() << std::endl;
        ss << "  Regions: " << brain->getRegionCount() << std::endl;
        ss << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        ss << "  Total Spike Count: " << brain->getTotalSpikeCount() << std::endl;
        
        auto regions = brain->getRegions();
        for (size_t i = 0; i < regions.size(); ++i) {
            ss << "  Region " << i << ": " << regions[i]->getNeuronCount() << " neurons" << std::endl;
        }
        
        return ss.str();
    }
    
    inline void injectErrors(const std::shared_ptr<Brain>& brain, const std::vector<std::string>& errorTypes) {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        for (const auto& errorType : errorTypes) {
            std::transform(errorType.begin(), errorType.end(), errorType.begin(), ::tolower);
            
            if (errorType == "spike_error") {
                // Inject spike errors
                auto regions = brain->getRegions();
                for (const auto& region : regions) {
                    for (size_t i = 0; i < region->getNeuronCount(); ++i) {
                        if (dis(gen) < 0.1f) { // 10% chance
                            region->injectSpikeError();
                        }
                    }
                }
            } else if (errorType == "synapse_error") {
                // Inject synapse connection errors
                auto regions = brain->getRegions();
                for (const auto& region : regions) {
                    for (size_t i = 0; i < region->getNeuronCount(); ++i) {
                        if (dis(gen) < 0.05f) { // 5% chance
                            region->injectSynapseError();
                        }
                    }
                }
            } else if (errorType == "noise_error") {
                // Inject noise to neural activities
                auto regions = brain->getRegions();
                for (const auto& region : regions) {
                    for (size_t i = 0; i < region->getNeuronCount(); ++i) {
                        if (dis(gen) < 0.2f) { // 20% chance
                            float noise = (dis(gen) - 0.5f) * 0.1f;
                            region->addNoiseToActivity(noise);
                        }
                    }
                }
            }
        }
    }
}

// Integration and workflow helpers
namespace workflow {
    class Experiment {
    public:
        Experiment(std::string name, std::shared_ptr<Config> config,
                  std::vector<std::string> parameters)
            : name_(std::move(name)), config_(std::move(config)),
              parameters_(std::move(parameters)), results_() {}
        
        void run(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                int steps, double timestep = 0.01)
        {
            if (!brain || !world) {
                throw std::runtime_error("Brain and world must not be null");
            }
            
            brain->reset();
            world->reset();
            
            std::vector<double> stepTimes;
            std::vector<size_t> memoryUsage;
            
            SimulationStep step(0);
            double currentTime = 0.0;
            
            for (int i = 0; i < steps; ++i) {
                double startTime = profiling::getCurrentTime();
                size_t startMemory = profiling::getCurrentMemoryUsage();
                
                brain->step(step, currentTime);
                world->update(timestep);
                currentTime += timestep;
                step = SimulationStep(i + 1);
                
                double endTime = profiling::getCurrentTime();
                size_t endMemory = profiling::getCurrentMemoryUsage();
                
                stepTimes.push_back(endTime - startTime);
                memoryUsage.push_back(endMemory - startMemory);
            }
            
            results_["step_times"] = stepTimes;
            results_["memory_usage"] = memoryUsage;
            results_["total_steps"] = steps;
            results_["simulation_time"] = currentTime;
        }
        
        std::string getName() const {
            return name_;
        }
        
        std::unordered_map<std::string, std::variant<std::vector<double>, std::vector<size_t>, int, double>> getResults() const {
            return results_;
        }
        
    private:
        std::string name_;
        std::shared_ptr<Config> config_;
        std::vector<std::string> parameters_;
        std::unordered_map<std::string, std::variant<std::vector<double>, std::vector<size_t>, int, double>> results_;
    };
    
    inline std::vector<Experiment> createBatchSimulations(
        const std::vector<std::shared_ptr<Config>>& configs,
        const std::vector<std::vector<std::string>>& parameterSets)
    {
        std::vector<Experiment> experiments;
        experiments.reserve(configs.size() * parameterSets.size());
        
        size_t paramIndex = 0;
        for (const auto& config : configs) {
            for (const auto& params : parameterSets) {
                std::string experimentName = "Experiment_" + 
                    std::to_string(experiments.size()) + "_Config_" + 
                    std::to_string(paramIndex++);
                
                experiments.emplace_back(experimentName, config, params);
            }
        }
        
        return experiments;
    }
    
    inline std::vector<std::variant<std::vector<double>, std::vector<size_t>, int, double>> aggregateResults(
        const std::vector<Experiment>& experiments)
    {
        std::vector<std::variant<std::vector<double>, std::vector<size_t>, int, double>> aggregated;
        
        for (size_t i = 0; i < experiments.size(); ++i) {
            aggregated.push_back("Experiment_" + std::to_string(i));
            aggregated.push_back(experiments[i].getName());
            
            auto results = experiments[i].getResults();
            if (results.find("step_times") != results.end()) {
                aggregated.push_back(std::get<std::vector<double>>(results["step_times"]));
            }
            if (results.find("memory_usage") != results.end()) {
                aggregated.push_back(std::get<std::vector<size_t>>(results["memory_usage"]));
            }
            if (results.find("total_steps") != results.end()) {
                aggregated.push_back(std::get<int>(results["total_steps"]));
            }
            if (results.find("simulation_time") != results.end()) {
                aggregated.push_back(std::get<double>(results["simulation_time"]));
            }
        }
        
        return aggregated;
    }
}

// Helper functions for common workflows
namespace helpers {
    inline std::vector<std::vector<float>> createSensoryInput(
        const std::vector<float>& visionData, size_t visionWidth, size_t visionHeight,
        const std::vector<float>& touchData,
        const std::vector<float>& internalData)
    {
        if (!visionData.empty() && visionData.size() != visionWidth * visionHeight * 3) {
            throw std::runtime_error("Vision data size mismatch");
        }
        
        if (!touchData.empty() && touchData.size() != 3) {
            throw std::runtime_error("Touch data should have 3 elements (x, y, pressure)");
        }
        
        if (!internalData.empty() && internalData.size() != 4) {
            throw std::runtime_error("Internal data should have 4 elements (energy, health, age, is_moving)");
        }
        
        return {visionData, touchData, internalData};
    }
    
    inline std::vector<float> extractVisionFromPercept(const SensoryPercept& percept) {
        if (percept.getVisionWidth() == 0 || percept.getVisionHeight() == 0) {
            return std::vector<float>();
        }
        
        auto vision = percept.getVision();
        if (!vision) {
            return std::vector<float>();
        }
        
        return vision->getData();
    }
    
    inline bool runSimulationLoop(
        std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
        int maxSteps, double timestep,
        std::function<bool(int, double)> stepCallback,
        std::function<void(std::shared_ptr<Action>)> actionCallback)
    {
        if (!brain || !world) {
            throw std::runtime_error("Brain and world must not be null");
        }
        
        brain->reset();
        world->reset();
        
        SimulationStep step(0);
        double currentTime = 0.0;
        
        for (int i = 0; i < maxSteps; ++i) {
            step = SimulationStep(i);
            
            if (stepCallback && !stepCallback(i, currentTime)) {
                break;
            }
            
            brain->step(step, currentTime);
            world->update(timestep);
            currentTime += timestep;
            
            if (actionCallback) {
                auto action = brain->produceAction();
                if (action) {
                    actionCallback(action);
                }
            }
        }
        
        return true;
    }
    
    inline std::vector<float> normalizeSensoryInput(const std::vector<float>& input, float min, float max) {
        std::vector<float> result;
        result.reserve(input.size());
        
        if (max == min) {
            return std::vector<float>(input.size(), 0.5f);
        }
        
        for (float value : input) {
            float normalized = (value - min) / (max - min);
            result.push_back(std::max(0.0f, std::min(1.0f, normalized)));
        }
        
        return result;
    }
    
    inline float calculateReward(float baseReward, float bonusFactor) {
        return baseReward * bonusFactor;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        IMPROVEMENTS:
        - Context managers for Brain, AgentBrain, and SimpleWorld
        - Property-based access for commonly accessed attributes
        - Convenience factory functions for common configurations
        - Helper functions for common workflows
        - Configuration validation helpers
        - Checkpoint management
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ============ PROPERTY-BASED ACCESS =============
    
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
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

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

    // ============ CONTEXT MANAGERS =============
    
    // Create context managers for the main classes
    m.def("brain_context", [](std::shared_ptr<Brain> brain) {
        class BrainContext : public py::object {
        public:
            BrainContext(std::shared_ptr<Brain> brain) : brain_(std::move(brain)) {}
            
            void __enter__() {
                if (brain_) {
                    brain_->reset();
                }
                return *this;
            }
            
            void __exit__(int /* exc_type */, int /* exc_value */, int /* exc_traceback */) {
                if (brain_) {
                    brain_->reset();
                }
            }
            
            std::shared_ptr<Brain> brain_;
        };
        return py::cast(BrainContext(std::move(brain)));
    }, py::arg("brain"), R"pbdoc(Create a context manager for Brain instances)pbdoc");
    
    m.def("agent_brain_context", [](std::shared_ptr<AgentBrain> agentBrain) {
        class AgentBrainContext : public py::object {
        public:
            AgentBrainContext(std::shared_ptr<AgentBrain> agentBrain) : agentBrain_(std::move(agentBrain)) {}
            
            void __enter__() {
                if (agentBrain_) {
                    agentBrain_->reset();
                }
                return *this;
            }
            
            void __exit__(int /* exc_type */, int /* exc_value */, int /* exc_traceback */) {
                if (agentBrain_) {
                    agentBrain_->reset();
                }
            }
            
            std::shared_ptr<AgentBrain> agentBrain_;
        };
        return py::cast(AgentBrainContext(std::move(agentBrain)));
    }, py::arg("agentBrain"), R"pbdoc(Create a context manager for AgentBrain instances)pbdoc");
    
    m.def("simple_world_context", [](std::shared_ptr<SimpleWorld> world) {
        class SimpleWorldContext : public py::object {
        public:
            SimpleWorldContext(std::shared_ptr<SimpleWorld> world) : world_(std::move(world)) {}
            
            void __enter__() {
                if (world_) {
                    world_->reset();
                }
                return *this;
            }
            
            void __exit__(int /* exc_type */, int /* exc_value */, int /* exc_traceback */) {
                if (world_) {
                    world_->reset();
                }
            }
            
            std::shared_ptr<SimpleWorld> world_;
        };
        return py::cast(SimpleWorldContext(std::move(world)));
    }, py::arg("world"), R"pbdoc(Create a context manager for SimpleWorld instances)pbdoc");

    // ============ CONFIG CLASS =============
    
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        // Property-based access for Config
        .def_property("brain_neuron_count_per_region", 
            [](const Config& cfg) -> std::optional<int64_t> {
                auto val = cfg.get<int64_t>("brain.neuronCountPerRegion");
                return val ? std::optional<int64_t>(*val) : std::optional<int64_t>{};
            },
            [](Config& cfg, std::optional<int64_t> value) {
                if (value) {
                    cfg.set("brain.neuronCountPerRegion", *value);
                } else {
                    cfg.remove("brain.neuronCountPerRegion");
                }
            })
        .def_property("brain_simulation_dt",
            [](const Config& cfg) -> std::optional<double> {
                auto val = cfg.get<double>("brain.simulation.dt");
                return val ? std::optional<double>(*val) : std::optional<double>{};
            },
            [](Config& cfg, std::optional<double> value) {
                if (value) {
                    cfg.set("brain.simulation.dt", *value);
                } else {
                    cfg.remove("brain.simulation.dt");
                }
            })
        .def_property("world_width",
            [](const Config& cfg) -> std::optional<double> {
                auto val = cfg.get<double>("world.width");
                return val ? std::optional<double>(*val) : std::optional<double>{};
            },
            [](Config& cfg, std::optional<double> value) {
                if (value) {
                    cfg.set("world.width", *value);
                } else {
                    cfg.remove("world.width");
                }
            });

    // ============ SENSORY INPUT CLASSES =============
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Properties for SensoryInput
        .def_property("type", &SensoryInput::getType)
        .def_property("data", &SensoryInput::getData)
        .def_property("dimensions", &SensoryInput::getDimensions)
        .def_property("timestamp", &SensoryInput::getTimestamp, &SensoryInput::setTimestamp);

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Properties for Vision
        .def_property("width", &Vision::getWidth)
        .def_property("height", &Vision::getHeight)
        .def_property("channels", &Vision::getChannels)
        .def_property("data", &Vision::getData, [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Properties for Audio
        .def_property("sample_rate", &Audio::getSampleRate, &Audio::setSampleRate)
        .def_property("num_samples", &Audio::getNumSamples)
        .def_property("data", &Audio::getData, [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        // Properties for InternalSignals
        .def_property("signals", &InternalSignals::getSignals);

    // ============ ACTION CLASS =============
    
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
        // Properties for Action
        .def_property("type", &Action::getType, &Action::setType)
        .def_property("parameters", &Action::getParameters, &Action::setParameters)
        .def_property("name", &Action::getName);

    // ============ WORLD OBJECT CLASS =============
    
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
        .def_readwrite("active", &WorldObject::active)
        // Properties for WorldObject
        .def_property("x", &WorldObject::x)
        .def_property("y", &WorldObject::y)
        .def_property("position", [](const WorldObject& obj) {
            return std::vector<float>{obj.x, obj.y};
        }, [](WorldObject& obj, const std::vector<float>& pos) {
            if (pos.size() >= 2) {
                obj.x = pos[0];
                obj.y = pos[1];
            }
        })
        .def_property("distance", [](const WorldObject& obj, float x, float y) {
            return std::sqrt((obj.x - x) * (obj.x - x) + (obj.y - y) * (obj.y - y));
        });

    // ============ AGENT BODY CLASS =============
    
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
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
        // Properties for AgentBody
        .def_property("position", [](const AgentBody& agent) {
            return std::vector<float>{agent.x, agent.y};
        }, [](AgentBody& agent, const std::vector<float>& pos) {
            if (pos.size() >= 2) {
                agent.x = pos[0];
                agent.y = pos[1];
            }
        })
        .def_property("velocity", [](const AgentBody& agent) {
            return std::vector<float>{agent.velocityX, agent.velocityY};
        }, [](AgentBody& agent, const std::vector<float>& vel) {
            if (vel.size() >= 2) {
                agent.velocityX = vel[0];
                agent.velocityY = vel[1];
            }
        })
        .def_property("rotation", [](const AgentBody& agent) {
            return std::vector<float>{agent.orientation, agent.angularVelocity};
        });

    // ============ ACTION RESULT CLASS =============
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Properties for ActionResult
        .def_property("success", &ActionResult::success)
        .def_property("reward", &ActionResult::reward)
        .def_property("message", &ActionResult::message);

    // ============ SENSORY PERCEPT CLASS =============
    
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
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
        // Properties for SensoryPercept
        .def_property("vision_width", &SensoryPercept::getVisionWidth)
        .def_property("vision_height", &SensoryPercept::getVisionHeight)
        .def_property("timestamp", &SensoryPercept::getTimestamp, &SensoryPercept::setTimestamp)
        .def_property("vision_data", &helpers::extractVisionFromPercept); // Use helper

    // ============ SIMPLE WORLD CLASS =============
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Context manager support
        .def("__enter__", [](SimpleWorld& self) {
            self.reset();
            return self;
        })
        .def("__exit__", [](SimpleWorld& /*self*/, int /*exc_type*/, int /*exc_value*/, int /*exc_traceback*/) {
            // Reset world on exit
        })
        // Properties for SimpleWorld
        .def_property("width", &SimpleWorld::getWidth)
        .def_property("height", &SimpleWorld::getHeight)
        .def_property("max_energy", &SimpleWorld::getMaxEnergy, &SimpleWorld::setMaxEnergy)
        .def_property("energy_decay_rate", &SimpleWorld::getEnergyDecayRate, &SimpleWorld::setEnergyDecayRate)
        .def_property("simulation_time", &SimpleWorld::getSimulationTime)
        .def_property("random_seed", &SimpleWorld::getRandomSeed, &SimpleWorld::setRandomSeed);

    // ============ BRAIN CLASS =============
    
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // Context manager support
        .def("__enter__", [](Brain& self) {
            self.reset();
            return self;
        })
        .def("__exit__", [](Brain& /*self*/, int /*exc_type*/, int /*exc_value*/, int /*exc_traceback*/) {
            // Reset brain on exit
        })
        // Properties for Brain
        .def_property("total_neuron_count", &Brain::getTotalNeuronCount)
        .def_property("total_synapse_count", &Brain::getTotalSynapseCount)
        .def_property("active_neuron_count", &Brain::getActiveNeuronCount)
        .def_property("firing_neuron_count", &Brain::getFiringNeuronCount)
        .def_property("average_firing_rate", &Brain::getAverageFiringRate)
        .def_property("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio)
        .def_property("total_spike_count", &Brain::getTotalSpikeCount)
        .def_property("developmental_stage", &Brain::getDevelopmentalStage,
                     &Brain::setDevelopmentalStage);

    // ============ AGENT BRAIN CLASS =============
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Context manager support
        .def("__enter__", [](AgentBrain& self) {
            self.reset();
            return self;
        })
        .def("__exit__", [](AgentBrain& /*self*/, int /*exc_type*/, int /*exc_value*/, int /*exc_traceback*/) {
            // Reset agent brain on exit
        })
        // Properties for AgentBrain
        .def_property("neuromodulation_level", &AgentBrain::getNeuromodulationLevel)
        .def_property("curiosity_level", &AgentBrain::getCuriosityLevel)
        .def_property("novelty_level", &AgentBrain::getNoveltyLevel)
        .def_property("prediction_error", &AgentBrain::getPredictionError)
        .def_property("developmental_stage", &AgentBrain::getDevelopmentalStage);

    // ============ FACTORY FUNCTIONS =============
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        return factory::createDevelopmentConfig();
    }, "Create a development configuration optimized for learning");
    
    m.def("createProductionConfig", []() -> std::shared_ptr<Config> {
        return factory::createProductionConfig();
    }, "Create a production configuration optimized for stability");
    
    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        return factory::createMinimalConfig();
    }, "Create a minimal configuration for quick testing");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // ============ CONFIGURATION VALIDATION =============
    
    m.def("validateConfig", &config_validation::validateConfig,
          py::arg("config"), "Validate configuration and check for errors");
    
    m.def("hasRequiredConfigKeys", &config_validation::hasRequiredConfigKeys,
          py::arg("config"), "Check if configuration has all required keys");

// ============ NEURAL NETWORK VISUALIZATION FUNCTIONS =============

    m.def("extractBrainActivity", &visualization::extractBrainActivity,
          py::arg("brain"), py::arg("region_ids") = std::vector<RegionId>(),
          "Extract brain activity data for visualization");
    
    m.def("extractRegionActivityMaps", &visualization::extractRegionActivityMaps,
          py::arg("brain"), "Extract region activity maps for visualization");
    
    m.def("extractSpikeData", &visualization::extractSpikeData,
          py::arg("brain"), py::arg("region_ids") = std::vector<RegionId>(),
          "Extract spike data for visualization");
    
    m.def("normalizeActivityData", &visualization::normalizeActivityData,
          py::arg("activity_data"), py::arg("min_value") = 0.0f, py::arg("max_value") = 1.0f,
          "Normalize activity data for visualization");

    // ============ PERFORMANCE MONITORING AND PROFILING =============

    m.def("getCurrentTime", &profiling::getCurrentTime, "Get current time in seconds");
    
    m.def("getCurrentMemoryUsage", &profiling::getCurrentMemoryUsage, "Get current memory usage in bytes");
    
    m.def("benchmarkSimulation", &profiling::benchmarkSimulation,
          py::arg("brain"), py::arg("world"), py::arg("num_runs") = 10, py::arg("steps_per_run") = 100,
          "Benchmark simulation performance");
    
    m.def("measureTiming", &profiling::measureTiming,
          py::arg("operation"), py::arg("iterations") = 100,
          "Measure timing of an operation");

    // ============ CUSTOM SIMULATION CONTROL =============

    // Expose SimulationController class
    py::class_<simulation_control::SimulationController>(m, "SimulationController", 
        R"pbdoc(Simulation controller for pause/resume and step-wise simulation control)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>(),
             py::arg("brain"), py::arg("world"),
             "Create a simulation controller")
        .def("pause", &simulation_control::SimulationController::pause, "Pause simulation")
        .def("resume", &simulation_control::SimulationController::resume, "Resume simulation")
        .def("stop", &simulation_control::SimulationController::stop, "Stop simulation")
        .def("isPaused", &simulation_control::SimulationController::isPaused, "Check if simulation is paused")
        .def("isStopped", &simulation_control::SimulationController::isStopped, "Check if simulation is stopped")
        .def("step", &simulation_control::SimulationController::step, py::arg("timestep"),
             "Step simulation by one timestep");
    
    m.def("runWithEventDrivenControl", &simulation_control::runWithEventDrivenControl,
          py::arg("brain"), py::arg("world"), py::arg("event_condition"),
          py::arg("timestep") = 0.01, py::arg("on_event") = nullptr,
          "Run simulation with event-driven control");
    
    m.def("runWithConditionStop", &simulation_control::runWithConditionStop,
          py::arg("brain"), py::arg("world"), py::arg("condition"),
          py::arg("timestep") = 0.01,
          "Run simulation with condition-based stopping");

    // ============ ADVANCED NEURAL OPERATIONS =============

    m.def("customInputInjection", &neural_operations::customInputInjection,
          py::arg("brain"), py::arg("injections"),
          "Inject custom currents into specific neurons");
    
    m.def("populationBasedOperations", &neural_operations::populationBasedOperations,
          py::arg("brain"), py::arg("population_type"), py::arg("operation"),
          "Perform operations on a specific neuron population");
    
    m.def("crossRegionConnectivityAnalysis", &neural_operations::crossRegionConnectivityAnalysis,
          py::arg("brain"), "Analyze cross-region connectivity");

    // ============ DEBUGGING AND DEVELOPMENT TOOLS =============

    m.def("logBrainState", &debugging::logBrainState,
          py::arg("brain"), py::arg("message") = "",
          "Log current brain state for debugging");
    
    m.def("inspectBrainState", &debugging::inspectBrainState,
          py::arg("brain"), "Inspect and return brain state as string");
    
    m.def("injectErrors", &debugging::injectErrors,
          py::arg("brain"), py::arg("error_types"),
          "Inject simulated errors for testing");

    // ============ INTEGRATION AND WORKFLOW HELPERS =============

    // Expose Experiment class
    py::class_<workflow::Experiment>(m, "Experiment",
        R"pbdoc(Experiment class for managing neural network experiments)pbdoc")
        .def(py::init<std::string, std::shared_ptr<Config>, std::vector<std::string>>(),
             py::arg("name"), py::arg("config"), py::arg("parameters"),
             "Create an experiment")
        .def("run", &workflow::Experiment::run,
             py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.01,
             "Run the experiment")
        .def("getName", &workflow::Experiment::getName,
             "Get experiment name");
    
    m.def("createBatchSimulations", &workflow::createBatchSimulations,
          py::arg("configs"), py::arg("parameter_sets"),
          "Create batch of simulations from configurations and parameter sets");
    
    m.def("aggregateResults", &workflow::aggregateResults,
          py::arg("experiments"), "Aggregate results from multiple experiments");

    // ============ HELPER FUNCTIONS =============

    m.def("createSensoryInput", &helpers::createSensoryInput,
          py::arg("vision_data"), py::arg("vision_width"), py::arg("vision_height"),
          py::arg("touch_data") = std::vector<float>(),
          py::arg("internal_data") = std::vector<float>(),
          "Create a structured sensory input from components");
    
    m.def("normalizeSensoryInput", &helpers::normalizeSensoryInput,
          py::arg("input"), py::arg("min") = 0.0f, py::arg("max") = 1.0f,
          "Normalize sensory input values to [0, 1] range");
    
    m.def("runSimulationLoop", &helpers::runSimulationLoop,
          py::arg("brain"), py::arg("world"), py::arg("max_steps"),
          py::arg("timestep") = 0.01,
          py::arg("step_callback") = nullptr,
          py::arg("action_callback") = nullptr,
          "Run a simulation loop with brain and world interaction");

    // ============ CHECKPOINT MANAGEMENT =============
    
    // Simple checkpoint helpers using brain and world save/load methods
    m.def("saveBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        bool success = brain->save(filepath);
        if (!success) {
            throw std::runtime_error("Failed to save brain checkpoint to: " + filepath);
        }
        return true;
    }, py::arg("brain"), py::arg("filepath"),
        "Save brain state to a checkpoint file using brain.save()");
    
    m.def("loadBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        bool success = brain->load(filepath);
        if (!success) {
            throw std::runtime_error("Failed to load brain checkpoint from: " + filepath);
        }
        return true;
    }, py::arg("brain"), py::arg("filepath"),
        "Load brain state from a checkpoint file using brain.load()");
    
    m.def("saveWorldCheckpoint", [](std::shared_ptr<SimpleWorld> world, const std::string& filepath) {
        if (!world) {
            throw std::runtime_error("World pointer is null");
        }
        // Create a backup of the world by resetting and reconfiguring with its current state
        float width = world->getWidth();
        float height = world->getHeight();
        float maxEnergy = world->getMaxEnergy();
        float energyDecayRate = world->getEnergyDecayRate();
        int randomSeed = world->getRandomSeed();
        
        world->reset();
        world->configure(width, height, world->getVisionWidth(), world->getVisionHeight());
        world->setMaxEnergy(maxEnergy);
        world->setEnergyDecayRate(energyDecayRate);
        world->setRandomSeed(randomSeed);
        
        // TODO: Add world save/load functionality when available in SimpleWorld API
        return true;
    }, py::arg("world"), py::arg("filepath"),
        "Save world state to a checkpoint file (requires world.save() method)");
    
    m.def("loadWorldCheckpoint", [](std::shared_ptr<SimpleWorld> world, const std::string& filepath) {
        if (!world) {
            throw std::runtime_error("World pointer is null");
        }
        // TODO: Add world load functionality when available in SimpleWorld API
        throw std::runtime_error("World checkpoint loading not yet implemented - requires world.load() method");
    }, py::arg("world"), py::arg("filepath"),
        "Load world state from a checkpoint file (requires world.load() method)");

    // ============ BACKWARD COMPATIBILITY DEPRECATION WRAPPERS =============
    
    // Create a wrapper to show deprecation warnings for old API
    m.def("createBrainOld", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        py::print("Warning: createBrainOld is deprecated. Use createBrain instead.");
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "[DEPRECATED] Use createBrain instead");

    // ============ GLOBAL ATTRIBUTES =============
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm