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

void PagedItemWidget::clearContents()
{
  // remove all page widgets
  {
    QLayoutItem* item;
    while ( ( item = ui->scrollVerticalLayout->takeAt( 0 ) ) != nullptr )
    {
      //delete item->widget();
      ItemWidget* itemW = (ItemWidget*)item->widget();
      itemW->removeMappings();
      item->widget()->deleteLater();
      delete item;
    }
  }
}

const QByteArray PagedItemWidget::personsPagePropertyName() const {
  const int propertyOffset = metaObject()->propertyOffset();
  return metaObject()->property(propertyOffset).name();
}

void PagedItemWidget::setPersonsPage(const QVariant &val)
{
  qDebug() << "setPersonsPage" << val;
  m_PersonsPage = val;

  clearContents();

  QList<std::shared_ptr<ItemMapper>> itemList = qvariant_cast<QList<std::shared_ptr<ItemMapper>>>(val.value<QVariant>());
  for (int i = 0; i < itemList.size(); i++) {
    std::shared_ptr<ItemMapper> itemMapper = itemList.at(i);
    ItemWidget* itemWidget = new ItemWidget();
    itemWidget->setMapper(itemMapper);
    itemWidget->setMappings();

    /// \note allows two-way data editing
    itemMapper->toFirst();

    ui->scrollVerticalLayout->addWidget(itemWidget);
  }

  emit personsPageChanged(val);
}
