#!/usr/bin/env python3
"""
Brain Batch Processor Example
Demonstrates batch processing capabilities for brain data.
"""

import sys
import os
import argparse
from pathlib import Path
from typing import List, Optional
import json
import time

def parse_args():
    parser = argparse.ArgumentParser(description="Brain Batch Processor")
    parser.add_argument("--input-dir", required=True, help="Input directory containing brain files")
    parser.add_argument("--output-dir", required=True, help="Output directory for processed files")
    parser.add_argument("--parallel", action="store_true", help="Use parallel processing")
    parser.add_argument("--threads", type=int, default=2, help="Number of threads for parallel processing")
    parser.add_argument("--format", default="json", choices=["json", "csv", "binary"], help="Output format")
    parser.add_argument("--monitor", action="store_true", help="Monitor performance during processing")
    return parser.parse_args()

class BrainBatchProcessor:
    def __init__(self, args):
        self.input_dir = Path(args.input_dir)
        self.output_dir = Path(args.output_dir)
        self.parallel = args.parallel
        self.threads = args.threads
        self.format = args.format
        self.monitor = args.monitor
        
        # Create output directory
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
        # Statistics
        self.stats = {
            "total_files": 0,
            "processed_files": 0,
            "successful": 0,
            "failed": 0,
            "total_time": 0.0
        }
    
    def process_file(self, input_path: Path) -> dict:
        """Process a single brain file."""
        result = {
            "input_path": str(input_path),
            "output_path": "",
            "success": False,
            "error": "",
            "processing_time": 0.0,
            "file_size": input_path.stat().st_size
        }
        
        try:
            start_time = time.time()
            
            # Determine output path
            filename = input_path.stem
            output_filename = f"{filename}.processed.{self.format}"
            result["output_path"] = str(self.output_dir / output_filename)
            
            # Read input file
            with open(input_path, 'r') as f:
                content = f.read()
            
            # Simulate brain processing
            processed_content = self.process_brain_data(content, input_path.suffix)
            
            # Write output file
            with open(result["output_path"], 'w') as f:
                if self.format == "json":
                    json_data = {
                        "source": str(input_path),
                        "processed": True,
                        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
                        "content": processed_content,
                        "file_size": len(processed_content)
                    }
                    f.write(json.dumps(json_data, indent=2))
                elif self.format == "csv":
                    f.write("field,value,source\n")
                    f.write(f"input_path,{input_path},{input_path.name}\n")
                    f.write(f"processed,{processed_content[:100]},partial\n")
                else:  # binary
                    with open(result["output_path"], 'wb') as bin_f:
                        bin_f.write(f"BRAIN_BIN_{len(processed_content)}".encode())
                        bin_f.write(processed_content.encode())
                        bin_f.write(b"\nEND_BIN\n")
                    
                    # Re-open in text mode for consistency
                    with open(result["output_path"], 'r') as text_f:
                        content = text_f.read()
                    f.write(content)
            
            end_time = time.time()
            result["processing_time"] = end_time - start_time
            result["success"] = True
            
        except Exception as e:
            result["error"] = str(e)
            self.stats["failed"] += 1
        
        return result
    
    def process_brain_data(self, data: str, file_extension: str) -> str:
        """Process brain data based on file type."""
        # Simple processing based on file extension
        if file_extension == '.json':
            try:
                json_data = json.loads(data)
                # Simulate enhancement
                json_data["processed"] = True
                json_data["processor"] = "brain_batch_processor"
                json_data["processing_time"] = time.strftime("%Y-%m-%dT%H:%M:%S")
                return json.dumps(json_data, indent=2)
            except:
                return data
        elif file_extension == '.txt':
            # Simple text processing
            lines = data.split('\n')
            processed_lines = []
            for i, line in enumerate(lines):
                if line.strip():
                    processed_lines.append(f"Line {i+1}: {line.strip()} [processed]")
            return '\n'.join(processed_lines)
        else:
            # Generic processing
            return data + f"\n[Processed at {time.strftime('%Y-%m-%d %H:%M:%S')}]"
    
    def process_sequential(self) -> List[dict]:
        """Process files sequentially."""
        results = []
        input_files = list(self.input_dir.glob('*'))
        
        self.stats["total_files"] = len(input_files)
        
        print(f"Processing {len(input_files)} files sequentially...")
        
        for input_path in input_files:
            if input_path.is_file():
                result = self.process_file(input_path)
                results.append(result)
                if result["success"]:
                    self.stats["successful"] += 1
                self.stats["processed_files"] += 1
        
        return results
    
    def process_parallel(self) -> List[dict]:
        """Process files in parallel using threading."""
        import concurrent.futures
        
        results = []
        input_files = list(self.input_dir.glob('*'))
        
        self.stats["total_files"] = len(input_files)
        
        print(f"Processing {len(input_files)} files in parallel with {self.threads} threads...")
        
        with concurrent.futures.ThreadPoolExecutor(max_workers=self.threads) as executor:
            # Submit all tasks
            future_to_file = {
                executor.submit(self.process_file, input_path): input_path 
                for input_path in input_files if input_path.is_file()
            }
            
            # Collect results as they complete
            for future in concurrent.futures.as_completed(future_to_file):
                input_path = future_to_file[future]
                try:
                    result = future.result()
                    results.append(result)
                    if result["success"]:
                        self.stats["successful"] += 1
                    self.stats["processed_files"] += 1
                except Exception as e:
                    print(f"Error processing {input_path}: {e}")
                    self.stats["failed"] += 1
        
        return results
    
    def generate_report(self, results: List[dict]) -> str:
        """Generate a processing report."""
        report = "# Brain Batch Processing Report\n"
        report += f"Generated: {time.strftime('%Y-%m-%dT%H:%M:%S')}\n\n"
        
        report += "## Summary\n"
        report += f"- Total files: {self.stats['total_files']}\n"
        report += f"- Processed files: {self.stats['processed_files']}\n"
        report += f"- Successful: {self.stats['successful']}\n"
        report += f"- Failed: {self.stats['failed']}\n"
        report += f"- Total time: {self.stats['total_time']:.2f} seconds\n\n"
        
        if results:
            avg_time = sum(r['processing_time'] for r in results) / len(results)
            report += f"- Average processing time: {avg_time:.2f} seconds\n\n"
        
        report += "## Detailed Results\n"
        for i, result in enumerate(results, 1):
            status = "SUCCESS" if result["success"] else "FAILED"
            report += f"### {i}. {result['input_path']}\n"
            report += f"- Status: {status}\n"
            report += f"- Output: {result['output_path']}\n"
            report += f"- Processing time: {result['processing_time']:.2f} seconds\n"
            if result["error"]:
                report += f"- Error: {result['error']}\n"
            report += "\n"
        
        return report
    
    def run(self):
        """Run the batch processor."""
        start_time = time.time()
        
        if self.monitor:
            print("Performance monitoring enabled")
        
        # Process files
        if self.parallel:
            results = self.process_parallel()
        else:
            results = self.process_sequential()
        
        end_time = time.time()
        self.stats["total_time"] = end_time - start_time
        
        # Generate report
        report = self.generate_report(results)
        
        # Save report
        report_path = self.output_dir / "processing_report.md"
        with open(report_path, 'w') as f:
            f.write(report)
        
        # Print summary
        print(f"\nProcessing complete!")
        print(f"Total files: {self.stats['total_files']}")
        print(f"Successful: {self.stats['successful']}")
        print(f"Failed: {self.stats['failed']}")
        print(f"Total time: {self.stats['total_time']:.2f} seconds")
        print(f"Report saved to: {report_path}")
        
        return len(results)

def main():
    args = parse_args()
    
    processor = BrainBatchProcessor(args)
    result_count = processor.run()
    
    return 0 if result_count > 0 else 1

if __name__ == "__main__":
    sys.exit(main())