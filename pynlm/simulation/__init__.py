"""
pynlm.simulation - High-level simulation control for NLM project

This module provides simulation control including:
- Agent simulation loops
- World interaction management
- Episode control
- Reward and learning management
"""

import time
import numpy as np
import pandas as pd
from typing import Dict, Any, List, Optional, Callable, Union
from dataclasses import dataclass, field
from abc import ABC, abstractmethod
import logging
from enum import Enum
import threading
import queue

logger = logging.getLogger(__name__)


class SimulationStatus(Enum):
    """Enumeration of simulation status."""
    INITIALIZED = "initialized"
    RUNNING = "running"
    PAUSED = "paused"
    COMPLETED = "completed"
    TERMINATED = "terminated"
    ERROR = "error"


@dataclass
class AgentState:
    """State of an agent in the simulation."""
    agent_id: str
    position: Tuple[float, float]
    velocity: Tuple[float, float]
    health: float
    energy: float
    memory: Dict[str, Any]
    last_action: Optional[str] = None
    last_reward: float = 0.0
    timestamp: float = field(default_factory=lambda: time.time())


@dataclass
class WorldState:
    """State of the simulation world."""
    width: float
    height: float
    obstacles: List[Tuple[float, float, float]]  # (x, y, radius)
    goals: List[Tuple[float, float, str]]  # (x, y, goal_type)
    time_step: int = 0
    max_time_steps: int = 1000
    weather_conditions: Dict[str, Any] = field(default_factory=dict)


class Agent(ABC):
    """Base class for agents in the simulation."""
    
    def __init__(self, agent_id: str, world: WorldState):
        """
        Initialize agent.
        
        Parameters
        ----------
        agent_id : str
            Agent identifier
        world : WorldState
            Simulation world
        """
        self.agent_id = agent_id
        self.world = world
        self.state = None
        self.action_history = []
        self.reward_history = []
        self.observation_history = []
        self.initialized = False
        
        # Initialize agent state
        self._initialize_state()
        self.initialized = True
    
    def _initialize_state(self) -> None:
        """Initialize agent state."""
        # Find a valid starting position
        start_position = self._find_valid_position()
        start_velocity = (0.0, 0.0)
        
        self.state = AgentState(
            agent_id=self.agent_id,
            position=start_position,
            velocity=start_velocity,
            health=1.0,
            energy=1.0,
            memory={}
        )
    
    def _find_valid_position(self) -> Tuple[float, float]:
        """Find a valid starting position for the agent."""
        # Try random positions until a valid one is found
        for _ in range(100):
            x = np.random.uniform(0, self.world.width)
            y = np.random.uniform(0, self.world.height)
            
            # Check if position is valid (not inside obstacles)
            valid = True
            for obstacle in self.world.obstacles:
                ox, oy, radius = obstacle
                if (x - ox)**2 + (y - oy)**2 < radius**2:
                    valid = False
                    break
            
            if valid:
                return (x, y)
        
        # Fallback to center if no valid position found
        return (self.world.width / 2, self.world.height / 2)
    
    @abstractmethod
    def act(self, observation: Dict[str, Any]) -> str:
        """
        Decide on an action based on observation.
        
        Parameters
        ----------
        observation : Dict[str, Any]
            Current observation
            
        Returns
        -------
        str
            Action to take
        """
        pass
    
    @abstractmethod
    def learn(self, experience: Dict[str, Any]) -> None:
        """
        Learn from experience.
        
        Parameters
        ----------
        experience : Dict[str, Any]
            Experience to learn from
        """
        pass
    
    def update_state(self, new_position: Tuple[float, float], 
                    new_velocity: Tuple[float, float],
                    reward: float, done: bool = False) -> None:
        """
        Update agent state.
        
        Parameters
        ----------
        new_position : Tuple[float, float]
            New position
        new_velocity : Tuple[float, float]
            New velocity
        reward : float
            Reward received
        done : bool, optional
            Whether episode is done, by default False
        """
        # Update position and velocity
        self.state.position = new_position
        self.state.velocity = new_velocity
        self.state.timestamp = time.time()
        
        # Record reward
        self.state.last_reward = reward
        self.reward_history.append(reward)
        
        # Update other state variables based on environment
        self._update_internal_state(reward, done)
    
    def _update_internal_state(self, reward: float, done: bool) -> None:
        """Update internal state variables."""
        # This can be overridden by subclasses
        pass
    
    def get_state(self) -> AgentState:
        """Get current agent state."""
        return self.state
    
    def get_history(self) -> Dict[str, Any]:
        """Get agent action and reward history."""
        return {
            "action_history": self.action_history,
            "reward_history": self.reward_history,
            "observation_history": self.observation_history
        }


class World:
    """Simulation world with agents and environment."""
    
    def __init__(self, width: float, height: float):
        """
        Initialize world.
        
        Parameters
        ----------
        width : float
            World width
        height : float
            World height
        """
        self.width = width
        self.height = height
        self.state = WorldState(width, height)
        self.agents = {}
        self.time_step = 0
        self.done = False
        
        # Add default obstacles and goals
        self._setup_environment()
    
    def _setup_environment(self) -> None:
        """Setup environment with obstacles and goals."""
        # Add some obstacles
        self.state.obstacles = [
            (200, 200, 50),
            (600, 400, 80),
            (100, 500, 60)
        ]
        
        # Add goals
        self.state.goals = [
            (150, 150, "collect"),
            (550, 450, "reach"),
            (300, 700, "avoid")
        ]
        
        # Set weather conditions
        self.state.weather_conditions = {
            "temperature": 25.0,
            "humidity": 0.7,
            "wind_speed": 5.0,
            "visibility": 100.0
        }
    
    def add_agent(self, agent: Agent) -> None:
        """
        Add agent to world.
        
        Parameters
        ----------
        agent : Agent
            Agent to add
        """
        self.agents[agent.agent_id] = agent
        logger.info(f"Added agent {agent.agent_id} to world")
    
    def step(self) -> Dict[str, Any]:
        """
        Advance simulation by one time step.
        
        Returns
        -------
        Dict[str, Any]
            Step information
        """
        self.time_step += 1
        self.state.time_step = self.time_step
        
        step_info = {
            "time_step": self.time_step,
            "agents": {},
            "world_state": self.state,
            "done": self._check_done()
        }
        
        # Get observations from agents
        observations = {}
        for agent_id, agent in self.agents.items():
            observation = self._get_observation(agent)
            agent.observation_history.append(observation)
            observations[agent_id] = observation
        
        # Update world state
        self._update_world_state()
        
        # Get actions from agents
        actions = {}
        for agent_id, agent in self.agents.items():
            action = agent.act(observations[agent_id])
            agent.action_history.append(action)
            actions[agent_id] = action
        
        # Apply actions and get new states
        for agent_id, agent in self.agents.items():
            new_position, new_velocity = self._apply_action(agent, actions[agent_id])
            reward = self._calculate_reward(agent, new_position, actions[agent_id])
            
            # Update agent state
            agent.update_state(new_position, new_velocity, reward)
            
            # Record step info
            step_info["agents"][agent_id] = {
                "position": agent.get_state().position,
                "velocity": agent.get_state().velocity,
                "health": agent.get_state().health,
                "energy": agent.get_state().energy,
                "reward": reward,
                "action": actions[agent_id]
            }
        
        # Check if episode is done
        step_info["done"] = self._check_done()
        
        return step_info
    
    def _get_observation(self, agent: Agent) -> Dict[str, Any]:
        """
        Get observation for agent.
        
        Parameters
        ----------
        agent : Agent
            Agent to get observation for
            
        Returns
        -------
        Dict[str, Any]
            Observation
        """
        state = agent.get_state()
        x, y = state.position
        
        # Get relative positions of other agents
        other_agents = []
        for other_id, other_agent in self.agents.items():
            if other_id != agent.agent_id:
                other_state = other_agent.get_state()
                other_x, other_y = other_state.position
                rel_x = (other_x - x) / self.width
                rel_y = (other_y - y) / self.height
                other_agents.append({"id": other_id, "rel_x": rel_x, "rel_y": rel_y})
        
        # Get distances to goals and obstacles
        goal_distances = []
        for goal_x, goal_y, goal_type in self.state.goals:
            distance = np.sqrt((goal_x - x)**2 + (goal_y - y)**2)
            goal_distances.append({"type": goal_type, "distance": distance})
        
        obstacle_distances = []
        for obstacle_x, obstacle_y, obstacle_radius in self.state.obstacles:
            distance = np.sqrt((obstacle_x - x)**2 + (obstacle_y - y)**2) - obstacle_radius
            obstacle_distances.append({"distance": max(0, distance)})
        
        return {
            "position": {"x": x, "y": y},
            "velocity": {"x": state.velocity[0], "y": state.velocity[1]},
            "health": state.health,
            "energy": state.energy,
            "other_agents": other_agents,
            "goal_distances": goal_distances,
            "obstacle_distances": obstacle_distances,
            "time_step": self.time_step
        }
    
    def _apply_action(self, agent: Agent, action: str) -> Tuple[Tuple[float, float], Tuple[float, float]]:
        """
        Apply action to agent.
        
        Parameters
        ----------
        agent : Agent
            Agent
        action : str
            Action to apply
            
        Returns
        -------
        Tuple[Tuple[float, float], Tuple[float, float]]
            New position and velocity
        """
        state = agent.get_state()
        x, y = state.position
        vx, vy = state.velocity
        
        # Parse action
        action_parts = action.split('_')
        move_type = action_parts[0]
        
        # Apply movement
        new_x, new_y = x, y
        new_vx, new_vy = vx, vy
        
        if move_type == "move_up":
            new_y += 5
            new_vy += 1
        elif move_type == "move_down":
            new_y -= 5
            new_vy -= 1
        elif move_type == "move_left":
            new_x -= 5
            new_vx -= 1
        elif move_type == "move_right":
            new_x += 5
            new_vx += 1
        elif move_type == "stay":
            pass
        elif move_type == "move_random":
            angle = np.random.uniform(0, 2 * np.pi)
            speed = np.random.uniform(1, 5)
            new_x += speed * np.cos(angle)
            new_y += speed * np.sin(angle)
            new_vx += speed * np.cos(angle) * 0.1
            new_vy += speed * np.sin(angle) * 0.1
        
        # Apply velocity damping
        new_vx *= 0.9
        new_vy *= 0.9
        
        # Keep within world bounds
        new_x = np.clip(new_x, 0, self.width)
        new_y = np.clip(new_y, 0, self.height)
        
        # Check collision with obstacles
        for obstacle_x, obstacle_y, obstacle_radius in self.state.obstacles:
            if (new_x - obstacle_x)**2 + (new_y - obstacle_y)**2 < obstacle_radius**2:
                # Reflect off obstacle
                dist_to_center = np.sqrt((new_x - obstacle_x)**2 + (new_y - obstacle_y)**2)
                if dist_to_center > 0:
                    overlap = obstacle_radius - dist_to_center
                    new_x += (overlap * (new_x - obstacle_x)) / dist_to_center
                    new_y += (overlap * (new_y - obstacle_y)) / dist_to_center
                break
        
        return (new_x, new_y), (new_vx, new_vy)
    
    def _calculate_reward(self, agent: Agent, new_position: Tuple[float, float], 
                         action: str) -> float:
        """
        Calculate reward for agent's action.
        
        Parameters
        ----------
        agent : Agent
            Agent
        new_position : Tuple[float, float]
            New position
        action : str
            Action taken
            
        Returns
        -------
        float
            Reward
        """
        reward = 0.0
        x, y = new_position
        
        # Reward for staying alive
        reward += 0.1
        
        # Reward for reaching goals
        for goal_x, goal_y, goal_type in self.state.goals:
            distance = np.sqrt((goal_x - x)**2 + (goal_y - y)**2)
            if distance < 30:
                if goal_type == "collect":
                    reward += 10.0
                elif goal_type == "reach":
                    reward += 20.0
                elif goal_type == "avoid":
                    reward += 5.0
        
        # Penalize for getting too close to obstacles
        for obstacle_x, obstacle_y, obstacle_radius in self.state.obstacles:
            distance = np.sqrt((obstacle_x - x)**2 + (obstacle_y - y)**2) - obstacle_radius
            if distance < 0:
                reward -= 5.0
        
        # Reward for efficient movement
        if "move_random" in action:
            reward -= 0.1
        elif "stay" in action:
            reward += 0.5
        
        # Update agent's state based on reward
        agent.state.last_reward = reward
        
        return reward
    
    def _update_world_state(self) -> None:
        """Update world state."""
        # Update weather conditions (simulate changing weather)
        self.state.weather_conditions["temperature"] += np.random.uniform(-2, 2)
        self.state.weather_conditions["humidity"] += np.random.uniform(-0.1, 0.1)
        
        # Ensure values stay within reasonable bounds
        self.state.weather_conditions["temperature"] = np.clip(
            self.state.weather_conditions["temperature"], 0, 50)
        self.state.weather_conditions["humidity"] = np.clip(
            self.state.weather_conditions["humidity"], 0.3, 1.0)
    
    def _check_done(self) -> bool:
        """Check if simulation should end."""
        # Check time limit
        if self.time_step >= self.state.max_time_steps:
            return True
        
        # Check if all agents are done
        for agent in self.agents.values():
            if agent.state.health <= 0 or agent.state.energy <= 0:
                return True
        
        return False


class EpisodeController:
    """Controller for simulation episodes."""
    
    def __init__(self, max_steps: int = 1000, save_frequency: int = 100):
        """
        Initialize episode controller.
        
        Parameters
        ----------
        max_steps : int, optional
            Maximum number of steps per episode, by default 1000
        save_frequency : int, optional
            Frequency of saving episode data, by default 100
        """
        self.max_steps = max_steps
        self.save_frequency = save_frequency
        self.current_episode = 0
        self.episode_history = []
    
    def run_episode(self, world: World, agents: List[Agent]) -> Dict[str, Any]:
        """
        Run a single episode.
        
        Parameters
        ----------
        world : World
            Simulation world
        agents : List[Agent]
            List of agents
            
        Returns
        -------
        Dict[str, Any]
            Episode results
        """
        # Reset world
        world.time_step = 0
        world.state.time_step = 0
        world.done = False
        
        # Reset agents
        for agent in agents:
            agent.state = None
            agent.action_history = []
            agent.reward_history = []
            agent.observation_history = []
            agent._initialize_state()
        
        # Run episode
        episode_steps = []
        total_reward = 0.0
        
        for step in range(self.max_steps):
            if world.done:
                break
            
            step_info = world.step()
            episode_steps.append(step_info)
            
            # Sum rewards for this step
            step_reward = sum(agent_info["reward"] for agent_info in step_info["agents"].values())
            total_reward += step_reward
            
            # Save episode state periodically
            if step % self.save_frequency == 0:
                self._save_episode_progress(episode_steps, step)
        
        # Final episode information
        episode_info = {
            "episode_number": self.current_episode,
            "total_steps": len(episode_steps),
            "total_reward": total_reward,
            "steps": episode_steps,
            "world_state": world.state,
            "agent_history": {
                agent.agent_id: agent.get_history() 
                for agent in agents
            }
        }
        
        self.episode_history.append(episode_info)
        self.current_episode += 1
        
        # Save final episode
        self._save_episode_progress(episode_steps, len(episode_steps))
        
        return episode_info
    
    def _save_episode_progress(self, episode_steps: List[Dict[str, Any]], step: int) -> None:
        """
        Save episode progress.
        
        Parameters
        ----------
        episode_steps : List[Dict[str, Any]]
            Episode steps
        step : int
            Current step
        """
        # Convert to serializable format
        serializable_steps = []
        for step_info in episode_steps:
            step_dict = step_info.copy()
            step_dict["world_state"] = {
                "width": step_dict["world_state"].width,
                "height": step_dict["world_state"].height,
                "time_step": step_dict["world_state"].time_step,
                "max_time_steps": step_dict["world_state"].max_time_steps
            }
            serializable_steps.append(step_dict)
        
        # Save to file
        filename = f"episode_{self.current_episode}_step_{step}.json"
        with open(filename, 'w') as f:
            json.dump(serializable_steps, f, indent=2)
        
        logger.info(f"Saved episode progress to {filename}")


class RewardManager:
    """Manage rewards and punishments in simulation."""
    
    def __init__(self, reward_types: Dict[str, Callable] = None):
        """
        Initialize reward manager.
        
        Parameters
        ----------
        reward_types : Dict[str, Callable], optional
            Reward types and their calculation functions, by default None
        """
        if reward_types is None:
            self.reward_types = {
                "step": self._calculate_step_reward,
                "survival": self._calculate_survival_reward,
                "efficiency": self._calculate_efficiency_reward,
                "exploration": self._calculate_exploration_reward
            }
        else:
            self.reward_types = reward_types
    
    def calculate_total_reward(self, step_info: Dict[str, Any]) -> float:
        """
        Calculate total reward for a step.
        
        Parameters
        ----------
        step_info : Dict[str, Any]
            Step information
            
        Returns
        -------
        float
            Total reward
        """
        total_reward = 0.0
        
        for reward_type, reward_func in self.reward_types.items():
            try:
                reward = reward_func(step_info)
                total_reward += reward
            except Exception as e:
                logger.warning(f"Reward calculation failed for {reward_type}: {e}")
        
        return total_reward
    
    def _calculate_step_reward(self, step_info: Dict[str, Any]) -> float:
        """Calculate step reward."""
        step_reward = 0.0
        
        for agent_id, agent_info in step_info["agents"].items():
            # Reward for positive actions
            if agent_info["action"] in ["move_up", "move_down", "move_left", "move_right"]:
                step_reward += 0.5
            
            # Reward for reaching goals
            x, y = agent_info["position"]
            for goal_x, goal_y, goal_type in step_info["world_state"].goals:
                distance = np.sqrt((goal_x - x)**2 + (goal_y - y)**2)
                if distance < 30:
                    step_reward += 2.0
        
        return step_reward
    
    def _calculate_survival_reward(self, step_info: Dict[str, Any]) -> float:
        """Calculate survival reward."""
        survival_reward = 0.0
        
        for agent_id, agent_info in step_info["agents"].items():
            survival_reward += agent_info["health"] * 0.1
        
        return survival_reward
    
    def _calculate_efficiency_reward(self, step_info: Dict[str, Any]) -> float:
        """Calculate efficiency reward."""
        efficiency_reward = 0.0
        
        for agent_id, agent_info in step_info["agents"].items():
            # Reward for minimal movement (efficiency)
            velocity = agent_info["velocity"]
            speed = np.sqrt(velocity[0]**2 + velocity[1]**2)
            efficiency_reward -= speed * 0.1
        
        return efficiency_reward
    
    def _calculate_exploration_reward(self, step_info: Dict[str, Any]) -> float:
        """Calculate exploration reward."""
        exploration_reward = 0.0
        
        for agent_id, agent_info in step_info["agents"].items():
            # Reward for visiting new areas
            x, y = agent_info["position"]
            exploration_reward += np.sqrt(x**2 + y**2) * 0.01
        
        return exploration_reward


class LearningController:
    """Controller for learning in simulation."""
    
    def __init__(self, learning_rate: float = 0.01, discount_factor: float = 0.99,
                 exploration_rate: float = 0.1):
        """
        Initialize learning controller.
        
        Parameters
        ----------
        learning_rate : float, optional
            Learning rate, by default 0.01
        discount_factor : float, optional
            Discount factor, by default 0.99
        exploration_rate : float, optional
            Exploration rate, by default 0.1
        """
        self.learning_rate = learning_rate
        self.discount_factor = discount_factor
        self.exploration_rate = exploration_rate
        
        # Store experiences
        self.experiences = []
        
        # Agent models (simplified)
        self.models = {}
    
    def add_experience(self, agent_id: str, state: Dict[str, Any],
                      action: str, reward: float, next_state: Dict[str, Any],
                      done: bool) -> None:
        """
        Add experience to memory.
        
        Parameters
        ----------
        agent_id : str
            Agent identifier
        state : Dict[str, Any]
            Current state
        action : str
            Action taken
        reward : float
            Reward received
        next_state : Dict[str, Any]
            Next state
        done : bool
            Whether episode is done
        """
        experience = {
            "agent_id": agent_id,
            "state": state,
            "action": action,
            "reward": reward,
            "next_state": next_state,
            "done": done
        }
        
        self.experiences.append(experience)
        
        # Keep only recent experiences if memory is full
        if len(self.experiences) > 10000:
            self.experiences = self.experiences[-10000:]
        
        # Update model for agent
        if agent_id not in self.models:
            self.models[agent_id] = self._initialize_model(agent_id)
        
        self._update_model(agent_id, experience)
    
    def _initialize_model(self, agent_id: str) -> Dict[str, Any]:
        """Initialize model for agent."""
        return {
            "agent_id": agent_id,
            "q_values": {},
            "policy": {},
            "value": 0.0,
            "last_state": None,
            "last_action": None,
            "last_reward": 0.0
        }
    
    def _update_model(self, agent_id: str, experience: Dict[str, Any]) -> None:
        """Update model with experience."""
        model = self.models[agent_id]
        
        state_key = self._state_to_key(experience["state"])
        action = experience["action"]
        reward = experience["reward"]
        next_state_key = self._state_to_key(experience["next_state"])
        done = experience["done"]
        
        # Update Q-value
        current_q = model["q_values"].get((state_key, action), 0.0)
        next_q = max([model["q_values"].get((next_state_key, a), 0.0) 
                     for a in self._get_possible_actions(agent_id)]) if not done else 0.0
        
        new_q = current_q + self.learning_rate * (reward + self.discount_factor * next_q - current_q)
        model["q_values"][(state_key, action)] = new_q
        
        # Update policy
        model["policy"] = self._compute_policy(model["q_values"])
    
    def _state_to_key(self, state: Dict[str, Any]) -> str:
        """Convert state to string key."""
        # Convert floats to strings with limited precision
        state_str = "|"
        for key, value in sorted(state.items()):
            if isinstance(value, dict):
                state_str += f"{key}:{json.dumps(value)}|"
            else:
                state_str += f"{key}:{value}|"
        
        return state_str
    
    def _get_possible_actions(self, agent_id: str) -> List[str]:
        """Get possible actions for agent."""
        return ["move_up", "move_down", "move_left", "move_right", "stay", "move_random"]
    
    def _compute_policy(self, q_values: Dict[tuple, float]) -> Dict[str, Any]:
        """Compute policy from Q-values."""
        policy = {}
        
        # Group Q-values by state
        state_q_values = defaultdict(list)
        for (state, action), q_value in q_values.items():
            state_q_values[state].append((action, q_value))
        
        # For each state, select best action
        for state, action_q_values in state_q_values.items():
            best_action = max(action_q_values, key=lambda x: x[1])
            policy[state] = best_action[0]

        return policy

    def get_action(self, agent_id: str, state: Dict[str, Any], training: bool = True) -> str:
        """
        Get action for agent.
        
        Parameters
        ----------
        agent_id : str
            Agent identifier
        state : Dict[str, Any]
            Current state
        training : bool, optional
            Whether in training mode, by default True
            
        Returns
        -------
        str
            Action
        """
        if agent_id not in self.models:
            self.models[agent_id] = self._initialize_model(agent_id)
        
        model = self.models[agent_id]
        state_key = self._state_to_key(state)
        
        # Get Q-values for current state
        state_q_values = []
        for (s, a), q_value in model["q_values"].items():
            if s == state_key:
                state_q_values.append((a, q_value))
        
        if not state_q_values:
            # If no Q-values found, use random action
            return np.random.choice(self._get_possible_actions(agent_id))
        
        # Epsilon-greedy action selection
        if training and np.random.random() < self.exploration_rate:
            return np.random.choice(self._get_possible_actions(agent_id))
        
        # Get best action
        best_action = max(state_q_values, key=lambda x: x[1])
        return best_action[0]
    
    def save_model(self, agent_id: str, path: str) -> None:
        """
        Save model for agent.
        
        Parameters
        ----------
        agent_id : str
            Agent identifier
        path : str
            Path to save model
        """
        if agent_id not in self.models:
            raise ValueError(f"No model found for agent: {agent_id}")
        
        with open(path, 'w') as f:
            json.dump(self.models[agent_id], f, indent=2)
        
        logger.info(f"Saved model for agent {agent_id} to {path}")
    
    def load_model(self, agent_id: str, path: str) -> None:
        """
        Load model for agent.
        
        Parameters
        ----------
        agent_id : str
            Agent identifier
        path : str
            Path to load model from
        """
        with open(path, 'r') as f:
            model = json.load(f)
        
        self.models[agent_id] = model
        logger.info(f"Loaded model for agent {agent_id} from {path}")


class SimulationLoop:
    """Main simulation loop."""
    
    def __init__(self, world: World, reward_manager: RewardManager,
                 learning_controller: LearningController,
                 episode_controller: EpisodeController):
        """
        Initialize simulation loop.
        
        Parameters
        ----------
        world : World
            Simulation world
        reward_manager : RewardManager
            Reward manager
        learning_controller : LearningController
            Learning controller
        episode_controller : EpisodeController
            Episode controller
        """
        self.world = world
        self.reward_manager = reward_manager
        self.learning_controller = learning_controller
        self.episode_controller = episode_controller
        
        self.status = SimulationStatus.INITIALIZED
        self.current_episode = None
    
    def run(self, agents: List[Agent], num_episodes: int = 100) -> List[Dict[str, Any]]:
        """
        Run simulation.
        
        Parameters
        ----------
        agents : List[Agent]
            List of agents
        num_episodes : int, optional
            Number of episodes to run, by default 100
            
        Returns
        -------
        List[Dict[str, Any]]
            Simulation results
        """
        self.status = SimulationStatus.RUNNING
        results = []
        
        logger.info(f"Starting simulation with {len(agents)} agents for {num_episodes} episodes")
        
        for episode in range(num_episodes):
            logger.info(f"Starting episode {episode + 1}/{num_episodes}")
            
            # Run episode
            episode_info = self.episode_controller.run_episode(self.world, agents)
            self.current_episode = episode_info
            
            # Process episode results
            processed_results = self._process_episode_results(episode_info, agents)
            results.append(processed_results)
            
            # Log episode results
            logger.info(f"Episode {episode + 1} completed. "
                       f"Steps: {episode_info['total_steps']}, "
                       f"Total reward: {episode_info['total_reward']:.2f}")
            
            # Save results periodically
            if (episode + 1) % 10 == 0:
                self._save_results(results, f"simulation_results_episode_{episode + 1}.json")
        
        self.status = SimulationStatus.COMPLETED
        logger.info("Simulation completed successfully")
        
        return results
    
    def _process_episode_results(self, episode_info: Dict[str, Any], 
                                agents: List[Agent]) -> Dict[str, Any]:
        """
        Process episode results.
        
        Parameters
        ----------
        episode_info : Dict[str, Any]
            Episode information
        agents : List[Agent]
            List of agents
            
        Returns
        -------
        Dict[str, Any]
            Processed results
        """
        # Extract key information from episodes
        episode_steps = episode_info["steps"]
        
        # Calculate episode statistics
        total_steps = episode_info["total_steps"]
        total_reward = episode_info["total_reward"]
        
        # Calculate agent statistics
        agent_stats = {}
        for agent in agents:
            history = agent.get_history()
            agent_stats[agent.agent_id] = {
                "total_reward": np.sum(history["reward_history"]),
                "average_reward": np.mean(history["reward_history"]) if history["reward_history"] else 0,
                "num_actions": len(history["action_history"]),
                "unique_actions": len(set(history["action_history"]))
            }
        
        return {
            "episode_number": episode_info["episode_number"],
            "total_steps": total_steps,
            "total_reward": total_reward,
            "agent_stats": agent_stats,
            "world_state": episode_info["world_state"],
            "step_count": len(episode_steps)
        }
    
    def _save_results(self, results: List[Dict[str, Any]], path: str) -> None:
        """
        Save simulation results.
        
        Parameters
        ----------
        results : List[Dict[str, Any]]
            Simulation results
        path : str
            Path to save results
        """
        with open(path, 'w') as f:
            json.dump(results, f, indent=2)
        
        logger.info(f"Saved simulation results to {path}")
    
    def pause(self) -> None:
        """Pause simulation."""
        self.status = SimulationStatus.PAUSED
        logger.info("Simulation paused")
    
    def resume(self) -> None:
        """Resume simulation."""
        self.status = SimulationStatus.RUNNING
        logger.info("Simulation resumed")
    
    def terminate(self) -> None:
        """Terminate simulation."""
        self.status = SimulationStatus.TERMINATED
        logger.info("Simulation terminated")
    
    def get_status(self) -> SimulationStatus:
        """Get simulation status."""
        return self.status


# Package information
__version__ = "1.0.0"
__author__ = "NLM Team"
__description__ = "High-level simulation control utilities for NLM project"

# Export all classes
__all__ = [
    'Agent',
    'World',
    'EpisodeController',
    'RewardManager',
    'LearningController',
    'SimulationLoop',
    'SimulationStatus'
]

# Example usage function
def example_usage():
    """Example usage of the simulation module."""
    # Create world
    world = World(800, 600)
    
    # Create reward manager
    reward_manager = RewardManager()
    
    # Create learning controller
    learning_controller = LearningController()
    
    # Create episode controller
    episode_controller = EpisodeController(max_steps=500, save_frequency=100)
    
    # Create simulation loop
    simulation = SimulationLoop(world, reward_manager, learning_controller, episode_controller)
    
    # Create simple agents (this would be expanded in real usage)
    class SimpleAgent(Agent):
        def act(self, observation: Dict[str, Any]) -> str:
            # Simple random action
            actions = ["move_up", "move_down", "move_left", "move_right", "stay"]
            return np.random.choice(actions)
        
        def learn(self, experience: Dict[str, Any]) -> None:
            # Simple learning (would be more complex in real implementation)
            pass
    
    agents = [SimpleAgent(f"agent_{i}", world) for i in range(3)]
    
    # Run simulation
    results = simulation.run(agents, num_episodes=5)
    
    return {
        "num_episodes": len(results),
        "total_steps": sum(r["step_count"] for r in results),
        "total_reward": sum(r["total_reward"] for r in results),
        "world_stats": {
            "width": world.width,
            "height": world.height
        }
    }

if __name__ == "__main__":
    print("Testing pynlm.simulation module...")
    
    # Run example usage
    results = example_usage()
    print("Example usage completed successfully!")
    print(f"Number of episodes: {results['num_episodes']}")
    print(f"Total steps: {results['total_steps']}")
    print(f"Total reward: {results['total_reward']:.2f}")
    print(f"World stats: {results['world_stats']}")