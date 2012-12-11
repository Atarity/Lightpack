#ifndef PROXYFUNCJMP_H
#define PROXYFUNCJMP_H

#include"ProxyFunc.hpp"

#define JMP_INSTRUCTION_SIZE 6

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
    char m_pOriginalCode[JMP_INSTRUCTION_SIZE];
    char m_pJmpCode[JMP_INSTRUCTION_SIZE];
};

#endif // PROXYFUNCJMP_H
