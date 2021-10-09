#include <iostream>
#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <tuple>
#include <stdlib.h>
#include <ctime>

#include <iostream>

#include <sql/item.h>
#include <sql/item_cmpfunc.h>
#include <sql/sql_optimizer.h>
#include "unittest/gunit/test_utils.h"
#include "unittest/gunit/test_utils_benchmark.h"
#include "parsertest.h"

using my_testing::Server_initializer;
using benchmark::Stopwatch;

class ItemBenchmarks : public testing::Test {

    protected:
        Server_initializer initializer;

        void SetUp() override {
            initializer.SetUp();
            srand((unsigned) time(0));
        }

        void TearDown() override {
            initializer.TearDown();
        }

        THD *thd() {
            return initializer.thd();
        }

        std::pair<std::chrono::nanoseconds, double> time_item(Item *item) {
            Stopwatch s;
            s.start();
            auto value = item->val_real();
            s.stop();
            
            auto elapsed_time = s.duration();

            return std::pair<std::chrono::nanoseconds, double>(elapsed_time, value);
        }

        std::pair<long long, double> num_instructions(Item *item) {

            int fd = benchmark::perf_init();
            benchmark::perf_start(fd);
            double value = item->val_real();
            benchmark::perf_stop(fd);
            long long result = benchmark::perf_read(fd);
            close(fd);
            return std::pair<long long, double>(result, value);

        }

        static Item* get_item_by_id(int id) {
            // int item_id = (rand() % 4);
            int item_id = id % 4;
            switch(item_id) {
                case 0:
                    return new Item_int(2);
                case 1:
                    return new Item_func_log10(POS(), new Item_int(2));
                case 2:
                    return new Item_decimal(5.4);
                case 3:
                    return new Item_func_sin(POS(), new Item_decimal(0.41));
                default:
                    return nullptr;
            }
        }

        Item* build_deep_tree(size_t n) {
            Item *func = new Item_func_plus(get_item_by_id(0), get_item_by_id(1));
            func->fix_fields(thd(), &func);
            for(size_t i = 2; i < n; i++) {
                func = new Item_func_plus(get_item_by_id(i), func);
                func->fix_fields(thd(), &func);
            }
            return func;
        }

        Item* build_wide_tree(int l, int r) {
            if(r <= l) {
                return nullptr;
            }

            if(r == l + 1) {
                return ItemBenchmarks::get_item_by_id(l + 1);
            }

            int middle = (l + r) / 2;
            Item *a = build_wide_tree(l, middle);
            Item *b = build_wide_tree(middle, r);
            return new Item_func_plus(a, b);
        }

        void benchmark_item(Item* item, size_t ins_iterations, size_t time_iterations) {
            
            long long sum_iterations = 0L;
            for(size_t i = 0; i < ins_iterations; i++) {
                auto result = num_instructions(item);
                sum_iterations += result.first;
            }
            

            std::chrono::nanoseconds total_time = std::chrono::nanoseconds::zero();
            for(size_t i = 0; i < time_iterations; i++) {
                auto result = time_item(item);
                total_time += result.first;
            }

            long long avg_iterations = sum_iterations/ins_iterations;
            auto avg_time = total_time / time_iterations;

            std::cout << "\tAvg Iterations: " << avg_iterations << " after " << ins_iterations << " runs." <<  std::endl;
            std::cout << "\tAvg Time: " << std::chrono::duration_cast<std::chrono::microseconds>(avg_time).count() << "μs after " << time_iterations << " iterations." <<  std::endl;

            std::cout << "\tValue: " << item->val_real() << std::endl;
        }

};


TEST_F(ItemBenchmarks, StopWatchTest) {

    Stopwatch s;
    s.start();
    sleep(1);
    s.stop();
    auto time = s.duration();

    auto expected = std::chrono::milliseconds(1);
    ASSERT_TRUE(time - std::chrono::seconds(1) <= expected);

    s.start();
    sleep(1);
    s.stop();

    time = s.duration();

    expected = std::chrono::milliseconds(1);
    ASSERT_TRUE(time - std::chrono::seconds(2) <= expected);
}

TEST_F(ItemBenchmarks, ItemDeepMathTest) {

    // Build tree
    // Query_block *query = ::parse(&initializer, "SELECT 2 + 2", 0);
    // ASSERT_TRUE(query != nullptr);
    


    // // Initialize
    // Item *root = query->fields.front();
    // root->fix_fields(thd(), &root);

    int ns[5] = {100, 1000, 5000, 10000, 20000};

    for(size_t i = 0; i < 5; i++) {
        std::cout << "N: " << ns[i] << std::endl;
        Item *func = ItemBenchmarks::build_deep_tree(ns[i]);    
        benchmark_item(func, 5, 100);
    }
}

TEST_F(ItemBenchmarks, ItemWideMathTest) {

    int ns[5] = {100, 1000, 5000, 10000, 20000};
    for(size_t i = 0; i < 5; i++) {
        // int number_of_leaf_nodes = (ns[i] + 1)/2;
        std::cout << "N: " << ns[i] << std::endl;
        Item *root = ItemBenchmarks::build_wide_tree(0, ns[i]);    
        root->fix_fields(thd(), &root); 
        benchmark_item(root, 5, 100);
    }
}


