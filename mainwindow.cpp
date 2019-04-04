#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "item_widget.h"
#include "paged_item_widget.h"

/// \note place no more than kItemsPerPage items into each ItemListModel

static int kItemsPerPage = 2;

static bool isDisconnected = false;

//static ItemListModel::Roles gFilterRole = ItemListModel::Roles::Name;

static std::shared_ptr<fetchedPageData> m_lastFetchedData;

static QVector<Item> dummyRemoteItems {
  Item{0, "0Atas", "Bork"},
  Item{1, "1Viktor", "Irman"},
  Item{2, "2Nadya", "Makeyn"},
  Item{3, "3Alli", "Shtadt"},
  Item{4, "4Gabriel", "Abrim"},
  Item{5, "5Gabram", "Bdhim"},
  Item{6, "6Ludovik", "Manstein"},
  Item{7, "7Klark", "Kent"},
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

static ItemWidget* createItemWidget(std::shared_ptr<ItemMapper> itemMapper) {
  ItemWidget* itemWidget = new ItemWidget();
  itemWidget->setMapper(itemMapper);
  itemWidget->setMappings();
  return itemWidget;
}

static QList<Item> retrieveRemoteFiltered(const QString& filter/*, const ItemListModel::Roles& filterRole*/) {
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

    if(!filter.isEmpty() && !filterItem.contains(filter)) {
      //qDebug() << "skipped " << filterItem;
      continue;
    } else {
      //qDebug() << "NOT skipped " << filterItem;
    }
    result.push_back(item);
  }
  return result;
}

static std::shared_ptr<fetchedPageData> retrieveRemoteItems(int pageNum, int itemsPerPage, const QString& filter/*, const ItemListModel::Roles& filterRole*/) {
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

static std::shared_ptr<fetchedPageData> fetchRemoteItemsToModel(bool clearCache, std::shared_ptr<ItemListModel> model, int pageNum, int itemsPerPage, const QString& filter/*, const ItemListModel::Roles& filterRole*/)
{
  if (pageNum < 0) {
    return nullptr;
  }

  std::shared_ptr<fetchedPageData> result = retrieveRemoteItems(pageNum, itemsPerPage, filter/*, filterRole*/);
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

  if (clearCache) {
    model->removeAllItems();
  }

  model->setRowsTotalSpace(remoteRowsTotal);

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

    /*{
        ItemModel* itemModel = createItemModel(item.guid, item.name, item.surname);
        if (itemModel) {
          std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
          itemModel->setParent(m_itemMapper.get());
          model->pushBack(m_itemMapper);
        }
        qDebug() << "fetchRemoteItemsToModel added item " << item.guid << item.name << item.surname;
    }*/

    // update cached item by GUID
    /*{
      const std::shared_ptr<ItemMapper> itemMapper = model->getItemById(item.guid);
      if (itemMapper.get()) {
        ItemModel* itemModel = static_cast<ItemModel*>(itemMapper->model());
        if (itemModel) {
          qDebug() << "fetchRemoteItemsToModel prev value " << itemModel->getName() << itemModel->getSurname() << itemModel->getGUID();
          itemModel->setName(item.name);
          itemModel->setSurname(item.surname);
        }
        qDebug() << "fetchRemoteItemsToModel replaced item " << item.guid << item.name << item.surname;
      }
    }*/

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
  m_filterItemTableProxyModel = new PagedItemTableProxyFilterModel();
  //m_filterItemTableProxyModel->setRowLimit(kItemsPerPage);
  m_pagedItemTableProxyModel = new PagedItemTableProxyFilterModel();
  m_pagedItemTableProxyModel->setRowLimit(kItemsPerPage); // limit shown items on page
  m_pagedItemListProxyFilterModel = new PagedItemListProxyFilterModel();
  m_pagedItemListProxyFilterModel->setWorkMode(PagedItemListProxyFilterModel::WorkMode::Offline);
  m_pagedItemListProxyFilterModel->setPageSize(kItemsPerPage); // limit items on widget page
  m_itemTableProxyModel = new ItemTableProxyModel();
  m_itemListModelCache = std::make_shared<ItemListModel>();
  m_itemTableProxyModel->setSourceModel(m_itemListModelCache.get());

  m_ui->prevButton->setEnabled(false);
  m_ui->nextButton->setEnabled(false);
  m_ui->pageSizeSpinBox->setValue(kItemsPerPage);
  m_ui->pageNumSpinBox->setValue(0);
  m_ui->clearCacheOnPagingCheckBox->setChecked(true);

  //m_ui->refreshButton->setEnabled(false);
  m_ui->refreshButton->setEnabled(true);

  {
    ItemModel* itemModel = createItemModel(0, "_0Alie", "Bork");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  {
    ItemModel* itemModel = createItemModel(1, "_1Bob", "Byorn");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  {
    ItemModel* itemModel = createItemModel(2, "_2Anna", "Kerman");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  {
    ItemModel* itemModel = createItemModel(3, "_3Hugo", "Geber");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  {
    ItemModel* itemModel = createItemModel(4, "_4Borat", "Nagler");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModelCache->pushBack(m_itemMapper);
  }

  m_pagedItemMapper->setModel(m_pagedItemListProxyFilterModel);
  m_pagedItemMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

  PagedItemWidget* m_pagedItemWidget = new PagedItemWidget;
  m_ui->scrollVerticalLayout->addWidget(m_pagedItemWidget);

  m_pagedItemMapper->addMapping(m_pagedItemWidget, static_cast<int>(PagedItemListProxyFilterModel::Columns::Page), m_pagedItemWidget->personsPagePropertyName());

  connect(m_ui->prevButton, &QAbstractButton::clicked, [this]() {
    const int prevPageIndex = std::max(0, m_pagedItemMapper->currentIndex() - 1);

    if (!isDisconnected) {
      m_lastFetchedData = fetchRemoteItemsToModel(m_ui->clearCacheOnPagingCheckBox->isChecked(), m_itemListModelCache, prevPageIndex, kItemsPerPage, m_ui->searchEdit->text()/*, gFilterRole*/);
    } else {
      m_lastFetchedData = nullptr;
    }
    onDataFetched(m_lastFetchedData);

    // allows dynamic loading while using pagination
    m_pagedItemMapper->setCurrentIndex(prevPageIndex);

    /*m_ui->tableView->update();
    m_ui->tableView->show();

    m_ui->listView->update();
    m_ui->listView->show();*/
  });

  connect(m_ui->pageSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int state) {
    kItemsPerPage = state;
  });

  connect(m_ui->goToPageButton, &QPushButton::clicked, [this]()
  {
    int pageNum = m_ui->pageNumSpinBox->value();
    pageNum = std::max(pageNum, 0);

    m_ui->searchEdit->setText(m_ui->searchEdit->text());
    m_filterItemTableProxyModel->setFilterFixedString(m_ui->searchEdit->text());

    if (!isDisconnected) {
      m_lastFetchedData = fetchRemoteItemsToModel(m_ui->clearCacheOnPagingCheckBox->isChecked(), m_itemListModelCache, pageNum, kItemsPerPage, m_ui->searchEdit->text()/*, gFilterRole*/);
    } else {
      m_lastFetchedData = nullptr;
    }
    onDataFetched(m_lastFetchedData);

    m_filterItemTableProxyModel->invalidate();
    m_pagedItemTableProxyModel->invalidate();

    // allows dynamic loading while using pagination
    m_pagedItemMapper->setCurrentIndex(pageNum);
  });

  connect(m_ui->checkBox, &QCheckBox::stateChanged, [this](int state) {
    isDisconnected = state > 0 ? true : false;
    qDebug() << "isDisconnected = " << isDisconnected;
    //m_ui->refreshButton->setEnabled(!isDisconnected);
  });

  connect(m_ui->searchButton, &QPushButton::clicked, [this]()
  {
    const int resetPageIndex = 0;

    if (!isDisconnected) {
      m_lastFetchedData = fetchRemoteItemsToModel(m_ui->clearCacheOnPagingCheckBox->isChecked(), m_itemListModelCache, resetPageIndex, kItemsPerPage, m_ui->searchEdit->text()/*, gFilterRole*/);
    } else {
      m_lastFetchedData = nullptr;
    }

    onDataFetched(m_lastFetchedData);

    m_filterItemTableProxyModel->setFilterFixedString(m_ui->searchEdit->text());

    m_filterItemTableProxyModel->invalidate();
    m_pagedItemTableProxyModel->invalidate();

    // allows dynamic loading while using pagination
    m_pagedItemMapper->setCurrentIndex(resetPageIndex);
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

    m_lastFetchedData = fetchRemoteItemsToModel(m_ui->clearCacheOnPagingCheckBox->isChecked(), m_itemListModelCache, refreshPageIndex, kItemsPerPage, m_ui->searchEdit->text()/*, gFilterRole*/);
    onDataFetched(m_lastFetchedData);

    m_filterItemTableProxyModel->invalidate();
    m_pagedItemTableProxyModel->invalidate();

     // allows dynamic loading while using pagination
     m_pagedItemMapper->setCurrentIndex(refreshPageIndex);
  });

  connect(m_ui->resetButton, &QPushButton::clicked, [this]()
  {
    const int resetPageIndex = 0;

    m_ui->searchEdit->setText("");
    m_filterItemTableProxyModel->setFilterFixedString("");

    if (!isDisconnected) {
      m_lastFetchedData = fetchRemoteItemsToModel(m_ui->clearCacheOnPagingCheckBox->isChecked(), m_itemListModelCache, resetPageIndex, kItemsPerPage, ""/*, gFilterRole*/);
    } else {
      m_lastFetchedData = nullptr;
    }

    onDataFetched(m_lastFetchedData);

    m_filterItemTableProxyModel->invalidate();
    m_pagedItemTableProxyModel->invalidate();

     // allows dynamic loading while using pagination
     m_pagedItemMapper->setCurrentIndex(resetPageIndex);
  });

  connect(m_ui->nextButton, &QAbstractButton::clicked, [this]() {
    const int nextPageIndex = m_pagedItemMapper->currentIndex() + 1;
    if (!isDisconnected) {
      m_lastFetchedData = fetchRemoteItemsToModel(m_ui->clearCacheOnPagingCheckBox->isChecked(), m_itemListModelCache, nextPageIndex, kItemsPerPage, m_ui->searchEdit->text()/*, gFilterRole*/);
     } else {
      m_lastFetchedData = nullptr;
    }

    onDataFetched(m_lastFetchedData);

    // allows dynamic loading while using pagination
    m_pagedItemMapper->setCurrentIndex(nextPageIndex);

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
  m_filterItemTableProxyModel->invalidate();

  m_pagedItemTableProxyModel->setSourceModel(m_filterItemTableProxyModel);
  m_pagedItemTableProxyModel->setDynamicSortFilter(true);
  m_pagedItemTableProxyModel->setSortCaseSensitivity (Qt::CaseInsensitive);
  m_pagedItemTableProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
  m_pagedItemTableProxyModel->setFilterMinSourceRowIndex(0);
  m_pagedItemTableProxyModel->setFilterMaxSourceRowIndex(kItemsPerPage);
  m_pagedItemTableProxyModel->invalidate();

  m_pagedItemListProxyFilterModel->setSourceModel(m_pagedItemTableProxyModel);//m_filterItemTableProxyModel);

  m_ui->tableView->setModel(m_pagedItemTableProxyModel);
  m_ui->tableView->setColumnWidth(1, 150);
  m_ui->tableView->update();
  m_ui->tableView->show();

  m_ui->tableView_2->setModel(m_filterItemTableProxyModel);
  m_ui->tableView_2->setColumnWidth(1, 150);
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
        m_lastFetchedData = fetchRemoteItemsToModel(m_ui->clearCacheOnPagingCheckBox->isChecked(), m_itemListModelCache, 0, kItemsPerPage, m_ui->searchEdit->text()/*, gFilterRole*/);
        onDataFetched(m_lastFetchedData);
        m_pagedItemMapper->toFirst();
    });
  }

}

void MainWindow::onDataFetched(std::shared_ptr<fetchedPageData> data) {
  if (m_lastFetchedData) {
    m_pagedItemListProxyFilterModel->setWorkMode(PagedItemListProxyFilterModel::WorkMode::Online);
    m_pagedItemListProxyFilterModel->setOnlinePagesTotal(m_lastFetchedData->totalPages);
    m_pagedItemListProxyFilterModel->setPageSize(m_lastFetchedData->requestedPageSize);
    m_pagedItemTableProxyModel->setRowLimit(m_lastFetchedData->requestedPageSize); // limit shown items on page
  } else {
    m_pagedItemListProxyFilterModel->setWorkMode(PagedItemListProxyFilterModel::WorkMode::Offline);
    m_pagedItemListProxyFilterModel->setOnlinePagesTotal(-1);
    m_pagedItemListProxyFilterModel->setPageSize(kItemsPerPage);
    m_pagedItemTableProxyModel->setRowLimit(kItemsPerPage); // limit shown items on page
  }
}

void MainWindow::onMapperIndexChanged(int pageNum) {
  m_ui->pageNumSpinBox->setValue(pageNum);

  m_pagedItemTableProxyModel->setFilterMinSourceRowIndex(pageNum*kItemsPerPage);
  m_pagedItemTableProxyModel->setFilterMaxSourceRowIndex(pageNum*kItemsPerPage+kItemsPerPage);

  //m_pagedItemListProxyFilterModel->setFilterMinSourceRowIndex(pageNum*kItemsPerPage);
  //m_pagedItemListProxyFilterModel->setFilterMaxSourceRowIndex(pageNum*kItemsPerPage+kItemsPerPage);

  m_ui->prevButton->setEnabled(pageNum > 0);
  m_ui->nextButton->setEnabled(pageNum < m_pagedItemMapper->model()->rowCount() - 1);
}

MainWindow::~MainWindow()
{
  delete m_ui;
}
