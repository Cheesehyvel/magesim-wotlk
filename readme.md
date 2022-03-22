# MageSim TBC

Mage DPS simulator for WoW TBC Classic.

Backend is written in C++ and compiled with Emscripten into WebAssembly.\
Frontend is written in Vue.js and SCSS and compiled with Laravel mix.

## Building the project
Requirements for backend
* Emscripten
* Make

Requirements for frontend
* npm

Standing in the root folder of the project.\
Build backend: `$ make`\
Build frontend: `$ npx mix`

## Common issues
Some browsers have issues with WebAssembly when simply opening the file in the browser. This is when the url is something like `C://Users/Name/Downloads/magesim-tbc/index.html`.\
To solve this, run the project through a web server. If you have Python3 installed you can simply run `$ python3 -m http.server` in the project folder to start a simple web server.