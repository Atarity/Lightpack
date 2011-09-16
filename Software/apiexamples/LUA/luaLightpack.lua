--[[

  luaLightpack 1.0
  
  Данный код распространяется по лицензии GNU GPL v3.
  Автор: Кочемаев Владислав (VladDev).

  luaLightpack -- это набор определений и методов для языка Lua, 
  которые сильно упрощают процесс взаимодействия с API и написание плагинов для Лайтпака.
  
  Название функций и принимаемые данные полностью соответствуют библиотеке pyLightpack,
  возвращаемые значения в некоторых случаях содержат флаг успешного выполнения (true или false).
  
  В данной версии нет поддержки маппинга! Будет введено в следующей версии!
  
  Для правильной работы библиотеки необходимы пакеты classlib и socket для Lua.
]]--

require ("classlib")
socket = require ("socket")

class.lightpack()

function lightpack:__init(host, port, ledMap)
  self.Host = host
  self.Port = port
  self.LedMap = ledMap
end

function lightpack:__init(host, port)
  self.Host = host
  self.Port = port
  self.LedMap = {1,2,3,4,5,6,7,8,9,10}
end

function lightpack:connect()
  self.Socket = socket.tcp()
  self.Socket:connect(self.Host, self.Port)
  self.Version,_,_,_ = self.Socket:receive('*l')
  self.Version = self.Version:sub(9)
  if self.Version == "1.0"
    then return true
    else return false
  end
end

function lightpack:disconnect()
  self.Socket:close()
  return true
end

function lightpack:getStatus()
  self.Socket:send("getstatus\r\n")
  status,_,_,_ = self.Socket:receive('*l')
  status = status:sub(8)
  return status
end

function lightpack:getAPIStatus()
  self.Socket:send("getstatusapi\r\n")
  apistatus,_,_,_ = self.Socket:receive('*l')
  apistatus = apistatus:sub(11)
  return apistatus
end

function lightpack:getProfile()
  self.Socket:send("getprofile\r\n")
  profile,_,_,_ = self.Socket:receive('*l')
  profile = profile:sub(9)
  return profile
end

function lightpack:getProfiles()
  self.Socket:send("getprofile\r\n")
  profiles,_,_,_ = self.Socket:receive('*l')
  profiles = profiles:sub(9)
  return profiles
end

function lightpack:lock()
  self.Socket:send("lock\r\n")
  res,_,_,_ = self.Socket:receive('*l')
  if res == "lock:success"
    then return true
    else return false
  end
end

function lightpack:unlock()
  self.Socket:send("unlock\r\n")
  res,_,_,_ = self.Socket:receive('*l')
  if res == "unlock:success"
    then return true
    else return false
  end
end

function lightpack:turnOn()
  self.Socket:send("setstatus:on\r\n")
  res,_,_,_ = self.Socket:receive('*l')
  if res == "setstatus:ok"
    then return true
    else return false
  end
end

function lightpack:turnOff()
  self.Socket:send("setstatus:off\r\n")
  res,_,_,_ = self.Socket:receive('*l')
  if res == "setstatus:ok"
    then return true
    else return false
  end
end

function lightpack:setProfile(profile)
  self.Socket:send("setprofile:"..profile.."\r\n")
  res,_,_,_ = self.Socket:receive('*l')
  if res == "setprofile:set " .. profile
    then return true
    else return false
  end
end


function lightpack:setColor(n, r, g, b)
  self.Socket:send("setcolor:"..n.."-"..r..","..g..","..b.."\r\n")
  res,_,_,_ = self.Socket:receive('*l')
  if res == "setcolor:ok"
    then return true
    else return false
  end
end

function lightpack:setColorToAll(r, g, b)
  self.Socket:send("setcolor:1-"..r..","..g..","..b..";2-"..r..","..g..","..b..";3-"..r..","..g..","..b..";4-"..r..","..g..","..b..";5-"..r..","..g..","..b..";6-"..r..","..g..","..b..";7-"..r..","..g..","..b..";8-"..r..","..g..","..b..";9-"..r..","..g..","..b..";10-"..r..","..g..","..b..";\r\n")
  res,_,_,_ = self.Socket:receive('*l')
  if res == "setcolor:ok"
    then return true
    else return false
  end
end

function lightpack:setGamma(gamma)
  self.Socket:send("setgamma:"..gamma.."\r\n")
  res,_,_,_ = self.Socket:receive('*l')
  if res == "setgamma:ok"
    then return true
    else return false
  end
end

function lightpack:setSmooth(smooth)
  self.Socket:send("setsmooth:"..smooth.."\r\n")
  res,_,_,_ = self.Socket:receive('*l')
  if res == "setsmooth:ok"
    then return true
    else return false
  end
end
