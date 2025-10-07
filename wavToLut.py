import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile
import os

def generate_lut_from_wav(wav_file, num_samples=128, output_name=None):
    """
    Generate a lookup table from a .wav file for STM32 DAC
    
    Parameters:
    - wav_file: path to the .wav file
    - num_samples: number of samples in the LUT (minimum 128)
    - output_name: name for the output (defaults to filename without extension)
    
    Returns:
    - lut: numpy array with values scaled to 0-4095 (12-bit)
    """
    
    # Read the WAV file
    sample_rate, audio_data = wavfile.read(wav_file)
    
    print(f"  Original dtype: {audio_data.dtype}")
    print(f"  Original shape: {audio_data.shape}")
    print(f"  Sample rate: {sample_rate} Hz")
    
    # If stereo, convert to mono by averaging channels
    if len(audio_data.shape) > 1:
        audio_data = np.mean(audio_data, axis=1)
        print(f"  Converted to mono")
    
    # Convert to float and normalize properly based on dtype
    if audio_data.dtype == np.int16:
        # int16 ranges from -32768 to 32767
        audio_data = audio_data.astype(np.float64) / 32768.0
    elif audio_data.dtype == np.int32:
        # int32 ranges from -2147483648 to 2147483647
        audio_data = audio_data.astype(np.float64) / 2147483648.0
    elif audio_data.dtype == np.uint8:
        # uint8 ranges from 0 to 255, center is at 128
        audio_data = (audio_data.astype(np.float64) - 128.0) / 128.0
    elif audio_data.dtype == np.float32 or audio_data.dtype == np.float64:
        # Already float, just ensure it's float64
        audio_data = audio_data.astype(np.float64)
    else:
        print(f"  Warning: Unknown dtype {audio_data.dtype}, treating as float")
        audio_data = audio_data.astype(np.float64)
    
    print(f"  Audio range after normalization: [{np.min(audio_data):.4f}, {np.max(audio_data):.4f}]")
    
    # Find the actual peak value for proper scaling
    peak = np.max(np.abs(audio_data))
    if peak > 0:
        # Normalize to use full range without clipping
        audio_data = audio_data / peak
        print(f"  Normalized by peak value: {peak:.4f}")
    
    # Resample to desired number of samples
    if len(audio_data) > num_samples:
        # Downsample by selecting evenly spaced samples
        indices = np.linspace(0, len(audio_data) - 1, num_samples, dtype=int)
        resampled = audio_data[indices]
    else:
        # Upsample using interpolation
        x_old = np.linspace(0, 1, len(audio_data))
        x_new = np.linspace(0, 1, num_samples)
        resampled = np.interp(x_new, x_old, audio_data)
    
    print(f"  Resampled range: [{np.min(resampled):.4f}, {np.max(resampled):.4f}]")
    
    # Scale from [-1, 1] to [0, 4095] (12-bit DAC range)
    # Add 1 to shift to [0, 2], then multiply by 2047.5 to get [0, 4095]
    lut = ((resampled + 1.0) * 2047.5).astype(np.uint16)
    
    # Clip to ensure values are within range
    lut = np.clip(lut, 0, 4095)
    
    print(f"  Final LUT range: [{np.min(lut)}, {np.max(lut)}]")
    print(f"  Mean LUT value: {np.mean(lut):.1f} (should be near 2048)")
    
    # Set output name
    if output_name is None:
        output_name = os.path.splitext(os.path.basename(wav_file))[0]
    
    return lut, resampled, sample_rate, output_name


def plot_waveform(lut, normalized_audio, sample_rate, output_name):
    """
    Plot the waveform with both normalized audio and LUT values
    """
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))
    
    # Plot normalized audio
    time_axis = np.arange(len(normalized_audio))
    ax1.plot(time_axis, normalized_audio, 'b-', linewidth=1.5)
    ax1.axhline(y=0, color='k', linestyle='--', alpha=0.3)
    ax1.set_xlabel('Sample Index')
    ax1.set_ylabel('Normalized Amplitude')
    ax1.set_title(f'{output_name} - Normalized Waveform (-1 to +1)')
    ax1.grid(True, alpha=0.3)
    ax1.set_ylim([-1.2, 1.2])
    
    # Plot LUT values
    sample_indices = np.arange(len(lut))
    ax2.plot(sample_indices, lut, 'r-', linewidth=1.5, marker='o', markersize=3)
    ax2.axhline(y=2048, color='k', linestyle='--', alpha=0.3, label='Center (2048)')
    ax2.set_xlabel('Sample Index')
    ax2.set_ylabel('DAC Value (12-bit)')
    ax2.set_title(f'{output_name} - Lookup Table (0-4095)')
    ax2.grid(True, alpha=0.3)
    ax2.set_ylim([0, 4095])
    ax2.legend()
    
    plt.tight_layout()
    plt.savefig(f'{output_name}_waveform.png', dpi=150, bbox_inches='tight')
    print(f"  Saved waveform plot: {output_name}_waveform.png")
    plt.close()


def format_lut_for_c(lut, var_name, values_per_line=12):
    """
    Format the LUT as C array code
    """
    c_code = f"// Lookup table for {var_name}\n"
    c_code += f"// {len(lut)} samples, 12-bit resolution (0-4095)\n"
    c_code += f"uint16_t {var_name}[{len(lut)}] = {{\n    "
    
    for i, value in enumerate(lut):
        c_code += f"{value}"
        if i < len(lut) - 1:
            c_code += ", "
            if (i + 1) % values_per_line == 0:
                c_code += "\n    "
    
    c_code += "\n};\n"
    return c_code


def process_wav_files(wav_files, num_samples=128):
    """
    Process multiple WAV files and generate LUTs
    
    Parameters:
    - wav_files: list of paths to .wav files
    - num_samples: number of samples in each LUT
    """
    
    all_c_code = "// ===== Generated Lookup Tables from WAV Files =====\n"
    all_c_code += f"// Sample rate: 44.1 kHz (original WAV files)\n"
    all_c_code += f"// LUT size: {num_samples} samples\n"
    all_c_code += f"// Resolution: 12-bit (0-4095)\n"
    all_c_code += f"// Center value: 2048 (represents 0V offset)\n\n"
    
    lut_info = []
    
    for wav_file in wav_files:
        if not os.path.exists(wav_file):
            print(f"Warning: File not found - {wav_file}")
            continue
        
        print(f"\nProcessing: {wav_file}")
        
        # Generate LUT
        lut, normalized_audio, sample_rate, output_name = generate_lut_from_wav(
            wav_file, num_samples
        )
        
        # Create a clean variable name
        var_name = output_name.replace('-', '_').replace(' ', '_').replace('.', '_')
        var_name = f"LUT_{var_name}"
        
        # Plot waveform
        plot_waveform(lut, normalized_audio, sample_rate, output_name)
        
        # Generate C code
        c_code = format_lut_for_c(lut, var_name)
        all_c_code += c_code + "\n"
        
        # Store info
        lut_info.append({
            'name': var_name,
            'file': wav_file,
            'samples': len(lut),
            'min': np.min(lut),
            'max': np.max(lut),
            'mean': np.mean(lut),
            'std': np.std(lut)
        })
        
        print(f"  Variable name: {var_name}")
        print(f"  Value range: {np.min(lut)} - {np.max(lut)}")
        print(f"  Mean: {np.mean(lut):.1f}, Std Dev: {np.std(lut):.1f}")
    
    # Save all C code to file
    with open('wav_lookup_tables.txt', 'w') as f:
        f.write(all_c_code)
    print(f"\n{'='*50}")
    print(f"Saved C code to: wav_lookup_tables.txt")
    
    # Print summary
    print("\n===== Summary =====")
    print(f"Processed {len(lut_info)} WAV files")
    print(f"LUT size: {num_samples} samples each")
    print("\nQuality Check:")
    for info in lut_info:
        center_offset = abs(info['mean'] - 2048)
        quality = "GOOD" if center_offset < 100 and info['std'] > 100 else "CHECK"
        print(f"  {info['name']}: {quality}")
        print(f"    Mean: {info['mean']:.1f} (offset from 2048: {center_offset:.1f})")
        print(f"    Range: {info['min']} to {info['max']}")
        print(f"    Dynamic range: {info['std']:.1f}")
    
    print("\nVariable names to use in main.c:")
    for info in lut_info:
        print(f"  - {info['name']}")
    
    return lut_info


# ===== MAIN EXECUTION =====
if __name__ == "__main__":
    # Replace these with your actual WAV file paths
    wav_files = [
        'piano.wav',
        'guitar.wav', 
        'drum.wav'
    ]
    
    # You can adjust the number of samples if needed (minimum 128)
    NUM_SAMPLES = 3000
    
    print("="*50)
    print("WAV to LUT Generator for STM32")
    print("="*50)
    print(f"Target: {NUM_SAMPLES} samples per LUT")
    print(f"Resolution: 12-bit (0-4095)")
    print(f"Expected sample rate: 44.1 kHz\n")
    
    # Process all WAV files
    lut_info = process_wav_files(wav_files, NUM_SAMPLES)
    
    print("\n" + "=" * 50)
    print("Processing complete!")
    print("\nNext steps:")
    print("1. Check the generated PNG files - waveforms should be centered")
    print("2. Verify mean values are near 2048 (DC offset)")
    print("3. Copy arrays from 'wav_lookup_tables.txt' into your main.c")
    print("4. If waveforms still look wrong, the WAV files may be corrupted")
    print("=" * 50)