#ifndef TODOLIST_HPP
#define TODOLIST_HPP

#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <regex>
#include <exception>
#include <iomanip>
#include <fstream>

class TodoListTest;

class TodoList
{
public:
    TodoList(bool processFile = true);
    TodoList(const TodoList&) = delete;
    TodoList(const TodoList&&) = delete;
    ~TodoList();

    int run();

private:
    struct Task
    {
        std::string name;
        std::string description;
        struct Date {
            int year;
            int month;
            int day;
            int hour;
            int minute;

            Date& operator=(const std::string& str);
            bool operator>=(const std::string& str);
            bool operator<=(const std::string& str);
            bool operator==(const std::string& str);
            bool operator==(const std::string&& str);
            bool operator>(const std::string& str);
            bool operator<(const std::string& str);
            bool operator!=(const std::string& str);
            unsigned int stoui(const std::string& str);
            Date makeDate(const std::string& );

            friend std::ostream& operator<<(std::ostream& os, const TodoList::Task::Date& date)
            {
                os << date.year << '-' << date.month << '-' << date.day << ' '
                   << std::setfill('0') << std::setw(2) << date.hour << ':' 
                   << std::setfill('0') << std::setw(2) << date.minute;
                return os;
            }
        } date {0};
        std::string category;
        enum class Status { IN_PROGRESS = 0, DONE = 1 } status = Status::IN_PROGRESS;

        friend std::ostream& operator<<(std::ostream& os, const TodoList::Task& task)
        {
            os << '\"' << task.name << "\" \"" << task.description << "\" \""
               << task.date  << "\" \"" << task.category << "\" "
               << (task.status == Status::IN_PROGRESS ? "inprog" : "done") << std::endl;
            return os;
        }
    };

    struct Expression
    {
        enum class Field { NAME = 0, DESC, DATE, CATEGORY, STATUS } field {0};
        enum class Operation { G = 0, L, E, LE, GE, LIKE } operation {0};
        std::string operand;
    };

    void lexer(const std::string&);
    void lexer(const std::string&&);
    bool parseExpression(const size_t&, std::vector<Expression>& expressions);

    void addTask();
    void doneTask();
    void undoneTask();
    void updateTask();
    void deleteTask();
    auto selectTasks(unsigned = 0) -> std::vector<std::map<std::string, Task>::iterator>;

    void readFile();
    void writeToFile();

    void newInfo(Task&);
    void changeTaskStatus(Task::Status&&);
    void printTask(const Task&);
    void printTasks(const std::vector<std::map<std::string, Task>::iterator>&&);

    std::vector<std::string> parsedInstruction;
    std::map<std::string, Task> tasks;

    bool processFile = true;
    const std::string_view filename = "todolist.bin";

    friend TodoListTest;
};

#endif