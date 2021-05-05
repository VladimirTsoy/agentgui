#include "localclient.h"

#include <QDebug>

#include "Deps/datastream.h"


LocalClient::LocalClient(QString serverName, QObject* parent)
    : QObject(parent), _serverName(serverName), sz_package(0)
{
    qDebug() << "----------- Connection to local socket -----------";
    localSocket = new QLocalSocket(this);

    connect(localSocket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error),
            this, &LocalClient::slotError);

//    localSocket->setServerName(serverName);
    // так работает только для одного включения - потом слетает

    connect(localSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(localSocket, SIGNAL(disconnected()), SLOT(slotDisconnected()));
    connect(localSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));

    connect(localSocket, &QLocalSocket::stateChanged,
            this, &LocalClient::socketStateChanged);

    connect(localSocket, &QLocalSocket::connected,
            this, [=](){ emit signalConnected(true); });
    connect(localSocket, &QLocalSocket::disconnected,
            this, [=](){ emit signalConnected(false); });

    qDebug() << "ServerName = " << localSocket->serverName();
    qDebug() << "Create timer connect";
    tmrToConnect = new QTimer(this);
    tmrToConnect->setSingleShot(true); // !! по умолчанию false
    // по таймеру делаем попытки подключения к AthenaAgent
    connect(tmrToConnect, SIGNAL(timeout()), this, SLOT(slotToConnect()));
    localSocket->connectToServer(serverName);
}

LocalClient::~LocalClient()
{
}

// этот слот только потому, что напрямую нельзя связать сигнал и слот (там параметры по умолчанию)
void LocalClient::slotToConnect()
{
    qDebug() << "\n Try to connect Interval = " << tmrToConnect->interval() ;
    qDebug() << "_serverName = " << _serverName << "\n";
    localSocket->connectToServer(_serverName);
}

void LocalClient::socketStateChanged(QLocalSocket::LocalSocketState state)
{
    switch(state)
    {
    case QLocalSocket::UnconnectedState:
        tmrToConnect->start(TIME_TO_CONNECT);
        break;
    case QLocalSocket::ConnectingState:
    case QLocalSocket::ConnectedState:
    case QLocalSocket::ClosingState:
        if(tmrToConnect->isActive())
            tmrToConnect->stop();
        break;
    }
}

bool LocalClient::isConnected()
{
    if(localSocket)
        return (localSocket->state() == QLocalSocket::ConnectedState);
    return false;
}

void LocalClient::slotError(QLocalSocket::LocalSocketError error)
{
    QString strError =
        "Error: " + (error == QLocalSocket::ServerNotFoundError ?
                     "The server was not found." :
                     error == QLocalSocket::PeerClosedError ?
                     "The server is closed." :
                     error == QLocalSocket::ConnectionRefusedError ?
                     "The connection was refused." :
                     QString(localSocket->errorString()));
    qDebug() << strError;
}


/// NOTE: там есть максимальный размер пакета в передаче..
// Слот чтения информации, получаемой от сервера
void LocalClient::slotReadyRead()
{
    qDebug() << "LocalClient::slotReadyRead()";
    message_head head;
    QString str_info;

    File file;

    QString pluginName;
    short pluginState;
    QString json;
    // заморочка в том, что json - std::string
    //      а для Qt "in >>" определены только Qstring и пр..
    QString command;

    QDataStream in(localSocket);
    in.setVersion(QDataStream::Qt_5_3);
    for(;;)
    {
        if(!sz_package) {
            if(localSocket->bytesAvailable() < (int)sizeof(quint16))
                break;
            in >> sz_package;
        }
        if(sz_package < sizeof(message_head)) {
            // величина пакета меньше величины заголовка пакета,
            // которые мы пересылаем
            qDebug() << "socket_client: Wrong package.";
            break;
        }
        if(localSocket->bytesAvailable() < sz_package)
            break;

        in >> head;
        switch(head.type)
        {
        case msg_string:
            in >> str_info;
            qDebug() << str_info;
            break;
        case msg_plugin_info:
            in >> pluginName;
            in >> pluginState;
            emit signalNewPluginInfo(pluginName, pluginState);
            break;
        case msg_file:
            in >> file;
            emit signalNewFile(file);
            break;
        case msg_json:
            in >> pluginName;
            in >> json;
            //+ проверка, что что-то прочитали вообще
            emit signalNewInfo(pluginName, json);
            break;
        case msg_cmd:
            in >> command;
            if(command == "scan_start")
                emit signalEnableScanBtn(false);
            if(command == "scan_complete")
                emit signalEnableScanBtn(true);
            break;
        }

        sz_package = 0;
/// WARNING: буфер надо чистить при "плохом пакете"
///     да и вообще следить за буфером локально сокета - см.заметку(..)
    }
}

void LocalClient::sendFileToServer(const File &file)
{
    message_head head;
    head.type = msg_file;
    head.sz_body = sizeof(file);

    // Блок для передачи формируется аналогично тому, как это делается на сервере
    QByteArray arrayBlock;
    QDataStream out(&arrayBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);

    out << quint16(0);
    out << head;
    out << file;

    out.device()->seek(0);
    out << quint16(arrayBlock.size() - sizeof(quint16));

    localSocket->write(arrayBlock);
//    localSocket->waitForBytesWritten();
}


/*!
 * \brief LocalClient::sendCommandToServer - отправить управляющую команду в AthenaAgent
 * \param cmd - строка-команда
 */
void LocalClient::sendCommandToServer(const QString &cmd)
{
    message_head head;
    head.type = msg_cmd;
    head.sz_body = sizeof(cmd);

    QByteArray arrayBlock;
    QDataStream out(&arrayBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);

    out << quint16(0);
    out << head;
    out << cmd;

    out.device()->seek(0);
    out << quint16(arrayBlock.size() - sizeof(quint16));

    localSocket->write(arrayBlock);
}

void LocalClient::slotScanBtnPressed()
{
    qDebug() << "slotScanBtnPressed()";
    sendCommandToServer(QString("scan_system"));
}

void LocalClient::slotConnected()
{
   qDebug() << "Received the connected() signal";
}
void LocalClient::slotDisconnected()
{
   qDebug() << "Received the disonnected() signal";
}

