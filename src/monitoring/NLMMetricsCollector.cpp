#include "NLMMetricsCollector.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <cmath>

namespace nlm {

NLMMetricsCollector::NLMMetricsCollector(std::shared_ptr<MetricRegistry> registry,
                                         std::shared_ptr<Config> config)
    : registry_(registry), config_(config), isInitialized_(false),
      updateInterval_(0.001), lastUpdateTime_(std::chrono::steady_clock::now()) {}

bool NLMMetricsCollector::initialize() {
    if (!registry_) {
        NLM_LOG_ERROR("MetricRegistry not provided");
        return false;
    }
    
    if (!config_) {
        NLM_LOG_ERROR("Config not provided");
        return false;
    }
    
    registerBuiltinMetrics();
    registerCustomMetrics();
    
    isInitialized_ = true;
    NLM_LOG_INFO("NLMMetricsCollector initialized successfully");
    return true;
}

void NLMMetricsCollector::registerBuiltinMetrics() {
    // Register neural performance metrics
    registry_->registerBuiltInMetric("total_spike_count",
                                    MetricCategory::NEURAL_PERFORMANCE,
                                    "Total number of spikes recorded",
                                    [this]() { return collectTotalSpikeCount(); });
    
    registry_->registerBuiltInMetric("firing_neuron_count",
                                    MetricCategory::NEURAL_PERFORMANCE,
                                    "Number of neurons currently firing",
                                    [this]() { return collectFiringNeuronCount(); });
    
    registry_->registerBuiltInMetric("average_firing_rate",
                                    MetricCategory::NEURAL_PERFORMANCE,
                                    "Average firing rate across all neurons",
                                    [this]() { return collectAverageFiringRate(); });
    
    registry_->registerBuiltInMetric("excitation_inhibition_ratio",
                                    MetricCategory::NEURAL_PERFORMANCE,
                                    "Ratio of excitation to inhibition",
                                    [this]() { return collectExcitationInhibitionRatio(); });
    
    registry_->registerBuiltInMetric("total_neuron_count",
                                    MetricCategory::NEURAL_PERFORMANCE,
                                    "Total number of neurons in the brain",
                                    [this]() { return collectTotalNeuronCount(); });
    
    registry_->registerBuiltInMetric("total_synapse_count",
                                    MetricCategory::NEURAL_PERFORMANCE,
                                    "Total number of synapses in the brain",
                                    [this]() { return collectTotalSynapseCount(); });
    
    registry_->registerBuiltInMetric("active_neuron_count",
                                    MetricCategory::NEURAL_PERFORMANCE,
                                    "Number of neurons currently active",
                                    [this]() { return collectActiveNeuronCount(); });
    
    registry_->registerBuiltInMetric("pending_spike_count",
                                    MetricCategory::NEURAL_PERFORMANCE,
                                    "Number of pending spike events",
                                    [this]() { return collectPendingSpikeCount(); });
    
    // Register memory performance metrics
    registry_->registerBuiltInMetric("working_memory_active_traces",
                                    MetricCategory::MEMORY_PERFORMANCE,
                                    "Number of active traces in working memory",
                                    [this]() { return collectWorkingMemoryActiveTraces(); });
    
    registry_->registerBuiltInMetric("episodic_memory_episode_count",
                                    MetricCategory::MEMORY_PERFORMANCE,
                                    "Number of episodic memory episodes",
                                    [this]() { return collectEpisodicMemoryEpisodeCount(); });
    
    registry_->registerBuiltInMetric("associative_memory_active_associations",
                                    MetricCategory::MEMORY_PERFORMANCE,
                                    "Number of active associative memory associations",
                                    [this]() { return collectAssociativeMemoryActiveAssociations(); });
    
    // Register neuromodulation metrics
    registry_->registerBuiltInMetric("dopamine_level",
                                    MetricCategory::NEURAL_PERFORMANCE,
                                    "Current dopamine level (reward signal)",
                                    [this]() { return collectDopamineLevel(); });
    
    registry_->registerBuiltInMetric("curiosity_level",
                                    MetricCategory::BEHAVIOR_PERFORMANCE,
                                    "Current curiosity level (exploration drive)",
                                    [this]() { return collectCuriosityLevel(); });
    
    registry_->registerBuiltInMetric("novelty_level",
                                    MetricCategory::COGNITIVE_PERFORMANCE,
                                    "Current novelty detection level",
                                    [this]() { return collectNoveltyLevel(); });
    
    registry_->registerBuiltInMetric("prediction_error",
                                    MetricCategory::COGNITIVE_PERFORMANCE,
                                    "Prediction error signal",
                                    [this]() { return collectPredictionError(); });
    
    // Register cognition metrics
    registry_->registerBuiltInMetric("planning_activity",
                                    MetricCategory::COGNITIVE_PERFORMANCE,
                                    "Current planning system activity",
                                    [this]() { return collectPlanningActivity(); });
    
    registry_->registerBuiltInMetric("concept_activation",
                                    MetricCategory::COGNITIVE_PERFORMANCE,
                                    "Overall concept activation level",
                                    [this]() { return collectConceptActivation(); });
    
    registry_->registerBuiltInMetric("attention_focus",
                                    MetricCategory::COGNITIVE_PERFORMANCE,
                                    "Current attention focus strength",
                                    [this]() { return collectAttentionFocus(); });
    
    // Register development metrics
    registry_->registerBuiltInMetric("developmental_stage",
                                    MetricCategory::NEURAL_PERFORMANCE,
                                    "Current developmental stage",
                                    [this]() { return collectDevelopmentalStage(); });
    
    registry_->registerBuiltInMetric("plasticity_rate",
                                    MetricCategory::NEURAL_PERFORMANCE,
                                    "Current plasticity rate",
                                    [this]() { return collectPlasticityRate(); });
    
    // Register system performance metrics
    registry_->registerBuiltInMetric("system_cpu_usage",
                                    MetricCategory::SYSTEM_PERFORMANCE,
                                    "CPU usage percentage",
                                    [this]() { return collectSystemCPUUsage(); });
    
    registry_->registerBuiltInMetric("system_memory_usage",
                                    MetricCategory::SYSTEM_PERFORMANCE,
                                    "Memory usage percentage",
                                    [this]() { return collectSystemMemoryUsage(); });
    
    registry_->registerBuiltInMetric("system_process_time",
                                    MetricCategory::SYSTEM_PERFORMANCE,
                                    "Total process time in seconds",
                                    [this]() { return collectSystemProcessTime(); });
    
    // Register behavior performance metrics
    registry_->registerBuiltInMetric("exploration_rate",
                                    MetricCategory::BEHAVIOR_PERFORMANCE,
                                    "Exploration vs exploitation balance",
                                    [this]() { return collectExplorationRate(); });
    
    registry_->registerBuiltInMetric("exploitation_rate",
                                    MetricCategory::BEHAVIOR_PERFORMANCE,
                                    "Exploitation vs exploration balance",
                                    [this]() { return collectExploitationRate(); });
    
    registry_->registerBuiltInMetric("adaptation_rate",
                                    MetricCategory::BEHAVIOR_PERFORMANCE,
                                    "Rate of behavioral adaptation",
                                    [this]() { return collectAdaptationRate(); });
    
    // Register spike pattern metrics
    registry_->registerBuiltInMetric("theta_gamma_coupling",
                                    MetricCategory::NEURAL_PERFORMANCE,
                                    "Theta-gamma oscillation coupling strength",
                                    [this]() { return collectThetaGammaCoupling(); });
    
    registry_->registerBuiltInMetric("burst_rate",
                                    MetricCategory::NEURAL_PERFORMANCE,
                                    "Rate of neural burst events",
                                    [this]() { return collectBurstRate(); });
    
    registry_->registerBuiltInMetric("synchrony_level",
                                    MetricCategory::NEURAL_PERFORMANCE,
                                    "Neural synchrony level",
                                    [this]() { return collectSynchronyLevel(); });
    
    // Register memory efficiency metrics
    registry_->registerBuiltInMetric("memory_consolidation_rate",
                                    MetricCategory::MEMORY_PERFORMANCE,
                                    "Memory consolidation rate",
                                    [this]() { return collectMemoryConsolidationRate(); });
    
    registry_->registerBuiltInMetric("memory_replay_frequency",
                                    MetricCategory::MEMORY_PERFORMANCE,
                                    "Memory replay frequency",
                                    [this]() { return collectMemoryReplayFrequency(); });
    
    registry_->registerBuiltInMetric("memory_encoding_accuracy",
                                    MetricCategory::MEMORY_PERFORMANCE,
                                    "Memory encoding accuracy",
                                    [this]() { return collectMemoryEncodingAccuracy(); });
    
    // Register prediction metrics
    registry_->registerBuiltInMetric("prediction_accuracy",
                                    MetricCategory::COGNITIVE_PERFORMANCE,
                                    "Prediction system accuracy",
                                    [this]() { return collectPredictionAccuracy(); });
    
    registry_->registerBuiltInMetric("prediction_latency",
                                    MetricCategory::COGNITIVE_PERFORMANCE,
                                    "Prediction system latency",
                                    [this]() { return collectPredictionLatency(); });
    
    registry_->registerBuiltInMetric("prediction_error_variance",
                                    MetricCategory::COGNITIVE_PERFORMANCE,
                                    "Prediction error variance",
                                    [this]() { return collectPredictionErrorVariance(); });
}

void NLMMetricsCollector::registerCustomMetrics() {
    // Register custom metrics that combine multiple sources
    registry_->registerCustomMetric("neural_efficiency",
                                  "NEURAL_PERFORMANCE",
                                  "Combined measure of neural efficiency",
                                  [this]() {
                                      double spikeCount = collectTotalSpikeCount();
                                      double neuronCount = collectTotalNeuronCount();
                                      double avgFiring = collectAverageFiringRate();
                                      return spikeCount / (neuronCount * avgFiring + 0.001);
                                  });
    
    registry_->registerCustomMetric("memory_utilization",
                                  "MEMORY_PERFORMANCE",
                                  "Memory system utilization ratio",
                                  [this]() {
                                      double wmTraces = collectWorkingMemoryActiveTraces();
                                      double emEpisodes = collectEpisodicMemoryEpisodeCount();
                                      return (wmTraces + emEpisodes) / 100.0;  // Normalized
                                  });
    
    registry_->registerCustomMetric("cognitive_load",
                                  "COGNITIVE_PERFORMANCE",
                                  "Overall cognitive load measure",
                                  [this]() {
                                      double planning = collectPlanningActivity();
                                      double concepts = collectConceptActivation();
                                      double attention = collectAttentionFocus();
                                      return (planning + concepts + attention) / 3.0;
                                  });
}

void NLMMetricsCollector::setupBrainIntegration(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot integrate null brain");
        return;
    }
    
    brain_ = brain;
    registerBrainMetrics(brain_);
    
    // Setup integration with other brain components
    workingMemory_ = brain_->getWorkingMemory();
    episodicMemory_ = brain_->getEpisodicMemory();
    associativeMemory_ = brain_->getAssociativeMemory();
    predictionSystem_ = brain_->getPredictionSystem();
    planner_ = brain_->getPlanner();
    conceptFormation_ = brain_->getConceptFormation();
    attention_ = brain_->getAttention();
    developmentSystem_ = brain_->getDevelopmentSystem();
    dopamine_ = brain_->getDopamine();
    curiosity_ = brain_->getCuriosity();
    novelty_ = brain_->getNovelty();
    predictionError_ = brain_->getPredictionErrorSignal();
    
    registerMemoryMetrics(workingMemory_, episodicMemory_, associativeMemory_);
    registerPredictionMetrics(predictionSystem_);
    registerCognitionMetrics(planner_, conceptFormation_, attention_);
    registerDevelopmentMetrics(developmentSystem_);
    registerNeuromodulationMetrics(dopamine_, curiosity_, novelty_, predictionError_);
    
    NLM_LOG_INFO("Brain integration completed");
}

void NLMMetricsCollector::updateMetrics() {
    if (!isInitialized_) {
        return;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<double>(now - lastUpdateTime_).count();
    
    if (elapsed < updateInterval_) {
        return;  // Not time to update yet
    }
    
    // Update all metrics by collecting fresh values
    // The metric collectors are called when needed via getMetricValue()
    // For real-time collection, this could be called periodically
    lastUpdateTime_ = now;
}

double NLMMetricsCollector::getMetricValue(const std::string& name) const {
    if (!registry_ || !registry_->hasMetric(name)) {
        return 0.0; // Metric not found
    }
    
    auto collector = registry_->getCollector(name);
    if (!collector) {
        return 0.0; // Collector not found
    }
    
    try {
        return collector();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error collecting metric ") + name + ": " + e.what());
        return 0.0;
    }
}

std::vector<MetricData> NLMMetricsCollector::getAllMetricValues() const {
    if (!registry_ || !isInitialized_) {
        return std::vector<MetricData>();
    }
    
    // Force metric collection by calling each collector
    std::vector<MetricData> metrics = registry_->collectAllMetrics();
    return metrics;
}

// Individual metric collection implementations

// Neural Performance Metrics
double NLMMetricsCollector::collectTotalSpikeCount() const {
    return brain_ ? static_cast<double>(brain_->getTotalSpikeCount()) : 0.0;
}

double NLMMetricsCollector::collectFiringNeuronCount() const {
    return brain_ ? static_cast<double>(brain_->getFiringNeuronCount()) : 0.0;
}

double NLMMetricsCollector::collectAverageFiringRate() const {
    return brain_ ? brain_->getAverageFiringRate() : 0.0;
}

double NLMMetricsCollector::collectExcitationInhibitionRatio() const {
    return brain_ ? brain_->getExcitationInhibitionRatio() : 0.0;
}

double NLMMetricsCollector::collectTotalNeuronCount() const {
    return brain_ ? static_cast<double>(brain_->getTotalNeuronCount()) : 0.0;
}

double NLMMetricsCollector::collectTotalSynapseCount() const {
    return brain_ ? static_cast<double>(brain_->getTotalSynapseCount()) : 0.0;
}

double NLMMetricsCollector::collectActiveNeuronCount() const {
    return brain_ ? static_cast<double>(brain_->getActiveNeuronCount()) : 0.0;
}

double NLMMetricsCollector::collectPendingSpikeCount() const {
    return brain_ ? static_cast<double>(brain_->getPendingSpikeEventCount()) : 0.0;
}

// Memory Performance Metrics
double NLMMetricsCollector::collectWorkingMemoryActiveTraces() const {
    return workingMemory_ ? static_cast<double>(workingMemory_->getActiveTraces()) : 0.0;
}

double NLMMetricsCollector::collectEpisodicMemoryEpisodeCount() const {
    return episodicMemory_ ? static_cast<double>(episodicMemory_->getEpisodeCount()) : 0.0;
}

double NLMMetricsCollector::collectAssociativeMemoryActiveAssociations() const {
    return associativeMemory_ ? static_cast<double>(0.0) : 0.0; // TODO: Add API method
}

// Neuromodulation Metrics
double NLMMetricsCollector::collectDopamineLevel() const {
    return dopamine_ ? dopamine_->getLevel() : 0.0;
}

double NLMMetricsCollector::collectCuriosityLevel() const {
    return curiosity_ ? curiosity_->getLevel() : 0.0;
}

double NLMMetricsCollector::collectNoveltyLevel() const {
    return novelty_ ? novelty_->getLevel() : 0.0;
}

double NLMMetricsCollector::collectPredictionError() const {
    return predictionError_ ? predictionError_->getError() : 0.0;
}

// Cognition Metrics
double NLMMetricsCollector::collectPlanningActivity() const {
    return planner_ ? 1.0 : 0.0; // TODO: Add API method
}

double NLMMetricsCollector::collectConceptActivation() const {
    return conceptFormation_ ? 1.0 : 0.0; // TODO: Add API method
}

double NLMMetricsCollector::collectAttentionFocus() const {
    return attention_ ? 1.0 : 0.0; // TODO: Add API method
}

// Development Metrics
double NLMMetricsCollector::collectDevelopmentalStage() const {
    return developmentSystem_ ? static_cast<double>(developmentSystem_->getDevelopmentalStage()) : 0.0;
}

double NLMMetricsCollector::collectPlasticityRate() const {
    return developmentSystem_ ? developmentSystem_->getPlasticityRate() : 0.0;
}

// System Metrics (simplified implementation)
double NLMMetricsCollector::collectSystemCPUUsage() const {
    // In a real implementation, this would read system CPU usage
    return 50.0; // Placeholder
}

double NLMMetricsCollector::collectSystemMemoryUsage() const {
    // In a real implementation, this would read system memory usage
    return 60.0; // Placeholder
}

double NLMMetricsCollector::collectSystemProcessTime() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<double>(now - lastUpdateTime_).count();
    return elapsed;
}

// Behavior Metrics
double NLMMetricsCollector::collectExplorationRate() const {
    double curiosity = collectCuriosityLevel();
    double novelty = collectNoveltyLevel();
    return (curiosity + novelty) / 2.0;
}

double NLMMetricsCollector::collectExploitationRate() const {
    double dopamine = collectDopamineLevel();
    return dopamine / 2.0; // Dopamine is reward-based, indicates exploitation
}

double NLMMetricsCollector::collectAdaptationRate() const {
    double plasticity = collectPlasticityRate();
    double learning = collectDopamineLevel();
    return (plasticity + learning) / 2.0;
}

// Spike Pattern Metrics
double NLMMetricsCollector::collectThetaGammaCoupling() const {
    // Simplified implementation
    return 0.5 + (rand() % 100) / 200.0; // Random value between 0.5 and 1.0
}

double NLMMetricsCollector::collectBurstRate() const {
    double spikeCount = collectTotalSpikeCount();
    return spikeCount / 1000.0; // Normalized spike rate
}

double NLMMetricsCollector::collectSynchronyLevel() const {
    // Simplified implementation - would require spike time analysis
    return 0.3 + (rand() % 100) / 500.0; // Random value between 0.3 and 0.8
}

// Memory Efficiency Metrics
double NLMMetricsCollector::collectMemoryConsolidationRate() const {
    // In a real implementation, this would track consolidation events
    return 0.1 + (rand() % 100) / 1000.0; // Random value between 0.1 and 0.2
}

double NLMMetricsCollector::collectMemoryReplayFrequency() const {
    // In a real implementation, this would track replay events
    return 0.05 + (rand() % 100) / 2000.0; // Random value between 0.05 and 0.15
}

double NLMMetricsCollector::collectMemoryEncodingAccuracy() const {
    // In a real implementation, this would measure encoding accuracy
    return 0.8 + (rand() % 100) / 500.0; // Random value between 0.8 and 1.0
}

// Prediction Metrics
double NLMMetricsCollector::collectPredictionAccuracy() const {
    // In a real implementation, this would measure prediction accuracy
    return 0.7 + (rand() % 100) / 500.0; // Random value between 0.7 and 1.0
}

double NLMMetricsCollector::collectPredictionLatency() const {
    // In a real implementation, this would measure prediction latency
    return 0.01 + (rand() % 100) / 10000.0; // Random value between 0.01 and 0.11 seconds
}

double NLMMetricsCollector::collectPredictionErrorVariance() const {
    double error = collectPredictionError();
    return error * error; // Variance is squared error
}

} // namespace nlm
