// UI demo application
#include "NLMUI.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== NLM UI Demo ===\n\n";
    
    // Create NLM UI
    auto ui = std::make_shared<nlm::NLMUI>();
    
    // Create a simple brain for demo
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    
    auto brain = std::make_shared<nlm::Brain>(config);
    if (brain->initialize()) {
        std::cout << "Brain initialized successfully.\n";
        ui->setBrain(brain);
    } else {
        std::cout << "Failed to initialize brain.\n";
        return 1;
    }
    
    // Start UI
    ui->start();
    
    std::cout << "NLM UI started.\n";
    std::cout << "HTML interface available at: http://localhost:8080\n\n";
    
    // Simulate some updates
    for (int i = 0; i < 10 && ui->isRunning(); ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ui->update(brain, 0.1 * i);
    }
    
    // Stop UI
    ui->stop();
    
    std::cout << "\n=== UI Demo Complete ===\n";
    
    return 0;
}
