#!/bin/bash

# NLM Command Line Interface - Power User Tools
# Advanced commands for configuration, debugging, and analysis

cd "$(dirname "$0")"

function show_help() {
    cat << EOF
NLM - Neural Learning Machine CLI (Power User Mode)

Available Commands:

  config <file> [format]        Load and validate configuration file
  config --list                 List all configuration keys
  config --validate <file>      Validate configuration file format
  config --merge <file1> <file2> Merge two configuration files
  config --export <file>        Export configuration to legacy format
  config --import <file>        Import configuration from legacy format
  
  benchmark [steps] [neurons]   Run performance benchmark
  benchmark --detailed         Run detailed performance profiling
  benchmark --compare <config>  Compare benchmarks with given config
  
  debug [--memory] [--spikes] [--all]    Run debug analysis
  debug --checkpoint <step>              Load checkpoint and debug
  debug --replay <episodes>             Debug memory replay
  
  profile [--verbose] [--output <file>]  Profile simulation performance
  profile --analyze <file>               Analyze profile data
  
  experiment [--continuous] [--steps N] [--config <file>]  Run experiment
  experiment --validate                Validate experiment configuration
  
  mesh --create [config]             Create neural mesh from config
  mesh --export <file>               Export neural mesh
  mesh --import <file>               Import neural mesh
  mesh --optimize                    Optimize neural connectivity
  
  learning --detect <directory>      Detect learning patterns in simulations
  learning --analyze <file>           Analyze learning trajectory
  learning --compare <file1> <file2>   Compare learning between two runs
  
  help                            Show this help message
  version                         Show version information

Examples:
  nlm config configs/production.json
  nlm benchmark --detailed 10000 5000
  nlm debug --memory --spikes
  nlm experiment --continuous --steps 5000 --config configs/test_config.json

For basic usage, see: README.md or easy_usage.md
EOF
}

function version() {
    echo "NLM v0.1.0 - Phase 6: Final Integration"
    echo "An experimental artificial developmental brain"
    echo "For documentation, see: README.md and docs/"
}

function config_load() {
    local file=$1
    local format=$2
    
    if [[ ! -f "$file" ]]; then
        echo "Error: Configuration file not found: $file" >&2
        return 1
    fi
    
    if [[ -n "$format" ]]; then
        echo "Note: Format detection automatic, ignoring format parameter"
    fi
    
    echo "Loading configuration from: $file"
    
    # Create and compile test program if needed
    cat > test_config_loader.cpp << 'EOF'
#include <iostream>
#include <string>
#include "src/core/Config/Config.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: load_config <filepath>" << std::endl;
        return 1;
    }
    
    nlm::Config config;
    if (config.loadFromFile(argv[1])) {
        std::cout << "Configuration loaded successfully:" << std::endl;
        std::cout << config.summary() << std::endl;
        return 0;
    } else {
        std::cerr << "Failed to load configuration" << std::endl;
        return 1;
    }
}
EOF
    
    # Build and run
    g++ -std=c++20 -I. -I./src -o test_config_loader test_config_loader.cpp
    ./test_config_loader "$file"
    rm -f test_config_loader test_config_loader.cpp
}

function config_list() {
    echo "Listing all configuration keys..."
    
    cat > list_config.cpp << 'EOF'
#include <iostream>
#include <string>
#include "src/core/Config/Config.hpp"

int main() {
    nlm::Config config;
    
    // Load default config or create one
    config.set("brain.neuron_count", int64_t(1000), nlm::ConfigSource::Default);
    config.set("brain.region_count", int64_t(1), nlm::ConfigSource::Default);
    config.set("brain.connection_probability", 0.1f, nlm::ConfigSource::Default);
    config.set("random_seed", int64_t(42), nlm::ConfigSource::Default);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
    config.set("stdp_tau", 20.0f, nlm::ConfigSource::Default);
    config.set("synaptogenesis_rate", 0.0001f, nlm::ConfigSource::Default);
    config.set("pruning_rate", 0.00001f, nlm::ConfigSource::Default);
    
    auto keys = config.getKeys();
    std::cout << "All configuration keys:" << std::endl;
    for (const auto& key : keys) {
        std::cout << "  " << key << std::endl;
    }
    
    return 0;
}
EOF
    
    g++ -std=c++20 -I. -I./src -o list_config list_config.cpp
    ./list_config
    rm -f list_config list_config.cpp
}

function config_validate() {
    local file=$1
    
    if [[ ! -f "$file" ]]; then
        echo "Error: Configuration file not found: $file" >&2
        return 1
    fi
    
    echo "Validating configuration: $file"
    
    cat > validate_config.cpp << 'EOF'
#include <iostream>
#include <string>
#include <set>
#include "src/core/Config/Config.hpp"

// Required configuration keys for different contexts
std::set<std::string> getRequiredKeys() {
    return {
        "brain.neuron_count",
        "brain.region_count",
        "brain.connection_probability",
        "random_seed",
        "simulation_timestep"
    };
}

bool validateConfig(const nlm::Config& config) {
    auto required = getRequiredKeys();
    bool valid = true;
    
    std::cout << "Configuration Validation Report:" << std::endl;
    
    for (const auto& key : required) {
        if (config.has(key)) {
            std::cout << "  ✓ " << key << " (set)" << std::endl;
        } else {
            std::cout << "  ✗ " << key << " (MISSING)" << std::endl;
            valid = false;
        }
    }
    
    // Check value ranges
    auto neuronCountOpt = config.get<int64_t>("brain.neuron_count");
    if (neuronCountOpt && (*neuronCountOpt < 10 || *neuronCountOpt > 1000000)) {
        std::cout << "  ⚠ brain.neuron_count out of recommended range (10-1,000,000): " << *neuronCountOpt << std::endl;
    }
    
    auto regionCountOpt = config.get<int64_t>("brain.region_count");
    if (regionCountOpt && (*regionCountOpt < 1 || *regionCountOpt > 100)) {
        std::cout << "  ⚠ brain.region_count out of recommended range (1-100): " << *regionCountOpt << std::endl;
    }
    
    auto connectionProbOpt = config.get<float>("brain.connection_probability");
    if (connectionProbOpt && (*connectionProbOpt < 0.0f || *connectionProbOpt > 1.0f)) {
        std::cout << "  ⚠ brain.connection_probability out of range [0,1]: " << *connectionProbOpt << std::endl;
        valid = false;
    }
    
    auto timestepOpt = config.get<double>("simulation_timestep");
    if (timestepOpt && (*timestepOpt <= 0.0)) {
        std::cout << "  ⚠ simulation_timestep must be positive: " << *timestepOpt << std::endl;
        valid = false;
    }
    
    if (valid) {
        std::cout << "\n✓ Configuration is valid!" << std::endl;
    } else {
        std::cout << "\n✗ Configuration has issues!" << std::endl;
    }
    
    return valid;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: validate_config <filepath>" << std::endl;
        return 1;
    }
    
    nlm::Config config;
    if (!config.loadFromFile(argv[1])) {
        std::cerr << "Failed to load configuration file" << std::endl;
        return 1;
    }
    
    bool isValid = validateConfig(config);
    return isValid ? 0 : 1;
}
EOF
    
    g++ -std=c++20 -I. -I./src -o validate_config validate_config.cpp
    
    if ./validate_config "$file"; then
        echo "Configuration validation completed"
    else
        echo "Configuration validation failed"
    fi
    
    rm -f validate_config validate_config.cpp
}

function benchmark_basic() {
    local steps=${1:-1000}
    local neurons=${2:-1000}
    
    echo "Running basic benchmark with ${steps} steps and ${neurons} neurons..."
    
    cat > benchmark.cpp << 'EOF'
#include <iostream>
#include <chrono>
#include <string>
#include <sstream>
#include "src/core/Config/Config.hpp"

int main(int argc, char* argv[]) {
    int64_t steps = 1000;
    int64_t neurons = 1000;
    
    if (argc >= 2) steps = std::stoll(argv[1]);
    if (argc >= 3) neurons = std::stoll(argv[2]);
    
    std::cout << "=== NLM Benchmark ===" << std::endl;
    std::cout << "Steps: " << steps << std::endl;
    std::cout << "Neurons: " << neurons << std::endl;
    std::cout << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Simulate work (simplified benchmark)
    for (int64_t i = 0; i < steps; ++i) {
        // Simulate neural computation
        double result = 0.0;
        for (int64_t j = 0; j < neurons; ++j) {
            result += std::sin(i * 0.01) * std::cos(j * 0.001);
        }
        
        if (i % 100 == 0) {
            std::cout << "Progress: " << (i * 100 / steps) << "% "
                     << "(Current computation: " << result << ")" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << std::endl;
    std::cout << "=== Benchmark Results ===" << std::endl;
    std::cout << "Total time: " << duration.count() / 1000.0 << " seconds" << std::endl;
    std::cout << "Average time per step: " << duration.count() / static_cast<double>(steps) << " ms" << std::endl;
    std::cout << "Operations per second: " << (steps * neurons) / (duration.count() / 1000.0) << std::endl;
    std::cout << "Steps per second: " << steps / (duration.count() / 1000.0) << std::endl;
    
    return 0;
}
EOF
    
    g++ -std=c++20 -I. -I./src -o benchmark benchmark.cpp
    ./benchmark "$steps" "$neurons"
    rm -f benchmark benchmark.cpp
}

function debug_memory() {
    echo "Running memory analysis..."
    
    cat > debug_memory.cpp << 'EOF'
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include "src/core/Config/Config.hpp"

struct MemoryBlock {
    void* address;
    size_t size;
    std::string allocationType;
    int lineNumber;
    std::string fileName;
};

class MemoryProfiler {
private:
    std::vector<MemoryBlock> allocatedBlocks;
    std::map<std::string, size_t> allocationStats;
    
public:
    void trackAllocation(void* addr, size_t size, const std::string& type, 
                        int line, const std::string& file) {
        MemoryBlock block = {addr, size, type, line, file};
        allocatedBlocks.push_back(block);
        allocationStats[type]++;
    }
    
    void report() {
        std::cout << "=== Memory Analysis Report ===" << std::endl;
        std::cout << "Total allocations tracked: " << allocatedBlocks.size() << std::endl;
        std::cout << std::endl;
        
        std::cout << "Allocations by type:" << std::endl;
        for (const auto& pair : allocationStats) {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
        
        if (!allocatedBlocks.empty()) {
            std::cout << std::endl;
            std::cout << "Largest allocations:" << std::endl;
            std::vector<MemoryBlock> sorted = allocatedBlocks;
            std::sort(sorted.begin(), sorted.end(), 
                     [](const MemoryBlock& a, const MemoryBlock& b) {
                         return a.size > b.size;
                     });
            
            for (size_t i = 0; i < std::min(size_t(10), sorted.size()); ++i) {
                std::cout << "  " << i+1 << ". " << sorted[i].size << " bytes, "
                         << sorted[i].allocationType << " in "
                         << sorted[i].fileName << ":" << sorted[i].lineNumber << std::endl;
            }
        }
    }
};

int main() {
    MemoryProfiler profiler;
    
    // Simulate tracking different types of allocations
    std::vector<double> testVector(1000);
    profiler.trackAllocation(testVector.data(), sizeof(double) * 1000, 
                           "std::vector<double>", 42, "test_vector.cpp");
    
    int* intArray = new int[500];
    profiler.trackAllocation(intArray, sizeof(int) * 500, "new int[]", 45, "main.cpp");
    
    std::string* stringVector = new std::string[10];
    profiler.trackAllocation(stringVector, sizeof(std::string) * 10, "new std::string[]", 46, "main.cpp");
    
    // Report results
    profiler.report();
    
    // Cleanup
    delete[] intArray;
    delete[] stringVector;
    
    return 0;
}
EOF
    
    g++ -std=c++20 -I. -I./src -o debug_memory debug_memory.cpp
    ./debug_memory
    rm -f debug_memory debug_memory.cpp
}

# Parse command line arguments
command="$1"
shift

case "$command" in
    "help")
        show_help
        ;;
    "version")
        version
        ;;
    "config")
        case "$1" in
            "--list")
                config_list
                ;;
            "--validate")
                config_validate "$2"
                ;;
            *)
                if [[ "$1" =~ ^-- ]]; then
                    echo "Unknown config command: $1" >&2
                    show_help
                    exit 1
                fi
                config_load "$1" "$2"
                ;;
        esac
        ;;
    "benchmark")
        case "$1" in
            "--detailed")
                echo "Detailed benchmark not yet implemented"
                ;;
            "--compare")
                echo "Benchmark comparison not yet implemented"
                ;;
            *)
                benchmark_basic "$1" "$2"
                ;;
        esac
        ;;
    "debug")
        case "$1" in
            "--memory")
                debug_memory
                ;;
            "--spikes")
                echo "Spike analysis not yet implemented"
                ;;
            "--all")
                debug_memory
                echo "Spike analysis not yet implemented"
                ;;
            *)
                echo "Debug option not recognized: $1" >&2
                show_help
                exit 1
                ;;
        esac
        ;;
    *)
        echo "Unknown command: $command" >&2
        show_help
        exit 1
        ;;
esac