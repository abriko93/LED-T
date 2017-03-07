#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <configurator.h>
#include <QGraphicsPixmapItem>

#include "image.h"

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
    void on_reloadImageNeeded();
    void on_brightnessSlider_valueChanged(int value);

protected:
    void setFile(QString title, QLineEdit *edt);
    void prepareForm();

    void reloadImage();
    void resizeImage();

    virtual void resizeEvent(QResizeEvent *event);

private:
    Ui::MainWindow *ui;

    Configurator configurator;
    QSharedPointer<QGraphicsPixmapItem> pixmapItem; // neede to render scene
};

#endif // MAINWINDOW_H