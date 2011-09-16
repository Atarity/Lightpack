Обёртка (wrapper) API 1.0 для LUA
Автор: Владислав Кочемаев
Репозиторий автора: http://code.google.com/p/vladdev/source/browse/#svn%2Ftrunk%2FluaLightpack
Вики: http://code.google.com/p/vladdev/wiki/luaLightpack

Получился практически полный аналог (пока нет маппингов) библиотеки pyLightpack. Имена функций те же самые, в некоторых случаях добавлены возвращаемые значения об успешном выполнении (true или false) операции, т.к. протокол это поддерживает.

Что касается окружения для Lua. Удобнее всего для Windows поставить пакет отсюда: http://code.google.com/p/luaforwindows/, там собрано воедино множество библиотек, есть все нужные для работы luaLightpack.