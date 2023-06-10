# MovieWallArt
 A simple program to create a piece of art with your favorite movie's frames.

## Configuration
All the parameters for the program are set in the constants defined at the beginning of the main.cpp file.

## Art Generation Styles
There are currently two ways to generate your art image.
- ART_STYLE_CENTER_PIXEL: this takes the color of the centered pixel on each movie frame and applies it to the art image column.
- ART_STYLE_AVERAGE_COLOR: this calculates the average color of the whole frame to apply to the art image column.
- ART_STYLE_PIXEL_STRIP: this makes strips of pixels to fill the columns based on the average color of segments of the frame.

## Have Fun!
Feel free to get in touch and share your creations.