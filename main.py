from gpustat import new_query
from os import name
from humanize import naturalsize
from time import sleep
import json
import serial
import psutil
import serial.tools.list_ports


def port_detect():
    for info in serial.tools.list_ports.comports(include_links=True):
        if name == 'posix':
            if 'n/a' not in info:
                return info[0]
        else:
            if "Arduino" in info.description:
                return info[0]


def get_gpu():
    stats = new_query()
    gpu = stats.gpus[0]

    gpu_data = {
        "usage": stats[0].utilization,
        "used": round(gpu.memory_used / 1024, 1),
        "total": round(gpu.memory_total / 1024, 1),
        "temperature": stats[0].temperature
    }

    return gpu_data


def resource_usage():
    ram = psutil.virtual_memory()

    cpu_temperature = None
    if name == 'posix':
        temperature = psutil.sensors_temperatures(fahrenheit=False)
        cpu_temperature = temperature["coretemp"][0].current
    else:
        from clr import AddReference

        AddReference(r"C:\Tools\LibreHardwareMonitor\LibreHardwareMonitorLib.dll")
        from LibreHardwareMonitor.Hardware import Computer

        computer = Computer()
        computer.IsCpuEnabled = True
        computer.Open()

        for hardware in computer.Hardware:
            if hardware.HardwareType.ToString() == "Cpu":

                hardware.Update()

                for sensor in hardware.Sensors:
                    if sensor.SensorType.ToString() == "Temperature":

                        if sensor.Name == "CPU Package":
                            cpu_temperature = sensor.Value
                            break

        if cpu_temperature is None:
            cpu_temperature = "CPU temperature unavailable"

        computer.Close()

    if name == 'nt':
        gpu_data = get_gpu()

        gpu_usage = gpu_data['usage']
        gpu_used = gpu_data['used']
        gpu_total = gpu_data['total']
        gpu_temperature = gpu_data['temperature']
    else:
        gpu_usage = "N/A"
        gpu_used = "N/A"
        gpu_total = "N/A"
        gpu_temperature = "N/A"

    resource_file = {
        'c': psutil.cpu_percent(),  # CPU_usage
        'r': ram.percent,  # RAM_usage
        'g': gpu_usage,  # GPU_usage
        'ru': float(naturalsize(ram.used, binary=True)[:-4]),  # RAM_GiB_used
        'rt': float(naturalsize(ram.total, binary=True)[:-4]),  # RAM_GiB_total
        'gu': gpu_used,  # GPU GiB_used
        'gt': gpu_total,  # GPU_GiB_total
        'tg': gpu_temperature,  # GPU_temperature
        'tc': cpu_temperature,  # CPU_temperature
    }

    return resource_file


def file_export(resource_usage):
    json_msg = json.dumps(resource_usage)

    arduino.write((json_msg + "\n").encode("utf-8"))
    arduino.flush()


if __name__ == '__main__':
    arduino = serial.Serial(port_detect(), 115200, timeout=1)
    arduino.reset_input_buffer()
    arduino.reset_output_buffer()

    while True:
        file_export(resource_usage())
        sleep(1)
