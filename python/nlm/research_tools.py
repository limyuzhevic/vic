#!/usr/bin/env python3
"""
Research Tools for NLM Neural Data

This module provides research tools for neural simulation, including statistical testing,
power analysis, meta-analysis, reproducibility tools, and collaboration tools
for research workflows.
"""

import numpy as np
import pandas as pd
import scipy.stats as stats
import scipy.signal as signal
from scipy import optimize
import statsmodels.api as sm
import statsmodels.stats.api as sms
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import cross_val_score, StratifiedKFold
from sklearn.metrics import (accuracy_score, precision_score, recall_score, f1_score,
                           roc_auc_score, confusion_matrix, mean_squared_error, r2_score)
from typing import List, Dict, Tuple, Optional, Any, Union, Callable
from dataclasses import dataclass, field
from enum import Enum
import warnings
import time
import json
import pickle
import os
import sys
import hashlib
from pathlib import Path
from datetime import datetime
from abc import ABC, abstractmethod
import itertools
from collections import defaultdict, Counter
import matplotlib.pyplot as plt
import seaborn as sns

warnings.filterwarnings('ignore')

class ResearchMethod(Enum):
    """Research methods"""
    STATISTICAL_TESTING = "statistical_testing"
    POWER_ANALYSIS = "power_analysis"
    META_ANALYSIS = "meta_analysis"
    REPRODUCIBILITY = "reproducibility"
    COLLABORATION = "collaboration"
    EXPERIMENT_DESIGN = "experiment_design"
    DATA_ANALYSIS = "data_analysis"
    VALIDATION = "validation"

@dataclass
class ExperimentSetup:
    """Setup for neural simulation experiment"""
    experiment_id: str
    name: str
    description: str
    hypotheses: List[str]
    variables: Dict[str, Any]
    sample_size: Optional[int] = None
    power_analysis: Optional[Dict[str, Any]] = None
    randomization_method: str = 'random'
    blinding: bool = False
    controls: List[str]
    interventions: List[Dict[str, Any]]
    outcomes: List[str]
    statistical_tests: List[str]
    data_collection_methods: List[Dict[str, Any]]
    ethical_considerations: Optional[Dict[str, Any]] = None
    funding_source: Optional[str] = None
    start_date: Optional[datetime] = None
    end_date: Optional[datetime] = None
    status: str = "planning"  # planning, active, completed, archived

@dataclass
class StatisticalTest:
    """Statistical test configuration"""
    test_id: str
    test_name: str
    test_type: str
    hypotheses: List[Dict[str, Any]]
    data_requirements: Dict[str, Any]
    assumptions: List[str]
    alpha: float = 0.05
    power: Optional[float] = None
    effect_size: Optional[float] = None
    sample_size: Optional[int] = None
    interpretation: Optional[str] = None
    warnings: List[str] = field(default_factory=list)
    result: Optional[Dict[str, Any]] = None

@dataclass
class PowerAnalysisResult:
    """Result from power analysis"""
    analysis_id: str
    test_type: str
    effect_size: float
    alpha: float
    power: float
    sample_size: int
    required_sample_size: int
    recommended_effect_size: float
    interpretation: str
    confidence_intervals: Optional[Dict[str, Any]] = None
    assumptions_check: Optional[Dict[str, Any]] = None

@dataclass
class MetaAnalysisResult:
    """Result from meta-analysis"""
    analysis_id: str
    study_ids: List[str]
    effect_sizes: np.ndarray
    sample_sizes: np.ndarray
    weights: np.ndarray
    overall_effect_size: float
    overall_variance: float
    heterogeneity_i2: float
    publication_bias: Optional[Dict[str, Any]] = None
    subgroup_analysis: Optional[Dict[str, Any]] = None
    sensitivity_analysis: Optional[Dict[str, Any]] = None

@dataclass
class ReproducibilityReport:
    """Report on reproducibility of neural data analysis"""
    reproducibility_id: str
    dataset_id: str
    analysis_type: str
    original_researcher: str
    replication_attempts: int
    successful_replications: int
    reproducibility_score: float
    methodological_differences: List[Dict[str, Any]]
    result_agreement: float
    issues_identified: List[str]
    recommendations: List[str]
    confidence_interval: Optional[Tuple[float, float]] = None

@dataclass
class CollaborationProject:
    """Collaboration project for neural simulation research"""
    project_id: str
    title: str
    description: str
    principal_investigator: str
    co_investigators: List[str]
    research_institutions: List[str]
    project_duration: Tuple[datetime, datetime]
    objectives: List[str]
    datasets: List[str]
    shared_resources: Dict[str, Any]
    data_shares: Dict[str, float]
    governance_structure: Dict[str, Any]
    publication_plan: Dict[str, Any]
    milestones: List[Dict[str, Any]]
    status: str = "planning"  # planning, active, completed, archived

@dataclass
class ValidationResult:
    """Validation result for neural data"""
    validation_id: str
    dataset_id: str
    validation_type: str
    validation_rules: List[Dict[str, Any]]
    passed_validations: List[Dict[str, Any]]
    failed_validations: List[Dict[str, Any]]
    overall_score: float
    recommendations: List[str]
    timestamp: datetime = field(default_factory=datetime.now)
    validator: Optional[str] = None
class StatisticalTestingFramework:
    """Framework for statistical testing of neural data"""
    
    def __init__(self, alpha: float = 0.05, correction_method: str = 'bonferroni'):
        self.alpha = alpha
        self.correction_method = correction_method
        self.test_history: List[Dict[str, Any]] = []
    
    def independent_t_test(self, group1: np.ndarray, group2: np.ndarray,
                          equal_var: bool = True, paired: bool = False) -> Dict[str, Any]:
        """Perform independent samples t-test"""
        if paired:
            # Paired t-test
            if len(group1) != len(group2):
                raise ValueError("Paired t-test requires equal sample sizes")
            
            t_stat, p_value = stats.ttest_rel(group1, group2)
        else:
            # Independent t-test
            t_stat, p_value = stats.ttest_ind(group1, group2, equal_var=equal_var)
        
        # Calculate effect size
        n1, n2 = len(group1), len(group2)
        pooled_std = np.sqrt(((n1-1)*np.var(group1) + (n2-1)*np.var(group2)) / (n1 + n2 - 2))
        effect_size = (np.mean(group1) - np.mean(group2)) / pooled_std if pooled_std > 0 else 0
        
        # Apply correction
        corrected_p_value = self._apply_correction(p_value)
        
        # Determine significance
        significant = corrected_p_value < self.alpha
        
        # Create test result
        test_result = {
            'test_name': 'independent_t_test',
            'paired': paired,
            'equal_var': equal_var,
            'n1': n1,
            'n2': n2,
            'mean1': np.mean(group1),
            'mean2': np.mean(group2),
            'std1': np.std(group1),
            'std2': np.std(group2),
            't_statistic': t_stat,
            'p_value': p_value,
            'corrected_p_value': corrected_p_value,
            'effect_size': effect_size,
            'significant': significant,
            'interpretation': self._interpret_t_test(t_stat, corrected_p_value, effect_size),
            'assumptions_met': self._check_t_test_assumptions(group1, group2, equal_var, paired)
        }
        
        self.test_history.append(test_result)
        return test_result
    
    def paired_t_test(self, before: np.ndarray, after: np.ndarray) -> Dict[str, Any]:
        """Perform paired samples t-test"""
        return self.independent_t_test(before, after, equal_var=True, paired=True)
    
    def one_sample_t_test(self, sample: np.ndarray, population_mean: float) -> Dict[str, Any]:
        """Perform one-sample t-test"""
        t_stat, p_value = stats.ttest_1samp(sample, population_mean)
        
        # Calculate effect size
        n = len(sample)
        population_std = np.std(sample)
        effect_size = (np.mean(sample) - population_mean) / population_std if population_std > 0 else 0
        
        # Apply correction
        corrected_p_value = self._apply_correction(p_value)
        
        # Determine significance
        significant = corrected_p_value < self.alpha
        
        test_result = {
            'test_name': 'one_sample_t_test',
            'sample_size': n,
            'sample_mean': np.mean(sample),
            'sample_std': np.std(sample),
            'population_mean': population_mean,
            't_statistic': t_stat,
            'p_value': p_value,
            'corrected_p_value': corrected_p_value,
            'effect_size': effect_size,
            'significant': significant,
            'interpretation': self._interpret_one_sample_t_test(t_stat, corrected_p_value, effect_size),
            'assumptions_met': self._check_one_sample_t_test_assumptions(sample)
        }
        
        self.test_history.append(test_result)
        return test_result
    
    def anova_test(self, groups: List[np.ndarray], 
                  equal_var: bool = True) -> Dict[str, Any]:
        """Perform one-way ANOVA test"""
        try:
            # Perform ANOVA
            f_stat, p_value = stats.f_oneway(*groups)
            
            # Calculate effect size (eta squared)
            ss_between = sum([len(g) * (np.mean(g) - np.mean(np.concatenate(groups))) ** 2 
                            for g in groups])
            ss_total = sum([(x - np.mean(np.concatenate(groups))) ** 2 for x in np.concatenate(groups)])
            eta_squared = ss_between / ss_total if ss_total > 0 else 0
            
            # Apply correction
            corrected_p_value = self._apply_correction(p_value)
            
            # Determine significance
            significant = corrected_p_value < self.alpha
            
            # Post-hoc tests if significant and more than 2 groups
            posthoc_tests = []
            if significant and len(groups) > 2:
                # Tukey's HSD test
                from statsmodels.stats.multicomp import pairwise_tukeyhsd
                
                # Prepare data for Tukey's test
                all_data = []
                group_labels = []
                for i, group in enumerate(groups):
                    all_data.extend(group)
                    group_labels.extend([f'group_{i}'] * len(group))
                
                tukey_result = pairwise_tukeyhsd(all_data, group_labels, alpha=self.alpha)
                posthoc_tests.append({
                    'test_name': 'tukey_hsd',
                    'result': tukey_result.summary()
                })
            
            test_result = {
                'test_name': 'anova',
                'k_groups': len(groups),
                'total_n': sum([len(g) for g in groups]),
                'grand_mean': np.mean(np.concatenate(groups)),
                'between_ss': ss_between,
                'within_ss': ss_total - ss_between,
                'total_ss': ss_total,
                'f_statistic': f_stat,
                'p_value': p_value,
                'corrected_p_value': corrected_p_value,
                'eta_squared': eta_squared,
                'significant': significant,
                'interpretation': self._interpret_anova(f_stat, corrected_p_value, eta_squared),
                'posthoc_tests': posthoc_tests,
                'assumptions_met': self._check_anova_assumptions(groups, equal_var)
            }
            
            self.test_history.append(test_result)
            return test_result
            
        except Exception as e:
            return {'error': str(e)}
    
    def chi_square_test(self, observed: np.ndarray, expected: Optional[np.ndarray] = None) -> Dict[str, Any]:
        """Perform chi-square goodness-of-fit test"""
        if expected is None:
            # Equal expected frequencies
            expected = np.full(len(observed), np.mean(observed))
        
        # Perform chi-square test
        chi2_stat, p_value = stats.chisquare(observed, expected)
        
        # Apply correction
        corrected_p_value = self._apply_correction(p_value)
        
        # Determine significance
        significant = corrected_p_value < self.alpha
        
        test_result = {
            'test_name': 'chi_square_goodness_fit',
            'observed': observed.tolist(),
            'expected': expected.tolist(),
            'degrees_of_freedom': len(observed) - 1,
            'chi2_statistic': chi2_stat,
            'p_value': p_value,
            'corrected_p_value': corrected_p_value,
            'significant': significant,
            'interpretation': self._interpret_chi_square(chi2_stat, corrected_p_value),
            'assumptions_met': self._check_chi_square_assumptions(observed, expected)
        }
        
        self.test_history.append(test_result)
        return test_result
    
    def mann_whitney_u_test(self, group1: np.ndarray, group2: np.ndarray) -> Dict[str, Any]:
        """Perform Mann-Whitney U test (non-parametric alternative to t-test)"""
        u_stat, p_value = stats.mannwhitneyu(group1, group2, alternative='two-sided')
        
        # Calculate effect size (r)
        n1, n2 = len(group1), len(group2)
        r = 1 - (4 * u_stat) / (n1 * n2)  # Small sample correction
        
        # Apply correction
        corrected_p_value = self._apply_correction(p_value)
        
        # Determine significance
        significant = corrected_p_value < self.alpha
        
        test_result = {
            'test_name': 'mann_whitney_u',
            'n1': n1,
            'n2': n2,
            'median1': np.median(group1),
            'median2': np.median(group2),
            'u_statistic': u_stat,
            'p_value': p_value,
            'corrected_p_value': corrected_p_value,
            'effect_size_r': r,
            'significant': significant,
            'interpretation': self._interpret_mann_whitney(u_stat, corrected_p_value, r),
            'assumptions_met': self._check_mann_whitney_assumptions(group1, group2)
        }
        
        self.test_history.append(test_result)
        return test_result
    
    def wilcoxon_signed_rank_test(self, before: np.ndarray, after: np.ndarray) -> Dict[str, Any]:
        """Perform Wilcoxon signed-rank test (non-parametric paired test)"""
        w_stat, p_value = stats.wilcoxon(before, after, alternative='two-sided')
        
        # Calculate effect size (r)
        n = len(before)
        r = w_stat / (n * (n + 1) / 2)  # Small sample correction
        
        # Apply correction
        corrected_p_value = self._apply_correction(p_value)
        
        # Determine significance
        significant = corrected_p_value < self.alpha
        
        test_result = {
            'test_name': 'wilcoxon_signed_rank',
            'n': n,
            'mean_before': np.mean(before),
            'mean_after': np.mean(after),
            'median_difference': np.median(after - before),
            'w_statistic': w_stat,
            'p_value': p_value,
            'corrected_p_value': corrected_p_value,
            'effect_size_r': r,
            'significant': significant,
            'interpretation': self._interpret_wilcoxon(w_stat, corrected_p_value, r),
            'assumptions_met': self._check_wilcoxon_assumptions(before, after)
        }
        
        self.test_history.append(test_result)
        return test_result
    
    def kruskal_wallis_test(self, groups: List[np.ndarray]) -> Dict[str, Any]:
        """Perform Kruskal-Wallis H test (non-parametric ANOVA)"""
        try:
            # Perform Kruskal-Wallis test
            h_stat, p_value = stats.kruskalwallis(*groups)
            
            # Calculate effect size (eta squared)
            # Approximation for Kruskal-Wallis
            n_total = sum([len(g) for g in groups])
            eta_squared = h_stat / (n_total - 1)  # Approximate
            
            # Apply correction
            corrected_p_value = self._apply_correction(p_value)
            
            # Determine significance
            significant = corrected_p_value < self.alpha
            
            test_result = {
                'test_name': 'kruskal_wallis',
                'k_groups': len(groups),
                'total_n': n_total,
                'h_statistic': h_stat,
                'p_value': p_value,
                'corrected_p_value': corrected_p_value,
                'eta_squared': eta_squared,
                'significant': significant,
                'interpretation': self._interpret_kruskal_wallis(h_stat, corrected_p_value, eta_squared),
                'posthoc_tests': self._perform_kruskal_wallis_posthoc(groups, corrected_p_value) if significant else []
            }
            
            self.test_history.append(test_result)
            return test_result
            
        except Exception as e:
            return {'error': str(e)}
    
    def _apply_correction(self, p_value: float) -> float:
        """Apply multiple comparison correction"""
        if self.correction_method == 'bonferroni':
            # Bonferroni correction
            return min(p_value * len(self.test_history), 1.0)
        elif self.correction_method == 'holm':
            # Holm-Bonferroni correction
            sorted_p_values = sorted(self.test_history, key=lambda x: x.get('p_value', 1.0))
            corrected = []
            for i, test in enumerate(sorted_p_values):
                corrected.append(min(test.get('p_value', 1.0) * (len(self.test_history) - i), 1.0))
            # Return corrected p-value for most significant test
            return min(corrected) if corrected else p_value
        elif self.correction_method == 'fdr':
            # False Discovery Rate (Benjamini-Hochberg)
            # Simplified FDR correction
            return min(p_value * len(self.test_history), 1.0)
        else:
            return p_value
    
    def _interpret_t_test(self, t_stat: float, p_value: float, effect_size: float) -> str:
        """Interpret t-test results"""
        if p_value < self.alpha:
            if effect_size < 0.2:
                return "Statistically significant but with small effect size. May not be practically significant."
            elif effect_size < 0.5:
                return "Statistically significant with moderate effect size. Practically significant."
            else:
                return "Statistically significant with large effect size. Very practically significant."
        else:
            return "Not statistically significant. No evidence to reject null hypothesis."
    
    def _interpret_one_sample_t_test(self, t_stat: float, p_value: float, effect_size: float) -> str:
        """Interpret one-sample t-test results"""
        return self._interpret_t_test(t_stat, p_value, effect_size)
    
    def _interpret_anova(self, f_stat: float, p_value: float, eta_squared: float) -> str:
        """Interpret ANOVA results"""
        if p_value < self.alpha:
            if eta_squared < 0.01:
                return "Statistically significant with very small effect size. Practically negligible."
            elif eta_squared < 0.06:
                return "Statistically significant with small effect size. Moderately important."
            elif eta_squared < 0.14:
                return "Statistically significant with medium effect size. Important."
            else:
                return "Statistically significant with large effect size. Very important."
        else:
            return "Not statistically significant. No evidence to reject null hypothesis."
    
    def _interpret_chi_square(self, chi2_stat: float, p_value: float) -> str:
        """Interpret chi-square test results"""
        if p_value < self.alpha:
            return "Statistically significant. Observed frequencies differ from expected."
        else:
            return "Not statistically significant. No evidence of difference from expected frequencies."
    
    def _interpret_mann_whitney(self, u_stat: float, p_value: float, r: float) -> str:
        """Interpret Mann-Whitney U test results"""
        if p_value < self.alpha:
            if r < 0.1:
                return "Statistically significant with small effect size. May not be practically significant."
            elif r < 0.3:
                return "Statistically significant with medium effect size. Practically significant."
            else:
                return "Statistically significant with large effect size. Very practically significant."
        else:
            return "Not statistically significant. No evidence to reject null hypothesis."
    
    def _interpret_wilcoxon(self, w_stat: float, p_value: float, r: float) -> str:
        """Interpret Wilcoxon signed-rank test results"""
        return self._interpret_mann_whitney(w_stat, p_value, r)
    
    def _interpret_kruskal_wallis(self, h_stat: float, p_value: float, eta_squared: float) -> str:
        """Interpret Kruskal-Wallis test results"""
        if p_value < self.alpha:
            if eta_squared < 0.01:
                return "Statistically significant with very small effect size. Practically negligible."
            elif eta_squared < 0.06:
                return "Statistically significant with small effect size. Moderately important."
            elif eta_squared < 0.14:
                return "Statistically significant with medium effect size. Important."
            else:
                return "Statistically significant with large effect size. Very important."
        else:
            return "Not statistically significant. No evidence to reject null hypothesis."
    
    def _check_t_test_assumptions(self, group1: np.ndarray, group2: np.ndarray,
                                 equal_var: bool, paired: bool) -> Dict[str, bool]:
        """Check assumptions for t-test"""
        assumptions = {}
        
        if paired:
            # Check for normality of differences
            differences = group1 - group2
            _, shapiro_p = stats.shapiro(differences)
            assumptions['differences_normal'] = shapiro_p > 0.05
        else:
            # Check for normality in each group
            _, shapiro_p1 = stats.shapiro(group1)
            _, shapiro_p2 = stats.shapiro(group2)
            assumptions['group1_normal'] = shapiro_p1 > 0.05
            assumptions['group2_normal'] = shapiro_p2 > 0.05
            
            # Check for equal variances (Levene's test)
            if equal_var:
                assumptions['equal_variances'] = True
            else:
                _, levene_p = stats.levene(group1, group2)
                assumptions['equal_variances'] = levene_p > 0.05
        
        return assumptions
    
    def _check_one_sample_t_test_assumptions(self, sample: np.ndarray) -> Dict[str, bool]:
        """Check assumptions for one-sample t-test"""
        _, shapiro_p = stats.shapiro(sample)
        return {'sample_normal': shapiro_p > 0.05}
    
    def _check_anova_assumptions(self, groups: List[np.ndarray], equal_var: bool) -> Dict[str, bool]:
        """Check assumptions for ANOVA"""
        assumptions = {}
        
        # Check for normality in each group
        normality_results = []
        for group in groups:
            _, shapiro_p = stats.shapiro(group)
            normality_results.append(shapiro_p > 0.05)
        
        assumptions['groups_normal'] = all(normality_results)
        
        # Check for equal variances (Levene's test)
        if equal_var:
            assumptions['equal_variances'] = True
        else:
            # Perform Levene's test
            try:
                _, levene_p = stats.levene(*groups)
                assumptions['equal_variances'] = levene_p > 0.05
            except:
                assumptions['equal_variances'] = False
        
        return assumptions
    
    def _check_chi_square_assumptions(self, observed: np.ndarray, expected: np.ndarray) -> Dict[str, bool]:
        """Check assumptions for chi-square test"""
        assumptions = {}
        
        # Check expected frequencies
        min_expected = np.min(expected)
        assumptions['expected_frequencies_sufficient'] = min_expected >= 5
        
        # Check independence
        assumptions['independence_assumption'] = True  # Subjective
        
        return assumptions
    
    def _check_mann_whitney_assumptions(self, group1: np.ndarray, group2: np.ndarray) -> Dict[str, bool]:
        """Check assumptions for Mann-Whitney U test"""
        assumptions = {}
        
        # Check for continuous or ordinal data
        assumptions['data_continuous_ordinal'] = True  # Subjective
        
        # Check independence
        assumptions['independent_samples'] = True  # Subjective
        
        return assumptions
    
    def _check_wilcoxon_assumptions(self, before: np.ndarray, after: np.ndarray) -> Dict[str, bool]:
        """Check assumptions for Wilcoxon signed-rank test"""
        assumptions = {}
        
        # Check for continuous or ordinal data
        assumptions['data_continuous_ordinal'] = True  # Subjective
        
        # Check for normality of differences
        differences = after - before
        _, shapiro_p = stats.shapiro(differences)
        assumptions['differences_normal'] = shapiro_p > 0.05
        
        return assumptions
    
    def _perform_kruskal_wallis_posthoc(self, groups: List[np.ndarray], alpha: float) -> List[Dict[str, Any]]:
        """Perform post-hoc tests for Kruskal-Wallis test"""
        posthoc_tests = []
        
        # This would be expanded with actual post-hoc test implementation
        # For now, return empty list
        return posthoc_tests
    
    def get_test_summary(self) -> Dict[str, Any]:
        """Get summary of all tests performed"""
        summary = {
            'total_tests': len(self.test_history),
            'significant_tests': sum(1 for test in self.test_history if test.get('significant', False)),
            'non_significant_tests': sum(1 for test in self.test_history if not test.get('significant', False)),
            'test_types': {},
            'significance_levels': {}
        }
        
        for test in self.test_history:
            test_type = test.get('test_name', 'unknown')
            if test_type not in summary['test_types']:
                summary['test_types'][test_type] = 0
            summary['test_types'][test_type] += 1
            
            if test.get('significant', False):
                if test_type not in summary['significance_levels']:
                    summary['significance_levels'][test_type] = 0
                summary['significance_levels'][test_type] += 1
        
        return summary
class PowerAnalyzer:
    """Power analysis for neural data experiments"""
    
    def __init__(self, alpha: float = 0.05):
        self.alpha = alpha
    
    def power_for_t_test(self, effect_size: float, sample_size: int, 
                        tails: int = 2) -> Dict[str, Any]:
        """Calculate power for t-test"""
        # Non-centrality parameter
        ncp = effect_size * np.sqrt(sample_size / 2)
        
        # Degrees of freedom
        df = sample_size - 2
        
        # Critical value
        if tails == 2:
            critical_value = stats.t.ppf(1 - self.alpha / 2, df)
        else:
            critical_value = stats.t.ppf(1 - self.alpha, df)
        
        # Power calculation
        power = 1 - stats.t.cdf(critical_value, df, ncp) + stats.t.cdf(-critical_value, df, ncp)
        
        # Required sample size for desired power
        required_sample_size = self._solve_sample_size_for_power(
            effect_size, self.alpha, power=0.8, tails=tails
        )
        
        return {
            'test': 'independent_t_test',
            'effect_size': effect_size,
            'alpha': self.alpha,
            'sample_size': sample_size,
            'power': power,
            'required_sample_size_for_power_0.8': required_sample_size,
            'tails': tails,
            'interpretation': self._interpret_power(power)
        }
    
    def power_for_anova(self, effect_size: float, sample_size: int, 
                       k_groups: int) -> Dict[str, Any]:
        """Calculate power for ANOVA"""
        # Non-centrality parameter
        ncp = effect_size ** 2 * sample_size
        
        # Degrees of freedom
        df1 = k_groups - 1
        df2 = sample_size * k_groups - k_groups
        
        # Critical value
        critical_value = stats.f.ppf(1 - self.alpha, df1, df2)
        
        # Power calculation
        power = 1 - stats.f.cdf(critical_value, df1, df2, ncp)
        
        # Required sample size for desired power
        required_sample_size = self._solve_sample_size_for_power_anova(
            effect_size, self.alpha, power=0.8, k_groups=k_groups
        )
        
        return {
            'test': 'anova',
            'effect_size': effect_size,
            'alpha': self.alpha,
            'sample_size': sample_size,
            'k_groups': k_groups,
            'power': power,
            'required_sample_size_for_power_0.8': required_sample_size,
            'interpretation': self._interpret_power(power)
        }
    
    def power_for_chi_square(self, effect_size: float, sample_size: int) -> Dict[str, Any]:
        """Calculate power for chi-square test"""
        # Non-centrality parameter
        ncp = effect_size ** 2 * sample_size
        
        # Degrees of freedom (assumed to be 1 for goodness-of-fit test)
        df = 1
        
        # Critical value
        critical_value = stats.chi2.ppf(1 - self.alpha, df)
        
        # Power calculation
        power = 1 - stats.chi2.cdf(critical_value, df, ncp)
        
        # Required sample size for desired power
        required_sample_size = self._solve_sample_size_for_power_chi_square(
            effect_size, self.alpha, power=0.8
        )
        
        return {
            'test': 'chi_square',
            'effect_size': effect_size,
            'alpha': self.alpha,
            'sample_size': sample_size,
            'power': power,
            'required_sample_size_for_power_0.8': required_sample_size,
            'interpretation': self._interpret_power(power)
        }
    
    def power_for_correlation(self, correlation: float, sample_size: int) -> Dict[str, Any]:
        """Calculate power for correlation test"""
        # Non-centrality parameter
        ncp = correlation ** 2 * (sample_size - 2)
        
        # Degrees of freedom
        df = sample_size - 2
        
        # Critical value
        critical_value = stats.t.ppf(1 - self.alpha / 2, df)
        
        # Power calculation
        power = 1 - stats.t.cdf(critical_value, df, ncp) + stats.t.cdf(-critical_value, df, ncp)
        
        # Required sample size for desired power
        required_sample_size = self._solve_sample_size_for_power_correlation(
            correlation, self.alpha, power=0.8
        )
        
        return {
            'test': 'correlation',
            'correlation': correlation,
            'alpha': self.alpha,
            'sample_size': sample_size,
            'power': power,
            'required_sample_size_for_power_0.8': required_sample_size,
            'interpretation': self._interpret_power(power)
        }
    
    def _solve_sample_size_for_power(self, effect_size: float, alpha: float, power: float, tails: int = 2) -> int:
        """Solve for required sample size for t-test"""
        # Function to minimize
        def objective_function(sample_size):
            ncp = effect_size * np.sqrt(sample_size / 2)
            df = sample_size - 2
            
            if tails == 2:
                critical_value = stats.t.ppf(1 - alpha / 2, df)
            else:
                critical_value = stats.t.ppf(1 - alpha, df)
            
            current_power = 1 - stats.t.cdf(critical_value, df, ncp) + stats.t.cdf(-critical_value, df, ncp)
            
            return (current_power - power) ** 2
        
        # Use optimization
        result = optimize.minimize(objective_function, x0=[100], bounds=[(10, 1000)])
        
        return int(np.round(result.x[0]))
    
    def _solve_sample_size_for_power_anova(self, effect_size: float, alpha: float, 
                                           power: float, k_groups: int) -> int:
        """Solve for required sample size for ANOVA"""
        # Function to minimize
        def objective_function(sample_size):
            ncp = effect_size ** 2 * sample_size
            df1 = k_groups - 1
            df2 = sample_size * k_groups - k_groups
            
            critical_value = stats.f.ppf(1 - alpha, df1, df2)
            
            current_power = 1 - stats.f.cdf(critical_value, df1, df2, ncp)
            
            return (current_power - power) ** 2
        
        # Use optimization
        result = optimize.minimize(objective_function, x0=[100], bounds=[(10, 1000)])
        
        return int(np.round(result.x[0]))
    
    def _solve_sample_size_for_power_chi_square(self, effect_size: float, alpha: float, power: float) -> int:
        """Solve for required sample size for chi-square test"""
        # Function to minimize
        def objective_function(sample_size):
            ncp = effect_size ** 2 * sample_size
            df = 1
            
            critical_value = stats.chi2.ppf(1 - alpha, df)
            
            current_power = 1 - stats.chi2.cdf(critical_value, df, ncp)
            
            return (current_power - power) ** 2
        
        # Use optimization
        result = optimize.minimize(objective_function, x0=[100], bounds=[(10, 1000)])
        
        return int(np.round(result.x[0]))
    
    def _solve_sample_size_for_power_correlation(self, correlation: float, alpha: float, power: float) -> int:
        """Solve for required sample size for correlation test"""
        # Function to minimize
        def objective_function(sample_size):
            ncp = correlation ** 2 * (sample_size - 2)
            df = sample_size - 2
            
            critical_value = stats.t.ppf(1 - alpha / 2, df)
            
            current_power = 1 - stats.t.cdf(critical_value, df, ncp) + stats.t.cdf(-critical_value, df, ncp)
            
            return (current_power - power) ** 2
        
        # Use optimization
        result = optimize.minimize(objective_function, x0=[100], bounds=[(10, 1000)])
        
        return int(np.round(result.x[0]))
    
    def _interpret_power(self, power: float) -> str:
        """Interpret power value"""
        if power < 0.05:
            return "Very low power. Very likely to commit Type II error."
        elif power < 0.2:
            return "Low power. High risk of Type II error."
        elif power < 0.8:
            return "Moderate power. Acceptable for exploratory research."
        else:
            return "High power. Good chance of detecting true effects."
class MetaAnalyzer:
    """Meta-analysis for neural data"""
    
    def __init__(self):
        self.analysis_history: List[Dict[str, Any]] = []
    
    def perform_meta_analysis(self, effect_sizes: List[float], sample_sizes: List[int],
                              weights: Optional[List[float]] = None,
                              publication_bias_correction: bool = False) -> MetaAnalysisResult:
        """Perform meta-analysis"""
        n_studies = len(effect_sizes)
        
        # Calculate weights
        if weights is None:
            # Default weights based on sample size
            weights = [max(1.0, s) for s in sample_sizes]
        
        # Normalize weights
        total_weight = sum(weights)
        normalized_weights = [w / total_weight for w in weights]
        
        # Calculate weighted effect size
        weighted_effect_size = sum(w * e for w, e in zip(normalized_weights, effect_sizes))
        
        # Calculate variance
        variances = []
        for i, (effect_size, weight) in enumerate(zip(effect_sizes, normalized_weights)):
            # Simplified variance calculation
            variance = 1 / weight
            variances.append(variance)
        
        total_variance = sum(variances)
        
        # Calculate heterogeneity (I²)
        heterogeneity = sum([(effect_sizes[i] - weighted_effect_size) ** 2 * normalized_weights[i]
                           for i in range(n_studies)]) / total_variance if total_variance > 0 else 0
        i2 = 100 * min(heterogeneity, 1.0)
        
        # Calculate confidence interval
        se = np.sqrt(total_variance)
        ci_lower = weighted_effect_size - 1.96 * se
        ci_upper = weighted_effect_size + 1.96 * se
        
        # Publication bias correction (Egger's test)
        publication_bias_result = None
        if publication_bias_correction:
            publication_bias_result = self._eggers_test(effect_sizes, sample_sizes)
        
        # Subgroup analysis
        subgroup_analysis = self._perform_subgroup_analysis(effect_sizes, sample_sizes, normalized_weights)
        
        # Sensitivity analysis
        sensitivity_analysis = self._perform_sensitivity_analysis(effect_sizes, sample_sizes)
        
        # Create result
        result = MetaAnalysisResult(
            analysis_id=f"meta_analysis_{len(self.analysis_history)}",
            study_ids=[f"study_{i}" for i in range(n_studies)],
            effect_sizes=np.array(effect_sizes),
            sample_sizes=sample_sizes,
            weights=normalized_weights,
            overall_effect_size=weighted_effect_size,
            overall_variance=total_variance,
            heterogeneity_i2=i2,
            publication_bias=publication_bias_result,
            subgroup_analysis=subgroup_analysis,
            sensitivity_analysis=sensitivity_analysis
        )
        
        self.analysis_history.append({
            'type': 'meta_analysis',
            'id': result.analysis_id,
            'n_studies': n_studies,
            'effect_size': weighted_effect_size,
            'heterogeneity_i2': i2
        })
        
        return result
    
    def _eggers_test(self, effect_sizes: List[float], sample_sizes: List[int]) -> Dict[str, Any]:
        """Perform Egger's test for publication bias"""
        # This is a simplified implementation
        # Full implementation would require regression of effect sizes on precision
        
        n_studies = len(effect_sizes)
        
        # Calculate precision
        precisions = [1 / np.sqrt(n) for n in sample_sizes]
        
        # Calculate intercept and slope
        x = np.array(precisions)
        y = np.array(effect_sizes)
        
        # Linear regression
        slope, intercept, r_value, p_value, std_err = stats.linregress(x, y)
        
        # Test for intercept significantly different from zero
        t_stat = intercept / std_err if std_err > 0 else 0
        df = n_studies - 2
        p_value_test = 2 * (1 - stats.t.cdf(abs(t_stat), df)) if df > 0 else 1.0
        
        return {
            'slope': slope,
            'intercept': intercept,
            'r_squared': r_value ** 2,
            'p_value': p_value_test,
            'significant_publication_bias': p_value_test < 0.05,
            'interpretation': 'Publication bias present' if p_value_test < 0.05 else 'No evidence of publication bias'
        }
    
    def _perform_subgroup_analysis(self, effect_sizes: List[float], sample_sizes: List[int],
                                   weights: List[float]) -> Dict[str, Any]:
        """Perform subgroup analysis"""
        # This would be expanded with actual subgroup analysis
        # For now, return a placeholder
        return {
            'subgroups': [],
            'between_subgroup_heterogeneity': 0.0,
            'within_subgroup_heterogeneity': 0.0,
            'interpretation': 'Subgroup analysis not yet implemented'
        }
    
    def _perform_sensitivity_analysis(self, effect_sizes: List[float], sample_sizes: List[int]) -> Dict[str, Any]:
        """Perform sensitivity analysis"""
        # This would be expanded with actual sensitivity analysis
        # For now, return a placeholder
        return {
            'influence_statistics': [],
            'leave_one_out_results': [],
            'interpretation': 'Sensitivity analysis not yet implemented'
        }
class ReproducibilityAnalyzer:
    """Analyzer for reproducibility of neural data analysis"""
    
    def __init__(self):
        self.reproducibility_history: List[Dict[str, Any]] = []
    
    def assess_reproducibility(self, dataset_id: str, analysis_type: str,
                              original_researcher: str, replication_attempts: int,
                              results: List[Dict[str, Any]],
                              methodological_differences: List[Dict[str, Any]]) -> ReproducibilityReport:
        """Assess reproducibility of neural data analysis"""
        
        # Calculate successful replications
        successful_replications = sum(1 for result in results 
                                    if result.get('result_agreement', 0) > 0.8)
        
        # Calculate reproducibility score
        reproducibility_score = (
            successful_replications / max(1, replication_attempts) *
            np.mean([result.get('result_agreement', 0) for result in results])
        )
        
        # Calculate result agreement
        if results:
            result_agreement = np.mean([result.get('result_agreement', 0) for result in results])
        else:
            result_agreement = 0.0
        
        # Interpret reproducibility score
        if reproducibility_score < 0.3:
            interpretation = "Very low reproducibility. Major concerns about reliability."
        elif reproducibility_score < 0.6:
            interpretation = "Low reproducibility. Limited confidence in results."
        elif reproducibility_score < 0.8:
            interpretation = "Moderate reproducibility. Some confidence but with limitations."
        else:
            interpretation = "High reproducibility. Good confidence in results."
        
        # Create report
        report = ReproducibilityReport(
            reproducibility_id=f"repro_{len(self.reproducibility_history)}",
            dataset_id=dataset_id,
            analysis_type=analysis_type,
            original_researcher=original_researcher,
            replication_attempts=replication_attempts,
            successful_replications=successful_replications,
            reproducibility_score=reproducibility_score,
            methodological_differences=methodological_differences,
            result_agreement=result_agreement,
            issues_identified=self._identify_issues(methodological_differences, results),
            recommendations=self._generate_recommendations(reproducibility_score, methodological_differences),
            confidence_interval=(reproducibility_score - 0.1, reproducibility_score + 0.1)
        )
        
        self.reproducibility_history.append({
            'type': 'reproducibility',
            'id': report.reproducibility_id,
            'dataset_id': dataset_id,
            'analysis_type': analysis_type,
            'reproducibility_score': reproducibility_score,
            'successful_replications': successful_replications
        })
        
        return report
    
    def _identify_issues(self, methodological_differences: List[Dict[str, Any]],
                         results: List[Dict[str, Any]]) -> List[str]:
        """Identify issues affecting reproducibility"""
        issues = []
        
        for diff in methodological_differences:
            if diff.get('impact', 'low') == 'high':
                issues.append(f"High-impact methodological difference: {diff.get('description', '')}")
        
        # Check for result variability
        if len(results) > 1:
            agreements = [r.get('result_agreement', 0) for r in results]
            if max(agreements) - min(agreements) > 0.3:
                issues.append("High variability in replication results")
        
        return issues
    
    def _generate_recommendations(self, reproducibility_score: float,
                                  methodological_differences: List[Dict[str, Any]]) -> List[str]:
        """Generate recommendations based on reproducibility assessment"""
        recommendations = []
        
        if reproducibility_score < 0.5:
            recommendations.extend([
                "Increase transparency in methodological reporting",
                "Provide detailed replication protocols",
                "Standardize data collection and analysis procedures",
                "Implement independent verification of results"
            ])
        elif reproducibility_score < 0.8:
            recommendations.extend([
                "Address key methodological differences",
                "Provide access to original data and code",
                "Document all analytical decisions"
            ])
        else:
            recommendations.extend([
                "Maintain current rigorous practices",
                "Continue independent verification",
                "Share methodology with broader community"
            ])
        
        return recommendations
class CollaborationManager:
    """Manager for neural data analysis collaboration"""
    
    def __init__(self):
        self.collaboration_projects: Dict[str, CollaborationProject] = {}
        self.data_shares: Dict[str, Dict[str, float]] = {}
    
    def create_project(self, project_config: Dict[str, Any]) -> CollaborationProject:
        """Create new collaboration project"""
        project_id = f"project_{len(self.collaboration_projects) + 1}"
        
        project = CollaborationProject(
            project_id=project_id,
            title=project_config.get('title', ''),
            description=project_config.get('description', ''),
            principal_investigator=project_config.get('principal_investigator', ''),
            co_investigators=project_config.get('co_investigators', []),
            research_institutions=project_config.get('research_institutions', []),
            project_duration=tuple(project_config.get('start_date', datetime.now()),
                                  project_config.get('end_date', datetime.now() + datetime.timedelta(days=365))),
            objectives=project_config.get('objectives', []),
            datasets=project_config.get('datasets', []),
            shared_resources=project_config.get('shared_resources', {}),
            data_shares=project_config.get('data_shares', {}),
            governance_structure=project_config.get('governance_structure', {}),
            publication_plan=project_config.get('publication_plan', {}),
            milestones=project_config.get('milestones', [])
        )
        
        self.collaboration_projects[project_id] = project
        return project
    
    def get_project(self, project_id: str) -> Optional[CollaborationProject]:
        """Get collaboration project"""
        return self.collaboration_projects.get(project_id)
    
    def update_data_share(self, project_id: str, researcher: str, share: float):
        """Update data share for researcher"""
        if project_id in self.collaboration_projects:
            project = self.collaboration_projects[project_id]
            
            if researcher not in project.data_shares:
                project.data_shares[researcher] = 0.0
            
            project.data_shares[researcher] = share
    
    def get_project_data_shares(self, project_id: str) -> Dict[str, float]:
        """Get data shares for project"""
        if project_id in self.collaboration_projects:
            return self.collaboration_projects[project_id].data_shares.copy()
        return {}
class ExperimentDesigner:
    """Designer for neural simulation experiments"""
    
    def __init__(self):
        self.experiment_templates: Dict[str, Dict[str, Any]] = {}
    
    def create_experiment(self, experiment_config: Dict[str, Any]) -> ExperimentSetup:
        """Create new experiment setup"""
        experiment_id = f"exp_{len(self.experiment_templates) + 1}"
        
        # Validate experiment configuration
        self._validate_experiment_config(experiment_config)
        
        # Create experiment setup
        experiment = ExperimentSetup(
            experiment_id=experiment_id,
            name=experiment_config.get('name', ''),
            description=experiment_config.get('description', ''),
            hypotheses=experiment_config.get('hypotheses', []),
            variables=experiment_config.get('variables', {}),
            sample_size=experiment_config.get('sample_size'),
            power_analysis=experiment_config.get('power_analysis'),
            randomization_method=experiment_config.get('randomization_method', 'random'),
            blinding=experiment_config.get('blinding', False),
            controls=experiment_config.get('controls', []),
            interventions=experiment_config.get('interventions', []),
            outcomes=experiment_config.get('outcomes', []),
            statistical_tests=experiment_config.get('statistical_tests', []),
            data_collection_methods=experiment_config.get('data_collection_methods', []),
            ethical_considerations=experiment_config.get('ethical_considerations'),
            funding_source=experiment_config.get('funding_source'),
            start_date=experiment_config.get('start_date'),
            end_date=experiment_config.get('end_date'),
            status='planning'
        )
        
        # Save experiment configuration
        self.experiment_templates[experiment_id] = experiment_config
        
        return experiment
    
    def _validate_experiment_config(self, config: Dict[str, Any]):
        """Validate experiment configuration"""
        required_fields = ['name', 'description', 'hypotheses', 'variables', 'controls', 'interventions', 'outcomes']
        
        for field in required_fields:
            if field not in config:
                raise ValueError(f"Missing required field: {field}")
        
        # Validate sample size if provided
        if 'sample_size' in config and config['sample_size'] is not None:
            if config['sample_size'] <= 0:
                raise ValueError("Sample size must be positive")
        
        # Validate power analysis if provided
        if 'power_analysis' in config and config['power_analysis'] is not None:
            pa = config['power_analysis']
            if 'effect_size' not in pa:
                raise ValueError("Power analysis must include effect size")
            if 'alpha' not in pa:
                raise ValueError("Power analysis must include alpha")
    
    def get_experiment_templates(self) -> List[Dict[str, Any]]:
        """Get all experiment templates"""
        return list(self.experiment_templates.values())
    
    def get_experiment(self, experiment_id: str) -> Optional[ExperimentSetup]:
        """Get experiment by ID"""
        if experiment_id in self.experiment_templates:
            config = self.experiment_templates[experiment_id]
            return self.create_experiment(config)
        return None
class DataValidationFramework:
    """Framework for validating neural data"""
    
    def __init__(self):
        self.validation_rules: List[ValidationRule] = []
        self.validation_history: List[Dict[str, Any]] = []
    
    def add_validation_rule(self, rule: ValidationRule):
        """Add validation rule"""
        self.validation_rules.append(rule)
    
    def validate_data(self, dataset_id: str, data: Any,
                     validation_type: str = 'comprehensive') -> ValidationResult:
        """Validate neural data"""
        validation_id = f"val_{len(self.validation_history)}"
        
        # Perform validation
        passed_validations = []
        failed_validations = []
        
        for rule in self.validation_rules:
            if rule.column not in data.columns:
                if rule.is_required:
                    failed_validations.append({
                        'rule': rule.rule_id,
                        'message': f"Required column '{rule.column}' is missing",
                        'severity': 'high'
                    })
                continue
            
            # Apply validation rule
            is_valid = self._apply_validation_rule(data[rule.column], rule)
            
            if is_valid:
                passed_validations.append({
                    'rule': rule.rule_id,
                    'message': f"Column '{rule.column}' passed validation",
                    'severity': rule.get('severity', 'low')
                })
            else:
                failed_validations.append({
                    'rule': rule.rule_id,
                    'message': rule.error_message,
                    'severity': 'high'
                })
        
        # Calculate overall score
        total_validations = len(self.validation_rules)
        passed_count = len(passed_validations)
        overall_score = passed_count / total_validations if total_validations > 0 else 0
        
        # Generate recommendations
        recommendations = self._generate_validation_recommendations(failed_validations)
        
        # Create validation result
        result = ValidationResult(
            validation_id=validation_id,
            dataset_id=dataset_id,
            validation_type=validation_type,
            validation_rules=[{
                'rule_id': rule.rule_id,
                'rule_type': rule.rule_type,
                'column': rule.column,
                'condition': rule.condition
            } for rule in self.validation_rules],
            passed_validations=passed_validations,
            failed_validations=failed_validations,
            overall_score=overall_score,
            recommendations=recommendations,
            validator='AutoValidation'
        )
        
        self.validation_history.append({
            'type': 'validation',
            'id': validation_id,
            'dataset_id': dataset_id,
            'validation_type': validation_type,
            'overall_score': overall_score,
            'passed_count': passed_count,
            'failed_count': len(failed_validations)
        })
        
        return result
    
    def _apply_validation_rule(self, column_data: Any, rule: ValidationRule) -> bool:
        """Apply validation rule to column data"""
        if rule.rule_type == 'range':
            min_val = rule.parameters.get('min')
            max_val = rule.parameters.get('max')
            
            if min_val is not None:
                if (column_data < min_val).any():
                    return False
            
            if max_val is not None:
                if (column_data > max_val).any():
                    return False
            
            return True
        
        elif rule.rule_type == 'pattern':
            pattern = rule.parameters.get('pattern')
            if pattern:
                # Check if all values match pattern
                for value in column_data:
                    if not str(value).match(pattern):
                        return False
            return True
        
        elif rule.rule_type == 'enum':
            allowed_values = rule.parameters.get('allowed_values', [])
            if not set(column_data).issubset(set(allowed_values)):
                return False
            return True
        
        elif rule.rule_type == 'unique':
            return column_data.nunique() == len(column_data)
        
        else:
            # Default validation - assume valid
            return True
    
    def _generate_validation_recommendations(self, failed_validations: List[Dict[str, Any]]) -> List[str]:
        """Generate validation recommendations"""
        recommendations = []
        
        for failed in failed_validations:
            rule_id = failed['rule']
            message = failed['message']
            
            if 'missing' in message.lower():
                recommendations.append(f"Add missing column to dataset")
            elif 'range' in message.lower():
                recommendations.append(f"Check data ranges and filter outliers")
            elif 'pattern' in message.lower():
                recommendations.append(f"Validate data format and clean invalid entries")
            elif 'unique' in message.lower():
                recommendations.append(f"Remove duplicate values in column")
        
        if not recommendations:
            recommendations.append("All validations passed. Data quality is good.")
        
        return recommendations
    
    def get_validation_summary(self) -> Dict[str, Any]:
        """Get summary of validation results"""
        summary = {
            'total_validations': len(self.validation_history),
            'average_score': np.mean([v.get('overall_score', 0) for v in self.validation_history]) if self.validation_history else 0,
            'total_failed': sum(v.get('failed_count', 0) for v in self.validation_history),
            'total_passed': sum(v.get('passed_count', 0) for v in self.validation_history)
        }
        
        return summary
class AdvancedResearchTools:
    """Main advanced tools for neural data research"""
    
    def __init__(self):
        self.statistical_framework = StatisticalTestingFramework()
        self.power_analyzer = PowerAnalyzer()
        self.meta_analyzer = MetaAnalyzer()
        self.reproducibility_analyzer = ReproducibilityAnalyzer()
        self.collaboration_manager = CollaborationManager()
        self.experiment_designer = ExperimentDesigner()
        self.validation_framework = DataValidationFramework()
    
    def setup_statistical_analysis(self, alpha: float = 0.05, correction_method: str = 'bonferroni'):
        """Setup statistical analysis framework"""
        self.statistical_framework = StatisticalTestingFramework(alpha, correction_method)
        return self.statistical_framework
    
    def perform_power_analysis(self, **kwargs) -> PowerAnalysisResult:
        """Perform power analysis"""
        return self.power_analyzer.power_for_t_test(**kwargs)
    
    def perform_meta_analysis(self, effect_sizes: List[float], sample_sizes: List[int],
                             **kwargs) -> MetaAnalysisResult:
        """Perform meta-analysis"""
        return self.meta_analyzer.perform_meta_analysis(
            effect_sizes, sample_sizes, **kwargs
        )
    
    def assess_reproducibility(self, dataset_id: str, analysis_type: str,
                               original_researcher: str, replication_attempts: int,
                               results: List[Dict[str, Any]],
                               methodological_differences: List[Dict[str, Any]]) -> ReproducibilityReport:
        """Assess reproducibility of neural data analysis"""
        return self.reproducibility_analyzer.assess_reproducibility(
            dataset_id, analysis_type, original_researcher, replication_attempts,
            results, methodological_differences
        )
    
    def create_collaboration_project(self, project_config: Dict[str, Any]) -> CollaborationProject:
        """Create collaboration project"""
        return self.collaboration_manager.create_project(project_config)
    
    def design_experiment(self, experiment_config: Dict[str, Any]) -> ExperimentSetup:
        """Design neural simulation experiment"""
        return self.experiment_designer.create_experiment(experiment_config)
    
    def validate_data(self, dataset_id: str, data: Any,
                     validation_type: str = 'comprehensive') -> ValidationResult:
        """Validate neural data"""
        return self.validation_framework.validate_data(dataset_id, data, validation_type)
    
    def get_research_summary(self) -> Dict[str, Any]:
        """Get research tools summary"""
        return {
            'statistical_tests': len(self.statistical_framework.test_history),
            'meta_analyses': len(self.meta_analyzer.analysis_history),
            'reproducibility_assessments': len(self.reproducibility_analyzer.reproducibility_history),
            'validations': len(self.validation_framework.validation_history),
            'collaboration_projects': len(self.collaboration_manager.collaboration_projects),
            'experiment_templates': len(self.experiment_designer.experiment_templates)
        }

# Convenience functions

def create_research_tools() -> AdvancedResearchTools:
    """Convenience function to create research tools"""
    return AdvancedResearchTools()

def setup_statistical_analysis(alpha: float = 0.05, correction_method: str = 'bonferroni'):
    """Convenience function to setup statistical analysis"""
    tools = AdvancedResearchTools()
    return tools.setup_statistical_analysis(alpha, correction_method)

def perform_power_analysis_for_t_test(effect_size: float, sample_size: int, 
                                     alpha: float = 0.05, tails: int = 2) -> PowerAnalysisResult:
    """Convenience function to perform power analysis for t-test"""
    analyzer = PowerAnalyzer(alpha)
    return analyzer.power_for_t_test(effect_size, sample_size, tails)

def create_meta_analysis(effect_sizes: List[float], sample_sizes: List[int],
                        weights: Optional[List[float]] = None,
                        publication_bias_correction: bool = False) -> MetaAnalysisResult:
    """Convenience function to create meta-analysis"""
    analyzer = MetaAnalyzer()
    return analyzer.perform_meta_analysis(effect_sizes, sample_sizes, weights, publication_bias_correction)

def assess_reproducibility(dataset_id: str, analysis_type: str,
                           original_researcher: str, replication_attempts: int,
                           results: List[Dict[str, Any]],
                           methodological_differences: List[Dict[str, Any]]) -> ReproducibilityReport:
    """Convenience function to assess reproducibility"""
    analyzer = ReproducibilityAnalyzer()
    return analyzer.assess_reproducibility(
        dataset_id, analysis_type, original_researcher, replication_attempts,
        results, methodological_differences
    )

# Export all classes and functions
__all__ = [
    'ResearchMethod', 'ExperimentSetup', 'StatisticalTest',
    'PowerAnalysisResult', 'MetaAnalysisResult', 'ReproducibilityReport',
    'CollaborationProject', 'ValidationResult',
    'StatisticalTestingFramework', 'PowerAnalyzer', 'MetaAnalyzer',
    'ReproducibilityAnalyzer', 'CollaborationManager', 'ExperimentDesigner',
    'DataValidationFramework', 'AdvancedResearchTools',
    'create_research_tools', 'setup_statistical_analysis',
    'perform_power_analysis_for_t_test', 'create_meta_analysis',
    'assess_reproducibility'
]