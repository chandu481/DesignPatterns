#include <iostream>
#include <memory>

// step1 : Strategy Interface
class WeaponStrategy
{
public :
    virtual void attack() = 0;
    virtual ~WeaponStrategy() = default;
};

// step2 : Concrete Stratigies
class SwordStrategy : public WeaponStrategy
{
public :
    void attack() override 
    {
        std::cout<<"Attacking with sword"<<std::endl;
    }
};

class BowStrategy : public WeaponStrategy
{
public :
    void attack() override 
    {
        std::cout<<"Attacking with Bow"<<std::endl;
    }
};

class MagicStrategy : public WeaponStrategy
{
public :
    void attack() override 
    {
        std::cout<<"Attacking with Magic"<<std::endl;
    }
};

//step3 : Context
class Weapon
{
    std::unique_ptr<WeaponStrategy> strategy;
public :
    void setWeapon(std::unique_ptr<WeaponStrategy> s)
    {
        strategy = std::move(s);
    }

    void attack() 
    {
        if(strategy)
        {
            strategy->attack();
        }
        else 
        {
            std::cout<<"attackStrategy not set"<<std::endl;
        }
    }
};

int main()
{
    Weapon w;

    w.setWeapon(std::make_unique<SwordStrategy>());
    w.attack();

    w.setWeapon(std::make_unique<BowStrategy>());
    w.attack();

    w.setWeapon(std::make_unique<MagicStrategy>());
    w.attack();
    
    return 0;
}