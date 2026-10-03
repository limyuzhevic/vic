#!/usr/bin/env python3
"""
NLM Documentation and Examples Generator

This script generates comprehensive documentation and examples for the NLM project,
including getting started guides, API references, and advanced usage examples.
"""

# ==============================================================================
# DOCUMENTATION GENERATOR
# ==============================================================================

class NLMDocumentationGenerator:
    """Generate comprehensive documentation for NLM"""
    
    def __init__(self):
        self.documentation_structure = {
            'getting_started': {
                'title': 'Getting Started',
                'description': 'Quick start guide for using NLM',
                'sections': [
                    'introduction',
                    'installation',
                    'basic_usage',
                    'running_simulations'
                ]
            },
            'user_guide': {
                'title': 'User Guide',
                'description': 'Comprehensive guide for using NLM',
                'sections': [
                    'core_concepts',
                    'working_memory',
                    'neuromodulation',
                    'plasticity_rules',
                    'development',
                    'simulation_control'
                ]
            },
            'api_reference': {
                'title': 'API Reference',
                'description': 'Complete API documentation',
                'sections': [
                    'brain_class',
                    'agentbrain_class',
                    'simpleworld_class',
                    'config_class',
                    'measurement_classes'
                ]
            },
            'examples': {
                'title': 'Examples Gallery',
                'description': 'Collection of example code',
                'sections': [
                    'basic_brain_example',
                    'agent_simulation_example',
                    'learning_environment_example',
                    'performance_benchmarks',
                    'advanced_features'
                ]
            },
            'advanced_topics': {
                'title': 'Advanced Topics',
                'description': 'Advanced usage and techniques',
                'sections': [
                    'custom_configurations',
                    'multi_environment_simulation',
                    'performance_optimization',
                    'plugin_development',
                    'distributed_computing'
                ]
            },
            'troubleshooting': {
                'title': 'Troubleshooting',
                'description': 'Common problems and solutions',
                'sections': [
                    'installation_issues',
                    'performance_issues',
                    'configuration_issues',
                    'simulation_problems',
                    'debugging_techniques'
                ]
            }
        }
        
        self.example_templates = {}
        self.code_examples = {}
    
    def generate_documentation(self, output_dir):
        """Generate all documentation"""
        import os
        
        os.makedirs(output_dir, exist_ok=True)
        
        for category, info in self.documentation_structure.items():
            category_dir = os.path.join(output_dir, category)
            os.makedirs(category_dir, exist_ok=True)
            
            # Generate index file
            index_content = self._generate_index(info)
            with open(os.path.join(category_dir, 'index.md'), 'w') as f:
                f.write(index_content)
            
            # Generate section files
            for section in info['sections']:
                section_content = self._generate_section(section, info)
                section_file = os.path.join(category_dir, f"{section}.md")
                with open(section_file, 'w') as f:
                    f.write(section_content)
            
            # Generate examples gallery if applicable
            if category == 'examples':
                self._generate_examples_gallery(category_dir)
        
        # Generate main README
        self._generate_main_readme(output_dir)
        
        return True
    
    def _generate_index(self, category_info):
        """Generate index file for a category"""
        content = f"""# {category_info['title']}

{category_info['description']}

## Contents

"""
        
        for section in category_info['sections']:
            content += f"- [{section.replace('_', ' ').title()}](./{section}.md)\n"
        
        return content
    
    def _generate_section(self, section_name, category_info):
        """Generate content for a section"""
        if section_name == 'introduction':
            return self._generate_introduction()
        elif section_name == 'installation':
            return self._generate_installation()
        elif section_name == 'basic_usage':
            return self._generate_basic_usage()
        elif section_name == 'running_simulations':
            return self._generate_running_simulations()
        elif section_name == 'core_concepts':
            return self._generate_core_concepts()
        elif section_name == 'working_memory':
            return self._generate_working_memory_section()
        elif section_name == 'neuromodulation':
            return self._generate_neuromodulation_section()
        elif section_name == 'plasticity_rules':
            return self._generate_plasticity_rules_section()
        elif section_name == 'development':
            return self._generate_development_section()
        elif section_name == 'simulation_control':
            return self._generate_simulation_control_section()
        elif section_name == 'brain_class':
            return self._generate_brain_class_reference()
        elif section_name == 'agentbrain_class':
            return self._generate_agentbrain_class_reference()
        elif section_name == 'simpleworld_class':
            return self._generate_simpleworld_class_reference()
        elif section_name == 'config_class':
            return self._generate_config_class_reference()
        elif section_name == 'measurement_classes':
            return self._generate_measurement_classes_reference()
        elif section_name == 'basic_brain_example':
            return self._generate_basic_brain_example()
        elif section_name == 'agent_simulation_example':
            return self._generate_agent_simulation_example()
        elif section_name == 'learning_environment_example':
            return self._generate_learning_environment_example()
        elif section_name == 'performance_benchmarks':
            return self._generate_performance_benchmarks()
        elif section_name == 'advanced_features':
            return self._generate_advanced_features_example()
        elif section_name == 'custom_configurations':
            return self._generate_custom_configurations()
        elif section_name == 'multi_environment_simulation':
            return self._generate_multi_environment_simulation()
        elif section_name == 'performance_optimization':
            return self._generate_performance_optimization()
        elif section_name == 'plugin_development':
            return self._generate_plugin_development()
        elif section_name == 'distributed_computing':
            return self._generate_distributed_computing()
        elif section_name == 'installation_issues':
            return self._generate_installation_issues()
        elif section_name == 'performance_issues':
            return self._generate_performance_issues()
        elif section_name == 'configuration_issues':
            return self._generate_configuration_issues()
        elif section_name == 'simulation_problems':
            return self._generate_simulation_problems()
        elif section_name == 'debugging_techniques':
            return self._generate_debugging_techniques()
        else:
            return f"# {section_name.replace('_', ' ').title()}\n\nContent for {section_name} would go here."
    
    def _generate_examples_gallery(self, output_dir):
        """Generate examples gallery"""
        gallery_content = """# Examples Gallery

This gallery showcases various NLM examples, from simple demonstrations to advanced applications.

## Basic Examples

### Basic Brain Example
This example demonstrates creating and running a simple brain simulation.

```python
import pynlm

# Create a brain with default configuration
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete! Total spikes: {brain.get_total_spike_count()}")
```

### Agent Simulation Example
This example demonstrates creating a complete agent simulation.

```python
import pynlm

# Create brain and agent
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

agent = pynlm.create_agent_brain(brain)

world = pynlm.create_simple_world()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable learning features
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
    
    if step % 100 == 0:
        print(f"Step {step}: Action={action}, Curiosity={agent.get_curiosity_level():.3f}")

print("Agent simulation complete!")
```

## Advanced Examples

### Learning Environment Example
This example demonstrates creating a complex learning environment.

```python
import pynlm

def run_learning_environment(config_file=None):
    """Run a learning environment with NLM"""
    # Load configuration
    if config_file:
        config = pynlm.create_config_from_file(config_file)
    else:
        config = pynlm.create_default_config()
    
    # Create brain
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.create_agent_brain(brain)
    
    # Create world
    world = pynlm.create_simple_world()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable subsystems
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    
    # Run simulation
    for episode in range(100):
        # Run episode
        for step in range(500):
            world.update(0.1)
            percept = world.get_sensory_percept()
            agent.process_sensory_input(percept)
            brain.step(step)
            action = agent.decode_motor_command()
            world.apply_motor_command(action, world.get_simulation_time())
        
        # Log episode statistics
        print(f"Episode {episode}:")
        print(f"  Total spikes: {brain.get_total_spike_count()}")
        print(f"  Firing neurons: {brain.get_firing_neuron_count()}")
        print(f"  Curiosity: {agent.get_curiosity_level():.3f}")
        print(f"  Novelty: {agent.get_novelty_level():.3f}")
        print(f"  Development stage: {brain.get_developmental_stage()}")
    
    return brain, agent, world

# Run learning environment
brain, agent, world = run_learning_environment()
```

### Performance Benchmarks Example
This example demonstrates performance benchmarking.

```python
import pynlm
import time
import psutil
import os

class PerformanceBenchmark:
    """Performance benchmarking for NLM"""
    
    def __init__(self):
        self.results = {}
        self.system_info = {}
    
    def get_system_info(self):
        """Get system information"""
        process = psutil.Process(os.getpid())
        memory_info = process.memory_info()
        
        self.system_info = {
            'cpu_count': psutil.cpu_count(),
            'memory_total': psutil.virtual_memory().total / (1024**3),  # GB
            'memory_available': psutil.virtual_memory().available / (1024**3),  # GB
            'cpu_frequency': psutil.cpu_freq().current if psutil.cpu_freq() else 0,
            'memory_usage': memory_info.rss / (1024**2),  # MB
            'python_version': f"{os.sys.version_info.major}.{os.sys.version_info.minor}.{os.sys.version_info.micro}"
        }
        
        return self.system_info
    
    def benchmark_brain_creation(self, neuron_counts=[100, 1000, 5000]):
        """Benchmark brain creation for different neuron counts"""
        print("Benchmarking brain creation...")
        
        for neuron_count in neuron_counts:
            start_time = time.time()
            start_memory = psutil.Process(os.getpid()).memory_info().rss
            
            config = pynlm.create_default_config()
            config.set('brain.neuron_count', neuron_count)
            
            brain = pynlm.create_brain(config)
            brain.initialize()
            
            end_time = time.time()
            end_memory = psutil.Process(os.getpid()).memory_info().rss
            
            creation_time = end_time - start_time
            memory_used = (end_memory - start_memory) / (1024**2)  # MB
            
            self.results[f'brain_creation_{neuron_count}'] = {
                'neurons': neuron_count,
                'time_seconds': creation_time,
                'memory_mb': memory_used,
                'time_per_neuron': creation_time / neuron_count
            }
        
        return self.results
    
    def benchmark_simulation(self, steps_list=[100, 1000, 5000], neuron_count=1000):
        """Benchmark simulation for different step counts"""
        print("Benchmarking simulation...")
        
        for steps in steps_list:
            start_time = time.time()
            start_memory = psutil.Process(os.getpid()).memory_info().rss
            
            config = pynlm.create_default_config()
            config.set('brain.neuron_count', neuron_count)
            
            brain = pynlm.create_brain(config)
            brain.initialize()
            
            for step in range(steps):
                brain.step(step)
            
            end_time = time.time()
            end_memory = psutil.Process(os.getpid()).memory_info().rss
            
            sim_time = end_time - start_time
            memory_used = (end_memory - start_memory) / (1024**2)  # MB
            
            self.results[f'simulation_{steps}_steps'] = {
                'neurons': neuron_count,
                'steps': steps,
                'time_seconds': sim_time,
                'memory_mb': memory_used,
                'time_per_step': sim_time / steps,
                'time_per_neuron_per_step': sim_time / steps / neuron_count
            }
        
        return self.results
    
    def print_results(self):
        """Print benchmark results"""
        print("\n" + "="*60)
        print("NLM PERFORMANCE BENCHMARK RESULTS")
        print("="*60)
        
        print("\nSystem Information:")
        for key, value in self.system_info.items():
            print(f"  {key}: {value}")
        
        print("\nBrain Creation Benchmarks:")
        for benchmark_name, result in self.results.items():
            if benchmark_name.startswith('brain_creation'):
                print(f"  {result['neurons']} neurons:")
                print(f"    Time: {result['time_seconds']:.3f}s")
                print(f"    Memory: {result['memory_mb']:.1f}MB")
                print(f"    Time/neuron: {result['time_per_neuron']*1000:.3f}ms")
        
        print("\nSimulation Benchmarks:")
        for benchmark_name, result in self.results.items():
            if benchmark_name.startswith('simulation_'):
                print(f"  {result['steps']} steps:")
                print(f"    Time: {result['time_seconds']:.3f}s")
                print(f"    Memory: {result['memory_mb']:.1f}MB")
                print(f"    Time/step: {result['time_per_step']*1000:.3f}ms")
                print(f"    Time/neuron/step: {result['time_per_neuron_per_step']*1000000:.3f}μs")
        
        print("\n" + "="*60)

# Example usage
if __name__ == "__main__":
    print("NLM Documentation Generator")
    print("=" * 60)
    
    generator = NLMDocumentationGenerator()
    
    # Generate documentation
    output_dir = "/tmp/nlm_docs"
    generator.generate_documentation(output_dir)
    
    print(f"Documentation generated in {output_dir}")
    print("\nNext Steps:")
    print("1. Review generated documentation")
    print("2. Add more detailed examples")
    print("3. Create API reference documentation")
    print("4. Add troubleshooting guide")
    print("5. Create performance optimization guide")
    print("6. Generate installation instructions")
    
    # Run performance benchmarks
    print("\nRunning performance benchmarks...")
    benchmark = PerformanceBenchmark()
    system_info = benchmark.get_system_info()
    
    # Create benchmarks
    brain_results = benchmark.benchmark_brain_creation()
    sim_results = benchmark.benchmark_simulation()
    
    # Print results
    benchmark.print_results()
    
    print("\n✅ NLM Documentation Generator Complete!")
