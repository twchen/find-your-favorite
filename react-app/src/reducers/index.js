import { combineReducers } from "redux";
import {
  SET_ACTIVE_COMPONENT,
  SET_DATASET,
  SET_CANDIDATES,
  TOGGLE_MASK,
  SET_LEFT_POINTS,
  PRUNE_POINTS,
  SET_MODE,
  RESTART,
  UPDATE_CONVEX_HULL
} from "../actions";

const activeComponent = (state = "Welcome", action) => {
  switch (action.type) {
    case SET_ACTIVE_COMPONENT:
      return action.component;
    default:
      return state;
  }
};

const points = (state = null, action) => {
  switch (action.type) {
    case SET_DATASET:
      return action.points;
    default:
      return state;
  }
};

const labels = (state = null, action) => {
  switch (action.type) {
    case SET_DATASET:
      return action.labels;
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

// array of [idx, step no.]
const prunedPoints = (state = [], action) => {
  switch (action.type) {
    case RESTART:
      return [];
    case PRUNE_POINTS:
      const points = action.indices.map(idx => [idx, action.step]);
      return [...points, ...state];
    default:
      return state;
  }
};

// array of indices
const leftPoints = (state = [], action) => {
  switch (action.type) {
    case RESTART:
      return [];
    case SET_LEFT_POINTS:
      return action.indices;
    case PRUNE_POINTS:
      return state.filter(idx => action.indices.indexOf(idx) === -1);
    default:
      return state;
  }
};

// history of the number of left points
const numLeftPoints = (state = [], action) => {
  switch (action.type) {
    case SET_LEFT_POINTS:
      return [action.indices.length];
    case PRUNE_POINTS:
      const prev = state[state.length - 1];
      return [...state, prev - action.indices.length];
    default:
      return state;
  }
};

const mode = (state = "simplex", action) => {
  switch (action.type) {
    case SET_MODE:
      return action.mode;
    default:
      return state;
  }
};

const vertices = (state = [], action) => {
  switch (action.type) {
    case UPDATE_CONVEX_HULL:
      return action.vertices;
    default:
      return state;
  }
};

export default combineReducers({
  activeComponent,
  points,
  labels,
  attributes,
  candidates,
  mask,
  prunedPoints,
  leftPoints,
  numLeftPoints,
  mode,
  vertices
});
