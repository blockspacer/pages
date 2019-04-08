# About

QT paginated model. Allows to use any widgets in scroll area.

*List* model stores pointers to 'Items' in QList. Every 'item' wrapped into it`s own mapper.
  -> *Table* Proxy converts flat list of 'Items' to multi-column table model (for easier filtering, sorting, e.t.c.)
    -> *Filter* Table Proxy provides filtering (search)
      -> *Paged* Table Proxy provides pagination in model (pagination built via passed start/end range)
        -> Paged *List* Proxy converts multi-column table model to flat item list limited to items on current page
          -> *Mapper* provides pagination used in mapped widgets. Passes item list to widgets on every page switch
            -> Paged *Widget* recieves item list from mapper on every page switch
              -> For every recieved item we create small widget call items mapper.

NOTE: 'Filter Proxy' and 'Paged Proxy' are separated because we can`t modify (override) row count in proxy without breaking model tests. So we changed row count by filtering.

NOTE: Widgets are destroyed/created on every page switch. You can also simply replace data without destroying widgets if you want constant page size. You can also hide/show some widgets if you have dynamic, but limited page size.

## TODO

TESTS https://github.com/qt/qtbase/blob/8a73085a0dc1bdb6d25ac4afcd5429018b55b967/tests/auto/corelib/itemmodels/qsortfilterproxymodel/tst_qsortfilterproxymodel.cpp
http://imaginativethinking.ca/qtest-101-writing-unittests-qt-application/
