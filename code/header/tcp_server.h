#ifndef TCP_SERVER_H
#define TCP_SERVER_H

// QT HEADER
#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QTimer>

// _SERVER HEADER
#include "messagehandler.h"
#include "datamodel.h"
namespace CONSTANS
{
    namespace PORT
    {
        static const quint16 MIN = 0;
        static const quint16 MAX = 65535;
        static const quint16 STD = 12345;   
    }

    namespace JSON
    {
        static const QString REQUEST        = "request";
        static const QString PATH           = "path";
        static const QString GET            = "get";
        static const QString SET            = "set";
        static const QString VALUE          = "value";
        static const QString STATUS         = "Status";
        static const QString ERROR          = "Error";
        static const QString MESSAGE        = "Message";

        static const QString PATH_NOT_FOUND  = "Path not found!";
        static const QString INVALID_REQUEST = "Invalid request!";
    }

    static const QString CLIENT_CONNECTED          = "Client connected:";
    static const QString SERVER_COULD_NOT_START    = "Server could not be started!";
    static const QString SOCKET_DESCRIPTOR_FAILED  = "Failed to set socket descriptor!";
    static const QString MESSAGEHANDLER_FAILD      = "Failed to create message handler!";
}

class TCP_Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit TCP_Server(DataModel *dataModel, quint16 port, QObject *parent = nullptr);
    ~TCP_Server();

protected:
    void incomingConnection(qintptr handle) override;

private:
    QTcpSocket *m_socket;
    QTimer m_statusTimer;
    DataModel *m_datamodel;
    quint16 m_port;
    MessageHandler *m_messageHandler;

    QJsonObject processGetRequest(const QString &path);
    bool processSetRequest(const QString &path, const QJsonValue &value);

    void notifyClientConnection();

signals:
    void clientConnected(const QString &welcomeMessage);

private slots:
    void onMessageReceived(const QJsonObject &message);

};

#endif // TCP_SERVER_H
