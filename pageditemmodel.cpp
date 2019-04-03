#include "pageditemmodel.h"

/*PagedItemModel::PagedItemModel()
{

}
*/

void ItemTableProxyModel::slotSourceModelChanged()
{
  disconnect(this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));
  connect(sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));
}

void ItemTableProxyModel::slotDataChanged(const QModelIndex first, QModelIndex last)
{
  emit dataChanged(mapFromSource(first), mapFromSource(last));
}
