#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>
#include <QSettings>
#include <QLineEdit>
#include <QSlider>

class Configurator : public QObject
{
    Q_OBJECT
public:
    explicit Configurator(QObject *parent = 0);

    void registerConfigurableWidget(QLineEdit *edit);
    void registerConfigurableWidget(QSlider *slider);

signals:

public slots:

private slots:
    void onEditChanged();
    void onSliderValueChanged(int value);

private:
    void fillField(QLineEdit *edt);
    void fillField(QSlider *slider);

    void storeFieldData(const QLineEdit *edt);
    void storeFieldData(const QSlider *slider);

private:
    QSettings settings;
};

#endif // CONFIGURATOR_H
