#include "ImgOps.h"
#include <QList>
#include <algorithm>
#include <array>

// ImgOps::ImgOps() {}

QImage ImgOps::path2image(QString path) {
  QImage image(path);
  if (image.isNull()) {
    qWarning() << "图片读取错误！";
  }

  return image;
}

QPixmap ImgOps::path2pixmap(QString path) {
  QImage image(path);
  if (image.isNull()) {
    qWarning() << "图片读取错误！";
  }
  QPixmap pixmap = QPixmap::fromImage(image);
  return pixmap;
}

QImage ImgOps::meanFilter(const QImage &image, int kernel_size) {
  QImage filteredImage(image.size(), image.format());
  int filterSize = kernel_size; // 滤波器的大小，这里以3x3为例
  int halfFilterSize = filterSize / 2;
  int stride = 1;

  for (int y = 0; y < image.height(); y += stride) {
    for (int x = 0; x < image.width(); x += stride) {
      int sumRed = 0;
      int sumGreen = 0;
      int sumBlue = 0;
      int pixelCount = 0;

      // 遍历滤波器范围内的像素
      for (int j = -halfFilterSize; j <= halfFilterSize; ++j) {
        for (int i = -halfFilterSize; i <= halfFilterSize; ++i) {
          int neighborX = x + i;
          int neighborY = y + j;

          // 检查邻居像素是否在图像范围内
          if (neighborX >= 0 && neighborX < image.width() && neighborY >= 0 &&
              neighborY < image.height()) {
            QRgb neighborPixel = image.pixel(neighborX, neighborY);
            sumRed += qRed(neighborPixel);
            sumGreen += qGreen(neighborPixel);
            sumBlue += qBlue(neighborPixel);
            ++pixelCount;
          }
        }
      }

      // 计算均值并将结果应用到新图像中
      int avgRed = sumRed / pixelCount;
      int avgGreen = sumGreen / pixelCount;
      int avgBlue = sumBlue / pixelCount;
      QRgb filteredPixel = qRgb(avgRed, avgGreen, avgBlue);
      filteredImage.setPixel(x, y, filteredPixel);
    }
  }
  return filteredImage;
}

QImage ImgOps::medianFilter(const QImage &image, int kernel_size) {
  QImage filteredImage(image.size(), image.format());
  int filterSize = kernel_size; // 滤波器的大小，这里以3x3为例
  int halfFilterSize = filterSize / 2;
  int stride = 1;

  for (int y = 0; y < image.height(); y += stride) {
    for (int x = 0; x < image.width(); x += stride) {
      QList<int> redValues;
      QList<int> greenValues;
      QList<int> blueValues;

      // 遍历滤波器范围内的像素
      for (int j = -halfFilterSize; j <= halfFilterSize; ++j) {
        for (int i = -halfFilterSize; i <= halfFilterSize; ++i) {
          int neighborX = x + i;
          int neighborY = y + j;

          // 检查邻居像素是否在图像范围内
          if (neighborX >= 0 && neighborX < image.width() && neighborY >= 0 &&
              neighborY < image.height()) {
            QRgb neighborPixel = image.pixel(neighborX, neighborY);
            redValues.append(qRed(neighborPixel));
            greenValues.append(qGreen(neighborPixel));
            blueValues.append(qBlue(neighborPixel));
          }
        }
      }

      // 对像素值进行排序
      std::sort(redValues.begin(), redValues.end());
      std::sort(greenValues.begin(), greenValues.end());
      std::sort(blueValues.begin(), blueValues.end());

      // 获取排序后的中间值作为滤波后的像素值
      int medianRed = redValues.at(redValues.size() / 2);
      int medianGreen = greenValues.at(greenValues.size() / 2);
      int medianBlue = blueValues.at(blueValues.size() / 2);

      QRgb filteredPixel = qRgb(medianRed, medianGreen, medianBlue);
      filteredImage.setPixel(x, y, filteredPixel);
    }
  }

  return filteredImage;
}

QImage ImgOps::maxFilter(const QImage &image, int kernel_size) {
  QImage filteredImage(image.size(), image.format());
  int filterSize = kernel_size; // 滤波器的大小，这里以3x3为例
  int halfFilterSize = filterSize / 2;
  int stride = 1;

  for (int y = 0; y < image.height(); y += stride) {
    for (int x = 0; x < image.width(); x += stride) {
      int maxRed = 0;
      int maxGreen = 0;
      int maxBlue = 0;

      // 遍历滤波器范围内的像素
      for (int j = -halfFilterSize; j <= halfFilterSize; ++j) {
        for (int i = -halfFilterSize; i <= halfFilterSize; ++i) {
          int neighborX = x + i;
          int neighborY = y + j;

          // 检查邻居像素是否在图像范围内
          if (neighborX >= 0 && neighborX < image.width() && neighborY >= 0 &&
              neighborY < image.height()) {
            QRgb neighborPixel = image.pixel(neighborX, neighborY);
            maxRed = qMax(maxRed, qRed(neighborPixel));
            maxGreen = qMax(maxGreen, qGreen(neighborPixel));
            maxBlue = qMax(maxBlue, qBlue(neighborPixel));
          }
        }
      }

      QRgb filteredPixel = qRgb(maxRed, maxGreen, maxBlue);
      filteredImage.setPixel(x, y, filteredPixel);
    }
  }

  return filteredImage;
}

QImage ImgOps::minFilter(const QImage &image, int kernel_size) {
  QImage filteredImage(image.size(), image.format());
  int filterSize = kernel_size; // 滤波器的大小，这里以3x3为例
  int halfFilterSize = filterSize / 2;
  int stride = 1;

  for (int y = 0; y < image.height(); y += stride) {
    for (int x = 0; x < image.width(); x += stride) {
      int minRed = 255;
      int minGreen = 255;
      int minBlue = 255;

      // 遍历滤波器范围内的像素
      for (int j = -halfFilterSize; j <= halfFilterSize; ++j) {
        for (int i = -halfFilterSize; i <= halfFilterSize; ++i) {
          int neighborX = x + i;
          int neighborY = y + j;

          // 检查邻居像素是否在图像范围内
          if (neighborX >= 0 && neighborX < image.width() && neighborY >= 0 &&
              neighborY < image.height()) {
            QRgb neighborPixel = image.pixel(neighborX, neighborY);
            minRed = qMin(minRed, qRed(neighborPixel));
            minGreen = qMin(minGreen, qGreen(neighborPixel));
            minBlue = qMin(minBlue, qBlue(neighborPixel));
          }
        }
      }

      QRgb filteredPixel = qRgb(minRed, minGreen, minBlue);
      filteredImage.setPixel(x, y, filteredPixel);
    }
  }

  return filteredImage;
}

// QImage ImgOps::edgeDetection(const QImage &image, int unused) {
//   // Convert QImage to grayscale
//   QImage grayImage = image.convertToFormat(QImage::Format_Grayscale8);
//   // QImage grayImage = image;
//   qInfo() << grayImage.size();

//   // Apply Sobel operator for horizontal and vertical edges
//   QImage edgesImage = grayImage;
//   qInfo() << "convertToFormat";

//   int width = edgesImage.width();
//   int height = edgesImage.height();

//   for (int y = 1; y < height - 1; ++y) {
//     qInfo() << QString("y = %1").arg(y);
//     for (int x = 1; x < width - 1; ++x) {
//       // Apply the Sobel operator
//       int gx = (qGray(edgesImage.pixel(x + 1, y - 1)) +
//                 2 * qGray(edgesImage.pixel(x + 1, y)) +
//                 qGray(edgesImage.pixel(x + 1, y + 1))) -
//                (qGray(edgesImage.pixel(x - 1, y - 1)) +
//                 2 * qGray(edgesImage.pixel(x - 1, y)) +
//                 qGray(edgesImage.pixel(x - 1, y + 1)));

//       int gy = (qGray(edgesImage.pixel(x - 1, y + 1)) +
//                 2 * qGray(edgesImage.pixel(x, y + 1)) +
//                 qGray(edgesImage.pixel(x + 1, y + 1))) -
//                (qGray(edgesImage.pixel(x - 1, y - 1)) +
//                 2 * qGray(edgesImage.pixel(x, y - 1)) +
//                 qGray(edgesImage.pixel(x + 1, y - 1)));

//       int magnitude = qBound(0, qAbs(gx) + qAbs(gy), 255);

//       edgesImage.setPixel(x, y, qRgb(magnitude, magnitude, magnitude));
//     }
//   }

//   // Convert back to RGB for display
//   QImage outputImage = edgesImage.convertToFormat(QImage::Format_RGB888);

//   return outputImage;
// }

QImage ImgOps::edgeDetection(const QImage &image, int mode) {
  QImage edgeImage(image.size(), image.format());

  using Kernel = std::array<std::array<int, 3>, 3>;

  Kernel kernelX, kernelY;
  // 定义Prewitt算子
  Kernel prewittX = {std::array<int, 3>{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}};
  Kernel prewittY = {std::array<int, 3>{-1, -1, -1}, {0, 0, 0}, {1, 1, 1}};
  // 定义Sobel算子
  Kernel sobelX = {std::array<int, 3>{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
  Kernel sobelY = {std::array<int, 3>{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

  qDebug() << "mode = " << mode;
  if (mode == 0) {
    qDebug() << "Prewitt";
    kernelX = prewittX;
    kernelY = prewittY;
  } else if (mode == 1) {
    qDebug() << "Sobel";
    kernelX = sobelX;
    kernelY = sobelY;
  }

  for (int y = 1; y < image.height() - 1; ++y) {
    for (int x = 1; x < image.width() - 1; ++x) {
      int gxRed = 0;
      int gxGreen = 0;
      int gxBlue = 0;
      int gyRed = 0;
      int gyGreen = 0;
      int gyBlue = 0;

      // 应用Sobel算子
      for (int j = -1; j <= 1; ++j) {
        for (int i = -1; i <= 1; ++i) {
          int neighborX = x + i;
          int neighborY = y + j;
          QRgb neighborPixel = image.pixel(neighborX, neighborY);
          int neighborRed = qRed(neighborPixel);
          int neighborGreen = qGreen(neighborPixel);
          int neighborBlue = qBlue(neighborPixel);

          gxRed += kernelX[j + 1][i + 1] * neighborRed;
          gxGreen += kernelX[j + 1][i + 1] * neighborGreen;
          gxBlue += kernelX[j + 1][i + 1] * neighborBlue;

          gyRed += kernelY[j + 1][i + 1] * neighborRed;
          gyGreen += kernelY[j + 1][i + 1] * neighborGreen;
          gyBlue += kernelY[j + 1][i + 1] * neighborBlue;
        }
      }

      // 计算梯度幅值
      int gradientRed = qAbs(gxRed) + qAbs(gyRed);
      int gradientGreen = qAbs(gxGreen) + qAbs(gyGreen);
      int gradientBlue = qAbs(gxBlue) + qAbs(gyBlue);

      // 将梯度幅值限制在0-255范围内
      gradientRed = qBound(0, gradientRed, 255);
      gradientGreen = qBound(0, gradientGreen, 255);
      gradientBlue = qBound(0, gradientBlue, 255);

      QRgb edgePixel = qRgb(gradientRed, gradientGreen, gradientBlue);
      edgeImage.setPixel(x, y, edgePixel);
    }
  }

  return edgeImage;
}
