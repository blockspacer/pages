#ifndef ITEM_H
#define ITEM_H

#include <QWidget>

#include "pageditemmodel.h"

namespace Ui {
class ItemWidget;
}

class ItemWidget : public QWidget
{
Q_OBJECT

public:
  explicit ItemWidget(QWidget *parent = nullptr);
  ~ItemWidget();

  void setMapper(std::shared_ptr<ItemMapper> mapper);

  std::shared_ptr<ItemMapper> getMapper();

  void setMappings();

  void removeMappings();
private:
  Ui::ItemWidget *m_ui;
  std::shared_ptr<ItemMapper> m_mapper;
};

#endif // ITEM_H
