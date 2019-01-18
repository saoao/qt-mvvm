#include "google_test.h"
#include "test_utils.h"
#include "sessionitem.h"
#include "itempool.h"
#include "taginfo.h"
#include <memory>

using namespace ModelView;

class TestSessionItem : public ::testing::Test
{
public:
    ~TestSessionItem();
};

TestSessionItem::~TestSessionItem() = default;

TEST_F(TestSessionItem, initialState)
{
    SessionItem item;
    const int role = ItemDataRole::DATA;

    EXPECT_EQ(item.model(), nullptr);
    EXPECT_EQ(item.parent(), nullptr);
    EXPECT_EQ(item.childrenCount(), 0);
    EXPECT_FALSE(item.data(role).isValid());
    EXPECT_TRUE(item.children().empty());
    EXPECT_TRUE(item.modelType().empty());
    EXPECT_TRUE(item.roles().empty());
}

TEST_F(TestSessionItem, modelType)
{
    SessionItem item2("Layer");
    EXPECT_EQ(item2.modelType(), "Layer");
}

TEST_F(TestSessionItem, setData)
{
    SessionItem item;
    const int role = ItemDataRole::DATA;

    EXPECT_FALSE(item.data(role).isValid());

    QVariant expected(42.0);
    EXPECT_TRUE(item.setData(expected, role));
    EXPECT_EQ(item.roles().size(), 1);
    EXPECT_EQ(item.data(role), expected);

    // setting another value
    EXPECT_TRUE(item.setData(QVariant::fromValue(43.0), role));
    EXPECT_EQ(item.roles().size(), 1);
    EXPECT_EQ(item.data(role), QVariant::fromValue(43.0));
}

//! Attempt to set the different Variant to already existing role.

TEST_F(TestSessionItem, variantMismatch)
{
    SessionItem item;
    const int role = ItemDataRole::DATA;
    QVariant expected(42.0);

    // setting data for the first time
    EXPECT_TRUE(item.setData(expected, role));
    EXPECT_EQ(item.roles().size(), 1);
    EXPECT_EQ(item.data(role), expected);

    // attempt to rewrite variant with another type
    EXPECT_THROW(item.setData(QVariant::fromValue(std::string("abc")), role), std::runtime_error);

    // removing value by passing invalid variant
    EXPECT_NO_THROW(item.setData(QVariant(), role));
    EXPECT_EQ(item.roles().size(), 0);
}

TEST_F(TestSessionItem, insertItem)
{
    std::unique_ptr<SessionItem> parent(new SessionItem);
    std::unique_ptr<SessionItem> child(new SessionItem);

    // empty parent
    EXPECT_EQ(parent->childrenCount(), 0);
    EXPECT_EQ(parent->rowOfChild(nullptr), -1);
    EXPECT_EQ(parent->rowOfChild(child.get()), -1);
    EXPECT_EQ(parent->childAt(0), nullptr);
    EXPECT_EQ(parent->childAt(-1), nullptr);
    EXPECT_EQ(parent->childAt(10), nullptr);

    // inserting child
    auto p_child = child.release();
    parent->insertItem(0, p_child);
    EXPECT_EQ(parent->childrenCount(), 1);
    EXPECT_EQ(parent->rowOfChild(p_child), 0);
    EXPECT_EQ(parent->children()[0], p_child);
    EXPECT_EQ(parent->childAt(0), p_child);
    EXPECT_EQ(p_child->parent(), parent.get());

    // deleting child
    delete p_child;
    EXPECT_EQ(parent->childrenCount(), 1);
    EXPECT_EQ(parent->children()[0], nullptr);
}

TEST_F(TestSessionItem, insertChildren)
{
    std::unique_ptr<SessionItem> parent(new SessionItem);
    auto child1 = new SessionItem;
    auto child2 = new SessionItem;
    auto child3 = new SessionItem;
    auto child4 = new SessionItem;

    // inserting two items
    parent->insertItem(-1, child1);
    parent->insertItem(-1, child2);
    EXPECT_EQ(parent->rowOfChild(child1), 0);
    EXPECT_EQ(parent->rowOfChild(child2), 1);
    EXPECT_EQ(parent->childAt(0), child1);
    EXPECT_EQ(parent->childAt(1), child2);
    std::vector<SessionItem*> expected = {child1, child2};
    EXPECT_EQ(parent->children(), expected);

    // inserting third item between two others
    parent->insertItem(1, child3);
    expected = {child1, child3, child2};
    EXPECT_EQ(parent->children(), expected);
    EXPECT_EQ(parent->rowOfChild(child1), 0);
    EXPECT_EQ(parent->rowOfChild(child2), 2);
    EXPECT_EQ(parent->rowOfChild(child3), 1);
    EXPECT_EQ(parent->childAt(0), child1);
    EXPECT_EQ(parent->childAt(1), child3);
    EXPECT_EQ(parent->childAt(2), child2);
    EXPECT_EQ(parent->childAt(3), nullptr);

    // inserting forth item using index equal to number of items
    parent->insertItem(parent->childrenCount(), child4);

    // checking parents
    EXPECT_EQ(child1->parent(), parent.get());
    EXPECT_EQ(child2->parent(), parent.get());
    EXPECT_EQ(child3->parent(), parent.get());
    EXPECT_EQ(child4->parent(), parent.get());

    // attempt to insert same item twice
    EXPECT_THROW(parent->insertItem(-1, child2), std::runtime_error);

    // attempt to insert item using out of scope index
    auto child5 = new SessionItem;
    EXPECT_THROW(parent->insertItem(parent->childrenCount()+1, child5), std::runtime_error);
    delete child5;
}

TEST_F(TestSessionItem, takeItem)
{
    std::unique_ptr<SessionItem> parent(new SessionItem);
    auto child1 = new SessionItem;
    auto child2 = new SessionItem;
    auto child3 = new SessionItem;

    // inserting items
    parent->insertItem(-1, child1);
    parent->insertItem(-1, child2);
    parent->insertItem(-1, child3);

    EXPECT_EQ(parent->childrenCount(), 3);

    // taking non-existing rows
    EXPECT_THROW(parent->takeItem(-1), std::runtime_error);
    EXPECT_THROW(parent->takeItem(parent->childrenCount()), std::runtime_error);

    // taking first row
    auto taken = parent->takeItem(0);
    EXPECT_EQ(taken->parent(), nullptr);
    std::vector<SessionItem*> expected = {child2, child3};
    EXPECT_EQ(parent->children(), expected);

    delete taken;
}

//! Item registration in a pool.

TEST_F(TestSessionItem, registerItem)
{
    std::unique_ptr<SessionItem> item(new SessionItem);
    std::shared_ptr<ItemPool> pool;

    EXPECT_TRUE(item->roles().empty());

    // registering item on unexisting pool
    item->register_item(pool);
    EXPECT_TRUE(item->roles().empty());

    // creating pool
    pool.reset(new ItemPool);
    item->register_item(pool);
    auto key = pool->key_for_item(item.get());
    std::vector<int> expected_roles = {ItemDataRole::IDENTIFIER};
    EXPECT_EQ(item->roles(), expected_roles);
    EXPECT_EQ(item->data(ItemDataRole::IDENTIFIER).value<std::string>(), key);
}

//! Item registration in a pool.

TEST_F(TestSessionItem, defaultTag)
{
    SessionItem item;
    EXPECT_EQ(item.defaultTag(), std::string("defaultTag"));
    EXPECT_TRUE(item.isTag("defaultTag"));
}

//! Registering tags

TEST_F(TestSessionItem, registerTag)
{
    SessionItem item;
    item.registerTag(TagInfo::universalTag("tagname"));
    EXPECT_TRUE(item.isTag("tagname"));

    //registering of tag with same name forbidden
    EXPECT_THROW(item.registerTag(TagInfo::universalTag("tagname")), std::runtime_error);
}


