#include "Brain.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <argparse/argparse.hpp>

using namespace nlm;

void run_simulation(int steps, const std::string& output) {
    std::cout << "=== NLM Brain Simulation ===" << std::endl;
    std::cout << "Running simulation for " << steps << " steps..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("connection_probability", 0.15);
    config->set("random_seed", 42);
    config->set("simulation_timestep", 0.001);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return;
    }
    
    // Run simulation
    std::cout << "Starting simulation for " << steps << " steps..." << std::endl;
    
    for (int step = 0; step < steps; ++step) {
        Timestamp currentTime = step * 0.001;
        
        // Simple input pattern
        if (step % 10 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, currentTime);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << "/" << steps 
                     << " | Avg firing rate: " << brain->getAverageFiringRate() 
                     << " | Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        }
    }
    
    std::cout << "Simulation completed successfully!" << std::endl;
    
    // Save output if requested
    if (!output.empty()) {
        if (brain->save(output)) {
            std::cout << "Results saved to " << output << std::endl;
        } else {
            std::cerr << "Failed to save results" << std::endl;
        }
    }
}

void create_configuration(int neurons, int regions, float connection_prob) {
    std::cout << "=== Create Configuration ===" << std::endl;
    std::cout << "Creating configuration with:" << std::endl;
    std::cout << "  Neurons: " << neurons << std::endl;
    std::cout << "  Regions: " << regions << std::endl;
    std::cout << "  Connection probability: " << connection_prob << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", neurons);
    config->set("region_count", regions);
    config->set("connection_probability", connection_prob);
    config->set("random_seed", 42);
    config->set("simulation_timestep", 0.001);
    
    std::cout << "Configuration created successfully!" << std::endl;
}

void run_experiment(const std::string& type, int neurons, int steps) {
    std::cout << "=== Experiment Mode ===" << std::endl;
    std::cout << "Experiment type: " << type << std::endl;
    std::cout << "Neurons: " << neurons << std::endl;
    std::cout << "Steps: " << steps << std::endl;
    
    std::cout << "Starting experiment..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", neurons);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1);
    config->set("random_seed", 42);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return;
    }
    
    // Run experiment with different patterns based on type
    for (int step = 0; step < steps; ++step) {
        Timestamp currentTime = step * 0.001;
        
        // Apply different patterns based on experiment type
        if (type == "integration") {
            if (step % 50 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
        } else if (type == "plasticity") {
            if (step % 10 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
                brain->injectCurrentToNeurons(NeuronType::Motor, 20.0f);
            }
        } else if (type == "learning") {
            // Add learning-specific patterns
            if (step % 20 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
            }
        }
        
        brain->step(step, currentTime);
    }
    
    std::cout << "Experiment completed!" << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser parser("nlm", "NLM Brain Simulation CLI");
    parser.add_argument("--version").default_value("1.0.0");
    parser.add_argument("--verbose").flag();
    
    // Subcommand: run
    auto& run_subcommand = parser.add_subcommand("run", "Run a simulation");
    run_subcommand.add_argument("steps").help("Number of simulation steps").default_value(1000);
    run_subcommand.add_argument("--output").help("Output file path");
    run_subcommand.add_argument("--config").help("Configuration file");
    
    // Subcommand: create
    auto& create_subcommand = parser.add_subcommand("create", "Create configuration");
    create_subcommand.add_argument("--neurons").help("Number of neurons").default_value(1000);
    create_subcommand.add_argument("--regions").help("Number of regions").default_value(1);
    create_subcommand.add_argument("--connection-prob").help("Connection probability").default_value(0.15);
    
    // Subcommand: experiment
    auto& experiment_subcommand = parser.add_subcommand("experiment", "Run experiment");
    experiment_subcommand.add_argument("--type").help("Experiment type").default_value("integration");
    experiment_subcommand.add_argument("--neurons").help("Number of neurons").default_value(500);
    experiment_subcommand.add_argument("--steps").help("Number of steps").default_value(2000);
    
    try {
        parser.parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << parser.help() << std::endl;
        return 1;
    }
    
    if (parser["--version"] == "true") {
        std::cout << "NLM Brain Simulation v1.0.0" << std::endl;
        return 0;
    }
    
    if (parser["--verbose"] == "true") {
        std::cout << "Verbose mode enabled" << std::endl;
    }
    
    if (parser["run"] == "true") {
        int steps = parser.get("steps").as<int>();
        std::string output = parser.get("--output");
        std::string config = parser.get("--config");
        run_simulation(steps, output);
    } else if (parser["create"] == "true") {
        int neurons = parser.get("--neurons").as<int>();
        int regions = parser.get("--regions").as<int>();
        float connection_prob = parser.get("--connection-prob").as<float>();
        create_configuration(neurons, regions, connection_prob);
    } else if (parser["experiment"] == "true") {
        std::string type = parser.get("--type");
        int neurons = parser.get("--neurons").as<int>();
        int steps = parser.get("--steps").as<int>();
        run_experiment(type, neurons, steps);
    } else {
        std::cout << parser.help() << std::endl;
    }
    
    return 0;
}
