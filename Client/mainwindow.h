#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <configurator.h>
#include <QGraphicsPixmapItem>
#include <QGroupBox>
#include <QSerialPort>

#include "image.h"
#include "converters.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_fromToolBtn_clicked();
    void on_brightnessSlider_valueChanged(int value);
    void on_saveAsImgToolBtn_clicked();
    void on_saveAsImgBtn_clicked();
    void on_saveAsTextToolBtn_clicked();
    void on_saveAsTextBtn_clicked();
    void on_saveAsBinaryToolBtn_clicked();
    void on_saveAsBinaryBtn_clicked();
    void on_transferDataBtn_clicked();

private slots:
    void onSerialReadyRead();
    void on_reloadImageNeeded();
    void on_reloadImageNeeded2(bool);

    void on_saveAsDinaryV2ToolBtn_clicked();

    void on_saveAsBinaryV2PushBtn_clicked();

protected:
    void setFile(QString title, QLineEdit *edt);
    void prepareForm();
    void fillBaudRates();
    void fillGroupBox(QGroupBox *box, QList<QString> const& data);

    void reloadImage();
    void resizeImage();
    Image getImage();
    QSharedPointer<PixmapConverter> getConverter();

    virtual void resizeEvent(QResizeEvent *event);

private:
    Ui::MainWindow *ui;

    Configurator configurator;
    QSharedPointer<QGraphicsPixmapItem> pixmapItem; // neede to render scene
    QSharedPointer<QSerialPort> serial;
};

#endif // MAINWINDOW_H
