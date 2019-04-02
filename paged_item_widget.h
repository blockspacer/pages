#ifndef PAGED_ITEM_WIDGET_H
#define PAGED_ITEM_WIDGET_H

#include <QWidget>
#include <QVariant>
#include <QtCore>

namespace Ui {
class PagedItemWidget;
}

class PagedItemWidget : public QWidget
{
Q_OBJECT

Q_PROPERTY(QVariant m_PersonsPage READ getPersonsPage WRITE setPersonsPage NOTIFY personsPageChanged USER true)

public:
  explicit PagedItemWidget(QWidget *parent = nullptr);
  ~PagedItemWidget();

  QVariant getPersonsPage() const;

  //void clearPage();

  //void setMapper(QDataWidgetMapper* mapper);

  //void refreshPageWidgets();
public slots:
    void setPersonsPage(const QVariant& val);

signals:
    void personsPageChanged(const QVariant& val);
   // void PersonsPageModified(const QVariant& val);


//private:
//  void refreshPageWidgets(QList<PersonsPageItem>& pageItems);

private:
  Ui::PagedItemWidget *ui;
  QVariant m_PersonsPage;
};

#endif // PAGED_ITEM_WIDGET_H
