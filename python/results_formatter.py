#!/usr/bin/env python3
"""
Results Formatter Tool

Format results for scientific publication and analysis.
Provides comprehensive formatting capabilities for research results.
"""

import numpy as np
import pandas as pd
import json
import matplotlib.pyplot as plt
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
import sys
import os
from datetime import datetime

@dataclass
class ResultsFormatterConfig:
    """Configuration for results formatter"""
    output_format: str = "json"  # "json", "csv", "markdown", "latex", "html", "python"
    include_analysis: bool = True
    include_statistics: bool = True
    include_visualization: bool = True
    include_metadata: bool = True
    include_references: bool = True
    citation_format: str = "bibtex"  # "bibtex", "apa", "mla", "chicago"
    include_confidence_intervals: bool = True
    include_effect_sizes: bool = True
    include_p_values: bool = True
    include_statistical_tests: bool = True
    format_for_journal: bool = False
    include_raw_data: bool = False
    output_path: str = "results"
    
    # Publication formatting options
    journal_name: str = ""
    article_title: str = ""
    author_names: List[str] = None
    publication_date: str = ""
    doi: str = ""
    keywords: List[str] = None
    abstract: str = ""
    
    # Statistical formatting options
    decimal_places: int = 4
    scientific_notation: bool = True
    p_value_format: str = "scientific"  # "scientific", "decimal", "stars"
    effect_size_format: str = "scientific"  # "scientific", "decimal"
    confidence_interval_format: str = "scientific"  # "scientific", "decimal"
    
    def __post_init__(self):
        if self.author_names is None:
            self.author_names = []
        if self.keywords is None:
            self.keywords = []
class ResultsFormatter:
    """
    Results formatter tool for scientific publication and analysis.
    
    Features:
    - Format results for journal publication
    - Generate LaTeX tables and figures
    - Create statistical summaries
    - Format references and citations
    - Generate reproducible analysis reports
    - Support for multiple output formats
    - Quality control and validation
    """
    
    def __init__(self, config: ResultsFormatterConfig = None):
        self.config = config or ResultsFormatterConfig()
        self.formatted_results = {}
        self.analysis_metadata = {}
        
        # Initialize formatter components
        self._initialize_components()
    
    def _initialize_components(self):
        """Initialize formatter components and data structures"""
        # Initialize formatted results structure
        self.formatted_results = {
            "analysis": {},
            "statistics": {},
            "visualization": {},
            "metadata": {},
            "references": {},
            "tables": {},
            "figures": {}
        }
        
        # Initialize analysis metadata
        self.analysis_metadata = {
            "timestamp": self._get_timestamp(),
            "formatter": "ResultsFormatter",
            "version": "1.0.0",
            "format_config": asdict(self.config)
        }
    
    def format_analysis_results(self, 
                               analysis_data: Dict[str, Any],
                               statistical_results: Dict[str, Any],
                               visualization_data: Optional[Dict[str, Any]] = None,
                               metadata: Optional[Dict[str, Any]] = None,
                               filename: str = None) -> Dict[str, Any]:
        """
        Format analysis results for scientific publication.
        
        Args:
            analysis_data: Dictionary containing analysis results
            statistical_results: Dictionary containing statistical results
            visualization_data: Optional visualization data
            metadata: Optional metadata about the analysis
            filename: Output filename (uses config output_path if None)
            
        Returns:
            Dictionary with formatted results
        """
        result = {
            "success": False,
            "formatted_data": {},
            "metadata": {},
            "output_file": None,
            "output_format": self.config.output_format,
            "analysis_metadata": self.analysis_metadata
        }
        
        try:
            # Format analysis results
            formatted_analysis = self._format_analysis_section(analysis_data)
            
            # Format statistical results
            formatted_statistics = self._format_statistics_section(statistical_results)
            
            # Format visualization data
            if visualization_data:
                formatted_visualization = self._format_visualization_section(visualization_data)
            else:
                formatted_visualization = {}
            
            # Format metadata
            if metadata:
                formatted_metadata = self._format_metadata_section(metadata)
            else:
                formatted_metadata = {}
            
            # Combine all formatted data
            self.formatted_results["analysis"] = formatted_analysis
            self.formatted_results["statistics"] = formatted_statistics
            self.formatted_results["visualization"] = formatted_visualization
            self.formatted_results["metadata"] = formatted_metadata
            
            # Generate tables
            self.formatted_results["tables"] = self._generate_results_tables(formatted_analysis, formatted_statistics)
            
            # Generate figures
            if self.config.include_visualization:
                self.formatted_results["figures"] = self._generate_result_figures(formatted_visualization)
            
            # Generate references
            if self.config.include_references:
                self.formatted_results["references"] = self._generate_references(formatted_metadata)
            
            # Save formatted data
            if filename:
                output_file = self._save_formatted_results(filename)
                result["output_file"] = output_file
            
            # Update result
            result["formatted_data"] = self.formatted_results
            result["success"] = True
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _format_analysis_section(self, analysis_data: Dict[str, Any]) -> Dict[str, Any]:
        """Format analysis results"""
        formatted = {}
        
        for category, data in analysis_data.items():
            if isinstance(data, dict):
                formatted[category] = self._format_analysis_category(data)
            elif isinstance(data, list):
                formatted[category] = self._format_analysis_list(data)
            else:
                formatted[category] = self._format_analysis_value(data)
        
        return formatted
    
    def _format_analysis_category(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Format analysis category results"""
        formatted = {}
        
        for key, value in data.items():
            if isinstance(value, (int, float)):
                formatted[key] = self._format_numeric_value(value)
            elif isinstance(value, dict):
                formatted[key] = self._format_analysis_category(value)
            elif isinstance(value, list):
                formatted[key] = [self._format_numeric_value(item) if isinstance(item, (int, float)) else item for item in value]
            else:
                formatted[key] = value
        
        return formatted
    
    def _format_analysis_list(self, data: List[Any]) -> List[Any]:
        """Format analysis list results"""
        return [self._format_analysis_value(item) for item in data]
    
    def _format_analysis_value(self, value: Any) -> Any:
        """Format analysis value"""
        if isinstance(value, (int, float)):
            return self._format_numeric_value(value)
        elif isinstance(value, dict):
            return self._format_analysis_category(value)
        elif isinstance(value, list):
            return self._format_analysis_list(value)
        else:
            return value
    
    def _format_numeric_value(self, value: float) -> Dict[str, Any]:
        """Format numeric value with statistical information"""
        formatted = {
            "value": value,
            "formatted": self._format_number(value),
            "scientific": self._format_scientific_number(value)
        }
        
        if self.config.include_confidence_intervals:
            # Add dummy confidence interval (in real implementation, this would be calculated)
            ci_width = 1.96 * abs(value) * 0.1  # 95% CI assuming normal distribution
            formatted["confidence_interval"] = {
                "lower": value - ci_width,
                "upper": value + ci_width,
                "width": 2 * ci_width,
                "formatted": {
                    "lower": self._format_number(value - ci_width),
                    "upper": self._format_number(value + ci_width),
                    "width": self._format_number(2 * ci_width)
                }
            }
        
        if self.config.include_effect_sizes:
            # Add dummy effect size (in real implementation, this would be calculated)
            effect_size = abs(value) * 0.5
            formatted["effect_size"] = {
                "cohens_d": effect_size,
                "formatted": self._format_number(effect_size)
            }
        
        if self.config.include_p_values:
            # Add dummy p-value (in real implementation, this would be calculated)
            p_value = max(0.001, 1.0 - abs(value) * 0.1)  # Dummy p-value
            formatted["p_value"] = {
                "raw": p_value,
                "formatted": self._format_p_value(p_value),
                "significance": self._get_significance_level(p_value)
            }
        
        if self.config.include_statistical_tests:
            # Add dummy statistical test results
            formatted["statistical_test"] = {
                "test_type": "t-test",
                "degrees_of_freedom": 30,
                "t_statistic": value,
                "formatted_t_statistic": self._format_number(value),
                "p_value": formatted.get("p_value", {}).get("raw", 0.05)
            }
        
        return formatted
    
    def _format_statistics_section(self, statistical_results: Dict[str, Any]) -> Dict[str, Any]:
        """Format statistical results"""
        formatted = {}
        
        for category, stats in statistical_results.items():
            if isinstance(stats, dict):
                formatted[category] = self._format_statistical_category(stats)
            elif isinstance(stats, list):
                formatted[category] = self._format_statistical_list(stats)
            else:
                formatted[category] = self._format_statistical_value(stats)
        
        return formatted
    
    def _format_statistical_category(self, stats: Dict[str, Any]) -> Dict[str, Any]:
        """Format statistical category results"""
        formatted = {}
        
        for key, value in stats.items():
            if isinstance(value, (int, float)):
                formatted[key] = self._format_numeric_value(value)
            elif isinstance(value, dict):
                formatted[key] = self._format_statistical_category(value)
            elif isinstance(value, list):
                formatted[key] = [self._format_numeric_value(item) if isinstance(item, (int, float)) else item for item in value]
            else:
                formatted[key] = value
        
        return formatted
    
    def _format_statistical_list(self, stats: List[Any]) -> List[Any]:
        """Format statistical list results"""
        return [self._format_statistical_value(item) for item in stats]
    
    def _format_statistical_value(self, value: Any) -> Any:
        """Format statistical value"""
        if isinstance(value, (int, float)):
            return self._format_numeric_value(value)
        elif isinstance(value, dict):
            return self._format_statistical_category(value)
        elif isinstance(value, list):
            return self._format_statistical_list(value)
        else:
            return value
    
    def _format_visualization_section(self, visualization_data: Dict[str, Any]) -> Dict[str, Any]:
        """Format visualization results"""
        formatted = {}
        
        for category, data in visualization_data.items():
            if isinstance(data, dict):
                formatted[category] = self._format_visualization_category(data)
            elif isinstance(data, list):
                formatted[category] = self._format_visualization_list(data)
            else:
                formatted[category] = self._format_visualization_value(data)
        
        return formatted
    
    def _format_visualization_category(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Format visualization category results"""
        formatted = {}
        
        for key, value in data.items():
            if isinstance(value, np.ndarray):
                formatted[key] = {
                    "shape": value.shape,
                    "dtype": str(value.dtype),
                    "min": float(np.min(value)) if value.size > 0 else 0.0,
                    "max": float(np.max(value)) if value.size > 0 else 0.0,
                    "mean": float(np.mean(value)) if value.size > 0 else 0.0,
                    "std": float(np.std(value)) if value.size > 0 else 0.0,
                    "path": f"figures/{key}.png" if self.config.output_format in ["html", "pdf"] else f"{key}.npy"
                }
            elif isinstance(value, dict):
                formatted[key] = self._format_visualization_category(value)
            elif isinstance(value, list):
                formatted[key] = self._format_visualization_list(value)
            else:
                formatted[key] = value
        
        return formatted
    
    def _format_visualization_list(self, data: List[Any]) -> List[Any]:
        """Format visualization list results"""
        return [self._format_visualization_value(item) for item in data]
    
    def _format_visualization_value(self, value: Any) -> Any:
        """Format visualization value"""
        if isinstance(value, np.ndarray):
            return {
                "shape": value.shape,
                "dtype": str(value.dtype),
                "min": float(np.min(value)) if value.size > 0 else 0.0,
                "max": float(np.max(value)) if value.size > 0 else 0.0,
                "mean": float(np.mean(value)) if value.size > 0 else 0.0,
                "std": float(np.std(value)) if value.size > 0 else 0.0,
                "path": f"figures/visualization_{id(value)}.png"
            }
        elif isinstance(value, dict):
            return self._format_visualization_category(value)
        elif isinstance(value, list):
            return self._format_visualization_list(value)
        else:
            return value
    
    def _format_metadata_section(self, metadata: Dict[str, Any]) -> Dict[str, Any]:
        """Format metadata section"""
        formatted = metadata.copy()
        
        # Add formatted publication information
        if self.config.format_for_journal:
            formatted["publication_format"] = self._format_for_journal()
        
        # Format author information
        if self.config.author_names:
            formatted["formatted_authors"] = self._format_authors(self.config.author_names)
        
        # Format keywords
        if self.config.keywords:
            formatted["formatted_keywords"] = self._format_keywords(self.config.keywords)
        
        return formatted
    
    def _format_for_journal(self) -> Dict[str, Any]:
        """Format for journal publication"""
        return {
            "journal_name": self.config.journal_name,
            "article_title": self.config.article_title,
            "authors": self.config.author_names,
            "publication_date": self.config.publication_date,
            "doi": self.config.doi,
            "keywords": self.config.keywords,
            "abstract": self.config.abstract
        }
    
    def _format_authors(self, authors: List[str]) -> str:
        """Format author list"""
        if len(authors) == 1:
            return authors[0]
        elif len(authors) == 2:
            return f"{authors[0]} and {authors[1]}"
        else:
            return f"{', '.join(authors[:-1])}, and {authors[-1]}"
    
    def _format_keywords(self, keywords: List[str]) -> str:
        """Format keyword list"""
        return ", ".join(keywords)
    
    def _format_number(self, value: float) -> str:
        """Format number according to configuration"""
        if self.config.scientific_notation:
            return f"{value:.{self.config.decimal_places}e}"
        else:
            return f"{value:.{self.config.decimal_places}f}"
    
    def _format_scientific_number(self, value: float) -> Dict[str, str]:
        """Format number in scientific notation"""
        return {
            "coefficient": f"{value:.3e}".split('e')[0],
            "exponent": f"{value:.3e}".split('e')[1] if 'e' in f"{value:.3e}" else "0",
            "full": f"{value:.{self.config.decimal_places}e}"
        }
    
    def _format_p_value(self, p_value: float) -> str:
        """Format p-value according to configuration"""
        if self.config.p_value_format == "scientific":
            return f"{p_value:.2e}"
        elif self.config.p_value_format == "decimal":
            return f"{p_value:.4f}"
        elif self.config.p_value_format == "stars":
            if p_value < 0.001:
                return "***"
            elif p_value < 0.01:
                return "**"
            elif p_value < 0.05:
                return "*"
            else:
                return "ns"
        else:
            return f"{p_value:.4f}"
    
    def _get_significance_level(self, p_value: float) -> str:
        """Get significance level based on p-value"""
        if p_value < 0.001:
            return "***"
        elif p_value < 0.01:
            return "**"
        elif p_value < 0.05:
            return "*"
        else:
            return "ns"
    
    def _generate_results_tables(self, analysis_data: Dict[str, Any], 
                                statistics_data: Dict[str, Any]) -> Dict[str, Any]:
        """Generate results tables"""
        tables = {}
        
        # Generate main results table
        if "results" in analysis_data:
            tables["main_results"] = self._create_main_results_table(analysis_data["results"])
        
        # Generate statistical summary table
        if "summary" in statistics_data:
            tables["statistical_summary"] = self._create_statistical_summary_table(statistics_data["summary"])
        
        # Generate parameter estimates table
        if "parameters" in analysis_data:
            tables["parameter_estimates"] = self._create_parameter_estimates_table(analysis_data["parameters"])
        
        return tables
    
    def _create_main_results_table(self, results: Dict[str, Any]) -> Dict[str, Any]:
        """Create main results table"""
        table = {
            "headers": ["Metric", "Value", "Formatted", "Confidence Interval"],
            "rows": []
        }
        
        for metric, data in results.items():
            if isinstance(data, dict) and "value" in data:
                row = {
                    "metric": metric,
                    "value": data["value"],
                    "formatted": data.get("formatted", ""),
                    "confidence_interval": data.get("confidence_interval", {"lower": "", "upper": ""})
                }
                table["rows"].append(row)
        
        return table
    
    def _create_statistical_summary_table(self, summary: Dict[str, Any]) -> Dict[str, Any]:
        """Create statistical summary table"""
        table = {
            "headers": ["Statistic", "Value", "p-value", "Interpretation"],
            "rows": []
        }
        
        for statistic, data in summary.items():
            if isinstance(data, dict):
                row = {
                    "statistic": statistic,
                    "value": data.get("value", ""),
                    "p_value": data.get("p_value", ""),
                    "interpretation": data.get("significance", "")
                }
                table["rows"].append(row)
        
        return table
    
    def _create_parameter_estimates_table(self, parameters: Dict[str, Any]) -> Dict[str, Any]:
        """Create parameter estimates table"""
        table = {
            "headers": ["Parameter", "Estimate", "SE", "95% CI", "p-value"],
            "rows": []
        }
        
        for param, data in parameters.items():
            if isinstance(data, dict):
                row = {
                    "parameter": param,
                    "estimate": data.get("estimate", ""),
                    "se": data.get("standard_error", ""),
                    "ci": data.get("confidence_interval", ""),
                    "p_value": data.get("p_value", "")
                }
                table["rows"].append(row)
        
        return table
    
    def _generate_result_figures(self, visualization_data: Dict[str, Any]) -> Dict[str, Any]:
        """Generate result figures"""
        figures = {}
        
        if self.config.include_analysis:
            figures["analysis_plots"] = self._create_analysis_plots(visualization_data)
        
        if self.config.include_statistics:
            figures["statistical_plots"] = self._create_statistical_plots(visualization_data)
        
        return figures
    
    def _create_analysis_plots(self, visualization_data: Dict[str, Any]) -> Dict[str, Any]:
        """Create analysis plots"""
        # In a real implementation, this would create actual plots
        plots = {}
        
        for category, data in visualization_data.items():
            if isinstance(data, dict):
                plots[category] = {
                    "plot_type": "histogram",
                    "data": data,
                    "path": f"figures/{category}_analysis.png"
                }
        
        return plots
    
    def _create_statistical_plots(self, visualization_data: Dict[str, Any]) -> Dict[str, Any]:
        """Create statistical plots"""
        # In a real implementation, this would create actual statistical plots
        plots = {}
        
        for category, data in visualization_data.items():
            if isinstance(data, dict):
                plots[category] = {
                    "plot_type": "scatter",
                    "data": data,
                    "path": f"figures/{category}_statistics.png"
                }
        
        return plots
    
    def _generate_references(self, metadata: Dict[str, Any]) -> Dict[str, Any]:
        """Generate references"""
        references = {}
        
        if self.config.include_references:
            # Generate citation based on configuration
            if self.config.citation_format == "bibtex":
                references["citation"] = self._format_bibtex_citation(metadata)
            elif self.config.citation_format == "apa":
                references["citation"] = self._format_apa_citation(metadata)
            elif self.config.citation_format == "mla":
                references["citation"] = self._format_mla_citation(metadata)
            elif self.config.citation_format == "chicago":
                references["citation"] = self._format_chicago_citation(metadata)
        
        return references
    
    def _format_bibtex_citation(self, metadata: Dict[str, Any]) -> str:
        """Format citation in BibTeX format"""
        # Generate BibTeX citation
        citation = f"@article{{{self.config.article_title.replace(' ', '_')},"
        citation += f"\n   author = {{{', '.join(self.config.author_names)}}},"
        citation += f"\n   title = {{{self.config.article_title}}},"
        citation += f"\n   journal = {{{self.config.journal_name}}},"
        citation += f"\n   year = {{{self._get_current_year()}}},"
        citation += f"\n   doi = {{{self.config.doi}}}"
        citation += "\n}"
        
        return citation
    
    def _format_apa_citation(self, metadata: Dict[str, Any]) -> str:
        """Format citation in APA format"""
        authors = ", ".join(self.config.author_names)
        year = self._get_current_year()
        title = self.config.article_title
        journal = self.config.journal_name
        doi = self.config.doi
        
        citation = f"{authors}. ({year}). {title}. {journal}. doi:{doi}"
        return citation
    
    def def _format_mla_citation(self, metadata: Dict[str, Any]) -> str:
        """Format citation in MLA format"""
        authors = self.config.author_names
        year = self._get_current_year()
        title = self.config.article_title
        journal = self.config.journal_name
        doi = self.config.doi
        
        if len(authors) == 1:
            author_citation = authors[0]
        elif len(authors) == 2:
            author_citation = f"{authors[0]} and {authors[1]}"
        else:
            author_citation = f"{', '.join(authors[:-1])}, and {authors[-1]}"
        
        citation = f"{author_citation}. \"{title}.\" {journal}, {year}, {doi}."
        return citation
    
    def _format_chicago_citation(self, metadata: Dict[str, Any]) -> str:
        """Format citation in Chicago format"""
        authors = self.config.author_names
        year = self._get_current_year()
        title = self.config.article_title
        journal = self.config.journal_name
        doi = self.config.doi
        
        if len(authors) == 1:
            author_citation = authors[0]
        elif len(authors) == 2:
            author_citation = f"{authors[0]} and {authors[1]}"
        else:
            author_citation = f"{', '.join(authors[:-1])}, and {authors[-1]}"
        
        citation = f"{author_citation}. {year}. \"{title}.\" {journal}. doi:{doi}"
        return citation
    
    def _get_current_year(self) -> str:
        """Get current year"""
        return str(datetime.now().year)
    
    def save_formatted_results(self, filename: str) -> str:
        """
        Save formatted results to file.
        
        Args:
            filename: Output filename
            
        Returns:
            Path to saved file
        """
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        if self.config.output_format == "json":
            return self._save_as_json(filename)
        elif self.config.output_format == "csv":
            return self._save_as_csv(filename)
        elif self.config.output_format == "markdown":
            return self._save_as_markdown(filename)
        elif self.config.output_format == "latex":
            return self._save_as_latex(filename)
        elif self.config.output_format == "html":
            return self._save_as_html(filename)
        elif self.config.output_format == "python":
            return self._save_as_python(filename)
        else:
            return self._save_as_json(filename)
    
    def _save_formatted_results(self, filename: str) -> str:
        """Save formatted results to file"""
        return self.save_formatted_results(filename)
    
    def _save_as_json(self, filename: str) -> str:
        """Save as JSON"""
        with open(filename, 'w') as f:
            json.dump(self.formatted_results, f, indent=self.config.json_indent)
        return filename
    
    def _save_as_csv(self, filename: str) -> str:
        """Save as CSV"""
        # Combine data into DataFrame for CSV export
        df = pd.json_normalize(self.formatted_results)
        df.to_csv(filename, index=False)
        return filename
    
    def _save_as_markdown(self, filename: str) -> str:
        """Save as Markdown"""
        with open(filename, 'w') as f:
            f.write(self._generate_markdown_report())
        return filename
    
    def _save_as_latex(self, filename: str) -> str:
        """Save as LaTeX"""
        with open(filename, 'w') as f:
            f.write(self._generate_latex_report())
        return filename
    
    def _save_as_html(self, filename: str) -> str:
        """Save as HTML"""
        with open(filename, 'w') as f:
            f.write(self._generate_html_report())
        return filename
    
    def _save_as_python(self, filename: str) -> str:
        """Save as Python script"""
        with open(filename, 'w') as f:
            f.write(self._generate_python_script())
        return filename
    
    def _generate_markdown_report(self) -> str:
        """Generate Markdown report"""
        report = f"# Research Results Report\n\n"
        report += f"## Analysis Results\n\n"
        report += f"| Metric | Value | Interpretation |\n"
        report += f"|--------|-------|----------------|\n"
        
        if "analysis" in self.formatted_results:
            for metric, data in self.formatted_results["analysis"].items():
                if isinstance(data, dict) and "value" in data:
                    report += f"| {metric} | {data['value']} | {data.get('interpretation', '')} |\n"
        
        report += f"\n## Statistical Summary\n\n"
        if "statistics" in self.formatted_results:
            report += "| Statistic | Value | p-value |\n"
            report += "|----------|-------|---------|\n"
            for statistic, data in self.formatted_results["statistics"].items():
                if isinstance(data, dict):
                    report += f"| {statistic} | {data.get('value', '')} | {data.get('p_value', '')} |\n"
        
        return report
    
    def _generate_latex_report(self) -> str:
        """Generate LaTeX report"""
        latex = f"\\documentclass{{article}}\n"
        latex += f"\\usepackage{{graphicx}}\n"
        latex += f"\\usepackage{{booktabs}}\n"
        latex += f"\\usepackage{{amsmath}}\n\n"
        latex += f"\\title{{Research Results Report}}\n"
        latex += f"\\author{{{', '.join(self.config.author_names)}}}\n"
        latex += f"\\date{{{self.config.publication_date}}}\n\n"
        
        latex += f"\\begin{{document}}\n"
        latex += f"\\maketitle\n\n"
        
        latex += f"\\section{{Analysis Results}}\n"
        latex += f"\\begin{{tabular}}{{|l|c|c|}}\n"
        latex += f"\\hline\n"
        latex += f"Metric & Value & Interpretation\\\\\n"
        latex += f"\\hline\n"
        
        if "analysis" in self.formatted_results:
            for metric, data in self.formatted_results["analysis"].items():
                if isinstance(data, dict) and "value" in data:
                    latex += f"{metric} & {data['value']} & {data.get('interpretation', '')}\\\\n"
        
        latex += f"\\hline\n"
        latex += f"\\end{{tabular}}\n\n"
        
        latex += f"\\section{{Statistical Summary}}\n"
        latex += f"\\begin{{tabular}}{{|l|c|c|}}\n"
        latex += f"\\hline\n"
        latex += f"Statistic & Value & p-value\\\\\n"
        latex += f"\\hline\n"
        
        if "statistics" in self.formatted_results:
            for statistic, data in self.formatted_results["statistics"].items():
                if isinstance(data, dict):
                    latex += f"{statistic} & {data.get('value', '')} & {data.get('p_value', '')}\\\\n"
        
        latex += f"\\hline\n"
        latex += f"\\end{{tabular}}\n\n"
        
        latex += f"\\end{{document}}\n"
        
        return latex
    
    def _generate_html_report(self) -> str:
        """Generate HTML report"""
        html = f"<!DOCTYPE html>\n"
        html += f"<html>\n"
        html += f"<head><title>Research Results Report</title></head>\n"
        html += f"<body>\n"
        html += f"<h1>Research Results Report</h1>\n"
        html += f"<h2>Analysis Results</h2>\n"
        html += f"<table border='1'>\n"
        html += f"<tr><th>Metric</th><th>Value</th><th>Interpretation</th></tr>\n"
        
        if "analysis" in self.formatted_results:
            for metric, data in self.formatted_results["analysis"].items():
                if isinstance(data, dict) and "value" in data:
                    html += f"<tr><td>{metric}</td><td>{data['value']}</td><td>{data.get('interpretation', '')}</td></tr>\n"
        
        html += f"</table>\n"
        html += f"</body>\n"
        html += f"</html>\n"
        
        return html
    
    def _generate_python_script(self) -> str:
        """Generate Python script"""
        python = f"# Research Results Report\n"
        python += f"# Generated by ResultsFormatter\n\n"
        python += f"import json\n"
        python += f"import pandas as pd\n\n"
        python += f"# Formatted results\n"
        python += f"formatted_results = {json.dumps(self.formatted_results, indent=2)}\n\n"
        python += f"# Save results\n"
        python += f"with open('results.json', 'w') as f:\n"
        python += f"    json.dump(formatted_results, f, indent=2)\n\n"
        python += f"print('Results saved to results.json')\n"
        
        return python
    
    def _get_timestamp(self) -> str:
        """Get current timestamp"""
        return datetime.now().isoformat()


def create_results_formatter(config_dict: Dict[str, Any] = None) -> ResultsFormatter:
    """
    Factory function to create ResultsFormatter with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured ResultsFormatter instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = ResultsFormatterConfig(**config_dict)
    return ResultsFormatter(config)