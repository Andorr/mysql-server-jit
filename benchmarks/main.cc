#include <iostream>

#include "sql/item.h"
#include "sql/item_func.h"
#include "sql/sql_class.h"
#include "sql/sql_lex.h"

// #include "benchmarks/test_utils.h"

int main() {
    // THD *m_thd = new THD(false);
    // THD *stack_thd = m_thd;

    // m_thd->set_new_thread_id();

    // m_thd->thread_stack = (char *)&stack_thd;
    // m_thd->store_globals();
    // lex_start(m_thd);

    // my_testing::Server_initializer initializer;
    // initializer.SetUp();

    // Stopwatch s;
    // s.start();

    THD thd(false);

    Item *func = new Item_func_plus(new Item_int(2), new Item_int(2));
    func->fix_fields(&thd, &func);



    std::cout << "Hello, world! :D" << std::endl;
    // s.stop();
    std::cout << func->val_int() << std::endl;

    // initializer.TearDown();
    return 0;
}