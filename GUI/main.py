import tkinter as tk
import time
import random

lectura = "-1"
file_path = "/dev/my_gpio_device"


def botonHandler():
    root.update_idletasks()  # Actualizar la ventana para que la etiqueta desaparezca de inmediato
    lectura = "0"
    etiqueta.config(text="presiona el boton...")
    num = random.randint(0, 3)

    while num > 0:
        time.sleep(1)
        num -= 1

    etiqueta.config(text="presiona el boton...AHORAAA!")
    escribirArchivo(lectura)


def sensorHandler():
    lectura = "1"
    escribirArchivo(lectura)


def procesarArchivo(x):
    etiqueta.config(text="")
    root.update_idletasks()  # Actualizar la ventana para que la etiqueta desaparezca de inmediato

    if x == "-1" or x == -1:
        etiqueta.config(text="UPS! algo salio mal..")
    elif x == "0" or x == 0:
        etiqueta.config(text="como te tiene tu jermuu")
    elif x == "1" or x == 1:
        etiqueta.config(text="se han tomado todo el vino ohohohh")

    root.after(3000, lambda: etiqueta.config(text=""))  # Eliminar el texto después de 3 segundos


def escribirArchivo(opt):
    with open(file_path, "w") as file:
        file.write(opt)   # escribimos en el archivo la opcion seleccionada
        file.close()

    leerArchivo()


def leerArchivo():
    estado = 1
    estado_aux = 0
    time_aux = 0
    start_time = time.time()
    while time.time() - start_time < 5:
        with open(file_path, "r") as file:
            content = file.read().strip()
            if content.startswith("BOTON:") or content.startswith("ALCOHOLIMETRO:"):
                estado = content.split(":")[1].strip()

                if estado == 0:
                    time_aux = time.time() - start_time

                if estado == 1:
                    estado_aux = 1

        time.sleep(1)

    file.close()

    estado = estado_aux

    if lectura == 0 and estado == 1:  # si apreto el boton y supero el tiempo de rta (0.5 s)
        estado = 1 if time_aux > 0.5 else 0

    procesarArchivo(estado)


root = tk.Tk()
root.geometry("400x400")
root.title("TEST DE ALCOHOLEMIA")

etiqueta_titulo = tk.Label(root, text="ALCOHOLIMETRO", font=("Helvetica", 16, "bold"))
etiqueta_titulo.pack(pady=20)

boton_sensor = tk.Button(root, text="SENSOR", width=10, command=sensorHandler)
boton_sensor.pack()

boton_boton = tk.Button(root, text="BOTON", width=10, command=botonHandler)
boton_boton.pack()

etiqueta = tk.Label(root, text="")
etiqueta.pack()

root.mainloop()