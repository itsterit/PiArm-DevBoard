from pymodbus.client import ModbusSerialClient
import matplotlib.pyplot as plt
import serial
import time
import numpy as np
from scipy.integrate import simps

client = ModbusSerialClient(method='rtu', port='COM8', baudrate=9600)  # Укажите нужный COM порт и настройки подключения
connection = client.connect()
print("Starting...") 


if connection:
    while 1:
        while 1:
            # Подача сигнала в катушку
            client.write_register(address=0x0000, value=0x001, unit=1)
            time.sleep(2.5)
            # Смотрим количество сэмплов
            result = client.read_input_registers(address=0x0009, count=1, unit=1)
            if not result.isError():
                samples_amount = result.registers
                print("Количество сэмплов: ", samples_amount) 
                break

        values = []
        counter = 0
        while counter < 100:
            result = client.read_input_registers(counter+10, 1, unit=1)  
            if not result.isError():
                values.append(result.registers[0])
                counter = counter + 1
            else:
                print(".") 
            time.sleep(0.1)
        client.close()


        plt.plot(range(100), values)
        plt.xlabel('Регистр')
        plt.ylabel('Значение')
        plt.title('Сигнал катушки')
        plt.show()
else:
    print('Connection lost, Try again')
