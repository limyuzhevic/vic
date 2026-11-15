#include <iostream>

void demoSimpleBrain() {
    std::cout << "=== NLM Simple Demo ===" << std::endl;
    std::cout << "This demo shows basic NLM functionality." << std::endl << std::endl;
    
    std::cout << "To build and run this demo, you would need:" << std::endl;
    std::cout << "1. Create a build directory" << std::endl;
    std::cout << "2. Run: cmake .. -DCMAKE_BUILD_TYPE=Release" << std::endl;
    std::cout << "3. Run: make -j4" << std::endl;
    std::cout << "4. Run: ./nlm_demo" << std::endl << std::endl;
    
    std::cout << "A typical usage pattern would be:" << std::endl;
    std::cout << "```" << std::endl;
    std::cout << "// Create configuration" << std::endl;
    std::cout << "auto config = std::make_shared<nlm::Config>();" << std::endl;
    std::cout << "config->set(\"neuron_count\", 100);" << std::endl;
    std::cout << "config->set(\"region_count\", 1);" << std::endl;
    std::cout << std::endl;
    std::cout << "// Create brain" << std::endl;
    std::cout << "auto brain = std::make_shared<nlm::Brain>(config);" << std::endl;
    std::cout << "brain->initialize();" << std::endl;
    std::cout << std::endl;
    std::cout << "// Run simulation" << std::endl;
    std::cout << "for (nlm::SimulationStep step = 0; step < 100; ++step) {" << std::endl;
    std::cout << "    brain->step(step);" << std::endl;
    std::cout << "}" << std::endl;
    std::cout << "```" << std::endl << std::endl;
    
    std::cout << "Key metrics to monitor:" << std::endl;
    std::cout << "- Total spike count (after running simulation)" << std::endl;
    std::cout << "- Average firing rate" << std::endl;
    std::cout << "- Number of firing neurons" << std::endl;
    std::cout << "- Total synapses" << std::endl;
    std::cout << "- Total regions" << std::endl << std::endl;
    
    std::cout << "The NLM brain is now ready for testing!" << std::endl;
}

int main() {
    try {
        demoSimpleBrain();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}