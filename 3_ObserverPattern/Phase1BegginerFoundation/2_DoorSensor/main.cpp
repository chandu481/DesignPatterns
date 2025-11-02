#include <iostream>
#include <vector>
#include <algorithm>

// step 1 : IObserver 
class IDoorObserver 
{
public :
    virtual void update(bool isOpened) = 0;
    virtual ~IDoorObserver() = default;
};

// step2 : ISubject
class IDoorSubject 
{
public :
    virtual void attach(IDoorObserver* obs) = 0;
    virtual void detach(IDoorObserver* obs) = 0;
    virtual void notify() = 0;
    virtual void removeAll() = 0;
};

// step3 : concrete subject 
class DoorSenor : public IDoorSubject
{
    std::vector<IDoorObserver*> observers;
    bool isOpen_{false};
public :
    void attach(IDoorObserver* obs) override  
    {
        if(std::find(observers.begin(), observers.end(), obs) == observers.end())
        {
            observers.push_back(obs);
        }
    }

    void detach(IDoorObserver* obs) override 
    {
        for(auto it = observers.begin(); it != observers.end(); it++)
        {
            if(*it != obs)
            {
                observers.erase(it);
                break;
            }
        }
    }

    void removeAll()
    {
        observers.clear();
    }

    void setOpen(bool open)
    {
        if(open == isOpen_) return ;
        isOpen_ = open;
        notify();
    }

    void open() { setOpen(true);}
    void close() { setOpen(false);}
    void toggle() { setOpen(!isOpen_);}

    void notify()
    {
        std::vector<IDoorObserver*> snapshot = observers;
        for(IDoorObserver* obs : snapshot)
        {
            obs->update(isOpen_);
        }
    }

};

class AlarmBuzzer : public IDoorObserver 
{
    std::string name;
public :
    AlarmBuzzer(std::string n) : name(n) {}

    void update(bool isOpened) override 
    {
        if(isOpened == true)
        {
            std::cout<<"Alarm : "<<name<<" Door opened  -- buzzzz "<<std::endl;
        }
        else 
        {
            std::cout<<"Alarm : "<<name<<" Door closed -- silent"<<std::endl;
        }
    }
};

class StatusLED : public IDoorObserver
{
    std::string name;
public :
    StatusLED(std::string n) : name(n) {}

    void update(bool isOpened)
    {
        if(isOpened == true)
        {
            std::cout<<"name : "<<name << "LED ON"<<std::endl;
        }
        else  
        {
            std::cout<<"name : "<<name<<"LED OFF"<<std::endl;
        }
    }
};

class DoorLogger : public IDoorObserver
{
    int opens_ = 0;
    int closes_ = 0;
public :
    void update(bool isOpened)
    {
        if(isOpened == true)
        {
            opens_++;
        }
        else 
            closes_++;

        std::cout<<"[Logger ] opens = "<<opens_ <<" closes_ ="<<closes_<<std::endl;
    }
};

class OpenOnceAlert : public IDoorObserver
{
    IDoorSubject* subject;
    bool fired_{false};
public :
    OpenOnceAlert(IDoorSubject* s) : subject(s) {}
    void update(bool isOpened)
    {
        if(fired_ == false && isOpened == true)
        {
            fired_ = true;
            std::cout<<"OpenOnceAlert : first time door open"<<std::endl;
            subject->detach(this);
        }
    }
};

class DoorCounter : public IDoorObserver
{
    int cnt = 0;
public :
    void update(bool isOpened) override 
    {
        cnt++;
        std::cout<<"DoorCounter cnt : "<<cnt<<std::endl;
    }
};

class DoorDisplay : public IDoorObserver
{
    std::string name_;
public :
    DoorDisplay(std::string n) : name_(n) {}
    void update(bool isOpened)
    {
        std::cout<<"The Door is "<< (isOpened ? "Open" : "Closed") <<std::endl;
    }
};


int main()
{
    DoorSenor door;
    AlarmBuzzer alarm("A");
    StatusLED led("L1");
    DoorLogger logger ;
    OpenOnceAlert once{&door};
    DoorCounter doorCnt;
    DoorDisplay d1("DoorDisplay1");

    door.attach(&alarm);
    door.attach(&doorCnt);
    door.attach(&led);
    door.attach(&logger);
    door.attach(&once);
    door.attach(&d1);

    std::cout<<"--- start (door closed) ---"<<std::endl;
    door.setOpen(false);

    std::cout<<"--- Open -----"<<std::endl;
    door.open();

    std::cout<<"---- close ----"<<std::endl;
    door.close();

    std::cout<<"--- Open -----"<<std::endl;
    door.open();

    std::cout<<"---- close ----"<<std::endl;
    door.close();

    door.removeAll();
    door.open();

    door.close();

    return 0;
}