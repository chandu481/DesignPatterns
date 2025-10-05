#include <iostream>
#include <memory>

//STEP1 : Stratergy Interface
class NotificationStratergy
{
public :
    virtual void send(const std::string& msg) = 0;
    virtual ~NotificationStratergy() = default;
};

// STEP2 : Concrete Stratergy
class EmailNotification : public NotificationStratergy
{
    std::string email;
public :
    EmailNotification(std::string emailAvg) : email(emailAvg) {}
    void send(const std::string& msg)
    {
        std::cout<<"Sending Email Notification Msg  : "<< msg << " to email : "<<email<<std::endl;
    }
};

class SMSNotification : public NotificationStratergy
{
    std::string phoneNumber;
public :
    SMSNotification(std::string phoneNumberAvg) : phoneNumber(phoneNumberAvg) {}
    void send(const std::string& msg) override
    {
        std::cout<<"Sending SMS Notificaiton Msg : "<<msg<<" to PhoneNumber : "<<phoneNumber<<std::endl;
    }
};

class PushNotification : public NotificationStratergy
{
public :
    void send(const std::string& msg) override 
    {
        std::cout<<"Push notification msg : "<<msg<<std::endl;
    }
};

//STEP 3 Context
class Notifier 
{
    std::unique_ptr<NotificationStratergy> stratergy;
public :
    void setNotifier(std::unique_ptr<NotificationStratergy> s)
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

    notifier.setNotifier(std::make_unique<EmailNotification>("charan481@gmail.com"));
    notifier.notify("this is testing msg1");

    notifier.setNotifier(std::make_unique<SMSNotification>("93929394059"));
    notifier.notify("this is testing msg2 ");

    notifier.setNotifier(std::make_unique<PushNotification>());
    notifier.notify("this is testing msg3");
    
    return 0;
}