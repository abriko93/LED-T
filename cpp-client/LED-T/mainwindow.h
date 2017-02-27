#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>

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

protected:
    void setFile(QString title, QLineEdit *edt);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
