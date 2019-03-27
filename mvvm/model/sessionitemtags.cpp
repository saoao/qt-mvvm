// ************************************************************************** //
//
//  Prototype of mini MVVM framework for bornagainproject.org
//
//! @homepage  http://www.bornagainproject.org
//! @license   GNU General Public License v3 or higher
//
// ************************************************************************** //

#include "sessionitemtags.h"
#include "sessionitemcontainer.h"
#include <stdexcept>

using namespace ModelView;

SessionItemTags::SessionItemTags() {}

SessionItemTags::~SessionItemTags()
{
    for (auto tag : m_containers)
        delete tag;
}

void SessionItemTags::registerTag(TagInfo tagInfo, bool set_as_default)
{
    if (exists(tagInfo.name()))
        throw std::runtime_error("SessionItemTags::registerTag() -> Error. Existing name '"
                                 + tagInfo.name() + "'");

    m_containers.push_back(new SessionItemContainer(tagInfo));
    if (set_as_default)
        m_default_tag = tagInfo.name();
}

//! Returns true if container with such name exists.

bool SessionItemTags::exists(const std::string& tag_name) const
{
    for (auto tag : m_containers)
        if (tag->name() == tag_name)
            return true;
    return false;
}

//! Returns the name of the default tag.

std::string SessionItemTags::defaultTag() const
{
    return m_default_tag;
}

//! Returns vector of items in the container with given name.

std::vector<SessionItem*> SessionItemTags::items(const std::string& tag) const
{
    return container(tag)->items();
}

std::vector<SessionItem*> SessionItemTags::allitems() const
{
    std::vector<SessionItem*> result;
    for (auto cont : m_containers) {
        auto container_items = cont->items();
        result.insert(result.end(), container_items.begin(), container_items.end());
    }

    return result;
}

//! Inserts item in container with given tag name and at given index.
//! Returns true in the case of success.

bool SessionItemTags::insertItem(SessionItem* item, int index, const std::string& tag)
{
    return container(tag)->insertItem(item, index);
}

//! Returns tag name and index of item in container.

std::pair<std::string, int> SessionItemTags::tagIndexOfItem(const SessionItem* item) const
{
    std::pair<std::string, int> result = std::make_pair("", -1);
    for (auto cont : m_containers) {
        int index = cont->indexOfItem(item);
        if (index != -1)
            return std::make_pair(cont->name(), index);
    }

    return result;
}

//! Returns container corresponding to given tag name. If name is empty,
//! default tag will be used. Exception is thrown if no such tag exists.

SessionItemContainer* SessionItemTags::container(const std::string& tag_name) const
{
    std::string tagName = tag_name.empty() ? defaultTag() : tag_name;
    auto container = find_container(tagName);
    if (!container)
        throw std::runtime_error("SessionItemTags::container() -> Error. No such container '"+tagName+"'");

    return container;
}

//! Returns container corresponding to given tag name.

SessionItemContainer* SessionItemTags::find_container(const std::string& tag_name) const
{
    for (auto cont : m_containers)
        if (cont->name() ==tag_name)
            return cont;

    return nullptr;
}
