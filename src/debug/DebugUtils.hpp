#pragma once

#include <memory>
#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <ctime>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"

using namespace nlm;

/**
 * Enhanced debug and profiling utilities for advanced users
 */
namespace Debug {
    
    /**
     * System state snapshot for debugging
     */
    struct SystemSnapshot {
        time_t timestamp;
        uint64_t stepNumber;
        double simulationTime;
        
        // Neural state
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        size_t totalSpikes;
        float averageFiringRate;
        float excitationInhibitionRatio;
        
        // Memory state
        size_t workingMemoryActiveTraces;
        size_t episodicMemoryEpisodes;
        size_t associativeMemoryAssociations;
        
        // Neuromodulation state
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
        
        // Development state
        std::string developmentalStage;
        float developmentalAge;
        
        // Performance metrics
        double stepTimeMs;
        size_t neuronsPerSecond;
        size_t spikesPerSecond;
        
        // Configuration snapshot
        std::unordered_map<std::string, std::string> configValues;
        
        SystemSnapshot() : timestamp(0), stepNumber(0), simulationTime(0.0),
                          totalNeurons(0), totalSynapses(0), activeNeurons(0),
                          firingNeurons(0), totalSpikes(0), averageFiringRate(0.0f),
                          excitationInhibitionRatio(0.0f), workingMemoryActiveTraces(0),
                          episodicMemoryEpisodes(0), associativeMemoryAssociations(0),
                          dopamineLevel(0.0f), curiosityLevel(0.0f), noveltyLevel(0.0f),
                          predictionError(0.0f), developmentalAge(0.0f),
                          stepTimeMs(0.0), neuronsPerSecond(0), spikesPerSecond(0) {}
        
        std::string toString(bool detailed = false) const {
            std::stringstream ss;
            ss << "Step: " << stepNumber 
               << " | Neurons: " << totalNeurons 
               << " | Synapses: " << totalSynapses
               << " | Active: " << activeNeurons
               << " | Firing: " << firingNeurons
               << " | Total Spikes: " << totalSpikes
               << " | Firing Rate: " << std::fixed << std::setprecision(2) << averageFiringRate
               << " | E/I Ratio: " << std::fixed << std::setprecision(3) << excitationInhibitionRatio;
            
            if (detailed) {
                ss << "\n  Memory: Working=" << workingMemoryActiveTraces 
                   << " | Episodic=" << episodicMemoryEpisodes
                   << " | Associative=" << associativeMemoryAssociations;
                ss << "\n  Neuromod: DA=" << dopamineLevel << " | Cur=" << curiosityLevel 
                   << " | Nov=" << noveltyLevel << " | PredErr=" << predictionError;
                ss << "\n  Dev: " << developmentalStage << " | Age=" << developmentalAge;
                ss << "\n  Perf: " << std::fixed << std::setprecision(3) << stepTimeMs << "ms/step"
                   << " | " << neuronsPerSecond << " neurons/sec";
            }
            
            return ss.str();
        }
    };
    
    /**
     * Enhanced performance profiler with detailed metrics
     */
    class AdvancedProfiler {
    public:
        AdvancedProfiler() : startTime_(std::chrono::high_resolution_clock::now()),
                           totalSteps_(0), peakMemoryMB_(0), samples_(0) {}
        
        ~AdvancedProfiler() = default;
        
        /**
         * Record a simulation step with detailed metrics
         */
        void recordStep(size_t step, const Brain* brain, double stepTimeMs,
                       size_t spikes, size_t activeNeurons) {
            auto now = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(now - startTime_).count();
            
            StepRecord record;
            record.stepNumber = step;
            record.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            record.elapsedTime = elapsed;
            record.stepTimeMs = stepTimeMs;
            record.spikes = spikes;
            record.activeNeurons = activeNeurons;
            
            // Capture brain state if available
            if (brain) {
                record.neuronCount = brain->getTotalNeuronCount();
                record.synapseCount = brain->getTotalSynapseCount();
                record.firingNeurons = brain->getFiringNeuronCount();
                record.totalSpikes = brain->getTotalSpikeCount();
                record.avgFiringRate = brain->getAverageFiringRate();
                record.eI = brain->getExcitationInhibitionRatio();
                
                // Memory systems
                auto workingMem = brain->getWorkingMemory();
                auto episodicMem = brain->getEpisodicMemory();
                auto assocMem = brain->getAssociativeMemory();
                
                record.workingMemoryActive = workingMem ? workingMem->getActiveTraces() : 0;
                record.episodicMemoryCount = episodicMem ? episodicMem->getEpisodeCount() : 0;
                record.associativeMemorySize = assocMem ? assocMem->getAssociationCount() : 0;
                
                // Neuromodulation
                auto dopamine = brain->getDopamine();
                auto curiosity = brain->getCuriosity();
                auto novelty = brain->getNovelty();
                auto predError = brain->getPredictionErrorSignal();
                
                record.dopamineLevel = dopamine ? dopamine->getLevel() : 0.0f;
                record.curiosityLevel = curiosity ? curiosity->getLevel() : 0.0f;
                record.noveltyLevel = novelty ? novelty->getLevel() : 0.0f;
                record.predictionError = predError ? predError->getLevel() : 0.0f;
                
                // Development
                record.developmentalStage = std::to_string(static_cast<int>(brain->getDevelopmentalStage()));
                record.developmentalAge = brain->getTotalSynapseCount(); // Use as proxy
            }
            
            stepRecords_.push_back(record);
            totalSteps_ = step;
            samples_++;
            
            // Update peak memory
            size_t currentMemory = getCurrentMemoryMB();
            if (currentMemory > peakMemoryMB_) {
                peakMemoryMB_ = currentMemory;
            }
        }
        
        /**
         * Get performance summary
         */
        std::string getPerformanceSummary() const {
            std::stringstream ss;
            
            if (stepRecords_.empty()) {
                return "No performance data recorded.";
            }
            
            size_t totalSteps = stepRecords_.back().stepNumber;
            double totalTime = stepRecords_.back().elapsedTime;
            double stepsPerSec = totalSteps / totalTime;
            double avgStepTime = totalTime / stepRecords_.size();
            
            size_t totalSpikes = 0;
            size_t maxSpikes = 0;
            size_t maxActiveNeurons = 0;
            
            for (const auto& record : stepRecords_) {
                totalSpikes += record.spikes;
                if (record.spikes > maxSpikes) maxSpikes = record.spikes;
                if (record.activeNeurons > maxActiveNeurons) maxActiveNeurons = record.activeNeurons;
            }
            
            double avgSpikesPerStep = static_cast<double>(totalSpikes) / stepRecords_.size();
            
            ss << "=== NLM Performance Summary ===" << std::endl;
            ss << std::fixed << std::setprecision(3);
            ss << "Total Steps: " << totalSteps << std::endl;
            ss << "Total Time: " << totalTime << " seconds" << std::endl;
            ss << "Steps per Second: " << stepsPerSec << std::endl;
            ss << "Average Step Time: " << avgStepTime * 1000 << " ms" << std::endl;
            ss << "Total Spikes: " << totalSpikes << std::endl;
            ss << "Average Spikes/Step: " << avgSpikesPerStep << std::endl;
            ss << "Max Spikes/Step: " << maxSpikes << std::endl;
            ss << "Max Active Neurons: " << maxActiveNeurons << std::endl;
            ss << "Peak Memory Usage: " << peakMemoryMB_ << " MB" << std::endl;
            ss << "Samples Recorded: " << samples_ << std::endl;
            
            return ss.str();
        }
        
        /**
         * Get step-by-step records
         */
        const std::vector<StepRecord>& getStepRecords() const { return stepRecords_; }
        
        /**
         * Get current memory usage
         */
        static size_t getCurrentMemoryMB() {
            // Platform-specific memory detection
            #ifdef __linux__
                return getLinuxMemoryUsage();
            #elif __APPLE__
                return getMacMemoryUsage();
            #elif _WIN32
                return getWindowsMemoryUsage();
            #else
                return 0; // Not implemented for other platforms
            #endif
        }
        
        /**
         * Export performance data to CSV
         */
        bool exportCSV(const std::string& filename) const {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << "Step,TimeMs,ElapsedSec,Spikes,ActiveNeurons,NeuronCount,SynapseCount,";
            file << "FiringNeurons,TotalSpikes,AvgFiringRate,ExcInhibitionRatio,WorkingMemoryActive,";
            file << "EpisodicMemoryCount,AssociativeMemorySize,DopamineLevel,CuriosityLevel,NoveltyLevel,PredictionError" << std::endl;
            
            for (const auto& record : stepRecords_) {
                file << record.stepNumber << ","
                     << record.stepTimeMs << ","
                     << record.elapsedTime << ","
                     << record.spikes << ","
                     << record.activeNeurons << ","
                     << record.neuronCount << ","
                     << record.synapseCount << ","
                     << record.firingNeurons << ","
                     << record.totalSpikes << ","
                     << record.avgFiringRate << ","
                     << record.eI << ","
                     << record.workingMemoryActive << ","
                     << record.episodicMemoryCount << ","
                     << record.associativeMemorySize << ","
                     << record.dopamineLevel << ","
                     << record.curiosityLevel << ","
                     << record.noveltyLevel << ","
                     << record.predictionError << std::endl;
            }
            
            file.close();
            return true;
        }
        
    private:
        struct StepRecord {
            uint64_t stepNumber;
            uint64_t timestamp;
            double elapsedTime;
            double stepTimeMs;
            size_t spikes;
            size_t activeNeurons;
            
            // Neural state
            size_t neuronCount;
            size_t synapseCount;
            size_t firingNeurons;
            size_t totalSpikes;
            float avgFiringRate;
            float eI;
            
            // Memory state
            size_t workingMemoryActive;
            size_t episodicMemoryCount;
            size_t associativeMemorySize;
            
            // Neuromodulation
            float dopamineLevel;
            float curiosityLevel;
            float noveltyLevel;
            float predictionError;
            
            // Development
            std::string developmentalStage;
            size_t developmentalAge;
        };
        
        std::chrono::high_resolution_clock::time_point startTime_;
        std::vector<StepRecord> stepRecords_;
        uint64_t totalSteps_;
        size_t peakMemoryMB_;
        size_t samples_;
        
        static size_t getLinuxMemoryUsage() {
            // Parse /proc/self/status or /proc/self/statm
            std::ifstream file("/proc/self/status");
            if (!file.is_open()) return 0;
            
            std::string line;
            while (std::getline(file, line)) {
                if (line.compare(0, 6, "VmRSS:") == 0) {
                    size_t kb = std::stoul(line.substr(6));
                    return kb / 1024; // Convert to MB
                }
            }
            return 0;
        }
        
        static size_t getMacMemoryUsage() {
            // Implementation for macOS (using ps or sysctl)
            return 0; // Placeholder
        }
        
        static size_t getWindowsMemoryUsage() {
            // Implementation for Windows (using GetProcessMemoryInfo)
            return 0; // Placeholder
        }
    };
    
    /**
     * System debugger for detailed state inspection
     */
    class SystemDebugger {
    public:
        SystemDebugger() : brain_(nullptr), enabled_(false), verbose_(false) {}
        
        void attachBrain(const std::shared_ptr<Brain>& brain) {
            brain_ = brain;
        }
        
        void enable(bool verbose = false) {
            enabled_ = true;
            verbose_ = verbose;
        }
        
        void disable() {
            enabled_ = false;
        }
        
        SystemSnapshot captureSnapshot(const std::string& description = "") {
            SystemSnapshot snapshot;
            snapshot.timestamp = std::time(nullptr);
            snapshot.stepNumber = brain_ ? brain_->getTotalSpikeCount() : 0; // Use as proxy for step
            snapshot.simulationTime = brain_ ? brain_->getAverageFiringRate() * 100 : 0; // Use as proxy
            
            if (brain_) {
                snapshot.totalNeurons = brain_->getTotalNeuronCount();
                snapshot.totalSynapses = brain_->getTotalSynapseCount();
                snapshot.activeNeurons = brain_->getActiveNeuronCount();
                snapshot.firingNeurons = brain_->getFiringNeuronCount();
                snapshot.totalSpikes = brain_->getTotalSpikeCount();
                snapshot.averageFiringRate = brain_->getAverageFiringRate();
                snapshot.excitationInhibitionRatio = brain_->getExcitationInhibitionRatio();
                
                auto workingMem = brain_->getWorkingMemory();
                auto episodicMem = brain_->getEpisodicMemory();
                auto assocMem = brain_->getAssociativeMemory();
                
                snapshot.workingMemoryActiveTraces = workingMem ? workingMem->getActiveTraces() : 0;
                snapshot.episodicMemoryEpisodes = episodicMem ? episodicMem->getEpisodeCount() : 0;
                snapshot.associativeMemoryAssociations = assocMem ? assocMem->getAssociationCount() : 0;
                
                auto dopamine = brain_->getDopamine();
                auto curiosity = brain_->getCuriosity();
                auto novelty = brain_->getNovelty();
                auto predError = brain_->getPredictionErrorSignal();
                
                snapshot.dopamineLevel = dopamine ? dopamine->getLevel() : 0.0f;
                snapshot.curiosityLevel = curiosity ? curiosity->getLevel() : 0.0f;
                snapshot.noveltyLevel = novelty ? novelty->getLevel() : 0.0f;
                snapshot.predictionError = predError ? predError->getLevel() : 0.0f;
                
                snapshot.developmentalStage = std::to_string(static_cast<int>(brain_->getDevelopmentalStage()));
            }
            
            if (enabled_) {
                std::cout << "=== System Snapshot ===" << std::endl;
                if (!description.empty()) {
                    std::cout << "Description: " << description << std::endl;
                }
                std::cout << snapshot.toString(verbose_) << std::endl;
                std::cout << "========================" << std::endl;
            }
            
            return snapshot;
        }
        
        /**
         * Compare two snapshots and identify changes
         */
        std::string compareSnapshots(const SystemSnapshot& before, const SystemSnapshot& after) {
            std::stringstream ss;
            
            ss << "=== System Snapshot Comparison ===" << std::endl;
            ss << std::fixed << std::setprecision(3);
            
            // Neural state changes
            ss << "\nNEURAL STATE:" << std::endl;
            ss << "  Neurons: " << before.totalNeurons << " -> " << after.totalNeurons 
               << " (" << (after.totalNeurons - before.totalNeurons) << ")" << std::endl;
            ss << "  Synapses: " << before.totalSynapses << " -> " << after.totalSynapses
               << " (" << (after.totalSynapses - before.totalSynapses) << ")" << std::endl;
            ss << "  Active Neurons: " << before.activeNeurons << " -> " << after.activeNeurons
               << " (" << (after.activeNeurons - before.activeNeurons) << ")" << std::endl;
            ss << "  Total Spikes: " << before.totalSpikes << " -> " << after.totalSpikes
               << " (" << (after.totalSpikes - before.totalSpikes) << ")" << std::endl;
            ss << "  Average Firing Rate: " << before.averageFiringRate << " -> " << after.averageFiringRate
               << " (" << (after.averageFiringRate - before.averageFiringRate) << ")" << std::endl;
            
            // Memory state changes
            ss << "\nMEMORY STATE:" << std::endl;
            ss << "  Working Memory Active: " << before.workingMemoryActiveTraces << " -> " << after.workingMemoryActiveTraces
               << " (" << (after.workingMemoryActiveTraces - before.workingMemoryActiveTraces) << ")" << std::endl;
            ss << "  Episodic Memory Episodes: " << before.episodicMemoryEpisodes << " -> " << after.episodicMemoryEpisodes
               << " (" << (after.episodicMemoryEpisodes - before.episodicMemoryEpisodes) << ")" << std::endl;
            
            // Neuromodulation changes
            ss << "\nNEUROMODULATION:" << std::endl;
            ss << "  Dopamine: " << before.dopamineLevel << " -> " << after.dopamineLevel
               << " (" << (after.dopamineLevel - before.dopamineLevel) << ")" << std::endl;
            ss << "  Curiosity: " << before.curiosityLevel << " -> " << after.curiosityLevel
               << " (" << (after.curiosityLevel - before.curiosityLevel) << ")" << std::endl;
            
            ss << "\n==============================" << std::endl;
            
            return ss.str();
        }
        
        /**
         * Debug specific brain component
         */
        std::string debugComponent(const std::string& component) {
            std::stringstream ss;
            
            if (!brain_) {
                return "No brain attached for debugging.";
            }
            
            ss << "=== NLM Component Debug: " << component << " ===" << std::endl;
            
            if (component == "brain") {
                ss << "Brain Summary:" << std::endl;
                ss << "  Regions: " << brain_->getRegionCount() << std::endl;
                ss << "  Total Neurons: " << brain_->getTotalNeuronCount() << std::endl;
                ss << "  Total Synapses: " << brain_->getTotalSynapseCount() << std::endl;
                ss << "  Total Spikes: " << brain_->getTotalSpikeCount() << std::endl;
                ss << "  Firing Neurons: " << brain_->getFiringNeuronCount() << std::endl;
                ss << "  Average Firing Rate: " << brain_->getAverageFiringRate() << std::endl;
                ss << "  E/I Ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
                
            } else if (component == "memory") {
                ss << "Memory Systems:" << std::endl;
                auto workingMem = brain_->getWorkingMemory();
                auto episodicMem = brain_->getEpisodicMemory();
                auto assocMem = brain_->getAssociativeMemory();
                
                ss << "  Working Memory: " << (workingMem ? "Enabled" : "Disabled") << std::endl;
                if (workingMem) {
                    ss << "    Active Traces: " << workingMem->getActiveTraces() << std::endl;
                    ss << "    Capacity: " << workingMem->getCapacity() << std::endl;
                }
                
                ss << "  Episodic Memory: " << (episodicMem ? "Enabled" : "Disabled") << std::endl;
                if (episodicMem) {
                    ss << "    Episode Count: " << episodicMem->getEpisodeCount() << std::endl;
                    ss << "    Max Episodes: " << episodicMem->getMaxEpisodes() << std::endl;
                }
                
                ss << "  Associative Memory: " << (assocMem ? "Enabled" : "Disabled") << std::endl;
                if (assocMem) {
                    ss << "    Association Count: " << assocMem->getAssociationCount() << std::endl;
                }
                
            } else if (component == "neuromodulation") {
                ss << "Neuromodulation Systems:" << std::endl;
                auto dopamine = brain_->getDopamine();
                auto curiosity = brain_->getCuriosity();
                auto novelty = brain_->getNovelty();
                auto predError = brain_->getPredictionErrorSignal();
                
                ss << "  Dopamine: " << (dopamine ? "Enabled" : "Disabled") << std::endl;
                if (dopamine) {
                    ss << "    Level: " << dopamine->getLevel() << std::endl;
                    ss << "    Plasticity Factor: " << dopamine->getPlasticityFactor() << std::endl;
                }
                
                ss << "  Curiosity: " << (curiosity ? "Enabled" : "Disabled") << std::endl;
                if (curiosity) {
                    ss << "    Level: " << curiosity->getLevel() << std::endl;
                }
                
                ss << "  Novelty: " << (novelty ? "Enabled" : "Disabled") << std::endl;
                if (novelty) {
                    ss << "    Level: " << novelty->getLevel() << std::endl;
                }
                
                ss << "  Prediction Error: " << (predError ? "Enabled" : "Disabled") << std::endl;
                
            } else if (component == "development") {
                ss << "Development System:" << std::endl;
                ss << "  Developmental Stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
                ss << "  Development System: " << (brain_->getDevelopmentSystem() ? "Enabled" : "Disabled") << std::endl;
                
            } else {
                ss << "Unknown component: " << component << std::endl;
                ss << "Available components: brain, memory, neuromodulation, development" << std::endl;
            }
            
            ss << "=========================" << std::endl;
            
            return ss.str();
        }
        
    private:
        std::shared_ptr<Brain> brain_;
        bool enabled_;
        bool verbose_;
    };
    
    /**
     * Configuration validator for debugging configuration issues
     */
    class ConfigurationValidator {
    public:
        ConfigurationValidator() = default;
        
        std::string validate(const std::shared_ptr<Config>& config) {
            std::stringstream ss;
            ss << "=== NLM Configuration Validation ===" << std::endl;
            
            if (!config) {
                ss << "ERROR: No configuration provided." << std::endl;
                return ss.str();
            }
            
            // Check required keys
            std::vector<std::string> requiredKeys = {
                "random_seed",
                "simulation_timestep",
                "neuron_count",
                "region_count",
                "connection_probability"
            };
            
            bool allValid = true;
            for (const auto& key : requiredKeys) {
                if (config->has(key)) {
                    ss << "✓ " << key << ": Present" << std::endl;
                } else {
                    ss << "✗ " << key << ": Missing" << std::endl;
                    allValid = false;
                }
            }
            
            // Check for obvious issues
            ss << "\n=== Issue Detection ===" << std::endl;
            
            if (config->has("random_seed")) {
                auto seedOpt = config->get<int64_t>("random_seed");
                if (seedOpt) {
                    ss << "Random Seed: " << *seedOpt << std::endl;
                    if (*seedOpt == 0) {
                        ss << "  WARNING: Random seed is 0 - results may not be reproducible" << std::endl;
                    }
                }
            }
            
            if (config->has("simulation_timestep")) {
                auto timestepOpt = config->get<double>("simulation_timestep");
                if (timestepOpt) {
                    ss << "Simulation Timestep: " << *timestepOpt << "s" << std::endl;
                    if (*timestepOpt <= 0.0) {
                        ss << "  WARNING: Timestep should be positive" << std::endl;
                    }
                }
            }
            
            if (config->has("neuron_count")) {
                auto neuronOpt = config->get<int64_t>("neuron_count");
                if (neuronOpt) {
                    ss << "Neuron Count: " << *neuronOpt << std::endl;
                    if (*neuronOpt < 1) {
                        ss << "  ERROR: Neuron count must be at least 1" << std::endl;
                        allValid = false;
                    } else if (*neuronOpt > 10000) {
                        ss << "  WARNING: Large neuron count may impact performance" << std::endl;
                    }
                }
            }
            
            ss << "\n=== Validation Result ===" << std::endl;
            if (allValid) {
                ss << "✓ Configuration appears valid." << std::endl;
            } else {
                ss << "✗ Configuration has issues that may affect functionality." << std::endl;
            }
            
            ss << "===========================" << std::endl;
            
            return ss.str();
        }
        
        /**
         * Show configuration differences
         */
        std::string compareConfigs(const std::shared_ptr<Config>& config1,
                                  const std::shared_ptr<Config>& config2) {
            std::stringstream ss;
            ss << "=== Configuration Comparison ===" << std::endl;
            
            auto keys1 = config1->getKeys();
            auto keys2 = config2->getKeys();
            
            ss << "Configuration 1 keys: " << keys1.size() << std::endl;
            ss << "Configuration 2 keys: " << keys2.size() << std::endl;
            
            // Find common keys
            std::set<std::string> commonKeys;
            for (const auto& key : keys1) {
                if (std::find(keys2.begin(), keys2.end(), key) != keys2.end()) {
                    commonKeys.insert(key);
                }
            }
            
            ss << "Common keys: " << commonKeys.size() << std::endl;
            
            // Show differences for common keys
            for (const auto& key : commonKeys) {
                auto val1 = config1->get<std::string>(key);
                auto val2 = config2->get<std::string>(key);
                
                if (val1 != val2) {
                    ss << "DIFFERENCE: " << key << std::endl;
                    ss << "  Config1: " << (val1.has_value() ? *val1 : "(empty)") << std::endl;
                    ss << "  Config2: " << (val2.has_value() ? *val2 : "(empty)") << std::endl;
                }
            }
            
            ss << "=========================" << std::endl;
            
            return ss.str();
        }
    };
}}

/**
 * Convenience functions for easy debugging
 */
namespace Debug {
    
    /**
     * Print system state summary
     */
    inline void printSystemState(const std::shared_ptr<Brain>& brain, 
                                const std::string& prefix = "System State") {
        std::cout << "=== " << prefix << " ===" << std::endl;
        if (brain) {
            std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
            std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
            std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
            std::cout << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
            
            // Memory systems
            auto workingMem = brain->getWorkingMemory();
            auto episodicMem = brain->getEpisodicMemory();
            auto assocMem = brain->getAssociativeMemory();
            
            if (workingMem) {
                std::cout << "Working Memory Active Traces: " << workingMem->getActiveTraces() << std::endl;
            }
            if (episodicMem) {
                std::cout << "Episodic Memory Episodes: " << episodicMem->getEpisodeCount() << std::endl;
            }
            if (assocMem) {
                std::cout << "Associative Memory Associations: " << assocMem->getAssociationCount() << std::endl;
            }
        } else {
            std::cout << "No brain attached." << std::endl;
        }
        std::cout << "=========================" << std::endl;
    }
    
    /**
     * Format performance metrics for output
     */
    inline std::string formatPerformance(size_t steps, double timeSec, size_t spikes) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Performance: " << steps << " steps in " << timeSec << "s ("
           << (timeSec > 0 ? steps / timeSec : 0) << " steps/s)" << std::endl;
        ss << "Spikes: " << spikes << " (" << (steps > 0 ? spikes / steps : 0) << " spikes/step)" << std::endl;
        return ss.str();
    }
    
    /**
     * Check for potential issues in brain state
     */
    inline std::string checkBrainHealth(const std::shared_ptr<Brain>& brain) {
        std::stringstream ss;
        ss << "=== Brain Health Check ===" << std::endl;
        
        if (!brain) {
            ss << "ERROR: No brain attached for health check." << std::endl;
            return ss.str();
        }
        
        bool healthy = true;
        
        // Check neuron count
        size_t neuronCount = brain->getTotalNeuronCount();
        if (neuronCount == 0) {
            ss << "ERROR: Brain has no neurons." << std::endl;
            healthy = false;
        } else if (neuronCount > 10000) {
            ss << "WARNING: High neuron count may impact performance." << std::endl;
        }
        
        // Check synapse count
        size_t synapseCount = brain->getTotalSynapseCount();
        if (synapseCount == 0 && neuronCount > 1) {
            ss << "WARNING: No synapses despite having neurons." << std::endl;
            healthy = false;
        }
        
        // Check firing activity
        size_t totalSpikes = brain->getTotalSpikeCount();
        if (totalSpikes == 0) {
            ss << "INFO: No spikes recorded yet." << std::endl;
        }
        
        // Check E/I ratio
        float eiRatio = brain->getExcitationInhibitionRatio();
        if (eiRatio > 100.0f) {
            ss << "WARNING: High E/I ratio indicates imbalance." << std::endl;
            healthy = false;
        } else if (eiRatio < 0.01f) {
            ss << "WARNING: Low E/I ratio may indicate inhibition dominance." << std::endl;
        }
        
        if (healthy) {
            ss << "✓ Brain appears healthy." << std::endl;
        }
        
        ss << "=============================" << std::endl;
        
        return ss.str();
    }
}

#endif // DEBUG_UTILITIES_H