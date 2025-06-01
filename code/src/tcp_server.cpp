#include "tcp_server.h"

TCP_Server::TCP_Server(DataModel *datamodel, quint16 port, QObject *parent)
    : QTcpServer(parent),
      m_socket(nullptr),
      m_datamodel(datamodel),
      m_port(port),
      m_messageHandler(nullptr)
{
    m_port = (port > CONSTANS::PORT::MIN  && port <= CONSTANS::PORT::MAX) ? port : CONSTANS::PORT::STD;
    if (!this->listen(QHostAddress::Any, m_port))
    {
        qDebug() << CONSTANS::SERVER_COULD_NOT_START << this->errorString();
    }
}

TCP_Server::~TCP_Server()
{
    delete m_socket;
    delete m_messageHandler;
}

void TCP_Server::incomingConnection(qintptr handle)
{
    // INIT SOCKET
    m_socket = new QTcpSocket(this);
    if (!m_socket->setSocketDescriptor(handle))
    {
        qDebug() << CONSTANS::SOCKET_DESCRIPTOR_FAILED;
        delete m_socket;
        m_socket = nullptr;
        return;
    }
    else // NOTIFY UI
    {
        notifyClientConnection();
    }

    // INIT MESSAGE HANDLER
    m_messageHandler = new MessageHandler(m_socket, m_datamodel, this);
    if (!m_messageHandler)
    {
        qDebug() << CONSTANS::MESSAGEHANDLER_FAILD;
        delete m_socket;
        m_socket = nullptr;
        return;
    }

    // CONNECTIONS
    connect(m_messageHandler, &MessageHandler::messageReceived,    this, &TCP_Server::onMessageReceived);
}

QJsonObject TCP_Server::processGetRequest(const QString &path)
{
    QJsonValue value = m_datamodel->getValue(path);

    if (value.isObject())
    {
        return value.toObject();
    }

    QJsonObject errorResponse;
    errorResponse[CONSTANS::JSON::STATUS] = CONSTANS::JSON::ERROR;
    errorResponse[CONSTANS::JSON::MESSAGE] = CONSTANS::JSON::PATH_NOT_FOUND;
    return errorResponse;
}

bool TCP_Server::processSetRequest(const QString &path, const QJsonValue &value)
{
    if(m_datamodel->getValue(path) != value)        // JUST CHANGE IF VALUE IS DIFFRENT
    {
        m_datamodel->setValue(path, value);
        return true;
    }
    return false;
}

void TCP_Server::notifyClientConnection()
{
    QString message = CONSTANS::CLIENT_CONNECTED + m_socket->peerAddress().toString() ;
    qDebug() << message;
    emit clientConnected(message);
}

/// SLOTS
void TCP_Server::onMessageReceived(const QJsonObject &receivedMessage)
{

    QString requestType = receivedMessage[CONSTANS::JSON::REQUEST].toString();
    QString path        = receivedMessage[CONSTANS::JSON::PATH].toString();

    if (requestType == CONSTANS::JSON::GET) // GET DATA
    {
        QJsonObject data = processGetRequest(path);
        m_messageHandler->sendMessage(data);
    }
    else if (requestType == CONSTANS::JSON::SET) // SET DATA
    {
        QJsonValue value = receivedMessage[CONSTANS::JSON::VALUE];
        bool success = processSetRequest(path, value);

        QJsonObject response;
        response[CONSTANS::JSON::STATUS] = success ? true: false;
        m_messageHandler->sendMessage(response);
    }
    else
    {
        QJsonObject errorResponse;
        errorResponse[CONSTANS::JSON::STATUS] = CONSTANS::JSON::ERROR;
        errorResponse[CONSTANS::JSON::MESSAGE] = CONSTANS::JSON::INVALID_REQUEST;
        m_messageHandler->sendMessage(errorResponse);
    }
}