#ifndef PROXYFUNCJMP_H
#define PROXYFUNCJMP_H

#include"ProxyFunc.hpp"

class ProxyFuncJmp : public ProxyFunc
{
public:
    ProxyFuncJmp(void * pTarget, void * pSubstFunc, Logger *logger) :
        ProxyFunc(pTarget, pSubstFunc, logger) {}
    virtual ~ProxyFuncJmp() { if (this->isHookInstalled()) this->removeHook(); }
    virtual bool init();
    virtual bool isHookInstalled();
    virtual bool installHook();
    virtual bool removeHook();

private:
    static const int kJmpInstructionSize = 6;
    unsigned char m_pOriginalCode[kJmpInstructionSize];
    unsigned char m_pJmpCode[kJmpInstructionSize];
};

#endif // PROXYFUNCJMP_H
