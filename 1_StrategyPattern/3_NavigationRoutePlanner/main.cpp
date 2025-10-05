#include <iostream>
#include <memory>

// STEP1 : Stratergy interface
class RouteStratergy
{
public :
    virtual void buildRoute() = 0;
    virtual ~RouteStratergy() = default;
};

// STEP2 : Concrete Stratergy
class CarRoute : public RouteStratergy
{
public :
    void buildRoute() override 
    {
        std::cout<<"building faster route for car"<<std::endl;
    }
};

class BikeRoute : public RouteStratergy
{
public :
    void buildRoute() override 
    {
        std::cout<<"building shortest route for bike"<<std::endl;
    }
};

class WalkRoute : public RouteStratergy 
{
public :
    void buildRoute() override 
    {
        std::cout<<"building sceneric route for walk"<<std::endl;
    }
};

// STEP 3 : CONTEXt
class Navigator 
{
    std::unique_ptr<RouteStratergy> routeStratergy;
public :
    void setRouteStragergy(std::unique_ptr<RouteStratergy> s)
    {
        routeStratergy = std::move(s);
    }

    void navigate()
    {
        if(routeStratergy)
        {
            routeStratergy->buildRoute();
        }
        else 
        {
            std::cout<<"routeStratergy is not set"<<std::endl;
        }
    }
};

int main()
{
    Navigator mapApp;

    int choice;

    std::cout<<"Enter RouteMode : \n";
    std::cout<<"\t1. Car Route \n\t2. Bike Route \n\t3. Walk Route \n ";
    std::cin>>choice;

    switch(choice)
    {
        case 1 : {
            mapApp.setRouteStragergy(std::make_unique<CarRoute>());
            break;
        }
        case 2 : {
            mapApp.setRouteStragergy(std::make_unique<BikeRoute>());
            break;
        }
        case 3 : {
            mapApp.setRouteStragergy(std::make_unique<WalkRoute>());
            break;
        }
        default : 
            std::cout<<"Invalid route "<<std::endl;
            return 0;
    }

    mapApp.navigate();

    return 0;
}