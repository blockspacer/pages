#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "item_widget.h"
#include "paged_item_widget.h"

/// \note place no more than kItemsPerPage items into each ItemListModel

static int kItemsPerPage = 2;

static std::shared_ptr<ItemListModel> m_itemListModelCache;

static bool isDisconnected = false;

//static ItemListModel::Roles gFilterRole = ItemListModel::Roles::Name;

static std::shared_ptr<fetchedPageData> m_lastFetchedData;

static QVector<Item> dummyRemoteItems {
  Item{0, "0Anna", "Bork"},
  Item{1, "1Boris", "Irman"},
  Item{2, "2Byorn", "Makeyn"},
  Item{3, "3Alli", "Keyn"},
  Item{4, "4Gabriel", "Abrim"},
};

static ItemModel* createItemModel(int guid, const QString& name, const QString& surname, QObject* parent = nullptr) {
  ItemModel* itemModel = new ItemModel();
  if(parent) {
    itemModel->setParent(parent);
  }
  itemModel->setName(name);
  itemModel->setSurname(surname);
  itemModel->setGUID(guid);
  QObject::connect(itemModel, &ItemModel::dataChanged, [](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles){
    qDebug() << "ItemModel::dataChanged";
  });
  return itemModel;
}

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
  std::shared_ptr<fetchedPageData> result = std::make_shared<fetchedPageData>();

  QList<Item> dummyRemotePage;

  auto filtered = retrieveRemoteFiltered(filter/*, filterRole*/);

  /*for ( Item& item : filtered) {
    qDebug() << "retrieveRemoteItems item " << item.name << item.surname;
  }*/

  int cursorI = pageNum * itemsPerPage;
  if (cursorI >= filtered.size()) {
    qDebug() << "not enough persons. Requested page " << pageNum;
  }

  int pageItemsCount = std::min(cursorI + itemsPerPage, filtered.size());

  for (; cursorI < pageItemsCount; cursorI++) {
    //filtered[cursorI].name = "Bao";//;GetRandomString();

    dummyRemotePage.push_back(filtered.at(cursorI));
  }

  std::div_t res = std::div(filtered.size(), itemsPerPage);
  // Fast ceiling of an integer division
  int pageCount = res.rem ? (res.quot + 1) : res.quot;

  result->items = dummyRemotePage;
  result->requestedPageNum = pageNum;
  result->totalPages = pageCount;
  result->totalItems = filtered.size(); // TODO
  result->recievedItemsCount = pageItemsCount;
  result->requestedPageSize = itemsPerPage;

  return result;
}

static std::shared_ptr<fetchedPageData> fetchRemoteItemsToModel(std::shared_ptr<ItemListModel> model, int pageNum, int itemsPerPage, const QString& filter/*, const ItemListModel::Roles& filterRole*/)
{
  std::shared_ptr<fetchedPageData> result = retrieveRemoteItems(pageNum, itemsPerPage, filter/*, filterRole*/);
  if (pageNum >= result->totalPages) {
    qDebug() << "nothing to show!";
    return result;
  }

  for ( Item& item : result->items) {
    const std::shared_ptr<ItemMapper> itemMapper = model->getItemById(item.guid);
    if (itemMapper.get()) {
      ItemModel* itemModel = static_cast<ItemModel*>(itemMapper->model());
      itemModel->setName(item.name);
      itemModel->setSurname(item.surname);
      qDebug() << "fetchRemoteItemsToModel replaced item " << item.guid << item.name << item.surname;
    } else {
      ItemModel* itemModel = createItemModel(item.guid, item.name, item.surname);
      std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
      itemModel->setParent(m_itemMapper.get());
      model->pushBack(m_itemMapper);
      qDebug() << "fetchRemoteItemsToModel added item " << item.guid << item.name << item.surname;
    }
  }
}

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);

  m_pagedItemMapper = std::make_shared<PagedItemMapper>();

  m_ui->prevButton->setEnabled(false);
  m_ui->nextButton->setEnabled(false);

  m_itemListModelCache = std::make_shared<ItemListModel>();

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

  std::shared_ptr<ItemListModel> m_itemListModelPage1 = std::make_shared<ItemListModel>();
  m_itemListModelPage1->pushBack(m_itemListModelCache->getItemAt(0));
  m_itemListModelPage1->pushBack(m_itemListModelCache->getItemAt(1));

  std::shared_ptr<ItemListModel> m_itemListModelPage2 = std::make_shared<ItemListModel>();
  m_itemListModelPage2->pushBack(m_itemListModelCache->getItemAt(2));

  //std::shared_ptr<PagedItemModel> m_pagedItemModel = std::make_shared<PagedItemModel>();
  ItemPageListModel* m_pagedItemModel = new ItemPageListModel(m_pagedItemMapper.get());
  m_pagedItemModel->pushBack(m_itemListModelPage1);
  m_pagedItemModel->pushBack(m_itemListModelPage2);

  //m_pagedItemModel->removePageAt(0);
  //m_pagedItemModel->replacePageAt(0, m_itemListModel1);
  //m_pagedItemModel->removeAllPages();
  //m_pagedItemMapper->currentIndexChanged(0);

  m_pagedItemMapper->setModel(m_pagedItemModel);
  m_pagedItemMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

  PagedItemWidget* m_pagedItemWidget = new PagedItemWidget;
  m_ui->scrollVerticalLayout->addWidget(m_pagedItemWidget);

  m_pagedItemMapper->addMapping(m_pagedItemWidget, 0, m_pagedItemWidget->personsPagePropertyName());

  connect(m_ui->prevButton, &QAbstractButton::clicked, [this]() {
    if (!isDisconnected) {
      m_lastFetchedData = fetchRemoteItemsToModel(m_itemListModelCache, m_pagedItemMapper->currentIndex() - 1, kItemsPerPage, m_ui->searchEdit->text()/*, gFilterRole*/);
    } else {
      m_lastFetchedData = nullptr;
    }

    /*refreshPageWidgets(m_lastFetchedData);
    // NOTE: empty mapper won't call currentIndexChanged
    m_personsWidget->clearPage();
    //model->dataChanged(QModelIndex(),QModelIndex());
    if (!m_lastFetchedData || !m_lastFetchedData->recievedPagePersonsNum) {
      qDebug() << "nothing to show";
      m_ui->prevButton->setEnabled(false);
      m_ui->nextButton->setEnabled(false);
      return;
    }
*/
    m_pagedItemMapper->toPrevious();

    //m_ui->tableView->setModel(m_itemListModelCache.get());
    //m_ui->tableView->rowCountChanged(0, m_itemListModelCache->rowCount());
    //m_ui->tableView->adjustSize();

    //m_ui->tableView->setModel(m_itemListModelCache.get());
    m_ui->tableView->update();
    m_ui->tableView->show();

    m_ui->listView->update();
    m_ui->listView->show();
  });

  connect(m_ui->checkBox, &QCheckBox::stateChanged, [this](int state) {
    isDisconnected = state > 0 ? true : false;
    qDebug() << "isDisconnected " << isDisconnected;
  });

  connect(m_ui->nextButton, &QAbstractButton::clicked, [this]() {
    if (!isDisconnected) {
      m_lastFetchedData = fetchRemoteItemsToModel(m_itemListModelCache, m_pagedItemMapper->currentIndex() + 1, kItemsPerPage, m_ui->searchEdit->text()/*, gFilterRole*/);
    } else {
      m_lastFetchedData = nullptr;
    }

    /*refreshPageWidgets(m_lastFetchedData);
    m_personsWidget->clearPage();
    // NOTE: empty mapper won't call currentIndexChanged
    //model->dataChanged(QModelIndex(),QModelIndex());
    if (!m_lastFetchedData || !m_lastFetchedData->recievedPagePersonsNum) {
      qDebug() << "nothing to show";
      m_ui->prevButton->setEnabled(false);
      m_ui->nextButton->setEnabled(false);
      return;
    }*/

    m_pagedItemMapper->toNext();

    //m_ui->tableView->setModel(m_itemListModelCache.get());
    m_ui->tableView->update();
    m_ui->tableView->show();

    m_ui->listView->update();
    m_ui->listView->show();
  });

  connect(m_pagedItemMapper.get(), &PagedItemMapper::currentIndexChanged, this, &MainWindow::onMapperIndexChanged);

  m_pagedItemMapper->toFirst();

  ItemTableProxyModel* itemTableProxyModel = new ItemTableProxyModel();
  itemTableProxyModel->setSourceModel(m_itemListModelCache.get());

  //m_ui->tableView->setModel(m_itemListModelCache.get());
  m_ui->tableView->setModel(itemTableProxyModel);
  //m_ui->tableView->setUpdatesEnabled(true);
  m_ui->tableView->setColumnWidth(1, 150);
  m_ui->tableView->update();
  m_ui->tableView->show();

  m_ui->listView->setModel(itemTableProxyModel);
  //m_ui->listView->scroll(0,1);
  //m_ui->listView->setSpacing(7);
  //m_ui->listView->resize(170,170);
  m_ui->listView->setModelColumn(0);
  //m_ui->listView->setUniformItemSizes(true);
  //m_ui->listView->setGridSize(QSize(150,150));
  // m_ui->listView->setModelColumn(0);
  //m_ui->listView->resizeRowsToContents
  m_ui->listView->update();
  m_ui->listView->show();

  m_ui->listView_2->setModel(itemTableProxyModel);
  m_ui->listView_2->setModelColumn(1);
  m_ui->listView_2->update();
  m_ui->listView_2->show();

  m_ui->listView_3->setModel(itemTableProxyModel);
  m_ui->listView_3->setModelColumn(2);
  m_ui->listView_3->update();
  m_ui->listView_3->show();

/*QListView* listView = new QListView(this);
    listView->setModel(m_itemListModelCache.get());
    listView->show();
    m_ui->scrollVerticalLayout->addWidget(listView);*/

  //QStringList numbers;
  //numbers << "One" << "Two" << "Three" << "Four" << "Five";
  //QStringListModel *model = new QStringListModel(numbers);
  //m_ui->listView->setModel(model);

  //m_ui->listView->setUpdatesEnabled(true);
  //m_ui->listView->setColumnWidth(1, 150);
}

void MainWindow::onMapperIndexChanged(int pageNum) {
  //qDebug() << "onMapperIndexChanged " << pageNum;

  m_ui->prevButton->setEnabled(pageNum > 0);
  //m_ui->nextButton->setEnabled(pageNum < m_pagedItemMapper->model()->rowCount() / kItemsPerPage);
  const int pagesTotal = m_pagedItemMapper->model()->rowCount() - 1;
  m_ui->nextButton->setEnabled(pageNum < pagesTotal);

  /*if (!m_lastFetchedData || !m_lastFetchedData->recievedPagePersonsNum) {
    qDebug() << "nothing to show";
    //ui->prevButton->setEnabled(false);
    //ui->nextButton->setEnabled(false);
    return;
  }

  int totalPersons = m_lastFetchedData->totalItems;
  int recievedPersons = m_lastFetchedData->recievedPagePersonsNum;
  int pageStart = m_lastFetchedData->requestedPageNum * m_lastFetchedData->requestedPageSize;
  //ui->prevButton->setEnabled(pageStart > 0 && pageStart < totalPersons);
  //ui->nextButton->setEnabled((pageStart + lastFetchedData->requestedPageSize) < totalPersons);

  //qDebug() << "onMapperIndexChanged requestedPageNum " << lastFetchedData->requestedPageNum;
  //qDebug() << "onMapperIndexChanged totalPages " << lastFetchedData->totalPages;

  m_ui->prevButton->setEnabled(m_lastFetchedData->requestedPageNum > 0);
  m_ui->nextButton->setEnabled(m_lastFetchedData->requestedPageNum < (m_lastFetchedData->totalPages - 1));
*/
}

MainWindow::~MainWindow()
{
  delete m_ui;
}
