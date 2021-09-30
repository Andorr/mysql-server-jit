#include <iostream>
#include <assert.h>
#include <gtest/gtest.h>
#include <stddef.h>

#include <sstream>
#include <string>
#include <vector>

#include "my_inttypes.h"
#include "my_table_map.h"
#include "sql/item.h"
#include "sql/item_cmpfunc.h"
#include "sql/parse_tree_helpers.h"
#include "sql/sql_class.h"
#include "sql/sql_lex.h"
#include "sql_string.h"
#include "unittest/gunit/fake_table.h"
#include "unittest/gunit/test_utils.h"

using my_testing::Server_initializer;

class ItemSumTest : public testing::Test {


    protected:
        Server_initializer initializer;

        void SetUp() override {
            initializer.SetUp();
        }

        void TearDown() override {
            initializer.TearDown();
        }

        THD *thd() {
            return initializer.thd();
        }

    public:
        /* static Item_func create_item(Item_func::Functype type, int v1, int v2) {
            Item_func *item;
            switch(type) {
                case Item_func::EQ_FUNC:
                item = new Item_equal()
            }
        } */
};

TEST_F(ItemSumTest, RandomTest) {
    ASSERT_TRUE(true);
    ASSERT_TRUE(1 == 1);

    Item_int *a = new Item_int(20);
    Item_int *b = new Item_int(10);
    Item_func_gt *item_func = new Item_func_gt(a,b);
    Item *item = static_cast<Item*>(item_func);
    item_func->fix_fields(thd(), &item);
    
    ASSERT_TRUE(item_func->val_int() == 1);
    ASSERT_TRUE(item_func->val_real() == 1.0);
    ASSERT_TRUE(item_func->val_bool());

    String s;
    String *result = item_func->val_str(&s);
    ASSERT_TRUE(strcmp(result->c_ptr(), "1") == 0);
};
