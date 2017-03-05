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

    qDebug() << "Registered by configurator widget:" << edit->objectName();
}

void Configurator::onEditChanged()
{
    qDebug() << "Editing finished emitted by" << sender()->objectName();

    QLineEdit *senderObj = qobject_cast<QLineEdit *>(sender());
    if (senderObj == NULL)
    {
        qWarning() << "Invalid line edit found in configurator:" << sender()->objectName();
        return;
    }

    storeFieldData(senderObj);
}

QString getDefaultFieldData(const QObject *obj)
{
    return "defaults/" + obj->objectName();
}

void Configurator::fillField(QLineEdit *edt)
{
    QVariant data = settings.value(getDefaultFieldData(edt));
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
    settings.setValue(getDefaultFieldData(edt), edt->text());
}
