/**
 * @file
 * @author
 * @brief
 * @note
 */
#include "detect.h"

namespace QRCode{
    detect::detect(){
//        score* score_ = new score();
//        score_->loadData();
////        score_->trainSVM(score_->trainMat, score_->labelMat);;
//        score_->display<cv::Ptr<cv::ml::SVM>>(score_->svm);
//        score_->display<cv::Ptr<cv::ml::Boost>>(score_->boost);
    }
    std::vector<cv::RotatedRect> detect::getDirectionFlags(cv::Mat image){
        cv::Mat grayImg, threshImg, blurImg, showImg = image.clone();
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::RotatedRect> minRect;
        cv::cvtColor(image, grayImg, cv::COLOR_BGR2GRAY);
        cv::threshold(grayImg, threshImg, thresh, maxval, cv::THRESH_OTSU);
        blurImg = threshImg.clone();
//        cv::GaussianBlur(threshImg, blurImg, cv::Size(5, 5), 3, 3);
//        cv::bilateralFilter(threshImg, blurImg, 10, 0, 0);
        cv::findContours(blurImg, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
//        std::vector<std::vector<cv::Point>> polyContours(contours.size());
        for(int i=0; i<contours.size(); i++){
//            cv::approxPolyDP(contours[i], polyContours[i], 20, true);
            //@TODO 更换检测策略
//            if(contours[i].size() == 11)
            if(judgeFlags(blurImg, contours[i]))
            {
                minRect.emplace_back(cv::minAreaRect(contours[i]));
                cv::drawContours(showImg, contours, i, cv::Scalar(0, 255, 0), 3);
#if FIND_LABEL_DEBUG
                cv::imshow("showImg", showImg);
                cv::waitKey(0);
                std::cout << "find one:" << contours[i] << std::endl;
#endif
            }
        }
#if CONTOURS_DEBUG
        for(int i=0; i<contours.size(); i++) {
            std::cout << contours[i].size() << std::endl;
            std::cout << judgeFlags(blurImg, contours[i]);
            for (int j = 0; j < contours[i].size(); j++) {
                std::cout << contours[i].at(j) << std::endl;
            }
        }
        cv::drawContours(showImg, contours, -1, cv::Scalar(0, 255, 0), 3);
        cv::imshow("showImg", showImg);
        cv::waitKey(0);
        std::cout << contours.size() << std::endl;
#endif
#if IMAGE_SHOW
        cv::imshow("gray", grayImg);
        cv::imshow("blur", blurImg);
        cv::imshow("thresh", threshImg);
        cv::imshow("contours", showImg);
        cv::waitKey(0);
#endif
        return minRect;
    }

    bool detect::judgeFlags(cv::Mat img, std::vector<cv::Point> contour) {
        labelFlag label;
        cv::RotatedRect minRect = cv::minAreaRect(contour);
//        cv::Rect rect = minRect.boundingRect();
//        label.w = rect.width;
//        label.h = rect.height;
//        label.widthHeightRatio = label.getWidthHeightRatio();
//        label.tl = rect.tl();
//        label.setPointsFromTopLeft(label.points);
        label.w = minRect.size.width;
        label.h = minRect.size.height;
        label.angle = -minRect.angle;
        label.widthHeightRatio = label.getWidthHeightRatio();
        label.center = minRect.center;
        label.setFivePoints(label.points);
        for(int i=0; i<5; i++) {
            cv::Point2f judgePoint = label.points[i];
//            std::cout << img.size << std::endl;
            label.judgePoints[i] = img.at<uchar>((int) judgePoint.y, (int) judgePoint.x) == 0;
        }
#if FEATURE_DEBUG
        std::cout << label.widthHeightRatio << label.points[0] << label.angle << std::endl;
        std::cout << label.judgePoints[0] << label.judgePoints[1] << label.judgePoints[2] << label.judgePoints[3] << label.judgePoints[4] << std::endl;
        std::cout << "sum " << std::accumulate(label.judgePoints.begin(), label.judgePoints.end(), 0) << std::endl;
        for(int i=0; i<5; i++){
            cv::circle(img, label.points[i], 1, cv::Scalar(255, 0, 0));
        }
        cv::imshow("points", img);
        cv::waitKey(0);
#endif
        return std::accumulate(label.judgePoints.begin(), label.judgePoints.end(), 0) == 2 &&
                label.widthHeightRatio >= 0.5 && label.widthHeightRatio <= 1.5;
    }

    cv::Mat detect::correctVisualLabel(cv::Mat img, std::vector<cv::RotatedRect> rect) {
        if(rect.size() != 3){
            // @TODO 增加筛选功能
//            std::cout << "rect.size : " << rect.size() << std::endl;
            for(int i=0; i<rect.size(); i++){
                labelFlag label;
                label.w = rect[i].size.width;
                label.h = rect[i].size.height;
                label.angle = -rect[i].angle;
                label.widthHeightRatio = label.getWidthHeightRatio();
                label.center = rect[i].center;
                label.setNinePoints(label.points);
                for(int j=0; j<9; j++) {
                    cv::Point2f judgePoint = label.points[j];
                    label.judgePoints[j] = img.at<uchar>((int) judgePoint.y, (int) judgePoint.x) == 0;
                }
                if(label.judgePoints[0] == 1 && label.judgePoints[1] == 1){
                    if(label.judgePoints[5] != 1 || label.judgePoints[6] != 1 || label.judgePoints[7] != 1 || label.judgePoints[8] == 1){
                        rect.erase(std::begin(rect)+i);
                    }
                }
                else if(label.judgePoints[0] == 1 && label.judgePoints[3] == 1){
                    if(label.judgePoints[5] != 1 || label.judgePoints[6] != 1 || label.judgePoints[8] != 1 || label.judgePoints[7] == 1){
                        rect.erase(std::begin(rect)+i);
                    }
                }
                else if(label.judgePoints[4] == 1 && label.judgePoints[3] == 1){
                    if(label.judgePoints[7] != 1 || label.judgePoints[6] != 1 || label.judgePoints[8] != 1 || label.judgePoints[5] == 1){
                        rect.erase(std::begin(rect)+i);
                    }
                }
                else if(label.judgePoints[1] == 1 && label.judgePoints[4] == 1){
                    if(label.judgePoints[5] != 1 || label.judgePoints[7] != 1 || label.judgePoints[8] != 1 || label.judgePoints[6] == 1){
                        rect.erase(std::begin(rect)+i);
                    }
                }
            }
//            std::cout << "rect.size : " << rect.size() << std::endl;
        }
        cv::Point2f cornerPoints[3][4], centerPoint[3];
//        std::vector<cv::Moments> mu(3); //@TODO 使用矩方法
        for(int i=0; i<3; i++){
            rect[i].points(cornerPoints[i]);
            centerPoint[i] = rect[i].center;
#if LABEL_DEBUG
            std::cout << cornerPoints[i][0] << cornerPoints[i][1] << cornerPoints[i][2] << cornerPoints[i][3] << std::endl;
            std::cout << centerPoint[i] << std::endl;
#endif
        }
        float innerProduct, outerProduct;
        cv::Point2f tl = {0, 0};
        cv::Point2f tr = {(float)img.cols, 0};
        cv::Point2f bl = {0, (float)img.rows};
        innerProduct = calInnerProduct(centerPoint[0], centerPoint[1], centerPoint[2]);
#if DEBUG
        std::cout << "inner " << innerProduct << std::endl;
#endif
        if(innerProduct <= 1000 && innerProduct >= -1000){
            outerProduct = calOuterProduct(centerPoint[0], centerPoint[1], centerPoint[2]);
            tl = centerPoint[0];
            tr = outerProduct>0?centerPoint[1]:centerPoint[2];
            bl = outerProduct>0?centerPoint[2]:centerPoint[1];
        }
        innerProduct = calInnerProduct(centerPoint[1], centerPoint[0], centerPoint[2]);
#if DEBUG
        std::cout << "inner " << innerProduct << std::endl;
#endif
        if(innerProduct <= 1000 && innerProduct >= -1000){
            outerProduct = calOuterProduct(centerPoint[1], centerPoint[0], centerPoint[2]);
            tl = centerPoint[1];
            tr = outerProduct>0?centerPoint[0]:centerPoint[2];
            bl = outerProduct>0?centerPoint[2]:centerPoint[0];
        }
        innerProduct = calInnerProduct(centerPoint[2], centerPoint[0], centerPoint[1]);
#if DEBUG
        std::cout << "inner " << innerProduct << std::endl;
#endif
        if(innerProduct <= 1000 && innerProduct >= -1000){
            outerProduct = calOuterProduct(centerPoint[2], centerPoint[0], centerPoint[1]);
            tl = centerPoint[2];
            tr = outerProduct>0?centerPoint[0]:centerPoint[1];
            bl = outerProduct>0?centerPoint[1]:centerPoint[0];
        }
        if(tl.x > tr.x){
            cv::flip(img, img, -1);
            cv::Point2f center = {(float)img.cols/2, (float)img.rows/2};
            tl = flipPoint(center, tl);
            tr = flipPoint(center, tr);
            bl = flipPoint(center, bl);
        }

//        std::cout << "tl" << tl << "tr" << tr << "bl" << bl << std::endl;
        return getROI(img, tl, tr, bl);
    }

    float detect::calInnerProduct(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3) {
        return (p2.x-p1.x)*(p3.x-p1.x) +
                (p2.y-p1.y)*(p3.y-p1.y);
    }
    float detect::calOuterProduct(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3) {
        return (p2.x-p1.x)*(p3.y-p1.y) -
                (p3.x-p1.x)*(p2.y-p1.y);
    }
    cv::Point2f detect::flipPoint(cv::Point2f center, cv::Point2f point) {
        point.x = center.x*2 - point.x;
        point.y = center.y*2 - point.y;
        return point;
    }

    cv::Mat detect::getROI(cv::Mat img, cv::Point2f tl, cv::Point2f tr, cv::Point2f bl) {
        int width = (int)(tr.x-tl.x);
        int height = (int)(bl.y-tl.y);
        if(width <= 0 || height <= 0){
            return img;
        }
        cv::Mat warpMat = cv::Mat(2, 3, CV_32FC1);
        cv::Mat warpImg = img.clone();
        cv::Point2f srcPoints[3] = {tl, tr, bl};
        cv::Point2f dstPoints[3] = {tl, {tl.x+width, tl.y}, {tl.x, tl.y+height}};
        warpMat = cv::getAffineTransform(srcPoints, dstPoints);
        cv::warpAffine(img, warpImg, warpMat, warpImg.size());
        cv::Mat ROI = warpImg(cv::Rect((int)tl.x, (int)tl.y, width, height));
        if(ROI.empty()){
            return ROI;
        }
        cv::resize(ROI, ROI, cv::Size(256, 128));
#if SHOW_ROI
        for(int i=0; i<3; i++) {
            cv::circle(warpImg, dstPoints[i], 1, cv::Scalar(255, 0, 0));
        }
        cv::imshow("warpImg", warpImg);
        cv::imshow("ROI", ROI);
        cv::waitKey(0);
#endif
        return ROI;
    }
}