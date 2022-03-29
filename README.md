This repository is for the demo paper:

Min Xie, Tianwen Chen and Raymond Chi-Wing Wong,
_"FindYourFavorite: An Interactive System for Finding the User's Favorite Tuple in the Database"_
the 2019 ACM Conference on Management of Data (SIGMOD)

Go to [https://twchen.github.io/find-your-favorite/](https://twchen.github.io/find-your-favorite/) for the demo.

# Build Instructions

1. Set up [Emscripten](https://emscripten.org) SDK.

   ```sh
   # in libraries/
   ./setup_emsdk.sh
   source emsdk/emsdk_env.sh
   ```

2. Compile GLPK for Emscripten

   ```sh
   # in libraries/
   ./make_lib.sh em++
   ```

3. Transcompile C++ code into JavaScript code.
   (The core algorithm is written in C/C++ by Min Xie).

   ```sh
   # in the project root folder
   make web
   ```

4. Install Yarn.
   [Installation Guide of Yarn](https://yarnpkg.com/lang/en/docs/install/)

5. Install dependencies

   ```sh
   # in react-app/
   yarn install
   ```

6. Run the demo

   ```sh
   # in react-app/
   yarn start
   ```

   The demo will be automatically opened in the browser. Follow this [YouTube video](https://www.youtube.com/watch?v=FjFbNcQYDFM) to interact with the demo.

7. Build the demo for deployment
   ```sh
   # in react-app/
   yarn run build
   ```
   The built webpages are placed in the `react-app/build/` folder.
