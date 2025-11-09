#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>

class Subscription
{
    std::function<void()> cancel_;
    bool active_{false};
public:
    Subscription() = default;
    Subscription(std::function<void()> f) : cancel_(std::move(f)), active_{true} {}

    Subscription(const Subscription& )=delete;
    Subscription& operator=(const Subscription& )= delete;
    Subscription(Subscription&& other) : cancel_(std::move(other.cancel_)), active_(other.active_)
    {
        other.active_ = false;
    }

    Subscription& operator=( Subscription&& other)
    {
        if(this != &other)
        {
            if(active_ && cancel_) cancel_;
            cancel_ = std::move(other.cancel_);
            active_ = other.active_;
            other.active_ = false;
        }

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

    ~Subscription() { cancel();}
};

class IObserver 
{
public :
    virtual void update() = 0;
    virtual ~IObserver() = default;
};

class ISubject 
{
public :
    virtual Subscription subscribe(std::shared_ptr<IObserver> obs) = 0;
    virtual void unsubscribe(std::size_t id) = 0;
    virtual void notify() = 0;

    virtual ~ISubject() = default;
};

class WeatherStation : public ISubject, public std::enable_shared_from_this<WeatherStation>
{
    float temp{0.0f};
    float hum{0.0f};
    float pres{0.0f};
    std::unordered_map<std::size_t, std::weak_ptr<IObserver>> slots;
    std::size_t nextId{1};
    mutable std::mutex m_;
    
    void unsubscribe(std::size_t id) override 
    {
        std::lock_guard<std::mutex> lk(m_);
        slots.erase(id);
        std::cout<<"Id : "<<id<<" unsubscribed"<<std::endl;
    }
public :
    Subscription subscribe(std::shared_ptr<IObserver> obs) override 
    {
        if(!obs) return {};
        std::size_t id;
        {
            std::lock_guard<std::mutex> lk(m_);
            id = nextId++;
            slots.emplace(id, obs);
        }

        auto selfW = shared_from_this();
        return Subscription([selfW, id]{
            selfW->unsubscribe(id);
        });
    }


    void notify() override 
    {
        std::vector<std::shared_ptr<IObserver>> live;
        std::vector<std::size_t> expired;

        {
            std::lock_guard<std::mutex> lk(m_);
            for(auto& [id, obs] : slots)
            {
                if(auto s = obs.lock())
                {
                    live.push_back(std::move(s));
                }
                else 
                {
                    expired.push_back(id);
                }
            }

            for(auto id : expired)
            {
                slots.erase(id);
            }
        }

        for(auto& l : live)
        {
            l->update();
        }
    }

    void setMeasurements(float t, float h, float p)
    {
        temp = t;
        hum = h;
        pres = p;
        notify();
    }

    float getTemp() {
        std::lock_guard<std::mutex> lk(m_);
         return temp;}
    float getHum() { 
        std::lock_guard<std::mutex> lk(m_);
        return hum; }
    float getPres() { 
        std::lock_guard<std::mutex> lk(m_);
        return pres; }
};

class CurrentDisplay : public IObserver 
{
    std::shared_ptr<WeatherStation> ws;
public :
    CurrentDisplay(std::shared_ptr<WeatherStation> s) : ws(std::move(s)) {}
    void update() override 
    {
        std::cout<<"[currentDisplay] got update"<<std::endl;
        float temp = ws->getTemp();
        float hum = ws->getHum();
        float pres = ws->getPres();

        std::cout<<"temp = "<<temp<<" hum = "<<hum<<" pres = "<<pres<<std::endl;
    }
};

class OnceHighTempAlert : public IObserver
{
    std::shared_ptr<WeatherStation> ws;
    float tempThreshold = 50.0f;
    Subscription token_;
public :
    OnceHighTempAlert(std::shared_ptr<WeatherStation> s) : ws(std::move(s))
    {
    }

    void setToken(Subscription& s)
    {
        token_ = std::move(s);
    }

    void update() override 
    {
        float temp = ws->getTemp();
        if(temp >= tempThreshold)
        {
            std::cout<<"OnceHighTemp Alert crossed the threshold"<<std::endl;
            token_.cancel();
        }
    }
};

int main()
{
    std::shared_ptr<WeatherStation> weather = std::make_shared<WeatherStation>();
    //currentDisplay d1(ws);
    std::shared_ptr<IObserver> d1 = std::make_shared<CurrentDisplay>(weather);
    std::shared_ptr<OnceHighTempAlert> once = std::make_shared<OnceHighTempAlert>(weather);

    auto sub = weather->subscribe(d1);
    Subscription subOnce = weather->subscribe(once);
    once->setToken(subOnce);

    weather->setMeasurements(28.0f, 60.0f, 1010.0f); // display prints
    weather->setMeasurements(30.0f, 58.0f, 1009.0f); // alert fires and detaches
    weather->setMeasurements(31.0f, 55.0f, 1008.0f); // only display remains
    sub.cancel();
    weather->setMeasurements(29.0f, 50.0f, 1007.0f);
};