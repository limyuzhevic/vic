"""
Performance metrics export utilities for NLM neural simulation framework.

This module provides utilities for exporting performance metrics and analysis
results to various file formats for reporting and documentation purposes.
"""

import json
import csv
import xml.etree.ElementTree as ET
from xml.dom import minidom
import os
from typing import Dict, List, Any, Optional
import pandas as pd

from .enhanced_api import NLMError

class MetricsExporter:
    """
    Utility class for exporting performance metrics and analysis results.
    
    Supports multiple export formats including JSON, CSV, XML, and tabular formats
    for various use cases including reporting, documentation, and analysis.
    """
    
    def __init__(self):
        self.export_formats = ['json', 'csv', 'xml', 'tabular', 'excel']
    
    def export_json(self, data: Dict[str, Any], filepath: str, **kwargs) -> str:
        """
        Export data as JSON format.
        
        Args:
            data: Data to export
            filepath: Output file path
            **kwargs: Additional export options
        
        Returns:
            Success message
        """
        try:
            # Prepare data for export
            export_data = self._prepare_json_data(data, **kwargs)
            
            # Write JSON file
            with open(filepath, 'w') as f:
                json.dump(export_data, f, indent=2, default=str)
            
            return f"JSON data exported to {filepath}"
            
        except Exception as e:
            raise NLMError(f"Error exporting JSON: {e}")
    
    def export_csv(self, data: Dict[str, Any], filepath: str, **kwargs) -> str:
        """
        Export data as CSV format.
        
        Args:
            data: Data to export
            filepath: Output file path
            **kwargs: Additional export options
        
        Returns:
            Success message
        """
        try:
            # Prepare data for CSV export
            export_data = self._prepare_csv_data(data, **kwargs)
            
            # Write CSV file
            with open(filepath, 'w', newline='') as f:
                writer = csv.writer(f)
                
                # Write header
                if export_data['headers']:
                    writer.writerow(export_data['headers'])
                
                # Write data rows
                for row in export_data['rows']:
                    writer.writerow(row)
            
            return f"CSV data exported to {filepath}"
            
        except Exception as e:
            raise NLMError(f"Error exporting CSV: {e}")
    
    def export_xml(self, data: Dict[str, Any], filepath: str, **kwargs) -> str:
        """
        Export data as XML format.
        
        Args:
            data: Data to export
            filepath: Output file path
            **kwargs: Additional export options
        
        Returns:
            Success message
        """
        try:
            # Create XML structure
            root = ET.Element('NLMPerformanceMetrics')
            root.set('version', '1.0')
            root.set('export_time', str(__import__('time').time()))
            
            # Add metadata
            metadata = ET.SubElement(root, 'Metadata')
            ET.SubElement(metadata, 'ExportedBy').text = 'NLM Performance Monitor'
            ET.SubElement(metadata, 'ExportTimestamp').text = str(__import__('time').time())
            
            # Add data sections
            self._add_data_to_xml(root, data, **kwargs)
            
            # Pretty print XML
            xml_str = ET.tostring(root, encoding='unicode')
            parsed_xml = minidom.parseString(xml_str)
            pretty_xml = parsed_xml.toprettyxml(indent='  ')
            
            # Write XML file
            with open(filepath, 'w') as f:
                f.write(pretty_xml)
            
            return f"XML data exported to {filepath}"
            
        except Exception as e:
            raise NLMError(f"Error exporting XML: {e}")
    
    def export_tabular(self, data: Dict[str, Any], filepath: str, **kwargs) -> str:
        """
        Export data as tabular format (Markdown or plain text).
        
        Args:
            data: Data to export
            filepath: Output file path
            **kwargs: Additional export options
        
        Returns:
            Success message
        """
        try:
            format_type = kwargs.get('format', 'markdown').lower()
            
            if format_type == 'markdown':
                table_data = self._prepare_markdown_table(data, **kwargs)
                return self._write_text_file(table_data, filepath)
            elif format_type == 'plain':
                table_data = self._prepare_plain_text_table(data, **kwargs)
                return self._write_text_file(table_data, filepath)
            else:
                raise NLMError(f"Unsupported tabular format: {format_type}")
                
        except Exception as e:
            raise NLMError(f"Error exporting tabular data: {e}")
    
    def export_excel(self, data: Dict[str, Any], filepath: str, **kwargs) -> str:
        """
        Export data as Excel format.
        
        Args:
            data: Data to export
            filepath: Output file path
            **kwargs: Additional export options
        
        Returns:
            Success message
        """
        try:
            # Check if pandas is available
            try:
                import pandas as pd
            except ImportError:
                raise NLMError("Excel export requires pandas. Install with: pip install pandas")
            
            # Prepare data for Excel
            excel_data = self._prepare_excel_data(data, **kwargs)
            
            # Write Excel file
            excel_data.to_excel(filepath, index=False, **kwargs)
            
            return f"Excel data exported to {filepath}"
            
        except Exception as e:
            raise NLMError(f"Error exporting Excel: {e}")
    
    def export_multiple_formats(self, data: Dict[str, Any], base_filepath: str, 
                               formats: Optional[List[str]] = None, **kwargs) -> List[str]:
        """
        Export data to multiple formats.
        
        Args:
            data: Data to export
            base_filepath: Base file path (without extension)
            formats: List of formats to export (default: all formats)
            **kwargs: Additional export options
        
        Returns:
            List of success messages
        """
        if formats is None:
            formats = self.export_formats
        
        results = []
        
        for format_name in formats:
            if format_name not in self.export_formats:
                raise NLMError(f"Unsupported export format: {format_name}")
            
            filepath = f"{base_filepath}.{format_name}"
            
            try:
                if format_name == 'json':
                    result = self.export_json(data, filepath, **kwargs)
                elif format_name == 'csv':
                    result = self.export_csv(data, filepath, **kwargs)
                elif format_name == 'xml':
                    result = self.export_xml(data, filepath, **kwargs)
                elif format_name == 'tabular':
                    result = self.export_tabular(data, filepath, **kwargs)
                elif format_name == 'excel':
                    result = self.export_excel(data, filepath, **kwargs)
                
                results.append(result)
                
            except NLMError as e:
                results.append(f"Error exporting {format_name}: {e}")
            except Exception as e:
                results.append(f"Unexpected error exporting {format_name}: {e}")
        
        return results
    
    def _prepare_json_data(self, data: Dict[str, Any], **kwargs) -> Dict[str, Any]:
        """Prepare data for JSON export"""
        export_data = data.copy()
        
        # Add metadata
        export_data['metadata'] = {
            'export_time': __import__('time').time(),
            'export_format': 'json',
            'version': '1.0.0',
            'generator': 'NLM Performance Monitor',
        }
        
        # Add summary information
        export_data['summary'] = self._generate_export_summary(data)
        
        return export_data
    
    def _prepare_csv_data(self, data: Dict[str, Any], **kwargs) -> Dict[str, Any]:
        """Prepare data for CSV export"""
        headers = []
        rows = []
        
        # Convert data to CSV format
        if 'performance_stats' in data:
            stats = data['performance_stats']
            
            # Add headers
            headers = ['Metric', 'Value', 'Unit', 'Status']
            
            # Add performance metrics rows
            for metric_name, metric_data in stats.items():
                if isinstance(metric_data, dict):
                    for sub_metric, value in metric_data.items():
                        rows.append([
                            f"{metric_name}.{sub_metric}",
                            value,
                            self._get_metric_unit(metric_name, sub_metric),
                            self._get_metric_status(metric_name, sub_metric, value)
                        ])
        
        return {'headers': headers, 'rows': rows}
    
    def _prepare_excel_data(self, data: Dict[str, Any], **kwargs) -> pd.DataFrame:
        """Prepare data for Excel export"""
        try:
            # Convert to DataFrame
            if 'performance_stats' in data:
                stats = data['performance_stats']
                
                # Flatten nested data
                flattened_data = {}
                for category, metrics in stats.items():
                    if isinstance(metrics, dict):
                        for metric_name, value in metrics.items():
                            flattened_data[f"{category}.{metric_name}"] = value
                
                return pd.DataFrame([flattened_data])
            else:
                return pd.DataFrame([data])
                
        except ImportError:
            raise NLMError("Excel export requires pandas. Install with: pip install pandas")
    
    def _prepare_markdown_table(self, data: Dict[str, Any], **kwargs) -> str:
        """Prepare data for Markdown table export"""
        if 'performance_stats' not in data:
            return "| Metric | Value | Unit | Status |\n|--------|-------|------|--------|\n| No data available | | | |"
        
        stats = data['performance_stats']
        
        # Create table
        table = "| Metric | Value | Unit | Status |\n"
        table += "|--------|-------|------|--------|\n"
        
        for metric_name, metric_data in stats.items():
            if isinstance(metric_data, dict):
                for sub_metric, value in metric_data.items():
                    table += f"| {metric_name}.{sub_metric} | {value} | {self._get_metric_unit(metric_name, sub_metric)} | {self._get_metric_status(metric_name, sub_metric, value)} |\n"
        
        return table
    
    def _prepare_plain_text_table(self, data: Dict[str, Any], **kwargs) -> str:
        """Prepare data for plain text table export"""
        table = "PERFORMANCE METRICS\n"
        table += "=" * 50 + "\n\n"
        
        if 'performance_stats' in data:
            stats = data['performance_stats']
            
            for metric_name, metric_data in stats.items():
                if isinstance(metric_data, dict):
                    table += f"{metric_name}:\n"
                    for sub_metric, value in metric_data.items():
                        table += f"  {sub_metric}: {value} ({self._get_metric_unit(metric_name, sub_metric)}) - {self._get_metric_status(metric_name, sub_metric, value)}\n"
                    table += "\n"
        
        return table
    
    def _add_data_to_xml(self, parent: ET.Element, data: Dict[str, Any], **kwargs):
        """Add data to XML structure"""
        # Add performance statistics
        if 'performance_stats' in data:
            stats_section = ET.SubElement(parent, 'PerformanceStatistics')
            self._add_dict_to_xml(stats_section, data['performance_stats'])
        
        # Add efficiency metrics
        if 'efficiency_metrics' in data:
            efficiency_section = ET.SubElement(parent, 'EfficiencyMetrics')
            self._add_dict_to_xml(efficiency_section, data['efficiency_metrics'])
        
        # Add optimization recommendations
        if 'optimization_recommendations' in data:
            recommendations_section = ET.SubElement(parent, 'OptimizationRecommendations')
            for i, recommendation in enumerate(data['optimization_recommendations']):
                ET.SubElement(recommendations_section, f'Recommendation{i+1}').text = recommendation
    
    def _add_dict_to_xml(self, parent: ET.Element, data: Dict[str, Any]):
        """Recursively add dictionary to XML"""
        for key, value in data.items():
            if isinstance(value, dict):
                sub_element = ET.SubElement(parent, key)
                self._add_dict_to_xml(sub_element, value)
            elif isinstance(value, list):
                list_element = ET.SubElement(parent, key)
                for i, item in enumerate(value):
                    item_element = ET.SubElement(list_element, f'Item{i+1}')
                    if isinstance(item, dict):
                        self._add_dict_to_xml(item_element, item)
                    else:
                        item_element.text = str(item)
            else:
                child_element = ET.SubElement(parent, key)
                child_element.text = str(value)
    
    def _get_metric_unit(self, metric_name: str, sub_metric: str) -> str:
        """Get unit for metric"""
        unit_map = {
            'steps_per_second': 'SPS',
            'spikes_per_second': 'SpS',
            'avg_time_per_step': 'ms',
            'memory_efficiency': '%',
            'cpu_efficiency': '%',
            'spike_efficiency': 'spikes/neuron',
            'time_coefficient_of_variation': 'unitless',
            'computational_efficiency': 'steps/ms',
            'memory_per_step': 'MB',
            'cpu_per_step': '%',
        }
        
        return unit_map.get(f"{metric_name}.{sub_metric}", 'unitless')
    
    def _get_metric_status(self, metric_name: str, sub_metric: str, value: Any) -> str:
        """Get status for metric value"""
        if isinstance(value, (int, float)):
            if metric_name == 'steps_per_second' and value < 100:
                return 'Poor'
            elif metric_name == 'memory_efficiency' and value < 0.5:
                return 'Poor'
            elif metric_name == 'cpu_efficiency' and value < 0.5:
                return 'Poor'
            elif metric_name == 'spike_efficiency' and value < 2:
                return 'Poor'
            else:
                return 'Good'
        return 'Unknown'
    
    def _generate_export_summary(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Generate summary information for export"""
        summary = {
            'export_timestamp': __import__('time').time(),
            'data_size': len(str(data)),
            'contains_performance_stats': 'performance_stats' in data,
            'contains_efficiency_metrics': 'efficiency_metrics' in data,
            'contains_recommendations': 'optimization_recommendations' in data,
        }
        
        if 'performance_stats' in data:
            stats = data['performance_stats']
            summary['total_steps'] = stats.get('total_steps', 0)
            summary['elapsed_time_seconds'] = stats.get('elapsed_time_seconds', 0.0)
            summary['final_efficiency_score'] = stats.get('final_efficiency_score', 0.0)
        
        return summary
    
    def _write_text_file(self, content: str, filepath: str) -> str:
        """Write content to text file"""
        with open(filepath, 'w') as f:
            f.write(content)
        
        return f"Text data exported to {filepath}"
