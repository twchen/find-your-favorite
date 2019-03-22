1. Set up [Emscripten](https://emscripten.org) SDK.
  ```sh
  # in libraries/
  ./setup_emsdk.sh
  ```

2. Compile GLPK for Emscripten
  ```sh
  # in libraries/
  ./make_lib.sh em++
  ```

3. Transcompile C++ code into JavaScript code.
  (The core algorithm is written in C/C++ by Min Xie).
  ```sh
  make web
  ```

4. Install npm.
  [Get npm](https://www.npmjs.com/get-npm)

5. Build the React app
  ```sh
  # in react-app/
  npm install && npm run build
  ```
