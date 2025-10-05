#include <iostream>
#include <memory>

// STEP1 : Stratergy Interface
class ImageFilterStratergy
{
public :
    virtual void filter() = 0;
    virtual ~ImageFilterStratergy() = default;
};

// STEP2 : Concrete Strategy
class SepiaFilter : public ImageFilterStratergy
{
public :
    void filter() override 
    {
        std::cout<<"SepiaFiler filtering the image"<<std::endl;
    }
};

class BlackAndWhiteFilter : public ImageFilterStratergy
{
public :
    void filter() override 
    {
        std::cout<<"BlackAndWhiteFilter filtering the image"<<std::endl;
    }
};

class BlurFilter : public ImageFilterStratergy
{
public :
    void filter() override 
    {
        std::cout<<"BlurFilter filtering the image"<<std::endl;
    }
};

//STEP 3: Context
class ImageFilter 
{
    std::unique_ptr<ImageFilterStratergy> stratergy;
public :
    void setFilterStratergy(std::unique_ptr<ImageFilterStratergy> s)
    {
        stratergy = std::move(s);
    }

    void processFilter()
    {
        if(stratergy)
        {
            stratergy->filter();
        }
        else
            std::cout<<"Filter stratergy not set"<<std::endl;
    }
};

int main()
{
    ImageFilter filter;

    filter.setFilterStratergy(std::make_unique<SepiaFilter>());
    filter.processFilter();

    filter.setFilterStratergy(std::make_unique<BlackAndWhiteFilter>());
    filter.processFilter();

    filter.setFilterStratergy(std::make_unique<BlurFilter>());
    filter.processFilter();
    
    return 0;
}