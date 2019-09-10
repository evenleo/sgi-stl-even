#ifndef __SKIPLIST_H
#define __SKIPLIST_H

#include <iostream>
#include <random>
#include <time.h>

template<typename T>
class skiplist {
public:
    using value_type = T;
    using pointer = T*;
    using size_type = size_t;
    
private:
    const static size_type MAX_LEVEL = 16;

public:
    struct node {
        node(value_type x, size_type lv) : value(x), max_level(lv) {}
        value_type value;
        pointer forwards[MAX_LEVEL] = { nullptr };
        size_type max_level;
    };

public:
    skiplist() {
        levelCount = 1;
        listHead = new node(value_type(), 0);
    };
    ~skiplist() {
        delete listHead;
    }
    node* find(const value_type& x) {
        node* pNode = listHead;
        for (int i = levelCount-1; i>=0; --i) {
            while (nullptr != pNode->forwards[i] && pNode->forwards[i]->value < x) {
                pNode = pNode->forwards[i];
            }
        }
        if (nullptr != pNode->forwards[0] && pNode->forwards[0]->value == x)
            return pNode->forwards[0];
        return nullptr;
    }
    void insert(const value_type& x) {
        size_type level = randomLevel();
        node* newNode = new node(x, level);
        node* update[levelCount];
        node* pNode = listHead;
        for (size_type i=levelCount-1; i>=0; --i) {
            while (nullptr != pNode->forwards[i] && pNode->forwards[i]->value < x) {
                pNode = pNode->forwards[i];
            }
            update[i] = pNode;
        }
        for (size_type i=0; i<level; ++i) {
            newNode->forwards[i] = pNode->forwards[i];
            pNode->forwards[i] = newNode;
        }
        
        if (levelCount < level) levelCount = level;
    }
    void erase(const value_type& x) {
        node* update[levelCount];
        node* pNode = listHead;
        for (size_type i=levelCount-1; i>=0; --i) {
            while (nullptr != pNode->forwards[i] && pNode->forwards[i]->value < x) {
                pNode = pNode->forwards[i];
            }
            update[i] = pNode;
        }
        if (nullptr != pNode->forwards[0] && pNode->forwards[0]->value == x) {
            for (size_type i=levelCount-1; i>=0; --i) {
                if (nullptr != update[i]->forwards[i] && x == update->forwards[i]->value) {
                    update[i]->forwards[i] = update->forwards[i]->forwards[i];
                }
            }
        }
        while (levelCount > 1 && listHead->forwards[levelCount] == nullptr) {
            levelCount--;
        }
    }
    void pintAll();
    void printLevel(size_type lv);
    size_type randomLevel() {
        size_type level = 1;
        for (int i=1; i < MAX_LEVEL; ++i) {
            if (getRandom() % 3 == 1) {
                level++;
            }
        }
        return level;
    }

    void test() {
        for (int i=0; i<10; ++i) {
            std::cout << randomLevel() << std::endl;
        }
    }

private:
    size_type getRandom() {
        static size_type count = 1;
        std::default_random_engine generator(time(0) + count);
        std::uniform_int_distribution<size_type> distribution(1, 99999);
        count += 100;
        return distribution(generator);
    }
private:
    size_type levelCount;
    node* listHead;
};

#endif