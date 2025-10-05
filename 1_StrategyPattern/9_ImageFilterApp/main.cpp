#include <iostream>
#include <memory>

struct Image
{
    std::string name;
    Image(std::string nameArg) : name(nameArg) {}
};

// STEP1 : Stratergy Interface
class ImageFilterStratergy
{
public :
    virtual void apply(Image& img) = 0;
    virtual ~ImageFilterStratergy() = default;
};

// STEP2 : Concrete Strategy
class SepiaFilter : public ImageFilterStratergy
{
public :
    void apply(Image& img) override 
    {
        std::cout<<"SepiaFiler filtering the image"<<img.name<<std::endl;
    }
};

class BlackAndWhiteFilter : public ImageFilterStratergy
{
public :
    void apply(Image& img) override 
    {
        std::cout<<"BlackAndWhiteFilter filtering the image"<<img.name<<std::endl;
    }
};

class BlurFilter : public ImageFilterStratergy
{
public :
    void apply(Image& img) override 
    {
        std::cout<<"BlurFilter filtering the image "<<img.name<<std::endl;
    }
};

//STEP 3: Context
class ImageProcessor 
{
    std::unique_ptr<ImageFilterStratergy> stratergy;
public :
    void setStratergy(std::unique_ptr<ImageFilterStratergy> s)
    {
        stratergy = std::move(s);
    }

    void process(Image& img)
    {
        if(stratergy)
        {
            stratergy->apply(img);
        }
        else
            std::cout<<"Filter stratergy not set"<<std::endl;
    }
};

int main()
{
    Image photo("holiday.jpg");
    ImageProcessor imgProcessor;

    imgProcessor.setStratergy(std::make_unique<SepiaFilter>());
    imgProcessor.process(photo);

    imgProcessor.setStratergy(std::make_unique<BlackAndWhiteFilter>());
    imgProcessor.process(photo);

    imgProcessor.setStratergy(std::make_unique<BlurFilter>());
    imgProcessor.process(photo);
    
    return 0;
}