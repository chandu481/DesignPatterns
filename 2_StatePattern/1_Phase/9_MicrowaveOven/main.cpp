#include <iostream>
#include <memory>

class Oven;

// step 1 : state interface
class State 
{
public :
    virtual void showState(Oven& oven) = 0;
    virtual void onStartCooking(Oven& oven, int tSec) {std::cout<<"Invalid in this state\n";}
    virtual void onOpenDoor(Oven& oven) {std::cout<<"Invalid in this state\n";}
    virtual void onCloseDoor(Oven& oven) { std::cout<<"Invalid in this state \n";}
    virtual void onTick(Oven& oven) {std::cout<<"Invalid in this state\n"; }
    virtual void onCancel(Oven& oven) { std::cout<<"Invalid in this state\n"; }

    virtual ~State() = default;
};

// step 2 : concrete states 
class Idle : public State 
{
public :
    void showState(Oven& oven) override { std::cout<<"IDLE"<<std::endl;}
    void onStartCooking(Oven& oven, int tSec) override;
    void onOpenDoor(Oven& oven) override;
    void onCancel(Oven& oven) {}
};

class Cooking : public State 
{
public :
    void showState(Oven& oven) override { std::cout<<"COOKING"<<std::endl; }
    void onTick(Oven& oven) override;
    void onOpenDoor(Oven& oven) override;
    void onCancel(Oven& oven) override;
};

class DoorOpen : public State 
{
public :
    void showState(Oven& oven) override { std::cout<<"DOOR_OPEN"<<std::endl; }
    void onCloseDoor(Oven& oven) override;
    void onStartCooking(Oven& oven, int tSec) override;
};

class Done : public State 
{
public :
    void showState(Oven& oven) override { std::cout<<"Done"<<std::endl; }
    void onOpenDoor(Oven& oven) override;
    void onCancel(Oven& oven) override;
};

// step 3 : context 
class Oven 
{
    std::unique_ptr<State> state;
public :
    bool doorClosed = true;
    int remainingCookSec = 0;
    Oven();
    void showState()
    {
        state->showState(*this);
    }

    void setState(std::unique_ptr<State> newState)
    {
        state = std::move(newState);
    }

    void openDoor()
    {
        state->onOpenDoor(*this);
    }

    void closeDoor()
    {
        state->onCloseDoor(*this);
    }

    void startCooking(int seconds)
    {
        state->onStartCooking(*this, seconds);
    }

    void tick()
    {
        state->onTick(*this);
    }

    void cancel()
    {
        state->onCancel(*this);
    }
};
Oven::Oven()
{
    state = std::make_unique<Idle>();
}
void Idle::onStartCooking(Oven& oven, int tSec)
{
    if(oven.doorClosed == false)
    {
        std::cout<<"Oven Door is opened, please close the oven door"<<std::endl;
        return;
    }

    if(tSec <= 0)
    {
        return;
    }
    oven.remainingCookSec = tSec;
    std::cout<<"Cooking for " << oven.remainingCookSec <<" seconds" << std::endl;
    oven.setState(std::make_unique<Cooking>());
}
void Idle::onOpenDoor(Oven& oven) 
{
    oven.setState(std::make_unique<DoorOpen>());
    oven.doorClosed = false;
}

void Cooking::onTick(Oven& oven)
{
    oven.remainingCookSec--;
    if(oven.remainingCookSec > 0)
    {
        std::cout<<oven.remainingCookSec<<" left"<<std::endl;
    }
    else 
    {
        oven.setState(std::make_unique<Done>());
        std::cout<<"Ding! Done"<<std::endl;
    }
}
void Cooking::onOpenDoor(Oven& oven)
{
    oven.setState(std::make_unique<DoorOpen>());
    std::cout<<"Cooking Paused"<<std::endl;
}
void Cooking::onCancel(Oven& oven)
{
    oven.setState(std::make_unique<Idle>());
    std::cout<<"Cooking Stopped"<<std::endl;
}

void DoorOpen::onCloseDoor(Oven& oven)
{
    oven.setState(std::make_unique<Idle>());
    oven.doorClosed = true;
    std::cout<<"Door closed"<<std::endl;
}
void DoorOpen::onStartCooking(Oven& oven, int tsec)
{
    std::cout<<"Cannot start : door open"<<std::endl;
}

void Done::onOpenDoor(Oven& oven)
{
    oven.setState(std::make_unique<DoorOpen>());
    oven.doorClosed = false;
    std::cout<<"Take food out"<<std::endl;
}
void Done::onCancel(Oven& oven)
{
    oven.setState(std::make_unique<Idle>());
    std::cout<<"reset to ready"<<std::endl;
}

int main()
{
    Oven m;

    m.startCooking(3);
    m.tick();
    m.tick();
    m.tick();
    m.openDoor();
    m.closeDoor();

    //m.showState();

    std::cout<<"---\n";
    m.startCooking(5);
    m.tick();
    m.openDoor();
    m.closeDoor();
    m.startCooking(2);
    m.tick();
    m.tick();
    m.openDoor();
    m.closeDoor();

    std::cout<<"---\n";
    m.startCooking(5);
    m.tick();
    m.cancel();
    
    return 0;
}