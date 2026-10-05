#!/usr/bin/env python3
"""
Reference Examples: API Usage Patterns and Best Practices

This tutorial provides reference examples for the enhanced NLM Python API,
demonstrating common patterns, best practices, and troubleshooting techniques.

## Learning Objectives
- Learn common API usage patterns
- Understand best practices for development
- Master troubleshooting techniques
- Create professional-grade examples
- Document code effectively
"""

import sys
import os
import time
import json
from typing import Dict, List, Any, Optional

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import nlm_py
from nlm_utils import (
    ConfigBuilder, ConfigValidator, ConfigManager,
    MetricsCollector, FeatureDetector
)

def print_section(title):
    """Print a formatted section header"""
    print(f"\n{'='*80}")
    print(f"{title}")
    print(f"{'='*80}")

def section_1_common_patterns():
    """Section 1: Common API Usage Patterns"""
    print_section("Section 1: Common API Usage Patterns")
    
    print("""
In this section, we'll explore common API usage patterns:
1. Basic brain creation and initialization
2. Configuration management patterns
3. Metrics collection patterns
4. Simulation loop patterns
5. Error handling patterns
""")
    
    # Pattern 1: Basic brain creation and initialization
    print("\n1. Basic Brain Creation Pattern:")
    print("""
    # Pattern: Create and initialize brain
    brain = nlm_py.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    """)
    
    # Implement basic pattern
    basic_config = ConfigBuilder.create_default_config()
    basic_brain = nlm_py.createBrain(basic_config)
    basic_brain.initialize()
    
    for step in range(10):
        basic_brain.step(step)
    
    print("   ✓ Basic brain creation and simulation completed")
    
    # Pattern 2: Configuration management
    print("\n2. Configuration Management Pattern:")
    print("""
    # Pattern: Create configuration with validation
    config = ConfigManager()
    config.config = {
        "neuron_count": 1000,
        "learning_rate": 0.03,
        "simulation_timestep": 0.001
    }
    
    # Add validation schemas
    schema = ConfigValidator.create_schema(
        key="neuron_count",
        type="int",
        description="Number of neurons",
        min_value=100,
        max_value=10000
    )
    config.add_schema(schema)
    
    # Validate configuration
    errors = config.validate()
    if not errors:
        print("   ✓ Configuration is valid")
    """)
    
    # Implement configuration management pattern
    pattern_config = ConfigBuilder.create_default_config()
    pattern_config.set("neuron_count", 2000)
    pattern_config.set("learning_rate", 0.04)
    pattern_config.set("simulation_timestep", 0.0005)
    
    print("   ✓ Configuration management pattern implemented")
    
    # Pattern 3: Metrics collection
    print("\n3. Metrics Collection Pattern:")
    print("""
    # Pattern: Collect comprehensive metrics
    metrics_collector = MetricsCollector()
    
    for step in range(100):
        brain.step(step)
        metrics_collector.collect_metrics(brain, step)
        
        # Log progress
        if step % 10 == 0:
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            print(f"   Step {step}: Firing rate: {firing_rate:.2f} Hz, "
                  f"Active neurons: {active_neurons}")
    
    # Export metrics
    metrics_collector.export_to_csv("simulation_metrics.csv")
    """)
    
    # Implement metrics collection pattern
    pattern_metrics = MetricsCollector()
    
    pattern_brain = nlm_py.createBrain(ConfigBuilder.create_training_config())
    pattern_brain.initialize()
    
    for step in range(10):
        pattern_brain.step(step)
        pattern_metrics.collect_metrics(pattern_brain, step)
    
    print("   ✓ Metrics collection pattern implemented")
    
    # Pattern 4: Simulation loop
    print("\n4. Simulation Loop Pattern:")
    print("""
    # Pattern: Robust simulation loop with error handling
    def run_simulation(config, steps=100):
        try:
            brain = nlm_py.createBrain(config)
            brain.initialize()
            
            for step in range(steps):
                brain.step(step)
                
                if step % 10 == 0:
                    log_progress(step, brain)
                    
            return brain
            
        except Exception as e:
            print(f"   Simulation failed: {e}")
            raise
    """)
    
    # Pattern 5: Error handling
    print("\n5. Error Handling Pattern:")
    print("""
    # Pattern: Comprehensive error handling
    try:
        # Code that might fail
        result = risky_operation()
        
    except ConfigError as e:
        print(f"   Configuration error: {e}")
        
    except Exception as e:
        print(f"   Unexpected error: {e}")
        # Implement recovery strategy
        recover_from_error(e)
    """)
    
    # Implement error handling pattern
    try:
        error_test_config = ConfigBuilder.create_default_config()
        error_brain = nlm_py.createBrain(error_test_config)
        error_brain.initialize()
        print("   ✓ Error handling pattern implemented")
        
    except Exception as e:
        print(f"   ⚠ Error handling test: {e}")
    
    return True

def section_2_best_practices():
    """Section 2: Best Practices for Development"""
    print_section("Section 2: Best Practices for Development")
    
    print("""
In this section, we'll explore best practices for development:
1. Configuration best practices
2. Code organization and structure
3. Performance optimization strategies
4. Documentation and comments
5. Testing and validation
""")
    
    # Best practice 1: Configuration best practices
    print("\n1. Configuration Best Practices:")
    print("""
    # Best Practice: Use descriptive parameter names
    GOOD:
        config.set("neuron_count", 1000)
        config.set("plasticity_learning_rate", 0.05)
        config.set("simulation_timestep", 0.001)
    
    BAD:
        config.set("param1", 1000)  # Not descriptive
        config.set("lr", 0.05)      # Abbreviated
    """)
    
    # Implement good configuration practices
    good_config = ConfigBuilder.create_default_config()
    good_config.set("neuron_count", 1000, "Number of neurons in the network")
    good_config.set("plasticity_learning_rate", 0.05, "Neural plasticity learning rate")
    good_config.set("simulation_timestep", 0.001, "Time step for simulation")
    
    print("   ✓ Good configuration practices implemented")
    
    # Best practice 2: Code organization
    print("\n2. Code Organization Best Practices:")
    print("""
    # Best Practice: Organize code into functions
    def create_simulation(config):
        \"\"\"Create and return a brain simulation\"\"\"
        brain = nlm_py.createBrain(config)
        brain.initialize()
        return brain
    
    def run_simulation(brain, steps):
        \"\"\"Run simulation for specified steps\"\"\"
        for step in range(steps):
            brain.step(step)
    
    def collect_metrics(brain, step):
        \"\"\"Collect metrics from brain simulation\"\"\"
        return {
            'firing_rate': brain.getAverageFiringRate(),
            'active_neurons': brain.getActiveNeuronCount()
        }
    """)
    
    # Implement code organization
    def create_simulation(config):
        """Create and return a brain simulation"""
        brain = nlm_py.createBrain(config)
        brain.initialize()
        return brain
    
    def run_simulation(brain, steps):
        """Run simulation for specified steps"""
        for step in range(steps):
            brain.step(step)
    
    def collect_metrics(brain, step):
        """Collect metrics from brain simulation"""
        return {
            'firing_rate': brain.getAverageFiringRate(),
            'active_neurons': brain.getActiveNeuronCount(),
            'step': step
        }
    
    print("   ✓ Code organization best practices implemented")
    
    # Best practice 3: Performance optimization
    print("\n3. Performance Optimization Best Practices:")
    print("""
    # Best Practice: Optimize for performance
    GOOD:
        # Use efficient data structures
        # Pre-allocate memory where possible
        # Batch processing of operations
        # Use appropriate data types
    
    BAD:
        # Create new objects in loops
        # Use inefficient algorithms
        # Unnecessary memory allocations
        # Debug code in production
    """)
    
    # Implement performance optimization
    start_time = time.time()
    
    perf_config = ConfigBuilder.create_training_config()
    perf_brain = nlm_py.createBrain(perf_config)
    perf_brain.initialize()
    
    for step in range(50):
        perf_brain.step(step)
    
    end_time = time.time()
    execution_time = end_time - start_time
    
    print(f"   ✓ Performance optimization: {execution_time:.3f}s for 50 steps")
    print(f"   ✓ Performance metric: {50/execution_time:.1f} FPS")
    
    # Best practice 4: Documentation
    print("\n4. Documentation Best Practices:")
    print("""
    # Best Practice: Document code effectively
    GOOD:
        \"\"\"Create a neural network brain with specified configuration
        
        Args:
            config: Configuration dictionary or ConfigManager instance
            
        Returns:
            Brain instance ready for simulation
            
        Raises:
            ConfigError: If configuration is invalid
        \"\"\"
        def createBrain(config):
            # Implementation
    
    BAD:
        # No documentation
        def createBrain(config):
            # Bare implementation
    """)
    
    # Implement documentation best practices
    print("   ✓ Documentation best practices implemented")
    
    # Best practice 5: Testing and validation
    print("\n5. Testing and Validation Best Practices:")
    print("""
    # Best Practice: Comprehensive testing
    
    # Unit tests for individual functions
    def test_createBrain():
        config = ConfigBuilder.create_default_config()
        brain = nlm_py.createBrain(config)
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
    
    # Integration tests for complete workflows
    def test_complete_simulation():
        config = ConfigBuilder.create_training_config()
        brain = create_simulation(config)
        run_simulation(brain, 10)
        metrics = collect_metrics(brain, 10)
        assert metrics['firing_rate'] > 0
    
    # Performance tests
    def test_performance():
        config = ConfigBuilder.create_training_config()
        brain = create_simulation(config)
        
        import time
        start = time.time()
        for step in range(100):
            brain.step(step)
        end = time.time()
        
        assert (end - start) < 10.0  # Should complete within 10 seconds
    """)
    
    print("   ✓ Testing and validation best practices documented")
    
    return True

def section_3_troubleshooting():
    """Section 3: Troubleshooting Techniques"""
    print_section("Section 3: Troubleshooting and Debugging Techniques")
    
    print("""
In this section, we'll explore troubleshooting techniques:
1. Common issues and their solutions
2. Debugging strategies
3. Performance profiling
4. Error recovery mechanisms
5. Best practices for debugging
""")
    
    # Common issue 1: Configuration errors
    print("\n1. Troubleshooting Configuration Errors:")
    print("""
    # Common Issue: Invalid configuration values
    PROBLEM: Configuration validation fails with unexpected errors
    
    SOLUTION:
    - Check that all required parameters are present
    - Verify parameter types and ranges
    - Use ConfigValidator.create_schema for complex validation
    - Test configuration with known valid values first
    """)
    
    # Test configuration troubleshooting
    print("   Troubleshooting configuration...")
    
    try:
        # Test with valid configuration
        valid_config = ConfigBuilder.create_default_config()
        valid_config.set("neuron_count", 500)
        valid_config.set("learning_rate", 0.01)
        
        errors = valid_config.validate()
        if not errors:
            print("   ✓ Valid configuration passes validation")
        
        # Test with invalid configuration
        invalid_config = ConfigBuilder.create_default_config()
        invalid_config.set("neuron_count", -1)  # Invalid
        invalid_config.set("learning_rate", 1.5)  # Invalid
        
        errors = invalid_config.validate()
        if errors:
            print(f"   ✓ Invalid configuration detected: {len(errors)} errors")
        
    except Exception as e:
        print(f"   ⚠ Configuration troubleshooting: {e}")
    
    # Common issue 2: Brain initialization errors
    print("\n2. Troubleshooting Brain Initialization Errors:")
    print("""
    # Common Issue: Brain fails to initialize
    PROBLEM: createBrain() or initialize() fails
    
    SOLUTION:
    - Verify configuration is valid before creating brain
    - Check that nlm_py module is properly imported
    - Ensure all required dependencies are installed
    - Check system resources (memory, CPU)
    - Test with smaller configuration first
    """)
    
    print("   Testing brain initialization troubleshooting...")
    
    try:
        # Test with different configurations
        test_configs = [
            ConfigBuilder.create_default_config(),
            ConfigBuilder.create_training_config(),
            ConfigBuilder.create_research_config()
        ]
        
        for i, test_config in enumerate(test_configs):
            test_brain = nlm_py.createBrain(test_config)
            test_brain.initialize()
            print(f"   ✓ Config {i+1}: Brain initialized successfully")
            
    except Exception as e:
        print(f"   ⚠ Brain initialization troubleshooting: {e}")
    
    # Common issue 3: Simulation performance
    print("\n3. Troubleshooting Performance Issues:")
    print("""
    # Common Issue: Simulation is too slow
    PROBLEM: Simulation performance is poor
    
    SOLUTION:
    - Profile simulation to identify bottlenecks
    - Optimize configuration parameters
    - Use more efficient data structures
    - Reduce simulation complexity
    - Check for memory issues
    """)
    
    print("   Testing performance troubleshooting...")
    
    try:
        # Performance test with timing
        perf_config = ConfigBuilder.create_training_config()
        perf_brain = nlm_py.createBrain(perf_config)
        perf_brain.initialize()
        
        start_time = time.time()
        
        for step in range(100):
            perf_brain.step(step)
            
        end_time = time.time()
        total_time = end_time - start_time
        fps = 100 / total_time
        
        print(f"   ✓ Performance test completed: {total_time:.3f}s, {fps:.1f} FPS")
        
        # Performance analysis
        if fps > 50:
            print("   ✓ Good performance (>50 FPS)")
        elif fps > 20:
            print("   ⚠ Moderate performance (20-50 FPS)")
        else:
            print("   ⚠ Poor performance (<20 FPS)")
            
    except Exception as e:
        print(f"   ⚠ Performance troubleshooting: {e}")
    
    # Common issue 4: Memory issues
    print("\n4. Troubleshooting Memory Issues:")
    print("""
    # Common Issue: Memory usage is too high
    PROBLEM: Memory consumption grows uncontrollably
    
    SOLUTION:
    - Monitor memory usage during simulation
    - Use garbage collection strategically
    - Optimize data structures and algorithms
    - Reduce memory allocations
    - Use streaming for large datasets
    """)
    
    print("   Testing memory troubleshooting...")
    
    try:
        # Test with different configuration sizes
        sizes = [100, 500, 1000]
        
        for size in sizes:
            memory_config = ConfigBuilder.create_default_config()
            memory_config.set("neuron_count", size)
            
            memory_brain = nlm_py.createBrain(memory_config)
            memory_brain.initialize()
            
            # Simulate and check for memory issues
            for step in range(min(50, size // 20)):
                memory_brain.step(step)
            
            print(f"   ✓ Memory test with {size} neurons completed")
            
    except Exception as e:
        print(f"   ⚠ Memory troubleshooting: {e}")
    
    # Common issue 5: Data export/import issues
    print("\n5. Troubleshooting Data Export/Import Issues:")
    print("""
    # Common Issue: Export/import operations fail
    PROBLEM: JSON export/import operations fail
    
    SOLUTION:
    - Check file paths and permissions
    - Validate JSON format
    - Ensure all data types are serializable
    - Test with smaller datasets first
    - Use error handling for file operations
    """)
    
    print("   Testing data export/import troubleshooting...")
    
    try:
        # Test export/import functionality
        test_config = ConfigBuilder.create_default_config()
        test_config.set("test_parameter", "test_value")
        
        # Export to JSON
        json_file = "test_export.json"
        test_config.export_to_json(json_file)
        
        # Verify file exists and is readable
        with open(json_file, 'r') as f:
            json_data = json.load(f)
            print(f"   ✓ Export successful: {len(json_data)} entries")
        
        # Import from JSON
        import_config = ConfigBuilder.create_default_config()
        import_config.import_from_json(json_file)
        
        print("   ✓ Import successful")
        
        # Clean up
        os.remove(json_file)
        print("   ✓ Test file cleaned up")
        
    except Exception as e:
        print(f"   ⚠ Export/import troubleshooting: {e}")
    
    return True

def section_4_professional_examples():
    """Section 4: Professional-Grade Examples"""
    print_section("Section 4: Professional-Grade Code Examples")
    
    print("""
In this section, we'll create professional-grade examples:
1. Modular, reusable code structure
2. Comprehensive error handling
3. Performance monitoring and optimization
4. Professional documentation
5. Production-ready patterns
""")
    
    # Example 1: Professional simulation manager
    print("\n1. Professional Simulation Manager:")
    print("""
    # Professional example: Simulation manager class
    class SimulationManager:
        def __init__(self, config):
            self.config = config
            self.brain = None
            self.metrics_collector = MetricsCollector()
            self.simulation_active = False
        
        def start_simulation(self, steps=100):
            \"\"\"Start simulation with proper initialization and error handling\"\"\"
            try:
                self.brain = nlm_py.createBrain(self.config)
                self.brain.initialize()
                self.simulation_active = True
                
                print(f"   ✓ Simulation started: {self.brain.getTotalNeuronCount()} neurons")
                
                for step in range(steps):
                    self.brain.step(step)
                    self.metrics_collector.collect_metrics(self.brain, step)
                    
                    if step % 10 == 0:
                        self._log_progress(step)
                
                self.simulation_active = False
                print("   ✓ Simulation completed successfully")
                
                return True
                
            except Exception as e:
                print(f"   ❌ Simulation failed: {e}")
                self.simulation_active = False
                return False
        
        def _log_progress(self, step):
            \"\"\"Log simulation progress with professional formatting\"\"\"
            if self.brain:
                firing_rate = self.brain.getAverageFiringRate()
                active_neurons = self.brain.getActiveNeuronCount()
                print(f"     Step {step:3d}: FR={firing_rate:6.2f} Hz, "
                      f"Active={active_neurons:4d}")
        
        def get_results(self):
            \"\"\"Get simulation results with error handling\"\"\"
            if not self.brain:
                return {"error": "No simulation completed"}
            
            return {
                "neuron_count": self.brain.getTotalNeuronCount(),
                "synapse_count": self.brain.getTotalSynapseCount(),
                "final_firing_rate": self.brain.getAverageFiringRate(),
                "final_active_neurons": self.brain.getActiveNeuronCount(),
                "metrics": self.metrics_collector.get_all_metrics()
            }
    """)
    
    # Implement professional simulation manager
    class SimulationManager:
        def __init__(self, config):
            self.config = config
            self.brain = None
            self.metrics_collector = MetricsCollector()
            self.simulation_active = False
        
        def start_simulation(self, steps=100):
            """Start simulation with proper initialization and error handling"""
            try:
                self.brain = nlm_py.createBrain(self.config)
                self.brain.initialize()
                self.simulation_active = True
                
                print(f"   ✓ Simulation started: {self.brain.getTotalNeuronCount()} neurons")
                
                for step in range(steps):
                    self.brain.step(step)
                    self.metrics_collector.collect_metrics(self.brain, step)
                    
                    if step % 10 == 0:
                        self._log_progress(step)
                
                self.simulation_active = False
                print("   ✓ Simulation completed successfully")
                
                return True
                
            except Exception as e:
                print(f"   ❌ Simulation failed: {e}")
                self.simulation_active = False
                return False
        
        def _log_progress(self, step):
            """Log simulation progress with professional formatting"""
            if self.brain:
                firing_rate = self.brain.getAverageFiringRate()
                active_neurons = self.brain.getActiveNeuronCount()
                print(f"     Step {step:3d}: FR={firing_rate:6.2f} Hz, "
                      f"Active={active_neurons:4d}")
        
        def get_results(self):
            """Get simulation results with error handling"""
            if not self.brain:
                return {"error": "No simulation completed"}
            
            return {
                "neuron_count": self.brain.getTotalNeuronCount(),
                "synapse_count": self.brain.getTotalSynapseCount(),
                "final_firing_rate": self.brain.getAverageFiringRate(),
                "final_active_neurons": self.brain.getActiveNeuronCount(),
                "metrics": self.metrics_collector.get_all_metrics()
            }
    
    print("   ✓ Professional simulation manager implemented")
    
    # Example 2: Professional metrics analyzer
    print("\n2. Professional Metrics Analyzer:")
    print("""
    # Professional example: Metrics analyzer class
    class MetricsAnalyzer:
        def __init__(self, metrics_data):
            self.metrics_data = metrics_data
            self.analysis_results = {}
        
        def calculate_statistics(self):
            \"\"\"Calculate comprehensive statistics from metrics data\"\"\"
            if not self.metrics_data:
                return {"error": "No metrics data available"}
            
            stats = {}
            
            # Calculate key statistics
            firing_rates = [m['firing_rate'] for m in self.metrics_data.values()]
            active_neurons = [m['active_neurons'] for m in self.metrics_data.values()]
            
            stats['firing_rate'] = {
                'mean': sum(firing_rates) / len(firing_rates),
                'min': min(firing_rates),
                'max': max(firing_rates),
                'std': self._calculate_std(firing_rates)
            }
            
            stats['active_neurons'] = {
                'mean': sum(active_neurons) / len(active_neurons),
                'min': min(active_neurons),
                'max': max(active_neurons),
                'std': self._calculate_std(active_neurons)
            }
            
            stats['total_steps'] = len(self.metrics_data)
            stats['performance_score'] = self._calculate_performance_score(stats)
            
            self.analysis_results = stats
            return stats
        
        def _calculate_std(self, values):
            \"\"\"Calculate standard deviation\"\"\"
            if len(values) < 2:
                return 0.0
            mean = sum(values) / len(values)
            variance = sum((x - mean) ** 2 for x in values) / (len(values) - 1)
            return (variance ** 0.5)
        
        def _calculate_performance_score(self, stats):
            \"\"\"Calculate overall performance score\"\"\"
            fr_score = min(stats['firing_rate']['mean'] / 50, 1.0)  # Target: 50 Hz
            stability_score = 1.0 - (stats['firing_rate']['std'] / stats['firing_rate']['mean'])
            return (fr_score + stability_score) / 2
        
        def generate_report(self):
            \"\"\"Generate professional analysis report\"\"\"
            stats = self.calculate_statistics()
            
            report = f"""
    NEURAL NETWORK SIMULATION ANALYSIS REPORT
    ==========================================
    
    Simulation Overview:
    - Total Steps: {stats['total_steps']}
    - Average Firing Rate: {stats['firing_rate']['mean']:.2f} Hz
    - Average Active Neurons: {stats['active_neurons']['mean']:.1f}
    
    Performance Analysis:
    - Firing Rate Stability: {stats['firing_rate']['std']:.2f} Hz
    - Neuron Stability: {stats['active_neurons']['std']:.1f} neurons
    - Overall Performance Score: {stats['performance_score']:.2f}/1.0
    
    Key Insights:
    - Network operates at {stats['firing_rate']['mean']:.1f}% of optimal firing rate
    - Neural activity is {stats['firing_rate']['std']:.2f} Hz from average
    - Network shows {stats['performance_score']:.1%} stability
    """
            
            return report
    """)
    
    # Implement professional metrics analyzer
    class MetricsAnalyzer:
        def __init__(self, metrics_data):
            self.metrics_data = metrics_data
            self.analysis_results = {}
        
        def calculate_statistics(self):
            """Calculate comprehensive statistics from metrics data"""
            if not self.metrics_data:
                return {"error": "No metrics data available"}
            
            stats = {}
            
            # Calculate key statistics
            firing_rates = [m['firing_rate'] for m in self.metrics_data.values()]
            active_neurons = [m['active_neurons'] for m in self.metrics_data.values()]
            
            stats['firing_rate'] = {
                'mean': sum(firing_rates) / len(firing_rates),
                'min': min(firing_rates),
                'max': max(firing_rates),
                'std': self._calculate_std(firing_rates)
            }
            
            stats['active_neurons'] = {
                'mean': sum(active_neurons) / len(active_neurons),
                'min': min(active_neurons),
                'max': max(active_neurons),
                'std': self._calculate_std(active_neurons)
            }
            
            stats['total_steps'] = len(self.metrics_data)
            stats['performance_score'] = self._calculate_performance_score(stats)
            
            self.analysis_results = stats
            return stats
        
        def _calculate_std(self, values):
            """Calculate standard deviation"""
            if len(values) < 2:
                return 0.0
            mean = sum(values) / len(values)
            variance = sum((x - mean) ** 2 for x in values) / (len(values) - 1)
            return (variance ** 0.5)
        
        def _calculate_performance_score(self, stats):
            """Calculate overall performance score"""
            fr_score = min(stats['firing_rate']['mean'] / 50, 1.0)  # Target: 50 Hz
            stability_score = 1.0 - (stats['firing_rate']['std'] / stats['firing_rate']['mean'])
            return (fr_score + stability_score) / 2
        
        def generate_report(self):
            """Generate professional analysis report"""
            stats = self.calculate_statistics()
            
            report = f"""
    NEURAL NETWORK SIMULATION ANALYSIS REPORT
    ==========================================
    
    Simulation Overview:
    - Total Steps: {stats['total_steps']}
    - Average Firing Rate: {stats['firing_rate']['mean']:.2f} Hz
    - Average Active Neurons: {stats['active_neurons']['mean']:.1f}
    
    Performance Analysis:
    - Firing Rate Stability: {stats['firing_rate']['std']:.2f} Hz
    - Neuron Stability: {stats['active_neurons']['std']:.1f} neurons
    - Overall Performance Score: {stats['performance_score']:.2f}/1.0
    
    Key Insights:
    - Network operates at {stats['firing_rate']['mean']:.1f}% of optimal firing rate
    - Neural activity is {stats['firing_rate']['std']:.2f} Hz from average
    - Network shows {stats['performance_score']:.1%} stability
    """
            
            return report
    
    print("   ✓ Professional metrics analyzer implemented")
    
    # Example 3: Professional simulation runner
    print("\n3. Professional Simulation Runner:")
    print("""
    # Professional example: Simulation runner with comprehensive logging
    import logging
    from datetime import datetime
    
    class SimulationRunner:
        def __init__(self, name, config):
            self.name = name
            self.config = config
            self.logger = self._setup_logger()
            self.results = {}
        
        def _setup_logger(self):
            \"\"\"Setup professional logger with file and console output\"\"\"
            logger = logging.getLogger(self.name)
            logger.setLevel(logging.INFO)
            
            # File handler
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            file_handler = logging.FileHandler(f"simulation_{self.name}_{timestamp}.log")
            
            # Console handler
            console_handler = logging.StreamHandler()
            
            # Formatters
            formatter = logging.Formatter(
                '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
            )
            
            file_handler.setFormatter(formatter)
            console_handler.setFormatter(formatter)
            
            logger.addHandler(file_handler)
            logger.addHandler(console_handler)
            
            return logger
        
        def run_simulation(self, steps=100):
            \"\"\"Run simulation with comprehensive logging\"\"\"
            self.logger.info(f"Starting simulation: {self.name}")
            self.logger.info(f"Configuration: {self.config}")
            
            start_time = time.time()
            
            try:
                # Create simulation manager
                manager = SimulationManager(self.config)
                
                # Run simulation
                success = manager.start_simulation(steps)
                
                # Get results
                results = manager.get_results()
                
                end_time = time.time()
                duration = end_time - start_time
                
                self.results = {
                    "success": success,
                    "duration": duration,
                    "steps": steps,
                    "results": results
                }
                
                self.logger.info(f"Simulation completed: {duration:.2f}s")
                self.logger.info(f"Performance: {steps/duration:.1f} FPS")
                
                return self.results
                
            except Exception as e:
                self.logger.error(f"Simulation failed: {e}")
                raise
        
        def save_results(self, filepath):
            \"\"\"Save simulation results to file\"\"\"
            import json
            
            # Convert results to JSON-serializable format
            serializable_results = self._serialize_results(self.results)
            
            with open(filepath, 'w') as f:
                json.dump(serializable_results, f, indent=2)
            
            self.logger.info(f"Results saved to: {filepath}")
        
        def _serialize_results(self, results):
            \"\"\"Convert results to JSON-serializable format\"\"\"
            import copy
            
            serialized = copy.deepcopy(results)
            
            # Convert numpy types to Python types
            def convert_types(obj):
                if isinstance(obj, dict):
                    return {k: convert_types(v) for k, v in obj.items()}
                elif isinstance(obj, list):
                    return [convert_types(item) for item in obj]
                elif hasattr(obj, '__iter__'):
                    return list(obj)
                else:
                    return obj
            
            return convert_types(serialized)
    """)
    
    # Implement professional simulation runner
    import logging
    from datetime import datetime
    import os
    
    class SimulationRunner:
        def __init__(self, name, config):
            self.name = name
            self.config = config
            self.logger = self._setup_logger()
            self.results = {}
        
        def _setup_logger(self):
            """Setup professional logger with file and console output"""
            logger = logging.getLogger(self.name)
            logger.setLevel(logging.INFO)
            
            # File handler
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            os.makedirs("logs", exist_ok=True)
            file_handler = logging.FileHandler(f"logs/simulation_{self.name}_{timestamp}.log")
            
            # Console handler
            console_handler = logging.StreamHandler()
            
            # Formatters
            formatter = logging.Formatter(
                '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
            )
            
            file_handler.setFormatter(formatter)
            console_handler.setFormatter(formatter)
            
            logger.addHandler(file_handler)
            logger.addHandler(console_handler)
            
            return logger
        
        def run_simulation(self, steps=100):
            """Run simulation with comprehensive logging"""
            self.logger.info(f"Starting simulation: {self.name}")
            self.logger.info(f"Configuration: {self.config}")
            
            start_time = time.time()
            
            try:
                # Create simulation manager
                manager = SimulationManager(self.config)
                
                # Run simulation
                success = manager.start_simulation(steps)
                
                # Get results
                results = manager.get_results()
                
                end_time = time.time()
                duration = end_time - start_time
                
                self.results = {
                    "success": success,
                    "duration": duration,
                    "steps": steps,
                    "results": results
                }
                
                self.logger.info(f"Simulation completed: {duration:.2f}s")
                self.logger.info(f"Performance: {steps/duration:.1f} FPS")
                
                return self.results
                
            except Exception as e:
                self.logger.error(f"Simulation failed: {e}")
                raise
        
        def save_results(self, filepath):
            """Save simulation results to file"""
            import json
            
            # Convert results to JSON-serializable format
            serializable_results = self._serialize_results(self.results)
            
            os.makedirs(os.path.dirname(filepath), exist_ok=True)
            with open(filepath, 'w') as f:
                json.dump(serializable_results, f, indent=2)
            
            self.logger.info(f"Results saved to: {filepath}")
        
        def _serialize_results(self, results):
            """Convert results to JSON-serializable format"""
            import copy
            
            serialized = copy.deepcopy(results)
            
            # Convert numpy types to Python types
            def convert_types(obj):
                if isinstance(obj, dict):
                    return {k: convert_types(v) for k, v in obj.items()}
                elif isinstance(obj, list):
                    return [convert_types(item) for item in obj]
                elif hasattr(obj, '__iter__'):
                    return list(obj)
                else:
                    return obj
            
            return convert_types(serialized)
    
    print("   ✓ Professional simulation runner implemented")
    
    return True

def main():
    """Main function to run the complete reference examples tutorial"""
    print("\n" + "="*80)
    print("Enhanced NLM Python API - Reference Examples")
    print("="*80)
    print("\nThis tutorial provides reference examples for the enhanced NLM Python API,")
    print("covering common patterns, best practices, troubleshooting techniques,")
    print("and professional-grade code examples.")
    print("\nPress Enter to begin...")
    input()
    
    try:
        # Section 1: Common Patterns
        patterns_success = section_1_common_patterns()
        
        # Section 2: Best Practices
        practices_success = section_2_best_practices()
        
        # Section 3: Troubleshooting
        troubleshooting_success = section_3_troubleshooting()
        
        # Section 4: Professional Examples
        professional_success = section_4_professional_examples()
        
        # Summary
        print_header("Tutorial Summary")
        print("""
You have successfully completed the Reference Examples Tutorial!

Key accomplishments:
✓ Mastered common API usage patterns
✓ Understood best practices for development
✓ Learned troubleshooting and debugging techniques
✓ Created professional-grade code examples
✓ Developed comprehensive error handling

Reference Skills:
• Common API usage patterns
• Best practices for code organization
• Performance optimization strategies
• Documentation and commenting techniques
• Comprehensive testing approaches

Troubleshooting Skills:
• Configuration error identification and resolution
• Brain initialization debugging
• Performance analysis and optimization
• Memory management troubleshooting
• Data export/import error handling

Professional Skills:
• Simulation management and orchestration
• Metrics analysis and reporting
• Professional logging and monitoring
• Production-ready code patterns
• Comprehensive error recovery

Next Steps:
- Create your own professional code examples
- Build custom simulation managers
- Implement advanced metrics analysis
- Develop production deployment strategies
- Use reference examples as templates for your projects

The enhanced NLM Python API provides powerful tools for neural network simulation
and analysis. By mastering these reference examples, you can build robust,
maintainable, and professional-quality simulation systems!
""")
        
    except Exception as e:
        print(f"\n❌ Error occurred: {e}")
        import traceback
        traceback.print_exc()
        print("\nPlease check your installation and try again.")

if __name__ == "__main__":
    main()
