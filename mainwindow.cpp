#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "item_widget.h"
#include "paged_item_widget.h"

/// \note place no more than kItemsPerPage items into each ItemListModel
// static int kItemsPerPage = 2;

static ItemModel* createItemModel(const QString& name, const QString& surname, QObject* parent = nullptr) {
  ItemModel* itemModel = new ItemModel();
  if(parent) {
    itemModel->setParent(parent);
  }
  itemModel->setName(name);
  itemModel->setSurname(surname);
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

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);

  m_pagedItemMapper = std::make_shared<PagedItemMapper>();

  m_ui->prevButton->setEnabled(false);
  m_ui->nextButton->setEnabled(false);

  std::shared_ptr<ItemListModel> m_itemListModel1 = std::make_shared<ItemListModel>();

  {
    ItemModel* itemModel = createItemModel("Alie", "Bork");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModel1->addItem(m_itemMapper);
  }

  {
    ItemModel* itemModel = createItemModel("Bob", "Byorn");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModel1->addItem(m_itemMapper);
  }

  std::shared_ptr<ItemListModel> m_itemListModel2 = std::make_shared<ItemListModel>();

  {
    ItemModel* itemModel = createItemModel("Anna", "Kerman");
    std::shared_ptr<ItemMapper> m_itemMapper = createItemMapper(itemModel);
    itemModel->setParent(m_itemMapper.get());
    ItemWidget* itemWidget = createItemWidget(m_itemMapper);
    //m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModel2->addItem(m_itemMapper);
  }

  //std::shared_ptr<PagedItemModel> m_pagedItemModel = std::make_shared<PagedItemModel>();
  PagedItemModel* m_pagedItemModel = new PagedItemModel(m_pagedItemMapper.get());
  m_pagedItemModel->addPage(m_itemListModel1);
  m_pagedItemModel->addPage(m_itemListModel2);

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
    /*if (!isDisconnected) {
      m_lastFetchedData = fetchRemotePersonsToModel(m_mapper->currentIndex() - 1, kPersonsPerPage, m_ui->searchEdit->text(), filterRole);
    } else {
      m_lastFetchedData = nullptr;
    }

    refreshPageWidgets(m_lastFetchedData);
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
  });

  /*connect(m_ui->checkBox, &QCheckBox::stateChanged, [this](int state) {
    isDisconnected = state > 0 ? true : false;
    qDebug() << "isDisconnected " << isDisconnected;
  });
*/
  connect(m_ui->nextButton, &QAbstractButton::clicked, [this]() {
    /*if (!isDisconnected) {
      m_lastFetchedData = fetchRemotePersonsToModel(m_mapper->currentIndex() + 1, kPersonsPerPage, m_ui->searchEdit->text(), filterRole);
    } else {
      m_lastFetchedData = nullptr;
    }

    refreshPageWidgets(m_lastFetchedData);
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
  });

  connect(m_pagedItemMapper.get(), &PagedItemMapper::currentIndexChanged, this, &MainWindow::onMapperIndexChanged);

  m_pagedItemMapper->toFirst();
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
