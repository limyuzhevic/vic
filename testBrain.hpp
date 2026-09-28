// Compile test for Brain.hpp improvements
#include "Brain.hpp"
#include "BrainBuilder.cpp"

int main() {
    // Test factory methods
    auto defaultBrain = nlm::Brain::createDefaultBrain();
    auto customBrain = nlm::Brain::createBrain(std::make_shared<nlm::Config>());
    
    // Test builder pattern
    auto builder = nlm::Brain::createBrainBuilder();
    auto brain = builder.build();
    
    // Test observer pattern
    auto observer = std::make_shared<nlm::BrainObserver>();
    brain->addObserver(observer);
    
    // Test system accessors
    auto spikeSystem = brain->getSpikeSystem();
    auto memory = brain->getWorkingMemory();
    
    // Test statistics
    auto regionCount = brain->getRegionCount();
    auto neuronCount = brain->getTotalNeuronCount();
    
    // Test versioning
    auto version = nlm::Brain::getVersion();
    
    return 0;
}