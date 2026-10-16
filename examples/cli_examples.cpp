#include "examples/cli_examples.cpp"
#include "NLMCLI.hpp"
#include <iostream>
#include <string>

void demonstrate_basic_usage() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                            Basic Usage Examples                             ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    std::cout << "1. Running a simulation:" << std::endl;
    std::cout << "   nlm-cli run movie_test --neuron_count=1000 --steps=2000 --random_seed=42" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "2. Configuration management:" << std::endl;
    std::cout << "   nlm-cli config create custom_config.cfg" << std::endl;
    std::cout << "   nlm-cli config load custom_config.cfg" << std::endl;
    std::cout << "   nlm-cli config show random_seed neuron_count" << std::endl;
    std::cout << "   nlm-cli config set plasticity.learning_rate 0.001" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "3. Running experiments:" << std::endl;
    std::cout << "   nlm-cli experiment Phase3Demo --steps=5000 --repeats=3" << std::endl;
    std::cout << "   nlm-cli experiment Phase4Demo --output=experiment_results.json" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "4. Visualization:" << std::endl;
    std::cout << "   nlm-cli visualize network_graph --format=dot --viz_type=network" << std::endl;
    std::cout << "   nlm-cli visualize activity_data --format=csv --viz_type=activity" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "5. Performance benchmarking:" << std::endl;
    std::cout << "   nlm-cli benchmark --neurons=2000 --steps=1000 --repeats=4" << std::endl;
    std::cout << "   nlm-cli benchmark --neurons=5000 --steps=500 --output=benchmark_results.csv" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "6. System status:" << std::endl;
    std::cout << "   nlm-cli status" << std::endl;
    std::cout << "   nlm-cli status --verbose" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "7. Help and information:" << std::endl;
    std::cout << "   nlm-cli help" << std::endl;
    std::cout << "   nlm-cli help run" << std::endl;
    std::cout << "   nlm-cli help config" << std::endl;
    std::cout << "" << std::endl;
}

void demonstrate_advanced_usage() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                          Advanced Usage Examples                            ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    std::cout << "1. Configuration templates:" << std::endl;
    std::cout << "   # Create from minimal template" << std::endl;
    std::cout << "   nlm-cli config template minimal > config.cfg" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "   # Create from performance template" << std::endl;
    std::cout << "   nlm-cli config template performance > config.cfg" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "2. Batch processing:" << std::endl;
    std::cout << "   # Run multiple scenarios" << std::endl;
    std::cout << "   for scenario in scene1 scene2 scene3; do" << std::endl;
    std::cout << "       nlm-cli run $scenario --output=$scenario.results" << std::endl;
    std::cout << "   done" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "3. Parameter sweeps:" << std::endl;
    std::cout << "   # Run with different neuron counts" << std::endl;
    std::cout << "   nlm-cli run movie_test --neuron_count=500 --output=results_500.txt" << std::endl;
    std::cout << "   nlm-cli run movie_test --neuron_count=1000 --output=results_1000.txt" << std::endl;
    std::cout << "   nlm-cli run movie_test --neuron_count=2000 --output=results_2000.txt" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "4. Configuration export/import:" << std::endl;
    std::cout << "   # Export to JSON" << std::endl;
    std::cout << "   nlm-cli config export json experiment_config.json" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "   # Import from YAML" << std::endl;
    std::cout << "   nlm-cli config import yaml custom_config.yaml" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "5. Result aggregation:" << std::endl;
    std::cout << "   # Merge results from multiple runs" << std::endl;
    std::cout << "   # Command to combine results from multiple experiment runs" << std::endl;
    std::cout << "   # (Implementation depends on result format)" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "6. Interactive mode:" << std::endl;
    std::cout << "   nlm-cli" << std::endl;
    std::cout << "   >>> help" << std::endl;
    std::cout << "   >>> run movie_test --steps=100" << std::endl;
    std::cout << "   >>> config show random_seed" << std::endl;
    std::cout << "   >>> exit" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "7. Integration with Python scripts:" << std::endl;
    std::cout << "   # Run simulation and capture output" << std::endl;
    std::cout << "   nlm-cli run movie_test --output=simulation_data.csv 2>&1 | python process_results.py" << std::endl;
    std::cout << "" << std::endl;
}

void demonstrate_error_handling() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                           Error Handling Examples                            ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    std::cout << "1. Common errors and solutions:" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "   Error: Unknown command" << std::endl;
    std::cout << "   Solution: nlm-cli help" << std::endl;
    std::cout << "   Shows all available commands" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "   Error: Missing required arguments" << std::endl;
    std::cout << "   Solution: nlm-cli help <command>" << std::endl;
    std::cout << "   Shows usage for the specific command" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "   Error: Configuration file not found" << std::endl;
    std::cout << "   Solution: Create configuration file or use defaults" << std::endl;
    std::cout << "   nlm-cli config create my_config.cfg" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "   Error: Invalid parameter value" << std::endl;
    std::cout << "   Solution: Check parameter range or use validation" << std::endl;
    std::cout << "   nlm-cli config validate" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "2. Troubleshooting tips:" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "   - Use --verbose flag for detailed output" << std::endl;
    std::cout << "   - Use --log-level debug for debugging" << std::endl;
    std::cout << "   - Check configuration file syntax" << std::endl;
    std::cout << "   - Verify file paths exist" << std::endl;
    std::cout << "   - Use 'nlm-cli help <command>' for usage" << std::endl;
    std::cout << "" << std::endl;
}

void demonstrate_performance_usage() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                         Performance Usage Examples                          ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    std::cout << "1. Quick performance test:" << std::endl;
    std::cout << "   nlm-cli benchmark --neurons=100 --steps=100 --repeats=2" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "2. Large-scale benchmark:" << std::endl;
    std::cout << "   nlm-cli benchmark --neurons=5000 --steps=500 --repeats=3" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "3. Benchmarking with specific configuration:" << std::endl;
    std::cout << "   nlm-cli config create benchmark.cfg" << std::endl;
    std::cout << "   nlm-cli config set neuron_count 2000" << std::endl;
    std::cout << "   nlm-cli config set region_count 2" << std::endl;
    std::cout << "   nlm-cli benchmark --config benchmark.cfg --output=bench_results.csv" << std::endl;
    std::cout << "" << std::endl;
}

void main() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                              CLI Usage Examples                             ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "This file provides comprehensive examples of using the NLM CLI." << std::endl;
    std::cout << "Examples are organized by usage complexity and include:" << std::endl;
    std::cout << "  - Basic usage (quick start)" << std::endl;
    std::cout << "  - Advanced usage (power users)" << std::endl;
    std::cout << "  - Error handling (troubleshooting)" << std::endl;
    std::cout << "  - Performance usage (optimization)" << std::endl;
    std::cout << std::endl;
    std::cout << "Run these examples to learn how to use the NLM CLI effectively." << std::endl;
    std::cout << std::endl;
    
    // Uncomment to run specific examples
    // demonstrate_basic_usage();
    // demonstrate_advanced_usage();
    // demonstrate_error_handling();
    // demonstrate_performance_usage();
    
    std::cout << "Examples available for demonstration when executed." << std::endl;
}