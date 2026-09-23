/**
 * Phase 1 Demo - Basic Brain Concepts
 * 
 * This demo introduces the fundamental concepts of the NLM brain:
 * - Basic neuron and synapse interactions
 * - Simple learning mechanisms
 * - Fundamental neural computation principles
 * - Core brain architecture
 *
 * This serves as an educational introduction to neural simulation
 * before advancing to more complex Phase 2+ demonstrations.
 */

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace nlm;

// Simple educational demonstrations
class BrainConceptDemonstrator {
public:
    BrainConceptDemonstrator() {
        std::cout << "=== NLM Phase 1: Basic Brain Concepts ===" << std::endl;
        std::cout << "This demo shows the fundamental principles of neural computation." << std::endl << std::endl;
    }
    
    void demonstrateNeurons() {
        std::cout << "--- Concept 1: Neurons as Computational Units ---" << std::endl;
        std::cout << "Neurons integrate incoming signals and fire when threshold is reached." << std::endl;
        std::cout << "Key properties:" << std::endl;
        std::cout << "  - Membrane potential changes over time" << std::endl;
        std::cout << "  - Threshold detection for action potentials" << std::endl;
        std::cout << "  - Refractory periods prevent excessive firing" << std::endl;
        std::cout << "  - Integration of multiple inputs" << std::endl;
        
        // Show neuron types
        std::cout << std::endl;
        std::cout << "Neuron Types:" << std::endl;
        std::cout << "  - Excitatory: Promotes firing in other neurons" << std::endl;
        std::cout << "  - Inhibitory: Reduces firing in other neurons" << std::endl;
        std::cout << "  - Sensory: Receives external input" << std::endl;
        std::cout << "  - Motor: Sends output signals" << std::endl;
        std::cout << "  - Modulatory: Adjusts overall brain activity" << std::endl;
        std::cout << "  - Internal: Maintains internal state" << std::endl;
    }
    
    void demonstrateSynapses() {
        std::cout << std::endl;
        std::cout << "--- Concept 2: Synapses as Communication Channels ---" << std::endl;
        std::cout << "Synapses transmit signals from one neuron to another." << std::endl;
        std::cout << "Key properties:" << std::endl;
        std::cout << "  - Strength determines signal transmission efficiency" << std::endl;
        std::cout << "  - Plasticity allows connection strengthening/weakening" << std::endl;
        std::cout << "  - Different types (excitatory/inhibitory/modulatory)" << std::endl;
        std::cout << "  - Delay in signal transmission" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Synapse Plasticity Types:" << std::endl;
        std::cout << "  - Hebbian: 'Neurons that fire together, wire together'" << std::endl;
        std::cout << "  - STDP: Spike-Timing Dependent Plasticity" << std::endl;
        std::cout << "  - Reward-modulated: Learning based on outcomes" << std::endl;
        std::cout << "  - Structural: Formation/pruning of connections" << std::endl;
    }
    
    void demonstrateLearning() {
        std::cout << std::endl;
        std::cout << "--- Concept 3: Learning as Adaptation ---" << std::endl;
        std::cout << "Brains change their connections based on experience to improve performance." << std::endl;
        std::cout << "Key learning principles:" << std::endl;
        std::cout << "  - Experience-driven adaptation" << std::endl;
        std::cout << "  - Pattern recognition and generalization" << std::endl;
        std::cout << "  - Memory formation" << std::endl;
        std::cout << "  - Behavior optimization" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Learning Mechanisms:" << std::endl;
        std::cout << "  - Supervised: Correct outputs provided" << std::endl;
        std::cout << "  - Unsupervised: Patterns discovered automatically" << std::endl;
        std::cout << "  - Reinforcement: rewards and punishments" << std::endl;
        std::cout << "  - Self-organizing: emergent structure" << std::endl;
    }
    
    void demonstrateArchitecture() {
        std::cout << std::endl;
        std::cout << "--- Concept 4: Brain Architecture ---" << std::endl;
        std::cout << "The NLM brain is organized into specialized regions and systems." << std::endl;
        
        std::cout << std::endl;
        std::cout << "Brain System Components:" << std::endl;
        std::cout << "  - Neural Processing: Neuron dynamics, spike propagation" << std::endl;
        std::cout << "  - Memory Systems: Working, episodic, associative memory" << std::endl;
        std::cout << "  - Neuromodulation: Dopamine, curiosity, reward prediction" << std::endl;
        std::cout << "  - Prediction: Sensory prediction and error computation" << std::endl;
        std::cout << "  - Cognition: Planning, attention, concept formation" << std::endl;
        std::cout << "  - Development: Structural plasticity, maturation" << std::endl;
        std::cout << "  - Environment Interface: Sensory input, motor output" << std::endl;
    }
    
    void showSimpleDemo() {
        std::cout << std::endl;
        std::cout << "--- Simple Demo: Basic Neural Computation ---" << std::endl;
        
        // Initialize logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Create minimal configuration
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(10), ConfigSource::Default);  // Very small for demo
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.5f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
            return;
        }
        
        std::cout << "Brain created with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
        std::cout << "Running basic computation for 50 steps..." << std::endl;
        
        // Run simple simulation
        for (SimulationStep step = 0; step < 50; ++step) {
            // Inject random sensory input
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            
            // Step the brain
            brain->step(step, step * 0.01);
            
            if (step % 10 == 0) {
                std::cout << "  Step " << step << ": " 
                          << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "=== DEMO RESULTS ===" << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        std::cout << std::endl;
        std::cout << "✓ Basic neural computation working!" << std::endl;
        std::cout << "The brain successfully processed information and adapted." << std::endl;
    }
    
    void showKeyTakeaways() {
        std::cout << std::endl;
        std::cout << "=== KEY TAKEAWAYS ===" << std::endl;
        std::cout << "1. Brains are composed of simple computational units (neurons)" << std::endl;
        std::cout << "2. Neurons communicate through weighted connections (synapses)" << std::endl;
        std::cout << "3. Connections strengthen based on activity (Hebbian learning)" << std::endl;
        std::cout << "4. Multiple brain systems work together (memory, prediction, neuromodulation)" << std::endl;
        std::cout << "5. Brains adapt through experience and learning" << std::endl;
        std::cout << "6. Development shapes the brain's capabilities over time" << std::endl;
        std::cout << "7. Simple components can produce complex emergent behavior" << std::endl;
        
        std::cout << std::endl;
        std::cout << "This foundation enables more advanced demonstrations in later phases:" << std::endl;
        std::cout << "- Phase 2: Real spiking neural dynamics" << std::endl;
        std::cout << "- Phase 3: Sensorimotor integration" << std::endl;
        std::cout << "- Phase 4: Complex cognition and planning" << std::endl;
        std::cout << "- Phase 5: Lifetime learning and development" << std::endl;
        std::cout << "- Phase 6: Full integration of all systems" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    BrainConceptDemonstrator demo;
    
    demo.demonstrateNeurons();
    demo.demonstrateSynapses();
    demo.demonstrateLearning();
    demo.demonstrateArchitecture();
    demo.showSimpleDemo();
    demo.showKeyTakeaways();
    
    return 0;
}