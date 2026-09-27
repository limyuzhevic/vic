#include "cli/RunCommand.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

using namespace nlm;
namespace cli {

RunCommand::RunCommand() 
    : CommandBase("run", "Run a simulation with specified configuration"),
      config_file_("configs/default.cfg"),
      steps_(1000),
      duration_(10.0),
      parallel_(false),
      threads_(std::thread::hardware_concurrency()) {
}

bool RunCommand::execute(const std::map<std::string, std::string>& args) {
    std::cout << "Running simulation..." << std::endl;
    std::cout << "Configuration file: " << config_file_ << std::endl;
    std::cout << "Steps: " << steps_ << std::endl;
    std::cout << "Duration: " << duration_ << "s" << std::endl;
    
    if (parallel_) {
        std::cout << "Parallel execution enabled with " << threads_ << " threads" << std::endl;
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    if (config_file_ != "configs/default.cfg") {
        config->loadFromFile(config_file_);
        std::cout << "Loaded configuration from: " << config_file_ << std::endl;
    }
    
    // Override with command line arguments
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", 500, ConfigSource::Default);
    config->set("region_count", 1, ConfigSource::Default);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain!" << std::endl;
        return false;
    }
    
    // Log initial status
    brain->logStatus();
    
    // Run simulation
    std::cout << "Running simulation for " << steps_ << " steps..." << std::endl;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < steps_; ++step) {
        // Inject sensory input periodically
        if (step % 100 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << "/" << steps_ 
                     << " | Spikes: " << brain->getTotalSpikeCount()
                     << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double total_time = std::chrono::duration<double>(end_time - start_time).count();
    
    // Log final status
    brain->logStatus();
    
    // Save results
    std::string output_file = "results/simulation_results_" + getTimestamp() + ".json";
    saveSimulationResults(brain, output_file);
    
    std::cout << std::endl;
    std::cout << "Simulation completed successfully!" << std::endl;
    std::cout << "Total time: " << std::fixed << std::setprecision(2) << total_time << "s" << std::endl;
    std::cout << "Results saved to: " << output_file << std::endl;
    
    return true;
}

void RunCommand::printHelp() const {
    std::cout << "Usage: nlm-cli run [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Run a simulation with the specified configuration." << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config <file>       Configuration file path (overrides default)" << std::endl;
    std::cout << "  -s, --steps <n>       Number of simulation steps to run" << std::endl;
    std::cout << "  -d, --duration <sec>  Simulation duration in seconds" << std::endl;
    std::cout << "  -p, --parallel        Enable parallel execution" << std::endl;
    std::cout << "  -t, --threads <n>     Number of threads for parallel execution" << std::endl;
    std::cout << "  -q, --quiet           Suppress console output" << std::endl;
    std::cout << "  -v, --verbose         Enable verbose logging" << std::endl;
}

void RunCommand::printOptions() const {
    std::cout << "  [command options]" << std::endl;
}

std::string RunCommand::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

json RunCommand::saveSimulationResults(std::shared_ptr<Brain> brain, const std::string& output_file) {
    json results;
    
    results["timestamp"] = getTimestamp();
    results["total_neurons"] = brain->getTotalNeuronCount();
    results["total_synapses"] = brain->getTotalSynapseCount();
    results["total_spikes"] = brain->getTotalSpikeCount();
    results["firing_neuron_count"] = brain->getFiringNeuronCount();
    
    // Calculate additional metrics
    json metrics;
    metrics["average_firing_rate"] = static_cast<double>(brain->getTotalSpikeCount()) / steps_;
    metrics["synaptic_density"] = static_cast<double>(brain->getTotalSynapseCount()) / 
                                 brain->getTotalNeuronCount();
    metrics["connectivity_index"] = 0.5; // Placeholder
    
    results["metrics"] = metrics;
    
    // Save to file
    std::ofstream file(output_file);
    if (file.is_open()) {
        file << results.dump(2);
        file.close();
        std::cout << "Results saved to: " << output_file << std::endl;
    }
    
    return results;
}

} // namespace cli
} // namespace nlm
