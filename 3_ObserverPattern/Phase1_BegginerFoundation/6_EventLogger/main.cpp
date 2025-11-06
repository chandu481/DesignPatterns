#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>

enum class Level {Info, Warn, Error};

class IFormater
{
public :
    virtual std::string format(const Level& level, const std::string& msg) = 0;
    virtual ~IFormater() = default;
};

std::string toStringLevel(const Level& level)
{
    switch(level)
    {
        case Level::Error : return "[Error]"; 
        case Level::Info :return  "[Info]";
        case Level::Warn : return "[Warn]";
    }

    return "?";
}

class SimpleFormater : public IFormater
{
public :
    std::string format(const Level& level, const std::string& msg) override 
    {
        std::string frm = toStringLevel(level) + " " + msg;
        return frm;
    }
};

//step1 : Observer interface
class ISinksObserver
{
protected:
    Level subscribedLevel_ = Level::Info;
public :
    std::string name;
    ISinksObserver(std::string n) : name(n) {}
    virtual void onEvent(const Level& level, std::string msg) = 0;
    void subscribeLevel(const Level& level)
    {
        subscribedLevel_ = level;
    }
    virtual ~ISinksObserver() = default;
};

//step2 : Subject Interfaces
class IEventSubject
{
public :
    virtual void attach(ISinksObserver* obs) = 0;
    virtual void detach(ISinksObserver* obs) = 0;
    virtual void publish(Level level, std::string msg) = 0;
    virtual void notify() = 0;
    virtual void removeAll() = 0;
    virtual ~IEventSubject() = default;
};

// step 3 : concerete Subject 
class EventBus : public IEventSubject
{
    std::unordered_map<std::string, ISinksObserver*> observers;
    Level level;
    std::string message;
public :
    

    void attach(ISinksObserver* obs) override 
    {
        observers[obs->name] = obs;
    }

    void detach(ISinksObserver* obs) override 
    {
        if(observers.find(obs->name) == observers.end())
        {
            std::cout<<obs->name<<" not found "<<std::endl;
            return ;
        }

        observers.erase(obs->name);
    }

    void removeAll() override 
    {
        observers.clear();
    }

    void publish(Level l, std::string msg) override 
    {
        level = l;
        message = msg;
        notify();
    }

    void notify()
    {
        std::vector<std::pair<std::string, ISinksObserver*>> snapshot(observers.begin(), observers.end());
        for(std::pair<std::string, ISinksObserver*> obs : snapshot)
        {
            obs.second->onEvent(level, message);
        }
    }
};

// step 4 : concrete observers
class ConsoleSink : public ISinksObserver
{
    IFormater* formater;
public :
    ConsoleSink(std::string name) : ISinksObserver(name) {}
    void onEvent(const Level& level, const std::string msg) override 
    {
            std::cout<<formater->format(level, msg)<<std::endl;
    }

    void setFormator(IFormater* formt) 
    {
        formater = formt;
    }
};

class FileSink : public ISinksObserver
{
    std::vector<std::string> file;
public :
    FileSink(std::string name) : ISinksObserver(name) {} 
    void onEvent(const Level& level, const std::string msg) override
    {
        std::string str = std::to_string((int)level) + " " +  msg;
        file.push_back(str);
    }
};

class RingBufferSink : public ISinksObserver
{
    std::queue<std::string> memory;
    int n = 3;
public :
    RingBufferSink(std::string name) : ISinksObserver(name) {}
    void onEvent(const Level& level, const std::string msg)
    {
        std::string str = std::to_string((int)level) + " " + msg;
        memory.push(str);
        if(memory.size() > n)
            memory.pop();
    }
};

int main()
{
    EventBus eventBus;

    ConsoleSink consoleSink("console");
    SimpleFormater simpleFormater;
    consoleSink.setFormator(&simpleFormater);
    FileSink fileSink("fileSink");
    RingBufferSink ringBufferSink("ringBuffer");

    eventBus.attach(&consoleSink);
    eventBus.attach(&fileSink);
    eventBus.attach(&ringBufferSink);

    eventBus.publish(Level::Info, "Start");
    eventBus.publish(Level::Warn, "low battery");
    eventBus.publish(Level::Error, "disk full");
    eventBus.publish(Level::Info, "retrying");

    eventBus.detach(&consoleSink);

    eventBus.publish(Level::Error, "still failing");

    eventBus.removeAll();
    eventBus.publish(Level::Info, "silent");
    return 0;
}