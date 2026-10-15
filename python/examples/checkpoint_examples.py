"""Checkpoint and restore examples for NLM Python bindings."""

from pynlm import NeuralSystem
import os
import tempfile
import numpy as np

print("=== NLM Checkpoint Examples ===")

# Create neural system
brain = NeuralSystem()
world = brain.create_world()

# Example 1: Basic checkpointing
print("\n1. Basic Checkpointing:")
print("Demonstrating checkpoint save/restore...")

# Create temporary directory for checkpoints
checkpoint_dir = tempfile.mkdtemp()
print(f"  Using checkpoint directory: {checkpoint_dir}")

# Run some simulation
print("  Running simulation...")
for i in range(5):
    episode, reward = brain.run_episode(max_steps=50)
    print(f"    Episode {i + 1}: {reward} reward")

# Save checkpoint
checkpoint_path = brain.save_checkpoint(os.path.join(checkpoint_dir, "brain_checkpoint.pth"))
print(f"  Checkpoint saved to: {checkpoint_path}")

# Run more simulation
print("  Running more simulation...")
for i in range(5, 10):
    episode, reward = brain.run_episode(max_steps=50)
    print(f"    Episode {i + 1}: {reward} reward")

# Restore checkpoint
print(f"  Restoring checkpoint from: {checkpoint_path}")
success = brain.load_checkpoint(checkpoint_path)
if success:
    print("  Checkpoint restored successfully")
    # Verify restoration
    metrics = brain.get_metrics()
    print(f"  Restored metrics: {metrics.total_neurons} neurons")
else:
    print("  Failed to restore checkpoint")

# Example 2: Multiple checkpoints
print("\n2. Multiple Checkpoints:")
print("Demonstrating multiple checkpoint management...")

# Create multiple checkpoints
checkpoints = []
for i in range(3):
    path = brain.save_checkpoint(os.path.join(checkpoint_dir, f"checkpoint_{i}.pth"))
    checkpoints.append(path)
    print(f"  Saved checkpoint {i + 1}: {path}")

# Restore from different checkpoint
print("  Restoring from checkpoint 1...")
success = brain.load_checkpoint(checkpoints[0])
if success:
    print("  Checkpoint 1 restored successfully")

print("  Restoring from checkpoint 2...")
success = brain.load_checkpoint(checkpoints[1])
if success:
    print("  Checkpoint 2 restored successfully")

# Example 3: Checkpoint versioning
print("\n3. Checkpoint Versioning:")
print("Demonstrating checkpoint versioning...")

# Create checkpoint versions
version_path = os.path.join(checkpoint_dir, "brain_v1.0.pth")
brain.save_checkpoint(version_path)
print(f"  Saved version 1.0 to: {version_path}")

# Upgrade and save new version
print("  Upgrading to version 2.0...")
# Simulate brain upgrade
print("  Applying upgrades...")

new_version_path = os.path.join(checkpoint_dir, "brain_v2.0.pth")
brain.save_checkpoint(new_version_path)
print(f"  Saved version 2.0 to: {new_version_path}")

# Demonstrate rollback
print("  Rolling back to version 1.0...")
success = brain.load_checkpoint(version_path)
if success:
    print("  Successfully rolled back to version 1.0")

# Example 4: Checkpoint analysis
print("\n4. Checkpoint Analysis:")
print("Analyzing checkpoint data...")

# List all checkpoints
checkpoint_files = [f for f in os.listdir(checkpoint_dir) if f.endswith('.pth')]
print(f"  Found {len(checkpoint_files)} checkpoint files:")
for f in checkpoint_files:
    full_path = os.path.join(checkpoint_dir, f)
    # Check file size
    file_size = os.path.getsize(full_path) / 1024  # KB
    print(f"    {f}: {file_size:.2f} KB")

# Example 5: Checkpoint best practices
print("\n5. Checkpoint Best Practices:")
print("Demonstrating checkpoint best practices...")

print("  Best practices:")
print("    1. Save checkpoints frequently")
print("    2. Use descriptive file names")
print("    3. Store checkpoints in multiple locations")
print("    4. Verify checkpoint integrity")
print("    5. Test checkpoint restoration")
print("    6. Use compression for large checkpoints")
print("    7. Include metadata in checkpoint name")

# Example 6: Automated checkpoint management
print("\n6. Automated Checkpoint Management:")
print("Demonstrating automated checkpoint management...")

print("  Simulating automated checkpoint management...")
print("    - Automatic checkpoint saving after X steps")
print("    - Checkpoint cleanup old versions")
print("    - Checkpoint verification")
print("    - Performance optimization")

print("\nCheckpoint examples completed!")
