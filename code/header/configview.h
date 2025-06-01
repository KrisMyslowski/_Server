#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// QT HEADER
#include <QWidget>
#include <QTreeView>
#include <QFile>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QJsonObject>
#include <QJsonDocument>
#include <QItemDelegate>
#include <QHeaderView>
#include <QStatusBar>

// _SERVER HEADER
#include "datamodel.h"

namespace CONSTANS
{
    static const QString HEADER_KEY   = "Key";
    static const QString HEADER_VALUE = "Value";
    static const QString EMPTY = "";
    static const QString SEPERATOR = ".";
}

class ConfigView : public QWidget
{
    Q_OBJECT
public:
    ConfigView(DataModel *datamodel, QWidget *parent = nullptr);
    ~ConfigView();

private:
    DataModel *m_dataModel;
    QTreeView *m_treeView;
    QStatusBar *m_statusBar;
    QStandardItemModel *m_viewModel;

    void populateTree(const QJsonObject &jsonObj, QStandardItem *parentItem, const QString &path);
    void updateJsonValue(QJsonObject &obj, const QStringList &keys, const QJsonValue &newValue);
    void resetAndExpandUI();

public slots:
    void onClientConnected(const QString &ip);    
    
private slots:
   void onItemChanged(QStandardItem *item);
   void onDataModelUpdated(const QJsonObject &updatedModel);
};

#endif // MAINWINDOW_H
