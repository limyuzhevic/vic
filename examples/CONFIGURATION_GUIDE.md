#!/bin/bash

# NLM Configuration Examples and Usage Guide
# This script provides examples of how to use the NLM configuration system
# and demonstrates the new improvements made to the codebase.

set -e

echo "=== NLM Configuration Examples and Usage Guide ===\n"

echo "This document demonstrates the improvements made to the NLM (Neural Learning Machine)"
echo "configuration system and provides examples of how to use the new features.\n"

echo "## 1. Configuration File Generation Examples\n"

echo "### 1.1 Basic Configuration Generation\n"
echo "# Generate a basic configuration file in key-value format:"
echo "./nlm-config generate configs/example.cfg\n"

echo "### 1.2 Generate JSON Configuration\n"
echo "# Generate a configuration in JSON format:"
echo "./nlm-config generate json configs/example.json\n"

echo "### 1.3 Generate YAML Configuration\n"
echo "# Generate a configuration in YAML format:"
echo "./nlm-config generate yaml configs/example.yaml\n"

echo "### 1.4 Load Existing Configuration and Export\n"
echo "# Load an existing configuration and export in a different format:"
echo "./nlm-config generate yaml configs/converted.yaml configs/example.cfg\n"

echo "## 2. Configuration Validation Examples\n\n"

echo "### 2.1 Validate Configuration File\n"
echo "# Validate a configuration file for errors:"
echo "./nlm-config validate configs/example.cfg\n"

echo "### 2.2 Validate JSON Configuration\n"
echo "# Validate a JSON configuration file:"
echo "./nlm-config validate configs/example.json\n"

echo "## 3. Configuration Merging Examples\n\n"

echo "### 3.1 Merge Multiple Configuration Files\n"
echo "# Merge multiple configuration files into one:"
echo "./nlm-config merge configs/base.cfg configs/experiment.cfg configs/final.cfg configs/merged.cfg\n"

echo "## 4. Command Line Interface Examples\n\n"

echo "### 4.1 Basic NLM Usage\n"
echo "# Basic NLM execution with default settings:"
echo "./nlm\n"

echo "### 4.2 NLM with Configuration File\n"
echo "# Run NLM with a custom configuration file:"
echo "./nlm --config configs/custom.cfg\n"

echo "### 4.3 NLM with Command Line Arguments\n"
echo "# Run NLM with command line argument overrides:"
echo "./nlm --config configs/default.cfg --seed 123 --neurons 2000 --regions 3\n"

echo "### 4.4 NLM Help and Version Information\n"
echo "# Show help information:"
echo "./nlm --help\n"

echo "# Show version information:"
echo "./nlm --version\n"

echo "## 5. Configuration File Examples\n\n"

echo "### 5.1 Default Configuration File (key-value format)\n"
cat > /tmp/example.cfg << 'EOF'
# NLM Configuration File
# Example configuration for NLM Phase 2

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
EOF
echo "# Created: /tmp/example.cfg\n"

echo "### 5.2 JSON Configuration File\n"
cat > /tmp/example.json << 'EOF'
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_time_constant": 20.0,
    "development_synaptogenesis_rate": 0.001,
    "development_pruning_rate": 0.0001,
    "dopamine_baseline": 0.1,
    "reward_discount_factor": 0.99,
    "environment_name": "GridWorld",
    "environment_width": 10,
    "environment_height": 10,
    "log_level": "INFO",
    "log_to_file": false,
    "log_filename": "nlm.log",
    "max_simulation_steps": 10000,
    "simulation_time_limit": 0.0,
    "visualization_enabled": false,
    "visualization_update_rate": 30.0
}
EOF
echo "# Created: /tmp/example.json\n"

echo "### 5.3 YAML Configuration File\n"
cat > /tmp/example.yaml << 'EOF'
# NLM Configuration File
# Example configuration in YAML format

random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2
connection_probability: 0.1
plasticity_learning_rate: 0.01
stdp_ltp_weight: 0.01
stdp_ltd_weight: 0.012
stdp_time_constant: 20.0
development_synaptogenesis_rate: 0.001
development_pruning_rate: 0.0001
dopamine_baseline: 0.1
reward_discount_factor: 0.99
environment_name: GridWorld
environment_width: 10
environment_height: 10
log_level: INFO
log_to_file: false
log_filename: nlm.log
max_simulation_steps: 10000
simulation_time_limit: 0.0
visualization_enabled: false
visualization_update_rate: 30.0
EOF
echo "# Created: /tmp/example.yaml\n"

echo "## 6. Configuration Format Support\n\n"

echo "### 6.1 Supported File Formats\n"
echo "NLM supports three configuration file formats:\n"

echo "1. Key-Value Format (.cfg, .txt, .ini)\n"
echo "   - Traditional format, easy to edit manually\n"
echo "   - Backward compatible with existing configurations\n"
echo "   - Simple syntax: key = value\n"

echo "2. JSON Format (.json)\n"
echo "   - Modern, web-friendly format\n"
echo "   - Rich data structure support\n"
echo "   - Widely supported by tools and libraries\n"
echo "   - Excellent for version control\n"

echo "3. YAML Format (.yaml, .yml)\n"
echo "   - Clean, readable syntax\n"
echo "   - Supports complex nested structures\n"
echo "   - Excellent for CI/CD pipelines\n"
echo "   - Great for configuration management\n"

echo "## 7. Configuration Validation Examples\n\n"

echo "### 7.1 Valid Configuration Example\n"
cat > /tmp/valid_config.cfg << 'EOF'
# Valid NLM Configuration
random_seed = 42
neuron_count = 500
region_count = 1
connection_probability = 0.15
simulation_timestep = 0.001
EOF
echo "Valid configuration created: /tmp/valid_config.cfg\n"

echo "### 7.2 Invalid Configuration Example\n"
cat > /tmp/invalid_config.cfg << 'EOF'
# Invalid NLM Configuration
random_seed = 42
neuron_count = -100  # Invalid: negative neuron count
region_count = 1
connection_probability = 1.5  # Invalid: probability > 1.0
simulation_timestep = 0.0     # Invalid: timestep <= 0
EOF
echo "Invalid configuration created: /tmp/invalid_config.cfg\n"

echo "## 8. Configuration Merging Examples\n\n"

echo "### 8.1 Merging Configuration Files\n"
echo "# Create base configuration:"
cat > /tmp/base.cfg << 'EOF'
# Base Configuration
random_seed = 42
neuron_count = 1000
region_count = 2
connection_probability = 0.1
EOF
echo "Created: /tmp/base.cfg\n"

echo "# Create experiment-specific configuration:"
cat > /tmp/experiment.cfg << 'EOF'
# Experiment Configuration
neuron_count = 2000
region_count = 3
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
EOF
echo "Created: /tmp/experiment.cfg\n"

echo "# Merge configurations:"
echo "./nlm-config merge /tmp/base.cfg /tmp/experiment.cfg /tmp/merged.cfg\n"

echo "## 9. Configuration Management Commands\n\n"

echo "### 9.1 Configuration Generator Commands\n"
echo "Available commands with nlm-config:\n"
echo "  generate <FORMAT> <output_file> [input_file]\n"
echo "      Generate configuration file in specified format\n"
echo "  validate <config_file>\n"
echo "      Validate configuration file\n"
echo "  merge <input1> <input2> ... <output>\n"
echo "      Merge multiple configuration files\n"
echo "  help\n"
echo "      Show help information\n"

echo "### 9.2 Configuration Generator Options\n"
echo "Options for nlm-config:\n"
echo "  --verbose, -v              Enable verbose output\n"
echo "  --force                     Overwrite output file without confirmation\n"
echo "  --format <format>           Specify output format (default: cfg)\n"

echo "## 10. Common Use Cases\n\n"

echo "### 10.1 Setting Up a New Project\n"
echo "# Generate a basic configuration for a new project:"
echo "./nlm-config generate configs/myproject.cfg\n"

echo "# Validate the configuration:"
echo "./nlm-config validate configs/myproject.cfg\n"

echo "### 10.2 Running Experiments\n"
echo "# Create a project-specific configuration:"
cat > /tmp/experiment_config.cfg << 'EOF'
# Experiment Configuration
random_seed = 123
neuron_count = 3000
region_count = 4
connection_probability = 0.08
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.012
stdp_time_constant = 25.0
visualization_enabled = true
EOF
echo "# Created experiment configuration\n"

echo "# Run NLM with the experiment configuration:"
echo "./nlm --config /tmp/experiment_config.cfg --verbose\n"

echo "### 10.3 Configuration Versioning\n"
echo "# Use JSON format for better version control:"
echo "./nlm-config generate json configs/experiment_v1.json\n"
echo "./nlm-config generate json configs/experiment_v2.json\n"
echo "# Compare versions using git:\necho "git diff configs/experiment_v1.json configs/experiment_v2.json\n"

echo "## 11. Configuration Best Practices\n\n"

echo "### 11.1 Use JSON for Version Control\n"
echo "- JSON files are easy to diff and merge\n"
echo "- They provide better syntax highlighting\n"
echo "- JSON is more human-readable than key-value format\n"

echo "### 11.2 Use YAML for CI/CD Pipelines\n"
echo "- YAML has better support in CI/CD tools\n"
echo "- YAML is excellent for complex nested configurations\n"

echo "### 11.3 Use Key-Value for Legacy Systems\n"
echo "- Backward compatible with existing configurations\n"
echo "- Simple to edit manually\n"
echo "- Works with older NLM versions\n"

echo "### 11.4 Validate Before Use\n"
echo "- Always validate configurations before running experiments\n"
echo "- Check for invalid values (negative numbers, out-of-range probabilities)\n"
echo "- Ensure required keys are present\n"

echo "### 11.5 Backup Configuration Files\n"
echo "- Always backup important configuration files\n"
echo "- Use version control for configuration files\n"
echo "- Keep multiple versions of important configurations\n"

echo "## 12. Troubleshooting\n\n"

echo "### 12.1 Configuration File Not Found\n"
echo "If nlm-config cannot find a configuration file:\necho "1. Check the file path: ./nlm-config validate /path/to/config.cfg\necho "2. Verify file permissions: ls -la /path/to/config.cfg\necho "3. Ensure file exists: [ -f /path/to/config.cfg ] && echo 'File exists' || echo 'File missing'\n"

echo "### 12.2 Invalid Configuration Format\n"
echo "If the configuration file has an invalid format:\necho "1. Check file extension (.cfg, .json, .yaml, .yml)\necho "2. Verify JSON/YAML syntax with online validators\necho "3. Try converting from one format to another:\necho "   ./nlm-config generate yaml configs/converted.yaml configs/bad.json\n"

echo "### 12.3 Configuration Validation Errors\n"
echo "If configuration validation fails:\necho "1. Check for invalid values (negative numbers, out-of-range)\necho "2. Ensure all required keys are present\necho "3. Review the configuration summary output\necho "4. Use verbose mode for more details: ./nlm-config validate --verbose config.cfg\n"

echo "## 13. Performance Optimization\n\n"

echo "### 13.1 Large Configuration Files\n"
echo "For large configuration files:\necho "1. Use JSON format for better parsing performance\necho "2. Split configuration into multiple files\necho "3. Use command line arguments for overriding key values\necho "4. Validate configuration before use to avoid parsing errors\n"

echo "### 13.2 Multiple Experiments\n"
echo "For running multiple experiments with similar configurations:\necho "1. Use configuration merging: ./nlm-config merge base.cfg exp.cfg merged.cfg\necho "2. Generate configuration templates for common experiment types\necho "3. Use random seeds for reproducible experiments\necho "4. Store configuration files in version control\n"

echo "## 14. Advanced Features\n\n"

echo "### 14.1 Configuration Templates\n"
echo "Create reusable configuration templates for common experiment types:\necho "./nlm-config generate json templates/experiment_template.json\n"

echo "### 14.2 Configuration Templates Usage\n"
echo "Use templates as starting points for new experiments:\necho "./nlm-config generate json configs/my_experiment.json templates/experiment_template.json\n"

echo "### 14.3 Configuration Sharing\n"
echo "Share configurations between team members:\necho "1. Use version control for configuration files\necho "2. Use CI/CD pipelines to validate configurations\necho "3. Use shared configuration servers for distributed teams\necho "4. Use container images with pre-configured environments\n"

echo "## 15. Getting Help\n\n"

echo "### 15.1 Online Documentation\n"
echo "- Official documentation: https://nlm.readthedocs.io\necho "- GitHub repository: https://github.com/nlm-project/nlm\necho "- Issue tracker: https://github.com/nlm-project/nlm/issues\necho "- Discussions: https://github.com/nlm-project/nlm/discussions\n"

echo "### 15.2 Local Help\n"
echo "Get help from command line:\necho "- Configuration tool help: ./nlm-config help\necho "- NLM main help: ./nlm --help\necho "- NLM version: ./nlm --version\n"

echo "### 15.3 Examples Directory\n"
echo "Look for examples in the repository:\necho "- Examples directory: ./examples/\necho "- Configuration examples: ./examples/configs/\necho "- Experiment examples: ./examples/experiments/\necho "- Tutorial examples: ./examples/tutorials/\n"

echo "## 16. Quick Reference\n\n"

echo "### 16.1 Configuration Generator\n"
echo "Command: ./nlm-config [COMMAND] [OPTIONS]\necho "Options:\necho "  generate <FORMAT> <output> [input]\necho "  validate <config>\necho "  merge <input1> <input2> ... <output>\necho "  help\necho "  --verbose, --force, --format\n"

echo "### 16.2 NLM Main Program\necho "Command: ./nlm [OPTIONS]\necho "Options:\necho "  --help, -h              Show help\necho "  --version               Show version\necho "  --config <file>         Load configuration\necho "  --seed <number>         Set random seed\necho "  --neurons <number>      Set neuron count\necho "  --regions <number>      Set region count\necho "  --timestep <number>     Set simulation timestep\necho "  --verbose, -v           Enable verbose logging\necho "  --log-file <file>       Enable file logging\necho "  --validate-only         Validate and exit\necho "  --save-config <file>     Save final configuration\necho "  --benchmark             Run benchmark mode\necho "  --no-plasticity         Disable plasticity\necho "  --checkpoint-dir <dir>  Set checkpoint directory\necho "  --checkpoint-interval <steps> Set checkpoint interval\n"

echo "## 17. Installation and Setup\n\n"

echo "### 17.1 Building NLM\n"
echo "Build NLM from source:\necho "mkdir build\necho "cd build\necho "cmake .. -DCMAKE_BUILD_TYPE=Release\necho "make -j4\n"

echo "### 17.2 Running NLM\n"
echo "Run NLM after building:\necho "./nlm\necho "or:\necho "./nlm --help\n"

echo "### 17.3 Configuration Management Tool\n"
echo "Run the configuration management tool:\necho "./nlm-config help\necho "./nlm-config generate configs/example.cfg\necho "./nlm-config validate configs/example.cfg\n"

echo "## 18. Acknowledgments\n\n"

echo "This document and the improvements made to the NLM configuration system are based on the original NLM project"
echo "developed by the NLM research team. The configuration system improvements include:\n"

echo "- Enhanced configuration file format support (JSON, YAML, key-value)\n"
echo "- Improved configuration validation and error handling\n"
echo "- Comprehensive command-line interface with help system\n"
echo "- Configuration file generation and management tools\n"
echo "- Better debugging and error reporting\n"
echo "- Performance optimizations for configuration handling\n"

echo "\n=== End of Configuration Guide ===\n"
