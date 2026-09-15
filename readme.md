# MageSim Wotlk

Mage DPS simulator for WoW Wotlk Classic.

[Live sim website](https://cheesehyvel.github.io/magesim-wotlk/)

Backend is written in C++ and compiled with Emscripten into WebAssembly.\
Frontend is built with Vue and compiled with Vite.

## Building the project
Requirements for backend
* Emscripten
* Make

Requirements for frontend
* npm

Standing in the root folder of the project.\
Build backend: `$ make`\
Build frontend: `$ npm run build`\
Frontend development: `$ npm run dev`
