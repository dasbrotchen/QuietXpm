# QuietXpm
CLI tool to generate XPM files from PNG images. The aim of this repo is to eventually also allow convertion of JPEG and JPG images.

## Current progression
As of Nov. 20, 2024, the programme can generate small XPM files with at most 91 different colours. 
Original PNG image            |  XPM image converted back to PNG
:-------------------------:|:-------------------------:
![valid](https://github.com/user-attachments/assets/c8d88d29-6ef1-460e-99c4-d45188c5d858)  |  ![reconstructed](https://github.com/user-attachments/assets/f4a10d6a-0ba3-483a-a5ee-f3c5efdfe561)


The left image is a small 124 x 124 pixels PNG that I found on my computer. I believe it is the logo of some programming course website, not sure. The right image is the PNG version of the XPM image reconstructed from the original PNG image.

## Challenges
Only fully transparent pixels are handled for now. As shown above, the edges of the squares and triangles are not exactly the right colour since their alpha channel is not 255. The result shows harsher edges instead of the anti-aliasing effect of the original image.
Another area of concern is the number of colours. The simple current implementation uses the ASCII table starting from `#' to map pixel colors. Once we reach ~, the final printable charater in the table, colors are not properly matched. 
