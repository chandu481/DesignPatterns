#include <iostream>
#include <memory>

// STEP1 : state interface
class FanState
{
public :
    virtual void handleButtonPress(class Fan& fan) = 0;
    virtual const char* getStateName() const = 0;
    virtual ~FanState() = default;
};

class Fan;

// STEP2 : concrete state
class FanOffState : public FanState 
{
public :
    void handleButtonPress(Fan& fan) override ;
    const char* getStateName() const override
    {
        return "Fan is at OFF state";
    }
};

class FanLowState : public FanState 
{
public :
    void handleButtonPress(Fan& fan) override;
    const char* getStateName() const override
    {
        return "Fan is at LOW state";
    }
};

class FanHighState : public FanState 
{
public :
    void handleButtonPress(Fan& fan) override;
    const char* getStateName() const override
    {
        return "Fan is at High State";
    }
};

// Context 
class Fan
{
    std::unique_ptr<FanState> state;
public :
    Fan();

    void setState(std::unique_ptr<FanState> newState)
    {
        state = std::move(newState);
    }

    void showState()
    {
        std::cout << state->getStateName() << std::endl;
    }

    void buttonPress()
    {
        state->handleButtonPress(*this);
    }

    std::unique_ptr<FanState> makeLowState() { return std::make_unique<FanLowState>();}
    std::unique_ptr<FanState> makeHighState() { return std::make_unique<FanHighState>();}
    std::unique_ptr<FanState> makeOffState() { return std::make_unique<FanOffState>(); }
};

Fan::Fan()
{
    state = std::make_unique<FanOffState>();
}

void FanOffState::handleButtonPress(Fan& fan)
{
    std::cout<<"Fan is turning to Low state"<<std::endl;
    fan.setState(fan.makeLowState());
}

void FanLowState::handleButtonPress(Fan& fan)
{
    std::cout<<"Fan is turning to High state"<<std::endl;
    fan.setState(fan.makeHighState());
}

void FanHighState::handleButtonPress(Fan& fan)
{
    std::cout<<"Fan is turning to OFF state"<<std::endl;
    fan.setState(fan.makeOffState());
}

int main()
{
    Fan fan;
    fan.showState();
    std::cout<<std::endl;

    fan.buttonPress();
    fan.showState();
    std::cout<<std::endl;

    fan.buttonPress();
    fan.showState();
    std::cout<<std::endl;

    fan.buttonPress();
    fan.showState();
    std::cout<<std::endl;

    return 0;
}