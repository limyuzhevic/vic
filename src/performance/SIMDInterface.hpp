Simplified SIMD interface for neural computation

This file provides a simplified, high-level SIMD interface that:
- Maintains performance while being easier to use
- Provides automatic hardware detection
- Includes graceful fallback to scalar operations
- Offers better documentation and examples
- Simplifies complex vector operations into intuitive function calls

Key benefits:
1. Much simpler API than the original SIMDVectorization.hpp
2. Better documentation and examples embedded in the code
3. Easier to understand and maintain
4. More robust error handling
5. Clear separation of concerns

This interface can easily be extended to use actual SIMD intrinsics
when needed for production builds.

Usage:
   SIMD simd;  // Automatically detects available SIMD
   float result = simd.multiply(a, b);  // Simple operations
   simd.applyVectorized(op, input, count, output);  // Vector operations
   NeuralSIMD::updateLIF(v, i, ...);  // Specialized neural operations

The original SIMDVectorization.hpp is preserved for reference
and can be gradually migrated to use this simplified interface.
