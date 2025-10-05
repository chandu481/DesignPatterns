#include <iostream>
#include <memory>

// STEP 1 : Stratergy Interface
class AttackStratergy
{
public :
    virtual void attack() = 0;
    virtual ~AttackStratergy() = default;
};

// STEP 2 : Concrete Stratergy
class AggressiveStratergy : public AttackStratergy 
{
public :
    void attack() override 
    {
        std::cout<<"Enemy attacks recklessly!\n";
    }
};

class DefensiveStratergy : public AttackStratergy 
{
public :
    void attack() override 
    {
        std::cout<<"Enemy blocks and counters.\n";
    }
};

class PassiveStratergy : public AttackStratergy
{
public :
    void attack() override 
    {
        std::cout<<"Enemy stays away, avoiding combat.\n";
    }
};

// STEP 3 : Context 
class Enemy
{
    std::unique_ptr<AttackStratergy> stratergy;
public :
    void setAttackStratergy(std::unique_ptr<AttackStratergy> s)
    {
        stratergy = std::move(s);
    }

    void performAttack()
    {
        if(stratergy)
        {
            stratergy->attack();
        }
        else 
        {
            std::cout<<"AttackStratergy not set"<<std::endl;
        }
    }
};

int main()
{
    Enemy enemy1;

    enemy1.setAttackStratergy(std::make_unique<AggressiveStratergy>());
    enemy1.performAttack();

    enemy1.setAttackStratergy(std::make_unique<DefensiveStratergy>());
    enemy1.performAttack();

    enemy1.setAttackStratergy(std::make_unique<PassiveStratergy>());
    enemy1.performAttack();

    return 0;
}