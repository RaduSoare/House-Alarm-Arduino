import smtplib, ssl
import serial

port = 587  # For starttls
smtp_server = "smtp.gmail.com"
sender_email = "srgdevelopement@gmail.com"
receiver_email = "radus932@gmail.com"
password = input("Type your password and press enter: ")
message = """\
Subject: Security issue

Movement was detected in the house."""

def send_email():
	# Start an SMTP connection that is secured using ssl
	context = ssl.create_default_context()
	with smtplib.SMTP(smtp_server, port) as server:
		server.ehlo()  # Can be omitted
		server.starttls(context=context)
		server.ehlo()  # Can be omitted
		server.login(sender_email, password)
		server.sendmail(sender_email, receiver_email, message)
		print('Email sent!')

arduino = serial.Serial('COM5', 9600)

while True:
	data = arduino.readline()[:-2] #the last bit gets rid of the new-line chars
	if data:
		if data.decode('utf-8') == 'Alarm':
			print('Send email!')
			#send_email()
			exit(0)
		else:
			print(data)




