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
  sourceReset();
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

  sourceReset();
}

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
  sourceReset();
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
  qDebug() << "slotRowsInserted";
  /// \see HistoryTreeModel qt example
  /// doc-snapshots.qt.io/qt5-5.9/qtwebengine-webenginewidgets-demobrowser-history-cpp.html
  Q_UNUSED(parent); // Avoid warnings when compiling release
  Q_ASSERT(!parent.isValid());

  sourceReset();
}

void ItemTableProxyFilterModel::slotSourceModelChanged()
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

void ItemTableProxyFilterModel::sourceReset()
{
  beginResetModel();
  endResetModel();
}

void ItemTableProxyFilterModel::slotDataChanged(const QModelIndex& first, const QModelIndex& last)
{
  sourceReset();
}

void ItemTableProxyFilterModel::slotRowsRemoved(const QModelIndex& parent, int start, int end)
{
  /// \see HistoryTreeModel qt example
  /// doc-snapshots.qt.io/qt5-5.9/qtwebengine-webenginewidgets-demobrowser-history-cpp.html
  Q_UNUSED(parent); // Avoid warnings when compiling release
  Q_ASSERT(!parent.isValid());
  Q_UNUSED(start);
  Q_UNUSED(end);

  sourceReset();
}

void ItemTableProxyFilterModel::slotRowsInserted(const QModelIndex& parent, int start, int end)
{
  /// \see HistoryTreeModel qt example
  /// doc-snapshots.qt.io/qt5-5.9/qtwebengine-webenginewidgets-demobrowser-history-cpp.html
  Q_UNUSED(parent); // Avoid warnings when compiling release
  Q_ASSERT(!parent.isValid());

  sourceReset();
}

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
  sourceReset();
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

  sourceReset();
}
