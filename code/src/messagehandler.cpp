#include "messagehandler.h"
#include "tcp_server.h"

MessageHandler::MessageHandler(QTcpSocket *socket, DataModel *dataModel, QObject *parent)
    : QObject(parent),
      m_socket(socket),
      m_dataModel(dataModel)
{
    if (m_socket)
    {
        connect(m_socket, &QTcpSocket::readyRead, this, &MessageHandler::handleReceivedMessage);
        connect(this, &MessageHandler::invalidMessageReceived, this, &MessageHandler::onInvalidMessageReceived);
    }
    else
    {
        qDebug() << CONSTANS::SOCKET_NULL_NOT_OPEN;
    }
}

MessageHandler::~MessageHandler()
{
    qDeleteAll(m_messages);
}

void MessageHandler::sendMessage(const QJsonObject &message)
{
    if (isSocketValid())
    {
        addMessageToList(new Respond(message)); // MESSAGE OUT: Respond
        QJsonDocument jsonDoc(message);
        QString jsonString = jsonDoc.toJson(QJsonDocument::Compact);
        QByteArray dataToSend = (jsonString + "\n").toUtf8();
        m_socket->write(dataToSend);
        m_socket->flush();
    }
    else
    {
        qDebug() << CONSTANS::SOCKET_NULL_NOT_OPEN;;
    }
}

void MessageHandler::addMessageToList(Message *message)
{
    QMutexLocker locker(&m_mutex);
    m_messages.append(message);
    emit messageListChanged();
}

QJsonObject MessageHandler::receiveMessage()
{
    if (!isSocketValid())
    {
        return QJsonObject();
    }

    QByteArray incomingData = m_socket->readAll();
    QList<QByteArray> messageParts = incomingData.split('\n');
    for (const QByteArray &part : messageParts)
    {
        if (part.trimmed().isEmpty())  
            continue;
        QJsonDocument doc = QJsonDocument::fromJson(part);
        if (doc.isObject())
        {
            return doc.object();
        }
        else
        {
            emit invalidMessageReceived(QString::fromUtf8(part));
        }
    }
    return QJsonObject();
}

bool MessageHandler::isSocketValid() const
{
    if (!m_socket || !m_socket->isOpen())
    {
        qDebug() << CONSTANS::SOCKET_NULL_NOT_OPEN;
        return false;
    }
    return true;
}

/// SLOTS
void MessageHandler::handleReceivedMessage()
{
    QJsonObject message = receiveMessage();

    if (!message.isEmpty())
    {
        addMessageToList(new Request(message)); // MESSAGE IN : Request
        emit messageReceived(message);          // Handover to tcp_server
    }
}

void MessageHandler::onInvalidMessageReceived(const QString &data)
{
    qDebug() << CONSTANS::INVALID_JSON << data ;
}

