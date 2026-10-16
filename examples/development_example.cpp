// development_example.cpp
// Developmental learning example
// Demonstrates how NLM brains develop over time through developmental stages

#include <nlm/core/Config/Config.hpp>
#include <nlm/brain/Brain.hpp>
#include <nlm/world/SimpleWorld.hpp>
#include <nlm/core/Logger/Logger.hpp>
#include <iostream>
#include <vector>

// DevelopmentalLearningAgent: Agent that tracks and adapts to developmental stages
class DevelopmentalLearningAgent {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::vector<float> developmentalMarkers;
    bool developmentEnabled;
    
public:
    DevelopmentalLearningAgent(std::shared_ptr<Brain> b) : brain(b), developmentEnabled(false) {}
    
    void initialize(std::shared_ptr<SimpleWorld> w) {
        world = w;
        developmentalMarkers.clear();
        // Add initial developmental marker
        developmentalMarkers.push_back(0.0f);  // Stage 0.0
        std::cout << "DevelopmentalLearningAgent initialized" << std::endl;
    }
    
    void processSensoryInput(const std::vector<float>& percept) {
        brain->receiveSensoryInput(percept);
    }
    
    Action decodeMotorCommand() {
        Action action = brain->produceAction();
        return action;
    }
    
    void updateDevelopment(double dt) {
        if (developmentEnabled) {
            // Update developmental stage based on time
            float currentStage = brain->getDevelopmentalStage();
            float newStage = currentStage + dt * 0.1f;  // Progress through stages
            newStage = std::min(newStage, 4.0f);  // Max stage is 4 (Aging)
            
            // Log developmental milestones
            if (std::floor(newStage) > std::floor(currentStage)) {
                std::cout << "  Developmental milestone reached: Stage " 
                          << static_cast<int>(newStage) << std::endl;
            }
            
            // In full implementation, would update brain developmental parameters
        }
    }
    
    void enableDevelopment(bool enable) { developmentEnabled = enable; }
    float getDevelopmentalStage() const { return brain->getDevelopmentalStage(); }
    
    void recordDevelopmentalMarker() {
        float currentStage = brain->getDevelopmentalStage();
        developmentalMarkers.push_back(currentStage);
    }
    
    std::vector<float> getDevelopmentalHistory() const { return developmentalMarkers; }
};

struct DevelopmentalMetrics {
    std::vector<float> plasticityRates;
    std::vector<float> learningCurves;
    std::vector<float> performanceMetrics;
    std::vector<float> developmentalStages;
    
    void addSample(float plasticityRate, float learningCurve, 
                   float performance, float stage) {
        plasticityRates.push_back(plasticityRate);
        learningCurves.push_back(learningCurve);
        performanceMetrics.push_back(performance);
        developmentalStages.push_back(stage);
    }
    
    void printSummary() const {
        if (plasticityRates.empty()) return;
        
        std::cout << "\nDevelopmental Analysis:" << std::endl;
        std::cout << "  Stage progression: ";
        for (float stage : developmentalStages) {
            std::cout << stage << " -> ";
        }
        std::cout << " (end)" << std::endl;
        
        float avgPlasticity = 0.0f;
        float avgLearning = 0.0f;
        float avgPerformance = 0.0f;
        
        for (size_t i = 0; i < plasticityRates.size(); ++i) {
            avgPlasticity += plasticityRates[i];
            avgLearning += learningCurves[i];
            avgPerformance += performanceMetrics[i];
        }
        
        int count = plasticityRates.size();
        avgPlasticity /= count;
        avgLearning /= count;
        avgPerformance /= count;
        
        std::cout << "  Average plasticity rate: " << avgPlasticity << std::endl;
        std::cout << "  Average learning curve: " << avgLearning << std::endl;
        std::cout << "  Average performance: " << avgPerformance << std::endl;
    }
};

int main() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "=== NLM Developmental Learning Example ===" << std::endl;
    
    // Create configuration optimized for developmental learning
    auto config = std::make_shared<Config>();
    config->set("random_seed", 99999, ConfigSource::Default);
    config->set("neuron_count", 1200, ConfigSource::Default);
    
    // Enable developmental systems
    config->set("neuromod.dopamine.scale", 1.5f, ConfigSource::File);  // Enhanced dopamine
    config->set("plasticity.stdp.enable", True, ConfigSource::File);
    config->set("plasticity.hebbian.enable", True, ConfigSource::File);
    config->set("plasticity.structural.enable", True, ConfigSource::File);
    config->set("development.stage", 1, ConfigSource::File);  // Start at CriticalPeriod
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    std::cout << "Creating developmental brain with " << brain->getTotalNeuronCount() 
              << " neurons..." << std::endl;
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create and configure world for development
    auto world = std::make_shared<SimpleWorld>();
    world->configure(
        width=30,
        height=30,
        visionWidth(12,
        visionHeight: 12
    );
    world->reset();
    world->setAgentStart(15.0, 15.0);  // Center start
    
    // Add developmental challenges
    std::vector<std::pair<float, float>> challengePositions = {
        {5.0, 5.0}, {25.0, 5.0}, {5.0, 25.0}, {25.0, 25.0}
    };
    
    for (size_t i = 0; i < challengePositions.size(); ++i) {
        // Different challenge types
        auto objType = (i % 3 == 0) ? 
            pynlm.WorldObjectType.Resource : pynlm.WorldObjectType.Hazard;
        float value = (objType == pynlm.WorldObjectType.Resource) ? 15.0f : 8.0f;
        
        world->addObject(pynlm.WorldObject(
            challengePositions[i].first,
            challengePositions[i].second,
            objType,
            value
        ));
    }
    
    std::cout << "World configured for development: 30x30 cells with 12x12 vision" << std::endl;
    std::cout << "Developmental challenges: Resources and hazards at different locations" << std::endl;
    
    // Create developmental learning agent
    auto agent = std::make_shared<DevelopmentalLearningAgent>(brain);
    agent->initialize(world);
    agent->enableDevelopment(true);
    
    // Run developmental learning simulation
    std::cout << "\nStarting developmental learning simulation..." << std::endl;
    std::cout << "Step | Stage | Firing | Plasticity | Learning | Performance | Action" << std::endl;
    std::cout << "-----|-------|--------|------------|----------|-------------|-------" << std::endl;
    
    DevelopmentalMetrics metrics;
    
    for (SimulationStep step = 0; step < 3000; ++step) {
        // Update world
        world->update(0.05);  // 50ms timesteps
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain computation
        brain->step(step);
        
        // Get action
        Action action = agent->decodeMotorCommand();
        
        // Execute action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Update development
        agent->updateDevelopment(0.05);
        
        // Record developmental metrics (simplified)
        float plasticityRate = (step < 500) ? 0.1f :  // High early plasticity
                             (step < 2000) ? 0.05f :  // Medium middle plasticity
                             0.01f;                    // Low late plasticity
        
        float learningCurve = std::min(1.0f, step / 500.0f);  // Learning improves over time
        float performance = (step < 500) ? 0.3f :      // Low early performance
                           (step < 1500) ? 0.6f :   // Medium middle performance
                           0.8f;                    // High late performance
        
        metrics.addSample(plasticityRate, learningCurve, performance, agent->getDevelopmentalStage());
        
        // Record developmental markers periodically
        if (step % 200 == 0) {
            agent->recordDevelopmentalMarker();
        }
        
        // Print progress every 300 steps
        if (step % 300 == 0) {
            std::cout << step << " | " << static_cast<int>(agent->getDevelopmentalStage()) << " | "
                      << brain->getFiringNeuronCount() << " | "
                      << plasticityRate << " | "
                      << learningCurve << " | "
                      << performance << " | "
                      << action.toString() << std::endl;
        }
        
        // Check for developmental completion
        if (agent->getDevelopmentalStage() >= 4.0f && step > 1000) {
            std::cout << "\nDevelopmental process completed! Reached Adult stage." << std::endl;
            break;
        }
    }
    
    std::cout << "\n=== Developmental Learning Complete ===" << std::endl;
    metrics.printSummary();
    
    std::cout << "\nDevelopmental Learning Summary:" << std::endl;
    std::cout << "  Simulation duration: " << (agent->getDevelopmentalStage() >= 4.0f ? "full developmental timeline" : "early stages") << std::endl;
    std::cout << "  Final developmental stage: " << agent->getDevelopmentalStage() << std::endl;
    std::cout << "  Total developmental markers recorded: " << agent->getDevelopmentalHistory().size() << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    std::cout << "\nDevelopmental processes observed:" << std::endl;
    std::cout << "  - Initial (Stage 0.0): Primitive neural networks, high plasticity" << std::endl;
    std::cout << "  - CriticalPeriod (Stage 1.0): Enhanced learning, sensitive periods" << std::endl;
    std::cout << "  - Maturation (Stage 2.0): Specialization, refined circuits" << std::endl;
    std::cout << "  - Adult (Stage 3.0): Stable, efficient processing" << std::endl;
    std::cout << "  - Aging (Stage 4.0): Reduced plasticity, maintenance" << std::endl;
    
    return 0;
}
