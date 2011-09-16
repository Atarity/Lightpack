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
		i = 1
		lp:setColorToAll(0, 0, 0)
	    while true do
          os.execute("sleep "..1)
		  if i - 1 == 0
		  then
		    lp:setColor(10, 0, 0, 0)
		  else
		    lp:setColor(i - 1, 0, 0, 0)
		  end
          lp:setColor(i, 255, 0, 0)
		  i=i+1
		  if i>10 then i = 1 end
		end
		lp:unlock()
        lp:disconnect()
    end
  else
    print("Not connected!")
end
