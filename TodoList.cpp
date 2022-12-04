#include "TodoList.hpp"

#define INVALID_COMMAND(commands) { std::cerr << "error: \'"; for (size_t internal_i = 0; internal_i < commands.size(); ++internal_i) { std::cerr << commands.at(internal_i); if (internal_i + 1 < commands.size()) std::cerr << ' '; } std::cerr << "\' is not a valid command" << std::endl; }
#define INVALID_COMMAND_RET(commands) { INVALID_COMMAND(commands); return; }
#define INVALID_COMMAND_RET_VAL(commands, ret) { INVALID_COMMAND(commands); return (ret); }
#define INVALID_DATA(data) { std::cerr << "error: \'" << data << "\' is invalid data" << std::endl; }
#define INVALID_DATA_RET(data) { INVALID_DATA(data); return; }
#define INVALID_DATA_RET_VAL(data, ret) { INVALID_DATA(data); return (ret); }

TodoList::TodoList(bool processFile) : processFile(processFile)
{
    if (processFile)
        readFile();
}

TodoList::~TodoList()
{
    if (processFile)
        writeToFile();
}

int TodoList::run()
{
    std::string input;

    while(true)
    {
        parsedInstruction.clear();

        std::getline(std::cin, input);

        lexer(input);
        
        if (!parsedInstruction.empty()) {
            if (parsedInstruction.front() == "add")
                addTask();
            else if (parsedInstruction.front() == "done")
                doneTask();
            else if (parsedInstruction.front() == "undone")
                undoneTask();
            else if (parsedInstruction.front() == "update")
                updateTask();
            else if (parsedInstruction.front() == "delete")
                deleteTask();
            else if (parsedInstruction.front() == "select")
                printTasks(selectTasks());
            else if (parsedInstruction.front() == "quit")
                break;
            else {
                INVALID_COMMAND(parsedInstruction);
            }
        } 
    }

    return 0;
}

void TodoList::lexer(const std::string& str)
{
    char token = ' ';
    size_t cur = 0, from = 0;

    std::regex regex(R"(\"([^\"]*)\"|(\S+))");
    std::sregex_iterator sregIter(std::begin(str), std::end(str), regex);
    std::sregex_iterator sregIterEnd;

    for(; sregIter != sregIterEnd; ++sregIter) {
        if (sregIter->str()[0] == '\"')
            parsedInstruction.push_back(sregIter->str().substr(1, sregIter->str().size() - 2));
        else 
            parsedInstruction.push_back(sregIter->str());
    }
}

void TodoList::lexer(const std::string&& str)
{
    lexer(str);
}

void TodoList::newInfo(Task& task)
{
    if (parsedInstruction.size() < 5 || parsedInstruction.size() > 6) {
        INVALID_COMMAND_RET(parsedInstruction);
    }

    task.name = parsedInstruction[1];
    task.description = parsedInstruction[2];
    task.date = parsedInstruction[3];
    task.category = parsedInstruction[4];
    
    if (parsedInstruction.size() == 6)
        task.status = parsedInstruction[5] == "inprog" ? Task::Status::IN_PROGRESS : Task::Status::DONE;
}

void TodoList::addTask()
{
    Task newTask;
    newInfo(newTask);
    tasks.insert({newTask.name, newTask});
}

void TodoList::changeTaskStatus(Task::Status&& status)
{
    if (parsedInstruction.size() < 2) {
        INVALID_COMMAND_RET(parsedInstruction);
    } else if (parsedInstruction.size() == 2) {
        auto&& task = tasks.find(parsedInstruction[1]);
        if (task == tasks.end()) {
            INVALID_DATA_RET(parsedInstruction[1]);
        }
        task->second.status = status;
    } else if (parsedInstruction[1] == "select") {
        auto&& selectedTasks = selectTasks(1);
        for (auto i : selectedTasks)
            i->second.status = status;
    } 
}

void TodoList::undoneTask()
{
    changeTaskStatus(Task::Status::IN_PROGRESS);
}

void TodoList::doneTask()
{
    changeTaskStatus(Task::Status::DONE);
}

void TodoList::updateTask()
{
    if (parsedInstruction.size() != 6) {
        INVALID_COMMAND_RET(parsedInstruction);
    }

    auto taskIter = tasks.find(parsedInstruction[1]);

    if (taskIter == tasks.end()) {
        INVALID_COMMAND_RET(parsedInstruction[1]);
    }

    newInfo(taskIter->second);
}

void TodoList::deleteTask()
{
    if (parsedInstruction.size() != 2) {
        INVALID_COMMAND_RET(parsedInstruction);
    }

    tasks.erase(parsedInstruction[1]);
}

auto TodoList::selectTasks(unsigned startFrom) -> std::vector<std::map<std::string, Task>::iterator>
{
    std::vector<std::map<std::string, Task>::iterator> iterators;

    if (parsedInstruction.size() - startFrom < 2) {
        INVALID_COMMAND_RET_VAL(parsedInstruction, iterators);
    } else if (parsedInstruction[startFrom + 1] != "*") {
        INVALID_COMMAND_RET_VAL(parsedInstruction, iterators);
    }

    std::vector<Expression> expressions;
    bool isProcessed = true;

    auto checkStr = [parsedInstruction = this->parsedInstruction](const Expression& expr, const std::string& field) -> bool const
    {
        if (expr.operation == Expression::Operation::E)
            return field == expr.operand;
        else if (expr.operation == Expression::Operation::LIKE)
            return field.find(expr.operand) != field.npos;
        else if ((expr.operation != Expression::Operation::E) && (expr.operation != Expression::Operation::LIKE)) {
            INVALID_COMMAND(parsedInstruction);
        }
        return false;
    };

    iterators.reserve(tasks.size());

    for (size_t i = startFrom + 2; i < parsedInstruction.size(); ++i)
    {
        if (i + 2 >= parsedInstruction.size()) {
            INVALID_COMMAND_RET_VAL(parsedInstruction, iterators);
        }

        if (parsedInstruction[i] == "and")
            continue;

        isProcessed = parseExpression(i, expressions);

        if (!isProcessed) {
            INVALID_COMMAND_RET_VAL(parsedInstruction, iterators);
        }

        i += 2;
    }

    for (auto task = tasks.begin(); task != tasks.end(); ++task)
    {
        isProcessed = true;

        for (auto& expr : expressions)
        {
            if (expr.field == Expression::Field::NAME)
                isProcessed = checkStr(expr, task->second.name);
            else if (expr.field == Expression::Field::DESC)
                isProcessed = checkStr(expr, task->second.description);
            else if (expr.field == Expression::Field::CATEGORY)
                isProcessed = checkStr(expr, task->second.category);
            else if (expr.field == Expression::Field::DATE)
            {
                isProcessed = false;
                if (expr.operation == Expression::Operation::E && task->second.date == expr.operand)
                    isProcessed = true;
                else if (expr.operation == Expression::Operation::G && task->second.date > expr.operand)
                    isProcessed = true;
                else if (expr.operation == Expression::Operation::GE && task->second.date >= expr.operand)
                    isProcessed = true;
                else if (expr.operation == Expression::Operation::LE && task->second.date <= expr.operand)
                    isProcessed = true;
                else if (expr.operation == Expression::Operation::L && task->second.date < expr.operand)
                    isProcessed = true;
            }
            else if (expr.field == Expression::Field::STATUS)
            {
                isProcessed = false;

                if (expr.operation == Expression::Operation::E)
                {
                    if (expr.operand == "inprog")
                        isProcessed = task->second.status == Task::Status::IN_PROGRESS;
                    else if (expr.operand == "done")
                        isProcessed = task->second.status == Task::Status::DONE;
                }
            }

            if (!isProcessed)
                break;
        }

        if (isProcessed)
            iterators.push_back(task);
    }

    return iterators;
}

bool TodoList::parseExpression(const size_t& i, std::vector<Expression>& expressions) 
{
    Expression expr;

    assert(i + 2 < parsedInstruction.size());

    if (parsedInstruction[i] == "name")
        expr.field = Expression::Field::NAME;
    else if (parsedInstruction[i] == "description")
        expr.field = Expression::Field::DESC;
    else if (parsedInstruction[i] == "date")
        expr.field = Expression::Field::DATE;
    else if (parsedInstruction[i] == "caregory")
        expr.field = Expression::Field::CATEGORY;
    else if (parsedInstruction[i] == "status")
        expr.field = Expression::Field::STATUS;
    else {
        INVALID_COMMAND_RET_VAL(parsedInstruction, false);
    }

    if (parsedInstruction[i + 1] == ">")
        expr.operation = Expression::Operation::G;
    else if (parsedInstruction[i + 1] == "<")
        expr.operation = Expression::Operation::L;
    else if (parsedInstruction[i + 1] == "=")
        expr.operation = Expression::Operation::E;
    else if (parsedInstruction[i + 1] == "<=")
        expr.operation = Expression::Operation::LE;
    else if (parsedInstruction[i + 1] == ">=")
        expr.operation = Expression::Operation::GE;
    else if (parsedInstruction[i + 1] == "like")
        expr.operation = Expression::Operation::LIKE;
    else {
        INVALID_COMMAND_RET_VAL(parsedInstruction, false);
    }

    expr.operand = parsedInstruction[i + 2];

    expressions.push_back(std::move(expr));

    return true;
}

void TodoList::printTask(const Task& task)
{
    std::cout << '[' << (task.status == Task::Status::DONE ? 'x' : ' ') << ']' << ' ' << task.name << ": " << task.description << ' ' << task.category << ' ' << task.date << std::endl;
}

void TodoList::printTasks(const std::vector<std::map<std::string, Task>::iterator>&& iterators)
{
    for (auto& taskIter : iterators)
        printTask(taskIter->second);
}

void TodoList::readFile()
{
    std::ifstream readStream(filename.data());

    if (readStream.is_open())
    {
        for(std::string line; std::getline(readStream, line);)
        {
            lexer("add " + line);
            addTask();
            parsedInstruction.clear();
        }
    }

    readStream.close();
}

void TodoList::writeToFile()
{
    std::ofstream writeStream(filename.data());

    if (writeStream.is_open())
        for (const auto& task : tasks)
            writeStream << task.second;
        

    writeStream.close();
}

unsigned int TodoList::Task::Date::stoui(const std::string& str)
{
    unsigned long lresult = std::stoul(str);
    unsigned int result = lresult;
    if (result != lresult) throw std::out_of_range("");
    return result;
}

TodoList::Task::Date TodoList::Task::Date::makeDate(const std::string& str)
{
    Date newDate;

    std::regex word_regex("\\d+");
    std::sregex_token_iterator iter(str.begin(), str.end(), word_regex);
    std::sregex_token_iterator iterEnd;

    if (std::distance(iter, iterEnd) != 5) {
        throw std::invalid_argument("");
    }

    newDate.year = stoui((iter++)->str());
    newDate.month = stoui((iter++)->str());
    newDate.day = stoui((iter++)->str());
    newDate.hour = stoui((iter++)->str());
    newDate.minute = stoui((iter++)->str());

    return newDate;
}

TodoList::Task::Date& TodoList::Task::Date::operator=(const std::string& str)
{
    Date newDate;
    try {
        newDate = std::move(makeDate(str));
    } catch(const std::out_of_range& e) {
        INVALID_DATA_RET_VAL(str, *this);
    } catch(const std::invalid_argument& e) {
        INVALID_DATA_RET_VAL(str, *this);
    }

    this->year = newDate.year;
    this->month = newDate.month;
    this->day = newDate.day;
    this->hour = newDate.hour;
    this->minute = newDate.minute;

    return *this;
}

bool TodoList::Task::Date::operator>=(const std::string& str) {
    Date newDate;

    try {
        newDate = std::move(makeDate(str));
    } catch(const std::out_of_range& e) {
        INVALID_DATA_RET_VAL(str, false);
    } catch(const std::invalid_argument& e) {
        INVALID_DATA_RET_VAL(str, false);
    }

    if (this->year >= newDate.year)
        return true;
    else if (this->month >= newDate.month)
        return true;
    else if (this->day >= newDate.day)
        return true;
    else if (this->hour >= newDate.hour)
        return true;
    else if (this->minute >= newDate.minute)
        return true;
    return false;
}

bool TodoList::Task::Date::operator<=(const std::string& str) 
{
    Date newDate;

    try {
        newDate = std::move(makeDate(str));
    } catch(const std::out_of_range& e) {
        INVALID_DATA_RET_VAL(str, false);
    } catch(const std::invalid_argument& e) {
        INVALID_DATA_RET_VAL(str, false);
    }

    if (this->year <= newDate.year)
        return true;
    else if (this->month <= newDate.month)
        return true;
    else if (this->day <= newDate.day)
        return true;
    else if (this->hour <= newDate.hour)
        return true;
    else if (this->minute <= newDate.minute)
        return true;
    return false;
}

bool TodoList::Task::Date::operator==(const std::string& str) 
{
    Date newDate;

    try {
        newDate = std::move(makeDate(str));
    } catch(const std::out_of_range& e) {
        INVALID_DATA_RET_VAL(str, false);
    } catch(const std::invalid_argument& e) {
        INVALID_DATA_RET_VAL(str, false);
    }

    if ((this->year == newDate.year) && (this->month == newDate.month) && (this->day == newDate.day) &&
        (this->hour == newDate.hour) && (this->minute == newDate.minute))
        return true;
    return false;
}

bool TodoList::Task::Date::operator==(const std::string&& str) 
{
    return operator==(str);
}

bool TodoList::Task::Date::operator>(const std::string& str) 
{
    Date newDate;

    try {
        newDate = std::move(makeDate(str));
    } catch(const std::out_of_range& e) {
        INVALID_DATA_RET_VAL(str, false);
    } catch(const std::invalid_argument& e) {
        INVALID_DATA_RET_VAL(str, false);
    }

    if (this->year > newDate.year)
        return true;
    else if (this->month > newDate.month)
        return true;
    else if (this->day > newDate.day)
        return true;
    else if (this->hour > newDate.hour)
        return true;
    else if (this->minute > newDate.minute)
        return true;
    return false;
}

bool TodoList::Task::Date::operator<(const std::string& str) 
{
    Date newDate;

    try {
        newDate = std::move(makeDate(str));
    } catch(const std::out_of_range& e) {
        INVALID_DATA_RET_VAL(str, false);
    } catch(const std::invalid_argument& e) {
        INVALID_DATA_RET_VAL(str, false);
    }

    if (this->year < newDate.year)
        return true;
    else if (this->month < newDate.month)
        return true;
    else if (this->day < newDate.day)
        return true;
    else if (this->hour < newDate.hour)
        return true;
    else if (this->minute < newDate.minute)
        return true;
    return false;
}

bool TodoList::Task::Date::operator!=(const std::string& str)
{
    Date newDate;

    try {
        newDate = std::move(makeDate(str));
    } catch(const std::out_of_range& e) {
        INVALID_DATA_RET_VAL(str, false);
    } catch(const std::invalid_argument& e) {
        INVALID_DATA_RET_VAL(str, false);
    }

    if (this->year != newDate.year)
        return true;
    else if (this->month != newDate.month)
        return true;
    else if (this->day != newDate.day)
        return true;
    else if (this->hour != newDate.hour)
        return true;
    else if (this->minute != newDate.minute)
        return true;
    return false;
}