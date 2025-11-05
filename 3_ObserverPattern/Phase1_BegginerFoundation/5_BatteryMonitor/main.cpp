#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// step 1 : observer interface
class IBatteryObserver
{
public :
    std::string name;
    IBatteryObserver(std::string n) : name(n) {}
    virtual void update(unsigned int level, bool isCharging) = 0;
    virtual ~IBatteryObserver() = default;
};

// step 2 : subject interface
class IBatterySubject 
{
public :
    virtual void attach(IBatteryObserver* obs) = 0;
    virtual void detach(IBatteryObserver* obs) = 0;
    virtual void setBatteryLevel(unsigned int level, bool isCharging) = 0;
    virtual void notify() = 0;
};

// step 3 : subject concerete
class BatterySensor : public IBatterySubject
{
    unsigned int level = 0;
    bool isCharging = false;
    std::unordered_map<std::string, IBatteryObserver*> observers;
public :
    void attach(IBatteryObserver* obs) override 
    {
        observers[obs->name] = obs;
    }

    void detach(IBatteryObserver* obs) override 
    {
        if(observers.find(obs->name) == observers.end())
        {
            std::cout<<obs->name<<" not found"<<std::endl;
            return;
        }

        observers.erase(obs->name);
    }

    void setBatteryLevel(unsigned int level, bool isCharging)
    {
        this->level = level;
        this->isCharging = isCharging;
        notify();
    }

    void notify() override 
    {
        std::vector<std::pair<std::string, IBatteryObserver*>> snapshot{observers.begin(), observers.end()};
        for(std::pair<std::string, IBatteryObserver*> obs : snapshot)
        {
            obs.second->update(level, isCharging);
        }
        std::cout<<"----------------------------------------------"<<std::endl;
    }
};

// step 4 : concerete Observers
class BatteryHUD : public IBatteryObserver
{
public :
    BatteryHUD(std::string name) : IBatteryObserver(name) {}
    void update(unsigned int level, bool isCharging) override 
    {
        std::cout<<"[BatteryHUD] : level = "<<level<<"  charging : "<<isCharging<<std::endl;
    }
};

class LowBatteryAlert : public IBatteryObserver
{
    bool thresholdUp = false;
    unsigned int thresholdLevel = 20;
public :
    LowBatteryAlert(std::string name) : IBatteryObserver(name) {}
    void update(unsigned int level, bool isCharging) override 
    {
        if(thresholdUp == true && level >= thresholdLevel )
        {
            std::cout<<"[warn] level raises above a threshold : "<<thresholdLevel<<std::endl;
        }
        else if(thresholdUp == false && level <= thresholdLevel)
        {
            std::cout<<"[warn] when level drops below a threshold : "<<thresholdLevel<<std::endl;
        }
    }

    void setThreshold(unsigned int tLevel, bool isdirectionUp)
    {
        thresholdLevel = tLevel;
        thresholdUp = isdirectionUp;
    }
};

class SaverModeController : public IBatteryObserver
{
    bool powerSaving = false;
    unsigned int thresholdLevel = 25;
public :
    SaverModeController(std::string name) : IBatteryObserver(name) {}

    void update(unsigned int level, bool isCharging) override 
    {
        if(level <= thresholdLevel)
        {
            powerSaving = true;
            std::cout<<"SaverModeController : powerSaving On"<<std::endl;
        }
        else 
        {
            powerSaving = false;
            std::cout<<"SaverModeController : powerSaving Off"<<std::endl;
        }
    }
};

class ChargeCompleteNotifier : public IBatteryObserver
{
public :
    ChargeCompleteNotifier(std::string name) : IBatteryObserver(name) {}

    void update(unsigned int level, bool isCharging) override 
    {
        if(level == 100 && isCharging == true)
        {
            std::cout<<"Charge Complete Notification"<<std::endl;
        }
    }
};

int main()
{
    BatterySensor batterySensor;

    BatteryHUD Hud("HUD");
    LowBatteryAlert alert("LowBatteryAlert");
    SaverModeController saver("SaverMode");
    ChargeCompleteNotifier complete("ChargeCompleteNotifier");

    batterySensor.attach(&Hud);
    batterySensor.attach(&alert);
    batterySensor.attach(&saver);
    batterySensor.attach(&complete);

    batterySensor.setBatteryLevel(30, true);
    batterySensor.setBatteryLevel(20, true);
    batterySensor.setBatteryLevel(10, true);
    batterySensor.setBatteryLevel(100, true);




    return 0;
}