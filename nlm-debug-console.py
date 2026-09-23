#!/usr/bin/env python3
"""
NLM Debug Console - Advanced Brain Inspection and Visualization Tools

This script provides comprehensive debugging and visualization capabilities for the NLM brain:

Features:
- Real-time brain state inspection
- Memory system analysis
- Neuromodulation monitoring
- Network visualization
- Performance profiling
- Configuration analysis
- Interactive debugging console

The debug console is designed to help researchers and developers:
- Understand brain behavior during simulation
- Debug issues with brain integration
- Analyze memory and learning processes
- Visualize neural network activity
- Profile performance bottlenecks
"""

import sys
import os
import json
import time
import argparse
from pathlib import Path
from typing import Dict, Any, List, Optional, Callable
import threading
import queue
import math

# Add the current directory to the path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available. Running in limited mode.")

# Color codes for terminal output
class Colors:
    HEADER = "\033[95m"
    OKBLUE = "\033[94m"
    OKCYAN = "\033[96m"
    OKGREEN = "\033[92m"
    WARNING = "\033[93m"
    FAIL = "\033[91m"
    ENDC = "\033[0m"
    BOLD = "\033[1m"
    UNDERLINE = "\033[4m"
    CLEAR = "\033[2J\033[H"

class NLMDebugConsole:
    """Comprehensive NLM brain debugging and visualization console"""
    
    def __init__(self):
        self.brain = None
        self.agent = None
        self.world = None
        self.debug_mode = "passive"
        self.visualization_mode = "text"
        self.update_interval = 1.0  # seconds
        self.monitor_thread = None
        self.stop_monitor = threading.Event()
        self.performance_data = {
            "brain_stats": [],
            "memory_stats": [],
            "neuromod_stats": [],
            "timestamps": []
        }
        self.debug_commands = self.initialize_debug_commands()
        
    def initialize_debug_commands(self):
        """Initialize available debug commands"""
        return {
            'help': self.show_help,
            'status': self.show_system_status,
            'brain': self.show_brain_info,
            'memory': self.show_memory_analysis,
            'neuromod': self.show_neuromodulation_analysis,
            'visualization': self.show_visualization,
            'network': self.show_network_analysis,
            'performance': self.show_performance_stats,
            'config': self.show_configuration,
            'trace': self.start_trace,
            'stop': self.stop_debugging,
            'quit': self.quit_debug,
            'cls': self.clear_screen,
            'refresh': self.refresh_display,
            'save': self.save_debug_session,
            'load': self.load_debug_session,
            'plot': self.create_plots,
            'export': self.export_data
        }
    
    def print_header(self):
        """Print debug console header"""
        print(f"{Colors.HEADER}{Colors.BOLD}")
        print("╔═══════════════════════════════════════════════════════════════╗")
        print("║                                                               ║")
        print("║     NLM Debug Console - Brain Inspection & Visualization      ║")
        print("║                                                               ║")
        print("║     Phase 6 - Advanced Brain Analysis Tools                   ║")
        print("║                                                               ║")
        print("╚═══════════════════════════════════════════════════════════════╝")
        print(f"{Colors.ENDC}")
        
        print(f"\n{Colors.OKCYAN}=== Debug Mode: {self.debug_mode.upper()} ==={Colors.ENDC}")
        print(f"Visualization: {self.visualization_mode}")
        print(f"Update Interval: {self.update_interval}s")
        print()
        
    def show_help(self, args: List[str] = None):
        """Show help information"""
        print(f"{Colors.OKCYAN}\n=== Available Commands ==={Colors.ENDC}")
        
        categories = {
            'Information Commands': ['status', 'brain', 'memory', 'neuromod', 'config'],
            'Visualization Commands': ['visualization', 'network', 'plot'],
            'Monitoring Commands': ['performance', 'trace', 'refresh'],
            'Session Commands': ['save', 'load'],
            'Control Commands': ['stop', 'quit', 'cls', 'help']
        }
        
        for category, commands in categories.items():
            print(f"\n{Colors.BOLD}{category}:{Colors.ENDC}")
            for cmd in commands:
                description = self.get_command_description(cmd)
                print(f"  {cmd:<15} - {description}")
        
        print(f"\n{Colors.OKCYAN}=== Debug Mode Controls ==={Colors.ENDC}")
        print("  passive  - Normal operation, periodic updates")
        print("  active   - Continuous monitoring, real-time updates")
        print("  trace    - Detailed logging of all operations")
        print()
        
    def get_command_description(self, command: str) -> str:
        """Get command description"""
        descriptions = {
            'status': 'Show system and brain status',
            'brain': 'Show detailed brain information and statistics',
            'memory': 'Analyze memory system (working + episodic + semantic + procedural)',
            'neuromod': 'Analyze neuromodulation systems (dopamine, curiosity, novelty)',
            'visualization': 'Show brain visualization in text/console',
            'network': 'Analyze neural network structure and activity',
            'performance': 'Show performance statistics and profiling data',
            'config': 'Show current configuration',
            'trace': 'Start detailed operation tracing',
            'save': 'Save current debug session',
            'load': 'Load a debug session',
            'plot': 'Create data plots and graphs',
            'export': 'Export debug data to various formats',
            'stop': 'Stop debugging session',
            'quit': 'Exit debug console',
            'cls': 'Clear screen',
            'refresh': 'Refresh current display'
        }
        return descriptions.get(command, "No description available")
    
    def show_system_status(self, args: List[str] = None):
        """Show complete system status"""
        print(f"{Colors.OKCYAN}\n=== System Status ==={Colors.ENDC}")
        
        if not PYNLM_AVAILABLE or self.brain is None:
            print(f"{Colors.WARNING}Brain not initialized or pynlm not available{Colors.ENDC}")
            return
        
        # Basic system information
        print(f"{Colors.OKGREEN}System Information:{Colors.ENDC}")
        print(f"  Brain initialized: {'Yes' if self.brain else 'No'}")
        if self.brain:
            print(f"  Total neurons: {self.brain.getTotalNeuronCount()}")
            print(f"  Total synapses: {self.brain.getTotalSynapseCount()}")
            print(f"  Currently firing: {self.brain.getFiringNeuronCount()}")
            print(f"  Average firing rate: {self.brain.getAverageFiringRate():.3f}")
        
        # Agent information
        if self.agent:
            print(f"{Colors.OKBLUE}Agent Information:{Colors.ENDC}")
            print(f"  Curiosity level: {self.agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {self.agent.getNoveltyLevel():.3f}")
            print(f"  Neuromodulation: {self.agent.getNeuromodulationLevel():.3f}")
            print(f"  Prediction error: {self.agent.getPredictionError():.3f}")
            print(f"  Developmental stage: {self.brain.getDevelopmentalStage().name}")
        
        # Memory systems
        print(f"{Colors.OKCYAN}Memory Systems:{Colors.ENDC}")
        if self.brain:
            if self.brain.getWorkingMemory():
                wm = self.brain.getWorkingMemory()
                print(f"  Working memory: {wm.getActiveTraces()} active traces")
                print(f"  Capacity: {wm.getCapacity()}")
                print(f"  Current size: {wm.getCurrentSize()}")
            
            if self.brain.getEpisodicMemory():
                em = self.brain.getEpisodicMemory()
                print(f"  Episodic memory: {em.getEpisodeCount()} episodes")
                print(f"  Max capacity: {em.getMaxCapacity()}")
            
            if self.brain.getSemanticMemory():
                sm = self.brain.getSemanticMemory()
                facts = sm.getAllFacts()
                print(f"  Semantic memory: {len(facts)} facts")
                
            if self.brain.getProceduralMemory():
                pm = self.brain.getProceduralMemory()
                skills = pm.getAllSkills()
                print(f"  Procedural memory: {len(skills)} skills")
        
        # Neuromodulation systems
        print(f"{Colors.WARNING}Neuromodulation Systems:{Colors.ENDC}")
        if self.brain:
            if self.brain.getDopamine():
                dopamine = self.brain.getDopamine()
                print(f"  Dopamine system: {dopamine.getLevel():.3f}")
                
            if self.brain.getCuriosity():
                curiosity = self.brain.getCuriosity()
                print(f"  Curiosity system: {curiosity.getLevel():.3f}")
                
            if self.brain.getNovelty():
                novelty = self.brain.getNovelty()
                print(f"  Novelty system: {novelty.getLevel():.3f}")
        
        # World state
        if self.world:
            print(f"{Colors.BOLD}World State:{Colors.ENDC}")
            print(f"  Simulation time: {self.world.getSimulationTime():.3f}")
            if hasattr(self.world, 'getWidth'):
                print(f"  World size: {self.world.getWidth()} x {self.world.getHeight()}")
        
        # Debug session info
        print(f"{Colors.OKCYAN}Debug Session:{Colors.ENDC}")
        print(f"  Debug mode: {self.debug_mode}")
        print(f"  Update interval: {self.update_interval}s")
        print(f"  Performance data points: {len(self.performance_data['timestamps'])}")
        
    def show_brain_info(self, args: List[str] = None):
        """Show detailed brain information"""
        print(f"{Colors.OKCYAN}\n=== Brain Information ==={Colors.ENDC}")
        
        if not PYNLM_AVAILABLE or self.brain is None:
            print(f"{Colors.WARNING}Brain not initialized or pynlm not available{Colors.ENDC}")
            return
        
        # Basic statistics
        print(f"{Colors.OKGREEN}Basic Statistics:{Colors.ENDC}")
        print(f"  Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"  Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {self.brain.getAverageFiringRate():.3f}")
        print(f"  Excitatory/Inhibitory ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
        print(f"  Total spike count: {self.brain.getTotalSpikeCount()}")
        
        # Neuron type distribution (if available)
        if hasattr(self.brain, 'getNeuronsByType'):
            print(f"{Colors.OKBLUE}Neuron Type Distribution:{Colors.ENDC}")
            neuron_types = self.brain.getNeuronsByType()
            for neuron_type, count in neuron_types.items():
                print(f"  {neuron_type}: {count}")
        
        # Region information
        if hasattr(self.brain, 'getRegions'):
            print(f"{Colors.OKCYAN}Brain Regions:{Colors.ENDC}")
            regions = self.brain.getRegions()
            for i, region in enumerate(regions):
                neurons = region.getAllNeurons()
                synapses = region.getSynapses()
                print(f"  Region {i}: {len(neurons)} neurons, {len(synapses)} synapses")
        
        # Memory system integration
        print(f"{Colors.WARNING}Memory Integration:{Colors.ENDC}")
        if self.brain.getWorkingMemory():
            wm = self.brain.getWorkingMemory()
            print(f"  Working memory: ✓ Integrated ({wm.getActiveTraces()} traces)")
        else:
            print(f"  Working memory: ✗ Not available")
            
        if self.brain.getEpisodicMemory():
            em = self.brain.getEpisodicMemory()
            print(f"  Episodic memory: ✓ Integrated ({em.getEpisodeCount()} episodes)")
        else:
            print(f"  Episodic memory: ✗ Not available")
        
        if self.brain.getSemanticMemory():
            sm = self.brain.getSemanticMemory()
            print(f"  Semantic memory: ✓ Integrated ({len(sm.getAllFacts())} facts)")
        else:
            print(f"  Semantic memory: ✗ Not available")
        
        if self.brain.getProceduralMemory():
            pm = self.brain.getProceduralMemory()
            print(f"  Procedural memory: ✓ Integrated ({len(pm.getAllSkills())} skills)")
        else:
            print(f"  Procedural memory: ✗ Not available")
        
        if self.brain.getAssociativeMemory():
            am = self.brain.getAssociativeMemory()
            print(f"  Associative memory: ✓ Integrated ({len(am.getAssociations())} associations)")
        else:
            print(f"  Associative memory: ✗ Not available")
        
        # Neuromodulation integration
        print(f"{Colors.OKCYAN}Neuromodulation Integration:{Colors.ENDC}")
        neuromod_systems = [
            ('Dopamine', self.brain.getDopamine),
            ('Curiosity', self.brain.getCuriosity),
            ('Novelty', self.brain.getNovelty),
            ('Prediction Error', self.brain.getPredictionError)
        ]
        
        for name, getter in neuromod_systems:
            if getter():
                print(f"  {name}: ✓ Integrated")
            else:
                print(f"  {name}: ✗ Not available")
        
        # Prediction system
        if self.brain.getPredictionSystem():
            print(f"  Prediction system: ✓ Integrated")
        else:
            print(f"  Prediction system: ✗ Not available")
        
        # Development system
        if self.brain.getDevelopmentSystem():
            dev = self.brain.getDevelopmentSystem()
            print(f"  Development system: ✓ Integrated (stage: {dev.getDevelopmentalStage().name})")
        else:
            print(f"  Development system: ✗ Not available")
        
        # Cognition systems
        if self.brain.getPlanner():
            print(f"  Neural planner: ✓ Integrated")
        else:
            print(f"  Neural planner: ✗ Not available")
            
        if self.brain.getConceptFormation():
            print(f"  Concept formation: ✓ Integrated")
        else:
            print(f"  Concept formation: ✗ Not available")
            
        if self.brain.getAttention():
            print(f"  Neural attention: ✓ Integrated")
        else:
            print(f"  Neural attention: ✗ Not available")
        
    def show_memory_analysis(self, args: List[str] = None):
        """Comprehensive memory system analysis"""
        print(f"{Colors.OKCYAN}\n=== Memory System Analysis ==={Colors.ENDC}")
        
        if not PYNLM_AVAILABLE or self.brain is None:
            print(f"{Colors.WARNING}Brain not available{Colors.ENDC}")
            return
        
        # Working Memory Analysis
        print(f"{Colors.OKGREEN}Working Memory Analysis:{Colors.ENDC}")
        if self.brain.getWorkingMemory():
            wm = self.brain.getWorkingMemory()
            print(f"  Active traces: {wm.getActiveTraces()}")
            print(f"  Capacity: {wm.getCapacity()}")
            print(f"  Current size: {wm.getCurrentSize()}")
            print(f"  Utilization: {wm.getCurrentSize() / wm.getCapacity() * 100:.1f}%")
            
            # Decay analysis
            print(f"  Decay behavior: {wm.getDecayRate():.3f} per timestep")
        else:
            print(f"  Working memory: ✗ Not available")
        
        # Episodic Memory Analysis
        print(f"{Colors.OKCYAN}Episodic Memory Analysis:{Colors.ENDC}")
        if self.brain.getEpisodicMemory():
            em = self.brain.getEpisodicMemory()
            episode_count = em.getEpisodeCount()
            print(f"  Total episodes: {episode_count}")
            print(f"  Maximum capacity: {em.getMaxCapacity()}")
            print(f"  Storage utilization: {episode_count / em.getMaxCapacity() * 100:.1f}%")
            
            if episode_count > 0:
                # Get recent episodes
                recent_episodes = em.getRecentEpisodes(5)
                print(f"  Most recent episodes:")
                for i, episode in enumerate(recent_episodes):
                    print(f"    Episode {i}: timestamp={episode.timestamp}, "
                          f"reward={episode.reward}, "
                          f"novelty={episode.novelty}")
        else:
            print(f"  Episodic memory: ✗ Not available")
        
        # Semantic Memory Analysis
        print(f"{Colors.WARNING}Semantic Memory Analysis:{Colors.ENDC}")
        if self.brain.getSemanticMemory():
            sm = self.brain.getSemanticMemory()
            facts = sm.getAllFacts()
            print(f"  Total facts: {len(facts)}")
            
            # Fact categories (if available)
            if hasattr(sm, 'getFactCategories'):
                categories = sm.getFactCategories()
                print(f"  Fact categories: {len(categories)}")
                for category, category_facts in categories.items():
                    print(f"    {category}: {len(category_facts)} facts")
        else:
            print(f"  Semantic memory: ✗ Not available")
        
        # Procedural Memory Analysis
        print(f"{Colors.OKBLUE}Procedural Memory Analysis:{Colors.ENDC}")
        if self.brain.getProceduralMemory():
            pm = self.brain.getProceduralMemory()
            skills = pm.getAllSkills()
            print(f"  Total skills: {len(skills)}")
            
            # Skill proficiency analysis
            proficient_skills = [s for s in skills if s.proficiency > 0.7]
            print(f"  Proficient skills (>0.7): {len(proficient_skills)}")
            print(f"  Novice skills (<0.3): {len([s for s in skills if s.proficiency < 0.3])}")
            
            if skills:
                avg_proficiency = sum(s.proficiency for s in skills) / len(skills)
                print(f"  Average proficiency: {avg_proficiency:.3f}")
        else:
            print(f"  Procedural memory: ✗ Not available")
        
        # Associative Memory Analysis
        print(f"{Colors.OKCYAN}Associative Memory Analysis:{Colors.ENDC}")
        if self.brain.getAssociativeMemory():
            am = self.brain.getAssociativeMemory()
            associations = am.getAllAssociations()
            print(f"  Total associations: {len(associations)}")
            
            # Association strength analysis
            if associations:
                avg_strength = sum(assoc.strength for assoc in associations) / len(associations)
                strong_associations = [a for a in associations if a.strength > 0.8]
                weak_associations = [a for a in associations if a.strength < 0.3]
                print(f"  Strong associations (>0.8): {len(strong_associations)}")
                print(f"  Weak associations (<0.3): {len(weak_associations)}")
                print(f"  Average strength: {avg_strength:.3f}")
        else:
            print(f"  Associative memory: ✗ Not available")
        
        # Memory consistency analysis
        print(f"{Colors.BOLD}Memory Consistency Analysis:{Colors.ENDC}")
        if all(self.brain.getMemorySystem(system) is not None for system in 
               ['working', 'episodic', 'semantic', 'procedural', 'associative']):
            print(f"  ✓ All memory systems integrated")
            
            # Check for inconsistencies
            inconsistencies = []
            if self.brain.getWorkingMemory():
                wm = self.brain.getWorkingMemory()
                if wm.getCurrentSize() > wm.getCapacity():
                    inconsistencies.append("Working memory over capacity")
            
            if inconsistencies:
                print(f"  ⚠ Inconsistencies found:")
                for issue in inconsistencies:
                    print(f"    - {issue}")
            else:
                print(f"  ✓ No inconsistencies detected")
        else:
            print(f"  ✗ Some memory systems not integrated")
        
    def show_neuromodulation_analysis(self, args: List[str] = None):
        """Comprehensive neuromodulation analysis"""
        print(f"{Colors.OKCYAN}\n=== Neuromodulation Analysis ==={Colors.ENDC}")
        
        if not PYNLM_AVAILABLE or self.brain is None:
            print(f"{Colors.WARNING}Brain not available{Colors.ENDC}")
            return
        
        # Neuromodulation system inventory
        print(f"{Colors.OKGREEN}Neuromodulation System Inventory:{Colors.ENDC}")
        neuromod_systems = [
            ('Dopamine', 'reward prediction error', self.brain.getDopamine),
            ('Curiosity', 'exploration drive', self.brain.getCuriosity),
            ('Novelty', 'surprise detection', self.brain.getNovelty),
            ('Prediction Error', 'mismatch signal', self.brain.getPredictionError),
            ('Reward Prediction', 'expected vs actual reward', self.brain.getRewardPrediction),
            ('Attention', 'selective focus', self.brain.getAttention)
        ]
        
        available_systems = []
        for name, description, getter in neuromod_systems:
            if getter():
                available_systems.append((name, description))
                print(f"  ✓ {name:20} - {description}")
            else:
                print(f"  ✗ {name:20} - {description} (not available)")
        
        # Current neuromodulation levels (if agent available)
        if self.agent:
            print(f"{Colors.OKCYAN}Current Neuromodulation Levels:{Colors.ENDC}")
            print(f"  Dopamine level: {self.agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity level: {self.agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {self.agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {self.agent.getPredictionError():.3f}")
            
            # Neuromodulation dynamics analysis
            print(f"{Colors.WARNING}Neuromodulation Dynamics:{Colors.ENDC}")
            
            # Curiosuty analysis
            curiosity = self.agent.getCuriosityLevel()
            if curiosity > 0.7:
                print(f"  🧩 Curiosity (High): Strong exploration drive")
            elif curiosity > 0.3:
                print(f"  🧩 Curiosity (Medium): Moderate exploration")
            else:
                print(f"  🧩 Curiosity (Low): Conservative behavior")
            
            # Novelty analysis
            novelty = self.agent.getNoveltyLevel()
            if novelty > 0.7:
                print(f"  🔍 Novelty (High): High sensitivity to new stimuli")
            elif novelty > 0.3:
                print(f"  🔍 Novelty (Medium): Moderate sensitivity")
            else:
                print(f"  🔍 Novelty (Low): Low sensitivity to novelty")
            
            # Dopamine analysis
            dopamine = self.agent.getNeuromodulationLevel()
            if dopamine > 0.5:
                print(f"  🎯 Dopamine (High): Positive prediction error, reward learning")
            elif dopamine < -0.5:
                print(f"  🎯 Dopamine (Low): Negative prediction error, avoidance learning")
            else:
                print(f"  🎯 Dopamine (Neutral): Stable expectation matching")
        
        # Neuromodulation interaction analysis
        print(f"{Colors.OKBLUE}Neuromodulation Interactions:{Colors.ENDC}")
        if self.agent:
            # Check for correlations
            curiosity = self.agent.getCuriosityLevel()
            novelty = self.agent.getNoveltyLevel()
            dopamine = self.agent.getNeuromodulationLevel()
            
            # Curiosuty-Novelty relationship
            if curiosity > 0.5 and novelty > 0.5:
                print(f"  🧩✨ High curiosity + high novelty: Strong exploration-innovation cycle")
            elif curiosity > 0.5:
                print(f"  🧩✨ High curiosity + low novelty: Focused exploration")
            elif novelty > 0.5:
                print(f"  🧩✨ Low curiosity + high novelty: Passive novelty detection")
            
            # Dopamine-Curiosity relationship
            if abs(dopamine) < 0.2 and curiosity > 0.5:
                print(f"  🎯🧩 Neutral dopamine + high curiosity: Optimal learning state")
            
            # Prediction error analysis
            pred_error = self.agent.getPredictionError()
            if abs(pred_error) > 0.5:
                print(f"  🔮 Large prediction error: Significant learning opportunity")
            elif abs(pred_error) > 0.2:
                print(f"  🔮 Moderate prediction error: Some learning needed")
            else:
                print(f"  🔮 Small prediction error: Stable expectations")
        
    def show_network_analysis(self, args: List[str] = None):
        """Neural network structure and activity analysis"""
        print(f"{Colors.OKCYAN}\n=== Neural Network Analysis ==={Colors.ENDC}")
        
        if not PYNLM_AVAILABLE or self.brain is None:
            print(f"{Colors.WARNING}Brain not available{Colors.ENDC}")
            return
        
        # Network topology analysis
        print(f"{Colors.OKGREEN}Network Topology:{Colors.ENDC}")
        
        if hasattr(self.brain, 'getRegions'):
            regions = self.brain.getRegions()
            print(f"  Brain regions: {len(regions)}")
            
            for i, region in enumerate(regions):
                neurons = region.getAllNeurons()
                synapses = region.getSynapses()
                if neurons:
                    density = len(synapses) / (len(neurons) * (len(neurons) - 1)) if len(neurons) > 1 else 0
                    print(f"    Region {i}: {len(neurons)} neurons, {len(synapses)} synapses, "
                          f"density: {density:.4f}")
        
        # Neuron type distribution
        print(f"{Colors.OKCYAN}Neuron Type Distribution:{Colors.ENDC}")
        if hasattr(self.brain, 'getNeuronsByType'):
            neuron_types = self.brain.getNeuronsByType()
            total_neurons = self.brain.getTotalNeuronCount()
            
            for neuron_type, count in neuron_types.items():
                percentage = count / total_neurons * 100
                print(f"  {neuron_type:15} {count:>5} neurons ({percentage:>5.1f}%)")
        
        # Network connectivity analysis
        print(f"{Colors.WARNING}Network Connectivity:{Colors.ENDC}")
        
        # Calculate connectivity metrics
        if hasattr(self.brain, 'getSynapseConnections'):
            connections = self.brain.getSynapseConnections()
            total_connections = len(connections)
            
            if total_connections > 0:
                # Find most and least connected neurons
                neuron_connections = {}
                for conn in connections:
                    neuron_connections[conn.pre_neuron] = neuron_connections.get(conn.pre_neuron, 0) + 1
                    neuron_connections[conn.post_neuron] = neuron_connections.get(conn.post_neuron, 0) + 1
                
                if neuron_connections:
                    max_connections = max(neuron_connections.values())
                    min_connections = min(neuron_connections.values())
                    avg_connections = sum(neuron_connections.values()) / len(neuron_connections)
                    
                    print(f"  Total connections: {total_connections}")
                    print(f"  Most connected neuron: {max_connections} connections")
                    print(f"  Least connected neuron: {min_connections} connections")
                    print(f"  Average connections per neuron: {avg_connections:.2f}")
        
        # Network activity analysis
        print(f"{Colors.OKBLUE}Network Activity:{Colors.ENDC}")
        print(f"  Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  Currently firing: {self.brain.getFiringNeuronCount()}")
        print(f"  Firing percentage: {self.brain.getFiringNeuronCount() / max(1, self.brain.getTotalNeuronCount()) * 100:.1f}%")
        
        if self.brain.getAverageFiringRate() > 0:
            firing_rate = self.brain.getAverageFiringRate()
            if firing_rate > 10:
                activity_level = "Very High"
            elif firing_rate > 5:
                activity_level = "High"
            elif firing_rate > 1:
                activity_level = "Moderate"
            elif firing_rate > 0.1:
                activity_level = "Low"
            else:
                activity_level = "Very Low"
                
            print(f"  Average firing rate: {firing_rate:.2f} Hz ({activity_level})")
        
        # Network clustering (if available)
        if hasattr(self.brain, 'getNetworkClustering'):
            clustering = self.brain.getNetworkClustering()
            print(f"{Colors.BOLD}Network Clustering:{Colors.ENDC}")
            print(f"  Clustering coefficient: {clustering.getClusteringCoefficient():.3f}")
            print(f"  Small-worldness: {clustering.getSmallWorldness():.3f}")
            
            if clustering.getClusteringCoefficient() > 0.5:
                print(f"  Network type: Highly clustered (modular)")
            elif clustering.getClusteringCoefficient() > 0.2:
                print(f"  Network type: Moderately clustered")
            else:
                print(f"  Network type: Random-like")
        
    def show_performance_stats(self, args: List[str] = None):
        """Performance monitoring and profiling"""
        print(f"{Colors.OKCYAN}\n=== Performance Statistics ==={Colors.ENDC}")
        
        if not self.performance_data['timestamps']:
            print(f"{Colors.WARNING}No performance data available{Colors.ENDC}")
            print("Run some simulation steps first, or use 'trace' command to start monitoring.")
            return
        
        print(f"{Colors.OKGREEN}Performance Summary:{Colors.ENDC}")
        
        timestamps = self.performance_data['timestamps']
        if len(timestamps) > 1:
            total_time = timestamps[-1] - timestamps[0]
            samples_per_second = len(timestamps) / total_time if total_time > 0 else 0
            
            print(f"  Total monitoring duration: {total_time:.1f}s")
            print(f"  Sampling rate: {samples_per_second:.1f} samples/second")
            print(f"  Data points collected: {len(timestamps)}")
        
        # Brain statistics trends
        if self.performance_data['brain_stats']:
            brain_stats = self.performance_data['brain_stats']
            
            neurons = [stat.get('neurons', 0) for stat in brain_stats]
            firing = [stat.get('firing', 0) for stat in brain_stats]
            
            print(f"{Colors.OKCYAN}Brain Statistics Trends:{Colors.ENDC}")
            if neurons:
                avg_neurons = sum(neurons) / len(neurons)
                print(f"  Average neurons: {avg_neurons:.0f}")
                
            if firing:
                avg_firing = sum(firing) / len(firing)
                max_firing = max(firing)
                min_firing = min(firing)
                print(f"  Firing rate: avg={avg_firing:.1f}, max={max_firing:.1f}, min={min_firing:.1f}")
        
        # Memory statistics
        if self.performance_data['memory_stats']:
            memory_stats = self.performance_data['memory_stats']
            
            traces = [stat.get('traces', 0) for stat in memory_stats]
            
            print(f"{Colors.WARNING}Memory Statistics:{Colors.ENDC}")
            if traces:
                avg_traces = sum(traces) / len(traces)
                print(f"  Average working memory traces: {avg_traces:.1f}")
        
        # Neuromodulation statistics
        if self.performance_data['neuromod_stats']:
            neuromod_stats = self.performance_data['neuromod_stats']
            
            dopamine_levels = [stat.get('dopamine', 0) for stat in neuromod_stats]
            curiosity_levels = [stat.get('curiosity', 0) for stat in neuromod_stats]
            
            print(f"{Colors.OKBLUE}Neuromodulation Statistics:{Colors.ENDC}")
            if dopamine_levels:
                avg_dopamine = sum(dopamine_levels) / len(dopamine_levels)
                print(f"  Average dopamine level: {avg_dopamine:.3f}")
                
            if curiosity_levels:
                avg_curiosity = sum(curiosity_levels) / len(curiosity_levels)
                print(f"  Average curiosity level: {avg_curiosity:.3f}")
        
        # Performance bottlenecks detection
        print(f"{Colors.BOLD}Performance Analysis:{Colors.ENDC}")
        
        if self.performance_data['brain_stats'] and len(self.performance_data['brain_stats']) > 5:
            # Check for performance degradation
            recent_stats = self.performance_data['brain_stats'][-5:]
            firing_rates = [stat.get('firing', 0) for stat in recent_stats]
            
            if len(set(firing_rates)) == 1 and firing_rates[0] == 0:
                print(f"  ⚠ Potential issue: No neurons firing in recent samples")
            elif all(fr == 0 for fr in firing_rates):
                print(f"  ⚠ Warning: Low neural activity detected")
        
        # Memory usage trends
        if self.performance_data['memory_stats'] and len(self.performance_data['memory_stats']) > 5:
            recent_memory = self.performance_data['memory_stats'][-5:]
            trace_counts = [stat.get('traces', 0) for stat in recent_memory]
            
            if max(trace_counts) - min(trace_counts) > 10:  # Large variance
                print(f"  ⚠ High memory usage variance detected")
            elif max(trace_counts) == 0:
                print(f"  ⚠ Working memory appears to be unused")
        
        # Neuromodulation stability
        if self.performance_data['neuromod_stats'] and len(self.performance_data['neuromod_stats']) > 5:
            recent_neuromod = self.performance_data['neuromod_stats'][-5:]
            dopamine_levels = [stat.get('dopamine', 0) for stat in recent_neuromod]
            
            dopamine_variance = max(dopamine_levels) - min(dopamine_levels)
            if dopamine_variance > 0.5:
                print(f"  ⚠ High neuromodulation variance - potential instability")
            elif dopamine_variance < 0.1:
                print(f"  ✓ Stable neuromodulation levels")
        
    def show_visualization(self, args: List[str] = None):
        """Show text-based visualization of brain state"""
        print(f"{Colors.OKCYAN}\n=== Brain Visualization ==={Colors.ENDC}")
        
        if not PYNLM_AVAILABLE or self.brain is None:
            print(f"{Colors.WARNING}Brain not available for visualization{Colors.ENDC}")
            return
        
        # Create a simple text-based visualization
        total_neurons = self.brain.getTotalNeuronCount()
        firing_neurons = self.brain.getFiringNeuronCount()
        
        if total_neurons == 0:
            print(f"{Colors.WARNING}No neurons available for visualization{Colors.ENDC}")
            return
        
        firing_percentage = firing_neurons / total_neurons * 100
        
        # Create a simple ASCII visualization
        print(f"{Colors.BOLD}Neural Activity Heatmap:{Colors.ENDC}")
        
        # Create multiple visualization modes
        if self.visualization_mode == "text":
            self._create_text_visualization(total_neurons, firing_neurons, firing_percentage)
        elif self.visualization_mode == "graph":
            self._create_graph_visualization()
        elif self.visualization_mode == "heatmap":
            self._create_heatmap_visualization()
        else:
            self._create_text_visualization(total_neurons, firing_neurons, firing_percentage)
        
        # Memory visualization
        print(f"{Colors.OKCYAN}\nMemory System Visualization:{Colors.ENDC}")
        self._create_memory_visualization()
        
        # Neuromodulation visualization
        print(f"{Colors.WARNING}Neuromodulation Visualization:{Colors.ENDC}")
        self._create_neuromodulation_visualization()
        
    def _create_text_visualization(self, total_neurons: int, firing_neurons: int, firing_percentage: float):
        """Create simple text-based visualization"""
        # Neuron activity bar
        bar_length = 50
        filled_length = int((firing_percentage / 100) * bar_length)
        
        print(f"  Activity Level:")
        print(f"    [{'#' * filled_length}{'.' * (bar_length - filled_length)}] {firing_percentage:.1f}%")
        
        # Neuron type distribution (if available)
        if hasattr(self.brain, 'getNeuronsByType'):
            neuron_types = self.brain.getNeuronsByType()
            print(f"  Neuron Types:")
            for neuron_type, count in neuron_types.items():
                type_percentage = count / total_neurons * 100
                type_bar_length = 30
                type_filled = int((type_percentage / 100) * type_bar_length)
                print(f"    {neuron_type:<12} [{'#' * type_filled}{'.' * (type_bar_length - type_filled)}] {type_percentage:.1f}%")
        
        # Network density (if available)
        if hasattr(self.brain, 'getTotalSynapseCount'):
            synapses = self.brain.getTotalSynapseCount()
            if total_neurons > 1:
                connection_density = synapses / (total_neurons * (total_neurons - 1))
                density_percentage = connection_density * 100
                density_bar_length = 30
                density_filled = int((density_percentage / 100) * density_bar_length)
                print(f"  Network Density: [{'#' * density_filled}{'.' * (density_bar_length - density_filled)}] {density_percentage:.1f}%")
        
    def _create_graph_visualization(self):
        """Create simple graph visualization"""
        print(f"  (Graph visualization would show network connections)")
        
        # If we have synapse connections, we could create a simple graph
        if hasattr(self.brain, 'getSynapseConnections'):
            connections = self.brain.getSynapseConnections()
            if connections:
                print(f"  Sample connections:")
                for i, conn in enumerate(connections[:5]):  # Show first 5 connections
                    print(f"    {conn.pre_neuron} -> {conn.post_neuron} (weight: {conn.weight:.3f})")
        
    def _create_heatmap_visualization(self):
        """Create heatmap-style visualization"""
        print(f"  Heatmap visualization would show spatial activity distribution")
        
        # If we have regions, we could show activity per region
        if hasattr(self.brain, 'getRegions'):
            regions = self.brain.getRegions()
            if regions:
                print(f"  Activity per region:")
                for i, region in enumerate(regions):
                    neurons = region.getAllNeurons()
                    firing = [n for n in neurons if n.getState().firingState == n.getState().FiringState.Active]
                    percentage = len(firing) / len(neurons) * 100 if neurons else 0
                    print(f"    Region {i}: {len(firing)}/{len(neurons)} neurons ({percentage:.1f}%)")
    
    def _create_memory_visualization(self):
        """Create memory system visualization"""
        if self.brain.getWorkingMemory():
            wm = self.brain.getWorkingMemory()
            traces = wm.getActiveTraces()
            capacity = wm.getCapacity()
            utilization = traces / capacity if capacity > 0 else 0
            
            print(f"  Working Memory: {traces}/{capacity} traces ({utilization*100:.1f}%)")
            
            # Memory bar
            bar_length = 30
            filled_length = int(utilization * bar_length)
            print(f"    [{'#' * filled_length}{'.' * (bar_length - filled_length)}]")
        
        if self.brain.getEpisodicMemory():
            em = self.brain.getEpisodicMemory()
            episodes = em.getEpisodeCount()
            capacity = em.getMaxCapacity()
            utilization = episodes / capacity if capacity > 0 else 0
            
            print(f"  Episodic Memory: {episodes}/{capacity} episodes ({utilization*100:.1f}%)")
            
            bar_length = 30
            filled_length = int(utilization * bar_length)
            print(f"    [{'#' * filled_length}{'.' * (bar_length - filled_length)}]")
    
    def _create_neuromodulation_visualization(self):
        """Create neuromodulation visualization"""
        if self.agent:
            curiosity = self.agent.getCuriosityLevel()
            novelty = self.agent.getNoveltyLevel()
            dopamine = self.agent.getNeuromodulationLevel()
            
            print(f"  Neuromodulation Levels:")
            
            # Curiosity meter
            curiosity_bar = int(curiosity * 30)
            print(f"    Curiosity [{'#' * curiosity_bar}{'.' * (30 - curiosity_bar)}] {curiosity:.3f}")
            
            # Novelty meter
            novelty_bar = int(novelty * 30)
            print(f"    Novelty  [{'#' * novelty_bar}{'.' * (30 - novelty_bar)}] {novelty:.3f}")
            
            # Dopamine meter
            dopamine_pos = int((dopamine + 1) * 15)  # Map from -1..1 to 0..30
            dopamine_bar = " " * dopamine_pos + "#" * (30 - dopamine_pos)
            print(f"    Dopamine [{' ' * dopamine_pos}###] {dopamine:.3f}")
        
    def show_configuration(self, args: List[str] = None):
        """Show current configuration"""
        print(f"{Colors.OKCYAN}\n=== Configuration ==={Colors.ENDC}")
        
        if not PYNLM_AVAILABLE or self.brain is None:
            print(f"{Colors.WARNING}Brain not available{Colors.ENDC}")
            return
        
        # Get configuration from brain (if available)
        if hasattr(self.brain, 'getConfig'):
            config = self.brain.getConfig()
            print(f"{Colors.OKGREEN}Brain Configuration:{Colors.ENDC}")
            
            # Show key configuration values
            config_keys = ['neuron_count', 'region_count', 'connection_probability']
            for key in config_keys:
                if config.has(key):
                    value = config.getOr(key, "N/A")
                    print(f"  {key}: {value}")
        
        # Agent configuration (if available)
        if self.agent and hasattr(self.agent, 'getConfig'):
            agent_config = self.agent.getConfig()
            print(f"{Colors.OKCYAN}Agent Configuration:{Colors.ENDC}")
            
            agent_keys = ['enable_reward_modulation', 'enable_curiosity', 
                         'enable_structural_plasticity', 'enable_development']
            for key in agent_keys:
                if agent_config.has(key):
                    value = agent_config.getOr(key, False)
                    print(f"  {key}: {value}")
        
        # Show debug settings
        print(f"{Colors.WARNING}Debug Settings:{Colors.ENDC}")
        print(f"  Debug mode: {self.debug_mode}")
        print(f"  Visualization mode: {self.visualization_mode}")
        print(f"  Update interval: {self.update_interval}s")
        print(f"  Performance monitoring: {'Enabled' if self.performance_data['timestamps'] else 'Disabled'}")
        
    def start_trace(self, args: List[str] = None):
        """Start detailed operation tracing"""
        print(f"{Colors.OKCYAN}\n=== Starting Operation Trace ==={Colors.ENDC}")
        
        if self.debug_mode == "trace":
            print(f"{Colors.WARNING}Trace mode already active{Colors.ENDC}")
            return
        
        # Initialize trace data
        self.trace_data = {
            'start_time': time.time(),
            'operations': [],
            'brain_states': [],
            'performance_samples': []
        }
        
        self.debug_mode = "trace"
        print(f"{Colors.OKGREEN}Trace mode enabled. Recording all operations...{Colors.ENDC}")
        print("Available trace commands:")
        print("  status  - Show current system status")
        print("  brain   - Record brain state")
        print("  memory  - Record memory state")
        print("  neuromod - Record neuromodulation state")
        print("  stop    - Stop tracing and save data")
        
    def stop_debugging(self, args: List[str] = None):
        """Stop debugging session"""
        print(f"{Colors.OKCYAN}\n=== Stopping Debugging ==={Colors.ENDC}")
        
        if self.debug_mode == "passive":
            print(f"{Colors.WARNING}Already in passive mode{Colors.ENDC}")
            return
        
        if hasattr(self, 'trace_data'):
            # Save trace data
            trace_path = f"debug_trace_{int(time.time())}.json"
            with open(trace_path, 'w') as f:
                json.dump(self.trace_data, f, indent=2)
            print(f"{Colors.OKGREEN}Trace data saved to: {trace_path}{Colors.ENDC}")
            
            # Analyze trace data
            self.analyze_trace_data()
        
        self.debug_mode = "passive"
        print(f"{Colors.OKGREEN}Debugging stopped. Enter 'help' for available commands.{Colors.ENDC}")
        
    def analyze_trace_data(self):
        """Analyze collected trace data"""
        if not hasattr(self, 'trace_data'):
            return
        
        operations = self.trace_data.get('operations', [])
        if not operations:
            print(f"{Colors.WARNING}No operations recorded{Colors.ENDC}")
            return
        
        print(f"{Colors.OKCYAN}Trace Analysis:{Colors.ENDC}")
        
        # Operation frequency
        operation_types = {}
        for op in operations:
            op_type = op.get('type', 'unknown')
            operation_types[op_type] = operation_types.get(op_type, 0) + 1
        
        print(f"  Operations recorded: {len(operations)}")
        print(f"  Operation types: {len(operation_types)}")
        
        for op_type, count in operation_types.items():
            print(f"    {op_type}: {count}")
        
        # Timeline analysis
        if operations:
            start_time = self.trace_data['start_time']
            end_time = time.time()
            duration = end_time - start_time
            
            print(f"  Trace duration: {duration:.1f}s")
            print(f"  Operations per second: {len(operations) / duration:.1f}")
        
    def save_debug_session(self, args: List[str] = None):
        """Save current debug session"""
        print(f"{Colors.OKCYAN}\n=== Saving Debug Session ==={Colors.ENDC}")
        
        session_data = {
            'timestamp': time.time(),
            'debug_mode': self.debug_mode,
            'visualization_mode': self.visualization_mode,
            'update_interval': self.update_interval,
            'performance_data': self.performance_data,
            'brain_state': self._capture_brain_state() if PYNLM_AVAILABLE and self.brain else None
        }
        
        session_path = f"debug_session_{int(time.time())}.json"
        
        try:
            with open(session_path, 'w') as f:
                json.dump(session_data, f, indent=2)
            print(f"{Colors.OKGREEN}Debug session saved to: {session_path}{Colors.ENDC}")
        except Exception as e:
            print(f"{Colors.FAIL}Error saving session: {e}{Colors.ENDC}")
        
    def load_debug_session(self, args: List[str] = None):
        """Load a debug session"""
        print(f"{Colors.OKCYAN}\n=== Loading Debug Session ==={Colors.ENDC}")
        
        # Find available sessions
        sessions_dir = Path("debug_sessions")
        if not sessions_dir.exists():
            print(f"{Colors.WARNING}No debug sessions directory found{Colors.ENDC}")
            return
        
        sessions = list(sessions_dir.glob("debug_session_*.json"))
        if not sessions:
            print(f"{Colors.WARNING}No debug sessions found{Colors.ENDC}")
            return
        
        print(f"Available sessions:")
        for session in sessions:
            try:
                with open(session, 'r') as f:
                    data = json.load(f)
                    timestamp = data.get('timestamp', 0)
                    time_str = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(timestamp))
                    print(f"  {session.name:<35} ({time_str})")
            except:
                print(f"  {session.name:<35} (unknown details)")
        
        # Load selected session
        print(f"\nEnter session name to load (or press Enter to skip):")
        session_name = input("> ").strip()
        
        if not session_name:
            return
            
        session_path = sessions_dir / session_name
        if not session_path.exists():
            print(f"{Colors.FAIL}Session not found: {session_name}{Colors.ENDC}")
            return
        
        try:
            with open(session_path, 'r') as f:
                session_data = json.load(f)
            
            # Restore session state
            self.debug_mode = session_data.get('debug_mode', 'passive')
            self.visualization_mode = session_data.get('visualization_mode', 'text')
            self.update_interval = session_data.get('update_interval', 1.0)
            self.performance_data = session_data.get('performance_data', {'brain_stats': [], 'memory_stats': [], 'neuromod_stats': [], 'timestamps': []})
            
            print(f"{Colors.OKGREEN}Debug session loaded: {session_name}{Colors.ENDC}")
            print(f"  Restored settings: mode={self.debug_mode}, visualization={self.visualization_mode}")
            
        except Exception as e:
            print(f"{Colors.FAIL}Error loading session: {e}{Colors.ENDC}")
    
    def _capture_brain_state(self):
        """Capture current brain state for debugging"""
        if not PYNLM_AVAILABLE or self.brain is None:
            return None
        
        state = {
            'timestamp': time.time(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'firing_count': self.brain.getFiringNeuronCount(),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'working_memory': self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0,
            'episodic_memory': self.brain.getEpisodicMemory().getEpisodeCount() if self.brain.getEpisodicMemory() else 0,
            'dopamine_level': self.brain.getDopamine().getLevel() if self.brain.getDopamine() else 0,
            'curiosity_level': self.brain.getCuriosity().getLevel() if self.brain.getCuriosity() else 0,
            'novelty_level': self.brain.getNovelty().getLevel() if self.brain.getNovelty() else 0
        }
        
        return state
    
    def clear_screen(self, args: List[str] = None):
        """Clear the screen"""
        print(f"{Colors.CLEAR}")
        self.print_header()
    
    def refresh_display(self, args: List[str] = None):
        """Refresh the display with current status"""
        print(f"{Colors.OKCYAN}\n=== Refreshing Display ==={Colors.ENDC}")
        
        if PYNLM_AVAILABLE and self.brain:
            self.show_system_status()
        else:
            print(f"{Colors.WARNING}Brain not available for refresh{Colors.ENDC}")
    
    def quit_debug(self, args: List[str] = None):
        """Quit debug console"""
        print(f"{Colors.OKCYAN}\n=== Quitting Debug Console ==={Colors.ENDC}")
        
        # Stop any monitoring
        if self.monitor_thread:
            self.stop_monitor.set()
            self.monitor_thread.join(timeout=5)
        
        print(f"{Colors.OKGREEN}Debug console closed. Goodbye!{Colors.ENDC}")
        sys.exit(0)
        
    def create_plots(self, args: List[str] = None):
        """Create data plots and graphs"""
        print(f"{Colors.OKCYAN}\n=== Creating Data Plots ==={Colors.ENDC}")
        
        # This would create actual plots using matplotlib or similar
        # For now, we'll create text-based representations
        
        if not self.performance_data['timestamps']:
            print(f"{Colors.WARNING}No performance data available for plotting{Colors.ENDC}")
            return
        
        timestamps = self.performance_data['timestamps']
        if len(timestamps) < 2:
            print(f"{Colors.WARNING}Insufficient data for plotting{Colors.ENDC}")
            return
        
        print(f"  Creating performance plots...")
        print(f"    Time range: {timestamps[0]:.1f}s to {timestamps[-1]:.1f}s")
        print(f"    Data points: {len(timestamps)}")
        
        # Create simple text plot of firing rates
        if self.performance_data['brain_stats']:
            brain_stats = self.performance_data['brain_stats']
            firing_rates = [stat.get('firing', 0) for stat in brain_stats]
            
            if firing_rates:
                avg_firing = sum(firing_rates) / len(firing_rates)
                max_firing = max(firing_rates)
                
                print(f"    Firing rate plot (avg={avg_firing:.1f}, max={max_firing:.1f}):")
                
                # Simple ASCII plot
                plot_height = 10
                plot_width = 40
                
                for row in range(plot_height, 0, -1):
                    threshold = (row / plot_height) * max_firing
                    line = "  "
                    for fr in firing_rates:
                        if fr >= threshold:
                            line += "*"
                        else:
                            line += " "
                    print(f"    {row:2d} {line}")
                
                print(f"    0    10    20    30    40 (data points)")
        
        print(f"{Colors.OKCYAN}Plot creation complete (text-based representation){Colors.ENDC}")
        
    def export_data(self, args: List[str] = None):
        """Export debug data to various formats"""
        print(f"{Colors.OKCYAN}\n=== Exporting Debug Data ==={Colors.ENDC}")
        
        export_formats = ['json', 'csv', 'txt', 'xml']
        
        print(f"Available export formats:")
        for i, fmt in enumerate(export_formats):
            print(f"  {i+1}. {fmt.upper()}")
        
        print(f"\nEnter format number (or 'all' for all formats):")
        choice = input("> ").strip().lower()
        
        if choice == 'all':
            formats = export_formats
        elif choice.isdigit() and 1 <= int(choice) <= len(export_formats):
            formats = [export_formats[int(choice)-1]]
        else:
            print(f"{Colors.WARNING}Invalid choice. Using JSON format.{Colors.ENDC}")
            formats = ['json']
        
        export_base = f"debug_export_{int(time.time())}"
        
        for fmt in formats:
            try:
                if fmt == 'json':
                    filename = f"{export_base}.json"
                    with open(filename, 'w') as f:
                        json.dump(self.performance_data, f, indent=2)
                    print(f"  ✓ JSON exported to: {filename}")
                
                elif fmt == 'csv':
                    filename = f"{export_base}.csv"
                    with open(filename, 'w') as f:
                        f.write("timestamp,brain_neurons,brain_synapses,brain_firing,brain_firing_rate,working_memory,episodic_memory,dopamine,curiosity,novelty\n")
                        for i, timestamp in enumerate(self.performance_data['timestamps']):
                            brain_stat = self.performance_data['brain_stats'][i] if i < len(self.performance_data['brain_stats']) else {}
                            memory_stat = self.performance_data['memory_stats'][i] if i < len(self.performance_data['memory_stats']) else {}
                            neuromod_stat = self.performance_data['neuromod_stats'][i] if i < len(self.performance_data['neuromod_stats']) else {}
                            
                            f.write(f"{timestamp},{brain_stat.get('neurons', 0)},{brain_stat.get('synapses', 0)},{brain_stat.get('firing', 0)},{brain_stat.get('firing_rate', 0)},{memory_stat.get('traces', 0)},{memory_stat.get('episodes', 0)},{neuromod_stat.get('dopamine', 0)},{neuromod_stat.get('curiosity', 0)},{neuromod_stat.get('novelty', 0)}\n")
                    print(f"  ✓ CSV exported to: {filename}")
                
                elif fmt == 'txt':
                    filename = f"{export_base}.txt"
                    with open(filename, 'w') as f:
                        f.write("NLM Debug Data Export\n")
                        f.write(f"Generated at: {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
                        f.write(f"Duration: {len(self.performance_data['timestamps'])} samples\n\n")
                        
                        if self.performance_data['brain_stats']:
                            f.write("Brain Statistics:\n")
                            for i, stat in enumerate(self.performance_data['brain_stats']):
                                f.write(f"  Sample {i}: {stat.get('neurons', 0)} neurons, {stat.get('firing', 0)} firing\n")
                        
                        if self.performance_data['memory_stats']:
                            f.write("\nMemory Statistics:\n")
                            for i, stat in enumerate(self.performance_data['memory_stats']):
                                f.write(f"  Sample {i}: {stat.get('traces', 0)} working memory traces, {stat.get('episodes', 0)} episodes\n")
                        
                        if self.performance_data['neuromod_stats']:
                            f.write("\nNeuromodulation Statistics:\n")
                            for i, stat in enumerate(self.performance_data['neuromod_stats']):
                                f.write(f"  Sample {i}: dopamine={stat.get('dopamine', 0):.3f}, "
                                       f"curiosity={stat.get('curiosity', 0):.3f}, "
                                       f"novelty={stat.get('novelty', 0):.3f}\n")
                    print(f"  ✓ TXT exported to: {filename}")
                
                elif fmt == 'xml':
                    filename = f"{export_base}.xml"
                    with open(filename, 'w') as f:
                        f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
                        f.write('<nlm_debug_data>\n')
                        f.write(f'  <timestamp>{time.time()}</timestamp>\n')
                        f.write(f'  <samples>{len(self.performance_data["timestamps"])}</samples>\n')
                        f.write('  <brain_statistics>\n')
                        
                        for stat in self.performance_data['brain_stats']:
                            f.write(f'    <sample>\n')
                            f.write(f'      <neurons>{stat.get("neurons", 0)}</neurons>\n')
                            f.write(f'      <synapses>{stat.get("synapses", 0)}</synapses>\n')
                            f.write(f'      <firing>{stat.get("firing", 0)}</firing>\n')
                            f.write(f'      <firing_rate>{stat.get("firing_rate", 0)}</firing_rate>\n')
                            f.write(f'    </sample>\n')
                        
                        f.write('  </brain_statistics>\n')
                        f.write('</nlm_debug_data>\n')
                    print(f"  ✓ XML exported to: {filename}")
                
            except Exception as e:
                print(f"  ✗ Error exporting {fmt.upper()}: {e}")
        
        print(f"{Colors.OKCYAN}Data export complete{Colors.ENDC}")


def main():
    """Main entry point for the NLM debug console"""
    console = NLMDebugConsole()
    
    parser = argparse.ArgumentParser(
        description="NLM Debug Console - Advanced Brain Inspection and Visualization",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-debug-console.py                    # Start interactive debug console
  nlm-debug-console.py --passive           # Start in passive mode (default)
  nlm-debug-console.py --active            # Start in active monitoring mode
  nlm-debug-console.py --trace            # Start in detailed trace mode
        """
    )
    
    parser.add_argument(
        '--mode', '-m',
        choices=['passive', 'active', 'trace'],
        default='passive',
        help='Debug mode (default: passive)'
    )
    
    parser.add_argument(
        '--visualization', '-v',
        choices=['text', 'graph', 'heatmap'],
        default='text',
        help='Visualization mode (default: text)'
    )
    
    parser.add_argument(
        '--interval', '-i',
        type=float,
        default=1.0,
        help='Update interval in seconds (default: 1.0)'
    )
    
    parser.add_argument(
        '--init', '-I',
        help='Initialize brain with configuration file'
    )
    
    args = parser.parse_args()
    
    # Set console properties
    console.debug_mode = args.mode
    console.visualization_mode = args.visualization
    console.update_interval = args.interval
    
    # Initialize brain if configuration provided
    if args.init and os.path.exists(args.init):
        try:
            import pynlm
            config = pynlm.createDefaultConfig()
            # Load config file logic here
            console.brain = pynlm.createBrain(config)
            console.brain.initialize()
            print(f"{Colors.OKGREEN}Brain initialized from: {args.init}{Colors.ENDC}")
        except Exception as e:
            print(f"{Colors.FAIL}Error initializing brain: {e}{Colors.ENDC}")
    
    # Start monitoring if in active mode
    if console.debug_mode == 'active':
        console.monitor_thread = threading.Thread(target=console._monitor_loop)
        console.monitor_thread.daemon = True
        console.monitor_thread.start()
    
    # Print header and show help
    console.print_header()
    console.show_help()
    
    # Main command loop
    print(f"\n{Colors.OKCYAN}Debug console ready. Type 'help' for commands.{Colors.ENDC}")
    
    try:
        while True:
            try:
                command_line = input(f"\n{Colors.OKCYAN}debug{Colors.ENDC} > ").strip()
                
                if not command_line:
                    continue
                
                # Parse command
                parts = command_line.split()
                command = parts[0].lower()
                args = parts[1:] if len(parts) > 1 else []
                
                # Execute command
                if command in console.debug_commands:
                    console.debug_commands[command](args)
                else:
                    print(f"{Colors.FAIL}Unknown command: {command}{Colors.ENDC}")
                    print(f"Type 'help' for available commands")
                    
            except KeyboardInterrupt:
                print(f"\n{Colors.WARNING}Interrupted. Use 'stop' or 'quit' to exit.{Colors.ENDC}")
                continue
                
    except Exception as e:
        print(f"{Colors.FAIL}Error in debug console: {e}{Colors.ENDC}")
        import traceback
        traceback.print_exc()
    finally:
        # Cleanup
        if console.debug_mode == 'active' and console.monitor_thread:
            console.stop_monitor.set()
            console.monitor_thread.join(timeout=5)


if __name__ == "__main__":
    main()