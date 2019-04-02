#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "item_widget.h"
#include "paged_item_widget.h"

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
  ui->setupUi(this);

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
    ui->scrollVerticalLayout->addWidget(itemWidget);
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

  PagedItemMapper* m_pagedItemMapper = new PagedItemMapper;
  m_pagedItemMapper->setModel(m_pagedItemModel);
  m_pagedItemMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

  PagedItemWidget* m_pagedItemWidget = new PagedItemWidget;
  ui->scrollVerticalLayout->addWidget(m_pagedItemWidget);
  m_pagedItemMapper->addMapping(m_pagedItemWidget, 0, "m_PersonsPage");

  m_pagedItemMapper->toFirst();
  //m_pagedItemMapper->toNext();
}

MainWindow::~MainWindow()
{
  delete ui;
}
