#include "AutoAttack.h"

namespace QRCode{
    Robot::Robot() {
        detect_ = new detect();
        locate_ = new locate();
    }
    Robot::~Robot(){
        delete detect_;
        delete locate_;
    }
    void Robot::train(){
        cv::Mat trainMat, labelMat;
        std::pair<cv::Mat, cv::Mat> data = locate_->loadData();
        trainMat = data.first, labelMat = data.second;
        locate_->trainSVM(trainMat, labelMat);
#if DEBUG
        std::cout << labelMat.size << labelMat << std::endl;
#endif
    }

    cv::Mat Robot::getImage(std::string path) {

    }

    void Robot::attack(){
#if USE_PIC
        for(int i=0; i<5; i++){
            cv::Mat imgR = cv::imread(locate_->root + "/R" + std::to_string(i) + ".png");
            process(imgR);
            cv::Mat imgB = cv::imread(locate_->root + "/B" + std::to_string(i) + ".png");
            process(imgB);
        }
#endif
#if USE_VIDEO
        cv::VideoCapture video;
        bool isOpen = video.open(locate_->root + path);
//        std::cout << isOpen << std::endl;
        cv::Mat img;
        int cnt = 0;
        while(video.read(img)){
            cnt++;
//            cv::imshow("img", img);
//            cv::waitKey(10);
            if(cnt % 15 == 0)
                process(img);
        }
        video.release();
#endif
        return;
    }
    void Robot::process(cv::Mat img) {
        cv::imshow("img", img);
        std::vector<cv::RotatedRect> rect;
        rect = detect_->getDirectionFlags(img);
        cv::Mat visualLabel = detect_->correctVisualLabel(img.clone(), rect);
        if(visualLabel.empty()){
            return;
        }
        std::pair<cv::Mat, cv::Mat> twoParts = locate_->divideIntoTwoParts(visualLabel);
        cv::Mat leftHalf = twoParts.first;
        cv::Mat rightHalf = twoParts.second;
        clock_t start = clock();
        int leftOcr = locate_->predict(leftHalf);
        int rightOcr = locate_->predict(rightHalf);
        std::cout << "svm time(twice):" << (double)(clock()-start) / CLOCKS_PER_SEC << std::endl;
        std::vector<int> location = locate_->getLocation(leftOcr*10+rightOcr);
        cv::putText(visualLabel, locate_->getString(location), cv::Point(10, 120), cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(255, 0, 0));
#if SHOW_TWO_PARTS
        cv::imshow("left", leftHalf);
        cv::imshow("right", rightHalf);
//        cv::waitKey(0);
#endif
#if DEBUG
        std::cout << rect.size() << std::endl;
        cv::imshow("visual label", visualLabel);
        cv::waitKey(0);
        std::cout << leftOcr << rightOcr << std::endl;
#endif
#if SHOW_RESULT
        cv::imshow("visualLable", visualLabel);
        cv::waitKey(10);
#endif
//        locate_->predict();
//        std::cout << trainMat << labelMat << std::endl;
        return;
    }

//    cv::Mat Robot::getFrame() {
//        return camera_->getFrame();
//    }
}