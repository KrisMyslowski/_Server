
# Qt TCP Server with Config Viewer
A lightweight Qt-based project for handling JSON-based TCP commands via a configurable server — optionally with a graphical user interface for configuration and display.

##  Features
- JSON-based GET/SET commands
- TCP server with configurable port
- Singleton DataModel for managing configuration data
- Optional Qt-based UI

## Dependencies
- C++17 
- Qt 5: 
    - `core`
    - `gui` 
    - `widgets` 
    - `network` 

## Compiling

### qmake
```bash
qmake
make
```

### cmake 
`tbd`

##  Execute

```bash
./_Server -c <path> [-p <port>] [-ui]
```

### Parameter
| Parameter      | Typ     | Description                               |
|----------------|---------|-------------------------------------------|
| `-c <path>`    | **REQ** | Path to JSON config                       |
| `-p <port>`    | OPT     | TCP-Port (default: 12345)                 |
| `-ui`          | OPT     | Start with UI to display data model       |
| `-h`, `--help` | INF     | Show help menu                            |

## Communication
The server accepts simple JSON messages over TCP.

### GET-Format
```JSON
GET: {"request":"<get>","path":"<path.to.value>"}
```

#### Dummys
```JSON
{"request":"get","path":"description"}
{"request":"get","path":"safety.emergency_shutdown.enabled"}
{"request":"get","path":"snowmaking_system.settings.air_pressure.value"}
{"request":"get","path":"snowmaking_system.maintenance.last_serviced"}
{"request":"get","path":"elements.element3.status"}
```

### SET-Format
```JSON
SET: {"request":"<set>","path":"<path.to.value>","value":"<value>"}
```

#### Dummys
```JSON
{"request":"set","path":"description","value":"new_description"}
{"request":"set","path":"safety.manual_override.enabled","value":false}
{"request":"set","path":"snowmaking_system.settings.fan_speed.value","value":1800}
{"request":"set","path":"elements.element1.status","value":"closed"}
{"request":"set","path":"snowmaking_system.operational_conditions.temperature_min","value":-10}
```

##  MVC
```text
+-------------------+       +-------------------+
|   ConfigView (UI) |<----->|   TCP_Server      |
+-------------------+       +-------------------+
           |                          |
           +--------------------------+
                      |
               +--------------+
               |  DataModel   |
               +--------------+
                      |
             JSON-Data from Disk
```

## TODOs
### _Server
- Handle JSON-Arrays
- Logging, Watchdog, etc.
- CMake Support

### UI
- Control Elements
- LogView

### Datamodel
- JSON-Array
---
---
# License

This project is licensed under the LGPL v3 License. See [LICENSE](./LICENSE/LGPL-3.0.txt) for details.

### Note on Qt Usage

This project uses the Qt framework, which is licensed under the GNU Lesser General Public License (LGPL) v3. For more information about Qt’s licensing terms, visit:

https://www.qt.io/licensing/

The source code for the used Qt libraries is available at:

https://download.qt.io/archive/qt/

Dynamic linking with Qt is used in accordance with the terms of the LGPL v3.