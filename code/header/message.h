#ifndef MESSAGE_H
#define MESSAGE_H

// QT HEADER
#include <QObject>
#include <QDateTime>
#include <QJsonObject>

enum Direction
{
    IN,
    OUT
};

// INTERFACE
class Message : public QObject
{
    Q_OBJECT
public:
    Message(const QJsonObject &message, Direction direction)
        : m_message(message), m_direction(direction), m_timeStamp(QDateTime::currentDateTime()) {}

    QJsonObject getMessage() const { return m_message; }
    Direction getDirection() const { return m_direction; }
    QDateTime getTimeStamp() const { return m_timeStamp; }

private:
    QJsonObject m_message;
    Direction m_direction;
    QDateTime m_timeStamp;
};

// REQUEST 
class Request : public Message
{
public:
    Request(const QJsonObject &message) : Message(message, IN) {}
};

// RESPOND 
class Respond : public Message
{
public:
    Respond(const QJsonObject &message) : Message(message, OUT) {}
};

#endif // MESSAGE_H
