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

  ImgOps();

  QImage path2image(QString path);
  QPixmap path2pixmap(QString path);
  static QImage meanFilter(const QImage &image, int kernel_size = 3);
  static QImage medianFilter(const QImage &image, int kernel_size = 3);
  static QImage maxFilter(const QImage &image, int kernel_size = 3);
  static QImage minFilter(const QImage &image, int kernel_size = 3);
  static QImage edgeDetection(const QImage &image, int unused = 0);
};

#endif // IMGOPS_H
