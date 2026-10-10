// Advanced Experiment Runner with enhanced batch processing capabilities
#include "ExperimentRunner.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    std::vector<std::shared_ptr<ExperimentMetrics>> metrics;
    
    Impl() : globalSeed(42) {}
};

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    
    // Initialize metrics for this experiment
    auto metrics = std::make_shared<ExperimentMetrics>();
    metrics->episode = 0;
    pImpl->metrics.push_back(metrics);
    
    return exp;
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                   std::shared_ptr<Brain> brain,
                                   std::shared_ptr<Environment> environment,
                                   SimulationStep maxSteps) {
    if (!experiment || !brain || !environment) {
        NLM_LOG_ERROR("Invalid experiment, brain, or environment");
        return false;
    }
    
    // Find corresponding metrics
    auto expMetrics = std::find_if(pImpl->metrics.begin(), pImpl->metrics.end(),
                                   [&](const auto& m) { return m->getExperimentName() == experiment->getName(); });
    
    if (expMetrics == pImpl->metrics.end()) {
        NLM_LOG_ERROR("No metrics found for experiment: " + experiment->getName());
        return false;
    }
    
    experiment->setStartStep(0);
    (*expMetrics)->reset();
    
    NLM_LOG_INFO("Running experiment: " + experiment->getName());
    
    // Reset environment and brain
    auto observation = environment->reset();
    brain->receiveSensoryInput(*observation);
    
    // Run simulation loop
    for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
        // Brain step
        brain->step(step);
        
        // Record data
        experiment->recordStep(
            step,
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getFiringNeuronCount(),
            brain->getAverageFiringRate()
        );
        
        // Get action from brain
        auto action = brain->produceAction();
        
        // Environment step
        observation = environment->step(*action);
        
        // Send observation to brain
        brain->receiveSensoryInput(*observation);
        
        // Record reward and metrics
        float reward = environment->getLastReward();
        experiment->recordMetric("total_reward", 
            experiment->getMetric("total_reward") + reward);
        
        // Update metrics
        (*expMetrics)->episode++;
        (*expMetrics)->steps = step + 1;
        (*expMetrics)->totalReward += reward;
        (*expMetrics)->totalSynapses = brain->getTotalSynapseCount();
        (*expMetrics)->averageWeight = brain->getExcitationInhibitionRatio();
        (*expMetrics)->totalSpikes = brain->getTotalSpikeCount();
        (*expMetrics)->averageFiringRate = brain->getAverageFiringRate();
        
        // Update developmental stage
        (*expMetrics)->developmentalAge = static_cast<float>(step) * 0.001f;
        auto stage = brain->getDevelopmentalStage();
        (*expMetrics)->developmentalStage = developmentalStageToString(stage);
        
        // Update action counts
        switch (action->getType()) {
            case MotorCommandType::MoveForward: (*expMetrics)->forwardCount++; break;
            case MotorCommandType::MoveBackward: (*expMetrics)->backwardCount++; break;
            case MotorCommandType::TurnLeft: (*expMetrics)->turnLeftCount++; break;
            case MotorCommandType::TurnRight: (*expMetrics)->turnRightCount++; break;
            case MotorCommandType::Interact: (*expMetrics)->interactCount++; break;
            case MotorCommandType::Wait: (*expMetrics)->waitCount++; break;
        }
        
        // Record neuromodulation levels
        if (auto* agentBrain = dynamic_cast<AgentBrain*>(brain.get())) {
            (*expMetrics)->finalEnergy = agentBrain->getNeuromodulationLevel();
            (*expMetrics)->finalNovelty = agentBrain->getNoveltyLevel();
            (*expMetrics)->finalCuriosity = agentBrain->getCuriosityLevel();
            (*expMetrics)->predictionError = agentBrain->getPredictionError();
        }
        
        // Log progress every 100 steps in batch mode
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Reward: " + std::to_string(reward) +
                        " | Neurons: " + std::to_string(brain->getTotalNeuronCount()) +
                        " | Synapses: " + std::to_string(brain->getTotalSynapseCount()));
        }
    }
    
    experiment->setEndStep(maxSteps);
    NLM_LOG_INFO("Experiment completed: " + experiment->getName() + 
                " | Total reward: " + std::to_string((*expMetrics)->totalReward) +
                " | Final stage: " + (*expMetrics)->developmentalStage);
    
    return true;
}

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    NLM_LOG_INFO("Starting batch experiment with " + std::to_string(experiments.size()) + " experiments");
    
    for (size_t i = 0; i < experiments.size(); ++i) {
        auto& exp = experiments[i];
        
        NLM_LOG_INFO("Running experiment " + std::to_string(i + 1) + "/" + 
                    std::to_string(experiments.size()) + ": " + exp->getName());
        
        // Create fresh brain and environment for each experiment
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        // Create fresh environment
        auto environment = std::make_shared<Environment>();
        
        if (runExperiment(exp, brain, environment, maxSteps)) {
            results.push_back(exp);
            NLM_LOG_INFO("✓ Completed: " + exp->getName());
        } else {
            NLM_LOG_ERROR("✗ Failed: " + exp->getName());
        }
    }
    
    NLM_LOG_INFO("Batch complete. " + std::to_string(results.size()) + "/" + 
                std::to_string(experiments.size()) + " experiments succeeded");
    
    return results;
}

const std::vector<std::shared_ptr<Experiment>>& ExperimentRunner::getExperiments() const {
    return pImpl->experiments;
}

std::shared_ptr<Experiment> ExperimentRunner::getExperiment(const std::string& name) const {
    for (auto& exp : pImpl->experiments) {
        if (exp->getName() == name) {
            return exp;
        }
    }
    return nullptr;
}

void ExperimentRunner::clearExperiments() {
    pImpl->experiments.clear();
    pImpl->metrics.clear();
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
}

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
            return false;
        }
        
        file << "NLM Experiment Results" << std::endl;
        file << "Generated: " << getCurrentTimestamp() << std::endl;
        file << "Total experiments: " << pImpl->experiments.size() << std::endl;
        file << "" << std::endl;
        
        for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
            const auto& exp = pImpl->experiments[i];
            const auto& metrics = pImpl->metrics[i];
            
            file << "=== Experiment " << i + 1 << ": " << exp->getName() << " ===" << std::endl;
            file << "Seed: " << exp->getSeed() << std::endl;
            file << "Start step: " << exp->getStartStep() << std::endl;
            file << "End step: " << exp->getEndStep() << std::endl;
            file << "Total reward: " << metrics->totalReward << std::endl;
            file << "Final energy: " << metrics->finalEnergy << std::endl;
            file << "Final novelty: " << metrics->finalNovelty << std::endl;
            file << "Final curiosity: " << metrics->finalCuriosity << std::endl;
            file << "Prediction error: " << metrics->predictionError << std::endl;
            file << "Development age: " << metrics->developmentalAge << std::endl;
            file << "Development stage: " << metrics->developmentalStage << std::endl;
            file << "Total synapses: " << metrics->totalSynapses << std::endl;
            file << "Average synaptic weight: " << metrics->averageWeight << std::endl;
            file << "Total spikes: " << metrics->totalSpikes << std::endl;
            file << "Average firing rate: " << metrics->averageFiringRate << std::endl;
            file << "Motor actions:" << std::endl;
            file << "  Forward: " << metrics->forwardCount << std::endl;
            file << "  Backward: " << metrics->backwardCount << std::endl;
            file << "  Turn left: " << metrics->turnLeftCount << std::endl;
            file << "  Turn right: " << metrics->turnRightCount << std::endl;
            file << "  Interact: " << metrics->interactCount << std::endl;
            file << "  Wait: " << metrics->waitCount << std::endl;
            file << "" << std::endl;
            
            // Save experiment data points
            file << "Data points:" << std::endl;
            for (const auto& dataPoint : exp->getDataPoints()) {
                file << "  Step " << dataPoint.step << 
                    ", Neurons " << dataPoint.neuronCount << 
                    ", Synapses " << dataPoint.synapseCount << 
                    ", Active " << dataPoint.activeNeurons << 
                    ", Rate " << dataPoint.firingRate << std::endl;
            }
            file << "" << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Results saved to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving results: " + std::string(e.what()));
        return false;
    }
}

std::string ExperimentRunner::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string ExperimentRunner::developmentalStageToString(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

} // namespace nlm