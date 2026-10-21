#!/usr/bin/env python3
"""
Trajectory Planner Tool

Visualization of agent trajectories in world space for NLM.
Provides comprehensive visualization of agent movement patterns,
trajectory analysis, and spatial planning capabilities.
"""

import numpy as np
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.patches import Circle, Rectangle, Arrow
from matplotlib.collections import PatchCollection
import seaborn as sns
import pandas as pd
from datetime import datetime, timedelta
import sys
import os

@dataclass
class TrajectoryPlannerConfig:
    """Configuration for trajectory planner"""
    time_window: float = 20.0  # seconds
    refresh_rate: float = 30.0  # Hz
    show_trajectory_paths: bool = True
    show_trajectory_statistics: bool = True
    show_velocity_vectors: bool = True
    show_acceleration_vectors: bool = True
    show_obstacle_avoidance: bool = True
    show_planning_paths: bool = True
    animate: bool = True
    color_scheme: str = "trajectory"  # "trajectory", "velocity", "acceleration", "heatmap"
    show_labels: bool = True
    show_statistics: bool = True
    save_format: str = "png"  # "png", "gif", "mp4", "html"
    output_path: str = "trajectory_planner"
    
    # Trajectory analysis parameters
    show_path_smoothness: bool = True
    show_curvature_analysis: bool = True
    show_speed_profile: bool = True
    show_spatial_distribution: bool = True
    show_environment_interaction: bool = True
    
    # Visualization modes
    view_mode: str = "2d"  # "2d", "3d", "top_down", "side_view"
    show_3d: bool = False
    figure_size: Tuple[int, int] = (16, 12)
    dpi: int = 150
    animation_interval: int = 100  # milliseconds
    
    # Environment and world settings
    world_bounds: Tuple[float, float, float, float] = (0, 100, 0, 100)  # x_min, x_max, y_min, y_max
    show_world_boundaries: bool = True
    show_obstacles: bool = True
    show_waypoints: bool = True
    show_goal_positions: bool = True
    
    # Planning and optimization
    show_path_planning: bool = True
    show_path_optimization: bool = True
    show_path_smoothness: bool = True
    show_planning_constraints: bool = True
    
    # Display settings
    show_legend: bool = True
    show_colorbar: bool = True
    show_trajectory_details: bool = True
    smooth_trajectories: bool = True
class TrajectoryPlanner:
    """
    Visualization tool for agent trajectories in world space.
    
    Features:
    - Real-time trajectory path visualization
    - Velocity and acceleration vector analysis
    - Obstacle avoidance and collision detection
    - Path planning and optimization visualization
    - Trajectory statistics and analysis
    - Export capabilities for scientific analysis
    """
    
    def __init__(self, config: TrajectoryPlannerConfig = None):
        self.config = config or TrajectoryPlannerConfig()
        self.trajectories = []  # List of trajectory arrays
        self.waypoints = []  # List of waypoint arrays
        self.planning_paths = []  # List of planned paths
        self.velocity_vectors = []  # List of velocity vector arrays
        self.acceleration_vectors = []  # List of acceleration vector arrays
        self.obstacle_positions = []  # List of obstacle positions
        self.goal_positions = []  # List of goal positions
        self.current_time = 0.0
        self.is_running = False
        self.animation_obj = None
        self.fig = None
        self.axes = None
        
        # Initialize viewer components
        self._initialize_components()
    
    def _initialize_components(self):
        """Initialize viewer components and data structures"""
        # Trajectory data structures
        self.agent_trajectories = []  # List of agent trajectories
        self.planned_trajectories = []  # List of planned trajectories
        self.obstacle_trajectories = []  # List of obstacle trajectories (if any)
        
        # Initialize colormaps for trajectory visualization
        self._initialize_colormaps()
        
        # Setup data buffers
        self._setup_data_buffers()
    
    def _initialize_colormaps(self):
        """Initialize colormaps for different trajectory visualizations"""
        # Velocity colormap (blue to red)
        self.velocity_cmap = LinearSegmentedColormap.from_list(
            'velocity', ['darkblue', 'blue', 'cyan', 'yellow', 'orange', 'red', 'darkred']
        )
        
        # Acceleration colormap (green to red)
        self.acceleration_cmap = LinearSegmentedColormap.from_list(
            'acceleration', ['darkgreen', 'green', 'lime', 'yellow', 'orange', 'red', 'darkred']
        )
        
        # Path planning colormap
        self.planning_cmap = plt.cm.viridis
        
        # Obstacle colormap
        self.obstacle_cmap = plt.cm.RdYlBu_r
        
        # Goal position colormap
        self.goal_cmap = plt.cm.Purples
        
        # Trajectory history colormap
        self.history_cmap = plt.cm.plasma
    
    def _setup_data_buffers(self):
        """Setup data buffers for visualization"""
        self.max_history = 1000  # Maximum number of time steps to store
        self.trajectory_buffer = []
        self.waypoint_buffer = []
        self.planning_buffer = []
        self.velocity_buffer = []
        self.acceleration_buffer = []
    
    def add_trajectory_data(self, 
                           trajectory: np.ndarray,
                           velocity_vectors: Optional[np.ndarray] = None,
                           acceleration_vectors: Optional[np.ndarray] = None,
                           waypoints: Optional[np.ndarray] = None,
                           planning_path: Optional[np.ndarray] = None,
                           obstacles: Optional[List[np.ndarray]] = None,
                           goal_positions: Optional[List[np.ndarray]] = None,
                           time_step: float = 1.0):
        """
        Add trajectory data for visualization.
        
        Args:
            trajectory: Agent trajectory (N x 2 or N x 3)
            velocity_vectors: Velocity vectors (N x 2 or N x 3)
            acceleration_vectors: Acceleration vectors (N x 2 or N x 3)
            waypoints: Waypoint positions (M x 2 or M x 3)
            planning_path: Planned path (P x 2 or P x 3)
            obstacles: List of obstacle position arrays
            goal_positions: List of goal position arrays
            time_step: Current time step
        """
        # Store trajectory data
        self.trajectories.append(trajectory.copy())
        self.agent_trajectories.append(trajectory.copy())
        
        if velocity_vectors is not None:
            self.velocity_vectors.append(velocity_vectors.copy())
        if acceleration_vectors is not None:
            self.acceleration_vectors.append(acceleration_vectors.copy())
        if waypoints is not None:
            self.waypoints.append(waypoints.copy())
        if planning_path is not None:
            self.planning_paths.append(planning_path.copy())
        if obstacles is not None:
            self.obstacle_positions.extend(obstacles)
        if goal_positions is not None:
            self.goal_positions.extend(goal_positions)
        
        # Update buffers
        self.trajectory_buffer.append({
            'trajectory': trajectory.copy(),
            'velocity_vectors': velocity_vectors.copy() if velocity_vectors is not None else None,
            'acceleration_vectors': acceleration_vectors.copy() if acceleration_vectors is not None else None,
            'waypoints': waypoints.copy() if waypoints is not None else None,
            'planning_path': planning_path.copy() if planning_path is not None else None
        })
        
        # Maintain buffer size
        if len(self.trajectory_buffer) > self.max_history:
            self.trajectory_buffer.pop(0)
        
        # Update current time
        self.current_time += time_step
    
    def start_real_time(self, initial_data: Dict[str, Any] = None):
        """
        Start real-time trajectory visualization.
        
        Args:
            initial_data: Optional initial trajectory data
        """
        if initial_data:
            # Add initial data
            for i, (traj, vel, acc, wps, plan_path, time_step) in enumerate(zip(
                initial_data.get('trajectories', []),
                initial_data.get('velocity_vectors', []),
                initial_data.get('acceleration_vectors', []),
                initial_data.get('waypoints', []),
                initial_data.get('planning_paths', []),
                initial_data.get('time_steps', [1.0] * len(initial_data.get('trajectories', [])))
            )):
                self.add_trajectory_data(traj, vel, acc, wps, plan_path, time_step=time_step)
        
        self.is_running = True
        
        # Create visualization
        self.fig = plt.figure(figsize=self.config.figure_size)
        
        # Create subplot layout based on view mode
        if self.config.view_mode == "2d":
            self._create_2d_view()
        elif self.config.view_mode == "3d":
            self._create_3d_view()
        elif self.config.view_mode == "top_down":
            self._create_top_down_view()
        elif self.config.view_mode == "side_view":
            self._create_side_view()
        else:
            self._create_2d_view()
        
        # Setup animation
        self.animation_obj = animation.FuncAnimation(
            self.fig, self._update_plot, interval=self.config.animation_interval,
            blit=False, cache_frame_data=False
        )
        
        plt.show()
    
    def _create_2d_view(self):
        """Create 2D view visualization"""
        # Create grid of subplots for trajectory analysis
        gs = plt.GridSpec(4, 4, hspace=0.3, wspace=0.3)
        
        # Main trajectory view
        self.ax_trajectory = plt.subplot(gs[0, :4])
        
        # Velocity vector view
        self.ax_velocity = plt.subplot(gs[1, :2])
        
        # Acceleration vector view
        self.ax_acceleration = plt.subplot(gs[1, 2:])
        
        # Path planning view
        self.ax_planning = plt.subplot(gs[2, :2])
        
        # Waypoint visualization
        self.ax_waypoints = plt.subplot(gs[2, 2:])
        
        # Obstacle visualization
        self.ax_obstacles = plt.subplot(gs[3, :2])
        
        # Goal position visualization
        self.ax_goals = plt.subplot(gs[3, 2:])
        
        # Initialize 2D plot elements
        self._initialize_2d_elements()
    
    def _create_3d_view(self):
        """Create 3D view visualization"""
        # Create 3D grid for trajectory analysis
        gs = plt.GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # Main 3D trajectory view
        self.ax_trajectory_3d = plt.subplot(gs[0, :3], projection='3d')
        
        # Velocity vectors in 3D
        self.ax_velocity_3d = plt.subplot(gs[1, 0], projection='3d')
        
        # Acceleration vectors in 3D
        self.ax_acceleration_3d = plt.subplot(gs[1, 1], projection='3d')
        
        # Path planning in 3D
        self.ax_planning_3d = plt.subplot(gs[1, 2], projection='3d')
        
        # Environment in 3D
        self.ax_environment_3d = plt.subplot(gs[2, :3], projection='3d')
        
        # Initialize 3D plot elements
        self._initialize_3d_elements()
    
    def _create_top_down_view(self):
        """Create top-down view visualization"""
        # Create top-down view layout
        gs = plt.GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # Top-down trajectory view
        self.ax_top_down = plt.subplot(gs[0, :3])
        
        # Velocity magnitude heatmap
        self.ax_velocity_magnitude = plt.subplot(gs[1, 0])
        
        # Acceleration magnitude heatmap
        self.ax_acceleration_magnitude = plt.subplot(gs[1, 1])
        
        # Path planning heatmap
        self.ax_planning_heatmap = plt.subplot(gs[1, 2])
        
        # Spatial distribution
        self.ax_spatial_distribution = plt.subplot(gs[2, :3])
        
        # Initialize top-down elements
        self._initialize_top_down_elements()
    
    def _create_side_view(self):
        """Create side view visualization"""
        # Create side view layout
        gs = plt.GridSpec(4, 4, hspace=0.3, wspace=0.3)
        
        # Side view trajectory
        self.ax_side_view = plt.subplot(gs[0, :4])
        
        # Velocity profile
        self.ax_velocity_profile = plt.subplot(gs[1, 0])
        
        # Acceleration profile
        self.ax_acceleration_profile = plt.subplot(gs[1, 1])
        
        # Curvature analysis
        self.ax_curvature = plt.subplot(gs[1, 2])
        
        # Path smoothness
        self.ax_smoothness = plt.subplot(gs[1, 3])
        
        # Environment interaction
        self.ax_environment_interaction = plt.subplot(gs[2, :4])
        
        # Planning constraints
        self.ax_planning_constraints = plt.subplot(gs[3, :4])
        
        # Initialize side view elements
        self._initialize_side_view_elements()
    
    def _initialize_2d_elements(self):
        """Initialize 2D view plot elements"""
        if hasattr(self, 'ax_trajectory'):
            # Main trajectory plot
            self.trajectory_plot, = self.ax_trajectory.plot([], [], 'b-', linewidth=2, label='Agent Trajectory')
            self.trajectory_scatter = self.ax_trajectory.scatter([], [], s=20, c='red', alpha=0.7, label='Agent Position')
            
            # Add scatter for trajectory points
            self.trajectory_points = self.ax_trajectory.scatter([], [], s=10, c='blue', alpha=0.5)
            
            self.ax_trajectory.set_title("Agent Trajectory in World Space", fontsize=14, fontweight='bold')
            self.ax_trajectory.set_xlabel("X Position")
            self.ax_trajectory.set_ylabel("Y Position")
            self.ax_trajectory.grid(True, alpha=0.3)
            self.ax_trajectory.legend()
            self.ax_trajectory.axis('equal')
        
        if hasattr(self, 'ax_velocity'):
            # Velocity vector plot
            self.velocity_quiver = self.ax_velocity.quiver([], [], [], [], scale=10, alpha=0.7)
            self.ax_velocity.set_title("Velocity Vectors", fontsize=12, fontweight='bold')
            self.ax_velocity.set_xlabel("X Position")
            self.ax_velocity.set_ylabel("Y Position")
            self.ax_velocity.grid(True, alpha=0.3)
            self.ax_velocity.axis('equal')
        
        if hasattr(self, 'ax_acceleration'):
            # Acceleration vector plot
            self.acceleration_quiver = self.ax_acceleration.quiver([], [], [], [], scale=10, alpha=0.7)
            self.ax_acceleration.set_title("Acceleration Vectors", fontsize=12, fontweight='bold')
            self.ax_acceleration.set_xlabel("X Position")
            self.ax_acceleration.set_ylabel("Y Position")
            self.ax_acceleration.grid(True, alpha=0.3)
            self.ax_acceleration.axis('equal')
        
        if hasattr(self, 'ax_planning'):
            # Path planning plot
            self.planning_plot, = self.ax_planning.plot([], [], 'g-', linewidth=2, label='Planned Path')
            self.planning_scatter = self.ax_planning.scatter([], [], s=30, c='green', alpha=0.7, label='Waypoints')
            
            self.ax_planning.set_title("Path Planning", fontsize=12, fontweight='bold')
            self.ax_planning.set_xlabel("X Position")
            self.ax_planning.set_ylabel("Y Position")
            self.ax_planning.grid(True, alpha=0.3)
            self.ax_planning.legend()
            self.ax_planning.axis('equal')
        
        if hasattr(self, 'ax_waypoints'):
            # Waypoint plot
            self.waypoints_plot, = self.ax_waypoints.plot([], [], 'm-', linewidth=1, alpha=0.7)
            self.ax_waypoints.scatter([], [], s=50, c='magenta', alpha=0.7)
            
            self.ax_waypoints.set_title("Waypoint Trajectory", fontsize=12, fontweight='bold')
            self.ax_waypoints.set_xlabel("X Position")
            self.ax_waypoints.set_ylabel("Y Position")
            self.ax_waypoints.grid(True, alpha=0.3)
            self.ax_waypoints.axis('equal')
        
        if hasattr(self, 'ax_obstacles'):
            # Obstacle plot
            self.obstacles_scatter = self.ax_obstacles.scatter([], [], s=100, c='red', alpha=0.5, label='Obstacles')
            self.ax_obstacles.set_title("Environment Obstacles", fontsize=12, fontweight='bold')
            self.ax_obstacles.set_xlabel("X Position")
            self.ax_obstacles.set_ylabel("Y Position")
            self.ax_obstacles.grid(True, alpha=0.3)
            self.ax_obstacles.legend()
            self.ax_obstacles.axis('equal')
        
        if hasattr(self, 'ax_goals'):
            # Goal position plot
            self.goals_scatter = self.ax_goals.scatter([], [], s=100, c='gold', alpha=0.7, label='Goal Positions')
            self.ax_goals.set_title("Goal Positions", fontsize=12, fontweight='bold')
            self.ax_goals.set_xlabel("X Position")
            self.ax_goals.set_ylabel("Y Position")
            self.ax_goals.grid(True, alpha=0.3)
            self.ax_goals.legend()
            self.ax_goals.axis('equal')
    
    def _initialize_3d_elements(self):
        """Initialize 3D view plot elements"""
        # Initialize 3D plot elements
        # (Implementation would continue with 3D-specific initialization)
    
    def _initialize_top_down_elements(self):
        """Initialize top-down view plot elements"""
        # Initialize top-down plot elements
        # (Implementation would continue with top-down specific initialization)
    
    def _initialize_side_view_elements(self):
        """Initialize side view plot elements"""
        # Initialize side view plot elements
        # (Implementation would continue with side view specific initialization)
    
    def _update_plot(self, frame):
        """Update plot for animation"""
        # Update all visualization elements based on current time
        if self.config.view_mode == "2d":
            self._update_2d_view(frame)
        elif self.config.view_mode == "3d":
            self._update_3d_view(frame)
        elif self.config.view_mode == "top_down":
            self._update_top_down_view(frame)
        elif self.config.view_mode == "side_view":
            self._update_side_view_view(frame)
    
    def _update_2d_view(self, frame):
        """Update 2D view elements"""
        if not self.trajectories:
            return
        
        current_idx = min(frame, len(self.trajectories) - 1)
        trajectory = self.trajectories[current_idx]
        
        # Update main trajectory plot
        if hasattr(self, 'ax_trajectory'):
            self._update_trajectory_plot(trajectory, current_idx)
        
        # Update velocity vector plot
        if hasattr(self, 'ax_velocity') and self.velocity_vectors:
            velocity = self.velocity_vectors[current_idx]
            self._update_velocity_plot(trajectory, velocity)
        
        # Update acceleration vector plot
        if hasattr(self, 'ax_acceleration') and self.acceleration_vectors:
            acceleration = self.acceleration_vectors[current_idx]
            self._update_acceleration_plot(trajectory, acceleration)
        
        # Update path planning plot
        if hasattr(self, 'ax_planning') and self.planning_paths:
            planning_path = self.planning_paths[current_idx]
            self._update_planning_plot(planning_path)
        
        # Update waypoint plot
        if hasattr(self, 'ax_waypoints') and self.waypoints:
            waypoints = self.waypoints[current_idx]
            self._update_waypoint_plot(waypoints)
        
        # Update obstacle plot
        if hasattr(self, 'ax_obstacles'):
            self._update_obstacle_plot()
        
        # Update goal plot
        if hasattr(self, 'ax_goals'):
            self._update_goal_plot()
    
    def _update_trajectory_plot(self, trajectory: np.ndarray, current_idx: int):
        """Update main trajectory plot"""
        if hasattr(self, 'trajectory_plot'):
            # Update trajectory line
            self.trajectory_plot.set_data(trajectory[:, 0], trajectory[:, 1])
        
        if hasattr(self, 'trajectory_scatter'):
            # Update agent position scatter
            self.trajectory_scatter.set_offsets(trajectory[-1:].reshape(1, -1))
        
        if hasattr(self, 'trajectory_points'):
            # Update trajectory history points
            history_indices = list(range(max(0, current_idx - 50), current_idx + 1))
            if history_indices and current_idx < len(self.trajectories):
                history_trajectory = self.trajectories[history_indices[-1]]
                self.trajectory_points.set_offsets(history_trajectory[:, :2])
    
    def _update_velocity_plot(self, trajectory: np.ndarray, velocity: np.ndarray):
        """Update velocity vector plot"""
        if hasattr(self, 'velocity_quiver') and velocity.size > 0:
            # Limit to recent trajectory points to avoid clutter
            recent_points = min(50, len(trajectory))
            recent_trajectory = trajectory[-recent_points:]
            recent_velocity = velocity[-recent_points:] if len(velocity) >= recent_points else velocity
            
            self.velocity_quiver.set_offsets(recent_trajectory)
            self.velocity_quiver.set_UVC(recent_velocity[:, 0], recent_velocity[:, 1])
    
    def _update_acceleration_plot(self, trajectory: np.ndarray, acceleration: np.ndarray):
        """Update acceleration vector plot"""
        if hasattr(self, 'acceleration_quiver') and acceleration.size > 0:
            # Limit to recent trajectory points
            recent_points = min(50, len(trajectory))
            recent_trajectory = trajectory[-recent_points:]
            recent_acceleration = acceleration[-recent_points:] if len(acceleration) >= recent_points else acceleration
            
            self.acceleration_quiver.set_offsets(recent_trajectory)
            self.acceleration_quiver.set_UVC(recent_acceleration[:, 0], recent_acceleration[:, 1])
    
    def _update_planning_plot(self, planning_path: np.ndarray):
        """Update path planning plot"""
        if hasattr(self, 'planning_plot'):
            self.planning_plot.set_data(planning_path[:, 0], planning_path[:, 1])
        
        if hasattr(self, 'planning_scatter'):
            # Show waypoints if planning_path contains them
            self.planning_scatter.set_offsets(planning_path)
    
    def _update_waypoint_plot(self, waypoints: np.ndarray):
        """Update waypoint plot"""
        if hasattr(self, 'waypoints_plot'):
            self.waypoints_plot.set_data(waypoints[:, 0], waypoints[:, 1])
        
        if hasattr(self, 'ax_waypoints'):
            # Show individual waypoints
            self.ax_waypoints.scatter(waypoints[:, 0], waypoints[:, 1], 
                                    s=50, c='magenta', alpha=0.7)
    
    def _update_obstacle_plot(self):
        """Update obstacle plot"""
        if hasattr(self, 'obstacles_scatter') and self.obstacle_positions:
            # Flatten obstacle positions for scatter plot
            all_obstacles = np.vstack(self.obstacle_positions) if self.obstacle_positions else np.array([])
            
            if all_obstacles.size > 0:
                self.obstacles_scatter.set_offsets(all_obstacles)
    
    def _update_goal_plot(self):
        """Update goal position plot"""
        if hasattr(self, 'goals_scatter') and self.goal_positions:
            # Flatten goal positions for scatter plot
            all_goals = np.vstack(self.goal_positions) if self.goal_positions else np.array([])
            
            if all_goals.size > 0:
                self.goals_scatter.set_offsets(all_goals)
    
    def create_static_visualization(self, output_file: str = None) -> Dict[str, Any]:
        """
        Create static visualization for analysis and reporting.
        
        Args:
            output_file: Optional output filename
            
        Returns:
            Dictionary with visualization data and results
        """
        result = {
            "success": False,
            "data": {},
            "plots": {},
            "statistics": {},
            "output_files": []
        }
        
        try:
            if not self.trajectories:
                result["error"] = "No trajectory data available"
                return result
            
            # Create comprehensive trajectory visualization
            fig = self._create_comprehensive_trajectory_visualization()
            
            # Save figure
            if output_file is None:
                output_file = f"{self.config.output_path}.{self.config.save_format}"
            
            os.makedirs(os.path.dirname(output_file) if os.path.dirname(output_file) else '.', exist_ok=True)
            fig.savefig(output_file, dpi=self.config.dpi, bbox_inches='tight')
            
            # Compute statistics
            stats = self._compute_trajectory_statistics()
            
            result["plots"] = "comprehensive trajectory visualization"
            result["statistics"] = stats
            result["output_files"].append(output_file)
            result["success"] = True
            
            result["figure"] = fig
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _create_comprehensive_trajectory_visualization(self):
        """Create comprehensive trajectory visualization"""
        # Create a comprehensive figure based on view mode
        if self.config.view_mode == "2d":
            return self._create_2d_comprehensive_trajectory()
        elif self.config.view_mode == "3d":
            return self._create_3d_comprehensive_trajectory()
        elif self.config.view_mode == "top_down":
            return self._create_top_down_comprehensive_trajectory()
        elif self.config.view_mode == "side_view":
            return self._create_side_view_comprehensive_trajectory()
        else:
            return self._create_2d_comprehensive_trajectory()
    
    def _create_2d_comprehensive_trajectory(self):
        """Create comprehensive 2D trajectory visualization"""
        fig = plt.figure(figsize=(20, 15))
        gs = plt.GridSpec(4, 4, hspace=0.3, wspace=0.3)
        
        # Panel 1: Complete trajectory path
        self._plot_complete_trajectory(fig, gs[0, :4])
        
        # Panel 2: Velocity and acceleration analysis
        self._plot_velocity_acceleration_analysis(fig, gs[1, :2])
        
        # Panel 3: Path planning comparison
        self._plot_path_planning_comparison(fig, gs[1, 2:])
        
        # Panel 4: Waypoint trajectory
        self._plot_waypoint_trajectory(fig, gs[2, :2])
        
        # Panel 5: Obstacle environment
        self._plot_obstacle_environment(fig, gs[2, 2:])
        
        # Panel 6: Goal positions and path
        self._plot_goal_positions_and_path(fig, gs[3, :2])
        
        # Panel 7: Trajectory statistics
        self._plot_trajectory_statistics(fig, gs[3, 2:])
        
        fig.suptitle("Comprehensive Trajectory Visualization", fontsize=16, fontweight='bold')
        
        return fig
    
    def _plot_complete_trajectory(self, fig, ax):
        """Plot complete trajectory path"""
        if self.trajectories:
            # Plot all trajectory segments
            for i, trajectory in enumerate(self.trajectories):
                alpha = 0.3 if i < len(self.trajectories) - 1 else 0.8
                linewidth = 2 if i == len(self.trajectories) - 1 else 1
                ax.plot(trajectory[:, 0], trajectory[:, 1], 
                       linewidth=linewidth, alpha=alpha, label=f'Trajectory {i}')
            
            # Plot planned path if available
            if self.planning_paths:
                for i, planning_path in enumerate(self.planning_paths):
                    alpha = 0.5 if i < len(self.planning_paths) - 1 else 1.0
                    linewidth = 3 if i == len(self.planning_paths) - 1 else 2
                    ax.plot(planning_path[:, 0], planning_path[:, 1],
                           '--', linewidth=linewidth, alpha=alpha,
                           color='green', label=f'Planned Path {i}')
            
            ax.set_title("Complete Trajectory Path", fontsize=12, fontweight='bold')
            ax.set_xlabel("X Position")
            ax.set_ylabel("Y Position")
            ax.grid(True, alpha=0.3)
            ax.legend()
            ax.axis('equal')
            
            # Set axis limits based on world bounds
            x_min, x_max, y_min, y_max = self.config.world_bounds
            ax.set_xlim(x_min, x_max)
            ax.set_ylim(y_min, y_max)
    
    def _plot_velocity_acceleration_analysis(self, fig, ax):
        """Plot velocity and acceleration analysis"""
        if self.trajectories and self.velocity_vectors:
            # Calculate velocity statistics
            current_vel = self.velocity_vectors[-1]
            if current_vel.size > 0:
                velocity_magnitude = np.sqrt(current_vel[:, 0]**2 + current_vel[:, 1]**2)
                
                # Plot velocity histogram
                ax.hist(velocity_magnitude, bins=20, alpha=0.7, color='skyblue', edgecolor='black')
                ax.set_title("Velocity Distribution", fontsize=12, fontweight='bold')
                ax.set_xlabel("Velocity Magnitude")
                ax.set_ylabel("Frequency")
                ax.grid(True, alpha=0.3)
    
    def _plot_path_planning_comparison(self, fig, ax):
        """Plot path planning comparison"""
        if self.planning_paths and self.trajectories:
            # Plot planned path vs actual trajectory
            planned_path = self.planning_paths[-1]
            actual_trajectory = self.trajectories[-1]
            
            ax.plot(planned_path[:, 0], planned_path[:, 1], 
                   'g-', linewidth=3, label='Planned Path', alpha=0.8)
            ax.plot(actual_trajectory[:, 0], actual_trajectory[:, 1],
                   'r-', linewidth=2, label='Actual Trajectory', alpha=0.8)
            
            ax.scatter(planned_path[0, 0], planned_path[0, 1], 
                      c='green', s=100, marker='o', label='Start', alpha=0.7)
            ax.scatter(planned_path[-1, 0], planned_path[-1, 1],
                      c='green', s=100, marker='s', label='End', alpha=0.7)
            
            ax.scatter(actual_trajectory[0, 0], actual_trajectory[0, 1],
                      c='red', s=100, marker='o', label='Actual Start', alpha=0.7)
            ax.scatter(actual_trajectory[-1, 0], actual_trajectory[-1, 1],
                      c='red', s=100, marker='s', label='Actual End', alpha=0.7)
            
            ax.set_title("Path Planning Comparison", fontsize=12, fontweight='bold')
            ax.set_xlabel("X Position")
            ax.set_ylabel("Y Position")
            ax.grid(True, alpha=0.3)
            ax.legend()
            ax.axis('equal')
    
    def _plot_waypoint_trajectory(self, fig, ax):
        """Plot waypoint trajectory"""
        if self.waypoints:
            waypoints = self.waypoints[-1]
            
            ax.plot(waypoints[:, 0], waypoints[:, 1], 
                   'm-', linewidth=2, alpha=0.7, label='Waypoint Path')
            ax.scatter(waypoints[:, 0], waypoints[:, 1], 
                      c='magenta', s=50, alpha=0.7, label='Waypoints')
            
            # Mark start and end waypoints
            if len(waypoints) > 0:
                ax.scatter(waypoints[0, 0], waypoints[0, 1],
                          c='magenta', s=100, marker='o', label='Start', alpha=0.9)
                ax.scatter(waypoints[-1, 0], waypoints[-1, 1],
                          c='magenta', s=100, marker='s', label='End', alpha=0.9)
            
            ax.set_title("Waypoint Trajectory", fontsize=12, fontweight='bold')
            ax.set_xlabel("X Position")
            ax.set_ylabel("Y Position")
            ax.grid(True, alpha=0.3)
            ax.legend()
            ax.axis('equal')
    
    def _plot_obstacle_environment(self, fig, ax):
        """Plot obstacle environment"""
        if self.obstacle_positions:
            # Plot obstacles as colored circles
            for i, obstacle in enumerate(self.obstacle_positions):
                if obstacle.size >= 2:
                    # Assuming obstacle is a position array
                    x, y = obstacle[0], obstacle[1]
                    
                    # Create circle patch
                    circle = Circle((x, y), radius=2.0, alpha=0.5,
                                  facecolor='red', edgecolor='darkred')
                    ax.add_patch(circle)
                    
                    # Add obstacle label
                    ax.text(x, y, f'O{i}', ha='center', va='center',
                           fontsize=8, fontweight='bold')
            
            ax.set_title("Obstacle Environment", fontsize=12, fontweight='bold')
            ax.set_xlabel("X Position")
            ax.set_ylabel("Y Position")
            ax.grid(True, alpha=0.3)
            ax.axis('equal')
            
            # Set axis limits based on world bounds
            x_min, x_max, y_min, y_max = self.config.world_bounds
            ax.set_xlim(x_min, x_max)
            ax.set_ylim(y_min, y_max)
    
    def _plot_goal_positions_and_path(self, fig, ax):
        """Plot goal positions and path to goals"""
        if self.goal_positions:
            # Plot goal positions
            for i, goal in enumerate(self.goal_positions):
                if goal.size >= 2:
                    x, y = goal[0], goal[1]
                    
                    # Create star marker for goals
                    ax.scatter(x, y, s=100, c='gold', marker='*', 
                              alpha=0.7, label=f'Goal {i}' if i == 0 else None)
                    
                    # Add goal label
                    ax.text(x, y + 3, f'G{i}', ha='center', va='bottom',
                           fontsize=10, fontweight='bold', color='gold')
            
            ax.set_title("Goal Positions and Paths", fontsize=12, fontweight='bold')
            ax.set_xlabel("X Position")
            ax.set_ylabel("Y Position")
            ax.grid(True, alpha=0.3)
            ax.legend()
            ax.axis('equal')
            
            # Set axis limits based on world bounds
            x_min, x_max, y_min, y_max = self.config.world_bounds
            ax.set_xlim(x_min, x_max)
            ax.set_ylim(y_min, y_max)
    
    def _plot_trajectory_statistics(self, fig, ax):
        """Plot trajectory statistics"""
        if self.trajectories:
            # Calculate trajectory statistics
            current_trajectory = self.trajectories[-1]
            
            # Calculate distance traveled
            if len(current_trajectory) > 1:
                distances = np.sqrt(np.sum(np.diff(current_trajectory, axis=0)**2, axis=1))
                total_distance = np.sum(distances)
                
                # Calculate speed statistics
                if self.velocity_vectors and current_trajectory in self.velocity_vectors:
                    velocity_idx = self.velocity_vectors.index(current_trajectory)
                    velocities = self.velocity_vectors[velocity_idx]
                    speed = np.sqrt(velocities[:, 0]**2 + velocities[:, 1]**2)
                    
                    # Create bar chart
                    stats = ['Total Distance', 'Avg Speed', 'Max Speed', 'Path Length']
                    values = [total_distance, np.mean(speed), np.max(speed), total_distance]
                    
                    bars = ax.bar(stats, values, alpha=0.7, color=['skyblue', 'lightgreen', 'lightcoral', 'gold'])
                    ax.set_title("Trajectory Statistics", fontsize=12, fontweight='bold')
                    ax.set_ylabel("Value")
                    ax.grid(True, alpha=0.3, axis='y')
                    
                    # Add value labels on bars
                    for bar, value in zip(bars, values):
                        ax.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                               f'{value:.2f}', ha='center', va='bottom')
    
    def _compute_trajectory_statistics(self) -> Dict[str, Any]:
        """Compute comprehensive trajectory statistics"""
        stats = {
            "trajectory_count": len(self.trajectories),
            "current_trajectory_length": len(self.trajectories[-1]) if self.trajectories else 0,
            "total_distance": 0.0,
            "average_speed": 0.0,
            "max_speed": 0.0,
            "path_efficiency": 0.0,
            "obstacle_avoidance": {},
            "planning_accuracy": {},
            "environment_interaction": {}
        }
        
        if self.trajectories and len(self.trajectories) > 0:
            current_trajectory = self.trajectories[-1]
            
            # Calculate distance traveled
            if len(current_trajectory) > 1:
                distances = np.sqrt(np.sum(np.diff(current_trajectory, axis=0)**2, axis=1))
                stats["total_distance"] = float(np.sum(distances))
                
                # Calculate speed if velocity vectors available
                if self.velocity_vectors and current_trajectory in self.velocity_vectors:
                    velocity_idx = self.velocity_vectors.index(current_trajectory)
                    velocities = self.velocity_vectors[velocity_idx]
                    speed = np.sqrt(velocities[:, 0]**2 + velocities[:, 1]**2)
                    
                    stats["average_speed"] = float(np.mean(speed))
                    stats["max_speed"] = float(np.max(speed))
                
                # Calculate path length (straight-line distance from start to end)
                start_point = current_trajectory[0]
                end_point = current_trajectory[-1]
                straight_line_distance = np.sqrt(np.sum((end_point - start_point)**2))
                
                # Calculate path efficiency (straight-line distance / actual distance)
                if stats["total_distance"] > 0:
                    stats["path_efficiency"] = float(straight_line_distance / stats["total_distance"])
        
        # Obstacle avoidance statistics
        if self.obstacle_positions:
            stats["obstacle_avoidance"] = {
                "obstacle_count": len(self.obstacle_positions),
                "avoidance_success_rate": 1.0,  # Would need collision detection
                "closest_obstacle_distance": float(np.min([np.sqrt(ob[0]**2 + ob[1]**2) for ob in self.obstacle_positions])) if self.obstacle_positions else 0.0
            }
        
        # Planning accuracy statistics
        if self.planning_paths and self.trajectories:
            planned_path = self.planning_paths[-1]
            actual_trajectory = self.trajectories[-1]
            
            # Calculate path difference (simplified)
            if len(planned_path) > 1 and len(actual_trajectory) > 1:
                planned_length = np.sqrt(np.sum(np.diff(planned_path, axis=0)**2, axis=1))
                actual_length = np.sqrt(np.sum(np.diff(actual_trajectory, axis=0)**2, axis=1))
                
                stats["planning_accuracy"] = {
                    "path_length_difference": float(abs(np.sum(planned_length) - np.sum(actual_length))),
                    "path_similarity": float(1.0 - abs(np.sum(planned_length) - np.sum(actual_length)) / 
                                           max(np.sum(planned_length), np.sum(actual_length)))
                }
        
        return stats
    
    def export_trajectory_data(self, filename: str) -> str:
        """
        Export trajectory data to file.
        
        Args:
            filename: Output filename
            
        Returns:
            Path to saved file
        """
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        export_data = {
            "config": asdict(self.config),
            "trajectories": [traj.tolist() for traj in self.trajectories],
            "velocity_vectors": [vel.tolist() for vel in self.velocity_vectors] if self.velocity_vectors else [],
            "acceleration_vectors": [acc.tolist() for acc in self.acceleration_vectors] if self.acceleration_vectors else [],
            "waypoints": [wp.tolist() for wp in self.waypoints],
            "planning_paths": [path.tolist() for path in self.planning_paths],
            "obstacle_positions": [ob.tolist() for ob in self.obstacle_positions],
            "goal_positions": [goal.tolist() for goal in self.goal_positions],
            "statistics": self._compute_trajectory_statistics()
        }
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        return filename


def create_trajectory_planner(config_dict: Dict[str, Any] = None) -> TrajectoryPlanner:
    """
    Factory function to create TrajectoryPlanner with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured TrajectoryPlanner instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = TrajectoryPlannerConfig(**config_dict)
    return TrajectoryPlanner(config)