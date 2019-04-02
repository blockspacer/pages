#include "paged_item_widget.h"
#include "pageditemmodel.h"
#include "item_widget.h"
#include "ui_paged_item_widget.h"

PagedItemWidget::PagedItemWidget(QWidget *parent) :
QWidget(parent),
ui(new Ui::PagedItemWidget)
{
  ui->setupUi(this);
}

PagedItemWidget::~PagedItemWidget()
{
  delete ui;
}

QVariant PagedItemWidget::getPersonsPage() const
{
  qDebug() << "getPersonsPage";
  return m_PersonsPage;
}

void PagedItemWidget::setPersonsPage(const QVariant &val)
{
  qDebug() << "setPersonsPage" << val;
  m_PersonsPage = val;

  qDebug() << "PersonPageWidget clearPage";
  // remove all page widgets
  {
    QLayoutItem* item;
    while ( ( item = ui->scrollVerticalLayout->takeAt( 0 ) ) != nullptr )
    {
        //delete item->widget();
        ItemWidget* itemW = (ItemWidget*)item->widget();
        itemW->removeMappings();
        item->widget()->deleteLater();
        //item->deleteLater();
        delete item;
    }
  }

  ItemListModel* itemListModel = qvariant_cast<ItemListModel*>(val.value<QVariant>());

  qDebug() << "setPersonsPage rowCount" << itemListModel->rowCount(QModelIndex());

  for (int i = 0; i < itemListModel->rowCount(QModelIndex()); i++) {
    QModelIndex index = itemListModel->index(i, 0);
    qDebug() << "setPersonsPage index" << index;

    //itemListModel->itemFromIndex(index_person);
    QVariant itemMapperVariant = itemListModel->data(index, 0);
    qDebug() << "setPersonsPage itemMapperVariant" << itemMapperVariant;
    ItemMapper* itemMapper = qvariant_cast<ItemMapper*>(itemMapperVariant.value<QVariant>());

    ItemWidget* itemWidget = new ItemWidget();
    itemWidget->setMapper(itemMapper);
    itemWidget->setMappings();

    //itemMapper->getModel()->setName("setName");

    /// \note allows two-way data editing
    itemMapper->toFirst();

    ui->scrollVerticalLayout->addWidget(itemWidget);
  }

  emit personsPageChanged(val);
}
