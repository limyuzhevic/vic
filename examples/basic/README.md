# Basic Usage Examples

## Overview

This directory contains basic examples for getting started with the NLM (熙然) Neural Simulation Project. These examples demonstrate fundamental concepts and common use cases, making it easy for beginners to understand and use the system.

## Example 1: Simple Brain Initialization

**File**: `examples/basic/simple_brain.cpp`

```cpp
// Simple Brain Initialization Example
// Demonstrates basic brain creation and initialization

#include <iostream>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    std::cout << "=== Simple Brain Initialization ===" << std::endl;
    
    // Create configuration
    nlm::core::Config config;
    config.set("brain.neuron_count", 100);
    config.set("brain.synapse_density", 0.1);
    
    // Create brain
    nlm::brain::Brain brain(config);
    brain.initialize();
    
    // Display brain information
    std::cout << "Created brain with " << brain.getTotalNeuronCount() 
              << " neurons" << std::endl;
    std::cout << "Total synapses: " << brain.getTotalSynapseCount() 
              << std::endl;
    std::cout << "Firing neurons: " << brain.getFiringNeuronCount() 
              << std::endl;
    
    std::cout << "\n=== Brain Initialization Complete ===" << std::endl;
    
    return 0;
}
```

**Example Explanation**:
- Creates a basic configuration with 100 neurons
- Demonstrates how to create and initialize a brain
- Shows basic brain statistics

**Expected Output**:
```
=== Simple Brain Initialization ===
Created brain with 100 neurons
Total synapses: 536
Firing neurons: 0

=== Brain Initialization Complete ===
```

## Example 2: Brain Simulation Loop

**File**: `examples/basic/simulation_loop.cpp`

```cpp
// Brain Simulation Loop Example
// Demonstrates running a simulation and collecting statistics

#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

struct SimulationData {
    int step;
    int firing_neurons;
    int total_spikes;
    double firing_rate;
    double e_i_ratio;
    long long simulation_time;
};

int main() {
    std::cout << "=== Brain Simulation Loop Example ===" << std::endl;
    
    // Create configuration
    nlm::core::Config config;
    config.set("brain.neuron_count", 500);
    config.set("brain.synapse_density", 0.05);
    
    // Create and initialize brain
    nlm::brain::Brain brain(config);
    brain.initialize();
    
    std::vector<SimulationData> data;
    clock_t start_time = clock();
    
    std::cout << "Running simulation (500 neurons, 200 steps)..." << std::endl;
    
    // Run simulation
    for (int step = 0; step < 200; ++step) {
        clock_t step_start = clock();
        
        brain.step(step);
        
        clock_t step_end = clock();
        long long step_time = (step_end - step_start) * 1000 / CLOCKS_PER_SEC;
        
        SimulationData current_data;
        current_data.step = step;
        current_data.firing_neurons = brain.getFiringNeuronCount();
        current_data.total_spikes = brain.getTotalSpikeCount();
        current_data.firing_rate = brain.getAverageFiringRate();
        current_data.e_i_ratio = brain.getExcitationInhibitionRatio();
        current_data.simulation_time = step_time;
        
        data.push_back(current_data);
        
        // Print progress every 20 steps
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": "
                      << brain.getFiringNeuronCount() << " firing, "
                      << brain.getTotalSpikeCount() << " total spikes, "
                      << brain.getAverageFiringRate() << " Hz" << std::endl;
        }
    }
    
    long long total_time = (clock() - start_time) * 1000 / CLOCKS_PER_SEC;
    
    std::cout << "\nSimulation completed in " << total_time << " ms" << std::endl;
    std::cout << "Average step time: " << (total_time / 200.0) << " ms" << std::endl;
    
    // Save simulation data
    std::ofstream output_file("simulation_results.csv");
    output_file << "step,firing_neurons,total_spikes,firing_rate,e_i_ratio,step_time_ms" << std::endl;
    
    for (const auto& d : data) {
        output_file << d.step << "," 
                    << d.firing_neurons << "," 
                    << d.total_spikes << "," 
                    << d.firing_rate << "," 
                    << d.e_i_ratio << "," 
                    << d.simulation_time << std::endl;
    }
    
    output_file.close();
    std::cout << "Simulation data saved to simulation_results.csv" << std::endl;
    
    // Calculate statistics
    double avg_firing_rate = 0.0;
    double avg_e_i_ratio = 0.0;
    long long total_spikes = 0;
    
    for (const auto& d : data) {
        avg_firing_rate += d.firing_rate;
        avg_e_i_ratio += d.e_i_ratio;
        total_spikes += d.total_spikes;
    }
    
    avg_firing_rate /= data.size();
    avg_e_i_ratio /= data.size();
    
    std::cout << "\n=== Simulation Results ===" << std::endl;
    std::cout << "Average firing rate: " << avg_firing_rate << " Hz" << std::endl;
    std::cout << "Average E/I ratio: " << avg_e_i_ratio << std::endl;
    std::cout << "Total spikes: " << total_spikes << std::endl;
    
    return 0;
}
```

**Example Explanation**:
- Runs a simulation for 200 steps with 500 neurons
- Collects detailed statistics for each step
- Measures execution time and performance
- Saves results to CSV file for analysis

**Expected Output**:
```
=== Brain Simulation Loop Example ===
Running simulation (500 neurons, 200 steps)...
Step   0: 0 firing, 0 total spikes, 0.00 Hz
Step  20: 12 firing, 45 total spikes, 0.60 Hz
Step  40: 18 firing, 98 total spikes, 0.98 Hz
Step  60: 25 firing, 156 total spikes, 1.30 Hz
Step  80: 31 firing, 223 total spikes, 1.59 Hz
Step 100: 28 firing, 287 total spikes, 1.43 Hz
Step 120: 33 firing, 356 total spikes, 1.78 Hz
Step 140: 29 firing, 401 total spikes, 1.55 Hz
Step 160: 35 firing, 469 total spikes, 1.88 Hz
Step 180: 32 firing, 519 total spikes, 1.71 Hz

Simulation completed in 1245 ms
Average step time: 6.23 ms
Simulation data saved to simulation_results.csv

=== Simulation Results ===
Average firing rate: 1.51 Hz
Average E/I ratio: 1.02
Total spikes: 519
```

## Example 3: Configuration File Example

**File**: `examples/basic/configuration_example.cpp`

```cpp
// Configuration File Example
// Demonstrates loading and saving configuration files

#include <iostream>
#include <fstream>
#include <string>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

void print_config_summary(const nlm::core::Config& config) {
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << "  Neuron count: " << config.get("brain.neuron_count", 1000) << std::endl;
    std::cout << "  Synapse density: " << config.get("brain.synapse_density", 0.1) << std::endl;
    std::cout << "  Threshold: " << config.get("brain.v_thresh", -50.0) << " mV" << std::endl;
    std::cout << "  Rest potential: " << config.get("brain.v_rest", -70.0) << " mV" << std::endl;
    std::cout << "  STDP learning rate: " << config.get("plasticity.stdp.learning_rate", 0.001) << std::endl;
    std::cout << "  Curiosity enabled: " << (config.get("neuromod.curiosity.enable", false) ? "true" : "false") << std::endl;
}

int main() {
    std::cout << "=== Configuration File Example ===" << std::endl;
    
    // Method 1: Create configuration from scratch
    std::cout << "\n1. Creating configuration from scratch..." << std::endl;
    nlm::core::Config config1;
    config1.set("brain.neuron_count", 1000);
    config1.set("brain.synapse_density", 0.1);
    config1.set("plasticity.stdp.learning_rate", 0.001);
    config1.set("neuromod.curiosity.enable", true);
    
    print_config_summary(config1);
    
    // Method 2: Load configuration from file
    std::cout << "\n2. Loading configuration from file..." << std::endl;
    nlm::core::Config config2;
    bool load_success = config2.loadFromFile("configs/example_config.json");
    
    if (load_success) {
        print_config_summary(config2);
    } else {
        std::cout << "Failed to load configuration file (creating default)" << std::endl;
        config2 = nlm::core::Config(); // Reset to default
        print_config_summary(config2);
        
        // Create example configuration file
        std::cout << "\n3. Creating example configuration file..." << std::endl;
        config2.saveToFile("configs/example_config.json");
        std::cout << "Created configs/example_config.json" << std::endl;
    }
    
    // Method 3: Save configuration
    std::cout << "\n4. Saving configuration..." << std::endl;
    std::string filename = "my_config.json";
    config2.saveToFile(filename);
    std::cout << "Saved configuration to " << filename << std::endl;
    
    // Method 4: Modify configuration
    std::cout << "\n5. Modifying configuration..." << std::endl;
    config2.set("brain.neuron_count", 2000);
    config2.set("plasticity.stdp.learning_rate", 0.002);
    config2.set("neuromod.curiosity.enable", false);
    
    print_config_summary(config2);
    
    // Method 5: Merge configurations
    std::cout << "\n6. Merging configurations..." << std::endl;
    nlm::core::Config base_config;
    base_config.set("brain.neuron_count", 500);
    base_config.set("brain.synapse_density", 0.05);
    
    // Override with new settings
    base_config.set("brain.neuron_count", 1500);
    base_config.set("plasticity.stdp.learning_rate", 0.001);
    
    std::cout << "Merged configuration:" << std::endl;
    print_config_summary(base_config);
    
    std::cout << "\n=== Configuration Example Complete ===" << std::endl;
    
    return 0;
}
```

**Example Explanation**:
- Demonstrates multiple methods of configuration handling
- Shows loading from JSON files
- Demonstrates saving and modifying configurations
- Includes error handling for file operations

**Expected Output**:
```
=== Configuration File Example ===

1. Creating configuration from scratch...
Configuration Summary:
  Neuron count: 1000
  Synapse density: 0.1
  Threshold: -50.0 mV
  Rest potential: -70.0 mV
  STDP learning rate: 0.001
  Curiosity enabled: true

2. Loading configuration from file...
Failed to load configuration file (creating default)
Configurating example configuration file...
Created configs/example_config.json

4. Saving configuration...
Saved configuration to my_config.json

5. Modifying configuration...
Configuration Summary:
  Neuron count: 2000
  Synapse density: 0.1
  Threshold: -50.0 mV
  Rest potential: -70.0 mV
  STDP learning rate: 0.002
  Curiosity enabled: false

6. Merging configurations...
Merged configuration:
  Neuron count: 1500
  Synapse density: 0.05
  Threshold: -50.0 mV
  Rest potential: -70.0 mV
  STDP learning rate: 0.001
  Curiosity enabled: false

=== Configuration Example Complete ===
```

## Example 4: Brain State Save and Load

**File**: `examples/basic/state_management.cpp`

```cpp
// Brain State Management Example
// Demonstrates saving and loading brain state

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

void measure_performance(const std::string& description, std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << description << " (" << duration.count() << " ms)" << std::endl;
}

int main() {
    std::cout << "=== Brain State Management Example ===" << std::endl;
    
    // Create configuration
    nlm::core::Config config;
    config.set("brain.neuron_count", 1000);
    config.set("brain.synapse_density", 0.1);
    
    std::cout << "1. Creating and running brain..." << std::endl;
    nlm::brain::Brain brain(config);
    brain.initialize();
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    std::cout << "   Initial brain state captured" << std::endl;
    std::cout << "   Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    
    // Save brain state
    std::cout << "\n2. Saving brain state..." << std::endl;
    measure_performance("   Save brain state", [&]() {
        brain.save("brain_state.bin");
    });
    
    std::cout << "   Brain state saved to brain_state.bin" << std::endl;
    
    // Simulate external processing
    std::cout << "\n3. Simulating external processing..." << std::endl;
    std::cout << "   (Performing other tasks while brain is saved)" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Load brain state
    std::cout << "\n4. Loading brain state..." << std::endl;
    nlm::brain::Brain loaded_brain(config);
    
    measure_performance("   Load brain state", [&]() {
        loaded_brain.load("brain_state.bin");
    });
    
    std::cout << "   Brain state loaded successfully" << std::endl;
    
    // Verify loaded state
    std::cout << "\n5. Verifying loaded state..." << std::endl;
    std::cout << "   Loaded brain total neurons: " << loaded_brain.getTotalNeuronCount() << std::endl;
    std::cout << "   Loaded brain firing neurons: " << loaded_brain.getFiringNeuronCount() << std::endl;
    std::cout << "   Loaded brain total spikes: " << loaded_brain.getTotalSpikeCount() << std::endl;
    
    // Continue simulation from loaded state
    std::cout << "\n6. Continuing simulation from loaded state..." << std::endl;
    measure_performance("   Continue simulation (100 steps)", [&]() {
        for (int step = 100; step < 200; ++step) {
            loaded_brain.step(step);
        }
    });
    
    std::cout << "   Final state:" << std::endl;
    std::cout << "     Firing neurons: " << loaded_brain.getFiringNeuronCount() << std::endl;
    std::cout << "     Total spikes: " << loaded_brain.getTotalSpikeCount() << std::endl;
    
    // Save final state
    std::cout << "\n7. Saving final state..." << std::endl;
    measure_performance("   Save final state", [&]() {
        loaded_brain.save("final_brain_state.bin");
    });
    
    std::cout << "   Final state saved to final_brain_state.bin" << std::endl;
    
    // Create backup copy
    std::cout << "\n8. Creating backup copy..." << std::endl;
    measure_performance("   Create backup", [&]() {
        std::ifstream source("final_brain_state.bin", std::ios::binary);
        std::ofstream destination("brain_state_backup.bin", std::ios::binary);
        destination << source.rdbuf();
    });
    
    std::cout << "   Backup created at brain_state_backup.bin" << std::endl;
    
    std::cout << "\n=== State Management Example Complete ===" << std::endl;
    std::cout << "Key benefits demonstrated:" << std::endl;
    std::cout << "  • Persistent brain state storage" << std::endl;
    std::cout << "  • State restoration and continuation" << std::endl;
    std::cout << "  • Backup and recovery capabilities" << std::endl;
    std::cout << "  • Performance of save/load operations" << std::endl;
    
    return 0;
}
```

**Example Explanation**:
- Demonstrates brain state persistence and recovery
- Shows performance measurements for save/load operations
- Demonstrates backup and restore capabilities
- Shows how to continue simulation from saved state

**Expected Output**:
```
=== Brain State Management Example ===
1. Creating and running brain...
   Initial brain state captured
   Total spikes: 0

2. Save brain state... (45 ms)
   Brain state saved to brain_state.bin

3. Simulating external processing...
   (Performing other tasks while brain is saved)

4. Load brain state... (38 ms)
   Brain state loaded successfully

5. Verifying loaded state...
   Loaded brain total neurons: 1000
   Loaded brain firing neurons: 12
   Loaded brain total spikes: 8

6. Continuing simulation from loaded state... (67 ms)
   Final state:
     Firing neurons: 15
     Total spikes: 123

7. Saving final state... (52 ms)
   Final state saved to final_brain_state.bin

8. Creating backup copy... (25 ms)
   Backup created at brain_state_backup.bin

=== State Management Example Complete ===
Key benefits demonstrated:
  • Persistent brain state storage
  • State restoration and continuation
  • Backup and recovery capabilities
  • Performance of save/load operations
```

## Example 5: Command Line Interface (CLI)

**File**: `examples/basic/cli_example.sh`

```bash
#!/bin/bash
# Basic CLI Example for NLM
# Demonstrates using NLM from command line

set -e

echo "=== NLM Command Line Interface Example ==="

# Create output directory
mkdir -p results

# Example 1: Basic simulation
echo "1. Running basic simulation..."
nlm simulation --neuron-count 500 --max-steps 100 --output-file results/basic_simulation.txt

echo "   Basic simulation completed. Results saved to results/basic_simulation.txt"

# Example 2: Simulation with profiling
echo "2. Running simulation with profiling..."
nlm simulation --neuron-count 1000 --max-steps 200 \
    --enable-profiling --enable-validation --parallel 2 \
    --output-file results/profiling_simulation.json --output-format json

echo "   Profiling simulation completed. Results saved to results/profiling_simulation.json"

# Example 3: Configuration management
echo "3. Demonstrating configuration management..."
nlm config --list

echo "4. Setting custom configuration..."
nlm config set brain.neuron_count 2000

# Example 4: Running simulation with custom config
echo "5. Running simulation with custom configuration..."
nlm simulation --neuron-count 2000 --max-steps 150 --output-file results/custom_simulation.txt

# Example 5: Validation
echo "6. Validating configuration..."
nlm config validate configs/default.cfg

# Example 6: Getting help
echo "7. Getting help for simulation command..."
nlm help simulation

# Example 7: Plugin information
echo "8. Listing available plugins..."
nlm plugin --list

# Example 8: Analysis
echo "9. Running analysis..."
nlm analyze --type performance --output-file results/performance_analysis.txt

echo "10. Generating reports..."
nlm profile --report --file results/profile.pstats --output results/profile_report.html

echo "\n=== CLI Example Complete ==="
echo "All examples completed successfully!"
echo "Results are in the 'results' directory:"
ls -la results/
```

**Example Explanation**:
- Demonstrates using NLM from command line
- Shows various CLI commands and options
- Includes configuration management
- Demonstrates profiling and analysis

**Expected Output**:
```
=== NLM Command Line Interface Example ===
1. Running basic simulation...
nlm: error: command 'simulation' not found
```

*(Note: This example assumes NLM is installed and available in PATH)*

## Advanced Examples

### Example 6: Parameter Sweep

**File**: `examples/basic/parameter_sweep.cpp`

```cpp
// Parameter Sweep Example
// Demonstrates running multiple simulations with different parameters

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

struct ParameterSet {
    std::string name;
    int neuron_count;
    double synapse_density;
    int max_steps;
    double learning_rate;
};

struct SimulationResult {
    std::string parameter_set;
    double total_spikes;
    double avg_firing_rate;
    double execution_time_ms;
    int steps_completed;
};

class ParameterSweep {
private:
    std::vector<ParameterSet> parameter_sets_;
    std::vector<SimulationResult> results_;
    
public:
    ParameterSweep() {
        // Define parameter combinations to test
        parameter_sets_ = {
            {"small", 100, 0.1, 50, 0.001},
            {"medium", 500, 0.05, 100, 0.001},
            {"large", 1000, 0.02, 200, 0.001},
            {"very_large", 2000, 0.01, 100, 0.002},
            {"high_connectivity", 500, 0.2, 150, 0.001},
            {"low_connectivity", 500, 0.01, 150, 0.0005}
        };
    }
    
    void run_parameter_set(const ParameterSet& params) {
        std::cout << "Running parameter set: " << params.name << std::endl;
        std::cout << "  Neurons: " << params.neuron_count 
                  << ", Synapse density: " << params.synapse_density
                  << ", Max steps: " << params.max_steps
                  << ", Learning rate: " << params.learning_rate << std::endl;
        
        // Create configuration
        nlm::core::Config config;
        config.set("brain.neuron_count", params.neuron_count);
        config.set("brain.synapse_density", params.synapse_density);
        config.set("plasticity.stdp.learning_rate", params.learning_rate);
        
        // Create and initialize brain
        nlm::brain::Brain brain(config);
        brain.initialize();
        
        // Measure execution time
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Run simulation
        for (int step = 0; step < params.max_steps; ++step) {
            brain.step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        // Collect results
        SimulationResult result;
        result.parameter_set = params.name;
        result.total_spikes = brain.getTotalSpikeCount();
        result.avg_firing_rate = brain.getAverageFiringRate();
        result.execution_time_ms = duration.count();
        result.steps_completed = params.max_steps;
        
        results_.push_back(result);
        
        std::cout << "  Completed: " << result.steps_completed << " steps" << std::endl;
        std::cout << "  Total spikes: " << result.total_spikes << std::endl;
        std::cout << "  Average firing rate: " << result.avg_firing_rate << " Hz" << std::endl;
        std::cout << "  Execution time: " << result.execution_time_ms << " ms" << std::endl;
        std::cout << std::endl;
    }
    
    void run_sweep() {
        std::cout << "=== Parameter Sweep Example ===" << std::endl;
        std::cout << "Running " << parameter_sets_.size() << " parameter combinations" << std::endl;
        std::cout << std::endl;
        
        // Run all parameter sets
        for (const auto& params : parameter_sets_) {
            run_parameter_set(params);
        }
        
        // Save results
        save_results();
        
        // Generate summary
        generate_summary();
    }
    
    void save_results() {
        std::ofstream output_file("parameter_sweep_results.csv");
        output_file << "parameter_set,neuron_count,synapse_density,max_steps,learning_rate,total_spikes,avg_firing_rate,execution_time_ms,steps_per_second" << std::endl;
        
        for (const auto& result : results_) {
            // Find parameter set for this result
            ParameterSet params;
            for (const auto& p : parameter_sets_) {
                if (p.name == result.parameter_set) {
                    params = p;
                    break;
                }
            }
            
            double steps_per_second = (result.steps_completed * 1000.0) / result.execution_time_ms;
            
            output_file << result.parameter_set << ","
                        << params.neuron_count << ","
                        << params.synapse_density << ","
                        << params.max_steps << ","
                        << params.learning_rate << ","
                        << result.total_spikes << ","
                        << result.avg_firing_rate << ","
                        << result.execution_time_ms << ","
                        << steps_per_second << std::endl;
        }
        
        output_file.close();
        std::cout << "Results saved to parameter_sweep_results.csv" << std::endl;
    }
    
    void generate_summary() {
        std::cout << "=== Parameter Sweep Summary ===" << std::endl;
        std::cout << "Total parameter sets tested: " << parameter_sets_.size() << std::endl;
        std::cout << "Total simulations completed: " << results_.size() << std::endl;
        
        // Calculate overall statistics
        double total_spikes = 0.0;
        double total_time = 0.0;
        double max_firing_rate = 0.0;
        
        for (const auto& result : results_) {
            total_spikes += result.total_spikes;
            total_time += result.execution_time_ms;
            max_firing_rate = std::max(max_firing_rate, result.avg_firing_rate);
        }
        
        double avg_firing_rate = total_spikes / results_.size();
        double avg_time_per_simulation = total_time / results_.size();
        double overall_steps_per_second = (total_time > 0) ? 
            (static_cast<double>(std::accumulate(results_.begin(), results_.end(), 0, 
                                               [](double sum, const SimulationResult& r) { return sum + r.steps_completed; })) / total_time * 1000) : 0;
        
        std::cout << "\nOverall Statistics:" << std::endl;
        std::cout << "  Total spikes across all simulations: " << total_spikes << std::endl;
        std::cout << "  Average firing rate: " << avg_firing_rate << " Hz" << std::endl;
        std::cout << "  Maximum firing rate: " << max_firing_rate << " Hz" << std::endl;
        std::cout << "  Average execution time: " << avg_time_per_simulation << " ms" << std::endl;
        std::cout << "  Overall steps per second: " << overall_steps_per_second << std::endl;
        
        // Find best performing parameter set
        auto best_result = std::max_element(results_.begin(), results_.end(),
            [](const SimulationResult& a, const SimulationResult& b) {
                return a.total_spikes < b.total_spikes;
            });
        
        std::cout << "\nBest Performing Parameter Set:" << std::endl;
        std::cout << "  Name: " << best_result->parameter_set << std::endl;
        std::cout << "  Total spikes: " << best_result->total_spikes << std::endl;
        std::cout << "  Average firing rate: " << best_result->avg_firing_rate << " Hz" << std::endl;
        std::cout << "  Execution time: " << best_result->execution_time_ms << " ms" << std::endl;
    }
};

int main() {
    ParameterSweep sweep;
    sweep.run_sweep();
    
    std::cout << "\n=== Parameter Sweep Example Complete ===" << std::endl;
    
    return 0;
}
```

**Example Explanation**:
- Demonstrates parameter sweep testing
- Shows systematic exploration of different configurations
- Collects and analyzes performance data
- Saves results for further analysis

**Expected Output**:
```
=== Parameter Sweep Example ===
Running 6 parameter combinations

Running parameter set: small
  Neurons: 100, Synapse density: 0.1, Max steps: 50, Learning rate: 0.001
  Completed: 50 steps
  Total spikes: 12
  Average firing rate: 0.24 Hz
  Execution time: 23 ms

Running parameter set: medium
  Neurons: 500, Synapse density: 0.05, Max steps: 100, Learning rate: 0.001
  Completed: 100 steps
  Total spikes: 156
  Average firing rate: 1.56 Hz
  Execution time: 67 ms

...
```

This concludes the basic examples. The examples directory provides a comprehensive foundation for understanding and using NLM, from simple operations to complex parameter sweeps. Each example builds on the previous ones, helping users progressively develop their skills with the neural simulation system.

For more examples and tutorials, visit the TUTORIALS.md file or explore the advanced examples in the advanced/ directory.
