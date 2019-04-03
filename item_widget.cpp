#include "item_widget.h"
#include "ui_item_widget.h"

ItemWidget::ItemWidget(QWidget *parent) :
QWidget(parent),
m_ui(new Ui::ItemWidget)
{
m_ui->setupUi(this);
}

void ItemWidget::setMapper(std::shared_ptr<ItemMapper> mapper) {
  m_mapper = mapper;
}

void ItemWidget::setMappings() {
  m_mapper->addMapping(m_ui->nameLineEdit, 0);
  m_mapper->addMapping(m_ui->surnameLineEdit, 1);
}

void ItemWidget::removeMappings() {
  m_mapper->removeMapping(m_ui->nameLineEdit);
  m_mapper->removeMapping(m_ui->surnameLineEdit);
}

std::shared_ptr<ItemMapper> ItemWidget::getMapper() {
  return m_mapper;
}

ItemWidget::~ItemWidget()
{
delete m_ui;
}
