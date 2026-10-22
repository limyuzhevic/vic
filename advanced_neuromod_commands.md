#!/usr/bin/bash
#!/bin/bash

# NLM Advanced Neuromodulation Commands
# This script provides command-line interface for advanced neuromodulation control

echo "=== NLM Advanced Neuromodulation Commands ==="
echo "Available commands for power users:"
echo ""

echo "1. List all neuromodulators and their current states:"
echo "   ./scripts/neuromod_cmd.sh list"
echo ""

echo "2. Set neuromodulator parameters:"
echo "   ./scripts/neuromod_cmd.sh set acetylcholine_level 0.8"
echo "   ./scripts/neuromod_cmd.sh set norepinephrine_level 0.6"
echo "   ./scripts/neuromod_cmd.sh set serotonin_level 0.9"
echo "   ./scripts/neuromod_cmd.sh set dopamine_level 0.7"
echo ""

echo "3. Trigger neuromodulator responses:"
echo "   ./scripts/neuromod_cmd.sh trigger acetylcholine stress 0.5 0.8"
echo "   ./scripts/neuromod_cmd.sh trigger norepinephrine arousal 0.7"
echo "   ./scripts/neuromod_cmd.sh trigger serotonin social 0.8 0.2"
echo ""

echo "4. Configure neuromodulator weights and sensitivities:"
echo "   ./scripts/neuromod_cmd.sh config acetylcholine attention_gain 3.0"
echo "   ./scripts/neuromod_cmd.sh config norepinephrine learning_amplification 2.5"
echo "   ./scripts/neuromod_cmd.sh config serotonin patience_level 1.5"
echo ""

echo "5. Export neuromodulator states for analysis:"
echo "   ./scripts/neuromod_cmd.sh export /tmp/neuromod_state.json"
echo ""

echo "6. Import neuromodulator states:"
echo "   ./scripts/neuromod_cmd.sh import /tmp/neuromod_state.json"
echo ""

echo "7. Reset all neuromodulators:"
echo "   ./scripts/neuromod_cmd.sh reset all"
echo ""

echo "8. Get detailed neuromodulator information:"
echo "   ./scripts/neuromod_cmd.sh info acetylcholine"
echo "   ./scripts/neuromod_cmd.sh info norepinephrine"
echo "   ./scripts/neuromod_cmd.sh info serotonin"
echo ""

echo "For Python users, use the pynlm.neuromod module:"
echo "   from pynlm.neuromod import Acetylcholine, Norepinephrine, Serotonin"
echo "   ach = Acetylcholine()"
echo "   ach.setLevel(0.8)"
echo "   ach.consolidateMemory(memory_trace, strength)"
echo ""

echo "=== End of Advanced Commands ==="
