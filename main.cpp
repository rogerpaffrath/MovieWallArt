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
* This project uses OpenCV - https://opencv.org/
*
* Written by Roger Paffrath, May 2023       
*/

#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

#ifndef MOVIE_WALL_ART
#define MOVIE_WALL_ART

#define ART_WIDTH 1920
#define ART_HEIGHT 1080

#define MOVIE_PATH "path/to/your/movie.mp4"
#define ART_PATH "path/to/your/art.png"

#define ART_STYLE_CENTER_PIXEL 1
#define ART_STYLE_AVERAGE_COLOR 2
#define ART_STYLE_PIXEL_STRIP 3

#endif // !MOVIE_WALL_ART

/**
 * Get the average color of a frame.
 *
 * @param frame A reference to the current frame of the movie that is going to be processed to create a column in the new image.
 */
Vec3b GetFrameAverageColor(Mat& frame) {
    int frame_h = frame.size[0];
    int frame_w = frame.size[1];
    int frame_dimension = frame_h * frame_w;

    float pixel_color_b = 0.0f;
    float pixel_color_g = 0.0f;
    float pixel_color_r = 0.0f;

    Vec3b average_color;

    for (int w = 0; w < frame_w; w++) {
        for (int h = 0; h < frame_h; h++) {
            Vec3b& pixel = frame.at<Vec3b>(h, w);
            pixel_color_b += pixel[0];
            pixel_color_g += pixel[1];
            pixel_color_r += pixel[2];
        }
    }

    average_color[0] = pixel_color_b / frame_dimension;
    average_color[1] = pixel_color_g / frame_dimension;
    average_color[2] = pixel_color_r / frame_dimension;

    return average_color;
}

/**
 * Get the pixel strip of a frame.
 *
 * @param frame A reference to the current frame of the movie that is going to be processed to create a column in the new image.
 */
vector<Vec3b> GetFramePixelStrip(Mat& frame, int strip_size) {
    vector<Vec3b> pixel_strip(strip_size);

    int frame_h = frame.size[0];
    int frame_w = frame.size[1];
    int frame_dimension = frame_h * frame_w;

    int sample_interval = frame_dimension / strip_size;
    int strip_interval = sample_interval / strip_size;
    int count = 0;
    int strip_index = 0;

    float g = 0.0f;
    float b = 0.0f;
    float r = 0.0f;

    for (int x = 0; x < frame_w; x++) {
        for (int y = 0; y < frame_h; y++) {
            Vec3b pixel = frame.at<Vec3b>(y, x);

            g += pixel[0];
            b += pixel[1];
            r += pixel[2];

            count++;

            if (count > sample_interval) {
                g = g / count;
                b = b / count;
                r = r / count;

                for (int i = 0; i < strip_interval; i++) {
                    if (strip_index < strip_size) {
                        pixel_strip[strip_index] = Vec3b(g, b, r);
                        strip_index++;
                    }
                }

                count = 0;
                g = 0.0f;
                b = 0.0f;
                r = 0.0f;
            }
        }
    }

    return pixel_strip;
}

/**
 * Create a column in the art image.
 *
 * @param frame A reference to the current frame of the movie that is going to be processed to create a column in the new image.
 * @param art_image A reference to the new image being created.
 * @param column_id The index of the column in the new image.
 * @param style The style to render the new image. It can be ART_STYLE_CENTER_PIXEL or ART_STYLE_AVERAGE_COLOR.
 */
void CreateArtColumn(Mat& frame, Mat& art_image, int column_id, int style = ART_STYLE_AVERAGE_COLOR) {
    try {
        if (style == ART_STYLE_CENTER_PIXEL) {
            int frame_h = frame.size[0];
            int frame_w = frame.size[1];

            Vec3b column_color;

            column_color = frame.at<Vec3b>(frame_h / 2, frame_w / 2);
            
            for (int i = 0; i < ART_HEIGHT; i++)
            {
                Vec3b& pixel = art_image.at<Vec3b>(i, column_id);
                pixel = column_color;
            }
        }
        else  if (style == ART_STYLE_AVERAGE_COLOR) {
            Vec3b column_color = GetFrameAverageColor(frame);

            for (int i = 0; i < ART_HEIGHT; i++)
            {
                Vec3b& pixel = art_image.at<Vec3b>(i, column_id);
                pixel = column_color;
            }
        }
        else if (style == ART_STYLE_PIXEL_STRIP) {
            vector<Vec3b> column_colors = GetFramePixelStrip(frame, ART_HEIGHT);

            for (int i = 0; i < ART_HEIGHT; i++)
            {
                Vec3b& pixel = art_image.at<Vec3b>(i, column_id);
                pixel = column_colors[i];
            }
        }
        else {
            throw invalid_argument("Style not found.");
        }

        imshow("FRAME", frame);
        imshow("RENDERING...", art_image);
        waitKey(1);
    }
    catch (Exception e) {
        cout << e.msg;
    }
}

/**
 * Starts the process of creating a new art image.
 *
 * @param movie_path The path to the movie that is going to be processed to generate the new art.
 * @param art_image A reference to the new image being created.
 */
void CreateMovieWallArt(string movie_path, Mat& art_image) {
    VideoCapture cap(movie_path);

    if (!cap.isOpened()) {
        cout << "Error opening video file." << endl;
    }
    else {
        // Getting the first frame guarantees that the properties are read correctly.
        Mat frame(cap.get(CAP_PROP_FRAME_HEIGHT), cap.get(CAP_PROP_FRAME_WIDTH), CV_8UC3, USAGE_ALLOCATE_HOST_MEMORY);
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

            CreateArtColumn(frame, art_image, column_id, ART_STYLE_PIXEL_STRIP);

            current_frame += sample_interval;
            column_id++;
        }

        cap.release();
        waitKey(0);
    }
}

// TODO Implement series and TV Shows.
int main(void) {
    Mat art_image(ART_HEIGHT, ART_WIDTH, CV_8UC3, USAGE_ALLOCATE_HOST_MEMORY);

    CreateMovieWallArt(MOVIE_PATH, art_image);
    imwrite(ART_PATH, art_image);

    destroyAllWindows();

	return 0;
}