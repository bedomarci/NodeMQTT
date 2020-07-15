#ifndef LANGUAGEINTERFACE_H
#define LANGUAGEINTERFACE_H

#include "DataInterface.hpp"

#define LANG_HUNGARIAN 0
#define LANG_ENGLISH 1

class LanguageInterface : public DataInterface<uint8_t>
{
  public:
    LanguageInterface();
    void init();

  protected:
    // void updatePhisicalInterface(int newValue);
};

inline LanguageInterface::LanguageInterface()
    : DataInterface<uint8_t>(LANGUAGE_TOPIC, LANGUAGE_TOPIC)
{
}

void LanguageInterface::init()
{
    this->setInterfaceName(LANGUAGE_NAME);
    this->setSamplingEnabled(false);
    this->setMQTTSubscribe(true);
    this->setMQTTPublish(false);
}

#endif //LANGUAGEINTERFACE_H