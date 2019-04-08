#include <QtCore>
#include <QAbstractListModel>
#include <QDataWidgetMapper>
#include <QLineEdit>
#include <QCheckBox>
#include <QDoubleSpinBox>

#ifndef PAGEDITEMMODEL_H
#define PAGEDITEMMODEL_H

class Item
{
public:
  Item()
    : m_guid(std::numeric_limits<int>::min()), m_name(""), m_surname("")
  { }

  Item(int guid, const QString& name, const QString& surname)
    : m_guid(guid), m_name(name), m_surname(surname)
  { }

  QString toString() const {
    return QString::number(m_guid) + " " + m_name + " " + m_surname;
  }

  QString getName() const {
    return m_name;
  }

  void setName(const QString& sString) {
    m_name = sString;
  }

  QString getSurname() const {
    return m_surname;
  }

  void setSurname(const QString& sString) {
    m_surname = sString;
  }

  int getGUID() const {
    return m_guid;
  }

  void setGUID(const int GUID) {
    m_guid = GUID;
  }

private:
    int m_guid;
    QString m_name;
    QString m_surname;
};

Q_DECLARE_METATYPE(Item)
Q_DECLARE_METATYPE(Item*)

/// \brief create model for item. Allows to add extra columns.
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
    , ItemMode // custom column, allows to mark unloaded items e.t.c.
    , Total
  };

  enum class ItemMode {
    Visible = 0
    , NotLoaded
    , Total
  };

  explicit ItemModel(QObject *pParent = nullptr) : QAbstractListModel(pParent) {
  }

  virtual ~ItemModel() {
  }

  QString itemAsString() const {
    return m_item.toString();
  }

  QString getName() const {
    return m_item.getName();
  }

  void setName(const QString& sString) {
    m_item.setName(sString);

    QModelIndex indexItem = index(0, static_cast<int>(Columns::Name));
    if(!indexItem.isValid()) {
      return;
    }
    emit dataChanged(indexItem, indexItem);
  }

  ItemMode getItemMode() const {
    return m_itemMode;
  }

  void setItemMode(const ItemMode& mode) {
    m_itemMode = mode;

    // need to emit dataChanged with any column
    QModelIndex indexItem = index(0, static_cast<int>(Columns::Name));
    if(!indexItem.isValid()) {
      return;
    }
    emit dataChanged(indexItem, indexItem);
  }

  QString getSurname() const {
    return m_item.getSurname();
  }

  void setSurname(const QString& sString) {
    m_item.setSurname(sString);

    QModelIndex indexItem = index(0, static_cast<int>(Columns::Surname));
    if(!indexItem.isValid()) {
      return;
    }
    emit dataChanged(indexItem, indexItem);
  }

  int getGUID() const {
    return m_item.getGUID();
  }

  void setGUID(const int GUID) {
    const int prevGUID = m_item.getGUID();
    m_item.setGUID(GUID);

    emit guidChanged(prevGUID, m_item.getGUID());

    QModelIndex indexItem = index(0, static_cast<int>(Columns::GUID));
    if(!indexItem.isValid()) {
      return;
    }
    emit dataChanged(indexItem, indexItem);
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
      case static_cast<int>(Columns::ItemMode):
        result = static_cast<int>(getItemMode());
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
      {
        if (!value.canConvert<QString>()) {
          return false;
        }
        setName(value.toString()); // emits dataChanged signal
        break;
      }
      case static_cast<int>(Columns::Surname):
      {
        if (!value.canConvert<QString>()) {
          return false;
        }
        setSurname(value.toString()); // emits dataChanged signal
        break;
      }
      case static_cast<int>(Columns::GUID):
      {
        if (!value.canConvert<int>()) {
          return false;
        }
        setGUID(value.toInt()); // emits dataChanged signal
        break;
      }
      case static_cast<int>(Columns::ItemMode):
      {
        if (!value.canConvert<int>()) {
          return false;
        }
        setItemMode(static_cast<ItemMode>(value.toInt())); // emits dataChanged signal
        break;
      }
    }
    return true;
  }

  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    Q_UNUSED(parent);
    Q_ASSERT(checkIndex(parent, QAbstractItemModel::CheckIndexOption::ParentIsInvalid)); // flat model

    /// \note modelholds 1 item
    return 1;
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    Q_UNUSED(parent);

    return static_cast<int>(Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE
  {
    Q_UNUSED(role);

    if (!index.isValid()) {
      return false;
    }

    return setDataByColumn(index.column(), value);
  }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE {
    Q_UNUSED(role);

    if (!index.isValid()) {
      return false;
    }

    QVariant result = getDataByColumn(index.column());

    return result;
  }

  // Returns the parent of the model index, or QModelIndex() if it has no parent.
  QModelIndex parent(const QModelIndex& child) const Q_DECL_OVERRIDE {
    Q_UNUSED(child);
    return QModelIndex(); // flat model, no hierarchy
  }

  /*!
      Returns \c{true} if \a parent has any children; otherwise returns \c{false}.
      Use rowCount() on the parent to find out the number of children.
      Note that it is undefined behavior to report that a particular index hasChildren
      with this method if the same index has the flag Qt::ItemNeverHasChildren set.
      \sa parent(), index()
  */
  bool hasChildren(const QModelIndex &parent) const Q_DECL_OVERRIDE {
    Q_UNUSED(parent);

    return false; // flat model, no hierarchy
  }

private:
  Item m_item;
  ItemMode m_itemMode = ItemMode::Visible;
};

Q_DECLARE_METATYPE(ItemModel*)

/// \brief allows to map data changes in model to some widgets
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

/// \brief collection of item mappers.
/// \note we can can source model from mapper.
class ItemListModel : public QAbstractListModel
{
    Q_OBJECT

public:
  enum class Columns {
     /// \note zero column must be convertable to string
     /// \see `Tests model's implementation of QAbstractItemModel::data()`
     ///      in QAbstractItemModelTesterPrivate::data()
     ///      in qtbase/src/testlib/qabstractitemmodeltester.cpp
    DebugIdentifier = 0
    , Item
    , Total
  };

  explicit ItemListModel(QObject *pParent = nullptr) : QAbstractListModel(pParent) {
    setDummyNotLoaded();
  }

  virtual ~ItemListModel() {
  }

  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    Q_UNUSED(parent);
    Q_ASSERT(checkIndex(parent, QAbstractItemModel::CheckIndexOption::ParentIsInvalid)); // flat model

    return m_items.size();
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    Q_UNUSED(parent);

    return static_cast<int>(Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE
  {
    if (!index.isValid()) {
      return false;
    }

    std::shared_ptr<ItemMapper> itemMapper = qvariant_cast<std::shared_ptr<ItemMapper>>(value.value<QVariant>());
    if (!itemMapper) {
      return false;
    }

    QVector<int> roles;
    roles << role;
    if (!replaceItemAt(index.row(), itemMapper, roles)) { // emits changed signal
      qDebug() << "ItemListModel::setData: invalid index.row() " << index.row();
    }

    return true;
  }

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE {
    if (role != Qt::DisplayRole) {
      return QVariant();
    }

    if (orientation == Qt::Horizontal) {
      return QString("Column: %1").arg(section);
    } else {
      return QString("Row: %1").arg(section);
    }
  }

  Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE
  {
    if (!index.isValid()) {
      return 0;
    }

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
  }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE {
    if (!index.isValid()) {
      return QVariant();
    }

    if( index.row() >= m_items.count() )
    {
      qCritical( "ComputerListModel::data(): index out of range!" );
    }

    std::shared_ptr<ItemMapper> item = m_items.at(index.row());
    if (!item)
      return QVariant();

    ItemModel* model = static_cast<ItemModel*>(item->model());
    if (!model)
      return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        return QStringLiteral("%1").arg( model->getGUID() ); // disable tooltips, icons, e.t.c
    }

    switch( role )
    {
    case Qt::SizeHintRole:
      return QVariant(QSize( 25, 25 ));
    case Qt::ToolTipRole:
    //case Qt::DisplayRole:
    case Qt::DecorationRole:
    case Qt::InitialSortOrderRole:
      return model->itemAsString();
    case Qt::CheckStateRole:
      return QVariant();
    default:
      break;
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        return QStringLiteral("%1").arg( model->getGUID() ); // disable tooltips, icons, e.t.c
    }

    if (index.column() == static_cast<int>(Columns::DebugIdentifier)) {
      return QStringLiteral("%1").arg( model->getGUID() ); // disable tooltips, icons, e.t.c
    }

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

    /*if (item) {
      int newGuid = static_cast<ItemModel*>(item->model())->getGUID();
      m_guidToItem[newGuid] = item;
    }*/

    emit endInsertRows();

    const int itemRowId = oldRows + 1;
    const QModelIndex indexMapped = index(itemRowId, static_cast<int>(Columns::Item));
    if(!indexMapped.isValid()) {
      return;
    }

    if (item) {
      connect(item->model(), &QAbstractItemModel::dataChanged, this, [this, indexMapped](const QModelIndex first, QModelIndex last) {
        Q_UNUSED(first);
        Q_UNUSED(last);

        //sourceReset();

        emit dataChanged(indexMapped, indexMapped); // TODO
      });
    }
  }

  bool removeItemAt(int rowIndex) {
    if(rowIndex < 0 || rowIndex >= itemsTotal()) {
      qCritical() << "Invalid row index to remove " << rowIndex;
      return false;
    }

    const int beforeCount = itemsTotal();

    //qDebug() << "m_items.size() before" << beforeCount;

    emit beginRemoveRows(QModelIndex(), rowIndex, rowIndex);

    std::shared_ptr<ItemMapper> prevItem = getItemAt(rowIndex);
    if (prevItem.get()) {
      //int oldGuid = static_cast<ItemModel*>(prevItem->model())->getGUID();
      //m_guidToItem.remove(oldGuid);

      m_items.removeAt(rowIndex);
    } else {
      qCritical() << "Invalid item at row " << rowIndex;
      Q_ASSERT(false); // force DEBUG crash here
    }

    emit endRemoveRows();

    const int afterCount = itemsTotal();

    //qDebug() << "m_items.size() after" << m_items.size();

    Q_ASSERT(m_items.size() != beforeCount);
    Q_ASSERT(m_items.size() == afterCount);
    Q_ASSERT(beforeCount == (afterCount + 1));

    return true;
  }

  void setDummyNotLoaded() {
    m_dummyNotLoaded = std::make_shared<ItemMapper>();

    ItemModel* itemModel = new ItemModel();
    itemModel->setParent(m_dummyNotLoaded.get());
    itemModel->setItemMode(ItemModel::ItemMode::NotLoaded);

    /// \note allows two-way data editing
    m_dummyNotLoaded->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_dummyNotLoaded->setModel(itemModel);
    m_dummyNotLoaded->toFirst();
  }

  void setRowsTotalSpace(int totalRows) {
    const int startingRowCount = rowCount();
    const int diff = std::abs(totalRows - startingRowCount);
    if (rowCount() < totalRows) {
      for (int i = 0; i < diff; i++) {
        /// \note need to emit beginInsertRows/endInsertRows/e.t.c
        pushBack(m_dummyNotLoaded);
      }
    } else {
      for (int i = 0; i < diff; i++) {
        /// \note need to emit beginInsertRows/endInsertRows/e.t.c
        removeItemAt(startingRowCount - 1 - i);
      }
    }
  }

  void setRowsMinSpace(int totalRows) {
    const int startingRowCount = rowCount();
    const int diff = std::abs(totalRows - startingRowCount);
    if (rowCount() < totalRows) {
      for (int i = 0; i < diff; i++) {
        /// \note need to emit beginInsertRows/endInsertRows/e.t.c
        pushBack(m_dummyNotLoaded);
      }
    }
  }

  bool replaceItemAt(int rowIndex, std::shared_ptr<ItemMapper> newItem, QVector<int> roles = QVector<int>()) {
    Q_UNUSED(roles);

    if(rowIndex < 0 || rowIndex >= itemsTotal()) {
      return false;
    }

    if(!newItem.get()) {
      return false;
    }

    const int oldRowCount = itemsTotal();

    /*
        First is the new index of the first element that will be inserted.
        Last is the new index of the last element that will be inserted.
        Since we're appending only one element at the end of the list, the
        index of the first and last elements is the same, and is equal to
        the current size of the list.
    */
    const int first = rowIndex;
    const int last = first;

    //emit beginRemoveRows(QModelIndex(), first, last);
    //m_items.removeAt(rowIndex);
    //emit endRemoveRows();

    removeItemAt(rowIndex);

    emit beginInsertRows(QModelIndex(), first, last);

    /*std::shared_ptr<ItemMapper> prevItem = getItemAt(rowIndex);
    if (prevItem.get()) {
      ItemModel* itemModel = static_cast<ItemModel*>(prevItem->model());
      if(itemModel) {
        int oldGuid = itemModel->getGUID();
        m_guidToItem.remove(oldGuid);
      }
    }*/

    //m_items.replace(rowIndex, newItem);
    m_items.insert(rowIndex, newItem);

    ItemModel* m1 = static_cast<ItemModel*>(m_items.at(rowIndex)->model());
    ItemModel* m2 = static_cast<ItemModel*>(newItem->model());
    Q_ASSERT(m1->getGUID() == m2->getGUID() );

    /*{
      ItemModel* itemModel = static_cast<ItemModel*>(newItem->model());
      if(itemModel) {
        int newGuid = itemModel->getGUID();
        m_guidToItem[newGuid] = newItem;
      }
    }*/

    emit endInsertRows();

    const int itemRowId = rowIndex;
    const QModelIndex indexMapped = index(itemRowId, static_cast<int>(Columns::Item));
    if(!indexMapped.isValid()) {
      return false;
    }

    emit dataChanged(indexMapped, indexMapped);

    const int newRowCount = itemsTotal();
    Q_ASSERT(oldRowCount == newRowCount);

    if (newItem) {
      connect(newItem->model(), &QAbstractItemModel::dataChanged, this, [this, indexMapped](const QModelIndex first, QModelIndex last){
        Q_UNUSED(first);
        Q_UNUSED(last);

        //sourceReset();

        emit dataChanged(indexMapped, indexMapped); // TODO
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

#ifdef nope
  void setItems(const QList<std::shared_ptr<ItemMapper>>& items) {
    removeAllItems(); // emit row remove signal

    emit beginInsertRows(QModelIndex(), items.size(), items.size());

    m_items = items;

    /*for (auto& newItem: items) {
      ItemModel* itemModel = static_cast<ItemModel*>(newItem->model());
      if(itemModel) {
        int newGuid = itemModel->getGUID();
        m_guidToItem[newGuid] = newItem;
      }
    }*/

    emit endInsertRows();
  }
#endif

  const std::shared_ptr<ItemMapper> getItemAt(int rowIndex) const {
    if(rowIndex < 0 || rowIndex >= itemsTotal()) {
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
    qDebug() << "removeAllItems itemsTotal()" << itemsTotal();

    const int staringItemsTotal = itemsTotal();
    for( int i = 0; i < staringItemsTotal; i++) {
      qDebug() << "removeAllItems removeItemAt" << i;
      removeItemAt(staringItemsTotal - i - 1);
      Q_ASSERT((staringItemsTotal - i - 1) >= 0);
    }

    /*emit beginRemoveRows(QModelIndex(), 0, itemsTotal());

    m_items.clear();
    //m_guidToItem.clear();

    emit endRemoveRows();

    Q_ASSERT(itemsTotal() == 0);*/
  }

  // Returns the parent of the model index, or QModelIndex() if it has no parent.
  QModelIndex parent(const QModelIndex& child) const Q_DECL_OVERRIDE {
    Q_UNUSED(child);
    return QModelIndex(); // flat model, no hierarchy
  }

  /*!
      Returns \c{true} if \a parent has any children; otherwise returns \c{false}.
      Use rowCount() on the parent to find out the number of children.
      Note that it is undefined behavior to report that a particular index hasChildren
      with this method if the same index has the flag Qt::ItemNeverHasChildren set.
      \sa parent(), index()
  */
  bool hasChildren(const QModelIndex &parent) const Q_DECL_OVERRIDE {
    Q_UNUSED(parent);

    return false; // flat model, no hierarchy
  }

private:
  //QHash<int, std::shared_ptr<ItemMapper>> m_guidToItem;
  QList<std::shared_ptr<ItemMapper>> m_items;
  std::shared_ptr<ItemMapper> m_dummyNotLoaded;
};

Q_DECLARE_METATYPE(ItemListModel*)
Q_DECLARE_METATYPE(std::shared_ptr<ItemListModel>)

/// \brief It takes multiple source models and morths/concatenates SOME their rows and columns into a single model.
/// \see QConcatenateTablesProxyModelPrivate
/// https://git.younemo.com/alex/qtbase/src/5.13/src/corelib/itemmodels/qconcatenatetablesproxymodel.cpp
/// https://git.younemo.com/alex/qtbase/src/5.13/src/corelib/itemmodels/qconcatenatetablesproxymodel.h
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
    Q_UNUSED(parent);
    Q_ASSERT(checkIndex(parent, QAbstractItemModel::CheckIndexOption::ParentIsInvalid)); // flat model

    ItemListModel* source = getSourceListModel();
    if (!source) {
      Q_ASSERT(false); // force DEBUG crash here
      return 0;
    }
    //qDebug() << "rowCount source->getItems().size()" << source->getItems().size();
    return source->getItems().size();
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    Q_UNUSED(parent);

    //qDebug() << "columnCount static_cast<int>(ItemTableProxyModel::Columns::Total)" << static_cast<int>(ItemTableProxyModel::Columns::Total);
    return static_cast<int>(ItemTableProxyModel::Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE
  {
    Q_UNUSED(role);

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

    QVariant result;

    std::shared_ptr<ItemMapper> mapper = getSourceListModel()->getItemAt(index.row());
    if(!mapper.get()) {
      return result;
    }

    ItemModel* model = static_cast<ItemModel*>(mapper->model());
    if(!model) {
      return result;
    }

    if(role != Qt::DisplayRole && role != Qt::EditRole){
      return QVariant(); // disable tooltips, icons, e.t.c
    }

    switch( role )
    {
    case Qt::SizeHintRole:
      return QVariant(QSize( 25, 25 ));
    case Qt::ToolTipRole:
    //case Qt::DisplayRole:
    case Qt::DecorationRole:
    case Qt::InitialSortOrderRole:
      return model->itemAsString();
    case Qt::CheckStateRole:
      return QVariant();
    default:
      break;
    }

    if(role != Qt::DisplayRole && role != Qt::EditRole){
      return QVariant(); // disable tooltips, icons, e.t.c
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

  // Returns the parent of the model index, or QModelIndex() if it has no parent.
  QModelIndex parent(const QModelIndex& child) const Q_DECL_OVERRIDE {
    Q_UNUSED(child);

    //return QAbstractProxyModel::parent(child);

    /*int offset = child.internalId();
    if (offset == 0 || !child.isValid())
        return QModelIndex();
    return createIndex(offset - 1, 0);*/
    /*QModelIndex sourceIndex = sourceModel()->index(child.internalId(), 0);
    QModelIndex mapSourceIndex = mapFromSource(sourceIndex);
    int sr = mapToSource(mapSourceIndex.parent()).row();
    return createIndex(bumpedItems + treeIndexParent.row(), treeIndexParent.column(), sr);*/
    return QModelIndex(); // flat model, no hierarchy
  }

  /*!
      Returns \c{true} if \a parent has any children; otherwise returns \c{false}.
      Use rowCount() on the parent to find out the number of children.
      Note that it is undefined behavior to report that a particular index hasChildren
      with this method if the same index has the flag Qt::ItemNeverHasChildren set.
      \sa parent(), index()
  */
  bool hasChildren(const QModelIndex &parent) const Q_DECL_OVERRIDE {
    Q_UNUSED(parent);

    return false; // flat model, no hierarchy
  }

  ///\brief returns index of source model
  QModelIndex mapToSource(const QModelIndex &proxyIndex)  const Q_DECL_OVERRIDE {
    QModelIndex res;
    if (!proxyIndex.isValid()) {
      return res;
    }

    Q_ASSERT(proxyIndex.column() >=0 && proxyIndex.column() <= (static_cast<int>(ItemModel::Columns::Total) - 1));

    /*switch(proxyIndex.column())
    {
      case static_cast<int>(ItemListModel::Columns::Item):
        //res = sourceModel()->index(proxyIndex.row(), proxyIndex.column());
        res = createIndex(proxyIndex.row(), proxyIndex.column(), static_cast<quintptr>(-1));
        break;
      default:
        res = createIndex(proxyIndex.row(), proxyIndex.column(), static_cast<quintptr>(-1));
        break;
    }*/

    if(proxyIndex.column() >= sourceModel()->columnCount() && proxyIndex.column() <= (static_cast<int>(ItemModel::Columns::Total) - 1))
    {
       res = createIndex(proxyIndex.row(), proxyIndex.column(), static_cast<quintptr>(-1));
    } else {
       res = sourceModel()->index(proxyIndex.row(), proxyIndex.column());
    }

    return res;
  }

  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE {
    QModelIndex res;
    //res = index(sourceIndex.row(), static_cast<int>(ItemListModel::Columns::Item));
    res = index(sourceIndex.row(), sourceIndex.column());
    return res;
  }

  Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE
  {
    if (!index.isValid()) {
      return 0;
    }

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
    if (role != Qt::DisplayRole) {
      return QVariant();
    }

    if (orientation == Qt::Horizontal) {
      return QString("Column: %1").arg(section);
    } else {
      return QString("Row: %1").arg(section);
    }
  }
};

Q_DECLARE_METATYPE(ItemTableProxyModel*)
Q_DECLARE_METATYPE(std::shared_ptr<ItemTableProxyModel>)

/// \see doc-snapshots.qt.io/qt5-5.9/qtwebengine-webenginewidgets-demobrowser-history-cpp.html
/// \see doc.qt.io/qt-5/qtwidgets-itemviews-customsortfiltermodel-example.html
class PagedItemTableProxyFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    enum class SortRole {
      Start = Qt::UserRole + 1
      , SourceRow
      , Column
      , Total
    };

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
        sourceReset();
        invalidateFilter();
    }

    int filterMaxSourceRowIndex() const { return maxSourceRowIndex; }

    void setFilterMaxSourceRowIndex(const int rowNum){
        maxSourceRowIndex = rowNum;
        sourceReset();
        invalidateFilter();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
      Q_UNUSED(parent);
      Q_ASSERT(checkIndex(parent, QAbstractItemModel::CheckIndexOption::ParentIsInvalid)); // flat model

      auto filteredRows = QSortFilterProxyModel::rowCount();
      //qDebug() << "filteredRows1" << filteredRows;

      //qDebug() << "filteredRows2" << filteredRows;

      return filteredRows;
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
      Q_UNUSED(parent);
      return sourceModel()->columnCount();///static_cast<int>(ItemTableProxyModel::Columns::Total);
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const Q_DECL_OVERRIDE {
        if (sourceRow < 0 || sourceRow >= sourceModel()->rowCount()) {
          return false;
        }

        const QModelIndex& indexItemMode = sourceModel()->index(sourceRow, static_cast<int>(ItemModel::Columns::ItemMode), sourceParent);
        const QVariant& itemModeData = sourceModel()->data(indexItemMode);
        if (!itemModeData.canConvert<int>()) {
          return false;
        }
        const ItemModel::ItemMode& itemMode = static_cast<ItemModel::ItemMode>(itemModeData.toInt());
        //qDebug() << "ItemMode" << sourceModel()->data(indexItemMode);
        const bool isNotLoaded = itemMode == ItemModel::ItemMode::NotLoaded;

        const bool isInRange = rowInRange(sourceRow);

        if(!isNotLoaded && isInRange) {
          return true;
        }

        return false;
    }

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const Q_DECL_OVERRIDE {
      return left.row() < right.row(); // sort from first added to last added

      QVariant leftData = sourceModel()->data(left);
      QVariant rightData = sourceModel()->data(right);

      if (sortRole() == static_cast<int>(SortRole::SourceRow) || !leftData.isValid() || !rightData.isValid()) {
        return left.row() < right.row(); // sort from first added to last added
      }

      if (sortRole() == static_cast<int>(SortRole::Column)) {
        if (leftData.type() == QVariant::DateTime) {
          return leftData.toDateTime() < rightData.toDateTime();
        } else if (leftData.type() == QVariant::String) {
          return leftData.toString() < rightData.toString();
        } else if (leftData.type() == QVariant::Int) {
          return leftData.toInt() < rightData.toInt();
        }
      }

      return left.row() < right.row(); // sort from first added to last added
    }

    // Returns the parent of the model index, or QModelIndex() if it has no parent.
    QModelIndex parent(const QModelIndex& child) const Q_DECL_OVERRIDE {
      Q_UNUSED(child);
      return QModelIndex(); // flat model, no hierarchy
    }

    /*!
        Returns \c{true} if \a parent has any children; otherwise returns \c{false}.
        Use rowCount() on the parent to find out the number of children.
        Note that it is undefined behavior to report that a particular index hasChildren
        with this method if the same index has the flag Qt::ItemNeverHasChildren set.
        \sa parent(), index()
    */
    bool hasChildren(const QModelIndex &parent) const Q_DECL_OVERRIDE {
      Q_UNUSED(parent);

      return false; // flat model, no hierarchy
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
};

Q_DECLARE_METATYPE(PagedItemTableProxyFilterModel*)
Q_DECLARE_METATYPE(std::shared_ptr<PagedItemTableProxyFilterModel>)

/// \see doc-snapshots.qt.io/qt5-5.9/qtwebengine-webenginewidgets-demobrowser-history-cpp.html
/// \see doc.qt.io/qt-5/qtwidgets-itemviews-customsortfiltermodel-example.html
class ItemTableProxyFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    enum class SortRole {
      Start = Qt::UserRole + 1
      , SourceRow
      , Column
      , Total
    };

public slots:
  void slotSourceModelChanged(void);
  void slotDataChanged(const QModelIndex& first, const QModelIndex& last);
  void slotRowsInserted(const QModelIndex& parent, int first, int last);
  void slotRowsRemoved(const QModelIndex &parent, int start, int end);
  void sourceReset();

public:
    explicit ItemTableProxyFilterModel(QObject *parent = nullptr): QSortFilterProxyModel(parent)
    {
      connect(this, SIGNAL(sourceModelChanged()), this, SLOT(slotSourceModelChanged()));
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
      Q_UNUSED(parent);
      Q_ASSERT(checkIndex(parent, QAbstractItemModel::CheckIndexOption::ParentIsInvalid)); // flat model

      return  QSortFilterProxyModel::rowCount();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
      Q_UNUSED(parent);
      return static_cast<int>(ItemTableProxyModel::Columns::Total);
    }

    void setSkipNotLoaded(bool skip) {
      m_skipNotLoaded = skip;
      sourceReset();
      invalidateFilter();
    }

    bool isSkippingNotLoaded() const {
      return m_skipNotLoaded;
    }

protected:

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const Q_DECL_OVERRIDE {
        const QRegExp& regexp = filterRegExp();

        const QModelIndex& indexItemMode = sourceModel()->index(sourceRow, static_cast<int>(ItemModel::Columns::ItemMode), sourceParent);
        if(!indexItemMode.isValid()) {
          return false;
        }

        const ItemModel::ItemMode& itemMode = static_cast<ItemModel::ItemMode>(sourceModel()->data(indexItemMode).toInt());
        //qDebug() << "ItemMode" << sourceModel()->data(indexItemMode);
        const bool isNotLoaded = itemMode == ItemModel::ItemMode::NotLoaded;

        const QString& filterPattern = regexp.pattern();

        /*if(!isNotLoaded && filterPattern.isEmpty()) {
          return true;
        }*/

        bool anyFieldMatch = true;
        if(!filterPattern.isEmpty()) {
          const QModelIndex &indexfilterColumn = sourceModel()->index(sourceRow, this->filterKeyColumn(), sourceParent);
          if(!indexfilterColumn.isValid()) {
            return false;
          }

          const QVariant& filterData = sourceModel()->data(indexfilterColumn);
          if (!filterData.canConvert<QString>()) {
            return false;
          }
          const QString& filterColumnData = filterData.toString();
          //const bool isNameFiltered = name.contains(filterPattern, filterCaseSensitivity());

          // indexIn attempts to find a match in str from position offset (0 by default).
          // Returns the position of the first match, or -1 if there was no match.
          const bool isNameFiltered = regexp.indexIn(filterColumnData) != -1;
          anyFieldMatch = (isNameFiltered
                //|| isSurnameFiltered
                );
        }

        /*const QModelIndex& indexSurname = sourceModel()->index(sourceRow, static_cast<int>(ItemModel::Columns::Surname), sourceParent);
        const bool isSurnameFiltered = sourceModel()->data(indexSurname).toString().contains(regexp);

        const QModelIndex& indexGuid = sourceModel()->index(sourceRow, static_cast<int>(ItemModel::Columns::GUID), sourceParent);
        const bool isGuidFiltered = sourceModel()->data(indexGuid).toString().contains(regexp);
*/
        //bool isGuidValid = sourceModel()->data(indexGuid).toInt() != ItemListModel::dummyItemId;

        //return !isNotLoaded && anyFieldMatch;

        bool needSkip = isNotLoaded || anyFieldMatch;

        if (isSkippingNotLoaded()) {
          needSkip = !isNotLoaded && anyFieldMatch;
        }

        return needSkip;
    }

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const Q_DECL_OVERRIDE {
      return left.row() < right.row(); // sort from first added to last added

      QVariant leftData = sourceModel()->data(left);
      QVariant rightData = sourceModel()->data(right);

      if (sortRole() == static_cast<int>(SortRole::SourceRow) || !leftData.isValid() || !rightData.isValid()) {
        return left.row() < right.row(); // sort from first added to last added
      }

      if (sortRole() == static_cast<int>(SortRole::Column)) {
        if (leftData.type() == QVariant::DateTime) {
          return leftData.toDateTime() < rightData.toDateTime();
        } else if (leftData.type() == QVariant::String) {
          return leftData.toString() < rightData.toString();
        } else if (leftData.type() == QVariant::Int) {
          return leftData.toInt() < rightData.toInt();
        }
      }

      return left.row() < right.row(); // sort from first added to last added
    }

    // Returns the parent of the model index, or QModelIndex() if it has no parent.
    QModelIndex parent(const QModelIndex& child) const Q_DECL_OVERRIDE {
      Q_UNUSED(child);
      return QModelIndex(); // flat model, no hierarchy
    }

    /*!
        Returns \c{true} if \a parent has any children; otherwise returns \c{false}.
        Use rowCount() on the parent to find out the number of children.
        Note that it is undefined behavior to report that a particular index hasChildren
        with this method if the same index has the flag Qt::ItemNeverHasChildren set.
        \sa parent(), index()
    */
    bool hasChildren(const QModelIndex &parent) const Q_DECL_OVERRIDE {
      Q_UNUSED(parent);

      return false; // flat model, no hierarchy
    }

private:
  bool m_skipNotLoaded = false;
};

Q_DECLARE_METATYPE(ItemTableProxyFilterModel*)
Q_DECLARE_METATYPE(std::shared_ptr<ItemTableProxyFilterModel>)

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

/// \see doc-snapshots.qt.io/qt5-5.9/qtwebengine-webenginewidgets-demobrowser-history-cpp.html
/// \see doc.qt.io/qt-5/qtwidgets-itemviews-customsortfiltermodel-example.html
class PagedItemListProxyFilterModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
  enum class Columns {
    Page = 0
    //DebugIdentifier = 0
    //, Page
    , Total
  };

  /*enum class WorkMode {
    Online = 0
    , Offline
    , Total
  };*/

  explicit PagedItemListProxyFilterModel(QObject *pParent = nullptr) : QAbstractProxyModel(pParent) {
    connect(this, SIGNAL(sourceModelChanged()), this, SLOT(slotSourceModelChanged()));
  }

  virtual ~PagedItemListProxyFilterModel() {
  }

  void setPageSize(int pageSize) {
    m_pageSize = pageSize;

      /*beginInsertRows(QModelIndex(), 0, 0);
      endInsertRows();*/
  }

  int getPageSize() const {
    return m_pageSize;
  }

  /*void setWorkMode(WorkMode mode) {
    m_workMode = mode;
  }

  WorkMode getWorkMode() const {
    return m_workMode;
  }*/

  void setPagesTotal(int val) {
    m_onlinePagesTotal = val;

      /*beginInsertRows(QModelIndex(), 0, 0);
      endInsertRows();*/
  }

  int getPagesTotal() const {
    return m_onlinePagesTotal;
  }

  ItemListModel* getExtraDataSource() const {
    return m_extraDataSource;
  }

  void setExtraDataSource(ItemListModel* datasource) {
    Q_ASSERT(datasource);
    m_extraDataSource = datasource;

    slotSourceModelChanged();
  }

public slots:
  void slotSourceModelChanged(void);
  void slotDataChanged(const QModelIndex& first, const QModelIndex& last);
  void slotRowsInserted(const QModelIndex& parent, int first, int last);
  void slotRowsRemoved(const QModelIndex &parent, int start, int end);
  void sourceReset();

protected:

  int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    Q_UNUSED(parent);
    Q_ASSERT(checkIndex(parent, QAbstractItemModel::CheckIndexOption::ParentIsInvalid)); // flat model

    /// \note always set total pages
    if (getPagesTotal() >= 0) { // if value is set and valid
      return getPagesTotal();
    }

    //return QAbstractProxyModel::rowCount(parent);

    /// \note always set extra data source
    ItemListModel* itemListModel = getExtraDataSource();
    if (!itemListModel) {
      Q_ASSERT(false); // force DEBUG crash here
      return 1;
    }

    /// \note possible unreachable code
    //Q_ASSERT(false); // force DEBUG crash here

    //return itemListModel->rowCount();

    /// \note need to show something to user, so we will add empty page if no pages recieved
    //return std::max(getPagesTotal(), 1);

    /*if (m_workMode == WorkMode::Online && getOnlinePagesTotal() > 0) {
      return getOnlinePagesTotal();
    }*/

    int pagesTotal = 0;
    if (getPageSize() != 0) {
      std::div_t res = std::div(sourceModel()->rowCount(), getPageSize());
      // Fast ceiling of an integer division
      pagesTotal = res.rem ? (res.quot + 1) : res.quot;
    }

    /// \note possible unreachable code
    return pagesTotal;
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {
    Q_UNUSED(parent);

    return static_cast<int>(Columns::Total);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE
  {
    Q_UNUSED(value);
    Q_UNUSED(role);

    if (!index.isValid()) {
      return false;
    }

    return false; // read only data
  }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE {
    if (!index.isValid()) {
      return QVariant();
    }



    if(role != Qt::DisplayRole && role != Qt::EditRole){
      return QVariant(); // disable tooltips, icons, e.t.c
    }

    QVariant result;

    switch( role )
    {
    case Qt::SizeHintRole:
      return QVariant(QSize( 25, 25 ));
    case Qt::ToolTipRole:
    //case Qt::DisplayRole:
    case Qt::DecorationRole:
    case Qt::InitialSortOrderRole:
    case Qt::CheckStateRole:
      return QVariant();
    default:
      break;
    }

    if(role != Qt::DisplayRole && role != Qt::EditRole){
      return QVariant(); // disable tooltips, icons, e.t.c
    }

    PagedItemTableProxyFilterModel* pagedItemTableProxyFilterModel = static_cast<PagedItemTableProxyFilterModel*>(sourceModel());

    ItemListModel* itemListModel = getExtraDataSource();
    if (!itemListModel) {
      return result;
    }

    QList<std::shared_ptr<ItemMapper>> items;

    int pageStartCursor = 0;//index.row() * getPageSize();

    for (int i = pageStartCursor; i < pageStartCursor + getPageSize(); i++) {
      //QModelIndex idx = pagedItemTableProxyFilterModel->index(i, static_cast<int>(ItemModel::Columns::GUID));
      QModelIndex idx = pagedItemTableProxyFilterModel->index(i, static_cast<int>(ItemTableProxyModel::Columns::SourceMappedRowNum));
      if(!idx.isValid()) {
        continue;
      }

      QVariant data = pagedItemTableProxyFilterModel->data(idx, Qt::DisplayRole);
      if (!data.isValid()) {
        continue;
      }
      if (!data.canConvert<int>()) {
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

  // Returns the parent of the model index, or QModelIndex() if it has no parent.
  QModelIndex parent(const QModelIndex& child) const Q_DECL_OVERRIDE {
    Q_UNUSED(child);

    return QModelIndex(); // flat model, no hierarchy
  }

  /*!
      Returns \c{true} if \a parent has any children; otherwise returns \c{false}.
      Use rowCount() on the parent to find out the number of children.
      Note that it is undefined behavior to report that a particular index hasChildren
      with this method if the same index has the flag Qt::ItemNeverHasChildren set.
      \sa parent(), index()
  */
  bool hasChildren(const QModelIndex &parent) const Q_DECL_OVERRIDE {
    Q_UNUSED(parent);

    return false; // flat model, no hierarchy
  }

  ///\brief returns index of source model
  QModelIndex mapToSource(const QModelIndex &proxyIndex)  const Q_DECL_OVERRIDE {
    QModelIndex res;

    if (!proxyIndex.isValid()) {
      return res;
    }

    Q_ASSERT(proxyIndex.column() >=0 && proxyIndex.column() <= sourceModel()->columnCount());

    res = sourceModel()->index(proxyIndex.row(), proxyIndex.column());

    /*Q_ASSERT(proxyIndex.column() >=0 && proxyIndex.column() <= (static_cast<int>(ItemModel::Columns::Total) - 1));

    if(proxyIndex.column() >= sourceModel()->columnCount() && proxyIndex.column() <= (static_cast<int>(ItemTableProxyModel::Columns::Total) - 1))
    {
       res = createIndex(proxyIndex.row(), proxyIndex.column(), static_cast<quintptr>(-1));
    } else {
       res = sourceModel()->index(proxyIndex.row(), proxyIndex.column());
    }*/

    return res;
  }

  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE {
    QModelIndex res;
    res = index(sourceIndex.row(), sourceIndex.column());
    return res;
  }

  /*bool canFetchMore(const QModelIndex & index) const Q_DECL_OVERRIDE
  {
    Q_UNUSED(index);
    return false;
  }*/

  Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE
  {
    if (!index.isValid()) {
      return 0;
    }

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
    if (role != Qt::DisplayRole) {
      return QVariant();
    }

    if (orientation == Qt::Horizontal) {
      return QString("Column: %1").arg(section);
    } else {
      return QString("Row: %1").arg(section);
    }
  }

private:
  int m_pageSize = 1;
  int m_onlinePagesTotal = -1;

  ItemListModel* m_extraDataSource;
};

Q_DECLARE_METATYPE(PagedItemListProxyFilterModel*)
Q_DECLARE_METATYPE(std::shared_ptr<PagedItemListProxyFilterModel>)

#endif // PAGEDITEMMODEL_H
