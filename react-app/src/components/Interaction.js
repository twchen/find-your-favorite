import React from "react";
import { connect } from "react-redux";
import {
  setActiveComponent,
  setResult,
  incrementQCount,
  setLeftPoints,
  prunePoints
} from "../actions";

const SIMPLEX = 2;
const RANDOM = 1;

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

function points2Array2D(points) {
  let arr = [];
  for (let i = 0; i < points.size(); ++i) {
    arr.push(point2Array(points.get(i)));
  }
  return arr;
}

class Interaction extends React.Component {
  constructor(props) {
    super(props);
    const smallerBetter = new window.Module.VectorInt();
    this.attributes = [];
    this.props.attributes.forEach(([attr, config]) => {
      if (this.props.mask[attr]) {
        smallerBetter.push_back(config.smallerBetter ? 1 : 0);
        this.attributes.push(attr);
      }
    });
    this.runner = new window.Module.AlgorithmRunner(
      this.props.candidates,
      smallerBetter,
      this.props.mode === "random" ? RANDOM : SIMPLEX
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
    if (this.runner.isFinished()) {
      this.state = { pair: [] };
      const result = point2Array(this.runner.getResult());
      window.Module.releasePoints(this.props.candidates);
      this.props.showResult(result);
    } else {
      this.state = {
        pair: points2Array2D(this.runner.nextPair())
      };
    }
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
      const result = point2Array(this.runner.getResult());
      window.Module.releasePoints(this.props.candidates);
      this.props.showResult(result);
    } else {
      const nextPair = points2Array2D(this.runner.nextPair());
      this.setState({
        pair: nextPair
      });
    }
  };

  render() {
    const ths = [<th key="Option No.">Option</th>];
    this.attributes.forEach(attr => {
      ths.push(<th key={attr}>{attr}</th>);
    });
    ths.push(<th key="chooseButton" />);

    const trs = this.state.pair.map((point, idx) => {
      const tds = [<td key="Option No.">{idx + 1}</td>];
      point.forEach((x, i) => {
        tds.push(<td key={i}>{x}</td>);
      });
      tds.push(
        <td key="chooseButton">
          <button
            type="button"
            className="btn btn-outline-success btn-sm"
            onClick={() => this.choose(idx)}
          >
            Choose
          </button>
        </td>
      );
      return <tr key={idx}>{tds}</tr>;
    });
    return (
      <div className="row justify-content-center">
        <div className="col-md-8">
          <h4>Q{this.props.numQuestions + 1}: choose the car you favor more</h4>
          <table className="table table-hover text-center">
            <thead>
              <tr>{ths}</tr>
            </thead>
            <tbody>{trs}</tbody>
          </table>
        </div>
      </div>
    );
  }
}

const mapStateToProps = ({
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

const mapDispatchToProps = dispatch => ({
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

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(Interaction);