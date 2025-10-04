#include <iostream>
#include <vector>
#include <memory>

// STEP1 : Strategy Interface
class PaymentStrategy
{
public :
    virtual void pay(double amount) = 0;
    virtual ~PaymentStrategy() = default;
};

// STEP2 : Concrete Strategy
class CreditCardPayment : public PaymentStrategy 
{
    std::string cardNumber;
public :
    CreditCardPayment(std::string cardNo) : cardNumber(cardNo) {}

    void pay(double amount) override 
    {
        std::cout<<"CreditCardPayment cardNumber : "<<cardNumber << " amount : "<<amount<<" Rupees"<<std::endl;
    }
};

class PaypalPayment : public PaymentStrategy 
{
    std::string email;
public :
    PaypalPayment(std::string email_) : email(email_){}

    void pay(double amount) override 
    {
        std::cout<<"PaypalPayment email : "<<email<<" amount : "<<amount<<" Rupees"<<std::endl;
    }
};

class UpiPayment : public PaymentStrategy
{
    std::string upiId;
public :
    UpiPayment(std::string upiId_) : upiId(upiId_) {}

    void pay(double amount) override 
    {
        std::cout<<"UpiPayment upiId : "<<upiId<<" amound : "<<amount<<" Rupees"<<std::endl;
    }
};

// STEP 3 : Context 
class ShoppingCart
{
    std::unique_ptr<PaymentStrategy> paymentStrategy;
public :
    void setPaymentStrategy(std::unique_ptr<PaymentStrategy> ps)
    {
        paymentStrategy = std::move(ps);
    }

    void checkout(double amount)
    {
        if(paymentStrategy)
        {
            paymentStrategy->pay(amount);
        }
        else
        {
            std::cout<<"paymentStrategy not selected"<<std::endl;
        }
    }
};

int main()
{
    ShoppingCart cart;
    int choice ;
    double amount;
    std::cout<<"Enter amount to pay:";
    std::cin>>amount;

    std::cout<<"\n select payment method: "<<std::endl;
    std::cout<<"1. Credit card\n";
    std::cout<<"2. PayPal\n";
    std::cout<<"3. UPI\n";
    std::cout<<"Enter your choice: ";
    std::cin>>choice;

    switch(choice)
    {
        case 1 : {
            std::string card;
            std::cout<<"Enter card Number : ";
            std::cin>>card;
            cart.setPaymentStrategy(std::make_unique<CreditCardPayment>(card));
            break;
        }
        case 2 : {
            std::string email;
            std::cout<<"Enter paypalId : ";
            std::cin>>email;
            cart.setPaymentStrategy(std::make_unique<PaypalPayment>(email));
            break;
        }
        case 3 : {
            std::string upiId;
            std::cout<<"Enter upiId : ";
            std::cin>>upiId;
            cart.setPaymentStrategy(std::make_unique<UpiPayment>(upiId));
            break;
        }
        default :
            std::cout<<"Invalid choice"<<std::endl;
            return 0;
    }

    cart.checkout(amount);
    return 0;
}