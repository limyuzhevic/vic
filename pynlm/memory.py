"""
Memory systems and learning storage.

This module contains memory systems and advanced memory management
features for expert users.
"""

from .core import *

class MemoryManager:
    """
    Advanced memory manager for expert users.
    
    Provides working memory, episodic memory, and semantic memory
    systems with advanced memory management features.
    """
    
    def __init__(self):
        self._working_memory = {}
        self._episodic_memory = []
        self._semantic_memory = {}
        self._procedural_memory = {}
        self._associative_memory = {}
        self._memory_usage = {
            'working_memory': 0,
            'episodic_memory': 0,
            'semantic_memory': 0,
            'procedural_memory': 0,
            'associative_memory': 0
        }
        self._memory_capacity = {
            'working_memory': 100,
            'episodic_memory': 1000,
            'semantic_memory': 10000,
            'procedural_memory': 1000,
            'associative_memory': 5000
        }
        self._memory_consolidation_schedule = {}
        
    def store_working_memory(self, key, value, priority=1.0, decay_rate=0.01):
        """
        Store information in working memory.
        
        Args:
            key: Memory key
            value: Memory value
            priority: Memory priority (0.0 to 1.0)
            decay_rate: Memory decay rate
        """
        if self._memory_usage['working_memory'] >= self._memory_capacity['working_memory']:
            # Remove lowest priority item
            min_priority_key = min(self._working_memory.keys(), 
                                 key=lambda k: self._working_memory[k].get('priority', 0))
            del self._working_memory[min_priority_key]
        
        self._working_memory[key] = {
            'value': value,
            'priority': priority,
            'decay_rate': decay_rate,
            'timestamp': time.time() if 'time' in globals() else 0.0,
            'access_count': 0
        }
        self._memory_usage['working_memory'] = len(self._working_memory)
        
    def retrieve_working_memory(self, key):
        """
        Retrieve information from working memory.
        
        Args:
            key: Memory key
            
        Returns:
            Memory value or None if not found
        """
        if key in self._working_memory:
            item = self._working_memory[key]
            item['access_count'] = item.get('access_count', 0) + 1
            item['timestamp'] = time.time() if 'time' in globals() else 0.0
            return item['value']
        return None
        
    def store_episodic_memory(self, episode):
        """
        Store an episodic memory.
        
        Args:
            episode: Episodic memory data
        """
        if self._memory_usage['episodic_memory'] >= self._memory_capacity['episodic_memory']:
            # Remove oldest episode
            self._episodic_memory.pop(0)
        
        self._episodic_memory.append({
            'data': episode,
            'timestamp': time.time() if 'time' in globals() else 0.0,
            'importance': self._calculate_episodic_importance(episode)
        })
        self._memory_usage['episodic_memory'] = len(self._episodic_memory)
        
    def retrieve_episodic_memory(self, time_range=None, keywords=None, importance_threshold=0.0):
        """
        Retrieve episodic memories within specified criteria.
        
        Args:
            time_range: Tuple of (start_time, end_time)
            keywords: List of keywords to search for
            importance_threshold: Minimum importance threshold
            
        Returns:
            list: Retrieved episodes
        """
        retrieved = []
        
        for episode in self._episodic_memory:
            include = True
            
            # Time range filter
            if time_range:
                episode_time = episode['timestamp']
                if episode_time < time_range[0] or episode_time > time_range[1]:
                    include = False
            
            # Keyword filter
            if keywords and include:
                episode_data = episode['data']
                if isinstance(episode_data, dict):
                    text_to_search = ' '.join(str(v) for v in episode_data.values())
                else:
                    text_to_search = str(episode_data)
                
                for keyword in keywords:
                    if keyword.lower() not in text_to_search.lower():
                        include = False
                        break
            
            # Importance filter
            if include and episode['importance'] < importance_threshold:
                include = False
            
            if include:
                retrieved.append(episode)
        
        return retrieved
        
    def store_semantic_memory(self, concept, representation, strength=1.0):
        """
        Store semantic memory (concept representations).
        
        Args:
            concept: Concept name
            representation: Concept representation
            strength: Memory strength (0.0 to 1.0)
        """
        if self._memory_usage['semantic_memory'] >= self._memory_capacity['semantic_memory']:
            # Remove weakest concept
            min_strength_concept = min(self._semantic_memory.keys(), 
                                      key=lambda k: self._semantic_memory[k].get('strength', 0))
            del self._semantic_memory[min_strength_concept]
        
        self._semantic_memory[concept] = {
            'representation': representation,
            'strength': strength,
            'last_accessed': time.time() if 'time' in globals() else 0.0,
            'usage_count': 0
        }
        self._memory_usage['semantic_memory'] = len(self._semantic_memory)
        
    def retrieve_semantic_memory(self, concept, threshold=0.0):
        """
        Retrieve semantic memory for a concept.
        
        Args:
            concept: Concept name
            threshold: Minimum strength threshold
            
        Returns:
            Semantic memory or None if not found or below threshold
        """
        if concept in self._semantic_memory:
            item = self._semantic_memory[concept]
            if item['strength'] >= threshold:
                item['last_accessed'] = time.time() if 'time' in globals() else 0.0
                item['usage_count'] = item.get('usage_count', 0) + 1
                return item['representation']
        return None
        
    def store_procedural_memory(self, skill_name, procedure, effectiveness=1.0):
        """
        Store procedural memory (skills and procedures).
        
        Args:
            skill_name: Skill name
            procedure: Procedure or skill implementation
            effectiveness: Procedure effectiveness (0.0 to 1.0)
        """
        if self._memory_usage['procedural_memory'] >= self._memory_capacity['procedural_memory']:
            # Remove least effective procedure
            min_effectiveness = min(self._procedural_memory.values(), 
                                   key=lambda v: v.get('effectiveness', 0))
            for name, data in list(self._procedural_memory.items()):
                if data.get('effectiveness', 0) == min_effectiveness['effectiveness']:
                    del self._procedural_memory[name]
                    break
        
        self._procedural_memory[skill_name] = {
            'procedure': procedure,
            'effectiveness': effectiveness,
            'last_practiced': time.time() if 'time' in globals() else 0.0,
            'success_count': 0,
            'total_attempts': 0
        }
        self._memory_usage['procedural_memory'] = len(self._procedural_memory)
        
    def practice_procedural_memory(self, skill_name, success=True):
        """
        Practice a procedural memory (skill).
        
        Args:
            skill_name: Skill name
            success: Whether the practice was successful
            
        Returns:
            float: Updated effectiveness
        """
        if skill_name in self._procedural_memory:
            item = self._procedural_memory[skill_name]
            item['total_attempts'] = item.get('total_attempts', 0) + 1
            item['success_count'] = item.get('success_count', 0) + (1 if success else 0)
            
            # Update effectiveness based on performance
            success_rate = item['success_count'] / item['total_attempts']
            item['effectiveness'] = min(1.0, item['effectiveness'] + 0.01 * (success_rate - 0.5))
            item['last_practiced'] = time.time() if 'time' in globals() else 0.0
            
            return item['effectiveness']
        
        return 0.0
        
    def store_associative_memory(self, key1, key2, strength=1.0, associative_type="bidirectional"):
        """
        Store associative memory (Hebbian learning).
n        
        Args:
            key1: First key
            key2: Second key
            strength: Association strength (0.0 to 1.0)
            associative_type: Type of association (bidirectional, unidirectional)
        """
        if self._memory_usage['associative_memory'] >= self._memory_capacity['associative_memory']:
            # Remove weakest association
            min_strength = min(self._associative_memory.values(), key=lambda v: v.get('strength', 0))
            for assoc_key, data in list(self._associative_memory.items()):
                if data.get('strength', 0) == min_strength['strength']:
                    del self._associative_memory[assoc_key]
                    break
        
        association_key = f"{key1}_{key2}"
        self._associative_memory[association_key] = {
            'key1': key1,
            'key2': key2,
            'strength': strength,
            'type': associative_type,
            'last_activated': time.time() if 'time' in globals() else 0.0
        }
        self._memory_usage['associative_memory'] = len(self._associative_memory)
        
    def retrieve_associative_memory(self, key1=None, key2=None, strength_threshold=0.0):
        """
        Retrieve associative memories matching criteria.
        
        Args:
            key1: First key (optional)
            key2: Second key (optional)
            strength_threshold: Minimum strength threshold
            
        Returns:
            list: Retrieved associations
        """
        retrieved = []
        
        for assoc_key, data in self._associative_memory.items():
            include = True
            
            if key1 and data['key1'] != key1:
                include = False
            if key2 and data['key2'] != key2:
                include = False
            if data['strength'] < strength_threshold:
                include = False
            
            if include:
                retrieved.append(data)
        
        return retrieved
        
    def consolidate_memories(self, consolidation_strength=0.5):
        """
        Consolidate memories (move from short-term to long-term).
        
        Args:
            consolidation_strength: Consolidation strength (0.0 to 1.0)
        """
        current_time = time.time() if 'time' in globals() else 0.0
        
        # Consolidate episodic memories
        for episode in self._episodic_memory:
            age = current_time - episode['timestamp']
            if age > 3600:  # 1 hour
                episode['importance'] *= (1.0 + consolidation_strength)
        
        # Consolidate semantic memories
        for concept, data in self._semantic_memory.items():
            age = current_time - data['last_accessed']
            if age > 1800:  # 30 minutes
                data['strength'] = min(1.0, data['strength'] * (1.0 + consolidation_strength))
        
        # Consolidate procedural memories
        for skill, data in self._procedural_memory.items():
            age = current_time - data['last_practiced']
            if age > 7200:  # 2 hours
                data['effectiveness'] = min(1.0, data['effectiveness'] * (1.0 + consolidation_strength))
        
        print(f"[INFO] Memory consolidation completed with strength {consolidation_strength}")
        
    def get_memory_usage(self):
        """
        Get comprehensive memory usage information.
        
        Returns:
            dict: Memory usage statistics
        """
        total_capacity = sum(self._memory_capacity.values())
        total_used = sum(self._memory_usage.values())
        
        return {
            'memory_usage': self._memory_usage.copy(),
            'memory_capacity': self._memory_capacity.copy(),
            'total_capacity': total_capacity,
            'total_used': total_used,
            'utilization_rate': total_used / total_capacity if total_capacity > 0 else 0,
            'episodic_count': len(self._episodic_memory),
            'semantic_count': len(self._semantic_memory),
            'procedural_count': len(self._procedural_memory),
            'associative_count': len(self._associative_memory)
        }
        
    def clear_memory(self, memory_type=None):
        """
        Clear specific memory type or all memories.
        
        Args:
            memory_type: Memory type to clear (working, episodic, semantic, procedural, associative)
        """
        if memory_type is None:
            # Clear all memories
            self._working_memory.clear()
            self._episodic_memory.clear()
            self._semantic_memory.clear()
            self._procedural_memory.clear()
            self._associative_memory.clear()
            self._memory_usage = {k: 0 for k in self._memory_usage}
            print("[INFO] All memory cleared")
        else:
            # Clear specific memory type
            memory_type = memory_type.lower()
            if memory_type == 'working':
                self._working_memory.clear()
                self._memory_usage['working_memory'] = 0
            elif memory_type == 'episodic':
                self._episodic_memory.clear()
                self._memory_usage['episodic_memory'] = 0
            elif memory_type == 'semantic':
                self._semantic_memory.clear()
                self._memory_usage['semantic_memory'] = 0
            elif memory_type == 'procedural':
                self._procedural_memory.clear()
                self._memory_usage['procedural_memory'] = 0
            elif memory_type == 'associative':
                self._associative_memory.clear()
                self._memory_usage['associative_memory'] = 0
            else:
                print(f"[ERROR] Unknown memory type: {memory_type}")
                
    def export_memory(self, filepath, format="json"):
        """
        Export memory to file.
        
        Args:
            filepath: Path to export file
            format: Export format (json, txt)
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            import json
            
            data = {
                'working_memory': self._working_memory,
                'episodic_memory': self._episodic_memory,
                'semantic_memory': self._semantic_memory,
                'procedural_memory': self._procedural_memory,
                'associative_memory': self._associative_memory,
                'memory_usage': self._memory_usage,
                'memory_capacity': self._memory_capacity
            }
            
            if format.lower() == "json":
                with open(filepath, 'w') as f:
                    json.dump(data, f, default=str, indent=2)
            elif format.lower() == "txt":
                with open(filepath, 'w') as f:
                    f.write("Memory Export\n")
                    f.write(f"Working Memory: {len(self._working_memory)} items\n")
                    f.write(f"Episodic Memory: {len(self._episodic_memory)} items\n")
                    f.write(f"Semantic Memory: {len(self._semantic_memory)} items\n")
                    f.write(f"Procedural Memory: {len(self._procedural_memory)} items\n")
                    f.write(f"Associative Memory: {len(self._associative_memory)} items\n")
            else:
                print(f"[ERROR] Unsupported format: {format}")
                return False
            
            print(f"[INFO] Memory exported to {filepath} ({format})")
            return True
            
        except Exception as e:
            print(f"[ERROR] Failed to export memory: {e}")
            return False
            
    def import_memory(self, filepath, format="json"):
        """
        Import memory from file.
        
        Args:
            filepath: Path to import file
            format: Import format (json, txt)
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            import json
            
            if format.lower() == "json":
                with open(filepath, 'r') as f:
                    data = json.load(f)
                    
                self._working_memory = data.get('working_memory', {})
                self._episodic_memory = data.get('episodic_memory', [])
                self._semantic_memory = data.get('semantic_memory', {})
                self._procedural_memory = data.get('procedural_memory', {})
                self._associative_memory = data.get('associative_memory', {})
                self._memory_usage = data.get('memory_usage', self._memory_usage)
                self._memory_capacity = data.get('memory_capacity', self._memory_capacity)
            
            elif format.lower() == "txt":
                # Parse text format (simplified)
                with open(filepath, 'r') as f:
                    lines = f.readlines()
                
                # Basic parsing logic for text format
                # This would need to be more sophisticated for real use
                pass
            
            else:
                print(f"[ERROR] Unsupported format: {format}")
                return False
            
            print(f"[INFO] Memory imported from {filepath} ({format})")
            return True
            
        except Exception as e:
            print(f"[ERROR] Failed to import memory: {e}")
            return False
            
    def _calculate_episodic_importance(self, episode):
        """Calculate importance of episodic memory."""
        # Simple importance calculation based on episode content
        if isinstance(episode, dict):
            importance = 0.5
            for key, value in episode.items():
                if isinstance(value, (int, float)):
                    importance += abs(value) * 0.1
                elif isinstance(value, str):
                    importance += len(value) * 0.01
            return min(1.0, importance)
        else:
            return 1.0
            
    def __str__(self):
        """String representation of memory manager."""
        return f"MemoryManager(working={len(self._working_memory)}, " \
               f"episodic={len(self._episodic_memory)}, " \
               f"semantic={len(self._semantic_memory)}, " \
               f"procedural={len(self._procedural_memory)}, " \
               f"associative={len(self._associative_memory)})"
        
    def __repr__(self):
        """Detailed representation of memory manager."""
        return f"MemoryManager(working={len(self._working_memory)}, " \
               f"episodic={len(self._episodic_memory)}, " \
               f"semantic={len(self._semantic_memory)}, " \
               f"procedural={len(self._procedural_memory)}, " \
               f"associative={len(self._associative_memory)})"
# Export advanced memory classes and functions
from .memory import MemoryManager