// Super Mario WebAssembly - Game Loader
// Production-quality loader with error handling, DPR scaling, and performance monitoring

(function() {
    'use strict';
    
    let audioEnabled = false;
    let gameLoaded = false;
    let loadStartTime = performance.now();
    let performanceMetrics = {
        loadTime: 0,
        assetsSize: 0,
        wasmSize: 0
    };
    
    // Canvas and display management
    function setupCanvas() {
        const canvas = document.getElementById('canvas');
        if (!canvas) return;
        
        const container = canvas.parentElement;
        const dpr = Math.min(window.devicePixelRatio || 1, 2); // Cap DPR for performance
        
        // Base dimensions
        const baseWidth = 1024;
        const baseHeight = 576;
        
        // Calculate display size based on container and viewport
        const containerRect = container.getBoundingClientRect();
        const maxWidth = Math.min(window.innerWidth * 0.95, 1024);
        const maxHeight = Math.min(window.innerHeight * 0.6, 576);
        
        const aspectRatio = baseWidth / baseHeight;
        let displayWidth = Math.min(maxWidth, maxHeight * aspectRatio);
        let displayHeight = displayWidth / aspectRatio;
        
        // Ensure it fits in viewport
        if (displayHeight > maxHeight) {
            displayHeight = maxHeight;
            displayWidth = displayHeight * aspectRatio;
        }
        
        // Set CSS dimensions
        canvas.style.width = Math.floor(displayWidth) + 'px';
        canvas.style.height = Math.floor(displayHeight) + 'px';
        
        // Set actual canvas dimensions with DPR
        canvas.width = baseWidth * dpr;
        canvas.height = baseHeight * dpr;
        
        // Scale context for DPR
        const ctx = canvas.getContext('2d');
        if (ctx) {
            ctx.scale(dpr, dpr);
        }
        
        console.log(`Canvas setup: Display(${displayWidth}x${displayHeight}) Canvas(${canvas.width}x${canvas.height}) DPR(${dpr})`);
    }
    
    // Fullscreen management
    window.requestFullscreen = function() {
        const canvas = document.getElementById('canvas');
        if (!canvas) return;
        
        const requestFS = canvas.requestFullscreen || 
                         canvas.webkitRequestFullscreen || 
                         canvas.mozRequestFullScreen || 
                         canvas.msRequestFullscreen;
        
        if (requestFS) {
            requestFS.call(canvas).catch(err => {
                console.warn('Fullscreen request failed:', err);
            });
        }
    };
    
    // Audio context management
    window.enableAudio = function() {
        if (audioEnabled) return;
        
        try {
            const AudioContextClass = window.AudioContext || 
                                    window.webkitAudioContext || 
                                    window.mozAudioContext;
            
            if (AudioContextClass) {
                const audioContext = new AudioContextClass();
                audioContext.resume().then(() => {
                    console.log('Audio context enabled successfully');
                    audioEnabled = true;
                    document.getElementById('audioNotice').style.display = 'none';
                }).catch(err => {
                    console.warn('Audio context resume failed:', err);
                });
            }
        } catch (err) {
            console.warn('Audio context initialization failed:', err);
        }
        
        // Hide notice regardless
        document.getElementById('audioNotice').style.display = 'none';
    };
    
    // Error handling
    function showError(message, details = '') {
        console.error('Game Error:', message, details);
        
        const errorDiv = document.getElementById('errorMessage');
        const errorText = document.getElementById('errorText');
        const loading = document.getElementById('loading');
        
        if (errorDiv && errorText) {
            errorText.textContent = message + (details ? ': ' + details : '');
            errorDiv.style.display = 'block';
        }
        
        if (loading) {
            loading.style.display = 'none';
        }
    }
    
    // Performance monitoring
    function logPerformanceMetrics() {
        const loadTime = (performance.now() - loadStartTime) / 1000;
        console.log('Performance Metrics:', {
            loadTime: loadTime.toFixed(2) + 's',
            assetsSize: (performanceMetrics.assetsSize / 1024 / 1024).toFixed(2) + 'MB',
            wasmSize: (performanceMetrics.wasmSize / 1024 / 1024).toFixed(2) + 'MB',
            memory: performance.memory ? {
                used: (performance.memory.usedJSHeapSize / 1024 / 1024).toFixed(2) + 'MB',
                total: (performance.memory.totalJSHeapSize / 1024 / 1024).toFixed(2) + 'MB'
            } : 'N/A'
        });
    }
    
    // Module configuration for Emscripten
    window.Module = {
        preRun: [],
        postRun: [
            function() {
                console.log('Game initialized successfully');
                gameLoaded = true;
                logPerformanceMetrics();
                
                // Show audio notice after a delay
                setTimeout(() => {
                    if (!audioEnabled) {
                        document.getElementById('audioNotice').style.display = 'block';
                    }
                }, 1500);
            }
        ],
        
        print: function(text) {
            console.log('[Game]', text);
        },
        
        printErr: function(text) {
            console.error('[Game Error]', text);
        },
        
        canvas: (function() {
            const canvas = document.getElementById('canvas');
            if (!canvas) {
                showError('Canvas element not found');
                return null;
            }
            
            canvas.addEventListener("webglcontextlost", function(e) {
                console.error('WebGL context lost');
                showError('WebGL context was lost. Please refresh the page.');
                e.preventDefault();
            }, false);
            
            return canvas;
        })(),
        
        setStatus: function(text) {
            const status = document.getElementById('status');
            const loading = document.getElementById('loading');
            
            if (!status || !loading) return;
            
            if (text) {
                status.textContent = text;
                console.log('Status:', text);
            } else {
                // Loading complete
                console.log('Game loading completed');
                loading.style.opacity = '0';
                setTimeout(() => {
                    loading.style.display = 'none';
                }, 500);
            }
        },
        
        totalDependencies: 0,
        monitorRunDependencies: function(left) {
            this.totalDependencies = Math.max(this.totalDependencies, left);
            const progress = document.getElementById('progress');
            
            if (!progress) return;
            
            if (left > 0) {
                const percent = Math.round(((this.totalDependencies - left) / this.totalDependencies) * 100);
                progress.style.width = percent + '%';
                Module.setStatus(`Loading dependencies: ${percent}%`);
            } else {
                progress.style.width = '100%';
                Module.setStatus('Starting game...');
                setTimeout(() => Module.setStatus(''), 1000);
            }
        },
        
        locateFile: function(path, prefix) {
            // Handle relative paths for different hosting scenarios
            const basePath = window.location.pathname.endsWith('/') ? 
                           window.location.pathname : 
                           window.location.pathname + '/';
            
            if (path.endsWith('.wasm')) {
                performanceMetrics.wasmSize = 5 * 1024 * 1024; // Estimate
                console.log('Loading WASM:', prefix + path);
            }
            
            return prefix + path;
        },
        
        onRuntimeInitialized: function() {
            console.log('WebAssembly runtime initialized');
        },
        
        // Memory configuration
        INITIAL_MEMORY: 64 * 1024 * 1024, // 64MB
        ALLOW_MEMORY_GROWTH: true,
        
        // Error handling
        onAbort: function(what) {
            showError('Game crashed during initialization', what);
        },
        
        onExit: function(status) {
            if (status !== 0) {
                showError('Game exited with error code', status.toString());
            }
        }
    };
    
    // Initialize canvas setup
    setupCanvas();
    
    // Event listeners
    window.addEventListener('resize', setupCanvas);
    window.addEventListener('orientationchange', () => setTimeout(setupCanvas, 100));
    
    // Prevent context menu on canvas
    document.addEventListener('contextmenu', function(e) {
        if (e.target.tagName === 'CANVAS') {
            e.preventDefault();
            return false;
        }
    });
    
    // Handle fullscreen changes
    document.addEventListener('fullscreenchange', function() {
        const canvas = document.getElementById('canvas');
        if (!canvas) return;
        
        if (document.fullscreenElement) {
            canvas.style.width = '100vw';
            canvas.style.height = '100vh';
            canvas.style.objectFit = 'contain';
        } else {
            setupCanvas();
        }
    });
    
    // Gamepad support
    window.addEventListener("gamepadconnected", function(e) {
        console.log("Gamepad connected:", e.gamepad.id);
        // Could show a toast notification here
    });
    
    window.addEventListener("gamepaddisconnected", function(e) {
        console.log("Gamepad disconnected:", e.gamepad.id);
    });
    
    // Handle page visibility for pause/resume
    document.addEventListener('visibilitychange', function() {
        if (gameLoaded && typeof Module !== 'undefined') {
            if (document.hidden) {
                // Game should pause itself when window loses focus
                console.log('Page hidden - game should pause');
            } else {
                // Ensure audio context is resumed when page becomes visible
                enableAudio();
                console.log('Page visible - game can resume');
            }
        }
    });
    
    // Initialize status
    if (Module.setStatus) {
        Module.setStatus('Downloading assets...');
    }
    
    // Performance observer for monitoring
    if ('PerformanceObserver' in window) {
        try {
            const observer = new PerformanceObserver((list) => {
                for (const entry of list.getEntries()) {
                    if (entry.entryType === 'resource' && entry.name.includes('.wasm')) {
                        performanceMetrics.wasmSize = entry.transferSize || entry.encodedBodySize || 0;
                    }
                }
            });
            observer.observe({ entryTypes: ['resource'] });
        } catch (e) {
            console.log('Performance observer not available');
        }
    }
    
    // Global error handling
    window.addEventListener('error', function(e) {
        if (e.filename && e.filename.includes('game.js')) {
            showError('Game script error', e.message);
        }
    });
    
    window.addEventListener('unhandledrejection', function(e) {
        console.error('Unhandled promise rejection:', e.reason);
    });
    
    console.log('Super Mario WebAssembly loader initialized');
    
})();