#include <QtCore>
#include <QAbstractListModel>
#include <QDataWidgetMapper>
#include <QLineEdit>
#include <QCheckBox>
#include <QDoubleSpinBox>

#ifndef PAGEDITEMMODEL_H
#define PAGEDITEMMODEL_H

struct Item // : public QObject
{
  //Q_PROPERTY
    QString name;
    QString surname;
};

Q_DECLARE_METATYPE(Item)
Q_DECLARE_METATYPE(Item*)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ItemModel : public QAbstractListModel
{
public:
  /*enum class Roles {
    NameRole = Qt::UserRole + 1
    , SurnameRole
    , Total
  };*/

  enum class Columns {
    Name = 0
    , Surname
    , Total
  };

  ItemModel(QObject *pParent = nullptr) : QAbstractListModel(pParent) {
  }

  virtual ~ItemModel() {
  }

  QString getName() const {
    return m_item.name;
  }

  void setName(QString sString) {
    m_item.name = sString;
  }

  QString getSurname() const {
    return m_item.surname;
  }

  void setSurname(QString sString) {
    m_item.surname = sString;
  }

  virtual int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const override {
    return 1;
  }

  virtual int columnCount(const QModelIndex &parent /*= QModelIndex()*/) const override {
    return static_cast<int>(Columns::Total);
  }

  virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override
  {
    //qDebug() << "setData PersonsModel " << value << value.toString();

    if (!index.isValid())
      return false;

    switch(index.column())
    {
      case static_cast<int>(Columns::Name):
        setName(value.toString());
        break;
      case static_cast<int>(Columns::Surname):
        setSurname(value.toString());
        break;
    }

    emit dataChanged(index, index);
    return true;
  }

  virtual QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const override {
    //qDebug() << "data" << index.column() << role;

    if (!index.isValid())
      return false;

    QVariant result;
    switch(index.column())
    {
      case static_cast<int>(Columns::Name):
        result = getName();
        break;
      case static_cast<int>(Columns::Surname):
        result = getSurname();
        break;
    }
    return result;
  }

private:
  Item m_item;
};

class ItemMapper : public QDataWidgetMapper
{
public:
  ItemMapper(QObject *pParent = nullptr) : QDataWidgetMapper(pParent)
  {
    m_model = new ItemModel(this);
    setModel(m_model);
  }
  virtual ~ItemMapper()
  { }

  ItemModel* getModel() {
    return m_model;
  }

  /*void AddNameMapping(QLineEdit *pLineEdit)
  {
    addMapping(pLineEdit, 0);
    this->setCurrentIndex(0);
  }

  void AddSurnameMapping(QLineEdit *pLineEdit)
  {
    addMapping(pLineEdit, 1);
    this->setCurrentIndex(0);
  }

  QString getName()
  {
    return static_cast<ItemModel *>(model())->getName();
  }

  void setName(QString sString)
  {
    static_cast<ItemModel *>(model())->setName(sString);
    setCurrentIndex(0);
  }

  QString getSurname()
  {
    return static_cast<ItemModel *>(model())->getSurname();
  }

  void setSurname(QString sString)
  {
    static_cast<ItemModel *>(model())->setSurname(sString);
    setCurrentIndex(0);
  }*/

private:
  ItemModel* m_model;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ItemListModel : public QAbstractListModel
{
public:
  enum class Roles {
    Name = Qt::UserRole + 1
    , Surname
    , Total
  };

  enum class Columns {
    Item = 0
    , Total
  };

  ItemListModel(QObject *pParent = nullptr) : QAbstractListModel(pParent) {
  }

  virtual ~ItemListModel() {
  }

  virtual int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const override {
    return m_items.size();
  }

  virtual int columnCount(const QModelIndex &parent /*= QModelIndex()*/) const override {
    return static_cast<int>(Columns::Total);
  }

  virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override
  {
    //qDebug() << "setData PersonsModel " << value << value.toString();

    if (!index.isValid())
      return false;

    ItemMapper* item = m_items.at(index.row());

    switch(role)
    {
      case static_cast<int>(Roles::Name):
        item->getModel()->setName(value.toString());
        break;
      case static_cast<int>(Roles::Surname):
        item->getModel()->setSurname(value.toString());
        break;
    }

    m_items.replace(index.row(), item);

    emit dataChanged(index, index);
    return true;
  }

  virtual QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const override {
    //qDebug() << "data" << index.column() << role;

    if (!index.isValid())
      return false;

    ItemMapper* item = m_items.at(index.row());

    if (!item)
      return false;

    QVariant result;
    switch(role)
    {
      case static_cast<int>(Roles::Name):
        result = item->getModel()->getName();
        break;
      case static_cast<int>(Roles::Surname):
        result = item->getModel()->getName();
        break;
    }
    return result;
  }

  void addItem(ItemMapper* item) {
    m_items.push_back(item);
  }

private:
  QList<ItemMapper*> m_items;
};

/*class PagedModel : public QAbstractListModel
{
    Q_OBJECT

    public:
        PagedModel();
        //PagedModel(const DirectoryScanner &dirScanner);
        //PagedModel(std::shared_ptr<piga::Host> host);
        virtual ~PagedModel();

        QHash<int, QByteArray> roleNames() const;

        //void readPackages(std::shared_ptr<packaging::PackageManager> pkgMgr);
        void addPage(const std::string &dir);

        //void setHost(std::shared_ptr<piga::Host> host);
        std::shared_ptr<Page> getPage(const std::string &dir);
        //std::shared_ptr<Game> getGame(const QString &dir);
        //std::shared_ptr<Game> lookupGame(const QString &dir);
        //std::shared_ptr<piga::Host> getHost() const;
        //const std::string& getDirectory() const;
        QList<std::shared_ptr<Page>>& getPages();

        virtual int rowCount(const QModelIndex &parent) const;
        virtual QVariant data(const QModelIndex &index, int role) const;
private:
        //std::shared_ptr<packaging::PackageManager> m_packageManager;
        //std::string m_directory = "";
        //std::shared_ptr<piga::Host> m_host;
        QList<ItemMapper> m_pages;
};

Q_DECLARE_METATYPE(PagedModel)
Q_DECLARE_METATYPE(PagedModel*)*/

#endif // PAGEDITEMMODEL_H
