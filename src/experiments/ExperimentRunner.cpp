// ExperimentRunner: Runs experiments and collects data

#include "ExperimentRunner.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <fstream>
#include <iomanip>

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    
    Impl() : globalSeed(42) {}
};

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

// Save experiment results
    bool ExperimentRunner::saveResults(const std::string& filepath) const {
        try {
            // Open output file
            std::ofstream outfile(filepath);
            if (!outfile.is_open()) {
                NLM_LOG_ERROR("Failed to open output file: " + filepath);
                return false;
            }
            
            // Write header
            outfile << "# NLM Experiment Results" << std::endl;
            outfile << "# Generated: " << std::ctime(&std::time(nullptr));
            outfile << std::endl;
            
            // Write each experiment
            for (const auto& experiment : pImpl->experiments) {
                outfile << "=== Experiment: " << experiment->getName() << " ===" << std::endl;
                outfile << "Seed: " << experiment->getSeed() << std::endl;
                outfile << std::endl;
                
                // Write step data
                outfile << "StepData:" << std::endl;
                const auto& stepData = experiment->getStepData();
                outfile << "Step,NeuronCount,SynapseCount,FiringCount,AvgFiringRate" << std::endl;
                for (const auto& step : stepData) {
                    outfile << step.step << ","
                            << step.neuronCount << ","
                            << step.synapseCount << ","
                            << step.firingCount << ","
                            << std::fixed << std::setprecision(6) << step.avgFiringRate
                            << std::endl;
                }
                outfile << std::endl;
                
                // Write metrics
                outfile << "Metrics:" << std::endl;
                const auto& metrics = experiment->getMetrics();
                for (const auto& metric : metrics) {
                    outfile << metric.first << ": " << std::fixed << std::setprecision(6) << metric.second << std::endl;
                }
                outfile << std::endl;
            }
            
            outfile.close();
            NLM_LOG_INFO("Experiment results saved to: " + filepath);
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception saving experiment results: ") + e.what());
            return false;
        }
    }

} // namespace nlm
