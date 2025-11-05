#include <iostream>
#include <string>
#include <unordered_map>

// step 1 : Observer interface
class IObserver 
{
public :
    std::string name;
    IObserver(std::string n) : name(n) {}
    virtual void update(std::string& symbol, double price) = 0;
    virtual ~IObserver() = default;
};

// step 2 : Subject interface
class ISubject 
{
public :
    virtual void attach(IObserver* obs) = 0;
    virtual void detach(IObserver* obs) = 0;
    virtual void notify(std::string& symbol, double price) = 0;
    virtual void publish(std::string symbol, double price) = 0;
    virtual void removeAll() = 0;
};

// step 3 : concrete subjects
class MarketFeed : public ISubject
{
    std::unordered_map<std::string, double> stocks;  // symbol --> price
    std::unordered_map<std::string, IObserver* > observers; // name --> IObserver
public :
    void attach(IObserver* obs) override 
    {
        observers[obs->name] = obs;
    }

    void detach(IObserver* obs) override 
    {
        if(observers.find(obs->name) == observers.end())
        {
            std::cout<<"Obserer "<<obs->name<<" Not found"<<std::endl;
            return ;
        }

        observers.erase(obs->name);
    }

    void removeAll()
    {
        observers.clear();
        std::cout<<"All removed"<<std::endl;
    }

    void publish(std::string symbol, double price) override 
    {
        if(stocks.find(symbol) != stocks.end())
        {
            double oldPrice = stocks[symbol];
            if(price == oldPrice)
            {
                std::cout<<"Old and New price are same no need to update to observers"<<std::endl;
                return;
            }
        }

        stocks[symbol] = price;
        notify(symbol, price);
    }

    void notify(std::string& symbol, double price)
    {
        std::unordered_map<std::string, IObserver*> snapshot = observers;
        for(std::pair<std::string, IObserver*> obs : snapshot)
        {
            obs.second->update(symbol, price);
        }

        std::cout<<"----------------------------------------"<<std::endl;
    }
};

// step 4 : concrete observers
class TickerBoard : public IObserver 
{
public :
    TickerBoard(std::string name) : IObserver(name) {}
    void update(std::string& symbol, double price) override 
    {
        std::cout<<"[TickerBoard] Symbol : "<<symbol<<" Price : "<<price<<std::endl;
    }
};

class MovingAverageWidget : public IObserver
{
    std::unordered_map<std::string, std::pair<double, int>> avgStockPrices;
public :
    MovingAverageWidget(std::string name) : IObserver(name) {}
    void update(std::string& symbol, double price) override
    {
        int total = 0;
        double avg = 0.0;
        if(avgStockPrices.find(symbol) != avgStockPrices.end())
        {
            total = avgStockPrices[symbol].second;
            avg = avgStockPrices[symbol].first;
        }

        total++;
        avg = (avg+price)/total;
        std::cout<<"Price : "<<price <<" avg = " << avg<<std::endl;
        avgStockPrices[symbol].first = avg;
        avgStockPrices[symbol].second = total;
    }
};

class PriceAlert : public IObserver 
{
    std::unordered_map<std::string, std::pair<double, bool>> stockThreshold; // symbol --> <price, direction up/down>
public :
    PriceAlert(std::string name) : IObserver(name) {}
    void update(std::string& symbol, double price) override 
    {
        if(stockThreshold.find(symbol) == stockThreshold.end())
        {
            std::cout<<"Stock is not set by the Price alert"<<std::endl;
            return ;
        }

        std::pair<double, bool> threshold = stockThreshold[symbol];
        if(threshold.second == true && price >= threshold.first)
        {
            std::cout<<"stock "<<symbol<<" from down reached to threshold price : "<<price<<std::endl;
        }
        else if(threshold.second == false && price <= threshold.first)
        {
            std::cout<<"stock "<<symbol<<" from up reached to threshold price : "<<price<<std::endl;
        }
    }

    void setStockThresholdPrice(std::string symbol, double price, bool ThresholdUp)
    {
        stockThreshold[symbol] = {price,ThresholdUp};
    }
};

int main()
{
    MarketFeed feed;

    TickerBoard board("TickerBoard");
    MovingAverageWidget maAAPL("AvgWidget");
    PriceAlert alertAAPLUp("PriceAlert");
    alertAAPLUp.setStockThresholdPrice("AAPL", 200.0, true); // default i took up

    feed.attach(&board);
    feed.attach(&maAAPL);
    feed.attach(&alertAAPLUp);

    std::cout << "\n-- Publish AAPL 198.00 --\n";
    feed.publish("AAPL", 198.0);  // board + MA; alert not yet

    std::cout << "\n-- Publish GOOG 101.00 --\n";
    feed.publish("GOOG", 101.0);  // board only

    std::cout << "\n-- Publish AAPL 198.00 (no change) --\n";
    feed.publish("AAPL", 198.0);  // no notify

    std::cout << "\n-- Publish AAPL 201.00 (crosses 200) --\n";
    feed.publish("AAPL", 201.0);  // board + MA + alert (fires once)

    std::cout << "\n-- Detach MA(AAPL), then AAPL 205.00 --\n";
    feed.detach(&maAAPL);
    feed.publish("AAPL", 205.0);  // board only; alert already fired

    std::cout << "\n-- removeAll(); then AAPL 210.00 --\n";
    feed.removeAll();
    feed.publish("AAPL", 210.0);  // no observers



    return 0;
}