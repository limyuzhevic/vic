# Fix the brain.py file
import re

# Read the brain.py file
with open('pynlm/brain.py', 'r') as f:
    content = f.read()

# Fix the _initialize_subsystems method
old_method = '''def _initialize_subsystems(self):
        """Initialize all subsystems with configuration."""
        # Create memory system
        self._memory = MemorySystem(self._config)
        
        # Create plasticity rules
        self._plasticity = PlasticityRule(self._config)
        
        # Create neuromodulation system
        self._neuromodulation = Neuromodulator(self._config)
        
        # Create cognitive system
        self._cognition = CognitiveSystem(self._config)
        
        # Create prediction system
        self._prediction = PredictionSystem(self._config)
        
        # Create development system
        self._development = DevelopmentSystem(self._config)
        
        # Initialize visualizer
        self._visualizer = Visualizer(self._config)'''

new_method = '''def _initialize_subsystems(self):
        """Initialize all subsystems with configuration."""
        # Create memory system with capacity from config
        if hasattr(self._config, 'get_or'):
            capacity = self._config.get_or('working_memory_capacity', 100)
        else:
            capacity = 100
        self._memory = MemorySystem(capacity=capacity)
        
        # Create plasticity rules
        self._plasticity = PlasticityRule()
        
        # Create neuromodulation system
        self._neuromodulation = Neuromodulator()
        
        # Create cognitive system
        self._cognition = CognitiveSystem()
        
        # Create prediction system
        self._prediction = PredictionSystem()
        
        # Create development system
        self._development = DevelopmentSystem()
        
        # Initialize visualizer
        self._visualizer = Visualizer()'''

# Replace the method
fixed_content = content.replace(old_method, new_method)

# Write the fixed content back
with open('pynlm/brain.py', 'w') as f:
    f.write(fixed_content)

print("Fixed _initialize_subsystems method in brain.py")

# Test the fix
import pynlm

print("Testing fixed brain...")

# Create config
config = pynlm.create_default_config()
print("Config created")

# Create brain with config
brain = pynlm.create_brain(config)
print("Brain created")

# Initialize brain
brain.initialize()
print("Brain initialized")

# Test basic functionality
stats = brain.get_stats()
print(f"Stats: {stats}")

print("All tests passed with fixed brain.py!")
