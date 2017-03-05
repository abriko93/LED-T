#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <configurator.h>
#include <QGraphicsPixmapItem>

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

    void on_toToolBtn_clicked();
    void on_convertBtn_clicked();
    void on_reloadImageNeeded();

protected:
    void setFile(QString title, QLineEdit *edt);
    void prepareForm();

    void reloadImage();
    void resizeImage();

    virtual void resizeEvent(QResizeEvent *event);

private:
    Ui::MainWindow *ui;

    Configurator configurator;
    QGraphicsPixmapItem *pixmap;
};

#endif // MAINWINDOW_H
