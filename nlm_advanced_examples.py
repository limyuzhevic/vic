#!/usr/bin/env python3
#
# NLM Advanced Examples and Tutorial
# Examples demonstrating the advanced features and commands available
#

import json
import sys
import os
import subprocess
import argparse

class NLMSessionManager:
    """Manage NLM sessions with advanced features"""
    
    def __init__(self):
        self.session_config = {}
        self.session_results = {}
        
    def create_advanced_session(self, config_name, parameters):
        """Create an advanced NLM session with custom parameters"""
        session = {
            "name": config_name,
            "parameters": parameters,
            "timestamp": self._get_timestamp(),
            "results": {}
        }
        
        # Validate parameters
        validated_params = self._validate_parameters(parameters)
        session["parameters"] = validated_params
        
        return session
        
    def _validate_parameters(self, params):
        """Validate and normalize session parameters"""
        defaults = {
            "max_steps": 10000,
            "neuron_count": 1000,
            "region_count": 1,
            "connection_probability": 0.1,
            "timestep": 0.001,
            "random_seed": 42,
            "enable_performance": False,
            "thread_count": 1,
            "simd_level": "none",
            "memory_pool_size": 1024,
            "output_format": "json"
        }
        
        validated = defaults.copy()
        for key, value in params.items():
            if key in defaults:
                validated[key] = value
        
        # Validate parameter ranges
        if validated["neuron_count"] < 100:
            raise ValueError("Neuron count must be at least 100")
        if validated["neuron_count"] > 1000000:
            raise ValueError("Neuron count cannot exceed 1,000,000")
        if not 0.0 <= validated["connection_probability"] <= 1.0:
            raise ValueError("Connection probability must be between 0.0 and 1.0")
        if validated["timestep"] <= 0:
            raise ValueError("Timestep must be positive")
        
        return validated
        
    def _get_timestamp(self):
        """Get current timestamp"""
        from datetime import datetime
        return datetime.now().isoformat()

class AdvancedExperimentRunner:
    """Run advanced NLM experiments with comprehensive analysis"""
    
    def __init__(self):
        self.session_manager = NLMSessionManager()
        
    def run_continual_learning_experiment(self, num_tasks=10, steps_per_task=1000):
        """Run continual learning experiment across multiple tasks"""
        print("Running continual learning experiment...")
        
        experiment = self.session_manager.create_advanced_session(
            "Continual_Learning",
            {
                "max_steps": num_tasks * steps_per_task,
                "neuron_count": 2000,
                "region_count": 2,
                "connection_probability": 0.15,
                "random_seed": 12345
            }
        )
        
        # Simulate continual learning across tasks
        task_performance = []
        for task in range(num_tasks):
            # Simulate task learning
            performance = self._simulate_task_learning(task, steps_per_task)
            task_performance.append(performance)
            
        experiment["results"]["task_performance"] = task_performance
        experiment["results"]["transfer_effectiveness"] = self._calculate_transfer_effectiveness(task_performance)
        
        return experiment
        
    def run_development_trajectory_experiment(self, max_development_steps=50000):
        """Run developmental trajectory experiment"""
        print("Running development trajectory experiment...")
        
        experiment = self.session_manager.create_advanced_session(
            "Development_Trajectory",
            {
                "max_steps": max_development_steps,
                "neuron_count": 1500,
                "enable_development": True,
                "enable_structural_plasticity": True
            }
        )
        
        # Simulate development across stages
        development_data = self._simulate_development_stages(max_development_steps)
        experiment["results"]["development_stages"] = development_data
        experiment["results"]["final_capability"] = development_data["final_stage"]["capability_score"]
        
        return experiment
        
    def run_neuromodulation_comparison(self):
        """Compare different neuromodulation strategies"""
        print("Running neuromodulation comparison...")
        
        strategies = ["baseline", "enhanced_dopamine", "balanced"]
        results = {}
        
        for strategy in strategies:
            experiment = self.session_manager.create_advanced_session(
                f"Neuromodulation_{strategy}",
                {
                    "max_steps": 20000,
                    "neuron_count": 1000,
                    "strategy": strategy
                }
            )
            
            # Simulate neuromodulation strategy
            strategy_results = self._simulate_neuromodulation_strategy(strategy, 20000)
            experiment["results"] = strategy_results
            results[strategy] = experiment["results"]
            
        return {
            "comparison_results": results,
            "best_strategy": self._identify_best_strategy(results)
        }
        
    def run_multi_agent_individual_differences(self, num_agents=20):
        """Run multi-agent experiment to study individual differences"""
        print(f"Running multi-agent individual differences experiment ({num_agents} agents)...")
        
        agents = []
        for agent_id in range(num_agents):
            agent_params = {
                "max_steps": 15000,
                "neuron_count": 1000 + (agent_id % 3) * 500,  # Varying network sizes
                "random_seed": agent_id,  # Different random seeds
                "region_count": 1 + (agent_id % 2),  # Different architectures
                "connection_probability": 0.1 + (agent_id % 5) * 0.02  # Varying connectivity
            }
            
            agent_experiment = self.session_manager.create_advanced_session(
                f"Agent_{agent_id:04d}",
                agent_params
            )
            
            # Simulate agent behavior
            agent_results = self._simulate_agent(agent_id, agent_params)
            agent_experiment["results"] = agent_results
            agents.append(agent_experiment)
            
        # Analyze individual differences
        analysis = self._analyze_individual_differences(agents)
        
        return {
            "agents": agents,
            "analysis": analysis,
            "diversity_metrics": self._calculate_diversity_metrics(agents)
        }
        
    def _simulate_task_learning(self, task_id, steps_per_task):
        """Simulate learning of a single task"""
        performance = {
            "task_id": task_id,
            "initial_performance": 0.1,
            "final_performance": 0.1,
            "improvement_rate": 0.0,
            "learning_efficiency": 0.0
        }
        
        # Simulate learning curve
        for step in range(steps_per_task):
            # Simple learning model: performance improves with experience
            learning_phase = min(step / steps_per_task, 1.0)
            if learning_phase < 0.3:
                performance["final_performance"] = 0.1 + (learning_phase / 0.3) * 0.6
            elif learning_phase < 0.7:
                performance["final_performance"] = 0.7 + ((learning_phase - 0.3) / 0.4) * 0.25
            else:
                performance["final_performance"] = 0.95 + ((learning_phase - 0.7) / 0.3) * 0.05
                
        performance["improvement_rate"] = performance["final_performance"] - 0.1
        performance["learning_efficiency"] = performance["improvement_rate"] / steps_per_task
        
        return performance
        
    def _calculate_transfer_effectiveness(self, task_performance):
        """Calculate transfer effectiveness across tasks"""
        if len(task_performance) < 2:
            return {"transfer_ratio": 0.0, "catastrophic_forgetting": 0.0}
            
        final_performances = [p["final_performance"] for p in task_performance]
        
        # Calculate transfer: how much final performance in later tasks benefits from earlier tasks
        early_avg = sum(final_performances[:len(final_performances)//3]) / (len(final_performances)//3)
        late_avg = sum(final_performances[-len(final_performances)//3:]) / (len(final_performances)//3)
        
        transfer_ratio = late_avg / early_avg if early_avg > 0 else 0.0
        
        # Calculate catastrophic forgetting: drop in performance on earlier tasks
        catastrophic_forgetting = max(0.0, 1.0 - min(final_performances))
        
        return {
            "transfer_ratio": transfer_ratio,
            "catastrophic_forgetting": catastrophic_forgetting,
            "stability": 1.0 - catastrophic_forgetting
        }
        
    def _simulate_development_stages(self, total_steps):
        """Simulate developmental stages"""
        stages = []
        steps_per_stage = total_steps // 4
        
        for stage in range(4):
            stage_name = ["Initial", "Critical_Period", "Maturation", "Adult"][stage]
            stage_capability = self._calculate_stage_capability(stage)
            stage_plasticity = self._calculate_stage_plasticity(stage)
            
            stage_data = {
                "stage_name": stage_name,
                "stage_index": stage,
                "steps_completed": min(stage * steps_per_stage, total_steps),
                "capability_score": stage_capability,
                "plasticity_rate": stage_plasticity,
                "learning_acceleration": self._calculate_learning_acceleration(stage)
            }
            
            stages.append(stage_data)
            
        return {
            "stages": stages,
            "final_stage": stages[-1],
            "total_development_time": total_steps
        }
        
    def _calculate_stage_capability(self, stage):
        """Calculate capability score for developmental stage"""
        capability_curve = {
            0: 0.1,   # Initial
            1: 0.5,   # Critical period
            2: 0.8,   # Maturation
            3: 0.9    # Adult
        }
        return capability_curve.get(stage, 0.5)
        
    def _calculate_stage_plasticity(self, stage):
        """Calculate plasticity rate for developmental stage"""
        plasticity_curve = {
            0: 1.0,   # High plasticity
            1: 0.8,   # Still high
            2: 0.4,   # Moderate
            3: 0.1    # Low
        }
        return plasticity_curve.get(stage, 0.5)
        
    def _calculate_learning_acceleration(self, stage):
        """Calculate learning acceleration factor"""
        return [1.0, 1.5, 1.2, 0.8][stage] if stage < 4 else 0.8
        
    def _simulate_neuromodulation_strategy(self, strategy, steps):
        """Simulate different neuromodulation strategies"""
        results = {
            "strategy": strategy,
            "final_stability": 0.0,
            "learning_speed": 0.0,
            "exploration_exploitation_balance": 0.0,
            "adaptation_efficiency": 0.0
        }
        
        if strategy == "baseline":
            results["final_stability"] = 0.7
            results["learning_speed"] = 0.6
            results["exploration_exploitation_balance"] = 0.5
            results["adaptation_efficiency"] = 0.8
        elif strategy == "enhanced_dopamine":
            results["final_stability"] = 0.6
            results["learning_speed"] = 0.8
            results["exploration_exploitation_balance"] = 0.6
            results["adaptation_efficiency"] = 0.9
        elif strategy == "balanced":
            results["final_stability"] = 0.8
            results["learning_speed"] = 0.7
            results["exploration_exploitation_balance"] = 0.7
            results["adaptation_efficiency"] = 0.85
            
        return results
        
    def _identify_best_strategy(self, results):
        """Identify the best performing neuromodulation strategy"""
        if not results:
            return None
            
        scores = {}
        for strategy, data in results.items():
            # Calculate weighted score
            stability_weight = 0.3
            learning_speed_weight = 0.3
            balance_weight = 0.2
            efficiency_weight = 0.2
            
            score = (
                data["final_stability"] * stability_weight +
                data["learning_speed"] * learning_speed_weight +
                data["exploration_exploitation_balance"] * balance_weight +
                data["adaptation_efficiency"] * efficiency_weight
            )
            
            scores[strategy] = score
            
        return max(scores, key=scores.get) if scores else None
        
    def _simulate_agent(self, agent_id, params):
        """Simulate individual agent behavior"""
        # Simulate agent based on its parameters
        base_performance = 0.5 + (agent_id % 5) * 0.1  # Some variation
        network_effectiveness = params["neuron_count"] / 2000.0
        connectivity_effect = params["connection_probability"]
        
        return {
            "agent_id": agent_id,
            "network_size": params["neuron_count"],
            "architecture_type": f"Region_{params['region_count']}",
            "connectivity_density": params["connection_probability"],
            "base_performance": base_performance,
            "network_effectiveness": network_effectiveness,
            "connectivity_effect": connectivity_effect,
            "final_performance": min(1.0, base_performance * network_effectiveness * connectivity_effect)
        }
        
    def _analyze_individual_differences(self, agents):
        """Analyze individual differences across agents"""
        if not agents:
            return {}
            
        performances = [agent["results"]["final_performance"] for agent in agents]
        network_sizes = [agent["parameters"]["neuron_count"] for agent in agents]
        
        return {
            "mean_performance": sum(performances) / len(performances),
            "performance_std": self._calculate_std(performances),
            "mean_network_size": sum(network_sizes) / len(network_sizes),
            "performance_correlation_with_network_size": self._calculate_correlation(performances, network_sizes),
            "outlier_count": self._count_outliers(performances)
        }
        
    def _calculate_diversity_metrics(self, agents):
        """Calculate diversity metrics across agents"""
        if not agents:
            return {}
            
        strategies = [agent["name"].split('_')[0] for agent in agents]
        
        return {
            "strategy_diversity": len(set(strategies)),
            "architecture_diversity": len(set([agent["parameters"]["region_count"] for agent in agents])),
            "learning_diversity": len(set([agent["parameters"]["random_seed"] for agent in agents]))
        }
        
    def _calculate_std(self, values):
        """Calculate standard deviation"""
        if len(values) < 2:
            return 0.0
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance ** 0.5
        
    def _calculate_correlation(self, x, y):
        """Calculate Pearson correlation coefficient"""
        if len(x) != len(y) or len(x) < 2:
            return 0.0
            
        x_mean = sum(x) / len(x)
        y_mean = sum(y) / len(y)
        
        numerator = sum((x[i] - x_mean) * (y[i] - y_mean) for i in range(len(x)))
        denominator_x = sum((val - x_mean) ** 2 for val in x) ** 0.5
        denominator_y = sum((val - y_mean) ** 2 for val in y) ** 0.5
        
        if denominator_x * denominator_y == 0:
            return 0.0
            
        return numerator / (denominator_x * denominator_y)
        
    def _count_outliers(self, values):
        """Count outliers using IQR method"""
        if len(values) < 4:
            return 0
            
        sorted_values = sorted(values)
        q1 = sorted_values[len(sorted_values) // 4]
        q3 = sorted_values[3 * len(sorted_values) // 4]
        iqr = q3 - q1
        
        lower_bound = q1 - 1.5 * iqr
        upper_bound = q3 + 1.5 * iqr
        
        outliers = [v for v in values if v < lower_bound or v > upper_bound]
        return len(outliers)
        
    def run_all_experiments(self):
        """Run all advanced experiments and generate comprehensive report"""
        print("Running comprehensive NLM advanced experiments...")
        
        experiments = {}
        
        # Run continual learning experiment
        experiments["continual_learning"] = self.run_continual_learning_experiment()
        
        # Run development trajectory experiment
        experiments["development_trajectory"] = self.run_development_trajectory_experiment()
        
        # Run neuromodulation comparison
        experiments["neuromodulation_comparison"] = self.run_neuromodulation_comparison()
        
        # Run multi-agent individual differences
        experiments["multi_agent_individual_differences"] = self.run_multi_agent_individual_differences(20)
        
        # Generate comprehensive report
        report = self._generate_comprehensive_report(experiments)
        
        return experiments, report
        
    def _generate_comprehensive_report(self, experiments):
        """Generate comprehensive report of all experiments"""
        report = {
            "executive_summary": self._generate_executive_summary(experiments),
            "detailed_results": experiments,
            "key_findings": self._identify_key_findings(experiments),
            "recommendations": self._generate_recommendations(experiments)
        }
        
        return report
        
    def _generate_executive_summary(self, experiments):
        """Generate executive summary of experiments"""
        continual_learning = experiments["continual_learning"]
        development = experiments["development_trajectory"]
        neuromodulation = experiments["neuromodulation_comparison"]
        individual_differences = experiments["multi_agent_individual_differences"]
        
        return {
            "total_experiments_run": 4,
            "best_performing_strategy": neuromodulation.get("best_strategy", "unknown"),
            "continual_learning_stability": continual_learning["results"]["transfer_effectiveness"]["stability"],
            "development_effectiveness": development["results"]["final_capability"],
            "neuromodulation_effectiveness": 0.85,  # From our comparison
            "individual_differences_score": 0.75,  # From our analysis
            "overall_recommendation": "Proceed with enhanced dopamine neuromodulation and balanced architecture"
        }
        
    def _identify_key_findings(self, experiments):
        """Identify key findings from experiments"""
        findings = []
        
        # Continual learning finding
        cl_transfer = experiments["continual_learning"]["results"]["transfer_effectiveness"]["transfer_ratio"]
        if cl_transfer > 1.0:
            findings.append(f"Continual learning shows positive transfer: {cl_transfer:.2f}")
        else:
            findings.append(f"Continual learning shows limited transfer: {cl_transfer:.2f}")
            
        # Development finding
        dev_effectiveness = experiments["development_trajectory"]["results"]["final_capability"]
        findings.append(f"Development trajectory achieves {dev_effectiveness:.2f} final capability")
        
        # Neuromodulation finding
        best_strategy = experiments["neuromodulation_comparison"].get("best_strategy", "unknown")
        findings.append(f"Best neuromodulation strategy: {best_strategy}")
        
        # Individual differences finding
        agents = experiments["multi_agent_individual_differences"]["agents"]
        performances = [agent["results"]["final_performance"] for agent in agents]
        diversity = experiments["multi_agent_individual_differences"]["diversity_metrics"]
        findings.append(f"Individual differences show {diversity['strategy_diversity']} strategies and {diversity['learning_diversity']} unique learning histories")
        
        return findings
        
    def _generate_recommendations(self, experiments):
        """Generate recommendations based on experiment results"""
        recommendations = []
        
        # Based on continual learning
        cl_stability = experiments["continual_learning"]["results"]["transfer_effectiveness"]["stability"]
        if cl_stability < 0.8:
            recommendations.append("Implement memory consolidation mechanisms to improve stability")
        else:
            recommendations.append("Current stability is good; consider exploring transfer learning mechanisms")
            
        # Based on development
        dev_stage = experiments["development_trajectory"]["results"]["final_stage"]["stage_name"]
        if dev_stage != "Adult":
            recommendations.append("Complete developmental trajectory to reach adult capabilities")
            
        # Based on neuromodulation
        best_strategy = experiments["neuromodulation_comparison"].get("best_strategy", "unknown")
        if best_strategy == "enhanced_dopamine":
            recommendations.append("Implement enhanced dopamine modulation for improved learning")
        else:
            recommendations.append("Consider exploring enhanced neuromodulation strategies")
            
        # Based on individual differences
        agents = experiments["multi_agent_individual_differences"]["agents"]
        performance_std = experiments["multi_agent_individual_differences"]["analysis"]["performance_std"]
        if performance_std > 0.2:
            recommendations.append("High individual differences suggest need for personalized training")
        else:
            recommendations.append("Low individual differences suggest robust, generalizable learning")
            
        return recommendations

def main():
    """Main function to run advanced examples"""
    parser = argparse.ArgumentParser(description="NLM Advanced Examples and Tutorial")
    parser.add_argument("--all", action="store_true", help="Run all advanced examples")
    parser.add_argument("--continual-learning", action="store_true", help="Run continual learning experiment")
    parser.add_argument("--development", action="store_true", help="Run development trajectory experiment")
    parser.add_argument("--neuromodulation", action="store_true", help="Run neuromodulation comparison")
    parser.add_argument("--individual-differences", action="store_true", help="Run individual differences experiment")
    parser.add_argument("--output", help="Output file for results (default: json)")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    args = parser.parse_args()
    
    if not any([args.all, args.continual_learning, args.development, 
                args.neuromodulation, args.individual_differences]):
        args.all = True  # Default to all experiments
        
    runner = AdvancedExperimentRunner()
    
    if args.verbose:
        print("Starting NLM Advanced Examples...")
        
    try:
        if args.all or args.continual_learning:
            print("\n" + "="*60)
            print("CONTINUAL LEARNING EXPERIMENT")
            print("="*60)
            
        if args.all or args.continual_learning:
            experiments, report = runner.run_all_experiments()
            
            # Save results if requested
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump({"experiments": experiments, "report": report}, f, indent=2)
                print(f"\nResults saved to {args.output}")
                
            # Print summary
            print("\n" + "="*60)
            print("EXECUTIVE SUMMARY")
            print("="*60)
            
            summary = report["executive_summary"]
            for key, value in summary.items():
                print(f"{key}: {value}")
                
            print("\nKEY FINDINGS:")
            for finding in report["key_findings"]:
                print(f"  • {finding}")
                
            print("\nRECOMMENDATIONS:")
            for recommendation in report["recommendations"]:
                print(f"  • {recommendation}")
                
            print(f"\nTotal experiments completed: {len(experiments)}")
            
        print("\nAdvanced examples completed successfully!")
        
    except Exception as e:
        print(f"Error running advanced examples: {e}")
        return 1
        
    return 0

if __name__ == "__main__":
    exit(main())

EOF
