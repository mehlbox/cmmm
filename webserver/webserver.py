#!/usr/bin/python
import SimpleHTTPServer
import SocketServer

PORT_NUMBER = 80

try:
	Handler = SimpleHTTPServer.SimpleHTTPRequestHandler
	httpd = SocketServer.TCPServer(("", PORT_NUMBER), Handler)
	print 'Started cmmm webserver on port ' , PORT_NUMBER
	httpd.serve_forever()

except KeyboardInterrupt:
	httpd.socket.close()
