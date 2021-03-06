// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#ifndef ITEMTESTWIDGET_H
#define ITEMTESTWIDGET_H

#include <string>
#include <mvvm/model/tagrow.h>

//! Various common utils for unit tests.

namespace ModelView {
class SessionItem;
class SessionModel;
}

//! Interface for testing callbacks comming from SessionItem within gmock framework.

class ItemTestWidgetInterface {
public:
    virtual ~ItemTestWidgetInterface();

    virtual void onItemDestroy(ModelView::SessionItem* item) = 0;
    virtual void onDataChange(ModelView::SessionItem* item, int role) = 0;
    virtual void onPropertyChange(ModelView::SessionItem* item, std::string name) = 0;
    virtual void onItemInserted(ModelView::SessionItem* item, ModelView::TagRow) = 0;
    virtual void onAboutToRemoveItem(ModelView::SessionItem* item, ModelView::TagRow) = 0;
};

//! Interface for testing callbacks comming from SessionModel within gmock framework.

class ModelTestWidgetInterface {
public:
    virtual ~ModelTestWidgetInterface();

    virtual void onModelDestroyed(ModelView::SessionModel*) = 0;
    virtual void onModelAboutToBeReset(ModelView::SessionModel*) = 0;
    virtual void onModelReset(ModelView::SessionModel*) = 0;
    virtual void onDataChange(ModelView::SessionItem*, int) = 0;
    virtual void onItemInserted(ModelView::SessionItem*, ModelView::TagRow) = 0;
    virtual void onItemRemoved(ModelView::SessionItem*, ModelView::TagRow) = 0;
    virtual void onAboutToRemoveItem(ModelView::SessionItem*, ModelView::TagRow) = 0;
};

#endif // ITEMTESTWIDGET_H
