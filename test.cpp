#include <iostream>
#include "gtest/gtest.h"
#include "list.h"

void dump(my::list<int> &list) {
    std::cout << "dump: \n";
    for (auto it = list.begin(); it != list.end(); ++it) {
        auto el = *it;
        std::cout << el << " ";
    }
    std::cout << "\n";
}

template <typename It1, typename It2>
void assert_range_equality(It1 f1, It1 l1, It2 f2, It2 l2) {
    for (; f1 != l1 and f2 != l2; ++f1, ++f2) {
        ASSERT_EQ(*f1, *f2);
    }
    ASSERT_TRUE(f1 == l1 and f2 == l2);
}

TEST(correctness, empty) {
    my::list<int> list;
    ASSERT_TRUE(list.empty());
}

TEST(correctness, initializer_list) {
    my::list<int> list{1, 2, 3, 4};
    std::vector<int> v{1, 2, 3, 4};
    assert_range_equality(list.begin(), list.end(), v.begin(), v.end());
}

TEST(correctness, back_front) {
    my::list<int> list{10, 8, 9, 6, 4};
    ASSERT_EQ(list.back(), 4);
    ASSERT_EQ(list.front(), 10);
    list.pop_back();
    list.pop_front();
    ASSERT_EQ(list.back(), 6);
    ASSERT_EQ(list.front(), 8);
}

TEST(correctness, reverse_iterator) {
    my::list<int> list{10, 9, 8, 7, 6};
    std::vector<int> v{6, 7, 8, 9, 10};
    assert_range_equality(list.rbegin(), list.rend(), v.begin(), v.end());
}

TEST(correctness, clear) {
    my::list<long> l{1, 65, 2, 3, 6, 2};
    l.clear();
    ASSERT_TRUE(l.empty());
    ASSERT_EQ(l.begin(), l.end());
}

TEST(correctness, insert) {
    my::list<int> l1{1, 2, 3, 4};
    my::list<int> l2;
    auto it = l2.insert(l2.begin(), 4);
    it = l2.insert(it, 3);
    l2.insert(l2.begin(), 1);
    l2.insert(it, 2);
    assert_range_equality(l1.begin(), l1.end(), l2.begin(), l2.end());
}

TEST(correctness, erase) {
    my::list<int> l1{2, 3, 4, 5, 6, 7};
    my::list<int> l2{4, 5, 6};
    auto it = l1.erase(l1.begin());
    l1.erase(it);
    l1.erase(std::prev(l1.end()));
    assert_range_equality(l1.begin(), l1.end(), l2.begin(), l2.end());
}

TEST(correctness, erase_range) {
    my::list<int> l1{1, 2, 3, 4, 5, 6, 7, 8, 9};
    my::list<int> l2{4, 5, 6};
    my::list<int> l3{7, 8, 9};
    auto it1 = l1.begin();
    ++it1, ++it1, ++it1;
    l1.erase(l1.begin(), it1);
    it1 = l1.begin();
    ++it1, ++it1, ++it1;
    assert_range_equality(l1.begin(), it1, l2.begin(), l2.end());
    l1.erase(l1.begin(), it1);
    assert_range_equality(l1.begin(), l1.end(), l3.begin(), l3.end());
    l1.erase(l1.begin(), l1.end());
    ASSERT_TRUE(l1.empty());
}

TEST(correctness, copy_const) {
    my::list<int> l;
    for (int i = 0; i < 22; i++) {
        l.push_back(i);
    }
    my::list<int> l2(l);
    while (!l2.empty()) {
        EXPECT_EQ(l2.back(), l.back());
        l2.pop_back();
        l.pop_back();
    }
    EXPECT_TRUE(l.empty());
}

TEST(correctness, assign) {
    my::list<int> l;
    for (int i = 0; i < 22; i++) {
        l.push_back(i);
    }
    my::list<int> l2 = l;
    while (!l2.empty()) {
        EXPECT_EQ(l2.back(), l.back());
        l2.pop_back();
        l.pop_back();
    }
    EXPECT_TRUE(l.empty());
}

TEST(correctness, assign1) {
    my::list<int> l, l2;
    for (int i = 0; i < 22; i++) {
        l.push_back(i);
    }
    l = l2;
    EXPECT_TRUE(l.empty());
    EXPECT_TRUE(l2.empty());
}

TEST(correctness, erase_multiple_1) {
    my::list<int> l;
    for (int i = 0; i < 10; i++) {
        l.push_back(i);
    }

    l.erase(l.begin(), l.end());
    EXPECT_EQ(true, l.empty());
}

TEST(correctness, erase_end) {
    my::list<int> l;
    auto i = l.end();
    l.push_back(42);
    --i;
    EXPECT_EQ(42, *i);
}

TEST(correctness, splice) {
    my::list<int> l1, l2;
    l1.push_back(1);
    l1.push_back(2);
    l1.push_back(3);
    l1.push_back(4);

    l2.push_back(5);
    l2.push_back(6);
    l2.push_back(7);
    l2.push_back(8);

    auto pos = l1.begin();
    ++pos;
    ++pos;

    auto first = l2.begin();
    ++first;
    auto second = first;
    ++second;
    ++second;

    std::vector<int> v1 = {1, 2, 6, 7, 3, 4};
    std::vector<int> v2 = {5, 8};

    l1.splice(pos, l2, first, second);

    while (!l1.empty()) {
        EXPECT_EQ(v1.back(), l1.back());
        v1.pop_back();
        l1.pop_back();
    }

    while (!l2.empty()) {
        EXPECT_EQ(v2.back(), l2.back());
        v2.pop_back();
        l2.pop_back();
    }
}

TEST(cirrectness, slice_only_and_empty) {
    my::list<int> x{1, 2}, y;
    y.splice(y.begin(), x, ++x.begin(), x.end());
    std::vector<int> a{2};
    assert_range_equality(y.begin(), y.end(), a.begin(), a.end());
    my::list<int> r{};
    y.clear();
    y.splice(y.begin(), r, r.begin(), r.end());
    assert_range_equality(y.begin(), y.end(), ++a.begin(), a.end());
}

TEST(correctness, slice_all) {
    my::list<int> l1, l2;
    l1.push_back(1);
    l1.push_back(2);
    l1.push_back(3);
    l1.push_back(4);

    l2.push_back(5);
    l2.push_back(6);
    l2.push_back(7);
    l2.push_back(8);
    l1.splice(l1.begin(), l2, l2.begin(), l2.end());
    EXPECT_TRUE(l2.empty());
}

TEST(correctness, pp) {
    my::list<int> l;
    for (int i = 0; i < 10; i++) {
        l.push_back(i);
    }

    for (int i = 9; i >= 0; i--) {
        EXPECT_EQ(i, l.back());
        l.pop_back();
    }

    EXPECT_EQ(true, l.empty());

    for (int i = 0; i < 10; i++) {
        l.push_back(i);
    }

    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(i, l.front());
        l.pop_front();
    }

    EXPECT_EQ(true, l.empty());
}

TEST(correctness, front) {
    my::list<int> l;
    for (int i = 0; i < 10; i++) {
        l.push_front(i);
    }

    for (int i = 9; i >= 0; i--) {
        EXPECT_EQ(i, l.front());
        l.pop_front();
    }

    EXPECT_EQ(true, l.empty());
}

TEST(correctness, swap) {
    my::list<int> o{1, 2, 3, 4}, t{1, 2, 3};
    if (true) {
        my::list<int> x{1, 2, 3};
        my::swap(o, x);
    }
    assert_range_equality(o.begin(), o.end(), t.begin(), t.end());
}

TEST(basic, push_pop_back) {
    my::list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    ASSERT_EQ(1, v.front());
    ASSERT_EQ(3, v.back());
    v.pop_back();
    ASSERT_EQ(2, v.back());
    v.pop_back();
    ASSERT_EQ(1, v.back());
    v.pop_back();
}

TEST(basic, push_pop_front) {
    my::list<int> v;
    v.push_front(2);
    v.push_front(3);
    v.push_back(1);
    ASSERT_EQ(3, v.front());
    ASSERT_EQ(1, v.back());
    v.pop_front();
    ASSERT_EQ(2, v.front());
}

TEST(basic, push_clear) {
    my::list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    ASSERT_FALSE(v.empty());
    v.clear();
    ASSERT_TRUE(v.empty());
}

// TEST(basic, initcopy) {
//    list<int> v;
//    v.push_back(1);
//    v.push_back(2);
//    v.push_back(3);
//
//    list<int> v2(v);
//    ASSERT_EQ(3, v2.back());
//    v.pop_back();
//    ASSERT_EQ(3, v2.back());
//    v.clear();
//    ASSERT_EQ(3, v2.back());
//}

TEST(iterators, begin_end) {
    my::list<int> v;
    v.push_back(1);
    ASSERT_EQ(1, *v.begin());
    ASSERT_EQ(1, *--v.end());
    v.push_back(2);
    v.push_back(3);
    ASSERT_EQ(1, *v.begin());
    ASSERT_EQ(3, *--v.end());
}

TEST(iterators, for_it) {
    my::list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    int i = 1;
    ASSERT_EQ(1, *v.begin());
    ASSERT_EQ(2, *++v.begin());
    ASSERT_EQ(3, *++++v.begin());
    for (auto it = v.begin(); it != v.end(); ++it) {  // NOLINT
        ASSERT_EQ(i, *it);
        i++;
    }
}

TEST(iterators, for_each) {
    my::list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    int i = 1;
    for (int it : v) {
        ASSERT_EQ(i, it);
        i++;
    }
}

TEST(iterators, rbegin_rend) {
    my::list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    ASSERT_EQ(3, *(v.rbegin()));
    ASSERT_EQ(1, *(--v.rend()));
}

TEST(iterators, insert) {
    my::list<int> v;
    v.push_back(1);
    v.push_back(2);

    v.insert(v.begin(), 0);
    ASSERT_EQ(0, *v.begin());

    v.insert(v.end(), 228);
    ASSERT_EQ(228, *(--v.end()));
}

TEST(iterators, insert_push) {
    my::list<int> v;
    v.insert(v.end(), 2);
    v.insert(v.begin(), 0);
    v.insert(++v.begin(), 1);

    ASSERT_EQ(0, *v.begin());
    ASSERT_EQ(1, *++v.begin());
    ASSERT_EQ(2, *++++v.begin());
}

void f() {
    my::list<int> const a;
    a.rbegin();
}

TEST(iterators, insert_push_pop) {
    my::list<int> v;
    v.insert(v.end(), 2);
    v.insert(v.begin(), 0);
    v.insert(++v.begin(), 1);

    auto it = v.end();

    v.pop_back();

    ASSERT_EQ(1, *(--v.end()));
    v.insert(it, 228);
    ASSERT_EQ(228, *(--v.end()));
}

TEST(iterators, const_iterator) {
    my::list<int> x{1, 2, 3, 4, 5};
    my::list<int>::const_iterator it(x.begin());
    it++;
    ASSERT_EQ(*it, 2);
}

TEST(iterators, const_iterator_cast) {
    my::list<int> x{1, 2, 3, 4, 5};
    my::list<int>::iterator z(x.begin());
    my::list<int>::const_iterator n(z);
    //(*n) = 4;
}

TEST(iterators, const_iterator_equality) {
    my::list<int> x{1, 2, 3, 4, 5};
    my::list<int>::iterator i(x.begin());
    my::list<int>::const_iterator j(x.begin());
    EXPECT_TRUE(i == j && j == i);
    EXPECT_FALSE(++i == j || j == ++i);
    i == i;
    j == j;
}

/*
int main(int ac, char **av) {
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}
*/
