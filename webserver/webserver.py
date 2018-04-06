#!/usr/bin/python
import SimpleHTTPServer
import SocketServer

PORT_NUMBER = 80

Handler = SimpleHTTPServer.SimpleHTTPRequestHandler

httpd = SocketServer.TCPServer(("", PORT_NUMBER), Handler)
print 'Started cmmm webserver on port ' , PORT_NUMBER
httpd.serve_forever()
