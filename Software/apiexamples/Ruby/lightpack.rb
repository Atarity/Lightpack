# coding: utf-8

# Title:       LightPack API wrapper for Ruby
# Author:      Mikhail S. Pobolovets
# License:     GPL v3
# Date:        2011/08/15
# API Version: 5.5

require 'socket'

class LightPack
  include Socket::Constants

  #  host = '127.0.0.1'                # The remote host
  #  port = 3636                       # The same port as used by the server
  #  led_map = [1,2,3,4,5,6,7,8,9,10]  # mapped LEDs

  def initialize(host, port, led_map)
    @host = host
    @port = port
    @led_map = led_map
  end

  def connect
    # Try to connect to the API server
    @connection = Socket.new(AF_INET, SOCK_STREAM, 0)
    sockaddr = Socket.pack_sockaddr_in(@port, @host)
    @connection.connect(sockaddr)
    read_result
  rescue
    puts "Lightpack API server is missing"
    exit 0
  end

  # Return last-command API answer
  def read_result
    @connection.recv(8192).to_s
  end

  # Execute command and recieve an answer
  def exec_cmd(cmd)
    @connection.send "#{cmd}\n"
    read_result
  end

  def get_profiles
    exec_cmd('getprofiles').split(':')[1].rstrip(';\n').split(';')
  end

  def get_profile
    exec_cmd('getprofile').split(':')[1]
  end

  def get_status
    exec_cmd('getstatus').split(':')[1]
  end

  def get_api_status
    exec_cmd('getstatusapi').split(':')[1]
  end

  # Set color to the define LED
  def set_color(n, r, g, b)
    exec_cmd "setcolor:#{@led_map[n-1]}-#{r},#{g},#{b}"
  end

  # Set one color to all LEDs
  def set_color_to_all(r, g, b)
    cmd = "setcolor:"
    @led_map.each {|i| cmd += "#{i}-#{r},#{g},#{b};" }
    exec_cmd cmd
  end

  def set_gamma(g)
    exec_cmd "setgamma:#{g}"
  end

  def set_smooth(s)
    exec_cmd "setsmooth:#{s}"
  end

  def set_profile(p)
    exec_cmd "setprofile:#{p}"
  end

  def lock
    exec_cmd 'lock'
  end

  def unlock
    exec_cmd 'unlock'
  end

  def turn_on
    exec_cmd 'setstatus:on'
  end

  def turn_off
    exec_cmd 'setstatus:off'
  end

  def disconnect
    unlock
    @connection.close()
  end

end

