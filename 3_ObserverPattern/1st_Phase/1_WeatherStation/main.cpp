#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

// step1 : observer interface
class Observer 
{
public :
    virtual void update(float temp, float humidity, float pressure) = 0;
    virtual ~Observer() = default;
};

// step 2 : Subject interface 
class Subject 
{
public :
    virtual void attach(Observer* obs) = 0;
    virtual void detach(Observer* obs) = 0;
    virtual void notify() = 0;

    virtual ~Subject() = default;
};

// step 3 : concrete subject
class WeatherStation : public Subject 
{
    std::vector<Observer*> observers;
    float temperature = 0.0f;
    float humidity = 0.0f;
    float pressure = 0.0f;
public :
    void attach(Observer* obs) override 
    {
        observers.push_back(obs);
    }

    void detach(Observer* obs) override 
    {
        observers.erase(remove(observers.begin(), observers.end(), obs), observers.end());
    }

    void notify() override 
    {
        for(auto obs : observers)
        {
            obs->update(temperature, humidity, pressure);
        }
    }

    void setMeasurements(float temp, float hum, float pres)
    {
        temperature = temp;
        humidity = hum;
        pressure = pres; 
        notify();
    }
};

// step 4 : conceret observer 
class Display : public Observer 
{
public :
    void update(float temp, float humidity, float pressure) override 
    {
        std::cout<<"received data  temperature : "<<temp <<" C, "<< "humidity : "<<humidity << " pressure : "<< pressure << std::endl;
    }
};

int main()
{
    WeatherStation station;

    Display d1;

    station.attach(&d1);

    station.setMeasurements(10.4, 20, 30);
    station.setMeasurements(26.2, 70, 1012);


    return 0;
}