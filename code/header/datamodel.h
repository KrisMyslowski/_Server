#ifndef DATAMODEL_H
#define DATAMODEL_H

// QT HEADER
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>

namespace CONSTANS
{
    namespace JSON
    {
        static const QString SEPERATOR = ".";
        static const QString INVALID = "Invalid JSON!";
    }

    static const QString ERROR_OPEN_FILE ="Can not open file:";
}

class DataModel : public QObject
{
    Q_OBJECT
public:
    explicit DataModel(const QString &filePath, QObject *parent = nullptr);
    ~DataModel() = default;

    static DataModel* getInstance(const QString &filePath);

    QJsonObject getDataModelObject() const;    
    QJsonObject setJsonValue(const QJsonObject &obj, const QStringList &keys, const QJsonValue &value);
    QJsonValue getValue(const QString &path) const;

    void setValue(const QString &path, const QJsonValue &value);
    void updateDataModelObject(const QJsonObject &settings);
    void saveToFile();


private:
    QString m_filePath;
    QJsonObject m_dataModelObject;

    void loadFromFile();

signals:
    void dataModelUpdated(const QJsonObject &updatedModel);
};

#endif // DATAMODEL_H
