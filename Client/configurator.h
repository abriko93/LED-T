#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>
#include <QSettings>
#include <QLineEdit>
#include <QSlider>
#include <QComboBox>

class Configurator : public QObject
{
    Q_OBJECT
public:
    explicit Configurator(QObject *parent = 0);

    void registerConfigurableWidget(QLineEdit *edit);
    void registerConfigurableWidget(QSlider *slider);
    void registerConfigurableWidget(QComboBox *box);

signals:

public slots:

private slots:
    void onEditChanged();
    void onSliderValueChanged(int value);
    void onComboBoxValueChanged(int idx);

private:
    void fillField(QLineEdit *edt);
    void fillField(QSlider *slider);
    void fillField(QComboBox *box);

    void storeFieldData(const QLineEdit *edt);
    void storeFieldData(const QSlider *slider);
    void storeFieldData(const QComboBox *box);

private:
    QSettings settings;
};

#endif // CONFIGURATOR_H
