// Copyright 2024 <Copyright Owner>

#include "train.h"

Train::Train() : countOp(0), first(nullptr) {}

Train::~Train() {
    if (first == nullptr) return;

    first->prev->next = nullptr;

    Car *current = first;
    Car *temp = nullptr;

    while (current != nullptr) {
        temp = current;
        current = current->next;
        delete temp;
    }
}

void Train::addCar(bool light) {
    Car *newCar = new Car();
    newCar->light = light;
    newCar->next = nullptr;
    newCar->prev = nullptr;

    if (first == nullptr) {
        first = newCar;
        newCar->next = newCar;
        newCar->prev = newCar;
    } else {
        newCar->next = first;
        newCar->prev = first->prev;
        first->prev->next = newCar;
        first->prev = newCar;
    }
}

int Train::getLength() {
    if (first == nullptr) return 0;

    countOp = 0;
    first->light = false;

    Car *cur = first;
    int len = 0;

    cur = cur->next;
    countOp++;
    len++;

    while (cur->light == true) {
        cur = cur->next;
        countOp++;
        len++;
    }

    cur->light = true;
    countOp++;
    Car *mark = cur;

    for (int i = 0; i < len; i++) {
        cur = cur->prev;
        countOp++;
    }

    if (cur->light == false) {
        return len;
    }

    int dist = len;

    while (true) {
        cur = mark->next;
        countOp++;
        int steps = 1;

        while (cur->light == true) {
            cur = cur->next;
            countOp++;
            steps++;
        }

        cur->light = true;
        countOp++;
        mark = cur;
        len += steps;

        for (int i = 0; i < len; i++) {
            cur = cur->prev;
            countOp++;
        }

        if (cur->light == false) {
            return len;
        }
    }
}

int Train::getOpCount() {
    return countOp;
}
