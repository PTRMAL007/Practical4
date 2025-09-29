import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile
from scipy import signal

def wav_to_lut(filename, num_samples=128, duration_ms=50):
    """
    Convert a .wav file to a lookup table for embedded systems.
    
    Parameters:
    - filename: path to .wav file
    - num_samples: number of samples in LUT (default 128)
    - duration_ms: duration in milliseconds to extract from audio (default 50ms)
    
    Returns:
    - lut: numpy array of 12-bit values (0-4095)
    """
    
    # Read the WAV file
    sample_rate, audio_data = wavfile.read(filename)
    print(f"File: {filename}")
    print(f"Sample rate: {sample_rate} Hz")
    print(f"Audio shape: {audio_data.shape}")
    print(f"Data type: {audio_data.dtype}")
    print(f"Duration: {len(audio_data)/sample_rate:.2f} seconds")
    
    # Convert stereo to mono if needed
    if len(audio_data.shape) > 1:
        audio_data = np.mean(audio_data, axis=1)
        print("Converted stereo to mono")
    
    # Normalize to [-1, 1] range
    if audio_data.dtype == np.int16:
        audio_data = audio_data.astype(np.float32) / 32768.0
    elif audio_data.dtype == np.int32:
        audio_data = audio_data.astype(np.float32) / 2147483648.0
    elif audio_data.dtype == np.uint8:
        audio_data = (audio_data.astype(np.float32) - 128) / 128.0
    else:
        audio_data = audio_data.astype(np.float32)
    
    print(f"Audio range after normalization: [{np.min(audio_data):.3f}, {np.max(audio_data):.3f}]")
    
    # Extract a portion of the audio (find the loudest section)
    samples_to_extract = int(sample_rate * duration_ms / 1000)
    
    # Find the section with highest RMS energy
    window_size = samples_to_extract
    if len(audio_data) > window_size:
        # Calculate RMS for sliding windows
        num_windows = len(audio_data) - window_size
        rms_values = np.array([np.sqrt(np.mean(audio_data[i:i+window_size]**2)) 
                               for i in range(0, num_windows, window_size//10)])
        
        # Find window with maximum energy
        best_window_idx = np.argmax(rms_values) * (window_size//10)
        audio_segment = audio_data[best_window_idx:best_window_idx + window_size]
        print(f"Extracted segment from sample {best_window_idx} (highest energy region)")
    else:
        audio_segment = audio_data
        print(f"Using entire audio file ({len(audio_segment)} samples)")
    
    # Resample to desired number of samples
    lut_float = signal.resample(audio_segment, num_samples)
    
    print(f"Resampled range: [{np.min(lut_float):.3f}, {np.max(lut_float):.3f}]")
    
    # Check if signal is too quiet
    if np.max(np.abs(lut_float)) < 0.01:
        print("WARNING: Signal is very quiet, may need amplification")
    
    # Normalize to ensure full range usage
    if np.max(lut_float) != np.min(lut_float):
        lut_float = lut_float - np.min(lut_float)  # Shift to positive
        lut_float = lut_float / np.max(lut_float)  # Normalize to [0, 1]
    else:
        print("ERROR: No variation in signal - all samples are the same!")
        lut_float = np.zeros(num_samples)
    
    # Scale to 12-bit range (0-4095)
    lut = np.round(lut_float * 4095).astype(np.uint16)
    
    # Ensure values are within range
    lut = np.clip(lut, 0, 4095)
    
    print(f"Final LUT range: [{np.min(lut)}, {np.max(lut)}]")
    
    return lut, sample_rate

def plot_waveforms(luts, labels):
    """
    Plot multiple waveforms for visualization.
    
    Parameters:
    - luts: list of numpy arrays
    - labels: list of labels for each waveform
    """
    fig, axes = plt.subplots(len(luts), 1, figsize=(12, 3*len(luts)))
    
    if len(luts) == 1:
        axes = [axes]
    
    for i, (lut, label) in enumerate(zip(luts, labels)):
        axes[i].plot(lut, linewidth=2, color='blue')
        axes[i].set_title(f'{label} Waveform (LUT)', fontsize=14, fontweight='bold')
        axes[i].set_xlabel('Sample Index')
        axes[i].set_ylabel('12-bit Value (0-4095)')
        axes[i].grid(True, alpha=0.3)
        axes[i].set_ylim(-100, 4195)
        
        # Add statistics
        stats_text = f'Min: {np.min(lut)}, Max: {np.max(lut)}, Mean: {np.mean(lut):.1f}'
        axes[i].text(0.02, 0.98, stats_text, transform=axes[i].transAxes,
                    verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
    
    plt.tight_layout()
    plt.savefig('waveform_luts.png', dpi=300, bbox_inches='tight')
    print("\nPlot saved as 'waveform_luts.png'")
    plt.show()

def export_to_c_array(lut, name, filename='luts.txt'):
    """
    Export LUT as C array format.
    
    Parameters:
    - lut: numpy array
    - name: variable name for the array
    - filename: output text file
    """
    with open(filename, 'a') as f:
        f.write(f"\n// {name} - {len(lut)} samples\n")
        f.write(f"uint32_t {name}[{len(lut)}] = {{\n    ")
        
        for i, value in enumerate(lut):
            f.write(f"{value}")
            if i < len(lut) - 1:
                f.write(", ")
                if (i + 1) % 8 == 0:  # 8 values per line
                    f.write("\n    ")
        
        f.write("\n};\n")
    
    print(f"Exported {name} to {filename}")

def main():
    """
    Main function to process audio files and generate LUTs.
    """
    # Clear the output file
    open('luts.txt', 'w').close()
    
    # Configuration
    num_samples = 128  # Number of samples in LUT
    target_freq = 440  # Target frequency in Hz
    
    # List of audio files to process
    # Replace these with your actual .wav file paths
    audio_files = [
        ('piano.wav', 'Piano_LUT'),
        ('guitar.wav', 'Guitar_LUT'),
        ('drum.wav', 'Drum_LUT')
    ]
    
    luts = []
    labels = []
    
    print("=" * 60)
    print("WAV to LUT Converter for STM32")
    print("=" * 60)
    
    for filename, var_name in audio_files:
        try:
            print(f"\nProcessing {filename}...")
            lut, sample_rate = wav_to_lut(filename, num_samples, target_freq)
            
            # Store for plotting
            luts.append(lut)
            labels.append(var_name.replace('_LUT', ''))
            
            # Export to C format
            export_to_c_array(lut, var_name, 'luts.txt')
            
            print(f"✓ Successfully processed {filename}")
            print(f"  LUT size: {len(lut)} samples")
            print(f"  Value range: {np.min(lut)} to {np.max(lut)}")
            
        except FileNotFoundError:
            print(f"✗ Error: File '{filename}' not found!")
        except Exception as e:
            print(f"✗ Error processing {filename}: {str(e)}")
    
    # Plot all waveforms
    if luts:
        print("\nGenerating plots...")
        plot_waveforms(luts, labels)
        print("\n" + "=" * 60)
        print(f"All LUTs exported to 'luts.txt'")
        print("Copy the arrays from luts.txt into your main.c file")
        print("=" * 60)
    else:
        print("\nNo files were successfully processed.")

# Alternative: Generate test waveforms if you don't have audio files
def generate_test_waveforms(num_samples=128):
    """
    Generate synthetic instrument-like waveforms for testing.
    """
    open('luts.txt', 'w').close()
    
    t = np.linspace(0, 2*np.pi, num_samples, endpoint=False)
    
    # Piano-like: Fundamental + harmonics with exponential decay
    piano = np.sin(t) + 0.5*np.sin(2*t) + 0.25*np.sin(3*t)
    piano = piano * np.exp(-t/2)  # Decay envelope
    piano = ((piano - np.min(piano)) / (np.max(piano) - np.min(piano)) * 4095).astype(np.uint16)
    
    # Guitar-like: Fundamental + odd harmonics
    guitar = np.sin(t) + 0.3*np.sin(3*t) + 0.1*np.sin(5*t)
    guitar = ((guitar - np.min(guitar)) / (np.max(guitar) - np.min(guitar)) * 4095).astype(np.uint16)
    
    # Drum-like: Noise with exponential decay
    drum = np.random.randn(num_samples) * np.exp(-np.linspace(0, 5, num_samples))
    drum = ((drum - np.min(drum)) / (np.max(drum) - np.min(drum)) * 4095).astype(np.uint16)
    
    # Export and plot
    export_to_c_array(piano, 'Piano_LUT', 'luts.txt')
    export_to_c_array(guitar, 'Guitar_LUT', 'luts.txt')
    export_to_c_array(drum, 'Drum_LUT', 'luts.txt')
    
    plot_waveforms([piano, guitar, drum], ['Piano', 'Guitar', 'Drum'])
    
    print("\nTest waveforms generated successfully!")
    print("Check 'luts.txt' for the C arrays")

if __name__ == "__main__":
    # Choose one of the following:
    
    # Option 1: Process actual .wav files
    main()
    
    # Option 2: Generate synthetic test waveforms
    # generate_test_waveforms()
    
    print("\nDone!")