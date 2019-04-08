#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "pageditemmodel.h"
#include "paged_item_widget.h"

#include <memory>

namespace Ui {
class MainWindow;
}

/// \brief stores data got from remote server
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

  void onMapperIndexChanged(int row);

private:

  /// \brief global source of data used by filters, models, pagination
  /// \note we store pointers to items
  std::shared_ptr<ItemListModel> m_itemListModelCache;

  /// \brief converts flat item list to multi-column table model
  ItemTableProxyModel* m_itemTableProxyModel;

  /// \brief provides filtering (search)
  ItemTableProxyFilterModel* m_filterItemTableProxyModel;

  /// \brief provides pagination in model
  /// \note pagination built via passed start/end range
  PagedItemTableProxyFilterModel* m_pagedItemTableProxyModel;

  /// \brief converts multi-column table model to flat item list
  /// \note item list will be limited to items on curent page
  /// \note gets items from passed extraDataSource
  PagedItemListProxyFilterModel* m_pagedItemListProxyFilterModel;

  /// \brief provides pagination used in mapped widgets
  /// \note passes item list to widgets on page switch
  std::shared_ptr<PagedItemMapper> m_pagedItemMapper;

  /// \note recieves item list from mapper on every page switch
  PagedItemWidget* m_pagedItemWidget;

  QTimer* m_timer;

  Ui::MainWindow *m_ui;

  /// \note When you submit changes to the model, it gets reset
  /// and all views (including all data mappers) along with it
  /// So, we need to save last mapper index somewhere
  int m_lastMapperPageNum = 0;
};

#endif // MAINWINDOW_H
