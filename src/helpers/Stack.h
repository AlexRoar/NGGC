//
// Created by Александр Дремов on 24.11.2020.
//

#ifndef DIFFERENTIATOR_STACK_H
#define DIFFERENTIATOR_STACK_H

#include <cstdlib>

template<typename Elem>
class ClassicStack {
    struct StackIterator {
        ClassicStack *stack;
        int pos;
        bool reversed;

        bool operator==(const StackIterator& other) const {
            return stack == other.stack && pos == other.pos;
        }

        Elem *operator->() {
            return stack->data + pos;
        }

        Elem &operator*() {
            return stack->data[pos];
        }

        void operator++() {
            if (reversed)
                pos--;
            else
                pos++;
        }

        void operator++(int) {
            if (reversed)
                pos--;
            else
                pos++;
        }

        void operator--() {
            if (reversed)
                pos++;
            else
                pos--;
        }

        void operator--(int) {
            if (reversed)
                pos++;
            else
                pos--;
        }
    };

    size_t capacity;
    size_t size;
    Elem *data;

    void reallocate() {
        if (size + 1 < capacity)
            return;
        size_t newCapacity = 2 * capacity + 1;
        data = static_cast<Elem *>(realloc(data, newCapacity * sizeof(Elem)));
        capacity = newCapacity;
    }

public:
    static ClassicStack *New() {
        auto *thou = static_cast<ClassicStack *>(calloc(1, sizeof(ClassicStack)));
        thou->capacity = 0;
        thou->size = 0;
        return thou;
    }

    void init(size_t initVal) {
        this->capacity = initVal;
        this->size = 0;
        this->data = static_cast<Elem *>(calloc(initVal, sizeof(Elem)));
    }

    void dest() {
        for (size_t i = 0; i < size; i++)
            data[i].dest();
        if (data) {
            free(data);
            data = reinterpret_cast<Elem *>(0xBADF);
        }
    }

    static void Delete(ClassicStack *obj) {
        obj->dest();
        free(obj);
    }

    void push(Elem elem) {
        reallocate();
        this->data[size++] = elem;
    }

    Elem pop() {
        if (size == 0)
            return Elem();
        return this->data[--size];
    }

    Elem clear() {
        size = 0;
    }

    Elem *top() {
        if (size == 0)
            return nullptr;
        return &(this->data[size - 1]);
    }

    Elem &get(size_t ind) {
        return data[ind];
    }

    [[nodiscard]] Elem *getStorage() const {
        return data;
    }

    [[nodiscard]] size_t getSize() const {
        return size;
    }

    [[nodiscard]] bool isEmpty() const {
        return size == 0;
    }

    StackIterator begin(){
        return {this, 0, false};
    }

    StackIterator end(){
        return {this, static_cast<int>(size), false};
    }

    StackIterator rbegin(){
        return {this, static_cast<int>(size - 1), true};
    }

    StackIterator rend(){
        return {this, -1, true};
    }
};

#endif //DIFFERENTIATOR_STACK_H
