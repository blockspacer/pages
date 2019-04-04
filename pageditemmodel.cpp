#include "pageditemmodel.h"

void ItemTableProxyModel::slotSourceModelChanged()
{
  disconnect(this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));
  disconnect(this, SLOT(slotRowsInserted(QModelIndex,int,int)));
  disconnect(this, SLOT(slotRowsRemoved(QModelIndex,int,int)));
  disconnect(this, SLOT(sourceReset()));

  connect(sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));
  connect(sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(slotRowsInserted(QModelIndex,int,int)));
  connect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(slotRowsRemoved(QModelIndex,int,int)));
  connect(sourceModel(), SIGNAL(modelReset()), this, SLOT(sourceReset()));
}

void ItemTableProxyModel::sourceReset()
{
  beginResetModel();
  endResetModel();
}

void ItemTableProxyModel::slotDataChanged(const QModelIndex& first, const QModelIndex& last)
{
  emit dataChanged(mapFromSource(first), mapFromSource(last));
}

void ItemTableProxyModel::slotRowsRemoved(const QModelIndex& parent, int start, int end)
{
  /// \see HistoryTreeModel qt example
  /// doc-snapshots.qt.io/qt5-5.9/qtwebengine-webenginewidgets-demobrowser-history-cpp.html
  Q_UNUSED(parent); // Avoid warnings when compiling release
  Q_ASSERT(!parent.isValid());
  Q_UNUSED(start);
  Q_UNUSED(end);

  sourceReset();
}

void ItemTableProxyModel::slotRowsInserted(const QModelIndex& parent, int start, int end)
{
  /// \see HistoryTreeModel qt example
  /// doc-snapshots.qt.io/qt5-5.9/qtwebengine-webenginewidgets-demobrowser-history-cpp.html
  Q_UNUSED(parent); // Avoid warnings when compiling release
  Q_ASSERT(!parent.isValid());

  if (start != 0 || start != end) {
      beginResetModel();
      //m_sourceRowCache.clear();
      endResetModel();
      return;
  }

  QModelIndex treeIndex = mapFromSource(sourceModel()->index(start, 0));
  QModelIndex treeParent = treeIndex.parent();
  if (rowCount(treeParent) == 1) {
      beginInsertRows(QModelIndex(), 0, 0);
      endInsertRows();
  } else {
      beginInsertRows(treeParent, treeIndex.row(), treeIndex.row());
      endInsertRows();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void PagedItemTableProxyFilterModel::slotSourceModelChanged()
{
  disconnect(this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));
  disconnect(this, SLOT(slotRowsInserted(QModelIndex,int,int)));
  disconnect(this, SLOT(slotRowsRemoved(QModelIndex,int,int)));
  disconnect(this, SLOT(sourceReset()));

  connect(sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));
  connect(sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(slotRowsInserted(QModelIndex,int,int)));
  connect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(slotRowsRemoved(QModelIndex,int,int)));
  connect(sourceModel(), SIGNAL(modelReset()), this, SLOT(sourceReset()));
}

void PagedItemTableProxyFilterModel::sourceReset()
{
  beginResetModel();
  endResetModel();
}

void PagedItemTableProxyFilterModel::slotDataChanged(const QModelIndex& first, const QModelIndex& last)
{
  emit dataChanged(mapFromSource(first), mapFromSource(last));
}

void PagedItemTableProxyFilterModel::slotRowsRemoved(const QModelIndex& parent, int start, int end)
{
  /// \see HistoryTreeModel qt example
  /// doc-snapshots.qt.io/qt5-5.9/qtwebengine-webenginewidgets-demobrowser-history-cpp.html
  Q_UNUSED(parent); // Avoid warnings when compiling release
  Q_ASSERT(!parent.isValid());
  Q_UNUSED(start);
  Q_UNUSED(end);

  sourceReset();
}

void PagedItemTableProxyFilterModel::slotRowsInserted(const QModelIndex& parent, int start, int end)
{
  /// \see HistoryTreeModel qt example
  /// doc-snapshots.qt.io/qt5-5.9/qtwebengine-webenginewidgets-demobrowser-history-cpp.html
  Q_UNUSED(parent); // Avoid warnings when compiling release
  Q_ASSERT(!parent.isValid());

  if (start != 0 || start != end) {
      beginResetModel();
      //m_sourceRowCache.clear();
      endResetModel();
      return;
  }

  QModelIndex treeIndex = mapFromSource(sourceModel()->index(start, 0));
  QModelIndex treeParent = treeIndex.parent();
  if (rowCount(treeParent) == 1) {
      beginInsertRows(QModelIndex(), 0, 0);
      endInsertRows();
  } else {
      beginInsertRows(treeParent, treeIndex.row(), treeIndex.row());
      endInsertRows();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void PagedItemListProxyFilterModel::slotSourceModelChanged()
{
  disconnect(this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));
  disconnect(this, SLOT(slotRowsInserted(QModelIndex,int,int)));
  disconnect(this, SLOT(slotRowsRemoved(QModelIndex,int,int)));
  disconnect(this, SLOT(sourceReset()));

  connect(sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));
  connect(sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(slotRowsInserted(QModelIndex,int,int)));
  connect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(slotRowsRemoved(QModelIndex,int,int)));
  connect(sourceModel(), SIGNAL(modelReset()), this, SLOT(sourceReset()));
}

void PagedItemListProxyFilterModel::sourceReset()
{
  beginResetModel();
  endResetModel();
}

void PagedItemListProxyFilterModel::slotDataChanged(const QModelIndex& first, const QModelIndex& last)
{
  emit dataChanged(mapFromSource(first), mapFromSource(last));
}

void PagedItemListProxyFilterModel::slotRowsRemoved(const QModelIndex& parent, int start, int end)
{
  /// \see HistoryTreeModel qt example
  /// doc-snapshots.qt.io/qt5-5.9/qtwebengine-webenginewidgets-demobrowser-history-cpp.html
  Q_UNUSED(parent); // Avoid warnings when compiling release
  Q_ASSERT(!parent.isValid());
  Q_UNUSED(start);
  Q_UNUSED(end);

  sourceReset();
}

void PagedItemListProxyFilterModel::slotRowsInserted(const QModelIndex& parent, int start, int end)
{
  /// \see HistoryTreeModel qt example
  /// doc-snapshots.qt.io/qt5-5.9/qtwebengine-webenginewidgets-demobrowser-history-cpp.html
  Q_UNUSED(parent); // Avoid warnings when compiling release
  Q_ASSERT(!parent.isValid());

  if (start != 0 || start != end) {
      beginResetModel();
      //m_sourceRowCache.clear();
      endResetModel();
      return;
  }

  QModelIndex treeIndex = mapFromSource(sourceModel()->index(start, 0));
  QModelIndex treeParent = treeIndex.parent();
  if (rowCount(treeParent) == 1) {
      beginInsertRows(QModelIndex(), 0, 0);
      endInsertRows();
  } else {
      beginInsertRows(treeParent, treeIndex.row(), treeIndex.row());
      endInsertRows();
  }
}
