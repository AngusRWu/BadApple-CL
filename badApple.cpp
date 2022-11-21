#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>

// using namespace std;

void printFrames (std::vector<std::string> frames) {
    for (const auto& frame : frames) {
        std::cout << frame << std::endl;
        // CSI[2J clears screen, CSI[H moves the cursor to top-left corner
        // std::cout << "\x1B[2J\x1B[H" << std::flush;
        std::cout << "\033[2J\033[1;1H";
        usleep(41666.7);
        // std::cout << "\033[2J\033[1;1H";
    }
}
std::vector<std::string> getFrames (std::vector<std::string> pngImages) {
    // cv::Mat image;
    // image = cv::imread(pngImages[500]);
    // if(image.empty()) {
    //     std::cout << "Could not read the image: " << pngImages[0] << std::endl;
    // }
    
    // cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(0, 0));
    // std::cout << image.size().width << std::endl;
    // std::cout << image.size().height << std::endl;

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    // std::cout << w.ws_row << std::endl;
    // std::cout << w.ws_col << std::endl;
    // )
    std::cout << "Please wait a moment as the frames are being prepared! Thanks" << std::endl;
    std::vector<std::string> frames; double count = 0; 
    for (const auto& png : pngImages) {
        printf("\r[%.3f%%]", ((count / pngImages.size()) * 100));
        cv::Mat image = cv::imread(png); std::string frame = "";
        for (int y = 0; y < image.size().height; y+= (image.size().height / w.ws_row)) {
            std::string line = "";
            
            for (int x = 0; x < image.size().width; x+= (image.size().width / w.ws_col)) {
                if (line.size() == w.ws_col) {
                    break;
                }
                cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(x, y));
                if (color == cv::Vec3b(0, 0, 0)) {
                    line += " ";
                }
                if (color == cv::Vec3b(255, 255, 255)) {
                    line += "*";
                }
            }
            line += "\n";
            frame += line;
            
            // usleep(347.2225);
        }
        frames.push_back(frame);
        count++;
    }
    
    return frames;
}

bool findSmallerNumber (std::string a, std::string b) {
    int aIndex = a.find_last_of("_") + 1;
    int bIndex = b.find_last_of("_") + 1;
    std::string aNum = a.substr(aIndex, a.find(".") - aIndex);
    std::string bNum = b.substr(bIndex, b.find(".") - bIndex);

    int aNumValue = std::stoi(aNum);
    int bNumValue = std::stoi(bNum);

    return (aNumValue < bNumValue);
}

void sortPNGVector(std::vector<std::string>& pngImages) {
    std::sort(pngImages.begin(), pngImages.end(), findSmallerNumber);
}

int main (int argc, char *argv[]) {
    try {
        std::string path = argv[1];
        std::vector<std::string> pngImages;
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            
            pngImages.push_back(entry.path());
        }
        sortPNGVector(pngImages);
        std::vector<std::string> frames = getFrames(pngImages);
        printFrames(frames);

    }
    catch (std::exception& e) {
        std::cout << "Directory not opening" << std::endl;
    }

    return 0;
}