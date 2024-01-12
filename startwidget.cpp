#include "startwidget.h"

#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QtWidgets>

StartWidget::StartWidget(QWidget *parent) : QWidget(parent) {
  ui->setupUi(this);
  this->installEventFilter(this);
  ui->labelA->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  ui->labelB->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

  QString imagePath = "C:\\Users\\1\\Desktop\\template.png";
  this->rawImage = ops.path2image(imagePath);
  this->imageA = rawImage;
  updatePanel(imagePath);
  qInfo() << "File opened.";
  this->imageB = rawImage;
  applyTrans(this->imageB);
  qInfo() << "File transformed.";

  this->resize(QSize(this->height() + 1, this->width() + 1));
  this->resize(QSize(this->height() - 1, this->width() - 1));

  this->setA(this->imageA);
  this->setB(this->imageB);
}

void StartWidget::updatePanel(const QString &path) {
  ui->filenameEdit->setText(path);
  QString text = QString("Resolution: (%1, %2)")
                     .arg(this->rawImage.height())
                     .arg(this->rawImage.width());
  qInfo() << text;
  ui->labelResolution->setText(text);
}

void StartWidget::applyTrans(QImage &inputImage) {
  int kernelSize = ui->kernelSizeBox->value();
  int repeatTime = ui->repeatTimeBox->value();
  auto trans = transList.at(ui->comboBox->currentIndex());
  while (repeatTime--) {
    this->imageB = trans(inputImage, kernelSize);
  }
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
}
