import sys
from PIL import Image
import numpy as np

def save_array(array, output_path, vector_name):
    with open(output_path, 'w') as file:
        file.write("const std::vector<std::vector<float>> {} = {{\n".format(vector_name))
        for row in array:
            file.write("    {")
            file.write(", ".join("{:.6f}f".format(value) for value in row))
            file.write("},\n")
        file.write("};\n")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python convertImage.py <image_path>")
        sys.exit(1)

    image_path = sys.argv[1]
    output_file = f"{image_path.split('.')[0]}.txt"
    vector_name = image_path.split('.')[0] + "_terrain"

    img = Image.open(image_path)
    img_gray = img.convert('L')
    img_array = np.array(img_gray)
    img_array = img_array / 255.0  # scale down to between 0-1
    save_array(img_array, output_file, vector_name)