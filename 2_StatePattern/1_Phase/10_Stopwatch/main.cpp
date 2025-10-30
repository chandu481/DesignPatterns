#include <iostream>
#include <memory>
#include <vector>

class Stopwatch;

// step1 : state interface
class State 
{
public :
    virtual void showState(Stopwatch& s) = 0;
    virtual void onEntry(Stopwatch& s) { showState(s); }
    virtual void onStart(Stopwatch& s) {std::cout<<"Invalid in this state"<<std::endl; }
    virtual void onPause(Stopwatch& s) {std::cout<<"Invalid in this state"<<std::endl; }
    virtual void onResume(Stopwatch& s) {std::cout<<"Invalid in this state"<<std::endl; }
    virtual void onReset(Stopwatch& s) {std::cout<<"Invalid in this state"<<std::endl; }
    virtual void onTick(Stopwatch& s) {std::cout<<"Invalid in this state"<<std::endl; }
    virtual void onLap(Stopwatch& s) {std::cout<<"Invalid in this state"<<std::endl; }

    virtual ~State() = default;
 };

 // step 2 : concrete state
class Idle : public State 
{
public :
    void showState(Stopwatch& s) override {std::cout<<"IDLE"<<std::endl;}
    //void onEntry(Stopwatch& s) override { showState(s); }
    void onStart(Stopwatch& s) override;
};

class Running : public State 
{
public :
    void showState(Stopwatch& s) override {std::cout<<"RUNNING"<<std::endl;}
    void onPause(Stopwatch& s) override;
    void onTick(Stopwatch& s) override;
    void onLap(Stopwatch& s) override;
};

class Pause : public State 
{
public :
    void showState(Stopwatch& s) override {std::cout<<"PAUSE"<<std::endl; }
    void onResume(Stopwatch& s) override;
    void onReset(Stopwatch& s) override;
};

// step 3 : context
class Stopwatch 
{
    std::unique_ptr<State> state;
public :
    int elapsedTime = 0;
    std::vector<int> recordTime;
    Stopwatch();
    void showState()
    {
        state->showState(*this);
    }

    void setState(std::unique_ptr<State> newState)
    {
        state = std::move(newState);
        state->onEntry(*this);
    }

    void start()
    {
        state->onStart(*this);
    }

    void pause()
    {
        state->onPause(*this);
    }

    void resume()
    {
        state->onResume(*this);
    }

    void reset()
    {
        state->onReset(*this);
    }

    void tick()
    {
        state->onTick(*this);
    }

    void lap()
    {
        state->onLap(*this);
    }
};
Stopwatch::Stopwatch()
{
    state = std::make_unique<Idle>();
    state->onEntry(*this);
}

void Idle::onStart(Stopwatch& s)
{
    s.setState(std::make_unique<Running>());
}

void Running::onPause(Stopwatch& s)
{
    s.setState(std::make_unique<Pause>());
}
void Running::onTick(Stopwatch& s)
{
    s.elapsedTime += 100;
    std::cout<<"Time : "<< s.elapsedTime<<std::endl;
}
void Running::onLap(Stopwatch& s)
{
    s.recordTime.push_back(s.elapsedTime);
}

void Pause::onResume(Stopwatch& s)
{
    s.setState(std::make_unique<Running>());
}
void Pause::onReset(Stopwatch& s)
{
    s.setState(std::make_unique<Idle>());
    s.elapsedTime = 0;
}

int main()
{
    Stopwatch sw;

     sw.start();
    sw.tick(); sw.tick();
    sw.pause();
    sw.resume();
    sw.tick();
    sw.pause();

    std::cout << "---\n";

    // 2) Reset flow
    sw.reset();
    sw.start();
    sw.tick();
    sw.pause();
    sw.reset();

    std::cout << "---\n";

    // 3) Laps
    sw.start();
    sw.tick();
    sw.lap();
    sw.tick();
    sw.lap();
    sw.pause();

    for(int i=0; i<sw.recordTime.size(); i++)
    {
        std::cout<<(i+1)<<" lap : "<<sw.recordTime[i]<<std::endl;
    }

    return 0;
}