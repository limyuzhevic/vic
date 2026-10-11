#!/usr/bin/env python3

"""
NLM Phase 6 Integration - Implementation Script

This script provides specific implementation steps for improving NLM integration
based on the Phase 6 Final Audit findings.
"""

import json
import sys
from pathlib import Path
from typing import Dict, List

class NLMIntegrationScript:
    def __init__(self, workspace_dir: str):
        self.workspace_dir = Path(workspace_dir)
        self.improvements = []
        
    def add_improvement(self, category: str, description: str, files: List[str], priority: str):
        """Add an improvement to the list"""
        improvement = {
            'category': category,
            'description': description,
            'files': files,
            'priority': priority,
            'status': 'pending'
        }
        self.improvements.append(improvement)
        return improvement
    
    def generate_implementation_plan(self) -> Dict:
        """Generate comprehensive implementation plan"""
        
        # Phase 1: CRITICAL INTEGRATION FIXES
        self.add_improvement(
            'Critical Integration',
            'Connect prediction system to sensory processing loop in Brain::step()',
            [
                'src/brain/Brain.cpp',
                'src/prediction/PredictionSystem.cpp',
                'src/agent/AgentBrain.cpp',
                'src/neuromodulation/Neuromodulator.cpp'
            ],
            'HIGH'
        )
        
        self.add_improvement(
            'Critical Integration',
            'Implement neural planner instantiation and connection to action selection',
            [
                'src/brain/Brain.cpp',
                'src/cognition/NeuralPlanner.cpp',
                'src/agent/AgentBrain.cpp',
                'src/motor/MotorSystem.cpp'
            ],
            'HIGH'
        )
        
        self.add_improvement(
            'Critical Integration',
            'Implement concept formation processing in brain loop',
            [
                'src/brain/Brain.cpp',
                'src/cognition/ConceptFormation.cpp',
                'src/memory/NeuralWorkingMemory.cpp',
                'src/memory/NeuralEpisodicMemory.cpp'
            ],
            'HIGH'
        )
        
        self.add_improvement(
            'Critical Integration',
            'Fix working memory integration with sensory processing',
            [
                'src/brain/Brain.cpp',
                'src/brain/Brain.hpp',
                'src/agent/AgentBrain.cpp',
                'src/sensory/Vision.cpp'
            ],
            'HIGH'
        )
        
        self.add_improvement(
            'Critical Integration',
            'Fix episodic memory integration with experience logging',
            [
                'src/brain/Brain.cpp',
                'src/memory/NeuralEpisodicMemory.cpp',
                'src/experiments/Experiment.cpp',
                'src/agent/AgentBrain.cpp'
            ],
            'HIGH'
        )
        
        # Phase 2: ADVANCED FEATURES
        self.add_improvement(
            'Configuration System',
            'Upgrade configuration system from simple key=value to JSON/YAML support',
            [
                'src/core/Config/Config.cpp',
                'src/core/Config/Config.hpp',
                'configs/'
            ],
            'MEDIUM'
        )
        
        self.add_improvement(
            'Performance',
            'Add performance monitoring and profiling tools',
            [
                'src/performance/',
                'tools/performance_monitor.cpp',
                'docs/PERFORMANCE.md'
            ],
            'MEDIUM'
        )
        
        self.add_improvement(
            'Developer Tools',
            'Add command-line interface for configuration management',
            [
                'src/cli/config_tool.cpp',
                'docs/HOW_TO_USE.md',
                'tools/'
            ],
            'MEDIUM'
        )
        
        # Phase 3: EXTENSIBILITY
        self.add_improvement(
            'Plugin Architecture',
            'Implement plugin system for custom cognitive modules',
            [
                'src/plugin/',
                'docs/PLUGIN_DESIGN.md',
                'examples/plugin/'
            ],
            'LOW'
        )
        
        self.add_improvement(
            'Testing Framework',
            'Add comprehensive integration testing framework',
            [
                'tests/integration/',
                'tests/unit/',
                'docs/TESTING.md'
            ],
            'LOW'
        )
        
        return {
            'total_improvements': len(self.improvements),
            'by_priority': {
                'HIGH': [i for i in self.improvements if i['priority'] == 'HIGH'],
                'MEDIUM': [i for i in self.improvements if i['priority'] == 'MEDIUM'],
                'LOW': [i for i in self.improvements if i['priority'] == 'LOW']
            },
            'improvements': self.improvements
        }
    
    def generate_code_templates(self) -> Dict:
        """Generate code templates for key improvements"""
        templates = {}
        
        # Template for prediction system integration
        templates['prediction_system_integration'] = '''
// In Brain.cpp - Step 8: Update prediction system
if (pImpl->predictionSystem) {
    pImpl->predictionSystem->updatePredictions(currentSensoryInput, brainState);
    pImpl->predictionSystem->train(currentSensoryInput);
    float predictionError = pImpl->predictionSystem->getPredictionError();
    
    // Apply prediction error to neuromodulation
    if (pImpl->predictionErrorSignal) {
        pImpl->predictionErrorSignal->computeError(
            pImpl->predictionSystem->getConfidence(),
            predictionError
        );
    }
}
'''
        
        # Template for neural planner integration
        templates['neural_planner_integration'] = '''
// In Brain.cpp - Constructor
pImpl->planner = std::make_unique<NeuralPlanner>();
pImpl->planner->initialize(this);

// In Brain.cpp - Modify produceAction() to use planner
std::unique_ptr<class Action> Brain::produceAction() {
    // Try neural planner first
    if (pImpl->planner && !pImpl->motorNeurons.empty()) {
        std::vector<float> currentState;
        // Extract current neural state
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    currentState.push_back(neuron->getMembranePotential());
                }
            }
        }
        
        ActionType plannedAction = pImpl->planner->planAction(
            currentState, 
            pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f
        );
        
        if (plannedAction != ActionType::Wait) {
            return std::make_unique<Action>(plannedAction);
        }
    }
    
    // Fallback to simple motor selection
    return produceSimpleAction();
}
'''
        
        # Template for concept formation
        templates['concept_formation_integration'] = '''
// In Brain.cpp - Step 10: Update concept formation
if (pImpl->conceptFormation && !currentState.empty()) {
    // Process current neural activity patterns to form concepts
    auto concepts = pImpl->conceptFormation->formConcepts(currentState);
    
    // Apply concepts to attention system
    if (pImpl->attention) {
        pImpl->attention->applyTopDownBias(concepts);
    }
    
    // Store concepts in semantic memory
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->storeConcepts(concepts);
    }
}
'''
        
        return templates
    
    def generate_build_script(self) -> str:
        """Generate build script for testing improvements"""
        return '''
#!/bin/bash
# NLM Phase 6 Integration Build Script

set -e

echo "🔨 Building NLM with Phase 6 integration improvements..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=ON \
    -DENABLE_PERFORMANCE_PROFILING=ON \
    -DENABLE_PYTHON_BINDINGS=ON

# Build with optimization
make -j$(nproc)

# Run tests if built
if [ -f "CTestTestfile.cmake" ]; then
    echo "🧪 Running tests..."
    ctest --output-on-failure
else
    echo "⚠️  No tests configured"
fi

# Run Phase 2 demo
echo "🚀 Running Phase 2 demo..."./nlm_phase2_demo

# Generate integration report
echo "📊 Generating integration report..."
cd ..
python3 analyze_nlm.py . --output analysis_results_improved.json

# Create improvement summary
echo "✅ Build and integration test complete!"
''', 'executable': True}
    
    def save_implementation_plan(self, output_path: str):
        """Save implementation plan to file"""
        plan = self.generate_implementation_plan()
        templates = self.generate_code_templates()
        build_script = self.generate_build_script()
        
        data = {
            'implementation_plan': plan,
            'code_templates': templates,
            'build_script': build_script,
            'summary': {
                'total_improvements': plan['total_improvements'],
                'high_priority_count': len(plan['by_priority']['HIGH']),
                'medium_priority_count': len(plan['by_priority']['MEDIUM']),
                'low_priority_count': len(plan['by_priority']['LOW']),
                'estimated_timeline': '6-8 weeks for complete Phase 1-3 implementation'
            }
        }
        
        with open(output_path, 'w') as f:
            json.dump(data, f, indent=2)
        
        return data

def main():
    workspace_dir = sys.argv[1] if len(sys.argv) > 1 else "."
    
    script = NLMIntegrationScript(workspace_dir)
    
    # Generate implementation plan
    print("📋 Generating NLM Phase 6 integration implementation plan...")
    plan = script.generate_implementation_plan()
    
    # Save implementation plan
    output_path = "nlm_integration_implementation.json"
    data = script.save_implementation_plan(output_path)
    
    print(f"✅ Implementation plan saved to {output_path}")
    print(f"📊 Total improvements: {plan['total_improvements']}")
    print(f"🔥 High priority: {plan['by_priority']['HIGH']}")
    print(f"⚡ Medium priority: {plan['by_priority']['MEDIUM']}")
    print(f"🔋 Low priority: {plan['by_priority']['LOW']}")
    
    # Print key improvements
    print("\n🎯 Key Integration Improvements:")
    for improvement in plan['improvements'][:5]:
        print(f"  {improvement['priority']}: {improvement['description']}")
    
    if len(plan['improvements']) > 5:
        print(f"  ... and {len(plan['improvements']) - 5} more improvements")
    
    print("\n📝 Next Steps:")
    print("  1. Review implementation plan")
    print("  2. Prioritize Phase 1 critical fixes")
    print("  3. Start with prediction system integration")
    print("  4. Follow build script for testing")
    print("  5. Update Phase 6 Final Audit after improvements")

if __name__ == "__main__":
    main()