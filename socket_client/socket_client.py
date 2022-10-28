import websocket
import time

ws = websocket.WebSocket()

# Repeat indefinitely
while True:
    # Start a connection to the server
    ws.connect("ws://192.168.4.1/")

    # Ask for the current temperature
    ws.send("May you please give me the current temperature readings?")

    # Wait for the response and read it as a floating point number
    current_temperature = float(ws.recv())

    # Test current temperature
    if current_temperature < 30:
        # Ask for the green LED to be turned on
        ws.send("Please, turn on the green LED.")
    else:
        # Ask for the red LED to be turned on
        ws.send("Please, turn on the red LED.")

    # Print the current temperature
    print(f"🌡️ The current temperature is {current_temperature}°C")

    # Close the connection
    ws.close()

    # Wait for 2 seconds
    time.sleep(2)
