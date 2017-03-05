#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>
#include <QSettings>
#include <QLineEdit>

class Configurator : public QObject
{
    Q_OBJECT
public:
    explicit Configurator(QObject *parent = 0);

    void registerConfigurableWidget(QLineEdit *edit);

signals:

public slots:

private slots:
    void onEditChanged();

private:
    void fillField(QLineEdit *edt);
    void storeFieldData(const QLineEdit *edt);

private:
    QSettings settings;
};

#endif // CONFIGURATOR_H
