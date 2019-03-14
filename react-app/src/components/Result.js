import React from "react";
import { connect } from "react-redux";
import { setActiveComponent, restart } from "../actions";
import Histogram from "./Histogram";

function Result({ leftPoints, attributes, mask, numQuestions, restart }) {
  let ths = [];
  attributes.forEach(([attr, config]) => {
    if (mask[attr]) {
      const th = <th key={attr}>{attr}</th>;
      ths.push(th);
    }
  });
  const trs = leftPoints.map((point, idx) => (
    <tr key={idx}>
      {point.slice(1).map((x, idx) => (
        <td key={idx}>{x}</td>
      ))}
    </tr>
  ));
  return (
    <div className="row justify-content-center">
      <div className="col">
        <h4>
          The Total No. of Questions Asked is: {numQuestions}.
        </h4>
        <h4>
          {
            leftPoints.length === 1 ?
              "Your Favourite Car is:" :
              `${leftPoints.length} Cars Left in the Database:`
          }
        </h4>
        <table className="table table-hover text-center">
          <thead>
            <tr>{ths}</tr>
          </thead>
          <tbody>
            {trs}
          </tbody>
        </table>
        <div className="d-flex justify-content-center">
          <Histogram />
        </div>
        <button type="button" className="btn btn-primary" onClick={restart}>
          Return to Welcome
        </button>
      </div>
    </div>
  );
}

const mapStateToProps = ({ leftPoints, attributes, mask, numQuestions }) => ({
  leftPoints,
  attributes,
  mask,
  numQuestions
});

const mapDispatchToProps = dispatch => ({
  restart: () => {
    dispatch(setActiveComponent("Welcome"));
    dispatch(restart());
  }
});

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(Result);
