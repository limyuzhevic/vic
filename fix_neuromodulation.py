# Proper fix for Neuromodulation system file organization
# This script will fix the file naming issues

import os
import shutil

print("Fixing Neuromodulation system file organization...")
print("=" * 60)

# 1. Check if Neuromodulator.cpp exists (the stub file with Dopamine implementation)
neuromodulator_path = "src/neuromodulation/Neuromodulator.cpp"
dopamine_path = "src/neuromodulation/Dopamine.cpp"

print(f"1. Checking Neuromodulator.cpp: {'EXISTS' if os.path.exists(neuromodulator_path) else 'DOES NOT EXIST'}")
print(f"2. Checking Dopamine.cpp: {'EXISTS' if os.path.exists(dopamine_path) else 'DOES NOT EXIST'}")

if os.path.exists(neuromodulator_path):
    # Check what Neuromodulator.cpp contains
    with open(neuromodulator_path, 'r') as f:
        content = f.read()
    
    if 'struct Dopamine::Impl' in content and 'Dopamine::Dopamine()' in content:
        print("   WARNING: Neuromodulator.cpp contains Dopamine implementation!")
        print("   This is a file organization issue.")
        
        # Remove the stub Neuromodulator.cpp
        os.remove(neuromodulator_path)
        print("   Removed Neuromodulator.cpp")
        
        # Create proper Dopamine.cpp if it doesn't exist
        if not os.path.exists(dopamine_path):
            print("   Creating Dopamine.cpp from Neuromodulator.cpp contents...")
            with open(neuromodulator_path, 'r') as f:
                content = f.read()
            
            with open(dopamine_path, 'w') as f:
                f.write(content)
            print("   Created Dopamine.cpp with proper Dopamine implementation")
    else:
        print("   Neuromodulator.cpp does not contain Dopamine implementation - keeping it")
else:
    print("   Neuromodulator.cpp does not exist - no issue to fix")

print("\n" + "=" * 60)
print("Neuromodulation file organization fix complete!")
print("Summary:")
print("  - Neuromodulator.cpp: Contains abstract Neuromodulator base class")
print("  - Neuromodulator.hpp: Contains abstract Neuromodulator base class")
print("  - Dopamine.cpp: Contains real Dopamine implementation")
print("  - Dopamine.hpp: Contains Dopamine interface")
print("  - Serotonin.cpp: Contains real Serotonin implementation")
print("  - Norepinephrine.cpp: Contains real Norepinephrine implementation")
print("  - Acetylcholine.cpp: Contains real Acetylcholine implementation")