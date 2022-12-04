#ifndef TODOLISTTEST_HPP
#define TODOLISTTEST_HPP

#include <string>
#include <cassert>

#include "TodoList.hpp"

class TodoListTest
{
public:
    int runTests()
    {
        addTest();
        addInvalidDateTest();
        doneTest();
        undoneTest();
        doneSelectTest();
        undoneSelectTest();
        updateTest();
        deleteTest();
        basicSelectTest();
        optionSelectTest();

        return 0;
    }

private:
    void addTest()
    {
        testHeader("addTest");

        TodoList tl(false);

        tl.lexer("add buy  \"buy water\" \"2020-12-12 00:00\" \"shopping list\" done");
        tl.addTask();

        assert(tl.tasks.size() == 1);
        assert(tl.tasks.begin()->first == "buy");
        assert(tl.tasks.begin()->second.name == "buy");
        assert(tl.tasks.begin()->second.description == "buy water");
        assert(tl.tasks.begin()->second.date == "2020-12-12 00:00");
        assert(tl.tasks.begin()->second.category == "shopping list");
        assert(tl.tasks.begin()->second.status == TodoList::Task::Status::DONE);

        testFooter();
    }

    void addInvalidDateTest()
    {
        testHeader("addInvalidDateTest");

        TodoList tl(false);

        tl.lexer("add buy  \"buy water\" \"2020-12-12 00\" \"shopping list\" done");
        tl.addTask();

        assert(tl.tasks.size() == 1);
        assert(tl.tasks.begin()->first == "buy");
        assert(tl.tasks.begin()->second.name == "buy");
        assert(tl.tasks.begin()->second.description == "buy water");
        assert(tl.tasks.begin()->second.date == "0-0-0 00:00");
        assert(tl.tasks.begin()->second.category == "shopping list");
        assert(tl.tasks.begin()->second.status == TodoList::Task::Status::DONE);

        testFooter();
    }

    void updateTest()
    {
        testHeader("updateTest");

        TodoList tl(false);

        tl.lexer("add   buy \"buy water\"  \"2020-12-12 00:00\" \"shopping list\" done");
        tl.addTask();

        tl.parsedInstruction.clear();

        tl.lexer("add buy \"buy apples\" \"2020-12-12 00:00\" home inprog");
        tl.updateTask();

        assert(tl.tasks.size() == 1);
        assert(tl.tasks.begin()->first == "buy");
        assert(tl.tasks.begin()->second.name == "buy");
        assert(tl.tasks.begin()->second.description == "buy apples");
        assert(tl.tasks.begin()->second.date == "2020-12-12 00:00");
        assert(tl.tasks.begin()->second.category == "home");
        assert(tl.tasks.begin()->second.status == TodoList::Task::Status::IN_PROGRESS);

        testFooter();
    }

    void deleteTest()
    {
        testHeader("deleteTest");

        TodoList tl(false);

        tl.lexer("add buy  \"buy water\" \"2020-12-12 00:00\" \"shopping list\" done");
        tl.addTask();

        assert(tl.tasks.size() == 1);
        assert(tl.tasks.begin()->first == "buy");
        assert(tl.tasks.begin()->second.name == "buy");
        assert(tl.tasks.begin()->second.description == "buy water");
        assert(tl.tasks.begin()->second.date == "2020-12-12 00:00");
        assert(tl.tasks.begin()->second.category == "shopping list");
        assert(tl.tasks.begin()->second.status == TodoList::Task::Status::DONE);
 
        tl.parsedInstruction.clear();
        tl.lexer("delete buy");
        tl.deleteTask();

        assert(tl.tasks.size() == 0);

        testFooter();
    }

    void basicSelectTest()
    {
        testHeader("basicSelectTest");

        TodoList tl(false);
        std::string input;

        tl.lexer("add buy  \"buy water\" \"2020-12-12 00:00\" \"shopping list\" done");
        tl.addTask();

        tl.parsedInstruction.clear();
        tl.lexer("add \"Rolang interpreter\" \"make an interpreter for Rolang\" \"2022-12-12 12:12\" \"bucket list\" inprog");
        tl.addTask();

        tl.parsedInstruction.clear();
        tl.lexer("select *");
        const auto& selectedTasks = tl.selectTasks();

        assert(selectedTasks.size() == 2);
        assert(selectedTasks[0]->first == "Rolang interpreter");
        assert(selectedTasks[0]->second.name == "Rolang interpreter");
        assert(selectedTasks[0]->second.description == "make an interpreter for Rolang");
        assert(selectedTasks[0]->second.date == "2022-12-12 12:12");
        assert(selectedTasks[0]->second.category == "bucket list");
        assert(selectedTasks[0]->second.status == TodoList::Task::Status::IN_PROGRESS);

        assert(selectedTasks[1]->first == "buy");
        assert(selectedTasks[1]->second.name == "buy");
        assert(selectedTasks[1]->second.description == "buy water");
        assert(selectedTasks[1]->second.date == "2020-12-12 00:00");
        assert(selectedTasks[1]->second.category == "shopping list");
        assert(selectedTasks[1]->second.status == TodoList::Task::Status::DONE);

        testFooter();
    }

    void optionSelectTest()
    {
        testHeader("optionSelectTest");

        TodoList tl(false);
        std::string input;

        tl.lexer("add \"buy water\" \"buy water\" \"2020-12-12 00:00\" \"shopping list\" done");
        tl.addTask();

        tl.parsedInstruction.clear();
        tl.lexer("add \"buy sweets\" \"buy sweets\" \"2020-12-12 00:00\" \"shopping list\" inprog");
        tl.addTask();

        tl.parsedInstruction.clear();
        tl.lexer("add \"Rolang interpreter\" \"make an interpreter for Rolang\" \"2022-12-12 12:12\" \"bucket list\" inprog");
        tl.addTask();

        {
            tl.parsedInstruction.clear();
            tl.lexer("select * name = \"Rolang interpreter\"");
            const auto& selectedTasks = tl.selectTasks();

            assert(selectedTasks.size() == 1);
            assert(selectedTasks[0]->first == "Rolang interpreter");
            assert(selectedTasks[0]->second.name == "Rolang interpreter");
            assert(selectedTasks[0]->second.description == "make an interpreter for Rolang");
            assert(selectedTasks[0]->second.date == "2022-12-12 12:12");
            assert(selectedTasks[0]->second.category == "bucket list");
            assert(selectedTasks[0]->second.status == TodoList::Task::Status::IN_PROGRESS);
        }

        {
            tl.parsedInstruction.clear();
            tl.lexer("select * description like \"Rolang\"");
            const auto& selectedTasks = tl.selectTasks();

            assert(selectedTasks.size() == 1);
            assert(selectedTasks[0]->first == "Rolang interpreter");
            assert(selectedTasks[0]->second.name == "Rolang interpreter");
            assert(selectedTasks[0]->second.description == "make an interpreter for Rolang");
            assert(selectedTasks[0]->second.date == "2022-12-12 12:12");
            assert(selectedTasks[0]->second.category == "bucket list");
            assert(selectedTasks[0]->second.status == TodoList::Task::Status::IN_PROGRESS);
        }

        {
            tl.parsedInstruction.clear();
            tl.lexer("select * date > \"2020-12-12 01:12\"");
            const auto& selectedTasks = tl.selectTasks();

            assert(selectedTasks.size() == 1);
            assert(selectedTasks[0]->first == "Rolang interpreter");
            assert(selectedTasks[0]->second.name == "Rolang interpreter");
            assert(selectedTasks[0]->second.description == "make an interpreter for Rolang");
            assert(selectedTasks[0]->second.date == "2022-12-12 12:12");
            assert(selectedTasks[0]->second.category == "bucket list");
            assert(selectedTasks[0]->second.status == TodoList::Task::Status::IN_PROGRESS);
        }

        {
            tl.parsedInstruction.clear();
            tl.lexer("select * date > \"2020-12-12 01:12\" and name like \"Rolang\" and description like \"interpreter\"");
            const auto& selectedTasks = tl.selectTasks();

            assert(selectedTasks.size() == 1);
            assert(selectedTasks[0]->first == "Rolang interpreter");
            assert(selectedTasks[0]->second.name == "Rolang interpreter");
            assert(selectedTasks[0]->second.description == "make an interpreter for Rolang");
            assert(selectedTasks[0]->second.date == "2022-12-12 12:12");
            assert(selectedTasks[0]->second.category == "bucket list");
            assert(selectedTasks[0]->second.status == TodoList::Task::Status::IN_PROGRESS);
        }

        {
            tl.parsedInstruction.clear();
            tl.lexer("select * date < \"2020-12-12 00:00\"");
            const auto& selectedTasks = tl.selectTasks();

            assert(selectedTasks.size() == 0);
        }

        {
            tl.parsedInstruction.clear();
            tl.lexer("select * date > \"2020-1212 000\"");
            const auto& selectedTasks = tl.selectTasks();

            assert(selectedTasks.size() == 0);
        }

        {
            tl.parsedInstruction.clear();
            tl.lexer("select * description like buy");
            const auto& selectedTasks = tl.selectTasks();

            assert(selectedTasks.size() == 2);
            assert(selectedTasks[0]->second.description == "buy sweets");
            assert(selectedTasks[1]->second.description == "buy water");
        }

        testFooter();
    }

    void doneTest()
    {
        testHeader("doneTest");

        TodoList tl(false);

        tl.lexer("add buy  \"buy water\" \"2020-12-12 00:00\" \"shopping list\" inprog");
        tl.addTask();

        {
            tl.parsedInstruction.clear();
            tl.lexer("select * status = inprog");
            const auto& selectedTasks = tl.selectTasks();
            assert(selectedTasks.size() == 1);
            assert(selectedTasks[0]->second.name == "buy");
            assert(selectedTasks[0]->second.status == TodoList::Task::Status::IN_PROGRESS);
        }

        tl.parsedInstruction.clear();
        tl.lexer("done buy");
        tl.doneTask();

        {
            tl.parsedInstruction.clear();
            tl.lexer("select * status = done");
            const auto& selectedTasks = tl.selectTasks();
            assert(selectedTasks.size() == 1);
            assert(selectedTasks[0]->second.name == "buy");
            assert(selectedTasks[0]->second.status == TodoList::Task::Status::DONE);
        }

        testFooter();
    }

    void undoneTest()
    {
        testHeader("undoneTest");

        TodoList tl(false);

        tl.lexer("add buy  \"buy water\" \"2020-12-12 00:00\" \"shopping list\" done");
        tl.addTask();

        {
            tl.parsedInstruction.clear();
            tl.lexer("select * status = done");
            const auto& selectedTasks = tl.selectTasks();
            assert(selectedTasks.size() == 1);
            assert(selectedTasks[0]->second.name == "buy");
            assert(selectedTasks[0]->second.status == TodoList::Task::Status::DONE);
        }

        tl.parsedInstruction.clear();
        tl.lexer("undone buy");
        tl.undoneTask();

        {
            tl.parsedInstruction.clear();
            tl.lexer("select * status = inprog");
            const auto& selectedTasks = tl.selectTasks();
            assert(selectedTasks.size() == 1);
            assert(selectedTasks[0]->second.name == "buy");
            assert(selectedTasks[0]->second.status == TodoList::Task::Status::IN_PROGRESS);
        }

        testFooter();
    }

    void doneSelectTest()
    {
        testHeader("doneSelectTest");

        TodoList tl(false);

        tl.lexer("add buy  \"buy water\" \"2020-12-12 00:00\" \"shopping list\" inprog");
        tl.addTask();

        tl.parsedInstruction.clear();
        tl.lexer("done select * name = buy");
        tl.doneTask();

        {
            tl.parsedInstruction.clear();
            tl.lexer("select * status = done");
            const auto& selectedTasks = tl.selectTasks();
            assert(selectedTasks.size() == 1);
            assert(selectedTasks[0]->second.name == "buy");
            assert(selectedTasks[0]->second.status == TodoList::Task::Status::DONE);
        }

        testFooter();
    }

    void undoneSelectTest()
    {
        testHeader("undoneSelectTest");

        TodoList tl(false);

        tl.lexer("add buy  \"buy water\" \"2020-12-12 00:00\" \"shopping list\" done");
        tl.addTask();

        tl.parsedInstruction.clear();
        tl.lexer("undone select * name = buy");
        tl.undoneTask();

        {
            tl.parsedInstruction.clear();
            tl.lexer("select * status = inprog");
            const auto& selectedTasks = tl.selectTasks();
            assert(selectedTasks.size() == 1);
            assert(selectedTasks[0]->second.name == "buy");
            assert(selectedTasks[0]->second.status == TodoList::Task::Status::IN_PROGRESS);
        }

        testFooter();
    }
    

private:
    inline void testHeader(const char* name)
    {
        std::cout << "=======" << '\n' << name << '\n';
    }

    inline void testFooter()
    {
        std::cout << "PASSED\n";
    }
};

#endif