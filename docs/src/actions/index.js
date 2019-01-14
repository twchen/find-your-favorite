export const SET_ACTIVE_COMPONENT = 'SET_ACTIVE_COMPONENT';
export const SET_CANDIDATES = 'SET_CANDIDATES';
export const SET_DATASET = 'SET_DATASET';
export const TOGGLE_MASK = 'TOGGLE_MASK';
export const SET_RESULT = 'SET_RESULT';

export const setActiveComponent = component => ({
  type: SET_ACTIVE_COMPONENT,
  component
})

export const setCandidates = candidates => ({
  type: SET_CANDIDATES,
  candidates
})

export const setDataset = (dataset, attributes) => ({
  type: SET_DATASET,
  dataset,
  attributes
})

export const toggleMask = (attr, val) => ({
  type: TOGGLE_MASK,
  mask: {[attr]: val}
})

export const setResult = result => ({
  type: SET_RESULT,
  result
})