# About

QT paginated model with QDataWidgetMapper.

Allows to use any widgets in scroll area with pagination/sorting/search e.t.c.

Can work both offline and online. Uses cache in offline mode.

```
*List* model stores pointers to 'Items' in QList. Every 'item' wrapped into it`s own mapper.
  -> *Table* Proxy converts flat list of 'Items' to multi-column table model (for easier filtering, sorting, showing in TableView e.t.c.)
    -> *Filter* Table Proxy provides filtering (search)
      -> *Paged* Table Proxy provides pagination in model (pagination built via passed start/end range)
        -> Paged *List* Proxy converts multi-column table model to flat item list limited to items on current page
          -> *Mapper* provides pagination used in mapped widgets. Passes item list to widgets on every page switch
            -> Paged *Widget* recieves item list from mapper on every page switch
              -> For every recieved item we create small widget call items mapper.
```

NOTE: 'Filter Proxy' and 'Paged Proxy' are separated because we can`t modify (override) row count in proxy without breaking model tests. So we changed row count by filtering.

NOTE: Widgets are destroyed/created on every page switch. You can also simply replace data without destroying widgets if you want constant page size. You can also hide/show some widgets if you have dynamic, but limited page size.

NOTE: You can draw controls/widgets (QStylePainter::drawControl). It is more complex and less flexible  solution. See https://stackoverflow.com/a/45390199

NOTE: Pagination reserves space in model: 1) You get page count from remote server 2) Reserve space in model by placing 'dummy' items (you may also want to clearCache/shrinkCache/fillCacheToMinimumRequiredSize e.t.c). 3) Use proxy filter to display items in range (page start to page end). You may want to show not loaded items (show loading circle e.t.c.) or hide em (for example, in offline mode to remove empty pages). See proxy model`s Q_PROPERTY for more settings.

NOTE: can also work with QML with minor modifications, see https://stackoverflow.com/a/19302329

## TODO

* TESTS

https://github.com/qt/qtbase/blob/8a73085a0dc1bdb6d25ac4afcd5429018b55b967/tests/auto/corelib/itemmodels/qsortfilterproxymodel/tst_qsortfilterproxymodel.cpp

http://imaginativethinking.ca/qtest-101-writing-unittests-qt-application/
