#ifndef QRCODE_SOLVEPNP_H
#define QRCODE_SOLVEPNP_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <opencv2/core/eigen.hpp>
#include "sophus/se3.h"
#include "sophus/so3.h"
//#include "transform.h"

namespace QRCode{
    class solvePNP{
    public:
        solvePNP();
        ~solvePNP();
        void solve();

        Sophus::SE3 TransformMatrix;

    private:
        void setCameraMatrix();
        void setDistCoeffs();

        cv::Mat cameraMatrix;
        cv::Mat distCoeffs;

//        transform* transform_;

    };
}

#endif //QRCODE_TRANSFORM_H