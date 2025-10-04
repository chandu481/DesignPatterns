#include <iostream>
#include <memory>
#include <vector>

// Step1 : Strategy Interface
class PaymentStrategy
{
public :
    virtual void pay(double amount) = 0;
    virtual ~PaymentStrategy() = default;
};

// Step2 : Concerte strategy
class CreditCardPayment : public PaymentStrategy 
{
    std::string cardNumber;
public :
    CreditCardPayment(std::string cardNo) : cardNumber(cardNo) {} 

    void pay(double amount) override 
    {
        std::cout<<"CreditCardPayment : "<< cardNumber << "paid : "<<amount<<" Rupees"<<std::endl;
    }
};

class PaypalPayment : public PaymentStrategy 
{
    std::string email;
public :
    PaypalPayment(std::string email_) : email(email_) {} 

    void pay(double amount) override 
    {
        std::cout<<"PaypalPayment : "<<email<<" paid : "<< amount<< " Rupees "<<std::endl;
    }
};

class UpiPayment : public PaymentStrategy
{
    std::string upiId;
public :
    UpiPayment(std::string upiId_) : upiId(upiId) {}

    void pay(double amount) override 
    {
        std::cout<<"UpiPayment : "<<upiId<<" paid "<< amount <<" Rupees"<<std::endl;
    }
};

//Step3 : Context 
class ShoppingCart
{
    std::unique_ptr<PaymentStrategy> strategy;
public :
    void setPaymentStrategy(std::unique_ptr<PaymentStrategy> s)
    {
        strategy = std::move(s);
    }

    void checkout(double amount)
    {
        if(strategy)
        {
            strategy->pay(amount);
        }
        else 
        {
            std::cout<<"strategy not set "<<std::endl;
        }
    }
};

int main()
{
    ShoppingCart shoppingCart;

    shoppingCart.setPaymentStrategy(std::make_unique<CreditCardPayment>("1234 4556 9856"));
    shoppingCart.checkout(1000.0);

    shoppingCart.setPaymentStrategy(std::make_unique<PaypalPayment>("charan481@gmai.com"));
    shoppingCart.checkout(2000.0);

    shoppingCart.setPaymentStrategy(std::make_unique<UpiPayment>("9898565432@ybl"));
    shoppingCart.checkout(4000.0);
    
    return 0;
}