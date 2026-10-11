# NLM Memory Systems Documentation

## Overview

This document describes the memory systems implemented in the NLM (Neural Learning Machine). NLM incorporates multiple memory systems that work together to enable learning, retention, and recall of experiences.

## Memory Systems Implemented

NLM implements five distinct memory systems:

### 1. Working Memory

#### Biological Inspiration
Working memory is the brain's ability to temporarily hold and manipulate information. In neuroscience, this is associated with persistent activity in prefrontal and parietal cortices, as well as prefrontal-basal ganglia loops.

#### Computational Implementation
- **Persistent Activity**: Networks of neurons maintain activity over time through recurrent connections
- **Competitive Dynamics**: Multiple memory traces compete, with the most active trace winning
- **Capacity Limitation**: Working memory has limited capacity, typically modeled as a soft limit
- **Forgetting Curves**: Memory decay follows characteristic time courses

#### Key Features
- Real-time storage of sensory and motor information
- Active maintenance through recurrent excitation
- Competitive inhibition between memory traces
- Integration with attention and executive control
- Bidirectional interaction with long-term memory systems

#### Known Limitations
- Simplified implementation of persistent activity
- Limited capacity modeling compared to biological working memory
- Complex prefrontal-basal ganglia interactions are abstracted
- Does not model neural replay or chunking processes

### 2. Episodic Memory

#### Biological Inspiration
Episodic memory stores specific experiences with temporal and spatial context. In neuroscience, this is associated with the hippocampus and medial temporal lobe structures. The hippocampus supports pattern separation (orthogonality) and pattern completion (associative recall).

#### Computational Implementation
- **Experience Encoding**: Each completed experience is stored as an episode with associated context
- **Temporal Sequencing**: Episodes are stored with temporal ordering
- **Spatial Context**: Memory incorporates spatial relationships and environmental context
- **Replay**: During rest or sleep, episodes are replayed to strengthen connections
- **Consolidation**: Important episodes undergo consolidation over time

#### Key Features
- Storage of complete experiences with rich context
- Temporal ordering of events
- Spatial relationships and environmental context
- Sleep-dependent memory consolidation
- Integration with working memory through retrieval

#### Known Limitations
- Simplified experience representation compared to biological episodic memory
- Limited model of hippocampal-cortical interactions
- Basic replay mechanism without biological realism
- Does not model declarative vs. procedural memory distinctions

### 3. Semantic Memory

#### Biological Inspiration
Semantic memory stores facts, concepts, and general knowledge. Associated with neocortical regions and distributed representations throughout the brain.

#### Computational Implementation
- **Pattern Recognition**: Detection of similarities across experiences
- **Concept Formation**: Abstraction of common features from multiple experiences
- **Knowledge Graphs**: Representation of relationships between concepts
- **Schema Theory**: Organization of knowledge into hierarchical structures
- **Semantic Vectors**: Distributed representations of meaning

#### Key Features
- Storage of generalized knowledge from multiple experiences
- Concept formation from experience patterns
- Representation of relationships and associations
- Hierarchical organization of knowledge
- Integration with episodic memory through episodic-to-semantic memory binding

#### Known Limitations
- Simplified concept formation compared to biological semantic memory
- Limited model of neocortical distributed representations
- Basic schema theory implementation
- Does not model multiple memory systems interactions (hippocampal-neocortical)

### 4. Procedural Memory

#### Biological Inspiration
Procedural memory stores skills and habits. Associated with basal ganglia, cerebellum, and motor cortex. Involves gradual learning through reinforcement and error signals.

#### Computational Implementation
- **Habit Learning**: Repetitive practice leads to automaticity
- **Motor Memory**: Storage of motor patterns and sequences
- **Skill Acquisition**: Gradual improvement through practice
- **Practice Effects**: Performance improvement with repetition
- **Forgetting**: Skill decay when not practiced

#### Key Features
- Storage of motor skills and habits
- Gradual skill acquisition through practice
- Performance optimization and automaticity
- Integration with working memory for novel situations
- Forgetting curves based on retention intervals

#### Known Limitations
- Simplified motor learning compared to biological systems
- Limited model of basal ganglia and cerebellar circuitry
- Basic habit formation mechanism
- Does not model different types of procedural memory (cognitive vs. motor)

### 5. Associative Memory

#### Biological Inspiration
Associative memory enables linking of related information. Associated with amygdala (emotional associations), hippocampus (spatial and temporal associations), and various cortical regions.

#### Computational Implementation
- **Hebbian Learning**: Connections strengthen between co-activated elements
- **Pattern Completion**: Partial cues can retrieve complete memories
- **Pattern Separation**: Similar inputs produce orthogonal representations
- **Cross-Modal Integration**: Integration of information from different sensory modalities
- **Memory Binding**: Association of features into coherent memories

#### Key Features
- Formation of associative links between memory elements
- Partial cue completion for robust retrieval
- Separation of similar memories to avoid interference
- Integration of information from multiple sources
- Dynamic association updating based on experience

#### Known Limitations
- Simplified implementation of biological associative processes
- Limited model of amygdala and hippocampal associative functions
- Basic pattern completion mechanism
- Does not model different types of associations (semantic, episodic, procedural)

## Integration Architecture

### Cross-System Interactions

The NLM memory systems interact through several mechanisms:

1. **Working Episodic Memory Bridge**: Working memory stores active episodes, which are consolidated into episodic memory
2. **Episodic-Semantic Integration**: Episodic experiences contribute to semantic knowledge through abstraction
3. **Procedural Working Memory**: Skills in working memory are consolidated into procedural memory
4. **Associative Enhancement**: Working memory associations strengthen with repeated experiences
5. **Attentional Modulation**: Attention modulates memory encoding and retrieval across all systems

### Memory Flow

The general memory flow in NLM is:

1. **Perception**: Sensory input enters working memory
2. **Active Processing**: Working memory maintains and manipulates information
3. **Experience Formation**: Completed experiences are encoded into episodic memory
4. **Abstraction**: Similar experiences contribute to semantic memory
5. **Skill Learning**: Repeated actions become procedural memory
6. **Association Formation**: Relationships between memories are stored in associative memory
7. **Retrieval**: Memory systems interact to retrieve relevant information
8. **Application**: Retrieved memories guide behavior and decision-making

## Memory Performance Metrics

### Capacity

| Memory System | Theoretical Capacity | NLM Implementation |
|---------------|-------------------|------------------|
| Working Memory | ~7±2 items | Limited by recurrent connections |
| Episodic Memory | Unlimited (experience-dependent) | Limited by storage time |
| Semantic Memory | Large but bounded | Limited by concept formation |
| Procedural Memory | Large but bounded | Limited by skill complexity |
| Associative Memory | Unlimited (connections) | Limited by learning rate |

### Time Courses

| Memory System | Encoding Time | Storage Duration | Retrieval Speed |
|---------------|-------------|----------------|---------------|
| Working Memory | Instantaneous | Seconds to minutes | Immediate |
| Episodic Memory | Seconds | Hours to years | Variable |
| Semantic Memory | Seconds | Permanent | Fast |
| Procedural Memory | Minutes | Permanent | Fast |
| Associative Memory | Instantaneous | Permanent | Variable |

## Memory Parameters

### Working Memory Parameters

- **Capacity**: Number of concurrent items (typically 4-8)
- **Persistence Rate**: Rate of memory decay (typically 0.9-0.99 per timestep)
- **Competition Strength**: Intensity of interference between traces (typically 0.5-2.0)
- **Attention Gain**: Modulation of memory traces by attention (typically 1.0-3.0)

### Episodic Memory Parameters

- **Max Episodes**: Maximum number of stored episodes (typically 100-10000)
- **Consolidation Rate**: Rate of memory consolidation (typically 0.001-0.01)
- **Replay Probability**: Probability of episode replay during rest (typically 0.1-0.5)
- **Context Granularity**: Detail level of stored contexts (typically coarse to fine)

### Semantic Memory Parameters

- **Concept Learning Rate**: Rate of concept formation (typically 0.01-0.1)
- **Similarity Threshold**: Threshold for concept grouping (typically 0.1-0.5)
- **Abstraction Strength**: Degree of generalization (typically 0.2-0.8)
- **Schema Hierarchy Depth**: Depth of knowledge organization (typically 2-5)

### Procedural Memory Parameters

- **Habit Formation Rate**: Rate of skill acquisition (typically 0.001-0.01)
- **Practice Effect**: Performance improvement per practice (typically 0.1-0.5)
- **Forgetting Rate**: Skill decay when not practiced (typically 0.0001-0.01)
- **Generalization**: Transfer of skills to novel situations (typically 0.1-0.9)

### Associative Memory Parameters

- **Hebbian Learning Rate**: Rate of association strengthening (typically 0.001-0.01)
- **Pattern Completion Strength**: Degree of completion from partial cues (typically 0.5-0.9)
- **Pattern Separation**: Degree of orthogonalization (typically 0.1-0.5)
- **Association Decay**: Rate of association weakening (typically 0.0001-0.01)

## Research Applications

### Studying Memory Phenomena

NLM can be used to study various memory phenomena:

1. **Forgetting Curves**: Modeling time-dependent memory loss
2. **Proactive Interference**: Effects of similar memories on recall
3. **Retroactive Interference**: Effects of new learning on old memories
4. **Memory Span**: Limits of working memory capacity
5. **Serial Position Effects**: Recall patterns across memory lists
6. **Spacing Effects**: Influence of learning spacing on retention

### Neural Basis of Memory

NLM provides insights into:

1. **Synaptic Mechanisms**: How synaptic changes support memory
2. **Circuit Dynamics**: Role of neural circuits in memory processing
3. **Systems Interaction**: How different brain systems work together
4. **Development**: How memory systems change over time
5. **Disorders**: Computational models of memory impairments

### Computational Models

NLM enables testing of:

1. **Memory Models**: Different theoretical frameworks of memory
2. **Learning Algorithms**: Various learning rules and optimization
3. **Network Architectures**: Different neural network organizations
4. **Neural Codes**: Different representations of memory information
5. **Neurobiology**: Mapping computational models to biological systems

## Future Extensions

### Enhanced Memory Systems

1. **Multiple Memory Stores**: Separate systems for different memory types
2. **Memory Modulation**: Hormonal and neuromodulatory influences
3. **Memory Search**: Efficient retrieval algorithms
4. **Memory Competition**: Dynamic resource allocation
5. **Memory Monitoring**: Metacognitive awareness

### Advanced Features

1. **Sleep Waves**: Memory processing during sleep cycles
2. **Dreaming**: Simulating memory consolidation during sleep
3. **Cognitive Maps**: Spatial representations and navigation
4. **Social Memory**: Memory for social information and relationships
5. **Emotional Memory**: Integration of affect with memory

## References

1. Baddeley, A. D. (2012). Working memory: Theories, models, and applications. Annals of the New York Academy of Sciences, 1265(1), 29-49.
2. Squire, L. R., & Zola-Morgan, S. (1991). The medial temporal lobe memory system. Science, 253(5017), 1445-1450.
3. McClelland, D. C., McNaughton, B. L., & O'Reilly, R. C. (1995). Complementary learning systems in the brain. Science, 269(5224), 1255-1258.
4. McClelland, D. C., & Rogers, T. T. (1993). The past, present, and future of connectionist approaches to memory. Annual review of psychology, 44(1), 141-173.
5. Moscovitch, M. (1992). Memory systems in the brain: A neuropsychologist's perspective. Annals of the New York Academy of Sciences, 667(1), 61-82.

## License

MIT