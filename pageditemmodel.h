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
    Q_OBJECT
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

  explicit ItemModel(QObject *pParent = nullptr) : QAbstractListModel(pParent) {
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

  int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const override {
    return 1;
  }

  int columnCount(const QModelIndex &parent /*= QModelIndex()*/) const override {
    return static_cast<int>(Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) override
  {
    qDebug() << "setData PersonsModel " << value << value.toString();

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

  QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const override {
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

//Q_DECLARE_METATYPE(ItemModel)
Q_DECLARE_METATYPE(ItemModel*)

class ItemMapper : public QDataWidgetMapper
{
    Q_OBJECT
public:
  explicit ItemMapper(QObject *pParent = nullptr) : QDataWidgetMapper(pParent)
  {
    //m_model = new ItemModel(this);
    //setModel(m_model);
  }
  virtual ~ItemMapper()
  {}

  /*ItemModel* getModel() {
    return m_model;
  }*/

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
  //ItemModel* m_model;
};

//Q_DECLARE_METATYPE(ItemMapper)
Q_DECLARE_METATYPE(ItemMapper*)
Q_DECLARE_METATYPE(std::shared_ptr<ItemMapper>)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ItemListModel : public QAbstractListModel
{
    Q_OBJECT
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

  explicit ItemListModel(QObject *pParent = nullptr) : QAbstractListModel(pParent) {
  }

  virtual ~ItemListModel() {
  }

  int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const override {
    return m_items.size();
  }

  int columnCount(const QModelIndex &parent /*= QModelIndex()*/) const override {
    return static_cast<int>(Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) override
  {
    qDebug() << "setData ItemListModel " << value << value.toString();

    if (!index.isValid()) {
      return false;
    }

    //ItemMapper* item = m_items.at(index.row());

    std::shared_ptr<ItemMapper> itemMapper = qvariant_cast<std::shared_ptr<ItemMapper>>(value.value<QVariant>());
    if (!itemMapper) {
      return false;
    }

    /*switch(role)
    {
      case static_cast<int>(Roles::Name):
        item->getModel()->setName(value.toString());
        break;
      case static_cast<int>(Roles::Surname):
        item->getModel()->setSurname(value.toString());
        break;
    }

    m_items.replace(index.row(), item);*/

    m_items.replace(index.row(), itemMapper);

    emit dataChanged(index, index);
    return true;
  }

  QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const override {
    //qDebug() << "data" << index.column() << role;

    if (!index.isValid()) {
      return QVariant();
    }

    std::shared_ptr<ItemMapper> item = m_items.at(index.row());

    if (!item)
      return QVariant();

    QVariant result;

    result = QVariant::fromValue(item);

    /*switch(role)
    {
      case static_cast<int>(Roles::Name):
        result = item->getModel()->getName();
        break;
      case static_cast<int>(Roles::Surname):
        result = item->getModel()->getName();
        break;
    }*/
    return result;
  }

  void addItem(std::shared_ptr<ItemMapper> item) {
    m_items.push_back(item);
  }

private:
  QList<std::shared_ptr<ItemMapper>> m_items;
};

//Q_DECLARE_METATYPE(ItemListModel)
Q_DECLARE_METATYPE(ItemListModel*)
Q_DECLARE_METATYPE(std::shared_ptr<ItemListModel>)

class PagedItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
  /*enum class Roles {
    Name = Qt::UserRole + 1
    , Surname
    , Total
  };*/

  enum class Columns {
    Page = 0
    , Total
  };

  explicit PagedItemModel(QObject *pParent = nullptr) : QAbstractListModel(pParent) {
  }

  virtual ~PagedItemModel() {
  }

  int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const override {
    return m_pages.size();
  }

  int columnCount(const QModelIndex &parent /*= QModelIndex()*/) const override {
    return static_cast<int>(Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) override
  {
    qDebug() << "setData PagedItemModel " << value << value.toString();

    if (!index.isValid()) {
      return false;
    }

    //ItemListModel* item = m_pages.at(index.row());

    std::shared_ptr<ItemListModel> item = qvariant_cast<std::shared_ptr<ItemListModel>>(value.value<QVariant>());
    if (!item) {
      return false;
    }

    /*switch(role)
    {
      case static_cast<int>(Roles::Name):
        item->getModel()->setName(value.toString());
        break;
      case static_cast<int>(Roles::Surname):
        item->getModel()->setSurname(value.toString());
        break;
    }*/

    m_pages.replace(index.row(), item);

    emit dataChanged(index, index);
    return true;
  }

  QVariant data(const QModelIndex &index, int role) const override {
    //qDebug() << "data" << index.column() << role;

    if (!index.isValid()) {
      return false;
    }

    QVariant result;

    std::shared_ptr<ItemListModel> item = m_pages.at(index.row());

    if (!item)
      return false;

    /*switch(role)
    {
      case static_cast<int>(Roles::Name):
        result = item->getModel()->getName();
        break;
      case static_cast<int>(Roles::Surname):
        result = item->getModel()->getName();
        break;
    }*/

    result = QVariant::fromValue(item);

    return result;
  }

  void addPage(const std::shared_ptr<ItemListModel> page) {
    m_pages.push_back(page);
  }

  bool removePageAt(int index) {
    if(index < 0 || index >= m_pages.size()) {
      return false;
    }

    m_pages.removeAt(index);
    return true;
  }

  bool replacePageAt(int index, std::shared_ptr<ItemListModel> newItem) {
    if(index < 0 || index >= m_pages.size()) {
      return false;
    }

    m_pages.replace(index, newItem);
    return true;
  }

  QList<std::shared_ptr<ItemListModel>> getPages() const {
    return m_pages;
  }

  int pagesTotal() const {
    return m_pages.size();
  }

  void setPages(const QList<std::shared_ptr<ItemListModel>>& pages) {
    m_pages = pages;
  }

  const std::shared_ptr<ItemListModel> getPageAt(int index) const {
    if(index < 0 || index >= m_pages.size()) {
      return nullptr;
    }

    return m_pages.at(index);
  }

  void removeAllPages() {
    m_pages.clear();
  }

private:
  QList<std::shared_ptr<ItemListModel>> m_pages;
};

//Q_DECLARE_METATYPE(PagedItemModel)
Q_DECLARE_METATYPE(PagedItemModel*)
//Q_DECLARE_METATYPE(std::shared_ptr<PagedItemModel>)

class PagedItemMapper : public QDataWidgetMapper
{
    Q_OBJECT
public:
  explicit PagedItemMapper(QObject *pParent = nullptr) : QDataWidgetMapper(pParent)
  {
    //m_model = new PagedItemModel(this);
    //setModel(m_model);
  }
  virtual ~PagedItemMapper()
  { }

  //PagedItemModel* getModel() {
  //  return m_model;
  //}

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
  //PagedItemModel* m_model;
};

//Q_DECLARE_METATYPE(PagedItemMapper)
Q_DECLARE_METATYPE(PagedItemMapper*)
Q_DECLARE_METATYPE(std::shared_ptr<PagedItemMapper>)

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
