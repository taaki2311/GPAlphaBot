from picamera import PiCamera
from socket import *
from io import BytesIO

WIDTH = 128
HEIGHT = 160
BUFFER_SIZE = 1024
HOST = ('192.168.11.229', 55555)

camera = PiCamera(resolution = (WIDTH, HEIGHT))
stream = BytesIO()
try:
	with socket(AF_INET, SOCK_DGRAM) as sock:
		sock.connect(HOST)
		for _ in camera.capture_continuous(stream, 'jpeg'):
			stream.seek(0)
			data = stream.read(BUFFER_SIZE)
			while data:
				sock.sendall(data)
				data = stream.read(BUFFER_SIZE)
			stream.seek(0)
except KeyboardInterrupt:
	pass
