#include "datamodel.h"

DataModel::DataModel(const QString &filePath, QObject *parent)
    : QObject(parent),
      m_filePath(filePath)
{
    loadFromFile();
}

// SINGILTON INSTACNE
DataModel* DataModel::getInstance(const QString &filePath)
{
    static DataModel instance(filePath);
    return &instance;
}

QJsonObject DataModel::getDataModelObject() const
{
    return m_dataModelObject;
}

// VALUE FROM JSON PATH (e.g.: "settings.component.status")
QJsonValue DataModel::getValue(const QString &path) const
{
    QStringList pathParts = path.split(CONSTANS::JSON::SEPERATOR);
    QJsonObject currentObject = m_dataModelObject;

    for (int i = 0; i < pathParts.size(); ++i)
    {
        const QString &part = pathParts[i];

        if (currentObject.contains(part))
        {
            if (currentObject[part].isObject())
            {
                currentObject = currentObject[part].toObject();
            }
            else
            {
                return currentObject[part]; // FOUND
            }
        }
        else
        {
            return QJsonValue(); // NOT VALID
        }
    }
    return currentObject;
}

void DataModel::setValue(const QString &path, const QJsonValue &value)
{
    QStringList keys = path.split(CONSTANS::JSON::SEPERATOR);
    m_dataModelObject = setJsonValue(m_dataModelObject, keys, value); // OBJECT UPDATE

    saveToFile();   // SAVE
    emit dataModelUpdated(m_dataModelObject); // REFRESH UI
}

void DataModel::updateDataModelObject(const QJsonObject &settings)
{
    m_dataModelObject = settings;
}

void DataModel::saveToFile()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << CONSTANS::ERROR_OPEN_FILE << m_filePath;
        return;
    }

    QJsonDocument doc(m_dataModelObject);
    file.write(doc.toJson());
    file.close();
}

void DataModel::loadFromFile()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << CONSTANS::ERROR_OPEN_FILE << m_filePath;
        return;
    }

    QByteArray fileData = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(fileData);

    if (doc.isObject())
    {
        m_dataModelObject = doc.object();
    }
    else
    {
        qWarning() << CONSTANS::JSON::INVALID;
    }
    file.close();
}

// RECURSIVE 
QJsonObject DataModel::setJsonValue(const QJsonObject &obj, const QStringList &keys, const QJsonValue &value)
{
    QJsonObject newObj = obj;
    if (keys.isEmpty())
    {
        return newObj;
    }

    const QString currentKey = keys.first();

    if (keys.size() == 1)
    {
        newObj[currentKey] = value;
    }
    else
    {
        QJsonObject childObj;
        if (newObj.contains(currentKey) && newObj[currentKey].isObject())
        {
            childObj = newObj[currentKey].toObject();
        }
        QStringList remainingKeys = keys.mid(1);
        childObj = setJsonValue(childObj, remainingKeys, value); // RECURSICVE
        newObj[currentKey] = childObj;
    }
    return newObj;
}