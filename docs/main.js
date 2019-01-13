class App extends React.Component {
  constructor(props){
    super(props);
    this.state = {
      started: false,
      finished: false
    }
  }
  start = (candidates, attrNames) => {
    this.setState({
      started: true,
      candidates,
      attrNames
    })
  }

  showResult = (result) => {
    this.setState({
      finished: true,
      result
    })
  }

  render(){
      if(!this.state.started){
        return <Welcome start={this.start} dataset={this.props.dataset}/>
      }else if(!this.state.finished){
        return <Interaction
          candidates={this.state.candidates}
          attrNames={this.state.attrNames}
          showResult={this.showResult}
        />
      }else{
        return <Result result={this.state.result} attrNames={this.state.attrNames}/>
      }
  }
}


class Welcome extends React.Component {
  constructor(props){
    super(props);

    this.attrAndDefaultRanges = [
      ['Price', 1000, 50000],
      ['Year', 2001, 2017],
      ['Power', 50, 400],
      ['Used KM', 10000, 150000]
    ];
    this.inputs = {};
    this.state = {};
    this.attrAndDefaultRanges.forEach(x => {
      const attr = x[0];
      this.inputs[attr] = [React.createRef(), React.createRef()];
      this.state[attr] = false;
    });
    this.inputs.maxPoints = React.createRef();
  }

  toggleCheckbox = (attr, disabled) => {
    let state = {};
    state[attr] = disabled;
    this.setState(state);
  }

  handleStart = () => {
    const ranges = new Module.Ranges();
    const attrMask = new Module.VectorInt();
    const regex = /^\d+$/;
    for(let i = 0; i < this.attrAndDefaultRanges.length; ++i){
      const attr = this.attrAndDefaultRanges[i][0];
      const range = this.attrAndDefaultRanges[i].slice(1);
      const discard = this.state[attr];
      if(!discard){
        for(let j = 0; j < 2; ++j){
          const str = this.inputs[attr][j].current.value.trim();
          if(str === ''){
            continue;
          }else if(regex.test(str)){
            range[j] = parseInt(str);
          }else{
            alert(`${str} in ${attr} range is not an interger!`);
            return;
          }
        }
      }
      attrMask.push_back(discard ? 0 : 1);
      ranges.push_back(range);
    }
    let maxPoints = 1000;
    const str = this.inputs.maxPoints.current.value.trim();
    if(str !== '' && regex.test(str)){
      maxPoints = parseInt(str);
    }
    const candidates = this.props.dataset.selectCandidates(ranges, attrMask, maxPoints);
    if(candidates.size() < 1000){
      alert('Not enough samples, try larger ranges');
      return;
    }
    let attrNames = [];
    for(var i = 0; i < this.attrAndDefaultRanges.length; ++i){
      if(attrMask.get(i)){
        const attr = this.attrAndDefaultRanges[i][0];
        attrNames.push(attr);
      }
    }
    this.props.start(candidates, attrNames);
  }

  render() {
    const trs = this.attrAndDefaultRanges.map(x => {
      const [attr, low, high] = x;
      const disabled = this.state[attr];
      return (
        <tr key={attr}>
          <td className="align-middle">{attr}</td>
          <td>
            <input type="text" className="form-control"
              placeholder={low}
              ref={this.inputs[attr][0]}
              disabled={disabled}
            />
          </td>
          <td>
            <input
              type="text" className="form-control"
              placeholder={high}
              ref={this.inputs[attr][1]}
              disabled={disabled}
            />
          </td>
          <td className="align-middle">
            <input type="checkbox"
              onChange={() => this.toggleCheckbox(attr, !disabled)}
            />
          </td>
        </tr>
      )
    });
    return (
      <div className="container">
        <div className="text-center m-auto" style={{maxWidth: '50rem'}}>
          <h1>Welcome</h1>
          <p className="lead text-muted">
            This is a user study on a secondhand cars dataset. <br />
            Enter your acceptable ranges for each attribute and the maximum number of cars.<br />
            Leave an input blank to use its default.<br />
            Check the checkbox if you want to discard an attribute.<br />
            You will be presented two cars at a time. And you need to choose the one you prefer. <br />
            Click the "Start" button to start the user study.
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
            <tbody>
              {trs}
            </tbody>
          </table>
          <form>
            <div className="form-group row justify-content-center">
              <label htmlFor="maxPoints" className="col-4 col-form-label">Max No. of Cars</label>
              <div className="col-6">
                <input type="text" className="form-control" id="maxPoints" placeholder="1000"
                  ref={this.inputs.maxPoints}/>
              </div>
            </div>
            <div className="form-group row">
              <div className="col">
                <button type="button" className="btn btn-primary" style={{width: '8rem'}}
                  onClick={this.handleStart}>
                  Start
                </button>
              </div>
            </div>
          </form>
        </div>
      </div>
    )
  }
}

class Interaction extends React.Component {
  constructor(props) {
    super(props);
    this.runner = new Module.AlgorithmRunner(props.candidates);
    this.state = {
      numAsked: 0,
      pair: this.runner.nextPair()
    }
  }

  choose = (idx) => {
    const option = idx + 1;
    this.runner.choose(option);
    if(this.runner.isFinished()){
      const result = this.runner.getResult();
      this.props.showResult(result);
    }else{
      const nextPair = this.runner.nextPair();
      this.setState({
        pair: nextPair,
        numAsked: this.state.numAsked + 1
      })
    }
  }

  render() {
    let trs = [];
    for(let i = 0; i < this.state.pair.size(); ++i){
      const point = this.state.pair.get(i);
      let tds = [<td key="Option No.">{i + 1}</td>];
      for(let j = 0; j < point.dim(); ++j){
        tds.push(
          <td key={this.props.attrNames[j]}>
            {point.get(j)}
          </td>
        );
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
      )
      const tr = <tr key={i}>{tds}</tr>;
      trs.push(tr);
    }
    let ths = [<th key="Option No.">Option</th>];
    this.props.attrNames.forEach(attr => {
      ths.push(<th key={attr}>{attr}</th>);
    });
    ths.push(<th key="chooseButton" />);
    return (
      <div className="container">
        <div className="row justify-content-center">
          <div className="col-md-8">
            <h4>No. of questions asked: {this.state.numAsked}. No. of points left: {this.runner.numLeftPoints()}</h4>
            <h4>Please choose the car you favor more: </h4>
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

function Result({result, attrNames}){
  const ths = attrNames.map(attr => <th key={attr}>{attr}</th>);
  let tds = [];
  for(let i = 0; i < result.dim(); ++i){
    const td = <td key={i}>{result.get(i)}</td>;
    tds.push(td);
  }
  return (
    <div className="container">
      <div className="row justify-content-center">
        <div className="col-md-8">
          <h4>Your favorite car is</h4>
          <table className="table table-hover text-center">
            <thead>
              <tr>
                {ths}
              </tr>
            </thead>
            <tbody>
              <tr>
                {tds}
              </tr>
            </tbody>
          </table>
        </div>
      </div>
    </div>
  )
}

/*
let Module = {
  onRuntimeInitialized: function() {
    const dataset = new Module.Dataset('car.txt');
    ReactDOM.render(
      <App dataset={dataset}/>,
      document.getElementById("root")
    );
  }
};
*/