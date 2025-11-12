#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>

class Subscripiton 
{
    std::function<void()> cancel_;
    bool active_{false};
public :
    Subscripiton() = default;
    Subscripiton(std::function<void()> f) : cancel_(std::move(f)) , active_{true} {}

    Subscripiton(const Subscripiton& ) = delete;
    Subscripiton& operator=(const Subscripiton& ) = delete;

    Subscripiton(Subscripiton&& other) : cancel_(std::move(other.cancel_)), active_(other.active_){other.active_ = false;}
    Subscripiton& operator=(Subscripiton&& other) 
    {
        if(active_ && cancel_)
        {
            cancel_();
        }
        cancel_ = std::move(other.cancel_);
        active_ = other.active_;
        other.active_ = false;

        return *this;
    }

    void cancel()
    {
        if(active_)
        {
            active_ = false;
            if(cancel_)
            {
                cancel_();
            }
        }
    }

    ~Subscripiton() { cancel(); }
};

// step1 : observer interface
class IObserver 
{
public :
    std::string name_;
    IObserver(std::string name) : name_(name) {}
    virtual void update(std::string from, std::string msg) = 0;

    virtual ~IObserver() = default;
};

// step2 : subject interface
class ISubject
{
    virtual void unsubscribe(std::size_t id) = 0;
public :
    virtual Subscripiton subscribe(std::shared_ptr<IObserver> obs) = 0;
    virtual void sendMsg(std::shared_ptr<IObserver> from, std::string msg) = 0;

    virtual void removeAll() = 0;
    virtual ~ISubject() = default;
};

// step3 : conceret subject 
class ChatRoom : public ISubject, public std::enable_shared_from_this<ChatRoom>
{
    std::unordered_map<std::size_t , std::weak_ptr<IObserver>> subs_; // id --> observer*
    std::size_t nextId{1};
    mutable std::mutex m_;
    void unsubscribe(std::size_t id) override 
    {
        std::cout<<"unsubscribed id "<<id<<std::endl;
        subs_.erase(id);
    }
public :
    Subscripiton subscribe(std::shared_ptr<IObserver> obs) override 
    {
        std::size_t id;
        {
            std::lock_guard<std::mutex> lk(m_);
            id = nextId++;
            subs_[id] = obs;
        }
        
        std::weak_ptr<ChatRoom> selfW = shared_from_this();
        return Subscripiton([selfW, id]{
            if(auto s = selfW.lock())
            {
                s->unsubscribe(id);
            }
        });
    }

    void sendMsg(std::shared_ptr<IObserver> from, std::string msg)
    {
        std::vector<std::shared_ptr<IObserver>> live;
        std::vector<std::size_t> expired;
        {
            std::lock_guard<std::mutex> lk(m_);
            for(auto& [id, obs] : subs_)
            {
                if(auto s = obs.lock())
                {
                    live.push_back(s);
                }
                else 
                {
                    expired.push_back(id);
                }
            }

            for(std::size_t id : expired)
            {
                subs_.erase(id);
            }
        }

        for(auto obs : live)
        {
            if(obs->name_ != from->name_)
                obs->update(from->name_, msg);
        }
    }

    void removeAll() override 
    {
        subs_.clear();
    }
};

// step 4 : conceret observers
class User : public IObserver
{
public :
    User(std::string name) : IObserver(name) {}

    void update(std::string from, std::string msg) override 
    {
        std::cout<<"[user : "<<name_<<"] [msg : "<<msg<<" ] [From : "<<from<<" ] "<<std::endl;
    }
};

class OnceKeywordBell : public IObserver 
{
    Subscripiton token_;
    bool fired_{false};
public :
    OnceKeywordBell(std::string name ) : IObserver(name) {}

    void setToken(Subscripiton& t)
    {
        token_ = std::move(t)  ;
    }

    void update(std::string from, std::string msg) override 
    {
        if(fired_ == false)
        {
            if(msg == "Bell")
            {
                fired_ = true;
                token_.cancel();
            }
        }
    }
};

int main()
{
    auto chatRoom = std::make_shared<ChatRoom>();
    auto user1 = std::make_shared<User>("user1");
    auto user2 = std::make_shared<User>("user2");
    auto user3 = std::make_shared<User>("user3");
    auto user4 = std::make_shared<User>("user4");
    auto onceBell = std::make_shared<OnceKeywordBell>("Bell");


    auto sub1 = chatRoom->subscribe(user1);
    auto sub2 = chatRoom->subscribe(user2);
    auto sub3 = chatRoom->subscribe(user3);
    auto sub4 = chatRoom->subscribe(user4);
    auto sub6Bell = chatRoom->subscribe(onceBell);
    onceBell->setToken(sub6Bell);

    chatRoom->sendMsg(user1, "hi all how are you");
    chatRoom->sendMsg(user1, "Bell");
    chatRoom->sendMsg(user2, "hi all how are you");
    return 0;
}