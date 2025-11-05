#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>

// step1 : Observer interface
class IObserver 
{
public :
    virtual void update(float temp, float hum, float pres) = 0;

    virtual ~IObserver() = default;
};

// step2 : Subject Interface
class ISubject 
{
public :
    virtual void attach(IObserver* observer) =0;
    virtual void detach(IObserver* observer) = 0;
    virtual void notify() = 0;

    virtual ~ISubject() = default;
};

// step3 : conceret subjects
class WeatherStation : public ISubject 
{
    std::vector<IObserver*> observers;
    float temperature{0.0f};
    float humidity{0.0f};
    float pressure{0.0f};
public :
    void attach(IObserver* obs) override 
    {
        if(std::find(observers.begin(), observers.end(), obs) == observers.end())
            observers.push_back(obs);
    }

    void detach(IObserver* obs) override 
    {
        for(auto it = observers.begin(); it != observers.end(); it++)
        {
            if(*it == obs)
            {
                observers.erase(it);
                break;
            }
        }
    }

    void setMeasurements(float t, float h, float p)
    {
        temperature = t;
        humidity = h;
        pressure = p;
        notify();
    }

    void notify() 
    {
        for(IObserver* obs : observers)
        {
            obs->update(temperature, humidity, pressure);
        }
    }

    void removeAll()
    {
        std::cout<<"Removing All Observers ("<<observers.size()<<") "<<std::endl;
        observers.clear();
    }
};

// step 4 : conceret Observers
class CurrentDisplay : public IObserver
{
    std::string name;
public :
    CurrentDisplay(std::string n) : name(n) {}
    void update(float t, float h, float p) override 
    {
        std::cout<<"Display : "<<name << " received temp = "<< t << " humidity = "<< h <<" Pressure = "<< p<< std::endl;
    }
};

class StaticDisplay : public IObserver 
{
    std::string name;
    float minTemp = std::numeric_limits<float>::max();
    float maxTemp = std::numeric_limits<float>::min();
    float avgTemp;
public :
    StaticDisplay(std::string n) : name(n) {}
    void update(float t, float h, float p) override 
    {
        minTemp = std::min(t, minTemp);
        maxTemp = std::max(t, maxTemp);

        avgTemp = (minTemp + maxTemp) / 2;
        
        std::cout<<"StaticDisplay name : "<<name <<" Received minTemp =  "<< minTemp << " MaxTemp = "<<maxTemp<<std::endl;
    }
};

class FirstAlertDisplay : public IObserver
{
    std::string name;
    ISubject* subject;
    bool fired = false;
public :
    FirstAlertDisplay(std::string n, ISubject* s) : name(n) , subject(std::move(s)) {}
    void update(float t, float h, float p) override 
    {
        if(!fired)
        {
            std::cout<<"[FirstAlertDisplay: "<< name << "]" << "First Alert temp : "
                << t << " hum : "<< h <<" pres : "<<p<<std::endl;
            fired = true;
            subject->detach(this);
        }
    }
};

int main()
{
    WeatherStation ws;
    CurrentDisplay d1("A");
    CurrentDisplay d2("B");
    StaticDisplay sd1("sd1");
    StaticDisplay sd2("sd2");
    FirstAlertDisplay once("once", &ws);

    ws.attach(&d1);
    ws.attach(&d2);
    ws.attach(&sd1);
    ws.attach(&sd2);
    ws.attach(&once);

    ws.setMeasurements(30.5f, 60.0f, 1008.0f);
    std::cout<<"----------------------------------------------"<<std::endl;
    ws.setMeasurements(31.2f, 58.0f, 1006.0f);
    std::cout<<"----------------------------------------------"<<std::endl;
    ws.detach(&d2);

    ws.removeAll();

    ws.setMeasurements(29.9f, 61.0f, 1009.0f);
    std::cout<<"----------------------------------------------"<<std::endl;
    return 0;
}
