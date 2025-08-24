#!/usr/bin/env python3
"""
Create minimal silent audio files for testing
"""

import os
import struct
import wave

def create_silent_wav(filepath, duration_ms=500):
    """Create a silent WAV file"""
    os.makedirs(os.path.dirname(filepath), exist_ok=True)
    
    sample_rate = 44100
    samples = int(sample_rate * duration_ms / 1000)
    
    with wave.open(filepath, 'wb') as wav_file:
        wav_file.setnchannels(1)  # Mono
        wav_file.setsampwidth(2)  # 16-bit
        wav_file.setframerate(sample_rate)
        
        # Write silent samples (all zeros)
        silent_data = b'\x00\x00' * samples
        wav_file.writeframes(silent_data)
    
    print(f"Created silent WAV: {filepath} ({duration_ms}ms)")

def main():
    print("Creating silent placeholder audio files...")
    
    # Sound effects (short)
    create_silent_wav("assets/sounds/jump.wav", 300)
    create_silent_wav("assets/sounds/land.wav", 200)
    create_silent_wav("assets/sounds/hurt.wav", 500)
    create_silent_wav("assets/sounds/die.wav", 1000)
    create_silent_wav("assets/sounds/powerup.wav", 800)
    create_silent_wav("assets/sounds/coin.wav", 400)
    create_silent_wav("assets/sounds/stomp.wav", 300)
    
    # Background music (longer, but still silent for now)
    create_silent_wav("assets/music/menu.wav", 2000)
    create_silent_wav("assets/music/overworld.wav", 3000)
    create_silent_wav("assets/music/underground.wav", 3000)
    create_silent_wav("assets/music/castle.wav", 3000)
    create_silent_wav("assets/music/victory.wav", 2000)
    create_silent_wav("assets/music/gameover.wav", 2000)
    
    print("All placeholder audio files created!")
    print("Note: These are silent WAV files. Replace with actual audio for production.")

if __name__ == "__main__":
    main()