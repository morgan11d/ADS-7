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

    Car *car = first;
    car->light = false;

    car = car->next;
    countOp++;
    int len = 1;

    while (car->light == true) {
        car = car->next;
        countOp++;
        len++;
    }

    car->light = true;

    int back = len;
    while (back > 0) {
        car = car->prev;
        countOp++;
        back--;
    }

    if (car->light == false) {
        return len;
    }

    Car *mark = car;
    for (int i = 0; i < len; i++) {
        mark = mark->next;
        countOp++;
    }

    while (true) {
        car = mark;
        int steps = 0;

        do {
            car = car->next;
            countOp++;
            steps++;
        } while (car->light == true);

        car->light = true;
        mark = car;
        len += steps;

        back = len;
        while (back > 0) {
            car = car->prev;
            countOp++;
            back--;
        }

        if (car->light == false) {
            return len;
        }
    }
}

int Train::getOpCount() {
    return countOp;
}
