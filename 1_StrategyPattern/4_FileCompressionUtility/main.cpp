#include <iostream>
#include <memory>

// STEP1 : Stratergy Interface
class CompressionStratergy
{
public :
    virtual void compress(std::string fileName) = 0;
    virtual ~CompressionStratergy() = default;
};

// STEP2 : Concrete Stratergy
class ZipCompression : public CompressionStratergy 
{
public :
    void compress(std::string fileName) override 
    {
        std::cout<<"compressing "<<fileName << " using zip format"<<std::endl;
    }
};

class RarCompression : public CompressionStratergy
{
public :
    void compress(std::string fileName) override 
    {
        std::cout<<"compression "<<fileName << " using Rar fomat"<<std::endl;
    }
};

class SevenZCompression : public CompressionStratergy 
{
public :
    void compress(std::string fileName) override 
    {
        std::cout<<"compression "<<fileName<<" using SevenZCompression"<<std::endl;
    }
};

// STEP 3 : context
class FileCompressor
{
    std::unique_ptr<CompressionStratergy> stratergy;
public :
    void setCompressionStratergy(std::unique_ptr<CompressionStratergy> s)
    {
        stratergy = std::move(s);
    }

    void compressFile(const std::string fileName)
    {
        if(stratergy)
        {
            stratergy->compress(fileName);
        }
        else 
        {
            std::cout<<"compression stratergy not set"<<std::endl;
        }
    }
};

int main()
{
    FileCompressor compressor;

    compressor.setCompressionStratergy(std::make_unique<ZipCompression>());
    compressor.compressFile("file1.txt");

    compressor.setCompressionStratergy(std::make_unique<RarCompression>());
    compressor.compressFile("file2.txt");

    compressor.setCompressionStratergy(std::make_unique<SevenZCompression>());
    compressor.compressFile("file3.txt");
    
    return 0;
}