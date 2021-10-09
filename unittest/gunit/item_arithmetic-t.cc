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
#include "parsertest.h"

using my_testing::Server_initializer;

class ItemSumTest : public testing::Test {


    protected:
        Server_initializer initializer;
        std::unordered_map<std::string, Fake_TABLE*> m_fake_tables;

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
        void query_block_initialize_table(Query_block *query_block) {
            int num_tables = 0;
            for(TABLE_LIST *tl = query_block->get_table_list(); tl != nullptr; tl = tl->next_global) {
                Fake_TABLE *fake_table = new Fake_TABLE(1, false);
                fake_table->alias = tl->alias;
                fake_table->pos_in_table_list = tl;
                tl->table = fake_table;
                tl->set_tableno(num_tables++);
                m_fake_tables[tl->alias] = fake_table;
            }
        }

        void item_field_resolve(Item_field *item_field) {
            Fake_TABLE *table = m_fake_tables["users"];
            item_field->table_ref = table->pos_in_table_list;
            if(strcmp(item_field->field_name, "x") == 0) {
                item_field->field = table->field[0];
                item_field->field->result_type();
            }
            else if(strcmp(item_field->field_name, "y") == 0) {
                item_field->field = table->field[1];
            } else {
                ASSERT_TRUE(false);
            }
            
            item_field->set_nullable(item_field->field->is_nullable());
            if(item_field->field != nullptr) {
                std::cout << "Field: " << item_field->field->type() << std::endl;
            }
        }

        void item_print_val(Item *it) {
            switch(it->result_type()) {
                case Item_result::INT_RESULT: {
                    std::cout << "(" << it->val_int() << ")";
                    break;
                }
                case Item_result::DECIMAL_RESULT: {
                    my_decimal d;
                    it->val_decimal(&d);
                    std::cout << "(";
                    // print_decimal(d1);
                    std::cout << ")";
                    break;
                }
                case Item_result::REAL_RESULT: {
                    std::cout << "(" << it->val_real() << ")";
                    break;
                }
                case Item_result::STRING_RESULT: {
                    String s;
                    it->val_str(&s);
                    std::cout << "('" << s.c_ptr() << "')";
                    break;
                }
                case Item_result::ROW_RESULT:
                case Item_result::INVALID_RESULT: {
                    std::cout << "(?)"; 
                }
            }
        }

        void item_print_children(Item* it, THD *thd, std::size_t depth = 0) {
            std::cout << std::string(depth, '\t') << typeid(*it).name();
            if(it->basic_const_item() || !it->fixed) {
                it->fix_fields(thd, &it);
            }
            item_print_val(it);
            std::cout << std::endl;
            if(Item_func* item = dynamic_cast<Item_func*>(it)) {
                for(uint i = 0; i < item->arg_count; i++) {
                    Item *child = item->m_embedded_arguments[i];
                    item_print_children(child, thd, depth + 1);
                }
            }
        }
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

TEST_F(ItemSumTest, ParseTest) {
    // TODO: Check the ParseAndResolve method to get a QueryBlock;

    Query_block *query = ::parse(&initializer, "SELECT LOG(2.0) + 1, x FROM users WHERE 0 = 1;", 0);
    if(query == nullptr) {
        return;
    }

    query_block_initialize_table(query);

    for(Item *f : query->fields) {
        if(Item_field *itf = dynamic_cast<Item_field*>(f)) {
            item_field_resolve(itf);
        } else {
            std::cout << "Item is not a Item_field: " << typeid(f).name() << std::endl; 
        }
    }

    std::cout << "SELECT:" << std::endl;
    for(Item* b : query->fields) {
        item_print_children(b, thd(), 1);
    }

    std::cout << "WHERE:" << std::endl;
    item_print_children(query->where_cond(), thd(), 1);

    std::cout << "FROM:" << std::endl;
    for(TABLE_LIST *tl =  query->get_table_list(); tl != nullptr; tl = tl->next_global) {
        printf("Table Name: %s\n", tl->table_name);
    }

    // query->walk([](*uchar chr) {}, enum_walk::PREFIX, nullptr);
}
