#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "item_widget.h"

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  m_itemListModel = new ItemListModel;

  {
    ItemWidget* itemWidget = new ItemWidget();
    ItemMapper* m_itemMapper = new ItemMapper;
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
    m_itemListModel->addItem(m_itemMapper);
  }

  {
    ItemWidget* itemWidget = new ItemWidget();
    ItemMapper* m_itemMapper = new ItemMapper;
    itemWidget->setMapper(m_itemMapper);
    itemWidget->setMappings();
    /// \note allows two-way data editing
    m_itemMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_itemMapper->getModel()->setName("Bob");
    m_itemMapper->getModel()->setSurname("Byorn");
    m_itemMapper->toFirst();
    QObject::connect(m_itemMapper->getModel(), &ItemModel::dataChanged, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles){
      qDebug() << "ItemModel::dataChanged";
    });
    ui->scrollVerticalLayout->addWidget(itemWidget);
    m_itemListModel->addItem(m_itemMapper);
  }
}

MainWindow::~MainWindow()
{
  delete ui;
}
