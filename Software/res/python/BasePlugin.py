import sys

class BasePlugin(object):
    def init(self):
        """ default init function """
        
    def name(self):
        """ return the name of the plugin """
        return self.__class__.__name__
 
    def description(self):
        """ return a short description of the plugin """
        return ""

    def author(self):
        """ return the author of the plugin """
        return ""

    def version(self):
        """ return the version of the plugin """
        return ""
        

    def run(self):
        """ default function start"""
        return 0;

    def stop(self):
        """ default function stop"""
        return 0;

    def settings(self):
        """ setting interface function """
        return 0;
