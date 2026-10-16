# NLM Configuration Templates

This directory contains example configuration templates for different use cases and complexity levels.

## Template Files

### 1. Minimal Configuration (minimal.json)
```json
{
    "version": "1.0",
    "description": "Minimal NLM Configuration - Basic Testing",
    "author": "NLM Team",
    "brain": {
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.05
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 100
    },
    "environment": {
        "type": "SimpleWorld",
        "width": 10,
        "height": 10
    }
}
```

### 2. Standard Configuration (standard.json)
```json
{
    "version": "1.0",
    "description": "Standard NLM Configuration - Typical Use",
    "author": "NLM Team",
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.001,
                "tau_plus": 20.0,
                "tau_minus": 20.0
            },
            "hebbian": {
                "enable": true,
                "learning_rate": 0.01
            }
        },
        "neuromodulation": {
            "dopamine": {
                "scale": 1.0,
                "baseline": 0.0
            },
            "curiosity": {
                "enable": true,
                "threshold": 0.5
            }
        }
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 1000
    },
    "environment": {
        "type": "SimpleWorld",
        "width": 20,
        "height": 20,
        "vision_width": 8,
        "vision_height": 8
    },
    "logging": {
        "level": "INFO",
        "to_file": true,
        "filename": "nlm_standard.log"
    }
}
```

### 3. High-Performance Configuration (high_perf.json)
```json
{
    "version": "1.0",
    "description": "High-Performance NLM Configuration",
    "author": "NLM Team",
    "brain": {
        "neuron_count": 5000,
        "region_count": 4,
        "connection_probability": 0.15,
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.02,
                "tau_plus": 15.0,
                "tau_minus": 15.0
            },
            "hebbian": {
                "enable": true,
                "learning_rate": 0.02
            }
        },
        "neuromodulation": {
            "dopamine": {
                "scale": 1.5,
                "baseline": 0.1
            },
            "curiosity": {
                "enable": true,
                "threshold": 0.3
            }
        }
    },
    "simulation": {
        "timestep": 0.0005,
        "max_steps": 5000
    },
    "environment": {
        "type": "SimpleWorld",
        "width": 50,
        "height": 50,
        "vision_width": 16,
        "vision_height": 16
    },
    "logging": {
        "level": "WARNING",
        "to_file": true,
        "filename": "nlm_highperf.log"
    },
    "checkpointing": {
        "enable": true,
        "path": "/tmp/nlm_highperf_checkpoint.bin",
        "interval": 2500
    }
}
```

### 4. Research Configuration (research.json)
```json
{
    "version": "1.0",
    "description": "Research-Grade NLM Configuration",
    "author": "NLM Team",
    "brain": {
        "neuron_count": 10000,
        "region_count": 8,
        "connection_probability": 0.12,
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.025,
                "tau_plus": 20.0,
                "tau_minus": 20.0
            },
            "hebbian": {
                "enable": true,
                "learning_rate": 0.025
            },
            "structural": {
                "enable": true,
                "synaptogenesis_rate": 0.001,
                "pruning_rate": 0.0001
            }
        },
        "neuromodulation": {
            "dopamine": {
                "scale": 1.0,
                "baseline": 0.2
            },
            "curiosity": {
                "enable": true,
                "threshold": 0.4
            },
            "novelty": {
                "enable": true
            },
            "prediction_error": {
                "enable": true
            }
        }
    },
    "simulation": {
        "timestep": 0.0002,
        "max_steps": 100000,
        "time_limit": 600.0
    },
    "environment": {
        "type": "SimpleWorld",
        "width": 100,
        "height": 100,
        "vision_width": 32,
        "vision_height": 32
    },
    "logging": {
        "level": "DEBUG",
        "to_file": true,
        "filename": "nlm_research.log",
        "max_size": 10485760
    },
    "checkpointing": {
        "enable": true,
        "path": "/research/checkpoints/experiment_1.bin",
        "interval": 10000,
        "compress": true
    },
    "memory": {
        "working_capacity": 500,
        "episodic_capacity": 10000,
        "replay_interval": 100,
        "consolidation_threshold": 0.8
    },
    "development": {
        "stage": "maturation",
        "plasticity_rate": 1.2,
        "synaptogenesis_rate": 0.002,
        "pruning_rate": 0.0002
    }
}
```

### 5. Legacy Configuration (legacy.cfg)
```
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

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
```

## How to Use Templates

### 1. Copy Template to Your Project
```bash
# Copy a template to your project directory
cp configs/templates/minimal.json my_project_config.json

# Or copy to a different location
cp configs/templates/research.json ~/nlm_config/experiment_config.json
```

### 2. Modify Template
Edit the template to suit your specific needs:

```bash
# Using sed to modify specific parameters
sed -i 's/"neuron_count": 100/"neuron_count": 500/' my_project_config.json
sed -i 's/"max_steps": 100/"max_steps": 1000/' my_project_config.json
```

### 3. Validate Configuration
```bash
# Validate the modified configuration
./nlm_phase6_demo --validate_config my_project_config.json

# If validation fails, fix common issues
./nlm_phase6_demo --fix_config my_project_config.json > fixed_config.json
```

### 4. Run Simulation
```bash
# Run simulation with the template configuration
./nlm_phase6_demo my_project_config.json
```

## Template Modification Guidelines

### 1. Brain Configuration
- **neuron_count**: Adjust based on available memory (100-10000)
- **region_count**: 1-8 regions recommended
- **connection_probability**: 0.05-0.2 typical range

### 2. Plasticity Settings
- **stdp.learning_rate**: 0.001-0.03 typical
- **stdp.tau_plus/tau_minus**: 10.0-30.0 typical
- **hebbian.learning_rate**: 0.01-0.05 typical

### 3. Neuromodulation
- **dopamine.scale**: 0.5-2.0 typical
- **dopamine.baseline**: 0.0-0.5 typical
- **curiosity.threshold**: 0.3-0.7 typical

### 4. Simulation Parameters
- **timestep**: 0.0001-0.001 for accuracy, larger for speed
- **max_steps**: Adjust based on experiment requirements
- **time_limit**: Set based on expected simulation duration

### 5. Environment Configuration
- **width/height**: 10-100 depending on use case
- **vision_width/height**: 4-32 for perception range

## Template Comparison

| Template | Neurons | Regions | Max Steps | Use Case |
|----------|---------|---------|-----------|----------|
| minimal.json | 100 | 1 | 100 | Quick testing |
| standard.json | 1000 | 2 | 1000 | Typical research |
| high_perf.json | 5000 | 4 | 5000 | Performance testing |
| research.json | 10000 | 8 | 100000 | Advanced research |

## Creating Custom Templates

### 1. Command Line Template Creation
```bash
# Create a custom template
cat > custom_config.json << EOF
{
    "brain": {
        "neuron_count": 2000,
        "region_count": 3,
        "plasticity": {
            "stdp": { "learning_rate": 0.015 }
        }
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 2000
    }
}
EOF

# Validate the custom template
./nlm_phase6_demo --validate_config custom_config.json
```

### 2. Template Library Script
```bash
#!/bin/bash

# Template library management script

TEMPLATES_DIR="templates"
BACKUP_DIR="backups"

function list_templates() {
    echo "Available configuration templates:"
    ls -la "$TEMPLATES_DIR"/*.json | head -10
}

function create_template() {
    local name="$1"
    shift
    local description="$1"
    shift
    local output_file="$1"
    shift
    
    cat > "$output_file" << EOF
{
    "version": "1.0",
    "description": "$description",
    "author": "NLM Team",
    "brain": {
        "neuron_count": 500,
        "region_count": 2
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 1000
    }
}
EOF
    echo "Template '$name' created at $output_file"
}

function validate_template() {
    local template_file="$1"
    echo "Validating template: $template_file"
    ./nlm_phase6_demo --validate_config "$template_file"
    return $?
}

# Parse command line arguments
case "$1" in
    list)
        list_templates
        ;;
    create)
        create_template "$2" "$3" "$4"
        ;;
    validate)
        validate_template "$2"
        ;;
    *)
        echo "Usage: $0 {list|create <name> <description> <output_file>|validate <template_file>}"
        ;;
esac
```

## Template Best Practices

### 1. File Naming
- Use descriptive names: `minimal.json`, `standard.json`, `research.json`
- Include version: `template_v1.0.json`
- Use consistent naming: template_[purpose].json

### 2. Documentation
- Include description of template purpose
- Document key parameters and their typical values
- Add usage instructions

### 3. Validation
- Always validate templates before use
- Include validation commands in template documentation
- Test templates with different simulation scenarios

### 4. Testing
- Test templates with different configurations
- Verify performance and memory usage
- Run templates multiple times to ensure consistency

## Template Repository

### 1. Template Organization
```
configs/
├── default.cfg                    # Legacy format
├── default.json                   # JSON format
├── templates/                     # Template files
│   ├── minimal.json
│   ├── standard.json
│   ├── high_perf.json
│   └── research.json
└── backups/                       # Backup templates
    ├── template_v0.9.json
    └── template_v1.0.json
```

### 2. Template Versioning
- Use semantic versioning for templates
- Document changes between versions
- Maintain backward compatibility when possible

### 3. Template Updates
- Update templates regularly based on user feedback
- Add new templates for common use cases
- Remove obsolete or unused templates

## Template Usage Examples

### 1. Using Minimal Template
```bash
# Copy and use minimal template
cp configs/templates/minimal.json .
./nlm_phase6_demo minimal.json
```

### 2. Using Standard Template
```bash
# Copy and modify standard template
cp configs/templates/standard.json my_experiment.json
# Modify parameters as needed
./nlm_phase6_demo my_experiment.json
```

### 3. Creating Custom Template
```bash
# Create a custom template
cat > my_custom_template.json << EOF
{
    "brain": { "neuron_count": 2000 },
    "simulation": { "max_steps": 2000 }
}
EOF

./nlm_phase6_demo --validate_config my_custom_template.json
./nlm_phase6_demo my_custom_template.json
```

## Template Summary

The configuration template system provides:

1. **Pre-configured Templates**: Ready-to-use configurations for different scenarios
2. **Template Library**: Collection of templates for various use cases
3. **Template Management**: Tools for creating and managing templates
4. **Best Practices**: Guidelines for effective template usage
5. **Validation**: Template validation and testing
6. **Documentation**: Comprehensive template documentation

These templates enable users to quickly start with NLM simulations while maintaining flexibility for customization and advanced configuration.