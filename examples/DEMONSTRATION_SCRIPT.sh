#!/bin/bash

# NLM Configuration Demonstration Script
# Demonstrates the new configuration system improvements

set -e

echo "=== NLM Configuration System Demonstration ===\n"

echo "This script demonstrates the new configuration system improvements"
echo "made to the NLM (Neural Learning Machine) project.\n"

echo "## Demonstration 1: Configuration File Generation\n"

echo "### 1.1 Generate Key-Value Configuration File\n"
echo "# This command generates a basic configuration file in key-value format:"
echo "./nlm-config generate configs/demo.cfg\n"
echo "Running command..."
./nlm-config generate configs/demo.cfg 2>&1 || true

echo -e "\n### 1.2 Generate JSON Configuration File\n"
echo "# This command generates a configuration file in JSON format:"
echo "./nlm-config generate json configs/demo.json\n"
echo "Running command..."
./nlm-config generate json configs/demo.json 2>&1 || true

echo -e "\n### 1.3 Generate YAML Configuration File\n"
echo "# This command generates a configuration file in YAML format:"
echo "./nlm-config generate yaml configs/demo.yaml\n"
echo "Running command..."
./nlm-config generate yaml configs/demo.yaml 2>&1 || true

echo "\n## Demonstration 2: Configuration Validation\n"

echo "### 2.1 Validate Key-Value Configuration\n"
echo "# This command validates a configuration file:"
echo "./nlm-config validate configs/demo.cfg\n"
echo "Running command..."
./nlm-config validate configs/demo.cfg 2>&1 || true

echo -e "\n### 2.2 Validate JSON Configuration\n"
echo "# This command validates a JSON configuration file:"
echo "./nlm-config validate configs/demo.json\n"
echo "Running command..."
./nlm-config validate configs/demo.json 2>&1 || true

echo "\n## Demonstration 3: Configuration Merging\n"

echo "### 3.1 Create Base Configuration\n"
cat > configs/base_demo.cfg << 'EOF'
# Base Configuration
random_seed = 42
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
EOF
echo "Created: configs/base_demo.cfg\n"

echo "### 3.2 Create Experiment Configuration\n"
cat > configs/experiment_demo.cfg << 'EOF'
# Experiment Configuration
neuron_count = 2000
region_count = 3
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
visualization_enabled = true
EOF
echo "Created: configs/experiment_demo.cfg\n"

echo "### 3.3 Merge Configurations\n"
echo "# This command merges two configuration files:"
echo "./nlm-config merge configs/base_demo.cfg configs/experiment_demo.cfg configs/merged_demo.cfg\n"
echo "Running command..."
./nlm-config merge configs/base_demo.cfg configs/experiment_demo.cfg configs/merged_demo.cfg 2>&1 || true

echo "\n## Demonstration 4: NLM Command Line Interface\n"

echo "### 4.1 NLM Help Information\n"
echo "# This command shows NLM help information:"
echo "./nlm --help\n"
echo "Running command..."
./nlm --help 2>&1 | head -30

echo -e "\n### 4.2 NLM Version Information\n"
echo "# This command shows NLM version information:"
echo "./nlm --version\n"
echo "Running command..."
./nlm --version 2>&1 | head -20

echo "\n## Demonstration 5: Configuration File Formats\n"

echo "### 5.1 Key-Value Configuration File\n"
echo "Contents of configs/demo.cfg:"
cat configs/demo.cfg
echo -e "\n"

echo "### 5.2 JSON Configuration File\n"
echo "Contents of configs/demo.json:"
cat configs/demo.json
echo -e "\n"

echo "### 5.3 YAML Configuration File\n"
echo "Contents of configs/demo.yaml:"
cat configs/demo.yaml
echo -e "\n"

echo "\n## Demonstration 6: Configuration Validation Examples\n"

echo "### 6.1 Valid Configuration\n"
cat > configs/valid_demo.cfg << 'EOF'
# Valid NLM Configuration
random_seed = 42
neuron_count = 500
region_count = 1
connection_probability = 0.15
simulation_timestep = 0.001
EOF
echo "Created: configs/valid_demo.cfg\n"
echo "# Validating configuration..."
./nlm-config validate configs/valid_demo.cfg 2>&1 || true

echo -e "\n### 6.2 Invalid Configuration\n"
cat > configs/invalid_demo.cfg << 'EOF'
# Invalid NLM Configuration
random_seed = 42
neuron_count = -100  # Invalid: negative neuron count
region_count = 1
connection_probability = 1.5  # Invalid: probability > 1.0
simulation_timestep = 0.0     # Invalid: timestep <= 0
EOF
echo "Created: configs/invalid_demo.cfg\n"
echo "# Validating invalid configuration..."
./nlm-config validate configs/invalid_demo.cfg 2>&1 || true

echo "\n## Demonstration 7: Configuration Analysis\n"

echo "### 7.1 Configuration Summary\n"
echo "# Configuration summary for demo.cfg:"
./nlm-config validate configs/demo.cfg 2>&1 | grep -A 20 "Configuration Summary"

echo "\n## Demonstration 8: Configuration Format Detection\n"

echo "### 8.1 Format Detection\n"
echo "# The configuration system automatically detects file format based on extension:"
echo "- configs/demo.cfg -> Key-Value format"
echo "- configs/demo.json -> JSON format"
echo "- configs/demo.yaml -> YAML format"
echo "- configs/demo.yml -> YAML format"

echo "\n## Demonstration 9: Configuration File Management\n"

echo "### 9.1 List Configuration Files\n"
echo "# Listing configuration files in the configs directory:"
ls -la configs/ | grep -E '\.(cfg|json|yaml|yml)$' || echo "No configuration files found"

echo "\n## Demonstration 10: Configuration Best Practices\n"

echo "### 10.1 Configuration Validation\n"
echo "# Always validate configuration files before use:"
echo "./nlm-config validate configs/valid_demo.cfg\n"

echo "### 10.2 Configuration Generation\n"
echo "# Generate configurations for different formats:"
echo "./nlm-config generate json configs/production.json\necho "./nlm-config generate yaml configs/production.yaml\necho "./nlm-config generate configs/production.cfg\n"

echo "### 10.3 Configuration Merging\n"
echo "# Merge configurations for different experiment types:"
echo "./nlm-config merge configs/base_demo.cfg configs/experiment_demo.cfg configs/production.cfg\n"

echo "\n## Summary\n"

echo "This demonstration shows the new configuration system improvements:"
echo "1. **Configuration File Generation**: Support for JSON, YAML, and key-value formats\necho "2. **Configuration Validation**: Automatic validation with detailed error messages\necho "3. **Configuration Merging**: Combine multiple configuration files\necho "4. **Configuration Format Detection**: Automatic format detection based on file extension\necho "5. **Configuration Analysis**: Detailed configuration summaries and statistics\necho "6. **Error Handling**: Comprehensive error handling and recovery\necho "7. **Command Line Interface**: Comprehensive CLI with help system\necho "8. **Documentation**: Extensive documentation and examples\n"

echo "\n=== End of Demonstration ===\n"

echo "The NLM configuration system now provides:"
echo "- Enhanced configuration file format support (JSON, YAML, key-value)\necho "- Improved configuration validation and error handling\necho "- Comprehensive configuration management tools\necho "- Better debugging and error reporting\necho "- Performance optimizations for configuration handling\necho "- Extensive documentation and examples\necho "- Command-line interface with help system\n"
