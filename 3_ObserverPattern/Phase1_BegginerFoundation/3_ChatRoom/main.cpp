#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <memory>

// step 1 : observer interface
class IObserver
{
    std::string name_;
public :
    IObserver(std::string n) : name_(n) {}
    virtual void onMessage(std::string& from, const std::string& text) = 0;
    virtual ~IObserver() = default;

    std::string& getName() { return name_;}
};

// step 2 : subject interface
class ISubject 
{
public :
    virtual ~ISubject() = default;

    virtual void join(IObserver* obs) = 0;
    virtual void leave(IObserver* obs) = 0;
    virtual void removeAll() = 0;

    virtual void sendMsg(IObserver& sender, const std::string& msg) = 0;
    virtual void sendPrivateMsg(IObserver& from, const std::string& toName, const std::string& msg) = 0;
    virtual void mute(IObserver& sender, const std::string& receiver) = 0;
};

// step 3 : conceret subject 
class ChatRoom : public ISubject
{
    std::unordered_map<std::string, IObserver*> observers_;
    std::unordered_map<std::string, std::string> muteUsersMap_;
public :
    void join(IObserver* obs) override 
    {
        if(observers_.find(obs->getName()) != observers_.end())
        {
            std::cout<<obs->getName()<< " already a user in this chatroom"<<std::endl;
            return ;
        }

        observers_[obs->getName()] = obs;
    }

    void leave(IObserver* obs) override 
    {
        if(observers_.find(obs->getName()) == observers_.end())
        {
            std::cout<<obs->getName()<<" is not a user in this chatroom"<<std::endl;
            return;
        }

        observers_.erase(obs->getName());
    }

    void removeAll() override 
    {
        observers_.clear();
        std::cout<<"All users are cleared from this chatRoom"<<std::endl;
    }

    bool checkMute(const std::string& user1, const std::string& user2)
    {
        if(muteUsersMap_.find(user1) != muteUsersMap_.end())
        {
            if(muteUsersMap_[user1] == user2)
            {
                std::cout<<user1<<" has mute "<<user2<<" [can't able to send msg]"<<std::endl;
                return true;
            }
        }
        return false;
    }

    void sendMsg(IObserver& sender, const std::string& msg) override 
    {
        
        if(observers_.find(sender.getName()) == observers_.end())
        {
            std::cout<<sender.getName()<<" not in this chatroom"<<std::endl;
            return;
        }
        
        std::unordered_map<std::string, IObserver*> snapshot = observers_;
        for(std::pair<std::string, IObserver*> obsPair : snapshot)
        {
            
            if((obsPair.first != sender.getName()) && (checkMute(sender.getName(), obsPair.first) == false))
            {
                obsPair.second->onMessage(sender.getName(), msg);
            }
        }
        
        std::cout<<"-------------------------------------------"<<std::endl;
    }

    void sendPrivateMsg(IObserver& from, const std::string& toName, const std::string& msg) override
    {
        if(observers_.find(from.getName()) == observers_.end())
        {
            std::cout<<from.getName()<<" has not joined in this chatRoom"<<std::endl;
            return;
        }

        if(observers_.find(toName) == observers_.end())
        {
            std::cout<<toName<<" has not joined in this chatRoom"<<std::endl;
            return;
        }
        
        if(checkMute(from.getName(), toName) == true)
        {
            return;
        }
        IObserver* receiver = observers_[toName];
        receiver->onMessage(from.getName(), msg);
    }

    void mute(IObserver& sender, const std::string& receiver) override 
    {
        if(observers_.find(sender.getName()) == observers_.end())
        {
            std::cout<<sender.getName()<<" is not member of this chatRoom"<<std::endl;
            return;
        }

        if(observers_.find(receiver) == observers_.end())
        {
            std::cout<<receiver<<" is not member of this chatRoom"<<std::endl;
            return ;
        }

        muteUsersMap_[sender.getName()] = receiver;
    }
};

//step 4 : concerete observers
class User : public IObserver
{
public :
    User(std::string name) : IObserver(name) {}

    void onMessage(std::string& from, const std::string& text) override 
    {
        std::cout<<"[user : "<<getName()<<"] [Received msg : "<< text <<"]  [From : "<<from<<"]"<<std::endl;
    }
};

// added one more feature logger to log the onMessage
class ILogSink 
{
public :
    virtual void write(const std::string& line) = 0;
    virtual ~ILogSink() = default;
};

class IMessageFormatter 
{
public :
    virtual ~IMessageFormatter() = default;
    virtual std::string format(const std::string& from, const std::string& text) = 0;
};

class ConsoleSink : public ILogSink 
{
public :
    void write(const std::string& line) override 
    {
        std::cout<<line<<std::endl;
    }
};

class FileSink : public ILogSink 
{
    std::ofstream out_;
public :
    FileSink(std::string path) : out_(path, std::ios::app) {}
    void write(const std::string& line) override 
    {
        if(out_)
            out_ << line << "\n";
    }
};

class SimpleFormatter : public IMessageFormatter
{
public :
    std::string format(const std::string& from, const std::string& text) override 
    {
        return "[ROOM] [From : " + from + "] msg: " + text;
    }
};

class LoggerObserver : public IObserver
{
    std::shared_ptr<ILogSink> sink_;
    std::shared_ptr<IMessageFormatter> format_;
public :
    LoggerObserver(std::string name, std::shared_ptr<ILogSink> sink, std::shared_ptr<IMessageFormatter> format) : 
        IObserver(name), sink_(std::move(sink)), format_(std::move(format))
    {
    }

    void onMessage(std::string& from, const std::string& text) override 
    {
        std::string line = format_->format(from, text);
        sink_->write(line);
    }
};

int main()
{
    ChatRoom chatRoom;

    User u1("Chandu");
    User u2("Dhana");
    User u3("Charan");

    //auto sink = std::make_shared<ConsoleSink>();
    auto sink = std::make_shared<FileSink>("log.txt");
    auto format = std::make_shared<SimpleFormatter>();
    LoggerObserver roomLogger("RoomLogger", sink, format );
    
    chatRoom.join(&u1);
    chatRoom.join(&u2);
    chatRoom.join(&u3);
    chatRoom.join(&roomLogger);
    

    chatRoom.sendMsg(u1, "Hi All");
    
    chatRoom.sendMsg(u3, "Hi Chandu how are you!");

    chatRoom.sendPrivateMsg(u3, "Chandu", "Chandu Maya After long time we are meeting");
    
    chatRoom.mute(u1, "Dhana");

    chatRoom.sendMsg(u1, "Bye all");
    return 0;
}