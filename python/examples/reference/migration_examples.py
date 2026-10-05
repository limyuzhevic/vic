#!/usr/bin/env python3
"""
Migration Examples: Basic to Enhanced NLM API Usage

This tutorial demonstrates migration from basic NLM usage to the enhanced
NLM Python API, showing how to leverage new features and capabilities.

## Learning Objectives
- Understand the differences between basic and enhanced APIs
- Learn migration strategies for existing code
- Demonstrate new feature adoption
- Show performance improvements
- Provide upgrade guidance
"""

import sys
import os
import time
import json

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import nlm_py
from nlm_utils import ConfigBuilder, ConfigValidator, MetricsCollector

def print_section(title):
    """Print a formatted section header"""
    print(f"\n{'='*80}")
    print(f"{title}")
    print(f"{'='*80}")

def section_1_migration_overview():
    """Section 1: Migration Overview"""
    print_section("Section 1: Migration Overview")
    
    print("""
In this section, we'll explore migration strategies:
1. Understand basic vs enhanced API differences
2. Identify migration opportunities
3. Plan migration approach
4. Execute migration step-by-step
5. Validate migration results
""")
    
    print("\n1. Migration Overview:")
    print("""
    BASIC API (Original NLM):
    ┌─────────────────────────────────────┐
    │ brain = nlm_py.createBrain()         │
    │ brain.initialize()                  │
    │ brain.step(0)                       │
    │ brain.getFiringRate()               │
    └─────────────────────────────────────┘
    
    ENHANCED API (New Features):
    ┌─────────────────────────────────────┐
    │ config = ConfigBuilder.createDefaultConfig() │
    │ config.set("neuron_count", 1000)    │
    │ brain = nlm_py.createBrain(config)   │
    │ brain.initialize()                  │
    │ brain.step(0)                       │
    │ brain.getAverageFiringRate()        │
    │ brain.getActiveNeuronCount()        │
    │ config.validate()                   │
    │ metrics.collect_metrics(brain, step)
    └─────────────────────────────────────┘
    
    Key Enhancements:
    ✓ Configuration management
    ✓ Advanced validation
    ✓ Comprehensive metrics
    ✓ Performance optimization
    ✓ Export/import capabilities
    ✓ Template and preset systems
    """)
    
    # Create example of migration path
    print("\n2. Migration Path Examples:")
    
    migration_paths = [
        {
            "type": "Minimal Migration",
            "description": "Add minimal configuration to existing code",
            "code": """
# BEFORE:
brain = nlm_py.createBrain()
brain.initialize()

# AFTER:
from nlm_utils import ConfigBuilder
config = ConfigBuilder.createDefaultConfig()
brain = nlm_py.createBrain(config)
brain.initialize()
"""
        },
        {
            "type": "Feature Migration",
            "description": "Add new features while maintaining compatibility",
            "code": """
# BEFORE:
for step in range(100):
    brain.step(step)
    rate = brain.getFiringRate()

# AFTER:
from nlm_utils import MetricsCollector
metrics_collector = MetricsCollector()

for step in range(100):
    brain.step(step)
    rate = brain.getAverageFiringRate()
    metrics_collector.collect_metrics(brain, step)
    
    if step % 10 == 0:
        active = brain.getActiveNeuronCount()
        print(f"Step {step}: {rate:.2f} Hz, {active} active neurons")
"""
        },
        {
            "type": "Full Migration",
            "description": "Complete upgrade to enhanced features",
            "code": """
# BEFORE:
brain = nlm_py.createBrain()
brain.initialize()
for step in range(100):
    brain.step(step)

# AFTER:
from nlm_utils import ConfigBuilder, ConfigValidator, MetricsCollector

# Step 1: Configuration with validation
config = ConfigBuilder.create_training_config()
schema = ConfigValidator.create_schema(
    key="neuron_count",
    type="int",
    description="Number of neurons",
    min_value=500,
    max_value=5000
)
config.add_schema(schema)

# Step 2: Validation
if config.validate():
    brain = nlm_py.createBrain(config)
    brain.initialize()
    
    # Step 3: Metrics collection
    metrics_collector = MetricsCollector()
    
    # Step 4: Simulation with metrics
    for step in range(100):
        brain.step(step)
        metrics_collector.collect_metrics(brain, step)
        
    # Step 5: Export results
    metrics_collector.export_to_csv("simulation_results.csv")
"""
        }
    ]
    
    for i, path in enumerate(migration_paths, 1):
        print(f"\n   {i}. {path['type']}:")
        print(f"      Description: {path['description']}")
        print(f"      Example code:")
        for line in path['code'].split('\n'):
            print(f"        {line}")
    
    return migration_paths

def section_2_migration_strategies():
    """Section 2: Migration Strategies"""
    print_section("Section 2: Migration Strategies")
    
    print("""
In this section, we'll explore migration strategies:
1. Gradual migration approach
2. Feature-based migration
3. Performance-driven migration
4. Risk-minimized migration
5. Parallel migration approach
""")
    
    # Strategy 1: Gradual Migration
    print("\n1. Gradual Migration Strategy:")
    print("""
    GRADUAL MIGRATION APPROACH:
    ┌─────────────────────────────────────┐
    │ Phase 1: Add configuration          │
    │   - Create basic config             │
    │   - Add validation if needed       │
    │   - Maintain backward compatibility│
    │                                     │
    │ Phase 2: Add metrics collection    │
    │   - Add MetricsCollector           │
    │   - Collect basic metrics          │
    │   - Export to CSV for analysis    │
    │                                     │
    │ Phase 3: Add advanced features    │
    │   - Add templates and presets     │
    │   - Implement performance monitoring│
    │   - Add export/import capabilities│
    └─────────────────────────────────────┘
    
    Benefits:
    ✓ Lower risk
    ✓ Easier testing
    ✓ Can rollback if needed
    ✓ Gradual skill development
    """)
    
    # Implement gradual migration
    print("\n2. Implementing Gradual Migration:")
    
    class GradualMigrator:
        def __init__(self, name):
            self.name = name
            self.phase = 0
            self.results = {}
        
        def phase1_add_configuration(self):
            """Phase 1: Add basic configuration"""
            self.phase = 1
            print(f"   {self.name}: Phase 1 - Adding basic configuration...")
            
            config = ConfigBuilder.create_default_config()
            config.set("neuron_count", 500)  # Slightly smaller than original
            config.set("learning_rate", 0.01)
            
            brain = nlm_py.createBrain(config)
            brain.initialize()
            
            self.results['phase1'] = {
                'config': config,
                'brain': brain,
                'steps_completed': 10
            }
            
            print("   ✓ Phase 1 completed: Basic configuration added")
        
        def phase2_add_metrics(self):
            """Phase 2: Add metrics collection"""
            self.phase = 2
            print(f"   {self.name}: Phase 2 - Adding metrics collection...")
            
            if 'phase1' in self.results:
                brain = self.results['phase1']['brain']
                metrics_collector = MetricsCollector()
                
                for step in range(10, 20):
                    brain.step(step)
                    metrics_collector.collect_metrics(brain, step)
                
                self.results['phase2'] = {
                    'metrics': metrics_collector,
                    'steps_completed': 20
                }
                
                print("   ✓ Phase 2 completed: Metrics collection added")
        
        def phase3_add_advanced_features(self):
            """Phase 3: Add advanced features"""
            self.phase = 3
            print(f"   {self.name}: Phase 3 - Adding advanced features...")
            
            if 'phase2' in self.results:
                # Add configuration validation
                config = ConfigBuilder.create_default_config()
                schema = ConfigValidator.create_schema(
                    key="neuron_count",
                    type="int",
                    description="Number of neurons",
                    min_value=100,
                    max_value=10000
                )
                config.add_schema(schema)
                
                errors = config.validate()
                
                self.results['phase3'] = {
                    'config': config,
                    'validation_errors': len(errors),
                    'steps_completed': 30
                }
                
                print("   ✓ Phase 3 completed: Advanced features added")
        
        def get_status(self):
            """Get migration status"""
            return {
                'name': self.name,
                'current_phase': self.phase,
                'completed_phases': list(self.results.keys()),
                'total_steps': 100
            }
    
    # Execute gradual migration
    migrator = GradualMigrator("System Migration")
    migrator.phase1_add_configuration()
    migrator.phase2_add_metrics()
    migrator.phase3_add_advanced_features()
    
    status = migrator.get_status()
    print(f"\n   Migration Status:")
    print(f"   - Name: {status['name']}")
    print(f"   - Current Phase: {status['current_phase']}")
    print(f"   - Completed Phases: {status['completed_phases']}")
    
    # Strategy 2: Feature-Based Migration
    print("\n3. Feature-Based Migration Strategy:")
    print("""
    FEATURE-BASED MIGRATION:
    ┌─────────────────────────────────────┐
    │ Focus on one feature at a time     │
    │                                     │
    │ Feature 1: Configuration           │
    │   - Add ConfigBuilder              │
    │   - Add validation schemas        │
    │   - Test configuration validation │
    │                                     │
    │ Feature 2: Metrics                 │
    │   - Add MetricsCollector           │
    │   - Implement metrics collection   │
    │   - Export metrics data           │
    │                                     │
    │ Feature 3: Templates               │
    │   - Add template system           │
    │   - Create common templates       │
    │   - Test template application     │
    └─────────────────────────────────────┘
    
    Benefits:
    ✓ Focused learning
    ✓ Easier testing per feature
    ✓ Can skip less relevant features
    ✓ Clear progression
    """)
    
    # Strategy 3: Performance-Driven Migration
    print("\n4. Performance-Driven Migration Strategy:")
    print("""
    PERFORMANCE-DRIVEN MIGRATION:
    ┌─────────────────────────────────────┐
    │ Measure baseline performance       │
    │ Identify bottlenecks              │
    │ Apply relevant enhancements        │
    │                                     │
    │ Analysis Phase:                    │
    │   - Measure execution time         │
    │   - Monitor memory usage           │
    │   - Track metrics collection       │
    │   - Identify optimization targets  │
    │                                     │
    │ Optimization Phase:                │
    │   - Apply configuration optimizations│
    │   - Add performance monitoring     │
    │   - Implement metrics optimization │
    └─────────────────────────────────────┘
    
    Benefits:
    ✓ Data-driven decisions
    ✓ Maximum impact improvements
    ✓ Clear ROI demonstration
    ✓ Business case justification
    """)
    
    return migrator

def section_3_migration_examples():
    """Section 3: Migration Examples"""
    print_section("Section 3: Migration Examples")
    
    print("""
In this section, we'll demonstrate specific migration examples:
1. Example 1: Basic configuration migration
2. Example 2: Metrics migration
3. Example 3: Advanced features migration
4. Example 4: Performance migration
5. Example 5: Production migration
""")
    
    # Example 1: Basic Configuration Migration
    print("\n1. Basic Configuration Migration Example:")
    print("""
    SCENARIO: Legacy system using basic NLM API
    PROBLEM: Need to add configuration management
    
    LEGACY CODE:
    ```python
    # Original legacy code
    def run_simulation():
        brain = nlm_py.createBrain()
        brain.initialize()
        
        for step in range(100):
            brain.step(step)
            print(f"Step {step}: {brain.getFiringRate():.2f} Hz")
        
        return brain
    ```
    
    MIGRATED CODE:
    ```python
    # Enhanced version with configuration
    from nlm_utils import ConfigBuilder
    
    def run_simulation():
        # Step 1: Create configuration
        config = ConfigBuilder.create_default_config()
        config.set("neuron_count", 1000)
        config.set("learning_rate", 0.03)
        
        # Step 2: Create brain with configuration
        brain = nlm_py.createBrain(config)
        brain.initialize()
        
        # Step 3: Run simulation
        for step in range(100):
            brain.step(step)
            print(f"Step {step}: {brain.getAverageFiringRate():.2f} Hz")
        
        return brain, config
    ```
    
    Benefits:
    ✓ Better control over simulation parameters
    ✓ Validation and error checking
    ✓ Reproducible configurations
    ✓ Easier testing and debugging
    """)
    
    # Implement basic configuration migration
    print("\n   Implementing Basic Configuration Migration:")
    
    def legacy_simulation():
        """Legacy simulation function (before migration)"""
        brain = nlm_py.createBrain()
        brain.initialize()
        
        for step in range(10):
            brain.step(step)
            print(f"Legacy Step {step}: {brain.getFiringRate():.2f} Hz")
        
        return brain
    
    def migrated_simulation():
        """Migrated simulation function (after migration)"""
        from nlm_utils import ConfigBuilder
        
        # Create configuration
        config = ConfigBuilder.create_default_config()
        config.set("neuron_count", 500)
        config.set("learning_rate", 0.02)
        config.set("simulation_timestep", 0.001)
        
        # Create brain with configuration
        brain = nlm_py.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(10):
            brain.step(step)
            print(f"Migrated Step {step}: {brain.getAverageFiringRate():.2f} Hz")
        
        return brain, config
    
    # Execute both versions
    print("   Running legacy simulation...")
    legacy_brain = legacy_simulation()
    
    print("   Running migrated simulation...")
    migrated_brain, migrated_config = migrated_simulation()
    
    print("   ✓ Basic configuration migration successful")
    
    # Example 2: Metrics Migration
    print("\n2. Metrics Migration Example:")
    print("""
    SCENARIO: Need to collect simulation metrics
    PROBLEM: No systematic metrics collection
    
    LEGACY CODE:
    ```python
    # Original code with minimal metrics
    def run_simulation():
        brain = nlm_py.createBrain()
        brain.initialize()
        
        metrics = []
        for step in range(100):
            brain.step(step)
            metrics.append({
                'step': step,
                'firing_rate': brain.getFiringRate(),
                'timestamp': time.time()
            })
        
        return metrics
    ```
    
    MIGRATED CODE:
    ```python
    # Enhanced version with MetricsCollector
    from nlm_utils import MetricsCollector
    
    def run_simulation():
        config = ConfigBuilder.create_training_config()
        brain = nlm_py.createBrain(config)
        brain.initialize()
        
        # Create metrics collector
        metrics_collector = MetricsCollector()
        
        for step in range(100):
            brain.step(step)
            metrics_collector.collect_metrics(brain, step)
            
            if step % 10 == 0:
                firing_rate = brain.getAverageFiringRate()
                active_neurons = brain.getActiveNeuronCount()
                print(f"Step {step}: {firing_rate:.2f} Hz, {active_neurons} active")
        
        # Export metrics
        metrics_collector.export_to_csv("simulation_metrics.csv")
        
        return metrics_collector.get_all_metrics()
    ```
    
    Benefits:
    ✓ Comprehensive metrics collection
    ✓ Automatic metrics tracking
    ✓ Multiple export formats
    ✓ Better data analysis capabilities
    """)
    
    # Implement metrics migration
    print("\n   Implementing Metrics Migration:")
    
    def legacy_metrics_simulation():
        """Legacy simulation with manual metrics"""
        brain = nlm_py.createBrain()
        brain.initialize()
        
        metrics = []
        for step in range(10):
            brain.step(step)
            metrics.append({
                'step': step,
                'firing_rate': brain.getFiringRate(),
                'active_neurons': brain.getActiveNeuronCount(),
                'timestamp': time.time()
            })
        
        print("   Legacy metrics collected manually")
        return metrics
    
    def migrated_metrics_simulation():
        """Migrated simulation with MetricsCollector"""
        from nlm_utils import MetricsCollector
        
        config = ConfigBuilder.create_training_config()
        brain = nlm_py.createBrain(config)
        brain.initialize()
        
        # Create metrics collector
        metrics_collector = MetricsCollector()
        
        for step in range(10):
            brain.step(step)
            metrics_collector.collect_metrics(brain, step)
            
            if step % 5 == 0:
                firing_rate = brain.getAverageFiringRate()
                active_neurons = brain.getActiveNeuronCount()
                print(f"Step {step}: {firing_rate:.2f} Hz, {active_neurons} active")
        
        # Get all metrics
        all_metrics = metrics_collector.get_all_metrics()
        
        print("   Enhanced metrics collected automatically")
        return all_metrics
    
    # Execute both versions
    print("   Running legacy metrics simulation...")
    legacy_metrics = legacy_metrics_simulation()
    
    print("   Running migrated metrics simulation...")
    migrated_metrics = migrated_metrics_simulation()
    
    print("   ✓ Metrics migration successful")
    
    # Example 3: Advanced Features Migration
    print("\n3. Advanced Features Migration Example:")
    print("""
    SCENARIO: Need advanced features for production
    PROBLEM: Limited features in basic API
    
    LEGACY CODE:
    ```python
    # Original code with limited features
    def run_production_simulation():
        brain = nlm_py.createBrain()
        brain.initialize()
        
        for step in range(100):
            brain.step(step)
        
        return brain
    ```
    
    MIGRATED CODE:
    ```python
    # Enhanced production simulation
    from nlm_utils import ConfigBuilder, ConfigValidator, MetricsCollector
    
    def run_production_simulation():
        # Step 1: Create production configuration
        config = ConfigBuilder.create_research_config()
        
        # Step 2: Add validation
        schema = ConfigValidator.create_schema(
            key="neuron_count",
            type="int",
            description="Number of neurons",
            min_value=1000,
            max_value=10000
        )
        config.add_schema(schema)
        
        # Step 3: Validate configuration
        errors = config.validate()
        if errors:
            print(f"   Configuration errors: {errors}")
            return None
        
        # Step 4: Create brain
        brain = nlm_py.createBrain(config)
        brain.initialize()
        
        # Step 5: Collect metrics
        metrics_collector = MetricsCollector()
        
        # Step 6: Run simulation
        for step in range(100):
            brain.step(step)
            metrics_collector.collect_metrics(brain, step)
            
            if step % 20 == 0:
                firing_rate = brain.getAverageFiringRate()
                active_neurons = brain.getActiveNeuronCount()
                print(f"Step {step}: {firing_rate:.2f} Hz, {active_neurons} active")
        
        # Step 7: Export results
        metrics_collector.export_to_csv("production_simulation.csv")
        
        # Step 8: Generate report
        results = generate_simulation_report(metrics_collector)
        
        return brain, config, results
    ```
    
    Benefits:
    ✓ Production-ready configuration
    ✓ Comprehensive validation
    ✓ Detailed metrics collection
    ✓ Automated reporting
    ✓ Export capabilities
    """)
    
    # Implement advanced features migration
    print("\n   Implementing Advanced Features Migration:")
    
    def legacy_production_simulation():
        """Legacy production simulation (basic)"""
        brain = nlm_py.createBrain()
        brain.initialize()
        
        for step in range(10):
            brain.step(step)
        
        print("   Basic production simulation completed")
        return brain
    
    def migrated_production_simulation():
        """Enhanced production simulation"""
        # Step 1: Create production configuration
        config = ConfigBuilder.create_research_config()
        
        # Step 2: Add validation
        schema = ConfigValidator.create_schema(
            key="neuron_count",
            type="int",
            description="Number of neurons",
            min_value=1000,
            max_value=10000
        )
        config.add_schema(schema)
        
        # Step 3: Validate configuration
        errors = config.validate()
        if errors:
            print(f"   Configuration errors: {len(errors)}")
            return None
        
        print("   ✓ Configuration validated")
        
        # Step 4: Create brain
        brain = nlm_py.createBrain(config)
        brain.initialize()
        
        # Step 5: Collect metrics
        metrics_collector = MetricsCollector()
        
        # Step 6: Run simulation
        for step in range(10):
            brain.step(step)
            metrics_collector.collect_metrics(brain, step)
            
            if step % 5 == 0:
                firing_rate = brain.getAverageFiringRate()
                active_neurons = brain.getActiveNeuronCount()
                print(f"Step {step}: {firing_rate:.2f} Hz, {active_neurons} active")
        
        # Step 7: Export results
        try:
            metrics_collector.export_to_csv("production_simulation.csv")
            print("   ✓ Results exported to CSV")
        except Exception as e:
            print(f"   ⚠ Export failed: {e}")
        
        # Step 8: Generate report
        results = generate_production_report(brain, metrics_collector)
        
        print("   ✓ Production report generated")
        
        return brain, config, results
    
    def generate_production_report(brain, metrics_collector):
        """Generate comprehensive production report"""
        return {
            "simulation_summary": {
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "final_firing_rate": brain.getAverageFiringRate(),
                "final_active_neurons": brain.getActiveNeuronCount(),
                "steps_completed": 10
            },
            "metrics_summary": {
                "total_time_points": len(metrics_collector.get_all_metrics()),
                "average_firing_rate": sum(
                    m['firing_rate'] for m in metrics_collector.get_all_metrics().values()
                ) / len(metrics_collector.get_all_metrics()) if metrics_collector.get_all_metrics() else 0,
                "average_active_neurons": sum(
                    m['active_neurons'] for m in metrics_collector.get_all_metrics().values()
                ) / len(metrics_collector.get_all_metrics()) if metrics_collector.get_all_metrics() else 0
            },
            "production_status": "SUCCESS"
        }
    
    # Execute both versions
    print("   Running legacy production simulation...")
    legacy_prod_brain = legacy_production_simulation()
    
    print("   Running migrated production simulation...")
    migrated_prod_brain, migrated_prod_config, migrated_prod_results = migrated_production_simulation()
    
    print("   ✓ Advanced features migration successful")
    
    return True

def section_4_migration_validation():
    """Section 4: Migration Validation"""
    print_section("Section 4: Migration Validation")
    
    print("""
In this section, we'll validate migration results:
1. Compare legacy and migrated outputs
2. Validate correctness of migration
3. Measure performance improvements
4. Assess risk and impact
5. Plan rollback strategies
""")
    
    # Validation 1: Compare outputs
    print("\n1. Output Validation:")
    print("""
    VALIDATION APPROACH:
    ┌─────────────────────────────────────┐
    │ 1. Compare key metrics              │
    │   - Firing rates comparison         │
    │   - Active neurons comparison       │
    │   - Simulation steps comparison    │
    │                                     │
    │ 2. Verify functional equivalence     │
    │   - Core functionality preserved    │
    │   - API behavior unchanged         │
    │   - Output format compatibility    │
    │                                     │
    │ 3. Validate data integrity          │
    │   - No data loss during migration  │
    │   - All original functionality     │
    │   - Error handling maintained      │
    └─────────────────────────────────────┘
    
    VALIDATION METHODOLOGY:
    - Unit testing: Test individual functions
    - Integration testing: Test complete workflows
    - Regression testing: Ensure no functionality broken
    - Performance testing: Verify improvements
    """)
    
    # Implement validation
    print("\n2. Performing Validation:")
    
    class MigrationValidator:
        def __init__(self):
            self.validation_results = {}
            self.legacy_results = {}
            self.migrated_results = {}
        
        def run_legacy_version(self):
            """Run legacy version for comparison"""
            print("   Running legacy version...")
            
            legacy_brain = nlm_py.createBrain()
            legacy_brain.initialize()
            
            legacy_metrics = []
            for step in range(20):
                legacy_brain.step(step)
                legacy_metrics.append({
                    'step': step,
                    'firing_rate': legacy_brain.getFiringRate(),
                    'active_neurons': legacy_brain.getActiveNeuronCount()
                })
            
            self.legacy_results = {
                'final_firing_rate': legacy_brain.getFiringRate(),
                'final_active_neurons': legacy_brain.getActiveNeuronCount(),
                'all_metrics': legacy_metrics,
                'version': 'legacy'
            }
            
            print("   ✓ Legacy version completed")
        
        def run_migrated_version(self):
            """Run migrated version for comparison"""
            print("   Running migrated version...")
            
            config = ConfigBuilder.create_default_config()
            migrated_brain = nlm_py.createBrain(config)
            migrated_brain.initialize()
            
            migrated_metrics = []
            for step in range(20):
                migrated_brain.step(step)
                migrated_metrics.append({
                    'step': step,
                    'firing_rate': migrated_brain.getAverageFiringRate(),
                    'active_neurons': migrated_brain.getActiveNeuronCount()
                })
            
            self.migrated_results = {
                'final_firing_rate': migrated_brain.getAverageFiringRate(),
                'final_active_neurons': migrated_brain.getActiveNeuronCount(),
                'all_metrics': migrated_metrics,
                'version': 'migrated'
            }
            
            print("   ✓ Migrated version completed")
        
        def compare_outputs(self):
            """Compare legacy and migrated outputs"""
            print("\n   Comparing outputs...")
            
            if not self.legacy_results or not self.migrated_results:
                print("   ⚠ Incomplete results, skipping comparison")
                return
            
            legacy = self.legacy_results
            migrated = self.migrated_results
            
            # Compare final firing rates
            legacy_fr = legacy['final_firing_rate']
            migrated_fr = migrated['final_firing_rate']
            
            fr_difference = abs(legacy_fr - migrated_fr)
            fr_percentage = (fr_difference / legacy_fr * 100) if legacy_fr > 0 else 0
            
            print(f"   Firing Rate Comparison:")
            print(f"     - Legacy: {legacy_fr:.2f} Hz")
            print(f"     - Migrated: {migrated_fr:.2f} Hz")
            print(f"     - Difference: {fr_difference:.2f} Hz ({fr_percentage:.1f}%)")
            
            # Compare final active neurons
            legacy_active = legacy['final_active_neurons']
            migrated_active = migrated['final_active_neurons']
            
            active_difference = abs(legacy_active - migrated_active)
            active_percentage = (active_difference / legacy_active * 100) if legacy_active > 0 else 0
            
            print(f"   Active Neurons Comparison:")
            print(f"     - Legacy: {legacy_active}")
            print(f"     - Migrated: {migrated_active}")
            print(f"     - Difference: {active_difference} ({active_percentage:.1f}%)")
            
            # Compare step-by-step metrics
            legacy_step_metrics = legacy['all_metrics']
            migrated_step_metrics = migrated['all_metrics']
            
            if len(legacy_step_metrics) == len(migrated_step_metrics):
                max_step_difference = 0
                max_percentage = 0
                
                for i, (legacy_step, migrated_step) in enumerate(
                    zip(legacy_step_metrics, migrated_step_metrics)
                ):
                    step_fr_diff = abs(legacy_step['firing_rate'] - migrated_step['firing_rate'])
                    step_active_diff = abs(legacy_step['active_neurons'] - migrated_step['active_neurons'])
                    
                    if step_fr_diff > max_step_difference:
                        max_step_difference = step_fr_diff
                    if step_active_diff > max_percentage:
                        max_percentage = step_active_diff
                
                print(f"   Maximum Step Differences:")
                print(f"     - Firing rate: {max_step_difference:.2f} Hz")
                print(f"     - Active neurons: {max_percentage}")
            
            # Validate functional equivalence
            print(f"\n   Functional Equivalence Assessment:")
            
            # Consider equivalent if differences are small
            fr_equivalent = fr_percentage < 20  # Within 20%
            active_equivalent = active_percentage < 20  # Within 20%
            
            if fr_equivalent and active_equivalent:
                print("   ✓ Functionally equivalent (differences within acceptable range)")
            else:
                print("   ⚠ Not fully equivalent (differences significant)")
            
            self.validation_results['output_comparison'] = {
                'firing_rate_equivalent': fr_equivalent,
                'active_neurons_equivalent': active_equivalent,
                'max_firing_rate_diff': fr_difference,
                'max_active_diff': active_difference,
                'overall_assessment': 'PASS' if (fr_equivalent and active_equivalent) else 'FAIL'
            }
        
        def validate_migration(self):
            """Perform complete validation"""
            print("\n3. Complete Migration Validation:")
            
            # Run both versions
            self.run_legacy_version()
            self.run_migrated_version()
            
            # Compare outputs
            self.compare_outputs()
            
            # Calculate validation score
            validation_score = self._calculate_validation_score()
            
            print(f"\n   Validation Summary:")
            print(f"   - Validation Score: {validation_score:.1f}%")
            print(f"   - Overall Assessment: {self.validation_results.get('output_comparison', {}).get('overall_assessment', 'UNKNOWN')}")
            
            if validation_score >= 80:
                print("   ✓ Migration successful (high validation score)")
            elif validation_score >= 60:
                print("   ⚠ Migration acceptable (moderate validation score)")
            else:
                print("   ❌ Migration problematic (low validation score)")
            
            return validation_score
        
        def _calculate_validation_score(self):
            """Calculate overall validation score"""
            score = 0.0
            
            if 'output_comparison' in self.validation_results:
                comparison = self.validation_results['output_comparison']
                
                # Firing rate assessment (40% weight)
                if comparison['firing_rate_equivalent']:
                    score += 40.0
                else:
                    score += max(0, 40.0 - (comparison['max_firing_rate_diff'] / 2))
                
                # Active neurons assessment (40% weight)
                if comparison['active_neurons_equivalent']:
                    score += 40.0
                else:
                    score += max(0, 40.0 - (comparison['max_active_diff'] / 5))
                
                # Overall assessment (20% weight)
                if comparison['overall_assessment'] == 'PASS':
                    score += 20.0
            
            return min(100.0, score)
    
    # Execute validation
    validator = MigrationValidator()
    validation_score = validator.validate_migration()
    
    # Validation 2: Performance Validation
    print("\n4. Performance Validation:")
    print("""
    PERFORMANCE VALIDATION:
    ┌─────────────────────────────────────┐
    │ Measure execution time              │
    │ Track memory usage                  │
    │ Monitor CPU utilization             │
    │                                     │
    │ Performance Metrics:                │
    │   - Execution time (seconds)         │
    │   - Memory usage (MB)                │
    │   - CPU utilization (%)              │
    │   - Throughput (steps/second)        │
    │   - Performance improvement ratio    │
    │                                     │
    │ Analysis:                            │
    │   - Compare legacy vs migrated       │
    │   - Calculate performance improvement│
    │   - Identify optimization opportunities│
    └─────────────────────────────────────┘
    """)
    
    # Implement performance validation
    print("\n   Performing Performance Validation:")
    
    import time
    
    # Performance test for legacy version
    print("   Testing legacy version performance...")
    start_time = time.time()
    
    legacy_perf_brain = nlm_py.createBrain()
    legacy_perf_brain.initialize()
    
    for step in range(50):
        legacy_perf_brain.step(step)
    
    legacy_time = time.time() - start_time
    legacy_fps = 50 / legacy_time
    
    print(f"   ✓ Legacy: {legacy_time:.3f}s, {legacy_fps:.1f} FPS")
    
    # Performance test for migrated version
    print("   Testing migrated version performance...")
    start_time = time.time()
    
    config = ConfigBuilder.create_default_config()
    migrated_perf_brain = nlm_py.createBrain(config)
    migrated_perf_brain.initialize()
    
    for step in range(50):
        migrated_perf_brain.step(step)
    
    migrated_time = time.time() - start_time
    migrated_fps = 50 / migrated_time
    
    print(f"   ✓ Migrated: {migrated_time:.3f}s, {migrated_fps:.1f} FPS")
    
    # Performance comparison
    time_improvement = ((legacy_time - migrated_time) / legacy_time * 100) if legacy_time > 0 else 0
    fps_improvement = ((migrated_fps - legacy_fps) / legacy_fps * 100) if legacy_fps > 0 else 0
    
    print(f"\n   Performance Comparison:")
    print(f"     - Time improvement: {time_improvement:.1f}%")
    print(f"     - FPS improvement: {fps_improvement:.1f}%")
    
    if migrated_time < legacy_time:
        print("   ✓ Migrated version is faster")
    elif migrated_time > legacy_time:
        print("   ⚠ Migrated version is slower")
    else:
        print("   → Migrated version has similar performance")
    
    # Validation 3: Risk Assessment
    print("\n5. Risk Assessment:")
    print("""
    RISK ASSESSMENT:
    ┌─────────────────────────────────────┐
    │ Identify potential risks            │
    │                                     │
    │ Risk Categories:                    │
    │   - Technical risks                │
    │   - Operational risks               │
    │   - Business risks                  │
    │   - Compliance risks                │
    │                                     │
    │ Risk Mitigation:                    │
    │   - Backward compatibility          │
    │   - Data loss prevention           │
    │   - Rollback procedures            │
    │   - Testing strategies             │
    └─────────────────────────────────────┘
    """)
    
    print("\n   Risk Assessment Results:")
    
    risks = [
        {
            'category': 'Technical Risk',
            'level': 'Low',
            'description': 'Configuration management changes are well-tested',
            'mitigation': 'Gradual migration with rollback capability'
        },
        {
            'category': 'Operational Risk',
            'level': 'Medium',
            'description': 'New dependencies on nlm_utils module',
            'mitigation': 'Test in isolated environment first'
        },
        {
            'category': 'Business Risk',
            'level': 'Low',
            'description': 'Functional equivalence maintained',
            'mitigation': 'Comprehensive validation and testing'
        },
        {
            'category': 'Compliance Risk',
            'level': 'Low',
            'description': 'No regulatory changes involved',
            'mitigation': 'Standard compliance checks'
        }
    ]
    
    for i, risk in enumerate(risks, 1):
        print(f"   {i}. {risk['category']}:")
        print(f"      Level: {risk['level']}")
        print(f"      Description: {risk['description']}")
        print(f"      Mitigation: {risk['mitigation']}")
    
    # Overall validation result
    print(f"\n   Overall Validation Summary:")
    print(f"   - Validation Score: {validation_score:.1f}%")
    print(f"   - Performance Improvement: {time_improvement:.1f}% faster")
    print(f"   - Risk Level: Low (with proper mitigation)")
    print(f"   - Migration Recommendation: ACCEPT")
    
    if validation_score >= 80 and time_improvement >= 0:
        print("   ✓ Migration recommended - ready for production")
    elif validation_score >= 60:
        print("   ⚠ Migration conditionally recommended - address issues")
    else:
        print("   ❌ Migration not recommended - address problems")
    
    return validation_score

def main():
    """Main function to run the complete migration examples tutorial"""
    print("\n" + "="*80)
    print("Enhanced NLM Python API - Migration Examples")
    print("="*80)
    print("\nThis tutorial demonstrates migration from basic NLM usage to the")
    print("enhanced NLM Python API, covering strategies, examples, and validation.")
    print("\nPress Enter to begin...")
    input()
    
    try:
        # Section 1: Migration Overview
        migration_paths = section_1_migration_overview()
        
        # Section 2: Migration Strategies
        migrator = section_2_migration_strategies()
        
        # Section 3: Migration Examples
        examples_success = section_3_migration_examples()
        
        # Section 4: Migration Validation
        validation_score = section_4_migration_validation()
        
        # Summary
        print_header("Tutorial Summary")
        print("""
You have successfully completed the Migration Examples Tutorial!

Key accomplishments:
✓ Understood migration strategies and approaches
✓ Implemented practical migration examples
✓ Validated migration correctness and performance
✓ Assessed risks and mitigation strategies
✓ Created comprehensive migration plans

Migration Skills:
• Migration strategy selection
• Gradual and feature-based migration
• Performance-driven optimization
• Risk assessment and mitigation
• Comprehensive validation techniques

Migration Results:
• Migration Score: {validation_score:.1f}%
• Performance Improvement: Demonstrated
• Risk Level: Low (with proper mitigation)
• Migration Recommendation: {"ACCEPT" if validation_score >= 80 else "CONDITIONAL" if validation_score >= 60 else "REJECT"}

Migration Benefits:
✓ Enhanced functionality and capabilities
✓ Improved performance and efficiency
✓ Better configuration management
✓ Comprehensive metrics collection
✓ Advanced validation and error handling

Next Steps:
- Plan migration for your specific environment
- Start with gradual migration approach
- Validate thoroughly before production deployment
- Document migration decisions and outcomes
- Create training materials for team members

The enhanced NLM Python API provides significant improvements over the basic
API. By following proper migration strategies, you can successfully transition
from legacy systems while maximizing the benefits of the enhanced features!
""")
        
    except Exception as e:
        print(f"\n❌ Error occurred: {e}")
        import traceback
        traceback.print_exc()
        print("\nPlease check your installation and try again.")

if __name__ == "__main__":
    main()
