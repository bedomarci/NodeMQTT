#ifndef FINITESTATEMACHINEINTERFACE_H
#define FINITESTATEMACHINEINTERFACE_H

#include "_NodeInterface.hpp"
#include "LinkedList.h"

struct State
{
    int stateId = -1;
    const char *stateName;
    void (*onState)();
    void (*onEnter)();
    void (*onExit)();
    State(int stateId, const char *stateName, void (*onState)(), void (*onEnter)(), void (*onExit)())
        : stateId(stateId),
          stateName(stateName),
          onState(onState),
          onEnter(onEnter),
          onExit(onExit){};
    State() : stateId(-1), stateName(nullptr), onState(nullptr), onEnter(nullptr), onExit(nullptr){};
};

class FiniteStateMachineInterface : public NodeInterface<int>
{
  public:
    FiniteStateMachineInterface(String topic);
    FiniteStateMachineInterface(String publishTopic, String subscribeTopic);
    void addState(uint8_t stateId, const char *stateName, void (*onState)() = nullptr, void (*onEnter)() = nullptr, void (*onExit)() = nullptr, bool initState = false);
    void addState(State *state, bool initState = false);
    void addTransition(int stateIdFrom, int stateIdTo, int event, void (*onTransition)() = nullptr);
    void writeByName(const char *nextStateName);
    void trigger(int event);
    void init() override;
    String valueToString() override;

  protected:
    State parsedValue;
    int fromJSON(JsonObject &rootObject) override;
    JsonObject &toJSON(int value, JsonObject &root) override;
    int cmp(int oldValue, int newValue) override;
    struct Transition
    {
        uint8_t event;
        uint8_t stateFromId;
        uint8_t stateToId;
        void (*onTransition)();
        Transition(uint8_t event, uint8_t stateFromId, uint8_t stateToId, void (*onTransition)())
            : event(event), stateFromId(stateFromId), stateToId(stateToId), onTransition(onTransition){};
    };
    void updatePhisicalInterface(int newValue) override;
    State *getStateByName(const char *stateName);
    State *getStateById(int stateId);
    LinkedList<State *> states;
    LinkedList<Transition *> transitions;
    int currentStateId = -1;
    bool initialized = false;
    void makeTransition(int nextStateId);
};

FiniteStateMachineInterface::FiniteStateMachineInterface(String publishTopic, String subscribeTopic)
    : NodeInterface<int>(publishTopic, subscribeTopic)
{
    states = LinkedList<State *>();
    transitions = LinkedList<Transition *>();
    this->setSamplingEnabled(false);
    this->setMQTTPublish(false);
    this->setMQTTSubscribe(true);
    this->setInterfaceName(FSM_NAME);
}
FiniteStateMachineInterface::FiniteStateMachineInterface(String topic)
    : FiniteStateMachineInterface(topic, topic)
{
}
inline void FiniteStateMachineInterface::init()
{
    if (currentStateId > -1)
    {
        this->write(currentStateId, false);
    }
    else if (states.size() > 0)
    {
        currentStateId = states.get(0)->stateId;
        this->init();
    }
    else
    {
        fatal(F("Could not initialize final state machine! Machine contains zero state."));
    }
    initialized = true;
}

inline String FiniteStateMachineInterface::valueToString()
{
    return String(this->getStateById(currentStateId)->stateName);
}
inline void FiniteStateMachineInterface::updatePhisicalInterface(int newValue)
{
    this->makeTransition(newValue);
}
inline void FiniteStateMachineInterface::writeByName(const char *nextStateName)
{
    int nextStateId = this->getStateByName(nextStateName)->stateId;
    this->write(nextStateId);
}
inline void FiniteStateMachineInterface::makeTransition(int nextStateId)
{
    State *currentState = this->getStateById(currentStateId);
    State *nextState = this->getStateById(nextStateId);

    Transition *t;
    if (nextStateId != this->currentStateId)
    {
        //LEAVING PREVIOUS STATE
        if (currentState && currentState->onExit != nullptr)
        {
            currentState->onExit();
        }

        //TRANSITION
        for (int i = 0; i < transitions.size(); i++)
        {
            t = transitions.get(i);
            if (t->stateFromId == this->currentStateId &&
                t->stateToId == nextStateId)
            {
                if (t->onTransition != nullptr)
                {
                    t->onTransition();
                }
                break;
            }
        }
    }

    bool sameState = nextStateId == this->currentStateId;
    this->currentStateId = nextStateId;

    //ENTERING NEW STATE
    if ((!sameState || !this->hasInitializedValue()) && nextState->onEnter != nullptr)
    {
        nextState->onEnter();
    }

    //CURRENT STATE
    if (nextState->onState != nullptr)
    {
        nextState->onState();
    }
}
inline void FiniteStateMachineInterface::addState(uint8_t stateId, const char *stateName, void (*onState)(), void (*onEnter)(), void (*onExit)(), bool initState)
{
    State *newState = new State(stateId, stateName, onState, onEnter, onExit);
    this->addState(newState, initState);
}
inline void FiniteStateMachineInterface::addState(State *state, bool initState)
{
    if (initState)
        currentStateId = state->stateId;

    states.add(state);
}
inline void FiniteStateMachineInterface::addTransition(int event, int stateFromId, int stateToId, void (*onTransition)())
{
    State *stateFrom = this->getStateById(stateFromId);
    State *stateTo = this->getStateById(stateToId);
    if (stateFrom == nullptr || stateTo == nullptr)
    {
        return;
        //e(F("States cannot be NULL! Transition has been discarded."));
    }
    transitions.add(new Transition(event, stateFromId, stateToId, onTransition));
}
inline void FiniteStateMachineInterface::trigger(int event)
{
    if (initialized)
    {
        Transition *t;
        for (int i = 0; i < transitions.size(); i++)
        {
            t = transitions.get(i);
            if (t->stateFromId == this->currentStateId &&
                t->event == event)
            {
                this->write(t->stateToId);
                return;
            }
        }
    }
}

inline int FiniteStateMachineInterface::fromJSON(JsonObject &rootObject)
{
    String stateName = rootObject["data"];
    int stateIndex = this->getStateByName(stateName.c_str())->stateId;
    return (stateIndex >= 0) ? stateIndex : currentValue;
}

inline JsonObject &FiniteStateMachineInterface::toJSON(int value, JsonObject &root)
{
    root["data"] = this->getStateById(value)->stateName;
    return root;
}

inline int FiniteStateMachineInterface::cmp(int oldValue, int newValue)
{
    return -1;
}

inline State *FiniteStateMachineInterface::getStateByName(const char *stateName)
{
    for (int i = 0; i < states.size(); i++)
    {
        State *state = states.get(i);
        if (strcmp(state->stateName, stateName) == 0)
            return state;
    }
    Logger.logf(ERROR, MSG_UNKNOWN_STATE, stateName);
    return nullptr;
}

inline State *FiniteStateMachineInterface::getStateById(int stateId)
{
    for (int i = 0; i < states.size(); i++)
    {
        State *state = states.get(i);
        if (stateId == state->stateId)
            return state;
    }
    Logger.logf(ERROR, MSG_UNKNOWN_STATE, stateId);
    return nullptr;
}

#endif //FINITESTATEMACHINEINTERFACE_H