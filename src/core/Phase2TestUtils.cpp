// Phase 2 Test Utilities Implementation
#include "core/Phase2TestUtils.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <ctime>

namespace nlm {
namespace phase2 {
namespace TestUtils {

    // Helper function implementations for test utilities

    std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
        // Simplified brain creation - in practice this would involve proper initialization
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        return brain;
    }

    void applyInputCurrent(std::shared_ptr<Brain> brain, size_t neuronCount, 
                          NeuronType type, double current) {
        if (!brain) return;
        
        auto neurons = brain->getAllNeurons();
        size_t count = std::min(neuronCount, neurons.size());
        
        for (size_t i = 0; i < count; ++i) {
            if (type == NeuronType::Sensory || type == NeuronType::Internal) {
                neurons[i]->injectCurrent(static_cast<MembranePotential>(current));
            }
        }
    }

    NetworkStats calculateNetworkStats(std::shared_ptr<Brain> brain) {
        NetworkStats stats;
        if (!brain) return stats;
        
        auto neurons = brain->getAllNeurons();
        stats.totalNeurons = neurons.size();
        stats.activeNeurons = brain->getFiringNeuronCount();
        stats.totalSpikes = brain->getTotalSpikeCount();
        
        if (stats.activeNeurons > 0) {
            stats.averageFiringRate = static_cast<double>(stats.totalSpikes) / stats.activeNeurons;
        }
        
        // Calculate average synaptic weight
        float totalWeight = 0.0f;
        size_t synapseCount = 0;
        
        auto regions = brain->getRegions();
        for (auto* region : regions) {
            if (region) {
                for (const auto& syn : region->getSynapses()) {
                    totalWeight += syn->getWeight();
                    synapseCount++;
                }
            }
        }
        
        if (synapseCount > 0) {
            stats.averageWeight = totalWeight / synapseCount;
            stats.neuronWeights.resize(synapseCount);
        }
        
        return stats;
    }

    std::string formatVector(const std::vector<float>& values, int precision) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision);
        
        oss << "[";
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << values[i];
        }
        oss << "]";
        
        return oss.str();
    }

    bool approximatelyEqual(double a, double b, double epsilon) {
        return std::abs(a - b) < epsilon;
    }

    bool learningOccurred(const std::vector<float>& initialWeights, 
                         const std::vector<float>& finalWeights,
                         double threshold) {
        if (initialWeights.empty() || finalWeights.empty()) {
            return false;
        }
        
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        return std::abs(finalMean - initialMean) > threshold;
    }

    WeightChangeStats calculateWeightChange(const std::vector<float>& initialWeights,
                                           const std::vector<float>& finalWeights) {
        WeightChangeStats stats;
        
        if (initialWeights.empty() || finalWeights.empty()) {
            return stats;
        }
        
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        
        stats.initialMean = initialSum / initialWeights.size();
        stats.finalMean = finalSum / finalWeights.size();
        stats.change = stats.finalMean - stats.initialMean;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++stats.strengthened;
            else if (delta < -0.01f) ++stats.weakened;
            else ++stats.unchanged;
        }
        
        return stats;
    }

    void createInputPattern(std::shared_ptr<Brain> brain, size_t neuronCount, 
                           double current) {
        applyInputCurrent(brain, neuronCount, NeuronType::Sensory, current);
    }

    void runSimulation(std::shared_ptr<Brain> brain, uint32_t steps, 
                      double timestep) {
        if (!brain) return;
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * timestep);
        }
    }

    void printBrainStatus(std::shared_ptr<Brain> brain, const std::string& title) {
        if (!brain) return;
        
        std::cout << "=== " << title << " ===" << std::endl;
        std::cout << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "=================================" << std::endl;
    }

    bool saveBrainCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath) {
        // Simplified checkpoint saving
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // In practice, this would serialize the brain state
        file << "CHECKPOINT_PLACEHOLDER" << std::endl;
        file.close();
        return true;
    }

    std::shared_ptr<Brain> loadBrainCheckpoint(const std::string& filepath, 
                                               std::shared_ptr<Config> config) {
        // Simplified checkpoint loading
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return nullptr;
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            return nullptr;
        }
        
        // In practice, this would deserialize the brain state
        std::string line;
        std::getline(file, line);
        file.close();
        
        return brain;
    }

    template<typename Func>
    double measureExecutionTime(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }

    void printFormatted(const std::string& label, double value, int precision) {
        std::cout << label << ": " << std::fixed << std::setprecision(precision) 
                  << value << std::endl;
    }

    void printFormatted(const std::string& label, uint64_t value) {
        std::cout << label << ": " << value << std::endl;
    }

    void printFormatted(const std::string& label, bool value) {
        std::cout << label << ": " << (value ? "Yes" : "No") << std::endl;
    }

    void printFormatted(const std::string& label, const std::string& value) {
        std::cout << label << ": " << value << std::endl;
    }

    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    std::string generateTestId(const std::string& prefix) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << prefix << "_" << time << "_" << std::this_thread::get_id();
        return ss.str();
    }

    bool validateConfig(const Config& config, std::vector<std::string>& errors) {
        if (!config.has("random_seed")) {
            errors.push_back("Missing random_seed");
        }
        if (!config.has("neuron_count")) {
            errors.push_back("Missing neuron_count");
        }
        if (!config.has("connection_probability")) {
            errors.push_back("Missing connection_probability");
        }
        return errors.empty();
    }

    std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return config;
    }

    void mergeConfigWithDefaults(Config& config) {
        // Apply defaults for missing values
        if (!config.has("random_seed")) {
            config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        }
        if (!config.has("simulation_timestep")) {
            config.set("simulation_timestep", 0.001, ConfigSource::Default);
        }
    }

    bool isBrainValid(std::shared_ptr<Brain> brain) {
        return brain && brain->initialize();
    }

    ConnectivityStats calculateConnectivityStats(std::shared_ptr<Brain> brain) {
        ConnectivityStats stats;
        if (!brain) return stats;
        
        auto regions = brain->getRegions();
        for (auto* region : regions) {
            if (region) {
                stats.totalSynapses += region->getSynapses().size();
                
                for (const auto& syn : region->getSynapses()) {
                    if (syn->getSynapseType() == SynapseType::Excitatory) {
                        stats.excitatorySynapses++;
                    } else {
                        stats.inhibitorySynapses++;
                    }
                }
            }
        }
        
        if (stats.totalSynapses > 0) {
            float totalWeight = 0.0f;
            auto regions = brain->getRegions();
            for (auto* region : regions) {
                if (region) {
                    for (const auto& syn : region->getSynapses()) {
                        totalWeight += syn->getWeight();
                    }
                }
            }
            stats.averageWeight = totalWeight / stats.totalSynapses;
            stats.connectionDensity = static_cast<float>(stats.totalSynapses) / 
                                     (brain->getTotalNeuronCount() * brain->getTotalNeuronCount());
        }
        
        return stats;
    }

} // namespace TestUtils

} // namespace phase2
} // namespace nlm