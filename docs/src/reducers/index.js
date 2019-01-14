import {
  SET_ACTIVE_COMPONENT,
  SET_DATASET,
  SET_CANDIDATES,
  TOGGLE_MASK,
  SET_RESULT
} from '../actions';

export const activeComponent = (state = 'Welcome', action) => {
  switch(action.type){
    case SET_ACTIVE_COMPONENT:
      return action.component;
    default:
      return state;
  }
}

export const dataset = (state = null, action) => {
  switch(action.type){
    case SET_DATASET:
      return action.dataset;
    default:
      return state;
  }
}

export const attributes = (state = null, action) => {
  switch(action.type){
    case SET_DATASET:
      return action.attributes;
    default:
      return state;
  }
}

export const candidates = (state = null, action) => {
  switch (action.type){
    case SET_CANDIDATES:
      return action.candidates;
    default:
      return state;
  }
}

export const mask = (state = null, action) => {
  switch(action.type){
    case SET_DATASET:
      const mask = {};
      action.attributes.forEach(([attr, config]) => {
        mask[attr] = 1;
      });
      return mask;
    case TOGGLE_MASK:
      return {...state, ...attr}
    default:
      return state;
  }
}

export const result = (state = null, action) => {
  switch(action.type){
    case SET_RESULT:
      return action.result;
    default:
      return state;
  }
}

export default combineReducers({
  activeComponent,
  dataset,
  attributes,
  candidates,
  mask,
  result
});