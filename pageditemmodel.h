#include <QtCore>
#include <QAbstractListModel>
#include <QDataWidgetMapper>
#include <QLineEdit>
#include <QCheckBox>
#include <QDoubleSpinBox>

#ifndef PAGEDITEMMODEL_H
#define PAGEDITEMMODEL_H

struct Item
{
    int guid;
    QString name;
    QString surname;
};

Q_DECLARE_METATYPE(Item)
Q_DECLARE_METATYPE(Item*)

class ItemModel : public QAbstractListModel
{
    Q_OBJECT

Q_SIGNALS:
    void guidChanged(const int &prevGUID, const int &newGUID);

public:
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

  QVariant getDataByColumn(int column) const {
    QVariant result;
    switch(column)
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

  bool setDataByColumn(int column, const QVariant &value) {
    if(column < 0 || column >= static_cast<int>(Columns::Total)) {
      return false;
    }

    if(!value.isValid()) {
      return false;
    }

    switch(column)
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
    return true;
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

  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    return 1;
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    return static_cast<int>(Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE
  {
    //qDebug() << "setData PersonsModel " << value << value.toString();

    if (!index.isValid())
      return false;

    return setDataByColumn(index.column(), value);
  }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE {
    if (!index.isValid())
      return false;

    QVariant result = getDataByColumn(index.column());

    return result;
  }

private:
  Item m_item;
};

Q_DECLARE_METATYPE(ItemModel*)

class ItemMapper : public QDataWidgetMapper
{
    Q_OBJECT
public:
  explicit ItemMapper(QObject *pParent = nullptr) : QDataWidgetMapper(pParent)
  {
  }
  virtual ~ItemMapper()
  {}
};

Q_DECLARE_METATYPE(ItemMapper*)
Q_DECLARE_METATYPE(std::shared_ptr<ItemMapper>)

class ItemListModel : public QAbstractListModel
{
    Q_OBJECT

public:
  enum class Columns {
    Item = 0
    , Total
  };

  static const int dummyItemId = std::numeric_limits<int>::min();

  explicit ItemListModel(QObject *pParent = nullptr) : QAbstractListModel(pParent) {
    setDummyItemMapper();
  }

  virtual ~ItemListModel() {
  }

  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    return m_items.size();
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    return static_cast<int>(Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE
  {
    //qDebug() << "setData ItemListModel " << value << value.toString();

    if (!index.isValid()) {
      return false;
    }

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

  Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE
  {
    if (!index.isValid())
      return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
  }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE {
    if (!index.isValid()) {
      return QVariant();
    }

    if (role == Qt::SizeHintRole) {
      return QVariant(QSize( 25, 25 ));
    }

    if(role != Qt::DisplayRole && role != Qt::EditRole){
      return QVariant(); // disable tooltips, icons, e.t.c
    }

    std::shared_ptr<ItemMapper> item = m_items.at(index.row());

    if (!item)
      return QVariant();

    QVariant result;

    result = QVariant::fromValue(item);

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

    if (item) {
      int newGuid = static_cast<ItemModel*>(item->model())->getGUID();
      //m_guidToItem[newGuid] = item;
    }

    emit endInsertRows();

    const int itemRowId = oldRows + 1;
    const QModelIndex indexMapped = index(itemRowId, static_cast<int>(Columns::Item));

    if (item) {
      connect(item->model(), &QAbstractItemModel::dataChanged, this, [this, indexMapped](const QModelIndex first, QModelIndex last){
        //qDebug() << "connect(item->model(), &QAbstractItemModel::dataChanged" << indexMapped;
        emit dataChanged(indexMapped, indexMapped);
      });
    }
  }

  bool removeItemAt(int rowIndex) {
    if(rowIndex < 0 || rowIndex >= itemsTotal()) {
      //qDebug() << "removeItemAt skippeeeed"  <<  rowIndex;
      return false;
    }

    //qDebug() << "removeItemAt"  <<  rowIndex;

    //auto idx = index(rowIndex, static_cast<int>(Columns::Item));
    emit beginRemoveRows(QModelIndex(), rowIndex, rowIndex);

    std::shared_ptr<ItemMapper> prevItem = getItemAt(rowIndex);
    if (prevItem.get()) {
      int oldGuid = static_cast<ItemModel*>(prevItem->model())->getGUID();
      //m_guidToItem.remove(oldGuid);

      m_items.removeAt(rowIndex);
    }

    emit endRemoveRows();

    return true;
  }

  void setDummyItemMapper() {
    m_dummyItemMapper = std::make_shared<ItemMapper>();
    ItemModel* itemModel = new ItemModel();
    itemModel->setParent(m_dummyItemMapper.get());
    //itemModel->setName("");
    //itemModel->setSurname("");
    itemModel->setGUID(dummyItemId);
    /// \note allows two-way data editing
    m_dummyItemMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_dummyItemMapper->setModel(itemModel);
    m_dummyItemMapper->toFirst();
  }

  void setRowsTotalSpace(int totalRows) {
    const int diff = std::abs(totalRows - rowCount());
    const int rowCountBeforeDelete = rowCount();
    //qDebug() << "setRowsTotalSpace" << totalRows << rowCount() << diff;
    if (rowCount() < totalRows) {
      for (int i = 0; i < diff; i++) {
        /// \note need to emit beginInsertRows/endInsertRows/e.t.c
        //pushBack(nullptr);
        pushBack(m_dummyItemMapper);
      }
    } else {
      for (int i = 0; i < diff; i++) {
        /// \note need to emit beginInsertRows/endInsertRows/e.t.c
        removeItemAt(rowCountBeforeDelete - 1 - i);
      }
    }
  }

  bool replaceItemAt(int rowIndex, std::shared_ptr<ItemMapper> newItem, QVector<int> roles = QVector<int>()) {
    if(rowIndex < 0 || rowIndex >= itemsTotal()) {
      return false;
    }

    const int oldRows = rowCount();

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

    std::shared_ptr<ItemMapper> prevItem = getItemAt(rowIndex);
    if (prevItem.get()) {
      ItemModel* itemModel = static_cast<ItemModel*>(prevItem->model());
      if(itemModel) {
        int oldGuid = itemModel->getGUID();
        //m_guidToItem.remove(oldGuid);
      }
    }

    m_items.replace(rowIndex, newItem);

    {
      ItemModel* itemModel = static_cast<ItemModel*>(newItem->model());
      if(itemModel) {
        int newGuid = itemModel->getGUID();
        //m_guidToItem[newGuid] = newItem;
      }
    }

    emit endInsertRows();

    const int itemRowId = oldRows + 1;
    const QModelIndex indexMapped = index(itemRowId, static_cast<int>(Columns::Item));

    if (newItem) {
      connect(newItem->model(), &QAbstractItemModel::dataChanged, this, [this, indexMapped](const QModelIndex first, QModelIndex last){
        //qDebug() << "connect(item->model(), &QAbstractItemModel::dataChanged" << indexMapped;
        emit dataChanged(indexMapped, indexMapped);
      });
    }

    return true;
  }

  const QList<std::shared_ptr<ItemMapper>> getItems() const {
    return m_items;
  }

  int itemsTotal() const {
    return m_items.size();
  }

  void setItems(const QList<std::shared_ptr<ItemMapper>>& items) {
    removeAllItems(); // emit row remove signal

    emit beginInsertRows(QModelIndex(), items.size(), items.size());

    m_items = items;

    for (auto& newItem: items) {
      ItemModel* itemModel = static_cast<ItemModel*>(newItem->model());
      if(itemModel) {
        int newGuid = itemModel->getGUID();
        //m_guidToItem[newGuid] = newItem;
      }
    }

    emit endInsertRows();
  }

  const std::shared_ptr<ItemMapper> getItemAt(int rowIndex) const {
    if(rowIndex < 0 || rowIndex >= m_items.size()) {
      return nullptr;
    }

    return m_items.at(rowIndex);
  }

  /*const std::shared_ptr<ItemMapper> getItemById(int guid) const {
    if(!m_guidToItem.contains(guid)) {
      qDebug() << "ItemListModel not contains guid = " << guid;
      return nullptr;
    }

    QHash<int, std::shared_ptr<ItemMapper>>::const_iterator it = m_guidToItem.find(guid);

    return it.value();
  }*/

  void removeAllItems() {
    emit beginRemoveRows(QModelIndex(), 0, itemsTotal());

    m_items.clear();
    //m_guidToItem.clear();

    emit endRemoveRows();
  }

private:
  //QHash<int, std::shared_ptr<ItemMapper>> m_guidToItem;
  QList<std::shared_ptr<ItemMapper>> m_items;
  std::shared_ptr<ItemMapper> m_dummyItemMapper;
};

Q_DECLARE_METATYPE(ItemListModel*)
Q_DECLARE_METATYPE(std::shared_ptr<ItemListModel>)

class ItemTableProxyModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
  enum class Columns {
     // adds exta column equal to row number of item in source model
    SourceMappedRowNum = static_cast<int>(ItemModel::Columns::Total)
    , Total
  };

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

  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    ItemListModel* source = getSourceListModel();
    if (!source) {
      return 0;
    }

    return source->getItems().size();
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    return static_cast<int>(ItemTableProxyModel::Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE
  {
    qDebug() << "setData ItemListModel " << value << value.toString();

    if (!index.isValid()) {
      return false;
    }

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

    bool isDataChanged = false;
    if (index.column() < static_cast<int>(ItemModel::Columns::Total)) {
      // return same data as in source item
      isDataChanged = model->setDataByColumn(index.column(), value);
    } else {
      // custom columns
      switch(index.column())
      {
        case static_cast<int>(ItemTableProxyModel::Columns::SourceMappedRowNum):
          // skip, no data change here
          isDataChanged = false;
          break;
      }
    }

    if (isDataChanged) {
      emit dataChanged(index, index);
    }

    return isDataChanged;
  }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE {
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

    if (index.column() < static_cast<int>(ItemModel::Columns::Total)) {
      // return same data as in source item
      result = model->getDataByColumn(index.column());
    } else {
      // custom columns
      switch(index.column())
      {
        case static_cast<int>(ItemTableProxyModel::Columns::SourceMappedRowNum):
          result = QVariant::fromValue(index.row());
          break;
      }
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

  QModelIndex parent(const QModelIndex& child) const Q_DECL_OVERRIDE {
    return QModelIndex();
  }

  QModelIndex mapToSource(const QModelIndex &proxyIndex)  const Q_DECL_OVERRIDE {
    QModelIndex res;
    if(proxyIndex.isValid())
    {
      if(proxyIndex.column() >= sourceModel()->columnCount() && proxyIndex.column() <= (static_cast<int>(ItemModel::Columns::Total) - 1))
      {
         res = createIndex(proxyIndex.row(), proxyIndex.column(), (quintptr)-1);
      } else {
         res = sourceModel()->index(proxyIndex.row(), proxyIndex.column());
      }
    }
    return res;
  }

  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE {
    QModelIndex res;
    res = index(sourceIndex.row(), sourceIndex.column());
    return res;
  }

  Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE
  {
    if (!index.isValid())
      return Qt::ItemIsEnabled;

   Qt::ItemFlags res;
   if(index.column() >= sourceModel()->columnCount() && index.column() <= (static_cast<int>(ItemTableProxyModel::Columns::Total) - 1))
   {
      // adds exta columns
      res = QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
   } else {
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
      connect(this, SIGNAL(sourceModelChanged()), this, SLOT(slotSourceModelChanged()));
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
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
      auto filteredRows = QSortFilterProxyModel::rowCount();
      //qDebug() << "filteredRows1" << filteredRows;

      if (getRowLimit() >= 0)
        return std::min(filteredRows, getRowLimit());
      //qDebug() << "filteredRows2" << filteredRows;

      return filteredRows;
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
      return static_cast<int>(ItemTableProxyModel::Columns::Total);
    }

protected:

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const Q_DECL_OVERRIDE {
        const QModelIndex indexName = sourceModel()->index(sourceRow, static_cast<int>(ItemModel::Columns::Name), sourceParent);
        const bool isNameFiltered = sourceModel()->data(indexName).toString().contains(filterRegExp());

        const QModelIndex indexSurname = sourceModel()->index(sourceRow, static_cast<int>(ItemModel::Columns::Surname), sourceParent);
        const bool isSurnameFiltered = sourceModel()->data(indexSurname).toString().contains(filterRegExp());

        const QModelIndex indexGuid = sourceModel()->index(sourceRow, static_cast<int>(ItemModel::Columns::GUID), sourceParent);
        const bool isGuidFiltered = sourceModel()->data(indexGuid).toString().contains(filterRegExp());

        bool isGuidValid = sourceModel()->data(indexGuid).toInt() != ItemListModel::dummyItemId;

        const bool anyFieldMatch = (isNameFiltered
                //|| isSurnameFiltered
                );

        return isGuidValid && anyFieldMatch && rowInRange(sourceRow);
    }

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const Q_DECL_OVERRIDE {
        return left.row() < right.row(); // sort from first added to last added
    }

private:
    bool rowInRange(const int rowNum) const {
        return (!isValidRow(minSourceRowIndex) || rowNum >= minSourceRowIndex)
                && (!isValidRow(maxSourceRowIndex) || rowNum < maxSourceRowIndex);
    }

    bool isValidRow(const int rowNum) const {
        return rowNum > 0 && rowNum < sourceModel()->rowCount();
    }

    int minSourceRowIndex = -1;
    int maxSourceRowIndex = -1;
    int rowLimit = -1;
};

Q_DECLARE_METATYPE(PagedItemTableProxyFilterModel*)
Q_DECLARE_METATYPE(std::shared_ptr<PagedItemTableProxyFilterModel>)

class PagedItemMapper : public QDataWidgetMapper
{
    Q_OBJECT
public:
  explicit PagedItemMapper(QObject *pParent = nullptr) : QDataWidgetMapper(pParent)
  {
  }
  virtual ~PagedItemMapper()
  { }
};

Q_DECLARE_METATYPE(PagedItemMapper*)
Q_DECLARE_METATYPE(std::shared_ptr<PagedItemMapper>)

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

  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    if (m_workMode == WorkMode::Online && getOnlinePagesTotal() > 0) {
      return getOnlinePagesTotal();
    }

    std::div_t res = std::div(sourceModel()->rowCount(), getPageSize());
    // Fast ceiling of an integer division
    int pagesTotal = res.rem ? (res.quot + 1) : res.quot;
    return pagesTotal;
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    return static_cast<int>(Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE
  {
    //qDebug() << "setData ItemListModel " << value << value.toString();

    if (!index.isValid()) {
      return false;
    }

    return false; // read only data
  }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE {
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

    PagedItemTableProxyFilterModel* pagedItemTableProxyFilterModel = static_cast<PagedItemTableProxyFilterModel*>(sourceModel());

    ItemTableProxyModel* itemTableProxyModel = static_cast<ItemTableProxyModel*>(pagedItemTableProxyFilterModel->sourceModel());
    ItemListModel* itemListModel = static_cast<ItemListModel*>(itemTableProxyModel->sourceModel());

    QList<std::shared_ptr<ItemMapper>> items;
    /*int pageStartCursor = index.row() * getPageSize();
    for (int i = pageStartCursor; i < pageStartCursor + getPageSize(); i++) {
      QModelIndex idx = pagedItemTableProxyFilterModel->index(i, static_cast<int>(ItemTableProxyModel::Columns::GUID));
      QVariant data = pagedItemTableProxyFilterModel->data(idx, Qt::DisplayRole);
      qDebug() << "pageStartCursor" << data << i;
      if (!data.isValid()) {
        continue;
      }
      int dataGUID = data.value<QVariant>().toInt();

      std::shared_ptr<ItemMapper> item = itemListModel->getItemById(dataGUID);
      if (!item) {
        continue;
      }
      items.push_back(item);
    }*/

    int pageStartCursor = 0;//index.row();//0;//index.row() * getPageSize();
    for (int i = pageStartCursor; i < pageStartCursor + getPageSize(); i++) {
      //QModelIndex idx = pagedItemTableProxyFilterModel->index(i, static_cast<int>(ItemModel::Columns::GUID));
      QModelIndex idx = pagedItemTableProxyFilterModel->index(i, static_cast<int>(ItemTableProxyModel::Columns::SourceMappedRowNum));
      QVariant data = pagedItemTableProxyFilterModel->data(idx, Qt::DisplayRole);
      qDebug() << "pageStartCursor" << data << i;
      if (!data.isValid()) {
        continue;
      }

      int SourceMappedRowNum = data.value<QVariant>().toInt();
      std::shared_ptr<ItemMapper> item = itemListModel->getItemAt(SourceMappedRowNum);
      if (!item) {
        continue;
      }
      items.push_back(item);

      /*int dataGUID = data.value<QVariant>().toInt();
      std::shared_ptr<ItemMapper> item = itemListModel->getItemById(dataGUID);
      if (!item) {
        continue;
      }
      items.push_back(item);*/

      /*std::shared_ptr<ItemMapper> item = itemListModel->getItemAt(index.row() * getPageSize() + i);
      if (!item) {
        continue;
      }
      items.push_back(item);*/
    }

    result = QVariant::fromValue(items);

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

  QModelIndex parent(const QModelIndex& child) const Q_DECL_OVERRIDE {
    return QModelIndex();
  }

  QModelIndex mapToSource(const QModelIndex &proxyIndex)  const Q_DECL_OVERRIDE {
    QModelIndex res;
    if(proxyIndex.isValid())
    {
      if(proxyIndex.column() >= sourceModel()->columnCount() && proxyIndex.column() <= (static_cast<int>(ItemTableProxyModel::Columns::Total) - 1))
      {
         res = createIndex(proxyIndex.row(), proxyIndex.column(), (quintptr)-1);
      } else {
         res = sourceModel()->index(proxyIndex.row(), proxyIndex.column());
      }
    }
    return res;
  }

  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE {
    QModelIndex res;
    res = index(sourceIndex.row(), sourceIndex.column());
    return res;
  }

  Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE
  {
    if (!index.isValid())
      return Qt::ItemIsEnabled;

   Qt::ItemFlags res;
   if(index.column() >= sourceModel()->columnCount() && index.column() <= (static_cast<int>(ItemTableProxyModel::Columns::Total) - 1))
   {
      // adds exta columns
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
  int m_pageSize = 1;
  WorkMode m_workMode = WorkMode::Total;
  int m_onlinePagesTotal = -1;
};

Q_DECLARE_METATYPE(PagedItemListProxyFilterModel*)
Q_DECLARE_METATYPE(std::shared_ptr<PagedItemListProxyFilterModel>)

#endif // PAGEDITEMMODEL_H
