# Brain region implementation

class Region:
    """Brain region containing neurons."""
    
    def __init__(self, region_id: int):
        self._region_id = region_id
        self._neurons = []
    
    @property
    def region_id(self) -> int:
        return self._region_id
    
    def add_neuron(self, neuron) -> None:
        """Add a neuron to this region."""
        self._neurons.append(neuron)
    
    def get_all_neurons(self) -> list:
        """Get all neurons in this region."""
        return self._neurons.copy()
    
    def get_synapses(self) -> list:
        """Get all synapses in this region (mock implementation)."""
        return []