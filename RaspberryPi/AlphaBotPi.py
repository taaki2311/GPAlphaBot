from picamera import PiCamera
from socket import *

WIDTH = 128
HEIGHT = 160
BUFFER_SIZE = 1024
HOST = ('192.168.11.229', 55555)

camera = PiCamera(resolution = (WIDTH, HEIGHT))

try:
	with socket(AF_INET, SOCK_DGRAM) as sock:
		sock.connect(HOST)
		while True:
			camera.capture('stream.jpg', 'jpeg')
			with open('stream.jpg', 'rb') as stream:
				data = stream.read(BUFFER_SIZE)
				while(data):
					sock.send(data)
					data = stream.read(BUFFER_SIZE)
except KeyboardInterrupt:
	pass
