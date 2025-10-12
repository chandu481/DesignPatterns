#include <iostream>
#include <memory>

// State Interface
class FanState
{
public :
    virtual void handleButtonPress(class Fan& fan) = 0;
    virtual const char* getName() const = 0;
    virtual ~FanState() = default;
};

// forward declaration of fan
class Fan;

// Concrete States
class FanOffState : public FanState
{
public :
    void handleButtonPress(Fan& fan) override ;

    const char* getName() const override 
    {
        return "Fan is off";
    }
};

class FanLowState : public FanState 
{
public :
    void handleButtonPress(Fan& fan) override;

    const char* getName() const override 
    {
        return "Fan is on Low State";
    }
};

class FanHighState : public FanState 
{
public :
    void handleButtonPress(Fan& fan) override ;

    const char* getName() const override 
    {
        return "Fan is on High State";
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

    void pressButton()
    {
        state->handleButtonPress(*this);
    }

    void showState()
    {
        std::cout<< state->getName() << std::endl;
    }

    std::unique_ptr<FanState> makeOffState()
    {
        return std::make_unique<FanOffState>();
    }

    std::unique_ptr<FanState> makeLowState()
    {
        return std::make_unique<FanLowState>();
    }

    std::unique_ptr<FanState> makeHighState()
    {
        return std::make_unique<FanHighState>();
    }
};

void FanOffState::handleButtonPress(Fan& fan)
{
    std::cout<<"Turning fan to low speed \n";
    fan.setState(fan.makeLowState());
}

void FanLowState::handleButtonPress(Fan& fan)
{
    std::cout<<"Turning fan to high speed \n";
    fan.setState(fan.makeHighState());
}

void FanHighState::handleButtonPress(Fan& fan)
{
    std::cout<<"Turning fan to off speed \n";
    fan.setState(fan.makeOffState());
}

Fan::Fan()
{
    state = std::make_unique<FanOffState>();
}

int main()
{
    Fan fan;
    fan.showState();

    fan.pressButton();
    fan.showState();

    fan.pressButton();
    fan.showState();

    fan.pressButton();
    fan.showState();
    
    return 0;
}