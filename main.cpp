/**
* Movie Wall Art
* Create a beautiful image based on a movie!
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* Written by Roger Paffrath, May 2023       
*/

#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

#ifndef MOVIE_WALL_ART
#define MOVIE_WALL_ART

#define ART_WIDTH 1080
#define ART_HEIGHT 1920

#define MOVIE_PATH "path/to/your/movie.mp4"
#define ART_PATH "path/to/your/art.png"

#define ART_STYLE_CENTER_PIXEL 1
#define ART_STYLE_AVERAGE_COLOR 2

#endif // !MOVIE_WALL_ART

/**
 * Create a column in the art image.
 *
 * @param frame A reference to the current frame of the movie that is going to be processed to create a column in the new image.
 * @param art_image A reference to the new image being created.
 * @param column_id The index of the column in the new image.
 * @param style The style to render the new image. It can be ART_STYLE_CENTER_PIXEL or ART_STYLE_AVERAGE_COLOR.
 */
void CreateArtColumn(Mat& frame, Mat& art_image, int column_id, int style) {
    try {
        int frame_h = frame.size[0];
        int frame_w = frame.size[1];
        int frame_dimension = frame_h * frame_w;

        if (style == ART_STYLE_CENTER_PIXEL) {
            Vec3b& pixel = frame.at<Vec3b>(frame_h / 2, frame_w / 2);
            
            for (int i = 0; i < ART_HEIGHT; i++)
            {
                Vec3b& new_pixel = art_image.at<Vec3b>(i, column_id);
                new_pixel[0] = pixel[0];
                new_pixel[1] = pixel[1];
                new_pixel[2] = pixel[2];
            }
        }
        else  if (style == ART_STYLE_AVERAGE_COLOR) {
            long pixel_color_b = 0.0f;
            long pixel_color_g = 0.0f;
            long pixel_color_r = 0.0f;

            for (int w = 0; w < frame_w; w++) {
                for (int h = 0; h < frame_h; h++) {
                    Vec3b& pixel = frame.at<Vec3b>(h, w);
                    pixel_color_b += pixel[0];
                    pixel_color_g += pixel[1];
                    pixel_color_r += pixel[2];
                }
            }

            for (int i = 0; i < ART_HEIGHT; i++)
            {
                Vec3b& new_pixel = art_image.at<Vec3b>(i, column_id);
                new_pixel[0] = pixel_color_b / frame_dimension;
                new_pixel[1] = pixel_color_g / frame_dimension;
                new_pixel[2] = pixel_color_r / frame_dimension;
            }
        }
        else {
            throw invalid_argument("Style not set or found.");
        }
    }
    catch (Exception e){
        cout << e.msg;
    }
}

/**
 * Starts the process of creating a new art image.
 *
 * @param movie_path The path to the movie that is going to be processed to generate the new art.
 * @param art_image A reference to the new image being created.
 */
void CreateMovieWallArt(string movie_path, Mat & art_image) {
    VideoCapture cap(movie_path);

    if (!cap.isOpened()) {
        cout << "Error opening video file." << endl;
    }
    else {
        // Getting the first frame guarantees that the properties are read correctly.
        Mat frame;
        cap >> frame;

        int frame_count = cap.get(CAP_PROP_FRAME_COUNT);
        int sample_interval = frame_count / ART_WIDTH;
        int current_frame = 0;
        int column_id = 0;

        while (current_frame < frame_count && column_id < ART_WIDTH)
        {
            cap.set(CAP_PROP_POS_FRAMES, current_frame);

            cap >> frame;

            if (frame.empty())
                break;

            CreateArtColumn(frame, art_image, column_id, ART_STYLE_AVERAGE_COLOR);
      
            current_frame += sample_interval;
            column_id++;
        }

        cap.release();
    }
}

// TODO Implement GPU processing.
int main(void) {
    Mat art_image(ART_HEIGHT, ART_WIDTH, CV_8UC3);

    CreateMovieWallArt(MOVIE_PATH, art_image);
    imwrite(ART_PATH, art_image);

	return 0;
}