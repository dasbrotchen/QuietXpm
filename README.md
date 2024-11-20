# QuietXpm
CLI tool to generate XPM files from PNG images. The aim of this repo is to eventually also allow convertion of JPEG and JPG images.

## Current progression
As of Nov. 20, 2024, the programme can generate small XPM files with at most 92 different colours. 
Original PNG image            |  XPM image converted back to PNG
:-------------------------:|:-------------------------:
![valid](https://github.com/user-attachments/assets/c8d88d29-6ef1-460e-99c4-d45188c5d858)  |  ![reconstructed](https://github.com/user-attachments/assets/5295ed5a-855e-4f1f-bfef-023c1339ca2f)

The left image is a small 124 x 124 pixels PNG that I found on my computer. I believe it is the logo of some programming course website, not sure. The right image is the PNG version of the XPM image reconstructed from the original PNG image.

## Challenges
As shown on the example above, the alpha channel (yes, the test image is RGBA) is not handled properly. Instead of seeing a transparent background, we see a black one. Similarly, the 'faded' pixels in the test image also lack the A component so they appear far brighter than they should.
Another area of concern is the number of colours. The simple current implementation uses the ASCII table starting from `#' to map pixel colors. Once we reach ~, the final printable charater in the table, colors are not properly matched. 
