#include <iostream>
#include <memory>

//STEP1 : Stratergy Interface
class NotificationStratergy
{
public :
    virtual bool send(const std::string& msg) = 0;
    virtual ~NotificationStratergy() = default;
};

// STEP2 : Concrete Stratergy
class EmailNotification final : public NotificationStratergy
{
    std::string email;
public :
    EmailNotification(std::string emailAvg) : email(emailAvg) {}
    bool send(const std::string& msg) override 
    {
        if(email.find('@') == std::string::npos)
        {
            std::cerr<<"[Email] invalid address : "<<email<<std::endl;
            return false;
        }
        std::cout<<"Sending Email Notification Msg  : "<< msg << " to email : "<<email<<std::endl;
        return true;
    }
};

class SMSNotification : public NotificationStratergy
{
    std::string phoneNumber;
public :
    SMSNotification(std::string phoneNumberAvg) : phoneNumber(phoneNumberAvg) {}
    bool send(const std::string& msg) override
    {
        if(phoneNumber.empty() == true || isdigit(static_cast<unsigned char>(phoneNumber[0])) == false)
        {
            std::cerr<<"PhoneNumber is invalid : "<<phoneNumber<<std::endl;
            return false;
        }
        std::cout<<"Sending SMS Notificaiton Msg : "<<msg<<" to PhoneNumber : "<<phoneNumber<<std::endl;
        return true;
    }
};

class PushNotification : public NotificationStratergy
{
public :
    bool send(const std::string& msg) override 
    {
        std::cout<<"Push notification msg : "<<msg<<std::endl;
        return true;
    }
};

//STEP 3 Context
class Notifier 
{
    std::unique_ptr<NotificationStratergy> stratergy;
public :
    void setStratergy(std::unique_ptr<NotificationStratergy> s)
    {
        stratergy = std::move(s);
    }

    void notify(const std::string& msg) 
    {
        if(stratergy)
        {
            stratergy->send(msg);
        }
        else 
        {
            std::cout<<"stratergy not set"<<std::endl;
        }
    }
};

int main()
{
    Notifier notifier;

    notifier.setStratergy(std::make_unique<EmailNotification>("charan481@gmail.com"));
    notifier.notify("this is testing msg1");

    notifier.setStratergy(std::make_unique<SMSNotification>("93929394059"));
    notifier.notify("this is testing msg2 ");

    notifier.setStratergy(std::make_unique<PushNotification>());
    notifier.notify("this is testing msg3");
    
    return 0;
}