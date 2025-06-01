#ifndef MESSAGEHANDLING_H
#define MESSAGEHANDLING_H

// QT HEADER
#include <QObject>
#include <QTcpSocket>
#include <QList>
#include <QMutex>
#include <QJsonObject>
#include <QJsonDocument>

// _SERVER HEADER
#include "message.h"
#include "datamodel.h"

namespace CONSTANS
{
    static const QString INVALID_JSON         = "Invalid JSON!";
    static const QString SOCKET_NULL_NOT_OPEN = "Socket is null or not open!";
}

class MessageHandler : public QObject
{
    Q_OBJECT
public:
    explicit MessageHandler(QTcpSocket *socket, DataModel *dataModel, QObject *parent = nullptr);
    ~MessageHandler();

    void sendMessage(const QJsonObject &message);
    void addMessageToList(Message *message);
    QJsonObject receiveMessage();

private:
    QTcpSocket *m_socket;
    QList<Message*> m_messages;
    DataModel *m_dataModel;
    QMutex m_mutex;

    bool isSocketValid() const;

signals:
    void messageListChanged();
    void messageReceived(QJsonObject &message);
    void invalidMessageReceived(const QString &data);

private slots:
    void handleReceivedMessage();
    void onInvalidMessageReceived(const QString &data);
};

#endif // MESSAGEHANDLING_H
