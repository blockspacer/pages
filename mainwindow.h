#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "pageditemmodel.h"
#include "paged_item_widget.h"

#include <memory>

namespace Ui {
class MainWindow;
}

struct fetchedPageData {
  QList<Item> items;
  int totalPages;
  int requestedPageSize;
  int totalItems;
  int requestedPageNum;
  int recievedItemsCount;
};

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();


  void onDataFetched(int requestedPageNum, std::shared_ptr<fetchedPageData> data);
  void onRowRangeChanged(int first, int last);
public slots:
  //void onCheckboxChanged(const int state);

  //void onPersonsPageChanged(const QVariant& val);

  void onMapperIndexChanged(int row);

private:
  ItemTableProxyFilterModel* m_filterItemTableProxyModel;
  PagedItemTableProxyFilterModel* m_pagedItemTableProxyModel;
  PagedItemListProxyFilterModel* m_pagedItemListProxyFilterModel;
  QTimer* m_timer;
  ItemTableProxyModel* m_itemTableProxyModel;
  Ui::MainWindow *m_ui;
  std::shared_ptr<PagedItemMapper> m_pagedItemMapper;
  std::shared_ptr<ItemListModel> m_itemListModelCache;
  PagedItemWidget* m_pagedItemWidget;

  /// \note When you submit changes to the model, it gets reset
  /// and all views (including all data mappers) along with it
  /// So, we need to save last mapper index somewhere
  int lastMapperPageNum = 0;

  //ItemPageListModel* m_pagedItemModel;
};

#endif // MAINWINDOW_H
