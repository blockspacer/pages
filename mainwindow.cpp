#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "item_widget.h"
#include "paged_item_widget.h"

#if defined(QT_TESTLIB_LIB) && QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
#include <QAbstractItemModelTester>
#endif

/// \note place no more than kItemsPerPage items into each ItemListModel
static int kItemsPerPage = 2;

static bool isDisconnected = false;

static bool enableAbstractItemModelTester = true;

//static ItemListModel::Roles gFilterRole = ItemListModel::Roles::Name;

static std::shared_ptr<fetchedPageData> m_lastFetchedData;

struct FilterSettings {
  QRegExp filterRegExp;
  Qt::CaseSensitivity filterCaseSensitivity;
};

static QVector<Item> dummyRemoteItems {
  Item{0, "0Atas", "Bork"},
  Item{1, "1Viktor", "Irman"},
  Item{2, "2Nadya", "Makeyn"},
  Item{3, "3Alli", "Shtadt"},
  Item{4, "4Gabriel", "Abrim"},
  Item{5, "5Gabram", "Bdhim"},
  Item{6, "6Ludovik", "Manstein"},
  Item{7, "7Klark", "Kent"},
  Item{8, "8Kattie", "Klark"},
  Item{9, "9Lisa", "Ali"},
  Item{10, "10Siren", "Ann"},
  Item{11, "11Kennie", "Demer"},
  Item{12, "12Clementine", "Dallas"},
};

static ItemModel* createItemModel(int guid, const QString& name, const QString& surname, QObject* parent = nullptr) {
  ItemModel* itemModel = new ItemModel();
  if(parent) {
    itemModel->setParent(parent);
  }
  itemModel->setName(name);
  itemModel->setSurname(surname);
  itemModel->setGUID(guid);
  /*QObject::connect(itemModel, &ItemModel::dataChanged, [](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles){
    qDebug() << "ItemModel::dataChanged";
  });*/
  return itemModel;
}

/*static std::shared_ptr<ItemModel> createSharedItemModel(int guid, const QString& name, const QString& surname, QObject* parent = nullptr) {
  std::shared_ptr<ItemModel> itemModel = std::make_shared<ItemModel>();
  if(parent) {
    itemModel->setParent(parent);
  }
  itemModel->setName(name);
  itemModel->setSurname(surname);
  itemModel->setGUID(guid);
  return itemModel;
}*/

static std::shared_ptr<ItemMapper> createItemMapper(ItemModel* itemModel) {
  std::shared_ptr<ItemMapper> itemMapper = std::make_shared<ItemMapper>();
  /// \note allows two-way data editing
  itemMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
  itemMapper->setModel(itemModel);
  itemMapper->toFirst();
  return itemMapper;
}

/*static ItemWidget* createItemWidget(std::shared_ptr<ItemMapper> itemMapper) {
  ItemWidget* itemWidget = new ItemWidget();
  itemWidget->setMapper(itemMapper);
  itemWidget->setMappings();
  return itemWidget;
}*/

static QList<Item> retrieveRemoteFiltered(const FilterSettings& filter/*, const ItemListModel::Roles& filterRole*/) {
  QList<Item> result;
  for (int i = 0; i < dummyRemoteItems.size(); i++) {
    // TODO: regex support
    const auto& item = dummyRemoteItems.at(i);
    QString filterItem = "";

    /*if (filterRole == ItemListModel::Roles::Name) {
      filterItem = item.name;
      //qDebug() << "PersonsModel::Roles::NameRole";
    } else if (filterRole == ItemListModel::Roles::Surname) {
      filterItem = item.surname;
      //qDebug() << "PersonsModel::Roles::SurnameRole";
    } else {
      // TODO
    }*/

    filterItem = item.getName(); // TODO

    // indexIn attempts to find a match in str from position offset (0 by default).
    // Returns the position of the first match, or -1 if there was no match.
    const bool isFiltered = filter.filterRegExp.pattern().isEmpty() || filter.filterRegExp.indexIn(filterItem) != -1;

    // !filter.filterRegExp.pattern().isEmpty() && !filterItem.contains(filter.filterRegExp.pattern(), filter.filterCaseSensitivity)

    if (!isFiltered) {
      //qDebug() << "skipped " << filterItem;
      continue;
    } else {
      //qDebug() << "NOT skipped " << filterItem;
    }
    result.push_back(item);
  }
  return result;
}

static std::shared_ptr<fetchedPageData> retrieveRemoteItems(int pageNum, int itemsPerPage, const FilterSettings& filter/*, const ItemListModel::Roles& filterRole*/) {
  if (pageNum < 0) {
    return nullptr;
  }

  std::shared_ptr<fetchedPageData> result = std::make_shared<fetchedPageData>();

  QList<Item> dummyRemotePage;

  auto filtered = retrieveRemoteFiltered(filter/*, filterRole*/);

  int cursorI = pageNum * itemsPerPage;
  if (cursorI >= filtered.size()) {
    qDebug() << "not enough persons. Requested page " << pageNum;
  }

  const int pageItemsLastCursor = std::min(cursorI + itemsPerPage, filtered.size());
  int sentItems = 0;

  for (; cursorI < pageItemsLastCursor; cursorI++) {
    dummyRemotePage.push_back(filtered.at(cursorI));
    sentItems++;
  }

  int pageCount = 1;
  if (itemsPerPage != 0) {
    std::div_t res = std::div(filtered.size(), itemsPerPage);
    // Fast ceiling of an integer division
    pageCount = res.rem ? (res.quot + 1) : res.quot;
  }

  result->items = dummyRemotePage;
  result->requestedPageNum = pageNum;
  result->totalPages = pageCount;
  result->totalItems = filtered.size(); // TODO
  result->recievedItemsCount = sentItems;
  result->requestedPageSize = itemsPerPage;

  return result;
}

static std::shared_ptr<fetchedPageData> fetchRemoteItemsToModel(bool forceClearCache, std::shared_ptr<ItemListModel> model, int pageNum, int itemsPerPage, const FilterSettings& filter/*, const ItemListModel::Roles& filterRole*/)
{
  if (pageNum < 0) {
    return nullptr;
  }

  std::shared_ptr<fetchedPageData> result = retrieveRemoteItems(pageNum, itemsPerPage, filter/*, filterRole*/);

  if (forceClearCache || result->recievedItemsCount == 0) {
    model->removeAllItems();
  }

  if (pageNum >= result->totalPages) {
    qDebug() << "nothing to show!";
    return result;
  }

  const int pageStartCursor = result->requestedPageNum * result->requestedPageSize;
  qDebug() << "pageStartCursor" << pageStartCursor;
  qDebug() << "result->recievedItemsCount" << result->recievedItemsCount;

  // place dummy items if not enough items in cache
  const int remoteRowsTotal = pageStartCursor + result->recievedItemsCount;
  qDebug() << "remoteRowsTotal" << remoteRowsTotal;

  //model->setRowsTotalSpace(remoteRowsTotal);

  model->setRowsMinSpace(remoteRowsTotal);

  int itemPageCursor = pageStartCursor;

  for ( Item& item : result->items) {
    // replace item by rowNum or add new item
    //const std::shared_ptr<ItemMapper> itemMapper = model->getItemAt(itemPageCursor);

    ItemModel* itemModel = createItemModel(item.getGUID(), item.getName(), item.getSurname());
    if (itemModel) {
      std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
      itemModel->setParent(m_itemMapper.get());
      //model->pushBack(m_itemMapper);
      /// \note reserving space guarantees that item to replace exists
      model->replaceItemAt(itemPageCursor, m_itemMapper);
    }

    itemPageCursor++;
  }

  return result;
}

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);

  m_pagedItemMapper = std::make_shared<PagedItemMapper>();

  m_filterItemTableProxyModel = new ItemTableProxyFilterModel();

  // dynamicSortFilter ensures that the model is sorted and filtered whenever
  // the contents of the source model change.
  m_filterItemTableProxyModel->setDynamicSortFilter(true);
  m_filterItemTableProxyModel->setFilterKeyColumn(static_cast<int>(ItemModel::Columns::Name));

  m_pagedItemTableProxyModel = new PagedItemTableProxyFilterModel();

  // dynamicSortFilter ensures that the model is sorted and filtered whenever
  // the contents of the source model change.
  m_pagedItemTableProxyModel->setDynamicSortFilter(true);

  m_pagedItemListProxyFilterModel = new PagedItemListProxyFilterModel();

  //m_pagedItemListProxyFilterModel->setWorkMode(PagedItemListProxyFilterModel::WorkMode::Offline);

  m_pagedItemListProxyFilterModel->setPageSize(kItemsPerPage); // limit items on widget page

  m_itemTableProxyModel = new ItemTableProxyModel();
  m_itemListModelCache = std::make_shared<ItemListModel>();
  m_itemTableProxyModel->setSourceModel(m_itemListModelCache.get());

  m_ui->prevButton->setEnabled(false);
  m_ui->nextButton->setEnabled(false);
  m_ui->pageSizeSpinBox->setValue(kItemsPerPage);
  m_ui->pageNumSpinBox->setValue(0);
  m_ui->clearCacheOnPagingCheckBox->setChecked(false);

  //m_ui->refreshButton->setEnabled(false);
  m_ui->refreshButton->setEnabled(true);

  {
    ItemModel* itemModel = createItemModel(0, "_0Alie", "Bork");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    //ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  {
    ItemModel* itemModel = createItemModel(1, "_1Bob", "Byorn");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    //ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  {
    ItemModel* itemModel = createItemModel(2, "_2Anna", "Kerman");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    //ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  {
    ItemModel* itemModel = createItemModel(3, "_3Hugo", "Geber");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    //ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  {
    ItemModel* itemModel = createItemModel(4, "_4Borat", "Nagler");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    //ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  // must be same as dummy NotLoaded model
  {
    ItemModel* itemModel = createItemModel(std::numeric_limits<int>::min(), "", "");
    itemModel->setItemMode(ItemModel::ItemMode::NotLoaded);
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    //ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  // must be same as dummy NotLoaded model
  {
    ItemModel* itemModel = createItemModel(std::numeric_limits<int>::min(), "", "");
    itemModel->setItemMode(ItemModel::ItemMode::NotLoaded);
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    //ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  // must be same as dummy NotLoaded model
  {
    ItemModel* itemModel = createItemModel(std::numeric_limits<int>::min(), "", "");
    itemModel->setItemMode(ItemModel::ItemMode::NotLoaded);
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    //ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  // must be same as dummy NotLoaded model
  {
    ItemModel* itemModel = createItemModel(std::numeric_limits<int>::min(), "", "");
    itemModel->setItemMode(ItemModel::ItemMode::NotLoaded);
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    //ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  // must be same as dummy NotLoaded model
  {
    ItemModel* itemModel = createItemModel(std::numeric_limits<int>::min(), "", "");
    itemModel->setItemMode(ItemModel::ItemMode::NotLoaded);
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    //ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  // must be same as dummy NotLoaded model
  {
    ItemModel* itemModel = createItemModel(std::numeric_limits<int>::min(), "", "");
    itemModel->setItemMode(ItemModel::ItemMode::NotLoaded);
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    //ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  {
    ItemModel* itemModel = createItemModel(11, "_11John", "Black");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    //ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  {
    ItemModel* itemModel = createItemModel(12, "_12Sara", "Parker");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    //ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  m_pagedItemMapper->setModel(m_pagedItemListProxyFilterModel);
  m_pagedItemMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

  m_pagedItemWidget = new PagedItemWidget;
  m_ui->scrollVerticalLayout->addWidget(m_pagedItemWidget);

  m_pagedItemMapper->addMapping(m_pagedItemWidget, static_cast<int>(PagedItemListProxyFilterModel::Columns::Page), m_pagedItemWidget->personsPagePropertyName());

  connect(m_ui->prevButton, &QAbstractButton::clicked, [this]() {
    const int prevPageIndex = std::max(0, m_pagedItemMapper->currentIndex() - 1);

    FilterSettings filterSettings;
    filterSettings.filterRegExp = m_filterItemTableProxyModel->filterRegExp();
    filterSettings.filterCaseSensitivity = m_filterItemTableProxyModel->filterCaseSensitivity();

    if (!isDisconnected) {
      m_lastFetchedData = fetchRemoteItemsToModel(m_ui->clearCacheOnPagingCheckBox->isChecked(), m_itemListModelCache, prevPageIndex, kItemsPerPage, filterSettings);
      onDataFetched(prevPageIndex, m_lastFetchedData);
      onRowRangeChanged(m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize, m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize+m_lastFetchedData->requestedPageSize);
    } else {
      onDataFetched(prevPageIndex, m_lastFetchedData);
      onRowRangeChanged(prevPageIndex*kItemsPerPage, prevPageIndex*kItemsPerPage+kItemsPerPage);
      m_lastFetchedData = nullptr;
    }
    //onDataFetched(prevPageIndex, m_lastFetchedData);

    // allows dynamic loading while using pagination
    m_pagedItemMapper->setCurrentIndex(prevPageIndex);
    // m_pagedItemMapper->submit();

    /*m_ui->tableView->update();
    m_ui->tableView->show();

    m_ui->listView->update();
    m_ui->listView->show();*/
  });

  connect(m_ui->pageSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int state) {
    Q_UNUSED(this);
    kItemsPerPage = state;
  });

  connect(m_ui->goToPageButton, &QPushButton::clicked, [this]()
  {
    int pageNum = m_ui->pageNumSpinBox->value();
    pageNum = std::max(pageNum, 0);

    m_ui->searchEdit->setText(m_ui->searchEdit->text());
    m_filterItemTableProxyModel->setFilterFixedString(m_ui->searchEdit->text());

    FilterSettings filterSettings;
    filterSettings.filterRegExp = m_filterItemTableProxyModel->filterRegExp();
    filterSettings.filterCaseSensitivity = m_filterItemTableProxyModel->filterCaseSensitivity();

    if (!isDisconnected) {
      m_lastFetchedData = fetchRemoteItemsToModel(m_ui->clearCacheOnPagingCheckBox->isChecked(), m_itemListModelCache, pageNum, kItemsPerPage, filterSettings);
      onDataFetched(pageNum, m_lastFetchedData);
      onRowRangeChanged(m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize, m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize+m_lastFetchedData->requestedPageSize);
    } else {
      onDataFetched(pageNum, m_lastFetchedData);
      onRowRangeChanged(pageNum*kItemsPerPage, pageNum*kItemsPerPage+kItemsPerPage);
      m_lastFetchedData = nullptr;
    }
    //onDataFetched(pageNum, m_lastFetchedData);

    // // Invalidates the current sorting and filtering.
    m_filterItemTableProxyModel->invalidate();
    m_pagedItemTableProxyModel->invalidate();

    //m_filterItemTableProxyModel->submit();
    //m_pagedItemTableProxyModel->submit();

    // allows dynamic loading while using pagination
    m_pagedItemMapper->setCurrentIndex(pageNum);
    //m_pagedItemMapper->submit();
  });

  connect(m_ui->checkBox, &QCheckBox::stateChanged, [this](int state) {
    Q_UNUSED(this);

    isDisconnected = state > 0 ? true : false;
    qDebug() << "isDisconnected = " << isDisconnected;
    //m_ui->refreshButton->setEnabled(!isDisconnected);
  });

  connect(m_ui->skipNotLoadedCheckBox, &QCheckBox::stateChanged, [this](int state) {
    Q_UNUSED(this);

    m_filterItemTableProxyModel->setSkipNotLoaded(state > 0);

    // // Invalidates the current sorting and filtering.
    m_filterItemTableProxyModel->invalidate();
    m_pagedItemTableProxyModel->invalidate();

    qDebug() << "skipNotLoadedCheckBox = " << state;
    //m_ui->refreshButton->setEnabled(!isDisconnected);
  });

  connect(m_ui->searchButton, &QPushButton::clicked, [this]()
  {
    const int resetPageIndex = 0;

    m_filterItemTableProxyModel->setFilterFixedString(m_ui->searchEdit->text());

    FilterSettings filterSettings;
    filterSettings.filterRegExp = m_filterItemTableProxyModel->filterRegExp();
    filterSettings.filterCaseSensitivity = m_filterItemTableProxyModel->filterCaseSensitivity();

    if (!isDisconnected) {
      m_lastFetchedData = fetchRemoteItemsToModel(m_ui->clearCacheOnPagingCheckBox->isChecked(), m_itemListModelCache, resetPageIndex, kItemsPerPage, filterSettings);
      onDataFetched(resetPageIndex, m_lastFetchedData);
      onRowRangeChanged(m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize, m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize+m_lastFetchedData->requestedPageSize);

    } else {
      m_lastFetchedData = nullptr;
      onDataFetched(resetPageIndex, m_lastFetchedData);
      onRowRangeChanged(resetPageIndex*kItemsPerPage, resetPageIndex*kItemsPerPage+kItemsPerPage);
    }

    //m_itemListModelCache->submit();

    // // Invalidates the current sorting and filtering.
    m_filterItemTableProxyModel->invalidate();
    m_pagedItemTableProxyModel->invalidate();

    //m_filterItemTableProxyModel->submit();
    //m_pagedItemTableProxyModel->submit();

    // allows dynamic loading while using pagination
    m_pagedItemMapper->setCurrentIndex(resetPageIndex);
    //m_pagedItemMapper->submit();
  });

  connect(m_ui->refreshButton, &QPushButton::clicked, [this]()
  {
    /*if (isDisconnected) {
      return;
    }*/

    int refreshPageIndex = m_pagedItemMapper->currentIndex();
    refreshPageIndex = std::max(refreshPageIndex, 0);

    m_ui->searchEdit->setText(m_ui->searchEdit->text());
    m_filterItemTableProxyModel->setFilterFixedString(m_ui->searchEdit->text());

    FilterSettings filterSettings;
    filterSettings.filterRegExp = m_filterItemTableProxyModel->filterRegExp();
    filterSettings.filterCaseSensitivity = m_filterItemTableProxyModel->filterCaseSensitivity();

    m_lastFetchedData = fetchRemoteItemsToModel(m_ui->clearCacheOnPagingCheckBox->isChecked(), m_itemListModelCache, refreshPageIndex, kItemsPerPage, filterSettings);
    onDataFetched(refreshPageIndex, m_lastFetchedData);
    onRowRangeChanged(m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize, m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize+m_lastFetchedData->requestedPageSize);

    // Invalidates the current sorting and filtering.
    m_filterItemTableProxyModel->invalidate();
    m_pagedItemTableProxyModel->invalidate();

    //m_filterItemTableProxyModel->submit();
    //m_pagedItemTableProxyModel->submit();

     // allows dynamic loading while using pagination
     m_pagedItemMapper->setCurrentIndex(refreshPageIndex);
     //m_pagedItemMapper->submit();
  });

  connect(m_ui->resetButton, &QPushButton::clicked, [this]()
  {
    const int resetPageIndex = 0;

    m_ui->searchEdit->setText("");
    m_filterItemTableProxyModel->setFilterFixedString("");

    FilterSettings filterSettings;
    filterSettings.filterRegExp = m_filterItemTableProxyModel->filterRegExp();
    filterSettings.filterCaseSensitivity = m_filterItemTableProxyModel->filterCaseSensitivity();

    if (!isDisconnected) {
      m_lastFetchedData = fetchRemoteItemsToModel(m_ui->clearCacheOnPagingCheckBox->isChecked(), m_itemListModelCache, resetPageIndex, kItemsPerPage, filterSettings);
      onDataFetched(resetPageIndex, m_lastFetchedData);
      onRowRangeChanged(m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize, m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize+m_lastFetchedData->requestedPageSize);
    } else {
      m_lastFetchedData = nullptr;
      onDataFetched(resetPageIndex, m_lastFetchedData);
      onRowRangeChanged(resetPageIndex*kItemsPerPage, resetPageIndex*kItemsPerPage+kItemsPerPage);
    }

    //onDataFetched(resetPageIndex, m_lastFetchedData);

    // // Invalidates the current sorting and filtering.
    m_filterItemTableProxyModel->invalidate();
    m_pagedItemTableProxyModel->invalidate();

    m_filterItemTableProxyModel->submit();
    m_pagedItemTableProxyModel->submit();

    // allows dynamic loading while using pagination
    m_pagedItemMapper->setCurrentIndex(resetPageIndex);
    m_pagedItemMapper->submit();
  });

  connect(m_ui->nextButton, &QAbstractButton::clicked, [this]() {
    const int nextPageIndex = std::max(0, m_pagedItemMapper->currentIndex() + 1);

    FilterSettings filterSettings;
    filterSettings.filterRegExp = m_filterItemTableProxyModel->filterRegExp();
    filterSettings.filterCaseSensitivity = m_filterItemTableProxyModel->filterCaseSensitivity();

    if (!isDisconnected) {
      m_lastFetchedData = fetchRemoteItemsToModel(m_ui->clearCacheOnPagingCheckBox->isChecked(), m_itemListModelCache, nextPageIndex, kItemsPerPage, filterSettings);
      onDataFetched(nextPageIndex, m_lastFetchedData);
      onRowRangeChanged(m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize, m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize+m_lastFetchedData->requestedPageSize);
     } else {
      m_lastFetchedData = nullptr;
      onDataFetched(nextPageIndex, m_lastFetchedData);
      onRowRangeChanged(nextPageIndex*kItemsPerPage, nextPageIndex*kItemsPerPage+kItemsPerPage);
    }

    //onDataFetched(nextPageIndex, m_lastFetchedData);

    // allows dynamic loading while using pagination
    m_pagedItemMapper->setCurrentIndex(nextPageIndex);
    m_pagedItemMapper->submit();

    /*m_ui->tableView->update();
    m_ui->tableView->show();

    m_ui->listView->update();
    m_ui->listView->show();*/
  });

  connect(m_pagedItemMapper.get(), &PagedItemMapper::currentIndexChanged, this, &MainWindow::onMapperIndexChanged);

  m_filterItemTableProxyModel->setSourceModel(m_itemTableProxyModel);
  m_filterItemTableProxyModel->setDynamicSortFilter(true);
  m_filterItemTableProxyModel->setSortCaseSensitivity (Qt::CaseInsensitive);
  m_filterItemTableProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
  // Invalidates the current sorting and filtering.
  m_filterItemTableProxyModel->invalidate();
  //m_filterItemTableProxyModel->submit();

  m_pagedItemTableProxyModel->setSourceModel(m_filterItemTableProxyModel);
  m_pagedItemTableProxyModel->setDynamicSortFilter(true);
  m_pagedItemTableProxyModel->setFilterMinSourceRowIndex(0);
  m_pagedItemTableProxyModel->setFilterMaxSourceRowIndex(kItemsPerPage);
  // Invalidates the current sorting and filtering.
  m_pagedItemTableProxyModel->invalidate();
  //m_pagedItemTableProxyModel->submit();

  m_pagedItemListProxyFilterModel->setSourceModel(m_pagedItemTableProxyModel);//m_filterItemTableProxyModel);
  m_pagedItemListProxyFilterModel->setExtraDataSource(m_itemListModelCache.get());

  m_ui->tableView->setModel(m_pagedItemTableProxyModel);
  m_ui->tableView->setColumnWidth(0, 150); // name
  m_ui->tableView->setColumnWidth(1, 150); // surname
  m_ui->tableView->update();
  m_ui->tableView->show();

  m_ui->tableView_2->setModel(m_filterItemTableProxyModel);
  m_ui->tableView_2->setColumnWidth(0, 150); // name
  m_ui->tableView_2->setColumnWidth(1, 150); // surname
  m_ui->tableView_2->update();
  m_ui->tableView_2->show();

  m_ui->listView->setModel(m_itemTableProxyModel);
  m_ui->listView->setModelColumn(0);
  m_ui->listView->update();
  m_ui->listView->show();

  m_ui->listView_2->setModel(m_itemTableProxyModel);
  m_ui->listView_2->setModelColumn(1);
  m_ui->listView_2->update();
  m_ui->listView_2->show();

  m_ui->listView_3->setModel(m_itemTableProxyModel);
  m_ui->listView_3->setModelColumn(2);
  m_ui->listView_3->update();
  m_ui->listView_3->show();

  if(!isDisconnected) {
    m_timer = new QTimer;
    m_timer->setSingleShot(true);
    // imitate remote load delay
    m_timer->start(1000);
    QObject::connect(m_timer, &QTimer::timeout, [this](){
        FilterSettings filterSettings;
        filterSettings.filterRegExp = m_filterItemTableProxyModel->filterRegExp();
        filterSettings.filterCaseSensitivity = m_filterItemTableProxyModel->filterCaseSensitivity();

        const int requestPageNum = 3;

        m_lastFetchedData = fetchRemoteItemsToModel(m_ui->clearCacheOnPagingCheckBox->isChecked(), m_itemListModelCache, requestPageNum, kItemsPerPage, filterSettings);
        onDataFetched(m_lastFetchedData->requestedPageNum, m_lastFetchedData);
        onRowRangeChanged(m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize, m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize+m_lastFetchedData->requestedPageSize);
        //m_pagedItemMapper->toFirst();
        m_pagedItemMapper->setCurrentIndex(requestPageNum);
        //m_pagedItemMapper->submit();
    });
  }

#if defined(QT_TESTLIB_LIB) && QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
  if (enableAbstractItemModelTester) {
    new QAbstractItemModelTester(m_itemListModelCache.get(), QAbstractItemModelTester::FailureReportingMode::Fatal, this);
    new QAbstractItemModelTester(m_filterItemTableProxyModel, QAbstractItemModelTester::FailureReportingMode::Fatal, this);
    new QAbstractItemModelTester(m_pagedItemTableProxyModel, QAbstractItemModelTester::FailureReportingMode::Fatal, this);
    new QAbstractItemModelTester(m_pagedItemListProxyFilterModel, QAbstractItemModelTester::FailureReportingMode::Fatal, this);
    new QAbstractItemModelTester(m_itemTableProxyModel, QAbstractItemModelTester::FailureReportingMode::Fatal, this);
  }
#endif

}

void MainWindow::onRowRangeChanged(int first, int last) {
  //m_pagedItemTableProxyModel->setFilterMinSourceRowIndex(m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize);
  //m_pagedItemTableProxyModel->setFilterMaxSourceRowIndex(m_lastFetchedData->requestedPageNum*m_lastFetchedData->requestedPageSize+m_lastFetchedData->requestedPageSize);
  m_pagedItemTableProxyModel->setFilterMinSourceRowIndex(first);
  m_pagedItemTableProxyModel->setFilterMaxSourceRowIndex(last);
}

void MainWindow::onDataFetched(int requestedPageNum, std::shared_ptr<fetchedPageData> data) {
  int pagesTotal = 1;
  int pageNum = requestedPageNum;

  if (data) {
    pagesTotal = data->totalPages;

    //m_pagedItemListProxyFilterModel->setWorkMode(PagedItemListProxyFilterModel::WorkMode::Online);
    m_pagedItemListProxyFilterModel->setPagesTotal(pagesTotal);
    m_pagedItemListProxyFilterModel->setPageSize(data->requestedPageSize);

    if (data->totalPages <= 0) {
      m_ui->prevButton->setEnabled(false);
      m_ui->nextButton->setEnabled(false);
    }
  } else {
    //m_pagedItemListProxyFilterModel->setWorkMode(PagedItemListProxyFilterModel::WorkMode::Offline);
    //m_pagedItemListProxyFilterModel->setOnlinePagesTotal(-1);

    m_pagedItemListProxyFilterModel->setPageSize(kItemsPerPage);

    // Invalidates the current sorting and filtering.
    m_filterItemTableProxyModel->invalidate();
    m_pagedItemTableProxyModel->invalidate();

    //m_filterItemTableProxyModel->submit();
    //m_pagedItemTableProxyModel->submit();

    if (kItemsPerPage != 0) {
      std::div_t res = std::div(m_filterItemTableProxyModel->rowCount(), kItemsPerPage);
      // Fast ceiling of an integer division
      pagesTotal = res.rem ? (res.quot + 1) : res.quot;
    }

    m_pagedItemListProxyFilterModel->setPagesTotal(pagesTotal);
  }

  //qDebug() << "pageNum" << pageNum << "pagesTotal" << pagesTotal;

  if (pagesTotal <= 0) {
    m_ui->prevButton->setEnabled(false);
    m_ui->nextButton->setEnabled(false);
  } else {
    m_ui->prevButton->setEnabled(pageNum > 0);
    m_ui->nextButton->setEnabled(pageNum < (pagesTotal - 1));
  }

  if (m_pagedItemMapper->model()->rowCount() <= 0) {
    /// \note currentIndexChanged don`t work on mapper with zero pages
    m_pagedItemWidget->clearContents();
  }
}

void MainWindow::onMapperIndexChanged(int pageNum) {
  m_ui->pageNumSpinBox->setValue(pageNum);


  // Invalidates the current sorting and filtering.
  m_filterItemTableProxyModel->invalidate();
  m_pagedItemTableProxyModel->invalidate();

  //m_pagedItemTableProxyModel->setFilterMinSourceRowIndex(pageNum*kItemsPerPage);
  //m_pagedItemTableProxyModel->setFilterMaxSourceRowIndex(pageNum*kItemsPerPage+kItemsPerPage);

  //m_pagedItemListProxyFilterModel->setFilterMinSourceRowIndex(pageNum*kItemsPerPage);
  //m_pagedItemListProxyFilterModel->setFilterMaxSourceRowIndex(pageNum*kItemsPerPage+kItemsPerPage);

  /*if (m_pagedItemMapper->model()->rowCount() <= 0) {
    m_ui->prevButton->setEnabled(false);
    m_ui->nextButton->setEnabled(false);
  } else {
    m_ui->prevButton->setEnabled(pageNum > 0);
    m_ui->nextButton->setEnabled(pageNum < (m_pagedItemMapper->model()->rowCount() - 1));
  }*/
}

MainWindow::~MainWindow()
{
  delete m_ui;
}
