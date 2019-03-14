import { createStore } from "redux";
import rootReducer from "./reducers";
import React from "react";
import { render } from "react-dom";
import { Provider } from "react-redux";
import App from "./components/App";

window.Module = {
  onRuntimeInitialized: function() {
    const dataset = new window.Module.Dataset("car.txt");
    const attributes = [
      ["Price (USD)", { low: 1000, high: 50000, smallerBetter: true }],
      ["Year", { low: 2001, high: 2017, smallerBetter: false }],
      ["Power (HP)", { low: 50, high: 400, smallerBetter: false }],
      ["Used KM", { low: 10000, high: 150000, smallerBetter: true }]
    ];
    const mask = {};
    attributes.forEach(([attr, config]) => {
      mask[attr] = 1;
    });

    const store = createStore(
      rootReducer,
      { dataset, attributes, mask }
      , window.__REDUX_DEVTOOLS_EXTENSION__ && window.__REDUX_DEVTOOLS_EXTENSION__()
    );
    render(
      <Provider store={store}>
        <App />
      </Provider>,
      document.getElementById("root")
    );
  }
};

const script = document.createElement("script");
script.src = "web.js";
script.async = true;
document.body.appendChild(script);
