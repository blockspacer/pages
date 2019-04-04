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
    int guid; // TODO: change guid map when item changes
    QString name;
    QString surname;
};

Q_DECLARE_METATYPE(Item)
Q_DECLARE_METATYPE(Item*)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ItemModel : public QAbstractListModel
{
    Q_OBJECT

Q_SIGNALS:
    void guidChanged(const int &prevGUID, const int &newGUID);

public:
  /*enum class Roles {
    NameRole = Qt::UserRole + 1
    , SurnameRole
    , Total
  };*/

  enum class Columns {
    Name = 0
    , Surname
    , GUID
    , Total
  };

  explicit ItemModel(QObject *pParent = nullptr) : QAbstractListModel(pParent) {
  }

  virtual ~ItemModel() {
  }

  QString getName() const {
    return m_item.name;
  }

  void setName(const QString& sString) {
    m_item.name = sString;

    QModelIndex indexItem = index(0, static_cast<int>(Columns::Name));
    emit dataChanged(indexItem, indexItem);
  }

  QString getSurname() const {
    return m_item.surname;
  }

  void setSurname(const QString& sString) {
    m_item.surname = sString;

    QModelIndex indexItem = index(0, static_cast<int>(Columns::Surname));
    emit dataChanged(indexItem, indexItem);
  }

  int getGUID() const {
    return m_item.guid;
  }

  void setGUID(const int GUID) {
    const int prevGUID = m_item.guid;
    m_item.guid = GUID;

    emit guidChanged(prevGUID, m_item.guid);

    QModelIndex indexItem = index(0, static_cast<int>(Columns::GUID));
    emit dataChanged(indexItem, indexItem);
  }

  int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const Q_DECL_OVERRIDE {
    return 1;
  }

  int columnCount(const QModelIndex &parent /*= QModelIndex()*/) const Q_DECL_OVERRIDE {
    return static_cast<int>(Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE
  {
    qDebug() << "setData PersonsModel " << value << value.toString();

    if (!index.isValid())
      return false;

    switch(index.column())
    {
      case static_cast<int>(Columns::Name):
        setName(value.toString()); // emits dataChanged signal
        break;
      case static_cast<int>(Columns::Surname):
        setSurname(value.toString()); // emits dataChanged signal
        break;
      case static_cast<int>(Columns::GUID):
        setGUID(value.toInt()); // emits dataChanged signal
        break;
    }

    //emit dataChanged(index, index);
    return true;
  }

  QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const Q_DECL_OVERRIDE {
    //qDebug() << "data" << index.column() << role;

    //return QVariant("QVariant");

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
      case static_cast<int>(Columns::GUID):
        result = getGUID();
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
    //Q_DISABLE_COPY(ItemListModel)
    Q_OBJECT


/*public:
  explicit ItemTableProxyModel(QObject *pParent = nullptr) : QAbstractProxyModel(pParent) {
    connect(this, SIGNAL(sourceModelChanged()), this, SLOT(slotSourceModelChanged()));
  }

  virtual ~ItemTableProxyModel() {
  }

public slots:
  void slotSourceModelChanged(void);
  void slotDataChanged(const QModelIndex first, QModelIndex last);


void slotSourceModelChanged()
{
  disconnect(this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));
  connect(sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));
}

void slotDataChanged(const QModelIndex first, QModelIndex last)
{
  emit dataChanged(mapFromSource(first), mapFromSource(last));
}*/

public:
  /*enum class Roles {
    Name = Qt::UserRole + 1
    , Surname
    , GUID
    , Total
  };*/

  enum class Columns {
    Item = 0
     // only for debug purposes
    //, DebugName
    , Total
  };

  explicit ItemListModel(QObject *pParent = nullptr) : QAbstractListModel(pParent) {
  }

  virtual ~ItemListModel() {
  }

  int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const Q_DECL_OVERRIDE {
    return m_items.size();
  }

  int columnCount(const QModelIndex &parent /*= QModelIndex()*/) const Q_DECL_OVERRIDE {
    return static_cast<int>(Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE
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

    QVector<int> roles;
    roles << role;
    if (!replaceItemAt(index.row(), itemMapper, roles)) { // emits dataChanged signal
      qDebug() << "ItemListModel::setData: invalid index.row() " << index.row();
    }

    return true;
  }

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
  }

  /*bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) Q_DECL_OVERRIDE {
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        m_items.push_back(nullptr);
    }

    endInsertRows();
    return true;
  }

  bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) Q_DECL_OVERRIDE {
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        m_items.removeAt(position);
    }

    endRemoveRows();
    return true;
  }*/

  Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE
  {
    if (!index.isValid())
      return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
  }

  QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const Q_DECL_OVERRIDE {
    //qDebug() << "data" << index.column() << role;
    if (!index.isValid()) {
      return QVariant();
    }

    if (role == Qt::SizeHintRole) {
      return QVariant(QSize( 25, 25 ));
    }

    if(role != Qt::DisplayRole && role != Qt::EditRole){
      return QVariant(); // disable tooltips, icons, e.t.c
    }

   // return QVariant("dataStore[index.row()]->getPerson()");


    std::shared_ptr<ItemMapper> item = m_items.at(index.row());

    if (!item)
      return QVariant();

    QVariant result;

    //if (role == Qt::DisplayRole)
    //  return QVariant("QVariant");

    /*if (index.column() == static_cast<int>(Columns::DebugName)) {
      return QVariant(static_cast<ItemModel*>(item->model())->getName());
    }*/

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

  void pushBack(std::shared_ptr<ItemMapper> item) {
    const int oldRows = itemsTotal();

    /*
        First is the new index of the first element that will be inserted.
        Last is the new index of the last element that will be inserted.
        Since we're appending only one element at the end of the list, the
        index of the first and last elements is the same, and is equal to
        the current size of the list.
    */
    const int first = oldRows;
    const int last = first;
    emit beginInsertRows(QModelIndex(), first, last);

    m_items.push_back(item);

    int newGuid = static_cast<ItemModel*>(item->model())->getGUID();
    m_guidToItem[newGuid] = item;

    //QModelIndex indexDebugName = index(rowCount(QModelIndex()), static_cast<int>(Columns::DebugName));
    //emit dataChanged(indexDebugName, indexDebugName);

    //QModelIndex indexItem = index(rowCount(QModelIndex()), static_cast<int>(Columns::Item));
    //emit dataChanged(indexItem, indexItem);

    emit endInsertRows();

    //connect(item->model(), &QAbstractItemModel::dataChanged, this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));
    const int itemRowId = oldRows + 1;
    const QModelIndex indexMapped = index(itemRowId, static_cast<int>(Columns::Item));
    /*connect(item->model(), &QAbstractItemModel::dataChanged, item->model(), [this, indexMapped](const QModelIndex first, QModelIndex last){
      qDebug() << "connect(item->model(), &QAbstractItemModel::dataChanged" << indexMapped;
      emit dataChanged(indexMapped, indexMapped);
    });*/
    connect(item->model(), &QAbstractItemModel::dataChanged, this, [this, indexMapped](const QModelIndex first, QModelIndex last){
      qDebug() << "connect(item->model(), &QAbstractItemModel::dataChanged" << indexMapped;
      emit dataChanged(indexMapped, indexMapped);
    });
  }

  bool removeItemAt(int rowIndex) {
    if(rowIndex < 0 || rowIndex >= itemsTotal()) {
      return false;
    }

    emit beginRemoveRows(QModelIndex(), rowIndex, rowIndex);

    int oldGuid = static_cast<ItemModel*>(getItemAt(rowIndex)->model())->getGUID();
    m_guidToItem.remove(oldGuid);
    m_items.removeAt(rowIndex);

    //QModelIndex indexItem = index(rowIndex, static_cast<int>(Columns::Item));
    //emit dataChanged(indexItem, indexItem);

    emit endRemoveRows();

    return true;
  }

  bool replaceItemAt(int rowIndex, std::shared_ptr<ItemMapper> newItem, QVector<int> roles = QVector<int>()) {
    if(rowIndex < 0 || rowIndex >= itemsTotal()) {
      return false;
    }

    int oldGuid = static_cast<ItemModel*>(getItemAt(rowIndex)->model())->getGUID();
    m_guidToItem.remove(oldGuid);

    m_items.replace(rowIndex, newItem);

    int newGuid = static_cast<ItemModel*>(newItem->model())->getGUID();
    m_guidToItem[newGuid] = newItem;

    QModelIndex indexItem = index(rowIndex, static_cast<int>(Columns::Item));
    emit dataChanged(indexItem, indexItem, roles);

    /*QModelIndex indexItemDebug = index(rowIndex, static_cast<int>(Columns::DebugName));
    emit dataChanged(indexItemDebug, indexItemDebug, roles);*/

    return true;
  }

  const QList<std::shared_ptr<ItemMapper>> getItems() const {
    return m_items;
  }

  int itemsTotal() const {
    Q_ASSERT(m_items.size() == m_guidToItem.size());
    return m_items.size();
  }

  void setItems(const QList<std::shared_ptr<ItemMapper>>& items) {
    removeAllItems(); // emit row remove signal

    emit beginInsertRows(QModelIndex(), items.size(), items.size());

    m_items = items;

    for (auto& newItem: items) {
      int newGuid = static_cast<ItemModel*>(newItem->model())->getGUID();
      m_guidToItem[newGuid] = newItem;
    }

    emit endInsertRows();

    Q_ASSERT(m_items.size() == m_guidToItem.size());
  }

  const std::shared_ptr<ItemMapper> getItemAt(int rowIndex) const {
    Q_ASSERT(m_items.size() == m_guidToItem.size());

    if(rowIndex < 0 || rowIndex >= m_items.size()) {
      return nullptr;
    }

    return m_items.at(rowIndex);
  }

  const std::shared_ptr<ItemMapper> getItemById(int guid) const {
    Q_ASSERT(m_items.size() == m_guidToItem.size());

    //qDebug() << "m_guidToItem.size()" << m_guidToItem.size();

    if(!m_guidToItem.contains(guid)) {
      qDebug() << "ItemListModel not contains guid = " << guid;
      return nullptr;
    }

    QHash<int, std::shared_ptr<ItemMapper>>::const_iterator it = m_guidToItem.find(guid);

    //qDebug() << "m_guidToItem.size() 3" << m_guidToItem.size();

    return it.value();
  }

  void removeAllItems() {

    emit beginRemoveRows(QModelIndex(), 0, itemsTotal());

    m_items.clear();
    m_guidToItem.clear();

    emit endRemoveRows();

    Q_ASSERT(m_items.size() == m_guidToItem.size());
  }

private:
  QHash<int, std::shared_ptr<ItemMapper>> m_guidToItem;
  QList<std::shared_ptr<ItemMapper>> m_items;
};

//Q_DECLARE_METATYPE(ItemListModel)
Q_DECLARE_METATYPE(ItemListModel*)
Q_DECLARE_METATYPE(std::shared_ptr<ItemListModel>)

class ItemTableProxyModel : public QAbstractProxyModel
{
    Q_OBJECT

  /*enum class Columns {
    Item = 0
    , Name
    , Surname
    , Total
  };*/

public:
  explicit ItemTableProxyModel(QObject *pParent = nullptr) : QAbstractProxyModel(pParent) {
    connect(this, SIGNAL(sourceModelChanged()), this, SLOT(slotSourceModelChanged()));
  }

  virtual ~ItemTableProxyModel() {
  }

public slots:
  void slotSourceModelChanged(void);
  void slotDataChanged(const QModelIndex& first, const QModelIndex& last);
  void slotRowsInserted(const QModelIndex& parent, int first, int last);
  void slotRowsRemoved(const QModelIndex &parent, int start, int end);
  void sourceReset();

protected:

  ItemListModel* getSourceListModel() const {
    return static_cast<ItemListModel*>(sourceModel());
  }

  /*bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override {
    return true;
  }

  bool filterAcceptsColumn(int sourceColumn, const QModelIndex &sourceParent) const override {
    return true;
  }

  bool lessThan(const QModelIndex &left, const QModelIndex &right) const override {
    return true;
  }*/

  int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const Q_DECL_OVERRIDE {
    ItemListModel* source = getSourceListModel();
    if (!source) {
      return 0;
    }

    return source->getItems().size();
  }

  int columnCount(const QModelIndex &parent /*= QModelIndex()*/) const Q_DECL_OVERRIDE {
    return static_cast<int>(ItemModel::Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE
  {
    qDebug() << "setData ItemListModel " << value << value.toString();

    if (!index.isValid()) {
      return false;
    }

    //ItemMapper* item = m_items.at(index.row());

    /*std::shared_ptr<ItemMapper> itemMapper = qvariant_cast<std::shared_ptr<ItemMapper>>(value.value<QVariant>());
    if (!itemMapper) {
      return false;
    }

    QVector<int> roles;
    roles << role;
    if (!replaceItemAt(index.row(), itemMapper, roles)) { // emits dataChanged signal
      qDebug() << "ItemListModel::setData: invalid index.row() " << index.row();
    }*/

    ItemListModel* source = getSourceListModel();
    if (!source) {
      return false;
    }

    std::shared_ptr<ItemMapper> mapper = getSourceListModel()->getItemAt(index.row());
    if(!mapper.get()) {
      return false;
    }

    ItemModel* model = static_cast<ItemModel*>(mapper->model());
    if(!model) {
      return false;
    }

    switch(index.column())
    {
      case static_cast<int>(ItemModel::Columns::Name):
        model->setName(value.toString());
        break;
      case static_cast<int>(ItemModel::Columns::Surname):
        model->setSurname(value.toString());
        break;
      case static_cast<int>(ItemModel::Columns::GUID):
        model->setGUID(value.toInt());
        break;
    }

    emit dataChanged(index, index);
    return true;
  }

  QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const Q_DECL_OVERRIDE {
    //qDebug() << "data" << index.column() << role;

    //return QVariant("QVariant");

    if (!index.isValid()) {
      return QVariant();
    }

    if (role == Qt::SizeHintRole) {
      return QVariant(QSize( 25, 25 ));
    }

    if(role != Qt::DisplayRole && role != Qt::EditRole){
      return QVariant(); // disable tooltips, icons, e.t.c
    }

    QVariant result;

    std::shared_ptr<ItemMapper> mapper = getSourceListModel()->getItemAt(index.row());
    if(!mapper.get()) {
      return result;
    }

    ItemModel* model = static_cast<ItemModel*>(mapper->model());
    if(!model) {
      return result;
    }

    switch(index.column())
    {
      case static_cast<int>(ItemModel::Columns::Name):
        result = model->getName();
        break;
      case static_cast<int>(ItemModel::Columns::Surname):
        result = model->getSurname();
        break;
      case static_cast<int>(ItemModel::Columns::GUID):
        result = model->getGUID();
        break;
    }
    return result;
  }

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    QModelIndex res;
    if(!parent.isValid())
    {
      res = createIndex(row, column);
    }
    return res;
  }

  QModelIndex parent(const QModelIndex & /*child*/) const Q_DECL_OVERRIDE {
    return QModelIndex();
  }

  QModelIndex mapToSource(const QModelIndex &proxyIndex)  const Q_DECL_OVERRIDE {
    QModelIndex res;
    if(proxyIndex.isValid())
    {
      if(proxyIndex.column() >= sourceModel()->columnCount() && proxyIndex.column() <= (static_cast<int>(ItemModel::Columns::Total) - 1))
      {
         // res = createIndex(proxyIndex.row(), -1, (quintptr)-1);
         res = createIndex(proxyIndex.row(), proxyIndex.column(), (quintptr)-1);
      }
      else// if(proxyIndex.column() < sourceModel()->columnCount())
      {
         res = sourceModel()->index(proxyIndex.row(), proxyIndex.column());
      }
      /*else
      {
         res = sourceModel()->index(proxyIndex.row(), proxyIndex.column()-m_nbCol);
      }*/
    }
    return res;
  }

  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE {
    QModelIndex res;
    /*if(!sourceIndex.isValid())
    {
      if((sourceIndex.row() > -1) &&
         (sourceIndex.column() >= sourceModel()->columnCount() && sourceIndex.column() <= (static_cast<int>(ItemModel::Columns::Total) - 1)) &&
         (sourceIndex.internalId() == (quintptr)(sourceIndex.column())))
      {
         res = index(sourceIndex.row(), sourceIndex.column());
      }
    }
    else
    {
      res = index(sourceIndex.row(), sourceIndex.column());
    }*/
    res = index(sourceIndex.row(), sourceIndex.column());
    return res;
  }

  Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE
  {
    if (!index.isValid())
      return Qt::ItemIsEnabled;

    /*return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;*/

   Qt::ItemFlags res;
   if(index.column() >= sourceModel()->columnCount() && index.column() <= (static_cast<int>(ItemModel::Columns::Total) - 1))
   {
      res = QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
   }
   else
   {
      res = sourceModel()->flags(mapToSource(index));
   }
   return res;
  }

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column: %1").arg(section);
    else
        return QString("Row: %1").arg(section);
  }

private:
};

Q_DECLARE_METATYPE(ItemTableProxyModel*)
Q_DECLARE_METATYPE(std::shared_ptr<ItemTableProxyModel>)

/// \see https://doc.qt.io/qt-5/qtwidgets-itemviews-customsortfiltermodel-example.html
class PagedItemTableProxyFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public slots:
  void slotSourceModelChanged(void);
  void slotDataChanged(const QModelIndex& first, const QModelIndex& last);
  void slotRowsInserted(const QModelIndex& parent, int first, int last);
  void slotRowsRemoved(const QModelIndex &parent, int start, int end);
  void sourceReset();

public:
    explicit PagedItemTableProxyFilterModel(QObject *parent = nullptr): QSortFilterProxyModel(parent)
    {
      //connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(slotRowsInserted(QModelIndex,int,int)));
      //connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(slotRowsRemoved(QModelIndex,int,int)));

      connect(this, SIGNAL(sourceModelChanged()), this, SLOT(slotSourceModelChanged()));

      /*connect(this, &QSortFilterProxyModel::rowsInserted, this, [this](const QModelIndex &parent, int first, int last, QPrivateSignal){
        qDebug() << "rowsInserted";
      });
      connect(this, &QSortFilterProxyModel::rowsRemoved, this, [this](const QModelIndex &parent, int first, int last, QPrivateSignal){
        qDebug() << "rowsInserted";
      });*/
    }

    int filterMinSourceRowIndex() const { return minSourceRowIndex; }

    void setFilterMinSourceRowIndex(const int rowNum){
        minSourceRowIndex = rowNum;
        invalidateFilter();
    }

    int filterMaxSourceRowIndex() const { return maxSourceRowIndex; }

    void setFilterMaxSourceRowIndex(const int rowNum){
        maxSourceRowIndex = rowNum;
        invalidateFilter();
    }

    int getRowLimit() const { return rowLimit; }

    void setRowLimit(const int num){
        rowLimit = num;
        //invalidateFilter();
    }

    /*int getLastRowSourceRowNum() const { return lastRowSourceRowNum; }

    void setLastRowSourceRowNum(const int num){
        lastRowSourceRowNum = num;
        //invalidateFilter();
    }

    int updateLastRowSourceRowNum() {
      int rowNum = rowCount() - 1;
      auto idx = index(rowNum,0,QModelIndex());
      qDebug() << "idx" << idx;
      int result = mapToSource(idx).row();
      setLastRowSourceRowNum(result);
      return result;
      //return QSortFilterProxyModel::rowCount();
    }*/

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
      //return 2;
      auto filteredRows = QSortFilterProxyModel::rowCount();

      if (getRowLimit() >= 0)
        return std::min(filteredRows, getRowLimit());

      return filteredRows;
      //return sourceModel()->rowCount();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
      return static_cast<int>(ItemModel::Columns::Total);
    }

protected:

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const Q_DECL_OVERRIDE {
        //qDebug() << "lastRowSourceRowNum" << lastRowSourceRowNum;

        /*if (sourceRow < 2) {
          return false;
        }
        if (lastRowSourceRowNum > 0 && sourceRow < lastRowSourceRowNum) {
          return false;
        }*/

        //if (prevFilteredMaxSourceRow)
//qDebug() << "sourceRow" << columnCount();
//qDebug() << "sourceParent" << sourceParent;
//qDebug() << "filteredRowCount" << filteredRowCount;

        const QModelIndex indexName = sourceModel()->index(sourceRow, static_cast<int>(ItemModel::Columns::Name), sourceParent);
        const bool isNameFiltered = sourceModel()->data(indexName).toString().contains(filterRegExp());

        const QModelIndex indexSurname = sourceModel()->index(sourceRow, static_cast<int>(ItemModel::Columns::Surname), sourceParent);
        const bool isSurnameFiltered = sourceModel()->data(indexSurname).toString().contains(filterRegExp());

        const bool anyFieldMatch = (isNameFiltered
                //|| isSurnameFiltered
                );
                //this->cou
//qDebug() << mapToSource(indexName);
//qDebug() << rowCount(index(sourceRow, static_cast<int>(ItemModel::Columns::Name), sourceParent));
        //return anyFieldMatch && (this->it++) < 2;
        //return sourceRow < 2;
//qDebug() << this->checkIndex(indexName);
//qDebug() << sourceModel()->rowCount();
        //if(filterRegExp().pattern().isEmpty()) {
        //  return rowInRange(sourceRow);
        //}

        //return anyFieldMatch && rowInRange(sourceRow);

        //return anyFieldMatch && rowInRange(sourceRow);
        //return anyFieldMatch && rowInRange(sourceRow);
        return anyFieldMatch && rowInRange(sourceRow);
    }

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const Q_DECL_OVERRIDE {
        return left.row() < right.row(); // sort from first added to last added

        /*const QVariant leftData = sourceModel()->data(left);
        const QVariant rightData = sourceModel()->data(right);

        if (leftData.type() == QVariant::String) {
            const QString leftString = leftData.toString();
            const QString rightString = rightData.toString();
            return QString::localeAwareCompare(leftString, rightString) < 0;
        }*/

        /*if (leftData.type() == QVariant::DateTime) {
            return leftData.toDateTime() < rightData.toDateTime();
        } else {
            static const QRegularExpression emailPattern("[\\w\\.]*@[\\w\\.]*");

            QString leftString = leftData.toString();
            if (left.column() == 1) {
                const QRegularExpressionMatch match = emailPattern.match(leftString);
                if (match.hasMatch())
                    leftString = match.captured(0);
            }
            QString rightString = rightData.toString();
            if (right.column() == 1) {
                const QRegularExpressionMatch match = emailPattern.match(rightString);
                if (match.hasMatch())
                    rightString = match.captured(0);
            }

            return QString::localeAwareCompare(leftString, rightString) < 0;
        }*/

        return false;
    }

private:
    bool rowInRange(const int rowNum) const {
        //if(minSourceRowIndex < 0 || maxSourceRowIndex < 0) {
        //  return true;
        //}

        return (!isValidRow(minSourceRowIndex) || rowNum >= minSourceRowIndex)
                && (!isValidRow(maxSourceRowIndex) || rowNum < maxSourceRowIndex);
    }

    bool isValidRow(const int rowNum) const {
        return rowNum > 0 && rowNum < sourceModel()->rowCount();
    }

    int minSourceRowIndex = -1;
    int maxSourceRowIndex = -1;
    int rowLimit = -1;
    //int lastRowSourceRowNum = -1;
};

Q_DECLARE_METATYPE(PagedItemTableProxyFilterModel*)
Q_DECLARE_METATYPE(std::shared_ptr<PagedItemTableProxyFilterModel>)

class ItemPageListModel : public QAbstractListModel
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

  explicit ItemPageListModel(QObject *pParent = nullptr) : QAbstractListModel(pParent) {
  }

  virtual ~ItemPageListModel() {
  }

  int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const Q_DECL_OVERRIDE {
    return m_pages.size();
  }

  int columnCount(const QModelIndex &parent /*= QModelIndex()*/) const Q_DECL_OVERRIDE {
    return static_cast<int>(Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE
  {
    qDebug() << "setData ItemPageListModel " << value << value.toString();

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

    QVector<int> roles;
    roles << role;
    if (!replaceItemAt(index.row(), item, roles)) { // emits dataChanged signal
      qDebug() << "ItemPageListModel::setData: invalid index.row() " << index.row();
    }

    //emit dataChanged(index, index);
    return true;
  }

  QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE {
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

  void pushBack(const std::shared_ptr<ItemListModel> page) {
    const int oldRows = itemsTotal();

    /*
        First is the new index of the first element that will be inserted.
        Last is the new index of the last element that will be inserted.
        Since we're appending only one element at the end of the list, the
        index of the first and last elements is the same, and is equal to
        the current size of the list.
    */
    const int first = oldRows;
    const int last = first;
    emit beginInsertRows(QModelIndex(), first, last);

    m_pages.push_back(page);

    //QModelIndex indexItem = index(rowCount(QModelIndex()), static_cast<int>(Columns::Page));

    //emit dataChanged(indexItem, indexItem);

    emit endInsertRows();
  }

  bool removeItemAt(int rowIndex) {
    if(rowIndex < 0 || rowIndex >= m_pages.size()) {
      return false;
    }

    emit beginRemoveRows(QModelIndex(), rowIndex, rowIndex);

    m_pages.removeAt(rowIndex);

    //QModelIndex indexItem = index(rowIndex, static_cast<int>(Columns::Page));
    //emit dataChanged(indexItem, indexItem);

    emit endRemoveRows();

    return true;
  }

  bool replaceItemAt(int rowIndex, std::shared_ptr<ItemListModel> newItem, QVector<int> roles = QVector<int>()) {
    if(rowIndex < 0 || rowIndex >= m_pages.size()) {
      return false;
    }

    m_pages.replace(rowIndex, newItem);

    QModelIndex indexItem = index(rowIndex, static_cast<int>(Columns::Page));
    emit dataChanged(indexItem, indexItem, roles);

    return true;
  }

  const QList<std::shared_ptr<ItemListModel>> getItems() const {
    return m_pages;
  }

  int itemsTotal() const {
    return m_pages.size();
  }

  void setItems(const QList<std::shared_ptr<ItemListModel>>& items) {
    removeAllItems(); // emit row remove signal

    emit beginInsertRows(QModelIndex(), items.size(), items.size());

    m_pages = items;

    emit endInsertRows();
  }

  const std::shared_ptr<ItemListModel> getItemAt(int index) const {
    if(index < 0 || index >= m_pages.size()) {
      return nullptr;
    }

    return m_pages.at(index);
  }

  void removeAllItems() {
    emit beginRemoveRows(QModelIndex(), 0, itemsTotal());

    m_pages.clear();

    emit endRemoveRows();
  }

private:
  QList<std::shared_ptr<ItemListModel>> m_pages;
};

//Q_DECLARE_METATYPE(PagedItemModel)
Q_DECLARE_METATYPE(ItemPageListModel*)
Q_DECLARE_METATYPE(std::shared_ptr<ItemPageListModel>)

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


class PagedItemListProxyFilterModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
  enum class Columns {
    Page = 0
    , Total
  };

  enum class WorkMode {
    Online = 0
    , Offline
    , Total
  };

  explicit PagedItemListProxyFilterModel(QObject *pParent = nullptr) : QAbstractProxyModel(pParent) {
    connect(this, SIGNAL(sourceModelChanged()), this, SLOT(slotSourceModelChanged()));
  }

  virtual ~PagedItemListProxyFilterModel() {
  }

  void setPageSize(int pageSize) {
    m_pageSize = pageSize;
  }

  int getPageSize() const {
    return m_pageSize;
  }

  void setWorkMode(WorkMode mode) {
    m_workMode = mode;
  }

  WorkMode getWorkMode() const {
    return m_workMode;
  }

  void setOnlinePagesTotal(int val) {
    m_onlinePagesTotal = val;
  }

  int getOnlinePagesTotal() const {
    return m_onlinePagesTotal;
  }

public slots:
  void slotSourceModelChanged(void);
  void slotDataChanged(const QModelIndex& first, const QModelIndex& last);
  void slotRowsInserted(const QModelIndex& parent, int first, int last);
  void slotRowsRemoved(const QModelIndex &parent, int start, int end);
  void sourceReset();

protected:

  int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const Q_DECL_OVERRIDE {
    if (m_workMode == WorkMode::Online && getOnlinePagesTotal() > 0) {
      return getOnlinePagesTotal();
    }

    //return sourceModel()->rowCount();
    std::div_t res = std::div(sourceModel()->rowCount(), getPageSize());
    // Fast ceiling of an integer division
    int pagesTotal = res.rem ? (res.quot + 1) : res.quot;
    return pagesTotal;
  }

  int columnCount(const QModelIndex &parent /*= QModelIndex()*/) const Q_DECL_OVERRIDE {
    return static_cast<int>(Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE
  {
    qDebug() << "setData ItemListModel " << value << value.toString();

    if (!index.isValid()) {
      return false;
    }

    /*ItemListModel* source = getSourceListModel();
    if (!source) {
      return false;
    }

    std::shared_ptr<ItemMapper> mapper = getSourceListModel()->getItemAt(index.row());
    if(!mapper.get()) {
      return false;
    }

    ItemModel* model = static_cast<ItemModel*>(mapper->model());
    if(!model) {
      return false;
    }

    switch(index.column())
    {
      case static_cast<int>(ItemModel::Columns::Name):
        model->setName(value.toString());
        break;
      case static_cast<int>(ItemModel::Columns::Surname):
        model->setSurname(value.toString());
        break;
      case static_cast<int>(ItemModel::Columns::GUID):
        model->setGUID(value.toInt());
        break;
    }

    emit dataChanged(index, index);
    return true;*/

    return false; // read only data
  }

  QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const Q_DECL_OVERRIDE {
    //qDebug() << "data" << index.column() << role;

    //return QVariant("QVariant");

    if (role == Qt::SizeHintRole) {
      return QVariant(QSize( 25, 25 ));
    }

    if(role != Qt::DisplayRole && role != Qt::EditRole){
      return QVariant(); // disable tooltips, icons, e.t.c
    }

    if (!index.isValid()) {
      return QVariant();
    }

    QVariant result;

    //result = QString("page %1").arg(index.row());

    PagedItemTableProxyFilterModel* pagedItemTableProxyFilterModel = static_cast<PagedItemTableProxyFilterModel*>(sourceModel());

    ItemTableProxyModel* itemTableProxyModel = static_cast<ItemTableProxyModel*>(pagedItemTableProxyFilterModel->sourceModel());
    ItemListModel* itemListModel = static_cast<ItemListModel*>(itemTableProxyModel->sourceModel());
    // m_itemListModelCache = std::make_shared<ItemListModel>();
    //ItemTableProxyModel* itemTableProxyModel = static_cast<ItemTableProxyModel*>(pagedItemTableProxyFilterModel->sourceModel());

    QList<std::shared_ptr<ItemMapper>> items;
    int pageStartCursor = index.row() * getPageSize();
    for (int i = pageStartCursor; i < pageStartCursor + getPageSize(); i++) {
      QModelIndex idx = pagedItemTableProxyFilterModel->index(i, static_cast<int>(ItemModel::Columns::GUID));
      QVariant data = pagedItemTableProxyFilterModel->data(idx, Qt::DisplayRole);
      //qDebug() << "data = pagedItemTableProxyFilterModel" << data;
      if (!data.isValid()) {
        continue;
      }
      int dataGUID = data.value<QVariant>().toInt();

      std::shared_ptr<ItemMapper> item = itemListModel->getItemById(dataGUID);
      if (!item) {
        continue;
      }
      items.push_back(item);
    }

    /*ItemTableProxyModel* itemTableProxyModel = static_cast<ItemTableProxyModel*>(pagedItemTableProxyFilterModel->sourceModel());
    ItemListModel* itemListModel = static_cast<ItemListModel*>(itemTableProxyModel->sourceModel());
    // m_itemListModelCache = std::make_shared<ItemListModel>();
    //ItemTableProxyModel* itemTableProxyModel = static_cast<ItemTableProxyModel*>(pagedItemTableProxyFilterModel->sourceModel());

    QList<std::shared_ptr<ItemMapper>> items;
    int pageStartCursor = index.row() * getPageSize();
    for (int i = pageStartCursor; i < pageStartCursor + getPageSize(); i++) {
      std::shared_ptr<ItemMapper> item = itemListModel->getItemById(i);
      if (!item) {
        continue;
      }
      items.push_back(item);
    }*/

    //if (role == Qt::DisplayRole)
    //  return QVariant("QVariant");

    /*if (index.column() == static_cast<int>(Columns::DebugName)) {
      return QVariant(static_cast<ItemModel*>(item->model())->getName());
    }*/

    result = QVariant::fromValue(items);

    /*std::shared_ptr<ItemMapper> mapper = getSourceListModel()->getItemAt(index.row());
    if(!mapper.get()) {
      return result;
    }

    ItemModel* model = static_cast<ItemModel*>(mapper->model());
    if(!model) {
      return result;
    }

    switch(index.column())
    {
      case static_cast<int>(ItemModel::Columns::Name):
        result = model->getName();
        break;
      case static_cast<int>(ItemModel::Columns::Surname):
        result = model->getSurname();
        break;
      case static_cast<int>(ItemModel::Columns::GUID):
        result = model->getGUID();
        break;
    }*/

    return result;
  }

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    QModelIndex res;
    if(!parent.isValid())
    {
      res = createIndex(row, column);
    }
    return res;
  }

  QModelIndex parent(const QModelIndex & /*child*/) const Q_DECL_OVERRIDE {
    return QModelIndex();
  }

  QModelIndex mapToSource(const QModelIndex &proxyIndex)  const Q_DECL_OVERRIDE {
    QModelIndex res;
    if(proxyIndex.isValid())
    {
      if(proxyIndex.column() >= sourceModel()->columnCount() && proxyIndex.column() <= (static_cast<int>(ItemModel::Columns::Total) - 1))
      {
         // res = createIndex(proxyIndex.row(), -1, (quintptr)-1);
         res = createIndex(proxyIndex.row(), proxyIndex.column(), (quintptr)-1);
      }
      else// if(proxyIndex.column() < sourceModel()->columnCount())
      {
         res = sourceModel()->index(proxyIndex.row(), proxyIndex.column());
      }
      /*else
      {
         res = sourceModel()->index(proxyIndex.row(), proxyIndex.column()-m_nbCol);
      }*/
    }
    return res;
  }

  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE {
    QModelIndex res;
    /*if(!sourceIndex.isValid())
    {
      if((sourceIndex.row() > -1) &&
         (sourceIndex.column() >= sourceModel()->columnCount() && sourceIndex.column() <= (static_cast<int>(ItemModel::Columns::Total) - 1)) &&
         (sourceIndex.internalId() == (quintptr)(sourceIndex.column())))
      {
         res = index(sourceIndex.row(), sourceIndex.column());
      }
    }
    else
    {
      res = index(sourceIndex.row(), sourceIndex.column());
    }*/
    res = index(sourceIndex.row(), sourceIndex.column());
    return res;
  }

  Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE
  {
    if (!index.isValid())
      return Qt::ItemIsEnabled;

    /*return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;*/

   Qt::ItemFlags res;
   if(index.column() >= sourceModel()->columnCount() && index.column() <= (static_cast<int>(ItemModel::Columns::Total) - 1))
   {
      res = QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
   }
   else
   {
      res = sourceModel()->flags(mapToSource(index));
   }
   return res;
  }

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column: %1").arg(section);
    else
        return QString("Row: %1").arg(section);
  }

private:
  int m_pageSize = 2;
  WorkMode m_workMode = WorkMode::Total;
  int m_onlinePagesTotal = -1;
};

Q_DECLARE_METATYPE(PagedItemListProxyFilterModel*)
Q_DECLARE_METATYPE(std::shared_ptr<PagedItemListProxyFilterModel>)

#endif // PAGEDITEMMODEL_H
