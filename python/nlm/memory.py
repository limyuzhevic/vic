"""Memory system integration for NLM Python API."""

import numpy as np
from typing import Dict, List, Optional, Any, Tuple
from collections import deque
import json
from pathlib import Path
import heapq
from dataclasses import dataclass, field
from enum import Enum
import time


class MemoryType(Enum):
    WORKING = "working"
    EPISODIC = "episodic"
    ASSOCIATIVE = "associative"


@dataclass
class MemoryEntry:
    """Base class for memory entries."""
    id: str
    timestamp: float
    strength: float = 1.0
    accessed: float = 0.0
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            "id": self.id,
            "timestamp": self.timestamp,
            "strength": self.strength,
            "accessed": self.accessed
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'MemoryEntry':
        return cls(**data)


class WorkingMemory:
    """Working memory with persistent activity and competition."""
    
    def __init__(self, capacity: int = 50, decay_rate: float = 0.01):
        """
        Initialize working memory.
        
        Args:
            capacity: Maximum number of items in working memory
            decay_rate: Rate at which memory items decay
        """
        self.capacity = capacity
        self.decay_rate = decay_rate
        self.items: Dict[str, MemoryEntry] = {}
        self.active_items: deque = deque(maxlen=capacity)
        self.compensation_matrix = np.eye(capacity) if capacity > 0 else np.array([])
        
    def add_item(self, item_id: str, timestamp: float, data: Any, strength: float = 1.0):
        """Add an item to working memory."""
        if len(self.items) >= self.capacity:
            # Remove least recently used item
            oldest_id = min(self.items.keys(), key=lambda k: self.items[k].accessed)
            del self.items[oldest_id]
            if oldest_id in self.active_items:
                self.active_items.remove(oldest_id)
                
        entry = MemoryEntry(item_id, timestamp, strength)
        self.items[item_id] = entry
        self.active_items.append(item_id)
        
    def get_item(self, item_id: str) -> Optional[MemoryEntry]:
        """Get an item from working memory and update access time."""
        if item_id in self.items:
            self.items[item_id].accessed = time.time()
            return self.items[item_id]
        return None
        
    def decay_all(self) -> None:
        """Apply decay to all items."""
        for entry in self.items.values():
            entry.strength *= (1 - self.decay_rate)
            
    def get_active_items(self) -> List[str]:
        """Get currently active items."""
        return list(self.active_items)
        
    def get_competition_strength(self, item_id: str) -> float:
        """Get competition strength for an item."""
        if item_id not in self.items:
            return 0.0
            
        # Calculate competition based on other active items
        competition = 0.0
        for other_id in self.active_items:
            if other_id != item_id:
                competition += self.items[other_id].strength
                
        return self.items[item_id].strength / (competition + 1.0)


class EpisodicMemory:
    """Episodic memory with experience encoding and replay."""
    
    def __init__(self, capacity: int = 1000, consolidation_threshold: float = 0.7):
        """
        Initialize episodic memory.
        
        Args:
            capacity: Maximum number of episodes
            consolidation_threshold: Threshold for consolidation
        """
        self.capacity = capacity
        self.consolidation_threshold = consolidation_threshold
        self.episodes: List[Dict[str, Any]] = []
        self.episode_index: Dict[str, int] = {}
        self.replay_queue: deque = deque(maxlen=100)
        self.consolidated_episodes: List[Dict[str, Any]] = []
        
    def encode(self, experience: Dict[str, Any], timestamp: float) -> str:
        """
        Encode an experience into episodic memory.
        
        Args:
            experience: Experience data to encode
            timestamp: Time of experience
            
        Returns:
            str: Episode ID
        """
        episode_id = f"episode_{len(self.episodes)}_{timestamp}"
        episode = {
            "id": episode_id,
            "timestamp": timestamp,
            "experience": experience,
            "strength": 1.0,
            "access_count": 0,
            "replay_count": 0,
            "importance": 0.5
        }
        
        self.episodes.append(episode)
        self.episode_index[episode_id] = len(self.episodes) - 1
        
        # Apply capacity constraint
        if len(self.episodes) > self.capacity:
            oldest_id = next(iter(self.episode_index))
            del self.episodes[self.episode_index[oldest_id]]
            del self.episode_index[oldest_id]
            
        # Schedule replay if importance is high
        if episode["importance"] > self.consolidation_threshold:
            self.replay_queue.append(episode_id)
            
        return episode_id
        
    def retrieve(self, episode_id: str = None, filter_keywords: List[str] = None) -> List[Dict[str, Any]]:
        """
        Retrieve episodes from episodic memory.
        
        Args:
            episode_id: Specific episode ID to retrieve
            filter_keywords: Keywords to filter episodes
            
        Returns:
            List[Dict[str, Any]]: Retrieved episodes
        """
        if episode_id:
            if episode_id in self.episode_index:
                episode = self.episodes[self.episode_index[episode_id]]
                episode["access_count"] += 1
                return [episode]
            return []
            
        results = self.episodes.copy()
        
        if filter_keywords:
            results = []
            for episode in self.episodes:
                if any(keyword.lower() in str(episode["experience"]).lower() 
                      for keyword in filter_keywords):
                    results.append(episode)
                    episode["access_count"] += 1
                    
        # Sort by strength and recency
        results.sort(key=lambda x: (x["strength"] * x["access_count"]) / (1 + x["timestamp"]), reverse=True)
        return results
        
    def replay(self) -> List[Dict[str, Any]]:
        """Replay scheduled episodes."""
        replayed_episodes = []
        
        while self.replay_queue:
            episode_id = self.replay_queue.popleft()
            if episode_id in self.episode_index:
                episode = self.episodes[self.episode_index[episode_id]]
                episode["replay_count"] += 1
                episode["strength"] *= 1.1  # Reinforcement from replay
                replayed_episodes.append(episode)
                
        return replayed_episodes
        
    def consolidate(self, importance_threshold: float = None) -> List[Dict[str, Any]]:
        """
        Consolidate important episodes to long-term storage.
        
        Args:
            importance_threshold: Threshold for consolidation
            
        Returns:
            List[Dict[str, Any]]: Consolidated episodes
        """
        if importance_threshold is None:
            importance_threshold = self.consolidation_threshold
            
        to_consolidate = []
        for episode in self.episodes:
            if episode["importance"] > importance_threshold and episode not in self.consolidated_episodes:
                to_consolidate.append(episode)
                self.consolidated_episodes.append(episode)
                
        return to_consolidate
        
    def analyze_patterns(self) -> Dict[str, Any]:
        """Analyze patterns in episodic memory."""
        if not self.episodes:
            return {}
            
        pattern_analysis = {
            "total_episodes": len(self.episodes),
            "consolidated_episodes": len(self.consolidated_episodes),
            "replay_queue_size": len(self.replay_queue),
            "avg_importance": sum(e["importance"] for e in self.episodes) / len(self.episodes),
            "most_common_keywords": self._extract_common_keywords(),
            "temporal_distribution": self._get_temporal_distribution()
        }
        
        return pattern_analysis
        
    def _extract_common_keywords(self) -> List[str]:
        """Extract common keywords from episodes."""
        # Simple keyword extraction (in real implementation, would use NLP)
        all_text = " ".join(str(e["experience"]) for e in self.episodes)
        words = all_text.lower().split()
        word_counts = {}
        for word in words:
            if len(word) > 3:  # Filter short words
                word_counts[word] = word_counts.get(word, 0) + 1
                
        return sorted(word_counts.items(), key=lambda x: x[1], reverse=True)[:10]
        
    def _get_temporal_distribution(self) -> Dict[str, Any]:
        """Get temporal distribution of episodes."""
        if not self.episodes:
            return {}
            
        timestamps = [e["timestamp"] for e in self.episodes]
        return {
            "min_timestamp": min(timestamps),
            "max_timestamp": max(timestamps),
            "span": max(timestamps) - min(timestamps),
            "avg_timestamp": sum(timestamps) / len(timestamps)
        }


class ConsolidationManager:
    """Automatic memory consolidation."""
    
    def __init__(self, episodic_memory: EpisodicMemory):
        """
        Initialize consolidation manager.
        
        Args:
            episodic_memory: Episodic memory to manage
        """
        self.episodic_memory = episodic_memory
        self.consolidation_schedule = []
        self.priority_queue = []  # Heap queue for prioritization
        
    def schedule_consolidation(self, episode_id: str, priority: float = 0.5):
        """
        Schedule episode for consolidation.
        
        Args:
            episode_id: ID of episode to consolidate
            priority: Priority level (higher = more important)
        """
        heapq.heappush(self.priority_queue, (-priority, episode_id))  # Negative for max-heap
        
    def consolidate_pending(self) -> List[Dict[str, Any]]:
        """Consolidate all pending episodes."""
        consolidated = []
        
        while self.priority_queue:
            priority, episode_id = heapq.heappop(self.priority_queue)
            
            if episode_id in self.episodic_memory.episode_index:
                episode = self.episodic_memory.episodes[self.episodic_memory.episode_index[episode_id]]
                if episode not in self.episodic_memory.consolidated_episodes:
                    self.episodic_memory.consolidated_episodes.append(episode)
                    consolidated.append(episode)
                    
        return consolidated
        
    def set_importance(self, episode_id: str, importance: float):
        """Set importance score for an episode."""
        if episode_id in self.episodic_memory.episode_index:
            episode = self.episodic_memory.episodes[self.episodic_memory.episode_index[episode_id]]
            episode["importance"] = max(0.0, min(1.0, importance))


class MemoryProfiler:
    """Memory usage analysis."""
    
    def __init__(self, episodic_memory: EpisodicMemory, working_memory: WorkingMemory):
        """
        Initialize memory profiler.
        
        Args:
            episodic_memory: Episodic memory to profile
            working_memory: Working memory to profile
        """
        self.episodic_memory = episodic_memory
        self.working_memory = working_memory
        
    def analyze_usage(self) -> Dict[str, Any]:
        """Analyze memory usage patterns."""
        analysis = {
            "working_memory": {
                "capacity": self.working_memory.capacity,
                "usage_percentage": len(self.working_memory.items) / self.working_memory.capacity,
                "active_items": len(self.working_memory.active_items),
                "average_strength": np.mean([e.strength for e in self.working_memory.items.values()]) if self.working_memory.items else 0.0,
                "competition_index": self._calculate_competition_index()
            },
            "episodic_memory": {
                "capacity": self.episodic_memory.capacity,
                "usage_percentage": len(self.episodic_memory.episodes) / self.episodic_memory.capacity,
                "consolidated_percentage": len(self.episodic_memory.consolidated_episodes) / max(len(self.episodic_memory.episodes), 1),
                "replay_queue_size": len(self.episodic_memory.replay_queue),
                "average_importance": np.mean([e["importance"] for e in self.episodic_memory.episodes]) if self.episodic_memory.episodes else 0.0
            },
            "system_health": self._assess_system_health()
        }
        
        return analysis
        
    def _calculate_competition_index(self) -> float:
        """Calculate competition index for working memory."""
        if not self.working_memory.active_items:
            return 0.0
            
        strengths = [self.working_memory.items[item_id].strength for item_id in self.working_memory.active_items]
        mean_strength = np.mean(strengths)
        std_strength = np.std(strengths)
        
        return std_strength / (mean_strength + 1e-6)
        
    def _assess_system_health(self) -> Dict[str, Any]:
        """Assess overall system health."""
        health_metrics = {
            "working_memory_stress": len(self.working_memory.items) / self.working_memory.capacity,
            "episodic_memory_stress": len(self.episodic_memory.episodes) / self.episodic_memory.capacity,
            "consolidation_needed": len(self.episodic_memory.replay_queue),
            "replay_efficiency": len(self.episodic_memory.consolidated_episodes) / max(len(self.episodic_memory.episodes), 1)
        }
        
        # Calculate overall health score (0-1, higher is better)
        stress_score = 1.0 - max(health_metrics["working_memory_stress"], health_metrics["episodic_memory_stress"])
        consolidation_score = 1.0 - health_metrics["consolidation_needed"] / 10.0
        replay_score = health_metrics["replay_efficiency"]
        
        overall_score = (stress_score + consolidation_score + replay_score) / 3.0
        
        return {
            "overall_health_score": overall_score,
            "stress_indicators": health_metrics,
            "recommendations": self._generate_recommendations(health_metrics)
        }
        
    def _generate_recommendations(self, metrics: Dict[str, Any]) -> List[str]:
        """Generate recommendations based on metrics."""
        recommendations = []
        
        if metrics["working_memory_stress"] > 0.8:
            recommendations.append("Increase working memory capacity or reduce item count")
            
        if metrics["episodic_memory_stress"] > 0.9:
            recommendations.append("Consolidate more episodes to reduce memory load")
            
        if metrics["consolidation_needed"] > 5:
            recommendations.append("Schedule more frequent memory consolidation")
            
        if metrics["replay_efficiency"] < 0.5:
            recommendations.append("Improve replay scheduling to increase consolidation")
            
        if not recommendations:
            recommendations.append("Memory system operating optimally")
            
        return recommendations


class RetrievalOptimizer:
    """Memory retrieval optimization."""
    
    def __init__(self, episodic_memory: EpisodicMemory, working_memory: WorkingMemory):
        """
        Initialize retrieval optimizer.
        
        Args:
            episodic_memory: Episodic memory to optimize
            working_memory: Working memory to optimize
        """
        self.episodic_memory = episodic_memory
        self.working_memory = working_memory
        self.retrieval_history = deque(maxlen=100)
        
    def optimize_retrieval(self, query: str, top_k: int = 5) -> List[Dict[str, Any]]:
        """
        Optimize retrieval for a query.
        
        Args:
            query: Search query
            top_k: Number of results to return
            
        Returns:
            List[Dict[str, Any]]: Retrieved episodes
        """
        # Simple retrieval optimization
        query_words = set(query.lower().split())
        
        scored_episodes = []
        for episode in self.episodic_memory.episodes:
            episode_text = str(episode["experience"]).lower()
            
            # Calculate score based on word overlap
            episode_words = set(episode_text.split())
            overlap = len(query_words & episode_words)
            total_words = len(query_words | episode_words)
            
            score = overlap / max(total_words, 1) * episode["strength"]
            
            scored_episodes.append((score, episode))
            
        # Sort by score and return top_k
        scored_episodes.sort(key=lambda x: x[0], reverse=True)
        results = [episode for score, episode in scored_episodes[:top_k]]
        
        # Update retrieval history
        self.retrieval_history.append({
            "query": query,
            "results_count": len(results),
            "avg_score": np.mean([score for score, _ in scored_episodes[:top_k]]) if scored_episodes else 0.0
        })
        
        return results
        
    def get_retrieval_performance(self) -> Dict[str, Any]:
        """Get retrieval performance metrics."""
        if not self.retrieval_history:
            return {}
            
        queries = len(self.retrieval_history)
        avg_results = np.mean([h["results_count"] for h in self.retrieval_history])
        avg_score = np.mean([h["avg_score"] for h in self.retrieval_history])
        
        return {
            "total_queries": queries,
            "average_results_per_query": avg_results,
            "average_relevance_score": avg_score,
            "recent_queries": list(self.retrieval_history)[-10:],
            "retrieval_efficiency": avg_score / max(avg_results, 1)
        }
        
    def learn_from_retrieval(self, query: str, relevant_episodes: List[str]) -> None:
        """
        Learn from retrieval results to improve future retrievals.
        
        Args:
            query: Original query
            relevant_episodes: IDs of relevant episodes
        """
        # In a real implementation, this would update internal models
        # For now, we just record the learning event
        learning_event = {
            "query": query,
            "relevant_episodes": relevant_episodes,
            "timestamp": time.time()
        }
        
        # Store learning event (would be persistent in real implementation)
        pass


# Re-export commonly used classes
__all__ = [
    "MemoryType", "MemoryEntry",
    "WorkingMemory", "EpisodicMemory",
    "ConsolidationManager", "MemoryProfiler", "RetrievalOptimizer"
]