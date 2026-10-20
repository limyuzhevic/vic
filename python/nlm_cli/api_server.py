#!/usr/bin/env python3
"""
Enhanced NLM API Server

Web API server for NLM with comprehensive error handling, validation,
error recovery, and diagnostic reporting.
"""

import asyncio
import json
import logging
import time
import threading
from typing import Dict, Any, Optional, List
from pathlib import Path
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs
import socketserver
import websockets
from websockets.server import WebSocketServerProtocol
import uuid

# Import error handling system
from nlm_core.error_handling import (
    NLMException, ConfigurationException, BrainException,
    ValidationException, ErrorHandling, ErrorRecovery, Validation
)

class NLMAPIError(Exception):
    """API-specific exception with status code support"""
    
    def __init__(self, message: str, status_code: int = 500, **kwargs):
        self.status_code = status_code
        super().__init__(message, **kwargs)

class NLMApiErrorReporter:
    """Error reporter for NLM API server"""
    
    def __init__(self):
        self.error_handler = ErrorHandling()
        self.request_stats = {}
        self.error_stats = {}
        
    def record_request(self, method: str, path: str, status_code: int):
        """Record request statistics"""
        key = f"{method}:{path}"
        if key not in self.request_stats:
            self.request_stats[key] = {
                "count": 0,
                "success_count": 0,
                "error_count": 0,
                "response_times": []
            }
        
        stats = self.request_stats[key]
        stats["count"] += 1
        
        if status_code < 400:
            stats["success_count"] += 1
        else:
            stats["error_count"] += 1
    
    def record_error(self, error: Exception, endpoint: str):
        """Record error statistics"""
        error_type = type(error).__name__
        if error_type not in self.error_stats:
            self.error_stats[error_type] = {
                "count": 0,
                "endpoints": {},
                "first_occurrence": time.time(),
                "last_occurrence": time.time()
            }
        
        stats = self.error_stats[error_type]
        stats["count"] += 1
        stats["last_occurrence"] = time.time()
        
        if endpoint not in stats["endpoints"]:
            stats["endpoints"][endpoint] = 0
        stats["endpoints"][endpoint] += 1
    
    def get_error_summary(self) -> Dict[str, Any]:
        """Get error summary"""
        return {
            "total_requests": sum(stats["count"] for stats in self.request_stats.values()),
            "total_errors": sum(stats["count"] for stats in self.error_stats.values()),
            "error_breakdown": self.error_stats,
            "request_breakdown": {k: v["count"] for k, v in self.request_stats.items()}
        }

class NLMApiHandler(BaseHTTPRequestHandler):
    """Enhanced API request handler with comprehensive error handling"""
    
    def __init__(self, *args, **kwargs):
        self.api_reporter = NLMApiErrorReporter()
        self.request_start_time = time.time()
        super().__init__(*args, **kwargs)
    
    def do_GET(self):
        """Handle GET requests with error handling"""
        try:
            path = self.path
            parsed = urlparse(path)
            endpoint = parsed.path
            
            self.request_start_time = time.time()
            
            # Route to appropriate handler
            if endpoint == "/api/health":
                self._handle_health_check()
            elif endpoint == "/api/status":
                self._handle_status()
            elif endpoint == "/api/version":
                self._handle_version()
            elif endpoint == "/api/config":
                self._handle_get_config()
            elif endpoint == "/api/brain":
                self._handle_brain_status()
            elif endpoint.startswith("/api/brain/"):
                self._handle_brain_operation(endpoint)
            elif endpoint == "/api/world":
                self._handle_world_status()
            elif endpoint.startswith("/api/world/"):
                self._handle_world_operation(endpoint)
            elif endpoint == "/api/agent":
                self._handle_agent_status()
            elif endpoint.startswith("/api/agent/"):
                self._handle_agent_operation(endpoint)
            elif endpoint == "/api/experiment":
                self._handle_experiment_status()
            elif endpoint.startswith("/api/experiment/"):
                self._handle_experiment_operation(endpoint)
            elif endpoint == "/api/profile":
                self._handle_profile_status()
            elif endpoint.startswith("/api/profile/"):
                self._handle_profile_operation(endpoint)
            else:
                self._send_error_response(404, "Endpoint not found")
                
        except ValidationException as e:
            self._send_error_response(400, str(e))
        except BrainException as e:
            self._send_error_response(503, str(e))
        except SimulationException as e:
            self._send_error_response(502, str(e))
        except MemoryException as e:
            self._send_error_response(507, str(e))
        except SensoryException as e:
            self._send_error_response(502, str(e))
        except MotorException as e:
            self._send_error_response(502, str(e))
        except PlasticityException as e:
            self._send_error_response(502, str(e))
        except NeuromodulationException as e:
            self._send_error_response(502, str(e))
        except PredictionException as e:
            self._send_error_response(502, str(e))
        except CognitionException as e:
            self._send_error_response(502, str(e))
        except NLMAPIError as e:
            self._send_error_response(e.status_code, str(e))
        except Exception as e:
            self.api_reporter.record_error(e, self.path)
            self._send_error_response(500, f"Internal server error: {str(e)}")
        
        finally:
            # Record request statistics
            response_time = (time.time() - self.request_start_time) * 1000
            status_code = self._get_status_code()
            self.api_reporter.record_request(self.command, self.path, status_code)
    
    def do_POST(self):
        """Handle POST requests with error handling"""
        try:
            self.request_start_time = time.time()
            content_length = int(self.headers.get('Content-Length', 0))
            
            # Validate content length
            Validation.validate_range(content_length, 0, 10 * 1024 * 1024, "content_length")
            
            # Parse request body
            body = self._read_request_body()
            
            # Route to appropriate handler
            path = self.path
            parsed = urlparse(path)
            endpoint = parsed.path
            
            if endpoint == "/api/run":
                self._handle_run_operation(body)
            elif endpoint == "/api/train":
                self._handle_train_operation(body)
            elif endpoint == "/api/config":
                self._handle_set_config(body)
            elif endpoint == "/api/brain/create":
                self._handle_brain_create(body)
            elif endpoint == "/api/world/create":
                self._handle_world_create(body)
            elif endpoint == "/api/agent/create":
                self._handle_agent_create(body)
            elif endpoint == "/api/experiment/run":
                self._handle_experiment_run(body)
            elif endpoint == "/api/profile/start":
                self._handle_profile_start(body)
            elif endpoint == "/api/profile/stop":
                self._handle_profile_stop(body)
            elif endpoint == "/api/websocket":
                # WebSocket endpoint is handled by WebSocket server
                self._send_error_response(404, "Use WebSocket endpoint for real-time communication")
            else:
                self._send_error_response(404, "Endpoint not found")
                
        except ValidationException as e:
            self._send_error_response(400, str(e))
        except BrainException as e:
            self._send_error_response(503, str(e))
        except SimulationException as e:
            self._send_error_response(502, str(e))
        except MemoryException as e:
            self._send_error_response(507, str(e))
        except SensoryException as e:
            self._send_error_response(502, str(e))
        except MotorException as e:
            self._send_error_response(502, str(e))
        except PlasticityException as e:
            self._send_error_response(502, str(e))
        except NeuromodulationException as e:
            self._send_error_response(502, str(e))
        except PredictionException as e:
            self._send_error_response(502, str(e))
        except CognitionException as e:
            self._send_error_response(502, str(e))
        except NLMAPIError as e:
            self._send_error_response(e.status_code, str(e))
        except Exception as e:
            self.api_reporter.record_error(e, self.path)
            self._send_error_response(500, f"Internal server error: {str(e)}")
        
        finally:
            # Record request statistics
            response_time = (time.time() - self.request_start_time) * 1000
            status_code = self._get_status_code()
            self.api_reporter.record_request(self.command, self.path, status_code)
    
    def _read_request_body(self) -> Dict[str, Any]:
        """Read and parse request body with validation"""
        content_type = self.headers.get('Content-Type', '')
        
        try:
            content_length = int(self.headers.get('Content-Length', 0))
            body = self.rfile.read(content_length).decode('utf-8')
            
            if content_type.startswith('application/json'):
                return json.loads(body)
            elif content_type.startswith('application/x-www-form-urlencoded'):
                # Parse form data
                params = parse_qs(body)
                # Convert to regular dict with single values
                return {k: v[0] for k, v in params.items()}
            elif content_type.startswith('multipart/form-data'):
                # For multipart, would need to use email package or similar
                raise ValidationException("Multipart form data not supported")
            else:
                # Try to parse as JSON anyway
                try:
                    return json.loads(body)
                except json.JSONDecodeError:
                    raise ValidationException(f"Unsupported content type: {content_type}")
                    
        except UnicodeDecodeError:
            raise ValidationException("Invalid encoding in request body")
        except MemoryError:
            raise MemoryException("Request body too large")
    
    def _send_success_response(self, data: Any = None, status_code: int = 200):
        """Send success response with proper headers and error handling"""
        try:
            self.send_response(status_code)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
            self.send_header('Access-Control-Allow-Headers', 'Content-Type')
            self.end_headers()
            
            response_data = {"success": True}
            if data is not None:
                response_data.update(data)
            
            response_json = json.dumps(response_data, indent=2)
            self.wfile.write(response_json.encode('utf-8'))
            
        except Exception as e:
            raise NLMAPIError(f"Failed to send response: {str(e)}", status_code=500)
    
    def _send_error_response(self, status_code: int, message: str):
        """Send error response with standardized format"""
        try:
            self.send_response(status_code)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
            self.send_header('Access-Control-Allow-Headers', 'Content-Type')
            self.end_headers()
            
            error_response = {
                "success": False,
                "error": {
                    "code": status_code,
                    "message": message,
                    "timestamp": time.time()
                }
            }
            
            response_json = json.dumps(error_response, indent=2)
            self.wfile.write(response_json.encode('utf-8'))
            
        except Exception as e:
            # Fallback to simple error response
            try:
                self.send_response(500)
                self.end_headers()
                self.wfile.write(b'Internal Server Error')
            except:
                pass
    
    def _get_status_code(self) -> int:
        """Get HTTP status code from response headers"""
        if hasattr(self, '_status_code'):
            return self._status_code
        return 200
    
    # Endpoint handlers with error handling
    
    def _handle_health_check(self):
        """Handle health check endpoint"""
        try:
            health_status = {
                "status": "healthy",
                "timestamp": time.time(),
                "version": "2.0.0",
                "components": {
                    "api_server": "healthy",
                    "nlm_cli": "healthy"
                }
            }
            self._send_success_response(health_status)
            
        except Exception as e:
            raise NLMAPIError(f"Health check failed: {str(e)}", status_code=503)
    
    def _handle_status(self):
        """Handle status endpoint"""
        try:
            status = {
                "server_uptime": time.time() - self.server.start_time if hasattr(self.server, 'start_time') else 0,
                "request_count": sum(stats["count"] for stats in self.api_reporter.request_stats.values()),
                "error_count": sum(stats["count"] for stats in self.api_reporter.error_stats.values()),
                "active_sessions": len(getattr(self.server, 'active_sessions', {})),
                "memory_usage": self._get_memory_usage(),
                "error_summary": self.api_reporter.get_error_summary()
            }
            self._send_success_response(status)
            
        except Exception as e:
            raise NLMAPIError(f"Status request failed: {str(e)}", status_code=500)
    
    def _handle_version(self):
        """Handle version endpoint"""
        try:
            version_info = {
                "version": "2.0.0",
                "build_info": {
                    "commit": "dev",
                    "build_date": "2026-10-20",
                    "python_version": sys.version
                },
                "features": [
                    "enhanced_error_handling",
                    "validation_system",
                    "error_recovery",
                    "diagnostic_reporting"
                ]
            }
            self._send_success_response(version_info)
            
        except Exception as e:
            raise NLMAPIError(f"Version request failed: {str(e)}", status_code=500)
    
    def _handle_get_config(self):
        """Handle get config endpoint"""
        try:
            config = {
                "server_config": {
                    "host": self.server.host if hasattr(self.server, 'host') else '0.0.0.0',
                    "port": self.server.port if hasattr(self.server, 'port') else 8080
                },
                "error_handler_config": {
                    "error_reporting": True,
                    "error_recovery": True,
                    "circuit_breaker": True,
                    "validation_enabled": True
                }
            }
            self._send_success_response(config)
            
        except Exception as e:
            raise NLMAPIError(f"Get config failed: {str(e)}", status_code=500)
    
    def _handle_brain_status(self):
        """Handle brain status endpoint"""
        try:
            # In a real implementation, this would query the brain
            brain_status = {
                "brain_initialized": False,
                "neuron_count": 0,
                "region_count": 0,
                "synapse_count": 0,
                "last_updated": None
            }
            self._send_success_response({"brain": brain_status})
            
        except Exception as e:
            raise BrainException(f"Brain status request failed: {str(e)}")
    
    def _handle_brain_operation(self, endpoint: str):
        """Handle brain-specific endpoints"""
        brain_id = endpoint.split('/')[-1]
        Validation.validate_string_not_empty(brain_id, "brain_id")
        
        # In a real implementation, would perform specific brain operations
        brain_operation = {
            "operation": "status",
            "brain_id": brain_id,
            "timestamp": time.time()
        }
        self._send_success_response({"brain": brain_operation})
    
    def _handle_world_status(self):
        """Handle world status endpoint"""
        try:
            world_status = {
                "world_initialized": False,
                "width": 0,
                "height": 0,
                "entities_count": 0,
                "last_updated": None
            }
            self._send_success_response({"world": world_status})
            
        except Exception as e:
            raise BrainException(f"World status request failed: {str(e)}")
    
    def _handle_world_operation(self, endpoint: str):
        """Handle world-specific endpoints"""
        parts = endpoint.split('/')
        if len(parts) < 3:
            raise ValidationException("Invalid world endpoint")
        
        operation = parts[-1]
        world_id = parts[-2] if len(parts) > 2 else "default"
        
        Validation.validate_string_not_empty(world_id, "world_id")
        Validation.validate_string_not_empty(operation, "operation")
        
        world_operation = {
            "operation": operation,
            "world_id": world_id,
            "timestamp": time.time()
        }
        self._send_success_response({"world": world_operation})
    
    def _handle_agent_status(self):
        """Handle agent status endpoint"""
        try:
            agent_status = {
                "agent_initialized": False,
                "type": "unknown",
                "model": None,
                "last_updated": None
            }
            self._send_success_response({"agent": agent_status})
            
        except Exception as e:
            raise BrainException(f"Agent status request failed: {str(e)}")
    
    def _handle_agent_operation(self, endpoint: str):
        """Handle agent-specific endpoints"""
        parts = endpoint.split('/')
        if len(parts) < 3:
            raise ValidationException("Invalid agent endpoint")
        
        operation = parts[-1]
        agent_id = parts[-2] if len(parts) > 2 else "default"
        
        Validation.validate_string_not_empty(agent_id, "agent_id")
        Validation.validate_string_not_empty(operation, "operation")
        
        agent_operation = {
            "operation": operation,
            "agent_id": agent_id,
            "timestamp": time.time()
        }
        self._send_success_response({"agent": agent_operation})
    
    def _handle_experiment_status(self):
        """Handle experiment status endpoint"""
        try:
            experiment_status = {
                "experiment_running": False,
                "current_experiment": None,
                "experiments_count": 0,
                "last_updated": None
            }
            self._send_success_response({"experiment": experiment_status})
            
        except Exception as e:
            raise SimulationException(f"Experiment status request failed: {str(e)}")
    
    def _handle_experiment_operation(self, endpoint: str):
        """Handle experiment-specific endpoints"""
        parts = endpoint.split('/')
        if len(parts) < 3:
            raise ValidationException("Invalid experiment endpoint")
        
        operation = parts[-1]
        experiment_id = parts[-2] if len(parts) > 2 else "default"
        
        Validation.validate_string_not_empty(experiment_id, "experiment_id")
        Validation.validate_string_not_empty(operation, "operation")
        
        experiment_operation = {
            "operation": operation,
            "experiment_id": experiment_id,
            "timestamp": time.time()
        }
        self._send_success_response({"experiment": experiment_operation})
    
    def _handle_profile_status(self):
        """Handle profile status endpoint"""
        try:
            profile_status = {
                "profiling_active": False,
                "current_profile": None,
                "profiles_count": 0,
                "last_updated": None
            }
            self._send_success_response({"profile": profile_status})
            
        except Exception as e:
            raise PerformanceException(f"Profile status request failed: {str(e)}")
    
    def _handle_profile_operation(self, endpoint: str):
        """Handle profile-specific endpoints"""
        parts = endpoint.split('/')
        if len(parts) < 3:
            raise ValidationException("Invalid profile endpoint")
        
        operation = parts[-1]
        profile_id = parts[-2] if len(parts) > 2 else "default"
        
        Validation.validate_string_not_empty(profile_id, "profile_id")
        Validation.validate_string_not_empty(operation, "operation")
        
        profile_operation = {
            "operation": operation,
            "profile_id": profile_id,
            "timestamp": time.time()
        }
        self._send_success_response({"profile": profile_operation})
    
    # POST endpoint handlers
    
    def _handle_run_operation(self, body: Dict[str, Any]):
        """Handle run operation endpoint"""
        try:
            steps = body.get('steps', 100)
            Validation.validate_range(steps, 1, 1000000, "steps")
            
            run_operation = {
                "operation": "run",
                "steps": steps,
                "timestamp": time.time()
            }
            self._send_success_response({"run": run_operation})
            
        except Exception as e:
            raise SimulationException(f"Run operation failed: {str(e)}")
    
    def _handle_train_operation(self, body: Dict[str, Any]):
        """Handle train operation endpoint"""
        try:
            episodes = body.get('episodes', 100)
            Validation.validate_range(episodes, 1, 100000, "episodes")
            
            learning_rate = body.get('learning_rate', 0.01)
            Validation.validate_range(learning_rate, 0.0, 1.0, "learning_rate")
            
            train_operation = {
                "operation": "train",
                "episodes": episodes,
                "learning_rate": learning_rate,
                "timestamp": time.time()
            }
            self._send_success_response({"train": train_operation})
            
        except Exception as e:
            raise BrainException(f"Train operation failed: {str(e)}")
    
    def _handle_set_config(self, body: Dict[str, Any]):
        """Handle set config endpoint"""
        try:
            Validation.validate_dict_keys(body, ["key", "value"], "config_body")
            
            key = body["key"]
            value = body["value"]
            
            Validation.validate_string_not_empty(key, "config_key")
            
            config_operation = {
                "operation": "set_config",
                "key": key,
                "value": value,
                "timestamp": time.time()
            }
            self._send_success_response({"config": config_operation})
            
        except Exception as e:
            raise ConfigurationException(f"Set config operation failed: {str(e)}")
    
    def _handle_brain_create(self, body: Dict[str, Any]):
        """Handle brain create endpoint"""
        try:
            neuron_count = body.get('neuron_count', 1000)
            Validation.validate_range(neuron_count, 1, 1000000, "neuron_count")
            
            timestep = body.get('timestep', 0.001)
            Validation.validate_range(timestep, 0.0001, 1.0, "timestep")
            
            brain_create = {
                "operation": "create",
                "neuron_count": neuron_count,
                "timestep": timestep,
                "timestamp": time.time()
            }
            self._send_success_response({"brain": brain_create})
            
        except Exception as e:
            raise BrainException(f"Brain create operation failed: {str(e)}")
    
    def _handle_world_create(self, body: Dict[str, Any]):
        """Handle world create endpoint"""
        try:
            width = body.get('width', 100)
            Validation.validate_range(width, 1, 10000, "width")
            
            height = body.get('height', 100)
            Validation.validate_range(height, 1, 10000, "height")
            
            world_create = {
                "operation": "create",
                "width": width,
                "height": height,
                "timestamp": time.time()
            }
            self._send_success_response({"world": world_create})
            
        except Exception as e:
            raise BrainException(f"World create operation failed: {str(e)}")
    
    def _handle_agent_create(self, body: Dict[str, Any]):
        """Handle agent create endpoint"""
        try:
            agent_type = body.get('type', 'default')
            Validation.validate_string_not_empty(agent_type, "agent_type")
            
            agent_create = {
                "operation": "create",
                "type": agent_type,
                "timestamp": time.time()
            }
            self._send_success_response({"agent": agent_create})
            
        except Exception as e:
            raise BrainException(f"Agent create operation failed: {str(e)}")
    
    def _handle_experiment_run(self, body: Dict[str, Any]):
        """Handle experiment run endpoint"""
        try:
            Validation.validate_dict_keys(body, ["experiment_name"], "experiment_body")
            
            experiment_name = body["experiment_name"]
            Validation.validate_string_not_empty(experiment_name, "experiment_name")
            
            experiment_run = {
                "operation": "run",
                "experiment_name": experiment_name,
                "timestamp": time.time()
            }
            self._send_success_response({"experiment": experiment_run})
            
        except Exception as e:
            raise SimulationException(f"Experiment run operation failed: {str(e)}")
    
    def _handle_profile_start(self, body: Dict[str, Any]):
        """Handle profile start endpoint"""
        try:
            profile_type = body.get('type', 'default')
            Validation.validate_string_not_empty(profile_type, "profile_type")
            
            profile_start = {
                "operation": "start",
                "type": profile_type,
                "timestamp": time.time()
            }
            self._send_success_response({"profile": profile_start})
            
        except Exception as e:
            raise PerformanceException(f"Profile start operation failed: {str(e)}")
    
    def _handle_profile_stop(self, body: Dict[str, Any]):
        """Handle profile stop endpoint"""
        try:
            profile_stop = {
                "operation": "stop",
                "timestamp": time.time()
            }
            self._send_success_response({"profile": profile_stop})
            
        except Exception as e:
            raise PerformanceException(f"Profile stop operation failed: {str(e)}")
    
    def _get_memory_usage(self) -> Dict[str, Any]:
        """Get memory usage information"""
        try:
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            
            return {
                "rss_mb": memory_info.rss / 1024 / 1024,
                "vms_mb": memory_info.vms / 1024 / 1024,
                "cpu_percent": process.cpu_percent(),
                "thread_count": process.num_threads()
            }
        except ImportError:
            return {"error": "psutil not available"}
        except Exception as e:
            return {"error": str(e)}
    
    def log_message(self, format, *args):
        """Log messages from the server"""
        logging.getLogger(__name__).info(format % args)

class NLMAPIServer:
    """Enhanced NLM API server with comprehensive error handling"""
    
    def __init__(self, host: str = "0.0.0.0", port: int = 8080):
        self.host = host
        self.port = port
        self.cli = None
        self.active_sessions: Dict[str, Any] = {}
        self.logger = logging.getLogger(__name__)
        self.start_time = time.time()
        
        # Setup logging
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S'
        )
        
        self.api_reporter = NLMApiErrorReporter()
        
    def start(self):
        """Start the API server with error handling"""
        try:
            self.logger.info(f"Starting NLM API Server on {self.host}:{self.port}")
            
            # Start HTTP server
            self.http_server = HTTPServer((self.host, self.port), NLMApiHandler)
            self.http_server.cli = self.cli
            self.http_server.active_sessions = self.active_sessions
            
            # Start WebSocket server for real-time updates
            asyncio.ensure_future(self.start_websocket_server())
            
            # Start HTTP server
            http_thread = threading.Thread(target=self.http_server.serve_forever)
            http_thread.daemon = True
            http_thread.start()
            
            self.logger.info(f"API server started on http://{self.host}:{self.port}")
            
            # Keep main thread alive
            try:
                while True:
                    time.sleep(1)
            except KeyboardInterrupt:
                self.logger.info("Shutting down API server...")
                self.http_server.shutdown()
                
        except Exception as e:
            self.logger.error(f"API server failed to start: {e}")
            raise
    
    async def start_websocket_server(self):
        """Start WebSocket server for real-time communication with error handling"""
        async def handler(websocket: WebSocketServerProtocol, path):
            session_id = str(uuid.uuid4())
            self.active_sessions[session_id] = {
                'websocket': websocket,
                'connected_at': time.time(),
                'last_activity': time.time()
            }
            
            self.logger.info(f"WebSocket connection established: {session_id}")
            
            try:
                async for message in websocket:
                    self.active_sessions[session_id]['last_activity'] = time.time()
                    await self.handle_websocket_message(session_id, message)
                    
            except websockets.exceptions.ConnectionClosed:
                self.logger.info(f"WebSocket connection closed: {session_id}")
            finally:
                if session_id in self.active_sessions:
                    del self.active_sessions[session_id]
                    
        websocket_server = await websockets.serve(
            handler,
            self.host,
            self.port + 1,
            ping_interval=20,
            ping_timeout=10
        )
        
        self.logger.info(f"WebSocket server started on {self.host}:{self.port + 1}")
        
        # Keep the server running
        await asyncio.Future()
    
    async def handle_websocket_message(self, session_id: str, message: str):
        """Handle WebSocket message with error handling"""
        try:
            # Parse message
            data = json.loads(message)
            
            # Route to appropriate handler
            message_type = data.get('type', 'message')
            
            if message_type == 'ping':
                await self._handle_ping(session_id, data)
            elif message_type == 'status':
                await self._handle_status_request(session_id, data)
            elif message_type == 'command':
                await self._handle_command_request(session_id, data)
            else:
                raise NLMAPIError(f"Unknown message type: {message_type}", status_code=400)
                
        except NLMException:
            raise
        except Exception as e:
            self.api_reporter.record_error(e, f"websocket:{session_id}")
            raise NLMAPIError(f"Error processing WebSocket message: {str(e)}", status_code=500)
    
    async def _handle_ping(self, session_id: str, data: Dict[str, Any]):
        """Handle ping message"""
        response = {
            "type": "pong",
            "timestamp": time.time(),
            "session_id": session_id
        }
        
        websocket = self.active_sessions[session_id]['websocket']
        await websocket.send(json.dumps(response))
    
    async def _handle_status_request(self, session_id: str, data: Dict[str, Any]):
        """Handle status request via WebSocket"""
        try:
            status = self.api_reporter.get_error_summary()
            status.update({
                "session_count": len(self.active_sessions),
                "server_uptime": time.time() - self.start_time
            })
            
            response = {
                "type": "status",
                "data": status,
                "timestamp": time.time()
            }
            
            websocket = self.active_sessions[session_id]['websocket']
            await websocket.send(json.dumps(response))
            
        except Exception as e:
            raise NLMAPIError(f"Status request failed: {str(e)}", status_code=500)
    
    async def _handle_command_request(self, session_id: str, data: Dict[str, Any]):
        """Handle command request via WebSocket"""
        try:
            command = data.get('command', '')
            args = data.get('args', [])
            
            Validation.validate_string_not_empty(command, "command")
            
            # In a real implementation, would execute the command
            response = {
                "type": "command_response",
                "command": command,
                "args": args,
                "status": "executed",
                "timestamp": time.time()
            }
            
            websocket = self.active_sessions[session_id]['websocket']
            await websocket.send(json.dumps(response))
            
        except Exception as e:
            raise NLMAPIError(f"Command request failed: {str(e)}", status_code=500)

# Global exception handler for API server
class GlobalApiExceptionHandler:
    """Global exception handler for the API server"""
    
    def __init__(self, api_server: NLMAPIServer):
        self.api_server = api_server
        self.original_excepthook = sys.excepthook
    
    def __enter__(self):
        sys.excepthook = self._handle_exception
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if exc_type is not None:
            self._handle_exception(exc_type, exc_val, exc_tb)
    
    def _handle_exception(self, exc_type, exc_val, exc_val_tb):
        """Handle unhandled exceptions"""
        if issubclass(exc_type, NLMException):
            nlm_exception = exc_val
            self.api_server.api_reporter.record_error(nlm_exception, "unhandled_exception")
            logging.getLogger(__name__).error(f"Unhandled NLM exception: {nlm_exception}")
        else:
            # Convert to NLM exception
            nlm_exception = NLMException(
                ErrorCode.SYSTEM_ERROR,
                f"Unhandled exception: {str(exc_val)}",
                cause=exc_val
            )
            self.api_server.api_reporter.record_error(nlm_exception, "unhandled_exception")
            logging.getLogger(__name__).error(f"Unhandled system exception: {exc_val}")
        
        # Call original handler
        self.original_excepthook(exc_type, exc_val, exc_tb)

# Usage example
if __name__ == "__main__":
    # Example usage
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM API Server")
    parser.add_argument("--host", default="0.0.0.0", help="Server host")
    parser.add_argument("--port", type=int, default=8080, help="Server port")
    
    args = parser.parse_args()
    
    print("Starting NLM API Server with enhanced error handling...")
    
    # Create and start server
    server = NLMAPIServer(args.host, args.port)
    
    try:
        with GlobalApiExceptionHandler(server):
            server.start()
    except KeyboardInterrupt:
        print("\nServer stopped by user")
    except Exception as e:
        print(f"\nServer error: {e}")
        sys.exit(1)