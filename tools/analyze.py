#!/usr/bin/env python3
"""
NLM Analyze Tool - Brain Analysis and Statistics

Analyze NLM brain files, connectivity, and system state.
"""

import argparse
import sys
import os
from pathlib import Path
import json
import struct
from typing import Dict, Any, List

def read_checkpoint(filepath: str) -> Dict[str, Any]:
    """Read NLM checkpoint file and extract statistics."""
    stats = {
        'file_size': os.path.getsize(filepath),
        'neuron_count': 0,
        'synapse_count': 0,
        'format_version': 'unknown',
        'metadata': {}
    }
    
    try:
        # Simple checkpoint format detection
        with open(filepath, 'rb') as f:
            header = f.read(64)
            if len(header) >= 4:
                # Try to parse metadata
                f.seek(0)
                data = f.read(256)
                # Look for magic number or version indicator
                if b'NLM_CHK' in header:
                    stats['format_version'] = 'nlm_checkpoint_v1'
                    # Parse simple metadata (placeholder)
                    f.seek(64)
                    # Read some basic structure
                    metadata_size = struct.unpack('<I', f.read(4))[0] if len(f.read(4)) == 4 else 0
                    if metadata_size > 0 and metadata_size < 1024:
                        meta_data = f.read(metadata_size)
                        try:
                            stats['metadata'] = json.loads(meta_data.decode('utf-8'))
                        except:
                            pass
    except Exception as e:
        stats['error'] = str(e)
    
    return stats

def analyze_brain_file(filepath: str) -> Dict[str, Any]:
    """Comprehensive analysis of brain file."""
    if not os.path.exists(filepath):
        return {'error': f"File not found: {filepath}"}
    
    if filepath.endswith('.json'):
        # JSON configuration analysis
        with open(filepath, 'r') as f:
            config = json.load(f)
        return {
            'type': 'config',
            'size': len(json.dumps(config)),
            'keys': list(config.keys()),
            'sections': {}
        }
    else:
        # Binary checkpoint analysis
        return read_checkpoint(filepath)

def print_summary(stats: Dict[str, Any]):
    """Print human-readable summary of analysis."""
    print("=== NLM Brain Analysis ===")
    
    if 'error' in stats:
        print(f"ERROR: {stats['error']}")
        return
    
    print(f"File type: {stats.get('type', 'unknown')}")
    print(f"File size: {stats.get('file_size', 0):,} bytes")
    print(f"Format version: {stats.get('format_version', 'unknown')}")
    
    if stats.get('type') == 'config':
        print(f"Configuration keys: {len(stats.get('keys', []))}")
        for key in stats.get('keys', [])[:10]:  # Show first 10 keys
            print(f"  - {key}")
        if len(stats.get('keys', [])) > 10:
            print(f"  ... and {len(stats.get('keys', [])) - 10} more")
    
    if 'metadata' in stats:
        print("\nMetadata:")
        for key, value in stats['metadata'].items():
            print(f"  {key}: {value}")

def main():
    parser = argparse.ArgumentParser(
        description="Analyze NLM brain files and checkpoints",
        epilog="Examples:\n"
               "  nlm-analyze my_brain.bin\n"
               "  nlm-analyze config.json --format json\n"
               "  nlm-analyze my_brain.bin --save analysis.json"
    )
    
    parser.add_argument('filepath', help='Path to NLM brain file (checkpoint or config)')
    parser.add_argument('--format', choices=['text', 'json'], default='text',
                       help='Output format (default: text)')
    parser.add_argument('--save', help='Save analysis results to JSON file')
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Verbose output with more details')
    
    args = parser.parse_args()
    
    # Analyze the file
    stats = analyze_brain_file(args.filepath)
    
    if args.format == 'json':
        print(json.dumps(stats, indent=2, default=str))
    else:
        print_summary(stats)
    
    # Save if requested
    if args.save and 'error' not in stats:
        try:
            with open(args.save, 'w') as f:
                json.dump(stats, f, indent=2, default=str)
            print(f"\nAnalysis saved to: {args.save}")
        except Exception as e:
            print(f"Error saving analysis: {e}")

if __name__ == '__main__':
    main()
