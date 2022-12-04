#include <iostream>
#include "TodoListTest.hpp"
#include "TodoList.hpp"

int main()
{
#ifndef NDEBUG
    TodoListTest todoTest;
    return todoTest.runTests();
#else
    TodoList todo;
    return todo.run();
#endif
}
