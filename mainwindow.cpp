#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "item_widget.h"
#include "paged_item_widget.h"

/// \note place no more than kItemsPerPage items into each ItemListModel
static int kItemsPerPage = 2;

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);

  // TODO: USE shared ptr!!!

  ItemListModel* m_itemListModel1 = new ItemListModel;

  {
    ItemMapper* m_itemMapper = new ItemMapper;
    ItemWidget* itemWidget = new ItemWidget();
    itemWidget->setMapper(m_itemMapper);
    itemWidget->setMappings();
    /// \note allows two-way data editing
    m_itemMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_itemMapper->getModel()->setName("Alie");
    m_itemMapper->getModel()->setSurname("Bork");
    m_itemMapper->toFirst();
    QObject::connect(m_itemMapper->getModel(), &ItemModel::dataChanged, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles){
      qDebug() << "ItemModel::dataChanged";
    });
    m_ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModel1->addItem(m_itemMapper);
  }

  {
    ItemMapper* m_itemMapper = new ItemMapper;
    /*ItemWidget* itemWidget = new ItemWidget();
    itemWidget->setMapper(m_itemMapper);
    itemWidget->setMappings();*/
    /// \note allows two-way data editing
    m_itemMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_itemMapper->getModel()->setName("Bob");
    m_itemMapper->getModel()->setSurname("Byorn");
    m_itemMapper->toFirst();
    QObject::connect(m_itemMapper->getModel(), &ItemModel::dataChanged, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles){
      qDebug() << "ItemModel::dataChanged";
    });
    //ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModel1->addItem(m_itemMapper);
  }

  ItemListModel* m_itemListModel2 = new ItemListModel;

  {
    ItemMapper* m_itemMapper = new ItemMapper;
    /*ItemWidget* itemWidget = new ItemWidget();
    itemWidget->setMapper(m_itemMapper);
    itemWidget->setMappings();*/
    /// \note allows two-way data editing
    m_itemMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_itemMapper->getModel()->setName("Frank");
    m_itemMapper->getModel()->setSurname("Gerin");
    m_itemMapper->toFirst();
    QObject::connect(m_itemMapper->getModel(), &ItemModel::dataChanged, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles){
      qDebug() << "ItemModel::dataChanged";
    });
    //ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModel2->addItem(m_itemMapper);
  }

  PagedItemModel* m_pagedItemModel = new PagedItemModel;
  m_pagedItemModel->addPage(m_itemListModel1);
  m_pagedItemModel->addPage(m_itemListModel2);

  m_pagedItemMapper = new PagedItemMapper;
  m_pagedItemMapper->setModel(m_pagedItemModel);
  m_pagedItemMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

  PagedItemWidget* m_pagedItemWidget = new PagedItemWidget;
  m_ui->scrollVerticalLayout->addWidget(m_pagedItemWidget);
  m_pagedItemMapper->addMapping(m_pagedItemWidget, 0, "m_PersonsPage");

  m_pagedItemMapper->toFirst();
  //m_pagedItemMapper->toNext();

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

  connect(m_pagedItemMapper, &PagedItemMapper::currentIndexChanged, this, &MainWindow::onMapperIndexChanged);
}

void MainWindow::onMapperIndexChanged(int pageNum) {
  m_ui->prevButton->setEnabled(pageNum > 0);
  m_ui->nextButton->setEnabled(pageNum < m_pagedItemMapper->model()->rowCount() / kItemsPerPage);

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
