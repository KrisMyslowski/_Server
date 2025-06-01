#include "configview.h"

ConfigView::ConfigView(DataModel *dataModel, QWidget *parent)
    : QWidget(parent),
      m_dataModel(dataModel),
      m_treeView(new QTreeView(this)),
      m_statusBar(new QStatusBar(this)),
      m_viewModel(new QStandardItemModel(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_treeView);
    layout->addWidget(m_statusBar);

    m_statusBar->setVisible(false);
    m_treeView->setModel(m_viewModel);
    m_viewModel->setHorizontalHeaderLabels({CONSTANS::HEADER_KEY,CONSTANS::HEADER_VALUE});

    m_treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    if (m_dataModel)
    {
        populateTree(m_dataModel->getDataModelObject(), nullptr, CONSTANS::EMPTY); // START EMTPY !
        connect(m_dataModel, &DataModel::dataModelUpdated, this, &ConfigView::onDataModelUpdated);
    }

    m_treeView->setItemDelegateForColumn(0, new QItemDelegate(this));
    connect(m_viewModel, &QStandardItemModel::itemChanged, this, &ConfigView::onItemChanged);
}

ConfigView::~ConfigView()
{
    delete m_treeView;
    delete m_viewModel;
    delete m_statusBar;
}

void ConfigView::populateTree(const QJsonObject &jsonObj, QStandardItem *parentItem, const QString &path)
{
    // TODO rm auto + rename
    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it)
    {
        // PATH
        QString currentPath = path.isEmpty() ? it.key() : path + CONSTANS::SEPERATOR + it.key();

        // KEY ITEM
        QStandardItem *keyItem = new QStandardItem(it.key());
        keyItem->setEditable(false);
        keyItem->setData(currentPath, Qt::UserRole);

        // VALUE ITEM
        QStandardItem *valueItem;

        // DATA TYPES
        if (it.value().isString())          // STRING
        {
            valueItem = new QStandardItem(it.value().toString());
        }
        else if (it.value().isDouble())     // DOUBLE
        {
            valueItem = new QStandardItem(QString::number(it.value().toDouble()));
        }
        else if (it.value().isBool())       // BOOL
        {
            valueItem = new QStandardItem(it.value().toBool() ? true : false);
        }
        else if (it.value().isObject())     // "EMPTY" ITEMS
        {
            valueItem = new QStandardItem("");
            valueItem->setEditable(false);
            valueItem->setBackground(QBrush(Qt::lightGray));
        }
        else
        {
            valueItem = new QStandardItem(it.value().toString());
        }

        // ADD KEY + VALUE
        if (parentItem)
        {
            parentItem->appendRow({keyItem, valueItem});
        }
        else
        {
            m_viewModel->appendRow({keyItem, valueItem});
        }

        // RECURSIVE CALL
        if (it.value().isObject())
        {
            populateTree(it.value().toObject(), keyItem, currentPath);
            m_treeView->setExpanded(keyItem->index(), true);
        }
    }
}

void ConfigView::updateJsonValue(QJsonObject &obj, const QStringList &keys, const QJsonValue &newValue) 
{
    if (keys.size() == 1)
    {
        obj[keys.first()] = newValue;
    }
    else
    {
        // RECURSIVE CALL
        QString firstKey = keys.first();
        QJsonObject childObj = obj[firstKey].toObject();
        updateJsonValue(childObj, keys.mid(1), newValue);
        obj[firstKey] = childObj;
    }
}

void ConfigView::resetAndExpandUI()
{
    m_treeView->reset();
    m_treeView->expandAll();
}

/// SLOTS
void ConfigView::onItemChanged(QStandardItem *item)
{
    if (item && item->column() == 1)
    {
        QStandardItem *keyItem = m_viewModel->itemFromIndex(item->index().siblingAtColumn(0));
        QString fullPath = keyItem->data(Qt::UserRole).toString();
        QStringList keys = fullPath.split(".");
        QString newValue = item->text();
        QJsonObject dataModelObject = m_dataModel->getDataModelObject();

        updateJsonValue(dataModelObject, keys, newValue);

        m_dataModel->updateDataModelObject(dataModelObject);
        m_dataModel->saveToFile();
        this->resetAndExpandUI();
    }
}

void ConfigView::onDataModelUpdated(const QJsonObject &updatedModel)
{
    m_viewModel->clear();
    m_viewModel->setHorizontalHeaderLabels({CONSTANS::HEADER_KEY, CONSTANS::HEADER_VALUE});
    populateTree(updatedModel, nullptr, CONSTANS::EMPTY);
    m_treeView->expandAll();
}

void ConfigView::onClientConnected(const QString &message)
{
    m_statusBar->setVisible(true);
    m_statusBar->showMessage(message);
}
