#!/bin/bash
# NLM Demo Runner
# Comprehensive demo runner for the Neural Learning Machine project

set -e  # Exit on any error

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_demo() {
    echo -e "${BLUE}[DEMO]${NC} $1"
}

log_tutorial() {
    echo -e "${PURPLE}[TUTORIAL]${NC} $1"
}

# Check if Python is available and module is installed
check_python_environment() {
    log_info "Checking Python environment..."
    
    if ! command -v python3 &> /dev/null; then
        log_error "Python 3 is required but not installed"
        exit 1
    fi
    
    # Check if pynlm module is available
    if ! python3 -c "import pynlm" 2>/dev/null; then
        log_error "pynlm Python module is not installed"
        log_info "Please install it with: pip install pynlm"
        exit 1
    fi
    
    log_success "Python environment is ready"
}

# Run simple brain demo
run_simple_brain_demo() {
    log_demo "=== Simple Brain Demo ==="
    log_tutorial "This demo shows a basic neural network with just neurons and synapses."
    
    python3 -c "
import pynlm

print('Creating NLM brain...')
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f'Brain created with {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses')

print('Running simulation...')
for i in range(10):
    brain.step(i)
    if i % 3 == 0:
        print(f'Step {i}: {brain.getFiringNeuronCount()} firing neurons, {brain.getTotalSpikeCount()} total spikes')

print(f'Final firing rate: {brain.getAverageFiringRate():.2f}')
print('Simple brain demo completed successfully!')
"
}

# Run agent-world demo
run_agent_world_demo() {
    log_demo "=== Agent-World Demo ==="
    log_tutorial "This demo shows an agent interacting with a world environment."
    
    python3 -c "
import pynlm

print('Creating brain...')
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print('Creating agent...')
agent = pynlm.createAgentBrain(brain)

print('Creating world...')
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
world.reset()
world.setAgentStart(5.0, 5.0)

print('Initializing agent with world...')
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print('Running agent simulation...')
for step in range(50):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 10 == 0:
        print(f'Step {step}: Firing={brain.getFiringNeuronCount()}, Reward={agent.getNeuromodulationLevel():.2f}, Curiosity={agent.getCuriosityLevel():.2f}')

print(f'Agent simulation completed!')
print(f'Final firing rate: {brain.getAverageFiringRate():.2f}')
print(f'Total spikes: {brain.getTotalSpikeCount()}')
"
}

# Run learning demo
run_learning_demo() {
    log_demo "=== Learning Demo ==="
    log_tutorial "This demo shows how the brain learns through experience and plasticity."
    
    python3 -c "
import pynlm

print('Setting up learning environment...')
config = pynlm.createDefaultConfig()
# Configure for learning
config.set('brain.neuron_count', 200)
config.set('brain.connection_probability', 0.05)
config.set('plasticity.stdp.enable', True)
config.set('plasticity.hebbian.enable', True)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(pynlm.createSimpleWorld())

# Configure world
world = agent.getBrain().getRegion(1).getParent().getWorld()
if not world:
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()

agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print('Running learning simulation...')
initial_spikes = brain.getTotalSpikeCount()

for step in range(200):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f'Step {step}: Spikes={brain.getTotalSpikeCount()-initial_spikes}, Firing={brain.getFiringNeuronCount()}, Reward={agent.getNeuromodulationLevel():.2f}')

print(f'Learning simulation completed!')
print(f'Total spikes generated: {brain.getTotalSpikeCount()-initial_spikes}')
print(f'Average firing rate: {brain.getAverageFiringRate():.2f}')
print(f'Final neuromodulation level: {agent.getNeuromodulationLevel():.2f}')
"
}

# Run comparison demo
run_comparison_demo() {
    log_demo "=== Learning Comparison Demo ==="
    log_tutorial "This demo compares different learning configurations."
    
    python3 -c "
import pynlm

print('Running comparison of learning configurations...')

# Configuration 1: Basic
config1 = pynlm.createDefaultConfig()
brain1 = pynlm.createBrain(config1)
brain1.initialize()

# Configuration 2: Enhanced learning
config2 = pynlm.createDefaultConfig()
config2.set('brain.neuron_count', 300)
config2.set('plasticity.stdp.learning_rate', 0.002)
config2.set('neuromod.dopamine.scale', 2.0)
brain2 = pynlm.createBrain(config2)
brain2.initialize()

print('Comparing configurations...')
for step in range(100):
    # Run both simulations
    brain1.step(step)
    brain2.step(step)
    
    if step % 25 == 0:
        print(f'Step {step}: Basic={brain1.getAverageFiringRate():.2f}, Enhanced={brain2.getAverageFiringRate():.2f}, Spikes-Basic={brain1.getTotalSpikeCount()}, Spikes-Enhanced={brain2.getTotalSpikeCount()}')

print(f'Comparison completed!')
print(f'Basic config average firing rate: {brain1.getAverageFiringRate():.2f}')
print(f'Enhanced config average firing rate: {brain2.getAverageFiringRate():.2f}')
print(f'Basic config total spikes: {brain1.getTotalSpikeCount()}')
print(f'Enhanced config total spikes: {brain2.getTotalSpikeCount()}')
"
}

# Run memory demo
run_memory_demo() {
    log_demo "=== Working Memory Demo ==="
    log_tutorial "This demo shows working memory functionality."
    
    python3 -c "
import pynlm

print('Testing working memory functionality...')

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

wm = brain.getWorkingMemory()
if wm:
    print('Working memory available!')
    
    # Test memory operations
    wm.store([0.5, 0.8, 0.3], 1.0)
    retrieved = wm.retrieve()
    print(f'Memory store/retrieve test: stored and retrieved {len(retrieved)} values')
    
    # Test capacity
    original_capacity = wm.getCapacity()
    print(f'Memory capacity: {original_capacity}')
    
    # Test update
    wm.update(0.1)
    print('Memory update completed')
    
    # Test clear
    wm.clear()
    print('Memory cleared')
    
    print('Working memory demo completed successfully!')
else:
    print('Working memory not available')
"
}

# Run attention demo
run_attention_demo() {
    log_demo "=== Attention Demo ==="
    log_tutorial "This demo shows attention and selection functionality."
    
    python3 -c "
import pynlm

print('Testing attention functionality...')

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

attn = brain.getAttentionalSelection()
if attn:
    print('Attentional selection available!')
    
    # Test attention setup
    attn.initialize(brain)
    
    # Test focus on region
    region_id = brain.addRegion('test_region')
    attn.focusOnRegion(region_id)
    print('Focused attention on region')
    
    # Test attention update
    attn.update(0.1)
    print('Attention update completed')
    
    # Test winner selection
    competitors = [1, 2, 3, 4, 5]  # Mock neuron IDs
    winners = attn.processCompetition(competitors)
    print(f'Competition resulted in {len(winners)} winners')
    
    print('Attention demo completed successfully!')
else:
    print('Attentional selection not available')
"
}

# Show Python API help
show_python_api() {
    log_demo "=== NLM Python API Overview ==="
    
    cat << EOF
NLM Python API Overview
=======================

Core Classes:
  pynlm.createDefaultConfig()          - Create default configuration
  pynlm.createBrain(config)            - Create neural network brain
  pynlm.createSimpleWorld()            - Create simulation world
  pynlm.createAgentBrain(brain)        - Create agent interface

Brain Class Methods:
  brain.initialize()                   - Initialize brain
  brain.step(step)                     - Run simulation step
  brain.getTotalNeuronCount()          - Get total neurons
  brain.getTotalSynapseCount()         - Get total synapses
  brain.getFiringNeuronCount()         - Get firing neurons
  brain.getAverageFiringRate()         - Get average firing rate
  brain.getTotalSpikeCount()           - Get total spikes
  brain.getDevelopmentalStage()       - Get developmental stage

AgentBrain Class Methods:
  agent.initialize(world)              - Initialize with world
  agent.processSensoryInput(percept)   - Process sensory input
  agent.decodeMotorCommand()          - Get motor command
  agent.applyRewardModulation(reward, predicted) - Apply reward
  agent.enableRewardModulation(bool)  - Enable/disable reward modulation

World Class Methods:
  world.configure(width, height, visionWidth, visionHeight) - Configure world
  world.update(timestep)               - Update world state
  world.applyMotorCommand(cmd, time)   - Apply action
  world.getSensoryPercept()            - Get sensory input

Working Memory Methods (if available):
  wm.store(pattern, strength)           - Store pattern
  wm.retrieve()                        - Retrieve memory
  wm.update(dt)                        - Update memory
  wm.clear()                            - Clear memory

Attention Methods (if available):
  attn.initialize(brain)               - Initialize attention
  attn.focusOnRegion(region)           - Focus on region
  attn.processCompetition(competitors) - Process competition
  attn.update(dt)                       - Update attention

Configuration Options:
  brain.neuron_count=1000              - Number of neurons
  brain.synapse_density=0.1            - Connection probability
  plasticity.stdp.enable=true          - Enable STDP
  neuromod.curiosity.enable=true       - Enable curiosity

Example Usage:
  import pynlm
  config = pynlm.createDefaultConfig()
  brain = pynlm.createBrain(config)
  brain.initialize()
  for step in range(100):
      brain.step(step)

Press Enter to continue...
EOF
}

# Show advanced features
show_advanced_features() {
    log_demo "=== Advanced NLM Features ==="
    
    cat << EOF
Advanced NLM Features
=======================

1. Neuromodulation Systems:
   - Reward modulation (dopamine-like)
   - Curiosity-driven exploration
   - Novelty detection
   - Prediction error signaling

2. Memory Systems:
   - Working memory (persistent activity)
   - Episodic memory (experience storage)
   - Associative memory (pattern completion)
   - Semantic memory (knowledge representation)

3. Cognitive Systems:
   - Attention (competitive selection)
   - Planning (predictive action selection)
   - Concept formation (pattern discovery)
   - Self-model (sensorimotor awareness)

4. Plasticity Mechanisms:
   - Spike-Timing-Dependent Plasticity (STDP)
   - Hebbian learning
   - Structural plasticity (synaptogenesis/pruning)
   - Reward-modulated plasticity

5. Development:
   - Developmental stages
   - Critical periods
   - Maturation and aging
   - Experience-dependent development

6. Integration Features:
   - Sensorimotor loop
   - Goal-directed behavior
   - Social learning
   - Sleep/rest cycles

To use advanced features:
   - Configure specific parameters
   - Enable subsystems with enable* methods
   - Use get* methods for state information
   - Call update/development methods regularly

Press Enter to continue...
EOF
}

# Main demo runner
main() {
    echo "========================================"
    echo "NLM Demo Runner - Neural Learning Machine"
    echo "========================================"
    echo
    
    check_python_environment
    
    echo "Available Demos:"
    echo "  1. simple           - Simple brain demonstration"
    echo "  2. agent_world      - Agent and world interaction"
    echo "  3. learning         - Learning and plasticity"
    echo "  4. comparison      - Compare learning configurations"
    echo "  5. memory          - Working memory demonstration"
    echo "  6. attention       - Attention and selection"
    echo "  7. python_api      - Show Python API reference"
    echo "  8. advanced        - Advanced features overview"
    echo "  9. all            - Run all demos"
    echo "  help             - Show this help message"
    echo
    
    read -p "Enter demo number (1-9) or 'help': " choice
    
    case $choice in
        1) run_simple_brain_demo ;;
        2) run_agent_world_demo ;;
        3) run_learning_demo ;;
        4) run_comparison_demo ;;
        5) run_memory_demo ;;
        6) run_attention_demo ;;
        7) show_python_api ;;
        8) show_advanced_features ;;
        9) 
            run_simple_brain_demo
            echo ""
            run_agent_world_demo
            echo ""
            run_learning_demo
            echo ""
            run_memory_demo
            echo ""
            run_attention_demo
            log_success "All demos completed!"
            ;;
        help|--help|-h)
            echo "========================================"
            echo "NLM Demo Runner - Available Commands"
            echo "========================================"
            echo
            echo "Simple demos (quick demonstrations):"
            echo "  simple           - Basic neural network"
            echo "  agent_world      - Agent-environment interaction"
            echo "  learning         - Learning and adaptation"
            echo "  memory           - Working memory"
            echo "  attention        - Attention mechanisms"
            echo
            echo "Comparison demos (comparative analysis):"
            echo "  comparison      - Compare learning configurations"
            echo
            echo "Reference demos (documentation):"
            echo "  python_api      - Python API reference"
            echo "  advanced        - Advanced features overview"
            echo
            echo "Comprehensive demos:"
            echo "  all             - Run all demos"
            echo
            echo "Utilities:"
            echo "  help            - Show this help"
            ;;
        *)
            log_error "Unknown demo: $choice"
            exit 1
            ;;
    esac
    
    log_success "Demo completed successfully!"
}

# Run main function with all arguments
main "$@"
