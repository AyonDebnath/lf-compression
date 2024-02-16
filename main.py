import subprocess
from io import BytesIO
from PIL import Image
import jpeg_decoder as jpeg
import numpy as np
import cv2
from struct import unpack




if __name__ == "__main__":

    input_image_path = input("Enter the input image file path: ")
    converted_image_path = "converted_image.jpeg"

    coordinate = input("Enter x and y coordinate of the block to compress separated by a space.")
    coordinate = coordinate.split()
    coordinate = (int(coordinate[0]), int(coordinate[1]))

    scaling_factor = 1

    convertImageWithSamplingFactor(input_image_path, converted_image_path, "4:4:4")

    width, height = Image.open(converted_image_path).size

    output = [[0 for _ in range(width*scaling_factor)] for _ in range(height*scaling_factor)]

    jpeg.initialize(converted_image_path, output, scaling_factor, coordinate)
    jpeg.decode()

    output = np.array(jpeg.output).astype(np.uint8)

    # Display the image
    # cv2.imshow('Result Image', np.array(output).astype(np.uint8))
    cv2.imwrite('images/'+ input_image_path.split('/')[-1].split('.')[0] + '_output.png', output)
    print("Output has been saved in the Images Directory.")
