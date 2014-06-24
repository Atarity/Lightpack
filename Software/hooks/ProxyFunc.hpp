#ifndef PROXYFUNC_HPP
#define PROXYFUNC_HPP

#include "LoggableTrait.hpp"

class ProxyFunc : public LoggableTrait
{
public:
    ProxyFunc(void *pTarget, void *pSubstFunc, Logger *logger): LoggableTrait(logger)
    {
        m_pTarget = pTarget;
        m_pSubstFunc = pSubstFunc;
        m_isInited = false;
    }

    virtual bool init()=0;
    virtual bool isHookInstalled()=0;
    virtual bool installHook()=0;
    virtual bool removeHook()=0;


protected:
    void *m_pTarget;
    void *m_pSubstFunc;
    bool m_isInited;
};

#endif // PROXYFUNC_HPP
