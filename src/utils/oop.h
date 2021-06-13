#pragma once

#define FIRST_ARG(arg, ...) arg
#define CALL_METHOD(func, ...) (FIRST_ARG(__VA_ARGS__))->func(__VA_ARGS__)
