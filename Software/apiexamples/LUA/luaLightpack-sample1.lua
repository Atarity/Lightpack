require("luaLightpack")

lp = lightpack("127.0.0.1", 3636)
if lp:connect()
  then
    print("Version: " .. lp.Version)
    print("Status: " .. lp:getStatus())
    print("Profiles: " .. lp:getProfiles())
	if lp:lock()
	  then
	    lp:setProfile("Lightpack")
		lp:setSmooth(200)
	    while true do
          os.execute("sleep "..2)
          lp:setColorToAll(0, 0, 255)
		  os.execute("sleep "..2)
          lp:setColorToAll(0, 0, 0)
		end
		lp:unlock()
        lp:disconnect()
    end
  else
    print("Not connected!")
end
