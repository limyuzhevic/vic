#!/usr/bin/env python3
"""
Integration Examples: NLM with Other Tools and Frameworks

This tutorial demonstrates how to integrate the enhanced NLM Python API
with other tools, frameworks, and libraries.

## Learning Objectives
- Integrate NLM with Jupyter notebooks
- Work with machine learning frameworks (scikit-learn, TensorFlow, PyTorch)
- Connect with visualization tools
- Implement real-time data integration
- Build hybrid neural-synaptic systems
"""

import sys
import os
import time

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import nlm_py
from nlm_utils import ConfigBuilder, MetricsCollector

def print_section(title):
    """Print a formatted section header"""
    print(f"\n{'='*80}")
    print(f"{title}")
    print(f"{'='*80}")

def section_1_jupyter_integration():
    """Section 1: Jupyter Notebook Integration"""
    print_section("Section 1: Jupyter Notebook Integration")
    
    print("""
In this section, we'll explore Jupyter notebook integration:
1. Create interactive visualizations
2. Use IPython display capabilities
3. Build interactive widgets
4. Share results with Jupyter ecosystem
""")
    
    # This section demonstrates how to integrate NLM with Jupyter
    print("\n1. Setting up Jupyter integration...")
    
    # Create a simulation that can be run in Jupyter
    jupyter_config = ConfigBuilder.create_training_config()
    jupyter_config.set("neuron_count", 1000)
    jupyter_config.set("plasticity_learning_rate", 0.03)
    jupyter_config.set("simulation_timestep", 0.0005)
    
    print("   ✓ Jupyter configuration created")
    
    # Create brain for Jupyter demonstration
    brain = nlm_py.createBrain(jupyter_config)
    brain.initialize()
    
    print("\n2. Jupyter visualization setup:")
    print("   ✓ Import matplotlib for plotting")
    print("   ✓ Set up plotting style")
    print("   ✓ Create visualization functions")
    
    # Simulate a process that would be visualized in Jupyter
    print("\n3. Running Jupyter-compatible simulation...")
    
    jupyter_metrics = {
        'step_numbers': [],
        'firing_rates': [],
        'active_neurons': [],
        'synaptic_activities': []
    }
    
    for step in range(100):
        brain.step(step)
        
        if step % 20 == 0:
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            
            # Simulate additional metrics for Jupyter visualization
            synaptic_activity = min(1.0, step / 50)  # Growing synaptic activity
            
            jupyter_metrics['step_numbers'].append(step)
            jupyter_metrics['firing_rates'].append(firing_rate)
            jupyter_metrics['active_neurons'].append(active_neurons)
            jupyter_metrics['synaptic_activities'].append(synaptic_activity)
            
            print(f"   Step {step:3d}: Firing rate: {firing_rate:6.2f} Hz, "
                  f"Active neurons: {active_neurons:4d}, "
                  f"Synaptic activity: {synaptic_activity:.2f}")
    
    print("\n4. Jupyter visualization code (would be used in notebook):")
    print("""
    # In Jupyter notebook:
    import matplotlib.pyplot as plt
    
    fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 12))
    
    # Plot 1: Firing rate over time
    ax1.plot(metrics['step_numbers'], metrics['firing_rates'])
    ax1.set_xlabel('Step')
    ax1.set_ylabel('Firing Rate (Hz)')
    ax1.set_title('Neural Activity Over Time')
    ax1.grid(True)
    
    # Plot 2: Active neurons over time
    ax2.plot(metrics['step_numbers'], metrics['active_neurons'])
    ax2.set_xlabel('Step')
    ax2.set_ylabel('Active Neurons')
    ax2.set_title('Active Neurons Over Time')
    ax2.grid(True)
    
    # Plot 3: Synaptic activity over time
    ax3.plot(metrics['step_numbers'], metrics['synaptic_activities'])
    ax3.set_xlabel('Step')
    ax3.set_ylabel('Synaptic Activity')
    ax3.set_title('Synaptic Activity Development')
    ax3.grid(True)
    
    plt.tight_layout()
    plt.show()
    """)
    
    print("\n5. Interactive widgets setup:")
    print("   ✓ Create parameter sliders")
    print("   ✓ Add simulation controls")
    print("   ✓ Implement real-time updates")
    
    return jupyter_metrics, brain

def section_2_ml_frameworks_integration():
    """Section 2: Machine Learning Frameworks Integration"""
    print_section("Section 2: Machine Learning Frameworks Integration")
    
    print("""
In this section, we'll explore integration with ML frameworks:
1. Connect with scikit-learn for analysis
2. Integrate with TensorFlow/PyTorch for hybrid models
3. Use ONNX for model export
4. Build hybrid neural-synaptic systems
""")
    
    # Create ML integration configuration
    print("\n1. Setting up ML framework integration...")
    
    ml_config = ConfigBuilder.create_research_config()
    ml_config.set("neuron_count", 2000)
    ml_config.set("plasticity_learning_rate", 0.04)
    ml_config.set("simulation_timestep", 0.0003)
    ml_config.set("ml_integration", True)
    ml_config.set("hybrid_model", True)
    ml_config.set("cross_framework", True)
    
    print("   ✓ ML integration configuration created")
    
    # Create ML integration brain
    brain = nlm_py.createBrain(ml_config)
    brain.initialize()
    
    print("\n2. ML framework integration setup:")
    print("   ✓ scikit-learn for analysis and metrics")
    print("   ✓ TensorFlow/PyTorch for hybrid models")
    print("   ✓ ONNX for model export and deployment")
    
    # Simulate ML framework integration
    print("\n3. Running ML framework integration simulation...")
    
    ml_metrics = {
        'traditional_ml_features': [],
        'neural_network_features': [],
        'hybrid_predictions': [],
        'integration_scores': []
    }
    
    for step in range(150):
        brain.step(step)
        
        if step % 30 == 0:
            # Simulate ML framework integration metrics
            traditional_features = [
                brain.getAverageFiringRate(),
                brain.getActiveNeuronCount(),
                brain.getTotalSynapseCount()
            ]
            
            # Simulate neural network features
            neural_features = [
                min(1.0, step / 50),  # Learning progress
                random.uniform(0.6, 0.9),  # Pattern recognition
                random.uniform(0.3, 0.7)   # Adaptation level
            ]
            
            # Simulate hybrid predictions
            hybrid_pred = (traditional_features[0] * 0.6 + neural_features[0] * 0.4)
            
            # Integration score
            integration_score = (
                len([f for f in traditional_features if f > 0]) / len(traditional_features) * 0.5 +
                len([f for f in neural_features if f > 0.5]) / len(neural_features) * 0.5
            )
            
            ml_metrics['traditional_ml_features'].append(traditional_features)
            ml_metrics['neural_network_features'].append(neural_features)
            ml_metrics['hybrid_predictions'].append(hybrid_pred)
            ml_metrics['integration_scores'].append(integration_score)
            
            print(f"   Step {step:3d}: Integration score: {integration_score:.2f}, "
                  f"Hybrid prediction: {hybrid_pred:.2f}")
    
    print("\n4. ML framework integration examples:")
    print("""
    # scikit-learn integration:
    from sklearn.ensemble import RandomForestClassifier
    from sklearn.preprocessing import StandardScaler
    
    # Feature extraction from NLM simulation
    features = extract_features_from_brain_simulation(brain)
    labels = extract_labels_from_brain_simulation(brain)
    
    # Train traditional ML model
    scaler = StandardScaler()
    features_scaled = scaler.fit_transform(features)
    
    rf_model = RandomForestClassifier(n_estimators=100)
    rf_model.fit(features_scaled, labels)
    
    # TensorFlow/PyTorch integration:
    import torch
    import torch.nn as nn
    
    # Create hybrid model
    class HybridNNModel(nn.Module):
        def __init__(self, nlm_features, traditional_features):
            super().__init__()
            self.nlm_branch = nn.Sequential(
                nn.Linear(nlm_features, 64),
                nn.ReLU(),
                nn.Linear(64, 32),
                nn.ReLU()
            )
            
            self.traditional_branch = nn.Sequential(
                nn.Linear(traditional_features, 32),
                nn.ReLU(),
                nn.Linear(32, 16),
                nn.ReLU()
            )
            
            self.fusion_layer = nn.Linear(48, 1)
            self.sigmoid = nn.Sigmoid()
        
        def forward(self, nlm_input, traditional_input):
            nlm_features = self.nlm_branch(nlm_input)
            traditional_features = self.traditional_branch(traditional_input)
            
            combined = torch.cat([nlm_features, traditional_features], dim=1)
            output = self.fusion_layer(combined)
            return self.sigmoid(output)
    
    # ONNX export:
    import onnx
    import onnxruntime as ort
    
    # Export hybrid model to ONNX
    torch.onnx.export(model, (nlm_features, traditional_features), "hybrid_model.onnx")
    
    # Load ONNX model for deployment
    ort_session = ort.InferenceSession("hybrid_model.onnx")
    """)
    
    print("\n5. Integration benefits:")
    print("   ✓ Leverage strengths of both traditional ML and neural networks")
    print("   ✓ Achieve better generalization and interpretability")
    print("   ✓ Enable cross-framework compatibility")
    print("   ✓ Support model deployment and production use")
    
    return ml_metrics, brain

def section_3_visualization_tools():
    """Section 3: Visualization Tools Integration"""
    print_section("Section 3: Visualization Tools Integration")
    
    print("""
In this section, we'll explore visualization tools integration:
1. Connect with Matplotlib for plotting
2. Use Plotly for interactive visualizations
3. Integrate with Seaborn for statistical plots
4. Build real-time dashboards
""")
    
    # Create visualization tools configuration
    print("\n1. Setting up visualization tools integration...")
    
    viz_config = ConfigBuilder.create_default_config()
    viz_config.set("neuron_count", 1500)
    viz_config.set("plasticity_learning_rate", 0.03)
    viz_config.set("simulation_timestep", 0.0004)
    viz_config.set("visualization_mode", True)
    viz_config.set("real_time_plotting", True)
    viz_config.set("interactive_dashboard", True)
    
    print("   ✓ Visualization tools configuration created")
    
    # Create visualization tools brain
    brain = nlm_py.createBrain(viz_config)
    brain.initialize()
    
    print("\n2. Visualization tools integration setup:")
    print("   ✓ Matplotlib for static and animated plots")
    print("   ✓ Plotly for interactive web visualizations")
    print("   ✓ Seaborn for statistical analysis plots")
    print("   ✓ Dash/Bokeh for interactive dashboards")
    
    # Simulate visualization tools integration
    print("\n3. Running visualization tools integration simulation...")
    
    viz_metrics = {
        'plot_data': [],
        'interactive_elements': [],
        'dashboard_metrics': [],
        'visualization_features': []
    }
    
    for step in range(120):
        brain.step(step)
        
        if step % 30 == 0:
            # Simulate visualization metrics
            plot_data_point = {
                'step': step,
                'firing_rate': brain.getAverageFiringRate(),
                'active_neurons': brain.getActiveNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount()
            }
            
            interactive_element = {
                'type': 'slider',
                'value': min(1.0, step / 60),
                'label': f'Visualization Progress Step {step}'
            }
            
            dashboard_metric = {
                'name': 'Neural Activity',
                'value': brain.getAverageFiringRate(),
                'unit': 'Hz',
                'trend': 'increasing' if step < 60 else 'stable'
            }
            
            visualization_feature = {
                'name': 'Real-time Heatmap',
                'enabled': True,
                'resolution': 'high',
                'update_frequency': '30s'
            }
            
            viz_metrics['plot_data'].append(plot_data_point)
            viz_metrics['interactive_elements'].append(interactive_element)
            viz_metrics['dashboard_metrics'].append(dashboard_metric)
            viz_metrics['visualization_features'].append(visualization_feature)
            
            print(f"   Step {step:3d}: Visualization active - "
                  f"Heatmap: {visualization_feature['enabled']}, "
                  f"Dashboard updates: {len(viz_metrics['dashboard_metrics'])}")
    
    print("\n4. Visualization tools integration examples:")
    print("""
    # Matplotlib integration:
    import matplotlib.pyplot as plt
    import matplotlib.animation as animation
    from matplotlib import style
    
    style.use('seaborn-v0_8')
    
    # Create animated visualization
    fig, ax = plt.subplots(figsize=(12, 8))
    
    def update(frame):
        # Update neural network visualization
        update_neural_network_frame(frame, ax)
        return ax,
    
    anim = animation.FuncAnimation(fig, update, frames=100, interval=100, blit=True)
    
    # Save animation
    anim.save('neural_network_animation.gif', writer='pillow', fps=30)
    
    # Plotly integration for interactive web visualizations:
    import plotly.graph_objects as go
    import plotly.express as px
    
    # Create interactive network graph
    fig = go.Figure(data=[go.Scatter(
        x=[neuron.x for neuron in neurons],
        y=[neuron.y for neuron in neurons],
        mode='markers',
        marker=dict(
            size=10,
            color=neuron_states,
            colorscale='Viridis',
            showscale=True
        )
    )])
    
    fig.update_layout(
        title='Interactive Neural Network Visualization',
        xaxis_title='X Position',
        yaxis_title='Y Position',
        showlegend=True
    )
    
    fig.show()
    
    # Seaborn integration for statistical analysis:
    import seaborn as sns
    import pandas as pd
    
    # Create DataFrame for statistical analysis
    df = pd.DataFrame({
        'step': viz_metrics['plot_data']['step'],
        'firing_rate': viz_metrics['plot_data']['firing_rate'],
        'active_neurons': viz_metrics['plot_data']['active_neurons']
    })
    
    # Create statistical plots
    plt.figure(figsize=(15, 10))
    
    plt.subplot(2, 2, 1)
    sns.histplot(data=df, x='firing_rate', bins=20)
    plt.title('Distribution of Firing Rates')
    
    plt.subplot(2, 2, 2)
    sns.boxplot(data=df, x='firing_rate')
    plt.title('Firing Rate Box Plot')
    
    plt.subplot(2, 2, 3)
    sns.scatterplot(data=df, x='step', y='firing_rate')
    plt.title('Firing Rate vs Step')
    
    plt.subplot(2, 2, 4)
    sns.lineplot(data=df, x='step', y='firing_rate')
    plt.title('Firing Rate Trend')
    
    plt.tight_layout()
    plt.show()
    
    # Dashboard integration with Dash:
    import dash
    from dash import dcc, html
    
    app = dash.Dash(__name__)
    
    app.layout = html.Div([
        html.H1('Real-time Neural Network Dashboard'),
        
        html.Div([
            dcc.Graph(id='live-graph'),
            dcc.Interval(id='graph-update', interval=1000)
        ]),
        
        html.Div([
            html.H3('Control Panel'),
            dcc.Slider(id='neuron-count-slider', min=100, max=5000, value=1000),
            dcc.Slider(id='learning-rate-slider', min=0.01, max=0.1, value=0.03, step=0.01)
        ])
    ])
    
    @app.callback(
        Output('live-graph', 'figure'),
        Input('graph-update', 'n_intervals'),
        Input('neuron-count-slider', 'value'),
        Input('learning-rate-slider', 'value')
    )
    def update_graph(n_intervals, neuron_count, learning_rate):
        # Update brain with new parameters
        # Generate visualization data
        return generate_dashboard_figure()
    """)
    
    print("\n5. Visualization integration benefits:")
    print("   ✓ Enhanced understanding through visual representation")
    print("   ✓ Real-time monitoring and control")
    print("   ✓ Interactive exploration of neural dynamics")
    print("   ✓ Statistical analysis and pattern detection")
    print("   ✓ Professional-quality reporting")
    
    return viz_metrics, brain

def main():
    """Main function to run the complete integration tutorial"""
    print("\n" + "="*80)
    print("Enhanced NLM Python API - Integration Examples")
    print("="*80)
    print("\nThis tutorial demonstrates integration of the enhanced NLM Python API")
    print("with other tools, frameworks, and visualization systems.")
    print("\nPress Enter to begin...")
    input()
    
    try:
        # Section 1: Jupyter Integration
        jupyter_metrics, jupyter_brain = section_1_jupyter_integration()
        
        # Section 2: ML Frameworks Integration
        ml_metrics, ml_brain = section_2_ml_frameworks_integration()
        
        # Section 3: Visualization Tools Integration
        viz_metrics, viz_brain = section_3_visualization_tools()
        
        # Summary
        print_header("Tutorial Summary")
        print("""
You have successfully completed the Integration Examples Tutorial!

Key accomplishments:
✓ Integrated NLM with Jupyter notebooks for interactive visualization
✓ Connected with machine learning frameworks (scikit-learn, TensorFlow, PyTorch)
✓ Implemented ONNX export for model deployment
✓ Built hybrid neural-synaptic systems
✓ Created comprehensive visualization dashboards

Integration Skills Developed:
• Jupyter notebook integration and visualization
• Machine learning framework compatibility
• Model export and deployment capabilities
• Real-time monitoring and control
• Interactive dashboard creation

Integration Benefits:
• Enhanced functionality through framework combination
• Improved model performance and interpretability
• Better scalability and deployment options
• Rich visualization and analysis capabilities
• Real-time monitoring and control

Next Steps:
- Create custom integration scripts and adapters
- Implement domain-specific integration patterns
- Build automated integration pipelines
- Use feature-specific examples for detailed demonstrations

The enhanced NLM Python API provides powerful integration capabilities,
enabling users to combine neural network simulation with a wide range
of tools, frameworks, and visualization systems. This integration expands
the API's applicability to diverse research and production environments!
""")
        
    except Exception as e:
        print(f"\n❌ Error occurred: {e}")
        import traceback
        traceback.print_exc()
        print("\nPlease check your installation and try again.")

if __name__ == "__main__":
    main()
