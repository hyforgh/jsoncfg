#include "jsonho.hpp"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#define PP_REMOVE_PARENS(T) PP_REMOVE_PARENS_IMPL T
#define PP_REMOVE_PARENS_IMPL(...) __VA_ARGS__
