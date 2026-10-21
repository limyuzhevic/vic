#!/usr/bin/env python3
"""
Animation Generator Tool

Generate videos of simulation dynamics for NLM.
Provides comprehensive animation generation capabilities for NLM simulations.
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.animation import FFMpegWriter, PillowWriter
import imageio
import os
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
import sys
from datetime import datetime

@dataclass
class AnimationGeneratorConfig:
    """Configuration for animation generator"""
    animation_type: str = "continuous"  # "continuous", "event", "statistical"
    duration: float = 30.0  # seconds
    fps: int = 30  # frames per second
    resolution: Tuple[int, int] = (1920, 1080)  # HD resolution
    codec: str = "ffmpeg"  # "ffmpeg", "pillow", "av", "gif"
    quality: str = "high"  # "low", "medium", "high", "ultra"
    output_path: str = "animation"
    format: str = "mp4"  # "mp4", "gif", "avi", "webm", "mov"
    include_audio: bool = False
    include_timestamps: bool = True
    show_trajectory: bool = True
    show_connections: bool = True
    show_neurons: bool = True
    show_activity: bool = True
    color_scheme: str = "default"  # "default", "activity", "time", "gradient"
    smooth_transitions: bool = True
    loop_count: int = 1
    
    # Animation sections
    show_intro: bool = True
    show_analysis: bool = True
    show_conclusion: bool = True
    
    # Quality settings
    antialiased: bool = True
    interpolation: str = "linear"  # "none", "linear", "cubic"
class AnimationGenerator:
    """
    Animation generator tool for NLM simulation animations.
    
    Features:
    - Generate continuous animations of neural network dynamics
    - Create event-based animations with specific triggers
    - Generate statistical visualizations and analysis
    - Support for multiple output formats and codecs
    - High-quality video production
    - Frame rate control and timing
    - Progress tracking and preview generation
    - Batch animation generation
    """
    
    def __init__(self, config: AnimationGeneratorConfig = None):
        self.config = config or AnimationGeneratorConfig()
        self.frames = []  # List of animation frames
        self.current_frame = 0
        self.animation_writer = None
        self.preview_image = None
        
        # Initialize generator components
        self._initialize_components()
    
    def _initialize_components(self):
        """Initialize animation generator components and data structures"""
        # Animation buffers
        self.frame_buffer = []
        self.preview_buffer = []
        self.metadata_buffer = []
        
        # Initialize animation writer based on configuration
        self._initialize_animation_writer()
        
        # Initialize frame generator
        self._setup_frame_generator()
    
    def _initialize_animation_writer(self):
        """Initialize animation writer based on configuration"""
        self.writer_info = {
            "codec": self.config.codec,
            "fps": self.config.fps,
            "resolution": self.config.resolution,
            "duration": self.config.duration,
            "quality": self.config.quality,
            "format": self.config.format,
            "loop": self.config.loop_count,
            "antialiased": self.config.antialiased,
            "interpolation": self.config.interpolation
        }
    
    def _setup_frame_generator(self):
        """Setup frame generation for animation"""
        # Generate frames based on animation type
        if self.config.animation_type == "continuous":
            self._generate_continuous_frames()
        elif self.config.animation_type == "event":
            self._generate_event_frames()
        elif self.config.animation_type == "statistical":
            self._generate_statistical_frames()
        else:
            self._generate_continuous_frames()
    
    def _generate_continuous_frames(self):
        """Generate continuous animation frames"""
        n_frames = int(self.config.duration * self.config.fps)
        
        for frame in range(n_frames):
            time = frame / self.config.fps
            frame_data = self._create_continuous_frame(time)
            self.frame_buffer.append(frame_data)
    
    def _generate_event_frames(self):
        """Generate event-based animation frames"""
        n_frames = int(self.config.duration * self.config.fps)
        
        # Create event timeline
        event_times = [0, 10, 20, 30]  # Example event times
        event_types = ["start", "peak", "transition", "end"]
        
        for frame in range(n_frames):
            time = frame / self.config.fps
            frame_data = self._create_event_frame(time, event_times, event_types)
            self.frame_buffer.append(frame_data)
    
    def def _generate_statistical_frames(self):
        """Generate statistical animation frames"""
        n_frames = int(self.config.duration * self.config.fps)
        
        for frame in range(n_frames):
            time = frame / self.config.fps
            frame_data = self._create_statistical_frame(time)
            self.frame_buffer.append(frame_data)
    
    def _create_continuous_frame(self, time: float) -> Dict[str, Any]:
        """Create continuous animation frame"""
        frame = {
            "time": time,
            "frame_id": self.current_frame,
            "timestamp": self._get_timestamp(time),
            "trajectory": self._generate_trajectory(time),
            "connections": self._generate_connections(time),
            "neurons": self._generate_neurons(time),
            "activity": self._generate_activity(time),
            "metadata": self._get_frame_metadata(time)
        }
        
        self.current_frame += 1
        return frame
    
    def _create_event_frame(self, time: float, event_times: List[float], event_types: List[str]) -> Dict[str, Any]:
        """Create event-based animation frame"""
        # Determine current event
        current_event = None
        for i, event_time in enumerate(event_times):
            if abs(time - event_time) < 0.1:  # Close to event time
                current_event = event_types[i]
                break
        
        frame = {
            "time": time,
            "frame_id": self.current_frame,
            "timestamp": self._get_timestamp(time),
            "event": current_event,
            "trajectory": self._generate_trajectory(time, event=current_event),
            "connections": self._generate_connections(time, event=current_event),
            "neurons": self._generate_neurons(time, event=current_event),
            "activity": self._generate_activity(time, event=current_event),
            "metadata": self._get_frame_metadata(time, event=current_event)
        }
        
        self.current_frame += 1
        return frame
    
    def _create_statistical_frame(self, time: float) -> Dict[str, Any]:
        """Create statistical animation frame"""
        frame = {
            "time": time,
            "frame_id": self.current_frame,
            "timestamp": self._get_timestamp(time),
            "statistics": self._generate_statistics(time),
            "distribution": self._generate_distribution(time),
            "correlation": self._generate_correlation(time),
            "trends": self._generate_trends(time),
            "metadata": self._get_frame_metadata(time)
        }
        
        self.current_frame += 1
        return frame
    
    def _generate_trajectory(self, time: float, event: str = None) -> Dict[str, Any]:
        """Generate trajectory visualization"""
        # Generate trajectory based on time and event
        n_points = 50
        t = np.linspace(0, time, n_points)
        
        # Create trajectory based on animation type
        if event:
            # Event-based trajectory
            if event == "start":
                x = np.sin(t * 2) * 10
                y = np.cos(t * 2) * 10
            elif event == "peak":
                x = np.exp(-t) * np.sin(t * 3) * 20
                y = np.exp(-t) * np.cos(t * 3) * 20
            elif event == "transition":
                x = np.sin(t * 0.5 + np.pi/4) * 15
                y = np.cos(t * 0.5) * 15
            else:  # end
                x = np.cos(t * 4) * 5
                y = np.sin(t * 4) * 5
        else:
            # Continuous trajectory
            x = np.sin(t * 1.5) * 15
            y = np.cos(t * 1.5) * 15
        
        return {
            "x": x.tolist(),
            "y": y.tolist(),
            "color": self._get_trajectory_color(time, event),
            "width": 3,
            "style": "solid" if not event else "dashed"
        }
    
    def _generate_connections(self, time: float, event: str = None) -> Dict[str, Any]:
        """Generate connection visualization"""
        # Generate connection network
        n_neurons = 20
        connections = []
        
        for i in range(n_neurons):
            for j in range(i + 1, n_neurons):
                # Create connections based on time
                distance = np.sqrt((i - j)**2 + (time % 10)**2)
                strength = max(0, 1 - distance / 20)
                
                if event:
                    strength *= self._get_event_factor(event)
                
                connections.append({
                    "source": i,
                    "target": j,
                    "strength": strength,
                    "color": self._get_connection_color(strength, time),
                    "width": int(strength * 5)
                })
        
        return {
            "connections": connections,
            "n_connections": len(connections),
            "average_strength": np.mean([c["strength"] for c in connections]) if connections else 0
        }
    
    def _generate_neurons(self, time: float, event: str = None) -> Dict[str, Any]:
        """Generate neuron visualization"""
        # Generate neurons in a network pattern
        n_neurons = 20
        neurons = []
        
        for i in range(n_neurons):
            # Calculate neuron position
            angle = 2 * np.pi * i / n_neurons + time * 0.1
            radius = 15 + np.sin(time * 0.5) * 5
            x = radius * np.cos(angle)
            y = radius * np.sin(angle)
            
            # Calculate neuron properties
            size = 10 + np.sin(time * 0.8 + i) * 5
            activity = self._get_neuron_activity(i, time, event)
            color = self._get_neuron_color(activity, time, event)
            
            neurons.append({
                "id": i,
                "x": x,
                "y": y,
                "size": size,
                "activity": activity,
                "color": color,
                "pulse": activity > 0.7
            })
        
        return {
            "neurons": neurons,
            "n_neurons": n_neurons,
            "average_activity": np.mean([n["activity"] for n in neurons])
        }
    
    def _generate_activity(self, time: float, event: str = None) -> Dict[str, Any]:
        """Generate activity visualization"""
        # Generate activity heatmap
        n_neurons = 20
        activity_grid = np.zeros((n_neurons, n_neurons))
        
        for i in range(n_neurons):
            for j in range(n_neurons):
                # Calculate activity based on position and time
                dist = np.sqrt((i - j)**2 + (time % 10)**2)
                activity = np.exp(-dist / 5) * (0.5 + 0.5 * np.sin(time + i + j))
                
                if event:
                    activity *= self._get_event_factor(event)
                
                activity_grid[i, j] = activity
        
        return {
            "grid": activity_grid.tolist(),
            "heatmap": self._create_heatmap(activity_grid),
            "max_activity": np.max(activity_grid),
            "min_activity": np.min(activity_grid),
            "mean_activity": np.mean(activity_grid)
        }
    
    def _generate_statistics(self, time: float) -> Dict[str, Any]:
        """Generate statistical visualization"""
        # Generate statistics based on time
        statistics = {
            "frame_time": time,
            "neuron_count": 20,
            "connection_count": 190,
            "activity_mean": np.sin(time) * 0.5 + 0.5,
            "activity_std": np.cos(time) * 0.2 + 0.3,
            "connectivity": np.exp(-time / 10) * 0.8 + 0.2,
            "efficiency": np.sin(time * 0.5) * 0.6 + 0.4
        }
        
        return statistics
    
    def _generate_distribution(self, time: float) -> Dict[str, Any]:
        """Generate distribution visualization"""
        # Generate distribution data
        n_samples = 1000
        distribution = np.random.normal(
            loc=np.sin(time) * 10,
            scale=np.cos(time) * 3 + 1,
            size=n_samples
        )
        
        return {
            "data": distribution.tolist(),
            "mean": float(np.mean(distribution)),
            "std": float(np.std(distribution)),
            "histogram": self._create_histogram(distribution)
        }
    
    def _generate_correlation(self, time: float) -> Dict[str, Any]:
        """Generate correlation visualization"""
        # Generate correlation matrix
        n_neurons = 10
        correlation_matrix = np.random.rand(n_neurons, n_neurons) * 0.8
        correlation_matrix = (correlation_matrix + correlation_matrix.T) / 2
        np.fill_diagonal(correlation_matrix, 1.0)
        
        return {
            "matrix": correlation_matrix.tolist(),
            "correlation_coefficient": np.corrcoef(correlation_matrix.flatten()),
            "sparsity": np.mean(correlation_matrix == 0)
        }
    
    def _generate_trends(self, time: float) -> Dict[str, Any]:
        """Generate trend visualization"""
        # Generate trend data
        trends = {
            "activity_trend": np.sin(time * 0.5),
            "connectivity_trend": np.cos(time * 0.3),
            "efficiency_trend": np.sin(time * 0.7),
            "growth_rate": np.exp(-time / 20) * 0.5 + 0.5
        }
        
        return trends
    
    def _create_heatmap(self, data: np.ndarray) -> Dict[str, Any]:
        """Create heatmap representation"""
        # Normalize data for visualization
        normalized = (data - np.min(data)) / (np.max(data) - np.min(data) + 1e-10)
        
        return {
            "normalized": normalized.tolist(),
            "colors": self._get_heatmap_colors(normalized),
            "min": float(np.min(data)),
            "max": float(np.max(data)),
            "mean": float(np.mean(data))
        }
    
    def _create_histogram(self, data: np.ndarray) -> Dict[str, Any]:
        """Create histogram representation"""
        # Create histogram bins
        hist, bins = np.histogram(data, bins=20, density=True)
        
        return {
            "bins": bins.tolist(),
            "frequencies": hist.tolist(),
            "bin_width": bins[1] - bins[0],
            "total_count": len(data)
        }
    
    def _get_trajectory_color(self, time: float, event: str = None) -> str:
        """Get trajectory color based on time and event"""
        if event:
            event_colors = {
                "start": "green",
                "peak": "red",
                "transition": "blue",
                "end": "orange"
            }
            return event_colors.get(event, "black")
        else:
            # Color based on time
            hue = (time % 10) / 10
            return self._get_color_from_hue(hue)
    
    def _get_connection_color(self, strength: float, time: float) -> str:
        """Get connection color based on strength and time"""
        if self.config.color_scheme == "activity":
            return self._get_color_from_hue(strength)
        elif self.config.color_scheme == "time":
            return self._get_color_from_hue((time % 10) / 10)
        elif self.config.color_scheme == "gradient":
            return self._get_gradient_color(strength)
        else:
            return "blue"
    
    def _get_neuron_color(self, activity: float, time: float, event: str = None) -> str:
        """Get neuron color based on activity and time"""
        if self.config.color_scheme == "activity":
            return self._get_color_from_hue(activity)
        elif self.config.color_scheme == "time":
            return self._get_color_from_hue((time % 10) / 10)
        elif self.config.color_scheme == "gradient":
            return self._get_gradient_color(activity)
        else:
            # Default color scheme
            if event:
                event_colors = {
                    "start": "lightgreen",
                    "peak": "lightred",
                    "transition": "lightblue",
                    "end": "lightyellow"
                }
                return event_colors.get(event, "lightgray")
            else:
                return "lightblue"
    
    def _get_event_factor(self, event: str) -> float:
        """Get event factor for intensity adjustment"""
        event_factors = {
            "start": 1.5,
            "peak": 2.0,
            "transition": 1.2,
            "end": 0.8
        }
        return event_factors.get(event, 1.0)
    
    def _get_neuron_activity(self, neuron_id: int, time: float, event: str = None) -> float:
        """Get neuron activity based on ID, time, and event"""
        base_activity = np.sin(time * 0.5 + neuron_id) * 0.5 + 0.5
        
        if event:
            event_factor = self._get_event_factor(event)
            base_activity *= event_factor
        
        return min(1.0, max(0.0, base_activity))
    
    def _get_color_from_hue(self, hue: float) -> str:
        """Get color from hue value (0-1)"""
        import colorsys
        r, g, b = colorsys.hsv_to_rgb(hue, 0.8, 0.9)
        return f"#{int(r*255):02x}{int(g*255):02x}{int(b*255):02x}"
    
    def _get_gradient_color(self, value: float) -> str:
        """Get color from gradient based on value (0-1)"""
        # Create gradient from blue to red
        if value < 0.5:
            # Blue to white
            t = value / 0.5
            r, g, b = t, t, 1.0
        else:
            # White to red
            t = (value - 0.5) / 0.5
            r, g, b = 1.0, 1.0 - t, 1.0 - t
        
        return f"#{int(r*255):02x}{int(g*255):02x}{int(b*255):02x}"
    
    def _get_heatmap_colors(self, data: np.ndarray) -> List[str]:
        """Get colors for heatmap"""
        colors = []
        for value in data.flatten():
            colors.append(self._get_heatmap_color(value))
        return colors
    
    def _get_heatmap_color(self, value: float) -> str:
        """Get heatmap color for value"""
        # Blue to red heatmap
        if value < 0.5:
            t = value / 0.5
            r, g, b = t, t, 1.0
        else:
            t = (value - 0.5) / 0.5
            r, g, b = 1.0, 1.0 - t, 1.0 - t
        
        return f"#{int(r*255):02x}{int(g*255):02x}{int(b*255):02x}"
    
    def _get_frame_metadata(self, time: float, event: str = None) -> Dict[str, Any]:
        """Get frame metadata"""
        metadata = {
            "time": time,
            "frame_id": self.current_frame,
            "timestamp": self._get_timestamp(time),
            "quality": self.config.quality,
            "resolution": self.config.resolution
        }
        
        if event:
            metadata["event"] = event
        
        return metadata
    
    def _get_timestamp(self, time: float) -> str:
        """Get timestamp for given time"""
        hours = int(time) // 3600
        minutes = (int(time) % 3600) // 60
        seconds = int(time) % 60
        milliseconds = int((time - int(time)) * 1000)
        
        return f"{hours:02d}:{minutes:02d}:{seconds:02d}.{milliseconds:03d}"
    
    def _get_timestamp(self) -> str:
        """Get current timestamp"""
        return datetime.now().isoformat()
    
    def create_animation(self, output_file: str = None) -> Dict[str, Any]:
        """
        Create animation from frames.
        
        Args:
            output_file: Output filename (uses config output_path if None)
            
        Returns:
            Dictionary with animation results
        """
        result = {
            "success": False,
            "animation_data": {},
            "metadata": {},
            "output_file": None,
            "frames_generated": len(self.frame_buffer),
            "animation_type": self.config.animation_type,
            "duration": self.config.duration,
            "fps": self.config.fps
        }
        
        try:
            # Set output filename
            if output_file is None:
                output_file = f"{self.config.output_path}.{self.config.format}"
            
            # Create preview image
            self._create_preview()
            
            # Save animation
            saved_file = self._save_animation(output_file)
            
            # Update result
            result["output_file"] = saved_file
            result["animation_data"] = {
                "frames": self.frame_buffer,
                "metadata": self.metadata_buffer
            }
            result["success"] = True
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _create_preview(self):
        """Create preview image"""
        if self.frame_buffer:
            self.preview_image = self.frame_buffer[0]
            self.preview_buffer.append(self.preview_image)
    
    def _save_animation(self, filename: str) -> str:
        """Save animation to file"""
        # Save based on format
        if self.config.format in ["mp4", "avi", "mov", "webm"]:
            return self._save_video(filename)
        elif self.config.format == "gif":
            return self._save_gif(filename)
        elif self.config.format == "png":
            return self._save_frames_as_images(filename)
        else:
            return self._save_default(filename)
    
    def _save_video(self, filename: str) -> str:
        """Save animation as video"""
        # Create video from frames
        if self.config.codec == "ffmpeg":
            self._save_with_ffmpeg(filename)
        elif self.config.codec == "pillow":
            self._save_with_pillow(filename)
        elif self.config.codec == "av":
            self._save_with_av(filename)
        else:
            self._save_with_ffmpeg(filename)
        
        return filename
    
    def _save_with_ffmpeg(self, filename: str):
        """Save animation using FFmpeg"""
        import subprocess
        
        # Prepare frame files
        frame_dir = f"{os.path.dirname(filename)}/frames"
        os.makedirs(frame_dir, exist_ok=True)
        
        for i, frame in enumerate(self.frame_buffer):
            frame_file = f"{frame_dir}/frame_{i:06d}.png"
            self._save_frame_as_image(frame, frame_file)
        
        # Use FFmpeg to create video
        cmd = [
            "ffmpeg", "-y",
            "-framerate", str(self.config.fps),
            "-i", f"{frame_dir}/frame_%06d.png",
            "-c:v", "libx264",
            "-pix_fmt", "yuv420p",
            "-crf", "23",
            "-preset", "medium",
            filename
        ]
        
        subprocess.run(cmd, check=True)
        
        # Clean up frame directory
        import shutil
        shutil.rmtree(frame_dir)
    
    def _save_with_pillow(self, filename: str):
        """Save animation using Pillow"""
        from PIL import Image
        
        # Create video using Pillow (simplified)
        frame_images = []
        for frame in self.frame_buffer:
            img = self._frame_to_image(frame)
            frame_images.append(img)
        
        # Save first frame as PNG (Pillow doesn't support video directly)
        frame_images[0].save(filename.replace('.mp4', '.png').replace('.avi', '.png'))
    
    def _save_with_av(self, filename: str):
        """Save animation using AV library"""
        # Placeholder for AV library implementation
        pass
    
    def _save_gif(self, filename: str):
        """Save animation as GIF"""
        # Create GIF from frames
        import imageio
        
        images = []
        for frame in self.frame_buffer:
            img = self._frame_to_image(frame)
            images.append(img)
        
        # Save GIF
        imageio.mimsave(filename, images, fps=self.config.fps, loop=self.config.loop_count)
    
    def _save_frames_as_images(self, filename: str):
        """Save frames as individual images"""
        # Create directory for frames
        frame_dir = os.path.dirname(filename) if os.path.dirname(filename) else '.'
        os.makedirs(frame_dir, exist_ok=True)
        
        # Save each frame as PNG
        for i, frame in enumerate(self.frame_buffer):
            frame_file = f"{frame_dir}/frame_{i:06d}.png"
            self._save_frame_as_image(frame, frame_file)
        
        return filename
    
    def _save_default(self, filename: str):
        """Default save method"""
        # Save as JSON (default)
        import json
        
        with open(filename, 'w') as f:
            json.dump(self.frame_buffer, f, indent=2)
        
        return filename
    
    def _save_frame_as_image(self, frame: Dict[str, Any], filename: str):
        """Save frame as image"""
        # Create image from frame data
        img = self._frame_to_image(frame)
        img.save(filename)
    
    def _frame_to_image(self, frame: Dict[str, Any]):n        """Convert frame to image"""
        # Create matplotlib figure
        fig, ax = plt.subplots(figsize=(self.config.resolution[0]/100, self.config.resolution[1]/100))
        
        # Plot trajectory
        if "trajectory" in frame:
            traj = frame["trajectory"]
            ax.plot(traj["x"], traj["y"], color=traj["color"], linewidth=traj["width"], alpha=0.7)
        
        # Plot neurons
        if "neurons" in frame:
            neurons = frame["neurons"]["neurons"]
            for neuron in neurons:
                color = neuron["color"]
                size = neuron["size"]
                ax.scatter([neuron["x"]], [neuron["y"]], s=size*10, c=color, alpha=0.7, edgecolors='black')
        
        # Plot connections
        if "connections" in frame:
            connections = frame["connections"]["connections"]
            for conn in connections:
                if conn["width"] > 0:
                    ax.plot([conn["source"], conn["target"]], [0, 0], color=conn["color"], 
                           linewidth=conn["width"]*0.1, alpha=0.3)
        
        # Remove axes for clean visualization
        ax.set_xlim(-20, 20)
        ax.set_ylim(-20, 20)
        ax.set_aspect('equal')
        ax.axis('off')
        
        # Add timestamp if enabled
        if self.config.include_timestamps and "timestamp" in frame["metadata"]:
            ax.text(0.95, 0.05, frame["metadata"]["timestamp"], 
                   transform=ax.transAxes, fontsize=8, ha='right', va='bottom',
                   bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
        
        # Save figure
        fig.tight_layout()
        img = fig
        
        # Convert to numpy array for imageio
        img_array = np.frombuffer(img.tobytes(), dtype=np.uint8)
        img_array = img_array.reshape(img.height, img.width, -1)
        
        plt.close(fig)
        
        return img_array
    
    def batch_create_animations(self, 
                               input_files: List[str],
                               output_format: str = None,
                               output_directory: str = None) -> Dict[str, Any]:
        """
        Create multiple animations in batch.
        
        Args:
            input_files: List of input file paths
            output_format: Output format (uses config if None)
            output_directory: Output directory (uses config output_path if None)
            
        Returns:
            Dictionary with batch animation results
        """
        results = {
            "total_files": len(input_files),
            "successful_animations": 0,
            "failed_animations": 0,
            "results": []
        }
        
        for i, input_file in enumerate(input_files):
            # Determine output filename
            base_name = os.path.splitext(os.path.basename(input_file))[0]
            output_filename = f"{output_directory or self.config.output_path}/{base_name}.{output_format or self.config.format}"
            
            try:
                # Create animation
                result = self.create_animation(output_filename)
                
                results["results"].append(result)
                if result["success"]:
                    results["successful_animations"] += 1
                else:
                    results["failed_animations"] += 1
                    
            except Exception as e:
                results["results"].append({
                    "success": False,
                    "input_file": input_file,
                    "error": str(e)
                })
                results["failed_animations"] += 1
        
        return results


def create_animation_generator(config_dict: Dict[str, Any] = None) -> AnimationGenerator:
    """
    Factory function to create AnimationGenerator with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured AnimationGenerator instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = AnimationGeneratorConfig(**config_dict)
    return AnimationGenerator(config)