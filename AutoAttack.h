#ifndef QRCODE_AUTOATTACK_H
#define QRCODE_AUTOATTACK_H

#include "detect.h"
#include "locate.h"
#include <ctime>

#define SHOW_RESULT 1
#define SHOW_TWO_PARTS 0
#define USE_PIC 0
#define USE_VIDEO 1

namespace QRCode{
    class Robot{
    public:
        Robot();
        ~Robot();
        void train();
        void attack();
        void process(cv::Mat img);
        cv::Mat getImage(std::string path);
        cv::Mat getFrame();

        std::string path = "/1.mp4";

        detect* detect_;
        locate* locate_;

//        Sophus::SE3 armor2camera;
    };
}

#endif //QRCODE_AUTOATTACK_H
