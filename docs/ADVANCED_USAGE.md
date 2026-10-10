# Advanced Usage Guide for NLM

# Expert Commands and Configuration

This section provides advanced commands and configuration options for experienced NLM users who want to extend and customize the neural learning machine for complex experiments.

## 1. Advanced Brain Configuration

### Brain Region Management

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()

# Add multiple specialized regions
config.addRegion("sensory", nlm.RegionType.Sensory, 50)
config.addRegion("motor", nlm.RegionType.Motor, 30)
config.addRegion("association", nlm.RegionType.Associative, 100)
config.addRegion("internal", nlm.RegionType.Internal, 20)

# Set custom connection probabilities
config.set("connection_probability", 0.15)
config.set("inhibition_strength", 0.8)
config.set("excitation_strength", 1.2)

brain = pynlm.createBrain(config)
```

### High-Performance Brain Setup

```python
import pynlm

# Ultra-large brain configuration
def createHighPerformanceBrain(neuron_count=5000, regions=4):
    config = pynlm.createDefaultConfig()
    
    # Create specialized regions for different functions
    region_types = [
        ("vision_sensory", nlm.RegionType.Sensory, neuron_count // 3),
        ("motor_control", nlm.RegionType.Motor, neuron_count // 4),
        ("working_memory", nlm.RegionType.Associative, neuron_count // 4),
        ("reward_processing", nlm.RegionType.Internal, neuron_count // 12)
    ]
    
    for region_name, region_type, neuron_count in region_types:
        config.addRegion(region_name, region_type, neuron_count)
    
    # Advanced plasticity settings
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.025)
    config.set("structural_plasticity_rate", 0.0001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

# Usage
large_brain = createHighPerformanceBrain(10000)
```

## 2. Advanced Agent Configuration

### Custom Agent Brain with Specialized Features

```python
import pynlm

class AdvancedAgentBrain(pynlm.AgentBrain):
    def __init__(self, brain):
        super().__init__(brain)
        self.exploration_buffer = []
        self.reward_history = []
        self.curiosity_decay = 0.995
        self.adaptation_rate = 0.01
    
    def enableAdvancedLearning(self):
        """Enable advanced learning mechanisms"""
        self.enableRewardModulation(True)
        self.enableStructuralPlasticity(True)
        self.enableDevelopment(True)
        self.enableCuriosity(True)
        
        # Custom learning parameters
        self.setAdaptationRate(self.adaptation_rate)
        self.setExplorationBufferSize(1000)
    
    def adaptiveExploration(self, current_action, reward):
        """Adaptive exploration strategy based on reward history"""
        self.reward_history.append(reward)
        
        # Calculate exploration bonus based on reward variance
        if len(self.reward_history) >= 10:
            reward_std = self.calculate_reward_std()
            exploration_bonus = min(reward_std * 0.5, 1.0)
            self.adapt_curiosity(exploration_bonus)
        
        return self.exploration_buffer
    
    def calculate_reward_std(self):
        """Calculate standard deviation of recent rewards"""
        if len(self.reward_history) < 2:
            return 0.0
        
        mean_reward = sum(self.reward_history) / len(self.reward_history)
        variance = sum((r - mean_reward) ** 2 for r in self.reward_history) / len(self.reward_history)
        return variance ** 0.5
    
    def adapt_curiosity(self, bonus):
        """Adapt curiosity level based on exploration bonus"""
        self.curiosityLevel_ = min(1.0, self.curiosityLevel_ + bonus * self.adaptation_rate)

# Usage
advanced_agent = AdvancedAgentBrain(brain)
advanced_agent.enableAdvancedLearning()
```

## 3. Experimental Environment Configuration

### Complex Multi-Agent Environment

```python
import pynlm

class MultiAgentEnvironment:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.worlds = []
        self.agents = []
        self.objects = []
    
    def createCoordinatedEnvironment(self):
        """Create an environment with multiple interacting agents"""
        for i in range(3):  # 3 agents
            world = pynlm.createSimpleWorld()
            world.configure(
                width=self.width,
                height=self.height,
                visionWidth=8,
                visionHeight=8
            )
            world.reset()
            self.worlds.append(world)
            
            # Place agent at different starting positions
            start_x = (i * self.width // 3) + (self.width // 6)
            start_y = self.height // 2
            world.setAgentStart(start_x, start_y)
            
            # Create specialized brain for each agent
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 200)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            self.agents.append(agent)
        
        # Add environmental objects
        self.add_resources()
        self.add_hazards()
        
        return self
    
    def add_resources(self):
        """Add resources for agents to collect"""
        for i in range(5):
            x = (i * self.width // 5) + (self.width // 10)
            y = self.height // 3
            obj = pynlm.WorldObject(
                x=x, y=y, 
                type=nlm.WorldObjectType.Resource,
                value=10.0
            )
            self.objects.append(obj)
    
    def add_hazards(self):
        """Add hazards that damage agents"""
        for i in range(3):
            x = (i * self.width // 3) + (self.width // 6)
            y = self.height * 2 // 3
            obj = pynlm.WorldObject(
                x=x, y=y,
                type=nlm.WorldObjectType.Hazard,
                value=-5.0
            )
            self.objects.append(obj)
    
    def runCoordinatedSimulation(self, steps):
        """Run simulation with multiple agents"""
        for step in range(steps):
            for i, (world, agent) in enumerate(zip(self.worlds, self.agents)):
                # Update world
                world.update(0.1)
                
                # Get percept
                percept = world.getSensoryPercept()
                
                # Process with agent
                agent.processSensoryInput(percept)
                
                # Brain step
                agent.getBrain().step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Check for interactions with objects
                self.checkInteractions(world, agent)
                
                # Log state for analysis
                self.logState(step, i)
    
    def checkInteractions(self, world, agent):
        """Check agent interactions with world objects"""
        body = world.getAgentBody()
        
        for obj in self.objects:
            distance = ((body.x - obj.x) ** 2 + (body.y - obj.y) ** 2) ** 0.5
            if distance < obj.radius + 0.5:  # Agent collision radius
                if obj.type == nlm.WorldObjectType.Resource:
                    # Collect resource
                    agent.applyRewardModulation(1.0, 0.0)
                elif obj.type == nlm.WorldObjectType.Hazard:
                    # Take damage
                    agent.applyRewardModulation(-1.0, 0.0)
                    obj.active = False  # Remove hazard after first hit
    
    def logState(self, step, agent_id):
        """Log simulation state for analysis"""
        # Can be extended to write to file, database, or analysis tools
        pass

# Usage
env = MultiAgentEnvironment(20, 20)
advanced_env = env.createCoordinatedEnvironment()
advanced_env.runCoordinatedSimulation(1000)
```

## 4. Performance Optimization Commands

### Custom Performance Monitoring

```python
import pynlm
import time

class PerformanceMonitor:
    def __init__(self):
        self.metrics = {
            'steps_per_second': [],
            'neuron_activity': [],
            'memory_usage': [],
            'cpu_usage': []
        }
        self.start_time = 0
        self.step_count = 0
    
    def startMonitoring(self, brain):
        """Start performance monitoring for a brain"""
        self.start_time = time.time()
        self.step_count = 0
        
        # Set up performance callbacks
        brain.setPerformanceCallback(self.recordMetrics)
        return self
    
    def recordMetrics(self, step):
        """Record performance metrics during simulation"""
        current_time = time.time()
        elapsed = current_time - self.start_time
        
        # Calculate steps per second
        if elapsed > 0:
            steps_per_second = (self.step_count + 1) / elapsed
            self.metrics['steps_per_second'].append(steps_per_second)
        
        # Record brain statistics
        self.metrics['neuron_activity'].append({
            'firing': brain.getFiringNeuronCount(),
            'total': brain.getTotalNeuronCount(),
            'rate': brain.getAverageFiringRate()
        })
        
        self.step_count += 1
    
    def generateReport(self):
        """Generate performance report"""
        report = {
            'total_steps': self.step_count,
            'avg_steps_per_second': sum(self.metrics['steps_per_second']) / len(self.metrics['steps_per_second']) if self.metrics['steps_per_second'] else 0,
            'peak_performance': max(self.metrics['steps_per_second']) if self.metrics['steps_per_second'] else 0,
            'neuron_stats': self.metrics['neuron_activity'][-10:] if self.metrics['neuron_activity'] else []
        }
        
        return report

# Usage
monitor = PerformanceMonitor()
monitored_brain = monitor.startMonitoring(brain)

# Run simulation with monitoring
for step in range(1000):
    monitored_brain.step(step)

# Generate performance report
report = monitor.generateReport()
print(f"Performance Report: {report}")
```

## 5. Batch Processing and Parallelization

### Advanced Batch Processing

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor
import queue

class BatchProcessor:
    def __init__(self, num_workers=4):
        self.num_workers = num_workers
        self.work_queue = queue.Queue()
        self.result_queue = queue.Queue()
        self.executor = ThreadPoolExecutor(max_workers=num_workers)
    
    def processBatch(self, brains, world, steps_per_brain):
        """Process multiple brains in parallel"""
        # Submit batch jobs
        futures = []
        for i, brain in enumerate(brains):
            future = self.executor.submit(
                self.processSingleBrain,
                brain, world, steps_per_brain, i
            )
            futures.append(future)
        
        # Collect results
        results = []
        for future in futures:
            results.append(future.result())
        
        return results
    
    def processSingleBrain(self, brain, world, steps, brain_id):
        """Process a single brain with detailed logging"""
        brain.initialize()
        
        results = {
            'brain_id': brain_id,
            'steps_completed': 0,
            'final_spike_count': 0,
            'performance': 0.0,
            'success': True
        }
        
        try:
            for step in range(steps):
                # Run brain step
                brain.step(step)
                results['steps_completed'] += 1
                
                # Record final spike count
                if step == steps - 1:
                    results['final_spike_count'] = brain.getTotalSpikeCount()
        
        except Exception as e:
            results['success'] = False
            results['error'] = str(e)
        
        return results
    
    def getBatchStatistics(self, results):
        """Calculate batch processing statistics"""
        successful = [r for r in results if r['success']]
        
        if not successful:
            return {
                'total_brains': len(results),
                'successful': 0,
                'failed': len(results),
                'average_performance': 0.0
            }
        
        return {
            'total_brains': len(results),
            'successful': len(successful),
            'failed': len(results) - len(successful),
            'average_spike_count': sum(r['final_spike_count'] for r in successful) / len(successful),
            'total_steps_completed': sum(r['steps_completed'] for r in successful)
        }

# Usage with large-scale simulation
batch_processor = BatchProcessor(num_workers=8)

# Create multiple brains
brains = []
for i in range(10):
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    brains.append(pynlm.createBrain(config))

# Create shared world for batch processing
world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)

# Process batch
results = batch_processor.processBatch(brains, world, 100)
stats = batch_processor.getBatchStatistics(results)

print(f"Batch Processing Statistics: {stats}")
```

## 6. Advanced Configuration Files

### Custom Brain Configuration Templates

```python
import json
import pynlm

class ConfigurationTemplate:
    def __init__(self, template_name):
        self.template_name = template_name
        self.templates = {
            'research': {
                'neuron_count': 1000,
                'region_count': 5,
                'connection_probability': 0.12,
                'plasticity_rate': 0.8,
                'learning_enabled': True,
                'development_enabled': True
            },
            'production': {
                'neuron_count': 5000,
                'region_count': 10,
                'connection_probability': 0.08,
                'plasticity_rate': 0.6,
                'learning_enabled': True,
                'development_enabled': False,
                'performance_optimized': True
            },
            'experimental': {
                'neuron_count': 2000,
                'region_count': 8,
                'connection_probability': 0.15,
                'plasticity_rate': 1.0,
                'learning_enabled': True,
                'development_enabled': True,
                'curiosity_enabled': True,
                'novelty_detection': True
            }
        }
    
    def createConfig(self, template='research'):
        """Create configuration based on template"""
        if template not in self.templates:
            raise ValueError(f"Unknown template: {template}")
        
        template_config = self.templates[template]
        config = pynlm.createDefaultConfig()
        
        # Apply template settings
        config.set("neuron_count", template_config['neuron_count'])
        config.set("region_count", template_config['region_count'])
        config.set("connection_probability", template_config['connection_probability'])
        
        # Additional template-specific settings
        if template_config.get('plasticity_rate'):
            config.set("plasticity_rate", template_config['plasticity_rate'])
        
        if template_config.get('learning_enabled'):
            config.set("enable_learning", template_config['learning_enabled'])
        
        if template_config.get('development_enabled'):
            config.set("enable_development", template_config['development_enabled'])
        
        if template_config.get('curiosity_enabled'):
            config.set("enable_curiosity", template_config['curiosity_enabled'])
        
        if template_config.get('performance_optimized'):
            config.set("performance_optimized", True)
        
        if template_config.get('novelty_detection'):
            config.set("novelty_detection", True)
        
        return config
    
    def saveTemplate(self, template_name, config):
        """Save configuration as template for future use"""
        if template_name not in self.templates:
            self.templates[template_name] = {}
        
        # Extract relevant config values
        self.templates[template_name] = {
            'neuron_count': config.get("neuron_count", 1000),
            'region_count': config.get("region_count", 4),
            'connection_probability': config.get("connection_probability", 0.1),
            'plasticity_rate': config.get("plasticity_rate", 0.8),
            'learning_enabled': config.get("enable_learning", True),
            'development_enabled': config.get("enable_development", True),
            'curiosity_enabled': config.get("enable_curiosity", True)
        }
        
        # Save to file
        filename = f"nlm_template_{template_name}.json"
        with open(filename, 'w') as f:
            json.dump(self.templates[template_name], f, indent=2)
    
    def loadTemplate(self, filename):
        """Load configuration from file"""
        try:
            with open(filename, 'r') as f:
                loaded_config = json.load(f)
            
            template_name = filename.replace('nlm_template_', '').replace('.json', '')
            self.templates[template_name] = loaded_config
            
            return loaded_config
        except FileNotFoundError:
            print(f"Template file not found: {filename}")
            return None

# Usage
config_manager = ConfigurationTemplate("advanced")

# Create different configurations
research_config = config_manager.createConfig('research')
production_config = config_manager.createConfig('production')
experimental_config = config_manager.createConfig('experimental')

# Save custom template
config_manager.saveTemplate("my_custom_config", experimental_config)

# Load saved template
loaded_config = config_manager.loadTemplate("nlm_template_my_custom_config.json")
```

## 7. Debugging and Analysis Tools

### Advanced Debugging Utilities

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

class DebugTools:
    def __init__(self):
        self.debug_data = {
            'neuron_activities': [],
            'spike_patterns': [],
            'reward_history': [],
            'action_sequences': []
        }
    
    def enableDetailedLogging(self, brain):
        """Enable detailed logging for debugging"""
        brain.enableDebugLogging(True)
        brain.setLogLevel(nlm.LogLevel.DEBUG)
        return self
    
    def recordNeuronActivity(self, brain):
        """Record detailed neuron activity data"""
        # Record spike data
        spike_count = brain.getTotalSpikeCount()
        self.debug_data['spike_patterns'].append({
            'step': len(self.debug_data['spike_patterns']),
            'count': spike_count,
            'rate': spike_count / (len(self.debug_data['spike_patterns']) + 1)
        })
        
        # Record region activities
        for i, region in enumerate(brain.getRegions()):
            self.debug_data['neuron_activities'].append({
                'region_id': region.getId(),
                'neuron_count': region.getNeuronCount(),
                'active_count': region.getActiveNeuronCount()
            })
    
    def analyzeSpatiotemporalPatterns(self):
        """Analyze spatiotemporal patterns in neural activity"""
        if len(self.debug_data['spike_patterns']) < 10:
            return {"error": "Insufficient data for pattern analysis"}
        
        # Analyze spike rate trends
        spike_rates = [p['rate'] for p in self.debug_data['spike_patterns']]
        
        analysis = {
            'mean_rate': np.mean(spike_rates),
            'std_rate': np.std(spike_rates),
            'trend': self.calculateTrend(spike_rates),
            'variability': np.var(spike_rates),
            'stability': self.calculateStability(spike_rates)
        }
        
        return analysis
    
    def calculateTrend(self, data):
        """Calculate trend in time series data"""
        if len(data) < 2:
            return "insufficient_data"
        
        x = np.arange(len(data))
        y = np.array(data)
        
        # Linear regression
        coefficients = np.polyfit(x, y, 1)
        slope = coefficients[0]
        
        if abs(slope) < 0.01:
            return "stable"
        elif slope > 0:
            return "increasing"
        else:
            return "decreasing"
    
    def calculateStability(self, data):
        """Calculate stability of time series data"""
        if len(data) < 3:
            return "unstable"
        
        # Calculate coefficient of variation
        mean_val = np.mean(data)
        std_val = np.std(data)
        
        if mean_val == 0:
            return "undefined"
        
        cv = std_val / mean_val
        
        if cv < 0.1:
            return "stable"
        elif cv < 0.3:
            return "moderately_stable"
        else:
            return "unstable"
    
    def visualizeActivityPatterns(self, output_file='activity_patterns.png'):
        """Generate visualization of neural activity patterns"""
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Spike rate over time
        axes[0, 0].plot([p['step'] for p in self.debug_data['spike_patterns']],
                       [p['rate'] for p in self.debug_data['spike_patterns']])
        axes[0, 0].set_title('Spike Rate Over Time')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Rate (spikes/step)')
        
        # Activity distribution
        if self.debug_data['neuron_activities']:
            region_ids = [a['region_id'] for a in self.debug_data['neuron_activities']]
            active_counts = [a['active_count'] for a in self.debug_data['neuron_activities']]
            
            axes[0, 1].bar(region_ids, active_counts)
            axes[0, 1].set_title('Region Activity Distribution')
            axes[0, 1].set_xlabel('Region ID')
            axes[0, 1].set_ylabel('Active Neurons')
        
        # Pattern analysis plot
        if len(self.debug_data['spike_patterns']) >= 10:
            spike_rates = [p['rate'] for p in self.debug_data['spike_patterns']]
            axes[1, 0].hist(spike_rates, bins=20, alpha=0.7)
            axes[1, 0].set_title('Spike Rate Distribution')
            axes[1, 0].set_xlabel('Spike Rate')
            axes[1, 0].set_ylabel('Frequency')
        
        # Pattern stability plot
        if len(self.debug_data['spike_patterns']) >= 20:
            # Show last 50% of data
            start_idx = len(self.debug_data['spike_patterns']) // 2
            recent_rates = [p['rate'] for p in self.debug_data['spike_patterns'][start_idx:]]
            axes[1, 1].plot(recent_rates)
            axes[1, 1].set_title('Recent Activity Stability')
            axes[1, 1].set_xlabel('Recent Step')
            axes[1, 1].set_ylabel('Spike Rate')
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=150)
        plt.close()
    
    def exportDebugData(self, filename='debug_data.json'):
        """Export debug data for external analysis"""
        with open(filename, 'w') as f:
            json.dump(self.debug_data, f, indent=2)

# Usage
debugger = DebugTools()
debugger.enableDetailedLogging(brain)

# Run simulation with debugging
for step in range(100):
    brain.step(step)
    debugger.recordNeuronActivity(brain)

# Analyze patterns
analysis = debugger.analyzeSpatiotemporalPatterns()
print(f"Pattern Analysis: {analysis}")

# Generate visualization
debugger.visualizeActivityPatterns()

# Export data for further analysis
debugger.exportDebugData()
```

## 8. Python Integration Examples

### Advanced Python NLM Integration

```python
import pynlm
import numpy as np
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans

class AdvancedPythonIntegration:
    def __init__(self):
        self.data_processor = DataProcessor()
        self.pattern_analyzer = PatternAnalyzer()
        self.performance_optimizer = PerformanceOptimizer()
    
    def analyzeNeuralData(self, brain, window_size=100):
        """Analyze neural data using machine learning techniques"""
        # Collect neural activity data
        activity_data = self.extractNeuralFeatures(brain, window_size)
        
        # Apply dimensionality reduction
        pca = PCA(n_components=2)
        reduced_data = pca.fit_transform(activity_data)
        
        # Cluster activity patterns
        kmeans = KMeans(n_clusters=3, random_state=42)
        clusters = kmeans.fit_predict(reduced_data)
        
        # Analyze cluster characteristics
        cluster_analysis = self.analyzeClusters(reduced_data, clusters)
        
        return {
            'reduced_data': reduced_data,
            'clusters': clusters,
            'cluster_analysis': cluster_analysis,
            'pca_explained_variance': pca.explained_variance_ratio_
        }
    
    def extractNeuralFeatures(self, brain, window_size):
        """Extract features from neural activity"""
        features = []
        
        for step in range(window_size):
            # Get region statistics
            region_stats = []
            for region in brain.getRegions():
                region_stats.append({
                    'firing_rate': region.getFiringRate(),
                    'active_neurons': region.getActiveNeuronCount(),
                    'total_neurons': region.getNeuronCount()
                })
            
            # Add temporal features
            features.append({
                'step': step,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'region_stats': region_stats
            })
        
        return np.array([f['total_spikes'] for f in features])
    
    def analyzeClusters(self, reduced_data, clusters):
        """Analyze characteristics of identified clusters"""
        analysis = {}
        
        for cluster_id in np.unique(clusters):
            cluster_mask = clusters == cluster_id
            cluster_data = reduced_data[cluster_mask]
            
            analysis[f'cluster_{cluster_id}'] = {
                'size': np.sum(cluster_mask),
                'centroid': np.mean(cluster_data, axis=0),
                'spread': np.std(cluster_data, axis=0),
                'activity_level': np.mean(cluster_data[:, 0]) if cluster_data.shape[1] > 0 else 0
            }
        
        return analysis
    
    def realTimePatternRecognition(self, brain, threshold=0.7):
        """Real-time pattern recognition using neural activity"""
        # Extract current activity patterns
        current_features = self.extractNeuralFeatures(brain, 1)[0]
        
        # Compare with historical patterns
        historical_data = self.getHistoricalPatterns()
        
        if len(historical_data) < 10:
            return {
                'pattern': 'unknown',
                'confidence': 0.0,
                'similarity': 0.0
            }
        
        # Calculate similarity
        similarities = []
        for historical_pattern in historical_data:
            similarity = self.calculatePatternSimilarity(
                current_features, historical_pattern
            )
            similarities.append(similarity)
        
        # Find best match
        best_match_idx = np.argmax(similarities)
        best_similarity = similarities[best_match_idx]
        
        # Pattern recognition
        if best_similarity > threshold:
            pattern = self.identifyPattern(best_match_idx, historical_data)
            confidence = min(best_similarity / threshold, 1.0)
        else:
            pattern = 'novel'
            confidence = 0.0
        
        return {
            'pattern': pattern,
            'confidence': confidence,
            'similarity': best_similarity,
            'historical_matches': similarities
        }
    
    def calculatePatternSimilarity(self, features1, features2):
        """Calculate similarity between two activity patterns"""
        # Normalize features
        features1_norm = (features1 - np.mean(features1)) / (np.std(features1) + 1e-8)
        features2_norm = (features2 - np.mean(features2)) / (np.std(features2) + 1e-8)
        
        # Calculate cosine similarity
        similarity = np.dot(features1_norm, features2_norm) / (
            np.linalg.norm(features1_norm) * np.linalg.norm(features2_norm)
        )
        
        return (similarity + 1) / 2  # Convert to 0-1 range

class DataProcessor:
    def __init__(self):
        self.normalization_stats = {}
    
    def normalizeNeuralData(self, data, method='zscore'):
        """Normalize neural activity data"""
        if method not in self.normalization_stats:
            self.normalization_stats[method] = {
                'mean': np.mean(data),
                'std': np.std(data)
            }
        
        stats = self.normalization_stats[method]
        
        if method == 'zscore':
            return (data - stats['mean']) / stats['std']
        elif method == 'minmax':
            min_val = np.min(data)
            max_val = np.max(data)
            return (data - min_val) / (max_val - min_val)
        else:
            return data

# Usage with advanced Python integration
advanced_integration = AdvancedPythonIntegration()

# Run simulation with data collection
for step in range(200):
    brain.step(step)
    
    # Analyze data every 50 steps
    if step % 50 == 0:
        analysis = advanced_integration.analyzeNeuralData(brain)
        print(f"Step {step} Analysis: {analysis['cluster_analysis']}")
        
        # Real-time pattern recognition
        pattern = advanced_integration.realTimePatternRecognition(brain)
        print(f"Detected Pattern: {pattern}")
```

## 9. API Extension and Customization

### Custom API Extensions

```python
import pynlm
from flask import Flask, request, jsonify
import threading
import queue

class NLMAPIExtension:
    def __init__(self):
        self.app = Flask(__name__)
        self.brain = None
        self.simulation_queue = queue.Queue()
        self.results_queue = queue.Queue()
        self.setup_routes()
    
    def setup_routes(self):
        """Setup API routes for brain control"""
        
        @self.app.route('/api/brain/init', methods=['POST'])
        def initialize_brain():
            data = request.json
            config = self.createConfigFromData(data)
            self.brain = pynlm.createBrain(config)
            self.brain.initialize()
            return jsonify({'status': 'success', 'brain_id': id(self.brain)})
        
        @self.app.route('/api/brain/step', methods=['POST'])
        def simulate_step():
            if not self.brain:
                return jsonify({'error': 'Brain not initialized'}), 400
            
            try:
                step_data = request.json
                step = step_data.get('step', 0)
                
                # Run simulation step
                self.brain.step(step)
                
                # Get current statistics
                stats = {
                    'step': step,
                    'total_spikes': self.brain.getTotalSpikeCount(),
                    'firing_rate': self.brain.getAverageFiringRate(),
                    'active_neurons': self.brain.getFiringNeuronCount()
                }
                
                return jsonify({'status': 'success', 'stats': stats})
            
            except Exception as e:
                return jsonify({'error': str(e)}), 500
        
        @self.app.route('/api/brain/control', methods=['POST'])
        def control_brain():
            if not self.brain:
                return jsonify({'error': 'Brain not initialized'}), 400
            
            try:
                control_data = request.json
                action = control_data.get('action')
                
                if action == 'reset':
                    self.brain.reset()
                elif action == 'save':
                    filepath = control_data.get('filepath', 'brain_state.json')
                    self.brain.save(filepath)
                elif action == 'load':
                    filepath = control_data.get('filepath', 'brain_state.json')
                    self.brain.load(filepath)
                elif action == 'enable_features':
                    features = control_data.get('features', {})
                    self.applyFeatureSettings(features)
                
                return jsonify({'status': 'success'})
            
            except Exception as e:
                return jsonify({'error': str(e)}), 500
        
        @self.app.route('/api/brain/query', methods=['GET'])
        def query_brain():
            if not self.brain:
                return jsonify({'error': 'Brain not initialized'}), 400
            
            query_type = request.args.get('type', 'status')
            
            try:
                if query_type == 'status':
                    status = {
                        'neuron_count': self.brain.getTotalNeuronCount(),
                        'synapse_count': self.brain.getTotalSynapseCount(),
                        'developmental_stage': self.brain.getDevelopmentalStage(),
                        'firing_rate': self.brain.getAverageFiringRate()
                    }
                    return jsonify(status)
                
                elif query_type == 'regions':
                    regions = []
                    for region in self.brain.getRegions():
                        regions.append({
                            'id': region.getId(),
                            'neuron_count': region.getNeuronCount(),
                            'active_count': region.getActiveNeuronCount()
                        })
                    return jsonify({'regions': regions})
                
                elif query_type == 'performance':
                    performance = self.getPerformanceMetrics()
                    return jsonify(performance)
                
                else:
                    return jsonify({'error': f'Unknown query type: {query_type}'}), 400
            
            except Exception as e:
                return jsonify({'error': str(e)}), 500
        
        @self.app.route('/api/batch/simulate', methods=['POST'])
        def batch_simulation():
            try:
                batch_data = request.json
                num_brains = batch_data.get('num_brains', 1)
                steps_per_brain = batch_data.get('steps', 100)
                
                # Create multiple brains for batch simulation
                brains = []
                for i in range(num_brains):
                    config = pynlm.createDefaultConfig()
                    config.set("neuron_count", batch_data.get('neuron_count', 100))
                    brain = pynlm.createBrain(config)
                    brain.initialize()
                    brains.append(brain)
                
                # Run batch simulation
                results = []
                for i, brain in enumerate(brains):
                    for step in range(steps_per_brain):
                        brain.step(step)
                    
                    results.append({
                        'brain_id': i,
                        'total_spikes': brain.getTotalSpikeCount(),
                        'final_firing_rate': brain.getAverageFiringRate()
                    })
                
                return jsonify({
                    'status': 'success',
                    'results': results,
                    'batch_stats': self.calculateBatchStats(results)
                })
            
            except Exception as e:
                return jsonify({'error': str(e)}), 500
    
    def createConfigFromData(self, data):
        """Create configuration from API request data"""
        config = pynlm.createDefaultConfig()
        
        if 'neuron_count' in data:
            config.set("neuron_count", data['neuron_count'])
        if 'region_count' in data:
            config.set("region_count", data['region_count'])
        if 'connection_probability' in data:
            config.set("connection_probability", data['connection_probability'])
        
        return config
    
    def applyFeatureSettings(self, features):
        """Apply feature settings to the brain"""
        if features.get('reward_modulation'):
            # Would need access to agent brain to apply this
            pass
        if features.get('development'):
            # Would need access to agent brain to apply this
            pass
        if features.get('curiosity'):
            # Would need access to agent brain to apply this
            pass
    
    def getPerformanceMetrics(self):
        """Get performance metrics for monitoring"""
        if not self.brain:
            return {}
        
        # Calculate performance metrics
        metrics = {
            'neuron_density': self.brain.getTotalNeuronCount() / self.brain.getRegionCount(),
            'synapse_efficiency': self.brain.getTotalSynapseCount() / self.brain.getTotalNeuronCount(),
            'activity_level': self.brain.getAverageFiringRate()
        }
        
        return metrics
    
    def calculateBatchStats(self, results):
        """Calculate statistics for batch simulation results"""
        if not results:
            return {}
        
        total_spikes = sum(r['total_spikes'] for r in results)
        avg_firing_rate = sum(r['final_firing_rate'] for r in results) / len(results)
        
        return {
            'average_spikes': total_spikes / len(results),
            'average_firing_rate': avg_firing_rate,
            'total_simulation_steps': len(results) * 100  # Assuming 100 steps per brain
        }
    
    def start_server(self, host='0.0.0.0', port=5000):
        """Start the API server"""
        self.app.run(host=host, port=port, threaded=True)

# Usage with API extension
api_extension = NLMAPIExtension()

# Start API server in background thread
api_thread = threading.Thread(target=api_extension.start_server)
api_thread.daemon = True
api_thread.start()

print("NLM API Extension started on http://localhost:5000")
print("Available endpoints:")
print("  - POST /api/brain/init")
print("  - POST /api/brain/step")
print("  - POST /api/brain/control")
print("  - GET /api/brain/query")
print("  - POST /api/batch/simulate")
```

## 10. Future Enhancement Roadmap

### Planned Advanced Features

The following advanced features are planned for future development:

1. **Multi-Modal Sensory Processing**: Integration of visual, auditory, and tactile sensory modalities with advanced feature extraction

2. **Hierarchical Memory Systems**: Implementation of multiple memory systems (working, episodic, semantic, procedural) with sophisticated interaction mechanisms

3. **Social Learning and Imitation**: Development of mechanisms for learning from other agents and environmental patterns

4. **Metacognitive Control**: Implementation of self-monitoring and self-regulation capabilities

5. **Transfer Learning**: Development of mechanisms for applying learned knowledge to new tasks

6. **Evolutionary Optimization**: Integration of genetic algorithms and evolutionary strategies for brain optimization

7. **Neural Architecture Search**: Automated discovery of optimal brain architectures

8. **Quantum-inspired Neural Models**: Exploration of quantum computing applications in neural simulation

9. **Explainable AI**: Development of interpretable neural representations and decision-making processes

10. **Real-time Adaptation**: Online learning and adaptation to dynamic environments

These advanced features will build upon the bug fixes and improvements described in this guide, enabling more sophisticated and capable neural learning machines.

## References

- Original NLM Documentation
- Advanced Neural Network Architectures
- Computational Neuroscience Principles
- Machine Learning Theory
- Optimization Algorithms

---

*This advanced usage guide provides comprehensive examples and tools for experienced NLM users. These features should be used with caution and tested thoroughly in your specific application environment.*
