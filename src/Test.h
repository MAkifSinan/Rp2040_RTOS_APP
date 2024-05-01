#ifndef TEST_H
#define TEST_H
#include <stdio.h>

#pragma once

class Test
{
public:
    Test(uint8_t pin_number, uint8_t led_number);
    void print();
    ~Test();

private:

};

#endif