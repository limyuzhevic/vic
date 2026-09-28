#include "BrainTools.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <filesystem>

namespace nlm {
namespace cli {
namespace brain {

std::shared_ptr<Brain> BrainCreator::createBrain(const std::string& configFile) {
    try {
        CLIUtils::printInfo("Creating brain with configuration: " + configFile);
        
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            CLIUtils::printWarning("Using default configuration (file not found)");
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw CLIError("Failed to initialize brain");
        }
        
        CLIUtils::printSuccess("Brain created successfully");
        return brain;
    } catch (const std::exception& e) {
        CLIUtils::printError("Brain creation failed: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<Brain> BrainCreator::createBrainWithParams(const std::string& params) {
    // TODO: Parse params string and configure brain accordingly
    CLIUtils::printWarning("Parameter parsing not yet implemented");
    return createBrain();
}

bool BrainCreator::saveBrain(const Brain& brain, const std::string& filepath) {
    try {
        CLIUtils::printInfo("Saving brain to: " + filepath);
        if (brain.save(filepath)) {
            CLIUtils::printSuccess("Brain saved successfully");
            return true;
        } else {
            CLIUtils::printError("Failed to save brain");
            return false;
        }
    } catch (const std::exception& e) {
        CLIUtils::printError("Brain save failed: " + std::string(e.what()));
        return false;
    }
}

std::shared_ptr<Brain> BrainCreator::loadBrain(const std::string& filepath) {
    try {
        CLIUtils::printInfo("Loading brain from: " + filepath);
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->load(filepath)) {
            throw CLIError("Failed to load brain from file");
        }
        
        CLIUtils::printSuccess("Brain loaded successfully");
        return brain;
    } catch (const std::exception& e) {
        CLIUtils::printError("Brain load failed: " + std::string(e.what()));
        return nullptr;
    }
}

void BrainCreator::displayBrainInfo(const Brain& brain) {
    CLIUtils::printHeader("Brain Information");
    brain.logStatus();
}

std::string BrainCreator::brainStatistics(const Brain& brain) {
    std::stringstream ss;
    ss << "=== Brain Statistics ===\n"
       << "Neurons: " << brain.getTotalNeuronCount() << "\n"
       << "Synapses: " << brain.getTotalSynapseCount() << "\n"
       << "Spikes: " << brain.getTotalSpikeCount() << "\n"
       << "Active Neurons: " << brain.getActiveNeuronCount() << "\n"
       << "Firing Neurons: " << brain.getFiringNeuronCount() << "\n"
       << "Average Firing Rate: " << brain.getAverageFiringRate() << " Hz\n"
       << "Excitatory/Inhibitory Ratio: " << brain.getExcitationInhibitionRatio() << "\n"
       << "Regions: " << brain.getRegionCount() << std::endl;
    return ss.str();
}

bool BrainManager::createBrain(const std::string& configFile, const std::string& brainName) {
    std::string filename = brainName + "_brain.bin";
    auto brain = BrainCreator::createBrain(configFile);
    if (!brain) return false;
    
    if (BrainCreator::saveBrain(*brain, filename)) {
        CLIUtils::printSuccess("Brain saved as: " + filename);
        return true;
    }
    return false;
}

bool BrainManager::loadBrain(const std::string& filepath) {
    auto brain = BrainCreator::loadBrain(filepath);
    if (!brain) return false;
    
    // Store for later operations
    static std::shared_ptr<Brain> currentBrain = brain;
    CLIUtils::printSuccess("Brain loaded and set as current");
    return true;
}

bool BrainManager::saveBrain(const std::string& filepath, bool includeSnapshots) {
    // For now, just save the brain
    auto brain = BrainCreator::loadBrain(filepath);
    if (!brain) return false;
    
    std::string fullPath = filepath + (CLIUtils::endsWith(filepath, ".bin") ? "" : ".bin");
    return BrainCreator::saveBrain(*brain, fullPath);
}

bool BrainManager::runSimulation(const std::string& configFile, size_t steps, double timestep) {
    CLIUtils::printInfo("Running simulation: " + std::to_string(steps) + " steps, timestep: " + std::to_string(timestep) + "s");
    
    auto config = std::make_shared<Config>();
    config->loadFromFile(configFile);
    config->set("simulation_timestep", timestep, ConfigSource::Runtime);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        CLIUtils::printError("Failed to initialize brain for simulation");
        return false;
    }
    
    CLITiming timer;
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * timestep);
        
        if (step % 100 == 0) {
            CLIUtils::printInfo("Step " + std::to_string(step) + "/" + std::to_string(steps));
        }
    }
    
    auto duration = timer.elapsed();
    CLIUtils::printSuccess("Simulation completed in " + CLIUtils::formatDuration(duration));
    BrainCreator::displayBrainInfo(*brain);
    return true;
}

bool BrainManager::analyzeBrain(const std::string& configFile, const std::string& analysisType) {
    auto brain = BrainCreator::createBrain(configFile);
    if (!brain) return false;
    
    if (analysisType == "comprehensive") {
        BrainAnalyzer::analyzeConnectivity(*brain);
        BrainAnalyzer::analyzePlasticity(*brain, 100);
        BrainAnalyzer::analyzeLearning(*brain, 500);
    } else if (analysisType == "connectivity") {
        BrainAnalyzer::analyzeConnectivity(*brain);
    } else if (analysisType == "plasticity") {
        BrainAnalyzer::analyzePlasticity(*brain, 100);
    } else if (analysisType == "learning") {
        BrainAnalyzer::analyzeLearning(*brain, 500);
    } else {
        CLIUtils::printError("Unknown analysis type: " + analysisType);
        return false;
    }
    
    return true;
}

void BrainManager::listBrains(const std::string& directory) {
    CLIUtils::printHeader("Available Brains");
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && CLIUtils::endsWith(entry.path().string(), ".bin")) {
            auto fileSize = entry.file_size();
            std::cout << entry.path().filename().string() 
                     << " (" << CLIUtils::humanReadableSize(fileSize) << ")" << std::endl;
        }
    }
}

bool BrainManager::deleteBrain(const std::string& brainFile) {
    std::string fullPath = brainFile;
    if (!CLIUtils::endsWith(fullPath, ".bin")) {
        fullPath += ".bin";
    }
    
    if (std::filesystem::remove(fullPath)) {
        CLIUtils::printSuccess("Brain deleted: " + brainFile);
        return true;
    } else {
        CLIUtils::printError("Brain file not found: " + brainFile);
        return false;
    }
}

BrainProfiler::PerformanceMetrics BrainProfiler::profileBrain(const Brain& brain) {
    PerformanceMetrics metrics;
    metrics.neuronCount = brain.getTotalNeuronCount();
    metrics.synapseCount = brain.getTotalSynapseCount();
    metrics.spikeCount = brain.getTotalSpikeCount();
    metrics.activeNeurons = brain.getActiveNeuronCount();
    metrics.firingRate = brain.getAverageFiringRate();
    metrics.excitationInhibitionRatio = brain.getExcitationInhibitionRatio();
    
    // Memory usage estimation (simplified)
    metrics.memoryUsage = brain.getTotalNeuronCount() * sizeof(Neuron) +
                         brain.getTotalSynapseCount() * sizeof(Synapse);
    
    return metrics;
}

void BrainProfiler::printDetailedStats(const Brain& brain) {
    auto metrics = profileBrain(brain);
    CLIUtils::printHeader("Detailed Brain Statistics");
    
    std::cout << "Performance Metrics:" << std::endl
              << "  Neurons: " << metrics.neuronCount << std::endl
              << "  Synapses: " << metrics.synapseCount << std::endl
              << "  Total Spikes: " << metrics.spikeCount << std::endl
              << "  Active Neurons: " << metrics.activeNeurons << std::endl
              << "  Firing Rate: " << metrics.firingRate << " Hz" << std::endl
              << "  E/I Ratio: " << metrics.excitationInhibitionRatio << std::endl
              << "  Memory Usage: " << CLIUtils::humanReadableSize(metrics.memoryUsage) << std::endl;
}

void BrainProfiler::saveStats(const Brain& brain, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw CLIError("Cannot open file for writing: " + filepath);
    }
    
    auto metrics = profileBrain(brain);
    file << "NLM Brain Profile" << std::endl;
    file << "=================" << std::endl << std::endl;
    file << "Generated: " << CLIUtils::currentTimestamp() << std::endl << std::endl;
    file << "Metrics:" << std::endl
         << "  Neuron Count: " << metrics.neuronCount << std::endl
         << "  Synapse Count: " << metrics.synapseCount << std::endl
         << "  Total Spikes: " << metrics.spikeCount << std::endl
         << "  Active Neurons: " << metrics.activeNeurons << std::endl
         << "  Average Firing Rate: " << metrics.firingRate << " Hz" << std::endl
         << "  Excitation/Inhibition Ratio: " << metrics.excitationInhibitionRatio << std::endl
         << "  Estimated Memory Usage: " << CLIUtils::humanReadableSize(metrics.memoryUsage) << std::endl;
    
    file.close();
    CLIUtils::printSuccess("Statistics saved to: " + filepath);
}

std::vector<BrainProfiler::PerformanceMetrics> BrainProfiler::profileSimulation(
    const std::string& configFile, size_t steps) {
    auto config = std::make_shared<Config>();
    config->loadFromFile(configFile);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw CLIError("Failed to initialize brain for profiling");
    }
    
    std::vector<PerformanceMetrics> metricsList;
    metricsList.reserve(steps / 100); // Estimate
    
    for (size_t step = 0; step < steps; step += 100) {
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            metricsList.push_back(profileBrain(*brain));
        }
    }
    
    return metricsList;
}

bool BrainCheckpoint::createCheckpoint(const Brain& brain, const std::string& label) {
    std::string filename = label.empty() ? ("checkpoint_" + CLIUtils::currentTimestamp() + ".bin") : (label + ".bin");
    
    if (brain.save(filename)) {
        CLIUtils::printSuccess("Checkpoint created: " + filename);
        return true;
    }
    return false;
}

bool BrainCheckpoint::restoreCheckpoint(const std::string& label) {
    std::string filename = label.empty() ? "latest.bin" : (label + ".bin");
    auto brain = BrainCreator::loadBrain(filename);
    return brain != nullptr;
}

bool BrainCheckpoint::listCheckpoints(const std::string& directory) {
    CLIUtils::printHeader("Available Checkpoints");
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string name = entry.path().filename().string();
            if (CLIUtils::endsWith(name, ".bin")) {
                auto fileSize = entry.file_size();
                std::cout << name << " (" << CLIUtils::humanReadableSize(fileSize) << ")" << std::endl;
            }
        }
    }
    return true;
}

bool BrainCheckpoint::deleteCheckpoint(const std::string& label) {
    std::string filename = label + ".bin";
    return BrainManager::deleteBrain(filename);
}

std::string BrainCheckpoint::getCheckpointPath(const std::string& label) {
    return label + ".bin";
}

void BrainAnalyzer::analyzeConnectivity(const Brain& brain) {
    CLIUtils::printHeader("Connectivity Analysis");
    
    size_t totalRegions = brain.getRegionCount();
    std::cout << "Brain has " << totalRegions << " regions" << std::endl;
    
    for (size_t i = 0; i < totalRegions; ++i) {
        auto regionId = brain.getRegionIds()[i];
        auto* region = brain.getRegion(regionId);
        if (region) {
            size_t neuronCount = region->getAllNeurons().size();
            size_t synapseCount = region->getSynapses().size();
            std::cout << "  Region " << regionId << ": " 
                     << neuronCount << " neurons, " 
                     << synapseCount << " synapses" << std::endl;
        }
    }
}

void BrainAnalyzer::analyzePlasticity(const Brain& brain, SimulationStep steps) {
    CLIUtils::printHeader("Plasticity Analysis");
    
    // Record initial weights
    std::vector<float> initialWeights;
    if (auto* region = brain.getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    // Simulate plasticity
    for (SimulationStep step = 0; step < steps; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Record final weights
    std::vector<float> finalWeights;
    if (auto* region = brain.getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    // Analyze weight changes
    if (initialWeights.size() > 0 && finalWeights.size() > 0) {
        size_t strengthened = 0, weakened = 0, unchanged = 0;
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) strengthened++;
            else if (delta < -0.01f) weakened++;
            else unchanged++;
        }
        
        std::cout << "Plasticity Results:" << std::endl
                  << "  Strengthened connections: " << strengthened << std::endl
                  << "  Weakened connections: " << weakened << std::endl
                  << "  Unchanged connections: " << unchanged << std::endl
                  << "  Total connections: " << minSize << std::endl;
    }
}

void BrainAnalyzer::analyzeLearning(const Brain& brain, SimulationStep steps) {
    CLIUtils::printHeader("Learning Analysis");
    
    // Check spike activity
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Simulate learning
    for (SimulationStep step = 0; step < steps; ++step) {
        brain.step(step, step * 0.001);
    }
    
    size_t finalSpikes = brain.getTotalSpikeCount();
    size_t spikeIncrease = finalSpikes - initialSpikes;
    
    std::cout << "Learning Results:" << std::endl
              << "  Initial spikes: " << initialSpikes << std::endl
              << "  Final spikes: " << finalSpikes << std::endl
              << "  Spike increase: " << spikeIncrease << std::endl
              << "  Activity change: " << (spikeIncrease > 0 ? "INCREASED" : "DECREASED") << std::endl;
}

void BrainAnalyzer::analyzeDevelopment(const Brain& brain) {
    CLIUtils::printHeader("Development Analysis");
    
    auto stage = brain.getDevelopmentalStage();
    std::cout << "Brain developmental stage: " << static_cast<int>(stage) << std::endl;
    
    // Analyze region properties
    size_t regionCount = brain.getRegionCount();
    std::cout << "Number of regions: " << regionCount << std::endl;
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto regionId = brain.getRegionIds()[i];
        auto* region = brain.getRegion(regionId);
        if (region) {
            auto neurons = region->getAllNeurons();
            size_t activeCount = 0;
            for (const auto& neuron : neurons) {
                if (neuron->isActive()) activeCount++;
            }
            std::cout << "  Region " << regionId << ": " 
                     << neurons.size() << " neurons, "
                     << activeCount << " active (" 
                     << (neurons.size() > 0 ? (100.0 * activeCount / neurons.size()) : 0) << "%)" << std::endl;
        }
    }
}

void BrainAnalyzer::saveAnalysisResults(const std::string& results, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw CLIError("Cannot open file for writing: " + filepath);
    }
    
    file << results << std::endl;
    file.close();
    
    CLIUtils::printSuccess("Analysis results saved to: " + filepath);
}

} // namespace brain
} // namespace cli
} // namespace nlm
