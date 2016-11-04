#include "video.h"
#include <QtCore>

Video::Video(const QFileInfo& fileInfo)
    : Value(fileInfo.fileName())
    , _fileInfo(fileInfo)
    , _frameWidth(0)
    , _frameHeight(0)
    , _frameCount(0)
    , _fps(0)
{
    if (_fileInfo.exists()) {
        open(fileInfo);
    }
}

Video::~Video()
{
    _videoCapture.release();
}

bool Video::isOpen() const
{
    return !_image.empty() || _videoCapture.isOpened();
}

const QFileInfo& Video::fileInfo() const
{
    return _fileInfo;
}

cv::Mat Video::frameMat(int frameIndex) const
{
    cv::Mat frame;
    if (_fps != 0) {
        // HACK: remove const to provide const-correctness for callers users,
        // as this is a read-only operation.
        cv::VideoCapture* vc = const_cast<cv::VideoCapture*>(&_videoCapture);
        vc->set(CV_CAP_PROP_POS_FRAMES, frameIndex);
        vc->read(frame);
    }
    else {
        frame = _image.clone();
    }
    return frame;
}

QImage Video::frameImage(int frameIndex) const
{
    cv::Mat frame(frameMat(frameIndex));
    QImage image(frame.data, frame.size().width, frame.size().height,
        static_cast<int>(frame.step), QImage::Format_RGB888);
    return image.rgbSwapped();
}

QSizeF Video::frameSize() const
{
    return QSizeF(_frameWidth, _frameHeight);
}

int Video::fps() const
{
    return _fps;
}

void Video::open(const QFileInfo& fileInfo)
{
    _fileInfo = fileInfo;
    if (!_fileInfo.exists()) {
        qWarning() << "Can not open file" << _fileInfo.absoluteFilePath();
        return;
    }
    // Convert to std::string without loosing characters.
    std::string stdFilePath = _fileInfo.absoluteFilePath().toUtf8().constData();
    // Try to open as image.
    _image = cv::imread(stdFilePath);
    if (!_image.empty()) {
        // Attempt to read as image was successful.
        _frameWidth = _image.cols;
        _frameHeight = _image.rows;
        _frameCount = 1;
        _fps = 0;
    }
    else if (_videoCapture.open(stdFilePath)) {
        // Attempt to read as video was successful.
        _frameWidth = _videoCapture.get(CV_CAP_PROP_FRAME_WIDTH);
        _frameHeight = _videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT);
        _frameCount = _videoCapture.get(CV_CAP_PROP_FRAME_COUNT);
        _fps = _videoCapture.get(CV_CAP_PROP_FPS);
    }
    else {
        qWarning() << "Reading" << _fileInfo.absoluteFilePath() << "failed.";
    }
}
