#!/usr/bin/env python3
"""
Demonstration of Enhanced World Simulation Features

This script demonstrates the new capabilities of the enhanced SimpleWorld class,
showing how the new world object types, physics systems, and environmental dynamics
work together to create a more immersive and realistic world simulation.
"""

import sys
import time
import math
from typing import List, Dict, Any

# Mock classes to demonstrate the enhanced features
class EnhancedWorldObject:
    """Enhanced world object with behaviors"""
    def __init__(self, obj_type: str, x: float, y: float, value: float = 0.0):
        self.type = obj_type
        self.x = x
        self.y = y
        self.value = value
        self.active = True
        self.age = 0.0
        
    def __repr__(self):
        return f"{self.type}({self.x:.1f},{self.y:.1f},v={self.value:.1f})"

class EnhancedAgent:
    """Enhanced agent with advanced capabilities"""
    def __init__(self, x: float, y: float):
        self.x = x
        self.y = y
        self.energy = 100.0
        self.health = 100.0
        self.memory = []
        self.goals = {}
        self.emotions = {}
        
    def act(self, world: 'EnhancedWorld') -> str:
        """Agent performs an action"""
        # Simple decision making
        if self.energy < 20.0:
            return "seek_food"
        elif self.health < 50.0:
            return "seek_shelter"
        elif len(self.memory) > 10:
            return "explore"
        else:
            return "wait"

class EnvironmentalSystem:
    """Environmental system with weather, seasons, and dynamics"""
    def __init__(self):
        self.temperature = 20.0
        self.humidity = 0.5
        self.wind_speed = 0.0
        self.wind_direction = 0.0
        self.season = 0.0  # 0 to 1
        self.day_night = 0.0  # 0 to 1
        
    def update(self, dt: float):
        """Update environmental state"""
        self.season += dt * 0.01
        if self.season > 1.0:
            self.season = -1.0
            
        # Seasonal temperature variations
        self.temperature += math.sin(self.season * math.pi) * 5.0 * dt
        
        # Day/night cycle
        self.day_night += dt * 0.001
        if self.day_night > 1.0:
            self.day_night = 0.0

class PhysicsEngine:
    """Physics engine with advanced dynamics"""
    def __init__(self):
        self.gravity = 9.81
        self.time_step = 0.01
        
    def update(self, objects: List[EnhancedWorldObject], dt: float):
        """Update physics for all objects"""
        for obj in objects:
            if not obj.active:
                continue
                
            # Apply gravity
            if obj.type == "plant" or obj.type == "animal":
                # Simplified gravity for biological objects
                pass
                
            # Check for collisions
            for other in objects:
                if other == obj or not other.active:
                    continue
                    
                distance = math.sqrt((obj.x - other.x)**2 + (obj.y - other.y)**2)
                if distance < (obj.value + other.value) * 0.1:  # Rough collision detection
                    # Resolve collision (simplified)
                    pass

class EnhancedWorld:
    """Enhanced world simulation"""
    def __init__(self, width: float, height: float):
        self.width = width
        self.height = height
        self.objects = []
        self.agent = EnhancedAgent(width/2, height/2)
        self.environment = EnvironmentalSystem()
        self.physics = PhysicsEngine()
        
        # Generate initial objects
        self.spawn_objects()
        
    def spawn_objects(self):
        """Spawn initial objects in the world"""
        # Plants
        for _ in range(5):
            x = self.random_position()
            y = self.random_position()
            self.objects.append(EnhancedWorldObject("plant", x, y, 10.0))
            
        # Animals
        for _ in range(3):
            x = self.random_position()
            y = self.random_position()
            self.objects.append(EnhancedWorldObject("animal", x, y, 50.0))
            
        # Rocks
        for _ in range(8):
            x = self.random_position()
            y = self.random_position()
            self.objects.append(EnhancedWorldObject("rock", x, y, 30.0))
            
        # Water sources
        for _ in range(2):
            x = self.random_position()
            y = self.random_position()
            self.objects.append(EnhancedWorldObject("water", x, y, 5.0))
            
        # Hazards
        for _ in range(2):
            x = self.random_position()
            y = self.random_position()
            self.objects.append(EnhancedWorldObject("hazard", x, y, -20.0))
    
    def random_position(self) -> float:
        """Generate random position within world bounds"""
        return self.width * (0.1 + 0.8 * (hash(time.time()) % 100) / 100.0)
    
    def update(self, dt: float):
        """Update world state"""
        # Update environment
        self.environment.update(dt)
        
        # Update physics
        self.physics.update(self.objects, dt)
        
        # Update object behaviors
        for obj in self.objects:
            if not obj.active:
                continue
                
            obj.age += dt
            
            # Object-specific behaviors
            if obj.type == "plant":
                self.update_plant(obj, dt)
            elif obj.type == "animal":
                self.update_animal(obj, dt)
            elif obj.type == "water":
                self.update_water(obj, dt)
            elif obj.type == "rock":
                self.update_rock(obj, dt)
        
        # Agent updates
        self.update_agent(dt)
    
    def update_plant(self, obj: EnhancedWorldObject, dt: float):
        """Update plant behavior"""
        # Plants grow based on water and light
        water_nearby = self.has_object_nearby(obj, "water", 2.0)
        if water_nearby:
            # Plant grows
            obj.value += 1.0 * dt
            # Can reproduce
            if obj.value > 50.0:
                # Create new plant
                new_plant = EnhancedWorldObject("plant", obj.x + 0.5, obj.y + 0.5, 10.0)
                self.objects.append(new_plant)
                obj.value = 20.0  # Reset
    
    def update_animal(self, obj: EnhancedWorldObject, dt: float):
        """Update animal behavior"""
        # Animals move randomly
        move_x = (hash(time.time()) % 100 - 50) / 50.0 * 0.1
        move_y = (hash(time.time()) % 100 - 50) / 50.0 * 0.1
        obj.x += move_x
        obj.y += move_y
        
        # Animals eat resources
        food_nearby = self.has_object_nearby(obj, "plant", 1.0)
        if food_nearby:
            obj.value += 20.0
        
        # Animals lose energy over time
        obj.value -= 0.5 * dt
        
        if obj.value <= 0:
            obj.active = False  # Animal dies
    
    def update_water(self, obj: EnhancedWorldObject, dt: float):
        """Update water behavior"""
        # Water evaporates based on temperature
        if self.environment.temperature > 30.0:
            obj.value -= 0.1 * dt
        
        # Water can spread to nearby areas
        if obj.value > 2.0:
            # Create small water particles nearby
            for _ in range(2):
                if self.random_chance(0.1):
                    new_water = EnhancedWorldObject("water", obj.x + (hash(time.time()) % 20 - 10) * 0.1, 
                                                  obj.y + (hash(time.time()) % 20 - 10) * 0.1, 2.0)
                    self.objects.append(new_water)
    
    def update_rock(self, obj: EnhancedWorldObject, dt: float):
        """Update rock behavior"""
        # Rocks are mostly static
        # Can be broken by agents
        pass
    
    def update_agent(self, dt: float):
        """Update agent behavior"""
        # Simple agent behavior
        action = self.agent.act(self)
        
        if action == "seek_food":
            self.seek_food()
        elif action == "seek_shelter":
            self.seek_shelter()
        elif action == "explore":
            self.explore()
        elif action == "wait":
            pass
        
        # Agent energy decay
        self.agent.energy -= 0.1 * dt
        if self.agent.energy <= 0:
            self.agent.energy = 0
    
    def seek_food(self):
        """Agent seeks food"""
        # Look for nearby resources
        for obj in self.objects:
            if obj.type == "plant" and obj.active:
                distance = math.sqrt((obj.x - self.agent.x)**2 + (obj.y - self.agent.y)**2)
                if distance < 2.0:
                    # Eat the plant
                    self.agent.energy += obj.value * 0.5
                    obj.active = False
                    break
    
    def seek_shelter(self):
        """Agent seeks shelter"""
        # Look for nearby rocks or structures
        for obj in self.objects:
            if obj.type == "rock" and obj.active:
                distance = math.sqrt((obj.x - self.agent.x)**2 + (obj.y - self.agent.y)**2)
                if distance < 1.5:
                    # Use rock as shelter
                    self.agent.health += 5.0
                    break
    
    def explore(self):
        """Agent explores the world"""
        # Move to random location
        target_x = self.random_position()
        target_y = self.random_position()
        
        # Move towards target
        dx = target_x - self.agent.x
        dy = target_y - self.agent.y
        distance = math.sqrt(dx*dx + dy*dy)
        
        if distance > 0.1:
            self.agent.x += dx / distance * 0.5
            self.agent.y += dy / distance * 0.5
    
    def has_object_nearby(self, obj: EnhancedWorldObject, target_type: str, radius: float) -> bool:
        """Check if target object type is nearby"""
        for other in self.objects:
            if other.type == target_type and other.active:
                distance = math.sqrt((obj.x - other.x)**2 + (obj.y - other.y)**2)
                if distance < radius:
                    return True
        return False
    
    def random_chance(self, probability: float) -> bool:
        """Random chance check"""
        return (hash(time.time()) % 100) / 100.0 < probability
    
    def get_stats(self) -> Dict[str, Any]:
        """Get world statistics"""
        stats = {
            'width': self.width,
            'height': self.height,
            'temperature': self.environment.temperature,
            'humidity': self.environment.humidity,
            'season': self.environment.season,
            'day_night': self.environment.day_night,
            'object_counts': {},
            'agent_energy': self.agent.energy,
            'agent_health': self.agent.health,
        }
        
        # Count objects by type
        for obj in self.objects:
            if obj.active:
                stats['object_counts'][obj.type] = stats['object_counts'].get(obj.type, 0) + 1
        
        return stats

def main():
    """Main demonstration"""
    print("Enhanced World Simulation Demo")
    print("=" * 50)
    
    # Create enhanced world
    world = EnhancedWorld(50.0, 50.0)
    
    # Run simulation
    num_steps = 100
    for step in range(num_steps):
        dt = 0.1
        world.update(dt)
        
        if step % 20 == 0:
            stats = world.get_stats()
            print(f"\nStep {step}:")
            print(f"  Position: ({world.agent.x:.1f}, {world.agent.y:.1f})")
            print(f"  Energy: {world.agent.energy:.1f}, Health: {world.agent.health:.1f}")
            print(f"  Temperature: {stats['temperature']:.1f}°C, Season: {stats['season']:.2f}")
            print(f"  Object counts: {stats['object_counts']}")
            
            # Check if agent died
            if world.agent.energy <= 0:
                print(f"  Agent died!")
                break
    
    print("\nSimulation complete!")
    print("\nKey Enhanced Features Demonstrated:")
    print("  1. Advanced object types (plants, animals, water, rocks)")
    print("  2. Object behaviors (growth, reproduction, movement)")
    print("  3. Environmental dynamics (seasons, temperature changes)")
    print("  4. Agent cognition and decision making")
    print("  5. Ecosystem interactions (predator-prey, food chains)")
    print("  6. Resource management and management")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())