// ************************************************************************** //
//
//  Prototype of mini MVVM framework for bornagainproject.org
//
//! @homepage  http://www.bornagainproject.org
//! @license   GNU General Public License v3 or higher
//
// ************************************************************************** //

#include "insertnewitemcommand.h"
#include "path.h"
#include "sessionitem.h"
#include "sessionmodel.h"
#include "itemfactory.h"
#include <sstream>

namespace
{
std::string generate_description(const std::string& modelType, const std::string& tag, int row);
} // namespace

using namespace ModelView;

struct InsertNewItemCommand::InsertNewItemCommandPrivate {
    model_type m_model_type;
    item_factory_func_t factory_func;
    std::string m_tag;
    int m_row;
    result_t m_result;
    Path m_item_path;
    InsertNewItemCommandPrivate(model_type modelType, item_factory_func_t func, std::string tag, int row)
        : m_model_type(std::move(modelType)), factory_func(func), m_tag(std::move(tag)), m_row(row), m_result(nullptr)
    {
    }
};

InsertNewItemCommand::InsertNewItemCommand(model_type modelType, item_factory_func_t func, SessionItem* parent,
                                           std::string tag, int row)
    : AbstractItemCommand(parent),
      p_impl(std::make_unique<InsertNewItemCommandPrivate>(modelType, func, tag, row))
{
    setDescription(generate_description(p_impl->m_model_type, p_impl->m_tag, p_impl->m_row));
    p_impl->m_item_path = pathFromItem(parent);
}

InsertNewItemCommand::~InsertNewItemCommand() = default;

void InsertNewItemCommand::undo_command()
{
    auto parent = itemFromPath(p_impl->m_item_path);
    int row = p_impl->m_row < 0 ? parent->itemCount(p_impl->m_tag)-1 : p_impl->m_row;
    delete parent->takeItem(p_impl->m_tag, row);
    p_impl->m_result = nullptr;
}

void InsertNewItemCommand::execute_command()
{
    auto parent = itemFromPath(p_impl->m_item_path);
    // FIXME get rid of manager in the favor of factory function generated in CommandService
//    auto child = model()->factory()->createItem(p_impl->m_model_type).release();
    auto child = p_impl->factory_func().release();
    parent->insertItem(child, p_impl->m_tag, p_impl->m_row);
    p_impl->m_result = child;
}

InsertNewItemCommand::result_t InsertNewItemCommand::result() const
{
    return p_impl->m_result;
}

namespace
{
std::string generate_description(const std::string& modelType, const std::string& tag, int row)
{
    std::ostringstream ostr;
    ostr << "New item type '" << modelType << "' tag:'" << tag << "', row:" << row;
    return ostr.str();
}
} // namespace
