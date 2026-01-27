# Estacion meteorológica

Este proyecto consiste en una estación meteorológica compacta diseñada para medir y mostrar en tiempo real tres parámetros ambientales fundamentales:
- Temperatura
- Humedad relativa
- Concentración de CO₂

La estación realiza lecturas en intervalos de tiempo de 5 segundos y muestra los valores obtenidos en una pantalla integrada, lo que permite visualizar el estado del ambiente de forma rápida y sencilla.

Las características principales de este proyecto son:
- Lectura periódica de temperatura, humedad y CO₂.
- Intervalos de medición ajustables.
- Visualización clara de los datos en una pantalla.
- Código modular y fácil de extender.
- Arquitectura basada en una máquina de estados

## Hardware

Este es un proyecto basado en una Raspberry Pi Pico (RP2040), un microcontrolador con 2MB de memoria sin acceso wifi y que a mayores cuenta con los siguientes periféricos:
- **Sensor DHT20:** un sensor para tomar medidas de humedad y temperatura
- **Sensor SPG30:** un sensor para tomar medias de calidad de aire y cantidad de CO₂ en el ambiente
- **Pantalla SSD1306:** un controlador CMOS de un solo chip diseñado para gestionar paneles de matriz de puntos OLED/PLED

A la hora de programar y conectar estos elementos de hardware, nos basamos en sus datasheets correspondientes y en librerías de código abierto.

## Usos del proyecto  

Este proyecto podría ser util en ambientes en los que mantener estos tres parámetros estables y conocidos en todo momento es necesario. Por ejemplo:

- **Invernaderos:** para mantener controlados los parámetros en las medidas óptimas para el crecimiento de las plantas
- **Museos:** para controlar valores elevados de humedad o CO₂ que pueden poner en peligro la conservación de las obras de arte.

## Máquina de estados

Este proyecto sigue la siguiente máquina de estados para controlar su comportamiento:

```mermaid
stateDiagram-v2
    direction TB
    [*] --> Temperatura
    Temperatura --> Humedad : tiempo
    Humedad --> CO₂ : tiempo
    CO₂ --> Temperatura : tiempo
```