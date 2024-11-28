# ESP32-TTGO-SIM800L Serial Console
Serial console for executing AT commands with the TTGO ESP32-SIM800L board.

---

## Overview

**A terminal-like interface for interacting with the SIM800L module via UART.**

- A UART-based console enabling real-time interaction with the SIM800L, providing a flexible command-line interface for debugging and testing.
- Supports echoing user input, and displaying clear responses from the SIM800L module.

---

## Features

- Execute AT commands for the SIM800L module directly from the console.
- Real-time command feedback and echo for a terminal-like experience.
---

## Supported Boards

The current implementation supports the following processors and boards:

| Architecture             | Processor          | Example Location                     | Board                  |
| :---                     | :--                | :---                                 | :-                     |
| Xtensa® 32 bit           | ESP32-S0WD         | examples/board/esp32-sim800l         | TTGO T-Call ESP32-SIM800L |

---

## Quick Implementation

### 1. Clone the Repository

```bash
git clone https://github.com/h3v0x/sim800l-console/
```

### 2. Add Required Files to Your Project

Include sim800l_console.h and sim800l_console.c in your project directory.

### 3. Integrate into Your FreeRTOS Application

To enable the console at runtime, follow these steps:

    Create a task that calls the gsm_console_task() function from the sim800l_console.c file.

#### Example code:
```C
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sim800l_console.h"

void app_main(void)
{
    // Initialize the SIM800L module
    init_uart();
    // Start the console task
    xTaskCreate(&gsm_console_task, "gsm_console_task", 8192, NULL, 5, NULL);
}
```


### Quick Demo

Run the application, and you'll see a prompt like this:

```shell
SIM800L> 
```

You can type AT commands and view responses:

```shell
SIM800L> ATI
I (32323) SIM800L: Sending command: ATI
I (38323) SIM800L: Response:
SIM800 R2.04
OK

SIM800L> AT+CSQ
I (42323) SIM800L: Sending command: AT+CSQ
I (48323) SIM800L: Response:
+CSQ: 18,0
OK

```

• AT console demonstration:

![Terminal Demo]()
