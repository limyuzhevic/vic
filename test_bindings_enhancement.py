# Test the enhanced Python bindings
import pynlm

# Test 1: Basic brain creation and initialization
print("Test 1: Basic brain functionality")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print(f"  Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"  Has {brain.getTotalSynapseCount()} synapses")

# Test 2: Memory system access
print("\nTest 2: Memory system access")
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"  Working memory has {working_memory.getActiveTraces()} active traces")
    print(f"  Working memory capacity: {working_memory.getCapacity()}")
    print(f"  Working memory decay rate: {working_memory.getDecayRate()}")

episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"  Episodic memory has {episodic_memory.getEpisodeCount()} episodes")
    print(f"  Replay enabled: {episodic_memory.isReplayEnabled()}")

associative_memory = brain.getAssociativeMemory()
if associative_memory:
    print(f"  Associative memory has {associative_memory.getAssociationCount()} associations")

# Test 3: Prediction system access
print("\nTest 3: Prediction system access")
prediction_system = brain.getPredictionSystem()
if prediction_system:
    print("  Prediction system available")

# Test 4: Cognition systems access
print("\nTest 4: Cognition systems access")
planner = brain.getPlanner()
if planner:
    print("  Neural planner available")

concept_formation = brain.getConceptFormation()
if concept_formation:
    print("  Concept formation system available")

attention = brain.getAttention()
if attention:
    print("  Attentional selection system available")

development_system = brain.getDevelopmentSystem()
if development_system:
    print(f"  Development system at stage: {brain.getDevelopmentalStage()}")

# Test 5: Neuromodulation systems access
print("\nTest 5: Neuromodulation systems access")
dopamine = brain.getDopamine()
if dopamine:
    print("  Dopamine neuromodulator available")

curiosity = brain.getCuriosity()
if curiosity:
    print(f"  Curiosity level: {curiosity.getCuriosityLevel():.3f}")

novelty = brain.getNovelty()
if novelty:
    print("  Novelty detection system available")

prediction_error = brain.getPredictionErrorSignal()
if prediction_error:
    print(f"  Prediction error: {prediction_error.getPredictionError():.3f}")

# Test  cerebellSystem access
print("\nTest 6: Spike system access")
spike_system = brain.getSpikeSystem()
if spike_system:
    print(f"  Spike system has {spike_system.getPendingSpikeEventCount()} pending events")

# Test 7: Plasticity systems access
print("\nTest 7: Plasticity systems access")
stdp = brain.getSTDP()
if stdp:
    print("  STDP plasticity system available")

hebbian = brain.getHebbian()
if hebbian:
    print("  Hebbian plasticity system available")

structural = brain.getStructuralPlasticity()
if structural:
    print("  Structural plasticity system available")

# Test 8: Random generator access
print("\nTest 8: Random generator access")
random_gen = brain.getRandomGenerator()
if random_gen:
    print("  Random generator available")

# Test 9: Learned preference access
print("\nTest 9: Learned preference access")
preference = brain.getLearnedPreference()
print(f"  Learned preference: {preference:.3f}")
brain.setLearnedPreference(0.5)
print(f"  Updated learned preference to: {brain.getLearnedPreference():.3f}")

# Test 10: AgentBrain advanced functionality
print("\nTest 10: AgentBrain advanced functionality")
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

if hasattr(agent, 'getThoughtTrace'):
    thought_trace = agent.getThoughtTrace()
    print(f"  Thought trace available: {len(thought_trace)} chars")

if hasattr(agent, 'getMotorGroupActivities'):
    activities = agent.getMotorGroupActivities()
    print(f"  Motor group activities: {len(activities)} groups")

if hasattr(agent, 'encodeText'):
    text_pattern = agent.encodeText("test")
    print(f"  Text encoding successful, pattern length: {len(text_pattern)}")

if hasattr(agent, 'getInternalFiringRates'):
    internal_rates = agent.getInternalFiringRates()
    print(f"  Internal firing rates: {len(internal_rates)} neurons")

# Test 11: Configuration enhanced access
print("\nTest 11: Enhanced configuration access")
config.set("test.int", 42)
config.set("test.double", 3.14)
config.set("test.bool", True)
config.set("test.string", "hello")

print(f"  Int value: {config.getOr('test.int', 0)}")
print(f"  Double value: {config.getOr('test.double', 0.0)}")
print(f"  Bool value: {config.getOr('test.bool', False)}")
print(f"  String value: {config.getOr('test.string', '')}")

print(f"  Has test.int: {config.has('test.int')}")
print(f"  Has non-existent: {config.has('nonexistent')}")

# Test 12: Clone functionality
print("\nTest 12: Clone functionality")
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

# Test Vision clone
vision = world.getSensoryPercept().getVision()
if hasattr(vision, 'clone'):
    cloned_vision = vision.clone()
    print(f"  Vision cloned: {vision.getWidth()}x{vision.getHeight()}")

# Test Audio clone  
audio = pynlm.Audio(44100, 1024)
if hasattr(audio, 'clone'):
    cloned_audio = audio.clone()
    print(f"  Audio cloned: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")

# Test InternalSignals clone
internal = pynlm.InternalSignals()
if hasattr(internal, 'clone'):
    cloned_internal = internal.clone()
    print(f"  InternalSignals cloned")

# Test Action clone
action = pynlm.Action(pynlm.ActionType.MoveForward)
if hasattr(action, 'clone'):
    cloned_action = action.clone()
    print(f"  Action cloned: type {action.getType()}")

print("\nAll tests completed successfully!")