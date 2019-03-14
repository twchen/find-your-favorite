import React from "react";
import { connect } from "react-redux";

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
          <h4>The No. of Cars We Have Pruned: {this.props.prunedPoints.length}</h4>
          <table className="table">
            <thead>
              <tr>{ths}</tr>
            </thead>
            <tbody>{prunedTrs}</tbody>
          </table>
        </div>
        <div className="col">
          <h4>The No. of Cars Left in the Database: {this.props.leftPoints.length}</h4>
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

export default connect(mapStateToPropsStats)(Stats);
