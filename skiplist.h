#ifndef __SKIPLIST_H
#define __SKIPLIST_H

#include <iostream>
#include <random>
#include <initializer_list>
#include <ctime>

template<typename T>
class skiplist {
public:
    using value_type = T;
    using size_type = int;
    
private:
    const static size_type MAX_LEVEL = 16;

public:
    struct node {
        node(value_type x, size_type l) : value(x), max_level(l) {
            for (size_type i=0; i<MAX_LEVEL; ++i) {
                forwards[i] = nullptr;
            }
        }
        value_type value;
        node* forwards[MAX_LEVEL];
        size_type max_level;
    };

public:
    skiplist() {
        levelCount = 1;
        listHead = new node(value_type(), 0);
    };
    skiplist(std::initializer_list<value_type> init) : skiplist() {
        for (const value_type& x : init) {
            insert(x);
        }
    }
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
        node* update[level];
        for (size_type i=0; i < level; ++i) {
            update[i] = listHead;
        }
        node* pNode = listHead;
        for (size_type i=level-1; i>=0; --i) {
            while ((nullptr != pNode->forwards[i]) && (pNode->forwards[i]->value < x)) {
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
    void printAll() {
        node* pNode = listHead;
        while (nullptr != pNode->forwards[0]) {
            std::cout << pNode->forwards[0]->value << ", ";
            pNode = pNode->forwards[0];
        }
    }
    void printAll(size_type l) {
        for(size_type i=MAX_LEVEL-1; i >=0; --i) {
            node* pNode = listHead;
            std::cout << "level: " << i << std::endl;
            if (l < 0 || (l >=0 && l == i)) {
                while (nullptr != pNode->forwards[i]) {
                    std::cout << pNode->forwards[i]->value << " ";
                    pNode = pNode->forwards[i];
                }
                std::cout << std::endl;
                if (l >= 0) {
                    break;
                }
            }
        }
    }
    size_type randomLevel() {
        size_type level = 1;
        for (size_type i=1; i < MAX_LEVEL; ++i) {
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