#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>

// A bad apple CL program which extracts the pixel colors from each frame of the Bad Apple animation and 
// plots it on the terminal window 

// This method takes all the prepared frames and outputs it onto the terminal window
void printFrames (std::vector<std::string> frames) {
    // For each frame in the vector frames
    for (const auto& frame : frames) {
        // Print out the frames
        std::cout << frame << std::endl;

        // Print out ANSI escapes. The first half ANSI escape, "\033[2J", clears the screen
        // The last half ANSI escape, "\033[1;1H", positions the cursor at row 1, column 1 
        // of the terminal screen which is the top left.
        std::cout << "\033[2J\033[1;1H";
        
        // usleep is used to suspend the current process.
        // Essentially it is used to make the frames print in a timely manner or 
        // sync with 24 frames/second
        // It takes in microseconds as its parameters.
        // Currently 41666.7 microseconds = 1/24 seconds
        usleep(41666.7);
    }
}

// This method will get the PNG files and extract pixel colors which then will be stored in a string
std::vector<std::string> getFrames (std::vector<std::string> pngImages) {

    // winsize and ioctl is used to determine the window height and width
    // winsize gives the values as column and row
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    std::cout << "Please wait a moment as the frames are being prepared! Thanks" << std::endl;

    // frames is the vector where all the frames will be stored in. 
    // double count is used for the progress indicator and keep track of how many frames were processed
    std::vector<std::string> frames; double count = 0; 
    
    // for each png file in pngImages
    for (const auto& png : pngImages) {
        // This printf statement is a progress indicator to see how much of the png files
        // has been processed.
        printf("\r[%.3f%%]", ((count / pngImages.size()) * 100));

        // cv::Mat image is a method from the OpenCV library used to read in a PNG file.
        // string frame is where the extracted data from the PNG file will be stored
        cv::Mat image = cv::imread(png); std::string frame = "";

        // The first for loop is used to get the y-value. The increment statement is scaled 
        // accordingly to fit the terminal window.
        for (int y = 0; y < image.size().height; y+= (image.size().height / w.ws_row)) {
            // string line is where the x-value data will be stored when the x-values is extracted from PNG file
            std::string line = "";
            // The second for loop is used to get the x-value. The increment statement is scaled 
            // accordinly to fit the terminal window
            for (int x = 0; x < image.size().width; x+= (image.size().width / w.ws_col)) {
                // This if statements is used to make sure the line will always have the size 
                // equal to the number of columns in the terminal window
                if (line.size() == w.ws_col) {
                    break;
                }
                // cv::Vec3b color is where a pixel color at a specified (x, y) point is stored
                cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(x, y));

                // This statement says if the color = black, add a " " to string line
                if (color == cv::Vec3b(0, 0, 0)) {
                    line += " ";
                }
                // This statement says if the color = white, add a "*" to string line
                if (color == cv::Vec3b(255, 255, 255)) {
                    line += "*";
                }
            }
            // Add escape to next line to string line
            line += "\n";
            // Add line to frame
            frame += line;
        }
        // add the frame to frames vector
        frames.push_back(frame);
        count++;
    }
    
    return frames;
}

// A helper method used compare which number is smaller in the PNG file name
bool findSmallerNumber (std::string a, std::string b) {
    // An example PNG file "bad_apple_001.png"

    // The two int indexes is used to locate the number from the PNG file name
    int aIndex = a.find_last_of("_") + 1;
    int bIndex = b.find_last_of("_") + 1;

    // The two string is used to get the actual numbers from the PNG file name
    std::string aNum = a.substr(aIndex, a.find(".") - aIndex);
    std::string bNum = b.substr(bIndex, b.find(".") - bIndex);

    // The two int values is the integer value converted from string
    int aNumValue = std::stoi(aNum);
    int bNumValue = std::stoi(bNum);

    return (aNumValue < bNumValue);
}

// The method to sort all the PNG files in order from 1 to n PNG files
void sortPNGVector(std::vector<std::string>& pngImages) {
    // sort takes three parameters and the third being an optional comparator method parameter
    // The first parameters indicates the starting and end point of the values you want to sort
    std::sort(pngImages.begin(), pngImages.end(), findSmallerNumber);
}

int main (int argc, char *argv[]) {
    try {
        // store the directory path into string path from the command line
        std::string path = argv[1];

        // A vector to store all the PNG files
        std::vector<std::string> pngImages;

        // for each PNG file in the PNG directory
        // The directory_iterator will take the file path parameter and create an iterator for 
        // the directory 
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            // Add PNG file to the vector
            pngImages.push_back(entry.path());
        }

        // Sort PNG method
        sortPNGVector(pngImages);
        // Gets the prepared frames and puts it into a vector
        std::vector<std::string> frames = getFrames(pngImages);
        // Print the frames to the terminal window
        printFrames(frames);

    }
    catch (std::exception& e) {
        std::cout << "Directory not opening" << std::endl;
    }

    return 0;
}