import { combineReducers } from "redux";
import {
  SET_ACTIVE_COMPONENT,
  SET_DATASET,
  SET_CANDIDATES,
  TOGGLE_MASK,
  INCREMENT_QCOUNT,
  SET_LEFT_POINTS,
  PRUNE_POINTS,
  SET_MODE,
  RESTART
} from "../actions";

const activeComponent = (state = "Welcome", action) => {
  switch (action.type) {
    case SET_ACTIVE_COMPONENT:
      return action.component;
    default:
      return state;
  }
};

const dataset = (state = null, action) => {
  switch (action.type) {
    case SET_DATASET:
      return action.dataset;
    default:
      return state;
  }
};

const attributes = (state = null, action) => {
  switch (action.type) {
    case SET_DATASET:
      return action.attributes;
    default:
      return state;
  }
};

const candidates = (state = null, action) => {
  switch (action.type) {
    case SET_CANDIDATES:
      return action.candidates;
    default:
      return state;
  }
};

const mask = (state = null, action) => {
  switch (action.type) {
    case SET_DATASET:
      const mask = {};
      action.attributes.forEach(([attr, config]) => {
        mask[attr] = 1;
      });
      return mask;
    case TOGGLE_MASK:
      return { ...state, ...action.mask };
    default:
      return state;
  }
};

const numQuestions = (state = 0, action) => {
  switch (action.type) {
    case RESTART:
      return 0;
    case INCREMENT_QCOUNT:
      return state + 1;
    default:
      return state;
  }
};

// array of [idx, step no., attributes]
const prunedPoints = (state = [], action) => {
  switch (action.type) {
    case RESTART:
      return [];
    case PRUNE_POINTS:
      return [...action.points, ...state];
    default:
      return state;
  }
};

// array of [idx, attributes...]
const leftPoints = (state = [], action) => {
  switch (action.type) {
    case SET_LEFT_POINTS:
      return action.points;
    case PRUNE_POINTS:
      return state.filter(x => !action.indices.includes(x[0]));
    default:
      return state;
  }
};

// history of the number of left points
const numLeftPoints = (state = [], action) => {
  switch (action.type) {
    case SET_CANDIDATES:
      return [action.candidates.size()];
    case SET_LEFT_POINTS:
      return [...state, action.points.length];
    case PRUNE_POINTS:
      const prev = state[state.length - 1];
      return [...state, prev - action.points.length];
    default:
      return state;
  }
}

const mode = (state = "simplex", action) => {
  switch (action.type) {
    case SET_MODE:
      return action.mode;
    default:
      return state;
  }
};

export default combineReducers({
  activeComponent,
  dataset,
  attributes,
  candidates,
  mask,
  numQuestions,
  prunedPoints,
  leftPoints,
  numLeftPoints,
  mode
});
