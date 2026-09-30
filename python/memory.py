"""
Enhanced NLM Memory Management APIs

This module provides advanced memory management capabilities for the NLM framework,
including smart pointers, memory pooling, RAII wrappers, and resource management.

Features:
- Smart pointer optimizations with custom allocators
- Memory pooling with thread-safe operations
- RAII wrapper classes for resource management
- Memory alignment optimizations
- Memory allocation debugging and tracking
- Custom allocators for large data structures
- Weak reference support for circular references
"""

import sys
import threading
import os
import std
import ctypes
import mmap
from typing import Dict, List, Optional, Type, TypeVar, Generic, Callable, Any
from dataclasses import dataclass, field
from abc import ABC, abstractmethod
from enum import Enum

# Forward references
from .nlm.core.binding_helpers import NLMError, TypeValidator

T = TypeVar('T')

class AllocationStrategy(Enum):
    """Memory allocation strategies."""
    FIRST_FIT = "first_fit"
    BEST_FIT = "best_fit"
    WORST_FIT = "worst_fit"
    NEXT_FIT = "next_fit"

class SmartPointerBase(ABC):
    """Base class for custom smart pointers."""
    
    def __init__(self, ptr: Any = None):
        self._ptr = ptr
        self._ref_count = 0
        self._lock = threading.RLock()
    
    @abstractmethod
    def allocate(self, size: int) -> Any:
        """Allocate memory."""
        pass
    
    @abstractmethod
    def deallocate(self, ptr: Any):
        """Deallocate memory."""
        pass
    
    @abstractmethod
    def is_valid(self, ptr: Any) -> bool:
        """Check if pointer is valid."""
        pass
    
    def use_count(self, ptr: Any = None) -> int:
        """Get reference count for pointer."""
        target = ptr if ptr is not None else self._ptr
        if not self.is_valid(target):
            return 0
        
        with self._lock:
            return self._ref_count
    
    def add_ref(self, ptr: Any = None):
        """Add reference to pointer."""
        target = ptr if ptr is not None else self._ptr
        if self.is_valid(target):
            with self._lock:
                self._ref_count += 1
    
    def release_ref(self, ptr: Any = None) -> bool:
        """Release reference to pointer. Returns true if pointer should be deleted."""
        target = ptr if ptr is not None else self._ptr
        if not self.is_valid(target):
            return False
        
        with self._lock:
            self._ref_count -= 1
            return self._ref_count <= 0
class CustomAllocator(SmartPointerBase):
    """Custom allocator with alignment and size optimizations."""
    
    def __init__(self, alignment: int = 64, strategy: AllocationStrategy = AllocationStrategy.FIRST_FIT):
        super().__init__()
        self._alignment = alignment
        self._strategy = strategy
        self._free_blocks: List[Dict[str, Any]] = []
        self._allocated_blocks: List[Dict[str, Any]] = []
        self._total_allocated = 0
        self._stats = {
            'allocations': 0,
            'deallocations': 0,
            'peak_memory': 0,
            'current_memory': 0
        }
    
    def allocate(self, size: int) -> Any:
        """Allocate memory with proper alignment."""
        if size <= 0:
            raise ValueError("Allocation size must be positive")
        
        # Align size to cache line boundary
        aligned_size = ((size + self._alignment - 1) // self._alignment) * self._alignment
        
        with self._lock:
            # Find suitable free block
            best_block = None
            best_size_diff = float('inf')
            
            for i, block in enumerate(self._free_blocks):
                block_size = block['size']
                if block_size >= aligned_size:
                    size_diff = block_size - aligned_size
                    if self._strategy == AllocationStrategy.BEST_FIT:
                        if size_diff < best_size_diff:
                            best_size_diff = size_diff
                            best_block = i
                    elif self._strategy == AllocationStrategy.WORST_FIT:
                        if size_diff > best_size_diff:
                            best_size_diff = size_diff
                            best_block = i
                    else:  # FIRST_FIT or NEXT_FIT
                        best_block = i
                        break
            
            if best_block is None:
                raise NLMError("Out of memory")
            
            # Split block if necessary
            free_block = self._free_blocks.pop(best_block)
            if free_block['size'] > aligned_size:
                # Create remaining block
                remaining_block = {
                    'ptr': (free_block['ptr'] + aligned_size),
                    'size': free_block['size'] - aligned_size
                }
                self._free_blocks.append(remaining_block)
                if self._strategy == AllocationStrategy.NEXT_FIT:
                    # Move to end of free list
                    self._free_blocks.insert(0, self._free_blocks.pop())
            
            # Record allocation
            allocated_block = {
                'ptr': free_block['ptr'],
                'size': aligned_size,
                'aligned_size': aligned_size
            }
            self._allocated_blocks.append(allocated_block)
            
            self._stats['allocations'] += 1
            self._stats['current_memory'] += aligned_size
            self._stats['peak_memory'] = max(self._stats['peak_memory'], self._stats['current_memory'])
            
            self._total_allocated += aligned_size
            self._ref_count += 1
            
            return allocated_block['ptr']
    
    def deallocate(self, ptr: Any):
        """Deallocate memory."""
        with self._lock:
            # Find allocated block
            for i, block in enumerate(self._allocated_blocks):
                if block['ptr'] == ptr:
                    self._stats['deallocations'] += 1
                    self._stats['current_memory'] -= block['size']
                    self._total_allocated -= block['size']
                    
                    # Add to free blocks
                    self._free_blocks.append({
                        'ptr': block['ptr'],
                        'size': block['size']
                    })
                    
                    # Merge adjacent free blocks
                    self._merge_free_blocks()
                    
                    self._allocated_blocks.pop(i)
                    self._ref_count -= 1
                    return
            
            raise NLMError("Invalid pointer for deallocation")
    
    def is_valid(self, ptr: Any) -> bool:
        """Check if pointer is valid (allocated)."""
        with self._lock:
            for block in self._allocated_blocks:
                if block['ptr'] == ptr:
                    return True
            return False
    
    def _merge_free_blocks(self):
        """Merge adjacent free blocks to reduce fragmentation."""
        if len(self._free_blocks) <= 1:
            return
        
        # Sort by address
        self._free_blocks.sort(key=lambda x: x['ptr'])
        
        merged = []
        current = self._free_blocks[0]
        
        for next_block in self._free_blocks[1:]:
            if current['ptr'] + current['size'] == next_block['ptr']:
                # Merge blocks
                current['size'] += next_block['size']
            else:
                merged.append(current)
                current = next_block
        
        merged.append(current)
        self._free_blocks = merged
    
    def get_stats(self) -> Dict[str, Any]:
        """Get allocation statistics."""
        with self._lock:
            return {
                'allocations': self._stats['allocations'],
                'deallocations': self._stats['deallocations'],
                'peak_memory': self._stats['peak_memory'],
                'current_memory': self._stats['current_memory'],
                'utilization': (self._stats['current_memory'] / 
                              max(1, self._stats['peak_memory'])) * 100,
                'total_allocated': self._total_allocated
            }
    
    def reset(self):
        """Reset allocator state."""
        with self._lock:
            self._free_blocks.clear()
            self._allocated_blocks.clear()
            self._stats = {
                'allocations': 0,
                'deallocations': 0,
                'peak_memory': 0,
                'current_memory': 0
            }
            self._total_allocated = 0
            self._ref_count = 0
class MemoryPool(SmartPointerBase):
    """Memory pool for efficient allocation of fixed-size objects."""
    
    def __init__(self, object_size: int, pool_size: int, alignment: int = 64):
        super().__init__()
        self._object_size = object_size
        self._pool_size = pool_size
        self._alignment = alignment
        self._objects: List[Any] = []
        self._free_indices: List[int] = []
        self._lock = threading.RLock()
        
        # Create pool
        for i in range(pool_size):
            obj_ptr = self.allocate(object_size)
            if obj_ptr:
                self._objects.append(obj_ptr)
                self._free_indices.append(i)
    
    def allocate(self) -> Any:
        """Allocate an object from the pool."""
        with self._lock:
            if not self._free_indices:
                raise NLMError("Pool exhausted")
            
            index = self._free_indices.pop()
            self._ref_count += 1
            return self._objects[index]
    
    def deallocate(self, obj_ptr: Any):
        """Deallocate an object back to the pool."""
        with self._lock:
            for i, obj in enumerate(self._objects):
                if obj == obj_ptr:
                    self._free_indices.append(i)
                    self._ref_count -= 1
                    return
            
            raise NLMError("Invalid object pointer for deallocation")
    
    def is_valid(self, obj_ptr: Any) -> bool:
        """Check if object is valid in the pool."""
        with self._lock:
            return obj_ptr in self._objects
    
    def available_count(self) -> int:
        """Get number of available objects."""
        with self._lock:
            return len(self._free_indices)
    
    def used_count(self) -> int:
        """Get number of used objects."""
        with self._lock:
            return self._pool_size - len(self._free_indices)
    
    def clear(self):
        """Clear all objects from the pool."""
        with self._lock:
            for obj_ptr in self._objects:
                self.deallocate(obj_ptr)
            self._free_indices.clear()
            for i in range(self._pool_size):
                self._free_indices.append(i)
class WeakReferenceManager:
    """Manager for weak references to handle circular references."""
    
    def __init__(self):
        self._weak_refs: Dict[int, List[Callable]] = {}
        self._strong_refs: Dict[int, int] = {}
        self._lock = threading.RLock()
    
    def create_weak_ref(self, obj_id: int, callback: Optional[Callable] = None):
        """Create a weak reference to an object."""
        with self._lock:
            if obj_id not in self._strong_refs:
                self._strong_refs[obj_id] = 0
            
            self._strong_refs[obj_id] += 1
            
            if obj_id not in self._weak_refs:
                self._weak_refs[obj_id] = []
            
            if callback:
                self._weak_refs[obj_id].append(callback)
    
    def release_weak_ref(self, obj_id: int):
        """Release a weak reference to an object."""
        with self._lock:
            if obj_id in self._weak_refs:
                self._strong_refs[obj_id] -= 1
                if self._strong_refs[obj_id] <= 0:
                    del self._strong_refs[obj_id]
                    del self._weak_refs[obj_id]
                    
                    # Call callbacks if object is being deleted
                    if obj_id in self._weak_refs:
                        for callback in self._weak_refs[obj_id]:
                            try:
                                callback()
                            except Exception:
                                pass
class RAIIWrapper:
    """Base class for RAII wrapper classes."""
    
    def __init__(self, resource_id: str):
        self._resource_id = resource_id
        self._acquired = False
    
    def acquire(self) -> bool:
        """Acquire the resource. Returns true if successful."""
        if not self._acquired:
            # Implement resource acquisition logic
            self._acquired = True
            return True
        return False
    
    def release(self):
        """Release the resource."""
        if self._acquired:
            # Implement resource release logic
            self._acquired = False
    
    def is_acquired(self) -> bool:
        """Check if resource is acquired."""
        return self._acquired
    
    def __enter__(self):
        self.acquire()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.release()
class MemoryBuffer(RAIIWrapper):
    """RAII wrapper for memory buffers with alignment and cleanup."""
    
    def __init__(self, size: int, alignment: int = 64):
        super().__init__(f"buffer_{id(self)}")
        self._size = size
        self._alignment = alignment
        self._ptr = None
        self._capacity = 0
    
    def allocate(self):
        """Allocate memory buffer."""
        if self._acquired:
            return
        
        # Allocate aligned memory
        aligned_size = ((self._size + self._alignment - 1) // self._alignment) * self._alignment
        
        # Use aligned allocation (platform-dependent)
        if sys.platform == "linux":
            self._ptr = ctypes.create_string_buffer(aligned_size)
        else:
            # Fallback for other platforms
            self._ptr = (ctypes.c_char * aligned_size)()
        
        self._capacity = aligned_size
        self._acquired = True
    
    def resize(self, new_size: int):
        """Resize the buffer."""
        if not self._acquired:
            raise NLMError("Buffer not allocated")
        
        new_aligned_size = ((new_size + self._alignment - 1) // self._alignment) * self._alignment
        
        if new_aligned_size > self._capacity:
            # Need to reallocate
            old_ptr = self._ptr
            self._ptr = None
            self._acquired = False
            self.release()
            
            self.allocate()
            
            # Copy old data if available
            if old_ptr:
                old_size = min(self._size, new_size)
                ctypes.memmove(self._ptr, old_ptr, old_size)
            
            self._capacity = new_aligned_size
        
        self._size = new_size
    
    def get_ptr(self):
        """Get pointer to buffer data."""
        if not self._acquired or not self._ptr:
            raise NLMError("Buffer not allocated")
        return self._ptr
    
    def get_data(self):
        """Get buffer data as bytes."""
        if not self._acquired or not self._ptr:
            raise NLMError("Buffer not allocated")
        
        # Extract data from buffer
        if hasattr(self._ptr, 'raw'):
            return bytes(self._ptr.raw[:self._size])
        else:
            return bytes((ctypes.c_char * self._size).from_address(ctypes.addressof(self._ptr))[0:self._size])
    
    def write(self, data: bytes, offset: int = 0):
        """Write data to buffer."""
        if not self._acquired:
            raise NLMError("Buffer not allocated")
        
        if offset + len(data) > self._size:
            raise ValueError("Data exceeds buffer size")
        
        if hasattr(self._ptr, 'raw'):
            self._ptr.raw[offset:offset + len(data)] = data
        else:
            ctypes.memmove(ctypes.addressof(self._ptr) + offset, data, len(data))
    
    def read(self, size: int, offset: int = 0) -> bytes:
        """Read data from buffer."""
        if not self._acquired:
            raise NLMError("Buffer not allocated")
        
        if offset + size > self._size:
            raise ValueError("Read exceeds buffer size")
        
        if hasattr(self._ptr, 'raw'):
            return bytes(self._ptr.raw[offset:offset + size])
        else:
            return bytes((ctypes.c_char * size).from_address(ctypes.addressof(self._ptr) + offset))[0:size]
    
    def flush(self):
        """Flush buffer contents to underlying storage."""
        # For volatile buffers, this may be a no-op
        pass
    
    def clear(self):
        """Clear buffer contents."""
        if not self._acquired:
            return
        
        if hasattr(self._ptr, 'raw'):
            self._ptr.raw[:] = b'\x00' * self._size
        else:
            ctypes.memset(ctypes.addressof(self._ptr), 0, self._size)
    
    def __del__(self):
        if self._acquired:
            self.release()
class MemoryMappedFile(RAIIWrapper):
    """RAII wrapper for memory-mapped files."""
    
    def __init__(self, filename: str, size: int, mode: str = "r+"):
        super().__init__(f"mmap_{filename}")
        self._filename = filename
        self._size = size
        self._mode = mode
        self._mapping = None
        self._data = None
    
    def acquire(self):
        """Map file to memory."""
        if self._acquired:
            return
        
        # Open and map file
        if self._mode == "r+":
            fd = os.open(self._filename, os.O_RDWR)
            self._data = mmap.mmap(fd, self._size, mmap.MAP_SHARED, mmap.PROT_READ | mmap.PROT_WRITE, 0, fd)
            os.close(fd)
        elif self._mode == "r":
            fd = os.open(self._filename, os.O_RDONLY)
            self._data = mmap.mmap(fd, self._size, mmap.MAP_PRIVATE, mmap.PROT_READ, 0, fd)
            os.close(fd)
        elif self._mode == "w":
            fd = os.open(self._filename, os.O_CREAT | os.O_RDWR | os.O_TRUNC, 0o644)
            os.ftruncate(fd, self._size)
            self._data = mmap.mmap(fd, self._size, mmap.MAP_SHARED, mmap.PROT_READ | mmap.PROT_WRITE, 0, fd)
            os.close(fd)
        
        self._acquired = True
    
    def get_ptr(self):
        """Get pointer to mapped memory."""
        if not self._acquired or not self._data:
            raise NLMError("File not mapped")
        return self._data
    
    def get_data(self) -> bytes:
        """Get mapped data as bytes."""
        if not self._acquired or not self._data:
            raise NLMError("File not mapped")
        return self._data.tobytes()
    
    def write(self, data: bytes, offset: int = 0):
        """Write data to mapped file."""
        if not self._acquired:
            raise NLMError("File not mapped")
        
        if offset + len(data) > self._size:
            raise ValueError("Data exceeds file size")
        
        self._data[offset:offset + len(data)] = data
    
    def sync(self):
        """Sync mapped data to disk."""
        if not self._acquired:
            return
        
        self._data.sync()
    
    def __del__(self):
        if self._acquired:
            self._data.close()
            self._acquired = False
class ThreadSafeMemoryManager:
    """Thread-safe memory manager with pooling and debugging."""
    
    def __init__(self):
        self._allocators: Dict[str, CustomAllocator] = {}
        self._pools: Dict[str, MemoryPool] = {}
        self._debug_mode = False
        self._lock = threading.RLock()
        self._thread_allocators: Dict[int, str] = {}
    
    def register_allocator(self, name: str, allocator: CustomAllocator):
        """Register a custom allocator."""
        with self._lock:
            self._allocators[name] = allocator
    
    def register_pool(self, name: str, pool: MemoryPool):
        """Register a memory pool."""
        with self._lock:
            self._pools[name] = pool
    
    def allocate(self, allocator_name: str, size: int) -> Any:
        """Allocate memory using specified allocator."""
        with self._lock:
            thread_id = threading.get_ident()
            
            if allocator_name not in self._allocators:
                raise NLMError(f"Allocator not found: {allocator_name}")
            
            allocator = self._allocators[allocator_name]
            ptr = allocator.allocate(size)
            
            if self._debug_mode:
                print(f"Thread {thread_id} allocated {size} bytes using {allocator_name}")
            
            return ptr
    
    def deallocate(self, allocator_name: str, ptr: Any):
        """Deallocate memory using specified allocator."""
        with self._lock:
            thread_id = threading.get_ident()
            
            if allocator_name not in self._allocators:
                raise NLMError(f"Allocator not found: {allocator_name}")
            
            allocator = self._allocators[allocator_name]
            allocator.deallocate(ptr)
            
            if self._debug_mode:
                print(f"Thread {thread_id} deallocated {ptr} using {allocator_name}")
    
    def allocate_pooled(self, pool_name: str) -> Any:
        """Allocate from a pool."""
        with self._lock:
            if pool_name not in self._pools:
                raise NLMError(f"Pool not found: {pool_name}")
            
            pool = self._pools[pool_name]
            return pool.allocate()
    
    def deallocate_pooled(self, pool_name: str, obj_ptr: Any):
        """Deallocate to pool."""
        with self._lock:
            if pool_name not in self._pools:
                raise NLMError(f"Pool not found: {pool_name}")
            
            pool = self._pools[pool_name]
            pool.deallocate(obj_ptr)
    
    def get_allocator_stats(self, allocator_name: str) -> Dict[str, Any]:
        """Get allocator statistics."""
        with self._lock:
            if allocator_name not in self._allocators:
                raise NLMError(f"Allocator not found: {allocator_name}")
            
            return self._allocators[allocator_name].get_stats()
    
    def get_pool_stats(self, pool_name: str) -> Dict[str, Any]:
        """Get pool statistics."""
        with self._lock:
            if pool_name not in self._pools:
                raise NLMError(f"Pool not found: {pool_name}")
            
            pool = self._pools[pool_name]
            return {
                'used_count': pool.used_count(),
                'available_count': pool.available_count(),
                'total_objects': pool.used_count() + pool.available_count()
            }
    
    def enable_debug_mode(self, enable: bool = True):
        """Enable or disable debug mode."""
        self._debug_mode = enable

# Global memory manager for convenience
_global_memory_manager = ThreadSafeMemoryManager()

# Convenience functions for global memory management

def allocate_memory(allocator_name: str, size: int) -> Any:
    """Allocate memory using global memory manager."""
    return _global_memory_manager.allocate(allocator_name, size)

def deallocate_memory(allocator_name: str, ptr: Any):
    """Deallocate memory using global memory manager."""
    _global_memory_manager.deallocate(allocator_name, ptr)

def allocate_pooled(pool_name: str) -> Any:
    """Allocate from pool using global memory manager."""
    return _global_memory_manager.allocate_pooled(pool_name)

def deallocate_pooled(pool_name: str, obj_ptr: Any):
    """Deallocate to pool using global memory manager."""
    _global_memory_manager.deallocate_pooled(pool_name, obj_ptr)

# Factory functions for common allocators and pools

def create_aligned_allocator(alignment: int = 64, 
                            strategy: AllocationStrategy = AllocationStrategy.FIRST_FIT) -> CustomAllocator:
    """Create a custom aligned allocator."""
    return CustomAllocator(alignment, strategy)

def create_object_pool(object_size: int, pool_size: int, 
                      alignment: int = 64) -> MemoryPool:
    """Create an object pool for fixed-size objects."""
    return MemoryPool(object_size, pool_size, alignment)

def create_buffer(size: int, alignment: int = 64) -> MemoryBuffer:
    """Create a memory buffer with RAII."""
    buffer = MemoryBuffer(size, alignment)
    buffer.allocate()
    return buffer

print(f"Enhanced NLM Memory Management APIs loaded. {__name__}")