#include "ImgOps.h"
#include <QList>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>

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

void ImgOps::saveImage(const QImage &image, QString path) {
  if (!(path.endsWith(".png") || path.endsWith("jpg")))
    path = path + ".png";

  image.save(path);
}

double ImgOps::calcPSNR(const QImage &image1, const QImage &image2) {
  double mse = 0.0;

  for (int y = 0; y < image1.height(); ++y) {
    for (int x = 0; x < image1.width(); ++x) {
      QRgb pixel1 = image1.pixel(x, y);
      QRgb pixel2 = image2.pixel(x, y);

      int r1 = qRed(pixel1);
      int g1 = qGreen(pixel1);
      int b1 = qBlue(pixel1);

      int r2 = qRed(pixel2);
      int g2 = qGreen(pixel2);
      int b2 = qBlue(pixel2);

      int dr = r1 - r2;
      int dg = g1 - g2;
      int db = b1 - b2;

      mse += (dr * dr + dg * dg + db * db) / 3.0;
    }
  }

  mse /= (image1.width() * image1.height());
  double psnr = 10.0 * log10((255.0 * 255.0) / mse);

  return psnr;
}

double ImgOps::calcSSIM(const QImage &image1, const QImage &image2) {
  const double C1 = 6.5025;
  const double C2 = 58.5225;

  double mean1 = 0.0, mean2 = 0.0;
  double var1 = 0.0, var2 = 0.0, covar = 0.0;

  for (int y = 0; y < image1.height(); ++y) {
    for (int x = 0; x < image1.width(); ++x) {
      QRgb pixel1 = image1.pixel(x, y);
      QRgb pixel2 = image2.pixel(x, y);

      int r1 = qRed(pixel1);
      int g1 = qGreen(pixel1);
      int b1 = qBlue(pixel1);

      int r2 = qRed(pixel2);
      int g2 = qGreen(pixel2);
      int b2 = qBlue(pixel2);

      mean1 += r1 + g1 + b1;
      mean2 += r2 + g2 + b2;
      var1 += r1 * r1 + g1 * g1 + b1 * b1;
      var2 += r2 * r2 + g2 * g2 + b2 * b2;
      covar += r1 * r2 + g1 * g2 + b1 * b2;
    }
  }

  int numPixels = image1.width() * image1.height();
  mean1 /= (3.0 * numPixels);
  mean2 /= (3.0 * numPixels);
  var1 = (var1 - 3.0 * numPixels * mean1 * mean1) / (3.0 * numPixels - 1.0);
  var2 = (var2 - 3.0 * numPixels * mean2 * mean2) / (3.0 * numPixels - 1.0);
  covar = (covar - 3.0 * numPixels * mean1 * mean2) / (3.0 * numPixels - 1.0);

  double ssim = ((2.0 * mean1 * mean2 + C1) * (2.0 * covar + C2)) /
                ((mean1 * mean1 + mean2 * mean2 + C1) * (var1 + var2 + C2));

  return ssim;
}

QImage ImgOps::addSAPNoise(const QImage &inputImage, float noiseRatio) {
  QImage noisyImage = inputImage.copy();

  int width = noisyImage.width();
  int height = noisyImage.height();
  int totalPixels = width * height;
  int noisyPixels = static_cast<int>(totalPixels * noiseRatio);

  // Generate random positions for noisy pixels
  srand(static_cast<unsigned int>(time(nullptr)));

  for (int i = 0; i < noisyPixels; ++i) {
    int x = rand() % width;
    int y = rand() % height;

    // Set pixel to black or white randomly
    int value = rand() % 2 ? 0 : 255;
    noisyImage.setPixel(x, y, qRgb(value, value, value));
  }

  return noisyImage;
}

float generateRandomGaussian() {
  static bool hasSpare = false;
  static float spare;

  if (hasSpare) {
    hasSpare = false;
    return spare;
  }

  hasSpare = true;
  static float u, v, s;
  do {
    u = (rand() / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
    v = (rand() / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
    s = u * u + v * v;
  } while (s >= 1.0 || s == 0.0);

  s = std::sqrt(-2.0f * std::log(s) / s);
  spare = v * s;

  return u * s;
}

QImage ImgOps::addStdNoise(const QImage &inputImage, float mean, float stdDev) {
  QImage noisyImage = inputImage.copy();

  int width = noisyImage.width();
  int height = noisyImage.height();

  // Generate random noise for each pixel
  srand(static_cast<unsigned int>(time(nullptr)));

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      // Get the pixel value
      QRgb pixel = noisyImage.pixel(x, y);
      int r = qRed(pixel);
      int g = qGreen(pixel);
      int b = qBlue(pixel);

      // Generate random noise using Gaussian distribution
      float noiseR = mean + stdDev * generateRandomGaussian();
      float noiseG = mean + stdDev * generateRandomGaussian();
      float noiseB = mean + stdDev * generateRandomGaussian();

      // Add noise to the pixel value
      r = qBound(0, static_cast<int>(r + noiseR), 255);
      g = qBound(0, static_cast<int>(g + noiseG), 255);
      b = qBound(0, static_cast<int>(b + noiseB), 255);

      // Set the noisy pixel value
      noisyImage.setPixel(x, y, qRgb(r, g, b));
    }
  }

  return noisyImage;
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
      std::vector<int> redValues;
      std::vector<int> greenValues;
      std::vector<int> blueValues;

      // 遍历滤波器范围内的像素
      for (int j = -halfFilterSize; j <= halfFilterSize; ++j) {
        for (int i = -halfFilterSize; i <= halfFilterSize; ++i) {
          int neighborX = x + i;
          int neighborY = y + j;

          // 检查邻居像素是否在图像范围内
          if (neighborX >= 0 && neighborX < image.width() && neighborY >= 0 &&
              neighborY < image.height()) {
            QRgb neighborPixel = image.pixel(neighborX, neighborY);
            redValues.push_back(qRed(neighborPixel));
            greenValues.push_back(qGreen(neighborPixel));
            blueValues.push_back(qBlue(neighborPixel));
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

QImage ImgOps::openOperation(const QImage &image, int kernel_size) {
  QImage filteredImage = minFilter(image, kernel_size);
  filteredImage = maxFilter(filteredImage, kernel_size);

  return filteredImage;
}

QImage ImgOps::closeOperation(const QImage &image, int kernel_size) {
  QImage filteredImage = maxFilter(image, kernel_size);
  filteredImage = minFilter(filteredImage, kernel_size);

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
