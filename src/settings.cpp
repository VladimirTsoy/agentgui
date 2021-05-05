#include "settings.h"
#include <QDir>
#include <QString>

Settings* Settings::instance = nullptr;

Settings::Settings(QObject* parent) : QObject(parent)
{
    /// NOTE: никакие пути указывать не надо - можно просто прочитать документацию по QSettings
    /// > "Fallback Mechanism" и пр.
    this->settings = new QSettings("uisettings", QSettings::IniFormat);

    if(this->settings->contains("disableNotifications"))
    {
        this->disableNotification = settings->value("disableNotifications").toBool();
        this->enableLightTheme = settings->value("lightTheme").toBool();
        this->disableColorVerdict = settings->value("disableColorVerdict").toBool();
        this->currentLanguage = settings->value("currentLanguage").toString();
    }
    else
    {
        this->settings->setValue("disableNotifications", false);
        this->settings->setValue("lightTheme", false);
        this->settings->setValue("disableColorVerdict", false);
        this->settings->setValue("currentLanguage","ru");
        this->settings->sync();
    }
}

void Settings::changeNotificationState(int arg)
{
    this->disableNotification = arg;
    settings->setValue("disableNotifications", disableNotification);
    settings->sync();
}

void Settings::changeLightThemeState(int arg)
{
    this->enableLightTheme = arg;
    settings->setValue("lightTheme", enableLightTheme);
    settings->sync();
    emit changeUITheme();
}

void Settings::changeColorVerdictState(int arg)
{
    this->disableColorVerdict = arg;
    settings->setValue("disableColorVerdict", disableColorVerdict);
    settings->sync();
}

void Settings::changeCurrentLanguage(QString lang)
{
    this->currentLanguage = lang;
    settings->setValue("currentLanguage",currentLanguage);
    settings->sync();
}

bool Settings::getDisableColorVerdict() const
{
    return disableColorVerdict;
}

QString Settings::getCurrentLanguage() const
{
    return currentLanguage;
}

bool Settings::getEnableLightTheme() const
{
    return enableLightTheme;
}

bool Settings::getDisableNotification() const
{
    return disableNotification;
}
