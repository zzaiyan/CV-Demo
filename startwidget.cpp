#include "startwidget.h"

#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QtWidgets>

#ifdef Q_OS_WIN
#define DEFAULT_IMAGE_PATH "C:\\Users\\1\\Desktop\\template.png"
#else
#define DEFAULT_IMAGE_PATH "/run/media/z/System/Users/1/Desktop/template.png"
#endif

StartWidget::StartWidget(QWidget *parent) : QWidget(parent) {
  ui->setupUi(this);
  this->installEventFilter(this);
  ui->labelA->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  ui->labelB->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

  QString imagePath = DEFAULT_IMAGE_PATH;
  this->rawImage = ops.path2image(imagePath);
  this->imageA = rawImage;
  updatePanel(imagePath);
  qInfo() << "File opened.";
  this->imageB = rawImage;
  applyTrans(this->imageB);
  qInfo() << "File transformed.";

  this->show();
  this->setA(this->imageA);
  this->setB(this->imageB);
}

void StartWidget::updatePanel(QString path) {
  if (path != "") {
    ui->filenameEdit->setText(path);
    QString resol = QString("Resolution: (%1, %2)")
                        .arg(this->rawImage.height())
                        .arg(this->rawImage.width());
    qDebug() << resol;
    ui->labelResolution->setText(resol);
  }
  double psnr = ops.calcPSNR(this->rawImage, this->imageB);
  double ssim = ops.calcSSIM(this->rawImage, this->imageB);
  QString metric =
      QString("PSNR: %1, SSIM: %2").arg(psnr, 0, 'f', 2).arg(ssim, 0, 'f', 4);
  qDebug() << metric;
  ui->labelMetric->setText(metric);
}

void StartWidget::applyTrans(QImage &inputImage) {
  int kernelSize = ui->kernelSizeBox->value();
  int repeatTime = ui->repeatTimeBox->value();
  int select = ui->comboBox->currentIndex();
  if (select >= 4) {
    // 前4种为滤波器，后2种为边缘检测
    // 模4: 4->0, 5->1
    kernelSize = select % 4;
  }
  auto trans = transList.at(select);
  while (repeatTime--) {
    this->imageB = trans(inputImage, kernelSize);
  }
  updatePanel();
}

void StartWidget::on_buttonApply_clicked() {
  ui->buttonApply->setDisabled(true);
  applyTrans(this->imageB);
  this->setA(this->imageA);
  this->setB(this->imageB);
  ui->buttonApply->setDisabled(false);
}

void StartWidget::handleResizeEvent(QResizeEvent *event) {
  this->setA(this->imageA);
  this->setB(this->imageB);
}

void StartWidget::_setLabel(QLabel *label, const QPixmap &pixmap) {
  QPixmap scaledPixmap = pixmap.scaled(label->size(), Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);
  label->setPixmap(scaledPixmap);
}

void StartWidget::_setLabel(QLabel *label, const QImage &image) {
  QSize labelSize = label->size(); // 获取QLabel的尺寸
  QPixmap pixmap = QPixmap::fromImage(image);
  QPixmap scaledPixmap =
      pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  label->setPixmap(scaledPixmap);
}

bool StartWidget::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::Resize && obj == this) {
    handleResizeEvent(static_cast<QResizeEvent *>(event));
  }
  return QObject::eventFilter(obj, event);
}

StartWidget::~StartWidget() { delete ui; }

void StartWidget::on_buttonOpenFile_clicked() {
  QString filePath =
      QFileDialog::getOpenFileName(nullptr, "选择文件", "", "所有文件 (*.*)");
  QImage newImage = ops.path2image(filePath);
  if (!newImage.isNull()) {
    this->rawImage = newImage;
    this->imageA = this->imageB = this->rawImage;
    updatePanel(filePath);
    this->setA(this->imageA);
    this->setB(this->imageB);
  }
}

void StartWidget::on_buttonReset_clicked() {
  this->imageA = this->imageB = this->rawImage;
  this->setA(this->imageA);
  this->setB(this->imageB);
  updatePanel();
}

void StartWidget::on_buttonStdNoise_clicked() {
  double std = ui->noiseStd->value();
  QImage noiseImage = ops.addStdNoise(this->imageA, 0., std);
  this->imageA = this->imageB = noiseImage;
  this->setA(this->imageA);
  this->setB(this->imageB);
  updatePanel();
}

void StartWidget::on_buttonSAPNoise_clicked() {
  double cover = ui->noiseRatio->value();
  QImage noiseImage = ops.addSAPNoise(this->imageA, cover);
  this->imageA = this->imageB = noiseImage;
  this->setA(this->imageA);
  this->setB(this->imageB);
  updatePanel();
}
