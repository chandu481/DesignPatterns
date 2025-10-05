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
class Character
{
    std::unique_ptr<WeaponStrategy> weapon;
public :
    void equipWeapon(std::unique_ptr<WeaponStrategy> newWeapon)
    {
        weapon = std::move(newWeapon);
    }

    void attack() 
    {
        if(weapon)
        {
            weapon->attack();
        }
        else 
        {
            std::cout<<"no weapon equipped"<<std::endl;
        }
    }
};

int main()
{
    Character hero;

    hero.equipWeapon(std::make_unique<SwordStrategy>());
    hero.attack();

    hero.equipWeapon(std::make_unique<BowStrategy>());
    hero.attack();

    hero.equipWeapon(std::make_unique<MagicStrategy>());
    hero.attack();
    
    return 0;
}