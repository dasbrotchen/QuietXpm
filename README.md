# QuietXpm
CLI tool to generate XPM files from PNG images. QuietXpm only handles RGB and RGBA images for now, the only difference being that pixels with alpha = 0 are fully transaparent.
This programme uses `ZLIB`'s `inflate()` to decompress `IDAT` chunks, and it is the only dependency. In fact, it works as a `PNG` decoder - most of the code could be adapted to fulfill another purpose, as using it to simply convert `PNG` files into `XPM` files is not that exciting.
## Usage
The programme takes a `PNG` file as its only argument, and prints the `XPM` version of that file in the terminal. This was done for simplicity and will change. You can easily create the corresponding `XPM` with an output redirection.

Example usage: `./quietxpm /path/to/my/png/file.png > file.xpm`
## Current progression
As of Nov. 30, 2024, the programme can generate XPM files with *any* number of colors.
Original PNG image            |  XPM image converted back to PNG
:-------------------------:|:-------------------------:
![wedjene](https://github.com/user-attachments/assets/9f75336a-90ed-4c8c-a97e-c3ab898e198b) | ![wejdene_quietxpm](https://github.com/user-attachments/assets/b23766bb-c8c1-4967-8512-c1446f090cef)

The left image is a small 226 x 155 pixels PNG image of Wejdene. It contains all filter types but type 0 (`None` type), multiple `IDAT` chunks, and 12029 different colors. It is thus a pretty good test case. The image on the right is the `PNG` version of the `XPM` generated from the left image. 
## Areas of improvement
Many, MANY things can be improved. The four following are, in my eyes, the most important ones.
1) Firstly, `malloc` calls are not all protected.
2) Secondly, I focused on making the whole thing _work_. Because of that, the codebase suffers from poor readability/maintainability.
3) Thirdly, tests are not automated and not rigorous enough. I am currently sending a `PNG` file in input, storing the output in an `XPM` file and converting it back to `PNG` to see if it worked. This definitely works, but could be optimised a lot to detect breaking changes.
4) Lastly, unless you have my exact setup (as the Makefile suggests), i.e. `libz.a` in `/opt/homebrew/Cellar/zlib/1.3.1/lib` which basically requires macOS, this won't compile. Improving the Makefile to have automatic dependency installation contingent on the operating system really is necessary !
## Why such a name ?
At some point, I actually needed to convert `PNG` files into `XPM` files (surprise surprise). I was doing it on shady websites with lots of ads, pop-ups, and a (too) long conversion process. I also learned that some of these websites only allow 256 colors. Overall, I thought these websites were noisy. Too much nonsense for a subpar result. This is why I chose the name QuietXpm - directly in the terminal, pretty fast, without ads. Isn't that quiet ?
