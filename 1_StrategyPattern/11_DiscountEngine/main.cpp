#include <iostream>
#include <memory>

struct Order 
{
    double price;
    int customerTier;  // 1 = normal,  2 = loyal,  3 = premium
};

// STEP1 : Strategy Interface
class DiscountStrategy
{
public :
    virtual double applyDiscount(const Order& order) = 0;
};

// STEP2 : Concrete Strategies 
class SeasonalDiscount : public DiscountStrategy
{
public :
    double applyDiscount(const Order& order) override 
    {
        return order.price * 0.90;  // 10% off
    }
};

class LoyaltyDiscount : public DiscountStrategy
{
public :
    double applyDiscount(const Order& order) override 
    {
        double discountPrice = order.price;
        if(order.customerTier >= 2)
        {
            discountPrice = discountPrice * 0.05;
        }
        return discountPrice; // 5% off
    }
};

class NoDiscount : public DiscountStrategy
{
public :
    double applyDiscount(const Order& order) override 
    {
        return order.price; // No Discount
    }
};

// STEP3 : Context
class CheckoutService
{
    std::unique_ptr<DiscountStrategy> strategy;
public :
    void setStrategy(std::unique_ptr<DiscountStrategy> s)
    {
        strategy = std::move(s);
    }

    double calculateTotal(const Order& order)
    {
        return  strategy ? strategy->applyDiscount(order) : order.price;
    }
};

int main()
{
    Order order{100.0, 2};

    CheckoutService checkout;
    checkout.setStrategy(std::make_unique<SeasonalDiscount>());
    std::cout << "Seasonal: " << checkout.calculateTotal(order) << "\n";

    checkout.setStrategy(std::make_unique<LoyaltyDiscount>());
    std::cout << "Loyalty: " << checkout.calculateTotal(order) << "\n";

    checkout.setStrategy(std::make_unique<NoDiscount>());
    std::cout << "No Discount: " << checkout.calculateTotal(order) << "\n";
    return 0;
}