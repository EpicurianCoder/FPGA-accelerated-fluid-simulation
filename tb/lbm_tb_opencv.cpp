#include "src/kria_lbm.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <opencv2/opencv.hpp>

void render_frame(cv::Mat &frame, lbm_type grid_f[GRID_HEIGHT][GRID_WIDTH][NUM_DIRECTIONS])
{
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            lbm_type rho = 0.0f;
            lbm_type ux = 0.0f;
            lbm_type uy = 0.0f;

            for (int i = 0; i < NUM_DIRECTIONS; i++)
            {
                rho += grid_f[y][x][i];
                ux += grid_f[y][x][i] * dirX[i];
                uy += grid_f[y][x][i] * dirY[i];
            }

            if (rho > 0.0f)
            {
                ux /= rho;
                uy /= rho;
            }

            lbm_type speed = sqrtf(ux * ux + uy * uy);
            lbm_type min_speed = 0.00f;
            lbm_type max_speed = 0.15f;

            lbm_type v = (speed - min_speed) / (max_speed - min_speed);
            if (v < 0.0f)
                v = 0.0f;
            if (v > 1.0f)
                v = 1.0f;

            float r = 0.0f, g = 0.0f, b = 0.0f;

            if (v < 0.25f)
            {
                r = 0.0f;
                g = 4.0f * v;
                b = 1.0f;
            }
            else if (v < 0.5f)
            {
                r = 0.0f;
                g = 1.0f;
                b = 1.0f - 4.0f * (v - 0.25f);
            }
            else if (v < 0.75f)
            {
                r = 4.0f * (v - 0.5f);
                g = 1.0f;
                b = 0.0f;
            }
            else
            {
                r = 1.0f;
                g = 1.0f - 4.0f * (v - 0.75f);
                b = 0.0f;
            }

            int R = static_cast<int>(r * 255.0f);
            int G = static_cast<int>(g * 255.0f);
            int B = static_cast<int>(b * 255.0f);

            // BGR (Blue, Green, Red) byte order by default, not RGB!
            frame.at<cv::Vec3b>(y, x) = cv::Vec3b(B, G, R);
        }
    }
}

int main()
{
    std::cout << "LBM Testbench started..." << std::endl;

    static lbm_type grid_f[GRID_HEIGHT][GRID_WIDTH][NUM_DIRECTIONS] = {0};
    static lbm_type grid_new_f[GRID_HEIGHT][GRID_WIDTH][NUM_DIRECTIONS] = {0};
    static bool obstacle_map[GRID_HEIGHT][GRID_WIDTH] = {false};

    static lbm_type omega = 1.93f;
    int total_steps = 2000;
    int scale_factor = 5; // Scale the video output up by 5x

    // ---------------------------------------------
    // OpenCV VideoWriter Setup
    // ---------------------------------------------
    std::string video_filename = "./fluid_simulation.mp4";
    int fps = 300; // 300 frames per second
    cv::Size frame_size(GRID_WIDTH * scale_factor, GRID_HEIGHT * scale_factor);

    // Use the mp4v codec for standard MP4 creation
    cv::VideoWriter video(video_filename, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps, frame_size);

    if (!video.isOpened())
    {
        std::cerr << "ERROR: Could not open OpenCV VideoWriter. Check your path." << std::endl;
        return -1;
    }

    // Step 1 - Initialize the fluid State
    lbm_type init_rho = 1.0f;
    lbm_type init_ux = 0.1f;
    lbm_type init_uy = 0.0f;
    lbm_type init_u_sq = init_ux * init_ux + init_uy * init_uy;

    // obstacle 1
    // int obstacle_x = 64;
    // int obstacle_y_start = (GRID_HEIGHT / 2) - 8;
    // int obstacle_y_end = (GRID_HEIGHT / 2) + 8;

    // // obstacle 2
    // int cx = 40; // Placed near the left to leave room for the wake
    // int cy = GRID_HEIGHT / 2;
    // int r = 8; // Radius of the cylinder

    // // obstacle 3
    // int nozzle_start = 50;
    // int nozzle_end = 90;
    // int wall_thickness = 22;

    // obstacle 4
    int start_x = 40;
    int start_y = 15;
    int plate_size = 30;
    int thickness = 3;

    // // obstacle 5
    // int cx1 = 45, cy1 = (GRID_HEIGHT / 2) - 10, r1 = 6;
    // int cx2 = 45, cy2 = (GRID_HEIGHT / 2) + 10, r2 = 6;

    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            // // obstacle 1
            // if (x == obstacle_x && y >= obstacle_y_start && y <= obstacle_y_end)
            // // obstacle 2
            // if ((x - cx) * (x - cx) + (y - cy) * (y - cy) <= r * r)
            // // obstacle 3
            // if (x > nozzle_start && x < nozzle_end && (y < wall_thickness || y > GRID_HEIGHT - wall_thickness))
            // obstacle 4
            if (x >= start_x && x <= start_x + plate_size &&
                y >= start_y && y <= start_y + plate_size &&
                std::abs((x - start_x) - (y - start_y)) <= thickness)
            // // onstacle 5
            // if (((x - cx1) * (x - cx1) + (y - cy1) * (y - cy1) <= r1 * r1) ||
            //     ((x - cx2) * (x - cx2) + (y - cy2) * (y - cy2) <= r2 * r2))
            {
                obstacle_map[y][x] = true;
            }
            else
            {
                obstacle_map[y][x] = false;
            }

            for (int i = 0; i < NUM_DIRECTIONS; i++)
            {
                lbm_type cu = dirX[i] * init_ux + dirY[i] * init_uy;
                grid_f[y][x][i] = weights[i] * init_rho * (1.0f + 3.0f * cu + 4.5f * cu * cu - 1.5f * init_u_sq);
            }
        }
    }

    std::cout << "Fluid initialized. Rendering video (" << total_steps << " frames)..." << std::endl;

    // Step 2 - Run Simulation and Write Video
    // Pre-allocate the base frame outside the loop for speed
    cv::Mat base_frame(GRID_HEIGHT, GRID_WIDTH, CV_8UC3);
    cv::Mat scaled_frame;

    for (int step = 0; step < total_steps; step++)
    {
        kria_lbm_core(grid_f, grid_new_f, obstacle_map, omega);

        // Populate the base frame with colors
        render_frame(base_frame, grid_f);

        // Scale the frame up 5x using Nearest Neighbor to keep the grid looking sharp
        cv::resize(base_frame, scaled_frame, frame_size, 0, 0, cv::INTER_NEAREST);

        // Write to the video file
        video.write(scaled_frame);

        // Print a progress update every 500 frames
        if (step % 500 == 0 && step > 0)
        {
            std::cout << "Processed " << step << " frames..." << std::endl;
        }
    }

    // Safely close the video file
    video.release();
    std::cout << "Video saved to: " << video_filename << std::endl;

    // Step 3 - Result Checking
    int check_x = GRID_WIDTH / 2;
    int check_y = GRID_HEIGHT / 2;

    lbm_type final_rho = 0.0f;
    lbm_type final_ux = 0.0f;

    for (int i = 0; i < NUM_DIRECTIONS; i++)
    {
        final_rho += grid_f[check_y][check_x][i];
        final_ux += grid_f[check_y][check_x][i] * dirX[i];
    }
    final_ux /= final_rho;

    std::cout << "--- Test Complete ---" << std::endl;
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Target Density:  1.00000  | Final Density:  " << final_rho << std::endl;
    std::cout << "Target Velocity: 0.10000  | Final Velocity: " << final_ux << std::endl;

    if (final_rho > 0.99f && final_rho < 1.01f)
    {
        std::cout << "RESULT: PASS (Mass conserved)" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "RESULT: FAIL (Mass explosion detected)" << std::endl;
        return 1;
    }
}