#include "configurator.h"

#include <QValidator>
#include <QtDebug>

Configurator::Configurator(QObject *parent)
    : QObject(parent)
    , settings(QSettings::IniFormat, QSettings::UserScope, "")
{
}

void Configurator::registerConfigurableWidget(QLineEdit *edit)
{
    connect(edit, SIGNAL(editingFinished()), this, SLOT(onEditChanged()));
    fillField(edit);
}

void Configurator::registerConfigurableWidget(QSlider *slider)
{
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
    fillField(slider);
}

void Configurator::onSliderValueChanged(int)
{
    QSlider *senderObj = qobject_cast<QSlider *>(sender());
    if (senderObj == NULL)
    {
        qWarning() << "Invalid slider found in configurator:" << sender()->objectName();
        return;
    }

    storeFieldData(senderObj);
}

void Configurator::onEditChanged()
{
    QLineEdit *senderObj = qobject_cast<QLineEdit *>(sender());
    if (senderObj == NULL)
    {
        qWarning() << "Invalid line edit found in configurator:" << sender()->objectName();
        return;
    }

    storeFieldData(senderObj);
}

QString getDefaultFieldDataOptName(const QObject *obj)
{
    return "defaults/" + obj->objectName();
}

void Configurator::fillField(QSlider *slider)
{
    QVariant data = settings.value(getDefaultFieldDataOptName(slider));
    if (data.canConvert(QVariant::Int))
    {
        int intData = data.toInt();

        if (intData < slider->minimum() || intData > slider->maximum())
        {
            qWarning() << "Can't fill slider" << slider->objectName() << ": invalid data: " << intData;
            return;
        }

        slider->setValue(intData);
        slider->valueChanged(intData);
    } else {
        qWarning() << "Invalid value found for slider" << slider->objectName() << ":" << data;
    }
}

void Configurator::fillField(QLineEdit *edt)
{
    QVariant data = settings.value(getDefaultFieldDataOptName(edt));
    if (data.canConvert(QVariant::String))
    {
        QString strData = data.toString();

        const QValidator *validator = edt->validator();
        int pos = 0;

        if (validator && validator->validate(strData, pos) == QValidator::Invalid)
        {
            qWarning() << "Can't fill" << edt->objectName() << ": invalid data " << strData;
            return;
        }

        edt->setText(strData);
    } else {
        qWarning() << "Can't set" << edt->objectName() << "text. Value is " << data;
    }
}

void Configurator::storeFieldData(const QLineEdit *edt)
{
    settings.setValue(getDefaultFieldDataOptName(edt), edt->text());
}

void Configurator::storeFieldData(const QSlider *slider)
{
    settings.setValue(getDefaultFieldDataOptName(slider), slider->value());
}