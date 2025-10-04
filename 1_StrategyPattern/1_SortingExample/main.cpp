#include <iostream>
#include <vector>
#include <memory>

// step1 : Strategy Interface
class SortStrategy
{
public :
    virtual void sort(std::vector<int>& data) = 0;
    virtual ~SortStrategy() = default;
};

// step2 : Concrete Strategies
class QuickSort : public SortStrategy 
{
public :
    void sort(std::vector<int>& data) override 
    {
        std::cout<<"sorting using quick sort"<<std::endl;
        // imagine QuickSort implementation here
    }
};

class MergeSort : public SortStrategy 
{
public :
    void sort(std::vector<int>& data) override 
    {
        std::cout<<"sorting using merge sort"<<std::endl;
        // imagine MergeSort implementation here
    }
};

// Step3 : Context
class Sorter 
{
private :
    std::unique_ptr<SortStrategy> strategy;
public :
    void setStrategy(std::unique_ptr<SortStrategy> s)
    {
        strategy = std::move(s);
    }

    void sortData(std::vector<int>& data)
    {
        if(strategy)
        {
            strategy->sort(data);
        }
        else
        {
            std::cout<<"No sorting strategy set"<<std::endl;
        }
    }
};

int main()
{
    std::vector<int> data = {5,4,3,2,4};

    Sorter sorter;

    sorter.setStrategy(std::make_unique<QuickSort>());
    sorter.sortData(data);

    sorter.setStrategy(std::make_unique<MergeSort>());
    sorter.sortData(data);
    
    return 0;
}