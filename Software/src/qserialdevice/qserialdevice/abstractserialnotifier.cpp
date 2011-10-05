#include "abstractserialnotifier.h"
#include "nativeserialnotifier.h"


AbstractSerialNotifier *AbstractSerialNotifier::createSerialNotifier(NativeSerialEngine *parent)
{
    return new NativeSerialNotifier(parent);
}

void AbstractSerialNotifier::deleteSerialNotifier(AbstractSerialNotifier *notifier)
{
    if (notifier) {
        NativeSerialNotifier *nnotifier = reinterpret_cast <NativeSerialNotifier *> (notifier);
        if (nnotifier)
            delete nnotifier;
    }
}
