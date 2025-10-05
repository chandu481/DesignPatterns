#include <iostream>
#include <memory>

// STEP 1 : Stratergy Interface 
class DiscountStratergy
{
public :
    virtual double applyDiscount(double price) = 0;
    virtual ~DiscountStratergy() = default;
};

// STEP 2 : 
class NoDiscount : public DiscountStratergy
{
public :
    double applyDiscount(double price) override 
    {
        std::cout<<"No Discount is applied "<<std::endl;
        return price;
    }
};

class SesonalDiscount : public DiscountStratergy
{
public :
    double applyDiscount(double price) override 
    {
        std::cout<<"Sesonal Discount 10% applied"<<std::endl;
        return price * 0.1; // 10% off
    }
};

class BlackFridayDiscount : public DiscountStratergy
{
public :
    double applyDiscount(double price) override 
    {
        std::cout<<"BlackFridayDiscount 50% applied"<<std::endl;
        return price * 0.5; // 50% off
    }
};

// STEP 3 : Context
class ShoppingCart
{
private :
    std::unique_ptr<DiscountStratergy> stratergy;
public :
    void setDiscountStratergy(std::unique_ptr<DiscountStratergy> s)
    {
        stratergy = std::move(s);
    }

    void checkout(double amount)
    {
        double price = stratergy->applyDiscount(amount);
        std::cout<<"original price INR : "<<amount<<std::endl;
        std::cout<<"Final price after discount INR : "<<price<<std::endl;
    }
};

int main()
{
    ShoppingCart cart;

    cart.setDiscountStratergy(std::make_unique<SesonalDiscount>());
    cart.checkout(40000.00);
    std::cout<<std::endl;

    cart.setDiscountStratergy(std::make_unique<NoDiscount>());
    cart.checkout(40000.00);
    std::cout<<std::endl;

    cart.setDiscountStratergy(std::make_unique<BlackFridayDiscount>());
    cart.checkout(40000.00);
    std::cout<<std::endl;
    
    return 0;
}