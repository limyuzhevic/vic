#include "ExperimentRunner.hpp"
#include "../visualization/VisualizationInterface.hpp"
#include "../visualization/MonitoringSystem.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    
    Impl() : globalSeed(42) {}
};

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    return exp;
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                     std::shared_ptr<Brain> brain,
                                     std::shared_ptr<Environment> environment,
                                     SimulationStep maxSteps) {
    if (!experiment || !brain || !environment) {
        return false;
    }
    
    experiment->setStartStep(0);
    
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
        
        // Record reward
        float reward = environment->getLastReward();
        experiment->recordMetric("total_reward", 
            experiment->getMetric("total_reward") + reward);
    }
    
    experiment->setEndStep(maxSteps);
    return true;
}

// Enhanced experiment execution with monitoring integration
bool ExperimentRunner::runExperimentWithMonitoring(std::shared_ptr<Experiment> experiment,
                                                  std::shared_ptr<Brain> brain,
                                                  std::shared_ptr<Environment> environment,
                                                  std::shared_ptr<VisualizationInterface> visualization,
                                                  std::shared_ptr<MonitoringSystem> monitoring,
                                                  SimulationStep maxSteps) {
    if (!experiment || !brain || !environment || !visualization || !monitoring) {
        return false;
    }
    
    // Start monitoring
    monitoring->startExperiment(experiment);
    visualization->setMonitoringSystem(monitoring);
    
    experiment->setStartStep(0);
    
    // Reset environment and brain
    auto observation = environment->reset();
    brain->receiveSensoryInput(*observation);
    
    // Run simulation loop with monitoring
    for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
        // Brain step
        brain->step(step);
        
        // Update visualization and monitoring
        visualization->update();
        monitoring->update(0.001); // Fixed timestep for monitoring
        
        // Record data to experiment
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
        
        // Record reward
        float reward = environment->getLastReward();
        experiment->recordMetric("total_reward", 
            experiment->getMetric("total_reward") + reward);
        
        // Capture experiment metrics in monitoring
        for (const auto& metric : experiment->getMetrics()) {
            monitoring->recordTimeSeries("experiment_" + metric.first, {metric.second});
        }
        
        // Generate insights and visualize
        visualization->update();
        
        // Export data periodically
        if (step % 1000 == 0) {
            visualization->exportVisualization("experiment_frame_" + std::to_string(step));
        }
    }
    
    experiment->setEndStep(maxSteps);
    
    // Stop monitoring
    monitoring->stopExperiment();
    
    // Generate final report
    std::string report = monitoring->generateReport();
    experiment->addNote("Monitoring Report:\n" + report);
    
    // Export final results
    visualization->exportVisualization("experiment_final");
    monitoring->exportData("experiment_results", ExportFormat::CSV);
    
    return true;
}

// Run batch of experiments
std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    for (auto& exp : experiments) {
        // Create fresh brain and environment for each experiment
        // PLACEHOLDER: In real implementation, would clone from template
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        // Would also create fresh environment here
        
        if (runExperiment(exp, brain, environmentTemplate, maxSteps)) {
            results.push_back(exp);
        }
    }
    
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
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
}

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    // Enhanced results saving with monitoring integration
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open results file: " + filepath);
        return false;
    }
    
    // Write experiment metadata
    file << "NLM Experiment Results" << std::endl;
    file << "Generated at: " << getCurrentTimestamp() << std::endl;
    file << "Total experiments: " << pImpl->experiments.size() << std::endl;
    file << std::endl;
    
    // Write each experiment results
    for (const auto& exp : pImpl->experiments) {
        file << "=== " << exp->getName() << " ===" << std::endl;
        file << "Seed: " << exp->getSeed() << std::endl;
        file << "Steps: " << exp->getStartStep() << " - " << exp->getEndStep() << std::endl;
        
        // Write metrics
        file << "Metrics:" << std::endl;
        for (const auto& metric : exp->getMetrics()) {
            file << "  " << metric.first << ": " << metric.second << std::endl;
        }
        
        // Write step data summary
        file << "Step Data Points: " << exp->getStepData().size() << std::endl;
        file << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Experiment results saved to: " + filepath);
    return true;
}

// Helper method for timestamp
std::string ExperimentRunner::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm
