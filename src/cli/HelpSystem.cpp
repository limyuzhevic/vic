// NLM Help System implementation
// Comprehensive help documentation and error reporting

#include "cli/HelpSystem.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace nlm::cli {

// Additional help content for NLM Phase 2
void HelpSystem::showAdditionalHelp() const {
    std::cout << "NLM Phase 2 - Real Neural Computation Help" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Phase 2 Features:" << std::endl;
    std::cout << "  • Leaky Integrate-and-Fire (LIF) neurons" << std::endl;
    std::cout << "  • Event-driven spike propagation with delays" << std::endl;
    std::cout << "  • STDP and Hebbian plasticity rules" << std::endl;
    std::cout << "  • Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Common Configurations:" << std::endl;
    std::cout << "  Basic: ./nlm --config configs/default.cfg" << std::endl;
    std::cout << "  Custom: ./nlm --config myconfig.json --verbose" << std::endl;
    std::cout << "  Debug:  ./nlm --verbose --quiet --help" << std::endl;
    std::cout << std::endl;
    
    std::cout << "File Extensions Supported:" << std::endl;
    std::cout << "  .json  - JSON configuration (most flexible)" << std::endl;
    std::cout << "  .yaml, .yml - YAML configuration (readable)" << std::endl;
    std::cout << "  .cfg, .txt - Key=value format (simple)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Performance Tips:" << std::endl;
    std::cout << "  • Use --verbose to monitor progress" << std::endl;
    std::cout << "  • Adjust neuron_count based on system resources" << std::endl;
    std::cout << "  • Use --config with pre-tuned configurations" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Troubleshooting:" << std::endl;
    std::cout << "  • Configuration file not found: Check path or create default.cfg" << std::endl;
    std::cout << "  • Invalid configuration: Check JSON/YAML syntax" << std::endl;
    std::cout << "  • Memory issues: Reduce neuron_count or region_count" << std::endl;
    std::cout << "  • Performance: Use optimized build (Release mode)" << std::endl;
    std::cout << std::endl;
}

// Quick reference guide
void HelpSystem::showQuickReference() const {
    std::cout << "NLM Quick Reference" << std::endl;
    std::cout << "===================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Basic Usage:" << std::endl;
    std::cout << "  ./nlm                    # Run with default config" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Common Options:" << std::endl;
    std::cout << "  --config <file>     Load configuration from file" << std::endl;
    std::cout << "  --verbose           Enable verbose output" << std::endl;
    std::cout << "  --quiet             Suppress non-essential output" << std::endl;
    std::cout << "  --help              Show help message" << std::endl;
    std::cout << "  --version           Show version information" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Environment Variables:" << std::endl;
    std::cout << "  NLM_CONFIG          Default config file path" << std::endl;
    std::cout << "  NLM_LOG_LEVEL       Logging level (debug, info, warning, error)" << std::endl;
    std::cout << "  NLM_SEED            Random seed for reproducibility" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration Sections:" << std::endl;
    std::cout << "  brain               Neural network parameters" << std::endl;
    std::cout << "  plasticity          Learning rule parameters" << std::endl;
    std::cout << "  neuromodulation      Neuromodulator settings" << std::endl;
    std::cout << "  development         Developmental stage parameters" << std::endl;
    std::cout << "  environment         World/environment settings" << std::endl;
    std::cout << "  logging             Logging configuration" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Key Configuration Files:" << std::endl;
    std::cout << "  configs/default.cfg    - Standard configuration" << std::endl;
    std::cout << "  configs/performance.cfg - Performance-tuned" << std::endl;
    std::cout << "  configs/debug.cfg      - Debug configuration" << std::endl;
    std::cout << std::endl;
}