#!/usr/bin/env python3
"""
Create minimal 1-pixel PNG files that can be used as placeholders
"""

import os
import struct

def create_1x1_png(filepath, r, g, b, a=255):
    """Create a 1x1 PNG file with specified RGBA color"""
    os.makedirs(os.path.dirname(filepath), exist_ok=True)
    
    # PNG file structure for 1x1 RGBA image
    width = 1
    height = 1
    
    # PNG signature
    png_signature = b'\x89PNG\r\n\x1a\n'
    
    # IHDR chunk (Image Header)
    ihdr_data = struct.pack('>2I5B', width, height, 8, 6, 0, 0, 0)  # 8-bit RGBA
    ihdr_crc = 0x2144DF1C  # Pre-calculated CRC for 1x1 RGBA IHDR
    ihdr_chunk = struct.pack('>I', len(ihdr_data)) + b'IHDR' + ihdr_data + struct.pack('>I', ihdr_crc)
    
    # IDAT chunk (Image Data) - single pixel RGBA
    pixel_data = bytes([0, r, g, b, a])  # Filter byte + RGBA
    
    # Simple uncompressed zlib data
    import zlib
    compressed_data = zlib.compress(pixel_data)
    idat_chunk_data = compressed_data
    
    # Calculate CRC32 for IDAT
    import binascii
    idat_crc = binascii.crc32(b'IDAT' + idat_chunk_data) & 0xffffffff
    idat_chunk = struct.pack('>I', len(idat_chunk_data)) + b'IDAT' + idat_chunk_data + struct.pack('>I', idat_crc)
    
    # IEND chunk
    iend_chunk = struct.pack('>I', 0) + b'IEND' + struct.pack('>I', 0xAE426082)
    
    # Write PNG file
    with open(filepath, 'wb') as f:
        f.write(png_signature)
        f.write(ihdr_chunk)
        f.write(idat_chunk)
        f.write(iend_chunk)
    
    print(f"Created {filepath}")

def create_simple_png(filepath, width, height, r, g, b, a=255):
    """Create a simple solid color PNG"""
    os.makedirs(os.path.dirname(filepath), exist_ok=True)
    
    # For now, just create 1x1 and let the game scale it
    create_1x1_png(filepath, r, g, b, a)

def main():
    print("Creating minimal placeholder PNG assets...")
    
    # Sprite assets
    create_simple_png("assets/sprites/mario_small.png", 128, 16, 255, 0, 0)    # Red
    create_simple_png("assets/sprites/mario_big.png", 128, 32, 255, 100, 0)   # Orange
    create_simple_png("assets/sprites/mario_fire.png", 128, 32, 255, 200, 0)  # Yellow
    create_simple_png("assets/sprites/goomba.png", 64, 16, 139, 69, 19)       # Brown
    create_simple_png("assets/sprites/koopa.png", 64, 24, 0, 128, 0)          # Green
    create_simple_png("assets/sprites/items.png", 128, 16, 255, 215, 0)       # Gold
    create_simple_png("assets/sprites/blocks.png", 128, 32, 139, 69, 19)      # Brown
    
    # Tile assets
    create_simple_png("assets/tiles/tileset.png", 256, 256, 34, 139, 34)      # Forest Green
    create_simple_png("assets/tiles/ground.png", 32, 32, 139, 69, 19)         # Brown
    create_simple_png("assets/tiles/brick.png", 32, 32, 178, 34, 34)          # Dark Red
    create_simple_png("assets/tiles/pipe.png", 64, 128, 0, 128, 0)            # Green
    
    # Background assets  
    create_simple_png("assets/backgrounds/sky.png", 1024, 576, 135, 206, 235)  # Sky Blue
    create_simple_png("assets/backgrounds/clouds.png", 1024, 200, 255, 255, 255, 100) # White w/ alpha
    create_simple_png("assets/backgrounds/hills.png", 1024, 200, 34, 139, 34, 200)   # Green w/ alpha
    
    # UI assets
    create_simple_png("assets/ui/hud.png", 512, 64, 0, 0, 0, 200)             # Black w/ alpha
    create_simple_png("assets/ui/font.png", 128, 256, 255, 255, 255)          # White
    
    print("All placeholder assets created!")
    print("Note: These are 1x1 pixel images that will be scaled by the game engine.")

if __name__ == "__main__":
    main()