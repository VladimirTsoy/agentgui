#ifndef LOCALCLIENT_H
#define LOCALCLIENT_H

//#include <QObject>
#include <QLocalSocket>
#include <QTimer>

#define TIME_TO_CONNECT 15000 //msec

#include "Core/file.h"
#include "Deps/message_head.h"


class LocalClient : public QObject
{
    Q_OBJECT

public:
    LocalClient(QString server_name, QObject* parent = 0);
    ~LocalClient();

    bool isConnected();

    friend QDataStream& operator <<(QDataStream& stream,const message_head& head);
    friend QDataStream& operator >>(QDataStream& stream, message_head& head);

    friend QDataStream& operator <<(QDataStream& stream,const File& file);
    friend QDataStream& operator >>(QDataStream& stream, File& file);

private:
    QString _serverName;

    QLocalSocket* localSocket;
    quint16 sz_package;

    QTimer* tmrToConnect;

public slots:
    // Единственное что мы отправляем - файлы на проверку
    void sendFileToServer(const File &file);
    void sendCommandToServer(const QString &cmd);

    void slotScanBtnPressed();

private slots:
    void slotToConnect();
    void slotConnected();
    void slotDisconnected();

    void socketStateChanged(QLocalSocket::LocalSocketState state);

    // а вот получаем - инфу по объектам
    // device_info / file / agent / license
    // (std::string pluginName, std::string json)
    void slotReadyRead();
    // вот там и разбирать json - и оттуда пулять сигналы на форму

    void slotError(QLocalSocket::LocalSocketError error);

signals:
    void signalConnected(bool);

    void signalNewPluginInfo(QString pluginName, short pluginState);
//    void signalNewInfo(std::string pluginName, std::string json);
    void signalNewInfo(QString pluginName, QString json);
    void signalNewFile(File file);

    void signalEnableScanBtn(bool);
};


#endif // LOCALCLIENT_H
