#include <iostream>
#include <random>
#include <iomanip>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "Graphic.h"


Graphic::Graphic(std::string img_path)
{
    image = cv::imread(img_path);
    if(image.empty())
    {
        CV_Error(cv::Error::StsError, "Image file (" + img_path + ") cannot open.");
    }
    // If image is larger than 600px in width, resize it
    const int resize_w = 600;
    if(image.cols > resize_w)
    {
        int resize_h = image.rows * ((float)resize_w/(float)image.cols);
        cv::Mat image_orig = image;
        cv::resize(image_orig, image, cv::Size(resize_w, resize_h));
    }

    cv::namedWindow(kWinName, cv::WINDOW_AUTOSIZE);
} 

void Graphic::setClassColor(int class_num)
{
    //std::random_device random_device;
    //std::mt19937 random_engine(random_device());
    std::mt19937 random_engine(42);
    std::uniform_int_distribution<int> distribution(0, 255);

    for(int i = 0; i < class_num; ++i)
    {
        cv::Scalar color = cv::Scalar(distribution(random_engine),
                                      distribution(random_engine),
                                      distribution(random_engine));
        class_color.push_back(color);
    }
}
cv::Mat &Graphic::getImage()
{
    return image;
}

void Graphic::drawResult(SSDModel &ssd_model, std::vector<int> &indices)
{
    for(size_t i = 0; i < indices.size(); i++)
    {
        std::cout << i << " : class = " << ssd_model.getDetectedClassName(indices[i]) << 
                    ", conf = " << ssd_model.getDetectedConfidence(indices[i]) << std::endl;
        // Box
        cv::Rect box = ssd_model.getDetectedBox(indices[i]);
        cv::Point p1 = cv::Point(box.x, box.y);
        cv::Point p2 = cv::Point(box.x + box.width, box.y + box.height);
        int classId = ssd_model.getDetectedClassId(indices[i]);
        CV_Assert(classId < class_color.size());
        cv::rectangle(image, p1, p2, class_color[classId], 2);

        // Label
        std::ostringstream streamObj;
        streamObj << std::fixed << std::setprecision(2) << ssd_model.getDetectedConfidence(indices[i]);
        std::string label = ssd_model.getDetectedClassName(indices[i])  + " : " + streamObj.str();

        int baseLine;
        cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.38, 1.5, &baseLine);

        int top = std::max(box.y, labelSize.height);
        cv::Point lp1 = cv::Point(box.x, top - labelSize.height);
        cv::Point lp2 = cv::Point(box.x + labelSize.width, top);
        cv::rectangle(image, lp1, lp2, class_color[classId], cv::FILLED);
        cv::putText(image, label, cv::Point(box.x, top), cv::FONT_HERSHEY_SIMPLEX, 0.38, cv::Scalar(), 1.5);

    }
    cv::imshow(kWinName, image);
}