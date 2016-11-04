#ifndef VIDEO_H
#define VIDEO_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QFileInfo>
#include <QString>
#include <QImage>
#include "../value.h"

class Video : public Value {
public:
    Video(const QFileInfo& fileInfo);
    ~Video();

    bool isOpen() const;
    const QFileInfo& fileInfo() const;

    cv::Mat frameMat(int frameIndex) const;
    QImage frameImage(int frameIndex) const;
    QSizeF frameSize() const;
    int fps() const;

protected:
    void open(const QFileInfo& fileInfo);

private:
    QFileInfo _fileInfo;
    cv::VideoCapture _videoCapture;
    cv::Mat _image;
    int _frameWidth;
    int _frameHeight;
    int _frameCount;
    int _fps;
};

#endif
