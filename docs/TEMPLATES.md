# Configuration Templates
# Phase 6 Integration - Configuration Templates

## Overview

Configuration templates provide pre-defined configuration sets for common use cases in NLM simulations. Templates can be customized and are useful for:

1. **Quick setup** - Set up common simulation configurations quickly
2. **Research experiments** - Reproduce experiments with consistent configurations
3. **Development** - Test different configuration scenarios
4. **Training** - Provide examples for users learning NLM

## Available Templates

### 1. Fast Simulation Template (`fast.cfg`)

```ini
# Fast Simulation Template
# Optimized for speed with fewer neurons and simplified connectivity

# Basic configuration - minimal for fast simulation
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500  # Reduced from default 1000
region_count = 1    # Single region for simplicity
connection_probability = 0.05  # Lower connection probability

# Plasticity parameters - basic but functional
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.012
stdp_tau = 15.0

# Structural plasticity - minimal
synaptogenesis_rate = 0.0005
pruning_rate = 0.00005

# Neuromodulation - minimal
novelty_threshold = 0.5
dopamine_initial_level = 0.3

# Memory systems - minimal
working_memory_capacity = 50
max_episodes = 200
episodic_memory_window = 25

# Development - initial only
developmental_stage = "Initial"

# Performance - optimized for speed
enable_performance_optimizations = true
memory_pool_size = 5000
event_queue_capacity = 1000
enable_simd = true
parallel_processing = false

# Logging - minimal for speed
log_level = "warning"
enable_console_logging = false
enable_file_logging = false
```

### 2. Memory Research Template (`memory_research.cfg`)

```ini
# Memory Research Template
# Optimized for memory system research and testing

# Basic configuration - standard for research
random_seed = 123
random_seed = 123
simulation_timestep = 0.001
neuron_count = 2000  # More neurons for memory capacity
region_count = 2     # Multiple regions for spatial memory
connection_probability = 0.15  # Higher connectivity

# Plasticity parameters - stable for memory research
stdp_ltp_weight = 0.018
stdp_ltd_weight = 0.017
stdp_tau = 25.0

# Structural plasticity - balanced
synaptogenesis_rate = 0.0002
pruning_rate = 0.00001

# Neuromodulation - curiosity-focused
curiosity_initial_level = 0.8
novelty_threshold = 0.9
novelty_learning_boost = 1.5

# Memory systems - research-optimized
working_memory_capacity = 300  # Larger working memory
max_episodes = 5000           # Extensive episodic memory
episodic_memory_window = 200  # Longer memory windows
consolidation_threshold = 0.8  # Strong consolidation
replay_frequency = 0.5        # Frequent replay

# Development - extended
developmental_stage = "Maturation"
plasticity_decay_rate = 0.95
neural_excitability_growth = 1.1

# Performance - balanced
enable_performance_optimizations = true
memory_pool_size = 20000
event_queue_capacity = 5000
enable_simd = true
parallel_processing = true

# Logging - detailed for research
log_level = "info"
enable_console_logging = true
enable_file_logging = true
log_rotation_mb = 200
```

### 3. Plasticity Research Template (`plasticity_research.cfg`)

```ini
# Plasticity Research Template
# Optimized for plasticity rule research and testing

# Basic configuration - stable for plasticity research
random_seed = 999
simulation_timestep = 0.001
neuron_count = 1500  # Good size for plasticity studies
region_count = 1     # Single region for focused study
connection_probability = 0.12  # Moderate connectivity

# Plasticity parameters - research-focused
stdp_ltp_weight = 0.025      # Higher LTP for strong learning
stdp_ltd_weight = 0.023      # Higher LTD for balanced learning
stdp_tau = 30.0              # Longer STDP window

# Structural plasticity - extensive
synaptogenesis_rate = 0.001      # High synaptogenesis
pruning_rate = 0.0002            # Moderate pruning
max_synapses_per_neuron = 150    # High synapse capacity

# Neuromodulation - plasticity-focused
dopamine_initial_level = 0.7     # High dopamine for plasticity
dopamine_plasticity_factor = 1.5
novelty_threshold = 0.4

# Memory systems - minimal (focus on plasticity)
working_memory_capacity = 100
max_episodes = 500
episodic_memory_window = 50

# Development - critical period focus
developmental_stage = "CriticalPeriod"
plasticity_decay_rate = 0.9
neural_excitability_growth = 0.95

# Performance - research precision
enable_performance_optimizations = true
memory_pool_size = 15000
event_queue_capacity = 3000
enable_simd = true
parallel_processing = true

# Logging - research-focused
log_level = "debug"
enable_console_logging = false
enable_file_logging = true
log_rotation_mb = 500
```

### 4. Full Integration Template (`full_integration.cfg`)

```ini
# Full Integration Template
# Complete Phase 6 configuration with all systems enabled

# Basic configuration - full scale
random_seed = 777
simulation_timestep = 0.001
neuron_count = 5000  # Maximum for comprehensive integration
region_count = 4     # Multiple regions for complex integration
connection_probability = 0.2  # High connectivity

# Plasticity parameters - full range
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.025
stdp_tau = 40.0

# Structural plasticity - full
synaptogenesis_rate = 0.002
pruning_rate = 0.0001
max_synapses_per_neuron = 200

# Neuromodulation - complete
curiosity_initial_level = 0.6
novelty_threshold = 0.7
prediction_error_initial = 0.1

# Memory systems - full capacity
working_memory_capacity = 1000
max_episodes = 10000
episodic_memory_window = 500
consolidation_threshold = 0.9
replay_frequency = 0.8

# Development - all stages
developmental_stage = "Adult"
plasticity_decay_rate = 0.99
neural_excitability_growth = 1.2

# Cognition systems - complete
planning_depth = 10
concept_threshold = 0.8
attention_strength = 1.0

# Prediction systems - full
prediction_accuracy_threshold = 0.9
prediction_update_rate = 0.2
confidence_decay_rate = 0.98

# Environment - complex
env_width = 300
env_height = 200
sensory_range = 20
action_resolution = 0.005

# Checkpoint - frequent
checkpoint_dir = "./checkpoints"
checkpoint_interval = 500
max_checkpoints = 50
checkpoint_compression = "high"

# Performance - maximum
enable_performance_optimizations = true
memory_pool_size = 50000
event_queue_capacity = 20000
enable_simd = true
parallel_processing = true

# Logging - comprehensive
log_level = "trace"
enable_console_logging = true
enable_file_logging = true
log_rotation_mb = 1000
log_format = "structured"

# Batch processing - enabled
batch_processing = true
batch_size = 20
parallel_batch = true

# Advanced features - enabled
environment_variables = false
dynamic_reloading = true
validation = true
history = true
```

### 5. Custom Template (`custom_template.cfg`)

```ini
# Custom Template
# Template for creating custom configurations
# Copy this template and modify as needed

# Basic configuration - modify as needed
# [random_seed] = 42
# [simulation_timestep] = 0.001
# [neuron_count] = 1000
# [region_count] = 1
# [connection_probability] = 0.1

# Plasticity parameters - modify as needed
# [stdp_ltp_weight] = 0.02
# [stdp_ltd_weight] = 0.015
# [stdp_tau] = 20.0
# [synaptogenesis_rate] = 0.0001
# [pruning_rate] = 0.00001

# Neuromodulation - modify as needed
# [dopamine_initial_level] = 0.5
# [curiosity_initial_level] = 0.3
# [novelty_threshold] = 0.7
# [prediction_error_initial] = 0.0

# Memory systems - modify as needed
# [working_memory_capacity] = 100
# [max_episodes] = 1000
# [episodic_memory_window] = 50
# [consolidation_threshold] = 0.7

# Development - modify as needed
# [developmental_stage] = "Initial"
# [plasticity_decay_rate] = 0.9
# [neural_excitability_growth] = 1.05

# Cognition systems - modify as needed
# [planning_depth] = 5
# [concept_threshold] = 0.7
# [attention_strength] = 0.8

# Prediction systems - modify as needed
# [prediction_accuracy_threshold] = 0.8
# [prediction_update_rate] = 0.1
# [confidence_decay_rate] = 0.95

# Environment - modify as needed
# [env_width] = 100
# [env_height] = 100
# [sensory_range] = 10
# [action_resolution] = 0.01

# Checkpoint - modify as needed
# [checkpoint_dir] = "./checkpoints"
# [checkpoint_interval] = 1000
# [max_checkpoints] = 10
# [checkpoint_compression] = "balanced"

# Performance - modify as needed
# [enable_performance_optimizations] = true
# [memory_pool_size] = 10000
# [event_queue_capacity] = 1000
# [enable_simd] = true
# [parallel_processing] = false

# Logging - modify as needed
# [log_level] = "info"
# [log_file] = "nlm.log"
# [enable_console_logging] = true
# [enable_file_logging] = true
# [log_rotation_mb] = 100

# Batch processing - modify as needed
# [batch_processing] = false
# [batch_size] = 10
# [parallel_batch] = true

# Advanced features - modify as needed
# [environment_variables] = false
# [dynamic_reloading] = false
# [validation] = false
# [history] = false

# Notes:
# - Uncomment and modify any setting as needed
# - Use comments to document your modifications
# - Save your custom template with a meaningful name
# - You can also create your own template format
```

## Template Usage Guide

### Loading Templates

Templates can be loaded in several ways:

#### 1. Command Line

```bash
# Load a specific template
./nlm --template fast

# Load a custom template
./nlm --template my_custom_template.cfg
```

#### 2. Configuration File

```bash
# Use a template file
./nlm --config templates/fast_template.cfg
```

#### 3. Programmatic

```cpp
Config config;

// Load a built-in template
if (config.loadFromTemplate("fast")) {
    std::cout << "Loaded fast template" << std::endl;
}

// Load a custom template
if (config.loadFromTemplate("my_research_template.cfg")) {
    std::cout << "Loaded custom template" << std::endl;
}
```

### Creating Custom Templates

#### 1. Copy and Modify Built-in Template

```bash
# Copy a built-in template
cp templates/fast.cfg my_templates/fast_research.cfg

# Edit the template using your favorite text editor
nano my_templates/fast_research.cfg
```

#### 2. Create from Scratch

```bash
# Create a new template
nano templates/my_custom_template.cfg
```

#### 3. Programmatic Template Creation

```cpp
// Create a custom template programmatically
ConfigTemplate customTemplate;
customTemplate.name = "My Custom Research Template";
customTemplate.description = "A custom template for memory research";
customTemplate.config = "random_seed = 999; neuron_count = 2000; ...";

// Save the custom template
if (customTemplate.save("/path/to/templates/custom.cfg")) {
    std::cout << "Saved custom template" << std::endl;
}
```

### Template Management

#### Template Commands

```bash
# List all available templates
./nlm --list-templates

# Show template details
./nlm --template-info fast

# Validate a template
./nlm --validate-template fast

# Run with a template
./nlm --template fast

# Generate documentation for a template
./nlm --template-docs fast
```

#### Template Commands (Programmatic)

```cpp
// List templates
std::vector<std::string> templateNames = ConfigTemplateManager::getAvailableTemplates();

// Show template details
TemplateInfo info = ConfigTemplateManager::getTemplateInfo("fast");

// Validate template
bool isValid = ConfigTemplateManager::validateTemplate("fast");

// Get template content
std::string content = ConfigTemplateManager::getTemplateContent("fast");

// Get template documentation
std::string docs = ConfigTemplateManager::getTemplateDocumentation("fast");
```

### Template Categories

#### By Use Case

1. **Research Templates** - Templates for specific research scenarios
2. **Performance Templates** - Templates optimized for performance
3. **Development Templates** - Templates for development and testing
4. **Education Templates** - Templates for learning and teaching

#### By Complexity

1. **Simple Templates** - Basic configurations for quick setup
2. **Standard Templates** - Comprehensive configurations for common use cases
3. **Advanced Templates** - Complex configurations for advanced research
4. **Custom Templates** - User-defined configurations

#### By Domain

1. **Memory Research** - Templates focused on memory systems
2. **Plasticity Research** - Templates focused on plasticity rules
3. **Integration Research** - Templates focused on system integration
4. **Development Research** - Templates focused on developmental processes

## Template Examples

### Example 1: Memory Research Template Usage

```bash
# Run memory research simulation
./nlm --template memory_research --output memory_research_results.txt

# Validate memory research template
./nlm --validate-template memory_research

# View memory research template documentation
./nlm --template-docs memory_research
```

```cpp
// Programmatic memory research template usage
Config config;

// Load memory research template
if (config.loadFromTemplate("memory_research")) {
    // Modify template configuration
    config.set("neuron_count", 3000);
    config.set("working_memory_capacity", 500);
    
    // Run simulation
    runSimulation(config);
    
    // Save results
    saveResults("memory_research_results.txt");
}
```

### Example 2: Custom Template Creation

```bash
# Create a custom template from fast template
cp templates/fast.cfg my_templates/high_performance.cfg

# Edit the custom template
nano my_templates/high_performance.cfg

# Update the custom template
./nlm --update-template my_templates/high_performance.cfg

# Use the custom template
./nlm --template my_templates/high_performance.cfg
```

```cpp
// Programmatic custom template creation
// Create a custom template
ConfigTemplate customTemplate;
customTemplate.name = "High Performance Research";
customTemplate.description = "Custom template for high-performance research";
customTemplate.content = "";

// Load base template
std::string baseContent = ConfigTemplateManager::getTemplateContent("fast");

// Customize the template
customTemplate.content = baseContent;
// Replace or modify specific values
customTemplate.content = "random_seed = 123; neuron_count = 1000; simulation_timestep = 0.001; ...";

// Save custom template
ConfigTemplateManager::saveCustomTemplate(customTemplate, "high_performance.cfg");
```

### Example 3: Template Comparison

```bash
# Compare templates
./nlm --compare-templates memory_research fast plasticity_research

# Find similar templates
./nlm --find-similar-templates memory_research
```

```cpp
// Programmatic template comparison
std::vector<std::string> templateNames = {"memory_research", "fast", "plasticity_research"};

// Compare templates
TemplateComparison comparison = ConfigTemplateManager::compareTemplates(templateNames);

// Find similar templates
similarity = ConfigTemplateManager::findSimilarTemplates("memory_research", 3);
```

## Template Management System

### Template Manager

The NLM configuration system includes a comprehensive template management system that provides:

1. **Template Storage** - Store templates in files or database
2. **Template Loading** - Load templates from various sources
3. **Template Validation** - Validate template syntax and semantics
4. **Template Documentation** - Generate documentation for templates
5. **Template Comparison** - Compare templates and find similar templates
6. **Template Customization** - Customize templates for specific use cases
7. **Template Export** - Export templates to various formats
8. **Template Import** - Import templates from various formats

### Template Structure

#### Template Metadata

```yaml
template:
  name: "Template Name"
  description: "Template description"
  version: "1.0.0"
  author: "Author Name"
  date: "2026-11-01"
  category: "Memory Research"
  tags: ["memory", "research", "high-performance"]
  complexity: "Advanced"
```

#### Template Content

```yaml
content:
  # Configuration key-value pairs
  random_seed: 42
  simulation_timestep: 0.001
  neuron_count: 1000
  
  # Comments
  # This is a comment in the template
  
  # Section headers
  [plasticity]
  stdp_ltp_weight: 0.02
  stdp_ltd_weight: 0.015
  stdp_tau: 20.0
```

#### Template Validation

```yaml
validation:
  required_keys: ["random_seed", "neuron_count", "simulation_timestep"]
  allowed_keys: ["random_seed", "neuron_count", "simulation_timestep", "region_count", "connection_probability", ...]
  key_ranges:
    random_seed: [1, 2147483647]
    neuron_count: [1, 10000]
    simulation_timestep: [0.0001, 0.1]
  validation_rules:
    - "neuron_count must be positive"
    - "simulation_timestep must be between 0.0001 and 0.1 seconds"
```

## Template Best Practices

### For Template Authors

1. **Document Your Template** - Include clear comments and documentation
2. **Use Consistent Formatting** - Follow consistent formatting conventions
3. **Include Validation Rules** - Add validation rules to ensure template correctness
4. **Provide Examples** - Include examples of template usage
5. **Test Your Template** - Test your template thoroughly before sharing
6. **Keep Templates Current** - Update templates to reflect current best practices
7. **Provide Support** - Include support information for template users

### For Template Users

1. **Understand the Template** - Read the template documentation carefully
2. **Customize Appropriately** - Customize templates to meet your needs
3. **Test Modifications** - Test template modifications thoroughly
4. **Document Changes** - Document any modifications made to templates
5. **Share Improvements** - Share improvements with the community
6. **Report Issues** - Report any issues with templates
7. **Contribute** - Contribute templates and improvements to the community

### For Template Administrators

1. **Maintain Templates** - Regularly maintain and update templates
2. **Validate Templates** - Regularly validate templates for correctness
3. **Categorize Templates** - Categorize templates for easy discovery
4. **Document Templates** - Document templates comprehensively
5. **Test Templates** - Test templates thoroughly
6. **Monitor Usage** - Monitor template usage and provide feedback
7. **Update Templates** - Update templates to reflect changes in best practices

## Template Security

### Security Considerations

1. **Template Validation** - Validate templates before loading
2. **Template Sanitization** - Sanitize template content to prevent security issues
3. **Template Access Control** - Control access to sensitive templates
4. **Template Logging** - Log template usage for security auditing
5. **Template Backup** - Backup templates to prevent data loss

### Security Best Practices

1. **Validate All Templates** - Validate all templates before loading
2. **Use Sandboxed Environments** - Load templates in sandboxed environments
3. **Limit Template Access** - Limit access to sensitive templates
4. **Monitor Template Usage** - Monitor template usage for unusual patterns
5. **Regular Security Audits** - Regularly audit template security

## Template Performance

### Performance Considerations

1. **Template Loading Time** - Optimize template loading time
2. **Template Validation Time** - Optimize template validation time
3. **Template Memory Usage** - Optimize template memory usage
4. **Template Processing Time** - Optimize template processing time

### Performance Optimization

1. **Cache Templates** - Cache templates to reduce loading time
2. **Validate Templates** - Validate templates only once
3. **Use Efficient Formats** - Use efficient formats for template storage
4. **Optimize Template Content** - Optimize template content for performance

## Template Testing

### Template Testing Framework

The NLM configuration system includes a comprehensive template testing framework that provides:

1. **Template Syntax Testing** - Test template syntax
2. **Template Semantic Testing** - Test template semantics
3. **Template Validation Testing** - Test template validation
4. **Template Import/Export Testing** - Test template import and export
5. **Template Integration Testing** - Test template integration
6. **Template Performance Testing** - Test template performance

### Running Template Tests

```bash
# Run all template tests
./nlm --test=templates

# Run specific template tests
./nlm --test=templates:syntax
./nlm --test=templates:semantic
./nlm --test=templates:validation
./nlm --test=templates:import_export
./nlm --test=templates:integration
./nlm --test=templates:performance
```

```cpp
// Programmatic template testing
bool testTemplateSyntax(const std::string& templateName);
bool testTemplateSemantics(const std::string& templateName);
bool testTemplateValidation(const std::string& templateName);
bool testTemplateImportExport(const std::string& templateName);
bool testTemplateIntegration(const std::string& templateName);
bool testTemplatePerformance(const std::string& templateName);
```

## Template Documentation

### Template Documentation Generator

The NLM configuration system includes a template documentation generator that provides:

1. **Automatic Documentation Generation** - Automatically generate documentation for templates
2. **Template Documentation Templates** - Documentation templates for different types of templates
3. **Template Usage Examples** - Usage examples for templates
4. **Template Parameter Documentation** - Documentation for template parameters
5. **Template Configuration Examples** - Configuration examples for templates

### Generating Template Documentation

```bash
# Generate documentation for a template
./nlm --template-docs fast

# Generate documentation for all templates
./nlm --template-docs-all

# Generate documentation for custom templates
./nlm --template-docs my_custom_template.cfg
```

```cpp
// Programmatic template documentation generation
std::string generateTemplateDocumentation(const std::string& templateName);
std::string generateTemplateDocumentation(const ConfigTemplate& template);
```

## Template Migration Guide

### From Previous Versions

If upgrading from previous versions of NLM, the following template changes apply:

#### Template Format Changes

| Version | Template Format | Notes |
|---------|----------------|-------|
| 5.0.0 | Simple key-value format | Basic template support |
| 6.0.0 | Enhanced key-value format | Advanced template features |

#### Template Features Changes

| Version | Template Features | Notes |
|---------|------------------|-------|
| 5.0.0 | Basic features | Limited template functionality |
| 6.0.0 | Advanced features | Comprehensive template functionality |

### Migration Examples

#### INI Format Migration

```ini
# Version 5.0.0 template
[basic]
random_seed = 42
neuron_count = 1000
simulation_timestep = 0.001

[plasticity]
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Version 6.0.0 template
[basic]
random_seed = 42
neuron_count = 1000
simulation_timestep = 0.001
region_count = 1
connection_probability = 0.1

[plasticity]
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

#### JSON Format Migration

```json
// Version 5.0.0 template
{
    "basic": {
        "random_seed": 42,
        "neuron_count": 1000,
        "simulation_timestep": 0.001
    },
    "plasticity": {
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012
    }
}

// Version 6.0.0 template
{
    "name": "Phase 6 Template",
    "version": "6.0.0",
    "description": "Complete Phase 6 configuration template",
    "content": {
        "basic": {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 1000,
            "region_count": 1,
            "connection_probability": 0.1
        },
        "plasticity": {
            "stdp": {
                "ltp_weight": 0.02,
                "ltd_weight": 0.015,
                "tau": 20.0
            },
            "structural": {
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001
            }
        }
    }
}
```

## Conclusion

The NLM configuration template system provides a comprehensive framework for managing and sharing NLM configurations. Templates enable:

1. **Quick Setup** - Quick setup of common configurations
2. **Research Reproducibility** - Reproducible research configurations
3. **Development Support** - Support for development and testing
4. **Education Support** - Support for education and learning
5. **Community Sharing** - Sharing of configurations and best practices

The template system is designed to be:

1. **Flexible** - Support for various template formats and customization
2. **Extensible** - Easy to add new templates and features
3. **Documented** - Comprehensive documentation and examples
4. **Tested** - Comprehensive test coverage
5. **Secure** - Security considerations and best practices
6. **Performant** - Optimized for performance
7. **User-friendly** - Easy to use and understand

The template system is a core component of the NLM configuration system and is essential for configuring, managing, and sharing NLM configurations.

---

*This documentation is for NLM Configuration Template System. For the latest information, visit the NLM documentation portal or contact the NLM development team.*

*Template system version: 6.0.0 (Phase 6)*
*Documentation version: 1.0.0*
*Last updated: 2026-11-01*
*Copyright © 2026 NLM Research Project. All rights reserved.*

---END OF TEMPLATE DOCUMENTATION---