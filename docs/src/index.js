import { createStore } from "redux";
import rootReducer from './reducers';
import React from "react";
import { render } from "react-dom";
import { Provider } from "react-redux";
import App from './components/App';

var Module = {
  onRuntimeInitialized: function () {
    const dataset = new Module.Dataset('car.txt');
    const attributes = [
      ['Price', { low: 1000, high: 50000, smallerBetter: true }],
      ['Year', { low: 2001, high: 2017, smallerBetter: false }],
      ['Power', { low: 50, high: 400, smallerBetter: false }],
      ['Used KM', {low: 10000, high: 150000, smallerBetter: true }]
    ];
    const mask = {};
    attributes.forEach(([attr, config]) => {
      mask[attr] = 1;
    });

    const store = createStore(rootReducer, {
      dataset, attributes, mask
    });
    render(
      <Provider store={store}>
        <App />
      </Provider>,
      document.getElementById("root")
    );
  }
};