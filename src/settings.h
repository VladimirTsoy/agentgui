#ifndef SETTINGS_H
#define SETTINGS_H

#include "QSettings"
#include <QObject>

class Settings : public QObject
{  
    Q_OBJECT

public:
    bool getDisableNotification() const;
    bool getEnableLightTheme() const;
    bool getDisableColorVerdict() const;
    QString getCurrentLanguage() const;

    static Settings* getSettings()
    {
      if(!instance)
      {
          instance = new Settings();
      }
      return instance;
    }

public slots:
    void changeNotificationState(int arg);
    void changeLightThemeState(int arg);
    void changeColorVerdictState(int arg);
    void changeCurrentLanguage(QString lang);

signals:
    void changeUITheme();

private:
      bool disableNotification = false;
      bool enableLightTheme = false;
      bool disableColorVerdict = false;
      QString currentLanguage = "ru";

      QSettings * settings;

      Settings(QObject* parent = nullptr);

      static Settings* instance;
};

#endif // SETTINGS_H
