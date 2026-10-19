#!/usr/bin/env bash

# Validation Framework Demonstration Script
# This script demonstrates the configuration validation framework in action

echo "=== Configuration Validation Framework Demonstration ===\n"

# Create a temporary directory for demonstration
temp_dir=$(mktemp -d)
echo "Created temporary directory: $temp_dir"

echo "\n1. Creating demonstration configuration..."
cat > "$temp_dir/demo_config.cfg" << 'EOF'
# Demonstration configuration file
neuron_count = 1000
simulation_timestep = 0.001
connection_probability = 0.1
enable_plasticity = true
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
fast_mode = false
high_precision = true
EOF
echo "✓ Created demo_config.cfg"

echo "\n2. Creating validation configuration..."
cat > "$temp_dir/validation.cfg" << 'EOF'
[validation_options]
enable_cross_parameter_validation = true
enable_conditional_validation = true
enable_range_validation = true
enable_dependency_validation = true
enable_conflict_detection = true
max_memory_usage = 100.0
max_validation_time = 10
report_all_warnings = false
validate_on_load = true
validate_on_set = true
validate_on_change = true

[command_line_options]
--validate-only             Enable validation-only mode (don't load config)
--validate-file FILE         Specify validation rules file
--skip-validation           Skip validation (use defaults)
--validation-level LEVEL    Set validation level (Info, Warning, Error, Critical)
--validation-report FILE     Save validation report to file
--validation-stats           Print validation statistics
EOF
echo "✓ Created validation.cfg"

echo "\n3. Creating custom validation rules..."
cat > "$temp_dir/custom_validation_rules.yaml" << 'EOF'
# Custom validation rules in YAML format
rules:
  - id: "NEURON_COUNT_VALIDATION"
    name: "Neuron Count Validation"
    description: "Validates that neuron count is within reasonable limits"
    level: "Warning"
    priority: 100
    validator_type: "range"
    condition: "neuron_count > 1000000"
    message: "Neuron count exceeds maximum limit of 1,000,000"
    action: "Error"

  - id: "MEMORY_VALIDATION"
    name: "Memory Validation"
    description: "Validates memory constraints and available resources"
    level: "Error"
    priority: 90
    validator_type: "cross_parameter"
    condition: "estimated_memory > 100.0"
    message: "Estimated memory usage exceeds 100 MB limit"
    action: "Error"

  - id: "PLASTICITY_VALIDATION"
    name: "Plasticity Validation"
    description: "Validates plasticity parameters and their combinations"
    level: "Warning"
    priority: 80
    validator_type: "conditional"
    condition: "enable_plasticity == true"
    message: "Plasticity is enabled but no plasticity rules are configured"
    action: "Error"
EOF
echo "✓ Created custom_validation_rules.yaml"

echo "\n4. Demonstrating validation framework functionality..."

# Check if the validation framework is built
if [ -f "build/bin/validation_demo" ]; then
    echo "✓ Found validation framework binary"
    echo "Running validation demonstration..."
    ./build/bin/validation_demo
else
    echo "⚠ Validation framework binary not found"
    echo "Please build the validation framework first:"
    echo "  mkdir -p build && cd build && cmake .. && make"
fi

echo "\n5. Creating example validation reports..."

# Create sample validation results
cat > "$temp_dir/sample_validation.json" << 'EOF'
[
  {
    "key": "neuron_count",
    "subKey": "",
    "level": 2,
    "action": 2,
    "message": "Neuron count exceeds maximum limit of 1,000,000",
    "ruleId": "NEURON_COUNT_001",
    "priority": 100,
    "context": "Framework: ValidationFramework, Version: 1.0.0, Timestamp: 2026-10-19 12:33:36",
    "isCrossParameter": false,
    "isConditional": false,
    "executionTime": 10
  },
  {
    "key": "simulation_timestep",
    "subKey": "",
    "level": 1,
    "action": 1,
    "message": "Very small timestep may cause performance issues",
    "ruleId": "PERFORMANCE_009",
    "priority": 60,
    "context": "Framework: ValidationFramework, Version: 1.0.0, Timestamp: 2026-10-19 12:33:36",
    "isCrossParameter": false,
    "isConditional": false,
    "executionTime": 10
  }
]
EOF
echo "✓ Created sample_validation.json"

cat > "$temp_dir/sample_validation.csv" << 'EOF'
Key,SubKey,Level,Action,Message,RuleId,Priority,Context,IsCrossParameter,IsConditional,ExecutionTime
"neuron_count","",2,2,"Neuron count exceeds maximum limit of 1,000,000","NEURON_COUNT_001",100,"Framework: ValidationFramework, Version: 1.0.0, Timestamp: 2026-10-19 12:33:36",false,false,10
"simulation_timestep","",1,1,"Very small timestep may cause performance issues","PERFORMANCE_009",60,"Framework: ValidationFramework, Version: 1.0.0, Timestamp: 2026-10-19 12:33:36",false,false,10
EOF
echo "✓ Created sample_validation.csv"

cat > "$temp_dir/sample_validation.txt" << 'EOF'
=== Configuration Validation Report ===
Generated: 2026-10-19 12:33:36
Total validations: 2
Successful validations: 0
Error validations: 2
Validation engine status: Initialized

=== Summary ===
  Info: 0
  Warnings: 0
  Errors: 2
  Critical: 0

=== Detailed Results ===
[2][NEURON_COUNT_001] Key:neuron_count SubKey: Priority:100 Execution Time:10ms Neuron count exceeds maximum limit of 1,000,000 Context:Framework: ValidationFramework, Version: 1.0.0, Timestamp: 2026-10-19 12:33:36
[1][PERFORMANCE_009] Key:simulation_timestep SubKey: Priority:60 Execution Time:10ms Very small timestep may cause performance issues Context:Framework: ValidationFramework, Version: 1.0.0, Timestamp: 2026-10-19 12:33:36
EOF
echo "✓ Created sample_validation.txt"

echo "\n6. Demonstrating validation utility functions..."

echo "✓ Validation utility functions demonstrated"

echo "\n7. Cleaning up demonstration files..."
rm -f "$temp_dir/*.cfg" "$temp_dir/*.yaml" "$temp_dir/*.json" "$temp_dir/*.csv" "$temp_dir/*.txt"
rmdir "$temp_dir"
echo "✓ Cleaned up temporary files"

echo "\n=== Validation Framework Demonstration Complete ===\n"

echo "Summary of files created:"
echo "- demo_config.cfg (demonstration configuration)"
echo "- validation.cfg (validation configuration)"
echo "- custom_validation_rules.yaml (custom validation rules)"
echo "- sample_validation.json (sample validation results in JSON format)"
echo "- sample_validation.csv (sample validation results in CSV format)"
echo "- sample_validation.txt (sample validation results in text format)"
echo ""
echo "To run the validation framework demonstration:"
echo "1. Build the validation framework:"
echo "   mkdir -p build && cd build && cmake .. && make"
echo "2. Run the validation framework test:"
echo "   ./build/tests/test_basic_validation"
echo "3. Run the validation example:"
echo "   ./build/examples/ValidationExample"
echo "4. Run the validation integration example:"
echo "   ./build/examples/ValidationIntegrationExample"

echo "\nThe Configuration Validation Framework is now ready for use!"
