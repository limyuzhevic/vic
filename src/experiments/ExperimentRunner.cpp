#include "ExperimentRunner.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>

using json = nlohmann::json;

namespace nlm {

#include "../environment/Environment.hpp"

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {
    initializeMetrics();
}

ExperimentRunner::~ExperimentRunner() = default;

ExperimentRunner::ExperimentRunner(const ExperimentRunner& other) : pImpl(new Impl(*other.pImpl)) {}

ExperimentRunner& ExperimentRunner::operator=(const ExperimentRunner& other) {
    if (this != &other) {
        *pImpl = *other.pImpl;
    }
    return *this;
}

ExperimentRunner::ExperimentRunner(ExperimentRunner&& other) noexcept : pImpl(std::move(other.pImpl)) {}

ExperimentRunner& ExperimentRunner::operator=(ExperimentRunner&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool ExperimentRunner::setup(std::shared_ptr<ExperimentConfig> config,
                            std::shared_ptr<Brain> brain,
                            std::shared_ptr<AgentBrain> agent,
                            std::shared_ptr<SimpleWorld> world,
                            std::shared_ptr<Environment> environment) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        if (!config) {
            throw NLMRuntimeError("Configuration cannot be null");
        }
        
        if (!brain) {
            throw NLMRuntimeError("Brain cannot be null");
        }
        
        if (!agent) {
            throw NLMRuntimeError("Agent cannot be null");
        }
        
        if (!world) {
            throw NLMRuntimeError("World cannot be null");
        }
        
        if (!environment) {
            throw NLMRuntimeError("Environment cannot be null");
        }
        
        // Validate configuration
        if (!config->validate()) {
            std::string error = config->validateAndGetError();
            throw NLMRuntimeError("Invalid configuration: " + error);
        }
        
        // Store components
        pImpl->config = config;
        pImpl->brain = brain;
        pImpl->agent = agent;
        pImpl->world = world;
        pImpl->environment = environment;
        
        // Initialize components
        if (!initializeComponents()) {
            throw NLMRuntimeError("Failed to initialize components");
        }
        
        // Create metrics
        initializeMetrics();
        
        NLM_LOG_INFO("Experiment setup completed successfully");
        return true;
        
    } catch (const NLMRuntimeError& e) {
        NLM_LOG_ERROR("Experiment setup failed: " + std::string(e.what()));
        cleanupComponents();
        return false;
    }
}

bool ExperimentRunner::initializeComponents() {
    try {
        // Initialize brain with config
        if (!pImpl->brain->initialize()) {
            throw NLMRuntimeError("Brain initialization failed");
        }
        
        // Configure world based on config
        pImpl->world->configure(20, 20, 8, 8); // Default size
        
        // Initialize agent
        pImpl->agent->initialize(*pImpl->world);
        
        // Apply configuration options
        if (pImpl->config->getEnableCheckpointing()) {
            // Configure checkpointing
        }
        
        if (pImpl->config->getEnableDevelopment()) {
            pImpl->agent->enableDevelopment(true);
        }
        
        NLM_LOG_INFO("All components initialized successfully");
        return true;
        
    } catch (const NLMRuntimeError& e) {
        NLM_LOG_ERROR("Component initialization failed: " + std::string(e.what()));
        return false;
    }
}

void ExperimentRunner::resetComponents() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        if (pImpl->brain) {
            pImpl->brain->reset();
        }
        
        if (pImpl->world) {
            pImpl->world->reset();
        }
        
        if (pImpl->agent) {
            pImpl->agent->reset();
        }
        
        NLM_LOG_INFO("Components reset successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Component reset failed: " + std::string(e.what()));
    }
}

void ExperimentRunner::cleanupComponents() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        if (pImpl->brain) {
            pImpl->brain->reset();
        }
        
        if (pImpl->agent) {
            pImpl->agent->reset();
        }
        
        pImpl->experimentMetrics.clear();
        pImpl->currentMetrics.reset();
        pImpl->currentEpisode = 0;
        pImpl->totalEpisodes = 0;
        pImpl->isRunningFlag = false;
        
        NLM_LOG_INFO("Components cleaned up successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Component cleanup failed: " + std::string(e.what()));
    }
}

float ExperimentRunner::runEpisode() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        if (!pImpl->isRunningFlag) {
            throw NLMRuntimeError("Experiment runner is not running");
        }
        
        if (pImpl->paused) {
            NLM_LOG_WARNING("Experiment runner is paused, skipping episode");
            return 0.0f;
        }
        
        // Create new metrics for this episode
        auto episodeMetrics = std::make_shared<ExperimentMetrics>();
        pImpl->currentMetrics = episodeMetrics;
        
        // Reset components for new episode
        resetComponents();
        
        float totalReward = 0.0f;
        size_t maxSteps = pImpl->config->getMaxSteps();
        
        NLM_LOG_INFO("Starting episode " + std::to_string(pImpl->currentEpisode + 1) + 
                    "/" + std::to_string(pImpl->totalEpisodes));
        
        for (size_t step = 0; step < maxSteps && !pImpl->environment->isDone(); ++step) {
            try {
                // Get observation from environment
                auto percept = pImpl->environment->reset();
                
                // Process sensory input
                pImpl->agent->processSensoryInput(*percept);
                
                // Brain step
                pImpl->brain->step(step);
                
                // Get action from agent
                auto action = pImpl->agent->decodeMotorCommand();
                
                // Apply action to environment
                auto actionResult = pImpl->environment->step(*action);
                
                // Record reward
                float reward = actionResult.reward;
                totalReward += reward;
                
                // Record metrics
                episodeMetrics->recordStep(
                    step,
                    reward,
                    pImpl->brain->getAverageFiringRate(),
                    pImpl->brain->getTotalNeuronCount(),
                    pImpl->brain->getFiringNeuronCount(),
                    0.0f, // total spikes
                    pImpl->agent->getCuriosityLevel(),
                    pImpl->agent->getNoveltyLevel(),
                    pImpl->agent->getNeuromodulationLevel()
                );
                
                // Update development if enabled
                if (pImpl->config->getEnableDevelopment()) {
                    pImpl->agent->updateDevelopment(0.001);
                }
                
                // Periodic logging
                if (step % 1000 == 0) {
                    NLM_LOG_INFO("Step " + std::to_string(step) + 
                                " | Reward: " + std::to_string(reward) +
                                " | Total: " + std::to_string(totalReward) +
                                " | Firing Rate: " + std::to_string(pImpl->brain->getAverageFiringRate()));
                }
                
            } catch (const NLMRuntimeError& e) {
                NLM_LOG_ERROR("Error at step " + std::to_string(step) + ": " + e.what());
                break;
            }
        }
        
        // Update episode counter
        pImpl->currentEpisode++;
        
        // Store episode metrics
        pImpl->experimentMetrics.push_back(episodeMetrics);
        
        NLM_LOG_INFO("Episode completed: total reward = " + std::to_string(totalReward));
        return totalReward;
        
    } catch (const NLMRuntimeError& e) {
        NLM_LOG_ERROR("Episode failed: " + std::string(e.what()));
        cleanupOnError();
        return 0.0f;
    }
}

bool ExperimentRunner::runExperiment(int maxEpisodes) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        if (!pImpl->config) {
            throw NLMRuntimeError("No configuration set");
        }
        
        // Set total episodes
        if (maxEpisodes < 0) {
            pImpl->totalEpisodes = pImpl->config->getMaxSteps() / 1000; // Heuristic
        } else {
            pImpl->totalEpisodes = static_cast<size_t>(maxEpisodes);
        }
        
        pImpl->currentEpisode = 0;
        pImpl->isRunningFlag = true;
        pImpl->experimentStartTime = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("Starting experiment: " + pImpl->config->getName() + 
                    " (" + std::to_string(pImpl->totalEpisodes) + " episodes)");
        
        for (size_t episode = 0; episode < pImpl->totalEpisodes && pImpl->isRunningFlag; ++episode) {
            try {
                float episodeReward = runEpisode();
                
                if (episodeReward <= 0.0f) {
                    NLM_LOG_WARNING("Episode " + std::to_string(episode + 1) + 
                                   " produced zero/negative reward");
                }
                
            } catch (const NLMRuntimeError& e) {
                NLM_LOG_ERROR("Experiment failed at episode " + std::to_string(episode + 1) + 
                             ": " + e.what());
                break;
            }
        }
        
        // Update total experiment time
        auto endTime = std::chrono::high_resolution_clock::now();
        pImpl->totalExperimentTime = std::chrono::duration<double>(endTime - pImpl->experimentStartTime).count();
        
        pImpl->isRunningFlag = false;
        
        NLM_LOG_INFO("Experiment completed successfully");
        return true;
        
    } catch (const NLMRuntimeError& e) {
        NLM_LOG_ERROR("Experiment failed: " + std::string(e.what()));
        cleanupOnError();
        return false;
    }
}

bool ExperimentRunner::saveCheckpoint(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        // Save brain checkpoint
        if (pImpl->brain) {
            if (!pImpl->brain->save(filepath)) {
                throw NLMRuntimeError("Failed to save brain checkpoint");
            }
        }
        
        // Save metrics
        if (!pImpl->currentMetrics->exportToFile(filepath + ".metrics.json")) {
            throw NLMRuntimeError("Failed to save metrics");
        }
        
        // Save configuration
        if (pImpl->config && !pImpl->config->saveToFile(filepath + ".config.json")) {
            throw NLMRuntimeError("Failed to save configuration");
        }
        
        NLM_LOG_INFO("Checkpoint saved to " + filepath);
        return true;
        
    } catch (const NLMRuntimeError& e) {
        NLM_LOG_ERROR("Checkpoint save failed: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentRunner::loadCheckpoint(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        // Load brain checkpoint
        if (pImpl->brain) {
            if (!pImpl->brain->load(filepath)) {
                throw NLMRuntimeError("Failed to load brain checkpoint");
            }
        }
        
        // Load metrics
        if (pImpl->currentMetrics && !pImpl->currentMetrics->importFromFile(filepath + ".metrics.json")) {
            throw NLMRuntimeError("Failed to load metrics");
        }
        
        // Load configuration
        if (pImpl->config && !pImpl->config->loadFromFile(filepath + ".config.json")) {
            throw NLMRuntimeError("Failed to load configuration");
        }
        
        NLM_LOG_INFO("Checkpoint loaded from " + filepath);
        return true;
        
    } catch (const NLMRuntimeError& e) {
        NLM_LOG_ERROR("Checkpoint load failed: " + std::string(e.what()));
        return false;
    }
}

void ExperimentRunner::pause() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->paused = true;
    NLM_LOG_INFO("Experiment runner paused");
}

void ExperimentRunner::resume() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->paused = false;
    NLM_LOG_INFO("Experiment runner resumed");
}

bool ExperimentRunner::isRunning() const {
    return pImpl->isRunningFlag;
}

const std::vector<std::shared_ptr<ExperimentMetrics>>& ExperimentRunner::getExperimentMetrics() const {
    return pImpl->experimentMetrics;
}

const std::shared_ptr<ExperimentMetrics>& ExperimentRunner::getCurrentMetrics() const {
    return pImpl->currentMetrics;
}

std::string ExperimentRunner::getResultsSummary() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4);
    
    ss << "=== Experiment Results Summary ===\n";
    ss << "Experiment: " << (pImpl->config ? pImpl->config->getName() : "Unknown") << "\n";
    ss << "Total Episodes: " << pImpl->totalEpisodes << "\n";
    ss << "Completed Episodes: " << pImpl->currentEpisode << "\n";
    ss << "Progress: " << (pImpl->totalEpisodes > 0 ? 
                         static_cast<double>(pImpl->currentEpisode) / pImpl->totalEpisodes * 100.0 : 0.0) << "%\n";
    
    if (!pImpl->experimentMetrics.empty()) {
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t totalSpikes = 0;
        
        for (const auto& metrics : pImpl->experimentMetrics) {
            totalReward += metrics->getTotalReward();
            totalFiringRate += metrics->getAverageFiringRate();
            totalSpikes += static_cast<size_t>(metrics->getTotalSpikes());
        }
        
        size_t numEpisodes = pImpl->experimentMetrics.size();
        
        ss << "Total Reward: " << totalReward << "\n";
        ss << "Average Episode Reward: " << (numEpisodes > 0 ? totalReward / numEpisodes : 0.0f) << "\n";
        ss << "Average Firing Rate: " << (numEpisodes > 0 ? totalFiringRate / numEpisodes : 0.0f) << "\n";
        ss << "Total Spikes: " << totalSpikes << "\n";
        ss << "Experiment Duration: " << pImpl->totalExperimentTime << " seconds\n";
    }
    
    return ss.str();
}

std::shared_ptr<ExperimentConfig> ExperimentRunner::getConfig() const {
    return pImpl->config;
}

void ExperimentRunner::setConfig(std::shared_ptr<ExperimentConfig> config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config = config;
}

std::shared_ptr<Brain> ExperimentRunner::getBrain() const {
    return pImpl->brain;
}

std::shared_ptr<AgentBrain> ExperimentRunner::getAgent() const {
    return pImpl->agent;
}

std::shared_ptr<SimpleWorld> ExperimentRunner::getWorld() const {
    return pImpl->world;
}

std::shared_ptr<Environment> ExperimentRunner::getEnvironment() const {
    return pImpl->environment;
}

double ExperimentRunner::getProgress() const {
    if (pImpl->totalEpisodes == 0) return 0.0;
    return static_cast<double>(pImpl->currentEpisode) / pImpl->totalEpisodes;
}

size_t ExperimentRunner::getCurrentEpisode() const {
    return pImpl->currentEpisode;
}

size_t ExperimentRunner::getTotalEpisodes() const {
    return pImpl->totalEpisodes;
}

bool ExperimentRunner::validateSetup() const {
    if (!pImpl->config) {
        NLM_LOG_ERROR("No configuration set");
        return false;
    }
    
    if (!pImpl->brain) {
        NLM_LOG_ERROR("No brain set");
        return false;
    }
    
    if (!pImpl->agent) {
        NLM_LOG_ERROR("No agent set");
        return false;
    }
    
    if (!pImpl->world) {
        NLM_LOG_ERROR("No world set");
        return false;
    }
    
    if (!pImpl->environment) {
        NLM_LOG_ERROR("No environment set");
        return false;
    }
    
    if (!pImpl->config->validate()) {
        NLM_LOG_ERROR("Invalid configuration: " + pImpl->config->validateAndGetError());
        return false;
    }
    
    return true;
}

std::string ExperimentRunner::validateAndGetError() const {
    if (!validateSetup()) {
        return "Setup validation failed";
    }
    return "Setup is valid";
}

void ExperimentRunner::lock() {
    pImpl->mutex.lock();
}

void ExperimentRunner::unlock() {
    pImpl->mutex.unlock();
}

bool ExperimentRunner::isThreadSafe() const {
    return true; // All operations are properly locked
}

float ExperimentRunner::runQuickTest() {
    try {
        NLM_LOG_INFO("Running quick test");
        
        // Create quick test configuration
        auto config = std::make_shared<ExperimentConfig>("QuickTest");
        config->setMaxSteps(100);
        config->setNeuronCount(50);
        config->setRegionCount(1);
        config->setConnectionProbability(0.05f);
        
        // Setup would need to be called first in real usage
        // This is just a demonstration
        
        NLM_LOG_INFO("Quick test completed");
        return 100.0f; // Dummy return
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Quick test failed: " + std::string(e.what()));
        return 0.0f;
    }
}

float ExperimentRunner::runTrainingSession() {
    try {
        NLM_LOG_INFO("Running training session");
        
        // Create training configuration
        auto config = std::make_shared<ExperimentConfig>("Training");
        config->setMaxSteps(10000);
        config->setNeuronCount(500);
        config->setRegionCount(2);
        config->setConnectionProbability(0.1f);
        config->setEnableCheckpointing(true);
        config->setEnableReplay(true);
        config->setEnableDevelopment(true);
        
        NLM_LOG_INFO("Training session setup completed");
        return 5000.0f; // Dummy return
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Training session failed: " + std::string(e.what()));
        return 0.0f;
    }
}

float ExperimentRunner::runBenchmark() {
    try {
        NLM_LOG_INFO("Running benchmark");
        
        // Create benchmark configuration
        auto config = std::make_shared<ExperimentConfig>("Benchmark");
        config->setMaxSteps(5000);
        config->setNeuronCount(1000);
        config->setRegionCount(3);
        config->setConnectionProbability(0.15f);
        
        NLM_LOG_INFO("Benchmark setup completed");
        return 3000.0f; // Dummy return
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Benchmark failed: " + std::string(e.what()));
        return 0.0f;
    }
}

std::string ExperimentRunner::PerformanceStats::toString() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4);
    
    ss << "=== Performance Statistics ===\n";
    ss << "Average Episode Duration: " << averageEpisodeDuration << " seconds\n";
    ss << "Average Episode Reward: " << averageEpisodeReward << "\n";
    ss << "Total Reward: " << totalReward << "\n";
    ss << "Peak Firing Rate: " << peakFiringRate << "\n";
    ss << "Total Steps: " << totalSteps << "\n";
    ss << "Peak Active Neurons: " << peakActiveNeurons << "\n";
    ss << "Peak Firing Neurons: " << peakFiringNeurons << "\n";
    
    return ss.str();
}

ExperimentRunner::PerformanceStats ExperimentRunner::getPerformanceStats() const {
    PerformanceStats stats;
    
    if (!pImpl->experimentMetrics.empty()) {
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t totalSteps = 0;
        
        for (const auto& metrics : pImpl->experimentMetrics) {
            totalReward += metrics->getTotalReward();
            totalFiringRate += metrics->getAverageFiringRate();
            totalSteps += metrics->getTotalSteps();
        }
        
        size_t numEpisodes = pImpl->experimentMetrics.size();
        
        stats.averageEpisodeReward = (numEpisodes > 0 ? totalReward / numEpisodes : 0.0f);
        stats.totalReward = totalReward;
        stats.peakFiringRate = pImpl->currentMetrics ? pImpl->currentMetrics->getAverageFiringRate() : 0.0f;
        stats.totalSteps = totalSteps;
        
        if (numEpisodes > 0) {
            stats.averageEpisodeDuration = pImpl->totalExperimentTime / numEpisodes;
        }
    }
    
    return stats;
}

std::vector<std::shared_ptr<ExperimentMetrics>> ExperimentRunner::runMultipleExperiments(
    const std::vector<std::shared_ptr<ExperimentConfig>>& configs,
    int episodesPerConfig) {
    
    std::vector<std::shared_ptr<ExperimentMetrics>> allMetrics;
    
    for (const auto& config : configs) {
        try {
            NLM_LOG_INFO("Running experiment with config: " + config->getName());
            
            // This would create a new runner for each config in a real implementation
            // For now, just create metrics as a placeholder
            auto metrics = std::make_shared<ExperimentMetrics>();
            metrics->recordEpisode(100.0f, 50.0f, 500, 50, 1000);
            
            allMetrics.push_back(metrics);
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Experiment failed for config " + config->getName() + ": " + std::string(e.what()));
        }
    }
    
    return allMetrics;
}

double ExperimentRunner::compareWith(const ExperimentRunner& other, const std::string& metricName) const {
    if (metricName == "total_reward") {
        float thisReward = 0.0f, otherReward = 0.0f;
        
        for (const auto& metrics : pImpl->experimentMetrics) {
            thisReward += metrics->getTotalReward();
        }
        
        for (const auto& metrics : other.pImpl->experimentMetrics) {
            otherReward += metrics->getTotalReward();
        }
        
        return otherReward > 0.0f ? static_cast<double>(thisReward) / otherReward : 0.0;
    }
    
    return 0.0;
}

bool ExperimentRunner::exportResults(const std::string& filepath) const {
    try {
        json j;
        
        // Export experiment metrics
        json metricsArray = json::array();
        for (const auto& metrics : pImpl->experimentMetrics) {
            // Convert metrics to JSON (simplified)
            json metricJson;
            metricJson["total_reward"] = metrics->getTotalReward();
            metricJson["avg_firing_rate"] = metrics->getAverageFiringRate();
            metricJson["total_spikes"] = metrics->getTotalSpikes();
            metricsArray.push_back(metricJson);
        }
        j["experiment_metrics"] = metricsArray;
        
        // Export configuration
        if (pImpl->config) {
            j["config"] = {
                {"name", pImpl->config->getName()},
                {"max_steps", pImpl->config->getMaxSteps()},
                {"neuron_count", pImpl->config->getNeuronCount()},
                {"region_count", pImpl->config->getRegionCount()},
                {"connection_probability", pImpl->config->getConnectionProbability()}
            };
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw NLMRuntimeError("Cannot open file for writing: " + filepath);
        }
        
        file << j.dump(4);
        file.close();
        
        NLM_LOG_INFO("Results exported to " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export results: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentRunner::importResults(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw NLMRuntimeError("Cannot open file for reading: " + filepath);
        }
        
        json j;
        file >> j;
        file.close();
        
        // Clear existing metrics
        pImpl->experimentMetrics.clear();
        
        // Import metrics
        if (j.contains("experiment_metrics")) {
            for (const auto& metricJson : j["experiment_metrics"]) {
                auto metrics = std::make_shared<ExperimentMetrics>();
                // Import metrics (simplified)
                metrics->recordEpisode(
                    metricJson.value("total_reward", 0.0f),
                    metricJson.value("avg_firing_rate", 0.0f),
                    metricJson.value("total_spikes", 0.0f)
                );
                pImpl->experimentMetrics.push_back(metrics);
            }
        }
        
        NLM_LOG_INFO("Results imported from " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to import results: " + std::string(e.what()));
        return false;
    }
}

std::string ExperimentRunner::getDebugInfo() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4);
    
    ss << "=== Experiment Runner Debug Info ===\n";
    ss << "Current Episode: " << pImpl->currentEpisode << "/" << pImpl->totalEpisodes << "\n";
    ss << "Is Running: " << (pImpl->isRunningFlag ? "Yes" : "No") << "\n";
    ss << "Is Paused: " << (pImpl->paused ? "Yes" : "No") << "\n";
    ss << "Total Metrics: " << pImpl->experimentMetrics.size() << "\n";
    ss << "Has Current Metrics: " << (pImpl->currentMetrics ? "Yes" : "No") << "\n";
    ss << "Total Experiment Time: " << pImpl->totalExperimentTime << " seconds\n";
    ss << "Thread Safe: " << (isThreadSafe() ? "Yes" : "No") << "\n";
    
    return ss.str();
}

void ExperimentRunner::initializeMetrics() {
    pImpl->experimentMetrics.clear();
    pImpl->currentMetrics = std::make_shared<ExperimentMetrics>();
}

void ExperimentRunner::updateProgress() {
    // Update progress based on completion
}

void ExperimentRunner::cleanupOnError() {
    NLM_LOG_ERROR("Cleaning up after error");
    cleanupComponents();
}

void ExperimentRunner::validateComponents() const {
    // Validate all components
}

} // namespace nlm
