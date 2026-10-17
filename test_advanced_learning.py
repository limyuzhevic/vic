# Test file for enhanced learning capabilities
# This demonstrates advanced plasticity mechanisms and learning rules

import numpy as np
import matplotlib.pyplot as plt
from nlm import createBrain, createDefaultConfig

class AdvancedLearningTest:
    """Test suite for advanced learning capabilities"""
    
    def __init__(self):
        self.config = createDefaultConfig()
        self.config.set("neuron_count", 1000)
        self.config.set("region_count", 2)
        self.config.set("connection_probability", 0.05f)
        
    def test_meta_learning(self):
        """Test meta-learning capabilities"""
        print("Testing Meta-Learning...")
        
        brain = createBrain(self.config)
        brain.initialize()
        
        # Run initial learning phase
        for step in range(100):
            brain.step(step)
        
        # Record initial weights
        initial_weights = []
        for region in brain.getRegions():
            for syn in region.getSynapses():
                initial_weights.append(syn.getWeight())
        
        # Apply learning rate adaptation
        for step in range(100, 200):
            # Meta-learning: adapt learning rate based on performance
            learning_rate = self.calculate_adaptive_learning_rate(step, brain)
            
            # Apply modified plasticity rules
            self.apply_meta_learning_rules(brain, learning_rate)
            brain.step(step)
        
        # Evaluate learning outcome
        final_weights = []
        for region in brain.getRegions():
            for syn in region.getSynapses():
                final_weights.append(syn.getWeight())
        
        # Calculate learning metrics
        weight_change = self.calculate_weight_change(initial_weights, final_weights)
        learning_efficiency = self.calculate_learning_efficiency(weight_change)
        
        print(f"Weight change: {weight_change:.4f}")
        print(f"Learning efficiency: {learning_efficiency:.4f}")
        
        return learning_efficiency
    
    def test_hierarchical_learning(self):
        """Test hierarchical temporal memory integration"""
        print("Testing Hierarchical Learning...")
        
        brain = createBrain(self.config)
        brain.initialize()
        
        # Create hierarchical structure
        self.create_hierarchical_connections(brain)
        
        # Run hierarchical learning
        for step in range(500):
            # Process at multiple temporal scales
            self.process_temporal_scales(brain, step)
            brain.step(step)
            
            # Apply hierarchical plasticity
            self.apply_hierarchical_plasticity(brain, step)
        
        # Evaluate hierarchical representation
        hierarchy_quality = self.evaluate_hierarchy_quality(brain)
        
        print(f"Hierarchy quality: {hierarchy_quality:.4f}")
        
        return hierarchy_quality
    
    def test_transfer_learning(self):
        """Test transfer learning between brain regions"""
        print("Testing Transfer Learning...")
        
        # Create two separate brain configurations
        config1 = createDefaultConfig()
        config1.set("neuron_count", 500)
        config1.set("region_count", 1)
        
        config2 = createDefaultConfig()
        config2.set("neuron_count", 500)
        config2.set("region_count", 1)
        
        # Initialize first brain for source learning
        source_brain = createBrain(config1)
        source_brain.initialize()
        
        # Train source brain
        for step in range(300):
            source_brain.step(step)
        
        # Transfer knowledge to second brain
        target_brain = createBrain(config2)
        target_brain.initialize()
        
        # Initialize target brain with transferred parameters
        self.transfer_parameters(source_brain, target_brain)
        
        # Continue training target brain
        for step in range(300, 600):
            target_brain.step(step)
        
        # Compare performance
        source_performance = self.evaluate_brain_performance(source_brain)
        target_performance = self.evaluate_brain_performance(target_brain)
        
        transfer_effectiveness = (target_performance - source_performance) / source_performance
        
        print(f"Source performance: {source_performance:.4f}")
        print(f"Target performance: {target_performance:.4f}")
        print(f"Transfer effectiveness: {transfer_effectiveness:.4f}")
        
        return transfer_effectiveness
    
    def test_neurogenesis(self):
        """Test neurogenesis (new neuron creation)"""
        print("Testing Neurogenesis...")
        
        brain = createBrain(self.config)
        brain.initialize()
        
        initial_neuron_count = brain.getTotalNeuronCount()
        
        # Run neurogenesis over time
        for step in range(500):
            # Apply neurogenesis rules
            self.apply_neurogenesis_rules(brain, step)
            brain.step(step)
        
        final_neuron_count = brain.getTotalNeuronCount()
        neuron_growth = final_neuron_count - initial_neuron_count
        growth_rate = neuron_growth / initial_neuron_count
        
        print(f"Initial neurons: {initial_neuron_count}")
        print(f"Final neurons: {final_neuron_count}")
        print(f"Neuron growth: {neuron_growth}")
        print(f"Growth rate: {growth_rate:.4f}")
        
        return growth_rate
    
    def calculate_adaptive_learning_rate(self, step, brain):
        """Calculate adaptive learning rate based on performance"""
        # Simple heuristic: decrease learning rate over time
        base_rate = 0.01f
        decay_factor = 0.995f
        return base_rate * (decay_factor ** (step / 10))
    
    def apply_meta_learning_rules(self, brain, learning_rate):
        """Apply meta-learning plasticity rules"""
        # Modify STDP parameters based on learning performance
        for region in brain.getRegions():
            for syn in region.getSynapses():
                # Adaptive STDP
                weight_change = syn.getWeight() * learning_rate * 0.001f
                new_weight = syn.getWeight() + weight_change
                syn.setWeight(new_weight)
    
    def create_hierarchical_connections(self, brain):
        """Create hierarchical connections between regions"""
        # Add extra connections from primary to secondary regions
        region_count = brain.getRegionCount()
        
        if region_count >= 2:
            # Get regions
            region1 = brain.getRegion(1)
            region2 = brain.getRegion(2)
            
            # Add hierarchical connections (simplified)
            # In real implementation, would add specific synapse connections
            pass
    
    def process_temporal_scales(self, brain, step):
        """Process inputs at different temporal scales"""
        # Short-term (fast) and long-term (slow) processing
        short_term_factor = 1.0f
        long_term_factor = 0.1f
        
        # Apply scale-specific processing
        if step % 10 == 0:  # Slow scale
            self.apply_slow_plasticity(brain, long_term_factor)
        else:  # Fast scale
            self.apply_fast_plasticity(brain, short_term_factor)
    
    def apply_hierarchical_plasticity(self, brain, step):
        """Apply hierarchical plasticity rules"""
        # Strengthen connections based on hierarchical importance
        for region in brain.getRegions():
            for syn in region.getSynapses():
                # Hierarchical importance factor
                importance = self.calculate_hierarchy_importance(syn, step)
                
                # Apply hierarchical plasticity
                weight_adjustment = importance * 0.0001f
                syn.setWeight(syn.getWeight() + weight_adjustment)
    
    def transfer_parameters(self, source_brain, target_brain):
        """Transfer parameters from source to target brain"""
        # Transfer synaptic weights and plasticity parameters
        for source_region in source_brain.getRegions():
            for source_syn in source_region.getSynapses():
                # Find corresponding synapse in target (simplified)
                for target_region in target_brain.getRegions():
                    for target_syn in target_region.getSynapses():
                        if (target_syn.getSourceNeuron().index() == source_syn.getSourceNeuron().index() and
                            target_syn.getDestinationNeuron().index() == source_syn.getDestinationNeuron().index()):
                            target_syn.setWeight(source_syn.getWeight())
                            target_syn.enablePlasticity(
                                source_syn.getPlasticityFlags().stdp,
                                source_syn.getPlasticityFlags().hebbian,
                                source_syn.getPlasticityFlags().structural
                            )
                            break
    
    def evaluate_hierarchy_quality(self, brain):
        """Evaluate quality of hierarchical representation"""
        # Simple metric: connection diversity and organization
        total_connections = 0
        for region in brain.getRegions():
            total_connections += region.getSynapseCount()
        
        # Calculate diversity metric
        if total_connections == 0:
            return 0.0f
        
        diversity_score = 1.0f / (1.0f + total_connections / 1000.0f)
        return diversity_score
    
    def evaluate_brain_performance(self, brain):
        """Evaluate overall brain performance"""
        firing_rate = brain.getAverageFiringRate()
        spike_count = brain.getTotalSpikeCount()
        
        # Combine metrics for performance score
        performance = firing_rate * 0.7f + spike_count * 0.3f / 1000.0f
        return performance
    
    def apply_neurogenesis_rules(self, brain, step):
        """Apply neurogenesis (new neuron creation) rules"""
        # Simple neurogenesis: create new neurons over time
        if step % 100 == 0 and step > 0:  # Every 100 steps
            # In real implementation, would create new neurons
            # For demo, we'll just log the concept
            pass
    
    def calculate_weight_change(self, initial_weights, final_weights):
        """Calculate average weight change"""
        if len(initial_weights) != len(final_weights):
            return 0.0f
        
        total_change = 0.0f
        for initial, final in zip(initial_weights, final_weights):
            total_change += abs(final - initial)
        
        return total_change / len(initial_weights)
    
    def calculate_learning_efficiency(self, weight_change):
        """Calculate learning efficiency metric"""
        # Normalize based on expected learning capacity
        max_expected_change = 0.5f
        efficiency = min(weight_change / max_expected_change, 1.0f)
        return efficiency
    
    def calculate_hierarchy_importance(self, synapse, step):
        """Calculate hierarchical importance of a synapse"""
        # Time-dependent importance
        importance = 1.0f / (1.0f + abs(step % 100 - 50) / 50.0f)
        return importance
    
    def apply_slow_plasticity(self, brain, factor):
        """Apply slow temporal scale plasticity"""
        # Long-term memory consolidation
        for region in brain.getRegions():
            for syn in region.getSynapses():
                # Stabilize weights
                weight_change = factor * (syn.getWeight() - 0.5f) * 0.0001f
                syn.setWeight(syn.getWeight() + weight_change)
    
    def apply_fast_plasticity(self, brain, factor):
        """Apply fast temporal scale plasticity"""
        # Short-term adaptation
        for region in brain.getRegions():
            for syn in region.getSynapses():
                # Rapid weight adjustment
                weight_change = factor * 0.001f
                syn.setWeight(syn.getWeight() + weight_change)

def run_advanced_learning_tests():
    """Run all advanced learning tests"""
    print("Advanced Learning Capabilities Test Suite")
    print("=" * 50)
    
    test_suite = AdvancedLearningTest()
    
    results = []
    
    try:
        results.append(("Meta-Learning", test_suite.test_meta_learning()))
    except Exception as e:
        print(f"Meta-Learning test failed: {e}")
        results.append(("Meta-Learning", 0.0f))
    
    try:
        results.append(("Hierarchical Learning", test_suite.test_hierarchical_learning()))
    except Exception as e:
        print(f"Hierarchical Learning test failed: {e}")
        results.append(("Hierarchical Learning", 0.0f))
    
    try:
        results.append(("Transfer Learning", test_suite.test_transfer_learning()))
    except Exception as e:
        print(f"Transfer Learning test failed: {e}")
        results.append(("Transfer Learning", 0.0f))
    
    try:
        results.append(("Neurogenesis", test_suite.test_neurogenesis()))
    except Exception as e:
        print(f"Neurogenesis test failed: {e}")
        results.append(("Neurogenesis", 0.0f))
    
    print("\n" + "=" * 50)
    print("Advanced Learning Test Results:")
    print("=" * 50)
    
    for test_name, score in results:
        print(f"{test_name}: {score:.4f}")
    
    # Calculate average score
    if results:
        avg_score = sum(score for _, score in results) / len(results)
        print(f"\nAverage Advanced Learning Score: {avg_score:.4f}")
        
        if avg_score > 0.5:
            print("✓ Advanced learning capabilities are well-developed")
        elif avg_score > 0.3:
            print("⚠ Advanced learning capabilities show promise")
        else:
            print("⚠ Advanced learning capabilities need improvement")
    
    return results

if __name__ == "__main__":
    try:
        run_advanced_learning_tests()
    except ImportError as e:
        print(f"Import error: {e}")
        print("Please ensure NLM Python bindings are properly installed.")
    except Exception as e:
        print(f"Test suite error: {e}")
        import traceback
        traceback.print_exc()
