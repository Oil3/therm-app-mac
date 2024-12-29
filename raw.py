import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
import os
import sys

def process_raw_to_jpg(input_path, output_folder, width=384, height=288, quality=96):
    """
    Converts raw files in the input path to jpg images in the output folder.

    :param input_path: Path to the input file or folder.
    :param output_folder: Path to the output folder.
    :param width: Width of the raw image.
    :param height: Height of the raw image.
    :param quality: JPEG quality (default 96).
    """
    if os.path.isfile(input_path):
        files = [input_path]
    elif os.path.isdir(input_path):
        files = [os.path.join(input_path, f) for f in os.listdir(input_path) if f.endswith('.raw')]
    else:
        print(f"Invalid input path: {input_path}")
        return

    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    for file in files:
        try:
            raw_data = np.fromfile(file, dtype=np.int16)
            image = raw_data.reshape((height, width))

            # Normalize the image data to the range 0-255 for JPEG
            normalized_image = ((image - image.min()) / (image.max() - image.min()) * 255).astype(np.uint8)
            
            output_filename = os.path.join(output_folder, os.path.basename(file).replace('.raw', '.jpg'))
            img = Image.fromarray(normalized_image)
            img.save(output_filename, 'JPEG', quality=quality)
            print(f"Converted {file} to {output_filename}")
        except Exception as e:
            print(f"Error processing {file}: {e}")

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Convert RAW files to JPG.")
    parser.add_argument("input", nargs="?", default=".", help="Input file or folder (default: current folder)")
    parser.add_argument("output", nargs="?", default="./jpg", help="Output folder (default: ./jpg)")
    parser.add_argument("--width", type=int, default=384, help="Width of the RAW image (default: 384)")
    parser.add_argument("--height", type=int, default=288, help="Height of the RAW image (default: 288)")
    parser.add_argument("--quality", type=int, default=96, help="JPEG quality (default: 96)")

    args = parser.parse_args()

    if len(sys.argv) == 1:
        print("Running in interactive mode.")
        input_path = input("Enter input file or folder (default: current folder): ") or "."
        output_folder = input("Output folder? (default: ./jpg): ") or "./jpg"
        width = int(input("Image width ?(default: 384): ") or 384)
        height = int(input("Image height ?(default: 288): ") or 288)
        quality = 96
        process_raw_to_jpg(input_path.strip('"').strip("'"), output_folder.strip('"').strip("'"), width, height, quality)
    else:
        process_raw_to_jpg(args.input.strip('"').strip("'"), args.output.strip('"').strip("'"), args.width, args.height, args.quality)
