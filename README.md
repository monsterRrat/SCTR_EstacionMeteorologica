Este proyecto es una estación meteorológica compacta diseñada para medir y mostrar en tiempo real tres parámetros ambientales fundamentales:
- Temperatura
- Humedad relativa
- Concentración de CO₂
La estación realiza lecturas en intervalos de tiempo de _ y muestra los valores obtenidos en una pantalla integrada, lo que permite visualizar el estado del ambiente de forma rápida y sencilla.

Características principales
- Lectura periódica de temperatura, humedad y CO₂.
- Intervalos de medición ajustables.
- Visualización clara de los datos en una pantalla.
- Código modular y fácil de extender.

```mermaid
FSM-v2
    Temperatura --> Humedad :tiempo
    Humedad --> CO2 : tiempo
    CO2 --> Temperatura : tiempo
```