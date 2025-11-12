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
    virtual Subscripiton subscribe(
        const std::shared_ptr<IObserver>& obs,
        std::function<bool(const std::string&, const std::string&)> filter) = 0 ;

    virtual void sendMsg(std::shared_ptr<IObserver> from, std::string msg) = 0;
    virtual void removeAll() = 0;
    virtual ~ISubject() = default;
};

// step3 : conceret subject 
class ChatRoom : public ISubject, public std::enable_shared_from_this<ChatRoom>
{
    struct Slot 
    {   
        std::weak_ptr<IObserver> obs;
        std::function<bool(const std::string&, const std::string&)> filter;
    };

    std::unordered_map<std::size_t ,  Slot> subs_; // id --> observer*
    std::size_t nextId{1};
    mutable std::mutex m_;
    void unsubscribe(std::size_t id) override 
    {
        std::cout<<"unsubscribed id "<<id<<std::endl;
        std::lock_guard<std::mutex> lk(m_);
        subs_.erase(id);
    }

    Subscripiton subscribeImpl(std::shared_ptr<IObserver> obs,
        std::function<bool(const std::string&, const std::string& )> filter)  
    {
        if(!obs)
        {
            return {};
        }

        std::size_t id;
        {
            std::lock_guard<std::mutex> lk(m_);
            id = nextId++;
            subs_.emplace(id, Slot{obs, std::move(filter)});
        }
        
        std::weak_ptr<ChatRoom> selfW = shared_from_this();
        return Subscripiton([selfW, id]{
            if(auto s = selfW.lock())
            {
                s->unsubscribe(id);
            }
        });
    }

public :
    Subscripiton subscribe(std::shared_ptr<IObserver> obs) override 
    {
        return subscribeImpl(obs, nullptr);
    }

    Subscripiton subscribe(
        const std::shared_ptr<IObserver>& obs,
        std::function<bool(const std::string&, const std::string&)> filter) override
    {
        return subscribeImpl(obs, std::move(filter));
    }

    void sendMsg(std::shared_ptr<IObserver> from, std::string msg)
    {
        std::vector<std::shared_ptr<IObserver>> live;
        std::vector<std::function<bool(const std::string, const std::string )>> filters;
        std::vector<std::size_t> expired;
        {
            std::lock_guard<std::mutex> lk(m_);
            for(auto& [id, slot] : subs_)
            {
                auto obs = slot.obs;
                if(auto s = obs.lock())
                {
                    live.push_back(std::move(s));
                    filters.push_back(slot.filter);
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

        for(int i=0; i<live.size(); i++)
        {
            auto& obs = live[i];
            auto& f = filters[i];
            if(obs->name_ != from->name_)
            {
                if(!f || f(from->name_, msg))
                    obs->update(from->name_, msg);
            }
        }
    }

    void removeAll() override 
    {
        std::lock_guard<std::mutex> lk(m_);
            subs_.clear();
    }
};

// step 4 : conceret observers
class UserDisplay : public IObserver
{
public :
    UserDisplay(std::string name) : IObserver(name) {}

    void update(std::string from, std::string msg) override 
    {
        std::cout<<"[user : "<<name_<<"] [msg : "<<msg<<" ] [From : "<<from<<" ] "<<std::endl;
    }
};

class KeywordAlert : public IObserver 
{
    std::string keyword_;
public :
    KeywordAlert(std::string name, std::string keyword) : IObserver(name) , keyword_(keyword)
    {
    }
    void update(std::string sender, std::string text) override {
        std::cout << "[KeywordAlert:" << name_ << "] MATCH from " << sender
                  << " -> " << text << '\n';
    }
    const std::string& keyword() const { return keyword_; }
};

class BlockSender : public IObserver {
    std::string blocked_;
public:
    BlockSender(std::string name, std::string blocked)
        : IObserver(name), blocked_(std::move(blocked)) {}
    void update( std::string sender, std::string text) override {
        std::cout << "[BlockSender:" << name_ << "] <- " << sender << ": " << text << '\n';
    }
    const std::string& blocked() const { return blocked_; }
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
    using std::make_shared;

    auto room = make_shared<ChatRoom>();

    auto alice = make_shared<UserDisplay>("Alice");
    auto bob = make_shared<KeywordAlert>("Bob", "urgent");
    auto carol = make_shared<BlockSender>("Carol", "Spambot");
    auto spamboot = make_shared<UserDisplay>("Spambot");

    Subscripiton sA = room->subscribe(alice);
    Subscripiton sB = room->subscribe(bob, [kw = bob->keyword()](const std::string& r, const std::string& msg){
        return msg.find(kw) != std::string::npos;
    });

    Subscripiton sC = room->subscribe(carol, [blocked = carol->blocked()](const std::string& sender, const std::string&) {
        return sender != blocked;
    });

    std::cout << "\n-- Messages --\n";
    room->sendMsg(alice  ,  "hello everyone");
    room->sendMsg(carol , "urgent update!");
    room->sendMsg(alice,   "urgent meeting at 3PM");

    room->sendMsg(spamboot, "this is spam bot msg");
    

    std::cout << "\n-- Drop Bob; send again --\n";
    sB.cancel();
    room->sendMsg(alice, "urgent: only Alice/Carol now");

    std::cout << "\n-- removeAll(); send again (silence) --\n";
    room->removeAll();
    room->sendMsg(alice, "nobody should see this");


    return 0;
}