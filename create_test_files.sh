#!/bin/bash

# Make build script executable
chmod +x build_bindings.sh
chmod +x test_bindings.py

# Create a simple test to verify the bindings can be imported
echo "#!/usr/bin/env python3" > test_import.py
echo "import pynlm" >> test_import.py
echo "print('✓ NLM Python bindings imported successfully')" >> test_import.py

echo "To test the bindings:"
echo "1. Run: ./build_bindings.sh"
echo "2. Install with: pip install ."
echo "3. Test with: python test_bindings.py"
echo ""
echo "For quick verification:"
echo "python test_import.py"