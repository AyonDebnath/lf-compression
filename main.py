import math

from PIL import Image
import jpeg_decoder as jpeg
import numpy as np
import cv2

if __name__ == "__main__":
    input_image_path = input("Enter the input image file path: ")
    converted_image_path = "converted_image.jpeg"  # Temporary image in the Y, Cr, Cb format
    imageCoordinate = input("Enter x and y coordinate of the image pixel to decode separated by a space.")


    # Converting the coordinates of the pixel to a list
    imageCoordinate = imageCoordinate.split()

    xBlockCoordinate = int(imageCoordinate[0])//8
    yBlockCoordinate = int(imageCoordinate[1])//8
    blockCoordinate = (xBlockCoordinate, yBlockCoordinate)

    scaling_factor = 1

    # Converting the input image to Y, Cr, Cb format
    jpeg.convertImageWithSamplingFactor(input_image_path, converted_image_path, "4:4:4")
    width, height = Image.open(converted_image_path).size

    # array that will store the decoded image.
    output = [[0 for _ in range(width * scaling_factor)] for _ in range(height * scaling_factor)]

    # decoding the image.
    jpeg.initialize(converted_image_path, output, scaling_factor, blockCoordinate)
    jpeg.decode()

    # saving the decoded image in the images directory
    output = np.array(output).astype(np.uint8)
    cv2.imwrite('images/' + input_image_path.split('/')[-1].split('.')[0] + '_output.png', output)

    print("Output has been saved in the Images Directory.\n")

    # printing the YCrCb value at the given pixel coordinate
    image = Image.open(converted_image_path)
    print("The YCrCb value at the image pixel is: ", image.getpixel((int(imageCoordinate[0]), int(imageCoordinate[1]))))

    # printing the decoded YCrCb value at the given pixel coordinate
    image = Image.open("Images/" + input_image_path.split('/')[-1].split('.')[0] + '_output.png')
    print("The decoded YCrCb value at the image pixel is: ", image.getpixel((int(imageCoordinate[0]), int(imageCoordinate[1]))))


