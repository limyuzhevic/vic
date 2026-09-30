# Plugin Marketplace

This module implements the plugin marketplace, including repository, rating system, certification system, and metadata system.

import os
import json
import time
import hashlib
import secrets
import shutil
import requests
import subprocess
from typing import Dict, List, Any, Optional, Callable, Type, Set, Union
from dataclasses import dataclass, field
from enum import Enum
from abc import ABC, abstractmethod

from .plugin_system import Plugin, PluginInfo, PluginType, PluginStatus
from .core_plugin_framework import EnhancedPluginManager


class MarketplaceError(Exception):
    """Exception raised during marketplace operations"""
    pass


class PluginRating(Enum):
    """Plugin rating"""
    EXCELLENT = 5
    GOOD = 4
    AVERAGE = 3
    POOR = 2
    TERRIBLE = 1


class CertificationLevel(Enum):
    """Plugin certification level"""
    COMMUNITY = "community"
    VERIFIED = "verified"
    TRUSTED = "trusted"
    OFFICIAL = "official"


@dataclass
class PluginReview:
    """Plugin review"""
    reviewer_id: str
    rating: int
    comment: str
    created_at: float
    updated_at: float
    helpful_votes: int
    not_helpful_votes: int
    verified_purchase: bool


@dataclass
class PluginRatingSummary:
    """Plugin rating summary"""
    plugin_id: str
    average_rating: float
    total_reviews: int
    rating_distribution: Dict[int, int]
    recent_reviews: List[PluginReview]
    helpfulness_score: float
    last_updated: float


@dataclass
class PluginCertification:
    """Plugin certification"""
    plugin_id: str
    certification_level: CertificationLevel
    certified_by: str
    certified_at: float
    expiry_date: Optional[float]
    requirements_met: List[str]
    security_audit_passed: bool
    performance_validation: bool
    documentation_quality: bool
    code_quality: bool
    conditions: List[str]


@dataclass
class PluginMetadata:
    """Plugin metadata"""
    plugin_id: str
    title: str
    description: str
    long_description: str
    version: str
    author: str
    author_id: str
    homepage: str
    repository: str
    documentation: str
    license: str
    categories: List[str]
    tags: List[str]
    keywords: List[str]
    plugin_type: str
    targets: List[str]
    dependencies: Dict[str, str]
    conflicts: List[str]
    requirements: Dict[str, Any]
    screenshots: List[str]
    videos: List[str]
    ratings: PluginRatingSummary
    certification: PluginCertification
    downloads: int
    install_count: int
    active_users: int
    last_updated: float
    created_at: float
    status: str
    quality_score: float
    security_score: float
    performance_score: float


class PluginMarketplace:
    """Plugin marketplace"""
    
    def __init__(self, config: Dict[str, Any] = None):
        self.config = config or {}
        self.registry = PluginRegistry()
        self.distribution = DistributionService()
        self.curation = CurationSystem()
        self.indexer = PluginIndexer()
        self.recommender = PluginRecommender()
        
    def register_plugin(self, plugin_bundle: Dict[str, Any]) -> str:
        """Register plugin in marketplace"""
        # Validate plugin bundle
        self._validate_plugin_bundle(plugin_bundle)
        
        # Extract plugin metadata
        metadata = self._extract_plugin_metadata(plugin_bundle)
        
        # Create plugin ID
        plugin_id = self._generate_plugin_id(plugin_bundle)
        metadata.plugin_id = plugin_id
        
        # Register in registry
        self.registry.register(metadata)
        
        # Store plugin bundle
        self.distribution.store_plugin_bundle(plugin_id, plugin_bundle)
        
        # Index plugin
        self.indexer.index_plugin(metadata)
        
        return plugin_id
        
    def search_plugins(self, query: str = None, filters: Dict[str, Any] = None,
                      sort_by: str = 'relevance', page: int = 1,
                      page_size: int = 20) -> Dict[str, Any]:
        """Search plugins"""
        # Search in registry
        results = self.registry.search(query, filters)
        
        # Apply sorting
        results = self._sort_plugins(results, sort_by)
        
        # Apply pagination
        total_results = len(results)
        start = (page - 1) * page_size
        end = start + page_size
        results = results[start:end]
        
        # Get full plugin metadata
        plugin_metadata_list = []
        for plugin_id in results:
            metadata = self.registry.get_plugin_metadata(plugin_id)
            if metadata:
                plugin_metadata_list.append(metadata)
        
        return {
            'plugins': plugin_metadata_list,
            'total_results': total_results,
            'page': page,
            'page_size': page_size,
            'has_more': end < total_results
        }
        
    def get_plugin_info(self, plugin_id: str) -> Optional[PluginMetadata]:
        """Get plugin information"""
        return self.registry.get_plugin_metadata(plugin_id)
        
    def download_plugin(self, plugin_id: str, output_path: str) -> bool:
        """Download plugin from marketplace"""
        # Get plugin bundle
        plugin_bundle = self.distribution.get_plugin_bundle(plugin_id)
        if not plugin_bundle:
            return False
        
        # Create output directory
        os.makedirs(output_path, exist_ok=True)
        
        # Save plugin bundle
        bundle_path = os.path.join(output_path, 'plugin_bundle.json')
        with open(bundle_path, 'w') as f:
            json.dump(plugin_bundle, f, indent=2)
        
        # Extract files if needed
        if 'files' in plugin_bundle:
            for file_info in plugin_bundle['files']:
                file_path = os.path.join(output_path, file_info['name'])
                if file_info['type'] == 'text':
                    with open(file_path, 'w') as f:
                        f.write(file_info['content'])
                else:
                    # Binary file
                    with open(file_path, 'wb') as f:
                        f.write(file_info['content'])
        
        # Update download count
        metadata = self.registry.get_plugin_metadata(plugin_id)
        if metadata:
            metadata.downloads += 1
            self.registry.update_metadata(metadata)
        
        return True
        
    def rate_plugin(self, plugin_id: str, user_id: str, rating: int,
                   comment: str = None, helpful: bool = None) -> bool:
        """Rate plugin"""
        # Validate rating
        if rating < 1 or rating > 5:
            raise MarketplaceError("Rating must be between 1 and 5")
        
        # Get plugin metadata
        metadata = self.registry.get_plugin_metadata(plugin_id)
        if not metadata:
            return False
        
        # Create review
        review = PluginReview(
            reviewer_id=user_id,
            rating=rating,
            comment=comment or '',
            created_at=time.time(),
            updated_at=time.time(),
            helpful_votes=0,
            not_helpful_votes=0,
            verified_purchase=False
        )
        
        # Add review to metadata
        if 'reviews' not in metadata.__dict__:
            metadata.reviews = []
        
        metadata.reviews.append(review)
        
        # Update rating summary
        self._update_rating_summary(metadata)
        
        # Update metadata
        self.registry.update_metadata(metadata)
        
        return True
        
    def get_plugin_reviews(self, plugin_id: str, page: int = 1,
                           page_size: int = 10) -> Dict[str, Any]:
        """Get plugin reviews"""
        metadata = self.registry.get_plugin_metadata(plugin_id)
        if not metadata or 'reviews' not in metadata.__dict__:
            return {
                'reviews': [],
                'total_reviews': 0,
                'page': page,
                'page_size': page_size,
                'has_more': False
            }
        
        reviews = metadata.reviews
        
        # Sort reviews by creation date (newest first)
        reviews.sort(key=lambda r: r.created_at, reverse=True)
        
        # Apply pagination
        total_reviews = len(reviews)
        start = (page - 1) * page_size
        end = start + page_size
        reviews = reviews[start:end]
        
        return {
            'reviews': reviews,
            'total_reviews': total_reviews,
            'page': page,
            'page_size': page_size,
            'has_more': end < total_reviews
        }
        
    def certify_plugin(self, plugin_id: str, certifier_id: str,
                      certification_level: CertificationLevel,
                      requirements: List[str] = None) -> bool:
        """Certify plugin"""
        # Get plugin metadata
        metadata = self.registry.get_plugin_metadata(plugin_id)
        if not metadata:
            return False
        
        # Create certification
        certification = PluginCertification(
            plugin_id=plugin_id,
            certification_level=certification_level,
            certified_by=certifier_id,
            certified_at=time.time(),
            expiry_date=time.time() + (365 * 24 * 60 * 60),  # 1 year
            requirements_met=requirements or [],
            security_audit_passed=True,
            performance_validation=True,
            documentation_quality=True,
            code_quality=True,
            conditions=[]
        )
        
        # Update metadata
        metadata.certification = certification
        metadata.status = 'certified'
        
        self.registry.update_metadata(metadata)
        
        return True
        
    def get_certified_plugins(self, certification_level: CertificationLevel = None) -> List[PluginMetadata]:
        """Get certified plugins"""
        # Get all plugins
        all_plugins = self.registry.get_all_plugins()
        
        # Filter by certification level
        if certification_level:
            return [
                p for p in all_plugins
                if p.certification and p.certification.certification_level == certification_level
            ]
        else:
            return [
                p for p in all_plugins
                if p.certification
            ]
        
    def get_trending_plugins(self, time_range: str = 'week') -> List[PluginMetadata]:
        """Get trending plugins"""
        # Get all plugins
        all_plugins = self.registry.get_all_plugins()
        
        # Calculate trending score based on downloads and reviews
        for plugin in all_plugins:
            # Calculate trending score
            download_score = min(plugin.downloads / 1000, 100)  # Normalize
            review_score = len(plugin.reviews) if hasattr(plugin, 'reviews') else 0
            time_score = self._get_time_score(plugin.last_updated, time_range)
            
            plugin.trending_score = download_score * 0.7 + review_score * 0.3 * time_score
        
        # Sort by trending score
        all_plugins.sort(key=lambda p: p.trending_score, reverse=True)
        
        return all_plugins[:10]  # Top 10 trending plugins
        
    def _validate_plugin_bundle(self, plugin_bundle: Dict[str, Any]):
        """Validate plugin bundle"""
        required_fields = ['metadata', 'files']
        for field in required_fields:
            if field not in plugin_bundle:
                raise MarketplaceError(f"Missing required field in plugin bundle: {field}")
        
        # Validate metadata
        metadata = plugin_bundle['metadata']
        required_metadata_fields = ['name', 'version', 'description', 'author', 'plugin_type']
        for field in required_metadata_fields:
            if field not in metadata:
                raise MarketplaceError(f"Missing required metadata field: {field}")
    
    def _extract_plugin_metadata(self, plugin_bundle: Dict[str, Any]) -> PluginMetadata:
        """Extract plugin metadata from bundle"""
        metadata = plugin_bundle['metadata']
        
        return PluginMetadata(
            plugin_id='',  # Will be set later
            title=metadata.get('name', ''),
            description=metadata.get('description', ''),
            long_description=metadata.get('long_description', metadata.get('description', '')),
            version=metadata.get('version', '1.0.0'),
            author=metadata.get('author', ''),
            author_id=metadata.get('author_id', ''),
            homepage=metadata.get('homepage', ''),
            repository=metadata.get('repository', ''),
            documentation=metadata.get('documentation', ''),
            license=metadata.get('license', 'MIT'),
            categories=metadata.get('categories', []),
            tags=metadata.get('tags', []),
            keywords=metadata.get('keywords', []),
            plugin_type=metadata.get('plugin_type', ''),
            targets=metadata.get('targets', ['python']),
            dependencies=metadata.get('dependencies', {}),
            conflicts=metadata.get('conflicts', []),
            requirements=metadata.get('requirements', {}),
            screenshots=metadata.get('screenshots', []),
            videos=metadata.get('videos', []),
            ratings=PluginRatingSummary(
                plugin_id='',
                average_rating=0.0,
                total_reviews=0,
                rating_distribution={},
                recent_reviews=[],
                helpfulness_score=0.0,
                last_updated=time.time()
            ),
            certification=PluginCertification(
                plugin_id='',
                certification_level=CertificationLevel.COMMUNITY,
                certified_by='',
                certified_at=0,
                expiry_date=None,
                requirements_met=[],
                security_audit_passed=False,
                performance_validation=False,
                documentation_quality=False,
                code_quality=False,
                conditions=[]
            ),
            downloads=0,
            install_count=0,
            active_users=0,
            last_updated=time.time(),
            created_at=time.time(),
            status='pending',
            quality_score=0.0,
            security_score=0.0,
            performance_score=0.0
        )
    
    def _generate_plugin_id(self, plugin_bundle: Dict[str, Any]) -> str:
        """Generate plugin ID"""
        metadata = plugin_bundle['metadata']
        name = metadata.get('name', 'unknown')
        version = metadata.get('version', '1.0.0')
        
        # Create ID from name and version
        id_string = f"{name}_{version}"
        
        # Hash the ID to make it unique
        return hashlib.sha256(id_string.encode()).hexdigest()[:16]
    
    def _sort_plugins(self, plugin_ids: List[str], sort_by: str) -> List[str]:
        """Sort plugins by criteria"""
        if sort_by == 'relevance':
            # Sort by downloads and rating
            plugins_with_scores = []
            for plugin_id in plugin_ids:
                metadata = self.registry.get_plugin_metadata(plugin_id)
                if metadata:
                    score = metadata.downloads * 0.7 + metadata.average_rating * 20 * 0.3
                    plugins_with_scores.append((plugin_id, score))
            
            plugins_with_scores.sort(key=lambda x: x[1], reverse=True)
            return [p[0] for p in plugins_with_scores]
        
        elif sort_by == 'downloads':
            # Sort by downloads
            plugins_with_downloads = []
            for plugin_id in plugin_ids:
                metadata = self.registry.get_plugin_metadata(plugin_id)
                if metadata:
                    plugins_with_downloads.append((plugin_id, metadata.downloads))
            
            plugins_with_downloads.sort(key=lambda x: x[1], reverse=True)
            return [p[0] for p in plugins_with_downloads]
        
        elif sort_by == 'rating':
            # Sort by rating
            plugins_with_ratings = []
            for plugin_id in plugin_ids:
                metadata = self.registry.get_plugin_metadata(plugin_id)
                if metadata:
                    plugins_with_ratings.append((plugin_id, metadata.average_rating))
            
            plugins_with_ratings.sort(key=lambda x: x[1], reverse=True)
            return [p[0] for p in plugins_with_ratings]
        
        elif sort_by == 'newest':
            # Sort by last updated
            plugins_with_dates = []
            for plugin_id in plugin_ids:
                metadata = self.registry.get_plugin_metadata(plugin_id)
                if metadata:
                    plugins_with_dates.append((plugin_id, metadata.last_updated))
            
            plugins_with_dates.sort(key=lambda x: x[1], reverse=True)
            return [p[0] for p in plugins_with_dates]
        
        else:
            return plugin_ids
    
    def _update_rating_summary(self, metadata: PluginMetadata):
        """Update rating summary"""
        if not hasattr(metadata, 'reviews') or not metadata.reviews:
            return
        
        reviews = metadata.reviews
        
        # Calculate average rating
        total_rating = sum(r.rating for r in reviews)
        average_rating = total_rating / len(reviews)
        
        # Count ratings distribution
        distribution = {}
        for rating in range(1, 6):
            count = sum(1 for r in reviews if r.rating == rating)
            distribution[rating] = count
        
        # Calculate helpfulness score
        total_helpful = sum(r.helpful_votes for r in reviews)
        total_not_helpful = sum(r.not_helpful_votes for r in reviews)
        helpfulness_score = total_helpful / max(1, total_helpful + total_not_helpful)
        
        # Get recent reviews (last 10)
        recent_reviews = sorted(reviews, key=lambda r: r.created_at, reverse=True)[:10]
        
        # Update rating summary
        metadata.ratings = PluginRatingSummary(
            plugin_id=metadata.plugin_id,
            average_rating=average_rating,
            total_reviews=len(reviews),
            rating_distribution=distribution,
            recent_reviews=recent_reviews,
            helpfulness_score=helpfulness_score,
            last_updated=time.time()
        )
    
    def _get_time_score(self, last_updated: float, time_range: str) -> float:
        """Get time score based on last updated"""
        current_time = time.time()
        age_seconds = current_time - last_updated
        
        if time_range == 'day':
            # Weight: newer is better
            return min(1.0, 86400 / max(1, age_seconds))
        elif time_range == 'week':
            return min(1.0, 604800 / max(1, age_seconds))
        elif time_range == 'month':
            return min(1.0, 2592000 / max(1, age_seconds))
        else:
            return 1.0
class PluginRegistry:
    """Plugin registry"""
    
    def __init__(self):
        self.plugins: Dict[str, PluginMetadata] = {}
        
    def register(self, metadata: PluginMetadata):
        """Register plugin"""
        self.plugins[metadata.plugin_id] = metadata
        
    def get_plugin_metadata(self, plugin_id: str) -> Optional[PluginMetadata]:
        """Get plugin metadata"""
        return self.plugins.get(plugin_id)
        
    def update_metadata(self, metadata: PluginMetadata):
        """Update plugin metadata"""
        self.plugins[metadata.plugin_id] = metadata
        
    def search(self, query: str = None, filters: Dict[str, Any] = None) -> List[str]:
        """Search plugins"""
        if not query and not filters:
            return list(self.plugins.keys())
        
        results = []
        
        for plugin_id, metadata in self.plugins.items():
            # Check query
            if query:
                query_lower = query.lower()
                if (query_lower not in metadata.title.lower() and
                    query_lower not in metadata.description.lower() and
                    query_lower not in metadata.long_description.lower()):
                    continue
            
            # Check filters
            if filters:
                if 'categories' in filters and 'categories' in metadata.__dict__:
                    if not any(cat in filters['categories'] for cat in metadata.categories):
                        continue
                
                if 'tags' in filters and 'tags' in metadata.__dict__:
                    if not any(tag in filters['tags'] for tag in metadata.tags):
                        continue
                
                if 'plugin_type' in filters and 'plugin_type' in metadata.__dict__:
                    if metadata.plugin_type != filters['plugin_type']:
                        continue
                
                if 'author' in filters and 'author' in metadata.__dict__:
                    if metadata.author != filters['author']:
                        continue
                
                if 'min_rating' in filters and 'ratings' in metadata.__dict__:
                    if metadata.ratings.average_rating < filters['min_rating']:
                        continue
                
                if 'max_price' in filters:
                    # Check if plugin has price
                    if hasattr(metadata, 'price') and metadata.price > filters['max_price']:
                        continue
            
            results.append(plugin_id)
        
        return results
        
    def get_all_plugins(self) -> List[PluginMetadata]:
        """Get all plugins"""
        return list(self.plugins.values())
class DistributionService:
    """Plugin distribution service"""
    
    def __init__(self):
        self.plugin_bundles: Dict[str, Dict[str, Any]] = {}
        self.storage_dir = os.path.join(os.getcwd(), '.nlm_plugin_storage')
        os.makedirs(self.storage_dir, exist_ok=True)
        
    def store_plugin_bundle(self, plugin_id: str, plugin_bundle: Dict[str, Any]):
        """Store plugin bundle"""
        self.plugin_bundles[plugin_id] = plugin_bundle
        
        # Also store to disk
        bundle_path = os.path.join(self.storage_dir, f"{plugin_id}.json")
        with open(bundle_path, 'w') as f:
            json.dump(plugin_bundle, f, indent=2)
        
    def get_plugin_bundle(self, plugin_id: str) -> Optional[Dict[str, Any]]:
        """Get plugin bundle"""
        # Check in memory
        if plugin_id in self.plugin_bundles:
            return self.plugin_bundles[plugin_id]
        
        # Check on disk
        bundle_path = os.path.join(self.storage_dir, f"{plugin_id}.json")
        if os.path.exists(bundle_path):
            with open(bundle_path, 'r') as f:
                bundle = json.load(f)
            
            # Store in memory
            self.plugin_bundles[plugin_id] = bundle
            
            return bundle
        
        return None
class CurationSystem:
    """Plugin curation system"""
    
    def __init__(self):
        self.featured_plugins = []
        self.trending_plugins = []
        self.quality_plugins = []
        
    def add_featured_plugin(self, plugin_id: str):
        """Add featured plugin"""
        if plugin_id not in self.featured_plugins:
            self.featured_plugins.append(plugin_id)
    
    def remove_featured_plugin(self, plugin_id: str):
        """Remove featured plugin"""
        if plugin_id in self.featured_plugins:
            self.featured_plugins.remove(plugin_id)
    
    def add_trending_plugin(self, plugin_id: str):
        """Add trending plugin"""
        if plugin_id not in self.trending_plugins:
            self.trending_plugins.append(plugin_id)
    
    def remove_trending_plugin(self, plugin_id: str):
        """Remove trending plugin"""
        if plugin_id in self.trending_plugins:
            self.trending_plugins.remove(plugin_id)
    
    def add_quality_plugin(self, plugin_id: str, quality_score: float):
        """Add quality plugin"""
        self.quality_plugins.append((plugin_id, quality_score))
        
    def remove_quality_plugin(self, plugin_id: str):
        """Remove quality plugin"""
        self.quality_plugins = [(pid, score) for pid, score in self.quality_plugins if pid != plugin_id]
        
    def get_featured_plugins(self) -> List[str]:
        """Get featured plugins"""
        return self.featured_plugins
        
    def get_trending_plugins(self) -> List[str]:
        """Get trending plugins"""
        return self.trending_plugins
        
    def get_quality_plugins(self) -> List[str]:
        """Get quality plugins"""
        return [pid for pid, score in sorted(self.quality_plugins, key=lambda x: x[1], reverse=True)]
class PluginIndexer:
    """Plugin indexer"""
    
    def __init__(self):
        self.index = {}
        
    def index_plugin(self, metadata: PluginMetadata):
        """Index plugin"""
        # Index title
        self._index_text(metadata.title, 'title', metadata.plugin_id)
        
        # Index description
        self._index_text(metadata.description, 'description', metadata.plugin_id)
        
        # Index long description
        self._index_text(metadata.long_description, 'long_description', metadata.plugin_id)
        
        # Index tags
        for tag in metadata.tags:
            self._index_text(tag, 'tag', metadata.plugin_id)
        
        # Index keywords
        for keyword in metadata.keywords:
            self._index_text(keyword, 'keyword', metadata.plugin_id)
        
        # Index plugin type
        self._index_text(metadata.plugin_type, 'plugin_type', metadata.plugin_id)
        
        # Index categories
        for category in metadata.categories:
            self._index_text(category, 'category', metadata.plugin_id)
    
    def _index_text(self, text: str, field: str, plugin_id: str):
        """Index text in specific field"""
        if text:
            text_lower = text.lower()
            words = text_lower.split()
            
            for word in words:
                if word not in self.index:
                    self.index[word] = {}
                
                if field not in self.index[word]:
                    self.index[word][field] = set()
                
                self.index[word][field].add(plugin_id)
    
    def search(self, query: str) -> List[str]:
        """Search plugins by query"""
        query_lower = query.lower()
        words = query_lower.split()
        
        if not words:
            return list(self.index.values())[0].get('title', set())
        
        # Find matching words
        matching_plugin_ids = set()
        for word in words:
            if word in self.index:
                # Add all plugins that contain this word
                for field, plugin_ids in self.index[word].items():
                    matching_plugin_ids.update(plugin_ids)
        
        return list(matching_plugin_ids)
class PluginRecommender:
    """Plugin recommender"""
    
    def __init__(self, marketplace: PluginMarketplace):
        self.marketplace = marketplace
        
    def get_recommendations(self, user_id: str, limit: int = 10) -> List[PluginMetadata]:
        """Get plugin recommendations for user"""
        # In a real implementation, this would use user preferences, ratings, etc.
        # For now, return trending plugins
        return self.marketplace.get_trending_plugins()[:limit]
class RatingSystem:
    """Plugin rating system"""
    
    def __init__(self, marketplace: PluginMarketplace):
        self.marketplace = marketplace
        self.ratings: Dict[str, List[PluginReview]] = {}
        
    def add_rating(self, plugin_id: str, review: PluginReview):
        """Add plugin rating"""
        if plugin_id not in self.ratings:
            self.ratings[plugin_id] = []
        
        self.ratings[plugin_id].append(review)
        
    def get_rating_summary(self, plugin_id: str) -> PluginRatingSummary:
        """Get rating summary for plugin"""
        if plugin_id not in self.ratings:
            return None
        
        reviews = self.ratings[plugin_id]
        
        # Calculate average rating
        total_rating = sum(r.rating for r in reviews)
        average_rating = total_rating / len(reviews)
        
        # Count ratings distribution
        distribution = {}
        for rating in range(1, 6):
            count = sum(1 for r in reviews if r.rating == rating)
            distribution[rating] = count
        
        # Calculate helpfulness score
        total_helpful = sum(r.helpful_votes for r in reviews)
        total_not_helpful = sum(r.not_helpful_votes for r in reviews)
        helpfulness_score = total_helpful / max(1, total_helpful + total_not_helpful)
        
        # Get recent reviews (last 10)
        recent_reviews = sorted(reviews, key=lambda r: r.created_at, reverse=True)[:10]
        
        return PluginRatingSummary(
            plugin_id=plugin_id,
            average_rating=average_rating,
            total_reviews=len(reviews),
            rating_distribution=distribution,
            recent_reviews=recent_reviews,
            helpfulness_score=helpfulness_score,
            last_updated=time.time()
        )
class CertificationSystem:
    """Plugin certification system"""
    
    def __init__(self, marketplace: PluginMarketplace):
        self.marketplace = marketplace
        self.certifications: Dict[str, PluginCertification] = {}
        
    def certify_plugin(self, plugin_id: str, certifier_id: str,
                      certification_level: CertificationLevel,
                      requirements: List[str] = None) -> bool:
        """Certify plugin"""
        # Create certification
        certification = PluginCertification(
            plugin_id=plugin_id,
            certification_level=certification_level,
            certified_by=certifier_id,
            certified_at=time.time(),
            expiry_date=time.time() + (365 * 24 * 60 * 60),  # 1 year
            requirements_met=requirements or [],
            security_audit_passed=True,
            performance_validation=True,
            documentation_quality=True,
            code_quality=True,
            conditions=[]
        )
        
        # Store certification
        self.certifications[plugin_id] = certification
        
        return True
        
    def get_certification(self, plugin_id: str) -> Optional[PluginCertification]:
        """Get certification for plugin"""
        return self.certifications.get(plugin_id)
        
    def validate_plugin(self, plugin_id: str) -> Dict[str, Any]:
        """Validate plugin for certification"""
        # In a real implementation, this would validate plugin
        # For now, return mock validation results
        return {
            'security': True,
            'performance': True,
            'documentation': True,
            'code_quality': True,
            'requirements_met': ['all']
        }
class MetadataSystem:
    """Plugin metadata system"""
    
    def __init__(self, marketplace: PluginMarketplace):
        self.marketplace = marketplace
        
    def extract_metadata(self, plugin_path: str) -> Dict[str, Any]:
        """Extract metadata from plugin"""
        # In a real implementation, this would extract metadata from plugin files
        # For now, return mock metadata
        return {
            'name': 'Sample Plugin',
            'version': '1.0.0',
            'description': 'A sample plugin',
            'author': 'Sample Author',
            'plugin_type': 'utility',
            'categories': ['general'],
            'tags': ['sample', 'utility'],
            'keywords': ['sample', 'utility'],
            'targets': ['python'],
            'dependencies': {},
            'license': 'MIT'
        }