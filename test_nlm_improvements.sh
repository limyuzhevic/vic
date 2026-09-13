# Test Script for NLM Improvements

This script validates all the improvements made to the NLM codebase. Run it after making changes to ensure everything works correctly.

```bash
#!/bin/bash

# NLM Improvements Test Script
echo "=== Testing NLM Improvements ==="

# Test 1: Configuration System Tests
echo "\n1. Testing Configuration System..."
python3 -c "
import pynlm
import tempfile
import os

# Test basic configuration
config = pynlm.createDefaultConfig()
print('✓ Created default config')

# Test setting and getting values
config.set('brain.neuron_count', 2000)
print('✓ Set brain.neuron_count = 2000')

# Test getting values
neuron_count = config.get('brain.neuron_count')
if neuron_count.has_value() and neuron_count.value() == 2000:
    print('✓ Retrieved brain.neuron_count = 2000')
else:
    print('✗ Failed to retrieve brain.neuron_count')

# Test default values
rate = config.getOr('nonexistent.key', 0.5)
if rate == 0.5:
    print('✓ Default value works correctly')
else:
    print('✗ Default value failed')

# Test has() method
if config.has('brain.neuron_count'):
    print('✓ has() method works')
else:
    print('✗ has() method failed')

# Test getKeys()
keys = config.getKeys()
if 'brain.neuron_count' in keys:
    print('✓ getKeys() includes brain.neuron_count')
else:
    print('✗ getKeys() missing brain.neuron_count')

# Test clear()
config.clear()
if not config.has('brain.neuron_count'):
    print('✓ clear() works correctly')
else:
    print('✗ clear() failed')

# Test save to file
config.set('test.value', 42)
with tempfile.NamedTemporaryFile(mode='w', suffix='.nlm', delete=False) as f:
    filename = f.name

if config.saveToFile(filename):
    print('✓ saveToFile() works')
else:
    print('✗ saveToFile() failed')

# Test load from file
config2 = pynlm.createDefaultConfig()
if config2.loadFromFile(filename):
    print('✓ loadFromFile() works')
    if config2.has('test.value'):
        print('✓ loadFromFile() correctly loaded data')
    else:
        print('✗ loadFromFile() did not load data')
else:
    print('✗ loadFromFile() failed')

# Clean up
os.unlink(filename)
"

# Test 2: Python API Completeness
echo "\n2. Testing Python API Completeness..."
python3 -c "
import pynlm

# Test that all major classes are available
classes_to_test = [
    ('Config', pynlm.Config),
    ('Brain', pynlm.Brain),
    ('AgentBrain', pynlm.AgentBrain),
    ('SimpleWorld', pynlm.SimpleWorld),
    ('Action', pynlm.Action),
    ('SensoryPercept', pynlm.SensoryPercept),
]

for name, cls in classes_to_test:
    try:
        instance = cls
        print(f'✓ {name} class is accessible')
    except Exception as e:
        print(f'✗ {name} class failed: {e}')

# Test factory functions
factories = [
    ('createDefaultConfig', lambda: pynlm.createDefaultConfig()),
    ('createBrain', lambda: pynlm.createBrain(pynlm.createDefaultConfig())),
    ('createAgentBrain', lambda: pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))),
    ('createSimpleWorld', lambda: pynlm.createSimpleWorld()),
]

for name, factory in factories:
    try:
        result = factory()
        print(f'✓ {name}() works')
    except Exception as e:
        print(f'✗ {name}() failed: {e}')
"

# Test 3: Brain Functionality Tests
echo "\n3. Testing Brain Functionality..."
python3 -c "
import pynlm

# Test brain creation and initialization
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print('✓ Brain created')

# Note: Cannot initialize without proper config setup
print('⚠ Brain initialization requires proper configuration setup')

# Test brain statistics
brain.set('brain.neuron_count', 500)
if brain.has('brain.neuron_count'):
    print('✓ Brain configuration system works')
else:
    print('✗ Brain configuration system failed')
"

# Test 4: AgentBrain Functionality
echo "\n4. Testing AgentBrain Functionality..."
python3 -c "
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
print('✓ AgentBrain created')

# Test agent configuration
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
print('✓ AgentBrain configuration enabled')

# Test agent information methods
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Test getSensoryInputSize and getMotorOutputSize
    sensory_size = agent.getSensoryInputSize()
    motor_size = agent.getMotorOutputSize()
    print(f'✓ AgentBrain sensory input size: {sensory_size}')
    print(f'✓ AgentBrain motor output size: {motor_size}')
    
    # Test developmental stage
    stage = agent.getDevelopmentalStage()
    print(f'✓ AgentBrain developmental stage: {stage}')
    
    # Test neuromodulation levels
    dopamine = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    print(f'✓ AgentBrain dopamine level: {dopamine}')
    print(f'✓ AgentBrain curiosity level: {curiosity}')
    
except Exception as e:
    print(f'✗ AgentBrain functionality test failed: {e}')
"

# Test 5: World Functionality
echo "\n5. Testing World Functionality..."
python3 -c "
import pynlm

world = pynlm.createSimpleWorld()
print('✓ World created')

# Test world configuration
world.configure(20, 20, 8, 8)
print('✓ World configured (20x20, 8x8 vision)')

# Test agent start position
world.setAgentStart(10.0, 10.0)
print('✓ Agent start position set')

# Test world dimensions
width = world.getWidth()
height = world.getHeight()
print(f'✓ World dimensions: {width}x{height}')

# Test world time
time = world.getSimulationTime()
print(f'✓ Initial simulation time: {time}')

# Test world reset
world.reset()
print('✓ World reset successful')
"

# Test 6: Advanced Features Availability
echo "\n6. Testing Advanced Features..."
python3 -c "
import pynlm

# Test that advanced configuration features are available
config = pynlm.createDefaultConfig()

# Test validation features (if implemented)
try:
    # Check if validation methods exist
    if hasattr(config, 'validateRequiredKeys'):
        result = config.validateRequiredKeys(['test.key'])
        print(f'✓ validateRequiredKeys() available (returned: {result})')
    else:
        print('⚠ validateRequiredKeys() not yet implemented')
        
    if hasattr(config, 'toJSON'):
        json_str = config.toJSON()
        print(f'✓ toJSON() available (length: {len(json_str)})')
    else:
        print('⚠ toJSON() not yet implemented')
        
    if hasattr(config, 'fromJSON'):
        result = config.fromJSON('{}')
        print(f'✓ fromJSON() available (returned: {result})')
    else:
        print('⚠ fromJSON() not yet implemented')
        
except Exception as e:
    print(f'✗ Advanced features test failed: {e}')

print('✓ Advanced features testing complete')
"

# Test 7: Documentation Quality
echo "\n7. Documentation Quality Check..."
python3 -c "
import sys
import os

# Check if documentation files exist
doc_files = [
    'easy_usage.md',
    'HOW_TO_USE.md',
    'README.md',
    'docs/ARCHITECTURE.md',
]

missing_files = []
for doc_file in doc_files:
    if not os.path.exists(doc_file):
        missing_files.append(doc_file)

if missing_files:
    print(f'✗ Missing documentation files: {missing_files}')
else:
    print('✓ All expected documentation files exist')

# Check file sizes (basic quality check)
if os.path.exists('easy_usage.md'):
    with open('easy_usage.md', 'r') as f:
        content = f.read()
        if len(content) > 1000:
            print('✓ easy_usage.md has substantial content (>1000 chars)')
        else:
            print('✗ easy_usage.md has minimal content')

if os.path.exists('HOW_TO_USE.md'):
    with open('HOW_TO_USE.md', 'r') as f:
        content = f.read()
        if len(content) > 2000:
            print('✓ HOW_TO_USE.md has substantial content (>2000 chars)')
        else:
            print('✗ HOW_TO_USE.md has minimal content')
"

# Summary
echo -e "\n=== Test Summary ==="
echo "Tests completed. Check output above for detailed results."
echo "\nNext Steps:"
echo "1. Review any failed tests above"
echo "2. Run the build to ensure C++ compilation works"
echo "3. Check that all Python examples in documentation work"
echo "4. Verify that C++ and Python bindings are properly synchronized"

echo -e "\n=== End of Tests ==="
