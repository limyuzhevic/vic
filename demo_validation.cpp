#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace nlm;

void demonstrateValidation() {
    std::cout << "=== NLM Configuration Validation Demo ===" << std::endl;
    std::cout << std::endl;
    
    // Example 1: Valid configuration
    std::cout << "1. Valid Configuration Example:" << std::endl;
    {
        Config config;
        config.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::File);
        config.set("region_count", static_cast<int64_t>(5), ConfigSource::File);
        config.set("connection_probability", 0.1f, ConfigSource::File);
        config.set("stdp_ltp_weight", 0.01f, ConfigSource::File);
        config.set("stdp_ltd_weight", 0.012f, ConfigSource::File);
        config.set("stdp_tau", 20.0f, ConfigSource::File);
        config.set("synaptogenesis_rate", 0.0001f, ConfigSource::File);
        config.set("pruning_rate", 0.00001f, ConfigSource::File);
        config.set("simulation_timestep", 0.001, ConfigSource::File);
        config.set("replay_interval", 100, ConfigSource::File);
        config.set("consolidation_interval", 1000, ConfigSource::File);
        
        if (config.validate()) {
            std::cout << "   ✓ Configuration is valid" << std::endl;
            std::cout << "   Summary: " << config.summary() << std::endl;
        } else {
            std::cout << "   ✗ Configuration is invalid" << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Example 2: Invalid configuration (region count exceeds neuron count)
    std::cout << "2. Invalid Configuration (region_count > neuron_count):" << std::endl;
    {
        Config config;
        config.set("neuron_count", static_cast<int64_t>(100), ConfigSource::File);
        config.set("region_count", static_cast<int64_t>(200), ConfigSource::File);
        
        if (!config.validate()) {
            std::cout << "   ✓ Correctly rejected invalid configuration" << std::endl;
            std::cout << "   Errors: " << config.summary() << std::endl;
        } else {
            std::cout << "   ✗ Should have rejected invalid configuration" << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Example 3: Invalid configuration (connection probability > 1.0)
    std::cout << "3. Invalid Configuration (connection_probability > 1.0):" << std::endl;
    {
        Config config;
        config.set("neuron_count", static_cast<int64_t>(100), ConfigSource::File);
        config.set("region_count", static_cast<int64_t>(5), ConfigSource::File);
        config.set("connection_probability", 1.5f, ConfigSource::File);
        
        if (!config.validate()) {
            std::cout << "   ✓ Correctly rejected invalid configuration" << std::endl;
            std::cout << "   Errors: " << config.summary() << std::endl;
        } else {
            std::cout << "   ✗ Should have rejected invalid configuration" << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Example 4: File loading with validation
    std::cout << "4. File Loading with Validation:" << std::endl;
    {
        Config config;
        std::string testFile = "demo_config.txt";
        
        // Create a test configuration file
        std::ofstream file(testFile);
        file << "# Demo Configuration File" << std::endl;
        file << "neuron_count = 500" << std::endl;
        file << "region_count = 2" << std::endl;
        file << "connection_probability = 0.2" << std::endl;
        file << "stdp_ltp_weight = 0.02" << std::endl;
        file << "stdp_ltd_weight = 0.025" << std::endl;
        file << "stdp_tau = 25.0" << std::endl;
        file << "synaptogenesis_rate = 0.0002" << std::endl;
        file << "pruning_rate = 0.00002" << std::endl;
        file << "simulation_timestep = 0.002" << std::endl;
        file << "replay_interval = 200" << std::endl;
        file << "consolidation_interval = 2000" << std::endl;
        file.close();
        
        bool loadSuccess = config.loadFromFile(testFile);
        if (loadSuccess && config.validate()) {
            std::cout << "   ✓ File loaded and validated successfully" << std::endl;
            std::cout << "   Config has " << config.getKeys().size() << " parameters" << std::endl;
        } else {
            std::cout << "   ✗ File loading failed or validation failed" << std::endl;
        }
        
        std::remove(testFile.c_str());
    }
    std::cout << std::endl;
    
    // Example 5: Command line argument validation
    std::cout << "5. Command Line Argument Validation:" << std::endl;
    {
        Config config;
        
        const char* argv[] = {"demo", "--neuron_count=1000", "--region_count=5", "--connection_probability=0.5"};
        int argc = 4;
        
        bool loadSuccess = config.loadFromArgs(argc, const_cast<char**>(argv));
        if (loadSuccess && config.validate()) {
            std::cout << "   ✓ Command line arguments loaded and validated" << std::endl;
            std::cout << "   Config has " << config.getKeys().size() << " parameters" << std::endl;
        } else {
            std::cout << "   ✗ Command line loading failed" << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Example 6: Comprehensive validation error reporting
    std::cout << "6. Comprehensive Error Reporting:" << std::endl;
    {
        Config config;
        
        // Set multiple invalid parameters
        config.set("neuron_count", static_cast<int64_t>(10000000), ConfigSource::File); // Too high
        config.set("region_count", static_cast<int64_t>(200), ConfigSource::File); // Too high
        config.set("connection_probability", 1.2f, ConfigSource::File); // Out of range
        
        if (!config.validate()) {
            std::cout << "   ✓ Multiple validation errors caught" << std::endl;
            std::cout << "   Error details:" << std::endl;
            std::string errors = config.summary();
            size_t pos = 0;
            while ((pos = errors.find('\n')) != std::string::npos) {
                std::string line = errors.substr(0, pos);
                if (!line.empty() && line.find("Configuration validation errors:") == std::string::npos) {
                    std::cout << "     - " << line << std::endl;
                }
                errors.erase(0, pos + 1);
            }
        } else {
            std::cout << "   ✗ Should have caught validation errors" << std::endl;
        }
    }
    std::cout << std::endl;
    
    std::cout << "=== Demo Complete ===" << std::endl;
}

int main() {
    demonstrateValidation();
    return 0;
}