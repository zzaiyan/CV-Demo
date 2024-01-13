#ifndef IMGOPS_H
#define IMGOPS_H

#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QtWidgets>
#include <functional>

class ImgOps {
public:
  using FilterType = std::function<QImage(const QImage &, int)>;

  // 读写文件
  static QImage path2image(QString path);
  static QPixmap path2pixmap(QString path);
  static void saveImage(const QImage &image);
  // 评估指标
  static double calcPSNR(const QImage &image1, const QImage &image2);
  static double calcSSIM(const QImage &image1, const QImage &image2);

  // 图像变换
  static QImage addSAPNoise(const QImage &inputImage, float noiseRatio = 0.1);
  static QImage addStdNoise(const QImage &inputImage, float mean = 0,
                            float stdDev = 1);
  static QImage meanFilter(const QImage &image, int kernel_size = 3);
  static QImage medianFilter(const QImage &image, int kernel_size = 3);
  static QImage maxFilter(const QImage &image, int kernel_size = 3);
  static QImage minFilter(const QImage &image, int kernel_size = 3);
  static QImage edgeDetection(const QImage &image, int mode = 0);
};

#endif // IMGOPS_H
