#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  m_mapper = new ItemMapper;
  /// \note allows two-way data editing
  m_mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
  m_mapper->AddNameMapping(ui->namelineEdit);
  m_mapper->AddSurnameMapping(ui->surnamelineEdit);
  m_mapper->setName("Alie");
  m_mapper->setSurname("Bork");
  m_mapper->toFirst();

  QObject::connect(m_mapper->getModel(), &ItemModel::dataChanged, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles){
    qDebug() << "ItemModel::dataChanged";
  });
}

MainWindow::~MainWindow()
{
  delete ui;
}
