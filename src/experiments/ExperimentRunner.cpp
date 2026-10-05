#include "Environment.hpp"
#include "GridWorldEnvironment.hpp"
#include "SimpleWorld.hpp"

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

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    for (auto& exp : experiments) {
        // Create fresh brain and environment for each experiment
        // Initialize from template configuration
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        // Create fresh environment based on template
        auto environment = createFreshEnvironment(environmentTemplate);
        
        if (runExperiment(exp, brain, environment, maxSteps)) {
            results.push_back(exp);
        }
    }
    
    return results;
}

std::shared_ptr<Environment> ExperimentRunner::createFreshEnvironment(
    std::shared_ptr<Environment> templateEnv) {
    
    if (!templateEnv) {
        return nullptr;
    }
    
    // Create a new environment instance based on the template
    // Check the template environment type and create appropriate fresh environment
    const char* templateName = templateEnv->getName();
    
    if (strcmp(templateName, "GridWorld") == 0) {
        // For GridWorld environments, extract dimensions and create fresh one
        // This is a simplified approach - in practice, you'd need to access
        // internal state of the template environment
        try {
            // Try to create a GridWorldEnvironment with default dimensions
            auto gridWorldEnv = std::make_shared<GridWorldEnvironment>();
            gridWorldEnv->initialize(16, 16); // Default size
            return gridWorldEnv;
        } catch (...) {
            // Fallback to simple environment
            NLM_LOG_ERROR("Failed to create fresh GridWorldEnvironment, using default");
        }
    } else if (strcmp(templateName, "SimpleWorld") == 0) {
        // Handle SimpleWorld environment
        try {
            auto simpleWorldEnv = std::make_shared<SimpleWorld>();
            simpleWorldEnv->initialize(16, 16);
            return simpleWorldEnv;
        } catch (...) {
            NLM_LOG_ERROR("Failed to create fresh SimpleWorld environment, using default");
        }
    }
    
    // If template type is unknown or cloning fails, create a fresh environment
    // with default parameters
    try {
        auto freshEnv = std::make_shared<Environment>();
        freshEnv->initialize(16, 16);
        return freshEnv;
    } catch (...) {
        // Last resort: create a minimal environment
        NLM_LOG_ERROR("All environment cloning attempts failed, creating minimal environment");
        auto freshEnv = std::make_shared<Environment>();
        freshEnv->initialize(16, 16);
        return freshEnv;
    }
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
    // TODO PHASE 2: Implement results saving
    return false;
}

} // namespace nlm
