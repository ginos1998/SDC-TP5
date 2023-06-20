import tkinter as tk
import time
import math

file_path = "/home/ginos/sensor"
# file_path = "/dev/my_gpio_device"
lbl_borracho = "hoy te portaste bien pibe"


def botonHandler():
    root.after(1000, lambda: etiqueta.config(text="¡Presiona el botón... AHORAAA!"))
    root.after(1040, lambda: escribirArchivo("0"))


def sensorHandler():
    escribirArchivo("1")


def procesarBoton(x, tiempo):
    etiqueta.config(text="")
    root.update_idletasks()  # Actualizar la ventana para que la etiqueta desaparezca de inmediato
    tiempo = round(tiempo, 2)
    if x == "-1" or x == -1:
        etiqueta.config(text="UPS! algo salio mal..")
    elif x == "0" or x == 0:
        etiqueta.config(text=lbl_borracho)
    elif x == "1" or x == 1:
        etiqueta.config(text="se han tomado todo el vino ohohohh! demoraste {} [s]".format(tiempo))

    root.after(3000, lambda: etiqueta.config(text=""))  # Eliminar el texto después de 3 segundos


def procesarSensor(x):
    etiqueta.config(text="")
    root.update_idletasks()  # Actualizar la ventana para que la etiqueta desaparezca de inmediato

    if x == "-1" or x == -1:
        etiqueta.config(text="UPS! algo salio mal..")
    elif x == "0" or x == 0:
        etiqueta.config(text="se han tomado todo el vino ohohohh")
    elif x == "1" or x == 1:
        etiqueta.config(text=lbl_borracho)

    root.after(3000, lambda: etiqueta.config(text=""))  # Eliminar el texto después de 3 segundos


def escribirArchivo(opt):
    with open(file_path, "w") as file:
        file.write(opt)   # escribimos en el archivo la opcion seleccionada
        file.close()

    leerArchivo(opt)


def leerArchivo(lectura):
    estado = "1"
    time_aux = 0
    start_time = time.time()
    entrada = ""
    while time.time() - start_time < 5:
        with open(file_path, "r") as file:
            content = file.read().strip()
            entrada = content
            if content.startswith("BOTON:") or content.startswith("ALCOHOLIMETRO:"):
                estado = content.split(":")[1].strip()
                if estado == "0":
                    time_aux = time.time() - start_time

                if estado == "0" and content.startswith("ALCOHOLIMETRO:"):
                    break

                if estado == "1" and content.startswith("BOTON:"):
                    break

        time.sleep(0.1)

    file.close()
    time_aux -= 0.040
    if lectura == "0":  # si apreto el boton y supero el tiempo de rta (1 s)
        estado = "1" if time_aux > 1 else "0"

    if entrada.startswith("BOTON:"):
        procesarBoton(estado, time_aux)
    elif entrada.startswith("ALCOHOLIMETRO:"):
        procesarSensor(estado)


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