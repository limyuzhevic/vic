#include "DependencyInjection.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <memory>

// Sample component that uses dependency injection
class SampleComponent : public nlm::IInjectable {
private:
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Brain> brain;
    std::shared_ptr<nlm::SimpleWorld> world;
    std::shared_ptr<nlm::AgentBrain> agentBrain;
    bool initialized = false;
    
public:
    SampleComponent() = default;
    
    std::string getName() const override {
        return "SampleComponent";
    }
    
    bool initialize() override {
        if (initialized) return true;
        
        auto& container = nlm::DependencyContainer::getInstance();
        
        // Resolve dependencies
        config = container.resolveSafe<nlm::ConfigComponent>("Config");
        brain = container.resolveSafe<nlm::BrainComponent>("Brain");
        world = container.resolveSafe<nlm::WorldComponent>("World");
        agentBrain = container.resolveSafe<nlm::AgentBrainComponent>("AgentBrain");
        
        if (!config || !brain || !world || !agentBrain) {
            std::cerr << "ERROR: Failed to resolve all dependencies for SampleComponent" << std::endl;
            return false;
        }
        
        // Use the components
        std::cout << "SampleComponent initialized successfully:" << std::endl;
        std::cout << "  - Config loaded: " << (config ? "Yes" : "No") << std::endl;
        std::cout << "  - Brain created: " << (brain ? "Yes" : "No") << std::endl;
        std::cout << "  - World created: " << (world ? "Yes" : "No") << std::endl;
        std::cout << "  - AgentBrain created: " << (agentBrain ? "Yes" : "No") << std::endl;
        
        initialized = true;
        return true;
    }
    
    bool shutdown() override {
        if (!initialized) return true;
        
        // Clean up components
        config.reset();
        brain.reset();
        world.reset();
        agentBrain.reset();
        
        initialized = false;
        std::cout << "SampleComponent shutdown successfully" << std::endl;
        return true;
    }
    
    // Public methods for using dependencies
    void runSimulation(int steps) {
        if (!brain) {
            std::cerr << "ERROR: Brain not available" << std::endl;
            return;
        }
        
        std::cout << "Running simulation with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
        std::cout << "and " << brain->getTotalSynapseCount() << " synapses for " << steps << " steps" << std::endl;
        
        // Simulate brain steps
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            if (step % 100 == 0) {
                std::cout << "  Step " << step << "/" << steps 
                          << ", firing neurons: " << brain->getFiringNeuronCount() 
                          << ", total spikes: " << brain->getTotalSpikeCount() << std::endl;
            }
        }
    }
    
    // Get component name for logging
    std::string getComponentName() const {
        return getName();
    }
};

// Factory function for creating SampleComponent
std::unique_ptr<SampleComponent> createSampleComponent() {
    return std::make_unique<SampleComponent>();
}

// Example usage of dependency injection
void demonstrateDependencyInjection() {
    std::cout << "\n=== Dependency Injection Demonstration ===\n";
    
    // Create dependency container
    nlm::DependencyContainer container;
    
    // Register components
    container.registerFactory<nlm::ConfigComponent>("Config", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        return std::make_unique<nlm::ConfigComponent>(config);
    });
    
    container.registerFactory<nlm::LoggerComponent>("Logger", []() {
        return std::make_unique<nlm::LoggerComponent>();
    });
    
    container.registerFactory<nlm::WorldComponent>("World", []() {
        return std::make_unique<nlm::WorldComponent>();
    });
    
    container.registerFactory<SampleComponent>("SampleComponent", createSampleComponent);
    
    // Initialize container
    container.initializeAll();
    
    std::cout << "Dependency container initialized successfully" << std::endl;
    
    // Resolve components
    auto config = container.resolve<nlm::ConfigComponent>("Config");
    auto logger = container.resolve<nlm::LoggerComponent>("Logger");
    auto world = container.resolve<nlm::WorldComponent>("World");
    auto sample = container.resolve<SampleComponent>("SampleComponent");
    
    // Use components
    if (sample) {
        std::cout << "\nRunning simulation using injected components:" << std::endl;
        sample->runSimulation(100);
    }
    
    // Shutdown container
    container.shutdownAll();
    std::cout << "\nDependency container shutdown successfully" << std::endl;
}

// Example of using injected pointers
void demonstrateInjectedPointers() {
    std::cout << "\n=== Injected Pointers Demonstration ===\n";
    
    nlm::DependencyContainer container;
    
    // Register components
    container.registerFactory<nlm::ConfigComponent>("Config", []() {
        return std::make_unique<nlm::ConfigComponent>();
    });
    
    container.registerFactory<nlm::BrainComponent>("Brain", []() {
        return std::make_unique<nlm::BrainComponent>();
    });
    
    // Initialize
    container.initializeAll();
    
    // Use injected pointers
    auto config = nlm::inject<nlm::ConfigComponent>(container, "Config");
    auto brain = nlm::inject<nlm::BrainComponent>(container, "Brain");
    
    std::cout << "Component injection demonstration:" << std::endl;
    std::cout << "  - Config has instance: " << (config.hasInstance() ? "Yes" : "No") << std::endl;
    std::cout << "  - Brain has instance: " << (brain.hasInstance() ? "Yes" : "No") << std::endl;
    
    // Use components
    if (brain.hasInstance()) {
        std::cout << "  - Brain component address: " << brain.get() << std::endl;
        std::cout << "  - Component name: " << brain->getName() << std::endl;
    }
    
    container.shutdownAll();
}

int main() {
    std::cout << "NLM Dependency Injection Demonstration" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Run demonstrations
    demonstrateDependencyInjection();
    demonstrateInjectedPointers();
    
    std::cout << "\n=== All demonstrations completed successfully ===\n";
    
    return 0;
}
