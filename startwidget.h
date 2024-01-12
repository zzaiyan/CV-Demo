#ifndef STARTWIDGET_H
#define STARTWIDGET_H

#include "ui_startwidget.h"
#include <ImgOps.h>
#include <QWidget>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class StartWidget;
}
QT_END_NAMESPACE

class StartWidget : public QWidget {
  Q_OBJECT

public:
  StartWidget(QWidget *parent = nullptr);
  ~StartWidget();

private:
  void setA(const QPixmap &pixmap) { _setLabel(ui->labelA, pixmap); }
  void setB(const QPixmap &pixmap) { _setLabel(ui->labelB, pixmap); }
  void _setLabel(QLabel *label, const QPixmap &pixmap);
  void setA(const QImage &image) { _setLabel(ui->labelA, image); }
  void setB(const QImage &image) { _setLabel(ui->labelB, image); }
  void _setLabel(QLabel *label, const QImage &image);
  // 转换函数
  void applyTrans(QImage &inputImage);
  // UI更新
  void updatePanel(const QString &path);

private slots:
  bool eventFilter(QObject *obj, QEvent *event) override;
  void handleResizeEvent(QResizeEvent *event);
  void on_buttonOpenFile_clicked();
  void on_buttonApply_clicked();
  void on_buttonReset_clicked();

private:
  Ui::StartWidget *ui = new Ui::StartWidget;
  ImgOps ops;
  QImage rawImage, imageA, imageB;
  std::vector<ImgOps::FilterType> transList{
      ops.meanFilter, ops.medianFilter,  ops.maxFilter,
      ops.minFilter,  ops.edgeDetection, ops.edgeDetection};
};
#endif // STARTWIDGET_H
