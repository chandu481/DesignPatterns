#include <iostream>
#include <memory>
class ATM;

// STEPT 1 : state interface
class ATMState
{
public :
    virtual void insertCard(ATM& atm) { std::cout<<"Action not allowed in this state"<<std::endl;}
    virtual void edjectCard(ATM& atm) { std::cout<<"Action not allowed in this state"<<std::endl;}
    virtual void enterPin(ATM& atm, int pin) { std::cout<<"Action not allowed in this state"<<std::endl;}
    virtual void withdraw(ATM& atm, int amount) { std::cout<<"Action not allowed in this state"<<std::endl;}
    virtual const char* getStateName() const = 0;
};

// STEP 2 : Context
class ATM
{
    std::unique_ptr<ATMState> state;
    int cash = 1000;
    bool pinOk = false;
public :
    ATM(std::unique_ptr<ATMState> initialState, int initialCash = 1000) 
        : state(std::move(initialState)), cash(initialCash) {}
    
    void setState(std::unique_ptr<ATMState> s) { state = std::move(s); }
    int& getCash() {return cash;}
    bool& isPinOk() { return pinOk; }

    void insertCard() {state->insertCard(*this);}
    void ejectCard() { state->edjectCard(*this);}
    void enterPin(int pin) { state->enterPin(*this, pin);}
    void withDraw(int amount) { state->withdraw(*this, amount);}

    void showState()
    {
        std::cout<< state->getStateName() << std::endl;
    }
};

// STEP3 : concrete state
class NoCard : public ATMState 
{
public :
    const char* getStateName() const override { return "NoCard";}
    void insertCard(ATM& atm) override;
};

class HasCard : public ATMState 
{
public :
    const char* getStateName() const override { return "Has Card";}
    void enterPin(ATM& atm, int pin) override;
    void edjectCard(ATM& atm) override;
};

class PinVerified : public ATMState 
{
public :
    const char* getStateName() const override { return "Pin Verified";}
    void edjectCard(ATM& atm) override;
    void withdraw(ATM& atm, int amount) override;
};

class OutOfCash : public ATMState 
{
public :
    const char* getStateName() const override { return "Out of Cash"; }
    void insertCard(ATM& atm) override 
    {
        std::cout<<"ATM out of cash. come back later"<<std::endl;
    }
};

// 4) Transactions 
void NoCard::insertCard(ATM& atm)
{
    std::cout<<"card inserted, please enter your PIN\n";
    atm.setState(std::make_unique<HasCard>());
}

void HasCard::enterPin(ATM& atm, int pin)
{
    if(pin == 1234)
    {
        std::cout<<"PIN OK"<<std::endl;
        atm.isPinOk() = true;
        atm.setState(std::make_unique<PinVerified>());
    }
    else 
    {
        std::cout<<"PIN incorrect"<<std::endl;
    }
}

void HasCard::edjectCard(ATM& atm)
{
    std::cout<<"Card edject"<<std::endl;
    atm.setState(std::make_unique<NoCard>());
}

void PinVerified::withdraw(ATM& atm, int amount)
{
    if(atm.getCash() < amount)
    {
        std::cout<<"Insufficient ATM cash, Switching to outOfcash"<<std::endl;
        atm.setState(std::make_unique<OutOfCash>());
        return;
    }

    std::cout<<" Dispensing amount : "<<amount<<std::endl;
    atm.getCash() -= amount;
    if(atm.getCash() == 0)
    {
        std::cout<< " ATM is empty\n ";
        atm.setState(std::make_unique<OutOfCash>());
    }
}

void PinVerified::edjectCard(ATM& atm)
{
    std::cout<<"card edjected" << std::endl;
    atm.setState(std::make_unique<NoCard>());
}

int main()
{
    ATM atm(std::make_unique<NoCard>());

    atm.showState();
    atm.withDraw(50);

    atm.insertCard();
    atm.enterPin(1111);
    atm.enterPin(1234);
    atm.withDraw(200);
    atm.withDraw(400);
    atm.ejectCard();
    atm.showState();

    return 0;
}