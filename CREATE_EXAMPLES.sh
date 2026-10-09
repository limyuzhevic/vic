#!/usr/bin/env bash

# NLM Configuration Examples
# Example configuration files demonstrating the improved NLM configuration system

set -e

echo "Creating NLM Configuration Examples..."
echo "====================================="

# Create examples directory
mkdir -p examples

cat > examples/development.cfg << 'EOF'
# NLM Development Configuration
# Optimized for rapid development and testing

# Brain Configuration
brain.neuron_count = 100
brain.region_count = 1
brain.connection_probability = 0.5

# Simulation Configuration
simulation.timestep = 0.001
simulation.max_steps = 1000

# Plasticity Configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.01
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Neuromodulation Configuration
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Development Configuration
development.synaptogenesis_rate = 0.001
development.pruning_rate = 0.0001

# Logging Configuration
log.level = DEBUG
log.to_file = true
log.filename = "nlm_development.log"

# Result Saving
output.prefix = "development_output"
output.save_results = true
EOF

echo "✓ Created development.cfg (development configuration)"

cat > examples/production.cfg << 'EOF'
# NLM Production Configuration
# Optimized for stable, high-performance simulations

# Brain Configuration
brain.neuron_count = 5000
brain.region_count = 2
brain.connection_probability = 0.1
brain.initial_weight_mean = 0.5
brain.initial_weight_std = 0.1

# Simulation Configuration
simulation.timestep = 0.001
simulation.max_steps = 100000
simulation.time_limit = 0.0

# Plasticity Configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.stdp.tau_plus = 20.0
plasticity.stdp.tau_minus = 20.0
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Neuromodulation Configuration
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Development Configuration
development.synaptogenesis_rate = 0.0001
development.pruning_rate = 0.00001

# Logging Configuration
log.level = INFO
log.to_file = true
log.filename = "nlm_production.log"

# Result Saving
output.prefix = "production_output"
output.save_results = true
output.progress_interval = 1000
EOF

echo "✓ Created production.cfg (production configuration)"

cat > examples/research.cfg << 'EOF'
# NLM Research Configuration
# Optimized for scientific research and experimentation

# Brain Configuration
brain.neuron_count = 10000
brain.region_count = 3
brain.connection_probability = 0.05
brain.initial_weight_mean = 0.5
brain.initial_weight_std = 0.15

# Simulation Configuration
simulation.timestep = 0.001
simulation.max_steps = 1000000
simulation.time_limit = 0.0

# Plasticity Configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.005
plasticity.stdp.tau_plus = 25.0
plasticity.stdp.tau_minus = 25.0
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Neuromodulation Configuration
neuromod.dopamine.scale = 2.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Development Configuration
development.synaptogenesis_rate = 0.00005
development.pruning_rate = 0.000005

# Logging Configuration
log.level = INFO
log.to_file = true
log.filename = "nlm_research.log"

# Result Saving
output.prefix = "research_output"
output.save_results = true
output.progress_interval = 5000
output.benchmark_mode = "speed"
EOF

echo "✓ Created research.cfg (research configuration)"

cat > examples/benchmark.cfg << 'EOF'
# NLM Benchmark Configuration
# Optimized for performance benchmarking

# Brain Configuration
brain.neuron_count = 1000
brain.region_count = 2
brain.connection_probability = 0.1

# Simulation Configuration
simulation.timestep = 0.001

# Plasticity Configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001

# Neuromodulation Configuration
neuromod.dopamine.scale = 1.0

# Benchmark Configuration
benchmark.mode = "speed"
benchmark.neuron_counts = [100, 500, 1000, 5000]
benchmark.iterations = 5

# Logging Configuration
log.level = WARNING
log.to_file = false

# Result Saving
output.prefix = "benchmark_output"
output.save_results = true
output.no_progress = false
EOF

echo "✓ Created benchmark.cfg (benchmark configuration)"

cat > examples/simple.json << 'EOF'
{
    "brain": {
        "neuron_count": 2000,
        "region_count": 3,
        "connection_probability": 0.15
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 5000
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.01
        },
        "hebbian": {
            "enable": true
        }
    },
    "log": {
        "level": "INFO",
        "to_file": true,
        "filename": "simple_simulation.log"
    }
}
EOF

echo "✓ Created simple.json (JSON configuration)"

cat > examples/simple.yaml << 'EOF'
brain:
    neuron_count: 3000
    region_count: 4
    connection_probability: 0.12
simulation:
    timestep: 0.001
    max_steps: 10000
plasticity:
    stdp:
        enable: true
        learning_rate: 0.005
    hebbian:
        enable: true
    structural:
        enable: true
neuromodulation:
    dopamine:
        scale: 1.5
    curiosity:
        enable: true
    novelty:
        enable: true
logging:
    level: DEBUG
    to_file: true
    filename: "simple_simulation.log"
output:
    prefix: "simple_output"
    save_results: true
EOF

echo "✓ Created simple.yaml (YAML configuration)"

echo ""
echo "Configuration examples created successfully!"
echo ""
echo "Examples directory contains:"
echo "  - development.cfg - Development configuration"
echo "  - production.cfg - Production configuration"
echo "  - research.cfg - Research configuration"
echo "  - benchmark.cfg - Benchmark configuration"
echo "  - simple.json - JSON format example"
echo "  - simple.yaml - YAML format example"
echo ""
echo "To use these examples, copy them to your project directory and run:"
echo "  ./nlm --config examples/development.cfg"
echo "  ./nlm --config examples/production.cfg"
echo "  ./nlm --config examples/research.cfg"
echo "  ./nlm --config examples/benchmark.cfg"
echo "  ./nlm --config examples/simple.json"
echo "  ./nlm --config examples/simple.yaml"
echo ""
echo "For more information, see config_help.md"
