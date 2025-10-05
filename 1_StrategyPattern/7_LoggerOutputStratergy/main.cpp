#include <iostream>
#include <memory>
#include <fstream>

// Step1 : Stratergy Interface
class LogStratergy
{
public :
    virtual void log(const std::string& message) = 0;
    virtual ~LogStratergy() = default;
};

// Step2 : Concrete Stratergy
class ConsoleLogger : public LogStratergy
{
public :
    void log(const std::string& message) override 
    {
        std::cout<<"[Console] "<<message<<std::endl;
    }
};

class FileLogger : public LogStratergy
{
public :
    void log(const std::string& message) override 
    {
        std::ofstream file("log.txt", std::ios::app);

        file << "[File] " << message << "\n";
    }
};

// STEP 3 : Context
class Logger
{
    std::unique_ptr<LogStratergy> logStratergy;
public :
    void setLoggerStratergy(std::unique_ptr<LogStratergy> s)
    {
        logStratergy = std::move(s);
    }

    void writeLog(const std::string& msg)
    {
        if(logStratergy)
        {
            logStratergy->log(msg);
        }
        else 
        {
            std::cout<<"logStratergy has not set"<<std::endl;
        }
    }
};

int main()
{
    Logger log;

    log.setLoggerStratergy(std::make_unique<ConsoleLogger>());
    log.writeLog("chandu");

    log.setLoggerStratergy(std::make_unique<FileLogger>());
    log.writeLog("dhana");

    return 0;
}