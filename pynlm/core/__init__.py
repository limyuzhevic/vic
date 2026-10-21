"""Brain components - neurons, synapses, and the main brain simulation."""

from .brain.neuron import Neuron
from .brain.synapse import Synapse
from .brain.brain import Brain
from .brain.brain_factory import BrainFactory
from .config import Config, create_default_config
from .stdp import STDP
from .random_generator import RandomGenerator

# Export functions with original camelCase names for compatibility
createDefaultConfig = create_default_config
def createBrain(config=None, preset=None):
    """Create a brain with configuration or preset."""
    return BrainFactory.create(config, preset)

__all__ = ['Neuron', 'Synapse', 'Brain', 'BrainFactory', 'Config', 'STDP', 'RandomGenerator', 'create_default_config']