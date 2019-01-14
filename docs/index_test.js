const SET_ACTIVE_COMPONENT = "SET_ACTIVE_COMPONENT";
const SET_CANDIDATES = "SET_CANDIDATES";
const SET_DATASET = "SET_DATASET";
const TOGGLE_MASK = "TOGGLE_MASK";
const SET_RESULT = "SET_RESULT";
const INCREMENT_QCOUNT = "INCREMENT_QCOUNT";
const PRUNE_POINTS = "PRUNE_POINTS";
const SET_LEFT_POINTS = "SET_LEFT_POINTS";
const CHANGE_MODE = "CHANGE_MODE";
const SIMPLEX = 2;
const RANDOM = 1;

const setActiveComponent = component => ({
  type: SET_ACTIVE_COMPONENT,
  component
});

const setCandidates = candidates => ({
  type: SET_CANDIDATES,
  candidates
});

const setDataset = (dataset, attributes) => ({
  type: SET_DATASET,
  dataset,
  attributes
});

const toggleMask = (attr, val) => ({
  type: TOGGLE_MASK,
  mask: { [attr]: val }
});

const setResult = result => ({
  type: SET_RESULT,
  result
});

const incrementQCount = () => ({
  type: INCREMENT_QCOUNT
});

const prunePoints = (points, indices) => ({
  type: PRUNE_POINTS,
  points,
  indices
});

const setLeftPoints = points => ({
  type: SET_LEFT_POINTS,
  points
});

const changeMode = mode => ({
  type: CHANGE_MODE,
  mode
})

function App({ activeComponent }) {
  if (activeComponent === "Welcome") {
    return <WelcomeEl />;
  } else if (activeComponent === "Interaction") {
    return (
      <div>
        <InteractionEl />
        <StatsEl />
      </div>
    );
  } else if (activeComponent === "Result") {
    return (
      <div>
        <ResultEl />
        <StatsEl />
      </div>
    );
  } else {
    return <h1>Hello, world</h1>;
  }
}

const mapStateToPropsApp = ({ activeComponent }) => ({
  activeComponent
});

const AppEl = ReactRedux.connect(mapStateToPropsApp)(App);

function getPrunedIndices(prevIndices, currIndices) {
  let prunedIndices = [];
  for (let i = 0, j = 0; i < prevIndices.size() || j < currIndices.size(); ) {
    if (j >= currIndices.size() || prevIndices.get(i) < currIndices.get(j)) {
      prunedIndices.push(prevIndices.get(i));
      ++i;
    } else {
      ++i;
      ++j;
    }
  }
  return prunedIndices;
}

function point2Array(point) {
  let arr = [];
  for (let i = 0; i < point.dim(); ++i) {
    arr.push(point.get(i));
  }
  return arr;
}

class Interaction extends React.Component {
  constructor(props) {
    super(props);
    const smallerBetter = new Module.VectorInt();
    this.attributes = [];
    this.props.attributes.forEach(([attr, config]) => {
      if (this.props.mask[attr]) {
        smallerBetter.push_back(config.smallerBetter ? 1 : 0);
        this.attributes.push(attr);
      }
    });
    this.runner = new Module.AlgorithmRunner(
      this.props.candidates,
      smallerBetter,
      this.props.mode === 'random' ? RANDOM : SIMPLEX
    );
    this.prevIndices = this.runner.getCandidatesIndices();
    let leftPoints = [];
    for (let i = 0; i < this.prevIndices.size(); ++i) {
      const idx = this.prevIndices.get(i);
      const point = this.props.candidates.get(idx);
      const p = [idx, ...point2Array(point)];
      leftPoints.push(p);
    }
    this.props.setLeftPoints(leftPoints);
    this.state = {
      pair: this.runner.nextPair()
    };
  }

  choose = idx => {
    const option = idx + 1;
    this.runner.choose(option);
    const currIndices = this.runner.getCandidatesIndices();
    const prunedIndices = getPrunedIndices(this.prevIndices, currIndices);
    const prunedPoints = prunedIndices.map(idx => {
      const point = this.props.candidates.get(idx);
      return [idx, this.props.numQuestions + 1, ...point2Array(point)];
    });
    this.props.prunePoints(prunedPoints, prunedIndices);
    this.prevIndices = currIndices;
    this.props.incrementQCount();
    if (this.runner.isFinished()) {
      const result = this.runner.getResult();
      this.props.showResult(result);
    } else {
      const nextPair = this.runner.nextPair();
      this.setState({
        pair: nextPair
      });
    }
  };

  render() {
    let trs = [];
    for (let i = 0; i < this.state.pair.size(); ++i) {
      const point = this.state.pair.get(i);
      let tds = [<td key="Option No.">{i + 1}</td>];
      for (let j = 0; j < point.dim(); ++j) {
        tds.push(<td key={this.attributes[j]}>{point.get(j)}</td>);
      }
      tds.push(
        <td key="Choose button">
          <button
            type="button"
            className="btn btn-outline-success btn-sm"
            onClick={() => this.choose(i)}
          >
            Choose
          </button>
        </td>
      );
      const tr = <tr key={i}>{tds}</tr>;
      trs.push(tr);
    }
    let ths = [<th key="Option No.">Option</th>];
    this.attributes.forEach(attr => {
      ths.push(<th key={attr}>{attr}</th>);
    });
    ths.push(<th key="chooseButton" />);
    return (
      <div className="container">
        <div className="row justify-content-center">
          <div className="col-md-8">
            <h4>
              Q{this.props.numQuestions + 1}: choose the car you favor more
            </h4>
            <table className="table table-hover text-center">
              <thead>
                <tr>{ths}</tr>
              </thead>
              <tbody>{trs}</tbody>
            </table>
          </div>
        </div>
      </div>
    );
  }
}

const mapStateToPropsInt = ({
  candidates,
  mask,
  attributes,
  numQuestions,
  mode
}) => ({
  candidates,
  mask,
  attributes,
  numQuestions,
  mode
});

const mapDispatchToPropsInt = dispatch => ({
  showResult: result => {
    dispatch(setResult(result));
    dispatch(setActiveComponent("Result"));
  },
  incrementQCount: () => {
    dispatch(incrementQCount());
  },
  setLeftPoints: leftPoints => {
    dispatch(setLeftPoints(leftPoints));
  },
  prunePoints: (points, indices) => {
    dispatch(prunePoints(points, indices));
  }
});

const InteractionEl = ReactRedux.connect(
  mapStateToPropsInt,
  mapDispatchToPropsInt
)(Interaction);

function Result({ result, attributes, mask, numQuestions }) {
  let ths = [];
  attributes.forEach(([attr, config]) => {
    if (mask[attr]) {
      const th = <th key={attr}>{attr}</th>;
      ths.push(th);
    }
  });
  let tds = [];
  for (let i = 0; i < result.dim(); ++i) {
    const td = <td key={i}>{result.get(i)}</td>;
    tds.push(td);
  }
  return (
    <div className="container">
      <div className="row justify-content-center">
        <div className="col-md-8">
          <h4>No. of questions asked: {numQuestions}</h4>
          <h4>Your favorite car is</h4>
          <table className="table table-hover text-center">
            <thead>
              <tr>{ths}</tr>
            </thead>
            <tbody>
              <tr>{tds}</tr>
            </tbody>
          </table>
        </div>
      </div>
    </div>
  );
}

const mapStateToPropsResult = ({ result, attributes, mask, numQuestions }) => ({
  result,
  attributes,
  mask,
  numQuestions
});

const ResultEl = ReactRedux.connect(mapStateToPropsResult)(Result);

class Welcome extends React.Component {
  constructor(props) {
    super(props);
    this.inputs = {};
    this.props.attributes.forEach(([attr, config]) => {
      this.inputs[attr] = [React.createRef(), React.createRef()];
    });
    this.inputs.maxPoints = React.createRef();
  }

  handleStart = () => {
    const ranges = new Module.Ranges();
    const mask = new Module.VectorInt();
    const regex = /^\d+$/;
    for (let i = 0; i < this.props.attributes.length; ++i) {
      const [attr, config] = this.props.attributes[i];
      const range = [config.low, config.high];
      if (this.props.mask[attr]) {
        for (let j = 0; j < 2; ++j) {
          const str = this.inputs[attr][j].current.value.trim();
          if (str === "") continue;
          else if (regex.test(str)) range[j] = parseInt(str);
          else {
            alert(`${str} in ${attr} range is not an integer`);
            return;
          }
        }
      }
      ranges.push_back(range);
      mask.push_back(this.props.mask[attr]);
    }
    let maxPoints = 1000;
    const str = this.inputs.maxPoints.current.value.trim();
    if (str !== "" && regex.test(str)) maxPoints = parseInt(str);
    const candidates = this.props.dataset.selectCandidates(
      ranges,
      mask,
      maxPoints
    );
    if (candidates.size() < 1000) {
      alert("Not enough samples, try larger ranges");
      return;
    }
    this.props.startAlgorithm(candidates);
  };

  handleModeChange = (event) => {
    this.props.changeMode(event.target.value);
  }

  render() {
    const trs = this.props.attributes.map(([attr, config]) => {
      const disabled = this.props.mask[attr] === 0;
      const { low, high } = config;
      return (
        <tr key={attr}>
          <td className="align-middle">{attr}</td>
          <td>
            <input
              type="text"
              className="form-control"
              placeholder={low}
              ref={this.inputs[attr][0]}
              disabled={disabled}
            />
          </td>
          <td>
            <input
              type="text"
              className="form-control"
              placeholder={high}
              ref={this.inputs[attr][1]}
              disabled={disabled}
            />
          </td>
          <td className="align-middle">
            <input
              type="checkbox"
              checked={disabled}
              onChange={() =>
                this.props.toggleMask(attr, 1 - this.props.mask[attr])
              }
            />
          </td>
        </tr>
      );
    });
    return (
      <div className="container">
        <div className="text-center m-auto" style={{ maxWidth: "50rem" }}>
          <h1>Welcome</h1>
          <p className="lead text-muted">
            This is a user study on a dataset of used cars.
          </p>
          <table className="table">
            <thead>
              <tr>
                <th className="align-middle">Attribute</th>
                <th>Lower bound</th>
                <th>Upper bound</th>
                <th>Discard</th>
                <th />
              </tr>
            </thead>
            <tbody>{trs}</tbody>
          </table>
          <form>
            <div className="form-group row align-items-center">
              <div className="col-6">
                <div className="row align-items-center">
                  <label htmlFor="maxPoints" className="col-4 col-form-label">
                    Max No. of Cars
                  </label>
                  <div className="col">
                    <input
                      type="text"
                      className="form-control"
                      id="maxPoints"
                      placeholder="1000"
                      ref={this.inputs.maxPoints}
                    />
                  </div>
                </div>
              </div>

              <div className="col-6">
                <div className="row align-items-center">
                  <label className="col-4 col-form-label">Mode</label>
                  <div className="col">
                    <div className="form-check form-check-inline">
                      <input
                        className="form-check-input"
                        type="radio"
                        name="inlineRadioOptions"
                        id="simplex"
                        value="simplex"
                        checked={this.props.mode === "simplex"}
                        onChange={this.handleModeChange}
                      />
                      <label className="form-check-label" htmlFor="simplex">
                        Simplex
                      </label>
                    </div>
                    <div className="form-check form-check-inline">
                      <input
                        className="form-check-input"
                        type="radio"
                        name="inlineRadioOptions"
                        id="random"
                        value="random"
                        checked={this.props.mode === "random"}
                        onChange={this.handleModeChange}
                      />
                      <label className="form-check-label" htmlFor="random">
                        Random
                      </label>
                    </div>
                  </div>
                </div>
              </div>
            </div>
            <div className="form-group row">
              <div className="col">
                <button
                  type="button"
                  className="btn btn-primary"
                  style={{ width: "8rem" }}
                  onClick={this.handleStart}
                >
                  Start
                </button>
              </div>
            </div>
          </form>
        </div>
      </div>
    );
  }
}

const mapStateToPropsWel = ({ attributes, mask, dataset, mode }) => ({
  attributes,
  mask,
  dataset,
  mode
});

const mapDispatchToPropsWel = dispatch => ({
  startAlgorithm: candidates => {
    dispatch(setCandidates(candidates));
    dispatch(setActiveComponent("Interaction"));
  },
  toggleMask: (attr, mask) => dispatch(toggleMask(attr, mask)),
  changeMode: mode => dispatch(changeMode(mode))
});

const WelcomeEl = ReactRedux.connect(
  mapStateToPropsWel,
  mapDispatchToPropsWel
)(Welcome);

class Stats extends React.Component {
  constructor(props) {
    super(props);
    this.attributes = [];
    this.props.attributes.forEach(([attr, config]) => {
      if (this.props.mask[attr]) {
        this.attributes.push(attr);
      }
    });
  }

  render() {
    let ths = [<th key="Step No.">Step</th>];
    this.attributes.forEach(attr => {
      ths.push(<th key={attr}>{attr}</th>);
    });
    const prunedTrs = this.props.prunedPoints.map((point, idx) => (
      <tr key={idx}>
        {point.slice(1).map((x, idx) => (
          <td key={idx}>{x}</td>
        ))}
      </tr>
    ));
    const leftTrs = this.props.leftPoints.map((point, idx) => (
      <tr key={idx}>
        {point.slice(1).map((x, idx) => (
          <td key={idx}>{x}</td>
        ))}
      </tr>
    ));
    return (
      <div className="row">
        <div className="col">
          <h4>No. of points pruned: {this.props.prunedPoints.length}</h4>
          <table className="table">
            <thead>
              <tr>{ths}</tr>
            </thead>
            <tbody>{prunedTrs}</tbody>
          </table>
        </div>
        <div className="col">
          <h4>No. of points left: {this.props.leftPoints.length}</h4>
          <table className="table">
            <thead>
              <tr>{ths.slice(1)}</tr>
            </thead>
            <tbody>{leftTrs}</tbody>
          </table>
        </div>
      </div>
    );
  }
}

const mapStateToPropsStats = ({
  prunedPoints,
  leftPoints,
  attributes,
  mask,
  numQuestions
}) => ({
  prunedPoints,
  leftPoints,
  attributes,
  mask,
  numQuestions
});

const StatsEl = ReactRedux.connect(mapStateToPropsStats)(Stats);

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

const result = (state = null, action) => {
  switch (action.type) {
    case SET_RESULT:
      return action.result;
    default:
      return state;
  }
};

const numQuestions = (state = 0, action) => {
  switch (action.type) {
    case INCREMENT_QCOUNT:
      return state + 1;
    default:
      return state;
  }
};

// array of [idx, step no., attributes]
const prunedPoints = (state = [], action) => {
  switch (action.type) {
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

const mode = (state = 'simplex', action) => {
  switch(action.type){
    case CHANGE_MODE:
      return action.mode;
    default:
      return state
  }
}

const rootReducer = Redux.combineReducers({
  activeComponent,
  dataset,
  attributes,
  candidates,
  mask,
  result,
  numQuestions,
  prunedPoints,
  leftPoints,
  mode
});
