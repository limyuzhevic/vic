#!/bin/bash

# NLM Code Generation Script
# Usage: ./generate-code.sh [options]

# Exit on error
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_info() {
    echo -e "${GREEN}[GENERATOR]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[GENERATOR-WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[GENERATOR-ERROR]${NC} $1"
}

print_header() {
    echo -e "${BLUE}[GENERATOR]${NC} $1"
}

# Check dependencies
check_dependencies() {
    print_info "Checking code generation dependencies..."

    local missing=()

    if ! command -v python3 &> /dev/null; then
        missing+=("Python 3")
    fi

    if ! command -v git &> /dev/null; then
        missing+=("Git")
    fi

    if [ ${#missing[@]} -ne 0 ]; then
        print_error "Missing dependencies: ${missing[*]}"
        exit 1
    fi

    print_info "All dependencies satisfied."
}

# Generate component template
generate_component() {
    local component_name=$1
    local component_path=$2
    local component_type=$3

    print_header "Generating component: $component_name"

    # Create directory structure
    local base_dir="src/"
    if [ -n "$component_path" ]; then
        base_dir="$base_dir$component_path/"
    fi

    mkdir -p "$base_dir$component_name"
    cd "$base_dir$component_name" || exit 1

    # Generate .cpp file
    cat > "${component_name}.cpp" << EOF
// $component_name.cpp
// $component_type component implementation

#include "$component_name.h"

namespace nlm {

$component_name::$component_name() {
    // Initialize component $component_name
}

$component_name::~$component_name() {
    // Clean up component $component_name
}

void $component_name::initialize() {
    // TODO: Implement initialization
}

void $component_name::update(double dt) {
    // TODO: Implement update logic
}

void $component_name::shutdown() {
    // TODO: Implement shutdown
}

bool $component_name::isInitialized() const {
    return m_initialized;
}

} // namespace nlm
EOF

    # Generate .h file
    cat > "${component_name}.h" << EOF
// $component_name.h
// $component_type component interface

#ifndef NLM_${component_name^^}_H
#define NLM_${component_name^^}_H

#include <string>

namespace nlm {

class $component_name {
public:
    $component_name();
    ~$component_name();

    // Core component interface
    void initialize();
    void update(double dt);
    void shutdown();

    // Status queries
    bool isInitialized() const;
    std::string getName() const { return "${component_name}"; }

protected:
    bool m_initialized;
    std::string m_name;
};

} // namespace nlm

#endif // NLM_${component_name^^}_H
EOF

    # Generate test file
    cat > "test_${component_name}.cpp" << EOF
// test_${component_name}.cpp
// Tests for $component_name component

#include "$component_name.h"
#include <cassert>
#include <iostream>

namespace nlm {

void test_$component_name() {
    print_info("Testing $component_name component...");

    $component_name component;

    // Test initialization
    assert(!component.isInitialized());
    component.initialize();
    assert(component.isInitialized());

    // Test update
    component.update(0.1);

    // Test shutdown
    component.shutdown();
    assert(!component.isInitialized());

    print_info("All $component_name tests passed.");
}

} // namespace nlm

int main() {
    try {
        nlm::test_$component_name();
        std::cout << "All tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
EOF

    # Generate CMake test entry
    cat > "CMakeLists.txt" << EOF
# Test for $component_name component

add_executable(test_${component_name} test_${component_name}.cpp)
target_link_libraries(test_${component_name} PRIVATE nlm_core)
add_test(NAME test_${component_name} COMMAND test_${component_name})
EOF

    cd - > /dev/null || exit 1

    print_info "Component $component_name generated successfully."
}

# Generate Python module template
generate_python_module() {
    local module_name=$1
    local module_path=$2

    print_header "Generating Python module: $module_name"

    # Create directory structure
    if [ -n "$module_path" ]; then
        mkdir -p "python/pynlm/$module_path"
    else
        mkdir -p "python/pynlm"
    fi

    # Generate __init__.py
    cat > "python/pynlm/__init__.py" << EOF
# pynlm/__init__.py
# NLM Python bindings package

from .core import *
from .memory import *
from .neuromodulation import *
from .sensory import *
from .motor import *
from .environment import *
from .simulation import *

__version__ = "0.1.0"
__author__ = "NLM Authors"

# Package metadata
__all__ = [
    'Brain',
    'Neuron',
    'Synapse',
    'WorkingMemory',
    'EpisodicMemory',
    'Reward',
    'Curiosity',
    'Vision',
    'Audio',
    'MotorSystem',
    'Environment',
    'Simulation',
    'create_brain',
    'create_neuron',
    'create_memory_system',
]

# Package-level functions
def create_brain(config=None):
    """Create a new brain simulation instance."""
    from .core import Brain
    return Brain(config)

def create_neuron(config=None):
    """Create a new neuron instance."""
    from .core import Neuron
    return Neuron(config)

def create_memory_system(type='working', config=None):
    """Create a memory system instance."""
    from .memory import WorkingMemory, EpisodicMemory
    if type == 'working':
        return WorkingMemory(config)
    elif type == 'episodic':
        return EpisodicMemory(config)
    else:
        raise ValueError(f"Unknown memory system type: {type}")
EOF

    # Generate core module
    cat > "python/pynlm/core.py" << EOF
# python/pynlm/core.py
# Core brain components

import numpy as np
from typing import Optional, Dict, Any, List

class Neuron:
    """NLM Neuron implementation."""

    def __init__(self, config: Optional[Dict[str, Any]] = None):
        self.config = config or {}
        self.v = 0.0  # Membrane potential
        self.refractory = 0.0
        self.spike_count = 0
        self.state = "resting"

    def update(self, dt: float, inputs: Optional[np.ndarray] = None) -> bool:
        """Update neuron state. Returns True if neuron spiked."""
        if self.refractory > 0:
            self.refractory -= dt
            return False

        # LIF neuron dynamics
        self.v += dt * (self.config.get('leak', -0.1) * (self.config.get('rest_potential', -65) - self.v))

        if inputs is not None:
            self.v += np.sum(inputs)

        if self.v >= self.config.get('threshold', -50):
            self.v = self.config.get('reset_potential', -65)
            self.refractory = self.config.get('refractory_period', 0.5)
            self.spike_count += 1
            return True

        return False

    def get_state(self) -> Dict[str, Any]:
        return {
            'v': self.v,
            'refractory': self.refractory,
            'spike_count': self.spike_count,
            'state': self.state
        }

    def set_state(self, state: Dict[str, Any]):
        self.v = state['v']
        self.refractory = state['refractory']
        self.spike_count = state['spike_count']
        self.state = state['state']

class Brain:
    """NLM Brain implementation."""

    def __init__(self, config: Optional[Dict[str, Any]] = None):
        self.config = config or {}
        self.neurons = []
        self.synapses = []
        self.state = "initialized"

    def add_neuron(self, neuron: Neuron):
        self.neurons.append(neuron)

    def add_synapse(self, pre_neuron: Neuron, post_neuron: Neuron, weight: float):
        self.synapses.append({
            'pre': pre_neuron,
            'post': post_neuron,
            'weight': weight
        })

    def update(self, dt: float) -> List[int]:
        """Update brain state. Returns list of spiked neuron indices."""
        spiked_neurons = []

        for i, neuron in enumerate(self.neurons):
            # Collect inputs from synapses
            inputs = []
            for synapse in self.synapses:
                if synapse['post'] is neuron:
                    # Check if presynaptic neuron spiked in previous time step
                    if synapse['pre'].refractory <= 0:
                        inputs.append(synapse['weight'])

            if neuron.update(dt, np.array(inputs)):
                spiked_neurons.append(i)

        return spiked_neurons

    def get_state(self) -> Dict[str, Any]:
        return {
            'neuron_count': len(self.neurons),
            'synapse_count': len(self.synapses),
            'neuron_states': [neuron.get_state() for neuron in self.neurons],
            'state': self.state
        }

    def set_state(self, state: Dict[str, Any]):
        # Restore state
        self.neurons = []
        self.synapses = []
        self.state = state['state']

    def save(self, filepath: str):
        """Save brain state to file."""
        import json
        state = self.get_state()
        with open(filepath, 'w') as f:
            json.dump(state, f, indent=2)

    def load(self, filepath: str):
        """Load brain state from file."""
        import json
        with open(filepath, 'r') as f:
            state = json.load(f)
        self.set_state(state)
EOF

    # Generate memory module
    cat > "python/pynlm/memory.py" << EOF
# python/pynlm/memory.py
# Memory systems

import numpy as np
from typing import Optional, Dict, Any, List

class WorkingMemory:
    """Working memory system with persistent activity."""

    def __init__(self, config: Optional[Dict[str, Any]] = None):
        self.config = config or {}
        self.pattern = np.zeros(self.config.get('size', 100))
        self.activations = np.zeros_like(self.pattern)
        self.strength = 1.0
        self.active = False

    def activate_pattern(self, pattern: np.ndarray, strength: float = 1.0):
        """Activate a pattern in working memory."""
        self.pattern = pattern.copy()
        self.activations = pattern.copy() * strength
        self.strength = strength
        self.active = True

    def update(self, dt: float, decay_rate: float = 0.1):
        """Update working memory dynamics."""
        if self.active:
            self.activations *= np.exp(-decay_rate * dt)
            # Maintain minimum activation for persistence
            self.activations = np.maximum(self.activations, 0.01)

    def get_activations(self) -> np.ndarray:
        return self.activations.copy()

    def is_active(self) -> bool:
        return self.active

class EpisodicMemory:
    """Episodic memory with experience encoding and replay."""

    def __init__(self, config: Optional[Dict[str, Any]] = None):
        self.config = config or {}
        self.episodes = []
        self.current_episode = []
        self.replay_rate = self.config.get('replay_rate', 0.1)

    def encode_experience(self, experience: Dict[str, Any]):
        """Encode an experience into episodic memory."""
        self.current_episode.append(experience)

    def store_episode(self):
        """Store current episode permanently."""
        if self.current_episode:
            self.episodes.append(self.current_episode.copy())
            self.current_episode = []

    def replay_episode(self, episode_index: int, replay_rate: float = None):
        """Replay a stored episode."""
        if episode_index < len(self.episodes):
            rate = replay_rate or self.replay_rate
            for i, event in enumerate(self.episodes[episode_index]):
                # Replay logic would go here
                if i % int(1/rate) == 0:
                    yield event

    def get_recent_episodes(self, count: int) -> List[List[Dict[str, Any]]]:
        """Get most recent episodes."""
        return self.episodes[-count:]
EOF

    # Generate neuromodulation module
    cat > "python/pynlm/neuromodulation.py" << EOF
# python/pynlm/neuromodulation.py
# Neuromodulation systems

import numpy as np
from typing import Optional, Dict, Any, List

class Reward:
    """Reward prediction error signal."""

    def __init__(self, config: Optional[Dict[str, Any]] = None):
        self.config = config or {}
        self.rpe = 0.0
        self.learning_rate = self.config.get('learning_rate', 0.01)
        self.discount_factor = self.config.get('discount_factor', 0.9)
        self.value_estimate = 0.0

    def update(self, reward: float, next_value: float = 0.0):
        """Update reward prediction error."""
        self.rpe = reward + self.discount_factor * next_value - self.value_estimate
        self.value_estimate += self.learning_rate * self.rpe

    def get_rpe(self) -> float:
        return self.rpe

    def get_value_estimate(self) -> float:
        return self.value_estimate

class Curiosity:
    """Curiosity-driven exploration."""

    def __init__(self, config: Optional[Dict[str, Any]] = None):
        self.config = config or {}
        self.curiosity = 0.0
        self.exploration_rate = self.config.get('exploration_rate', 0.1)
        self.intrinsic_reward = 0.0

    def update(self, state_change: float, action: int):
        """Update curiosity signal."""
        # Intrinsic reward based on unexpected state changes
        self.curiosity = abs(state_change)
        self.intrinsic_reward = self.curiosity * self.exploration_rate

    def get_curiosity(self) -> float:
        return self.curiosity

    def get_intrinsic_reward(self) -> float:
        return self.intrinsic_reward
EOF

    # Generate sensory module
    cat > "python/pynlm/sensory.py" << EOF
# python/pynlm/sensory.py
# Sensory processing modules

import numpy as np
from typing import Optional, Dict, Any

class Vision:
    """Visual sensory input."""

    def __init__(self, config: Optional[Dict[str, Any]] = None):
        self.config = config or {}
        self.field_of_view = self.config.get('field_of_view', 90)
        self.resolution = self.config.get('resolution', (64, 64))
        self.input_field = np.zeros(self.resolution)

    def process_input(self, visual_input: np.ndarray):
        """Process visual input."""
        # Resize and normalize input
        self.input_field = visual_input.copy()
        # Apply sensory filtering
        self.input_field *= self.config.get('sensitivity', 1.0)

    def get_input(self) -> np.ndarray:
        return self.input_field.copy()

class Audio:
    """Auditory sensory input."""

    def __init__(self, config: Optional[Dict[str, Any]] = None):
        self.config = config or {}
        self.frequency_range = self.config.get('frequency_range', (100, 10000))
        self.input_signal = np.zeros(self.config.get('buffer_size', 1024))

    def process_input(self, audio_input: np.ndarray):
        """Process audio input."""
        self.input_signal = audio_input.copy()
        # Apply auditory filtering
        self.input_signal *= self.config.get('gain', 1.0)

    def get_input(self) -> np.ndarray:
        return self.input_signal.copy()
EOF

    # Generate CLI module
    cat > "python/pynlm/cli.py" << EOF
# python/pynlm/cli.py
# Command-line interface for NLM

import argparse
import sys
from typing import Optional
import json

from .core import Brain, Neuron
from .memory import WorkingMemory, EpisodicMemory
from .neuromodulation import Reward, Curiosity
from .sensory import Vision, Audio

__version__ = "0.1.0"

def create_brain(args):
    """Create and configure a brain."""
    config = {}
    if args.config:
        with open(args.config, 'r') as f:
            config = json.load(f)

    brain = Brain(config)
    print(f"Created brain with {args.neurons} neurons")
    return brain

def run_simulation(args):
    """Run a brain simulation."""
    brain = create_brain(args)

    print("Starting simulation...")
    for step in range(args.steps):
        spiked = brain.update(0.01)
        if spiked and args.verbose:
            print(f"Step {step}: {len(spiked)} neurons spiked")

        if step % 100 == 0 and args.verbose:
            state = brain.get_state()
            print(f"Step {step}: {state['neuron_count']} neurons")

    print("Simulation completed.")
    return brain

def save_brain(args):
    """Save brain state to file."""
    brain = create_brain(args)
    brain.save(args.output)
    print(f"Brain saved to {args.output}")

def load_brain(args):
    """Load brain state from file."""
    brain = Brain()
    brain.load(args.input)
    print(f"Brain loaded from {args.input}")
    return brain

def main():
    parser = argparse.ArgumentParser(description="NLM - Neural Learning Machine")
    parser.add_argument('--version', action='version', version=f'NLM {__version__}')

    subparsers = parser.add_subparsers(dest='command', help='Command to execute')

    # Create command
    create_parser = subparsers.add_parser('create', help='Create a brain')
    create_parser.add_argument('--config', help='Configuration file')
    create_parser.add_argument('--neurons', type=int, default=100, help='Number of neurons')

    # Simulate command
    sim_parser = subparsers.add_parser('simulate', help='Run simulation')
    sim_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    sim_parser.add_argument('--config', help='Configuration file')
    sim_parser.add_argument('--verbose', action='store_true', help='Verbose output')

    # Save command
    save_parser = subparsers.add_parser('save', help='Save brain state')
    save_parser.add_argument('--config', help='Configuration file')
    save_parser.add_argument('--output', required=True, help='Output file')

    # Load command
    load_parser = subparsers.add_parser('load', help='Load brain state')
    load_parser.add_argument('--input', required=True, help='Input file')

    args = parser.parse_args()

    if not args.command:
        parser.print_help()
        sys.exit(1)

    try:
        if args.command == 'create':
            create_brain(args)
        elif args.command == 'simulate':
            run_simulation(args)
        elif args.command == 'save':
            save_brain(args)
        elif args.command == 'load':
            load_brain(args)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()
EOF

    print_info "Python module $module_name generated successfully."
}

# Generate example script
generate_example() {
    local example_type=$1
    local output_file=$2

    print_header "Generating example: $example_type"

    case "$example_type" in
        "simple_brain")
            cat > "$output_file" << 'EOF'
#!/usr/bin/env python3
# examples/simple_brain.py
# Simple brain simulation example

import sys
sys.path.insert(0, 'python')

from pynlm import Brain, Neuron, create_brain
import numpy as np

def main():
    print("NLM Simple Brain Example")
    print("========================")

    # Create a simple brain
    brain = Brain({
        'neuron_count': 10,
        'threshold': -50,
        'rest_potential': -65,
        'reset_potential': -65,
        'leak': -0.1
    })

    # Add neurons
    for i in range(10):
        neuron = Neuron({
            'threshold': -50,
            'rest_potential': -65,
            'reset_potential': -65,
            'leak': -0.1
        })
        brain.add_neuron(neuron)

    # Run simulation
    print("Running simulation...")
    for step in range(100):
        spiked = brain.update(0.01)
        if spiked:
            print(f"Step {step}: {len(spiked)} neurons spiked")

    print("Simulation completed successfully!")

if __name__ == '__main__':
    main()
EOF
            ;;
        "agent_world")
            cat > "$output_file" << 'EOF'
#!/usr/bin/env python3
# examples/agent_world.py
# Complete agent-environment interaction example

import sys
sys.path.insert(0, 'python')

from pynlm import Brain, Neuron, WorkingMemory, Environment, Reward, Curiosity
import numpy as np

class Agent:
    def __init__(self, brain):
        self.brain = brain
        self.memory = WorkingMemory({'size': 50})
        self.reward = Reward()
        self.curiosity = Curiosity()

    def perceive(self, environment_state):
        # Store environment state in memory
        self.memory.activate_pattern(environment_state)

    def act(self):
        # Get brain spikes to determine action
        spiked = self.brain.update(0.01)
        return len(spiked)

    def learn(self, reward, next_state):
        # Update reward prediction error
        next_value = 0.0  # Simplified
        self.reward.update(reward, next_value)

        # Update curiosity
        state_change = np.linalg.norm(next_state - self.memory.get_activations())
        self.curiosity.update(state_change, 0)

def main():
    print("NLM Agent-Environment Example")
    print("=============================")

    # Create environment
    environment = Environment({
        'world_size': 100,
        'obstacle_density': 0.1
    })

    # Create agent brain
    brain = Brain()
    for i in range(20):
        neuron = Neuron()
        brain.add_neuron(neuron)

    # Create agent
    agent = Agent(brain)

    # Run episode
    for episode in range(10):
        print(f"Episode {episode + 1}")
        state = environment.reset()
        agent.perceive(state)

        for step in range(100):
            action = agent.act()
            next_state, reward, done = environment.step(action)
            agent.learn(reward, next_state)

            if done:
                break

        print(f"  Episode {episode + 1} completed")

    print("Agent-environment simulation completed!")

if __name__ == '__main__':
    main()
EOF
            ;;
        "plasticity_experiment")
            cat > "$output_file" << 'EOF'
#!/usr/bin/env python3
# examples/plasticity_experiment.py
# Plasticity learning experiment example

import sys
sys.path.insert(0, 'python')

from pynlm import Brain, Neuron
import numpy as np
import matplotlib.pyplot as plt

def run_plasticity_experiment():
    print("NLM Plasticity Experiment")
    print("=========================")

    # Create brain with plasticity rules
    brain = Brain({
        'neuron_count': 50,
        'learning_rate': 0.01,
        'plasticity_strength': 1.0
    })

    # Initialize neurons with random connections
    neurons = []
    for i in range(50):
        neuron = Neuron({
            'threshold': -50 + np.random.randn() * 10,
            'rest_potential': -65,
            'reset_potential': -65,
            'learning_rate': 0.01
        })
        neurons.append(neuron)
        brain.add_neuron(neuron)

    # Create random synaptic connections
    for i in range(50):
        for j in range(i + 1, 50):
            if np.random.rand() < 0.1:  # 10% connection probability
                weight = np.random.randn() * 0.1
                # In a real implementation, we would add synapse connections

    # Run learning experiment
    learning_rates = []
    spike_counts = []

    for episode in range(100):
        # Reset neurons
        for neuron in neurons:
            neuron.v = neuron.config.get('rest_potential', -65)

        # Run simulation
        for step in range(100):
            spiked = brain.update(0.01)

        # Record statistics
        total_spikes = sum(neuron.spike_count for neuron in neurons)
        spike_counts.append(total_spikes)

        # Calculate average learning rate (simplified)
        avg_rate = np.mean([n.config.get('learning_rate', 0.01) for n in neurons])
        learning_rates.append(avg_rate)

        if episode % 20 == 0:
            print(f"Episode {episode}: {total_spikes} spikes, rate {avg_rate:.4f}")

    # Plot results
    plt.figure(figsize=(12, 5))

    plt.subplot(1, 2, 1)
    plt.plot(spike_counts)
    plt.title('Spike Count Over Time')
    plt.xlabel('Episode')
    plt.ylabel('Total Spikes')

    plt.subplot(1, 2, 2)
    plt.plot(learning_rates)
    plt.title('Learning Rate Over Time')
    plt.xlabel('Episode')
    plt.ylabel('Average Learning Rate')

    plt.tight_layout()
    plt.savefig('plasticity_experiment.png')
    print("Results saved to plasticity_experiment.png")

    print("Plasticity experiment completed!")

if __name__ == '__main__':
    run_plasticity_experiment()
EOF
            ;;
        *)
            print_error "Unknown example type: $example_type"
            return
            ;;
    esac

    print_info "Example $example_type generated successfully."
}

# Main function
main() {
    echo "NLM Code Generation Script"
    echo "=========================="
    echo

    if [ $# -lt 1 ]; then
        echo "Usage: $0 <command> [options]"
        echo ""
        echo "Commands:"
        echo "  component <name> [path] [type]    Generate component template"
        echo "  python-module <name> [path]        Generate Python module template"
        echo "  example <type> <output>             Generate example script"
        echo ""
        echo "Examples:"
        echo "  $0 component neural_population brain/neural"
        echo "  $0 python-module pynlm core"
        echo "  $0 example simple_brain examples/simple_brain.py"
        exit 1
    fi

    command=$1
    shift

    case "$command" in
        "component")
            check_dependencies
            if [ $# -lt 1 ]; then
                print_error "Component name required"
                exit 1
            fi
            component_name=$1
            shift
            component_path=${1:-""}
            component_type=${1:-"general"}
            generate_component "$component_name" "$component_path" "$component_type"
            ;;
        "python-module")
            check_dependencies
            if [ $# -lt 1 ]; then
                print_error "Module name required"
                exit 1
            fi
            module_name=$1
            shift
            module_path=${1:-""}
            generate_python_module "$module_name" "$module_path"
            ;;
        "example")
            check_dependencies
            if [ $# -lt 2 ]; then
                print_error "Example type and output file required"
                exit 1
            fi
            example_type=$1
            output_file=$2
            generate_example "$example_type" "$output_file"
            ;;
        *)
            print_error "Unknown command: $command"
            exit 1
            ;;
    esac
}

# Run main function
main "$@"