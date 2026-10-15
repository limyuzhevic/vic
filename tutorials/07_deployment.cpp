// Tutorial 7: Deployment and Integration
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <memory>
#include <fstream>
#include <string>

void saveScript(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
        std::cout << "✓ Script saved to " << filename << std::endl;
    } else {
        std::cerr << "✗ Failed to save script to " << filename << std::endl;
    }
}

int main() {
    std::cout << "Tutorial 7: Deployment and Integration" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15);
    config->set("plasticity_learning_rate", 0.01);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "✗ Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Brain initialized successfully!" << std::endl;
    
    // Save deployment script
    std::string deploymentScript = R"(#!/bin/bash
# NLM Deployment Script
# This script demonstrates how to deploy NLM in a production environment

# Configuration
CONFIG_FILE="config.cfg"
OUTPUT_DIR="results"
LOG_FILE="deployment.log"

# Create output directory
mkdir -p $OUTPUT_DIR

# Run NLM with configuration
./nlm --config $CONFIG_FILE --output $OUTPUT_DIR --log-file $LOG_FILE

# Check exit status
if [ $? -eq 0 ]; then
    echo "Deployment completed successfully!"
else
    echo "Deployment failed!"
    exit 1
fi
)";
    
    saveScript("deployment.sh", deploymentScript);
    
    // Make script executable
    system("chmod +x deployment.sh");
    
    std::cout << "✓ Deployment script created and made executable" << std::endl;
    
    // Save integration script
    std::string integrationScript = R"(#!/bin/bash
# NLM Integration Script
# This script demonstrates how to integrate NLM with other systems

# Configuration
NLM_CONFIG="config.cfg"
OTHER_SYSTEM_CONFIG="other_system_config.json"
OUTPUT_DIR="integration_results"

# Create output directory
mkdir -p $OUTPUT_DIR

# Run NLM
./nlm --config $NLM_CONFIG --output $OUTPUT_DIR

# Integrate with other system
python3 integrate_with_other_system.py --nlm-output integration_results.json --other-system-output other_system_output.json

# Check exit status
if [ $? -eq 0 ]; then
    echo "Integration completed successfully!"
else
    echo "Integration failed!"
    exit 1
fi
)";
    
    saveScript("integration.sh", integrationScript);
    
    // Make script executable
    system("chmod +x integration.sh");
    
    std::cout << "✓ Integration script created and made executable" << std::endl;
    
    // Save monitoring script
    std::string monitoringScript = R"(#!/bin/bash
# NLM Monitoring Script
# This script demonstrates how to monitor NLM performance

# Configuration
NLM_OUTPUT_DIR="monitoring_results"
MONITORING_SCRIPT="monitor_nlm.py"

# Create output directory
mkdir -p $NLM_OUTPUT_DIR

# Run NLM with monitoring
./nlm --config config.cfg --output $NLM_OUTPUT_DIR --profile detailed

# Run monitoring script
python3 $MONITORING_SCRIPT --input $NLM_OUTPUT_DIR/results.json --output $NLM_OUTPUT_DIR/monitor.json

# Check exit status
if [ $? -eq 0 ]; then
    echo "Monitoring completed successfully!"
else
    echo "Monitoring failed!"
    exit 1
fi
)";
    
    saveScript("monitoring.sh", monitoringScript);
    
    // Make script executable
    system("chmod +x monitoring.sh");
    
    std::cout << "✓ Monitoring script created and made executable" << std::endl;
    
    // Run deployment script
    std::cout << "Running deployment script..." << std::endl;
    if (system("./deployment.sh") == 0) {
        std::cout << "✓ Deployment script completed successfully!" << std::endl;
    } else {
        std::cout << "✗ Deployment script failed!" << std::endl;
    }
    
    // Run integration script
    std::cout << "Running integration script..." << std::endl;
    if (system("./integration.sh") == 0) {
        std::cout << "✓ Integration script completed successfully!" << std::endl;
    } else {
        std::cout << "✗ Integration script failed!" << std::endl;
    }
    
    // Run monitoring script
    std::cout << "Running monitoring script..." << std::endl;
    if (system("./monitoring.sh") == 0) {
        std::cout << "✓ Monitoring script completed successfully!" << std::endl;
    } else {
        std::cout << "✗ Monitoring script failed!" << std::endl;
    }
    
    std::cout << "✓ Deployment and integration tutorial completed successfully!" << std::endl;
    return 0;
}