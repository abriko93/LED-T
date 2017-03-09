#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>
#include <QSettings>
#include <QLineEdit>
#include <QSlider>
#include <QComboBox>
#include <QGroupBox>

class Configurator : public QObject
{
    Q_OBJECT
public:
    explicit Configurator(QObject *parent = 0);

    void registerConfigurableWidget(QLineEdit *edit);
    void registerConfigurableWidget(QSlider *slider);
    void registerConfigurableWidget(QComboBox *box);
    void registerConfigurableWidget(QGroupBox *box);

signals:

public slots:

private slots:
    void onEditChanged();
    void onSliderValueChanged(int value);
    void onComboBoxValueChanged(int idx);
    void onRadioClicked(bool);

private:
    void fillField(QLineEdit *edt);
    void fillField(QSlider *slider);
    void fillField(QComboBox *box);
    void fillField(QGroupBox *box);

    void storeFieldData(const QLineEdit *edt);
    void storeFieldData(const QSlider *slider);
    void storeFieldData(const QComboBox *box);
    void storeFieldData(const QGroupBox *box, QString value);

private:
    QSettings settings;
};

#endif // CONFIGURATOR_H
