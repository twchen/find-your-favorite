import React from 'react';
import { setActiveComponent, setCandidates, toggleMask } from '../actions';
import { connect } from "react-redux";
//import Module from '../lib/web';

class Welcome extends React.Component {
  constructor(props){
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
    for(let i = 0; i < this.props.attributes.length; ++i){
      const [attr, config] = this.props.attributes[i];
      const range = [config.low, config.high];
      if(this.props.mask[attr]){
        for(let j = 0; j < 2; ++j){
          const str = this.inputs[attr][j].current.value.trim();
          if(str === '')
            continue;
          else if(regex.test(str))
            range[j] = parseInt(str);
          else{
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
    if(str !== '' && regex.test(str))
      maxPoints = parseInt(str);
    const candidates = this.props.dataset.selectCandidates(ranges, mask, maxPoints);
    if(candidates.size() < 1000){
      alert('Not enough samples, try larger ranges');
      return;
    }
    this.props.startAlgorithm(candidates);
  }

  render() {
    const trs = this.props.attributes.map(([attr, config]) => {
      const disabled = this.props.mask[attr] === 0;
      const {low, high} = config;
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
              onChange={() => this.props.toggleMask(attr, 1 - this.props.mask[attr])}
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
            <div className="form-group row justify-content-center">
              <label htmlFor="maxPoints" className="col-4 col-form-label">
                Max No. of Cars
              </label>
              <div className="col-6">
                <input
                  type="text"
                  className="form-control"
                  id="maxPoints"
                  placeholder="1000"
                  ref={this.inputs.maxPoints}
                />
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

const mapStateToProps = ({attributes, mask, dataset}) => ({
  attributes,
  mask,
  dataset
})


const mapDispatchToProps = dispatch => ({
  startAlgorithm: (candidates) => {
    dispatch(setCandidates(candidates));
    dispatch(setActiveComponent('Interaction'));
  },
  toggleMask: (attr, mask) => dispatch(toggleMask(attr, mask))
})

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(Welcome);