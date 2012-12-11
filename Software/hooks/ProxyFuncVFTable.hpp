#ifndef PROXYFUNCVFTABLE_H
#define PROXYFUNCVFTABLE_H

#include"ProxyFunc.hpp"
#include<stdint.h>

class ProxyFuncVFTable : public ProxyFunc
{
public:
    ProxyFuncVFTable(void * pTarget, void *pSubstFunc, Logger *logger): ProxyFunc(pTarget, pSubstFunc, logger) {}
    virtual ~ProxyFuncVFTable() { if (this->isHookInstalled()) this->removeHook(); }
    virtual bool init();
    virtual bool isHookInstalled() {
        return m_isInited && *(reinterpret_cast<uintptr_t *>(m_pTarget)) != reinterpret_cast<uintptr_t>(m_pOriginalFunc); }

    virtual bool installHook();
    virtual bool removeHook();
    void *getOriginalFunc() { return m_pOriginalFunc; }
private:
    void *m_pOriginalFunc;
};

#endif // PROXYFUNCVFTABLE_H
