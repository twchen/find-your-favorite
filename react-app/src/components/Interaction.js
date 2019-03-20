import React from "react";
import { connect } from "react-redux";
import {
  setActiveComponent,
  incrementQCount,
  setLeftPoints,
  prunePoints,
  updateConvexHull
} from "../actions";

const SIMPLEX = 2;
const RANDOM = 1;

function getPrunedIndices(prevIndices, currIndices) {
  let prunedIndices = [];
  for (let i = 0, j = 0; i < prevIndices.size() || j < currIndices.size();) {
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
    if (leftPoints.length < 2) {
      this.state = { pair: [] };
      this.stopInteraction();
    } else {
      const convexHullVertices = window.Module.read_convex_hull_vertices();
      this.props.updateConvexHull(convexHullVertices);
      this.state = {
        pair: points2Array2D(this.runner.nextPair())
      };
    }
  }

  choose = idx => {
    const option = idx + 1;
    this.runner.choose(option);
    const convexHullVertices = window.Module.read_convex_hull_vertices();
    this.props.updateConvexHull(convexHullVertices);
    const currIndices = this.runner.getCandidatesIndices();
    const prunedIndices = getPrunedIndices(this.prevIndices, currIndices);
    const prunedPoints = prunedIndices.map(idx => {
      const point = this.props.candidates.get(idx);
      return [idx, this.props.numQuestions + 1, ...point2Array(point)];
    });
    this.props.prunePoints(prunedPoints, prunedIndices);
    this.prevIndices = currIndices;
    this.props.incrementQCount();
    if (this.prevIndices.size() < 2) {
      this.stopInteraction();
    } else {
      const nextPair = points2Array2D(this.runner.nextPair());
      this.setState({
        pair: nextPair
      });
    }
  };

  stopInteraction = () => {
    window.Module.releasePoints(this.props.candidates);
    this.props.showResult();
  }

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
      <div>
        <h4>Q{this.props.numQuestions + 1}: Choose the Car You Favor More among the Following Options</h4>
        <div className="row justify-content-center align-items-center">
          <div className="col-md-8">
            
            <table className="table table-hover text-center">
              <thead>
                <tr>{ths}</tr>
              </thead>
              <tbody>{trs}</tbody>
            </table>
          </div>
          <div className="col-md-1">
            <button type="button" className="btn btn-primary" onClick={this.stopInteraction}>
              Stop
            </button>
          </div>
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

function parseVertices(vertices) {
  if (vertices.size() < 1 || vertices.get(0).size() < 2) return [];
  const verts = [];
  for (let i = 0; i < vertices.size(); ++i) {
    const vertex = vertices.get(i);
    const vert = [];
    for (let j = 0; j < vertex.size() - 1; ++j) {
      vert.push(vertex.get(j));
    }
    verts.push(vert);
  }
  return verts;
}

const mapDispatchToProps = dispatch => ({
  showResult: () => {
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
  },
  updateConvexHull: (vertices) => {
    dispatch(updateConvexHull(parseVertices(vertices)))
  }
});

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(Interaction);
